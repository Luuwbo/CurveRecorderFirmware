/*
 * BefehlsInterpreter.c
 *
 * Created: 23.03.2019 15:01:46
 *  Author: Uwe
 */ 


 #include <avr/io.h>
 #include <stdlib.h>
 #include "Com_Debug.h"
 #include "KennLinienSchreiber.h"
 #include "Befehlsinterpreter.h"
 
 // Format: "!" + Kennung + Attribut (feste Zeichenzahl)
 // !?			-> Antwort ">"
 // !D xxxxxx	-> UD [mV] setzen (6 Zeichen)
 // !G xxxxxx	-> UG [mV] setzen (6 Zeichen)
 // !H xx		-> UGv Gatespannungsbereich (Relais) [0,1], Sollwert wird neu skaliert
 // !I xx		-> UG Messempfindlichkeit (Relais) [0,1]
 // !J xx		-> RG per Relais setzen
 // !K xx		-> UD Spannungsbereich [0,1,2], Sollwert wird neu skaliert
 // !P			-> Messimpuls ausloesen
 // !R xx		-> RS per Relais setzen
 // !S			-> statische Messung ausloesen
 // !W xxx		-> Pulsweite [x 100us]
 // !X xx		-> VG statisch (=1) / dynamisch (=0)
 // !Y xx		-> VD statisch (=1) / dynamisch (=0)
 // !Z xxxxxxyyyyyy -> UD und UG [mV] zusammen setzen (2 x 6 Zeichen)
 // !b !d !g !r !s !t !v	-> RG / UD / UG1 / RS / URS / US / UGv1 an PC senden
 // !z			-> alle Daten en Block senden

 #define c_AtrTimeout 200							// Timeout fuer Attribut lesen [Aufrufe a 100us]

 static char aa;									// '!' = Befehl aktiv
 static char bf;									// Befehlskennung
 static uint16_t bfcnt;								// Timeout Counter fuer BefehlsKennung
 static uint16_t atrcnt;							// Timeout Counter fuer Attribut
 static char sAtr[16];								// Attribut-String (nullterminiert)

 // Befehl abgeschlossen oder verworfen
 static void BefEnde(void)
 {
	aa = 0;
	bf = 0;
 }

 // Liest n Attributzeichen nach sAtr und terminiert den String.
 // Rueckgabe 1: Attribut vollstaendig gelesen; 0: noch nicht genug Zeichen im Puffer.
 // Kommen die Zeichen nicht innerhalb des Timeouts, wird der Befehl verworfen.
 static uint8_t BefReadAtr(uint8_t n)
 {
	uint8_t l;
	atrcnt++;
	if (n <= Com_Debug_CharsInBuffer())
	{
		for (l = 0; l < n; l++)
		{
			sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
		}
		sAtr[l] = 0;								// String terminieren (Reste vom letzten Befehl abschneiden)
		return 1;
	}
	if (atrcnt >= c_AtrTimeout)
	{
		BefEnde();
		atrcnt = 0;
	}
	return 0;
 }

 // Befehl mit n Attributzeichen, Wert direkt in eine uint8-Variable
 static void BefSetU8(uint8_t *ziel, uint8_t n)
 {
	if (BefReadAtr(n))
	{
		*ziel = atoi(sAtr);
		BefEnde();
	}
 }

 // Antwort: Kennung + Zahl dezimal + LF CR
 static void BefSendVal(char kennung, int32_t wert)
 {
	Com_Debug_AddCharToBuffer(kennung);
	Com_Debug_AddIntToBuffer(wert,10);
	Com_Debug_AddCharToBuffer(10);					// LineFeed
	Com_Debug_AddCharToBuffer(13);					// CarriageReturn
	BefEnde();
 }

 void BefInt (void)
 {
	uint8_t v;

	if (aa != '!')									// kein Befehl in Sicht
	{
		aa = Com_Debug_ReadCharFromInBuffer ();
		bf = 0;
		bfcnt = 0;
		atrcnt = 0;
		return;
	}
	if ((bf == 0) && (bfcnt <= 100))				// Befehl aktiv, auf Kennung warten
	{
		bf = Com_Debug_ReadCharFromInBuffer ();
		bfcnt++;
		return;
	}

	/* Befehl Kennung aktiv */
	switch (bf)
	{
		case '?':									// Befehl ? wird mit ">" beantwortet
			Com_Debug_AddCharToBuffer('>');
			BefEnde();
			break;

		case 'D':									// DrainSpannung setzen
			if (BefReadAtr(6))
			{
				i32_UDs = strtol(sAtr,0,10);
				KsK_SetRegUDmV(i32_UDs);
				BefEnde();
			}
			break;

		case 'G':									// GateSpannung G1 setzen
			if (BefReadAtr(6))
			{
				i32_UGs = strtol(sAtr,0,10);
				KsK_SetRegUGmV(i32_UGs);
				BefEnde();
			}
			break;

		case 'H':									// UGv Gatespannungsbereich per Relais, aktuell high = 0 (2,400V), low = 1 (24V)
			if (BefReadAtr(2))
			{
				v = atoi(sAtr);
				if (v <= 1)							// nur gueltige Bereiche 0,1 uebernehmen
				{
					ui8_UGvVoltageRange = v;
					KsK_SetRegUGmV(i32_UGs);		// Sollwert fuer neuen Bereich neu skalieren
				}
				BefEnde();
			}
			break;

		case 'I':	BefSetU8(&ui8_UGMeasInputRange, 2);	break;	// UG Messempfindlichkeit per Relais
		case 'J':	BefSetU8(&ui8_RGRange, 2);			break;	// RG per Relais

		case 'K':									// UD Spannungsbereich per Relais, 0 = 2,5V / 1 = 25V / 2 = 250V
			if (BefReadAtr(2))
			{
				v = atoi(sAtr);
				if (v <= 2)							// nur gueltige Bereiche 0,1,2 uebernehmen
				{
					ui8_UDVoltageRange = v;
					KsK_SetRegUDmV(i32_UDs);		// Sollwert fuer neuen Bereich neu skalieren
				}
				BefEnde();
			}
			break;

		case 'P':									// Messpuls ausloesen
			ui8_PulsCycle = 1;
			BefEnde();
			break;

		case 'R':	BefSetU8(&ui8_RSRange, 2);		break;	// RS per Relais

		case 'S':									// statische Messung ausloesen
			ui8_StatCycle = 1;
			BefEnde();
			break;

		case 'W':	BefSetU8(&ui8_PulsWidth, 3);	break;	// Pulsweite
		case 'X':	BefSetU8(&ui8_UGstatic, 2);		break;	// VG statisch (=1) oder dynamisch (=0)
		case 'Y':	BefSetU8(&ui8_UDstatic, 2);		break;	// VD statisch (=1) oder dynamisch (=0)

		case 'Z':									// UD und UG zusammen setzen
			if (BefReadAtr(12))
			{
				i32_UGs = strtol(&sAtr[6],0,10);	// zweite Haelfte zuerst, dann String nach 6 Zeichen trennen
				sAtr[6] = 0;
				i32_UDs = strtol(sAtr,0,10);
				KsK_SetRegUDmV(i32_UDs);
				KsK_SetRegUGmV(i32_UGs);
				BefEnde();
			}
			break;

		case 'b':	BefSendVal('b', KsK_RG_Ohm(ui8_RGRange));	break;	// RG an PC senden
		case 'd':	BefSendVal('d', i32_UD);					break;	// UD an PC senden
		case 'g':	BefSendVal('g', i32_UG1);					break;	// UG an PC senden
		case 'r':	BefSendVal('r', KsK_RS_Ohm(ui8_RSRange));	break;	// RS an PC senden
		case 's':	BefSendVal('s', i32_URS);					break;	// URS an PC senden
		case 't':	BefSendVal('t', i32_US);					break;	// US an PC senden
		case 'v':	BefSendVal('v', i32_UGv1);					break;	// UGv an PC senden

		case 'z':									// alle Daten en Block zum PC senden
			SendDataAll();
			BefEnde();
			break;

		default:
			BefEnde();
			break;
	}
 }
