/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_POWER_MODE_CONTROLLER_H
#define GNSS_DIAGNOSTIC_POWER_MODE_CONTROLLER_H

#pragma once

#include "gnss_event_types.h"

namespace gnss_diagnostic
{

class gnss_diagnostic_manager;

/*
 * Adapts external "system power mode" events into pause/resume calls on
 * the diagnostic manager. Implements a tiny two-state state machine
 * (active / low_power) so that duplicate notifications are no-ops.
 */
class power_mode_controller final
{
public:
    explicit power_mode_controller(gnss_diagnostic_manager& manager);

    void notify(power_mode mode);

    power_mode current() const { return current_; }

private:
    gnss_diagnostic_manager& manager_;
    power_mode               current_ = power_mode::active;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_POWER_MODE_CONTROLLER_H
