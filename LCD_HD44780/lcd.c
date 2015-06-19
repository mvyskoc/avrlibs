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

#include <stdlib.h>
#include <util/delay.h>

#include "avrtime.h"
#include "lcd.h"

/* Private functions and variables */
#ifdef LCD_IS_8BITMODE
    inline static void init8bits(TLcd *lcd);
    static void write8bits(TLcd *lcd, uint8_t value);
#endif

#ifdef LCD_IS_4BITMODE
    inline static void init4bits(TLcd *lcd);
    static void write4bits(TLcd *lcd, uint8_t value);
#endif

#ifdef LCD_PIN_RW
    static void lcd_setReadMode(TLcd *lcd);
#endif // LCD_PIN_RW


static const uint8_t LCD_ROW_OFFSET[4] PROGMEM = {0x00, 0x40, 0x10, 0x50};

inline static void lcd_command_timeout(TLcd *lcd, uint16_t time);

static void lcd_setWriteMode(TLcd *lcd);
static void pulseEnable(TLcd *lcd);


void lcd_init(TLcd *lcd, uint8_t pin_enable, TDisplayType lcd_type)
{
    uint8_t displayfunction=0;


    lcd->wait_start = 0; //Initial value of timer0 when begin measure time
    lcd->wait_diff = 0;  //Deference between actual time and wait_start
    lcd->send_text = NULL;
    lcd->ddram_address = 0;
    lcd->type = lcd_type;
    lcd->pin_enable = pin_enable;

    if ((lcd_type == LCD_TYPE_2LINE) || (lcd_type == LCD_TYPE_1X16)) {
         displayfunction |= LCD_2LINE;
    } else {
        //For one line display we try to setup bigger font size
         displayfunction |= LCD_1LINE | LCD_5x10DOTS;
    }


    PINMODE(LCD_PIN_RS, OUTPUT);
    #ifdef LCD_PIN_RW
        PINMODE(LCD_PIN_RW, OUTPUT);
    #endif // LCD_PIN_RW

    pinMode(lcd->pin_enable, OUTPUT);
    digitalWrite(lcd->pin_enable, LOW);
    lcd_setWriteMode(lcd); //Write to LCD

    _delay_ms(50);
    #ifdef LCD_IS_4BITMODE
        init4bits(lcd);
    #else
        init8bits(lcd);
        displayfunction |= LCD_8BITMODE;
    #endif // LCD_IS_4BITMODE

    lcd_command(lcd, LCD_FUNCTIONSET | displayfunction);
    _delay_us(100);

     // turn the display on with no cursor or blinking default
    lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

    lcd_display(lcd);
     _delay_us(2000);

    lcd_clear(lcd);
    _delay_us(2000);

    // Initialize to default text direction (for romance languages)
    lcd->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
    lcd_wait(lcd);
}

#ifdef LCD_IS_4BITMODE
inline static void init4bits(TLcd *lcd) {
    // we start in 8bit mode, try to set 4 bit mode
    write4bits(lcd, 0x03);
    _delay_us(4500); // wait min 4.1ms

    // second try
    write4bits(lcd, 0x03);
    _delay_us(150); // wait min  100 us

    // third go!
    write4bits(lcd, 0x03);
    _delay_us(150);

    // finally, set to 4-bit interface
    write4bits(lcd, 0x02);
    _delay_us(150);
}
#endif // LCD_IS_4BITMODE

#ifdef LCD_IS_8BITMODE
inline static void init8bits(TLcd *lcd) {
    lcd_command(lcd, LCD_FUNCTIONSET);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    lcd_command(lcd, LCD_FUNCTIONSET | LCD_8BITMODE);
    _delay_us(150);

    // third go
    lcd_command(lcd, LCD_FUNCTIONSET | LCD_8BITMODE);
    _delay_us(150);
}
#endif // LCD_IS_8BITMODE

/* WRITE: val == 0
   READ : val == 1
*/

