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


/* !!! DO NOT INCLUDE THIS FILE DIRECTLY !!! */ 
/* You must define following macros before including
      USART_NUMBER
      USART_RX_BUFFER
 */

#ifdef _RX_BUFFER
  #error Macro name collision - please undef _RX_BUFFER macro
#endif
#ifdef USART
  #error Macro name collision - please under USART macro
#endif
#ifdef UART
  #error Macro name collision - please under UART macro
#endif
#ifdef H
  #error Macro name collision - please undef H macro
#endif
#ifdef L
  #error Macro name collision - please undef L macro
#endif
#ifdef A
  #error Macro name collision - please undef A macro
#endif
#ifdef B
  #error Macro name collision - please undef A macro
#endif
#ifdef V
  #error Macro name collision - please undef V macro
#endif
#ifdef _RECV
  #error Macro name collision - please undef V macro
#endif
#ifdef _DATA
  #error Macro name collision - please undef V macro
#endif



#if defined(CHR9) || defined(CHR90) 
/* UART */
  #define _AVR_UART
  #if IS_NOT_EMPTY_DEF(USART_NUMBER) 
    #if USART_NUMBER>1
      #error Only two UARTs are supported, defined USART_NUMBER > 1
    #endif  
  #endif

  #ifdef UBRRHI
    #define _UBRRH UBRRHI
  #elif defined(UBRRH)
    #define _UBRRH UBRRH
  #else 
    #error UART not supported, register value UBRRHI not defined
  #endif  
  
  #define _UBRRL CAT(UBRR, USART_NUMBER) 
  #define _CHR9 CAT(CHR9, USART_NUMBER)
#else  
/* USART */
  #undef UBRR0
  #undef UBRR1
  #undef UBRR2
  #undef UBRR3
  #define _AVR_USART
  #define _UBRRH CAT3(UBRR, USART_NUMBER, H)
  #define _UBRRL CAT3(UBRR, USART_NUMBER, L)
  #define _UCSRC CAT3(UCSR, USART_NUMBER, C)
  #define _UCSZ0 CAT3(UCSZ, USART_NUMBER, 0)
  #define _UCSZ1 CAT3(UCSZ, USART_NUMBER, 1)
  #define _UCSZ2 CAT3(UCSZ, USART_NUMBER, 2)
  #define _UPM0  CAT3(UPM, USART_NUMBER, 0)
  #define _UPM1  CAT3(UPM, USART_NUMBER, 1)
  #define _USBS  CAT(USBS, USART_NUMBER)
  #define _UMSEL CAT(UMSEL, USART_NUMBER)
  #define _UCPOL CAT(UCPOL, USART_NUMBER)
#endif

/* Common definition for UART and USART */
#define _UCSRA CAT3(UCSR, USART_NUMBER, A)
#define _UCSRB CAT3(UCSR, USART_NUMBER, B)
#define _UDR   CAT(UDR, USART_NUMBER)
#define _RXEN  CAT(RXEN, USART_NUMBER)
#define _TXEN  CAT(TXEN, USART_NUMBER)
#define _RXCIE CAT(RXCIE, USART_NUMBER)
#define _UDRE  CAT(UDRE, USART_NUMBER)
#define _U2X   CAT(U2X, USART_NUMBER)
#define _UDRIE CAT(UDRIE, USART_NUMBER)
#define _DOR CAT(DOR, USART_NUMBER)
#define _RXC CAT(RXC, USART_NUMBER)

/* Different AVR devices uses different names for interrupt vector.
 * For more information see: 
 * http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */
#if defined(UART_RX_vect) || defined(UART0_RX_vect)
  #define _UART_RX_vect   CAT3(UART, USART_NUMBER, _RX_vect)
  #define _UART_UDRE_vect CAT3(UART, USART_NUMBER, _UDRE_vect)
#elif defined(USART_RXC_vect) || defined(USART0_RXC_vect)
  #define _UART_RX_vect   CAT3(USART, USART_NUMBER, _RXC_vect)
  #define _UART_UDRE_vect CAT3(USART, USART_NUMBER, _UDRE_vect)
