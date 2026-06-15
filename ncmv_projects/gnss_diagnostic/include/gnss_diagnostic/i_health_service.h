/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_HEALTH_SERVICE_H
#define GNSS_DIAGNOSTIC_I_HEALTH_SERVICE_H

#pragma once

#include <string>

namespace gnss_diagnostic
{

/*
 * Adapter to the platform HealthService used for graceful platform reboot.
 */
class i_health_service
{
public:
    virtual ~i_health_service() = default;
    virtual void request_platform_reboot(const std::string& reason) = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_HEALTH_SERVICE_H
