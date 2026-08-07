#include <iostream>
#include <tuple>
#include <cstdint>

// timestamp, fix_type, latitude, longitude, altitude, speed, heading
using location_data = std::tuple<uint64_t, int, double, double, double, double, double>;
using loc_timestamp_t = std::tuple_element<0, location_data>::type;
using loc_fix_type_t = std::tuple_element<1, location_data>::type;
using loc_latitude_t = std::tuple_element<2, location_data>::type;
using loc_longitude_t = std::tuple_element<3, location_data>::type;
using loc_altitude_t = std::tuple_element<4, location_data>::type;
using loc_speed_t = std::tuple_element<5, location_data>::type;
using loc_heading_t = std::tuple_element<6, location_data>::type;

location_data make_location_data(uint64_t timestamp, int fix_type, double latitude, double longitude, double altitude, double speed, double heading)
{
    return std::make_tuple(timestamp, fix_type, latitude, longitude, altitude, speed, heading);
}

int main()
{
    location_data data = make_location_data(1627847284, 3, 37.7749, -122.4194, 30.0, 50.0, 180.0);
    loc_timestamp_t timestamp = std::get<0>(data);
    loc_fix_type_t fix_type = std::get<1>(data);
    loc_latitude_t latitude = std::get<2>(data);
    loc_longitude_t longitude = std::get<3>(data);
    loc_altitude_t altitude = std::get<4>(data);
    loc_speed_t speed = std::get<5>(data);
    loc_heading_t heading = std::get<6>(data);

    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "Fix Type: " << fix_type << std::endl;
    std::cout << "Latitude: " << latitude << std::endl;
    std::cout << "Longitude: " << longitude << std::endl;
    std::cout << "Altitude: " << altitude << std::endl;
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Heading: " << heading << std::endl;

    location_data data2 = make_location_data(1627847285, 2, 34.0522, -118.2437, 100.0, 60.0, 90.0);
    auto [timestamp2, fix_type2, latitude2, longitude2, altitude2, speed2, heading2] = data2;
    std::cout << "Timestamp: " << timestamp2 << std::endl;
    std::cout << "Fix Type: " << fix_type2 << std::endl;
    std::cout << "Latitude: " << latitude2 << std::endl;
    std::cout << "Longitude: " << longitude2 << std::endl;
    std::cout << "Altitude: " << altitude2 << std::endl;
    std::cout << "Speed: " << speed2 << std::endl;
    std::cout << "Heading: " << heading2 << std::endl;

    std::tie(timestamp, fix_type, latitude, longitude, altitude, speed, heading) = data2;
    std::cout << "Timestamp: " << timestamp << std::endl;
    std::cout << "Fix Type: " << fix_type << std::endl;
    std::cout << "Latitude: " << latitude << std::endl;
    std::cout << "Longitude: " << longitude << std::endl;
    std::cout << "Altitude: " << altitude << std::endl;
    std::cout << "Speed: " << speed << std::endl;
    std::cout << "Heading: " << heading << std::endl;

    return 0;
}
