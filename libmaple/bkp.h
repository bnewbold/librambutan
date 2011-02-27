/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs, LLC.
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
 * @file bkp.h
 * @brief Backup register support.
 */

#ifndef _BKP_H_
#define _BKP_H_

#include "libmaple.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bkp_reg_map {
    const uint32 RESERVED1;
    __io uint32 DR1;            ///< Data register 1
    __io uint32 DR2;            ///< Data register 2
    __io uint32 DR3;            ///< Data register 3
    __io uint32 DR4;            ///< Data register 4
    __io uint32 DR5;            ///< Data register 5
    __io uint32 DR6;            ///< Data register 6
    __io uint32 DR7;            ///< Data register 7
    __io uint32 DR8;            ///< Data register 8
    __io uint32 DR9;            ///< Data register 9
    __io uint32 DR10;           ///< Data register 10
    __io uint32 RTCCR;          ///< RTC control register
    __io uint32 CR;             ///< Control register
    __io uint32 CSR;            ///< Control and status register
#if NR_BKP_REGS > 10
    const uint32 RESERVED2;
    const uint32 RESERVED3;
    __io uint32 DR11;           ///< Data register 11
    __io uint32 DR12;           ///< Data register 12
    __io uint32 DR13;           ///< Data register 13
    __io uint32 DR14;           ///< Data register 14
    __io uint32 DR15;           ///< Data register 15
    __io uint32 DR16;           ///< Data register 16
    __io uint32 DR17;           ///< Data register 17
    __io uint32 DR18;           ///< Data register 18
    __io uint32 DR19;           ///< Data register 19
    __io uint32 DR20;           ///< Data register 20
    __io uint32 DR21;           ///< Data register 21
    __io uint32 DR22;           ///< Data register 22
    __io uint32 DR23;           ///< Data register 23
    __io uint32 DR24;           ///< Data register 24
    __io uint32 DR25;           ///< Data register 25
    __io uint32 DR26;           ///< Data register 26
    __io uint32 DR27;           ///< Data register 27
    __io uint32 DR28;           ///< Data register 28
    __io uint32 DR29;           ///< Data register 29
    __io uint32 DR30;           ///< Data register 30
    __io uint32 DR31;           ///< Data register 31
    __io uint32 DR32;           ///< Data register 32
    __io uint32 DR33;           ///< Data register 33
    __io uint32 DR34;           ///< Data register 34
    __io uint32 DR35;           ///< Data register 35
    __io uint32 DR36;           ///< Data register 36
    __io uint32 DR37;           ///< Data register 37
    __io uint32 DR38;           ///< Data register 38
    __io uint32 DR39;           ///< Data register 39
    __io uint32 DR40;           ///< Data register 40
    __io uint32 DR41;           ///< Data register 41
    __io uint32 DR42;           ///< Data register 42
#endif
} bkp_reg_map;

typedef struct bkp_dev {
    bkp_reg_map *regs;
} bkp_dev;

/**
 * Backup device.
 */
extern const bkp_dev *BKP;

/*
 * Backup peripheral base.
 */
#define BKP_BASE        ((bkp_reg_map*)0x40006C00)

void bkp_init(void);
void bkp_enable_writes(void);
void bkp_disable_writes(void);
uint16 bkp_read(uint8 reg);
void bkp_write(uint8 reg, uint16 val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
