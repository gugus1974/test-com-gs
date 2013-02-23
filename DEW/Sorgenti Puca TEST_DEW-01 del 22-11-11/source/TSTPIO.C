/*****************************************************************************
* Mar.09.sett.03           v.1.00   sw_st\sw_stb\test_stbxx\source\tstpio.c  *
******************************************************************************
* Routine per test della dual port ram                                       *
******************************************************************************
* 						                             *
*****************************************************************************/

#include <stdio.h>
#include "serbuf.h"
#include "consolle.h"
#include <reg167.h>
#include <bitfield.h>
#include <handshk.h>
#include <shd_var.h>
#include "tstflash.h"   
#include <cod_st.h>

#pragma ot(3,SPEED)



sbit	P7_5	= P7^5;
sbit	DP7_5	= DP7^5;
sbit	ODP7_5= ODP7^5;
sbit	P7_6	= P7^6;
sbit	DP7_6	= DP7^6;
sbit	ODP7_6= ODP7^6;
sbit	P7_7	= P7^7;
sbit	DP7_7	= DP7^7;
sbit	ODP7_7= ODP7^7;

/* costante di ritardo per consentire la stampa dei messaggi */
#define  WAIT    15000



