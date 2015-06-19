#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>  //functions itoa

#include "avrtime.h"
#include "avrio.h"
#include "lcd.h"
#include "pt/pt.h"

#include <util/delay.h>

#define LCD_PIN_EN ioPB3
#define LED_PIN ioPB0

static TLcd lcd;
static struct pt pt_displej;
static uint8_t counter, start_count=0;


static uint8_t update_displej(struct pt *pt);

ISR (TIMER0_OVF_vect) {
  time0++;
  update_displej(&pt_displej); 
}

static void setup(void) {
    TCCR0 = _BV(CS01);   // set timer0 prescaller 8 
    TIMSK = _BV(TOIE0);  //enable timer0 interrupt
    PINMODE(LED_PIN, OUTPUT);

    PT_INIT(&pt_displej); //init display protothread before interrupts start	
    sei(); //enable interrupts and begin counting on display
}

// Function is very fast because does not use any blockind delay
static uint8_t update_displej(struct pt *pt) {
  static uint16_t last_time;
  static char buffer[4]; 

  PT_BEGIN(pt);
  PT_WAIT_UNTIL(pt, start_count); //Wait until start_count <> 0

  // Update display every 250 ms
  if ((time0 - last_time) > millis(250)) {
      last_time = time0;
      lcd_clear(&lcd); //nonblocking function
      PT_lcd_wait(pt, &lcd); //wait than lcd_clear finish

      PT_lcd_print_P(pt, &lcd, PSTR("Counting: "));
      itoa(counter, buffer, 10); 
      PT_lcd_print(pt, &lcd, buffer); //buffer is in RAM - we cannot use PT_lcd_print_P
  }

  PT_END(pt);
}


int main(void)
{
    setup();
    lcd_init(&lcd, LCD_PIN_EN, LCD_TYPE_1X16);

    lcd_bprint_P(&lcd, PSTR("Hello World !")); // Block print command
    _delay_ms(5000); //wait 2 seconds	
    
    lcd_bclear(&lcd);
    lcd_bgotoxy(&lcd, 5, 0); //row 0, col 5
    lcd_bprint_P(&lcd, PSTR("Goto 5,0"));  
    _delay_ms(5000); //wait 2 seconds

    start_count = 1; //Start display_update 

    //Infinite loop 
    //Every 200ms led change state and we increment counter
    //Value of counter is shown on display from timer interrupt routine
    while (1) {
      _delay_ms(200);
      DIGITAL_WRITE(LED_PIN, ! DIGITAL_READ(LED_PIN)); //LED blinking
      counter ++;
    }
    return 0;
}
