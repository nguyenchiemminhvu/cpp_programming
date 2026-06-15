/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_RECOVERY_ACTION_H
#define GNSS_DIAGNOSTIC_I_RECOVERY_ACTION_H

#pragma once

#include <cstdint>

namespace gnss_diagnostic
{

enum class recovery_level : std::uint8_t
{
    idle                  = 0,
    level1_chip_reset     = 1,
    level2_platform_reboot= 2,
    terminal_no_more      = 3,
};

/*
 * Strategy interface for a single recovery step.
 * Concrete examples:
 *   - chip_reset_action       (level 1)
 *   - platform_reboot_action  (level 2)
 *
 * New escalation steps can be added by implementing this interface
 * without modifying recovery_manager (Open/Closed).
 */
class i_recovery_action
{
public:
    virtual ~i_recovery_action() = default;

    virtual recovery_level level() const = 0;
    virtual bool execute(std::uint32_t triggering_dtc) = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_RECOVERY_ACTION_H
