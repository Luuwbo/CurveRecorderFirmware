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
#define F_CPU 32000000UL
#include <util/delay.h>


#include "KennLinienSchreiber.h"
#include "Com_Debug.h"
#include "ADS131.h"
#include "DAC8554.h"
#include "Modules.h"



void KsK_SetRegUDmV(int32_t i32_Ud)		// skaliert den Sollwert in mV auf den DAC
{
	int64_t i64_v1;
	//UD in bits für 16bit DAC umrechnen
	i64_v1 = i32_UDs << 15;
	if (ui8_UDVoltageRange == 1)
	{
		i32_UDOutReg = (i64_v1 / c_i32_UDR1MaxAbs_mV) + 32768;
	}
	if (ui8_UDVoltageRange == 0)
	{
		i32_UDOutReg = (i64_v1 / c_i32_UDR0MaxAbs_mV) + 32768;
	}
	if (ui8_UDVoltageRange == 2)
	{
		i32_UDOutReg = (i64_v1 / c_i32_UDR2MaxAbs_mV) + 32768;
	}
	
}
void KsK_SetUD()
{
	DAC8554_SetChan(c_UDDACchan,i32_UDOutReg);
}
void KsK_SetUDtoZero()
{
	DAC8554_SetChan(c_UDDACchan,32768);
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
	DAC8554_SetChan(c_UG1DACchan,i32_UGOutReg);
}
void KsK_SetUGtoZero()
{
	DAC8554_SetChan(c_UG1DACchan,32768);
}
//-------------------------------------------------------------------------------------
void KsK_PulseMeas()
{
	switch (ui8_PulsCycle){
		
		case 0: break;							// for speed
		
		case 1: KsK_SetUD();		
				KsK_SetUG();
				_delay_us(2);
				DAC8554_LoadDataHW();			// über HW-Leitung LDAC alle DAC Kanäle setzen
				ui8_PulsCycle += 1;
				break;
				
		case 2: ui8_PulsCycle += 1;
				break;
				
		case 3: ADS131_READDATA();
				i32_UD = ((ADC_data[3+(c_UDADCchan<<1)])<<8) + ADC_data[3+(c_UDADCchan<<1)+1];
				i32_UG1 = ((ADC_data[3+(c_UG1ADCchan<<1)])<<8) + ADC_data[3+(c_UG1ADCchan<<1)+1];
				i32_UGv1 = ((ADC_data[3+(c_UGv1ADCchan<<1)])<<8) + ADC_data[3+(c_UGv1ADCchan<<1)+1];
				i32_US = ((ADC_data[3+(c_USADCchan<<1)])<<8) + ADC_data[3+(c_USADCchan<<1)+1];
				KsK_SetUDtoZero();
				KsK_SetUGtoZero();
				ui8_PulsCycle = 0;
				_delay_ms(1);
				Com_Debug_AddCharToBuffer(80);					// "P" Pulsmessung fertig melden
				Com_Debug_AddCharToBuffer(10);					// LineFeed
				Com_Debug_AddCharToBuffer(13);					// LineFeed
				break;
	
		default: ui8_PulsCycle = 0;  
				break;
	}
}


//void KsK_StatMeas()
//{
	//switch (ui8_StatCycle){
		//case 1: KsK_SetUD();
		//KsK_SetUG();
		//ADS1115StartConversion(c_UDADCchan);
		//ADS1115StartConversion(c_USADCchan);
		//ui8_StatCycle += 1;
		//break;
		//
		//case 2: i32_UD = ADS1115GetDiffVal(c_UDADCchan);
		//i32_US = ADS1115GetDiffVal(c_USADCchan);
		//ADS1115StartConversion(c_UGADCchan);
		//ADS1115StartConversion(c_UGvADCchan);
		//ui8_StatCycle += 1;
		//break;
		//
		//case 3:	ADS1115StartConversion(c_UGADCchan);
		//ADS1115StartConversion(c_UGvADCchan);
		//ui8_StatCycle += 1;
		//break;
		//
		//case 4:	i32_UG = ADS1115GetDiffVal(c_UGADCchan);
		//i32_UGv = ADS1115GetDiffVal(c_UGvADCchan);
		//ui8_StatCycle += 1;
		//break;
		//
		//default: ui8_StatCycle = 0;  break;
	//}
//}

void KsK_SetRelais() {
// UGv Relais setzen
	if (ui8_UGvVoltageRange != ui8_UGvVoltageRangeOld) {
		Modules_UGvRange_SET(ui8_UGvVoltageRange,1);
		ui8_UGvVoltageRangeOld = ui8_UGvVoltageRange;
	}
	
// UG Messeingang Empfindlichkeit (UGV6 an PD7)
	if (ui8_UGMeasInputRange != ui8_UGMeasInputRangeOld) {
		Modules_UGRange_SET(ui8_UGMeasInputRange,1);
		ui8_UGMeasInputRangeOld = ui8_UGMeasInputRange;
	}

// UD Relais setzen
	if (ui8_UDVoltageRangeOld != ui8_UDVoltageRange) {
		Modules_VDRANGE_SET (ui8_UDVoltageRange);
		ui8_UDVoltageRangeOld = ui8_UDVoltageRange;
		}
	
//// RG Umschalten (UGV1 bis UG4 an PD2 bis PD5)
	if (ui8_RGRangeOld != ui8_RGRange) {
		Modules_RG_SET (ui8_RGRange,1);
		ui8_RGRangeOld = ui8_RGRange;
	}

	//switch(ui8_RSRange)
	//{
		//case 1:

	//}
}
