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
 
 // !D[68]xxxxx -> UD [mV] setzen / string 6 byte /
 // !G[71]xxxxx -> UG [mV] setzen / string 6 byte / 
 // !H[72]x		-> Gatespannungsbereich setzen (Relais)
 // !I[73]x		-> Ug Messempfindlichkeit (Relais)
 // !J[74]x		-> RG per Relais setzen
 // !K[75]xxxxx	-> UD Spannungsbereich 
 // !P[80]x		-> Messimpuls auslösen
 // !W[87]x		-> Pulsweite einstellen
 // !X[88]x		-> VG static (=1)
 // !X[89]x		-> VD static (=1)
 // !s[116]x	-> US senden
 // !z[122]x		-> alle Daten en Block senden
 
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

					case 71:	// Befehl "G" für GateSpannung G1
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
								// aktuell high = 0(2,400V), Low = 1 (24V) 
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
					
					case 87:	// Befehl "W" Pulsweite setzen
					{
						AtrChars = 3;
						atrcnt++;
						if (AtrChars <= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_PulsWidth = atoi(sAtr);
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
					
					case 88:	// Befehl "X" setzt VG auf statisch (=1) oder dynamisch (=0)
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars <= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_UGstatic = atoi(sAtr);
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
					
					case 89:	// Befehl "Y" setzt VD auf statisch (=1) oder dynamisch (=0)
					{
						AtrChars = 2;
						atrcnt++;
						if (AtrChars <= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<AtrChars;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							ui8_UDstatic = atoi(sAtr);
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
					
					case 90:  // Befehl "Z" setzt UD und UGv zusammen
					{
						AtrChars = 12;
						atrcnt++;
						if (AtrChars<= Com_Debug_CharsInBuffer())
						{
							for(l=0;l<6;l++)
							{
								sAtr[l] = Com_Debug_ReadCharFromInBuffer ();
							}
							i32_UDs = strtol(sAtr,0,10);
							KsK_SetRegUDmV(i32_UDs);
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
						Com_Debug_AddIntToBuffer(i32_UG1,10);
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
							case 5:
							{
								Com_Debug_AddIntToBuffer(100000,10);
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

					case 115:	// Befehl "s" für URS an PC senden
					{
						Com_Debug_AddCharToBuffer(115);
						Com_Debug_AddIntToBuffer(i32_URS,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}
					case 116:	// Befehl "t" für US an PC senden
					{
						Com_Debug_AddCharToBuffer(116);
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
						Com_Debug_AddIntToBuffer(i32_UGv1,10);
						Com_Debug_AddCharToBuffer(10);					// LineFeed
						Com_Debug_AddCharToBuffer(13);					// LineFeed
						aa=0;
						bf=0;
						break;
					}
					
					case 122:	// Befehl "z" für alle Daten en Block zum PC senden
					{
						SendDataAll();
						aa=0;
						bf=0;
						break;
					}
					default:	aa=0;bf=0;break;
				}			
			}
		} 
 }