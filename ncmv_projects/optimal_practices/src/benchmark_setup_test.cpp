#include <benchmark/benchmark.h>

#include <iostream>

static void BM_test(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_test);

BENCHMARK_MAIN();