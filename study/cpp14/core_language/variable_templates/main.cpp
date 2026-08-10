#include <iostream>
#include <cstddef>
#include <cstdint>

template <typename T>
constexpr T pi = T(3.1415926535897932385);

template <typename T>
constexpr bool is_floating = std::is_floating_point<T>::value;

struct imu_sensor {};
struct wheel_speed_sensor {};
struct temperature_sensor {};

template <typename sensor_t>
constexpr std::size_t sensor_buffer_size = 0;

template <>
constexpr std::size_t sensor_buffer_size<imu_sensor> = 1024;

template <>
constexpr std::size_t sensor_buffer_size<wheel_speed_sensor> = 512;

template <>
constexpr std::size_t sensor_buffer_size<temperature_sensor> = 256;

template <typename sensor_t>
class sensor_ring_buffer
{
public:
    sensor_ring_buffer() : head(0) {}

    uint32_t size() const noexcept
    {
        return sensor_buffer_size<sensor_t>;
    }

private:
    uint8_t buffer[sensor_buffer_size<sensor_t>];
    uint32_t head;
};

int main()
{
    constexpr float pi_low_precision = pi<float>;
    constexpr double pi_high_precision = pi<double>;
    constexpr int pi_no_precision = pi<int>;

    std::cout << "Pi (float): " << pi_low_precision << std::endl;
    std::cout << "Pi (double): " << pi_high_precision << std::endl;

    if (is_floating<decltype(pi_high_precision)>)
    {
        std::cout << "Pi<double> is a floating point type." << std::endl;
    }
    else
    {
        std::cout << "Pi<double> is not a floating point type." << std::endl;
    }

    if (is_floating<decltype(pi_no_precision)>)
    {
        std::cout << "Pi<int> is a floating point type." << std::endl;
    }
    else
    {
        std::cout << "Pi<int> is not a floating point type." << std::endl;
    }

    sensor_ring_buffer<imu_sensor> imu_buffer;
    sensor_ring_buffer<wheel_speed_sensor> wheel_speed_buffer;
    sensor_ring_buffer<temperature_sensor> temperature_buffer;

    std::cout << "IMU buffer size: " << imu_buffer.size() << std::endl;
    std::cout << "Wheel speed buffer size: " << wheel_speed_buffer.size() << std::endl;
    std::cout << "Temperature buffer size: " << temperature_buffer.size() << std::endl;

    return 0;
}
