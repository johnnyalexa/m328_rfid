/*
 * drivers.h
 *
 * Created: 4/30/2014 10:14:19 PM
 *  Author: John
 */ 


#ifndef DRIVERS_H_
#define DRIVERS_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdio.h>


#define USE_LOGGING			(0xFF)

#ifdef USE_LOGGING
#define SYS_LOG(...)	printf(__VA_ARGS__)
#else
#define SYS_LOG(...)	{}
#endif //USE_LOGGING

#define waitspi() while(!(SPSR&(1<<SPIF)))


// Reset AVR using watchdog
#define Reset_AVR() wdt_enable(WDTO_500MS); while(1) {}

void Init_Uart(void);
void USART_Transmit(uint8_t data);
uint8_t USART_Receive(uint8_t * data);
void USART_print(char * text);
unsigned char usart_kbhit(void);
char usart_getchar(void);


/********* SPI **********/
void SPI_Init();
void SPI_Write(uint8_t data);
uint8_t SPI_Read(void);
void SPI_DummyRead(void);

#endif /* DRIVERS_H_ */