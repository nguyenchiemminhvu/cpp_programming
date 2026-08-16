#include <iostream>
#include <bit>

int main()
{
    float f = 3.14f;

    // Use std::bit_cast to reinterpret the bits of the float as an integer
    uint32_t i = std::bit_cast<uint32_t>(f);
    std::cout << "Float: " << f << ", Bit-cast to uint32_t: " << i << std::endl;

    f = std::bit_cast<float>(i);
    std::cout << "Bit-cast back to float: " << f << std::endl;

    return 0;
}
