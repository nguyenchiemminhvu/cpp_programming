#include <iostream>
#include <functional>
#include <tuple>

void free_function(int a, int b)
{
    std::cout << "Free function called with arguments: " << a << ", " << b << std::endl;
}

struct functor
{
    void operator()(int a, int b)
    {
        std::cout << "Functor called with arguments: " << a << ", " << b << std::endl;
    }
};

class func_class
{
public:
    void member_function(int a, int b)
    {
        std::cout << "Member function called with arguments: " << a << ", " << b << std::endl;
    }
};

inline std::function<void(int, int)> f = [](int a, int b) {
    std::cout << "Lambda function called with arguments: " << a << ", " << b << std::endl;
};

int main()
{
    std::invoke(free_function, 1, 2);
    std::invoke(functor(), 3, 4);
    func_class obj;
    std::invoke(&func_class::member_function, obj, 5, 6);
    std::invoke(f, 7, 8);

    std::tuple<int, int> args = std::make_tuple(9, 10);
    std::apply(free_function, args);
    std::apply(functor(), args);
    std::apply(&func_class::member_function, std::tuple<func_class&, int, int>(obj, 11, 12));
    std::apply(f, args);

    return 0;
}
