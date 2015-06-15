# avrlibs
AVR 8-bit microcontroller library

The library is divided into the following folders, where is supposed you will copy content 
of required folder into your project directory.

  BASE/ 
    Base library - functions needed in almost every program: work with digital I/O pins
    by similar (but more efective) way as in Arduino project, macro function for time measurement, ...
  
  LCD_HD44780/
    Library for communication with alphanumerical liquid crystal displays (LCDs)
    based on the Hitachi HD44780 (or a compatible for example St7066) chipset. Before use you must
    modify lcd.h.
  
  pt/ 
    Copy of Protothreads library from Adam Dunkels. For details, see pt/ or http://dunkels.com/adam/pt/

  
        
