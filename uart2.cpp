#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/** USART2 **/
static volatile uint8_t UART2_TxBuffer[UART_TX2_BUFFER_SIZE];
static volatile uint8_t UART2_RxBuffer[UART_RX2_BUFFER_SIZE];
/* Buffer size must not exceed 256 bytes */

/* uart2 instance */
uart uart2 = uart( UART2_TxBuffer,
                   UART2_RxBuffer,
                   &UBRR2H,
                   &UBRR2L,
                   &UCSR2C,
                   &UCSR2A,
                   &UCSR2B,
                   &UDR2,
                   UCSZ20,
                   U2X2,
                   RXCIE2,
                   RXEN2,
                   UDRIE2,
                   TXEN2,
                   FE2,
                   DOR2,
                   (uint8_t)UART_RX2_BUFFER_SIZE,
                   (uint8_t)UART_TX2_BUFFER_SIZE );

/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
ISR(USART2_RX_vect)
{
    uart2.rx_handler();
}

/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
ISR(USART2_UDRE_vect)
{
    uart2.tx_handler();
}
