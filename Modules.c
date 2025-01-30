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
	PORTE_OUTSET = (1<<MODULES_VD_DSET);	// Impuls for dataset
	_delay_us(5);
	PORTE_OUTCLR = (1<<MODULES_VD_DSET);
	
}
