/*
 * Com_Debug.h
 *
 * Created: 02.03.2014 16:36:56
 *  Author: UC
 */ 


#ifndef Com_Debug_H_
#define Com_Debug_H_

#define SerOutBufMax 50
#define SerInBufMax 30

char SerOutBuf[SerOutBufMax+1];
char SerInBuf[SerInBufMax+1];
uint8_t SerOutBufHigh;
uint8_t SerOutBufLow;
uint8_t SerInBufHigh;
uint8_t SerInBufLow;

void Com_Debug_Init(void);
void Com_Debug_SendCharFromBuffer (void);					/* sendet Zeichen aus Buffer, muss regelm‰ﬂig aufgerufen werden */
void Com_Debug_AddCharToBuffer (char data);					/* addiert Zeichen zum Buffer */
void Com_Debug_AddStringToBuffer (char *s);					/* addiert String zum Buffer */
void Com_Debug_AddIntToBuffer(uint32_t zahl, uint8_t form);	/* addiert Integer Zahl um Buffer */
char Com_Debug_ReadCharFromInBuffer (void);					/* Zeichen aus dem Buffer auslesen */
uint8_t Com_Debug_CharsInBuffer(void);

#endif /* Com_Debug_H_ */