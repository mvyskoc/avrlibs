# Button library 
Library for debouncing and reading momentary contact switches. Every button use only 3 
bytes of RAM for its internal state.
 
Following functions are supported:
  * button press and release detection
  * button long press and release detection with fixed time for all buttons (default is 1 second)
  * button double click event

For more information look at the example [Buttons](../Examples/Buttons/main.c)

**Requirements:**
  - BASE library (`avrio.h`, `avrtime.h`)



