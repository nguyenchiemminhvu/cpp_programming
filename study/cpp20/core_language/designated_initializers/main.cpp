#include <iostream>
#include <cstdint>

enum class fix_type : uint8_t
{
    none = 0,
    two_d = 2,
    three_d = 3,
    dr = 4,
};

struct imu_data
{
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float vehicle_speed;
};

struct location_data
{
    uint64_t timestamp;
    fix_type fix;
    double latitude;
    double longitude;
    double altitude;
    float acc_horizontal;
    float acc_vertical;
    float speed;
    float speed_accuracy;
    uint8_t HDOP;
    uint8_t PDOP;
    uint8_t TDOP;
    uint8_t GDOP;

    imu_data imu;
};

int main()
{
    location_data loc = {
        .timestamp = 1234567890,
        .fix = fix_type::three_d,
        .latitude = 37.7749,
        .longitude = -122.4194,
        .altitude = 30.0,
        .acc_horizontal = 5.0f,
        .acc_vertical = 10.0f,
        .speed = 15.0f,
        .speed_accuracy = 1.0f,
        .HDOP = 2,
        .PDOP = 3,
        .TDOP = 4,
        .GDOP = 5,
        .imu = {
            .acc_x = 0.1f,
            .acc_y = 0.2f,
            .acc_z = 0.3f,
            .gyro_x = 0.01f,
            .gyro_y = 0.02f,
            .gyro_z = 0.03f,
            .vehicle_speed = 15.0f
        }
    };

    std::cout << "Timestamp: " << loc.timestamp << std::endl;
    std::cout << "Fix Type: " << static_cast<int>(loc.fix) << std::endl;
    std::cout << "Latitude: " << loc.latitude << std::endl;
    std::cout << "Longitude: " << loc.longitude << std::endl;
    std::cout << "Altitude: " << loc.altitude << std::endl;
    std::cout << "Horizontal Accuracy: " << loc.acc_horizontal << std::endl;
    std::cout << "Vertical Accuracy: " << loc.acc_vertical << std::endl;
    std::cout << "Speed: " << loc.speed << std::endl;
    std::cout << "Speed Accuracy: " << loc.speed_accuracy << std::endl;
    std::cout << "HDOP: " << static_cast<int>(loc.HDOP) << std::endl;
    std::cout << "PDOP: " << static_cast<int>(loc.PDOP) << std::endl;
    std::cout << "TDOP: " << static_cast<int>(loc.TDOP) << std::endl;
    std::cout << "GDOP: " << static_cast<int>(loc.GDOP) << std::endl;

    std::cout << "IMU Data:" << std::endl;
    std::cout << "  Acceleration X: " << loc.imu.acc_x << std::endl;
    std::cout << "  Acceleration Y: " << loc.imu.acc_y << std::endl;
    std::cout << "  Acceleration Z: " << loc.imu.acc_z << std::endl;
    std::cout << "  Gyroscope X: " << loc.imu.gyro_x << std::endl;
    std::cout << "  Gyroscope Y: " << loc.imu.gyro_y << std::endl;
    std::cout << "  Gyroscope Z: " << loc.imu.gyro_z << std::endl;
    std::cout << "  Vehicle Speed: " << loc.imu.vehicle_speed << std::endl;

    return 0;
}
