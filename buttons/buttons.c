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

#include "buttons.h"
#include "avrtime.h"

void button_init(TButtonState *button_state, uint8_t pin, uint8_t mode)
{
  button_state->state_all = 0;
  button_state->pin = pin;
  button_state->last_time = time0;
  #if BTN_PRESSED == 0
    button_state->state = 1;
  #else
    button_state->state = 0;
  #endif

  pinMode(pin, mode);
}

void button_update(TButtonState *button_state)
{
   uint8_t raw_state;
   uint16_t debounce_time; 
   uint16_t elapsed_time;
   
   raw_state = digitalRead(button_state->pin);

   if (raw_state == button_state->state) {
     elapsed_time = time0 - button_state->last_time;
     button_state->bounce = 0;
     //Long click detection
     if ( (!button_state->long_press) && 
          (elapsed_time > millis(BUTTON_LONG_CLICK)) ) {
       
       button_state->long_press = 1;
       button_state->long_event = 1;
     }

     if ( elapsed_time > millis(BUTTON_DOUBLE_CLICK) ) {
       #if BTN_PRESSED == 0
         button_state->last_values = 0b00;
       #else
         button_state->last_values = 0b11;
       #endif
     }
   } else {
       if (button_state->bounce == 0) {
         button_state->last_time = time0;
         button_state->bounce = 1; 
       }
       elapsed_time = time0 - button_state->last_time;
        
       #if BTN_PRESSED == 0
         debounce_time = button_state->state ? millis(BUTTON_PRESS_MSEC) : millis(BUTTON_RELEASE_MSEC);
       #else
         debounce_time = button_state->state ? millis(BUTTON_RELEASE_MSEC) : millis(BUTTON_PRESS_MSEC);
       #endif

       if ( elapsed_time > debounce_time ) {
         button_state->state = raw_state;
         button_state->changed = 1;
         button_state->last_time = time0;
         button_state->long_press = 0;

         #if BTN_PRESSED == 0
           if (button_state->last_values == 0b01) 
         #else
           if (button_state->last_values == 0b10) 
         #endif  
           {
             button_state->dbl_click = 1;   
           }
         button_state->last_values = (button_state->last_values << 1) | raw_state; 
       } // if ( elapsed_time > debounce_time ) 
   }
}


uint8_t button_read(TButtonState *button_state, uint8_t mask)
{
  uint8_t result;
  result = button_state->state_all & mask;
  button_state->state_all &= ~(BTN_MASK_EVENT & mask); //Clear event state according the mask
  return result;
}


