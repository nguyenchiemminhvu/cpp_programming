#include <benchmark/benchmark.h>

#include <iostream>
#include <vector>

class Base
{
public:
    virtual ~Base() = default;
    virtual void do_work() = 0;
};

class DerivedA : public Base
{
public:
    void do_work() override
    {
        // Simulate some work
        for (volatile int i = 0; i < 1000; ++i);
    }
};

class DerivedB : public Base
{
public:
    void do_work() override
    {
        // Simulate some work
        for (volatile int i = 0; i < 2000; ++i);
    }
};

void do_work_virtual(Base* obj)
{
    obj->do_work();
}

template <typename T>
void do_work_template(T& obj)
{
    obj.do_work();
}

static void BM_virtual_prediction(benchmark::State& state)
{
    Base* obj = new DerivedB();
    for (auto _ : state)
    {
        do_work_virtual(obj);
        benchmark::ClobberMemory();
    }
}

static void BM_template_prediction(benchmark::State& state)
{
    DerivedB obj;
    for (auto _ : state)
    {
        do_work_template(obj);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(BM_virtual_prediction);
BENCHMARK(BM_template_prediction);

BENCHMARK_MAIN();