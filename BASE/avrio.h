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

/* AVRIO Library - input/output library for AVR 8-bit microcontrollers.
   The idea of this library is taken from Arduino project. Every pin of the microcontroller
   has unique 8-bit number given by constants ioPB0, ioBP1, ioPD0, etc.  In your program you use
   functions DIGITAL_READ, DIGITAL_WRITE for working with these pins.
   The main benefit is you don't need to worry about I/O registers.

   There are two kinds of function in this library:
     1) Static (macro) functions written by capital letters (PINMODE, DIGITAL_WRITE, DIGITAL_READ),
        where all input parameters must be constants - known at compile time.
     2) Dynamic function written by lower case (pinMode, digitalWrite, digitalRead) are
        more versatile but much more compute demanding too.

   The basic IO functions are:

   PINMODE(pin, type)
   pinMode(pin, type)
     Configures the specified pin to behave either as an input, output, or
     input with enabled the internal pullup resistors.

     Parameters:
        pin: the number of the pin whose mode you wish to set
        type: INPUT, OUTPUT, or INPUT_PULLUP.
   -------------------------------------------------------------------------

   DIGITAL_READ(pin)
   digitalRead(pin)
     Reads the value from a specified digital pin, either HIGH or LOW.

     Parameters:
       pin: the number of the digital pin you want to read (uint8_t)

     Returns:
       HIGH or LOW
   -------------------------------------------------------------------------

   DIGITAL_WRITE(pin, value)
   DIGITAL_WRITE(pin0, pin1, ... pin7, value)
   digitalWrite(pin, value)
     Write a HIGH or a LOW value to a digital pin. The function set or clear appropriate bit
     in PORT register of the pin. The macro function can setup multiple pins to the same value.
     Macro function is very sophisticated function. In the case of setting multiple pins, the
     pins belonging to one port register are setting in one bit-mask manipulation.

     For example:
        DIGITAL_WRITE(ioPB0, ioPB1, ioPB7, ioPD0, ioPD3, ioPC1, HIGH) is converted to:
        PORTB |= ((1 << 0) | (1 << 1) | (1 << 7);
        PORTD |= (1 << 0) | (1 << 3);
        sbi(PORTC, 1);  //Inline assembler function

     Parameters:
       pin: the number of the digital pin you want to write (uint8_t)
       value: HIGH (or non zero value), LOW (zero value)

   -------------------------------------------------------------------------

*/

#ifndef AVRIO_H_INCLUDED
#define AVRIO_H_INCLUDED


#include <avr/pgmspace.h>
#include <avr/../inttypes.h>

#include "preprocessor.h"
#include "avrio_pins.h"

#ifdef IO_REG16
    typedef volatile uint16_t ioreg_t;
#else
    typedef volatile uint8_t ioreg_t;
#endif // IO_REG16

#define NOT_A_PORT 255
#define NOT_A_PIN  255


/* Pin types */
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

/* Logical state values */
#define HIGH 1
#define LOW  0



/*
Following functions are compatible with Arduino functions
except of different pin values
Pin number is format PPPPPBBB, where
   PPPPP is port number
     BBB is bit number
 */
#define digitalPinToBit(pin) ((pin) & 0x07)
#define digitalPinToBitMask(pin) _BV(digitalPinToBit(pin))
#define digitalPinToPort(pin) (pin >> 3)

/* Returns address of register PORT , DDR or PIN for number pin
   For example:
        PORTB == ioPORT(ioPB1)
        DDRC  == ioDDR(ioPC2)
        PIND  == ioPIN(ioPD3)
*/
#define ioPORT(pin) CAT(PORT_AT_P, pin)
#define ioDDR(pin) CAT(DDR_AT_P, pin)
#define ioPIN(pin) CAT(PIN_AT_P, pin)


/*
    The following function are simillar to ioPIN, ioDDR, ioPORT
    The main differences are:
       - the functions return pointers
       - the input value is port number (result of digitalPinToPort).
       - functions cab be use in runtime. Input value can be variable not constant value.

    We can write:
        & ioPORT(ioPB2) == portOutputRegister(digitalPinToPort(ioPB2))

    portInputRegister   - return register PIN address
    portModeRegister    - return register DDR address
    portOutputRegister  - return register PORT address
*/

#ifdef IO_REG16
    #define portInputRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_word( &port_to_input_PGM[(port)] )) )
    #define portModeRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_word( &port_to_input_PGM[(port)])+1) )
    #define portOutputRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_word( &port_to_input_PGM[(port)])+2) )
