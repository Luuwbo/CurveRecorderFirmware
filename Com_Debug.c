/*
 * Com_debug.c
 *
 * Created: 02.03.2014 16:35:49
 *  Author: UC
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "Com_Debug.h"

void Com_Debug_Init(void)
{
	SerOutBufHigh=0;
	SerOutBufLow=0;
	SerInBufHigh=0;
	SerInBufLow=0;
}
void Com_Debug_AddCharToBuffer (char data)
{
	++SerOutBufHigh;
	if (SerOutBufHigh >= SerOutBufMax)
	{
		SerOutBufHigh = 0;
	}
	SerOutBuf[SerOutBufHigh] = data;
}
void Com_Debug_SendCharFromBuffer (void)
{	
	cli();
	if (SerOutBufLow != SerOutBufHigh)
		{
		if ((USARTF0_STATUS & (1<<USART_DREIF_bp)) != 0)
		{
			USARTF0_DATA = SerOutBuf[SerOutBufLow];
			++SerOutBufLow;
			if (SerOutBufLow >= SerOutBufMax)
			{
				SerOutBufLow = 0;
			}
		}
	}
	sei();
}
char Com_Debug_ReadCharFromInBuffer (void)				//Zeichen aus dem Buffer auslesen
{
	char aa;
	cli();
	aa = 0;
	if (SerInBufLow != SerInBufHigh)					//Zeichen in Buffer
	{
		aa = SerInBuf[SerInBufLow];
		++SerInBufLow;
		if (SerInBufLow >= SerInBufMax)
		{
			SerInBufLow = 0;
		}
	}
	sei();
	return aa;
}
void Com_Debug_AddStringToBuffer (char *s)
{
	while (*s)
	{
		Com_Debug_AddCharToBuffer(*s);
		s++;
	}
}
void Com_Debug_AddIntToBuffer(uint32_t zahl, uint8_t form)
// form :: 
{
	char s[33];
	ltoa(zahl,s,form);
	Com_Debug_AddStringToBuffer(s);
}

uint8_t Com_Debug_CharsInBuffer(void)		//Anzahl der Bytes zum lesen im InBuffer
{
	int8_t Erg;
	Erg = SerInBufHigh - SerInBufLow;
	if (Erg < 0)
	{
		Erg += SerInBufMax;
	}
	return Erg;
}