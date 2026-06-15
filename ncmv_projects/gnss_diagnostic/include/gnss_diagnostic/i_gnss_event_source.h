/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_GNSS_EVENT_SOURCE_H
#define GNSS_DIAGNOSTIC_I_GNSS_EVENT_SOURCE_H

#pragma once

#include "gnss_event_types.h"

namespace gnss_diagnostic
{

/*
 * Observer interface implemented by every component that needs raw
 * events from the GNSS Hardware Adapter Layer.
 */
class i_gnss_event_listener
{
public:
    virtual ~i_gnss_event_listener() = default;

    virtual void on_pvt(const pvt_event& /*ev*/) {}
    virtual void on_antenna(const antenna_event& /*ev*/) {}
};

/*
 * Subject interface that the GNSS Hardware Adapter Layer must implement.
 * The diagnostic module only depends on this abstraction, never on the
 * concrete adapter, which keeps it chip-agnostic and unit-testable.
 */
class i_gnss_event_source
{
public:
    virtual ~i_gnss_event_source() = default;
    virtual void subscribe(i_gnss_event_listener* listener) = 0;
    virtual void unsubscribe(i_gnss_event_listener* listener) = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_GNSS_EVENT_SOURCE_H