#else
    #define portInputRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_byte( &port_to_input_PGM[(port)])) )
    #define portModeRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_byte( &port_to_input_PGM[(port)])+1) )
    #define portOutputRegister(port) ( (volatile ioreg_t *)(uint16_t) (pgm_read_byte( port_to_input_PGM + (port))+2) )
#endif // IO_REG16



#define DIGITAL_READ(pin)    \
    ((ioPIN(pin) >> digitalPinToBit(pin)) & 0x01)


/*
  Return true expression if port1 == port2, otherwiese returns
  false expression.
  Input parameters port1, port2 are constant values ioPORTA, ioPORTB, ...
*/

#define IIF_PORT_EQ(port1, port2, true, false)  \
    CAT(                                        \
        CAT(_IIF_PORT_EQ_, port1),              \
        CAT(_, port2)                           \
    ) (true, false)


/* Return port number for given pin */
#define PIN_TO_IOPORT(pin) CAT(IOPORT_AT_P, pin)


#define SET_IOSET_BITS0(reg)
#define CLEAR_IOSET_BITS0(reg)

#define SET_IOSET_BITS1(reg, pin0)                     \
    if (_SFR_IO_ADDR(reg) <= 0x1F) {                   \
        __asm__ __volatile__ (                         \
                    "sbi %0, %1" "\n\t"                \
                    : /* no outputs */                 \
                    : "I" (_SFR_IO_ADDR(reg)),         \
                      "I" (digitalPinToBit(pin0))      \
        );                                             \
    } else {                                           \
        reg |= digitalPinToBitMask(pin0);              \
    }

#define CLEAR_IOSET_BITS1(reg, pin0)                   \
    if (_SFR_IO_ADDR(reg) <= 0x1F) {                   \
        __asm__ __volatile__ (                         \
                    "cbi %0, %1" "\n\t"                \
                    : /* no outputs */                 \
                    : "I" (_SFR_IO_ADDR(reg)),         \
                      "I" (digitalPinToBit(pin0))      \
        );                                             \
    } else {                                           \
        reg &= ~digitalPinToBitMask(pin0);             \
    }


#define SET_IOSET_BITS2(reg, pin0, pin1)     \
    reg |= digitalPinToBitMask(pin0) |       \
           digitalPinToBitMask(pin1);

#define CLEAR_IOSET_BITS2(reg, pin0, pin1)   \
    reg &= ~digitalPinToBitMask(pin0) &      \
           ~digitalPinToBitMask(pin1);

#define SET_IOSET_BITS3(reg, pin0, pin1, pin2)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2);

#define CLEAR_IOSET_BITS3(reg, pin0, pin1, pin2)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2);

#define SET_IOSET_BITS4(reg, pin0, pin1, pin2, pin3)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2) | \
           digitalPinToBitMask(pin3);

#define CLEAR_IOSET_BITS4(reg, pin0, pin1, pin2, pin3)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2) & \
           ~digitalPinToBitMask(pin3);

#define SET_IOSET_BITS5(reg, pin0, pin1, pin2, pin3, pin4)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2) | \
           digitalPinToBitMask(pin3) | \
           digitalPinToBitMask(pin4);

#define CLEAR_IOSET_BITS5(reg, pin0, pin1, pin2, pin3, pin4)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2) & \
           ~digitalPinToBitMask(pin3) & \
           ~digitalPinToBitMask(pin4);

#define SET_IOSET_BITS6(reg, pin0, pin1, pin2, pin3, pin4, pin5)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2) | \
           digitalPinToBitMask(pin3) | \
           digitalPinToBitMask(pin4) | \
           digitalPinToBitMask(pin5);

#define CLEAR_IOSET_BITS6(reg, pin0, pin1, pin2, pin3, pin4, pin5)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2) & \
           ~digitalPinToBitMask(pin3) & \
           ~digitalPinToBitMask(pin4) & \
           ~digitalPinToBitMask(pin5);

#define SET_IOSET_BITS7(reg, pin0, pin1, pin2, pin3, pin4, pin5, pin6)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2) | \
           digitalPinToBitMask(pin3) | \
           digitalPinToBitMask(pin4) | \
           digitalPinToBitMask(pin5) | \
           digitalPinToBitMask(pin6);

