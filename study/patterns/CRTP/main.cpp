#include <iostream>
#include <vector>

// basic pattern
template <typename Derived>
class base
{
public:
    void interface()
    {
        static_cast<Derived *>(this)->implementation();
    }
};

class derived : public base<derived>
{
public:
    void implementation()
    {
        std::cout << "derived implementation" << std::endl;
    }
};

template <typename Derived>
class addable
{
public:
    Derived operator+(const Derived &other) const
    {
        return static_cast<const Derived *>(this)->add(other);
    }
};

class vector2d : public addable<vector2d>
{
public:
    vector2d(float x, float y) : x_(x), y_(y) {}

    vector2d add(const vector2d &other) const
    {
        return vector2d(x_ + other.x_, y_ + other.y_);
    }

    void print() const
    {
        std::cout << "vector2d(" << x_ << ", " << y_ << ")" << std::endl;
    }
private:
    float x_;
    float y_;
};

template <typename Derived>
class instance_counter
{
public:
    static size_t get_count()
    {
        return count;
    }

protected:
    instance_counter()
    {
        ++count;
    }

    virtual ~instance_counter()
    {
        --count;
    }

private:
    inline static size_t count = 0U;
};

class engine1 : public instance_counter<engine1>
{
public:
    engine1() = default;
    ~engine1() = default;
};

class engine2 : public instance_counter<engine2>
{
public:
    engine2() = default;
    ~engine2() = default;
};

int main()
{
    vector2d v1(1.0f, 2.0f);
    vector2d v2(3.0f, 4.0f);
    vector2d v3 = v1 + v2;
    v3.print();

    engine1 e1_1, e1_2;
    {
        engine2 e2_1, e2_2, e2_3;
        std::cout << "engine2 count: " << engine2::get_count() << std::endl;
    }
    std::cout << "engine1 count: " << engine1::get_count() << std::endl;
    std::cout << "engine2 count: " << engine2::get_count() << std::endl;

    return 0;
}