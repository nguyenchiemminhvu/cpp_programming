/**
 * @file        perf_timer.h
 * @brief       Timer to check the performance in nano, micro, mili seconds...
 * @author      nguyenchiemminhvu
 * @version     0.01.001 (A: major change, B: refactoring or structure change, C: minor change (ex) 1.12.133)
 * @usage
 * perf_timer* timer = get_perf_timer("func_name");
 * timer->start();
 * // do something here
 * timer->stop();
 */

#include "timer.h"
#include <iostream>

perf_timer::perf_timer()
{
    m_time_table.resize(measurement_type::MEASUREMENT_TYPE_TOTAL);
    for (std::size_t i = 0U; i < m_time_table.size(); i++)
    {
        m_time_table[i].resize(PERF_TIMER_TABLE_MAX_SIZE);
    }
}

perf_timer::~perf_timer()
{

}

void perf_timer::record(std::size_t pt_id)
{
    std::deque<clock_t>& row = m_time_table[pt_id];
    row[0] = row[1];
    row[1] = std::chrono::steady_clock::now();

    measure(pt_id);
}

void perf_timer::start(std::size_t pt_id)
{
    std::deque<clock_t>& row = m_time_table[pt_id];
    row[0] = std::chrono::steady_clock::now();
}

void perf_timer::stop(std::size_t pt_id)
{
    std::deque<clock_t>& row = m_time_table[pt_id];
    row[1] = std::chrono::steady_clock::now();

    measure(pt_id);
}

perf_timer * perf_timer::get_instance()
{
    static perf_timer s_instance;
    return &s_instance;
}

void perf_timer::measure(std::size_t pt_id)
{
    std::deque<clock_t>& row = m_time_table[pt_id];
    duration_t d = row[1] - row[0];
    double dMs = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(d).count()) * 1e-6;

    std::cout << MEASUREMENT_TYPE_TO_STRING(pt_id) << ": " << dMs << " (milliseconds)" << std::endl;
}
