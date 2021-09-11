/*

MIT License

Copyright (c) 2019-2021 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

This file is part of hardware agnostic I2C driver for AXP173:
https://github.com/tuupola/axp173

SPDX-License-Identifier: MIT
Version: 0.1.0-dev

*/

#include <stdarg.h>
#include <stdint.h>

#include "axp173_config.h"
#include "axp173.h"

static axp173_err_t read_coloumb_counter(const axp173_t *axp, float *buffer);
static axp173_err_t read_battery_power(const axp173_t *axp, float *buffer);

static const axp173_init_command_t init_commands[] = {
#ifdef AXP173_INCLUDE_SDKCONFIG_H
    /* Currently you have to use menuconfig to be able to use axp173_init() */
    {AXP173_DCDC1_VOLTAGE, {CONFIG_AXP173_DCDC1_VOLTAGE}, 1},
    {AXP173_LDO4_VOLTAGE, {CONFIG_AXP173_LDO4_VOLTAGE}, 1},
    {AXP173_LDO23_VOLTAGE, {CONFIG_AXP173_LDO23_VOLTAGE}, 1},
    {AXP173_DCDC13_LDO23_CONTROL, {CONFIG_AXP173_DCDC13_LDO23_CONTROL}, 1},
    {AXP173_EXTEN_DCDC2_CONTROL, {CONFIG_AXP173_EXTEN_DCDC2_CONTROL}, 1},
    {AXP173_ADC_ENABLE_1, {CONFIG_AXP173_ADC_ENABLE_1}, 1},
    {AXP173_CHARGE_CONTROL_1, {CONFIG_AXP173_CHARGE_CONTROL_1}, 1},
    {AXP173_BATTERY_CHARGE_CONTROL, {CONFIG_AXP173_BATTERY_CHARGE_CONTROL}, 1},
#endif /* AXP173_INCLUDE_SDKCONFIG_H */
    /* End of commands. */
    {0, {0}, 0xff},
};

axp173_err_t axp173_init(const axp173_t *axp)
{
    uint8_t cmd = 0;
    axp173_err_t status;

    /* Send all the commands. */
    while (init_commands[cmd].count != 0xff) {
        status = axp->write(
            axp->handle,
            AXP173_ADDRESS,
            init_commands[cmd].command,
            init_commands[cmd].data,
            init_commands[cmd].count & 0x1f
        );
        if (AXP173_OK != status) {
            return status;
        }
        cmd++;
    }

    return AXP173_OK;
}

static axp173_err_t axp173_read_adc(const axp173_t *axp, uint8_t reg, float *buffer)
{
    uint8_t tmp[4];
    float sensitivity = 1.0;
    float offset = 0.0;
    axp173_err_t status;

    switch (reg) {
    case AXP173_ACIN_VOLTAGE:
    case AXP173_VBUS_VOLTAGE:
        /* 1.7mV per LSB */
        sensitivity = 1.7 / 1000;
        break;
    case AXP173_ACIN_CURRENT:
        /* 0.375mA per LSB */
        sensitivity = 0.625 / 1000;
        break;
    case AXP173_VBUS_CURRENT:
        /* 0.375mA per LSB */
        sensitivity = 0.375 / 1000;
        break;
    case AXP173_TEMP:
        /* 0.1C per LSB, 0x00 = -144.7C */
        sensitivity = 0.1;
        offset = -144.7;
        break;
    case AXP173_TS_INPUT:
        /* 0.8mV per LSB */
        sensitivity = 0.8 / 1000;
        break;
    case AXP173_BATTERY_POWER:
        /* 1.1mV * 0.5mA per LSB */
        return read_battery_power(axp, buffer);
        break;
    case AXP173_BATTERY_VOLTAGE:
        /* 1.1mV per LSB */
        sensitivity = 1.1 / 1000;
        break;
    case AXP173_CHARGE_CURRENT:
    case AXP173_DISCHARGE_CURRENT:
        /* 0.5mV per LSB */
        sensitivity = 0.5 / 1000;
        break;
    case AXP173_APS_VOLTAGE:
        /* 1.4mV per LSB */
        sensitivity = 1.4 / 1000;
        break;
    case AXP173_COULOMB_COUNTER:
        /* This is currently untested. */
        return read_coloumb_counter(axp, buffer);
        break;
    }

    status = axp->read(axp->handle, AXP173_ADDRESS, reg, tmp, 2);
    if (AXP173_OK != status) {
        return status;
    }
    *buffer = (((tmp[0] << 4) + tmp[1]) * sensitivity) + offset;

    return AXP173_OK;
}

