/*
 * SPI.c
 *
 * Created: 4/30/2014 10:38:26 PM
 *  Author: John
 */ 
#include "drivers.h"


void SPI_Init(){
	/* Set MOSI and SCK output, all others input */
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	DDRB = (1<<PINB3)|(1<<PINB5);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_Write(uint8_t data){
	SPDR = data;
	waitspi();
}

void SPI_DummyRead(void){
	SPDR = 0x00;
	waitspi();	
}

uint8_t SPI_Read(void){
	return SPDR;
}