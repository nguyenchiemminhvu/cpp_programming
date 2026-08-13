#include <iostream>
#include <memory_resource>
#include <vector>
#include <array>
#include <unordered_map>
#include <thread>

/**
class memory_resource
{
public:
    void* allocate(std::size_t bytes,
                   std::size_t alignment = alignof(std::max_align_t));

    void deallocate(void* ptr,
                    std::size_t bytes,
                    std::size_t alignment = alignof(std::max_align_t));

    bool is_equal(const memory_resource& other) const noexcept;

private:
    virtual void* do_allocate(std::size_t bytes,
                              std::size_t alignment) = 0;

    virtual void do_deallocate(void* ptr,
                               std::size_t bytes,
                               std::size_t alignment) = 0;

    virtual bool do_is_equal(const memory_resource& other) const noexcept = 0;
};
 */

void test_monotonic_buffer_resource()
{
    std::array<std::byte, 1024> buffer;
    std::pmr::monotonic_buffer_resource mem_pool(buffer.data(), buffer.size());
    std::pmr::vector<int> vec{&mem_pool};
    vec.reserve(10); // Reserve space for 10 elements
    for (int i = 0; i < 10; ++i)
    {
        vec.emplace_back(i);
    }
    for (const auto& val : vec)
    {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // check the addresses
    std::cout << "Address of buffer: " << static_cast<void*>(buffer.data()) << std::endl;
    std::cout << "Address of vec data: " << static_cast<void*>(vec.data()) << std::endl;
    std::cout << "Address of mem pool: " << &mem_pool << std::endl;

    vec.clear(); // Clear the vector, but do not deallocate memory
    mem_pool.release(); // Release all memory allocated from the pool
}

void test_unsynchronized_pool_resource()
{
    std::pmr::unsynchronized_pool_resource mem_pool;
    std::pmr::vector<int> vec{&mem_pool};

    for (int i = 0; i < 1'000; ++i)
    {
        vec.emplace_back(i);
    }

    vec.clear();

    for (int i = 10; i < 1'000; ++i)
    {
        vec.emplace_back(i);
    }

    vec.clear(); // Clear the vector, but do not deallocate memory
    mem_pool.release(); // Release all memory allocated from the pool
}

void test_synchronized_pool_resource()
{
    auto thread_worker = [](std::pmr::memory_resource* mem_pool)
    {
        std::pmr::string str{mem_pool};
        for (int i = 0; i < 100; ++i)
        {
            str += std::to_string(i);
        }
        std::cout << "Thread " << std::this_thread::get_id() << ": " << str << std::endl;
    };

    std::pmr::synchronized_pool_resource mem_pool;
    std::thread t1(thread_worker, &mem_pool);
    std::thread t2(thread_worker, &mem_pool);
    t1.join();
    t2.join();

    mem_pool.release(); // Release all memory allocated from the pool
}

void test_polymorphic_allocator_T()
{
    std::pmr::monotonic_buffer_resource mem_pool;
    std::pmr::polymorphic_allocator<int> alloc(&mem_pool);
    std::pmr::vector<int> vec(alloc);
    for (int i = 0; i < 10; ++i)
    {
        vec.emplace_back(i);
    }

    for (const auto& val : vec)
    {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void test_new_delete_resource()
{
    std::pmr::memory_resource* mem_pool = std::pmr::new_delete_resource();
    std::pmr::vector<int> vec{mem_pool};
    for (int i = 0; i < 10; ++i)
    {
        vec.emplace_back(i);
    }

    for (const auto& val : vec)
    {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void test_null_memory_resource()
{
    // Prevent fallback to heap allocation
    std::array<std::byte, 10> small_buffer;
    std::pmr::monotonic_buffer_resource small_mem_pool(small_buffer.data(), small_buffer.size(), std::pmr::null_memory_resource());
    std::pmr::vector<int> small_vec{&small_mem_pool};
    try
    {
        small_vec.reserve(20); // Reserve space for 20 elements, which exceeds the buffer
        for (int i = 0; i < 20; ++i)
        {
            small_vec.emplace_back(i);
        }
    }
    catch (const std::bad_alloc& e)
    {
        std::cout << "Caught std::bad_alloc: " << e.what() << std::endl;
    }
}

int main()
{
    test_monotonic_buffer_resource();
    test_unsynchronized_pool_resource();
    test_synchronized_pool_resource();
    test_polymorphic_allocator_T();
    test_new_delete_resource();
    test_null_memory_resource();

    return 0;
}