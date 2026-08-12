#include <iostream>
#include <cstddef>
#include <cstdint>
#include <array>

struct telematic_packet
{
    std::array<std::byte, 4> from_ip;
    std::array<std::byte, 4> to_ip;
    std::array<std::byte, 8> payload;
    std::array<std::byte, 2> checksum;
};

class telematic_packet_processor
{
public:
    void process_packet(const telematic_packet& packet)
    {
        std::cout << "Processing packet from: ";
        for (const auto& byte : packet.from_ip)
        {
            std::cout << std::to_integer<int>(byte) << ".";
        }
        std::cout << "\b to: ";
        for (const auto& byte : packet.to_ip)
        {
            std::cout << std::to_integer<int>(byte) << ".";
        }
        std::cout << "\b with payload: ";
        for (const auto& byte : packet.payload)
        {
            std::cout << std::to_integer<int>(byte) << " ";
        }
        std::cout << "and checksum: ";
        for (const auto& byte : packet.checksum)
        {
            std::cout << std::to_integer<int>(byte) << " ";
        }
        std::cout << std::endl;
    }

    bool build_and_validate_packet(telematic_packet& packet)
    {
        // Build the packet (for demonstration purposes, we just fill it with dummy data)
        packet.from_ip = {std::byte{192}, std::byte{168}, std::byte{1}, std::byte{1}};
        packet.to_ip = {std::byte{192}, std::byte{168}, std::byte{1}, std::byte{2}};
        packet.payload = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
                          std::byte{0x05}, std::byte{0x06}, std::byte{0x07}, std::byte{0x08}};
        packet.checksum = {std::byte{0xAA}, std::byte{0xBB}};

        // Validate the packet (for demonstration purposes, we just return true)
        return true;
    }
};

int main()
{
    std::byte b1{0x01};
    std::byte b2{0x02};
    std::cout << "Byte 1: " << std::to_integer<int>(b1) << std::endl;
    std::cout << "Byte 2: " << std::to_integer<int>(b2) << std::endl;
    std::cout << "Byte 1 & Byte 2: " << static_cast<int>(b1 & b2) << std::endl;
    std::cout << "Byte 1 | Byte 2: " << static_cast<int>(b1 | b2) << std::endl;
    std::cout << "Byte 1 ^ Byte 2: " << static_cast<int>(b1 ^ b2) << std::endl;
    std::cout << "Byte 1 << 1: " << static_cast<int>(b1 << 1) << std::endl;
    std::cout << "Byte 1 >> 1: " << static_cast<int>(b1 >> 1) << std::endl;
    telematic_packet_processor processor;
    telematic_packet packet;
    if (processor.build_and_validate_packet(packet))
    {
        processor.process_packet(packet);
    }
    else
    {
        std::cerr << "Failed to build and validate packet." << std::endl;
    }

    return 0;
}
