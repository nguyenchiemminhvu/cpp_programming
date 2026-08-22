#pragma once
// =============================================================================
// BAD DESIGN: reinterpret_cast a raw byte buffer to a struct
//
// Problems demonstrated in this header:
//   1. Alignment fault. Automotive DMA / Ethernet drivers hand out buffers
//      whose start address is not necessarily aligned to 4 or 8 bytes. Casting
//      such a buffer to a struct containing uint32_t triggers a HardFault on
//      strict-alignment cores (ARM Cortex-M0/M0+, Cortex-R, TriCore).
//   2. Endianness bug. Network protocols (SOME/IP, DoIP, most CAN-FD payloads)
//      transmit multi-byte fields as Big-Endian. Little-Endian CPUs will read
//      the value with bytes reversed. The system does not crash, it just uses
//      wrong numbers - the most expensive bug class in HIL integration.
//   3. Strict aliasing violation (MISRA C++ Rule 5-2-7). Accessing the same
//      memory through two unrelated pointer types is Undefined Behaviour.
//      Polyspace / QAC / Coverity will flag it, and the optimizer is free to
//      break the intended logic during LTO.
//   4. No boundary validation. If the caller lies about the size, the parser
//      reads past the end of the buffer -> Buffer Overflow Attack surface.
// =============================================================================

#include <cstddef>
#include <cstdint>
#include <cstdio>

// -----------------------------------------------------------------------------
// A wheel-speed packet as it appears on the wire. The compiler is free to add
// padding, so `sizeof(wheel_speed_packet)` may differ from the on-wire size.
// -----------------------------------------------------------------------------
struct wheel_speed_packet
{
    std::uint32_t timestamp;         // 4 bytes, needs 4-byte alignment
    std::uint16_t front_left_speed;  // 2 bytes, needs 2-byte alignment
    std::uint16_t front_right_speed; // 2 bytes
};

class bad_protocol_parser
{
public:
    // The caller passes the raw buffer straight from the driver. The parser
    // "trusts" that:
    //   - the address is aligned for the target struct,
    //   - the byte order matches the CPU,
    //   - the size is at least sizeof(wheel_speed_packet).
    // None of those assumptions hold in practice.
    void parse_network_buffer(const std::uint8_t* raw_buffer, std::size_t size)
    {
        if (size < sizeof(wheel_speed_packet))
        {
            return;
        }

        // PROBLEM 1: raw_buffer may point to an odd address -> alignment fault.
        // PROBLEM 2: values are interpreted with the CPU's byte order, not the
        //            wire's byte order -> silently wrong data.
        // PROBLEM 3: reinterpret_cast between unrelated pointer types is UB.
        const auto* packet =
            reinterpret_cast<const wheel_speed_packet*>(raw_buffer);

        std::printf("[BAD] timestamp=0x%08X  fl_speed=0x%04X  fr_speed=0x%04X\n",
                    packet->timestamp,
                    packet->front_left_speed,
                    packet->front_right_speed);
    }
};
