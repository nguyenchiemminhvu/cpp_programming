/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_CLOCK_H
#define GNSS_DIAGNOSTIC_I_CLOCK_H

#pragma once

#include <chrono>

namespace gnss_diagnostic
{

/*
 * Abstract monotonic clock. Production code injects a steady_clock-backed
 * adapter; unit tests inject a fake clock to drive time deterministically.
 */
class i_clock
{
public:
    virtual ~i_clock() = default;
    virtual std::chrono::steady_clock::time_point now() const = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_CLOCK_H
