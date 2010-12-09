/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 * @file timers.h
 *
 * @brief Timer prototypes and various definitions
 */

/* Note to self:
 * The timer clock frequencies are automatically fixed by hardware.
 * There are two cases:
 *     1.  if the APB prescaler is 1, the timer clock frequencies are
 *         set to the same frequency as that of the APB domain to which
 *         the timers are connected.
 *     2.  otherwise, they are set to twice (x2) the frequency of the
 *         APB domain to which the timers are connected.
 * See stm32 manual, 77/995
 *
 * hence, 72 mhz timers
 */

/* Maple Timer channels:
 * Timer        Maple Pin               STM32 Pin     Type
 * TIM1_CH1     D6                      PA8           Advanced
 * TIM1_CH2     D7                      PA9           Advanced
 * TIM1_CH3     D8                      PA10          Advanced
 *
 * TIM2_CH1     D2                      PA0
 * TIM2_CH2     D3                      PA1
 * TIM2_CH3     D1                      PA2
 * TIM2_CH4     D0                      PA3
 *
 * TIM3_CH1     D12                     PA6
 * TIM3_CH2     D11                     PA7
 * TIM3_CH3     EXT7                    PB0
 * TIM3_CH4     EXT8                    PB1
 *
 * TIM4_CH1     EXT5                    PB6
 * TIM4_CH1     EXT9                    PB7
 * TIM4_CH1     EXT15                   PB8
 * TIM4_CH1     EXT4                    PB9
 *
 * Not supported:
 * TIM1_CH4    USBDM, not available     PA11          Advanced
 * TIM1_CH1_N  EXT12                    PB13
 * TIM1_CH2_N  EXT13                    PB14
 * TIM1_CH3_N  EXT14                    PB15
 * */

/* I don't like the Arduino API for dealing with pin modes.
 * How about...
 *
 * pinMode(digitalPin, PWM);
 * pwmWrite(digitalPin) */

#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef volatile uint16* TimerCCR;

#define TIMER1_BASE        0x40012C00
#define TIMER2_BASE        0x40000000
#define TIMER3_BASE        0x40000400
#define TIMER4_BASE        0x40000800
#define TIMER5_BASE        0x40000C00   // High-density devices only
#define TIMER6_BASE        0x40001000   // High-density devices only
#define TIMER7_BASE        0x40001400   // High-density devices only
#define TIMER8_BASE        0x40013400   // High-density devices only

#define ARPE               BIT(7)       // Auto-reload preload enable
#define NOT_A_TIMER        0

#define TIMER_CCR(NUM,CHAN)  (TIMER ## NUM ## _CH ## CHAN ## _CRR)

/* Timers 1-4 are present on the entire STM32 line. */

#define TIMER1_CH1_CCR     ((TimerCCR)(TIMER1_BASE + 0x34))
#define TIMER1_CH2_CCR     ((TimerCCR)(TIMER1_BASE + 0x38))
#define TIMER1_CH3_CCR     ((TimerCCR)(TIMER1_BASE + 0x3C))
#define TIMER1_CH4_CCR     ((TimerCCR)(TIMER1_BASE + 0x40))

#define TIMER2_CH1_CCR     ((TimerCCR)(TIMER2_BASE + 0x34))
#define TIMER2_CH2_CCR     ((TimerCCR)(TIMER2_BASE + 0x38))
#define TIMER2_CH3_CCR     ((TimerCCR)(TIMER2_BASE + 0x3C))
#define TIMER2_CH4_CCR     ((TimerCCR)(TIMER2_BASE + 0x40))

#define TIMER3_CH1_CCR     ((TimerCCR)(TIMER3_BASE + 0x34))
#define TIMER3_CH2_CCR     ((TimerCCR)(TIMER3_BASE + 0x38))
#define TIMER3_CH3_CCR     ((TimerCCR)(TIMER3_BASE + 0x3C))
#define TIMER3_CH4_CCR     ((TimerCCR)(TIMER3_BASE + 0x40))

#define TIMER4_CH1_CCR     ((TimerCCR)(TIMER4_BASE + 0x34))
#define TIMER4_CH2_CCR     ((TimerCCR)(TIMER4_BASE + 0x38))
#define TIMER4_CH3_CCR     ((TimerCCR)(TIMER4_BASE + 0x3C))
#define TIMER4_CH4_CCR     ((TimerCCR)(TIMER4_BASE + 0x40))

/* Timers 5 and 8 are in high-density devices only (such as Maple
   Native).  Timers 6 and 7 in these devices have no output compare
   pins. */

#define TIMER5_CH1_CCR     ((TimerCCR)(TIMER5_BASE + 0x34))
#define TIMER5_CH2_CCR     ((TimerCCR)(TIMER5_BASE + 0x38))
#define TIMER5_CH3_CCR     ((TimerCCR)(TIMER5_BASE + 0x3C))
#define TIMER5_CH4_CCR     ((TimerCCR)(TIMER5_BASE + 0x40))

#define TIMER8_CH1_CCR     ((TimerCCR)(TIMER8_BASE + 0x34))
#define TIMER8_CH2_CCR     ((TimerCCR)(TIMER8_BASE + 0x38))
#define TIMER8_CH3_CCR     ((TimerCCR)(TIMER8_BASE + 0x3C))
#define TIMER8_CH4_CCR     ((TimerCCR)(TIMER8_BASE + 0x40))

