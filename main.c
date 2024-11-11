/*
 * HL-Kennlinienschreiber.c
 *
 * Created: 03.02.2019 09:35:30
 * Author : Uwe
 */ 
 
 #define CPU_SPEED 16000000
 #define F_CPU 16000000UL

 #include <avr/io.h>
 #include <avr/wdt.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <util/twi.h>
 #include <avr/interrupt.h>
 #include <math.h>
 #include <string.h>
 
 // Globale Variablen
 
 #include "i2cmaster.h"
 #include "Com_Debug.h"
 #include "KennLinienSchreiber.h"
 #include "Befehlsinterpreter.h"
 
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
 
uint16_t MainTimer1000ms;
uint16_t MainTimer100ms;
uint8_t MainTimerxms;
uint8_t MainTimer10ms;
uint8_t MainTimer1ms;
uint16_t MainTime;

char sh[10];
uint8_t PowerToggleTimer;
//System
uint8_t ui8_PulsCycle;				//Statusvariable für Pulsausgabe

//Test

int32_t DACvalTest;
uint8_t av;

int main(void)
{
	InitIO();
	InitCPU();
	
	TimerInit();
	i2c_init();
	

	DACvalTest = 0;
	av = 0;
	ui8_PulsCycle = 255;

	SetLTC1655Output(1,32768);
	SetLTC1655Output(2,32768);

    while (1) 
    {
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

}
void Loop10ms (void)
{
	KsK_SetRelais();

}
void Loopxms()
{
	KsK_PulseMeas();
	KsK_StatMeas();
}
void Loop1ms (void)
{
	BefInt();
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
	ui8_UGvVoltageRange = 0;			
	ui8_UGMeasInputRange = 0;		
}

void InitCPU (void)
{
	cli();
// Baudrate auf 57600 baud einstellen
	UBRR0H = 0;
	UBRR0L = 16;	//16MHz
	//UBRR0L = 51;	//8MHz
//Schnittstelle für USB initialisieren	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);			/* Enable receiver and transmitter */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);		/* Set frame format: 8data, 2stop bit */
	UCSR0B |= (1<<RXCIE0);					/* Receive Interupt enable */
	UCSR0B |= (1<< TXCIE0);					/* Transmit Interupt enable */
// SPI initialisieren
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0)| (1<<SPR1);		/* SPE=1 DORD=0 MSTR=1 CPOL=0 CPHA=0 SPR1=1 SPR0=1 */
	sei();
}

void TimerInit (void)
{
	// Timer 1 als Systemtimer 100us konfigurieren
	TCCR1B = (1<<WGM12) | (1<<CS10);									// switch CTC Mode on, CS10-CS12 == clock select CLKIO/1
	OCR1A   =  (F_CPU / 10000) - 1;                                     // Output Compare Register 1 A / compare value: 1/10000 of CPU frequency
	TIMSK1 = (1<<OCIE1A);												// Timer/Counter1, Output Compare A Match Interrupt Enable
}

void InitIO (void)
{
	MCUCR |= (1<<PUD);													/*alle PullUps ausschalten*/
	PORTB = 0b00010000;   												/* activate all pull-ups */
	DDRB = 0b11101111;       											/* all pins input */
	PORTC = 0b11000000;   												/* AD ports Tristate setzen */
	DDRC = 0b00111111;       											/* all pins */
	PORTD = 0b00000011;  												/* PD2 - PD7 für Relais GateModul*/
	DDRD = 0b11111100;
	SPCR = 0b01010001;						/* SPE=1 DORD=0 MSTR=1 CPOL=0 CPHA=0 SPR1=0 SPR0=1 */

	PORTB |= (1 << PB1);					/* CS auf 1 */
	PORTB |= (1 << PB2);					/* CS auf 1 */
}

ISR (TIMER1_COMPA_vect)													//Timer 1 compare handler wird F_Interupt / sec aufgerufen
{
	MainTimer1000ms += 1;
	MainTimer100ms += 1;
	MainTimerxms +=1;
	MainTimer10ms +=1;
	MainTimer1ms +=1;
	MainTime += 1;
	SystemTime100u += 1;
};

ISR(USART_RX_vect)														//Receive Ready interrupt
{
	SerInBuf[SerInBufHigh] = UDR0;
	++SerInBufHigh;
	if (SerInBufHigh >= SerInBufMax)
	{
		SerInBufHigh = 0;
	}
}

ISR(USART_TX_vect)														//Transmit Ready interrupt
{
	if (SerOutBufLow != SerOutBufHigh)
	{		
		UDR0 = SerOutBuf[SerOutBufLow];
		++SerOutBufLow;
		if (SerOutBufLow >= SerOutBufMax)
		{
			SerOutBufLow = 0;
		}
	}
}