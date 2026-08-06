#include <iostream>
#include <ratio>

using deg_to_rad = std::ratio<31415926535897932, 1800000000000000000>; // Represents the ratio π/180
using rad_to_deg = std::ratio<1800000000000000000, 31415926535897932>; // Represents the ratio 180/π
using lat_lon_deg_to_milliarcsec = std::ratio<3600000, 1>; // Represents the ratio 3600000/1
using milliarcsec_to_lat_lon_deg = std::ratio<1, 3600000>; // Represents the ratio 1/3600000

int main()
{
    std::cout << "Numerator: " << std::kilo::num << ", Denominator: " << std::kilo::den << std::endl; // Represents the ratio 1000/1
    std::cout << "Numerator: " << std::milli::num << ", Denominator: " << std::milli::den << std::endl; // Represents the ratio 1/1000
    std::cout << "Numerator: " << std::micro::num << ", Denominator: " << std::micro::den << std::endl; // Represents the ratio 1/1000000
    std::cout << "Numerator: " << std::nano::num << ", Denominator: " << std::nano::den << std::endl; // Represents the ratio 1/1000000000

    double lat = 37.7749; // Latitude in degrees
    double lon = -122.4194; // Longitude in degrees

    // Convert latitude and longitude from degrees to milliarcseconds
    double lat_milliarcsec = lat * lat_lon_deg_to_milliarcsec::num / lat_lon_deg_to_milliarcsec::den;
    double lon_milliarcsec = lon * lat_lon_deg_to_milliarcsec::num / lat_lon_deg_to_milliarcsec::den;
    std::cout << "Latitude in milliarcseconds: " << lat_milliarcsec << std::endl;
    std::cout << "Longitude in milliarcseconds: " << lon_milliarcsec << std::endl;

    // Convert latitude and longitude from milliarcseconds back to degrees
    lat = lat_milliarcsec * milliarcsec_to_lat_lon_deg::num / milliarcsec_to_lat_lon_deg::den;
    lon = lon_milliarcsec * milliarcsec_to_lat_lon_deg::num / milliarcsec_to_lat_lon_deg::den;
    std::cout << "Latitude in degrees: " << lat << std::endl;
    std::cout << "Longitude in degrees: " << lon << std::endl;

    // deg to rad conversion
    double half_circle_deg = 180.0;
    double half_circle_rad = half_circle_deg * deg_to_rad::num / deg_to_rad::den;
    std::cout << "Half circle in radians: " << half_circle_rad << std::endl;

    // rad to deg conversion
    double half_circle_rad_back = half_circle_rad * rad_to_deg::num / rad_to_deg::den;
    std::cout << "Half circle in degrees: " << half_circle_rad_back << std::endl;

    return 0;
}
