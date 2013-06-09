* BD32-Treiber zum Löschen externer Flashes (16-Bit breit)
* vorher wurden bereits die Chip-Selects definiert
* A4 wird für den Offset verwendet.

	CHIP	68360

	include ipd.inc
NUM_VEC	EQU	40	Anzahl Exception Vektoren
S9REC	EQU	2

* Anfang des Treibers
START	DC.L	PROG_START	Anfang des Treibers
VEC_TAB	DS.L	NUM_VEC	Exception Vektoren
Start_Msg	DC.B	'Erasing of 16-Bit AMD-Flashes',13,10,0
	DS.W	0

PROG_START	LEA.L	(STACK,PC),A7	Stackpointer definieren
	MOVE.L	D0,D7	Anzahl Parameter
	MOVE.L	A0,A6	Pointer auf die Parameter

*Vektortabelle aufbauen
	LEA.L	(START,PC),A1
	MOVEC	A1,VBR
	ADDQ.L	#4,A1
	LEA.L	(EXCEPT,PC),A2	Adresse Exception Handler
	MOVEQ	#NUM_VEC-1,D1	Anzahl Vektoren (DBF!)
INIT_VEC	MOVE.L	A2,(A1)+
	DBF	D1,INIT_VEC

* Message ausgeben
	LEA.L	(Start_Msg,PC),A0
	MOVEQ	#BD_PUTS,D0	function call
	BGND

	LEA.L	0,A4	Offset := 0

* Parameter prüfen
	CMP.W	#1,D7	Anzahl Parameter
	BEQ.B	ERASE
	CMP.W	#2,D7	Gibt es einen Offset?
	BNE	PAR_ERR	bei <>1 Fehler
* Offset einlesen
	MOVE.L	(4,A6),A0	Pointer auf Offset
	MOVEQ	#BD_EVAL,D0	function call EVAL Param
	BGND
	TST.W	D0
	BNE	OFFS_ERR	Error: Fehler im Offset
	MOVE.L	D1,A4	Offset merken


* jetzt löschen
ERASE	MOVE.W	#$AAAA,($5555*2,A4)	löschen gem. AMD-Handbuch
	MOVE.W	#$5555,($2AAA*2,A4)
	MOVE.W	#$8080,($5555*2,A4)
	MOVE.W	#$AAAA,($5555*2,A4)
	MOVE.W	#$5555,($2AAA*2,A4)
	MOVE.W	#$1010,($5555*2,A4)
	NOP
	NOP
* warten, bis fertig
WAIT	MOVEQ	#-1,D0	kurze Pause
PAUSE	 TST.W	D0
	DBF	D0,PAUSE
	MOVE.W	#$2E,D1	Indikation auf dem Schirm
	MOVEQ	#BD_PUTCHAR,D0
	BGND
	MOVE.W	(A4),D0	erste Speicherzelle im Flash lesen
	MOVE.L	D0,D1	und merken
	CMP.W	#$FFFF,D0
	BEQ	ERA_OK	bei $FFFF ist alles in Ordnung
* jetzt prüfen, ob bei einem Baustein Zeitüberschreitung vorliegt
* erst unteres Byte testen
	AND.W	#$FF,D1
	CMP.W	#$FF,D1	Byte gelöscht?
	BEQ.B	TST_HIGH	Low-Byte ist schon gelöscht

	AND.W	#$A0,D1 D7+D5
	BEQ	WAIT	Wenn D7=0 und D5=0, dann noch nicht fertig
	AND.W	#$20,D1	auf Fehler prüfen (D5=1)?
	BNE	ERA_ERR	Time Limit im unteren Byte
* dasselbe für das obere Byte
TST_HIGH	AND.W	#$FF00,D1
	CMP.W	#$FF00,D1	Byte gelöscht?
	BEQ.B	ERA_OK	High-Byte ist schon gelöscht
	AND.W	#$A000,D0
	BEQ	WAIT	D7=0, D5=0, dann noch warten
	AND.W	#$2000,D0	Fehlerbit abfragen
	BNE	ERA_ERR

* Fertig mit dem Löschen
ERA_OK	CLR.L	D7	kein Fehler
ERA_END	MOVE.L	D7,D1	vorheriger Fehler
	MOVEQ	#BD_QUIT,D0	Fertig
	BGND

OFFS_ERR	MOVEQ	#4,D7	Fehler im Offset
	BRA	ERA_END

EXCEPT_ERR	MOVEQ	#3,D7	Exception aufgetreten
	BRA	ERA_END

ERA_ERR	BSR	AMD_RESET	
	MOVEQ	#2,D7	Eine Speicherzelle konnte nicht gelöscht werden
	BRA	ERA_END

PAR_ERR	MOVEQ	#1,D7	Anzahl Parameter stimmt nicht
	BRA	ERA_END

* Handler für beliebige Exceptions
EXCEPT	BRA	EXCEPT_ERR	nur Fehlermeldung ausgeben

AMD_RESET	
	MOVE.W	#$AAAA,($5555*2,A4)	Spezialzyklen gem. AMD-Handbuch
	MOVE.W	#$5555,($2AAA*2,A4)	speziell für Byteweisen Anschlu· von
	MOVE.W	#$F0F0,($5555*2,A4)	16-Bit breiten Flashes
	MOVE.L	(A4),D0	Dummy-Lesen
	RTS

* Ende des Programms
	DS.W	100	Stack ist 100 Worte groß
STACK	DS.W	1

	END
