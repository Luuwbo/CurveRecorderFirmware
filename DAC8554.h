/*
 * DAC8554.h
 *
 * Created: 09.01.2025 18:13:19
 *  Author: uwe
 */ 


#ifndef DAC8554_H_
#define DAC8554_H_

#define serdelay 2

void DAC8554_INIT (void);
void DAC8554_SetChan (int chan, int val);   // einen Kanal mit einem Wert belegen
void DAC8554_PreSetChan (int chan, int val);
void DAC8554_LoadDataHW(void);

#endif /* DAC8554_H_ */