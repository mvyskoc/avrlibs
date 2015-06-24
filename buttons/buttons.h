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


#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED 

#include <avr/../inttypes.h>
#include "avrio.h"
#include "global.h"
#include "avrtime.h"

/* Definition of value HIGH or LOW when the button is pressed. Default is LOW (0)
 * because with internal pullup resistor the button can be directly connected to AVR
 * without need to use other resistor.
 */
#ifndef BTN_PRESSED
  #define BTN_PRESSED 0       
#endif

#ifndef BUTTON_PRESS_MSEC
  #define BUTTON_PRESS_MSEC 10         // Stable time before registering pressed
#endif
#ifndef BUTTON_RELEASE_MSEC
  #define BUTTON_RELEASE_MSEC 50       // Stable time before registering released
#endif
#ifndef BUTTON_LONG_CLICK
  #define BUTTON_LONG_CLICK 1000       // Time for long registering long press or release
#endif
#ifndef BUTTON_DOUBLE_CLICK
  #define BUTTON_DOUBLE_CLICK 250      // Maximum length of button click or release during double click
#endif

/* Bit-mask of event flags, which are cleared by button_read */
#define BTN_MASK_EVENT  0b00001110   // Mask of change, long_click and double click event

/* The following functions return True when related event arise. 
 * Functions clear flags of processed events in TButtonState.  Only specific 
 * flag for given functions is cleared. 
 *
 * List of flags which are cleared by given functions
 *    changed      - button_changed, button_pressed, button_released
 *    long_event   - button_long_press, button_long_release
 *    dbl_click    - button_double_click
 *
 * You must avoid of writing of the following code. Condition B will never be executed, 
 * because condition A clear flag `changed` which is later tested in condition B. 
 * !!! WRONG !!! 
 *   A)  if ( button_pressed(btn) ) ...   OK
 *        ...
 *   B)  if ( button_released(btn) ) ...  NEVER EXECUTED
 *
 * !!! RIGHT !!!
 * If you need to test the above condition you must do that manully:
 *   A) if ( (btn.change) && (btn.state == BTN_PRESSED) ) ...
 *        ...
 *   B) if ( (btn.change) && (btn.state != BTN_PRESSED) ) ...
 *        ...
 *   C) btn.change = 0
 *    
 *
 */

#define button_change(btn)         (button_read(btn, 0b00000010) == 0b00000010)
#define button_double_click(btn)   (button_read(btn, 0b00001000) == 0b00001000)
#if BTN_PRESSED == 0
  #define button_pressed(btn)      (button_read(btn, 0b00000011) == 0b00000010)
  #define button_released(btn)     (button_read(btn, 0b00000011) == 0b00000011)
  #define button_long_press(btn)   (button_read(btn, 0b00000101) == 0b00000100)
  #define button_long_release(btn) (button_read(btn, 0b00000101) == 0b00000101)
#else
  #define button_pressed(btn)      (button_read(btn, 0b00000011) == 0b00000011)
  #define button_released(btn)     (button_read(btn, 0b00000011) == 0b00000010)
  #define button_long_press(btn)   (button_read(btn, 0b00000101) == 0b00000101)
  #define button_long_release(btn) (button_read(btn, 0b00000101) == 0b00000100)
#endif


typedef struct {
   union {
     /* Don't change order of bitfield members */ 
     struct {
        uint8_t state        : 1;           // Debounced button state
        uint8_t changed      : 1;           // State of button changed, it is cleared by button_read function
        uint8_t long_event   : 1;           // Set by update_button and clear by button_read, flag that long press event was processed  
        uint8_t dbl_click    : 1;           // 1 - double click event, clear by button_read function
        uint8_t long_press   : 1;           // Long press or release of the button
        uint8_t last_values  : 2;           // Doubleclick detection, two last changed values
        uint8_t bounce       : 1;           // Flag of bounce presence, start of time measuremnt
     };
     uint8_t state_all; 
   };
   uint8_t pin;                           // Button pin number defined in avrio library      
   volatile uint16_t last_time;           // Event time measurement
} TButtonState;

/* Initialize TButtonState variable, setup AVR IO pin as input or input with 
 * pullup resistor:
 *
 * Parameters:
 *   button_state - variable of type TButtonState
 *   pin - number of pin (ioPB0, ioPB1, ioPB2 ...) defined in avrio.h
 *   mode - pin mode INPUT or INPUT_PULLUP (turn on internal pullup resistor)
 */
extern void button_init(TButtonState *button_state, uint8_t pin, uint8_t mode); 

/* Update TButtonState. Function MUST be periodicaly called -  there is 
 * sufficient every 5 ms. The best is probably call the function from
 * the timmer interrupt routine.
 *
 * The function set the following variables in TButtonState
 *      state           - current debounce button state
 *      changed         - set to 1 when button state changed from pressed to released
 *                        and vice versa.
 *      long_event      - set to 1 when current state holds longer than time
 *                        in milliseconds given by BUTTON_LONG_CLICK.
 *      dbl_click       - double click event
 *   
 * Private variables - don't change from user program  
 *      long_press      - internal state for long press detection
 *      last_values     - internal state for double click detection. State of
 *                        two last state changes. The last_values variable is automaticly 
 *                        cleared if you are in one state (pressed or released) longer than
 *                        BUTTON_LONG_CLICK milliseconds.
 *      bounce          - internal flag - bounce (first change of state) detected.
 *                        It initiate time measuremnt
 *
 */
extern void button_update(TButtonState *button_state);

/* Return True according to selected mask. The function everytimes clear flag changed. */
extern uint8_t button_read(TButtonState *button_state, uint8_t mask);

#endif     /* -----  BUTTONS_H_INCLUDED  ----- */
