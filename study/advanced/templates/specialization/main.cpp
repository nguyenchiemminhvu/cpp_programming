#include <iostream>
#include <memory>
#include <cstdint>

template <typename T>
class ring_buffer
{
public:
    ring_buffer(size_t size)
        : size_(size), buffer_(new T[size]), head_(0), tail_(0), full_(false)
    {
        std::cout << "ring_buffer<" << typeid(T).name() << "> created with size " << size_ << std::endl;
    }

private:
    size_t size_;
    std::unique_ptr<T[]> buffer_;
    size_t head_;
    size_t tail_;
    bool full_;
};

template <typename T>
class ring_buffer<T*>
{
public:
    ring_buffer(size_t size)
        : size_(size), buffer_(new T*[size]), head_(0), tail_(0), full_(false)
    {
        std::cout << "ring_buffer<" << typeid(T*).name() << "> created with size " << size_ << std::endl;
    }

private:
    size_t size_;
    std::unique_ptr<T*[]> buffer_;
    size_t head_;
    size_t tail_;
    bool full_;
};

template <>
class ring_buffer<void*>
{
public:
    ring_buffer(size_t size)
        : size_(size), buffer_(new uint8_t*[size]), head_(0), tail_(0), full_(false)
    {
        std::cout << "ring_buffer<void*> created with size " << size_ << std::endl;
    }

private:
    size_t size_;
    std::unique_ptr<uint8_t*[]> buffer_;
    size_t head_;
    size_t tail_;
    bool full_;
};

int main()
{
    ring_buffer<int> int_buffer(10);
    ring_buffer<int*> int_ptr_buffer(10);
    ring_buffer<void*> void_ptr_buffer(10);

    return 0;
}