#pragma once

#include <cassert>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#ifndef SINGLETON_ASSERT
    #define SINGLETON_ASSERT(cond, msg) assert((cond) && (msg))
#endif

template <typename T>
class singleton
{
public:
    using value_type = T;

    template <typename... Args>
    static void create(Args&&... args)
    {
        SINGLETON_ASSERT(!is_initialized_, "Singleton instance is already initialized.");
        new (&storage_) value_type(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

    static void destroy()
    {
        SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
        reinterpret_cast<value_type*>(&storage_)->~value_type();
        is_initialized_ = false;
    }

    static T& instance()
    {
        SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
        return *reinterpret_cast<value_type*>(&storage_);
    }

    static bool is_initialized() noexcept
    {
        return is_initialized_;
    }

private:

    singleton() = delete;
    ~singleton() = delete;
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;

    using storage_type = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;

    static storage_type storage_;
    static bool is_initialized_;
};

template <typename T>
typename singleton<T>::storage_type singleton<T>::storage_{};

template <typename T>
bool singleton<T>::is_initialized_ = false;