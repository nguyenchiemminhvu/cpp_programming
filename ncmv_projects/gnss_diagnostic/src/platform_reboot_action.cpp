/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/platform_reboot_action.h"

#include <string>

namespace gnss_diagnostic
{

platform_reboot_action::platform_reboot_action(i_health_service& health)
    : health_(health)
{
}

recovery_level platform_reboot_action::level() const
{
    return recovery_level::level2_platform_reboot;
}

bool platform_reboot_action::execute(std::uint32_t triggering_dtc)
{
    health_.request_platform_reboot(
        "gnss_diagnostic: persistent DTC 0x" + std::to_string(triggering_dtc));
    return true;
}

} // namespace gnss_diagnostic
