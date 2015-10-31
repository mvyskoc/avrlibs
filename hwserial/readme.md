# Hardware USART/UART serial library

It is interrupt library using built-in UART or USART with the receive circular buffer. The transmission buffer is not used. You only provide pointer to transmitted data (e.g. pointer to string) and you must ensure that this pointer will be valid and not changed until all data are transmitted.

# Usage
The library use many macro definition and the result code is depended on the used MCU. There is not possible to create a true precompiled library. You must compile the library for your needs and your type of MCU.

Copy all files from the `hwserial` folder into your project directory or create suitable `Makefile` (look at the [Examples/hwserial](../Examples/hwserial) )

You can define following macro constants in the `global.h` , where *n* indicates individual UARTs (i.e.: empty, 1, 2, 3):

  - `USARTn_ENABLE` - Enable individual USARTs. if nothing is specified the first USART is enabled by default. Don't enable USARTs which you don't need.
  - `USARTn_RX_BUFFER` - size of circular receive buffere. The size must be power of 2.
  - `USARTn_TX_ISR_DISABLE` - disable interrupt routines for data transmission. Only blocking function for data transmission can be used.

# Library files
  - `hwserial.h` - library header file
  - `hwserial.c` 
  - `hwusart_single.inc` - generic code of one USART/UART.  Please don't include this file directly. The file is included from the files `hwserial.h` and `hwserial.c`for every enabled USART.

# Requirements
  - [BASE/preprocessor.h](../BASE/preprocessor.h)


