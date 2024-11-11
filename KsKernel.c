/*
 * KsKernel.c
 *
 * Created: 16.03.2019 10:52:22
 *  Author: Uwe
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>

#include "KennLinienSchreiber.h"
#include "Com_Debug.h"



void KsK_SetRegUDmV(int32_t i32_Ud)
{
	int64_t i64_v1;
	//UD in bits für 16bit DAC umrechnen
	i64_v1 = i32_UDs << 15;
	if (ui8_UDVoltageRange == 1)
	{
		i32_UDOutReg = (i64_v1 / c_i32_UDR1MaxAbs_mV) + 32768;
	}
	else
	{
		i32_UDOutReg = (i64_v1 / c_i32_UDR0MaxAbs_mV) + 32768;
	}
}
void KsK_SetUD()
{
	//SetMCP4725Output(c_UDDACchan,i32_UDOutReg>>4);
	SetLTC1655Output(c_UDDACchan,i32_UDOutReg);
}
void KsK_SetUDtoZero()
{
	//SetMCP4725Output(c_UDDACchan,2048 + i16_UDKalVal);
	SetLTC1655Output(c_UDDACchan,32768);
}

//-------------------------------------------------------------------------------------
void KsK_SetRegUGmV(int32_t i32_UG)
{
	int64_t i64_v1;
	//UG in bits für 16bit DAC umrechnen
	i64_v1 = i32_UG << 15;		//
	if (ui8_UGvVoltageRange == 0)
	{
		i32_UGOutReg = (i64_v1 / c_i32_UGr0MaxAbs_mV) + 32768;
	}
	else
	{
		i32_UGOutReg = (i64_v1 / c_i32_UGr1MaxAbs_mV) + 32768;
	}
}
void KsK_SetUG()
{
	//SetMCP4725Output(c_UGDACchan,i32_UGOutReg>>4);
	SetLTC1655Output(c_UGDACchan,i32_UGOutReg);

}
void KsK_SetUGtoZero()
{
	//SetMCP4725Output(c_UGDACchan,2048 + i16_UGKalVal);
	SetLTC1655Output(c_UGDACchan,32768);
}
//-------------------------------------------------------------------------------------
void KsK_PulseMeas()
{
	switch (ui8_PulsCycle){
		case 1: KsK_SetUD();
				KsK_SetUG();
				ui8_PulsCycle += 1;
				break;

		case 2: ADS1115StartConversion(c_UDADCchan);
				ADS1115StartConversion(c_USADCchan);
				ui8_PulsCycle += 1;
				break;

		case 3: i32_UD = ADS1115GetDiffVal(c_UDADCchan);
				i32_US = ADS1115GetDiffVal(c_USADCchan);
				ADS1115StartConversion(c_UGADCchan);
				ADS1115StartConversion(c_UGvADCchan);
				ui8_PulsCycle += 1;
				break;
	
		case 4:	i32_UG = ADS1115GetDiffVal(c_UGADCchan);
				i32_UGv = ADS1115GetDiffVal(c_UGvADCchan);
				KsK_SetUDtoZero();
				KsK_SetUGtoZero();
				ui8_PulsCycle += 1;
				Com_Debug_AddCharToBuffer(80);					// "P" Pulsmessung fertig melden
				Com_Debug_AddCharToBuffer(10);					// LineFeed
				Com_Debug_AddCharToBuffer(13);					// LineFeed
				break;
	
		default: ui8_PulsCycle = 0;  break;
	}
}

void KsK_StatMeas()
{
	switch (ui8_StatCycle){
		case 1: KsK_SetUD();
				KsK_SetUG();
				ADS1115StartConversion(c_UDADCchan);
				ADS1115StartConversion(c_USADCchan);
				ui8_StatCycle += 1;
				break;
		
		case 2: i32_UD = ADS1115GetDiffVal(c_UDADCchan);
				i32_US = ADS1115GetDiffVal(c_USADCchan);
				ADS1115StartConversion(c_UGADCchan);
				ADS1115StartConversion(c_UGvADCchan);
				ui8_StatCycle += 1;
				break;
		
		case 3:	ADS1115StartConversion(c_UGADCchan);
				ADS1115StartConversion(c_UGvADCchan);
				ui8_StatCycle += 1;
				break;
		
		case 4:	i32_UG = ADS1115GetDiffVal(c_UGADCchan);
				i32_UGv = ADS1115GetDiffVal(c_UGvADCchan);
				ui8_StatCycle += 1;
				break;
		
		default: ui8_StatCycle = 0;  break;
	}
}

void KsK_SetRelais() {
// UGV Relais setzen (UGV5 an PD6)
	if (ui8_UGvVoltageRange == 1)
	{
		PORTD |= (1 << PD6);
	}
	else
	{
		PORTD &= ~(1 << PD6);
	}

// UG Messeingang Empfindlichkeit (UGV6 an PD7)
	if (ui8_UGMeasInputRange == 1)
	{
		PORTD |= (1 << PD7);
	}
	else
	{
		PORTD &= ~(1 << PD7);
	}

// UD Relais setzen (UD Relais an PB0)
	if (ui8_UDVoltageRange == 1)		// Spannung < 2V
	{
		PORTB |= (1 << PB0);
	}
	else
	{
		PORTB &= ~(1 << PB0);
	}

// RG Umschalten (UGV1 bis UG4 an PD2 bis PD5)

	switch(ui8_RGRange)
	{
		case 1:
		{
			PORTD &= ~(1 << PD2);
			PORTD &= ~(1 << PD3);
			PORTD &= ~(1 << PD4);
			PORTD |= (1 << PD5);
			break;
		}
		case 2:
		{
			PORTD &= ~(1 << PD2);
			PORTD &= ~(1 << PD3);
			PORTD &= ~(1 << PD5);
			PORTD |= (1 << PD4);
			break;
		}
		case 3:
		{
			PORTD &= ~(1 << PD2);
			PORTD &= ~(1 << PD4);
			PORTD &= ~(1 << PD5);
			PORTD |= (1 << PD3);
			break;
		}
		case 4:
		{
			PORTD |= (1 << PD2);
			PORTD &= ~(1 << PD3);
			PORTD &= ~(1 << PD4);
			PORTD &= ~(1 << PD5);
			break;
		}
		default:
		{
			PORTD &= ~(1 << PD2);
			PORTD &= ~(1 << PD3);
			PORTD &= ~(1 << PD4);
			PORTD &= ~(1 << PD5);
			break;
		}
	}
	switch(ui8_RSRange)
	{
		case 1:
		{
			PORTC |= (1 << PC0);
			PORTC &= ~(1 << PC1);
			PORTC &= ~(1 << PC2);
			PORTC &= ~(1 << PC3);
			break;
		}
		case 2:
		{
			PORTC &= ~(1 << PC0);
			PORTC &= ~(1 << PC2);
			PORTC &= ~(1 << PC3);
			PORTC |= (1 << PC1);
			break;
		}
		case 3:
		{
			PORTC &= ~(1 << PC0);
			PORTC &= ~(1 << PC1);
			PORTC &= ~(1 << PC3);
			PORTC |= (1 << PC2);
			break;
		}
		case 4:
		{
			PORTC &= ~(1 << PC0);
			PORTC &= ~(1 << PC1);
			PORTC &= ~(1 << PC2);
			PORTC |= (1 << PC3);
			break;
		}
		default:
		{
			PORTC &= ~(1 << PC0);
			PORTC &= ~(1 << PC1);
			PORTC &= ~(1 << PC2);
			PORTC &= ~(1 << PC3);
			break;
		}
	}
}

	