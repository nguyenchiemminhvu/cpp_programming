/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_I_CHIP_CONTROLLER_H
#define GNSS_DIAGNOSTIC_I_CHIP_CONTROLLER_H

#pragma once

namespace gnss_diagnostic
{

/*
 * Chip-agnostic GNSS controller interface.
 *
 * Concrete implementations live in the GNSS Hardware Adapter Layer:
 *   - ublox_chip_controller   -> sends UBX-CFG-RST
 *   - quectel_chip_controller -> AT+QGPSEND/AT+QGPS=1
 *   - ...
 *
 * The diagnostic module never knows which chip is in use.
 */
class i_chip_controller
{
public:
    virtual ~i_chip_controller() = default;

    /*
     * Performs a hardware/firmware reset of the GNSS chip.
     * Returns true if the reset command was successfully dispatched
     * (not necessarily that the chip has finished re-initializing).
     */
    virtual bool reset_chip() = 0;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_I_CHIP_CONTROLLER_H
