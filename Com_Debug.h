/*
 * Com_Debug.h
 *
 * Created: 02.03.2014 16:36:56
 *  Author: UC
 */ 


#ifndef Com_Debug_H_
#define Com_Debug_H_

#define SerOutBufMax 200
#define SerInBufMax 30

// volatile: werden in den USART-ISRs (main.c) gelesen/geschrieben, Definition in Com_Debug.c
extern volatile char SerOutBuf[SerOutBufMax+1];
extern volatile char SerInBuf[SerInBufMax+1];
extern volatile uint8_t SerOutBufHigh;
extern volatile uint8_t SerOutBufLow;
extern volatile uint8_t SerInBufHigh;
extern volatile uint8_t SerInBufLow;

void Com_Debug_Init(void);
void Com_Debug_SendCharFromBuffer (void);					/* sendet Zeichen aus Buffer, muss regelm‰ﬂig aufgerufen werden */
void Com_Debug_AddCharToBuffer (char data);					/* addiert Zeichen zum Buffer */
void Com_Debug_AddStringToBuffer (char *s);					/* addiert String zum Buffer */
void Com_Debug_AddIntToBuffer(uint32_t zahl, uint8_t form);	/* addiert Integer Zahl um Buffer */
char Com_Debug_ReadCharFromInBuffer (void);					/* Zeichen aus dem Buffer auslesen */
uint8_t Com_Debug_CharsInBuffer(void);

#endif /* Com_Debug_H_ */