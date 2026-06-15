/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/chip_reset_action.h"

namespace gnss_diagnostic
{

chip_reset_action::chip_reset_action(i_chip_controller& chip)
    : chip_(chip)
{
}

recovery_level chip_reset_action::level() const
{
    return recovery_level::level1_chip_reset;
}

bool chip_reset_action::execute(std::uint32_t /*triggering_dtc*/)
{
    return chip_.reset_chip();
}

} // namespace gnss_diagnostic
