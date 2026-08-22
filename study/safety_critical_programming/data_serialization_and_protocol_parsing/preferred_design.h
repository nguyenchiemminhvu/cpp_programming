#pragma once
// =============================================================================
// PREFERRED DESIGN: Zero-Copy, Alignment-Safe, Endian-Explicit Stream Parser
//
// Goals:
//   - ZERO copy: the parser reads directly from the driver buffer via a view
//     (std::string_view in C++17, std::span<const std::byte> in C++20).
//   - ZERO heap allocation on the parse path.
//   - Alignment-independent: values are reassembled byte-by-byte with bit
//     shifts, so any start address is safe on any CPU.
//   - Endianness is spelled out in code, not left to the CPU. The same source
//     file gives the same result on Little-Endian ARM and Big-Endian PowerPC.
//   - Boundary checks are the first thing every parse function does, before a
//     single byte is dereferenced. This is the last line of defence against
//     malformed / malicious packets on V2X and diagnostic buses.
// =============================================================================

#include <cstddef>
#include <cstdint>
#include <string_view>

// -----------------------------------------------------------------------------
// SOME/IP header - a plain value object.
//
// Kept as an aggregate on purpose: no constructors, no invariants inside the
// struct itself. All validation lives in the parser, so the struct is trivially
// copyable and can be forwarded across component boundaries by value without
// worrying about hidden logic.
// -----------------------------------------------------------------------------
struct some_ip_header
{
    std::uint32_t message_id        = 0;
    std::uint32_t length            = 0;
    std::uint32_t request_id        = 0;
    std::uint8_t  protocol_version  = 0;
    std::uint8_t  interface_version = 0;
    std::uint8_t  message_type      = 0;
    std::uint8_t  return_code       = 0;
};

// -----------------------------------------------------------------------------
// Parse result codes. We return an enum instead of throwing:
//   - AUTOSAR C++14 discourages exceptions on the safety path.
//   - Many embedded builds use -fno-exceptions, in which case throwing is UB.
//   - The caller may need to distinguish "corrupt packet" (log & drop) from
//     "packet too short for header" (wait for more data).
// -----------------------------------------------------------------------------
enum class parse_status : std::uint8_t
{
    ok = 0,
    truncated,           // buffer smaller than the fixed header
    invalid_protocol,    // protocol_version != 0x01
    declared_length_out_of_range // length field claims more bytes than we have
};

// =============================================================================
// safe_network_parser - the actual decoder.
//
// The class holds no state. Every method is static; the parser only exists as
// a namespace-like grouping so that the byte-level helpers (readU32BigEndian,
// readU16BigEndian, readU8) stay private.
//
// Senior insight - why each byte-level helper is its own function:
//   * Reuse. DoIP, UDS, and CAN-FD all need Big-Endian multi-byte reads.
//   * Testability. Each helper can be unit-tested in isolation with the full
//     edge-case matrix (offset 0, offset near end, all-zeros, all-ones).
//   * Swappability. When profiling proves that bit-shifting is a bottleneck,
//     we can replace the body of a single helper with __builtin_bswap32 and
//     the rest of the codebase does not change.
// =============================================================================
class safe_network_parser
{
public:
    static constexpr std::size_t some_ip_header_size = 16;

    static parse_status parse_some_ip_header(std::string_view buffer,
                                             some_ip_header&  out_header) noexcept
    {
        // DEFENSIVE: check the boundary BEFORE dereferencing any byte.
        if (buffer.size() < some_ip_header_size)
        {
            return parse_status::truncated;
        }

        out_header.message_id        = read_u32_big_endian(buffer, 0);
        out_header.length            = read_u32_big_endian(buffer, 4);
        out_header.request_id        = read_u32_big_endian(buffer, 8);
        out_header.protocol_version  = read_u8(buffer, 12);
        out_header.interface_version = read_u8(buffer, 13);
        out_header.message_type      = read_u8(buffer, 14);
        out_header.return_code       = read_u8(buffer, 15);

        // AUTOSAR SOME/IP mandates protocol version 0x01. Anything else is
        // either a bug on the sender or a probing attempt - drop the frame.
        if (out_header.protocol_version != 0x01)
        {
            return parse_status::invalid_protocol;
        }

        // DEFENSIVE: the SOME/IP `length` field measures the bytes that follow
        // the first 8 bytes of the header. A hostile sender can put 0xFFFFFFFF
        // there hoping the receiver will read past the buffer. This is the
        // check that stops that class of attack cold.
        if (out_header.length > buffer.size() - 8)
        {
            return parse_status::declared_length_out_of_range;
        }

        return parse_status::ok;
    }

private:
    // -------------------------------------------------------------------------
    // Read 4 bytes in Network Byte Order (Big-Endian).
    //
    // Alignment note: buffer[offset] is a byte access - it never triggers an
    // alignment fault, even if `offset` is odd. The result is assembled into a
    // register-sized value with OR + shift, which is what the compiler would
    // do anyway if we asked for a portable load.
    // -------------------------------------------------------------------------
    static std::uint32_t read_u32_big_endian(std::string_view buf,
                                             std::size_t      offset) noexcept
    {
        return (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buf[offset]))     << 24) |
               (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buf[offset + 1])) << 16) |
               (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buf[offset + 2])) <<  8) |
               (static_cast<std::uint32_t>(static_cast<std::uint8_t>(buf[offset + 3])));
    }

    static std::uint16_t read_u16_big_endian(std::string_view buf,
                                             std::size_t      offset) noexcept
    {
        return static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(static_cast<std::uint8_t>(buf[offset]))     << 8) |
            (static_cast<std::uint16_t>(static_cast<std::uint8_t>(buf[offset + 1]))));
    }

    static std::uint8_t read_u8(std::string_view buf, std::size_t offset) noexcept
    {
        return static_cast<std::uint8_t>(buf[offset]);
    }
};

// =============================================================================
// Optional: portable Big-Endian -> Host converter using compiler intrinsics.
//
// Rule of thumb: reach for this only when a profiler on the target ECU shows
// that the bit-shift readers are actually a bottleneck. For SOME/IP headers
// (16 bytes) or CAN-FD frames (<= 64 bytes) the difference is invisible in
// end-to-end latency.
//
// Important: intrinsics operate on a value that is already in a CPU register.
// You still have to load the bytes out of the buffer with a mechanism that is
// alignment-safe (memcpy or the byte-by-byte reader above) before calling the
// intrinsic. Combining reinterpret_cast + bswap does NOT fix alignment.
// =============================================================================
inline std::uint32_t network_to_host_u32(std::uint32_t net_value) noexcept
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(net_value); // one ASM instruction: REV / BSWAP
#elif defined(_MSC_VER)
    return _byteswap_ulong(net_value);
#else
    return ((net_value & 0xFF000000u) >> 24) |
           ((net_value & 0x00FF0000u) >>  8) |
           ((net_value & 0x0000FF00u) <<  8) |
           ((net_value & 0x000000FFu) << 24);
#endif
}
