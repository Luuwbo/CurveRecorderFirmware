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
 
 void BefInt ()
 {
	static char aa;
	static char bf;
	static char AtrChars;
	static uint16_t bfcnt;								// Timeout Counter für BefehlsKennung
	static char sAtr[8];
	uint8_t l;
	static uint16_t atrcnt;
	//char hh;
		
		if (aa != 33)									// ! kein Befehl in Sicht
		{
			aa = Com_Debug_ReadCharFromInBuffer ();
			bf=0;
			bfcnt = 0;
			atrcnt = 0;
		}
		else  
		{	/* Befehl aktiv */
			if ((bf == 0) && (bfcnt <= 100) )
			{	
				bf = Com_Debug_ReadCharFromInBuffer ();
				bfcnt++;
			}
			else
			{
				/* Befehl Kennung aktiv */
				switch (bf)
				{
					case 63:	
					{
						Com_Debug_AddCharToBuffer(62);
						aa=0;
						bf=0;
						break;	//Befehl ? wird mit ">" beantwortet 
					}

					case 68:	// Befehl "D" für DrainSpannung setzen
					{
						AtrChars = 6;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<6;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							i32_UDs = strtol(sAtr,0,10);
							KsK_SetRegUDmV(i32_UDs);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							} 
						}
						break;
					}

					case 71:	// Befehl "G" für GateSpannung 
					{
						AtrChars = 6;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<6;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							i32_UGs = strtol(sAtr,0,10);
							KsK_SetRegUGmV (i32_UGs);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							} 
						}
						break;
					}

					case 72:	// Befehl "H" für UGv GateSpannungsbereich per Relais setzen
								// aktuell high = 1(2,048V), Low = 0 (12V) 
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_UGvVoltageRange = atoi(sAtr);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							}
						}
						break;
					}

					case 73:	// Befehl "I" für UG Messempfindlichkeit per Relais setzen
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_UGMeasInputRange = atoi(sAtr);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							}
						}
						break;
					}

					case 74:	// Befehl "J" für RG per Relais setzen
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars <= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_RGRange = atoi(sAtr);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							}
						}
						break;
					}

					case 75:	// Befehl "K" für UD Spannungsbereich per Relais setzen
								// aktuell high = 1(2,5V), Low = 0 (25V)
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_UDVoltageRange = atoi(sAtr);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							}
						}
						break;
					}

					case 80:	// Befehl "P" löst Messpuls aus
					{
						ui8_PulsCycle = 1;		// Puls auslösen
						aa=0;
						bf=0;
						break;
					}

					case 82:	// Befehl "R" Rs setzen
					{
						
						AtrChars = 2;
						atrcnt++;
						if (AtrChars <= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_RSRange = atoi(sAtr);
							aa=0;
							bf=0;
						}
						else
						{
							if (atrcnt >= 200)  //Timeout für Atrribut lesen
							{
								aa = 0;
								bf = 0;
								atrcnt = 0;
							}
						}
						break;
					}

					case 83:	// Befehl "S" löst statische Messung aus
					{
						ui8_StatCycle = 1;		// Puls auslösen
						aa=0;
						bf=0;
						break;
					}

					case 98:	// Befehl "b" für RG an PC senden
					{
						Com_Debug_AddCharToBuffer(98);
						switch (ui8_RGRange)
						{
							case 0:
							{
								Com_Debug_AddIntToBuffer(1000000,10);
								break;
							}
							case 1:
							{
								Com_Debug_AddIntToBuffer(99099,10);
								break;
							}
							case 2:
							{
								Com_Debug_AddIntToBuffer(9901,10);
								break;
							}
							case 3:
							{
								Com_Debug_AddIntToBuffer(999,10);
								break;
							}
							case 4:
							{
								Com_Debug_AddIntToBuffer(100,10);
								break;
							}
							default:
							{
								Com_Debug_AddIntToBuffer(1000000,10);
							}
						}
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}

					case 100:	// Befehl "d" für UD an PC senden
					{
						Com_Debug_AddCharToBuffer(100);
						Com_Debug_AddIntToBuffer(i32_UD,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}

					case 103:	// Befehl "g" für UG an PC senden
					{
						Com_Debug_AddCharToBuffer(103);
						Com_Debug_AddIntToBuffer(i32_UG,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}

					case 114:	// Befehl "r" für RS an PC senden
					{
						Com_Debug_AddCharToBuffer(114);
						switch (ui8_RSRange)
						{
							case 1:
							{
								Com_Debug_AddIntToBuffer(10,10);
								break;
							}
							case 2:
							{
								Com_Debug_AddIntToBuffer(100,10);
								break;
							}
							case 3:
							{
								Com_Debug_AddIntToBuffer(1000,10);
								break;
							}
							case 4:
							{
								Com_Debug_AddIntToBuffer(10000,10);
								break;
							}
							default:
							{
								Com_Debug_AddIntToBuffer(10,10);
							}
						}
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;

					}

					case 115:	// Befehl "s" für US an PC senden
					{
						Com_Debug_AddCharToBuffer(115);
						Com_Debug_AddIntToBuffer(i32_US,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}

					case 118:	// Befehl "v" für UGv an PC senden
					{
						Com_Debug_AddCharToBuffer(118);
						Com_Debug_AddIntToBuffer(i32_UGv,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}

					default:	aa=0;bf=0;break;
				}			
			}
		} 
 }