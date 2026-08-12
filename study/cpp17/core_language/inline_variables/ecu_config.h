#include <cstdint>
#include <array>

inline constexpr uint32_t ECU_CONFIG_VERSION = 1;
inline uint32_t SYSTEM_TICK_RATE = 1000; // in Hz

struct register_map
{
    uint32_t mem_address;
    uint32_t reset_bit;
    const char* description;
};

namespace hardware_hal
{
    inline constexpr std::array<register_map, 3> registers = {{
        {0x40000000, 0x01, "Control Register"},
        {0x40000004, 0x02, "Status Register"},
        {0x40000008, 0x04, "Data Register"}
    }};
}