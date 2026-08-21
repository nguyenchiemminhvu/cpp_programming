#include <iostream>
#include <type_traits>
#include <cstdint>
#include <new> // for placement new
#include <cstddef> // for std::size_t

constexpr uint8_t buffer[64] = {0};

using invoke_ptr_t = void(*)(void*, int);
invoke_ptr_t invoke_ptr = nullptr;

void free_callback(int x)
{
    std::cout << "Free callback called with value: " << x << std::endl;
}

struct functor_callback
{
    void operator()(int x) const
    {
        std::cout << "Functor callback called with value: " << x << std::endl;
    }
};

class callback_wrapper
{
public:
    void method_callback(int x)
    {
        std::cout << "Method callback called with value: " << x << std::endl;
    }

    static void static_method_callback(int x)
    {
        std::cout << "Static method callback called with value: " << x << std::endl;
    }
};

int main()
{
    auto free_func_lambda = [](int x) { std::cout << "Free function lambda called with value: " << x << std::endl; };
    
    new (const_cast<uint8_t*>(buffer)) decltype(free_func_lambda)(free_func_lambda);

    invoke_ptr = [](void* storage, int arg) {
        auto& func = *static_cast<decltype(free_func_lambda)*>(storage);
        func(arg);
    };

    invoke_ptr(const_cast<uint8_t*>(buffer), 42);

    // ===================================================

    auto functor = functor_callback();
    new (const_cast<uint8_t*>(buffer)) functor_callback(functor);

    invoke_ptr = [](void* storage, int arg) {
        auto& func = *static_cast<functor_callback*>(storage);
        func(arg);
    };

    invoke_ptr(const_cast<uint8_t*>(buffer), 84);

    // ===================================================

    auto wrapper = callback_wrapper();
    new (const_cast<uint8_t*>(buffer)) callback_wrapper(wrapper);

    invoke_ptr = [](void* storage, int arg) {
        auto& func = *static_cast<callback_wrapper*>(storage);
        func.method_callback(arg);
    };

    invoke_ptr(const_cast<uint8_t*>(buffer), 168);

    return 0;
}