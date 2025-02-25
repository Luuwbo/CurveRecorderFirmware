/*
 * HL-Kennlinienschreiber 2.c
 *
 * Created: 22.11.2024
 * Author : Uwe
 */ 

#define CCRV 2
 
 #define CPU_SPEED 32000000
 #define F_CPU 32000000UL

 #include <avr/io.h>
 #include <avr/wdt.h>
 #include <stdlib.h>
 #include <stdint.h>
// #include <util/twi.h>
 #include <avr/interrupt.h>
 #include <math.h>
 #include <string.h>
 
 // Globale Variablen
 
// #include "i2cmaster.h"
 #include "Com_Debug.h"
 #include "KennLinienSchreiber.h"
 #include "ADS131.h"
 #include "DAC8554.h"
 #include "Befehlsinterpreter.h"
 #include "Modules.h"
 
 /* Prototypen */
 void InitCPU (void);
 void InitIO (void);
 void TimerInit (void);
 void InitVariables (void);



 void Loop1000ms (void);
 void Loop100ms (void);
 void Loop10ms (void);
 void Loop20ms (void);
 void Loop1ms (void);
 void Loopxms (void);
 void Loop100us(void);
 
uint16_t MainTimer1000ms;
uint16_t MainTimer100ms;
uint8_t MainTimerxms;
uint8_t MainTimer10ms;
uint8_t MainTimer1ms;
uint8_t MainTimer100us;
uint16_t MainTime;

char sh[10];
uint8_t PowerToggleTimer;
//System
uint8_t ui8_PulsCycle;				//Statusvariable für Pulsausgabe

//Test

int16_t DACvalTest = 0;
uint8_t av;
uint8_t chan = 0;


int main(void)
{
	InitIO();
	InitCPU();
	
	TimerInit();
//	i2c_init();
	

	DACvalTest = 0;
	av = 0;
	ui8_PulsCycle = 255;

	Com_Debug_AddStringToBuffer("Start");

	ADS131_INIT();
	ADS131_ChanSet(0,0);
	ADS131_ChanSet(1,0);
	ADS131_ChanSet(2,0);
	ADS131_ChanSet(3,0);
	ADS131_ChanSet(4,0);
	ADS131_ChanSet(5,0);
	ADS131_ChanSet(6,0);
	ADS131_ChanSet(7,0);
	
	DAC8554_INIT();
	
	MODULES_INIT();

    while (1) 
    {
		if (MainTimer100us >= 1)
		{
			MainTimer100us = 0;
			Loop100us();
		}
		if (MainTimer1ms >= 10)
		{
			MainTimer1ms -= 10;
			Loop1ms();
		}
		if (MainTimerxms >= 15)
		{
			MainTimerxms -= 15;
			Loopxms();
		}
		if (MainTimer10ms >= 100)
		{
			MainTimer10ms -= 100;
			Loop10ms();
		}
		if (MainTimer100ms >= 1000)
		{
			MainTimer100ms -= 1000;
			Loop100ms();
		}
		if (MainTimer1000ms >= 10000)
		{
			MainTimer1000ms -= 10000;
			Loop1000ms();
		}
		Com_Debug_SendCharFromBuffer();
    }
}

void Loop1000ms(void)
{

}
void Loop100ms (void)
{	
	//uint8_t i;
	//for (i=0;i<12;i++)
	//{
		//Com_Debug_AddIntToBuffer(ADS131_ReadRegister(i),2);
		//Com_Debug_AddStringToBuffer("-");
	//}
	//Com_Debug_AddCharToBuffer(13);
}
void Loop10ms (void)
{
	uint8_t i;
	int16_t erg;
	if (ui8_PulsCycle == 0) {KsK_SetRelais();}		//Relais setting only without pulse
	
	//ADS131_READDATA();
	//Com_Debug_AddIntToBuffer(ADC_data[0],2);
	//Com_Debug_AddIntToBuffer(ADC_data[1],2);
	//Com_Debug_AddIntToBuffer(ADC_data[2],2);
	//Com_Debug_AddStringToBuffer(";");
	//for (i=3;i<19;i+=2)
	//{
		//erg = ADC_data[i]*256+ADC_data[i+1];
		//Com_Debug_AddIntToBuffer(erg,10);
		//Com_Debug_AddStringToBuffer(";");
	//}
	//Com_Debug_AddCharToBuffer(13);
}

void Loopxms()
{

//	KsK_StatMeas();
}
void Loop1ms (void)
{

}
void Loop100us(void)
{	
	BefInt();
	KsK_PulseMeas();
	//DAC8554_SetChan (1,DACvalTest);
	//DACvalTest++;	
}


void InitVariables (void)
{
	MainTimer1000ms = 0;
	MainTimer100ms = 0;
	MainTimer10ms = 0;
	MainTimer1ms = 0;
	MainTimerxms = 0;
	PowerToggleTimer = 0;
	MainTime = 0;

	ui8_RGRange = 0;
	ui8_RGRangeOld = 0;
	ui8_UGvVoltageRange = 0;
	ui8_UGvVoltageRangeOld = 0;
	ui8_UGMeasInputRange = 0;	
	ui8_UGMeasInputRangeOld = 0;
	GA_Rel_Status[1] = 0;
	GA_Rel_Status[2] = 0;	
}

