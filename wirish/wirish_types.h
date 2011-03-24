/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file   wirish_types.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Wirish library type definitions.
 */

#include "gpio.h"
#include "timer.h"

#ifndef _WIRISH_TYPES_H_
#define _WIRISH_TYPES_H_

/** Stores STM32-specific information related to a given pin. */
typedef struct stm32_pin_info {
    gpio_dev *gpio_device;   /**< Maple pin's GPIO device */
    timer_dev *timer_device; /**< Maple pin's timer device, or NULL if none. */
    uint8 gpio_pin;          /**< GPIO pin */
    uint8 timer_chan;        /**< Timer channel, or 0 if none. */
    uint8 adc_channel;       /**< Pin ADC channel, or ADCx if none. */
} stm32_pin_info;

/** Invalid adc_channel value */
#define ADCx 0xFF

#endif
