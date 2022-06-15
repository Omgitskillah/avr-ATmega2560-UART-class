#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/**
 * @brief Construct a new uart::uart object
 * 
 * @param _TxBuffer       - Transmit buffer
 * @param _RxBuffer       - Receive Buffer
 * @param _ubrro_h        
 * @param _ubrro_l 
 * @param _ucsrc 
 * @param _uart_status 
 * @param _uart_control 
 * @param _data_reg 
 * @param _ucsz_0 
 * @param _uart_speed 
 * @param _rxcie 
 * @param _rxen 
 * @param _udrie 
 * @param _txen 
 * @param _fe 
 * @param _dor 
 * @param _rx_buffer_size 
 * @param _tx_buffer_size 
 */
uart::uart( volatile uint8_t * _TxBuffer,
            volatile uint8_t * _RxBuffer,
            volatile uint8_t * _ubrro_h,
            volatile uint8_t * _ubrro_l,
            volatile uint8_t * _ucsrc,
            volatile uint8_t * _uart_status,
            volatile uint8_t * _uart_control,
            volatile uint8_t * _data_reg,
            uint8_t _ucsz_0,
            uint8_t _uart_speed,
            uint8_t _rxcie,
            uint8_t _rxen,
            uint8_t _udrie,
            uint8_t _txen,
            uint8_t _fe,
            uint8_t _dor,
            uint8_t _rx_buffer_size,
            uint8_t _tx_buffer_size )
{
  TxBuffer        = _TxBuffer;
  RxBuffer        = _RxBuffer;
  ubrro_h         = _ubrro_h;
  ubrro_l         = _ubrro_l;
  ucsrc           = _ucsrc;
  uart_status     = _uart_status;
  uart_control    = _uart_control;
  data_reg        = _data_reg;
  ucsz_0          =_ucsz_0;
  uart_speed      = _uart_speed;
  rxcie           = _rxcie;
  rxen            = _rxen;
  udrie           = _udrie;
  txen            = _txen;
  fe              = _fe;
  dor             = _dor;
  rx_buffer_size  = _rx_buffer_size;
  tx_buffer_size  = _tx_buffer_size;
}

/**
 * @brief init( uint16_t baudrate )
 * initialize UART and set baudrate
 * @param baudrate baudrate e.g 9600
 */
void uart::init(uint16_t baudrate)
{
  /* Define F_CPU frequency in Hz in Makefile or toolchain compiler configuration */
  uint16_t baud_select = (((F_CPU) + 8UL * (baudrate)) / (16UL * (baudrate)) - 1UL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    UART_TxHead = 0;
    UART_TxTail = 0;
    UART_RxHead = 0;
    UART_RxTail = 0;
  }

  /* Set baud rate */
  if (baud_select & 0x8000)
  {
    *uart_status = (1 << uart_speed); // Enable 2x speed
    baud_select &= ~0x8000;
  }

  *ubrro_h = (uint8_t)(baud_select >> 8);
  *ubrro_l = (uint8_t)baud_select;

  /* Enable USART receiver and transmitter and receive complete interrupt */
  *uart_control = _BV(rxcie) | (1 << rxen) | (1 << txen);

  /* Set frame format: asynchronous, 8data, no parity, 1stop bit */
  // * ucsrc = (1<<URSEL0)|(3<<UCSZ00); // in case URSEL0 is defined
  *ucsrc = (3 << ucsz_0);
}

/**
 * @brief process_rx_buffer( uint8_t * data, bool pop )
 * Returns next byte without popping it from queue
 * @param data data buffer to read into
 * @param pop pass in true to remove byte from queue, false to keep it
 * @return uart_error_t
 */
uart_error_t uart::process_rx_buffer(uint8_t& data, bool pop)
{
  uint16_t tmptail;
  uart_error_t ret = UART_OK;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (UART_RxHead == UART_RxTail)
    {
      ret = NO_DATA; /* no data available */
    }
  }

  tmptail = (UART_RxTail + 1) & (rx_buffer_size - 1);

  if (true == pop)
  {
    UART_RxTail = tmptail;
  }

  /* get data from receive buffer */
  data = RxBuffer[tmptail];

  return ret;
} /* process_rx_buffer */

/**
 * @brief read( uint8_t * data )
 *  read byte from ringbuffer
 * @param data, pointer to a character buffer
 * @return uart_error_t NO_DATA if there is no received byte
 */
uart_error_t uart::read(uint8_t& data)
{
  return process_rx_buffer(data, true);
} /*read */

/**
 * @brief peek( uint8_t * data )
 * Returns next byte without popping it from queue
 * @param data, data buffer to read into
 * @return uart_error_t, NO_DATA if no byte in queue
 */
uart_error_t uart::peek(uint8_t& data)
{
  return process_rx_buffer(data, false);
} /* peek */

/**
 * @brief write(uint8_t data)
 *
 * @param data byte to send
 * @return uart_error_t BUFFER_OVERFLOW if TX buffer is full
 */
uart_error_t uart::write(uint8_t data)
{
  uart_error_t ret = UART_OK;
  uint16_t tmphead;

  tmphead = (UART_TxHead + 1) & (tx_buffer_size - 1);

  if (tmphead == UART_TxTail)
  {
    ret = BUFFER_OVERFLOW;
  }
  else
  {
    TxBuffer[tmphead] = data;
    UART_TxHead = tmphead;
    *uart_control |= _BV(udrie);
  }

  return ret;
} /* write */

/**
 * @brief rx_handler(void)
 * rx_handler for the uart
 */
void uart::rx_handler(void)
{
  uint16_t tmphead;
  uint8_t data;
  uint8_t usr;
  uint8_t lastRxError;

  /* read UART status register and UART data register */

  usr = *uart_status;
  data = *data_reg;

  /* Possibly prcess these errors differently at a system level */
  lastRxError = (usr & (_BV(fe) | _BV(dor))); 

  /* calculate buffer index */
  tmphead = (UART_RxHead + 1) & (rx_buffer_size - 1);

  if (tmphead == UART_RxTail)
  {
    /* error: receive buffer overflow */
    lastRxError = UART_BUFFER_OVERFLOW >> 8;
  }
  else
  {
    /* store new index */
    UART_RxHead = tmphead;
    /* store received data in buffer */
    RxBuffer[tmphead] = data;
  }

  UART_LastRxError = lastRxError;
}


/**
 * @brief tx_handler(void)
 * tx_handler for the uart
 */
void uart::tx_handler(void)
{
  uint16_t tmptail;

  if (UART_TxHead != UART_TxTail)
  {
    /* calculate and store new buffer index */
    tmptail = (UART_TxTail + 1) & (tx_buffer_size - 1);
    UART_TxTail = tmptail;
    /* get one byte from buffer and write it to UART */
    *data_reg = TxBuffer[tmptail]; /* start transmission */
  }
  else
  {
    /* tx buffer empty, disable UDRE interrupt */
    *uart_control &= ~_BV(udrie);
  }
}
