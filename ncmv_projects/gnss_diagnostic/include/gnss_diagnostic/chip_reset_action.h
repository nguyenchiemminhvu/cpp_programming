/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_CHIP_RESET_ACTION_H
#define GNSS_DIAGNOSTIC_CHIP_RESET_ACTION_H

#pragma once

#include "i_chip_controller.h"
#include "i_recovery_action.h"

namespace gnss_diagnostic
{

class chip_reset_action final : public i_recovery_action
{
public:
    explicit chip_reset_action(i_chip_controller& chip);

    recovery_level level() const override;
    bool           execute(std::uint32_t triggering_dtc) override;

private:
    i_chip_controller& chip_;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_CHIP_RESET_ACTION_H
