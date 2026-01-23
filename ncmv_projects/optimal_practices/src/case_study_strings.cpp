#include <benchmark/benchmark.h>

#include <iostream>
#include <string>

std::string trim_whitespace_naive(std::string s)
{
    std::string res;
    for (std::size_t i = 0U; i < s.length(); i++)
    {
        if (s[i] != ' ')
        {
            res = res + s[i];
        }
    }

    return res;
}

static void BM_trim_whitespace_naive(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        std::string res = trim_whitespace_naive(input);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_naive);

/////////////////////////////////////////////////////////////////////////////////

std::string trim_whitespace_eliminate_copying_args(const std::string& s)
{
    std::string res;
    for (std::size_t i = 0U; i < s.length(); i++)
    {
        if (s[i] != ' ')
        {
            res = res + s[i];
        }
    }

    return res;
}

static void BM_trim_whitespace_eliminate_copying_args(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        std::string res = trim_whitespace_eliminate_copying_args(input);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_eliminate_copying_args);

/////////////////////////////////////////////////////////////////////////////////

std::string trim_whitespace_eliminate_temporaries(const std::string& s)
{
    std::string res;
    for (std::size_t i = 0U; i < s.length(); i++)
    {
        if (s[i] != ' ')
        {
            res += s[i];
        }
    }

    return res;
}

static void BM_trim_whitespace_eliminate_temporaries(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        std::string res = trim_whitespace_eliminate_temporaries(input);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_eliminate_temporaries);

/////////////////////////////////////////////////////////////////////////////////

void trim_whitespace_eliminate_copying_returned(const std::string& s, std::string& res)
{
    res.clear();
    res.reserve(s.length());
    for (std::size_t i = 0U; i < s.length(); i++)
    {
        if (s[i] != ' ')
        {
            res += s[i];
        }
    }
}

static void BM_trim_whitespace_eliminate_copying_returned(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        std::string res;
        trim_whitespace_eliminate_copying_returned(input, res);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_eliminate_copying_returned);

/////////////////////////////////////////////////////////////////////////////////

std::string trim_whitespace_different_approach(std::string s)
{
    for (std::size_t i = 0U; i < s.length(); )
    {
        if (s[i] == ' ')
        {
            s.erase(i, 1);
        }
        else
        {
            i++;
        }
    }

    return s;
}

static void BM_trim_whitespace_different_approach(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        std::string res = trim_whitespace_different_approach(input);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_different_approach);

/////////////////////////////////////////////////////////////////////////////////

void trim_whitespace_c_strings(const char* src, char* dest, std::size_t len)
{
    for (std::size_t i = 0U; i < len; i++)
    {
        if (src[i] != ' ')
        {
            *dest = src[i];
            dest++;
        }
    }
    *dest = '\0';
}

static void BM_trim_whitespace_c_strings(benchmark::State& state)
{
    std::string input = "this is a very long string using for the case study strings";
    for (auto _ : state)
    {
        char* res = new char[input.length() + 1];
        trim_whitespace_c_strings(input.c_str(), res, input.length());
        delete[] res;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_trim_whitespace_c_strings);


BENCHMARK_MAIN();