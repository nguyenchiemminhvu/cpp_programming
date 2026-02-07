#include "performancetest.h"
#include "timer.h"

#include <iostream>

void performance_test::run(int n_time, callback_function func, const char* func_name)
{
    if (func != nullptr)
    {
        if (func_name != nullptr)
        {
            std::cout << "Run " << func_name << "....... ";
        }

        get_perf_timer()->start(perf_timer::measurement_type::CPP_OPTIMIZATION_MEASUREMENT);
        for (int i = 0; i < n_time; i++)
        {
            func();
        }
        get_perf_timer()->stop(perf_timer::measurement_type::CPP_OPTIMIZATION_MEASUREMENT);
    }
}
