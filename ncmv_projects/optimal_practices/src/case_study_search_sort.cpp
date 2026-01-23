#include <benchmark/benchmark.h>

#include <iostream>
#include <cstring>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <random>

static void BM_binary_search_tree_data_structure(benchmark::State& state)
{
    std::map<int, int> M;
    for (auto _ : state)
    {
        for (int i = 0; i < 10000; i++)
        {
            M.insert({i, i});
            if (M.find(i) != M.end())
            {
                benchmark::DoNotOptimize(M);
            }
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_binary_search_tree_data_structure);

////////////////////////////////////////////////////////////////

static void BM_hash_table_data_structure(benchmark::State& state)
{
    std::unordered_map<int, int> M;
    for (auto _ : state)
    {
        for (int i = 0; i < 10000; i++)
        {
            M.insert({i, i});
            if (M.find(i) != M.end())
            {
                benchmark::DoNotOptimize(M);
            }
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_hash_table_data_structure);

////////////////////////////////////////////////////////////////

static void BM_normal_sort(benchmark::State& state)
{
    std::vector<int> data(10000U);
    std::generate(data.begin(), data.end(), std::rand);
    for (auto _ : state)
    {
        std::sort(data.begin(), data.end());
        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_normal_sort);

////////////////////////////////////////////////////////////////

static void BM_stable_sort(benchmark::State& state)
{
    std::vector<int> data(10000U);
    std::generate(data.begin(), data.end(), std::rand);
    for (auto _ : state)
    {
        std::stable_sort(data.begin(), data.end());
        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_stable_sort);


BENCHMARK_MAIN();