#include <iostream>

consteval int square(int x)
{
    return x * x;
}

constexpr int s5 = square(5);
constexpr int s6 = square(6);

consteval uint16_t make_ubx_message_key(uint8_t cls, uint8_t id)
{
    return (static_cast<uint16_t>(cls) << 8) | static_cast<uint16_t>(id);
}

constexpr uint16_t get_ubx_key_from_buffer(const uint8_t* buffer)
{
    return (static_cast<uint16_t>(buffer[0]) << 8) | static_cast<uint16_t>(buffer[1]);
}

#define UBX_MSG_KEY(cls, id) make_ubx_message_key(cls, id)
#define UBX_NAV_PVT_KEY UBX_MSG_KEY(0x01, 0x07)
#define UBX_NAV_STATUS_KEY UBX_MSG_KEY(0x01, 0x03)
#define UBX_NAV_DOP_KEY UBX_MSG_KEY(0x01, 0x04)

int main()
{
    std::cout << "s5: " << s5 << std::endl;
    std::cout << "s6: " << s6 << std::endl;

    uint16_t key_from_buffer = get_ubx_key_from_buffer(reinterpret_cast<const uint8_t*>("\x01\x07"));
    return 0;
}