#define CLEAR_IOSET_BITS7(reg, pin0, pin1, pin2, pin3, pin4, pin5, pin6)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2) & \
           ~digitalPinToBitMask(pin3) & \
           ~digitalPinToBitMask(pin4) & \
           ~digitalPinToBitMask(pin5) & \
           ~digitalPinToBitMask(pin6);

#define SET_IOSET_BITS8(reg, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)  \
    reg |= digitalPinToBitMask(pin0) | \
           digitalPinToBitMask(pin1) | \
           digitalPinToBitMask(pin2) | \
           digitalPinToBitMask(pin3) | \
           digitalPinToBitMask(pin4) | \
           digitalPinToBitMask(pin5) | \
           digitalPinToBitMask(pin6) | \
           digitalPinToBitMask(pin7);

#define CLEAR_IOSET_BITS8(reg, pin0, pin1, pin2, pin3, pin4, pin5, pin6)   \
    reg &= ~digitalPinToBitMask(pin0) & \
           ~digitalPinToBitMask(pin1) & \
           ~digitalPinToBitMask(pin2) & \
           ~digitalPinToBitMask(pin3) & \
           ~digitalPinToBitMask(pin4) & \
           ~digitalPinToBitMask(pin5) & \
           ~digitalPinToBitMask(pin6) & \
           ~digitalPinToBitMask(pin7);

/*
  Set/clear bits inside the register reg
  Input parameters:
     mode - value SET, CLEAR
     reg  - PORT, PIN, DDR
     ...  - pins for example ioPB1, ioPB2 ...
            or bit numbers 0..7
*/
#define ioSET_BITS(mode, ...)               \
    GET_MACRO9(__VA_ARGS__,                 \
               mode##_IOSET_BITS8,          \
               mode##_IOSET_BITS7,          \
               mode##_IOSET_BITS6,          \
               mode##_IOSET_BITS5,          \
               mode##_IOSET_BITS4,          \
               mode##_IOSET_BITS3,          \
               mode##_IOSET_BITS2,          \
               mode##_IOSET_BITS1,          \
               mode##_IOSET_BITS0)(__VA_ARGS__)


#define _SET_PINS4REG_ARGUMENT(port_name, pin)                                \
    ADD_ARGUMENT(                                                             \
        IS_NOT_EMPTY(pin,                                                     \
            IIF_PORT_EQ(ioPORT##port_name, PIN_TO_IOPORT(pin), pin, EMPTY())  \
        )                                                                     \
    )

/*
  Input parameters:
    reg         - expected values: PORT, PIN, DDR
    port_name   - expected values: A, B, C, D, E, ...
    mode        - expected values: CLEAR, SET
*/
#define _SET_PINS4REG_PORT_ITEM(reg, port_name, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
    IIF_PORT##port_name( \
        DEFER(ioSET_BITS)(mode, reg##port_name              \
                          _SET_PINS4REG_ARGUMENT(port_name, pin0)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin1)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin2)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin3)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin4)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin5)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin6)   \
                          _SET_PINS4REG_ARGUMENT(port_name, pin7)   \
                         ) , \
        EMPTY() \
    )

#define _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, A, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, B, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, C, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, D, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, E, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, F, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, G, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, H, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, I, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, J, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, K, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \
        _SET_PINS4REG_PORT_ITEM(reg, L, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7) \

#define _SET_PINS4REG7(reg, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, EMPTY())

#define _SET_PINS4REG6(reg, mode, pin0, pin1, pin2, pin3, pin4, pin5) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, pin3, pin4, pin5, EMPTY(), EMPTY())

#define _SET_PINS4REG5(reg, mode, pin0, pin1, pin2, pin3, pin4) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, pin3, pin4, EMPTY(), EMPTY(), EMPTY())

#define _SET_PINS4REG4(reg, mode, pin0, pin1, pin2, pin3) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, pin3, EMPTY(), EMPTY(), EMPTY(), EMPTY())

#define _SET_PINS4REG3(reg, mode, pin0, pin1, pin2) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, pin2, EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY())

#define _SET_PINS4REG2(reg, mode, pin0, pin1) \
    _SET_PINS4REG8(reg, mode, pin0, pin1, EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY())

#define _SET_PINS4REG1(reg, mode, pin0) \
    _SET_PINS4REG8(reg, mode, pin0, EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY(), EMPTY())

