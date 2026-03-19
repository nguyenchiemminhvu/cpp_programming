#include <iostream>
#include <thread>
#include <condition_variable>
#include <vector>
#include <memory>
#include <mutex>
#include <map>
#include <iomanip>
#include <atomic>
#include <sstream>
#include <boost/asio.hpp>

// Define this macro to enable signal handling
#define ENABLE_SIGNAL_HANDLING

std::mutex log_mutex;
void logging(const std::string& message)
{
    std::scoped_lock its_lock(log_mutex);
    std::cout << message << std::endl;
}

class application : public std::enable_shared_from_this<application>
{
public:
    application(const std::string& name) 
        : name_(name),
          work_(std::make_shared<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(io_.get_executor())),
#ifdef ENABLE_SIGNAL_HANDLING
          signals_(io_, SIGINT, SIGTERM),
#endif
          is_initialized_(false),
          stopped_(false),
          stopped_called_(false),
          is_dispatching_(false),
          block_stop_condition_(false)
    {
    }

    bool init()
    {
        std::scoped_lock its_lock(initialize_mutex_);
        
        if (is_initialized_) {
            logging("Application \"" + name_ + "\" is already initialized.");
            return true;
        }

        // Perform initialization tasks here
        // In vsomeip: load configuration, setup routing, security, etc.
        logging("Initializing application \"" + name_ + "\".");
        
        is_initialized_ = true;
        return true;
    }

    void start()
    {
        // Check initialization state
        {
            std::scoped_lock its_lock(initialize_mutex_);
            if (!is_initialized_) {
                logging("ERROR: Trying to start uninitialized application \"" + name_ + "\"");
                return;
            }
        }

        const size_t io_thread_count = 5; // std::thread::hardware_concurrency();
        
        {
            std::scoped_lock its_lock(start_stop_mutex_);
            
            // Handle restart scenario
            if (io_.stopped()) {
                logging("Restarting io_context for application \"" + name_ + "\"");
                io_.restart();
            } else if (stop_thread_.joinable()) {
                logging("ERROR: Trying to start already started application \"" + name_ + "\"");
                return;
            }
            
            // Handle restart after stop
            if (stopped_) {
                std::scoped_lock its_block_lock(block_stop_mutex_);
                block_stop_condition_ = true;
                block_stop_cv_.notify_all();
                
                stopped_ = false;
                return;
            }
            
            stopped_ = false;
            stopped_called_ = false;

            logging("Starting application \"" + name_ + "\" using " 
                    + std::to_string(io_thread_count) + " threads");

#ifdef ENABLE_SIGNAL_HANDLING
            // Setup signal handlers
            signals_.async_wait([this](boost::system::error_code const& _error, int _signal) {
                if (!_error) {
                    logging("Received signal " + std::to_string(_signal) + ", shutting down...");
                    stop();
                }
            });
            logging("Signal handlers registered (SIGINT, SIGTERM)");
#endif

            start_caller_id_ = std::this_thread::get_id();

            // Start main dispatcher thread (like vsomeip's main_dispatch)
            {
                std::scoped_lock its_dispatcher_lock(dispatcher_mutex_);
                is_dispatching_ = true;
                
                auto its_main_dispatcher = std::make_shared<std::thread>([this]() {
                    main_dispatch();
                });
                
                dispatchers_[its_main_dispatcher->get_id()] = its_main_dispatcher;
            }

            // Create shutdown thread - waits for stop signal
            if (stop_thread_.joinable()) {
                stop_thread_.join();
            }
            stop_thread_ = std::thread(&application::shutdown, shared_from_this());

            // Create worker threads (all except main thread)
            for (size_t i = 0; i < io_thread_count - 1; i++) {
                auto its_thread = std::make_shared<std::thread>([this, i] {
                    std::ostringstream oss;
                    oss << "Started thread " << name_ << "_io" << std::setfill('0') << std::setw(2) << (i + 1)
                        << ", id " << std::hex << std::this_thread::get_id() << ".";
                    logging(oss.str());

                    while (true) {
                        try {
                            io_.run();

                            if (!stopped_) {
                                oss.str("");
                                oss.clear();
                                oss << "FATAL: I/O context has unexpectedly exited for thread " << name_ << "_io" 
                                    << std::setfill('0') << std::setw(2) << (i + 1)
                                    << ", id " << std::hex << std::this_thread::get_id() << ".";
                                logging(oss.str());
                            }
                            break;
                        } catch (const std::exception& e) {
                            logging("application::start() worker thread caught exception: " + std::string(e.what()));
                        }
                    }

                    oss.str("");
                    oss.clear();
                    oss << "Stopped thread " << name_ << "_io" << std::setfill('0') << std::setw(2) << (i + 1)
                        << ", id " << std::hex << std::this_thread::get_id() << ".";
                    logging(oss.str());
                });
                io_threads_.push_back(its_thread);
            }
        }

        std::ostringstream oss;
        oss << "Started thread " << name_ << "_io00, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss.str());

        // Main IO thread loop
        while (true) {
            try {
                io_.run();
                if (!stopped_) {
                    std::ostringstream oss_exit;
                    oss_exit << "FATAL: I/O context has unexpectedly exited for thread " << name_ 
                             << "_io00, id " << std::hex << std::this_thread::get_id() << ".";
                    logging(oss_exit.str());
                }
                
                // Wait for shutdown thread to complete
                if (stop_thread_.joinable()) {
                    stop_thread_.join();
                }
                break;
            } catch (const std::exception& e) {
                logging("application::start() main thread caught exception: " + std::string(e.what()));
            }
        }
        
        // Notify any blocked threads
        {
            std::scoped_lock its_block_lock(block_stop_mutex_);
            block_stop_condition_ = true;
            block_stop_cv_.notify_all();
        }

        {
            std::scoped_lock its_lock(start_stop_mutex_);
            stopped_ = false;
        }

        std::ostringstream oss_stop;
        oss_stop << "Stopped thread " << name_ << "_io00, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss_stop.str());
    }

