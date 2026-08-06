#include <iostream>
#include <string>
#include <vector>
#include <memory>

class vehicle_ecu;

class base_diagnostic
{
public:
    virtual void run_diagnose() = 0;
};

class base_sensor
{
public:
    base_sensor() = default;
    virtual ~base_sensor() = default;
    
    void set_parent(std::shared_ptr<vehicle_ecu> ecu)
    {
        ecu_ = ecu;
    }

    virtual void read_data() = 0;

protected:
    std::weak_ptr<vehicle_ecu> ecu_;
};

class vehicle_ecu : public std::enable_shared_from_this<vehicle_ecu>
{
public:
    std::string get_ecu_name() const
    {
        return "Vehicle ECU";
    }

    void attach_sensor(base_sensor* sensor)
    {
        sensors_.emplace_back(sensor);
        sensor->set_parent(shared_from_this());
    }

    void run_diagnose()
    {
        std::cout << "Running vehicle ECU diagnostics..." << std::endl;
        for (const auto& sensor : sensors_)
        {
            if (auto diagnostic_sensor = std::dynamic_pointer_cast<base_diagnostic>(sensor))
            {
                diagnostic_sensor->run_diagnose();
            }
        }
    }

private:
    std::vector<std::shared_ptr<base_sensor>> sensors_;
};

class temperature_sensor : public base_sensor, public base_diagnostic
{
public:
    temperature_sensor() = default;
    virtual ~temperature_sensor() = default;

    void read_data() override
    {
        std::cout << "Reading temperature data..." << std::endl;
        std::cout << "Report data to ECU: " << ecu_.lock()->get_ecu_name() << std::endl;
    }

    void run_diagnose() override
    {
        std::cout << "Running temperature sensor diagnostics..." << std::endl;
        read_data();
    }
};

class pressure_sensor : public base_sensor, public base_diagnostic
{
public:
    pressure_sensor() = default;
    virtual ~pressure_sensor() = default;

    void read_data() override
    {
        std::cout << "Reading pressure data..." << std::endl;
        std::cout << "Report data to ECU: " << ecu_.lock()->get_ecu_name() << std::endl;
    }

    void run_diagnose() override
    {
        std::cout << "Running pressure sensor diagnostics..." << std::endl;
        read_data();
    }
};

int main()
{
    auto ecu = std::make_shared<vehicle_ecu>();
    auto t_sensor = std::make_shared<temperature_sensor>();
    auto p_sensor = std::make_shared<pressure_sensor>();

    ecu->attach_sensor(t_sensor.get());
    ecu->attach_sensor(p_sensor.get());
    ecu->run_diagnose();

    return 0;
}
