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

/* Library for communication with alphanumerical liquid crystal displays (LCDs).
   It allows to control LiquidCrystal displays (LCDs) based on the Hitachi HD44780
   (or a compatible for example St7066) chipset.

   The library works with in either 4 or 8 bit mode (i.e. using 4 or 8 data lines
   in addition to the rs, enable, and, optionally, the rw control lines). It is possible
   to control more than one LCDs, where all of them must use same data lines with only
   different enable lines.

   Before you will use this library you must copy "lcd.h", "lcd.c" files to your
   AVR project and modify header file "lcd.h" for your needs.

   Example:

   static TLcd lcd;

   lcd_init(&lcd, LCD_PIN_EN, LCD_TYPE_1X16);   //Init one times at the beginning of the program
   lcd_bprint_P(&lcd, PSTR("Hello world"));     //Blocking function of print function
*/

#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#include "avrio.h"
#include "global.h"

/* Define LCD_PIN_ values */

#define LCD_PIN_RS ioPB1
#define LCD_PIN_RW ioPB2        //Can be omitted

/* 4-bit bus */
#define LCD_PIN_D4 ioPC2
#define LCD_PIN_D5 ioPC3
#define LCD_PIN_D6 ioPC4
#define LCD_PIN_D7 ioPC5

/* 8-bit bus. For 4-bit bus comment out. */
#define LCD_PIN_D0 ioPB4
#define LCD_PIN_D1 ioPB5
#define LCD_PIN_D2 ioPC0
#define LCD_PIN_D3 ioPC1

/* */
#ifdef LCD_PIN_D0
    #define LCD_IS_8BITMODE
#else
    #define LCD_IS_4BITMODE
#endif


/* LCD instruction set */
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

typedef enum {
   LCD_TYPE_1LINE = 0,
   LCD_TYPE_2LINE = 1,
   LCD_TYPE_1X16 =  2,
   LCD_TYPE_4LINE = 3
} TDisplayType;

typedef struct  {
   uint8_t displaycontrol : 3;
   uint8_t displaymode : 2;

   TDisplayType type;
   uint8_t pin_enable;
   uint8_t ddram_address;
   const char* send_text;  // Pointer to sended text data
   uint16_t wait_start;    // Start of time measurement
   uint8_t  wait_diff;     // How many ticks we will wait
} TLcd;

extern void lcd_init(TLcd *lcd, uint8_t pin_enable, TDisplayType lcd_type) ;

#define LCD_BLOCK_COMMAND(fce, lcd, ...) \
   do {                                  \
       fce(lcd, ## __VA_ARGS__);         \
       lcd_wait(lcd);                    \
   } while (0)

/* Print text on LCD display - NON Blocking functions.
   Pointer must be defined during calling of lcd_print functions.
 */
extern uint8_t lcd_print(TLcd *lcd, const char *text);
extern uint8_t lcd_print_P(TLcd *lcd, PGM_P text);

/* Blocking print functions */
extern inline void lcd_bprint(TLcd *lcd, const char *text);
extern inline void lcd_bprint_P(TLcd *lcd, PGM_P text);

extern inline void lcd_command(TLcd *lcd, uint8_t value);
extern inline void lcd_write(TLcd *lcd, uint8_t value);

//Return True If LCD is busy and cannot accept any orders
extern uint8_t lcd_is_busy(TLcd *lcd);


//Block program until lcd_is_busy is True
extern inline void lcd_wait(TLcd *lcd);

// clear display, set cursor position to zero
extern void lcd_clear(TLcd *lcd);
#define lcd_bclear(lcd) LCD_BLOCK_COMMAND(lcd_clear, lcd)

// set cursor position to zero
extern void lcd_home(TLcd *lcd);
#define lcd_bhome(lcd) LCD_BLOCK_COMMAND(lcd_home, lcd)

//Quick turn on and off the display
extern void lcd_noDisplay(TLcd *lcd);
#define lcd_bnoDisplay(lcd) LCD_BLOCK_COMMAND(lcd_noDisplay, lcd)

extern void lcd_display(TLcd *lcd);
#define lcd_bdisplay(lcd) LCD_BLOCK_COMMAND(lcd_display, lcd)

extern void lcd_gotoxy(TLcd *lcd, uint8_t col, uint8_t row);
#define lcd_bgotoxy(lcd) LCD_BLOCK_COMMAND(lcd_gotoxy, lcd)

// Turns the underline cursor on/off
extern void lcd_noCursor(TLcd *lcd);
#define lcd_bnoCursor(lcd) LCD_BLOCK_COMMAND(lcd_noCursor, lcd)

extern void lcd_cursor(TLcd *lcd);
#define lcd_bcursor(lcd) LCD_BLOCK_COMMAND(lcd_cursor, lcd)

// Turn on and off the blinking cursor
extern void lcd_noBlink(TLcd *lcd);
#define lcd_bnoBlink(lcd) LCD_BLOCK_COMMAND(lcd_noBlink, lcd)

extern void lcd_blink(TLcd *lcd);
#define lcd_bblink(lcd) LCD_BLOCK_COMMAND(lcd_blink, lcd)

extern void lcd_scrollDisplayLeft(TLcd *lcd);
#define lcd_bscrollDisplayLeft(lcd) LCD_BLOCK_COMMAND(lcd_scrollDisplayLeft, lcd)

extern void lcd_scrollDisplayRight(TLcd *lcd);
#define lcd_bscrollDisplayRight(lcd) LCD_BLOCK_COMMAND(lcd_scrollDisplayRight, lcd)

// This is for text that flows Left to Right
extern void lcd_leftToRight(TLcd *lcd);
#define lcd_bleftToRight(lcd) LCD_BLOCK_COMMAND(lcd_leftToRight, lcd)

extern void lcd_rightToLeft(TLcd *lcd);
#define lcd_brightToLeft(lcd) LCD_BLOCK_COMMAND(lcd_rightToLeft, lcd)

// This will 'right justify' text from the cursor
extern void lcd_autoscroll(TLcd *lcd);
#define lcd_bautoscroll(lcd) LCD_BLOCK_COMMAND(lcd_autoscroll, lcd)

extern void lcd_noAutoscroll(TLcd *lcd);
#define lcd_bnoAutoscroll(lcd) LCD_BLOCK_COMMAND(lcd_noAutoscroll, lcd)

/* The following macro function are helper function with using Protothread library
   from  http://dunkels.com/adam/pt/
   Meaning of defined function is simillar to previous declared function with PT_ prefix.

   Input parameters:
     pt   - A pointer to the protothread control structure.
     lcd  - A pointer to lcd structure.
 */

#define PT_lcd_wait(pt, lcd) PT_WAIT_UNTIL((pt), !lcd_is_busy(lcd))
#define PT_lcd_print_P(pt, lcd, text)           \
    while ( ! lcd_print_P((lcd), (text)) ) {    \
       PT_lcd_wait(pt, lcd);                    \
    }

/* Attention! - text variable must be global NOT local variable */
#define PT_lcd_print(pt, lcd, text)               \
    while ( ! lcd_print((lcd), (text)) ) {        \
       PT_lcd_wait(pt, lcd);                      \
    }

#endif // LCD_H_INCLUDED
