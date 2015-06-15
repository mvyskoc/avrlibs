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

/* AVRTIME library - simple and not critical time measurement.

   All time functions are based on the change of the value of 16-bit counter `time0`.
   Counter `time0` is global variable defined in file avrtime.c and you must increase its
   value by 1 in the timer overflow interrupt routine.

   Before use:
     1) Define timer prescaler value CLK_DIV and processor frequency F_CPU.
     2) Initialize timer interrupt and select suitable timer prescaler.
        TCCR0 = _BV(CS01);      // Timer0 prescaler 8 => #define CLK_DIV 8
        TIMSK = _BV(TOIE0);     // enable timer overflow interrupt for Timer0
        sei();                  // enable interrupts

     3) Define timer overflow interrupt routine, where you will increase `time0` variable
        ISR (TIMER0_OVF_vect) {
            time0++;
        }

    Example of blink led every 500 ms on PORTB3:
    #define CLK_DIV 8		//Must defined before you include "avrtime.h"
    #include "avrtime.h"

    int main(void) {
        static uint16_t last_time;

        while (1) {
            if ((time0 - last_time) > millis(500)) {
                last_time = time0;
                PORTB ^= _BV(3);
            }
        }
    }
*/

#ifndef AVRTIME_H_INCLUDED
#define AVRTIME_H_INCLUDED

#include <avr/../inttypes.h>

#define LTime(x) (*((unsigned char*)  &(x)+0))
#define HTime(x) (*((unsigned char*) &(x)+1))

/* Returns how many times must the HW timer overflows than elapse the time
   given by functions micros or millis.
   The time than the HW timer one times overflows is given by (CLK_DIV * 256) / F_CPU. This is the
   shortest time which you can measure. The longest time is given by 16-bit value 
   of variable time0 i.e. 65535 * (CLK_DIV * 256) / F_CPU

   Example of use is above. You count difference between value time0 and
   its previous value stored in variable last_time. The difference is the elapsed time
   which you compare with value returned by functions micros, millis.
*/

#define micros(us) ( ((us) * (F_CPU / 1000000UL)) / ((CLK_DIV) * 256UL) )
#define millis(ms) ( ((ms) * (F_CPU / 1000UL)) / ((CLK_DIV) * 256UL) )

//Vraci hodnotu citacte s prednastavenou delickou CLK_DIV, v case ms
#define TIMER_VALUE_MS(CLK_DIV, ms) ( ((F_CPU)/((CLK_DIV)*1000UL))*(ms) )
#define TIMER_VALUE_US(CLK_DIV, us) ( ((F_CPU)/( (CLK_DIV)*1000000UL) )*(us) )

/* Pocitadlo poctu preteceni citace/casovace 0 */
extern volatile uint16_t time0;

#endif
