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
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "avrio.h"
#include "avrtime.h"
#include "buttons.h"
#include "hwserial.h"
#include "pt/pt.h"

#define LED_A ioPB0
#define LED_B ioPD7
#define LED_C ioPD6

#define BTN_A ioPD4

static uint8_t update_serial(struct pt *pt);

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

static uint8_t update_serial(struct pt *pt)
{
  PT_BEGIN(pt); 
  PT_WAIT_UNTIL(pt, usart_available() >= 3);
  
  //Suppress line endings
  if ( usart_readstr_P(PSTR("\n")) || usart_readstr_P(PSTR("\r")) ) 
    PT_RESTART(pt);
  

  if ( usart_readstr_P(PSTR("LED")) ) {
    PT_WAIT_UNTIL(pt, usart_available() >= 2); //Expected: "A0", "A1"
    
    char led = usart_getchar();
    uint8_t state = usart_getchar() - '0';

    switch (led) {
      case 'A':
        DIGITAL_WRITE(LED_A, state);
        break;
      case 'B':
        DIGITAL_WRITE(LED_B, state);
        break;
      case 'C':
        DIGITAL_WRITE(LED_C, state);
        break;
      default:
        usart_print_P(PSTR("Unknown command\n"));
        PT_WAIT_UNTIL(pt, usart_tx_empty());
        usart_clear();
        PT_RESTART(pt);
    }
  } else {
    usart_print_P(PSTR("Unknown command\n"));
    PT_WAIT_UNTIL(pt, usart_tx_empty());
    usart_clear();
    PT_RESTART(pt);
  }
  
  PT_END(pt);
}

static uint8_t process_buttons(struct pt *pt)
{
  PT_BEGIN(pt); 
  if (button_pressed(&btnA) ) {
     usart_print_P(PSTR("BUTTON PRESSED\n"));
     DIGITAL_WRITE(LED_C, ! digitalRead(LED_C) );
     PT_WAIT_UNTIL(pt, usart_tx_empty());
  }
  PT_END(pt);
}

int main(void)
{
    struct pt pt_serial;
    struct pt pt_buttons;

    // Setup timer0
    TCCR0 = _BV(CS01);      // timer prescaler 8
    TIMSK = _BV(TOIE0);     // enable timer 0 overflow interrupt

    usart_init(9600, 8, UARTS_PARITY_NONE, 1);
    PINMODE(LED_A, LED_B, LED_C, OUTPUT);
    
    // Button setup - we use internal pullup resistors
    button_init(&btnA, BTN_A, INPUT_PULLUP);
    
    sei();                  // enable interrupts
    
    usart_bprint_P(PSTR("Example of HW serial uarts.\n"));
    usart_bprint_P(PSTR("Write following commands, where 'X' can be A, B, C:\n"));
    usart_bprint_P(PSTR("  LEDX0 - turn off led X\n"));
    usart_bprint_P(PSTR("  LEDX1 - turn on led X\n"));
    usart_bprint_P(PSTR("\n"));
    usart_bprint_P(PSTR("When you press button on pin D0, message 'BUTTON PRESSED' will be sent.\n"));


    //Program loop
    while (1) {
      //_delay_ms(1000);
      update_serial(&pt_serial);
      process_buttons(&pt_buttons);

      if (usart_overrun()) {
        usart_bprint_P(PSTR("Buffer overrun !\n")); 
        usart_clear();  // Clear receive buffer and overrun flag
      }

    } // while
    return 0;
}
