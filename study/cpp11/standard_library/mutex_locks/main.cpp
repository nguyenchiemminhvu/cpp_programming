#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

// 1. Setup our Mutexes
std::mutex simple_mtx;             // For basic locking
std::recursive_mutex recur_mtx;    // For nesting locks
std::timed_mutex timed_mtx;        // For waiting with a timeout

int shared_balance = 100;

// Use Case A & B: std::lock_guard vs std::unique_lock
void basic_demo()
{
    // std::lock_guard locks immediately and unlocks at the end of the function
    {
        std::lock_guard<std::mutex> guard(simple_mtx);
        shared_balance += 10;
        std::cout << "LockGuard: Balance is now " << shared_balance << "\n";
    } // guard unlocks here

    // std::unique_lock allows us to unlock early manually
    {
        std::unique_lock<std::mutex> lock(simple_mtx);
        shared_balance += 5;
        std::cout << "UniqueLock: Balance updated to " << shared_balance << "\n";
        
        lock.unlock(); // We can unlock manually right here!
        std::cout << "UniqueLock: Doing other work unlocked...\n";
    } 
}

// Use Case C: std::recursive_mutex
void recursive_helper()
{
    std::lock_guard<std::recursive_mutex> lock(recur_mtx);
    std::cout << "Recursive: Inside helper function.\n";
}

void recursive_main()
{
    std::lock_guard<std::recursive_mutex> lock(recur_mtx);
    std::cout << "Recursive: Inside main function.\n";
    
    // Calls helper, which locks the SAME mutex. Safe because it is recursive.
    recursive_helper(); 
}

// Use Case D: std::timed_mutex
void timed_demo()
{
    // Try to get the lock, but give up if it takes longer than 50 milliseconds
    if (timed_mtx.try_lock_for(std::chrono::milliseconds(50)))
    {
        std::cout << "TimedLock: Got the lock successfully!\n";
        timed_mtx.unlock(); // Must unlock manually if try_lock succeeds
    }
    else
    {
        std::cout << "TimedLock: Too slow! Timed out and skipped.\n";
    }
}

int main()
{
    // Run basic guard and unique lock test
    std::thread t1(basic_demo);
    
    // Run recursive test
    std::thread t2(recursive_main);

    // Run timed test (we intentionally lock it first to trigger the timeout)
    timed_mtx.lock(); 
    std::thread t3(timed_demo); // This will time out because main thread holds the lock
    
    t1.join();
    t2.join();
    t3.join();
    
    timed_mtx.unlock(); // Clean up main thread lock
    std::cout << "All done!\n";
    return 0;
}
