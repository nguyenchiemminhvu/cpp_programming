/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/dtc_monitor_base.h"

namespace gnss_diagnostic
{

dtc_monitor_base::dtc_monitor_base(dtc_config       cfg,
                                   i_dtc_service&   dtc_svc,
                                   const i_clock&   clk)
    : cfg_(cfg)
    , dtc_svc_(dtc_svc)
    , clk_(clk)
{
}

std::uint32_t dtc_monitor_base::dtc_code() const
{
    return cfg_.dtc_code;
}

bool dtc_monitor_base::is_active() const
{
    return dtc_active_;
}

void dtc_monitor_base::set_observer(i_dtc_observer* observer)
{
    observer_ = observer;
}

void dtc_monitor_base::pause()
{
    paused_ = true;
    fail_run_ = false;
    ok_run_   = false;
}

void dtc_monitor_base::resume(std::chrono::steady_clock::time_point /*now*/)
{
    paused_ = false;
    // Force a fresh debounce cycle on the next evaluate().
    fail_run_ = false;
    ok_run_   = false;
}

void dtc_monitor_base::reset()
{
    if (dtc_active_)
    {
        dtc_svc_.clear_dtc(cfg_.dtc_code);
        if (observer_)
        {
            observer_->on_dtc_cleared(cfg_.dtc_code);
        }
    }
    dtc_active_ = false;
    fail_run_   = false;
    ok_run_     = false;
}

void dtc_monitor_base::evaluate(std::chrono::steady_clock::time_point now)
{
    if (paused_)
    {
        return;
    }

    const bool failing = condition_currently_failing(now);

    if (failing)
    {
        ok_run_ = false;
        if (!fail_run_)
        {
            fail_run_     = true;
            fail_started_ = now;
        }

        if (!dtc_active_ && (now - fail_started_) >= cfg_.trigger_duration)
        {
            dtc_active_ = true;
            dtc_svc_.set_dtc(cfg_.dtc_code);
            if (observer_)
            {
                observer_->on_dtc_set(cfg_.dtc_code);
            }
        }
    }
    else
    {
        fail_run_ = false;
        if (!ok_run_)
        {
            ok_run_     = true;
            ok_started_ = now;
        }

        if (dtc_active_ && (now - ok_started_) >= cfg_.clear_duration)
        {
            dtc_active_ = false;
            dtc_svc_.clear_dtc(cfg_.dtc_code);
            if (observer_)
            {
                observer_->on_dtc_cleared(cfg_.dtc_code);
            }
        }
    }
}

} // namespace gnss_diagnostic
