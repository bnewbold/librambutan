.. highlight:: c
.. _libmaple-rcc:

``<libmaple/rcc.h>``
====================

Reset and Clock Control (RCC) support.

The RCC is responsible for managing the MCU's various clocks. This
includes the core clock SYSCLK, which determines the CPU clock
frequency, as well as the clock lines that drive peripherals.

Because of this, the available RCC functionality varies by target.
There are a :ref:`variety of abstractions <libmaple-rcc-core-types>`
in place to make managing this more convenient.

.. contents:: Contents
   :local:

.. _libmaple-rcc-core-types:

Core Types
----------

The core abstractions in place are
:ref:`rcc_clk_id <libmaple-rcc-rcc_clk_id>`,
:ref:`rcc_clk <libmaple-rcc-rcc_clk>`,
:ref:`rcc_sysclk_src <libmaple-rcc-rcc_sysclk_src>`,
:ref:`rcc_clk_domain <libmaple-rcc-rcc_clk_domain>`, and
:ref:`rcc_prescaler <libmaple-rcc-rcc_prescaler>`.

.. _libmaple-rcc-rcc_clk_id:

Peripheral Identifiers: ``rcc_clk_id``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``rcc_clk_id`` is an enum used to identify peripherals. The RCC
back-ends use them to look up a peripheral's bus and clock line, but
they are also generally useful as unique identifiers for each
peripheral.  You can manage peripherals using their ``rcc_clk_id``\ s
with :ref:`these functions <libmaple-rcc-clk-id-funcs>`.

Peripherals which are common across targets have the same token
(though not necessarily the same value) for their ``rcc_clk_id``
across different targets. For example, the ``rcc_clk_id`` for the ADC1
peripheral is always ``RCC_ADC1`` regardless of the target.
Additionally, as explained in :ref:`libmaple-overview-devices`, each
peripheral device type struct contains the ``rcc_clk_id`` for that
peripheral in a ``clk_id`` field.

The available ``rcc_clk_id``\ s on each supported target series are as
follows.

STM32F1 Targets
+++++++++++++++

.. doxygenenum:: stm32f1::rcc_clk_id

STM32F2 Targets
+++++++++++++++

.. doxygenenum:: stm32f2::rcc_clk_id

.. _libmaple-rcc-rcc_clk:

System and Secondary Clock Sources: ``rcc_clk``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``rcc_clk`` type gives available system and secondary clock
sources (e.g. HSI, HSE, LSE). As with :ref:`rcc_clk_id
<libmaple-rcc-rcc_clk_id>`, clock sources which are common across
targets have the same token, but not necessarily the same value, for
their ``rcc_clk`` on each target.  A variety of :ref:`clock management
functions <libmaple-rcc-clk-funcs>` are available.

Note that the inclusion of secondary clock sources, like LSI and LSE,
makes ``rcc_clk`` different from the SYSCLK sources, which are managed
using :ref:`rcc_sysclk_src <libmaple-rcc-rcc_sysclk_src>`.

The available ``rcc_clk``\ s for each supported target series are as
follows.

STM32F1 Targets
+++++++++++++++

.. doxygenenum:: stm32f1::rcc_clk

STM32F2 Targets
+++++++++++++++

.. doxygenenum:: stm32f2::rcc_clk

.. _libmaple-rcc-rcc_sysclk_src:

SYSCLK Sources: ``rcc_sysclk_src``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This gives the possible SYSCLK sources. It's portable. Switch SYSCLK
with :ref:`rcc_switch_sysclk() <libmaple-rcc-rcc_switch_sysclk>`.

.. doxygenenum:: rcc_sysclk_src

As a special case, you can configure the PLL with a call to
:ref:`rcc_configure_pll() <libmaple-rcc-rcc_configure_pll>`.

To set SYSCLK to PLL driven by an external oscillator, you can use
something like this, which is portable except for the initialization
of ``your_pll_cfg``::

    /* You need to make this point to something valid for your target; see
     * the documentation for rcc_configure_pll() for more details. */
    extern rcc_pll_cfg *your_pll_cfg;

    void pll_reconfigure() {
        /* Turn on HSI using rcc_turn_on_clk() and wait for it to
         * become ready by busy-waiting on rcc_is_clk_ready().
         *
         * Switch to HSI to ensure we're not using the PLL while we
         * reconfigure it. */
        rcc_turn_on_clk(RCC_CLK_HSI);
        while (!rcc_is_clk_ready(RCC_CLK_HSI))
            ;
        rcc_switch_sysclk(RCC_CLKSRC_HSI);

        /* Turn off HSE and the PLL, or we can't reconfigure it. */
        rcc_turn_off_clk(RCC_CLK_PLL);
        rcc_turn_off_clk(RCC_CLK_HSE);

        /* Reconfigure the PLL. You can also perform any other
         * prescaler management here. */
        rcc_configure_pll(your_pll_cfg);

        /* Turn on RCC_CLK_HSE. */
        rcc_turn_on_clk(RCC_CLK_HSE);
        while (!rcc_is_clk_ready(RCC_CLK_HSE))
            ;

        /* Turn on RCC_CLK_PLL. */
        rcc_turn_on_clk(RCC_CLK_PLL);
        while (!rcc_is_clk_ready(RCC_CLK_PLL))
            ;

        /* Finally, switch to the PLL. */
        rcc_switch_sysclk(RCC_CLKSRC_PLL);
    }

.. _libmaple-rcc-rcc_clk_domain:

Clock Domains: ``rcc_clk_domain``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These specify the available clock domains.  For example, each AHB and
APB is a clock domain.

This type mostly exists to enable asking devices what bus they're on,
which, given knowledge of your system's clock configuration, can be
useful when making decisions about prescalers, etc.