axp173_err_t axp173_read(const axp173_t *axp, uint8_t reg, void *buffer) {
    switch (reg) {
    case AXP173_ACIN_VOLTAGE:
    case AXP173_VBUS_VOLTAGE:
    case AXP173_ACIN_CURRENT:
    case AXP173_VBUS_CURRENT:
    case AXP173_TEMP:
    case AXP173_TS_INPUT:
    case AXP173_BATTERY_POWER:
    case AXP173_BATTERY_VOLTAGE:
    case AXP173_CHARGE_CURRENT:
    case AXP173_DISCHARGE_CURRENT:
    case AXP173_APS_VOLTAGE:
    case AXP173_COULOMB_COUNTER:
        /* Return ADC value. */
        return axp173_read_adc(axp, reg, buffer);
        break;
    default:
        /* Return raw register value. */
        return axp->read(axp->handle, AXP173_ADDRESS, reg, buffer, 1);
    }
}

axp173_err_t axp173_write(const axp173_t *axp, uint8_t reg, const uint8_t *buffer) {
    switch (reg) {
    case AXP173_ACIN_VOLTAGE:
    case AXP173_VBUS_VOLTAGE:
    case AXP173_ACIN_CURRENT:
    case AXP173_VBUS_CURRENT:
    case AXP173_TEMP:
    case AXP173_TS_INPUT:
    case AXP173_BATTERY_POWER:
    case AXP173_BATTERY_VOLTAGE:
    case AXP173_CHARGE_CURRENT:
    case AXP173_DISCHARGE_CURRENT:
    case AXP173_APS_VOLTAGE:
    case AXP173_COULOMB_COUNTER:
        /* Read only register. */
        return AXP173_ERROR_ENOTSUP;
        break;
    default:
        /* Write raw register value. */
        return axp->write(axp->handle, AXP173_ADDRESS, reg, buffer, 1);
    }
}

