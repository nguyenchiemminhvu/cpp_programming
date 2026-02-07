/**
 * @file        perf_timer.h
 * @brief       Timer to check the performance in nano, micro, mili seconds...
 * @author      nguyenchiemminhvu
 * @version     0.01.001 (A: major change, B: refactoring or structure change, C: minor change (ex) 1.12.133)
 * @usage
 * perf_timer* timer = get_perf_timer();
 * timer->start(measurement_type::CPP_OPTIMIZATION_MEASUREMENT);
 * // do something here
 * timer->stop();
 */

#ifndef PERF_TIMER_H
#define PERF_TIMER_H

#include <chrono>
#include <string>
#include <vector>
#include <deque>

#define PERF_TIMER_TABLE_MAX_SIZE (2U)

class perf_timer
{
public:
    using clock = std::chrono::steady_clock;
    using clock_t = std::chrono::time_point<std::chrono::steady_clock>;
    using duration_t = std::chrono::time_point<std::chrono::steady_clock>::duration;

    enum measurement_type : std::size_t
    {
        CPP_OPTIMIZATION_MEASUREMENT,

        MEASUREMENT_TYPE_TOTAL
    };

// macro to map MeasurementType enum values to their corresponding names
#define MEASUREMENT_TYPE_TO_STRING(type) \
    (((type) == CPP_OPTIMIZATION_MEASUREMENT) ? "CPP_OPTIMIZATION_MEASUREMENT" : "")

    void record(std::size_t pt_id);
    void start(std::size_t pt_id);
    void stop(std::size_t pt_id);

    static perf_timer * get_instance();

private:
    std::vector<std::deque<clock_t>> m_time_table;

private:

    perf_timer();
    ~perf_timer();

    void measure(std::size_t pt_id);
};

#define get_perf_timer() perf_timer::get_instance()

#endif // PERF_TIMER_H