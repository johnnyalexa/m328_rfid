/*
 * SPI.c
 *
 * Created: 4/30/2014 10:38:26 PM
 *  Author: John
 */ 
#include "drivers.h"


void SPI_Init(){

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