#ifdef LCD_PIN_RW
static void lcd_setReadMode(TLcd *lcd) {
    DIGITAL_WRITE(LCD_PIN_RW, HIGH);
    #ifdef LCD_IS_8BITMODE
        DIGITAL_WRITE(LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3,
                      LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                      LOW);
        PINMODE(LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3,
                LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                INPUT);

    #else
        PINMODE(LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                INPUT);
        DIGITAL_WRITE(LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                      LOW);
    #endif
}
#endif // LCD_PIN_RW

static void lcd_setWriteMode(TLcd *lcd) {
    #ifdef LCD_PIN_RW
        DIGITAL_WRITE(LCD_PIN_RW, LOW);
    #endif // LCD_PIN_RW

    #ifdef LCD_IS_8BITMODE
        PINMODE(LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3,
                LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                OUTPUT);
    #else
        PINMODE(LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
                OUTPUT);
    #endif  //LCD_IS_8BITMODE
}


void lcd_clear(TLcd *lcd)
{
  lcd_command(lcd, LCD_CLEARDISPLAY);   // clear display, set cursor position to zero
  lcd->ddram_address = 0;
  lcd_command_timeout(lcd, millis(2));  // this command takes a long time!
}


void lcd_home(TLcd *lcd) {
  lcd_command(lcd, LCD_RETURNHOME);     // set cursor position to zero
  lcd->ddram_address = 0;
  lcd_command_timeout(lcd, millis(2));  // this command takes a long time!
}

