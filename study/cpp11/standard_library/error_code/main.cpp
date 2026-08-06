#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <system_error>
#include <cerrno>

std::error_code make_permmission_denied_error()
{
    return std::error_code(EACCES, std::system_category());
}

void check_permission()
{
    std::error_code ec = make_permmission_denied_error();
    if (ec)
    {
        std::cerr << "Error: " << ec.message() << std::endl;
    }
}

enum class can_error
{
    success = 0,
    overrun,
    bus_off,
    stuff_error,
    timeout,
    unknown_error
};

namespace std
{
    template <>
    struct is_error_code_enum<can_error> : true_type {};
}

class can_error_category : public std::error_category
{
public:
    const char* name() const noexcept override
    {
        return "can_error";
    }

    std::string message(int ev) const override
    {
        switch (static_cast<can_error>(ev))
        {
        case can_error::success:
            return "Success";
        case can_error::overrun:
            return "Overrun error";
        case can_error::bus_off:
            return "Bus off error";
        case can_error::stuff_error:
            return "Stuff error";
        case can_error::timeout:
            return "Timeout error";
        case can_error::unknown_error:
            return "Unknown error";
        default:
            return "Unrecognized error";
        }
    }
};

// overload the make_error_code function for can_error
std::error_code make_error_code(can_error e)
{
    static can_error_category category;
    return std::error_code(static_cast<int>(e), category);
}

class can_driver
{
public:
    can_driver() = default;

    std::error_code transmit(uint32_t id, const std::vector<uint8_t>& payload)
    {
        if (payload.size() > 8)
        {
            return can_error::stuff_error; // pretend that the transmission failed with a stuff error
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return can_error::timeout; // pretend that the transmission failed with a timeout error
    }
};

int main()
{
    check_permission();
    can_driver driver;
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09}; // payload with more than 8 bytes
    std::error_code ec = driver.transmit(0x123, payload);
    if (ec)
    {
        std::cerr << "CAN transmission error: " << ec.message() << std::endl;
    }

    payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; // payload with 7 bytes
    ec = driver.transmit(0x123, payload);
    if (ec)
    {
        std::cerr << "CAN transmission error: " << ec.message() << std::endl;
    }

    return 0;
}