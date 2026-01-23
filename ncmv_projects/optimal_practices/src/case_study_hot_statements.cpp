#include <benchmark/benchmark.h>

#include <iostream>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <numeric>

static void BM_uncache_loop_end_value(benchmark::State& state)
{
    char* input = new char[100000U];
    std::memset(input, ' ', 100000U);
    for (auto _ : state)
    {
        int count_whitespace = 0;
        for (std::size_t i = 0U; i < strlen(input); i++)
        {
            if (input[i] == ' ')
            {
                count_whitespace++;
            }
        }
        benchmark::DoNotOptimize(count_whitespace);
        benchmark::ClobberMemory();
    }
    delete[] input;
}
BENCHMARK(BM_uncache_loop_end_value);

//////////////////////////////////////////////////////////////////

static void BM_cache_loop_end_value(benchmark::State& state)
{
    char* input = new char[100000U];
    std::memset(input, ' ', 100000U);
    std::size_t len = strlen(input);
    for (auto _ : state)
    {
        int count_whitespace = 0;
        for (std::size_t i = 0U; i < len; i++)
        {
            if (input[i] == ' ')
            {
                count_whitespace++;
            }
        }
        benchmark::DoNotOptimize(count_whitespace);
        benchmark::ClobberMemory();
    }
    delete[] input;
}
BENCHMARK(BM_cache_loop_end_value);

//////////////////////////////////////////////////////////////////

static void BM_allocate_inside_loop(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<int> data;
        for (std::size_t i = 0U; i < 10000U; i++)
        {
            data.push_back(i);
        }
        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_allocate_inside_loop);

//////////////////////////////////////////////////////////////////

static void BM_pre_allocate(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<int> data;
        data.reserve(10000U);
        for (std::size_t i = 0U; i < 10000U; i++)
        {
            data.push_back(i);
        }
        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_pre_allocate);

//////////////////////////////////////////////////////////////////

static void BM_old_cpp_feature_string(benchmark::State& state)
{
    std::string s(10000U, ' ');
    for (auto _ : state)
    {
        std::string temp = s;
        benchmark::DoNotOptimize(temp);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_old_cpp_feature_string);

//////////////////////////////////////////////////////////////////

static void BM_modern_cpp_feature_string(benchmark::State& state)
{
    std::string s(10000U, ' ');
    for (auto _ : state)
    {
        std::string_view temp = s;
        benchmark::DoNotOptimize(temp);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_modern_cpp_feature_string);

//////////////////////////////////////////////////////////////////

static void BM_multi_branching(benchmark::State& state)
{
    std::string s(10000U, ' ');
    for (auto _ : state)
    {
        int count = 0;
        for (std::size_t i = 0U; i < s.length(); i++)
        {
            if (s[i] == ' ')
            {
                count++;
            }
        }
        benchmark::DoNotOptimize(count);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_multi_branching);

//////////////////////////////////////////////////////////////////

static void BM_reduce_branching(benchmark::State& state)
{
    std::string s(10000U, ' ');
    for (auto _ : state)
    {
        int count = 0;
        for (char c : s)
        {
            count += (c == ' ');
        }
        benchmark::DoNotOptimize(count);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_reduce_branching);

//////////////////////////////////////////////////////////////////

static void BM_normal_copying(benchmark::State& state)
{
    std::vector<std::string> input(1000, "this is a string");
    for (auto _ : state)
    {
        std::vector<std::string> res = input;
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_normal_copying);

//////////////////////////////////////////////////////////////////

static void BM_move_semantics(benchmark::State& state)
{
    std::vector<std::string> input(1000, "this is a string");
    for (auto _ : state)
    {
        std::vector<std::string> res = std::move(input);
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_move_semantics);


BENCHMARK_MAIN();