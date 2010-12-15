.. _usart:

=======
 USART
=======

.. contents::
   :local:

Hardware/Circuit Design
-----------------------

The Maple has 3 separate USART devices. In the most simple use case,
the RX and TX pins are used to send data at a predetermined baudrate
with the line voltage level relative to ground.  Their usage is
documented in the :ref:`Serial Ports <lang-serial>` language reference
page.

Compatible Devices and Specifications
-------------------------------------

We have successfully used the Maple USART ports with an FT232R-based
USB-serial converter at up to 115200 baud; higher speeds should
certainly be possible.

Recommended Reading
-------------------

* `Wikipedia article on Universal asynchronous receiver/transmitter (USART) <http://en.wikipedia.org/wiki/Universal_asynchronous_receiver/transmitter>`_
* `Arduino reference on Serial <http://arduino.cc/en/Reference/Serial>`_
* STMicro documentation for STM32F103RB microcontroller:

  * `Datasheet <http://www.st.com/stonline/products/literature/ds/13587.pdf>`_ (pdf)
  * `Reference Manual <http://www.st.com/stonline/products/literature/rm/13902.pdf>`_ (pdf)
