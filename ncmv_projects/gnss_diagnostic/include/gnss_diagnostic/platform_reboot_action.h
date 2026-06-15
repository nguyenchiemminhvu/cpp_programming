/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_PLATFORM_REBOOT_ACTION_H
#define GNSS_DIAGNOSTIC_PLATFORM_REBOOT_ACTION_H

#pragma once

#include "i_health_service.h"
#include "i_recovery_action.h"

namespace gnss_diagnostic
{

class platform_reboot_action final : public i_recovery_action
{
public:
    explicit platform_reboot_action(i_health_service& health);

    recovery_level level() const override;
    bool           execute(std::uint32_t triggering_dtc) override;

private:
    i_health_service& health_;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_PLATFORM_REBOOT_ACTION_H
