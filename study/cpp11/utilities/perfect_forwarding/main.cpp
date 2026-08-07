#include <iostream>
#include <utility>
#include <cstdint>

class buffer
{
public:
    buffer(uint32_t size)
        : size_(size)
    {
        bytes_ = new uint8_t[size_];
    }

    ~buffer()
    {
        if (bytes_)
            delete[] bytes_;
    }

    buffer(const buffer& other) = delete;
    buffer& operator=(const buffer& other) = delete;

    buffer(buffer&& other) noexcept
        : bytes_(other.bytes_)
        , size_(other.size_)
    {
        other.bytes_ = nullptr;
        other.size_ = 0;
        std::cout << "Move constructor called" << std::endl;
    }

    buffer& operator=(buffer&& other) noexcept
    {
        if (this != &other)
        {
            if (bytes_)
                delete[] bytes_;
            bytes_ = other.bytes_;
            size_ = other.size_;
            other.bytes_ = nullptr;
            other.size_ = 0;
        }
        std::cout << "Move assignment operator called" << std::endl;
        return *this;
    }

    uint32_t size() const { return size_; }

private:
    uint8_t* bytes_;
    uint32_t size_;
};

void process_buffer(buffer& buf)
{
    std::cout << "Processing buffer& of size: " << buf.size() << std::endl;
}

void process_buffer(buffer&& buf)
{
    std::cout << "Processing buffer&& of size: " << buf.size() << std::endl;
}

template <typename T>
void process_buffer(T&& buf)
{
    std::cout << "Processing universal reference of size: " << buf.size() << std::endl;
    process_buffer(std::forward<T>(buf));
}

int main()
{
    buffer buf1(1024);
    buffer buf2(std::move(buf1));
    buffer buf3 = std::move(buf2);

    process_buffer(buf3);
    process_buffer(std::forward<buffer>(buf3));
    process_buffer(std::forward<buffer&&>(buf3));

    return 0;
}
