#include <cstdint>
#include <iostream>
#include <string>
#include <cstring>

#define BUFFER_SIZE 1024

struct UnalignedStruct
{
    uint32_t n;
    union
    {
        char c_buffer[BUFFER_SIZE];
        uint8_t u8_buffer[BUFFER_SIZE];
    };
};

struct alignas(16) AlignedStruct
{
    uint16_t n;
    union
    {
        char c_buffer[BUFFER_SIZE];
        uint8_t u8_buffer[BUFFER_SIZE];
    };
};

int main()
{
    alignas(int32_t) int32_t local_buffer[BUFFER_SIZE];
    alignas(1) char local_buffer2[BUFFER_SIZE];

    std::cout << "Alignment of UnalignedStruct: " << alignof(UnalignedStruct) << std::endl;
    std::cout << "Alignment of AlignedStruct: " << alignof(AlignedStruct) << std::endl;

    std::cout << "Alignment of local_buffer: " << alignof(local_buffer) << std::endl;
    std::cout << "Alignment of local_buffer2: " << alignof(local_buffer2) << std::endl;

    // fill data to the buffers properly
    UnalignedStruct unaligned_struct;
    AlignedStruct aligned_struct;

    unaligned_struct.n = 10;
    aligned_struct.n = 10;

    std::string sample_data = "This is a sample data to fill the buffer.";
    std::memcpy(unaligned_struct.c_buffer, sample_data.c_str(), sample_data.size() + 1);
    std::memcpy(aligned_struct.c_buffer, sample_data.c_str(), sample_data.size() + 1);

    std::cout << "UnalignedStruct buffer content: " << unaligned_struct.c_buffer << std::endl;
    std::cout << "AlignedStruct buffer content: " << aligned_struct.c_buffer << std::endl;

    // safe casting
    uint8_t *ptr = reinterpret_cast<uint8_t *>(unaligned_struct.c_buffer);
    for (size_t i = 0; i < sample_data.size(); ++i)
    {
        std::cout << "unaligned_struct.c_buffer[" << i << "] = " << static_cast<int>(ptr[i]) << std::endl;
    }

    uint16_t *ptr16 = reinterpret_cast<uint16_t *>(aligned_struct.c_buffer);
    for (size_t i = 0; i < sample_data.size() / 2; ++i)
    {
        std::cout << "aligned_struct.c_buffer[" << i << "] = " << static_cast<int>(ptr16[i]) << std::endl;
    }

    return 0;
}