/**
 * Used to configure the behavior of a timer.
 */
typedef enum TimerMode {
    TIMER_DISABLED, /**< In this mode, the timer stops counting,
                       interrupts are not called, and no state changes
                       are output. */
    TIMER_PWM, /**< This is the default mode for pins after
                  initialization. */
    TIMER_OUTPUTCOMPARE, /**< In this mode, the timer counts from 0 to
                            the overflow value repeatedly; every time
                            the counter value reaches one of the
                            channel compare values, the corresponding
                            interrupt is fired. */
} TimerMode;

typedef struct {
    /* Fields up to ARR common to general purpose (2,3,4,5), advanced
       control (1,8) and basic (6, 7) timers: */
    volatile uint16 CR1;
    uint16  RESERVED0;
    volatile uint16 CR2;
    uint16  RESERVED1;
    volatile uint16 SMCR;
    uint16  RESERVED2;
    volatile uint16 DIER;
    uint16  RESERVED3;
    volatile uint16 SR;
    uint16  RESERVED4;
    volatile uint16 EGR;
    uint16  RESERVED5;
    volatile uint16 CCMR1;
    uint16  RESERVED6;
    volatile uint16 CCMR2;
    uint16  RESERVED7;
    volatile uint16 CCER;
    uint16  RESERVED8;
    volatile uint16 CNT;
    uint16  RESERVED9;
    volatile uint16 PSC;
    uint16  RESERVED10;
    volatile uint16 ARR;
    uint16  RESERVED11;
    /* Basic timers have none of the following: */
    volatile uint16 RCR;        /* Advanced control timers only */
    uint16  RESERVED12;         /* Advanced control timers only */
    volatile uint16 CCR1;
    uint16  RESERVED13;
    volatile uint16 CCR2;
    uint16  RESERVED14;
    volatile uint16 CCR3;
    uint16  RESERVED15;
    volatile uint16 CCR4;
    uint16  RESERVED16;
    volatile uint16 BDTR;       /* Advanced control timers only */
    uint16  RESERVED17;         /* Advanced control timers only */
    volatile uint16 DCR;
    uint16  RESERVED18;
    volatile uint16 DMAR;
    uint16  RESERVED19;
} timer_port;

/**
 * Timer device numbers.  See STM32 reference manual, chapters 13-15.
 */
/* several locations depend on TIMER1=0, etc.; don't change the
   enumerator values to start at 1. */
typedef enum {
    TIMER1,      /*< Advanced control timer TIM1 */
    TIMER2,      /*< General purpose timer TIM2 */
    TIMER3,      /*< General purpose timer TIM3 */
    TIMER4,      /*< General purpose timer TIM4 */
#if NR_TIMERS >= 8
    TIMER5,      /*< General purpose timer TIM5; high density only */
    /* TIMER6,      /\*< Basic timer TIM6; high density only *\/ */
    /* TIMER7,      /\*< Basic timer TIM7; high density only *\/ */
    TIMER8,      /*< Advanced control timer TIM8; high density only */
#endif
    TIMER_INVALID /* FIXME: this is starting to seem like a bad idea */
} timer_dev_num;

/* timer descriptor */
struct timer_dev {
    timer_port *base;
    const uint8 rcc_dev_num;
    const uint8 nvic_dev_num;
    volatile voidFuncPtr handlers[4];
};

extern struct timer_dev timer_dev_table[];

/* Turn on timer with prescale as the divisor
 * void timer_init(uint32 timer, uint16 prescale)
 *      timer     ->  {1-4}
 *      prescale  ->  {1-65535}
 */
void timer_init(timer_dev_num, uint16);
void timer_disable_all(void);

uint16 timer_get_count(timer_dev_num);
void timer_set_count(timer_dev_num,uint16);

void timer_pause(timer_dev_num);
void timer_resume(timer_dev_num);

uint16 timer_get_prescaler(timer_dev_num timer_num);
void timer_set_prescaler(timer_dev_num timer_num, uint16 prescale);

uint16 timer_get_reload(timer_dev_num timer_num);
void timer_set_reload(timer_dev_num timer_num, uint16 max_reload);

/* TODO: timer_get_mode */
void timer_set_mode(timer_dev_num timer_num, uint8 channel_num, uint8 mode);

uint16 timer_get_compare_value(timer_dev_num timer_num, uint8 channel_num);
void timer_set_compare_value(timer_dev_num timer_num, uint8 channel_num, uint16 value);

void timer_attach_interrupt(timer_dev_num timer_num, uint8 channel_num,
                            voidFuncPtr handler);
void timer_detach_interrupt(timer_dev_num timer_num, uint8 channel_num);

/* generate UEV */
void timer_generate_update(timer_dev_num timer_num);

/* Turn on PWM with duty_cycle on the specified channel in timer.
 * This function takes in a pointer to the corresponding CCR
 * register for the pin cause it saves pwmWrite() a couple of
 * cycles.
 *
 * void timer_pwm(uint8 channel, uint8 duty_cycle);
 *      channel    -> {TIMERx_CHn_CCR}
 *      duty_cycle -> {0-65535}
 *
 * PRECONDITIONS:
 *      pin has been set to alternate function output
 *      timer has been initialized
 */
static inline void timer_pwm_write_ccr(TimerCCR CCR, uint16 duty_cycle) {
    *CCR = duty_cycle;
}

#ifdef __cplusplus
} // extern "C"
#endif


#endif

