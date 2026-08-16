#include <format>
#include <iostream>
#include <string>
#include <cstdint>

struct location_data
{
    double latitude;
    double longitude;
    double altitude;
};

int main()
{
    location_data loc{
        .latitude = 37.7749,
        .longitude = -122.4194,
        .altitude = 30.0
    };

    std::string formatted = std::format("Location: ({:.4f}, {:.4f}), Altitude: {:.1f} m",
                                        loc.latitude, loc.longitude, loc.altitude);
    std::cout << formatted << std::endl;
    return 0;
}