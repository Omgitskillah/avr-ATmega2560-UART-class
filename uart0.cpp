#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/** USART0 **/
static volatile uint8_t UART0_TxBuffer[UART_TX0_BUFFER_SIZE];
static volatile uint8_t UART0_RxBuffer[UART_RX0_BUFFER_SIZE];
/* Buffer size must not exceed 256 bytes */

/* uart0 instance */
uart uart0 = uart( UART0_TxBuffer,
                   UART0_RxBuffer,
                   &UBRR0H,
                   &UBRR0L,
                   &UCSR0C,
                   &UCSR0A,
                   &UCSR0B,
                   &UDR0,
                   UCSZ00,
                   U2X0,
                   RXCIE0,
                   RXEN0,
                   UDRIE0,
                   TXEN0,
                   FE0,
                   DOR0,
                   (uint8_t)UART_RX0_BUFFER_SIZE,
                   (uint8_t)UART_TX0_BUFFER_SIZE );

/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
ISR(USART0_RX_vect)
{
    uart0.rx_handler();
}

/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
ISR(USART0_UDRE_vect)
{
    uart0.tx_handler();
}
