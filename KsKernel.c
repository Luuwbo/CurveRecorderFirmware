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
{  // wird alle 100us aufgerufen
	switch (ui8_PulsCycle){
		
		case 0: break;							// for speed
		
		case 1: KsK_SetUD();		
				KsK_SetUG();
				_delay_us(2);
				DAC8554_LoadDataHW();			// über HW-Leitung LDAC alle DAC Kanäle setzen
				ui8_PulsWidthCounter = 0;
				ui8_PulsCycle += 1;
				break;
				
		case 2: ui8_PulsWidthCounter += 1;
				if (ui8_PulsWidthCounter >= ui8_PulsWidth)
				{
					ui8_PulsCycle = 3;
				} 
				break;
				
		case 3: ADS131_READDATA();
				i32_UD = ((ADC_data[3+(c_UDADCchan<<1)])<<8) + ADC_data[3+(c_UDADCchan<<1)+1];
				i32_UG1 = ((ADC_data[3+(c_UG1ADCchan<<1)])<<8) + ADC_data[3+(c_UG1ADCchan<<1)+1];
				i32_UGv1 = ((ADC_data[3+(c_UGv1ADCchan<<1)])<<8) + ADC_data[3+(c_UGv1ADCchan<<1)+1];
				i32_URS = ((ADC_data[3+(c_URSADCchan<<1)])<<8) + ADC_data[3+(c_URSADCchan<<1)+1];
				i32_US = ((ADC_data[3+(c_USDutADCchan<<1)])<<8) + ADC_data[3+(c_USDutADCchan<<1)+1];
				if (ui8_UDstatic == 0) {KsK_SetUDtoZero();}
				if (ui8_UGstatic == 0) {KsK_SetUGtoZero();}
				ui8_PulsCycle = 0;
				_delay_us(100);
				SendDataAll();
				break;
	
		default: ui8_PulsCycle = 0;  
				break;
	}
}

//void SendDataAll(){
	//
	//Com_Debug_AddCharToBuffer(122);			//sende z
	//
	//Com_Debug_AddCharToBuffer(100);			// d für UD Anfang
	//Com_Debug_AddIntToBuffer(i32_UD,10);	// Wert
	//Com_Debug_AddCharToBuffer(68);			// D für UD Ende
	//
	//Com_Debug_AddCharToBuffer(103);			// UG
	//Com_Debug_AddIntToBuffer(i32_UG1,10);
	//Com_Debug_AddCharToBuffer(71);
	//
	//Com_Debug_AddCharToBuffer(118);			// UG1v
	//Com_Debug_AddIntToBuffer(i32_UGv1,10);
	//Com_Debug_AddCharToBuffer(86);
	//
	//Com_Debug_AddCharToBuffer(115);			// URs
	//Com_Debug_AddIntToBuffer(i32_URS,10);
	//Com_Debug_AddCharToBuffer(83);
	//
	//Com_Debug_AddCharToBuffer(114);			// Rs
//
	//switch (ui8_RSRange)
	//{
		//case 1:
		//{
			//Com_Debug_AddIntToBuffer(10,10);
			//break;
		//}
		//case 2:
		//{
			//Com_Debug_AddIntToBuffer(100,10);
			//break;
		//}
		//case 3:
		//{
			//Com_Debug_AddIntToBuffer(1000,10);
			//break;
		//}
		//case 4:
		//{
			//Com_Debug_AddIntToBuffer(10000,10);
			//break;
		//}
		//case 5:
		//{
			//Com_Debug_AddIntToBuffer(100000,10);
			//break;
		//}
		//default:
		//{
			//Com_Debug_AddIntToBuffer(10,10);
		//}
	//}
	//Com_Debug_AddCharToBuffer(82);
	//Com_Debug_AddCharToBuffer(98);
	//switch (ui8_RGRange)
	//{
		//case 0:
		//{
			//Com_Debug_AddIntToBuffer(1000000,10);
			//break;
		//}
		//case 1:
		//{
			//Com_Debug_AddIntToBuffer(99099,10);
			//break;
		//}
		//case 2:
		//{
			//Com_Debug_AddIntToBuffer(9901,10);
			//break;
		//}
		//case 3:
		//{
			//Com_Debug_AddIntToBuffer(999,10);
			//break;
		//}
		//case 4:
		//{
			//Com_Debug_AddIntToBuffer(100,10);
			//break;
		//}
		//default:
		//{
			//Com_Debug_AddIntToBuffer(1000000,10);
		//}
	//}
	//Com_Debug_AddCharToBuffer(66);
		//
	//Com_Debug_AddCharToBuffer(116);			// US
	//Com_Debug_AddIntToBuffer(i32_US,10);
	//Com_Debug_AddCharToBuffer(84);
	//
	//Com_Debug_AddCharToBuffer(10);					// LineFeed
	//Com_Debug_AddCharToBuffer(13);					// LineFeed
