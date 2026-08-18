#include <iostream>
#include <vector>
#include <memory>
#include "sensor.h"
#include "engine.h"
#include "hal.h"

int main()
{
    engine eg{position_engine{}};
    eg.start();
    eg.stop();

    imu_sensor imu{};
    motion_sensor motion{};

    std::vector<sensor> sensors;
    sensors.push_back(sensor(imu));
    sensors.push_back(sensor(motion));

    for (auto& s : sensors)
    {
        s.read();
    }

    hal ublox_hal{hal_ublox{}};
    ublox_hal.start();
    ublox_hal.report_data(nullptr); // Example of reporting data
    ublox_hal.report_data(&eg); // Example of reporting data
    ublox_hal.stop();

    return 0;
}