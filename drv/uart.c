/*
 * uart.c
 *
 * Created: 4/30/2014 10:15:10 PM
 *  Author: John
 */ 
#include "drivers.h"
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

/********************************************************************************
Function Prototypes
********************************************************************************/
char usart_getchar( void );
void usart_putchar( char data );
void usart_pstr (char *s);
unsigned char usart_kbhit(void);
int usart_putchar_printf(char var, FILE *stream);
int usart_getchar_scanf(FILE *stream);

/********************************************************************************
Global Variables
********************************************************************************/

/*
*
*	Set the standard I/O stream to be used in the project
*
*/
static FILE mystdio = FDEV_SETUP_STREAM(usart_putchar_printf, usart_getchar_scanf, _FDEV_SETUP_RW);


/*-------------------- Init_Uart      -------------------------
*    Function:    Init_Uart
*    Purpose:    Initialize serial connectivity @115200 baud.
*
*    Parameters:    none
*    Returns:    none
*------------------------------------------------------------*/
void Init_Uart(void){
	cli();
	// Enable U2Xn to get a baud rate with less error margin
	//UCSR0A = (1<<U2X0);
	// Transmitter enable and Receiver enable
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	// Asynchronous USART | No parity | 1 stopbit | CH size 8-bit
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01) | (1<<USBS0);
	// 115200 Baudrate @ 0.9216 Mhz
	UBRR0L = 0x35; //9600 @ 12.5 Mhz
	sei();
	
	// setup our stdio stream
	stdin=stdout= &mystdio;
}

/*-------------------- USART_Transmit   -------------------------
*    Function:    USART_Transmit
*    Purpose:    Transmit a byte through UART.
*
*    Parameters:
*        data - data to be sent
*    Returns: none
*------------------------------------------------------------*/
void USART_Transmit(uint8_t data){
	/* Wait for empty transmit buffer */
	//    while(!(UCSR0A & (1<<UDRE0)));  // Make sure that the data register is empty before putting the device to sleep
	//        ;
	/* Put data into buffer, sends the data */
	UDR0 = data;
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1<<UDRE0)));  // Make sure that the data register is empty before putting the device to sleep
	;
}

/*-------------------- usart_putchar   -------------------------
*    Function:    usart_putchar
*    Purpose:    Transmit a byte through UART.
*
*    Parameters:
*        data - data to be sent
*    Returns: none
*------------------------------------------------------------*/
void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while(!(UCSR0A & (1<<UDRE0)));
	// Start transmission
	UDR0 = data;
}

/*-------------------- usart_pstr   -------------------------
*    Function:    usart_pstr
*    Purpose:    Transmit a string through UART.
*
*    Parameters:
*        s - Pointer to the data to be sent
*    Returns: none
*------------------------------------------------------------*/
void usart_pstr(char *s) {
	// loop through entire string
	while (*s) {
		usart_putchar(*s);
		s++;
	}
}

/*-------------------- USART_Receive   -------------------------
*    Function:    USART_Receive
*    Purpose:    Receive a byte from UART.
*
*    Parameters:
*        data - data pointer to save data to
*    Returns: none
*------------------------------------------------------------*/
uint8_t USART_Receive(uint8_t * data)
{
	/* Wait for data to be received */
	if(UCSR0A & (1<<RXC0)){
		/* Get and return received data from buffer */
		*data = UDR0;
		return 1;
	}else
	return 0;
}

/*-------------------- usart_getchar   -------------------------
*    Function:    usart_getchar
*    Purpose:    Receive a byte from UART.
*
*    Parameters:
*        none
*    Returns: data byte receiver through UART
*------------------------------------------------------------*/
char usart_getchar(void) {
	char rc;
	// Wait for incoming data
	while (!(UCSR0A & (1<<RXC0)));
	rc = UDR0;
	usart_putchar(rc);
	// Return the data
	return rc;
}

/*-------------------- usart_kbhit   -------------------------
*    Function:    usart_kbhit
*    Purpose:    Tell if a character is waiting to be read on UART.
*
*    Parameters:
*        none
*    Returns: 
*		1 - Character is waiting to be read
*		0 - no data is waiting on UART
*------------------------------------------------------------*/
unsigned char usart_kbhit(void) {
	//return nonzero if char waiting polled version
	unsigned char b;
	b=0;
	if(UCSR0A & (1<<RXC0)) b=1;
	return b;
}


/*-------------------- usart_getchar_scanf   -------------------------
*    Function:    usart_getchar_scanf
*    Purpose:    Reads a character on the UART.
*				- each character read is echoed back
*    Parameters:
*        I/O Stream
*    Returns:
*		The character data from the UART RX
*------------------------------------------------------------*/
int usart_getchar_scanf(FILE *stream)
{
	uint8_t u8Data;
	// Wait for byte to be received
	while(!(UCSR0A&(1<<RXC0))){};
	u8Data=UDR0;
	//echo input data
	usart_putchar_printf(u8Data,stream);
	// Return received data
	return (int)u8Data;
}

/*-------------------- USART_print   -------------------------
*    Function:    USART_print
*    Purpose:    Print a string to UART.
*
*    Parameters:
*        text - pointer to byte array (string)
*    Returns: none
*------------------------------------------------------------*/
void USART_print(char * text){
	
	uint8_t text_size=strlen(text);
	uint8_t i;
	
	for(i=0;i<text_size;i++){
		USART_Transmit(text[i]);
	}
	USART_Transmit(0x0D);
	USART_Transmit(0x0A);
}


/*-------------------- usart_putchar_printf   -------------------------
*    Function:    usart_putchar_printf
*    Purpose:    Sends a character through a stream.
*
*    Parameters:
*        var - character to be transmited
*		stream - pointer to the I/O stream
*    Returns: 
*		0 - always error free
*------------------------------------------------------------*/
int usart_putchar_printf(char var, FILE *stream) {
	// translate \n to \r for br@y++ terminal
	if (var == '\n') usart_putchar('\r');
	usart_putchar(var);
	return 0;
}


