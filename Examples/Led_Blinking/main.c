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

#define F_CPU 8000000UL
#define CLK_DIV 8

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrtime.h"
#include "avrio.h"

#define LED_PIN ioPB0

ISR (TIMER0_OVF_vect) {
  time0++;
}

int main(void)
{

    static uint16_t last_time;

    // Setup timer0
    TCCR0 = _BV(CS01);      // timer prescaler 8
    TIMSK = _BV(TOIE0);     // enable timer 0 overflow interrupt
    sei();                  // enable interrupts

    //IO PINS Setup
    PINMODE(LED_PIN, OUTPUT);

    //Program loop
    while (1) {
        if ((time0 - last_time) > millis(500)) {
            last_time = time0;
            DIGITAL_WRITE(LED_PIN, ! DIGITAL_READ(LED_PIN));
        }
    }
    return 0;
}
