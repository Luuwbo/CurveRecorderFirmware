/*
 * DAC8554.c
 *
 * Created: 09.01.2025 18:20:17
 *  Author: uwe
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>

#include "DAC8554.h"
#include "KennLinienSchreiber.h"



void DAC8554_INIT (void)
{
	//  Offset der Kanäle definieren
	DACchanOffset[0] = 61;
	DACchanOffset[1] = 0;
	DACchanOffset[2] = 0;
	DACchanOffset[3] = 0;

	PORTD_DIRSET = (1<<3) | (1<<4);
	PORTD_OUTCLR = (1<<3);			// DAC_LDAC auf low
	PORTD_OUTSET = (1<<4);			// DAC_SYNC auf high
	_delay_us(20);
	PORTD_OUTCLR = (1<<4);			// DAC_SYNC auf low
	_delay_us(10);
	SPID_DATA = 0b00110100;			// alle Kanäle 0mV schreiben und laden
	_delay_us(serdelay);
	SPID_DATA = 0b10000000;
	_delay_us(serdelay);
	SPID_DATA = 0b00000000;
	_delay_us(serdelay);
	PORTD_OUTSET = (1<<4);			// DAC_SYNC auf high
	_delay_us(10);
}

void DAC8554_SetChan (int chan, int val)
// write to buffer and load val
{
	val = val - DACchanOffset[chan];
	PORTD_OUTCLR = (1<<4);			// DAC_SYNC to low
	_delay_us(5);
	SPID_DATA = 0b00010000 | (chan<<1);
	while(!(SPID_STATUS & SPI_IF_bm));
	SPID_DATA = (val >> 8) & 0xFF;
	while(!(SPID_STATUS & SPI_IF_bm));
	SPID_DATA = val & 0xFF;
	while(!(SPID_STATUS & SPI_IF_bm));
	PORTD_OUTSET = (1<<4);			// DAC_SYNC to high
}
void DAC8554_PreSetChan (int chan, int val)
// write to buffer
{
	val = val - DACchanOffset[chan];
	PORTD_OUTCLR = (1<<4);			// DAC_SYNC to low
	_delay_us(5);
	SPID_DATA = 0b00000000 | (chan<<1);
	while(!(SPID_STATUS & SPI_IF_bm));
	SPID_DATA = (val >> 8) & 0xFF;
	while(!(SPID_STATUS & SPI_IF_bm));
	SPID_DATA = val & 0xFF;
	while(!(SPID_STATUS & SPI_IF_bm));
	PORTD_OUTSET = (1<<4);			// DAC_SYNC to high
}
void DAC8554_LoadDataHW(void)
// write all synchronous data from buffer to output register by HW line LDAC
{
	PORTD_OUTSET = (1<<3);			// DAC_LDAC auf high
	_delay_us(2);
	PORTD_OUTCLR = (1<<3);			// DAC_LDAC auf low
}