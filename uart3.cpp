#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/** USART3 **/
static volatile uint8_t UART3_TxBuffer[UART_TX3_BUFFER_SIZE];
static volatile uint8_t UART3_RxBuffer[UART_RX3_BUFFER_SIZE];
/* Buffer size must not exceed 256 bytes */

/* uart3 instance */
uart uart3 = uart( UART3_TxBuffer,
                   UART3_RxBuffer,
                   &UBRR3H,
                   &UBRR3L,
                   &UCSR3C,
                   &UCSR3A,
                   &UCSR3B,
                   &UDR3,
                   UCSZ30,
                   U2X3,
                   RXCIE3,
                   RXEN3,
                   UDRIE3,
                   TXEN3,
                   FE3,
                   DOR3,
                   (uint8_t)UART_RX3_BUFFER_SIZE,
                   (uint8_t)UART_TX3_BUFFER_SIZE );

/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
ISR(USART3_RX_vect)
{
    uart3.rx_handler();
}

/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
ISR(USART3_UDRE_vect)
{
    uart3.tx_handler();
}
