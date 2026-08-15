#include <iostream>
#include <concepts>
#include <type_traits>
#include <array>

template <typename T>
concept must_be_integral = std::is_integral_v<T>;

template <typename T>
concept must_be_floating_point = std::is_floating_point_v<T>;

template <typename T, std::size_t N>
class ring_buffer;

template <must_be_integral T, std::size_t N>
class ring_buffer<T, N>
{
public:
    ring_buffer() : head(0), tail(0) {}

    void push(const T& value)
    {
        buffer[head] = value;
        head = (head + 1) % N;
        if (head == tail)
        {
            tail = (tail + 1) % N; // Overwrite the oldest element
        }
    }

    T pop()
    {
        if (head == tail)
        {
            throw std::runtime_error("Buffer is empty");
        }
        T value = buffer[tail];
        tail = (tail + 1) % N;
        return value;
    }

    T front() const
    {
        if (head == tail)
        {
            throw std::runtime_error("Buffer is empty");
        }
        return buffer[tail];
    }

private:
    std::array<T, N> buffer;
    uint32_t head;
    uint32_t tail;
};

// template specialization for floating point types
template <must_be_floating_point T, std::size_t N>
class ring_buffer<T, N>
{
public:
    ring_buffer() : head(0), tail(0) {}

    void push(const T& value)
    {
        buffer[head] = value;
        head = (head + 1) % N;
        if (head == tail)
        {
            tail = (tail + 1) % N; // Overwrite the oldest element
        }
    }

    T pop()
    {
        if (head == tail)
        {
            throw std::runtime_error("Buffer is empty");
        }
        T value = buffer[tail];
        tail = (tail + 1) % N;
        return value;
    }

    T front() const
    {
        if (head == tail)
        {
            throw std::runtime_error("Buffer is empty");
        }
        return buffer[tail];
    }

private:
    std::array<T, N> buffer;
    uint32_t head;
    uint32_t tail;
};

template <typename T>
class resizable_buffer
{
public:
    resizable_buffer() : buffer_(nullptr), size_(0), capacity_(0) {}

    void resize(std::size_t n)
    {
        if (n > capacity_)
        {
            T* new_buffer = new T[n];
            if (buffer_)
            {
                std::copy(buffer_, buffer_ + size_, new_buffer);
                delete[] buffer_;
            }
            buffer_ = new_buffer;
            capacity_ = n;
        }
        size_ = n;
    }

    std::size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return !buffer_ || size_ == 0;
    }

    T* data() const
    {
        return buffer_;
    }

private:
    T* buffer_;
    std::size_t size_;
    std::size_t capacity_;
};

template <typename T>
concept resizable = requires(T& t, std::size_t n) {
    { t.resize(n) } -> std::same_as<void>;
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.empty() } -> std::same_as<bool>;
};

template <typename T>
requires resizable<T>
void process_resizable_buffer(T& buffer)
{
    buffer.resize(10);
    std::cout << "Buffer size: " << buffer.size() << std::endl;
    std::cout << "Buffer empty: " << std::boolalpha << buffer.empty() << std::endl;
}

template <typename T>
concept sortable = requires(const T& left, const T& right) {
    { left < right } -> std::convertible_to<bool>;
    { left > right } -> std::convertible_to<bool>;
};

template <sortable T>
void sort_buffer(T* buffer, std::size_t size)
{
    std::sort(buffer, buffer + size);
}

int main()
{
    ring_buffer<int, 5> int_buffer; // compile ok
    ring_buffer<double, 5> double_buffer; // compile ok with specialization
    // ring_buffer<std::string, 5> string_buffer; // compile error, std::string is not integral or floating point

    resizable_buffer<int> resizable_int_buffer;
    process_resizable_buffer(resizable_int_buffer); // compile ok

    sort_buffer(resizable_int_buffer.data(), resizable_int_buffer.size()); // compile ok
    return 0;
}
