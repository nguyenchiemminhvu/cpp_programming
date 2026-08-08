#include <cstdint>

namespace registers
{
    constexpr std::uint8_t inj_mode_dual = 0b0000'0011;
    constexpr std::uint8_t pump_enable = 0b0000'0100;
    constexpr std::uint8_t diag_int_enable = 0b0000'1000;

    constexpr std::uint8_t safety_critical_check = inj_mode_dual | pump_enable | diag_int_enable;
}

int main()
{
    uint8_t cur_mask = registers::inj_mode_dual | registers::pump_enable;
    cur_mask &= 0b1111'0000; // Clear the lower 4 bits
    cur_mask |= registers::diag_int_enable; // Set the diag_int_enable bit

    return 0;
}
