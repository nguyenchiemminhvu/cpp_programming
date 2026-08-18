#ifndef HAL_H
#define HAL_H

#include <iostream>

class hal
{
private:
    struct hal_func_table;

public:
    ~hal()
    {
        reset();
    }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, hal>>>
    explicit hal(T&& obj)
    {
        using concrete_hal_type = std::decay_t<T>;

        static_assert(sizeof(concrete_hal_type) <= STORAGE_SIZE, "Object size exceeds storage size");
        static_assert(alignof(concrete_hal_type) <= STORAGE_ALIGN, "Object alignment exceeds storage alignment");

        ::new (static_cast<void*>(storage)) concrete_hal_type(std::forward<T>(obj));
        func_table_ptr = &func_table_lookup<concrete_hal_type>;
    }

    hal(const hal& other)
    {
        if (other.func_table_ptr)
        {
            other.func_table_ptr->clone(storage, other.storage);
            func_table_ptr = other.func_table_ptr;
        }
    }

    hal& operator=(const hal& other)
    {
        if (this != &other)
        {
            reset();
            if (other.func_table_ptr)
            {
                other.func_table_ptr->clone(storage, other.storage);
                func_table_ptr = other.func_table_ptr;
            }
        }
        return *this;
    }

    void start()
    {
        if (func_table_ptr)
        {
            func_table_ptr->start(storage);
        }
    }

    void stop()
    {
        if (func_table_ptr)
        {
            func_table_ptr->stop(storage);
        }
    }

    void report_data(void* data)
    {
        if (func_table_ptr)
        {
            func_table_ptr->report_data(storage, data);
        }
    }

    void cleanup()
    {
        if (func_table_ptr)
        {
            func_table_ptr->cleanup(storage);
            func_table_ptr = nullptr;
        }
    }

private:
    void reset()
    {
        if (func_table_ptr)
        {
            func_table_ptr->cleanup(storage);
            func_table_ptr = nullptr;
        }
    }

    struct hal_func_table
    {
        void (*clone)(void*, const void*); // param 1: destination, param 2: source
        void (*start)(void*);
        void (*stop)(void*);
        void (*report_data)(void*, void*);
        void (*cleanup)(void*);
    };

    template <typename concrete_hal_type>
    static inline hal_func_table func_table_lookup = hal_func_table{
        .clone = [](void* dest, const void* src) { new (dest) concrete_hal_type(*static_cast<const concrete_hal_type*>(src)); },
        .start = [](void* obj) { static_cast<concrete_hal_type*>(obj)->start(); },
        .stop = [](void* obj) { static_cast<concrete_hal_type*>(obj)->stop(); },
        .report_data = [](void* obj, void* data) { static_cast<concrete_hal_type*>(obj)->report_data(data); },
        .cleanup = [](void* obj) { static_cast<concrete_hal_type*>(obj)->~concrete_hal_type(); }
    };

    static constexpr std::size_t STORAGE_SIZE = 1024;
    static constexpr std::size_t STORAGE_ALIGN = alignof(std::max_align_t);
    alignas(STORAGE_ALIGN) std::byte storage[STORAGE_SIZE];
    hal_func_table* func_table_ptr = nullptr;
};

class hal_ublox
{
public:
    void start()
    {
        std::cout << "Starting u-blox HAL" << std::endl;
    }

    void stop()
    {
        std::cout << "Stopping u-blox HAL" << std::endl;
    }

    void report_data(void* data)
    {
        std::cout << "Reporting data from u-blox HAL" << std::endl;
    }
};

#endif // HAL_H