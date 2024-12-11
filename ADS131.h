/*
 * ADS131.h
 *
 * Created: 09.12.2024 09:26:20
 *  Author: uwe
 */ 


#ifndef ADS131_H_
#define ADS131_H_

#define ADS131_command_wakeup 0x02
#define ADS131_command_reset 0x06
#define ADS131_command_start 0x08
#define ADS131_command_stop 0x0a
#define ADS131_command_rdatac 0x10
#define ADS131_command_sdatac 0x11
#define ADS131_command_rdata 0x12

#define serdelay 2

void ADS131_INIT (void);
void ADS131_READDATA(void);
void ADS131_ChanSet(uint8_t chan, uint8_t mux);
uint8_t ADS131_ReadRegister(int reg);


#endif /* ADS131_H_ */