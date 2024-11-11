/*
 * Kal.c
 *
 * Created: 16.03.2019 13:35:22
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


//void Kal_UD()
//{
	//int32_t i32_ADC;
	//uint8_t l;
	//int16_t i16_UDKalTol;
//
	//i16_UDKalVal = 2048;
	//i16_UDKalTol = 6;
	//
	//do
	//{
		//SetMCP4725Output(c_UDDACchan, i16_UDKalVal);	//UD setzen
		//i32_ADC = 0;
		//for (l=0;l<16;l++)
		//{
			//i32_ADC += GetADS1115DiffVal(c_UDADCchan);
		//}
		//i32_ADC >>= 4;
		//
		//if (i32_ADC > i16_UDKalTol) 
		//{
			//i16_UDKalVal -= 1;
		//}
		//if (i32_ADC < -i16_UDKalTol)
		//{
			//i16_UDKalVal += 1;
		//}
	//} 
	//while ((i32_ADC < -i16_UDKalTol) || (i32_ADC > i16_UDKalTol)) ;
	//i16_UDKalVal -= 2048;
//}
//void Kal_UG()
//{
	//int32_t i32_ADC;
	//uint8_t l;
	//int16_t i16_UGKalTol;
//
	//i16_UGKalVal = 2048;
	//i16_UGKalTol = 3;
	//
	//do
	//{
		//SetMCP4725Output(c_UDDACchan, i16_UGKalVal);	//UD setzen
		//i32_ADC = 0;
		//for (l=0;l<16;l++)
		//{
			//i32_ADC += GetADS1115DiffVal(c_UGADCchan);
		//}
		//i32_ADC >>= 4;
		//
		//if (i32_ADC > i16_UGKalTol)
		//{
			//i16_UGKalVal -= 1;
		//}
		//if (i32_ADC < -i16_UGKalTol)
		//{
			//i16_UGKalVal += 1;
		//}
	//}
	//while ((i32_ADC < -i16_UGKalTol) || (i32_ADC > i16_UGKalTol)) ;
	//i16_UGKalVal -= 2048;
//}