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
Version: 0.6.0-dev

*/

#ifndef _AXP173_H
#define _AXP173_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define	AXP173_ADDRESS	                (0x34)

/* Power control registers */
#define AXP173_POWER_STATUS             (0x00)
#define AXP173_CHARGE_STATUS            (0x01)
#define AXP173_OTG_VBUS_STATUS          (0x04)
#define AXP173_DATA_BUFFER0             (0x06)
#define AXP173_DATA_BUFFER1             (0x07)
#define AXP173_DATA_BUFFER2             (0x08)
#define AXP173_DATA_BUFFER3             (0x09)
#define AXP173_DATA_BUFFER4             (0x0a)
#define AXP173_DATA_BUFFER5             (0x0b)
/* Output control: 2 EXTEN, 0 DCDC2 */
#define AXP173_EXTEN_DCDC2_CONTROL      (0x10)
/* Power output control: 6 EXTEN, 4 DCDC2, 3 LDO3, 2 LDO2, 1 DCDC3, 0 DCDC1 */
#define AXP173_DCDC13_LDO23_CONTROL     (0x12)
#define AXP173_DCDC2_VOLTAGE            (0x23)
#define AXP173_DCDC2_SLOPE              (0x25)
#define AXP173_DCDC1_VOLTAGE            (0x26)
#define AXP173_DCDC3_VOLTAGE            (0x27)
/* Output voltage control: 7-4 LDO2, 3-0 LDO3 */
#define AXP173_LDO23_VOLTAGE            (0x28)
#define AXP173_VBUS_IPSOUT_CHANNEL      (0x30)
#define AXP173_SHUTDOWN_VOLTAGE         (0x31)
#define AXP173_SHUTDOWN_BATTERY_CHGLED_CONTROL (0x32)
#define AXP173_CHARGE_CONTROL_1         (0x33)
#define AXP173_CHARGE_CONTROL_2         (0x34)
#define AXP173_BATTERY_CHARGE_CONTROL   (0x35)
#define AXP173_PEK                      (0x36)
#define AXP173_DCDC_FREQUENCY           (0x37)
#define AXP173_BATTERY_CHARGE_LOW_TEMP  (0x38)
#define AXP173_BATTERY_CHARGE_HIGH_TEMP (0x39)
#define AXP173_APS_LOW_POWER1           (0x3A)
#define AXP173_APS_LOW_POWER2           (0x3B)
#define AXP173_BATTERY_DISCHARGE_LOW_TEMP  (0x3c)
#define AXP173_BATTERY_DISCHARGE_HIGH_TEMP (0x3d)
#define AXP173_DCDC_MODE                (0x80)
#define AXP173_ADC_ENABLE_1             (0x82)
#define AXP173_ADC_ENABLE_2             (0x83)
#define AXP173_ADC_RATE_TS_PIN          (0x84)
#define AXP173_GPIO30_INPUT_RANGE       (0x85)
#define AXP173_GPIO0_ADC_IRQ_RISING     (0x86)
#define AXP173_GPIO0_ADC_IRQ_FALLING    (0x87)
#define AXP173_TIMER_CONTROL            (0x8a)
#define AXP173_VBUS_MONITOR             (0x8b)
#define AXP173_TEMP_SHUTDOWN_CONTROL    (0x8f)

/* GPIO control registers */
#define AXP173_GPIO0_CONTROL            (0x90)
#define AXP173_GPIO0_LDOIO0_VOLTAGE     (0x91)
#define AXP173_GPIO1_CONTROL            (0x92)
#define AXP173_GPIO2_CONTROL            (0x93)
#define AXP173_GPIO20_SIGNAL_STATUS     (0x94)
#define AXP173_GPIO43_FUNCTION_CONTROL  (0x95)
#define AXP173_GPIO43_SIGNAL_STATUS     (0x96)
#define AXP173_GPIO20_PULLDOWN_CONTROL  (0x97)
#define AXP173_PWM1_FREQUENCY           (0x98)
#define AXP173_PWM1_DUTY_CYCLE_1        (0x99)
#define AXP173_PWM1_DUTY_CYCLE_2        (0x9a)
#define AXP173_PWM2_FREQUENCY           (0x9b)
#define AXP173_PWM2_DUTY_CYCLE_1        (0x9c)
#define AXP173_PWM2_DUTY_CYCLE_2        (0x9d)
#define AXP173_N_RSTO_GPIO5_CONTROL     (0x9e)

/* Interrupt control registers */
#define AXP173_ENABLE_CONTROL_1         (0x40)
#define AXP173_ENABLE_CONTROL_2         (0x41)
#define AXP173_ENABLE_CONTROL_3         (0x42)
#define AXP173_ENABLE_CONTROL_4         (0x43)
#define AXP173_ENABLE_CONTROL_5         (0x4a)
#define AXP173_IRQ_STATUS_1             (0x44)
#define AXP173_IRQ_STATUS_2             (0x45)
#define AXP173_IRQ_STATUS_3             (0x46)
#define AXP173_IRQ_STATUS_4             (0x47)
#define AXP173_IRQ_STATUS_5             (0x4d)

