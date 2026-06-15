/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/power_mode_controller.h"

#include "gnss_diagnostic/gnss_diagnostic_manager.h"

namespace gnss_diagnostic
{

power_mode_controller::power_mode_controller(gnss_diagnostic_manager& manager)
    : manager_(manager)
{
}

void power_mode_controller::notify(power_mode mode)
{
    if (mode == current_)
    {
        return;
    }
    current_ = mode;

    if (mode == power_mode::low_power)
    {
        manager_.pause();
    }
    else
    {
        manager_.resume();
    }
}

} // namespace gnss_diagnostic
