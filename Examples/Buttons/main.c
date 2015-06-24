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

/* Simple button example. We have one button BTN_A and three LEDs A, B, C.
 * Button is connected to the pin PD4 and internal AVR pullup resistor 
 * is used. Most of the time when the button is released there is HIGH 
 * value on the pin. On the contrary when it is pressed there is LOW value.
 * The value HIGH or LOW when the button is pressed is given
 * by BTN_PRESSED macro. Default is BTN_PRESSED 0, because the button
 * can be directly connected to AVR and no other compoments are needed.
 *
 * LEDs are lit under the following condition:
 *      A - button is pressed
 *      B - button is long pressed (1 seconds - given by BUTTON_LONG_CLICK)
 *      C - double click arise when:
 *            a) button is pressed than is released and finally again pressed
 *            b) every of this action takes less than 250 ms BUTTON_DOUBLE_CLICK
 */     

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrtime.h"
#include "avrio.h"
#include "buttons.h"

#define PIN_LED_A ioPB0
#define PIN_LED_B ioPD7
#define PIN_LED_C ioPD6

#define PIN_BTN_A ioPD4

TButtonState btnA;

ISR (TIMER0_OVF_vect) {
volatile static uint16_t last_time;
  time0++; // Time measurement - look at avrtime.h

  /* We update button state every 5 miliseconds. The time could be shorter
   * but it is not necessary. On the contrary the longer update time
   * is problem. So it is not much suitable call button_update function
   * in main loop.
   */
  if ((last_time - time0) > millis(5)) {
    button_update(&btnA);
    last_time = time0;
  }
}

int main(void)
{
    // Setup timer0
    TCCR0 = _BV(CS01);      // timer prescaler 8
    TIMSK = _BV(TOIE0);     // enable timer 0 overflow interrupt
    sei();                  // enable interrupts

    //IO PINS Setup
    PINMODE(PIN_LED_A, PIN_LED_B, PIN_LED_C, OUTPUT);
    DIGITAL_WRITE(PIN_LED_A, HIGH);

    // Button setup - we use internal pullup resistors
    button_init(&btnA, PIN_BTN_A, INPUT_PULLUP);

    //Program loop
    while (1) {
      if (button_pressed(&btnA) )  {
        DIGITAL_WRITE(PIN_LED_A, ! digitalRead(PIN_LED_A) );
      }
      
      if ( button_long_press(&btnA) ) {
        DIGITAL_WRITE(PIN_LED_B, ! DIGITAL_READ(PIN_LED_B) );
      }

      if ( button_double_click(&btnA) ) {
        DIGITAL_WRITE(PIN_LED_C, ! DIGITAL_READ(PIN_LED_C) );
      }
    }

    return 0;
}
