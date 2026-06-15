/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_DTC_MONITOR_H
#define GNSS_DIAGNOSTIC_I_DTC_MONITOR_H

#pragma once

#include <chrono>
#include <cstdint>

#include "i_gnss_event_source.h"

namespace gnss_diagnostic
{

/*
 * Observer of DTC state transitions. Implemented by the recovery_manager
 * so that escalation timing starts when a DTC actually transitions to set,
 * not on the very first instant the underlying condition becomes unhealthy.
 */
class i_dtc_observer
{
public:
    virtual ~i_dtc_observer() = default;
    virtual void on_dtc_set(std::uint32_t code) = 0;
    virtual void on_dtc_cleared(std::uint32_t code) = 0;
};

/*
 * Common abstraction for every monitor in the diagnostic module.
 * Every monitor:
 *   - listens to GNSS events (inherited from i_gnss_event_listener)
 *   - is periodically evaluated by the manager via evaluate(now)
 *   - can be paused/resumed by the power mode controller
 */
class i_dtc_monitor : public i_gnss_event_listener
{
public:
    ~i_dtc_monitor() override = default;

    virtual std::uint32_t dtc_code() const = 0;
    virtual bool is_active() const = 0;

    virtual void evaluate(std::chrono::steady_clock::time_point now) = 0;

    virtual void pause() = 0;
    virtual void resume(std::chrono::steady_clock::time_point now) = 0;
    virtual void reset() = 0;

    virtual void set_observer(i_dtc_observer* observer) = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_DTC_MONITOR_H
