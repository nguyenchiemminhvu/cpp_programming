#include <iostream>
#include <type_traits>

template <typename T>
void check_type(T&& arg)
{
    if constexpr (std::is_pointer_v<std::decay_t<T>>)
    {
        auto ptr = std::forward<T>(arg);
        std::cout << "Argument is a pointer type." << std::endl;
    }
    else if constexpr (std::is_reference_v<T>)
    {
        auto& ref = arg;
        std::cout << "Argument is a reference type." << std::endl;
    }
    else
    {
        auto value = std::forward<T>(arg);
        std::cout << "Argument is not a pointer or reference type." << std::endl;
    }
}

struct atomic_register
{
    uint32_t atomic_bits;
};

struct mmio_register
{
    volatile uint32_t* address;
};

class peripheral_manager
{
public:
    template <typename TargetReg>
    void clear_device_bits(TargetReg& reg, uint32_t bits_to_clear) noexcept
    {
        static_assert(std::is_same_v<TargetReg, atomic_register> || std::is_same_v<TargetReg, mmio_register>,
                      "TargetReg must be either atomic_register or mmio_register.");
        if constexpr (std::is_same_v<TargetReg, atomic_register>)
        {
            reg.atomic_bits &= ~bits_to_clear;
        }
        else if constexpr (std::is_same_v<TargetReg, mmio_register>)
        {
            if (reg.address != nullptr)
            {
                *reg.address &= ~bits_to_clear;
            }
        }
    }
};

int main()
{
    check_type(42); // Argument is not a pointer or reference type.
    int x = 10;
    check_type(&x); // Argument is a pointer type.
    check_type(x); // Argument is a reference type.
    const int y = 20;
    check_type(&y); // Argument is a pointer type.
    check_type(y); // Argument is a reference type.

    peripheral_manager pm;
    atomic_register atomic_reg{0xFFFFFFFF};
    mmio_register mmio_reg{new uint32_t(0xFFFFFFFF)};

    pm.clear_device_bits(atomic_reg, 0x0000FFFF);
    pm.clear_device_bits(mmio_reg, 0x0000FFFF);

    delete mmio_reg.address;

    return 0;
}
