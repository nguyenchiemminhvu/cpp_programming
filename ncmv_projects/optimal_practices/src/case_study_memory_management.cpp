#include <benchmark/benchmark.h>

#include <iostream>
#include <memory>
#include <cstring>
#include <string>

static void BM_cpp_builtin_allocator(benchmark::State& state)
{
    for (auto _ : state)
    {
        int* p = new int[10000U];
        delete[] p;
        benchmark::DoNotOptimize(p);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_cpp_builtin_allocator);

////////////////////////////////////////////////////////////////////

template <typename T>
struct simple_allocator
{
    using value_type = T;

    simple_allocator() = default;

    template <class U>
    simple_allocator(const simple_allocator<U>&) {}

    T* allocate(std::size_t n, void const* = 0)
    {
        return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* ptr, size_t)
    {
        ::operator delete(ptr);
    }
};

template <typename T, typename U>
inline bool operator==(const simple_allocator<T>&, const simple_allocator<U>&)
{
    return true;
}

template <typename T, typename U>
inline bool operator!=(const simple_allocator<T>& a, const simple_allocator<U>& b)
{
    return !(a == b);
}

static void BM_customize_a_simple_allocator(benchmark::State& state)
{
    simple_allocator<int> allocator;
    for (auto _ : state)
    {
        int* p = allocator.allocate(10000U);
        allocator.deallocate(p, 10000U);
        benchmark::DoNotOptimize(p);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_customize_a_simple_allocator);

////////////////////////////////////////////////////////////////////

static void BM_c_normal_allocation(benchmark::State& state)
{
    for (auto _ : state)
    {
        int* p = (int*)malloc(sizeof(int) * 10000U);
        std::memset(p, 0, sizeof(int) * 10000U);
        free(p);
        benchmark::DoNotOptimize(p);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_c_normal_allocation);

////////////////////////////////////////////////////////////////////

static void BM_cpp_inplace_allocation(benchmark::State& state)
{
    void* buffer = malloc(65536U);
    if (buffer == nullptr)
    {
        state.SkipWithError("Memory allocation failed");
        return;
    }

    for (auto _ : state)
    {
        int* p = new (buffer) int[10000U];
        benchmark::DoNotOptimize(p);
        benchmark::ClobberMemory();
    }
    free(buffer);
}
BENCHMARK(BM_cpp_inplace_allocation);


BENCHMARK_MAIN();