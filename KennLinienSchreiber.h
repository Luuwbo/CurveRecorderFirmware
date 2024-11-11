/*
 * KennLinienSchreiber.h
 *
 * Created: 17.03.2019 15:35:05
 *  Author: Uwe
 */ 


#ifndef KENNLINIENSCHREIBER_H_
#define KENNLINIENSCHREIBER_H_

// Modul ADS1115
#define ADS1115_Adr_GND 0b10010000			//Adresse für ADDR = GND
#define ADS1115_Adr_VDD 0b10010010			//Adresse für ADDR = VDD
#define ADS1115_Adr_Reg_Config 0b00000001	//Adresse Config Register
#define ADS1115_Adr_Reg_Conversion 0		//Adresse Result Register
int32_t GetADS1115DiffVal(int8_t Chan);
void ADS1115StartConversion(int8_t Chan);
int32_t ADS1115GetDiffVal(int8_t Chan);

// Modul DAC_MCP4725
#define MPC4725_Adr_GND 0b11000000			//Adresse für ADDR = GND
#define MPC4725_Adr_VDD 0b11000010			//Adresse für ADDR = VDD
#define MPC4725_DAC_Reg 0b01000000			//command
void SetMCP4725Output(int8_t Chan, uint32_t DACval);
void SetLTC1655Output(int8_t Chan, uint32_t DACval);

// Modul KAL
void Kal_UD();
void Kal_UG();


// HW Leiterplatte
#define c_UDDACchan 1			//DAC KAnal für UD
#define c_UGDACchan 2			//DAC KAnal für UG

#define c_UDADCchan 2
#define c_UGADCchan 1
#define c_UGvADCchan 4
#define c_USADCchan 5

#define c_i32_UDR0MaxAbs_mV 25227		//maximale Ausgangsspannung von UD bei Relais aus
#define c_i32_UDR1MaxAbs_mV	2500		//maximale Ausgangsspannung von UD bei Relais ein
#define c_i32_UGr0MaxAbs_mV 12250		//maximale Ausgangsspannung von UGr0
#define c_i32_UGr1MaxAbs_mV 2500		//maximale Ausgangsspannung von UGr0


//int32_t c_i32_UDMaxAbs_mV = 25000;		//maximale Ausgangsspannung von UD
//int32_t c_i32_UGMaxAbs_mV = 12250;		//maximale Ausgangsspannung von UG

uint32_t SystemTime100u;
int16_t i16_UDKalVal;							//Kalibrierwert für den Offset von Uds in digitwerte des DAC (Diff zu 2^12/2)
int16_t i16_UGKalVal;
int32_t i32_UDOutReg;							//Wert, auf den DA-Wandler gesetzt werden soll
int32_t i32_UGOutReg;

int32_t i32_UDs;						// Sollwert UD in mV
int32_t i32_UGs;						// Sollwert UG in mV
int32_t i32_UD;							// Istwert UD in mV
int32_t i32_UG;							// Istwert UG in mV
int32_t i32_UGv;
int32_t i32_US;							// Istwert US in uV
int32_t i32_RS;							// Istwert RS in uV
uint8_t ui8_UGvVoltageRange;			// Istwert des Spannungsbereichs für das Gate [0,1]
uint8_t ui8_UDVoltageRange;				// Verstärkung des Spannungsbereichs für UD [0,1]
uint8_t ui8_UGMeasInputRange;			// Istwert der Messbereichsumschaltung für UG [0,1]
uint8_t ui8_RGRange;					// eingestellter RG [0,1,2,3,4}		
uint8_t ui8_RSRange;					// eingestellter RG [1,2,3,4}	


uint8_t ui8_PulsCycle;				// zum auslösen eines Pulses auf 1 setzten
uint8_t ui8_StatCycle;

void KsK_SetUD();
void KsK_SetRegUDmV(int32_t i32_UD);
void KsK_SetUDtoZero();
void KsK_SetUG();
void KsK_SetRegUGmV(int32_t i32_UD);
void KsK_SetUGtoZero();
void KsK_PulseMeas();
void KsK_StatMeas();
void KsK_SetRelais();

#endif /* KENNLINIENSCHREIBER_H_ */