#define _SET_PINS4REG(reg, mode, ...)       \
    GET_MACRO8(__VA_ARGS__,            \
               _SET_PINS4REG8,          \
               _SET_PINS4REG7,          \
               _SET_PINS4REG6,          \
               _SET_PINS4REG5,          \
               _SET_PINS4REG4,          \
               _SET_PINS4REG3,          \
               _SET_PINS4REG2,          \
               _SET_PINS4REG1)(reg, mode, __VA_ARGS__)


#define SET_PINS4REG(reg, mode, ...)                 \
    do {                                             \
        if (mode) {                                  \
            _SET_PINS4REG(reg, SET, __VA_ARGS__)     \
        } else {                                     \
            _SET_PINS4REG(reg, CLEAR, __VA_ARGS__)   \
        }                                            \
    } while (0)


/* Digital write functions */
#define _DIGITAL_WRITE1(pin0, value)  SET_PINS4REG(PORT, value, pin0)
#define _DIGITAL_WRITE2(pin0, pin1, value) SET_PINS4REG(PORT, value, pin0, pin1)
#define _DIGITAL_WRITE3(pin0, pin1, pin2, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2)
#define _DIGITAL_WRITE4(pin0, pin1, pin2, pin3, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2, pin3)
#define _DIGITAL_WRITE5(pin0, pin1, pin2, pin3, pin4, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2, pin3, pin4)
#define _DIGITAL_WRITE6(pin0, pin1, pin2, pin3, pin4, pin5, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2, pin3, pin4, pin5)
#define _DIGITAL_WRITE7(pin0, pin1, pin2, pin3, pin4, pin5, pin6, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2, pin3, pin4, pin5, pin6)
#define _DIGITAL_WRITE8(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, value) SET_PINS4REG(PORT, value, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)

/*
   Set digital IO pins
   Input parameters:
        DIGITAL_WRITE(pin0, ..., pin7, value)
*/

#define DIGITAL_WRITE(...)               \
    GET_MACRO9(__VA_ARGS__,              \
               _DIGITAL_WRITE8,          \
               _DIGITAL_WRITE7,          \
               _DIGITAL_WRITE6,          \
               _DIGITAL_WRITE5,          \
               _DIGITAL_WRITE4,          \
               _DIGITAL_WRITE3,          \
               _DIGITAL_WRITE2,          \
               _DIGITAL_WRITE1,          \
               ERROR)(__VA_ARGS__)

/* Set pin to mode INPUT==0, INPUT_PULLUP==2, OUTPUT==1.
   Parameters pin, mode MUST BE constant value
 */
#define _PINMODE_GENERIC(mode, ...)                     \
    do {                                                \
        if ((mode) == INPUT) {                          \
            _SET_PINS4REG(DDR, CLEAR, __VA_ARGS__);     \
            _SET_PINS4REG(PORT, CLEAR, __VA_ARGS__);    \
        } else if ((mode) == INPUT_PULLUP) {            \
            _SET_PINS4REG(DDR, CLEAR, __VA_ARGS__);     \
            _SET_PINS4REG(PORT, SET, __VA_ARGS__);      \
        } else {                                        \
            _SET_PINS4REG(DDR, SET, __VA_ARGS__);       \
        }                                               \
    } while (0)

#define _PINMODE1(pin0, mode) _PINMODE_GENERIC(mode, pin0)
#define _PINMODE2(pin0, pin1, mode) _PINMODE_GENERIC(mode, pin0, pin1)
#define _PINMODE3(pin0, pin1, pin2, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2)
#define _PINMODE4(pin0, pin1, pin2, pin3, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2, pin3)
#define _PINMODE5(pin0, pin1, pin2, pin3, pin4, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2, pin3, pin4)
#define _PINMODE6(pin0, pin1, pin2, pin3, pin4, pin5, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2, pin3, pin4, pin5)
#define _PINMODE7(pin0, pin1, pin2, pin3, pin4, pin5, pin6, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6)
#define _PINMODE8(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7, mode) _PINMODE_GENERIC(mode, pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7)

#define PINMODE(...)               \
    GET_MACRO9(__VA_ARGS__,        \
               _PINMODE8,          \
               _PINMODE7,          \
               _PINMODE6,          \
               _PINMODE5,          \
               _PINMODE4,          \
               _PINMODE3,          \
               _PINMODE2,          \
               _PINMODE1,          \
               ERROR)(__VA_ARGS__)


extern const ioreg_t PROGMEM port_to_input_PGM[];
extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t val);
extern int digitalRead(uint8_t pin);

#endif // AVRIO_H_INCLUDED
