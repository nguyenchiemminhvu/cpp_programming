/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/gnss_diagnostic_manager.h"

#include <utility>

namespace gnss_diagnostic
{

gnss_diagnostic_manager::gnss_diagnostic_manager(
    i_gnss_event_source&              events,
    const i_clock&                    clk,
    std::unique_ptr<recovery_manager> recovery)
    : events_(events)
    , clk_(clk)
    , recovery_(std::move(recovery))
{
}

gnss_diagnostic_manager::~gnss_diagnostic_manager()
{
    stop();
}

void gnss_diagnostic_manager::register_monitor(std::unique_ptr<i_dtc_monitor> monitor)
{
    if (!monitor)
    {
        return;
    }
    if (recovery_)
    {
        monitor->set_observer(recovery_.get());
    }
    monitors_.push_back(std::move(monitor));
}

void gnss_diagnostic_manager::start()
{
    if (started_)
    {
        return;
    }
    events_.subscribe(this);
    started_ = true;
}

void gnss_diagnostic_manager::stop()
{
    if (!started_)
    {
        return;
    }
    events_.unsubscribe(this);
    started_ = false;
}

void gnss_diagnostic_manager::pause()
{
    if (paused_)
    {
        return;
    }
    paused_ = true;
    for (auto& m : monitors_)
    {
        m->pause();
    }
    if (recovery_)
    {
        recovery_->pause();
    }
}

void gnss_diagnostic_manager::resume()
{
    if (!paused_)
    {
        return;
    }
    paused_ = false;
    const auto now = clk_.now();
    for (auto& m : monitors_)
    {
        m->resume(now);
    }
    if (recovery_)
    {
        recovery_->resume(now);
    }
}

void gnss_diagnostic_manager::on_tick(std::chrono::steady_clock::time_point now)
{
    if (paused_)
    {
        return;
    }
    for (auto& m : monitors_)
    {
        m->evaluate(now);
    }
    if (recovery_)
    {
        recovery_->evaluate(now);
    }
}

void gnss_diagnostic_manager::on_pvt(const pvt_event& ev)
{
    if (paused_)
    {
        return;
    }
    for (auto& m : monitors_)
    {
        m->on_pvt(ev);
    }
}

void gnss_diagnostic_manager::on_antenna(const antenna_event& ev)
{
    if (paused_)
    {
        return;
    }
    for (auto& m : monitors_)
    {
        m->on_antenna(ev);
    }
}

} // namespace gnss_diagnostic
