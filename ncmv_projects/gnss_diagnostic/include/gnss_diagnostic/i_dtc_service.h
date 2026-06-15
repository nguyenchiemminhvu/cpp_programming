/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_DTC_SERVICE_H
#define GNSS_DIAGNOSTIC_I_DTC_SERVICE_H

#pragma once

#include <cstdint>

namespace gnss_diagnostic
{

/*
 * Adapter to the platform DiagService.
 *
 * The real DiagService lives in another microservice. The adapter
 * translates set_dtc / clear_dtc into the appropriate IPC call
 * (vsomeip, gRPC, AUTOSAR DEM, etc.) and is the only place where that
 * binding exists.
 */
class i_dtc_service
{
public:
    virtual ~i_dtc_service() = default;
    virtual void set_dtc(std::uint32_t code) = 0;
    virtual void clear_dtc(std::uint32_t code) = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_DTC_SERVICE_H
