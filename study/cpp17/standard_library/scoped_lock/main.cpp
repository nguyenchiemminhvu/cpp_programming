#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


// ============================================================
// Resource 1: Sensor data
// ============================================================

struct sensor_data
{
    std::array<std::byte, 8> payload{};
    std::mutex mtx;
};


// ============================================================
// Resource 2: Vehicle state
// ============================================================

struct vehicle_state
{
    int speed_kmh{0};
    int engine_rpm{0};

    std::mutex mtx;
};


// ============================================================
// Resource 3: Diagnostic information
// ============================================================

struct diagnostic_data
{
    bool fault_active{false};
    int fault_code{0};

    std::mutex mtx;
};


// ============================================================
// Resource manager
// ============================================================

class resource_manager
{
public:
    resource_manager(
        sensor_data& sensor,
        vehicle_state& state,
        diagnostic_data& diagnostic)
        : sensor_(sensor),
          state_(state),
          diagnostic_(diagnostic)
    {
        std::cout << "Resource manager created\n";
    }

    ~resource_manager()
    {
        std::cout << "Resource manager destroyed\n";
    }


    // --------------------------------------------------------
    // Operation 1
    //
    // Needs sensor + vehicle state.
    // --------------------------------------------------------

    void update_vehicle_state(std::size_t worker_id)
    {
        std::cout
            << "Worker " << worker_id
            << " updating vehicle state\n";

        {
            std::scoped_lock lock(
                sensor_.mtx,
                state_.mtx);

            std::cout
                << "Worker " << worker_id
                << " acquired SENSOR + STATE\n";

            // Read sensor data.
            const auto sensor_value =
                std::to_integer<int>(sensor_.payload[0]);

            // Update vehicle state.
            state_.speed_kmh = sensor_value;

            state_.engine_rpm = sensor_value * 100;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(50));
        }

        // Both locks have been released here.

        std::cout
            << "Worker " << worker_id
            << " released SENSOR + STATE\n";
    }


    // --------------------------------------------------------
    // Operation 2
    //
    // Needs vehicle state + diagnostics.
    // --------------------------------------------------------

    void update_diagnostics(std::size_t worker_id)
    {
        std::cout
            << "Worker " << worker_id
            << " updating diagnostics\n";

        {
            std::scoped_lock lock(
                state_.mtx,
                diagnostic_.mtx);

            std::cout
                << "Worker " << worker_id
                << " acquired STATE + DIAGNOSTIC\n";

            if (state_.speed_kmh > 120)
            {
                diagnostic_.fault_active = true;
                diagnostic_.fault_code = 1001;
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(50));
        }

        // Both locks released here.

        std::cout
            << "Worker " << worker_id
            << " released STATE + DIAGNOSTIC\n";
    }


    // --------------------------------------------------------
    // Operation 3
    //
    // Needs ALL THREE resources.
    // --------------------------------------------------------

    void process_vehicle(std::size_t worker_id)
    {
        std::cout
            << "Worker " << worker_id
            << " processing vehicle\n";

        {
            std::scoped_lock lock(
                sensor_.mtx,
                state_.mtx,
                diagnostic_.mtx);

            std::cout
                << "Worker " << worker_id
                << " acquired ALL resources\n";

            const auto sensor_value =
                std::to_integer<int>(sensor_.payload[0]);

            state_.speed_kmh = sensor_value;

            state_.engine_rpm =
                sensor_value * 100;

            if (state_.speed_kmh > 120)
            {
                diagnostic_.fault_active = true;
                diagnostic_.fault_code = 1001;
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
        }

        // ALL THREE mutexes are released here.

        std::cout
            << "Worker " << worker_id
            << " released ALL resources\n";
    }


private:
    sensor_data& sensor_;
    vehicle_state& state_;
    diagnostic_data& diagnostic_;
};


// ============================================================
// Worker functions
// ============================================================

void sensor_worker(resource_manager& manager)
{
    for (std::size_t i = 0; i < 5; ++i)
    {
        manager.update_vehicle_state(i);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(20));
    }
}


void diagnostic_worker(resource_manager& manager)
{
    for (std::size_t i = 0; i < 5; ++i)
    {
        manager.update_diagnostics(i + 10);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(30));
    }
}


void vehicle_worker(resource_manager& manager)
{
    for (std::size_t i = 0; i < 5; ++i)
    {
        manager.process_vehicle(i + 20);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(40));
    }
}


// ============================================================
// main
// ============================================================

int main()
{
    sensor_data sensor;
    vehicle_state state;
    diagnostic_data diagnostic;

    // Initial sensor value.
    sensor.payload[0] = std::byte{100};

    resource_manager manager(
        sensor,
        state,
        diagnostic);

    std::thread sensor_thread(
        sensor_worker,
        std::ref(manager));

    std::thread diagnostic_thread(
        diagnostic_worker,
        std::ref(manager));

    std::thread vehicle_thread(
        vehicle_worker,
        std::ref(manager));

    sensor_thread.join();
    diagnostic_thread.join();
    vehicle_thread.join();

    std::cout << "\nFinal state:\n";
    std::cout
        << "Speed: "
        << state.speed_kmh
        << " km/h\n";

    std::cout
        << "RPM: "
        << state.engine_rpm
        << '\n';

    std::cout
        << "Fault active: "
        << std::boolalpha
        << diagnostic.fault_active
        << '\n';

    std::cout
        << "Fault code: "
        << diagnostic.fault_code
        << '\n';

    return 0;
}