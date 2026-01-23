#include <benchmark/benchmark.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

static void BM_read_whole_file_assign_streambuf_to_string(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::ifstream f("files/sample_file.txt", std::ios::in | std::ios::out);
        if (f.is_open())
        {
            std::string s;
            s.assign(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>());
            f.close();
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_read_whole_file_assign_streambuf_to_string);

///////////////////////////////////////////////////////////////////////

static void BM_read_whole_file_via_stringstream(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::ifstream f("files/sample_file.txt", std::ios::in | std::ios::out);
        if (f.is_open())
        {
            std::stringstream ss;
            std::copy(std::istreambuf_iterator<char>(f.rdbuf()), std::istreambuf_iterator<char>(), std::ostreambuf_iterator(ss));
            std::string res = ss.str();
            f.close();
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_read_whole_file_via_stringstream);

///////////////////////////////////////////////////////////////////////

static void BM_read_line_by_line_to_preallocate_string(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::ifstream f("files/sample_file.txt", std::ios::in | std::ios::out);
        if (f.is_open())
        {
            std::string s;
            f.seekg(0, std::ios::end);
            s.reserve(f.tellg());
            f.seekg(0, std::ios::beg);
            std::string line;
            while (std::getline(f, line))
            {
                s += line;
                s += '\n';
            }
            f.close();
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_read_line_by_line_to_preallocate_string);

///////////////////////////////////////////////////////////////////////

static void BM_read_whole_file_to_preallocate_string(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::ifstream f("files/sample_file.txt", std::ios::in | std::ios::out);
        if (f.is_open())
        {
            std::string s;
            f.seekg(0, std::ios::end);
            s.reserve(f.tellg());
            f.seekg(0, std::ios::beg);
            f.read(&s[0], s.length());
            f.close();
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_read_whole_file_to_preallocate_string);

///////////////////////////////////////////////////////////////////////

static void BM_read_whole_binary_file_to_preallocate_string(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::ifstream f("files/sample_file.txt", std::ios::in | std::ios::out | std::ios::binary);
        if (f.is_open())
        {
            std::string s;
            f.seekg(0, std::ios::end);
            s.reserve(f.tellg());
            f.seekg(0, std::ios::beg);
            f.read(&s[0], s.length());
            f.close();
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_read_whole_binary_file_to_preallocate_string);


BENCHMARK_MAIN();