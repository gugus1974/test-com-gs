/************************************** definizione dei codici di Stato test *********************************************/
/* Test Flash total erase chip 1 KO */ 
#define  TFLATOTERSCH1KO 	0x0002
/* Test Flash total erase chip 2 KO */ 
#define  TFLATOTERSCH2KO 	0x0006
/* Test Flash program chip 1 KO */ 
#define  TFLAPRGCH1KO    	0x000A
/* Test Flash program chip 2 KO */ 
#define  TFLAPRGCH2KO    	0x000E
/* Test Flash forzata exit da total erase */ 
#define  TFLAFZEXITTOTERS 	0x0012
/* Test Flash forzata exit da copy ram2fla */ 
#define  TFLAFZEXITR2F	 	0x0016
/* Test Flash copy ram2fla OK*/ 
#define  TFLACOPYR2FOK	 	0x0019
/* Test Flash copy ram2fla KO*/ 
#define  TFLACOPYR2FKO	 	0x001A
/* Test Flash sector erase chip 1 KO */ 
#define  TFLASCTERSCH1KO 	0x001E
/* Test Flash sector erase chip 2 KO */ 
#define  TFLASCTERSCH2KO 	0x0022
/* Test Flash forzata exit da sector erase */ 
#define  TFLAFZEXITSCTERS 	0x0026
/* Test Flash forzata exit da write-verify55*/
#define  TFLAFZEXITWRVR55 	0x002A
/* Test Flash forzata exit da write-verifyAA */ 
#define  TFLAFZEXITWRVRAA 	0x002B
/* Test Flash write-verify OK */ 
#define  TFLAWRVROK 		0x0029
/* Test Flash verify FF KO */ 
#define  TFLAFVRFFKO 		0x002E

/* Test Flash write 55 KO */ 
#define  TFLAFWR55KO 		0x002F
/* Test Flash verify 55 KO */ 
#define  TFLAFVR55KO 		0x0030
/* Test Flash write AA KO */ 
#define  TFLAFWRAAKO 		0x0031
/* Test Flash verify AA KO */ 
#define  TFLAFVRAAKO 		0x0032


/* Test Flash verify 0 KO */ 
#define  TFLAVR0KO 		0x0033
/* Test SIO OK */ 
#define  TSIOOK 		0x0201
/* Test SIO KO */ 
#define  TSIOKO 		0x0202

/* Test EEPROM OK */ 
#define  TE2PROMOK 		0x0203

/* Test EEPROM KO */ 
#define  TE2PROMKO 		0x0204

/* Test IO OK */ 
#define  TESTIOOK 		0x0300

/* Test IO KO */ 
#define  TESTIOKO 		0x0301
/* Test MVB  SW1 OK */ 
#define  TESTSW1OK 		0x0301



/* Test ADC OK */ 
#define  TADCOK 		0x0601
/* Test ADC KO */ 
#define  TADCKO 		0x0602

#define  TESTMVB_LINE_A_OK 		0x0610
#define  TESTMVB_LINE_A_KO 		0x0611
#define  TESTMVB_LINE_A_OFF_OK 	0x0612
#define  TESTMVB_LINE_A_OFF_KO 	0x0613

#define  TESTMVB_LINE_B_OK 		0x0620
#define  TESTMVB_LINE_B_KO 		0x0621
#define  TESTMVB_LINE_B_OFF_OK 	0x0622
#define  TESTMVB_LINE_B_OFF_KO 	0x0623



/* Test PIO OK */
#define  TPIOOK 		0x0710
/* Test PIO KO */ 
#define  TPIOKO 		0x0712

/* Test LED OK */
#define  TESTLED_OK 		0x0701

/* Test LED KO */
#define  TESTLED_KO 		0x0702


/* Test MVB Service Area Shift 0 KO */ 
#define  TMVBSASHF0KO 		0x0902
/* Test MVB Service Area Shift 0 OK */ 
#define  TMVBSASHF0OK 		0x0901
/* Test MVB Service Area Shift 1 KO */ 
#define  TMVBSASHF1KO 		0x0906
/* Test MVB Service Area Shift 1 OK */ 
#define  TMVBSASHF1OK 		0x0905
/* Test MVB Service Area March C- KO */ 
#define  TMVBSAMARCHKO 	        0x090A
/* Test MVB Service Area March C- OK */ 
#define  TMVBSAMARCHOK	 	0x0909
/* Test MVB Service Area Massimo rumore KO */ 
#define  TMVBSAMXRUMKO	 	0x090E
/* Test MVB Service Area Massimo rumore OK */ 
#define  TMVBSAMXRUMOK 		0x090D
/* Test MVB Traffic Memory Shift 0 KO */ 
#define  TMVBTMSHF0KO 		0x0912
/* Test MVB Traffic Memory Shift 0 OK */ 
#define  TMVBTMSHF0OK 		0x0911
/* Test MVB Traffic Memory Shift 1 KO */ 
#define  TMVBTMSHF1KO 		0x0916
/* Test MVB Traffic Memory Shift 1 OK */ 
#define  TMVBTMSHF1OK 		0x0915
/* Test MVB Traffic Memory March C- KO */ 
#define  TMVBTMMARCHKO 	        0x091A
/* Test MVB Traffic Memory March C- OK */ 
#define  TMVBTMMARCHOK	 	0x0919
/* Test MVB Traffic Memory Massimo rumore KO */ 
#define  TMVBTMMXRUMKO	 	0x091E
/* Test MVB Traffic Memory Massimo rumore OK */ 
#define  TMVBTMMXRUMOK 		0x091D
/* Test MVB Selettore indirizzi KO */ 
#define  TMVBSELKO 		0x0922
/* Test MVB Selettore indirizzi OK */ 
#define  TMVBSELOK 		0x0921
/* Test MVB BUS KO */ 
#define  TMVBBUSKO 		0x0926
/* Test MVB BUS OK */ 
#define  TMVBBUSOK 		0x0925
/* Test UARTA in TX UARTB in RX KO */ 
#define  TTXUARTAKO 		0x0A02
/* Test UARTA in TX UARTB in RX OK */ 
#define  TTXUARTAOK 		0x0A01
/* Test UARTB in TX UARTA in RX KO */ 
#define  TTXUARTBKO 		0x0A06
/* Test UARTB in TX UARTA in RX OK */ 
#define  TTXUARTBOK 		0x0A05
/* Test UARTC in TX UARTD in RX KO */ 
#define  TTXUARTCKO 		0x0A0A
/* Test UARTC in TX UARTD in RX OK */ 
#define  TTXUARTCOK 		0x0A09
/* Test UARTD in TX UARTC in RX KO */ 
#define  TTXUARTDKO 		0x0A0E
/* Test UARTD in TX UARTC in RX OK */ 
#define  TTXUARTDOK 		0x0A0D










                                     
