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

// globale Variablen (Deklaration als extern in KennLinienSchreiber.h)
volatile uint32_t SystemTime100u;			// wird in der Timer-ISR hochgezählt
int16_t i16_UDKalVal;							//Kalibrierwert für den Offset von Uds in digitwerte des DAC (Diff zu 2^12/2)
int16_t i16_UGKalVal;
int32_t i32_UDOutReg;							//Wert, auf den DA-Wandler gesetzt werden soll
int32_t i32_UGOutReg;

int32_t i32_UDs;						// Sollwert UD in mV
int32_t i32_UGs;						// Sollwert UG in mV
int32_t i32_UD;							// Istwert UD in mV
int32_t i32_UG1;							// Istwert UG in mV
int32_t i32_UGv1;
int32_t i32_UG2;							// Istwert UG in mV
int32_t i32_UGv2;
int32_t i32_URS;							// Istwert URS in uV
int32_t i32_RS;							// Istwert RS in uV
int32_t i32_US;							// Istwert US in mV

uint8_t ui8_UDVoltageRange;				// Verstärkung des Spannungsbereichs für UD [0,1,2]
uint8_t ui8_UDVoltageRangeOld;	
uint8_t ui8_UGvVoltageRange;			// Istwert des Spannungsbereichs für das Gate [0,1]
uint8_t ui8_UGvVoltageRangeOld;
uint8_t ui8_UGMeasInputRange;			// Istwert der Messbereichsumschaltung für UG [0,1]
uint8_t ui8_UGMeasInputRangeOld;
uint8_t ui8_RGRange;					// eingestellter RG [0,1,2,3,4}	
uint8_t ui8_RGRangeOld;	
uint8_t ui8_RSRange;					// eingestellter RG [1,2,3,4}	
uint8_t ui8_RSRangeOld;
uint8_t ui8_UGstatic;
uint8_t ui8_UDstatic;

uint8_t ui8_PulsCycle;				// zum auslösen eines Pulses auf 1 setzten
uint8_t ui8_StatCycle;
uint8_t ui8_PulsWidth;
uint8_t ui8_PulsWidthCounter;
uint8_t ADC_data[20];

uint8_t DACchanOffset[4];

uint8_t GA_Rel_Status[3];			// Status der Relaisposoitionen auf 2 GateAmps
uint8_t IS_Rel_Status;



// begrenzt einen DAC-Wert auf den gültigen Bereich 0..65535
static int32_t KsK_ClampDAC(int64_t i64_val)
{
	if (i64_val < 0) {return 0;}
	if (i64_val > 65535) {return 65535;}
	return (int32_t)i64_val;
}

void KsK_SetRegUDmV(int32_t i32_Ud)		// skaliert den Sollwert in mV auf den DAC
{
	int64_t i64_v1;
	//UD in bits für 16bit DAC umrechnen, Shift in 64 Bit (sonst Überlauf ab 65,5V)
	i64_v1 = (int64_t)i32_Ud << 15;
	if (ui8_UDVoltageRange == 1)
	{
		i32_UDOutReg = KsK_ClampDAC((i64_v1 / c_i32_UDR1MaxAbs_mV) + 32768);
	}
	if (ui8_UDVoltageRange == 0)
	{
		i32_UDOutReg = KsK_ClampDAC((i64_v1 / c_i32_UDR0MaxAbs_mV) + 32768);
	}
	if (ui8_UDVoltageRange == 2)
	{
		i32_UDOutReg = KsK_ClampDAC((i64_v1 / c_i32_UDR2MaxAbs_mV) + 32768);
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
	i64_v1 = (int64_t)i32_UG << 15;		// Shift in 64 Bit
	if (ui8_UGvVoltageRange == 0)
	{
		i32_UGOutReg = KsK_ClampDAC((i64_v1 / c_i32_UGr0MaxAbs_mV) + 32768);
	}
	else
	{
		i32_UGOutReg = KsK_ClampDAC((i64_v1 / c_i32_UGr1MaxAbs_mV) + 32768);
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

// Widerstandswert in Ohm zum RG-Bereich (zentrale Tabelle für SendDataAll und Befehl "b")
int32_t KsK_RG_Ohm(uint8_t range)
{
	switch (range)
	{
		case 1:  return 99099;
		case 2:  return 9901;
		case 3:  return 999;
		case 4:  return 100;
		default: return 1000000;		// auch Bereich 0
	}
}

// Widerstandswert in Ohm zum RS-Bereich (zentrale Tabelle für SendDataAll und Befehl "r")
int32_t KsK_RS_Ohm(uint8_t range)
{
	switch (range)
	{
		case 2:  return 100;
		case 3:  return 1000;
		case 4:  return 10000;
		case 5:  return 100000;
		default: return 10;				// auch Bereich 1
	}
}

// sendet einen Wert als n Zeichen zu je 6 Bit (+32 -> druckbares Zeichen), höchstwertige Gruppe zuerst
static void SendVal6Bit(int32_t h32, uint8_t n)
{
	while (n > 0)
	{
		n--;
		Com_Debug_AddCharToBuffer(((h32 >> (6 * n)) & 0b00111111) + 32);
	}
}

void SendDataAll(void){
	Com_Debug_AddCharToBuffer(1);			//sende z
	Com_Debug_AddCharToBuffer(2);			// d für UD Anfang
	SendVal6Bit(i32_UD, 3);
	SendVal6Bit(i32_UG1, 3);
	SendVal6Bit(i32_UGv1, 3);
	SendVal6Bit(i32_URS, 3);
	SendVal6Bit(KsK_RS_Ohm(ui8_RSRange), 3);
	SendVal6Bit(KsK_RG_Ohm(ui8_RGRange), 4);
	SendVal6Bit(i32_US, 3);
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
