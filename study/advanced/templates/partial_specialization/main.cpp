#include <iostream>
#include <memory>
#include <cstdint>

template <typename T, typename U>
class operation
{
public:
    void execute(T a, U b)
    {
        std::cout << "Executing operation with types: " << typeid(T).name() << " and " << typeid(U).name() << std::endl;
    }
};

template <typename T>
class operation<T, int>
{
public:
    void execute(T a, int b)
    {
        std::cout << "Executing operation with type: " << typeid(T).name() << " and int" << std::endl;
    }
};

template <typename T, typename U>
class processor
{
public:
    static void process(T a, U b)
    {
        std::cout << "Processing with types: " << typeid(T).name() << " and " << typeid(U).name() << std::endl;
    }
};

template <typename T>
class processor<T, int>
{
public:
    static void process(T a, int b)
    {
        std::cout << "Processing with type: " << typeid(T).name() << " and int" << std::endl;
    }
};

// tricky way to call the partial specialization of process_data function
template <typename T, typename U>
void process_data(T a, U b)
{
    processor<T, U>::process(a, b);
}

int main()
{
    operation<double, int> op1;
    op1.execute(3.14, 42);

    operation<std::string, std::string> op2;
    op2.execute("Hello", "World");

    process_data(3.14, 42);
    process_data("Hello", "World");

    return 0;
}