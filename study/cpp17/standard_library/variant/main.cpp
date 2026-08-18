#include <iostream>
#include <array>
#include <variant>
#include <cstdint>

template <typename... Handlers>
struct Overloaded : Handlers...
{
    using Handlers::operator()...;
};

template <typename... Handlers>
Overloaded(Handlers...) -> Overloaded<Handlers...>;

struct can_bus_frame
{
    uint32_t id;
    std::array<uint8_t, 8> payload;
};

struct lin_bus_frame
{
    uint8_t id;
    std::array<uint8_t, 8> payload;
};

struct diag_message
{
    uint8_t id;
    const uint8_t* payload;
};

using ecu_event = std::variant<can_bus_frame, lin_bus_frame, diag_message>;

class ecu_router
{
public:
    void route(const ecu_event& event) noexcept
    {
        if (auto* can_ptr = std::get_if<can_bus_frame>(&event); can_ptr)
        {
            std::cout << "Routing CAN bus frame with ID: " << can_ptr->id << std::endl;
        }
        else if (auto* lin_ptr = std::get_if<lin_bus_frame>(&event); lin_ptr)
        {
            std::cout << "Routing LIN bus frame with ID: " << static_cast<int>(lin_ptr->id) << std::endl;
        }
        else if (auto* diag_ptr = std::get_if<diag_message>(&event); diag_ptr)
        {
            std::cout << "Routing diagnostic message with ID: " << static_cast<int>(diag_ptr->id) << std::endl;
        }
        else
        {
            std::cerr << "Unknown event type!" << std::endl;
        }
    }

    void route_overloaded(const ecu_event& event) noexcept
    {
        std::visit([](const auto& e) {
            Overloaded{
                [](const can_bus_frame& can) {
                    std::cout << "Routing CAN bus frame with ID: " << can.id << std::endl;
                },
                [](const lin_bus_frame& lin) {
                    std::cout << "Routing LIN bus frame with ID: " << static_cast<int>(lin.id) << std::endl;
                },
                [](const diag_message& diag) {
                    std::cout << "Routing diagnostic message with ID: " << static_cast<int>(diag.id) << std::endl;
                }
            }(e);
        }, event);
    }
};

namespace ecu
{
    struct normal_state
    {
        int temperature;
    };

    struct high_load_state
    {
        int load_percentage;
    };

    struct error_state
    {
        int error_code;
    };

    using ecu_state = std::variant<normal_state, high_load_state, error_state>;

    void report_state(const ecu_state& state) noexcept
    {
        std::visit([](const auto& state) {
            using T = std::decay_t<decltype(state)>;
            if constexpr (std::is_same_v<T, normal_state>)
            {
                std::cout << "ECU is in normal state with temperature: " << state.temperature << std::endl;
            }
            else if constexpr (std::is_same_v<T, high_load_state>)
            {
                std::cout << "ECU is in high load state with load percentage: " << state.load_percentage << std::endl;
            }
            else if constexpr (std::is_same_v<T, error_state>)
            {
                std::cout << "ECU is in error state with error code: " << state.error_code << std::endl;
            }
        }, state);
    }
}

int main()
{
    std::variant<int, float, std::string> v;

    try
    {
        v = 42; // Assign an int
        std::cout << "v holds int: " << std::get<int>(v) << std::endl;

        v = 3.14f; // Assign a float
        std::cout << "v holds float: " << std::get<float>(v) << std::endl;

        v = "Hello, Variant!"; // Assign a string
        std::cout << "v holds string: " << std::get<std::string>(v) << std::endl;

        // Attempt to get the wrong type
        std::cout << "Trying to get int from v: " << std::get<int>(v) << std::endl;
    }
    catch (const std::bad_variant_access& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    ecu_router router;
    can_bus_frame can_frame{0x123, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}};
    lin_bus_frame lin_frame{0x45, {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10}};
    diag_message diag_msg{0x01, nullptr};

    router.route(can_frame);
    router.route(lin_frame);
    router.route(diag_msg);

    router.route_overloaded(can_frame);
    router.route_overloaded(lin_frame);
    router.route_overloaded(diag_msg);

    ecu::ecu_state state = ecu::normal_state{75};
    ecu::report_state(state);

    ecu::ecu_state high_load = ecu::high_load_state{90};
    ecu::report_state(high_load);

    ecu::ecu_state error = ecu::error_state{404};
    ecu::report_state(error);

    return 0;
}
