# avrlibs
AVR 8-bit microcontroller library

The library is divided into the following folders. Copy all files of required library into your AVR project. In most cases you will need copy at least the **BASE** library. 

* **BASE/** common functions needed in almost every project.
  - `avrio.h` work with digital I/O pins by the similar (but more efective) way as in Arduino project.
  - `global.h` global constants definitions, you must at least modify (processor frequency `F_CPU` and timer prescaler value `CLK_DIV` needed in `avrtime.h`). 
  - `avrtime.h` - helper macro functions for time measurement
  - `preprocessor.h` - Pre-processor helper macro definitions for writing more complex macros. It is required by `avrio.h`.    
* **LCD_HD44780/** Library for communication with alphanumerical liquid crystal displays (LCDs) based on the Hitachi HD44780 (or a compatible for example St7066) chipset. You must modify `lcd.h` before use.
  
* **pt/** Copy of Protothreads library from Adam Dunkels. For details, see pt/ or http://dunkels.com/adam/pt/ The library can be used with LCD_HD44780 library.

  
        
