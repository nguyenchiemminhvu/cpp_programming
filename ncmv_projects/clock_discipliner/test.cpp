#include "clock_discipliner.h"

#include <chrono>
#include <thread>
#include <random>
#include <iostream>
#include <vector>

int main()
{
    clock_discipliner discipliner;
    std::vector<int> jitter_ms = {0, 0, 0, 0, 15, -15, 20, -20, 10, -10 };

    uint64_t time_source_ms;
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        time_source_ms = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
    }

    printf("Starting GNSS clock discipline test...\n");

    for (int i = 0; i < 300; ++i)
    {
        int jitter = jitter_ms[i % jitter_ms.size()];

        /*
         * Simulate GNSS arrival jitter:
         * sleep 100ms +/- jitter
         */
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 + jitter));

        /*
         * GNSS timestamp itself is clean and increases exactly 100ms
         */
        time_source_ms += 100;

        printf("[source] tick %3d | jitter = %+3d ms | time_source = %llu ms\n",
               i, jitter,
               (unsigned long long)time_source_ms);

        discipliner.on_time_source_tick(time_source_ms);
    }

    printf("Test completed.\n");
    return 0;
}