void InitCPU (void)
{
	cli();
// ------ System Clock set ---------------------------------------------------------------------------------------
	OSC_CTRL = 0b00000111;					//32kHz & 32MHz Osc on
	while(OSC_STATUS < 3){ };
	CPU_CCP = CCP_IOREG_gc;					//protection register
	//CLK_CTRL = 0b00000001;			//select RC32Mhz
	asm("LDI R24,0x01");
	asm("STS 0x0040,R24");
	
// ------ USART F0 for USB Com set	------------------------------------------------------------------------------
	// IO set
	PORTF_DIRSET = 1<<3;	// PF3 = TxD -> out
	PORTF_OUTSET = 1<<3;	// TxD -> high
	PORTF_DIRCLR = 1<<2;	// PF2 = RxD -> in
	
	// Baudrate set   (115200=33,-1//2000000=0,0//57600=34,00//
	USARTF0_BAUDCTRLA = 33;												// tested to 2Mbit
	USARTF0_BAUDCTRLB = -1<<USART_BSCALE0_bp ;							// 
	
	//USART F0 for USB initialisieren	
	USARTF0_CTRLC = (1<<USART_SBMODE_bp)|(0b011<<USART_CHSIZE0_bp);		/* Set frame format: 8data, 2stop bit */
	USARTF0_CTRLA |= (0b10<<USART_RXCINTLVL0_bp);						/* Receive Interrupt enable & interrupt prio medium */
	USARTF0_CTRLA |= (0b10<<USART_TXCINTLVL0_bp);						/* Transmit Interrupt enable */
	USARTF0_CTRLB = (1<<USART_RXEN_bp)|(1<<USART_TXEN_bp);				/* Enable receiver and transmitter */	
	
// ------ SPIs initialisieren ---------------------------------------------------------------------------------------
	//SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0)| (1<<SPR1);		/* SPE=1 DORD=0 MSTR=1 CPOL=0 CPHA=0 SPR1=1 SPR0=1 */
	//SPI for ADC
	PORTC_DIRSET = 0b10110000;		// MOSI & SCLK & SS as Output
	SPIC_CTRL = 0b01010100;			// 0 clock double - 1 enable - 0 msb first - 00 clk rising - 00 prescaler 1/4
	//SPI for DAC
	PORTD_DIRSET = 0b10110000;		// MOSI & SCLK & SS as Output
	SPID_CTRL = 0b01010100;			// 0 clock double - 1 enable - 0 msb first - 00 clk rising - 00 prescaler 1/4
	//SPI for Modules
	PORTE_DIRSET = 0b10111111;
	SPIE_CTRL = 0b01010001;			// 0 clock double - 1 enable - 0 msb first - 00 clk rising - 01 prescaler 1/16
	
	
	
// ------ Interrupt System init -------------------------------------------------------------------------------------
	PMIC_CTRL = 0b00000111;										//low/mid/high prio Ints enabled
	
	sei();
}


void TimerInit (void)
{
	// Timer F als Systemtimer 100us konfigurieren
	TCF0_CTRLA = 0b0001;					// prescaler x1
	TCF0_CTRLB = 0;							// switch CTC Mode on, CS10-CS12 == clock select CLKIO/1
	TCF0_PER   =  (F_CPU / 10000) - 1;      // period Register compare value: 1/10000 of CPU frequency
	TCF0_INTCTRLA = 3;						// Overflow/underflow interrupt enable & prio high	
}

void InitIO (void)
{
	PORTB_OUT = 0b00000000;   												/* activate all pull-ups */
	PORTB_DIR = 0b00000000;       											/* all pins input */
	PORTC_OUT = 0b00000000;   												/* AD ports Tristate setzen */
	PORTC_DIR = 0b00000000;       											/* all pins */
	PORTD_OUT = 0b00000000;  												/* PD2 - PD7 für Relais GateModul*/
	PORTD_DIR = 0b00000000;

}

ISR (TCF0_OVF_vect)													//Timer 1 compare handler wird F_Interupt / sec aufgerufen
{
	MainTimer1000ms += 1;
	MainTimer100ms += 1;
	MainTimerxms +=1;
	MainTimer10ms +=1;
	MainTimer1ms +=1;
	MainTimer100us += 1;
	MainTime += 1;
	SystemTime100u += 1;
};

ISR(USARTF0_RXC_vect)														//Receive Ready interrupt
{
	SerInBuf[SerInBufHigh] = USARTF0_DATA;
	++SerInBufHigh;
	if (SerInBufHigh >= SerInBufMax)
	{
		SerInBufHigh = 0;
	}
}

ISR(USARTF0_TXC_vect)														//Transmit Ready interrupt
{
	if (SerOutBufLow != SerOutBufHigh)
	{		
		USARTF0_DATA = SerOutBuf[SerOutBufLow];
		++SerOutBufLow;
		if (SerOutBufLow >= SerOutBufMax)
		{
			SerOutBufLow = 0;
		}
	}
}

ISR(SPIC_INT_vect)
{}
	