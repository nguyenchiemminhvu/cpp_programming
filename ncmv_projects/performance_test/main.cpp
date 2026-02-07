#include "performancetest.h"
#include <iostream>
#include <vector>
#include <algorithm>

void foo()
{
    std::vector<int> v(10000, 0);
    for (size_t i = 0; i < v.size(); i++)
    {
        for (size_t j = i + 1; j < v.size(); j++)
        {
            if (v[i] > v[j])
            {
                std::swap(v[i], v[j]);
            }
        }
    }
}

void bar()
{
    std::vector<int> v(10000, 0);
    std::sort(v.begin(), v.end());
}

int main()
{
    performance_test::run(1, foo, "foo");
    performance_test::run(1, bar, "bar");

    return 0;
}
