#include <iostream>
#include <concepts>
#include <numbers>

namespace automotive::controls
{
    using float32_t = float;
    using float64_t = double;

    template <typename T>
    concept floating_point = std::is_floating_point_v<T>;

    template <floating_point T>
    constexpr T rad_to_deg(T radians)
    {
        return radians * (180.0 / std::numbers::pi_v<T>);
    }

    template <floating_point T>
    constexpr T deg_to_rad(T degrees)
    {
        return degrees * (std::numbers::pi_v<T> / 180.0);
    }
}

int main()
{
    using namespace automotive::controls;
    constexpr float32_t steering_angle_deg = 45.0f;
    constexpr float32_t steering_angle_rad = deg_to_rad(steering_angle_deg);

    std::cout << "Steering angle in degrees: " << steering_angle_deg << "°" << std::endl;
    std::cout << "Steering angle in radians: " << steering_angle_rad << "rad" << std::endl;

    return 0;
}
