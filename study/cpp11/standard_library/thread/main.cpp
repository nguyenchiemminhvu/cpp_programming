#include <iostream>
#include <thread>

void thread_func()
{
    std::cout << "Hello from thread function!" << std::endl;
}

void thread_func_with_arg(int x)
{
    std::cout << "Hello from thread function with arg: " << x << std::endl;
}

class MyThread
{
public:
    void operator()()
    {
        std::cout << "Hello from MyThread!" << std::endl;
    }
};

class worker
{
public:
    void work()
    {
        std::cout << "Hello from worker thread!" << std::endl;
    }
};

int main()
{
    std::thread t([]() {
        std::cout << "Hello from thread!" << std::endl;
    });
    t.join();

    std::thread t2(thread_func);
    t2.join();

    std::thread t3(thread_func_with_arg, 42);
    t3.join();

    MyThread myThread;
    std::thread t4(myThread);
    t4.join();

    worker w;
    std::thread t5(&worker::work, &w);
    t5.join();

    std::thread daemon([]() {
        while (true) 
        {
            std::cout << "Daemon thread running..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    daemon.detach();

    pthread_t native = daemon.native_handle();
    sched_param param{99};
    pthread_setschedparam(native, SCHED_FIFO, &param);

    while (true) 
    {
        std::cout << "Main thread running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
