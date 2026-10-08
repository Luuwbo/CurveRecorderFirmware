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
#define c_TempDACchan 3			//DAC KANAL für DUT Temperatur soll oder Heizspannung soll

#define c_UDADCchan 0			//ADC Kanal für UD ist
#define c_UG1ADCchan 1			//ADC Kanal für UG1 ist
#define c_UGv1ADCchan 2
#define c_UG2ADCchan 3			//ADC Kanal für UG2 ist
#define c_UGv2ADCchan 4
#define c_URSADCchan 5			//ADC Kanal für US ist
#define c_USDutADCchan 6		//ADC Kanal für USDUT ist (Innenwiderstand Relais etc)
#define c_TempDutADCchan 7		//ADC Kanal für Temp ist oder Heizspannung ist


#define c_i32_UDR0MaxAbs_mV 2500		//maximale Ausgangsspannung von UD 
#define c_i32_UDR1MaxAbs_mV	25000		//maximale Ausgangsspannung von UD 
#define c_i32_UDR2MaxAbs_mV	250000

#define c_i32_UGr0MaxAbs_mV 2500		//maximale Ausgangsspannung von UGr0
#define c_i32_UGr1MaxAbs_mV 25000		//maximale Ausgangsspannung von UGr0


// Definition der Variablen in KsKernel.c
extern volatile uint32_t SystemTime100u;			// wird in der Timer-ISR hochgezählt
extern int16_t i16_UDKalVal;							//Kalibrierwert für den Offset von Uds in digitwerte des DAC (Diff zu 2^12/2)
extern int16_t i16_UGKalVal;
extern int32_t i32_UDOutReg;							//Wert, auf den DA-Wandler gesetzt werden soll
extern int32_t i32_UGOutReg;

extern int32_t i32_UDs;						// Sollwert UD in mV
extern int32_t i32_UGs;						// Sollwert UG in mV
extern int32_t i32_UD;							// Istwert UD in mV
extern int32_t i32_UG1;							// Istwert UG in mV
extern int32_t i32_UGv1;
extern int32_t i32_UG2;							// Istwert UG in mV
extern int32_t i32_UGv2;
extern int32_t i32_URS;							// Istwert URS in uV
extern int32_t i32_RS;							// Istwert RS in uV
extern int32_t i32_US;							// Istwert US in mV

extern uint8_t ui8_UDVoltageRange;				// Verstärkung des Spannungsbereichs für UD [0,1,2]
extern uint8_t ui8_UDVoltageRangeOld;	
extern uint8_t ui8_UGvVoltageRange;			// Istwert des Spannungsbereichs für das Gate [0,1]
extern uint8_t ui8_UGvVoltageRangeOld;
extern uint8_t ui8_UGMeasInputRange;			// Istwert der Messbereichsumschaltung für UG [0,1]
extern uint8_t ui8_UGMeasInputRangeOld;
extern uint8_t ui8_RGRange;					// eingestellter RG [0,1,2,3,4}	
extern uint8_t ui8_RGRangeOld;	
extern uint8_t ui8_RSRange;					// eingestellter RG [1,2,3,4}	
extern uint8_t ui8_RSRangeOld;
extern uint8_t ui8_UGstatic;
extern uint8_t ui8_UDstatic;

extern uint8_t ui8_PulsCycle;				// zum auslösen eines Pulses auf 1 setzten
extern uint8_t ui8_StatCycle;
extern uint8_t ui8_PulsWidth;
extern uint8_t ui8_PulsWidthCounter;
extern uint8_t ADC_data[20];

extern uint8_t DACchanOffset[4];

extern uint8_t GA_Rel_Status[3];			// Status der Relaisposoitionen auf 2 GateAmps
extern uint8_t IS_Rel_Status;


void KsK_SetUD(void);
void KsK_SetRegUDmV(int32_t i32_Ud);
//void KsK_SetUDtoZero();
//void KsK_SetUG();
void KsK_SetRegUGmV(int32_t i32_Ug);
//void KsK_SetUGtoZero();
void KsK_PulseMeas(void);
//void KsK_StatMeas();
void KsK_SetRelais(void);
void SendDataAll(void);
int32_t KsK_RG_Ohm(uint8_t range);			// Widerstandswert zum RG-Bereich
int32_t KsK_RS_Ohm(uint8_t range);			// Widerstandswert zum RS-Bereich

#endif /* KENNLINIENSCHREIBER_H_ */