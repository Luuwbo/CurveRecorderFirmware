/*
 * ADS1115.c
 *
 * Created: 15.03.2019 14:17:58
 *  Author: Uwe
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "KennLinienSchreiber.h"

int32_t GetADS1115DiffVal(int8_t Chan)			//chan 1-6 
{
	int32_t i32_result;
	uint32_t T1;
	
	// Multiplexer setzen
	if (Chan <= 3)
	{
		i2c_start(ADS1115_Adr_VDD + 0);			//Adresse zum schreiben 1.ADC
	}
	else
	{
		i2c_start(ADS1115_Adr_GND + 0);			//Adresse zum schreiben 2. ADC
	}		
	i2c_write(ADS1115_Adr_Reg_Config);			//Zeiger auf Config Register
	switch(Chan){
		case 1: i2c_write(0b10010101);break;	//starting a conv, MUX AIN 1-3, single shot,
		case 2: i2c_write(0b10100101);break;
		case 3: i2c_write(0b10110101);break;
		case 4: i2c_write(0b10010101);break;	//starting a conv, MUX AIN 1-3, single shot,
		case 5: i2c_write(0b10100101);break;
		case 6: i2c_write(0b10110101);break;
		}
	i2c_write(0b11100011);						//Data rate 860sps, no comp
	i2c_stop();
	// warten auf Conversion ready
	
	T1 = 0;
	while(T1 < 4000){							//Zeitschleife ca 1ms
		T1++;}
	
	//Ergebnis lesen
	i2c_start(ADS1115_Adr_VDD + 0);				//Adresse zum schreiben
	i2c_write(ADS1115_Adr_Reg_Conversion);		//Zeiger auf conversion register
	i2c_stop();
	i2c_start(ADS1115_Adr_VDD + 1);				//Adresse zum lesen
	i32_result = ((i2c_readAck()<<8) + i2c_readNak());	//High und Low Byte aus conversion register auslesen und verrechnen
	i2c_stop();
	return i32_result;
}

void ADS1115StartConversion(int8_t Chan)			//chan 1-6 
{
	// Multiplexer setzen
	if (Chan <= 3)
	{
		i2c_start(ADS1115_Adr_VDD + 0);			//Adresse zum schreiben 1.ADC
	}
	else
	{
		i2c_start(ADS1115_Adr_GND + 0);			//Adresse zum schreiben 2. ADC
	}		
	i2c_write(ADS1115_Adr_Reg_Config);			//Zeiger auf Config Register
	switch(Chan){
		case 1: i2c_write(0b10010101);break;	//starting a conv, MUX AIN 1-3, single shot,
		case 2: i2c_write(0b10100101);break;
		case 3: i2c_write(0b10110101);break;
		case 4: i2c_write(0b10010101);break;	//starting a conv, MUX AIN 1-3, single shot,
		case 5: i2c_write(0b10100101);break;
		case 6: i2c_write(0b10110101);break;
		}
	i2c_write(0b11100011);						//Data rate 860sps, no comp
	i2c_stop();
}
int32_t ADS1115GetDiffVal(int8_t Chan)			//chan 1-6 
{
	int32_t i32_result;
	
	//Ergebnis lesen
	if (Chan<=3)
	{
		i2c_start(ADS1115_Adr_VDD + 0);				//Adresse zum schreiben
	}
	else
	{
		i2c_start(ADS1115_Adr_GND + 0);				//Adresse zum schreiben
	}
	
	i2c_write(ADS1115_Adr_Reg_Conversion);		//Zeiger auf conversion register
	i2c_stop();
	if (Chan<=3)
	{
		i2c_start(ADS1115_Adr_VDD + 1);				//Adresse zum lesen
	}
	else
	{
		i2c_start(ADS1115_Adr_GND + 1);				//Adresse zum lesen
	}
	i32_result = ((i2c_readAck()<<8) + i2c_readNak());	//High und Low Byte aus conversion register auslesen und verrechnen
	i2c_stop();
	return i32_result;
}