axp173_err_t axp173_ioctl(const axp173_t *axp, int command, ...)
{
    uint8_t reg = command >> 8;
    uint8_t tmp;
    uint16_t argument;
    va_list ap;

    switch (command) {
    case AXP173_COULOMB_COUNTER_ENABLE:
        tmp = 0b10000000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_COULOMB_COUNTER_DISABLE:
        tmp = 0b00000000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_COULOMB_COUNTER_SUSPEND:
        tmp = 0b11000000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_COULOMB_COUNTER_CLEAR:
        tmp = 0b10100000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_LDOIO0_ENABLE:
        /* 0x02 = LDO */
        tmp = 0b00000010;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_LDOIO0_DISABLE:
        /* 0x07 = float */
        tmp = 0b00000111;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO2_ENABLE:
    /* This is currently untested. */
    case AXP173_EXTEN_ENABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp |= 0b00000100;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO2_DISABLE:
    /* This is currently untested. */
    case AXP173_EXTEN_DISABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp &= ~0b00000100;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO3_ENABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp |= 0b00001000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO3_DISABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp &= ~0b00001000;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_DCDC1_ENABLE:
    /* This is currently untested. */
    case AXP173_DCDC2_ENABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp |= 0b00000001;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_DCDC1_DISABLE:
    /* This is currently untested. */
    case AXP173_DCDC2_DISABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp &= ~0b00000001;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO4_ENABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp |= 0b00000010;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_LDO4_DISABLE:
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        tmp &= ~0b00000010;
        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    case AXP173_DCDC1_SET_VOLTAGE:
    case AXP173_LDO4_SET_VOLTAGE:
        va_start(ap, command);
        argument = (uint16_t) va_arg(ap, int);
        va_end(ap);

        /*  700-3500mv 25mV per step */
        if ((argument < 700) || (argument > 3500)) {
            return AXP173_ERROR_EINVAL;
        }
        tmp = (argument - 700) / 25;

        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_DCDC2_SET_VOLTAGE:
        va_start(ap, command);
        argument = (uint16_t) va_arg(ap, int);
        va_end(ap);

        /*  700-2275mV 25mV per step */
        if ((argument < 700) || (argument > 2275)) {
            return AXP173_ERROR_EINVAL;
        }
        tmp = (argument - 700) / 25;

        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_LDO2_SET_VOLTAGE:
        va_start(ap, command);
        argument = (uint16_t) va_arg(ap, int);
        va_end(ap);

        /*  1800-3300mV 100mV per step */
        if ((argument < 1800) || (argument > 3300)) {
            return AXP173_ERROR_EINVAL;
        }
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);

        tmp &= ~0xf0;
        tmp |= (((argument - 1800) / 100) << 4);

        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_LDO3_SET_VOLTAGE:
        va_start(ap, command);
        argument = (uint16_t) va_arg(ap, int);
        va_end(ap);

        /*  1800-3300mV 100mV per step */
        if ((argument < 1800) || (argument > 3300)) {
            return AXP173_ERROR_EINVAL;
        }
        axp->read(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);

        tmp &= ~0x0f;
        tmp |= ((argument - 1800) / 100);

        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    /* This is currently untested. */
    case AXP173_LDOIO0_SET_VOLTAGE:
        va_start(ap, command);
        argument = (uint16_t) va_arg(ap, int);
        va_end(ap);

        /*  1800-3300mV 100mV per step, 2800mV default. */
        if ((argument < 1800) || (argument > 3300)) {
            return AXP173_ERROR_EINVAL;
        }
        tmp = (((argument - 1800) / 100) << 4);

        return axp->write(axp->handle, AXP173_ADDRESS, reg, &tmp, 1);
        break;
    }

    return AXP173_ERROR_NOTTY;
}

static axp173_err_t read_coloumb_counter(const axp173_t *axp, float *buffer)
{
    uint8_t tmp[4];
    int32_t coin, coout;
    axp173_err_t status;

    status = axp->read(axp->handle, AXP173_ADDRESS, AXP173_CHARGE_COULOMB, tmp, sizeof(coin));
    if (AXP173_OK != status) {
        return status;
    }
    coin = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];

    status = axp->read(axp->handle, AXP173_ADDRESS, AXP173_DISCHARGE_COULOMB, tmp, sizeof(coout));
    if (AXP173_OK != status) {
        return status;
    }
    coout = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];

    /* CmAh = 65536 * 0.5mA *（coin - cout) / 3600 / ADC sample rate */
    *buffer = 32768 * (coin - coout) / 3600 / 25;

    return AXP173_OK;
}

static axp173_err_t read_battery_power(const axp173_t *axp, float *buffer)
{
    uint8_t tmp[4];
    float sensitivity;
    axp173_err_t status;

    /* 1.1mV * 0.5mA per LSB */
    sensitivity = 1.1 * 0.5 / 1000;
    status = axp->read(axp->handle, AXP173_ADDRESS, AXP173_BATTERY_POWER, tmp, 3);
    if (AXP173_OK != status) {
        return status;
    }
    *buffer = (((tmp[0] << 16) + (tmp[1] << 8) + tmp[2]) * sensitivity);
    return AXP173_OK;
}