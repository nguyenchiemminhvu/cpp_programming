#include <coroutine>
#include <iostream>
#include <exception>

template <typename T>
class Generator
{
public:
    struct promise_type;

    using handle_type =
        std::coroutine_handle<promise_type>;

    struct promise_type
    {
        T current_value{};

        Generator get_return_object()
        {
            return Generator{
                handle_type::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        std::suspend_always yield_value(T value) noexcept
        {
            current_value = value;
            return {};
        }

        void return_void() noexcept
        {
        }

        void unhandled_exception()
        {
            exception_ = std::current_exception();
        }

        std::exception_ptr exception_;
    };

private:
    handle_type handle_{};

    explicit Generator(handle_type handle)
        : handle_(handle)
    {
    }

public:
    Generator(const Generator&) = delete;

    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept
        : handle_(other.handle_)
    {
        other.handle_ = nullptr;
    }

    Generator& operator=(Generator&& other) noexcept
    {
        if (this != &other)
        {
            if (handle_)
            {
                handle_.destroy();
            }

            handle_ = other.handle_;
            other.handle_ = nullptr;
        }

        return *this;
    }

    ~Generator()
    {
        if (handle_)
        {
            handle_.destroy();
        }
    }

    class Iterator
    {
    private:
        handle_type handle_{};

    public:
        explicit Iterator(handle_type handle)
            : handle_(handle)
        {
        }

        Iterator& operator++()
        {
            handle_.resume();

            if (handle_.done() &&
                handle_.promise().exception_)
            {
                std::rethrow_exception(
                    handle_.promise().exception_);
            }

            return *this;
        }

        const T& operator*() const
        {
            return handle_.promise().current_value;
        }

        bool operator==(std::default_sentinel_t) const
        {
            return !handle_ || handle_.done();
        }
    };

    Iterator begin()
    {
        if (handle_)
        {
            handle_.resume();

            if (handle_.done() &&
                handle_.promise().exception_)
            {
                std::rethrow_exception(
                    handle_.promise().exception_);
            }
        }

        return Iterator{handle_};
    }

    std::default_sentinel_t end()
    {
        return {};
    }
};

Generator<int> numbers()
{
    co_yield 10;
    co_yield 20;
    co_yield 30;
}

int main()
{
    auto generator = numbers();

    for (int value : generator)
    {
        std::cout << value << '\n';
    }

    return 0;
}