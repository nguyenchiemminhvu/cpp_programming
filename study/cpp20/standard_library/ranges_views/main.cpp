#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <numeric>
#include <ranges>

namespace automotive
{
    enum class can_status
    {
        OK,
        ERROR
    };

    struct can_message
    {
        can_status status;
        int id;
        std::array<std::uint8_t, 8> data;
    };
}

int main()
{
    std::vector<automotive::can_message> messages{
        {
            automotive::can_status::OK,
            1,
            {0x01, 0x02, 0x03, 0x04,
             0x05, 0x06, 0x07, 0x08}
        },
        {
            automotive::can_status::ERROR,
            2,
            {0x09, 0x0A, 0x0B, 0x0C,
             0x0D, 0x0E, 0x0F, 0x10}
        },
        {
            automotive::can_status::OK,
            3,
            {0x11, 0x12, 0x13, 0x14,
             0x15, 0x16, 0x17, 0x18}
        }
    };

    auto processed =
        messages
        | std::views::filter(
            [](const automotive::can_message& msg)
            {
                return msg.status == automotive::can_status::OK;
            })
        | std::views::filter(
            [](const automotive::can_message& msg)
            {
                const auto checksum =
                    std::accumulate(
                        msg.data.begin(),
                        msg.data.end(),
                        std::uint32_t{0});

                return checksum % 2U == 0U;
            })
        | std::views::take(2);

    for (const auto& msg : processed)
    {
        std::cout
            << "Message ID: " << msg.id
            << ", Status: "
            << (msg.status == automotive::can_status::OK
                    ? "OK"
                    : "ERROR")
            << ", Data: ";

        for (const auto byte : msg.data)
        {
            std::cout
                << std::hex
                << static_cast<unsigned int>(byte)
                << ' ';
        }

        std::cout << std::dec << '\n';
    }

    return 0;
}
