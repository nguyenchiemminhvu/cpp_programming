#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>

void trigger_callback(void (*callback)(int))
{
    callback(42);
}

// what compiler generates for a lambda function
class __lambda_unique_name
{
public:
    __lambda_unique_name(int val) : value(val) {}

    void operator()(int value) const
    {
        std::cout << "Callback called with value: " << value << std::endl;
    }

private:
    // captured variables would be stored here
    int value;
};

class lambda_capture_this
{
public:
    lambda_capture_this(int val) : value(val) {}

    void foo()
    {
        auto lambda = [this]() {
            std::cout << "Captured this->value: " << this->value << std::endl;
        };
        lambda();
    }

private:
    int value;
};

class net_logger
{
public:
    void register_callback(std::function<void(const std::string&)> callback)
    {
        m_event_callback = callback;
    }

    void trigger_log(const std::string& message)
    {
        if (m_event_callback)
        {
            m_event_callback(message);
        }
    }

private:
    std::function<void(const std::string&)> m_event_callback;
};

class net_manager
{
public:
    net_manager() {}
    ~net_manager()
    {
        if (m_worker.joinable())
        {
            m_worker.join();
        }
    }

    void start_monitoring()
    {
        m_logger.register_callback([this](const std::string& message) {
            m_packet_count++;
            std::cout << "Packet count: " << m_packet_count << ", Log: " << message << std::endl;
        });

        m_worker = std::thread([this]() {
            for (int i = 0; i < 5; ++i)
            {
                m_logger.trigger_log("Packet received");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
    }

private:
    std::thread m_worker;
    int m_packet_count = 0;
    net_logger m_logger;
};

int main()
{
    trigger_callback([](int value) {
        std::cout << "Callback called with value: " << value << std::endl;
    });

    // variables can be captured by the lambda
    int local_val = 10;
    auto f1 = [&]() {
        std::cout << "Captured local_val: " << local_val << std::endl;
    };

    auto f2 = [&]() {
        local_val++;
    };

    f1();
    f2();
    f1();

    // what behind the scenes
    __lambda_unique_name lambda_instance(local_val);
    lambda_instance(42);

    // capture this pointer
    lambda_capture_this obj(100);
    obj.foo();

    net_manager manager;
    manager.start_monitoring();

    return 0;
}
