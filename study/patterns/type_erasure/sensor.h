#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <memory>

class sensor
{
public:
    template <typename T>
    explicit sensor(T obj)
        : p_concept(std::make_unique<sensor_model<T>>(std::move(obj)))
    {
    }

    void read()
    {
        if (p_concept)
        {
            p_concept->read();
        }
    }

private:
    class sensor_concept
    {
    public:
        virtual ~sensor_concept() = default;
        virtual void read() = 0;
    };

    template <typename T>
    class sensor_model : public sensor_concept
    {
    public:
        explicit sensor_model(T obj)
            : sensor_(std::move(obj))
        {

        }

        void read() override
        {
            sensor_.read();
        }
    
    private:
        T sensor_;
    };

    std::unique_ptr<sensor_concept> p_concept;
};

class imu_sensor
{
public:
    void read()
    {
        std::cout << "accel x = " << 0 << std::endl;
        std::cout << "accel y = " << 0 << std::endl;
        std::cout << "accel z = " << 0 << std::endl;
    }
};

class motion_sensor
{
public:
    void read()
    {
        std::cout << "vehicle speed = " << 100 << std::endl;
        std::cout << "vehicle direction = " << 180 << std::endl;
    }
};

#endif // SENSOR_H