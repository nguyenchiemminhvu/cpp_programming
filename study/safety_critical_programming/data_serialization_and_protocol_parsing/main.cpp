// =============================================================================
// Data Serialization & Protocol Parsing (SOME/IP & CAN-FD)
//
// This demo contrasts a raw reinterpret_cast parser (bad_design.h) against a
// zero-copy, alignment-safe, endian-explicit stream parser (preferred_design.h).
//
// Run the program to see:
//   * how the bad parser silently produces wrong numbers on a Little-Endian
//     host when the wire format is Big-Endian, and
//   * how the safe parser reconstructs the exact values regardless of the
//     buffer alignment.
// =============================================================================

#include "bad_design.h"
#include "preferred_design.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <string_view>

namespace
{
// The literal on-wire frame every ECU on the bus would see.
// Big-Endian encoding, exactly as the sender emitted it.
constexpr std::uint8_t k_some_ip_frame[] = {
    0x12, 0x34, 0x56, 0x78, // message_id  = 0x12345678
    0x00, 0x00, 0x00, 0x08, // length      = 8
    0xAB, 0xCD, 0x00, 0x01, // request_id  = 0xABCD0001
    0x01,                   // protocol_version  = 1
    0x02,                   // interface_version = 2
    0x00,                   // message_type      = REQUEST
    0x00                    // return_code       = E_OK
};

// A wheel-speed frame the bad parser will try to consume. The +1 offset below
// simulates a DMA buffer starting on an odd address, which is what actually
// happens with Ethernet MAC receive rings.
constexpr std::uint8_t k_wheel_speed_bytes[] = {
    0x00, 0x00, 0x01, 0x51, 0x80, // timestamp   = 0x00015180 (Big-Endian on wire)
    0x03, 0xE8,                   // fl_speed    = 1000
    0x03, 0xF2                    // fr_speed    = 1010
};
} // namespace

int main()
{
    // -- 1. Bad parser on an unaligned, Big-Endian buffer ---------------------
    // We start reading at offset 1 to simulate an unaligned DMA hand-off.
    // On strict-alignment cores (Cortex-M0/M0+, TriCore) this branch would
    // trigger a HardFault. On x86_64 / Cortex-A it "works" but returns wrong
    // values because the CPU is Little-Endian while the wire is Big-Endian.
    std::printf("--- Bad parser (reinterpret_cast) ---\n");
    {
        bad_protocol_parser parser;
        parser.parse_network_buffer(k_wheel_speed_bytes + 1,
                                    sizeof(k_wheel_speed_bytes) - 1);
    }

    // -- 2. Safe parser on the same SOME/IP frame -----------------------------
    // std::string_view is a non-owning view: no copy, no allocation, zero cost
    // beyond a pointer and a length.
    std::printf("\n--- Safe parser (bit-shift, zero-copy) ---\n");
    {
        std::string_view payload(
            reinterpret_cast<const char*>(k_some_ip_frame),
            sizeof(k_some_ip_frame));

        some_ip_header header{};
        const auto status =
            safe_network_parser::parse_some_ip_header(payload, header);

        if (status == parse_status::ok)
        {
            std::printf("[OK] message_id   = 0x%08X\n",
                        header.message_id);
            std::printf("     length       = %u bytes\n",
                        header.length);
            std::printf("     request_id   = 0x%08X\n",
                        header.request_id);
            std::printf("     protocol_ver = %u\n",
                        static_cast<unsigned>(header.protocol_version));
            std::printf("     message_type = 0x%02X\n",
                        static_cast<unsigned>(header.message_type));
        }
        else
        {
            std::printf("[DROP] parse_status = %u\n",
                        static_cast<unsigned>(status));
        }
    }

    // -- 3. Safe parser rejecting a malformed / hostile packet ----------------
    // Same frame, but the length field is rewritten to 0xFFFFFFFF, a classic
    // attempt to make the receiver read past its buffer. The boundary check
    // inside parse_some_ip_header stops this before a single payload byte is
    // touched.
    std::printf("\n--- Safe parser rejecting a hostile length ---\n");
    {
        std::array<std::uint8_t, sizeof(k_some_ip_frame)> tampered{};
        for (std::size_t i = 0; i < tampered.size(); ++i)
        {
            tampered[i] = k_some_ip_frame[i];
        }
        tampered[4] = 0xFF;
        tampered[5] = 0xFF;
        tampered[6] = 0xFF;
        tampered[7] = 0xFF;

        std::string_view payload(
            reinterpret_cast<const char*>(tampered.data()), tampered.size());

        some_ip_header header{};
        const auto status =
            safe_network_parser::parse_some_ip_header(payload, header);

        std::printf("parse_status = %u (expected %u = length_out_of_range)\n",
                    static_cast<unsigned>(status),
                    static_cast<unsigned>(
                        parse_status::declared_length_out_of_range));
    }

    return 0;
}
