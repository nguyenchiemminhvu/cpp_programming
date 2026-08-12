#include <iostream>
#include <any>

struct event
{
    std::string topic;
    std::any payload;
};

class dispatcher
{
public:
    void dispatch(const event& e) noexcept
    {
        if (e.topic == "int_event")
        {
            try
            {
                int value = std::any_cast<int>(e.payload);
                std::cout << "Received int event with value: " << value << std::endl;
            }
            catch (const std::bad_any_cast& e)
            {
                std::cerr << "Bad any cast: " << e.what() << std::endl;
            }
        }
        else if (e.topic == "string_event")
        {
            try
            {
                std::string value = std::any_cast<std::string>(e.payload);
                std::cout << "Received string event with value: " << value << std::endl;
            }
            catch (const std::bad_any_cast& e)
            {
                std::cerr << "Bad any cast: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Unknown event topic: " << e.topic << std::endl;
        }
    }
};

int main()
{
    std::any a = 42;
    std::cout << "a contains: " << std::any_cast<int>(a) << std::endl;

    int* ptr = std::any_cast<int>(&a);
    if (ptr)
    {
        std::cout << "a contains: " << *ptr << std::endl;
    }
    else
    {
        std::cout << "a does not contain an int" << std::endl;
    }

    dispatcher d;
    event e1{"int_event", 100};
    d.dispatch(e1);
    event e2{"string_event", std::string("Hello, world!")};
    d.dispatch(e2);
    
    return 0;
}
