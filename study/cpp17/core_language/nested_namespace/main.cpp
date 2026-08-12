#include <iostream>

namespace lge::chassis::engine
{
    void start()
    {
        std::cout << "Engine started" << std::endl;
    }
}

namespace lge::chassis::transmission
{
    void shift()
    {
        std::cout << "Transmission shifted" << std::endl;
    }
}

namespace bosch::sensors
{
    void calibrate()
    {
        std::cout << "Sensors calibrated" << std::endl;
    }
}

int main()
{
    lge::chassis::engine::start();
    lge::chassis::transmission::shift();
    bosch::sensors::calibrate();

    return 0;
}