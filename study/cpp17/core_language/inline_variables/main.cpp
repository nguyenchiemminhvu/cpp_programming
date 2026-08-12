#include <iostream>
#include "ecu_config.h"
#include <chrono>
#include <functional>

using clock_now = std::function<std::chrono::time_point<std::chrono::steady_clock>()>;

inline clock_now now_func = []() {
    return std::chrono::steady_clock::now();
};

// in unit test for example
class override_clock
{
public:
    override_clock(clock_now new_clock_now_func)
        : original_clock_now_func(now_func)
    {
        now_func = new_clock_now_func;
    }

    ~override_clock()
    {
        now_func = original_clock_now_func;
    }

private:
    clock_now original_clock_now_func;
};

int main()
{
    std::cout << "ECU Config Version: " << ECU_CONFIG_VERSION << std::endl;
    std::cout << "System Tick Rate: " << SYSTEM_TICK_RATE << " Hz" << std::endl;

    std::cout << "Hardware Registers:" << std::endl;
    for (const auto& reg : hardware_hal::registers)
    {
        std::cout << "Address: 0x" << std::hex << reg.mem_address << ", Reset Bit: 0x" << std::hex << reg.reset_bit << ", Description: " << reg.description << std::endl;
    }

    bool is_expired = now() > std::chrono::steady_clock::now() + std::chrono::seconds(1);
    std::cout << "Is expired: " << std::boolalpha << is_expired << std::endl;

    const auto fake_now = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    {
        override_clock clock_override([fake_now]() { return fake_now; });
        is_expired = now() > std::chrono::steady_clock::now() + std::chrono::seconds(1);
        std::cout << "Is expired after override: " << std::boolalpha << is_expired << std::endl;
    }

    return 0;
}