//}

void SendDataAll(){
	int8_t b1,b2,b3,b4;
	int32_t h32;
	
	Com_Debug_AddCharToBuffer(1);			//sende z
	
	Com_Debug_AddCharToBuffer(2);			// d für UD Anfang
	//Com_Debug_AddIntToBuffer(i32_UD,10);	// Wert
	h32 = i32_UD; //+1048576
	b3 = (((h32) >> 12) & 0b00111111) + 32; 
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	//Com_Debug_AddIntToBuffer(i32_UG1,10);
	h32 = i32_UG1;
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	//Com_Debug_AddIntToBuffer(i32_UGv1,10);
	h32 = i32_UGv1;
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	// URS
	//Com_Debug_AddIntToBuffer(i32_URS,10);
	h32 = i32_URS;
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	//RS
	switch (ui8_RSRange)
	{
		case 1:
		{	h32 = 10;
			break;	}
		case 2:
		{	h32 = 100;
			break;	}
		case 3:
		{	h32 = 1000;
			break;	}
		case 4:
		{	h32 = 10000;
			break;	}
		case 5:
		{	h32 = 100000;
			break;	}
		default:
		{	h32 = 10;	}
	}
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	//RG
	switch (ui8_RGRange)
	{
		case 0:
		{	h32 = 1000000;
			break;	}
		case 1:
		{	h32 = 99099;
			break;	}
		case 2:
		{	h32 = 9901;
			break;	}
		case 3:
		{	h32 = 999;
			break;	}
		case 4:
		{	h32 = 100;
			break;	}
		default:
		{	h32 = 1000000;
		}
	}
	b4 = (((h32) >> 18) & 0b00111111) + 32;
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b4);
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	//US
	h32 = i32_US;
	b3 = (((h32) >> 12) & 0b00111111) + 32;
	b2 = (((h32) >> 6) & 0b00111111) + 32;
	b1 = (((h32) >> 0) & 0b00111111) + 32;
	Com_Debug_AddCharToBuffer(b3);
	Com_Debug_AddCharToBuffer(b2);
	Com_Debug_AddCharToBuffer(b1);
	Com_Debug_AddCharToBuffer(3);
	
	Com_Debug_AddCharToBuffer(10);					// LineFeed
	Com_Debug_AddCharToBuffer(13);					// LineFeed
}
void KsK_SetRelais() {
// UGv Relais setzen
	if (ui8_UGvVoltageRange != ui8_UGvVoltageRangeOld) {
		Modules_UGvRange_SET(ui8_UGvVoltageRange,1);
		ui8_UGvVoltageRangeOld = ui8_UGvVoltageRange;
	}
	
// UG Messeingang Empfindlichkeit
	if (ui8_UGMeasInputRange != ui8_UGMeasInputRangeOld) {
		Modules_UGRange_SET(ui8_UGMeasInputRange,1);
		ui8_UGMeasInputRangeOld = ui8_UGMeasInputRange;
	}

// UD Relais setzen
	if (ui8_UDVoltageRangeOld != ui8_UDVoltageRange) {
		Modules_VDRANGE_SET (ui8_UDVoltageRange);
		ui8_UDVoltageRangeOld = ui8_UDVoltageRange;
		}
	
//// RG Umschalten
	if (ui8_RGRangeOld != ui8_RGRange) {
		Modules_RG_SET (ui8_RGRange,1);
		ui8_RGRangeOld = ui8_RGRange;
	}

//// RS Umschalten
if (ui8_RSRangeOld != ui8_RSRange) {
	Modules_RS_SET (ui8_RSRange);
	ui8_RSRangeOld = ui8_RSRange;
	}
}
