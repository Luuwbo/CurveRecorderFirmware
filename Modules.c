/*
 * Modules.c
 *
 * Created: 29.01.2025 19:02:23
 *  Author: uwe
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>

#include "Modules.h"
#include "KennLinienSchreiber.h"


void MODULES_INIT (void)
{
	PORTE_DIRSET = 0b00001111;		// all ports for modules_dset as output
	PORTE_OUTCLR = 0b00001111;		// all ports for modules_dset as low
	SPIE_DATA = 0b000000000;		// alle mit 0 laden
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (0b00001111);	// Impuls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (0b00001111);
	ui8_UDVoltageRange = 0;
	ui8_UDVoltageRangeOld = 0;
	
}
void Modules_VDRANGE_SET (int8_t vdra)  //set the amplification of VD Amp
// 0 := 1 / 1 := 10 / 2 := 100
{
	switch (vdra){
	case 0:
		SPIE_DATA = 0b000000000;
		break;
	case 1:
		SPIE_DATA = 0b000000001;
		break;
	case 2:
		SPIE_DATA = 0b000000010;
		break;
	default:
		SPIE_DATA = 0b000000000;
		break;
	}
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (1<<MODULES_VD_DSET);	// Puls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<MODULES_VD_DSET);
}

void Modules_RG_SET (int8_t rgra, uint8_t chan)		//set RG
{	uint8_t rv;
	switch (rgra){
		case 0:
			rv = 0b00000000;
			break;
		case 1:
			rv = 0b00000001;
			break;	
		case 2:
			rv = 0b00000010;
			break;
		case 3:
			rv = 0b00000100;
			break;
		case 4:
			rv = 0b00001000;
			break;
		case 5:
		rv = 0b00010000;
		break;
		default:
			rv = 0b00000000;
			break;
	}
	GA_Rel_Status[chan]	&= 0b11110000;
	GA_Rel_Status[chan] |= rv;
	SPIE_DATA = GA_Rel_Status[chan];
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (1<<Modules_VGA1_DSET);	// Puls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<Modules_VGA1_DSET);
}

void Modules_UGvRange_SET (int8_t range, int8_t chan)
{	
	if (range == 0) {GA_Rel_Status[chan] &= 0b11101111;}
	else {GA_Rel_Status[chan] |= 0b00010000;}
	SPIE_DATA = GA_Rel_Status[chan];
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (1<<Modules_VGA1_DSET);	// Puls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<Modules_VGA1_DSET);
}

void Modules_UGRange_SET (int8_t range, int8_t chan)
{
	if (range == 0) {GA_Rel_Status[chan] &= 0b11011111;}
	else {GA_Rel_Status[chan] |= 0b00100000;}
	SPIE_DATA = GA_Rel_Status[chan];
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (1<<Modules_VGA1_DSET);	// Puls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<Modules_VGA1_DSET);
}

void Modules_RS_SET (int8_t rsr)		//set RS
{	uint8_t rv;
	switch (rsr){
		case 1:
		rv = 0b00000001;
		break;
		case 2:
		rv = 0b00100010;
		break;
		case 3:
		rv = 0b00100100;
		break;
		case 4:
		rv = 0b00101000;
		break;
		case 5:
		rv = 0b00110000;
		break;
		default:
		rv = 0b00000001;
		break;
	}
	IS_Rel_Status &= 0b11000000;
	IS_Rel_Status |= rv;
	SPIE_DATA = IS_Rel_Status;
	while(!(SPIE_STATUS & SPI_IF_bm));
	PORTE_OUTSET = (1<<Modules_IS_DSET);	// Puls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<Modules_IS_DSET);
}