import automotive.sensor;

#include <iostream>

int main()
{
    automotive::sensor my_sensor(1);
    std::cout << "Sensor ID: " << my_sensor.id() << std::endl;
    std::cout << "Sensor Value: " << my_sensor.get_value() << std::endl;

    return 0;
}
