#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

class VehicleSafetyManager
{
private:
    std::atomic<bool> m_emergencyShutdownRequested{false};

public:
    void runControlLoop()
    {
        std::cout << "Control Loop started." << std::endl;
        while (!m_emergencyShutdownRequested.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        triggerEmergencyBrake();
    }

    void requestShutdown()
    {
        m_emergencyShutdownRequested.store(true);
    }

private:
    void triggerEmergencyBrake()
    {
        std::cout << "[CRITICAL] Emergency brake engaged! Vehicle stopped safely." << std::endl;
    }
};

int main()
{
    VehicleSafetyManager manager;
    std::thread controlThread(&VehicleSafetyManager::runControlLoop, &manager);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "[Sensor] Collision Detected!" << std::endl;
    manager.requestShutdown();

    controlThread.join();
    return 0;
}