// Turn the display on/off (quickly)
void lcd_noDisplay(TLcd *lcd) {
  lcd->displaycontrol &= ~LCD_DISPLAYON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void lcd_display(TLcd *lcd) {
  lcd->displaycontrol |= LCD_DISPLAYON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor(TLcd *lcd) {
  lcd->displaycontrol &= ~LCD_CURSORON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void lcd_cursor(TLcd *lcd) {
  lcd->displaycontrol |= LCD_CURSORON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink(TLcd *lcd) {
  lcd->displaycontrol &= ~LCD_BLINKON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void lcd_blink(TLcd *lcd) {
  lcd->displaycontrol |= LCD_BLINKON;
  lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(TLcd *lcd) {
  lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(TLcd *lcd) {
  lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(TLcd *lcd) {
  lcd->displaymode |= LCD_ENTRYLEFT;
  lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(TLcd *lcd) {
  lcd->displaymode &= ~LCD_ENTRYLEFT;
  lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(TLcd *lcd) {
  lcd->displaymode |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(TLcd *lcd) {
  lcd->displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}


void lcd_gotoxy(TLcd *lcd, uint8_t col, uint8_t row) {
  uint8_t row_offset;

  if ((col > 7) && (lcd->type == LCD_TYPE_1X16)) {
    col -= 8; row = 1;
  }
  row_offset = pgm_read_byte(&LCD_ROW_OFFSET[row]);
  lcd->ddram_address = row_offset + col;
  lcd_command(lcd, LCD_SETDDRAMADDR | (lcd->ddram_address));
}


uint8_t lcd_print(TLcd *lcd, const char *text) {
  if (lcd->send_text == NULL) {
    lcd->send_text = text;
  }

  if ((lcd->type == LCD_TYPE_1X16) && (lcd->ddram_address == 8)) {
      lcd_gotoxy(lcd, 0, 1);
      return 0;
  }

  char ch = *(lcd->send_text++);
  if ( ch != '\0' ) {
      lcd_write(lcd, ch);
      lcd->ddram_address++;
      return 0;
  } else {
      lcd->send_text = NULL;
      return 1;
  }
}

uint8_t lcd_print_P(TLcd *lcd, PGM_P text) {
  if (lcd->send_text == NULL) {
    lcd->send_text = text;
  }

  if ((lcd->type == LCD_TYPE_1X16) && (lcd->ddram_address == 8)) {
      lcd_gotoxy(lcd, 0, 1);
      return 0;
  }

  char ch = pgm_read_byte(lcd->send_text++);
  if ( ch != '\0' ) {
      lcd_write(lcd, ch);
      lcd->ddram_address++;
      return 0;
  } else {
      lcd->send_text = NULL;
      return 1;
  }
}

inline void lcd_bprint(TLcd *lcd, const char *text) {
    while ( !lcd_print(lcd, text) ) {
        lcd_wait(lcd);
    }
}

inline void lcd_bprint_P(TLcd *lcd, PGM_P text) {
    while ( !lcd_print_P(lcd, text) ) {
        lcd_wait(lcd);
    }
}

inline void lcd_command(TLcd *lcd, uint8_t value) {
  lcd_send(lcd, value, LOW);
}

inline void lcd_write(TLcd *lcd, uint8_t value) {
  lcd_send(lcd, value, HIGH);
}

// write either command or data, with automatic 4/8-bit selection
void lcd_send(TLcd *lcd, uint8_t value, uint8_t mode) {
  DIGITAL_WRITE(LCD_PIN_RS, mode);

  // if there is a RW pin indicated, set it low to Write
  lcd_setWriteMode(lcd);
  #ifdef LCD_IS_8BITMODE
    write8bits(lcd, value);
  #else
    write4bits(lcd, value>>4);
    write4bits(lcd, value);
  #endif // LCD_8BITMODE

  /* Common needs more than 37 us */
  lcd_command_timeout(lcd, micros(50));
}

#ifdef LCD_IS_4BITMODE
static void write4bits(TLcd *lcd, uint8_t value) {
    DIGITAL_WRITE(LCD_PIN_D4, (value & _BV(0)) );
    DIGITAL_WRITE(LCD_PIN_D5, (value & _BV(1)) );
    DIGITAL_WRITE(LCD_PIN_D6, (value & _BV(2)) );
    DIGITAL_WRITE(LCD_PIN_D7, (value & _BV(3)) );
    pulseEnable(lcd);
}
#endif // LCD_IS_4BITMODE

#ifdef LCD_IS_8BITMODE
static void write8bits(TLcd *lcd, uint8_t value) {
    DIGITAL_WRITE(LCD_PIN_D0, (value & _BV(0)) );
    DIGITAL_WRITE(LCD_PIN_D1, (value & _BV(1)) );
    DIGITAL_WRITE(LCD_PIN_D2, (value & _BV(2)) );
    DIGITAL_WRITE(LCD_PIN_D3, (value & _BV(3)) );
    DIGITAL_WRITE(LCD_PIN_D4, (value & _BV(4)) );
    DIGITAL_WRITE(LCD_PIN_D5, (value & _BV(5)) );
    DIGITAL_WRITE(LCD_PIN_D6, (value & _BV(6)) );
    DIGITAL_WRITE(LCD_PIN_D7, (value & _BV(7)) );
    pulseEnable(lcd);
}
#endif // LCD_IS_8BITMODE

static void pulseEnable(TLcd *lcd) {
    digitalWrite(lcd->pin_enable, HIGH);
    _delay_us(1);    // enable pulse must be >450ns
    digitalWrite(lcd->pin_enable, LOW);
}


//Return True when busy
uint8_t lcd_is_busy(TLcd *lcd) {
    if (lcd->wait_diff > 0) { //Waiting
        if ((time0 - lcd->wait_start) >= lcd->wait_diff) {
            lcd->wait_diff = 0;
            return 0;
        }
        return 1;
    }
    return 0;
}

inline static void lcd_command_timeout(TLcd *lcd, uint16_t time) {
    lcd->wait_start = time0;
    lcd->wait_diff = ((time == 0) ? 1 : time);
}


inline void lcd_wait(TLcd *lcd) {
  while (lcd_is_busy(lcd)) {
    ;
  }

}