#elif defined(USART_RX_vect) || defined(USART0_RX_vect)
  #define _UART_RX_vect   CAT3(USART, USART_NUMBER, _RX_vect)
  #define _UART_UDRE_vect CAT3(USART, USART_NUMBER, _UDRE_vect)
#endif

#define _USART_RX_BUFFER CAT3(USART, USART_NUMBER, _RX_BUFFER)

#if CAT(URSEL, USART_NUMBER) > 0
  #define _URSEL _BV(CAT(URSEL, USART_NUMBER))
#else
  #define _URSEL 0
#endif

#define _THWUsart CAT(THWUsart, USART_NUMBER)

/* Name of global variable with USART state */ 
#define _global_hwusart CAT(_global_hwusart, USART_NUMBER)

/* Multiple USARTs is supported. Unique functions for the given
 * UART number must be defined. Macro `_usart_function` is helper
 * macro function, which create function definition in the following format:
 *    usart<USART_NUMBER>_<name>(arg0, arg1, ...)
 * USART number `0` is everytimes defined without number i.e.: usart_<name>(arguments ...)
 */
#if IS_NOT_EMPTY_DEF(USART_NUMBER)
  #if USART_NUMBER==0 
    /* Multiple UARTs - UART0 settings */
    #define _usart_function(name, ...) \
      CAT(usart, _ ## name)(__VA_ARGS__)
  #else
    /* Multiple UARTs - UART number USART_NUMBER */
    #define _usart_function(name, ...) \
      CAT3(usart, USART_NUMBER, _ ## name)(__VA_ARGS__)
  #endif
#else
    /* Single USART */
    #define _usart_function(name, ...) \
      CAT(usart, _ ## name)(__VA_ARGS__)
#endif

#if (_USART_RX_BUFFER!=0)   &&                                \
    (_USART_RX_BUFFER!=2)   && (_USART_RX_BUFFER!=4)   &&     \
    (_USART_RX_BUFFER!=8)   && (_USART_RX_BUFFER!=16)  &&     \
    (_USART_RX_BUFFER!=32)  && (_USART_RX_BUFFER!=64)  &&     \
    (_USART_RX_BUFFER!=128) && (_USART_RX_BUFFER!=256) 
  #error USART_RX_BUFFER must be power of two !
#endif

/*****************************************************************************
  STATIC DECLARATION - included in serial.h
 *****************************************************************************/

typedef struct {
  #if _USART_RX_BUFFER > 0
    char rx_buffer[_USART_RX_BUFFER];
    uint8_t rx_read_pos;  // read position in rx_buffer
    uint8_t rx_write_pos; // write position in rx_buffer 
    /* This flag bit is set when there are unread data in the 
     * receive buffer and cleared when the receive buffer is empty
     * (i.e. does not contain any unread data).
     */
    uint8_t receive_complete : 1;

    /* A Data OverRun occurs when the receive buffer is full.
     * This flag is valid until the receive buffer (getchar) is read.
     */
    uint8_t overrun : 1;
  #endif  
  
  /* At the beginning of the transmission we setup tx_data to point
   * to transmitted data and we set tx_length to length of transmitted data
   * During the transmission, when one character is sended the tx_data pointer
   * is increased by one and tx_length is decreased by one.
   * The transmission is finished when tx_length reach zero.
   */
  #ifndef _USART_TX_ISR_DISABLE  
  const char * tx_data;    // Pointer to TX - buffer
  uint8_t tx_data_pgm  : 1;    // True - if tx_data is pointer to pgm_space
  uint8_t tx_data_text : 1;    // True - if tx_data is pointer to null terminated text data
  uint8_t tx_length;
  #endif
} _THWUsart;

extern _THWUsart _global_hwusart; 

extern void _usart_function(tx_wait, void);
#ifndef _USART_TX_ISR_DISABLE
extern uint8_t _usart_function(tx_empty, void);
extern void _usart_function(putchar, char ch); 
extern void _usart_function(send, const char* data, uint8_t len);
extern void _usart_function(send_P, const char* data, uint8_t len);
extern void _usart_function(print, const char * text);
extern void _usart_function(print_P, const char * text);
#endif

/* Blocking function - wait to finish transmission */
extern void _usart_function(bputchar, char ch);
extern void _usart_function(bsend, const char* data, uint8_t len);
extern void _usart_function(bsend_P, const char* data, uint8_t len);
extern void _usart_function(bprint, const char * text);
extern void _usart_function(bprint_P, const char * text);

#if _USART_RX_BUFFER>0
extern uint8_t _usart_function(getchar, void);
extern uint8_t _usart_function(available,void); 
/* Return True (non-zero value) if buffer starts with 'text'.
   The function don't change buffer content. 
   Attention ! Buffer size must be sufficient for storage 'text' data.
   'text' is zero terminated string
 */
extern uint8_t _usart_function(startswith, const char * text);
extern uint8_t _usart_function(startswith_P, const char * text);

/* Return True (non-zero value) if the buffer starts with 'text'.
   Function read 'text' data from the buffer 
   Attention ! Buffer size must be sufficient for storage 'text' data.
   'text' is zero terminated string
 */  
extern uint8_t _usart_function(readstr, const char * text);
extern uint8_t _usart_function(readstr_P, const char * text);

/* 
   Copy characters from buffer the first num characters of buffer to destination.
   If the end of the buffer is reached before num characters have been copied,
   destination is padded with zeros until a total of num characters have been
   written to it.

   No null-character is implicitly appended at the end of destination if buffer is
   longer than num. Thus, in this case, destination shall not be considered a
   null terminated C string (reading it as such would overflow).

   Return number of written characters to destination
 */
extern uint8_t _usart_function(readn, char * destination, uint8_t num);

/* Clear receive buffer and flags: overrun, receive_complete */
extern void _usart_function(clear, void);
#endif

/* List of external functions by this module */

static inline void _usart_function(init, 
      unsigned long baud, 
      uint8_t data_size, uint8_t parity, uint8_t stopbits
  ) {
  uint8_t use_u2x;
  uint16_t ubrr = F_CPU / 16 / baud - 1;
  uint16_t ubrr_u2x = F_CPU / 8 / baud - 1;

  // U2X mode is needed for baud rates higher than (CPU Hz / 16)
  if (baud > F_CPU / 16UL) {
    use_u2x = 1;
  } else {
    use_u2x = abs(ubrr-baud) > abs(ubrr_u2x-baud);
  }

  _UCSRA = 0;
  _UCSRB = 0;
  _UCSRC = 0;
  
  #if defined(_AVR_USART) 
    if (use_u2x) {
      _UCSRA = _BV(_U2X);
      _UBRRH = ubrr_u2x >> 8;
      _UBRRL = ubrr_u2x;
    } else {
      _UBRRH = ubrr >> 8;
      _UBRRL = ubrr;
    }

    switch (data_size) {
      case 5: 
        break;
      case 6: 
        _UCSRC |= _BV(_UCSZ0) | _URSEL; 
        break;
      case 7: 
        _UCSRC |= _BV(_UCSZ1) | _URSEL;
        break;
      case 8: 
        _UCSRC |= _BV(_UCSZ1) | _BV(_UCSZ0) | _URSEL;
        break;
      case 9: 
        _UCSRC |= _BV(_UCSZ1) | _BV(_UCSZ0) | _URSEL;
        _UCSRB |= _BV(_UCSZ2);
        break;
    }

    switch (parity) {
      case UARTS_PARITY_NONE:   //None parity
        break;
      case UARTS_PARITY_EVEN:
        _UCSRC |= _BV(_UPM1) | _URSEL;
        break;
      case UARTS_PARITY_ODD:
        _UCSRC |= _BV(_UPM1) | _BV(_UPM0) | _URSEL;
        break;
    }
  
    if (stopbits == 2) {
      _UCSRC |= _BV(_USBS) | _URSEL; 
    }

  #elif defined(_AVR_UART)
    /* UART support only 8bit or 9 Bits data */
    if (use_u2x) {
      _UCSRA = _BV(_U2X);
      _UBRRL = ubrr_u2x;
       #if IS_EMPTY_DEF(USART_NUMBER) 
         _UBRRH &= 0xF0;
         _UBRRH |= (ubrr_u2x >> 8) & 0x0F;
       #elif USART_NUMBER==0
         _UBRRH &= 0xF0;
         _UBRRH |= (ubrr_u2x >> 8) & 0x0F;
       #elif USART_NUMBER==1
         _UBRRH &= 0x0F;
         _UBRRH |= (ubrr_u2x >> 4) & 0xF0;
       #else
         #error Not supported UART number
       #endif 
    } else {
       _UBRRL = ubrr;
       #if IS_EMPTY_DEF(USART_NUMBER) 
         _UBRRH &= 0xF0;
         _UBRRH |= (ubrr >> 8) & 0x0F;
       #elif USART_NUMBER==0
         _UBRRH &= 0xF0;
         _UBRRH |= (ubrr >> 8) & 0x0F;
       #elif USART_NUMBER==1
         _UBRRH &= 0x0F;
         _UBRRH |= (ubrr >> 4) & 0xF0;
       #else
         #error Not supported UART number
       #endif 
    }
    
    switch (data_size) {
      case 9: 
        _UCSRB |= _BV(_CHR9);
        break;
    }
  #else
    #error UART or USART not defined. Maybe the device does not support.
  #endif
  
  /* UART global variable initialization */
  #if _USART_RX_BUFFER>0
    _usart_function(clear);
  #endif
  #ifndef _USART_TX_ISR_DISABLE   
    _global_hwusart.tx_data_text=0;
    _global_hwusart.tx_length=0;
  #endif

  _UCSRB |= _BV(_RXEN) |          // Enable RX 
            _BV(_TXEN)            // Enable TX 
    #if _USART_RX_BUFFER>0  
            | _BV(_RXCIE)         // Enable Interrupt
    #endif 
            ;
}


/*****************************************************************************
                               _USART_RX_BUFFER > 0
 *****************************************************************************/
#if _USART_RX_BUFFER>0
static inline uint8_t _usart_function(overrun, void) 
{
   return _global_hwusart.overrun;
}

#else
/*****************************************************************************
                               _USART_RX_BUFFER == 0
 *****************************************************************************/

static inline uint8_t _usart_function(available,void) 
{
  if (_UCSRA & _BV(_RXC)) {
    return 1;
  }
  return 0;
}

static inline uint8_t _usart_function(getchar, void) 
{
   if (_usart_function(available)) {
    return _UDR;
   } 
   return '\0';
}

static inline uint8_t _usart_function(overrun, void) 
{
   return _UCSRA & _BV(_DOR);
}
#endif
/************************** _USART_RX_BUFFER == 0 ****************************/

#ifdef _USART_TX_ISR_DISABLE
static inline uint8_t _usart_function(tx_empty, void)
{
   return _UCSRA & _BV(_UDRE);
}
#endif


#ifdef HWUSART_IMPLEMENTATION 
/*****************************************************************************
  IMPEMENTATION - INCLUDED IN serial.c
 *****************************************************************************/

_THWUsart _global_hwusart;  /* Global variable with USART state */

/*****************************************************************************
                               _USART_RX_BUFFER > 0
 *****************************************************************************/
#if _USART_RX_BUFFER>0

ISR (_UART_RX_vect)
{
  if ( (_global_hwusart.receive_complete) &&   
       (_global_hwusart.rx_read_pos == _global_hwusart.rx_write_pos) ) 
  {
    _global_hwusart.overrun=1;
    _global_hwusart.rx_read_pos = _global_hwusart.rx_write_pos + 1;
    _global_hwusart.rx_read_pos &= (_USART_RX_BUFFER-1);    /* RX_BUFFER must be power of two ! */
  }
  _global_hwusart.rx_buffer[_global_hwusart.rx_write_pos] = _UDR;
  _global_hwusart.rx_write_pos++;
  _global_hwusart.rx_write_pos &= (_USART_RX_BUFFER-1);    /* RX_BUFFER must be power of two ! */
  _global_hwusart.receive_complete=1;
}

uint8_t _usart_function(available,void) 
{
  uint8_t len;
  
  if (_global_hwusart.receive_complete) {
    len = _global_hwusart.rx_write_pos - _global_hwusart.rx_read_pos;
    if (len == 0) {
      return _USART_RX_BUFFER;
    }
    return len & (_USART_RX_BUFFER-1);
  }
  return 0;
}

uint8_t _usart_function(getchar, void) 
{
  char ch;
  if (_usart_function(available)) {
      ch = _global_hwusart.rx_buffer[_global_hwusart.rx_read_pos++];
      _global_hwusart.rx_read_pos &= (_USART_RX_BUFFER-1);
      _global_hwusart.overrun=0; //Clear overrun flag

      if (_global_hwusart.rx_read_pos==_global_hwusart.rx_write_pos) {
        _global_hwusart.receive_complete=0;
      }
      return ch;
  }
  return '\0';
}

uint8_t _usart_function(startswith, const char * text)
{
  uint8_t buffer_index;
  uint8_t buffer_end = 0;

  if (_usart_function(available)) {
    buffer_index = _global_hwusart.rx_read_pos;
    while ( (*text != '\0') && (!buffer_end) ) 
    {
      if ( *(text++) != _global_hwusart.rx_buffer[buffer_index++] ) {
        return 0; // False
      }
      buffer_index &= (_USART_RX_BUFFER-1);

      if (buffer_index == _global_hwusart.rx_write_pos) {
        buffer_end = 1;
      }
    }
    return (*text == '\0');
  }
    return 0;
}

uint8_t _usart_function(startswith_P, const char * text_P)
{
  uint8_t buffer_index;
  uint8_t buffer_end = 0;
  char ch;

  if (_usart_function(available)) {
    buffer_index = _global_hwusart.rx_read_pos;
    ch = pgm_read_byte(text_P++);
    while ( (ch != '\0') && (!buffer_end) )  
    {
      if ( ch != _global_hwusart.rx_buffer[buffer_index++] ) {
        return 0; // False
      }
      buffer_index &= (_USART_RX_BUFFER-1);

      if (buffer_index == _global_hwusart.rx_write_pos) {
        buffer_end = 1;
      }
      ch = pgm_read_byte(text_P++);
    }
    return (ch == '\0');
  }
    return 0;
}

uint8_t _usart_function(readstr, const char * text)
{
  if (_usart_function(startswith, text)) {
    while ( *(text++) != '\0' ) {
      _usart_function(getchar);
    }
    return 1;
  }
  return 0;
}

uint8_t _usart_function(readstr_P, const char * text_P)
{
  if (_usart_function(startswith_P, text_P)) {
    while ( pgm_read_byte(text_P++) != '\0' ) {
      _usart_function(getchar);
    }
    return 1;
  }
  return 0;
}

uint8_t _usart_function(readn, char * destination, uint8_t num)
{
  uint8_t buf_len, result;
  
  result = buf_len = _usart_function(available);
  while ((buf_len > 0) && (num > 0)) {
    *(destination++) = _usart_function(getchar);
    buf_len--;
    num--;
  }
  while (num > 0) {
    *(destination++) = '\0';
    num--;
  }
  return result;
}

void _usart_function(clear, void)
{
  _global_hwusart.rx_read_pos=0;
  _global_hwusart.rx_write_pos=0;
  _global_hwusart.overrun=0;
  _global_hwusart.receive_complete=0;
}

#else
/*****************************************************************************
                               _USART_RX_BUFFER == 0
 *****************************************************************************/
#endif //_USART_RX_BUFFER

/*****************************************************************************
                            ifndef _USART_TX_ISR_DISABLE 
 *****************************************************************************/

#ifndef _USART_TX_ISR_DISABLE
ISR (_UART_UDRE_vect)
{
  char ch;
  if (! _usart_function(tx_empty)) {

    if (_global_hwusart.tx_data_pgm) {
      ch = pgm_read_byte(_global_hwusart.tx_data++);
    } else {
      ch = *(_global_hwusart.tx_data++);
    }

    _UDR = ch;
    _global_hwusart.tx_length--;
  } else {
   _UCSRB &= ~_BV(_UDRIE);  /* No data - Disable TX interrupt */
  }
  return;
}


uint8_t _usart_function(tx_empty, void)
{
  if (_global_hwusart.tx_data_text) {
    char ch;
    if (_global_hwusart.tx_data_pgm) {
      ch = pgm_read_byte(_global_hwusart.tx_data);
    } else {
      ch = * _global_hwusart.tx_data;
    }
    if (ch == '\0') {
      return _UCSRA & _BV(_UDRE);
    }
  } else {
    if (_global_hwusart.tx_length == 0) {
      return _UCSRA & _BV(_UDRE);
    }
  }
  return 0;
}


void _usart_function(putchar, char ch) 
{
  _global_hwusart.tx_length = 0;
  _global_hwusart.tx_data_text = 0;
  _UDR = ch; 
}

void _usart_function(send, const char* data, uint8_t len)
{
   if (len > 0) {
     _global_hwusart.tx_data = data;
     _global_hwusart.tx_length = len;
     _global_hwusart.tx_data_pgm = 0;
     _global_hwusart.tx_data_text = 0;  // Send binary data with tx_length
     _UCSRB |= _BV(_UDRIE);
   }
}

void _usart_function(send_P, const char* data, uint8_t len)
{
   if (len > 0) {
     _global_hwusart.tx_data = data;
     _global_hwusart.tx_length = len;
     _global_hwusart.tx_data_pgm = 1;
     _global_hwusart.tx_data_text = 0; // Send binary data with tx_length
     _UCSRB |= _BV(_UDRIE);
   }
}

void _usart_function(print, const char * text)
{
   _global_hwusart.tx_data = text;
   _global_hwusart.tx_data_pgm = 0;
   _global_hwusart.tx_data_text = 1; // Zero terminated text string
   _UCSRB |= _BV(_UDRIE);
}

void _usart_function(print_P, const char * text)
{
   _global_hwusart.tx_data = text;
   _global_hwusart.tx_data_pgm = 1;
   _global_hwusart.tx_data_text = 1; // Zero terminated text string
   _UCSRB |= _BV(_UDRIE);
}
#endif // ifndef _USART_TX_ISR_DISABLE 


/* Blocking function - wait to finish transmission */
void _usart_function(tx_wait, void)
{
  while (! _usart_function(tx_empty)) 
    ;
}

void _usart_function(bputchar, char ch)
{
   _UDR = ch;
   _usart_function(tx_wait);
}

void _usart_function(bsend, const char* data, uint8_t len)
{
  for(uint8_t i=0; i<len; i++) {
    _usart_function(bputchar, *(data++) );
  }
}

void _usart_function(bsend_P, const char* data, uint8_t len)
{
  for(uint8_t i=0; i<len; i++) {
    _usart_function(bputchar, pgm_read_byte(data++) );
  }
}

void _usart_function(bprint, const char * text)
{
  char ch;
  while ( (ch = *(text++)) != '\0' ) {
    _usart_function(bputchar, ch);
  }
}

void _usart_function(bprint_P, const char * text)
{
  char ch;
  while ( (ch = pgm_read_byte(text++)) != '\0' ) {
    _usart_function(bputchar, ch);
  }
}

#endif


#undef _AVR_UART
#undef _AVR_USART
#undef _UBRRH
#undef _UBRR
#undef _UCSRA
#undef _UCSRB
#undef _UDR 
#undef _RXEN
#undef _TXEN
#undef _RXCIE
#undef _UDRE
#undef _U2X
#undef _USART_RX_BUFFER
#undef _URSEL
#undef _usart_function
#undef _UCSZ0
#undef _UCSZ1
#undef _UCSZ2
#undef _UPM0 
#undef _UPM1 
#undef _USBS 
#undef _UMSEL 
#undef _UCPOL
#undef _UDRIE
#undef _CHR9
#undef _DOR
#undef _RXC
#undef _UART_RX_vect
#undef _UART_UDRE_vect
#undef _THWUsart 
#undef _global_hwusart 