    void stop()
    {
        logging("Stopping application \"" + name_ + "\".");

        bool block = true;
        {
            std::scoped_lock its_lock(start_stop_mutex_);
            
            // Prevent multiple stop calls
            if (stopped_called_) {
                return;
            }
            
            stop_caller_id_ = std::this_thread::get_id();
            stopped_ = true;
            stopped_called_ = true;
            
            // Check if stop is called from one of the IO threads
            for (const auto& thread : io_threads_) {
                if (thread->get_id() == std::this_thread::get_id()) {
                    block = false;
                    break;
                }
            }
            
            // Check if stop is called from the same thread as start (main IO thread)
            if (start_caller_id_ == stop_caller_id_) {
                block = false;
            }
        }

        // Notify shutdown thread OUTSIDE the lock scope (vsomeip pattern)
        // This prevents potential deadlocks and ensures the waiting thread
        // can acquire the lock immediately after being notified
        stop_cv_.notify_one();

        if (block) {
            // Block until shutdown completes with timeout
            std::unique_lock<std::mutex> block_stop_lock(block_stop_mutex_);
            bool completed = block_stop_cv_.wait_for(
                block_stop_lock, 
                std::chrono::milliseconds(1000), 
                [this] { return block_stop_condition_; }
            );
            
            if (!completed) {
                logging("WARNING: Stop operation timed out waiting for shutdown to complete.");
            }
            
            block_stop_condition_ = false;
        }
    }

    void main_dispatch()
    {
        std::ostringstream oss;
        oss << "Started main_dispatch thread, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss.str());

        // This simulates vsomeip's main_dispatch which handles
        // availability, message, and subscription handlers
        while (is_dispatching_) {
            try {
                // Simulate dispatcher work
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                // In vsomeip, this would process queued handlers:
                // - availability handlers
                // - message handlers
                // - subscription status handlers
                // - state handlers
            } catch (const std::exception& e) {
                logging("main_dispatch caught exception: " + std::string(e.what()));
            }
        }

        oss.str("");
        oss.clear();
        oss << "Stopped main_dispatch thread, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss.str());
    }