Given an :ref:`rcc_clk_id <libmaple-rcc-rcc_clk_id>`, you can get the
peripheral's clock domain with :ref:`rcc_dev_clk()
<libmaple-rcc-rcc_dev_clk>`.  Clock domains that are common across
series have the same token (but not necessarily the same value) for
their corresponding ``rcc_clk_domain``.

The available ``rcc_clk_domain``\ s for each supported target series
are as follows.

STM32F1 Targets
+++++++++++++++

.. doxygenenum:: stm32f1::rcc_clk_domain

STM32F2 Targets
+++++++++++++++

.. doxygenenum:: stm32f2::rcc_clk_domain

.. _libmaple-rcc-rcc_prescaler:

Prescalers: ``rcc_prescaler`` and friends
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Available prescalers are managed via the ``rcc_prescaler`` type, the
``rcc_set_prescaler()`` function, and a variety of related prescaler
divider types.  See :ref:`libmaple-rcc-prescalers` for more
information and usage notes.

Functions
---------

.. _libmaple-rcc-sysclk-funcs:
.. _libmaple-rcc-rcc_switch_sysclk:

SYSCLK Management
~~~~~~~~~~~~~~~~~

Change the SYSCLK source with ``rcc_switch_sysclk()``.

.. doxygenfunction:: rcc_switch_sysclk

.. _libmaple-rcc-rcc_configure_pll:

Configuring the PLL
~~~~~~~~~~~~~~~~~~~

You can configure the PLL with ``rcc_configure_pll()``.  This takes an
``rcc_pll_cfg`` struct as its argument.  Though the definition of
``rcc_pll_cfg`` is common across series, its contents are entirely
target-dependent.

.. doxygenstruct:: rcc_pll_cfg
.. doxygenfunction:: rcc_configure_pll

The fields in an ``rcc_pll_cfg`` on each target are as follows.

rcc_pll_cfg on STM32F1 Targets
++++++++++++++++++++++++++++++

The ``pllsrc`` field is chosen from the following.

.. doxygenenum:: stm32f1::rcc_pllsrc

.. FIXME [0.0.13] We've got plans to redo this; make sure you watch
.. libmaple for changes here.

The ``data`` field must point to a ``struct stm32f1_rcc_pll_data``.
This just contains an ``rcc_pll_multiplier``.

.. doxygenenum:: stm32f1::rcc_pll_multiplier

.. doxygenstruct:: stm32f1::stm32f1_rcc_pll_data

rcc_pll_cfg on STM32F2 Targets
++++++++++++++++++++++++++++++

The ``pllsrc`` field is chosen from the following.

.. doxygenenum:: stm32f2::rcc_pllsrc

The ``data`` field must point to a ``struct stm32f2_rcc_pll_data``.

.. doxygenstruct:: stm32f2::stm32f2_rcc_pll_data

.. _libmaple-rcc-clk-funcs:

System and Secondary Clock Sources
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions are useful for managing clocks via their :ref:`rcc_clk
<libmaple-rcc-rcc_clk>`.

.. doxygenfunction:: rcc_turn_on_clk
.. doxygenfunction:: rcc_turn_off_clk
.. doxygenfunction:: rcc_is_clk_on
.. doxygenfunction:: rcc_is_clk_ready

.. _libmaple-rcc-clk-id-funcs:

Peripheral Management
~~~~~~~~~~~~~~~~~~~~~

These functions are useful for managing peripherals via their
:ref:`rcc_clk_id <libmaple-rcc-rcc_clk_id>`.

.. _libmaple-rcc-rcc_clk_enable:
.. doxygenfunction:: rcc_clk_enable
.. doxygenfunction:: rcc_reset_dev
.. _libmaple-rcc-rcc_dev_clk:
.. doxygenfunction:: rcc_dev_clk

.. _libmaple-rcc-prescalers:

Prescalers
~~~~~~~~~~

All RCC prescalers are managed with a single function,
``rcc_set_prescaler()``.

.. doxygenfunction:: rcc_set_prescaler

The arguments to ``rcc_set_prescaler()`` are target-dependent, but
follow a common pattern.

The first argument specifies the prescaler to set.  Each target has a
corresponding ``rcc_prescaler`` type.  Prescalers which are common
across targets have the same token, though not necessarily the same
value, for their ``rcc_prescaler`` (for example,
``RCC_PRESCALER_APB1`` is available on both STM32F1 and STM32F2
targets).

The second argument determines the divider; it is chosen based on the
prescaler chosen in the first argument. Along with each prescaler,
there is a separate enum type listing all of the available clock
dividers corresponding to that prescaler. For example, on STM32F1, the
corresponding dividers for ``RCC_PRESCALER_APB1`` are given by the
``rcc_apb1_divider`` type. It has values like ``RCC_APB1_HCLK_DIV_1``,
``RCC_APB1_HCLK_DIV_2``, etc.  In general, the dividers for
``RCC_PRESCALER_FOO`` are given by the type ``rcc_foo_divider``.

The available prescalers and dividers on each supported target series
are as follows.

STM32F1 Targets
+++++++++++++++

.. doxygenenum:: stm32f1::rcc_prescaler
.. doxygenenum:: stm32f1::rcc_adc_divider
.. doxygenenum:: stm32f1::rcc_apb1_divider
.. doxygenenum:: stm32f1::rcc_apb2_divider
.. doxygenenum:: stm32f1::rcc_ahb_divider

STM32F2 Targets
+++++++++++++++

.. doxygenenum:: stm32f2::rcc_prescaler
.. doxygenenum:: stm32f2::rcc_mco2_divider
.. doxygenenum:: stm32f2::rcc_mco1_divider
.. doxygenenum:: stm32f2::rcc_rtc_divider
.. doxygenenum:: stm32f2::rcc_apb2_divider
.. doxygenenum:: stm32f2::rcc_apb1_divider
.. doxygenenum:: stm32f2::rcc_ahb_divider

Register Maps
-------------

These vary by target. The base pointer is always ``RCC_BASE``.

.. doxygendefine:: RCC_BASE

STM32F1 Targets
~~~~~~~~~~~~~~~

.. doxygenstruct:: stm32f1::rcc_reg_map

STM32F2 Targets
~~~~~~~~~~~~~~~

.. doxygenstruct:: stm32f2::rcc_reg_map

Register Bit Definitions
------------------------

These are given as source code.  Available register bit definitions
vary by target.

STM32F1 Targets
~~~~~~~~~~~~~~~

Clock control register
++++++++++++++++++++++

::

    #define RCC_CR_PLLRDY_BIT               25
    #define RCC_CR_PLLON_BIT                24
    #define RCC_CR_CSSON_BIT                19
    #define RCC_CR_HSEBYP_BIT               18
    #define RCC_CR_HSERDY_BIT               17
    #define RCC_CR_HSEON_BIT                16
    #define RCC_CR_HSIRDY_BIT               1
    #define RCC_CR_HSION_BIT                0

    #define RCC_CR_PLLRDY                   (1U << RCC_CR_PLLRDY_BIT)
    #define RCC_CR_PLLON                    (1U << RCC_CR_PLLON_BIT)
    #define RCC_CR_CSSON                    (1U << RCC_CR_CSSON_BIT)
    #define RCC_CR_HSEBYP                   (1U << RCC_CR_HSEBYP_BIT)
    #define RCC_CR_HSERDY                   (1U << RCC_CR_HSERDY_BIT)
    #define RCC_CR_HSEON                    (1U << RCC_CR_HSEON_BIT)
    #define RCC_CR_HSICAL                   (0xFF << 8)
    #define RCC_CR_HSITRIM                  (0x1F << 3)
    #define RCC_CR_HSIRDY                   (1U << RCC_CR_HSIRDY_BIT)
    #define RCC_CR_HSION                    (1U << RCC_CR_HSION_BIT)

Clock configuration register
++++++++++++++++++++++++++++

::

    #define RCC_CFGR_USBPRE_BIT             22
    #define RCC_CFGR_PLLXTPRE_BIT           17
    #define RCC_CFGR_PLLSRC_BIT             16

    #define RCC_CFGR_MCO                    (0x3 << 24)
    #define RCC_CFGR_USBPRE                 (1U << RCC_CFGR_USBPRE_BIT)
    #define RCC_CFGR_PLLMUL                 (0xF << 18)
    #define RCC_CFGR_PLLXTPRE               (1U << RCC_CFGR_PLLXTPRE_BIT)
    #define RCC_CFGR_PLLSRC                 (1U << RCC_CFGR_PLLSRC_BIT)
    #define RCC_CFGR_ADCPRE                 (0x3 << 14)
    #define RCC_CFGR_PPRE2                  (0x7 << 11)
    #define RCC_CFGR_PPRE1                  (0x7 << 8)
    #define RCC_CFGR_HPRE                   (0xF << 4)
    #define RCC_CFGR_SWS                    (0x3 << 2)
    #define RCC_CFGR_SWS_PLL                (0x2 << 2)
    #define RCC_CFGR_SWS_HSE                (0x1 << 2)
    #define RCC_CFGR_SW                     0x3
    #define RCC_CFGR_SW_PLL                 0x2
    #define RCC_CFGR_SW_HSE                 0x1

Clock interrupt register
++++++++++++++++++++++++

::

    #define RCC_CIR_CSSC_BIT                23
    #define RCC_CIR_PLLRDYC_BIT             20
    #define RCC_CIR_HSERDYC_BIT             19
    #define RCC_CIR_HSIRDYC_BIT             18
    #define RCC_CIR_LSERDYC_BIT             17
    #define RCC_CIR_LSIRDYC_BIT             16
    #define RCC_CIR_PLLRDYIE_BIT            12
    #define RCC_CIR_HSERDYIE_BIT            11
    #define RCC_CIR_HSIRDYIE_BIT            10
    #define RCC_CIR_LSERDYIE_BIT            9
    #define RCC_CIR_LSIRDYIE_BIT            8
    #define RCC_CIR_CSSF_BIT                7
    #define RCC_CIR_PLLRDYF_BIT             4
    #define RCC_CIR_HSERDYF_BIT             3
    #define RCC_CIR_HSIRDYF_BIT             2
    #define RCC_CIR_LSERDYF_BIT             1
    #define RCC_CIR_LSIRDYF_BIT             0

    #define RCC_CIR_CSSC                    (1U << RCC_CIR_CSSC_BIT)
    #define RCC_CIR_PLLRDYC                 (1U << RCC_CIR_PLLRDYC_BIT)
    #define RCC_CIR_HSERDYC                 (1U << RCC_CIR_HSERDYC_BIT)
    #define RCC_CIR_HSIRDYC                 (1U << RCC_CIR_HSIRDYC_BIT)
    #define RCC_CIR_LSERDYC                 (1U << RCC_CIR_LSERDYC_BIT)
    #define RCC_CIR_LSIRDYC                 (1U << RCC_CIR_LSIRDYC_BIT)
    #define RCC_CIR_PLLRDYIE                (1U << RCC_CIR_PLLRDYIE_BIT)
    #define RCC_CIR_HSERDYIE                (1U << RCC_CIR_HSERDYIE_BIT)
    #define RCC_CIR_HSIRDYIE                (1U << RCC_CIR_HSIRDYIE_BIT)
    #define RCC_CIR_LSERDYIE                (1U << RCC_CIR_LSERDYIE_BIT)
    #define RCC_CIR_LSIRDYIE                (1U << RCC_CIR_LSIRDYIE_BIT)
    #define RCC_CIR_CSSF                    (1U << RCC_CIR_CSSF_BIT)
    #define RCC_CIR_PLLRDYF                 (1U << RCC_CIR_PLLRDYF_BIT)
    #define RCC_CIR_HSERDYF                 (1U << RCC_CIR_HSERDYF_BIT)
    #define RCC_CIR_HSIRDYF                 (1U << RCC_CIR_HSIRDYF_BIT)
    #define RCC_CIR_LSERDYF                 (1U << RCC_CIR_LSERDYF_BIT)
    #define RCC_CIR_LSIRDYF                 (1U << RCC_CIR_LSIRDYF_BIT)

Peripheral reset registers
++++++++++++++++++++++++++

::

    #define RCC_APB2RSTR_TIM11RST_BIT       21
    #define RCC_APB2RSTR_TIM10RST_BIT       20
    #define RCC_APB2RSTR_TIM9RST_BIT        19
    #define RCC_APB2RSTR_ADC3RST_BIT        15
    #define RCC_APB2RSTR_USART1RST_BIT      14
    #define RCC_APB2RSTR_TIM8RST_BIT        13
    #define RCC_APB2RSTR_SPI1RST_BIT        12
    #define RCC_APB2RSTR_TIM1RST_BIT        11
    #define RCC_APB2RSTR_ADC2RST_BIT        10
    #define RCC_APB2RSTR_ADC1RST_BIT        9
    #define RCC_APB2RSTR_IOPGRST_BIT        8
    #define RCC_APB2RSTR_IOPFRST_BIT        7
    #define RCC_APB2RSTR_IOPERST_BIT        6
    #define RCC_APB2RSTR_IOPDRST_BIT        5
    #define RCC_APB2RSTR_IOPCRST_BIT        4
    #define RCC_APB2RSTR_IOPBRST_BIT        3
    #define RCC_APB2RSTR_IOPARST_BIT        2
    #define RCC_APB2RSTR_AFIORST_BIT        0

    #define RCC_APB2RSTR_TIM11RST           (1U << RCC_APB2RSTR_TIM11RST_BIT)
    #define RCC_APB2RSTR_TIM10RST           (1U << RCC_APB2RSTR_TIM10RST_BIT)
    #define RCC_APB2RSTR_TIM9RST            (1U << RCC_APB2RSTR_TIM9RST_BIT)
    #define RCC_APB2RSTR_ADC3RST            (1U << RCC_APB2RSTR_ADC3RST_BIT)
    #define RCC_APB2RSTR_USART1RST          (1U << RCC_APB2RSTR_USART1RST_BIT)
    #define RCC_APB2RSTR_TIM8RST            (1U << RCC_APB2RSTR_TIM8RST_BIT)
    #define RCC_APB2RSTR_SPI1RST            (1U << RCC_APB2RSTR_SPI1RST_BIT)
    #define RCC_APB2RSTR_TIM1RST            (1U << RCC_APB2RSTR_TIM1RST_BIT)
    #define RCC_APB2RSTR_ADC2RST            (1U << RCC_APB2RSTR_ADC2RST_BIT)
    #define RCC_APB2RSTR_ADC1RST            (1U << RCC_APB2RSTR_ADC1RST_BIT)
    #define RCC_APB2RSTR_IOPGRST            (1U << RCC_APB2RSTR_IOPGRST_BIT)
    #define RCC_APB2RSTR_IOPFRST            (1U << RCC_APB2RSTR_IOPFRST_BIT)
    #define RCC_APB2RSTR_IOPERST            (1U << RCC_APB2RSTR_IOPERST_BIT)
    #define RCC_APB2RSTR_IOPDRST            (1U << RCC_APB2RSTR_IOPDRST_BIT)
    #define RCC_APB2RSTR_IOPCRST            (1U << RCC_APB2RSTR_IOPCRST_BIT)
    #define RCC_APB2RSTR_IOPBRST            (1U << RCC_APB2RSTR_IOPBRST_BIT)
    #define RCC_APB2RSTR_IOPARST            (1U << RCC_APB2RSTR_IOPARST_BIT)
    #define RCC_APB2RSTR_AFIORST            (1U << RCC_APB2RSTR_AFIORST_BIT)

    #define RCC_APB1RSTR_DACRST_BIT         29
    #define RCC_APB1RSTR_PWRRST_BIT         28
    #define RCC_APB1RSTR_BKPRST_BIT         27
    #define RCC_APB1RSTR_CANRST_BIT         25
    #define RCC_APB1RSTR_USBRST_BIT         23
    #define RCC_APB1RSTR_I2C2RST_BIT        22
    #define RCC_APB1RSTR_I2C1RST_BIT        21
    #define RCC_APB1RSTR_UART5RST_BIT       20
    #define RCC_APB1RSTR_UART4RST_BIT       19
    #define RCC_APB1RSTR_USART3RST_BIT      18
    #define RCC_APB1RSTR_USART2RST_BIT      17
    #define RCC_APB1RSTR_SPI3RST_BIT        15
    #define RCC_APB1RSTR_SPI2RST_BIT        14
    #define RCC_APB1RSTR_WWDRST_BIT         11
    #define RCC_APB1RSTR_TIM14RST_BIT       8
    #define RCC_APB1RSTR_TIM13RST_BIT       7
    #define RCC_APB1RSTR_TIM12RST_BIT       6
    #define RCC_APB1RSTR_TIM7RST_BIT        5
    #define RCC_APB1RSTR_TIM6RST_BIT        4
    #define RCC_APB1RSTR_TIM5RST_BIT        3
    #define RCC_APB1RSTR_TIM4RST_BIT        2
    #define RCC_APB1RSTR_TIM3RST_BIT        1
    #define RCC_APB1RSTR_TIM2RST_BIT        0

    #define RCC_APB1RSTR_DACRST             (1U << RCC_APB1RSTR_DACRST_BIT)
    #define RCC_APB1RSTR_PWRRST             (1U << RCC_APB1RSTR_PWRRST_BIT)
    #define RCC_APB1RSTR_BKPRST             (1U << RCC_APB1RSTR_BKPRST_BIT)
    #define RCC_APB1RSTR_CANRST             (1U << RCC_APB1RSTR_CANRST_BIT)
    #define RCC_APB1RSTR_USBRST             (1U << RCC_APB1RSTR_USBRST_BIT)
    #define RCC_APB1RSTR_I2C2RST            (1U << RCC_APB1RSTR_I2C2RST_BIT)
    #define RCC_APB1RSTR_I2C1RST            (1U << RCC_APB1RSTR_I2C1RST_BIT)
    #define RCC_APB1RSTR_UART5RST           (1U << RCC_APB1RSTR_UART5RST_BIT)
    #define RCC_APB1RSTR_UART4RST           (1U << RCC_APB1RSTR_UART4RST_BIT)
    #define RCC_APB1RSTR_USART3RST          (1U << RCC_APB1RSTR_USART3RST_BIT)
    #define RCC_APB1RSTR_USART2RST          (1U << RCC_APB1RSTR_USART2RST_BIT)
    #define RCC_APB1RSTR_SPI3RST            (1U << RCC_APB1RSTR_SPI3RST_BIT)
    #define RCC_APB1RSTR_SPI2RST            (1U << RCC_APB1RSTR_SPI2RST_BIT)
    #define RCC_APB1RSTR_WWDRST             (1U << RCC_APB1RSTR_WWDRST_BIT)
    #define RCC_APB1RSTR_TIM14RST           (1U << RCC_APB1RSTR_TIM14RST_BIT)
    #define RCC_APB1RSTR_TIM13RST           (1U << RCC_APB1RSTR_TIM13RST_BIT)
    #define RCC_APB1RSTR_TIM12RST           (1U << RCC_APB1RSTR_TIM12RST_BIT)
    #define RCC_APB1RSTR_TIM7RST            (1U << RCC_APB1RSTR_TIM7RST_BIT)
    #define RCC_APB1RSTR_TIM6RST            (1U << RCC_APB1RSTR_TIM6RST_BIT)
    #define RCC_APB1RSTR_TIM5RST            (1U << RCC_APB1RSTR_TIM5RST_BIT)
    #define RCC_APB1RSTR_TIM4RST            (1U << RCC_APB1RSTR_TIM4RST_BIT)
    #define RCC_APB1RSTR_TIM3RST            (1U << RCC_APB1RSTR_TIM3RST_BIT)
    #define RCC_APB1RSTR_TIM2RST            (1U << RCC_APB1RSTR_TIM2RST_BIT)

Peripheral clock enable registers
+++++++++++++++++++++++++++++++++

::

    #define RCC_AHBENR_SDIOEN_BIT           10
    #define RCC_AHBENR_FSMCEN_BIT           8
    #define RCC_AHBENR_CRCEN_BIT            7
    #define RCC_AHBENR_FLITFEN_BIT          4
    #define RCC_AHBENR_SRAMEN_BIT           2
    #define RCC_AHBENR_DMA2EN_BIT           1
    #define RCC_AHBENR_DMA1EN_BIT           0

    #define RCC_AHBENR_SDIOEN               (1U << RCC_AHBENR_SDIOEN_BIT)
    #define RCC_AHBENR_FSMCEN               (1U << RCC_AHBENR_FSMCEN_BIT)
    #define RCC_AHBENR_CRCEN                (1U << RCC_AHBENR_CRCEN_BIT)
    #define RCC_AHBENR_FLITFEN              (1U << RCC_AHBENR_FLITFEN_BIT)
    #define RCC_AHBENR_SRAMEN               (1U << RCC_AHBENR_SRAMEN_BIT)
    #define RCC_AHBENR_DMA2EN               (1U << RCC_AHBENR_DMA2EN_BIT)
    #define RCC_AHBENR_DMA1EN               (1U << RCC_AHBENR_DMA1EN_BIT)

    #define RCC_APB2ENR_TIM11EN_BIT         21
    #define RCC_APB2ENR_TIM10EN_BIT         20
    #define RCC_APB2ENR_TIM9EN_BIT          19
    #define RCC_APB2ENR_ADC3EN_BIT          15
    #define RCC_APB2ENR_USART1EN_BIT        14
    #define RCC_APB2ENR_TIM8EN_BIT          13
    #define RCC_APB2ENR_SPI1EN_BIT          12
    #define RCC_APB2ENR_TIM1EN_BIT          11
    #define RCC_APB2ENR_ADC2EN_BIT          10
    #define RCC_APB2ENR_ADC1EN_BIT          9
    #define RCC_APB2ENR_IOPGEN_BIT          8
    #define RCC_APB2ENR_IOPFEN_BIT          7
    #define RCC_APB2ENR_IOPEEN_BIT          6
    #define RCC_APB2ENR_IOPDEN_BIT          5
    #define RCC_APB2ENR_IOPCEN_BIT          4
    #define RCC_APB2ENR_IOPBEN_BIT          3
    #define RCC_APB2ENR_IOPAEN_BIT          2
    #define RCC_APB2ENR_AFIOEN_BIT          0

    #define RCC_APB2ENR_TIM11EN             (1U << RCC_APB2ENR_TIM11EN_BIT)
    #define RCC_APB2ENR_TIM10EN             (1U << RCC_APB2ENR_TIM10EN_BIT)
    #define RCC_APB2ENR_TIM9EN              (1U << RCC_APB2ENR_TIM9EN_BIT)
    #define RCC_APB2ENR_ADC3EN              (1U << RCC_APB2ENR_ADC3EN_BIT)
    #define RCC_APB2ENR_USART1EN            (1U << RCC_APB2ENR_USART1EN_BIT)
    #define RCC_APB2ENR_TIM8EN              (1U << RCC_APB2ENR_TIM8EN_BIT)
    #define RCC_APB2ENR_SPI1EN              (1U << RCC_APB2ENR_SPI1EN_BIT)
    #define RCC_APB2ENR_TIM1EN              (1U << RCC_APB2ENR_TIM1EN_BIT)
    #define RCC_APB2ENR_ADC2EN              (1U << RCC_APB2ENR_ADC2EN_BIT)
    #define RCC_APB2ENR_ADC1EN              (1U << RCC_APB2ENR_ADC1EN_BIT)
    #define RCC_APB2ENR_IOPGEN              (1U << RCC_APB2ENR_IOPGEN_BIT)
    #define RCC_APB2ENR_IOPFEN              (1U << RCC_APB2ENR_IOPFEN_BIT)
    #define RCC_APB2ENR_IOPEEN              (1U << RCC_APB2ENR_IOPEEN_BIT)
    #define RCC_APB2ENR_IOPDEN              (1U << RCC_APB2ENR_IOPDEN_BIT)
    #define RCC_APB2ENR_IOPCEN              (1U << RCC_APB2ENR_IOPCEN_BIT)
    #define RCC_APB2ENR_IOPBEN              (1U << RCC_APB2ENR_IOPBEN_BIT)
    #define RCC_APB2ENR_IOPAEN              (1U << RCC_APB2ENR_IOPAEN_BIT)
    #define RCC_APB2ENR_AFIOEN              (1U << RCC_APB2ENR_AFIOEN_BIT)

    #define RCC_APB1ENR_DACEN_BIT           29
    #define RCC_APB1ENR_PWREN_BIT           28
    #define RCC_APB1ENR_BKPEN_BIT           27
    #define RCC_APB1ENR_CANEN_BIT           25
    #define RCC_APB1ENR_USBEN_BIT           23
    #define RCC_APB1ENR_I2C2EN_BIT          22
    #define RCC_APB1ENR_I2C1EN_BIT          21
    #define RCC_APB1ENR_UART5EN_BIT         20
    #define RCC_APB1ENR_UART4EN_BIT         19
    #define RCC_APB1ENR_USART3EN_BIT        18
    #define RCC_APB1ENR_USART2EN_BIT        17
    #define RCC_APB1ENR_SPI3EN_BIT          15
    #define RCC_APB1ENR_SPI2EN_BIT          14
    #define RCC_APB1ENR_WWDEN_BIT           11
    #define RCC_APB1ENR_TIM14EN_BIT         8
    #define RCC_APB1ENR_TIM13EN_BIT         7
    #define RCC_APB1ENR_TIM12EN_BIT         6
    #define RCC_APB1ENR_TIM7EN_BIT          5
    #define RCC_APB1ENR_TIM6EN_BIT          4
    #define RCC_APB1ENR_TIM5EN_BIT          3
    #define RCC_APB1ENR_TIM4EN_BIT          2
    #define RCC_APB1ENR_TIM3EN_BIT          1
    #define RCC_APB1ENR_TIM2EN_BIT          0

    #define RCC_APB1ENR_DACEN               (1U << RCC_APB1ENR_DACEN_BIT)
    #define RCC_APB1ENR_PWREN               (1U << RCC_APB1ENR_PWREN_BIT)
    #define RCC_APB1ENR_BKPEN               (1U << RCC_APB1ENR_BKPEN_BIT)
    #define RCC_APB1ENR_CANEN               (1U << RCC_APB1ENR_CANEN_BIT)
    #define RCC_APB1ENR_USBEN               (1U << RCC_APB1ENR_USBEN_BIT)
    #define RCC_APB1ENR_I2C2EN              (1U << RCC_APB1ENR_I2C2EN_BIT)
    #define RCC_APB1ENR_I2C1EN              (1U << RCC_APB1ENR_I2C1EN_BIT)
    #define RCC_APB1ENR_UART5EN             (1U << RCC_APB1ENR_UART5EN_BIT)
    #define RCC_APB1ENR_UART4EN             (1U << RCC_APB1ENR_UART4EN_BIT)
    #define RCC_APB1ENR_USART3EN            (1U << RCC_APB1ENR_USART3EN_BIT)
    #define RCC_APB1ENR_USART2EN            (1U << RCC_APB1ENR_USART2EN_BIT)
    #define RCC_APB1ENR_SPI3EN              (1U << RCC_APB1ENR_SPI3EN_BIT)
    #define RCC_APB1ENR_SPI2EN              (1U << RCC_APB1ENR_SPI2EN_BIT)
    #define RCC_APB1ENR_WWDEN               (1U << RCC_APB1ENR_WWDEN_BIT)
    #define RCC_APB1ENR_TIM14EN             (1U << RCC_APB1ENR_TIM14EN_BIT)
    #define RCC_APB1ENR_TIM13EN             (1U << RCC_APB1ENR_TIM13EN_BIT)
    #define RCC_APB1ENR_TIM12EN             (1U << RCC_APB1ENR_TIM12EN_BIT)
    #define RCC_APB1ENR_TIM7EN              (1U << RCC_APB1ENR_TIM7EN_BIT)
    #define RCC_APB1ENR_TIM6EN              (1U << RCC_APB1ENR_TIM6EN_BIT)
    #define RCC_APB1ENR_TIM5EN              (1U << RCC_APB1ENR_TIM5EN_BIT)
    #define RCC_APB1ENR_TIM4EN              (1U << RCC_APB1ENR_TIM4EN_BIT)
    #define RCC_APB1ENR_TIM3EN              (1U << RCC_APB1ENR_TIM3EN_BIT)
    #define RCC_APB1ENR_TIM2EN              (1U << RCC_APB1ENR_TIM2EN_BIT)

Backup domain control register
++++++++++++++++++++++++++++++

::

    #define RCC_BDCR_BDRST_BIT              16
    #define RCC_BDCR_RTCEN_BIT              15
    #define RCC_BDCR_LSEBYP_BIT             2
    #define RCC_BDCR_LSERDY_BIT             1
    #define RCC_BDCR_LSEON_BIT              0

    #define RCC_BDCR_BDRST                  (1U << RCC_BDCR_BDRST_BIT)
    #define RCC_BDCR_RTCEN                  (1U << RCC_BDCR_RTC_BIT)
    #define RCC_BDCR_RTCSEL                 (0x3 << 8)
    #define RCC_BDCR_RTCSEL_NONE            (0x0 << 8)
    #define RCC_BDCR_RTCSEL_LSE             (0x1 << 8)
    #define RCC_BDCR_RTCSEL_HSE             (0x3 << 8)
    #define RCC_BDCR_LSEBYP                 (1U << RCC_BDCR_LSEBYP_BIT)
    #define RCC_BDCR_LSERDY                 (1U << RCC_BDCR_LSERDY_BIT)
    #define RCC_BDCR_LSEON                  (1U << RCC_BDCR_LSEON_BIT)

Control/status register
+++++++++++++++++++++++

::

    #define RCC_CSR_LPWRRSTF_BIT            31
    #define RCC_CSR_WWDGRSTF_BIT            30
    #define RCC_CSR_IWDGRSTF_BIT            29
    #define RCC_CSR_SFTRSTF_BIT             28
    #define RCC_CSR_PORRSTF_BIT             27
    #define RCC_CSR_PINRSTF_BIT             26
    #define RCC_CSR_RMVF_BIT                24
    #define RCC_CSR_LSIRDY_BIT              1
    #define RCC_CSR_LSION_BIT               0

    #define RCC_CSR_LPWRRSTF                (1U << RCC_CSR_LPWRRSTF_BIT)
    #define RCC_CSR_WWDGRSTF                (1U << RCC_CSR_WWDGRSTF_BIT)
    #define RCC_CSR_IWDGRSTF                (1U << RCC_CSR_IWDGRSTF_BIT)
    #define RCC_CSR_SFTRSTF                 (1U << RCC_CSR_SFTRSTF_BIT)
    #define RCC_CSR_PORRSTF                 (1U << RCC_CSR_PORRSTF_BIT)
    #define RCC_CSR_PINRSTF                 (1U << RCC_CSR_PINRSTF_BIT)
    #define RCC_CSR_RMVF                    (1U << RCC_CSR_RMVF_BIT)
    #define RCC_CSR_LSIRDY                  (1U << RCC_CSR_LSIRDY_BIT)
    #define RCC_CSR_LSION                   (1U << RCC_CSR_LSION_BIT)

STM32F2 Targets
~~~~~~~~~~~~~~~

Clock control register
++++++++++++++++++++++

::

    #define RCC_CR_PLLI2SRDY_BIT            27
    #define RCC_CR_PLLI2SON_BIT             26
    #define RCC_CR_PLLRDY_BIT               25
    #define RCC_CR_PLLON_BIT                24
    #define RCC_CR_CSSON_BIT                19
    #define RCC_CR_HSEBYP_BIT               18
    #define RCC_CR_HSERDY_BIT               17
    #define RCC_CR_HSEON_BIT                16
    #define RCC_CR_HSIRDY_BIT               1
    #define RCC_CR_HSION_BIT                0

    #define RCC_CR_PLLI2SRDY                (1U << RCC_CR_PLLI2SRDY_BIT)
    #define RCC_CR_PLLI2SON                 (1U << RCC_CR_PLLI2SON_BIT)
    #define RCC_CR_PLLRDY                   (1U << RCC_CR_PLLRDY_BIT)
    #define RCC_CR_PLLON                    (1U << RCC_CR_PLLON_BIT)
    #define RCC_CR_CSSON                    (1U << RCC_CR_CSSON_BIT)
    #define RCC_CR_HSEBYP                   (1U << RCC_CR_HSEBYP_BIT)
    #define RCC_CR_HSERDY                   (1U << RCC_CR_HSERDY_BIT)
    #define RCC_CR_HSEON                    (1U << RCC_CR_HSEON_BIT)
    #define RCC_CR_HSICAL                   (0xFF << 8)
    #define RCC_CR_HSITRIM                  (0x1F << 3)
    #define RCC_CR_HSIRDY                   (1U << RCC_CR_HSIRDY_BIT)
    #define RCC_CR_HSION                    (1U << RCC_CR_HSION_BIT)

PLL configuration register
++++++++++++++++++++++++++

::

    #define RCC_PLLCFGR_PLLSRC_BIT          22

    #define RCC_PLLCFGR_PLLQ                (0xF << 24)
    #define RCC_PLLCFGR_PLLSRC              (1U << RCC_PLLCFGR_PLLSRC_BIT)
    #define RCC_PLLCFGR_PLLSRC_HSI          (0x0 << RCC_PLLCFGR_PLLSRC_BIT)
    #define RCC_PLLCFGR_PLLSRC_HSE          (0x1 << RCC_PLLCFGR_PLLSRC_BIT)
    #define RCC_PLLCFGR_PLLP                (0x3 << 16)
    #define RCC_PLLCFGR_PLLN                (0x1FF << 6)
    #define RCC_PLLCFGR_PLLM                0x1F

Clock configuration register
++++++++++++++++++++++++++++

::

    #define RCC_CFGR_I2SSRC_BIT             23

    #define RCC_CFGR_MCO2                   (0x3 << 30)
    #define RCC_CFGR_MCO2_SYSCLK            (0x0 << 30)
    #define RCC_CFGR_MCO2_PLLI2S            (0x1 << 30)
    #define RCC_CFGR_MCO2_HSE               (0x2 << 30)
    #define RCC_CFGR_MCO2_PLL               (0x3 << 30)

    #define RCC_CFGR_MCO2PRE                (0x7 << 27)
    #define RCC_CFGR_MCO2PRE_DIV_1          (0x0 << 27)
    #define RCC_CFGR_MCO2PRE_DIV_2          (0x4 << 27)
    #define RCC_CFGR_MCO2PRE_DIV_3          (0x5 << 27)
    #define RCC_CFGR_MCO2PRE_DIV_4          (0x6 << 27)
    #define RCC_CFGR_MCO2PRE_DIV_5          (0x7 << 27)

    #define RCC_CFGR_MCO1PRE                (0x7 << 24)
    #define RCC_CFGR_MCO1PRE_DIV_1          (0x0 << 24)
    #define RCC_CFGR_MCO1PRE_DIV_2          (0x4 << 24)
    #define RCC_CFGR_MCO1PRE_DIV_3          (0x5 << 24)
    #define RCC_CFGR_MCO1PRE_DIV_4          (0x6 << 24)
    #define RCC_CFGR_MCO1PRE_DIV_5          (0x7 << 24)

    #define RCC_CFGR_I2SSRC                 (1U << RCC_CFGR_I2SSRC_BIT)
    #define RCC_CFGR_I2SSRC_PLLI2S          (0 << RCC_CFGR_I2SSRC_BIT)
    #define RCC_CFGR_I2SSRC_I2S_CKIN        (1 << RCC_CFGR_I2SSRC_BIT)

    #define RCC_CFGR_MCO1                   (0x3 << 21)
    #define RCC_CFGR_MCO1_HSI               (0x0 << 21)
    #define RCC_CFGR_MCO1_LSE               (0x1 << 21)
    #define RCC_CFGR_MCO1_HSE               (0x2 << 21)
    #define RCC_CFGR_MCO1_PLL               (0x3 << 21)

    #define RCC_CFGR_RTCPRE                 (0x1F << 16)

    /* Skipped: all the 0b0xx values meaning "not divided" */
    #define RCC_CFGR_PPRE2                  (0x7 << 13)
    #define RCC_CFGR_PPRE2_AHB_DIV_2        (0x4 << 13)
    #define RCC_CFGR_PPRE2_AHB_DIV_4        (0x5 << 13)
    #define RCC_CFGR_PPRE2_AHB_DIV_8        (0x6 << 13)
    #define RCC_CFGR_PPRE2_AHB_DIV_16       (0x7 << 13)

    /* Skipped: all the 0b0xx values meaning "not divided" */
    #define RCC_CFGR_PPRE1                  (0x7 << 10)
    #define RCC_CFGR_PPRE1_AHB_DIV_2        (0x4 << 10)
    #define RCC_CFGR_PPRE1_AHB_DIV_4        (0x5 << 10)
    #define RCC_CFGR_PPRE1_AHB_DIV_8        (0x6 << 10)
    #define RCC_CFGR_PPRE1_AHB_DIV_16       (0x7 << 10)

    /* Skipped: all the 0b0xxx values meaning "not divided" */
    #define RCC_CFGR_HPRE                   (0xF << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_2      (0x8 << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_4      (0x9 << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_8      (0xA << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_16     (0xB << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_64     (0xC << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_128    (0xD << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_256    (0xE << 4)
    #define RCC_CFGR_HPRE_SYSCLK_DIV_512    (0xF << 4)

    #define RCC_CFGR_SWS                    (0x3 << 2)
    #define RCC_CFGR_SWS_HSI                (0x0 << 2)
    #define RCC_CFGR_SWS_HSE                (0x1 << 2)
    #define RCC_CFGR_SWS_PLL                (0x2 << 2)

    #define RCC_CFGR_SW                     0x3
    #define RCC_CFGR_SW_HSI                 0x0
    #define RCC_CFGR_SW_HSE                 0x1
    #define RCC_CFGR_SW_PLL                 0x2

Clock interrupt register
++++++++++++++++++++++++

::

    #define RCC_CIR_CSSC_BIT                23

    #define RCC_CIR_PLLI2SRDYC_BIT          21
    #define RCC_CIR_PLLRDYC_BIT             20
    #define RCC_CIR_HSERDYC_BIT             19
    #define RCC_CIR_HSIRDYC_BIT             18
    #define RCC_CIR_LSERDYC_BIT             17
    #define RCC_CIR_LSIRDYC_BIT             16

    #define RCC_CIR_PLLI2SRDYIE_BIT         13
    #define RCC_CIR_PLLRDYIE_BIT            12
    #define RCC_CIR_HSERDYIE_BIT            11
    #define RCC_CIR_HSIRDYIE_BIT            10
    #define RCC_CIR_LSERDYIE_BIT            9
    #define RCC_CIR_LSIRDYIE_BIT            8

    #define RCC_CIR_CSSF_BIT                7

    #define RCC_CIR_PLLI2SRDYF_BIT          5
    #define RCC_CIR_PLLRDYF_BIT             4
    #define RCC_CIR_HSERDYF_BIT             3
    #define RCC_CIR_HSIRDYF_BIT             2
    #define RCC_CIR_LSERDYF_BIT             1
    #define RCC_CIR_LSIRDYF_BIT             0

    #define RCC_CIR_CSSC                    (1U << RCC_CIR_CSSC_BIT)

    #define RCC_CIR_PLLI2SRDYC              (1U << RCC_CIR_PLLI2SRDYC_BIT)
    #define RCC_CIR_PLLRDYC                 (1U << RCC_CIR_PLLRDYC_BIT)
    #define RCC_CIR_HSERDYC                 (1U << RCC_CIR_HSERDYC_BIT)
    #define RCC_CIR_HSIRDYC                 (1U << RCC_CIR_HSIRDYC_BIT)
    #define RCC_CIR_LSERDYC                 (1U << RCC_CIR_LSERDYC_BIT)
    #define RCC_CIR_LSIRDYC                 (1U << RCC_CIR_LSIRDYC_BIT)

    #define RCC_CIR_PLLI2SRDYIE             (1U << RCC_CIR_PLLI2SRDYIE_BIT)
    #define RCC_CIR_PLLRDYIE                (1U << RCC_CIR_PLLRDYIE_BIT)
    #define RCC_CIR_HSERDYIE                (1U << RCC_CIR_HSERDYIE_BIT)
    #define RCC_CIR_HSIRDYIE                (1U << RCC_CIR_HSIRDYIE_BIT)
    #define RCC_CIR_LSERDYIE                (1U << RCC_CIR_LSERDYIE_BIT)
    #define RCC_CIR_LSIRDYIE                (1U << RCC_CIR_LSIRDYIE_BIT)

    #define RCC_CIR_CSSF                    (1U << RCC_CIR_CSSF_BIT)

    #define RCC_CIR_PLLI2SRDYF              (1U << RCC_CIR_PLLI2SRDYF_BIT)
    #define RCC_CIR_PLLRDYF                 (1U << RCC_CIR_PLLRDYF_BIT)
    #define RCC_CIR_HSERDYF                 (1U << RCC_CIR_HSERDYF_BIT)
    #define RCC_CIR_HSIRDYF                 (1U << RCC_CIR_HSIRDYF_BIT)
    #define RCC_CIR_LSERDYF                 (1U << RCC_CIR_LSERDYF_BIT)
    #define RCC_CIR_LSIRDYF                 (1U << RCC_CIR_LSIRDYF_BIT)

Peripheral reset registers
++++++++++++++++++++++++++

::

    /* AHB1 */

    #define RCC_AHB1RSTR_OTGHSRST_BIT       29
    #define RCC_AHB1RSTR_ETHMACRST_BIT      25
    #define RCC_AHB1RSTR_DMA2RST_BIT        22
    #define RCC_AHB1RSTR_DMA1RST_BIT        21
    #define RCC_AHB1RSTR_CRCRST_BIT         12
    #define RCC_AHB1RSTR_GPIOIRST_BIT       8
    #define RCC_AHB1RSTR_GPIOHRST_BIT       7
    #define RCC_AHB1RSTR_GPIOGRST_BIT       6
    #define RCC_AHB1RSTR_GPIOFRST_BIT       5
    #define RCC_AHB1RSTR_GPIOERST_BIT       4
    #define RCC_AHB1RSTR_GPIODRST_BIT       3
    #define RCC_AHB1RSTR_GPIOCRST_BIT       2
    #define RCC_AHB1RSTR_GPIOBRST_BIT       1
    #define RCC_AHB1RSTR_GPIOARST_BIT       0

    #define RCC_AHB1RSTR_OTGHSRST           (1U << RCC_AHB1RSTR_OTGHSRST_BIT)
    #define RCC_AHB1RSTR_ETHMACRST          (1U << RCC_AHB1RSTR_ETHMACRST_BIT)
    #define RCC_AHB1RSTR_DMA2RST            (1U << RCC_AHB1RSTR_DMA2RST_BIT)
    #define RCC_AHB1RSTR_DMA1RST            (1U << RCC_AHB1RSTR_DMA1RST_BIT)
    #define RCC_AHB1RSTR_CRCRST             (1U << RCC_AHB1RSTR_CRCRST_BIT)
    #define RCC_AHB1RSTR_GPIOIRST           (1U << RCC_AHB1RSTR_GPIOIRST_BIT)
    #define RCC_AHB1RSTR_GPIOHRST           (1U << RCC_AHB1RSTR_GPIOHRST_BIT)
    #define RCC_AHB1RSTR_GPIOGRST           (1U << RCC_AHB1RSTR_GPIOGRST_BIT)
    #define RCC_AHB1RSTR_GPIOFRST           (1U << RCC_AHB1RSTR_GPIOFRST_BIT)
    #define RCC_AHB1RSTR_GPIOERST           (1U << RCC_AHB1RSTR_GPIOERST_BIT)
    #define RCC_AHB1RSTR_GPIODRST           (1U << RCC_AHB1RSTR_GPIODRST_BIT)
    #define RCC_AHB1RSTR_GPIOCRST           (1U << RCC_AHB1RSTR_GPIOCRST_BIT)
    #define RCC_AHB1RSTR_GPIOBRST           (1U << RCC_AHB1RSTR_GPIOBRST_BIT)
    #define RCC_AHB1RSTR_GPIOARST           (1U << RCC_AHB1RSTR_GPIOARST_BIT)

    /* AHB2 */

    #define RCC_AHB2RSTR_OTGFSRST_BIT       7
    #define RCC_AHB2RSTR_RNGRST_BIT         6
    #define RCC_AHB2RSTR_HASHRST_BIT        5
    #define RCC_AHB2RSTR_CRYPRST_BIT        4
    #define RCC_AHB2RSTR_DCMIRST_BIT        0

    #define RCC_AHB2RSTR_OTGFSRST           (1U << RCC_AHB2RSTR_OTGFSRST_BIT)
    #define RCC_AHB2RSTR_RNGRST             (1U << RCC_AHB2RSTR_RNGRST_BIT)
    #define RCC_AHB2RSTR_HASHRST            (1U << RCC_AHB2RSTR_HASHRST_BIT)
    #define RCC_AHB2RSTR_CRYPRST            (1U << RCC_AHB2RSTR_CRYPRST_BIT)
    #define RCC_AHB2RSTR_DCMIRST            (1U << RCC_AHB2RSTR_DCMIRST_BIT)

    /* AHB3 */

    #define RCC_AHB3RSTR_FSMCRST_BIT        0

    #define RCC_AHB3RSTR_FSMCRST            (1U << RCC_AHB3RSTR_FSMCRST_BIT)

    /* APB1 */

    #define RCC_APB1RSTR_DACRST_BIT         29
    #define RCC_APB1RSTR_PWRRST_BIT         28
    #define RCC_APB1RSTR_CAN2RST_BIT        26
    #define RCC_APB1RSTR_CAN1RST_BIT        25
    #define RCC_APB1RSTR_I2C3RST_BIT        23
    #define RCC_APB1RSTR_I2C2RST_BIT        22
    #define RCC_APB1RSTR_I2C1RST_BIT        21
    #define RCC_APB1RSTR_UART5RST_BIT       20
    #define RCC_APB1RSTR_UART4RST_BIT       19
    #define RCC_APB1RSTR_UART3RST_BIT       18
    #define RCC_APB1RSTR_UART2RST_BIT       17
    #define RCC_APB1RSTR_SPI3RST_BIT        15
    #define RCC_APB1RSTR_SPI2RST_BIT        14
    #define RCC_APB1RSTR_WWDGRST_BIT        11
    #define RCC_APB1RSTR_TIM14RST_BIT       8
    #define RCC_APB1RSTR_TIM13RST_BIT       7
    #define RCC_APB1RSTR_TIM12RST_BIT       6
    #define RCC_APB1RSTR_TIM7RST_BIT        5
    #define RCC_APB1RSTR_TIM6RST_BIT        4
    #define RCC_APB1RSTR_TIM5RST_BIT        3
    #define RCC_APB1RSTR_TIM4RST_BIT        2
    #define RCC_APB1RSTR_TIM3RST_BIT        1
    #define RCC_APB1RSTR_TIM2RST_BIT        0

    #define RCC_APB1RSTR_DACRST             (1U << RCC_APB1RSTR_DACRST_BIT)
    #define RCC_APB1RSTR_PWRRST             (1U << RCC_APB1RSTR_PWRRST_BIT)
    #define RCC_APB1RSTR_CAN2RST            (1U << RCC_APB1RSTR_CAN2RST_BIT)
    #define RCC_APB1RSTR_CAN1RST            (1U << RCC_APB1RSTR_CAN1RST_BIT)
    #define RCC_APB1RSTR_I2C3RST            (1U << RCC_APB1RSTR_I2C3RST_BIT)
    #define RCC_APB1RSTR_I2C2RST            (1U << RCC_APB1RSTR_I2C2RST_BIT)
    #define RCC_APB1RSTR_I2C1RST            (1U << RCC_APB1RSTR_I2C1RST_BIT)
    #define RCC_APB1RSTR_UART5RST           (1U << RCC_APB1RSTR_UART5RST_BIT)
    #define RCC_APB1RSTR_UART4RST           (1U << RCC_APB1RSTR_UART4RST_BIT)
    #define RCC_APB1RSTR_UART3RST           (1U << RCC_APB1RSTR_UART3RST_BIT)
    #define RCC_APB1RSTR_UART2RST           (1U << RCC_APB1RSTR_UART2RST_BIT)
    #define RCC_APB1RSTR_SPI3RST            (1U << RCC_APB1RSTR_SPI3RST_BIT)
    #define RCC_APB1RSTR_SPI2RST            (1U << RCC_APB1RSTR_SPI2RST_BIT)
    #define RCC_APB1RSTR_WWDGRST            (1U << RCC_APB1RSTR_WWDGRST_BIT)
    #define RCC_APB1RSTR_TIM14RST           (1U << RCC_APB1RSTR_TIM14RST_BIT)
    #define RCC_APB1RSTR_TIM13RST           (1U << RCC_APB1RSTR_TIM13RST_BIT)
    #define RCC_APB1RSTR_TIM12RST           (1U << RCC_APB1RSTR_TIM12RST_BIT)
    #define RCC_APB1RSTR_TIM7RST            (1U << RCC_APB1RSTR_TIM7RST_BIT)
    #define RCC_APB1RSTR_TIM6RST            (1U << RCC_APB1RSTR_TIM6RST_BIT)
    #define RCC_APB1RSTR_TIM5RST            (1U << RCC_APB1RSTR_TIM5RST_BIT)
    #define RCC_APB1RSTR_TIM4RST            (1U << RCC_APB1RSTR_TIM4RST_BIT)
    #define RCC_APB1RSTR_TIM3RST            (1U << RCC_APB1RSTR_TIM3RST_BIT)
    #define RCC_APB1RSTR_TIM2RST            (1U << RCC_APB1RSTR_TIM2RST_BIT)

    /* APB2 */

    #define RCC_APB2RSTR_TIM11RST_BIT       18
    #define RCC_APB2RSTR_TIM10RST_BIT       17
    #define RCC_APB2RSTR_TIM9RST_BIT        16
    #define RCC_APB2RSTR_SYSCFGRST_BIT      14
    #define RCC_APB2RSTR_SPI1RST_BIT        12
    #define RCC_APB2RSTR_SDIORST_BIT        11
    #define RCC_APB2RSTR_ADCRST_BIT         8
    #define RCC_APB2RSTR_USART6RST_BIT      5
    #define RCC_APB2RSTR_USART1RST_BIT      4
    #define RCC_APB2RSTR_TIM8RST_BIT        1
    #define RCC_APB2RSTR_TIM1RST_BIT        0

    #define RCC_APB2RSTR_TIM11RST           (1U << RCC_APB2RSTR_TIM11RST_BIT)
    #define RCC_APB2RSTR_TIM10RST           (1U << RCC_APB2RSTR_TIM10RST_BIT)
    #define RCC_APB2RSTR_TIM9RST            (1U << RCC_APB2RSTR_TIM9RST_BIT)
    #define RCC_APB2RSTR_SYSCFGRST          (1U << RCC_APB2RSTR_SYSCFGRST_BIT)
    #define RCC_APB2RSTR_SPI1RST            (1U << RCC_APB2RSTR_SPI1RST_BIT)
    #define RCC_APB2RSTR_SDIORST            (1U << RCC_APB2RSTR_SDIORST_BIT)
    #define RCC_APB2RSTR_ADCRST             (1U << RCC_APB2RSTR_ADCRST_BIT)
    #define RCC_APB2RSTR_USART6RST          (1U << RCC_APB2RSTR_USART6RST_BIT)
    #define RCC_APB2RSTR_USART1RST          (1U << RCC_APB2RSTR_USART1RST_BIT)
    #define RCC_APB2RSTR_TIM8RST            (1U << RCC_APB2RSTR_TIM8RST_BIT)
    #define RCC_APB2RSTR_TIM1RST            (1U << RCC_APB2RSTR_TIM1RST_BIT)

Peripheral clock enable registers
+++++++++++++++++++++++++++++++++

::

    /* AHB1 */

    #define RCC_AHB1ENR_OTGHSULPIEN_BIT     30
    #define RCC_AHB1ENR_OTGHSEN_BIT         29
    #define RCC_AHB1ENR_ETHMACPTPEN_BIT     28
    #define RCC_AHB1ENR_ETHMACRXEN_BIT      27
    #define RCC_AHB1ENR_ETHMACTXEN_BIT      26
    #define RCC_AHB1ENR_ETHMACEN_BIT        25
    #define RCC_AHB1ENR_DMA2EN_BIT          22
    #define RCC_AHB1ENR_DMA1EN_BIT          21
    #define RCC_AHB1ENR_BKPSRAMEN_BIT       18
    #define RCC_AHB1ENR_CRCEN_BIT           12
    #define RCC_AHB1ENR_GPIOIEN_BIT         8
    #define RCC_AHB1ENR_GPIOHEN_BIT         7
    #define RCC_AHB1ENR_GPIOGEN_BIT         6
    #define RCC_AHB1ENR_GPIOFEN_BIT         5
    #define RCC_AHB1ENR_GPIOEEN_BIT         4
    #define RCC_AHB1ENR_GPIODEN_BIT         3
    #define RCC_AHB1ENR_GPIOCEN_BIT         2
    #define RCC_AHB1ENR_GPIOBEN_BIT         1
    #define RCC_AHB1ENR_GPIOAEN_BIT         0

    #define RCC_AHB1ENR_OTGHSULPIEN         (1U << RCC_AHB1ENR_OTGHSULPIEN_BIT)
    #define RCC_AHB1ENR_OTGHSEN             (1U << RCC_AHB1ENR_OTGHSEN_BIT)
    #define RCC_AHB1ENR_ETHMACPTPEN         (1U << RCC_AHB1ENR_ETHMACPTPEN_BIT)
    #define RCC_AHB1ENR_ETHMACRXEN          (1U << RCC_AHB1ENR_ETHMACRXEN_BIT)
    #define RCC_AHB1ENR_ETHMACTXEN          (1U << RCC_AHB1ENR_ETHMACTXEN_BIT)
    #define RCC_AHB1ENR_ETHMACEN            (1U << RCC_AHB1ENR_ETHMACEN_BIT)
    #define RCC_AHB1ENR_DMA2EN              (1U << RCC_AHB1ENR_DMA2EN_BIT)
    #define RCC_AHB1ENR_DMA1EN              (1U << RCC_AHB1ENR_DMA1EN_BIT)
    #define RCC_AHB1ENR_BKPSRAMEN           (1U << RCC_AHB1ENR_BKPSRAMEN_BIT)
    #define RCC_AHB1ENR_CRCEN               (1U << RCC_AHB1ENR_CRCEN_BIT)
    #define RCC_AHB1ENR_GPIOIEN             (1U << RCC_AHB1ENR_GPIOIEN_BIT)
    #define RCC_AHB1ENR_GPIOHEN             (1U << RCC_AHB1ENR_GPIOHEN_BIT)
    #define RCC_AHB1ENR_GPIOGEN             (1U << RCC_AHB1ENR_GPIOGEN_BIT)
    #define RCC_AHB1ENR_GPIOFEN             (1U << RCC_AHB1ENR_GPIOFEN_BIT)
    #define RCC_AHB1ENR_GPIOEEN             (1U << RCC_AHB1ENR_GPIOEEN_BIT)
    #define RCC_AHB1ENR_GPIODEN             (1U << RCC_AHB1ENR_GPIODEN_BIT)
    #define RCC_AHB1ENR_GPIOCEN             (1U << RCC_AHB1ENR_GPIOCEN_BIT)
    #define RCC_AHB1ENR_GPIOBEN             (1U << RCC_AHB1ENR_GPIOBEN_BIT)
    #define RCC_AHB1ENR_GPIOAEN             (1U << RCC_AHB1ENR_GPIOAEN_BIT)

    /* AHB2 */

    #define RCC_AHB2ENR_OTGFSEN_BIT         7
    #define RCC_AHB2ENR_RNGEN_BIT           6
    #define RCC_AHB2ENR_HASHEN_BIT          5
    #define RCC_AHB2ENR_CRYPEN_BIT          4
    #define RCC_AHB2ENR_DCMIEN_BIT          0

    #define RCC_AHB2ENR_OTGFSEN             (1U << RCC_AHB2ENR_OTGFSEN_BIT)
    #define RCC_AHB2ENR_RNGEN               (1U << RCC_AHB2ENR_RNGEN_BIT)
    #define RCC_AHB2ENR_HASHEN              (1U << RCC_AHB2ENR_HASHEN_BIT)
    #define RCC_AHB2ENR_CRYPEN              (1U << RCC_AHB2ENR_CRYPEN_BIT)
    #define RCC_AHB2ENR_DCMIEN              (1U << RCC_AHB2ENR_DCMIEN_BIT)

    /* AHB3 */

    #define RCC_AHB3ENR_FSMCEN_BIT          0

    #define RCC_AHB3ENR_FSMCEN              (1U << RCC_AHB3ENR_FSMCEN_BIT)

    /* APB1 */

    #define RCC_APB1ENR_DACEN_BIT           29
    #define RCC_APB1ENR_PWREN_BIT           28
    #define RCC_APB1ENR_CAN2EN_BIT          26
    #define RCC_APB1ENR_CAN1EN_BIT          25
    #define RCC_APB1ENR_I2C3EN_BIT          23
    #define RCC_APB1ENR_I2C2EN_BIT          22
    #define RCC_APB1ENR_I2C1EN_BIT          21
    #define RCC_APB1ENR_UART5EN_BIT         20
    #define RCC_APB1ENR_UART4EN_BIT         19
    #define RCC_APB1ENR_USART3EN_BIT        18
    #define RCC_APB1ENR_USART2EN_BIT        17
    #define RCC_APB1ENR_SPI3EN_BIT          15
    #define RCC_APB1ENR_SPI2EN_BIT          14
    #define RCC_APB1ENR_WWDGEN_BIT          11
    #define RCC_APB1ENR_TIM14EN_BIT         8
    #define RCC_APB1ENR_TIM13EN_BIT         7
    #define RCC_APB1ENR_TIM12EN_BIT         6
    #define RCC_APB1ENR_TIM7EN_BIT          5
    #define RCC_APB1ENR_TIM6EN_BIT          4
    #define RCC_APB1ENR_TIM5EN_BIT          3
    #define RCC_APB1ENR_TIM4EN_BIT          2
    #define RCC_APB1ENR_TIM3EN_BIT          1
    #define RCC_APB1ENR_TIM2EN_BIT          0

    #define RCC_APB1ENR_DACEN               (1U << RCC_APB1ENR_DACEN_BIT)
    #define RCC_APB1ENR_PWREN               (1U << RCC_APB1ENR_PWREN_BIT)
    #define RCC_APB1ENR_CAN2EN              (1U << RCC_APB1ENR_CAN2EN_BIT)
    #define RCC_APB1ENR_CAN1EN              (1U << RCC_APB1ENR_CAN1EN_BIT)
    #define RCC_APB1ENR_I2C3EN              (1U << RCC_APB1ENR_I2C3EN_BIT)
    #define RCC_APB1ENR_I2C2EN              (1U << RCC_APB1ENR_I2C2EN_BIT)
    #define RCC_APB1ENR_I2C1EN              (1U << RCC_APB1ENR_I2C1EN_BIT)
    #define RCC_APB1ENR_UART5EN             (1U << RCC_APB1ENR_UART5EN_BIT)
    #define RCC_APB1ENR_UART4EN             (1U << RCC_APB1ENR_UART4EN_BIT)
    #define RCC_APB1ENR_USART3EN            (1U << RCC_APB1ENR_USART3EN_BIT)
    #define RCC_APB1ENR_USART2EN            (1U << RCC_APB1ENR_USART2EN_BIT)
    #define RCC_APB1ENR_SPI3EN              (1U << RCC_APB1ENR_SPI3EN_BIT)
    #define RCC_APB1ENR_SPI2EN              (1U << RCC_APB1ENR_SPI2EN_BIT)
    #define RCC_APB1ENR_WWDGEN              (1U << RCC_APB1ENR_WWDGEN_BIT)
    #define RCC_APB1ENR_TIM14EN             (1U << RCC_APB1ENR_TIM14EN_BIT)
    #define RCC_APB1ENR_TIM13EN             (1U << RCC_APB1ENR_TIM13EN_BIT)
    #define RCC_APB1ENR_TIM12EN             (1U << RCC_APB1ENR_TIM12EN_BIT)
    #define RCC_APB1ENR_TIM7EN              (1U << RCC_APB1ENR_TIM7EN_BIT)
    #define RCC_APB1ENR_TIM6EN              (1U << RCC_APB1ENR_TIM6EN_BIT)
    #define RCC_APB1ENR_TIM5EN              (1U << RCC_APB1ENR_TIM5EN_BIT)
    #define RCC_APB1ENR_TIM4EN              (1U << RCC_APB1ENR_TIM4EN_BIT)
    #define RCC_APB1ENR_TIM3EN              (1U << RCC_APB1ENR_TIM3EN_BIT)
    #define RCC_APB1ENR_TIM2EN              (1U << RCC_APB1ENR_TIM2EN_BIT)

    /* APB2 */

    #define RCC_APB2ENR_TIM11EN_BIT         18
    #define RCC_APB2ENR_TIM10EN_BIT         17
    #define RCC_APB2ENR_TIM9EN_BIT          16
    #define RCC_APB2ENR_SYSCFGEN_BIT        14
    #define RCC_APB2ENR_SPI1EN_BIT          12
    #define RCC_APB2ENR_SDIOEN_BIT          11
    #define RCC_APB2ENR_ADC3EN_BIT          10
    #define RCC_APB2ENR_ADC2EN_BIT          9
    #define RCC_APB2ENR_ADC1EN_BIT          8
    #define RCC_APB2ENR_USART6EN_BIT        5
    #define RCC_APB2ENR_USART1EN_BIT        4
    #define RCC_APB2ENR_TIM8EN_BIT          1
    #define RCC_APB2ENR_TIM1EN_BIT          0

    #define RCC_APB2ENR_TIM11EN             (1U << RCC_APB2ENR_TIM11EN_BIT)
    #define RCC_APB2ENR_TIM10EN             (1U << RCC_APB2ENR_TIM10EN_BIT)
    #define RCC_APB2ENR_TIM9EN              (1U << RCC_APB2ENR_TIM9EN_BIT)
    #define RCC_APB2ENR_SYSCFGEN            (1U << RCC_APB2ENR_SYSCFGEN_BIT)
    #define RCC_APB2ENR_SPI1EN              (1U << RCC_APB2ENR_SPI1EN_BIT)
    #define RCC_APB2ENR_SDIOEN              (1U << RCC_APB2ENR_SDIOEN_BIT)
    #define RCC_APB2ENR_ADC3EN              (1U << RCC_APB2ENR_ADC3EN_BIT)
    #define RCC_APB2ENR_ADC2EN              (1U << RCC_APB2ENR_ADC2EN_BIT)
    #define RCC_APB2ENR_ADC1EN              (1U << RCC_APB2ENR_ADC1EN_BIT)
    #define RCC_APB2ENR_USART6EN            (1U << RCC_APB2ENR_USART6EN_BIT)
    #define RCC_APB2ENR_USART1EN            (1U << RCC_APB2ENR_USART1EN_BIT)
    #define RCC_APB2ENR_TIM8EN              (1U << RCC_APB2ENR_TIM8EN_BIT)
    #define RCC_APB2ENR_TIM1EN              (1U << RCC_APB2ENR_TIM1EN_BIT)

Peripheral clock enable in low power mode registers
+++++++++++++++++++++++++++++++++++++++++++++++++++

::

    /* AHB1 */

    #define RCC_AHB1LPENR_OTGHSULPILPEN_BIT 30
    #define RCC_AHB1LPENR_OTGHSLPEN_BIT     29
    #define RCC_AHB1LPENR_ETHMACPTPLPEN_BIT 28
    #define RCC_AHB1LPENR_ETHMACRXLPEN_BIT  27
    #define RCC_AHB1LPENR_ETHMACTXLPEN_BIT  26
    #define RCC_AHB1LPENR_ETHMACLPEN_BIT    25
    #define RCC_AHB1LPENR_DMA2LPEN_BIT      22
    #define RCC_AHB1LPENR_DMA1LPEN_BIT      21
    #define RCC_AHB1LPENR_BKPSRAMLPEN_BIT   18
    #define RCC_AHB1LPENR_SRAM2LPEN_BIT     17
    #define RCC_AHB1LPENR_SRAM1LPEN_BIT     16
    #define RCC_AHB1LPENR_FLITFLPEN_BIT     15
    #define RCC_AHB1LPENR_CRCLPEN_BIT       12
    #define RCC_AHB1LPENR_GPIOILPEN_BIT     8
    #define RCC_AHB1LPENR_GPIOGLPEN_BIT     6
    #define RCC_AHB1LPENR_GPIOFLPEN_BIT     5
    #define RCC_AHB1LPENR_GPIOELPEN_BIT     4
    #define RCC_AHB1LPENR_GPIODLPEN_BIT     3
    #define RCC_AHB1LPENR_GPIOCLPEN_BIT     2
    #define RCC_AHB1LPENR_GPIOBLPEN_BIT     1
    #define RCC_AHB1LPENR_GPIOALPEN_BIT     0

    #define RCC_AHB1LPENR_OTGHSULPILPEN     (1U << RCC_AHB1LPENR_OTGHSULPILPEN_BIT)
    #define RCC_AHB1LPENR_OTGHSLPEN         (1U << RCC_AHB1LPENR_OTGHSLPEN_BIT)
    #define RCC_AHB1LPENR_ETHMACPTPLPEN     (1U << RCC_AHB1LPENR_ETHMACPTPLPEN_BIT)
    #define RCC_AHB1LPENR_ETHMACRXLPEN      (1U << RCC_AHB1LPENR_ETHMACRXLPEN_BIT)
    #define RCC_AHB1LPENR_ETHMACTXLPEN      (1U << RCC_AHB1LPENR_ETHMACTXLPEN_BIT)
    #define RCC_AHB1LPENR_ETHMACLPEN        (1U << RCC_AHB1LPENR_ETHMACLPEN_BIT)
    #define RCC_AHB1LPENR_DMA2LPEN          (1U << RCC_AHB1LPENR_DMA2LPEN_BIT)
    #define RCC_AHB1LPENR_DMA1LPEN          (1U << RCC_AHB1LPENR_DMA1LPEN_BIT)
    #define RCC_AHB1LPENR_BKPSRAMLPEN       (1U << RCC_AHB1LPENR_BKPSRAMLPEN_BIT)
    #define RCC_AHB1LPENR_SRAM2LPEN         (1U << RCC_AHB1LPENR_SRAM2LPEN_BIT)
    #define RCC_AHB1LPENR_SRAM1LPEN         (1U << RCC_AHB1LPENR_SRAM1LPEN_BIT)
    #define RCC_AHB1LPENR_FLITFLPEN         (1U << RCC_AHB1LPENR_FLITFLPEN_BIT)
    #define RCC_AHB1LPENR_CRCLPEN           (1U << RCC_AHB1LPENR_CRCLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOILPEN         (1U << RCC_AHB1LPENR_GPIOILPEN_BIT)
    #define RCC_AHB1LPENR_GPIOGLPEN         (1U << RCC_AHB1LPENR_GPIOGLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOFLPEN         (1U << RCC_AHB1LPENR_GPIOFLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOELPEN         (1U << RCC_AHB1LPENR_GPIOELPEN_BIT)
    #define RCC_AHB1LPENR_GPIODLPEN         (1U << RCC_AHB1LPENR_GPIODLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOCLPEN         (1U << RCC_AHB1LPENR_GPIOCLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOBLPEN         (1U << RCC_AHB1LPENR_GPIOBLPEN_BIT)
    #define RCC_AHB1LPENR_GPIOALPEN         (1U << RCC_AHB1LPENR_GPIOALPEN_BIT)

    /* AHB2 */

    #define RCC_AHB2LPENR_OTGFSLPEN_BIT     7
    #define RCC_AHB2LPENR_RNGLPEN_BIT       6
    #define RCC_AHB2LPENR_HASHLPEN_BIT      5
    #define RCC_AHB2LPENR_CRYPLPEN_BIT      4
    #define RCC_AHB2LPENR_DCMILPEN_BIT      0

    #define RCC_AHB2LPENR_OTGFSLPEN         (1U << RCC_AHB2LPENR_OTGFSLPEN_BIT)
    #define RCC_AHB2LPENR_RNGLPEN           (1U << RCC_AHB2LPENR_RNGLPEN_BIT)
    #define RCC_AHB2LPENR_HASHLPEN          (1U << RCC_AHB2LPENR_HASHLPEN_BIT)
    #define RCC_AHB2LPENR_CRYPLPEN          (1U << RCC_AHB2LPENR_CRYPLPEN_BIT)
    #define RCC_AHB2LPENR_DCMILPEN          (1U << RCC_AHB2LPENR_DCMILPEN_BIT)

    /* AHB3 */

    #define RCC_AHB3LPENR_FSMCLPEN_BIT      0

    #define RCC_AHB3LPENR_FSMCLPEN          (1U << RCC_AHB3LPENR_FSMCLPEN_BIT)

    /* APB1 */

    #define RCC_APB1LPENR_DACLPEN_BIT       29
    #define RCC_APB1LPENR_PWRLPEN_BIT       28
    #define RCC_APB1LPENR_CAN2LPEN_BIT      26
    #define RCC_APB1LPENR_CAN1LPEN_BIT      25
    #define RCC_APB1LPENR_I2C3LPEN_BIT      23
    #define RCC_APB1LPENR_I2C2LPEN_BIT      22
    #define RCC_APB1LPENR_I2C1LPEN_BIT      21
    #define RCC_APB1LPENR_UART5LPEN_BIT     20
    #define RCC_APB1LPENR_UART4LPEN_BIT     19
    #define RCC_APB1LPENR_USART3LPEN_BIT    18
    #define RCC_APB1LPENR_USART2LPEN_BIT    17
    #define RCC_APB1LPENR_SPI3LPEN_BIT      15
    #define RCC_APB1LPENR_SPI2LPEN_BIT      14
    #define RCC_APB1LPENR_WWDGLPEN_BIT      11
    #define RCC_APB1LPENR_TIM14LPEN_BIT     8
    #define RCC_APB1LPENR_TIM13LPEN_BIT     7
    #define RCC_APB1LPENR_TIM12LPEN_BIT     6
    #define RCC_APB1LPENR_TIM7LPEN_BIT      5
    #define RCC_APB1LPENR_TIM6LPEN_BIT      4
    #define RCC_APB1LPENR_TIM5LPEN_BIT      3
    #define RCC_APB1LPENR_TIM4LPEN_BIT      2
    #define RCC_APB1LPENR_TIM3LPEN_BIT      1
    #define RCC_APB1LPENR_TIM2LPEN_BIT      0

    #define RCC_APB1LPENR_DACLPEN           (1U << RCC_APB1LPENR_DACLPEN_BIT)
    #define RCC_APB1LPENR_PWRLPEN           (1U << RCC_APB1LPENR_PWRLPEN_BIT)
    #define RCC_APB1LPENR_CAN2LPEN          (1U << RCC_APB1LPENR_CAN2LPEN_BIT)
    #define RCC_APB1LPENR_CAN1LPEN          (1U << RCC_APB1LPENR_CAN1LPEN_BIT)
    #define RCC_APB1LPENR_I2C3LPEN          (1U << RCC_APB1LPENR_I2C3LPEN_BIT)
    #define RCC_APB1LPENR_I2C2LPEN          (1U << RCC_APB1LPENR_I2C2LPEN_BIT)
    #define RCC_APB1LPENR_I2C1LPEN          (1U << RCC_APB1LPENR_I2C1LPEN_BIT)
    #define RCC_APB1LPENR_UART5LPEN         (1U << RCC_APB1LPENR_UART5LPEN_BIT)
    #define RCC_APB1LPENR_UART4LPEN         (1U << RCC_APB1LPENR_UART4LPEN_BIT)
    #define RCC_APB1LPENR_USART3LPEN        (1U << RCC_APB1LPENR_USART3LPEN_BIT)
    #define RCC_APB1LPENR_USART2LPEN        (1U << RCC_APB1LPENR_USART2LPEN_BIT)
    #define RCC_APB1LPENR_SPI3LPEN          (1U << RCC_APB1LPENR_SPI3LPEN_BIT)
    #define RCC_APB1LPENR_SPI2LPEN          (1U << RCC_APB1LPENR_SPI2LPEN_BIT)
    #define RCC_APB1LPENR_WWDGLPEN          (1U << RCC_APB1LPENR_WWDGLPEN_BIT)
    #define RCC_APB1LPENR_TIM14LPEN         (1U << RCC_APB1LPENR_TIM14LPEN_BIT)
    #define RCC_APB1LPENR_TIM13LPEN         (1U << RCC_APB1LPENR_TIM13LPEN_BIT)
    #define RCC_APB1LPENR_TIM12LPEN         (1U << RCC_APB1LPENR_TIM12LPEN_BIT)
    #define RCC_APB1LPENR_TIM7LPEN          (1U << RCC_APB1LPENR_TIM7LPEN_BIT)
    #define RCC_APB1LPENR_TIM6LPEN          (1U << RCC_APB1LPENR_TIM6LPEN_BIT)
    #define RCC_APB1LPENR_TIM5LPEN          (1U << RCC_APB1LPENR_TIM5LPEN_BIT)
    #define RCC_APB1LPENR_TIM4LPEN          (1U << RCC_APB1LPENR_TIM4LPEN_BIT)
    #define RCC_APB1LPENR_TIM3LPEN          (1U << RCC_APB1LPENR_TIM3LPEN_BIT)
    #define RCC_APB1LPENR_TIM2LPEN          (1U << RCC_APB1LPENR_TIM2LPEN_BIT)

    /* APB2 */

    #define RCC_APB2LPENR_TIM11LPEN_BIT     18
    #define RCC_APB2LPENR_TIM10LPEN_BIT     17
    #define RCC_APB2LPENR_TIM9LPEN_BIT      16
    #define RCC_APB2LPENR_SYSCFGLPEN_BIT    14
    #define RCC_APB2LPENR_SPI1LPEN_BIT      12
    #define RCC_APB2LPENR_SDIOLPEN_BIT      11
    #define RCC_APB2LPENR_ADC3LPEN_BIT      10
    #define RCC_APB2LPENR_ADC2LPEN_BIT      9
    #define RCC_APB2LPENR_ADC1LPEN_BIT      8
    #define RCC_APB2LPENR_USART6LPEN_BIT    5
    #define RCC_APB2LPENR_USART1LPEN_BIT    4
    #define RCC_APB2LPENR_TIM8LPEN_BIT      1
    #define RCC_APB2LPENR_TIM1LPEN_BIT      0

    #define RCC_APB2LPENR_TIM11LPEN         (1U << RCC_APB2LPENR_TIM11LPEN_BIT)
    #define RCC_APB2LPENR_TIM10LPEN         (1U << RCC_APB2LPENR_TIM10LPEN_BIT)
    #define RCC_APB2LPENR_TIM9LPEN          (1U << RCC_APB2LPENR_TIM9LPEN_BIT)
    #define RCC_APB2LPENR_SYSCFGLPEN        (1U << RCC_APB2LPENR_SYSCFGLPEN_BIT)
    #define RCC_APB2LPENR_SPI1LPEN          (1U << RCC_APB2LPENR_SPI1LPEN_BIT)
    #define RCC_APB2LPENR_SDIOLPEN          (1U << RCC_APB2LPENR_SDIOLPEN_BIT)
    #define RCC_APB2LPENR_ADC3LPEN          (1U << RCC_APB2LPENR_ADC3LPEN_BIT)
    #define RCC_APB2LPENR_ADC2LPEN          (1U << RCC_APB2LPENR_ADC2LPEN_BIT)
    #define RCC_APB2LPENR_ADC1LPEN          (1U << RCC_APB2LPENR_ADC1LPEN_BIT)
    #define RCC_APB2LPENR_USART6LPEN        (1U << RCC_APB2LPENR_USART6LPEN_BIT)
    #define RCC_APB2LPENR_USART1LPEN        (1U << RCC_APB2LPENR_USART1LPEN_BIT)
    #define RCC_APB2LPENR_TIM8LPEN          (1U << RCC_APB2LPENR_TIM8LPEN_BIT)
    #define RCC_APB2LPENR_TIM1LPEN          (1U << RCC_APB2LPENR_TIM1LPEN_BIT)

Backup domain control register
++++++++++++++++++++++++++++++

::

    #define RCC_BDCR_BDRST_BIT              16
    #define RCC_BDCR_RTCEN_BIT              15
    #define RCC_BDCR_LSEBYP_BIT             2
    #define RCC_BDCR_LSERDY_BIT             1
    #define RCC_BDCR_LSEON_BIT              0

    #define RCC_BDCR_BDRST                  (1U << RCC_BDCR_BDRST_BIT)
    #define RCC_BDCR_RTCEN                  (1U << RCC_BDCR_RTCEN_BIT)
    #define RCC_BDCR_RTCSEL                 (0x3 << 8)
    #define RCC_BDCR_RTCSEL_NOCLOCK         (0x0 << 8)
    #define RCC_BDCR_RTCSEL_LSE             (0x1 << 8)
    #define RCC_BDCR_RTCSEL_LSI             (0x2 << 8)
    #define RCC_BDCR_RTCSEL_HSE_DIV         (0x3 << 8)
    #define RCC_BDCR_LSEBYP                 (1U << RCC_BDCR_LSEBYP_BIT)
    #define RCC_BDCR_LSERDY                 (1U << RCC_BDCR_LSERDY_BIT)
    #define RCC_BDCR_LSEON                  (1U << RCC_BDCR_LSEON_BIT)

Clock control and status register
+++++++++++++++++++++++++++++++++

::

    #define RCC_CSR_LPWRRSTF_BIT            31
    #define RCC_CSR_WWDGRSTF_BIT            30
    #define RCC_CSR_IWDGRSTF_BIT            29
    #define RCC_CSR_SFTRSTF_BIT             28
    #define RCC_CSR_PORRSTF_BIT             27
    #define RCC_CSR_PINRSTF_BIT             26
    #define RCC_CSR_BORRSTF_BIT             25
    #define RCC_CSR_RMVF_BIT                24
    #define RCC_CSR_LSIRDY_BIT              1
    #define RCC_CSR_LSION_BIT               0

    #define RCC_CSR_LPWRRSTF                (1U << RCC_CSR_LPWRRSTF_BIT)
    #define RCC_CSR_WWDGRSTF                (1U << RCC_CSR_WWDGRSTF_BIT)
    #define RCC_CSR_IWDGRSTF                (1U << RCC_CSR_IWDGRSTF_BIT)
    #define RCC_CSR_SFTRSTF                 (1U << RCC_CSR_SFTRSTF_BIT)
    #define RCC_CSR_PORRSTF                 (1U << RCC_CSR_PORRSTF_BIT)
    #define RCC_CSR_PINRSTF                 (1U << RCC_CSR_PINRSTF_BIT)
    #define RCC_CSR_BORRSTF                 (1U << RCC_CSR_BORRSTF_BIT)
    #define RCC_CSR_RMVF                    (1U << RCC_CSR_RMVF_BIT)
    #define RCC_CSR_LSIRDY                  (1U << RCC_CSR_LSIRDY_BIT)
    #define RCC_CSR_LSION                   (1U << RCC_CSR_LSION_BIT)

Spread spectrum clock generation register
+++++++++++++++++++++++++++++++++++++++++

::

    #define RCC_SSCGR_SSCGEN_BIT            31
    #define RCC_SSCGR_SPREADSEL_BIT         30

    #define RCC_SSCGR_SSCGEN                (1U << RCC_SSCGR_SSCGEN_BIT)
    #define RCC_SSCGR_SPREADSEL             (1U << RCC_SSCGR_SPREADSEL_BIT)
    #define RCC_SSCGR_SPREADSEL_CENTER      (0x0 << RCC_SSCGR_SPREADSEL_BIT)
    #define RCC_SSCGR_SPREADSEL_DOWN        (0x1 << RCC_SSCGR_SPREADSEL_BIT)
    #define RCC_SSCGR_INCSTEP               (0xFFF << 16)
    #define RCC_SSCGR_MODPER                0xFFFF

PLLI2S configuration register
+++++++++++++++++++++++++++++

::

    #define RCC_PLLI2SCFGR_PLLI2SR          (0x7 << 28)
    #define RCC_PLLI2SCFGR_PLLI2SN          (0x1FF << 6)

Deprecated Functionality
------------------------

.. doxygenfunction:: stm32f1::rcc_clk_init

