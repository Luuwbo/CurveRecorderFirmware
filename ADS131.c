/*
 * ADS131.c
 *
 * Created: 09.12.2024 11:33:41
 *  Author: uwe
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>

#include "ADS131.h" 
#include "KennLinienSchreiber.h"

void ADS131_INIT (void)
{
	PORTC_DIRSET = (1<<2) | (1<<4) ;		// Reset & CS as output
	PORTC_DIRCLR = (1<<3);					// RDY as Input 
	PORTC_OUTSET = (1<<2);					// Reset high	
	PORTC_OUTCLR = (1<<4);					// CS low
	
	_delay_us(20);
	SPIC_DATA = ADS131_command_reset;
	_delay_us(50);
	SPIC_DATA = ADS131_command_sdatac;		// stop continuous reading	
	_delay_us(serdelay);
	SPIC_DATA = 0b01000011;					// write ab Register CONFIG3
	_delay_us(serdelay);
	SPIC_DATA = 0;							// für 1 Register
	_delay_us(serdelay);
	SPIC_DATA = 0b11000000;					// internal Ref / 2V / 
	_delay_us(serdelay);
//	while(!(SPIC_STATUS & SPI_IF_bm))
	SPIC_DATA = ADS131_command_start;		// conversion start in cont mode
	_delay_us(serdelay);
//	SPIC_DATA = ADS131_command_rdatac;
//	_delay_us(serdelay);
	PORTC_OUTSET = (1<<4);					// CS high
	_delay_us(serdelay);
}

void ADS131_ChanSet(uint8_t chan, uint8_t mux)
{
	PORTC_OUTCLR = (1<<4);					// CS low
	_delay_us(serdelay);
	SPIC_DATA = 0b01000101 + chan;					// write ab Register CH0Set
	_delay_us(serdelay);
	SPIC_DATA = 0;							// für 1 Register
	_delay_us(serdelay);
	SPIC_DATA = 0b00010000 + mux;					// gain = 1
	_delay_us(serdelay);
	PORTC_OUTSET = (1<<4);					// CS high
	_delay_us(serdelay);
}

void ADS131_READDATA(void)
{	uint8_t i;
	
	PORTC_OUTCLR = (1<<4);					// CS low
//	while (PORTC_IN & (1<<3))					// nicht mit ADC_RDY auf high starten
	SPIC_DATA = ADS131_command_rdata;		// data register update
	//_delay_us(serdelay);
	while(!(SPIC_STATUS & SPI_IF_bm));
	for (i = 0; i < 19; i++)
	{
		SPIC_DATA = 0;
		//_delay_us(serdelay);
		while(!(SPIC_STATUS & SPI_IF_bm));
		ADC_data[i] = SPIC_DATA;
	}
	PORTC_OUTSET = (1<<4);					// CS high
}

uint8_t ADS131_ReadRegister(int reg)
{uint8_t regval = 0;
	PORTC_OUTCLR = (1<<4);					// CS low
	_delay_us(serdelay);
	SPIC_DATA = 0b00100000 + reg;
	_delay_us(serdelay);
	SPIC_DATA = 0;
	_delay_us(serdelay);
	SPIC_DATA = 0;
	_delay_us(serdelay);
	regval = SPIC_DATA;
	PORTC_OUTSET = (1<<4);					// CS high
	return regval;
}




