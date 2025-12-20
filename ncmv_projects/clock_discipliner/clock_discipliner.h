/**
MIT License

Copyright (c) 2025 nguyenchiemminhvu@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CLOCK_DISCIPLINER_H
#define CLOCK_DISCIPLINER_H

#pragma once

#include <time.h>
#include <sys/timex.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * ClockDiscipliner
 *
 * - Collects jittery 10Hz Clock timestamps
 * - Filters offset using EWMA
 * - Disciplines CLOCK_REALTIME at 1Hz
 *
 * This class never directly sets system time on every Clock message.
 * It behaves like a simplified NTP clock discipline algorithm.
 */

class clock_discipliner
{
public:
    clock_discipliner()
        : ewma_offset_ns(0),
          ewma_alpha(0.2),
          sample_count(0),
          last_discipline_sec(0)
    {}

    /*
     * Called when a GNSS message arrives.
     *
     * time_source_ms:
     *   Absolute GNSS time in milliseconds since epoch
     */
    void on_time_source_tick(uint64_t time_source_ms)
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        uint64_t system_time_ms = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
        int64_t offset_ms = (int64_t)time_source_ms - (int64_t)system_time_ms;
        int64_t offset_ns = offset_ms * 1000000LL;

        update_ewma(offset_ns);

        discipline_if_needed(ts.tv_sec);
    }

private:
    /* Exponentially weighted moving average of offset */
    int64_t ewma_offset_ns;
    const double ewma_alpha;

    int64_t sample_count;
    time_t last_discipline_sec;

    void update_ewma(int64_t offset_ns)
    {
        if (sample_count == 0)
        {
            ewma_offset_ns = offset_ns;
        }
        else
        {
            ewma_offset_ns = (int64_t)((1.0 - ewma_alpha) * ewma_offset_ns + ewma_alpha * offset_ns);
        }
        sample_count++;
    }

    /*
     * Discipline system clock at most once per second
     */
    void discipline_if_needed(time_t current_sec)
    {
        if (current_sec == last_discipline_sec)
        {
            return;
        }

        last_discipline_sec = current_sec;

        int64_t abs_offset_ns = ewma_offset_ns >= 0 ? ewma_offset_ns : -ewma_offset_ns;

        printf("[discipline] filtered offset = %.3f ms\n", ewma_offset_ns / 1e6);

        if (abs_offset_ns > 3LL * 1000000LL) // > 3 ms
        {
            step_clock();
        }
        else
        {
            slew_clock();
        }
    }

    /*
     * Hard step: only used for very large errors
     */
    void step_clock()
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        int64_t new_ns = ts.tv_sec * 1000000000LL + ts.tv_nsec + ewma_offset_ns;

        struct timespec new_ts;
        new_ts.tv_sec = new_ns / 1000000000LL;
        new_ts.tv_nsec = new_ns % 1000000000LL;

        int ret = clock_settime(CLOCK_REALTIME, &new_ts);
        if (ret < 0)
        {
            perror("[step] clock_settime failed");
        }
        else
        {
            printf("[step] clock stepped by %.3f ms\n", ewma_offset_ns / 1e6);
        }

        ewma_offset_ns = 0;
    }

    /*
     * Slew clock using clock_adjtime()
     */
    void slew_clock()
    {
        struct timex tx;
        memset(&tx, 0, sizeof(tx));

        /*
         * offset is in microseconds
         * kernel slews clock gradually
         */
        tx.modes = ADJ_OFFSET;
        tx.offset = ewma_offset_ns / 1000;

        int ret = clock_adjtime(CLOCK_REALTIME, &tx);
        if (ret < 0)
        {
            perror("[slew] clock_adjtime failed");
        }
        else
        {
            printf("[slew] clock slewed by %.3f ms\n", ewma_offset_ns / 1e6);
        }
    }
};

#endif // CLOCK_DISCIPLINER_H