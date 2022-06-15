#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/** USART1 **/
static volatile uint8_t UART1_TxBuffer[UART_TX1_BUFFER_SIZE];
static volatile uint8_t UART1_RxBuffer[UART_RX1_BUFFER_SIZE];
/* Buffer size must not exceed 256 bytes */

/* uart1 instance */
uart uart1 = uart( UART1_TxBuffer,
                   UART1_RxBuffer,
                   &UBRR1H,
                   &UBRR1L,
                   &UCSR1C,
                   &UCSR1A,
                   &UCSR1B,
                   &UDR1,
                   UCSZ10,
                   U2X1,
                   RXCIE1,
                   RXEN1,
                   UDRIE1,
                   TXEN1,
                   FE1,
                   DOR1,
                   (uint8_t)UART_RX1_BUFFER_SIZE,
                   (uint8_t)UART_TX1_BUFFER_SIZE );

/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
ISR(USART1_RX_vect)
{
    uart1.rx_handler();
}

/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
ISR(USART1_UDRE_vect)
{
    uart1.tx_handler();
}
