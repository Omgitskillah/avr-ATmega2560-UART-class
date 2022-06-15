#ifndef _UART_
#define _UART_

/**< Size of the circular receive buffer, must be power of 2 */
#define UART_RX0_BUFFER_SIZE 8 
#define UART_RX1_BUFFER_SIZE 8 
#define UART_RX2_BUFFER_SIZE 8 
#define UART_RX3_BUFFER_SIZE 8 
/**< Size of the circular transmit buffer, must be power of 2 */
#define UART_TX0_BUFFER_SIZE 64
#define UART_TX1_BUFFER_SIZE 64
#define UART_TX2_BUFFER_SIZE 64
#define UART_TX3_BUFFER_SIZE 64

#define UART_FRAME_ERROR      0x0800 /**< Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400 /**< Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200 /**< receive ringbuffer overflow */
#define UART_NO_DATA          0x0100 /**< no receive data available   */

typedef enum uart_error
{
    UART_OK,
    NO_DATA,
    BUFFER_OVERFLOW,
    OVERUN,
    FRAME_ERROR
} uart_error_t;

class uart
{
    public:
        uart( volatile uint8_t * _TxBuffer,
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
              uint8_t _tx_buffer_size );

        void init( uint16_t baudrate );
        uart_error_t read( uint8_t& data );
        uart_error_t peek( uint8_t& data );
        uart_error_t write(uint8_t data);
        void rx_handler( void );
        void tx_handler( void );
    private:
        volatile uint8_t UART_TxHead;
        volatile uint8_t UART_TxTail;
        volatile uint8_t UART_RxHead;
        volatile uint8_t UART_RxTail;
        volatile uint8_t UART_LastRxError;
        volatile uint8_t * TxBuffer;
        volatile uint8_t * RxBuffer;
        volatile uint8_t * ubrro_h;
        volatile uint8_t * ubrro_l;
        volatile uint8_t * ucsrc;
        volatile uint8_t * uart_status;
        volatile uint8_t * uart_control;
        volatile uint8_t * data_reg;
        uint8_t fe;
        uint8_t dor;
        uint8_t rxcie;
        uint8_t rxen;
        uint8_t udrie;
        uint8_t txen;
        uint8_t uart_speed;
        uint8_t ucsz_0;
        uint8_t rx_buffer_size;
        uint8_t tx_buffer_size;
        uart_error_t process_rx_buffer( uint8_t& data, bool pop );
};

extern uart uart0;
extern uart uart1;
extern uart uart2;
extern uart uart3;

#endif