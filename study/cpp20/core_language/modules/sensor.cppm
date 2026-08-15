module; // global module fragment: #includes are not part of the module's purview

#include <cstdint>

export module automotive.sensor;

export namespace automotive
{
    class sensor
    {
    public:
        explicit sensor(std::uint32_t id) : id_(id) {}

        std::uint32_t id() const { return id_; }
        int get_value() const { return 42; } // Placeholder for actual sensor value retrieval

    private:
        std::uint32_t id_;
    };
}