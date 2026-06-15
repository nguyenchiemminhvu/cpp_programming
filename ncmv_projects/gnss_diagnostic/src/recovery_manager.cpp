/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/recovery_manager.h"

#include <utility>

namespace gnss_diagnostic
{

recovery_manager::recovery_manager(
    const i_clock&                                 clk,
    recovery_config                                cfg,
    std::vector<std::unique_ptr<i_recovery_action>> actions)
    : clk_(clk)
    , cfg_(cfg)
    , actions_(std::move(actions))
{
}

i_recovery_action* recovery_manager::find_action(recovery_level lvl) const
{
    for (const auto& a : actions_)
    {
        if (a && a->level() == lvl)
        {
            return a.get();
        }
    }
    return nullptr;
}

void recovery_manager::on_dtc_set(std::uint32_t code)
{
    if (state_ == state::terminal)
    {
        return;
    }

    active_dtcs_.insert(code);
    last_trigger_ = code;

    if (state_ == state::idle)
    {
        state_    = state::armed;
        armed_at_ = clk_.now();
        level_    = recovery_level::idle;
    }
}

void recovery_manager::on_dtc_cleared(std::uint32_t code)
{
    active_dtcs_.erase(code);

    if (state_ == state::terminal)
    {
        return;
    }

    if (active_dtcs_.empty())
    {
        // Healthy again -> reset escalation.
        state_ = state::idle;
        level_ = recovery_level::idle;
    }
}

void recovery_manager::pause()
{
    paused_ = true;
}

void recovery_manager::resume(std::chrono::steady_clock::time_point now)
{
    if (!paused_)
    {
        return;
    }
    paused_ = false;
    // Re-arm timers from "now" so that LPM duration does not count
    // towards escalation.
    if (state_ == state::armed)
    {
        armed_at_ = now;
    }
    else if (state_ == state::level1_done)
    {
        level1_at_ = now;
    }
}

void recovery_manager::evaluate(std::chrono::steady_clock::time_point now)
{
    if (paused_ || state_ == state::idle || state_ == state::terminal)
    {
        return;
    }

    if (state_ == state::armed)
    {
        if ((now - armed_at_) >= cfg_.level1_timeout)
        {
            if (auto* act = find_action(recovery_level::level1_chip_reset))
            {
                act->execute(last_trigger_);
            }
            state_     = state::level1_done;
            level1_at_ = now;
            level_     = recovery_level::level1_chip_reset;
        }
        return;
    }

    if (state_ == state::level1_done)
    {
        if ((now - level1_at_) >= cfg_.level2_timeout)
        {
            if (auto* act = find_action(recovery_level::level2_platform_reboot))
            {
                act->execute(last_trigger_);
            }
            state_ = state::terminal;
            level_ = recovery_level::terminal_no_more;
        }
        return;
    }
}

} // namespace gnss_diagnostic
