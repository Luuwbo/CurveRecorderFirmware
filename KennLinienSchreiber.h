/*
 * KennLinienSchreiber.h
 *
 * Created: 17.03.2019 15:35:05
 *  Author: Uwe
 */ 


#ifndef KENNLINIENSCHREIBER_H_
#define KENNLINIENSCHREIBER_H_

// Modul KAL
//void Kal_UD();
//void Kal_UG();


// HW Leiterplatte
#define c_UDDACchan 0			//DAC Kanal für UD soll
#define c_UG1DACchan 1			//DAC Kanal für UG1 soll
#define c_UG2DACchan 2			//DAC KANAL für UG2 soll
#define c_TempDACchan 3			//DAC KANAL für DUT Temperatur soll

#define c_UDADCchan 0			//ADC Kanal für UD ist
#define c_UG1ADCchan 1			//ADC Kanal für UG1 ist
#define c_UGv1ADCchan 2
#define c_UG2ADCchan 3			//ADC Kanal für UG2 ist
#define c_UGv2ADCchan 4
#define c_USADCchan 5			//ADC Kanal für US ist
#define c_USDutADCchan 6		//ADC Kanal für USDUT ist (Innenwiderstand Relais etc)
#define c_TempDutADCchan 7


#define c_i32_UDR0MaxAbs_mV 2500		//maximale Ausgangsspannung von UD 
#define c_i32_UDR1MaxAbs_mV	25000		//maximale Ausgangsspannung von UD 
#define c_i32_UDR2MaxAbs_mV	250000

#define c_i32_UGr0MaxAbs_mV 2500		//maximale Ausgangsspannung von UGr0
#define c_i32_UGr1MaxAbs_mV 25000		//maximale Ausgangsspannung von UGr0


uint32_t SystemTime100u;
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
int32_t i32_US;							// Istwert US in uV
int32_t i32_RS;							// Istwert RS in uV

uint8_t ui8_UDVoltageRange;				// Verstärkung des Spannungsbereichs für UD [0,1,2]
uint8_t ui8_UDVoltageRangeOld;	
uint8_t ui8_UGvVoltageRange;			// Istwert des Spannungsbereichs für das Gate [0,1]
uint8_t ui8_UGvVoltageRangeOld;
uint8_t ui8_UGMeasInputRange;			// Istwert der Messbereichsumschaltung für UG [0,1]
uint8_t ui8_UGMeasInputRangeOld;
uint8_t ui8_RGRange;					// eingestellter RG [0,1,2,3,4}	
uint8_t ui8_RGRangeOld;	
uint8_t ui8_RSRange;					// eingestellter RG [1,2,3,4}	

uint8_t ui8_PulsCycle;				// zum auslösen eines Pulses auf 1 setzten
uint8_t ui8_StatCycle;
uint8_t ADC_data[20];

uint8_t DACchanOffset[3];

uint8_t GA_Rel_Status[2];			// Status der Relaisposoitionen auf 2 GateAmps


void KsK_SetUD();
void KsK_SetRegUDmV(int32_t i32_UD);
//void KsK_SetUDtoZero();
//void KsK_SetUG();
void KsK_SetRegUGmV(int32_t i32_UD);
//void KsK_SetUGtoZero();
void KsK_PulseMeas();
//void KsK_StatMeas();
void KsK_SetRelais();

#endif /* KENNLINIENSCHREIBER_H_ */