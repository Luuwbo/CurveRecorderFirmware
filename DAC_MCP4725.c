/*
 * DAC_MCP4725.c
 *
 * Created: 15.03.2019 15:39:58
 *  Author: Uwe
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "KennLinienSchreiber.h"

void SetMCP4725Output(int8_t Chan, uint32_t DACval)
{
	if (Chan == 1)
	{
		i2c_start(MPC4725_Adr_GND + 0);				//Adresse zum schreiben
	}	
	if (Chan == 2)
	{
		i2c_start(MPC4725_Adr_VDD + 0);				//Adresse zum schreiben
	}
	i2c_write(0b01000000);						//Command 
	i2c_write(DACval>>4);						//Bit 11-4
	i2c_write((DACval<<4));						//Bit 3-0 << 4
	i2c_stop();
}

void SetLTC1655Output(int8_t Chan, uint32_t DACval)
{
	if (Chan == 1)
	{
		PORTB &= ~(1 << PB2);		//CS auf 0
	}
	if (Chan == 2)
	{
		PORTB &= ~(1 << PB1);
	}
	SPDR = DACval >> 8;					//MSB schreiben
	while ((SPSR & (1<<SPIF)) == 0){};
	SPDR = DACval;						//LSB schreiben
	while ((SPSR & (1<<SPIF)) == 0){};

	PORTB |= (1 << PB1);
	PORTB |= (1 << PB2);
}

