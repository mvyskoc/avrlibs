/*
 * Copyright 2015 Martin Vyskocil <m.vyskoc@seznam.cz>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Interrupt driven USART library for up to 4 USARTs.  
 *
 * In the global.h you can define following optional settings: 
 *   USART_ENABLE         - USART/0 enable 
 *   USART_RX_BUFFER      - USART/0 size of circular RX buffer 
 *   USART_TX_ISR_DISABLE - USART/0 disable TX interrupt routine. 
 *
 *   USART1_ENABLE     - USART1 enable
 *   USART1_RX_BUFFER  - USART1 size of circular RX buffer
 *   USART1_TX_ISR_DISABLE - USART1 disable TX interrupt routine. 
 *
 *   USART2_ENABLE     - USART2 enable
 *   USART2_RX_BUFFER  - USART2 size of circular RX buffer
 *   USART2_TX_ISR_DISABLE - USART2 disable TX interrupt routine. 
 *    
 *   USART3_ENABLE     - USART3 enable
 *   USART3_RX_BUFFER  - USART3 size of circular RX buffer
 *   USART3_TX_ISR_DISABLE - USART3 disable TX interrupt routine. 
 *
 * If you will not enable any USART. The USART/0 is enabled 
 * by default. Default size of RX buffer is 8. If you want to use 
 * for example USART0 and USART1 you must manualy enable both USARTs i.e. 
 * you must define USART_ENABLE and USART1_ENABLE.
 *
 */

#ifndef HWSERIAL_H_INCLUDED
#define HWSERIAL_H_INCLUDED

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "preprocessor.h"
#include "global.h"

#define UARTS_PARITY_NONE 0
#define UARTS_PARITY_EVEN 2
#define UARTS_PARITY_ODD 3
#define UARTS_STOPBIT_ONE 1
#define UARTS_STOPBIT_TWO 2

/* Default length of RX buffer.
 * The value must be power of 2: i.e.: 0, 2, 4, 8, 16, 32, 64, 128, 256 */
#define USART_DEFAULT_RX_BUFFER 8

/* We default enable USART0 if user did not enabled other */
#if !defined(USART_ENABLE) && !defined(USART1_ENABLE) && \
    !defined(USART2_ENABLE) && !defined(USART3_ENABLE) 
  #define USART_ENABLE
#endif
    
#ifdef USART0_RX_BUFFER
  #error Do not use USART0_RX_BUFFER. Define USART_RX_BUFFER for USART0.
#endif

#ifdef USART0_TX_ISR_DISABLE
  #error Do not use USART0_TX_ISR_DISABLE. Define USART_TX_ISR_DISABLE for USART0.
#endif

#if defined(USART_RX_BUFFER) && !defined(USART_ENABLE)
  #error USART_RX_BUFFER is set but USART/0 is disabled (USART_ENABLE not defined). 
#endif
#if defined(USART1_RX_BUFFER) && !defined(USART1_ENABLE)
  #error USART1_RX_BUFFER is set but USART1 is disabled (USART1_ENABLE not defined). 
#endif
#if defined(USART2_RX_BUFFER) && !defined(USART2_ENABLE)
  #error USAR2T_RX_BUFFER is set but USART2 is disabled (USART2_ENABLE not defined). 
#endif
#if defined(USART3_RX_BUFFER) && !defined(USART3_ENABLE)
  #error USART_RX_BUFFER is set but USART3 is disabled (USART3_ENABLE not defined). 
#endif

#ifndef USART_RX_BUFFER 
  #define USART_RX_BUFFER USART_DEFAULT_RX_BUFFER
#endif
#define USART0_RX_BUFFER USART_RX_BUFFER

#ifndef USART1_RX_BUFFER 
  #define USART1_RX_BUFFER USART_DEFAULT_RX_BUFFER
#endif
#ifndef USART2_RX_BUFFER 
  #define USART2_RX_BUFFER USART_DEFAULT_RX_BUFFER
#endif
#ifndef USART3_RX_BUFFER 
  #define USART3_RX_BUFFER USART_DEFAULT_RX_BUFFER
#endif

#ifdef USART_ENABLE
  #ifdef UDR
    #define USART_NUMBER
  #elif defined (UDR0)
    #define USART_NUMBER 0
  #else
    #error Sorry your device does not have any HW UARTs.
  #endif

  #ifdef USART_TX_ISR_DISABLE   
    #define _USART_TX_ISR_DISABLE
  #endif

  #ifdef USART_NUMBER
    #include "hwusart_single.inc"
  #endif
  
#endif //USART0_ENABLE
#undef USART_NUMBER
#undef _USART_TX_ISR_DISABLE

#ifdef USART1_ENABLE
  #ifdef USART1_TX_ISR_DISABLE   
    #define _USART_TX_ISR_DISABLE
  #endif

  #ifdef UDR1
    #define USART_NUMBER 1
    #include "hwusart_single.inc"
  #else
    #error Sorry your device does not have HW USART1.
  #endif
#endif
#undef USART_NUMBER
#undef _USART_TX_ISR_DISABLE

#ifdef USART2_ENABLE
  #ifdef USART2_TX_ISR_DISABLE   
    #define _USART_TX_ISR_DISABLE
  #endif
  #ifdef UDR2
    #define USART_NUMBER 2
    #include "hwusart_single.inc"
  #else
    #error Sorry your device does not support HW USART2.
  #endif
#endif
#undef USART_NUMBER
#undef _USART_TX_ISR_DISABLE

#ifdef USART3_ENABLE
  #ifdef USART3_TX_ISR_DISABLE   
    #define _USART_TX_ISR_DISABLE
  #endif
  #ifdef UDR3
    #define USART_NUMBER 3
    #include "hwusart_single.inc"
  #else
    #error Sorry your device does not support HW USART3.
  #endif
#endif
#undef USART_NUMBER
#undef _USART_TX_ISR_DISABLE


#undef USART_DEFAULT_RX_BUFFER


#endif //SERIAL_H_INCLUDED