    void shutdown()
    {
        std::ostringstream oss;
        oss << "Started thread " << name_ << "_shutdown, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss.str());

        // Wait for stop signal
        {
            std::unique_lock<std::mutex> its_lock(start_stop_mutex_);
            stop_cv_.wait(its_lock, [this] { return stopped_called_; });
        }
        
        // Stop dispatching
        {
            std::scoped_lock its_lock(dispatcher_mutex_);
            is_dispatching_ = false;
        }

        // Join all dispatcher threads
        try {
            std::scoped_lock its_lock(dispatcher_mutex_);
            for (const auto& its_dispatcher : dispatchers_) {
                if (its_dispatcher.second->get_id() != stop_caller_id_) {
                    if (its_dispatcher.second->joinable()) {
                        its_dispatcher.second->join();
                    }
                } else {
                    // If stop() is called from a dispatcher thread,
                    // detach it to prevent deadlock. The thread will
                    // exit naturally when main_dispatch() completes.
                    logging("Detaching dispatcher thread called from stop()");
                    its_dispatcher.second->detach();
                }
            }
            dispatchers_.clear();
        } catch (const std::exception& e) {
            logging("application::shutdown() stopping dispatchers, caught exception: " + std::string(e.what()));
        }

        // Stop io_context
        try {
            work_.reset();
            io_.stop();
        } catch (const std::exception& e) {
            logging("application::shutdown() stopping io, caught exception: " + std::string(e.what()));
        }

        // Join all IO threads
        try {
            std::unique_lock its_lock(start_stop_mutex_);
            std::vector<std::shared_ptr<std::thread>> its_threads = io_threads_;
            io_threads_.clear();
            its_lock.unlock();
            
            for (auto& t : its_threads) {
                if (t->joinable()) {
                    t->join();
                }
            }
        } catch (const std::exception& e) {
            logging("application::shutdown() joining threads, caught exception: " + std::string(e.what()));
        }

        std::ostringstream oss_stop;
        oss_stop << "Stopped thread " << name_ << "_shutdown, id " << std::hex << std::this_thread::get_id() << ".";
        logging(oss_stop.str());
    }

private:
    std::string name_;
    boost::asio::io_context io_;
    std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
    
#ifdef ENABLE_SIGNAL_HANDLING
    boost::asio::signal_set signals_;
#endif
    
    // Initialization state
    std::mutex initialize_mutex_;
    bool is_initialized_;
    
    // Lifecycle state
    bool stopped_;
    bool stopped_called_;
    
    // Thread management
    std::vector<std::shared_ptr<std::thread>> io_threads_;
    std::thread stop_thread_;
    
    std::thread::id start_caller_id_;
    std::thread::id stop_caller_id_;
    
    // Synchronization
    std::mutex start_stop_mutex_;
    std::condition_variable stop_cv_;
    
    std::mutex block_stop_mutex_;
    std::condition_variable block_stop_cv_;
    bool block_stop_condition_;
    
    // Dispatcher management (like vsomeip)
    std::mutex dispatcher_mutex_;
    std::map<std::thread::id, std::shared_ptr<std::thread>> dispatchers_;
    bool is_dispatching_;
};

void test_1_stop_normally()
{
    auto app = std::make_shared<application>("MyApp");
    
    // Initialize the application (like vsomeip)
    if (!app->init()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return;
    }
    
    // Start application in a separate thread
    std::thread app_thread([app]() {
        app->start();
    });

    // Simulate some work
    std::cout << "Main thread: Simulating work..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Stop the application properly
    app->stop();
    
    if (app_thread.joinable()) {
        app_thread.join();
    }
    
    std::cout << "Test 1 complete.\n" << std::endl;
}

void test_2_stop_by_signal_SIGINT()
{
    auto app = std::make_shared<application>("MyApp");
    
    // Initialize the application
    if (!app->init()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return;
    }
    
    // Start application in a separate thread
    std::thread app_thread([app]() {
        app->start();
    });

    // Simulate some work
    std::cout << "Main thread: Simulating work..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Simulate sending SIGINT signal to the application
    std::cout << "Main thread: Sending SIGINT signal to application..." << std::endl;
    std::raise(SIGINT);
    
    // Give time for signal handler to trigger stop
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (app_thread.joinable()) {
        app_thread.join();
    }
    
    std::cout << "Test 2 complete.\n" << std::endl;
}

void test_3_multiple_stop_calls()
{
    std::cout << "=== Test 3: Multiple stop() calls (should be safe) ===" << std::endl;
    
    auto app = std::make_shared<application>("MyApp");
    
    if (!app->init()) {
        std::cout << "Failed to initialize application!" << std::endl;
        return;
    }
    
    std::thread app_thread([app]() {
        app->start();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Call stop multiple times - should be safe
    std::cout << "Main thread: Calling stop() first time..." << std::endl;
    app->stop();
    
    std::cout << "Main thread: Calling stop() second time (should be no-op)..." << std::endl;
    app->stop();
    
    std::cout << "Main thread: Calling stop() third time (should be no-op)..." << std::endl;
    app->stop();
    
    if (app_thread.joinable()) {
        app_thread.join();
    }
    
    std::cout << "Test 3 complete.\n" << std::endl;
}

int main()
{
    test_1_stop_normally();
    test_2_stop_by_signal_SIGINT();
    test_3_multiple_stop_calls();

    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}