/* ADC data registers */
#define AXP173_ACIN_VOLTAGE             (0x56)
#define AXP173_ACIN_CURRENT             (0x58)
#define AXP173_VBUS_VOLTAGE             (0x5a)
#define AXP173_VBUS_CURRENT             (0x5c)
#define AXP173_TEMP                     (0x5e)
#define AXP173_TS_INPUT                 (0x62)
#define AXP173_GPIO0_VOLTAGE            (0x64)
#define AXP173_GPIO1_VOLTAGE            (0x66)
#define AXP173_GPIO2_VOLTAGE            (0x68)
#define AXP173_GPIO3_VOLTAGE            (0x6a)
#define AXP173_BATTERY_POWER            (0x70)
#define AXP173_BATTERY_VOLTAGE          (0x78)
#define AXP173_CHARGE_CURRENT           (0x7a)
#define AXP173_DISCHARGE_CURRENT        (0x7c)
#define AXP173_APS_VOLTAGE              (0x7e)
#define AXP173_CHARGE_COULOMB           (0xb0)
#define AXP173_DISCHARGE_COULOMB        (0xb4)
#define AXP173_COULOMB_COUNTER_CONTROL  (0xb8)

/* Computed ADC */
#define AXP173_COULOMB_COUNTER          (0xff)

/* IOCTL commands */
#define	AXP173_READ_POWER_STATUS        (0x0001)
#define	AXP173_READ_CHARGE_STATUS       (0x0101)

#define AXP173_COULOMB_COUNTER_ENABLE   (0xb801)
#define AXP173_COULOMB_COUNTER_DISABLE  (0xb802)
#define AXP173_COULOMB_COUNTER_SUSPEND  (0xb803)
#define AXP173_COULOMB_COUNTER_CLEAR    (0xb804)

#define AXP173_LDOIO0_ENABLE            (0x9000)
#define AXP173_LDOIO0_DISABLE           (0x9001)

#define AXP173_DCDC2_ENABLE             (0x1000)
#define AXP173_DCDC2_DISABLE            (0x1001)
#define AXP173_EXTEN_ENABLE             (0x1002)
#define AXP173_EXTEN_DISABLE            (0x1003)

#define AXP173_LDO2_ENABLE              (0x1200)
#define AXP173_LDO2_DISABLE             (0x1201)
#define AXP173_LDO3_ENABLE              (0x1202)
#define AXP173_LDO3_DISABLE             (0x1203)
#define AXP173_DCDC1_ENABLE             (0x1204)
#define AXP173_DCDC1_DISABLE            (0x1205)
#define AXP173_DCDC3_ENABLE             (0x1206)
#define AXP173_DCDC3_DISABLE            (0x1207)

#define AXP173_DCDC1_SET_VOLTAGE        (0x2600)
#define AXP173_DCDC2_SET_VOLTAGE        (0x2300)
#define AXP173_DCDC3_SET_VOLTAGE        (0x2700)
#define AXP173_LDO2_SET_VOLTAGE         (0x2800)
#define AXP173_LDO3_SET_VOLTAGE         (0x2801)
#define AXP173_LDOIO0_SET_VOLTAGE       (0x9100)

#define AXP173_LOW                      (0)
#define AXP173_HIGH                     (1)

#define AXP173_GPIO0_SET_LEVEL          (0x9400)
#define AXP173_GPIO1_SET_LEVEL          (0x9401)
#define AXP173_GPIO2_SET_LEVEL          (0x9402)
#define AXP173_GPIO4_SET_LEVEL          (0x9601)

/* Error codes */
#define AXP173_OK                       (0)
#define AXP173_ERROR_NOTTY              (-1)
#define AXP173_ERROR_EINVAL             (-22)
#define AXP173_ERROR_ENOTSUP            (-95)

typedef struct {
    uint8_t command;
    uint8_t data[2];
    uint8_t count;
} axp173_init_command_t;

/* These should be provided by the HAL. */
typedef struct {
    int32_t (* read)(void *handle, uint8_t address, uint8_t reg, uint8_t *buffer, uint16_t size);
    int32_t (* write)(void *handle, uint8_t address, uint8_t reg, const uint8_t *buffer, uint16_t size);
    void *handle;
} axp173_t;

typedef int32_t axp173_err_t;

axp173_err_t axp173_init(const axp173_t *axp);
axp173_err_t axp173_read(const axp173_t *axp, uint8_t reg, void *buffer);
axp173_err_t axp173_write(const axp173_t *axp, uint8_t reg, const uint8_t *buffer);
axp173_err_t axp173_ioctl(const axp173_t *axp, int command, ...);

#ifdef __cplusplus
}
#endif
#endif
