#include <iostream>
#include <utility>
#include <string>

class buffer
{
public:
    buffer(size_t size)
        : size_(size)
        , data_(new char[size])
    {
        std::cout << "buffer(size_t size) called" << std::endl;
    }

    buffer(const buffer& other)
        : size_(other.size_)
        , data_(new char[other.size_])
    {
        std::copy(other.data_, other.data_ + other.size_, data_);
        std::cout << "buffer(const buffer& other) called" << std::endl;
    }

    buffer(buffer&& other) noexcept
    {
        size_ = other.size_;
        data_ = other.data_;
        other.size_ = 0;
        other.data_ = nullptr;
        std::cout << "buffer(buffer&& other) called" << std::endl;
    }

    buffer& operator=(const buffer& other)
    {
        if (this != &other)
        {
            if (data_ != nullptr)
            {
                delete[] data_;
                data_ = nullptr;
            }
            size_ = other.size_;
            data_ = new char[other.size_];
            std::copy(other.data_, other.data_ + other.size_, data_);
        }
        std::cout << "buffer& operator=(const buffer& other) called" << std::endl;
        return *this;
    }

    buffer& operator=(buffer&& other) noexcept
    {
        if (this != &other)
        {
            if (data_ != nullptr)
            {
                delete[] data_;
                data_ = nullptr;
            }
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        std::cout << "buffer& operator=(buffer&& other) called" << std::endl;
        return *this;
    }

    ~buffer()
    {
        delete[] data_;
        std::cout << "~buffer() called" << std::endl;
    }

private:
    size_t size_;
    char* data_;
};

using rvalue_reference = typename std::remove_reference<buffer>::type;

int main()
{
    buffer buf1(10);
    buffer buf2 = std::move(buf1); // Move constructor is called
    buffer buf3(20);
    buf3 = std::move(buf2); // Move assignment operator is called

    rvalue_reference ref = static_cast<rvalue_reference>(buf3);
    buffer buf4 = std::move(ref); // Move constructor is called
    return 0;
}
