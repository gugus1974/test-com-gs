/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> (unmarked) SPrint_F() replaced with sprintf()                                       */
/* <ATR:02> removed the O_960 pragmas                                                           */
/* <ATR:03> Include "atr_map.h" with UIC Mapping Server spacific patches                        */
/* <ATR:04> read_event_buffer_to_term() commented out                                           */
/* <ATR:05> hex_asc() commented out                                                             */
/* <ATR:06> recorder() commented out                                                            */
/* <ATR:07> err_rec() commented out                                                             */
/*==============================================================================================*/

#include "atr_map.h"			/* <ATR:03> */

/*
   000000120
   MAP_NFU
   Wolfgang Landwehr
   11. Oktober 1995, 16:01
   3
 */

/*
   GENERAL TOP OF FILE
 */

   /*
      This module contains all functions for setup and update mode, needed in
      MAP_NPT.C (train bus) and MAP_NPV.C (vihicle bus).
    */

   /*
      20.05.94 W. Landwehr
      Start of developement.
    */

   /*
      Copyright (c) 1995 by ABB-Henschel
    */


/*
   MODUL HISTORY
 */

   /*
      1.00 20.05.94  W. Landwehr
      Functions are separated from map_npt.c.
      1.01 01.11.94  W. Landwehr
      Extended with MVB Recorder.
      1.02 20.01.95  W. Landwehr
      REC_UPDATE_WTB buffer is now 32 byte long.
      1.03 08.02.95  W. Landwehr
      Rule 4 for UIC numbering now is no longer depended on L_DIRECTION.
      1.04 22.02.95  W. Landwehr
      Adaption for WTB 1.9, Problems with compiler 3.5.
      1.05 05.04.95  W. Landwehr
      prefix map_ to all global var's.
      1.06 21.04.95  W. Landwehr  {---on train until 23.9.95, MAP 2.09---}
      function read_event_buffer.. second page corrected.
      1.07 11.10.95  W. Landwehr
      adoptions for MyFriend, remove DOS test features.
    */


/*
   COMPILER SWITCHES
 */

/* <ATR:02> */
// #ifdef O_960    
// #pragma noalign    
// #endif

/*
   INCLUDEFILES
 */
#include "am_sys.h"				/* am@@@ messenger */

#include "bmi.h"				/* TCN Control Interface */

#include "recorder.h"			/* record buffer */

#include "map.h"				/* includes for all map modules and user */
#include "map_ums.h"			/* includes for all map modules */


/*
   IMPLEMENTATION
 */

   /*
      EXTERNALS
    */
extern map_NodeConfigurationTable NodeKonf;

extern s_vw *p_map_vw;			/* global data MAP_NPT */

extern map_TrainTopography NADI;	/* NADI */

extern s_CommInfo map_ci;		/* common interface MAP_BMI, MAP_NPT */

extern unsigned short MAP_EVENT_BUF;	/* flag to display event buffer content */


   /*
      GLOBAL DATA
    */

	  /*
	     MAP internal trace buffers
	   */
	  /* b y t e s t r u c t u r e       ( 8 bit) */
s_rec15 map_REC_TCN;			/* Recorder TCN events */
s_rec15 map_REC_PHASE;			/* Recorder work phases */
s_rec15 map_REC_SETUP;			/* Test step in setup mode (0x10..0x4F) */
s_rec31 map_REC_UPDATE_WTB;		/* Test step in update mode (0x50..0x9F) */
s_rec15 map_REC_UPDATE_MVB;		/* Test step in update mode (0xA0..0xDF) */

	  /* w o r d s t r u c t u r e       (16 bit) */
s_rec7 map_REC_ORD;				/* Recorder order in update */
s_rec7 map_REC_ERR_CODE;		/* Recorder error.code */
s_rec7 map_REC_ERR_INFO;		/* Recorder error.info */


   /*
      PROTOTYPES
    */
//      void read_event_buffer_to_term( void );             /* <ATR:04> */
//      void hex_asc( unsigned short, unsigned char [] );   /* <ATR:05> */
//      void recorder( unsigned char );                     /* <ATR:06> */
//      void err_rec( void );                               /* <ATR:07> */
void ascend_range (char);
void descend_range (char);
void build_vrno (void);


   /*
      FUNCTIONS
    */

	  /*
	     read_event_buffer_to_term
	   */
/* <ATR:04> */
//         void read_event_buffer_to_term(void)
//    {
//    
//            /*
//            Writes the content of the event_buffer to the terminal.
//            */
//            NSE_EVENT_TYPE event_buffer[ 40 ];
//            short error, i, j, lines, offset;
//            char evbuf[ 60 ];
//            int *p_entries;
//            unsigned short hour, minute, second, msec;
//            unsigned long timer;
//    
//            error = nse_read ( NSE_READ_MODE_OLDEST, 40, event_buffer, p_entries);
//    
//            if ( MAP_EVENT_BUF == 1 )
//    {
//               lines = 24;
//               offset = 0;
//    
//               if ( *p_entries < 25 )
//    {
//                  lines = *p_entries;
//                  /* disable the starter */
//                  MAP_EVENT_BUF = 0;
//    }else{
//                  /* disable the starter */
//                  MAP_EVENT_BUF = 2;
//    }
//    }else{
//               /* 2nd time is last time */
//               lines = *p_entries - 24;
//               offset = 24;
//
//               /* disable the starter */
//               MAP_EVENT_BUF = 0;
//    }
//            write_serv("\n\n\r-------------Content of the event recorder-------------");
//            sprintf(evbuf,"\r-------------Entries: %2u-------------------------------",
//                     lines );
//            write_serv(evbuf);
//    
//            /*
//            WRITE LINES
//            */
//    
//               for (i = 0; i<lines; i++)
//    {
//    
//                  for (j = 0; j<60; j++)
//    {
//                     evbuf[ j ] = 0;
//    }
//                  timer  = event_buffer[i+offset].time;
//                  msec   = timer % 1000;
//                  second = ( timer / 1000 ) % 60;
//                  minute = ( timer / 1000 / 60 ) % 60;
//                  hour   = timer / 1000 / 60 / 60;
//    
//                  sprintf(evbuf, "\r%02u:%02u:%02u.%03u %s %s ",
//                                  hour, minute, second, msec,
//                                  &event_buffer[i+offset].name,
//                                  &event_buffer[i+offset].list);
//                  write_serv(evbuf);
//    }
//    
//            write_serv("\r-------------------------------------------------------");
//    
//            return;
//    }


	  /*
	     hex_asc
	   */
/* <ATR:05> */
//         void hex_asc( unsigned short ival, unsigned char aval[] )
//    {
//    
//            /*
//            Converts a 16 digit binary value into a 4 char ASCII value. 
//              input:  ival    e.g. '23AF' as 16 bit value.
//              output: aval    e.g. "23AF" as 4 char value.
//            */
//            unsigned char low, high, c;
//    
//            /* example: 'C37A' */
//            low  = ival % 256;  /*   7A */
//            high = ival / 256;  /* C3   */
//    
//            /*
//            value bits 0..3   { xxxX }
//            */
//               c = ( low  & 0x0F ) + 0x30; /* 0A + 30 = 3A */
//    
//               if( c > 0x39 )
//    {
//                  c += 7; /* 3A --> 41 ("A") */
//    }
//               aval[ 3 ] = c;
//    
//    
//            /*
//            value bits 4..7   { xxXx }
//            */
//               c = ( low  >> 4 ) + 0x30; /* 07 + 30 = 37 */
//    
//               if( c > 0x39 )
//    {
//                  c += 7; /* 37 --> 37 ("7") */
//    }
//               aval[ 2 ] = c;
//    
//    
//            /*
//            value bits 8..11  { xXxx }
//            */
//               c = ( high & 0x0F ) + 0x30; /* 03 + 30 = 33 */
//    
//               if( c > 0x39 )
//    {
//                  c += 7; /* 33 --> 33 ("3") */
//    }
//               aval[ 1 ] = c;
//    
//    
//            /*
//            value bits 12..15 { Xxxx }
//            */
//               c = ( high >> 4 ) + 0x30; /* 0C + 30 = 3C */
//    
//               if( c > 0x39 )
//    {
//                  c += 7; /* 3C --> 43 ("C") */
//    }
//               aval[ 0 ] = c;
//    
//            return;
//    }


	  /*
	     recorder
	   */
/* <ATR:06> */
//         void recorder( unsigned char type )
//    {
//    
//            /*
//            map internal test recorder, traces program steps.
//            */
//            short i;
//    
//#if defined ( RECORD )
//    
//    
//            switch( type )
//    {
//    
//               /*
//               case 0, 0xEE, 0xFF, 0xF1: initializes trace buffers
//               */
//    
//                  case 0:
//    {
//    
//                     /*
//                     Initialize all recorders.
//                     */
//    
//                        for ( i=0; i<MAX_REC15; i++ )
//    {
//                           map_REC_TCN.val[ i ] = 0xCC;
//                           map_REC_PHASE.val[ i ] = 0xDD;
//                           map_REC_SETUP.val[ i ] = 0xEE;
//                           map_REC_UPDATE_MVB.val[ i ] = 0xBB;
//    }
//    
//                        for ( i=0; i<MAX_REC31; i++ )
//    {
//                           map_REC_UPDATE_WTB.val[ i ] = 0xFF;
//    }
//    
//                        for ( i=0; i<MAX_REC7; i++ )
//    {
//                           map_REC_ORD.val[ i ] = 0x1111;
//                           map_REC_ERR_CODE.val[ i ] = 0x2222;
//                           map_REC_ERR_INFO.val[ i ] = 0x3333;
//    }
//                        map_REC_TCN.ix = MAX_REC15;
//                        map_REC_PHASE.ix = MAX_REC15;
//                        map_REC_SETUP.ix = MAX_REC15;
//                        map_REC_UPDATE_WTB.ix = MAX_REC31;
//                        map_REC_UPDATE_MVB.ix = MAX_REC15;
//                        map_REC_ORD.ix = MAX_REC7;
//                        map_REC_ERR_CODE.ix = MAX_REC7;
//                        map_REC_ERR_INFO.ix = MAX_REC7;
//    
//                        break;
//    
//    }
//    
//                  case 0xEE:
//    {
//    
//                     /*
//                     Initialize only setup recorder.
//                     */
//    
//                        for ( i=0; i<MAX_REC15; i++ )
//    {
//                           map_REC_SETUP.val[ i ] = 0xEE;
//    }
//                        map_REC_SETUP.ix = MAX_REC15;
//    
//                        break;
//    
//    }
//    
//                  case 0xFF:
//    {
//    
//                     /*
//                     Initialize only update WTB recorder.
//                     */
//    
//                        for ( i=0; i<MAX_REC31; i++ )
//    {
//                           map_REC_UPDATE_WTB.val[ i ] = 0xFF;
//    }
//                        map_REC_UPDATE_WTB.ix = MAX_REC31;
//    
//                        break;
//    
//    }
//    
//                  case 0xF1:
//    {
//    
//                     /*
//                     Initialize only update MVB recorder.
//                     */
//    
//                        for ( i=0; i<MAX_REC15; i++ )
//    {
//                           map_REC_UPDATE_MVB.val[ i ] = 0xBB;
//    }
//                        map_REC_UPDATE_MVB.ix = MAX_REC15;
//    
//                        break;
//    
//    }
//    
//    
//               case R_ERR:
//    {
//    
//                  /*
//                  Error recorder
//                  */
//                     i = map_REC_ERR_CODE.ix;
//    
//                     if ( ( map_REC_ERR_CODE.val[ i ] != p_map_vw->code ) ||
//                          ( map_REC_ERR_INFO.val[ i ] != p_map_vw->info ) )
//    {
//    
//                        if ( ++i >= MAX_REC7 )
//    {
//                           i = 0;
//    }
//                        map_REC_ERR_CODE.ix = i;
//                        map_REC_ERR_INFO.ix = i;
//                        map_REC_ERR_CODE.val[ i ] = p_map_vw->code;
//                        map_REC_ERR_INFO.val[ i ] = p_map_vw->info;
//    }
//                     break;
//    
//    }
//    
//               case R_TCN:
//    {
//    
//                  /*
//                  Recorder for TCN events.
//                  */
//                     i = map_REC_TCN.ix;
//    
//                     if ( map_REC_TCN.val[ i ] != map_ci.bmi_event )
//    {
//    
//                        if ( ++i >= MAX_REC15 )
//    {
//                           i = 0;
//    }
//                        map_REC_TCN.ix = i;
//                        map_REC_TCN.val[ i ] = map_ci.bmi_event;
//    }
//                     break;
//    
//    }
//    
//               case R_PHASE:
//    {
//    
//                  /*
//                  Recorder for internal work phases.
//                  */
//                     i = map_REC_PHASE.ix;
//    
//                     if ( map_REC_PHASE.val[ i ] != p_map_vw->phase )
//    {
//    
//                        if ( ++i >= MAX_REC15 )
//    {
//                           i = 0;
//    }
//                        map_REC_PHASE.ix = i;
//                        map_REC_PHASE.val[ i ] = p_map_vw->phase;
//    }
//                     break;
//    
//    }
//    
//               case R_ORD:
//    {
//    
//                  /*
//                  Recorder for orders in UPDATE phase.
//                  */
//                     i = map_REC_ORD.ix;
//    
//                     if ( map_REC_ORD.val[ i ] != p_map_vw->order )
//    {
//    
//                        if ( ++i >= MAX_REC7 )
//    {
//                           i = 0;
//    }
//                        map_REC_ORD.ix = i;
//                        map_REC_ORD.val[ i ] = p_map_vw->order;
//    }
//                     break;
//    
//    }
//    default:{
//    
//                  /*
//                  trace recorder
//                  */
//    
//                     if ( type < 0x50 )
//    {
//    
//                        /*
//                        trace during SETUP
//                        */
//                           i = map_REC_SETUP.ix;
//    
//                           if ( map_REC_SETUP.val[ i ] != type )
//    {
//    
//                              if ( ++i >= MAX_REC15 )
//    {
//                                 i = 0;
//    }
//                              map_REC_SETUP.ix = i;
//                              map_REC_SETUP.val[ i ] = type;
//    }
//    
//    }else{
//    
//                        if ( ( type >= 0xA0 ) && (type <= 0xDF ) )
//    {
//    
//                           /*
//                           MVB trace UPDATE mode
//                           */
//                              i = map_REC_UPDATE_MVB.ix;
//    
//                              if ( map_REC_UPDATE_MVB.val[ i ] != type )
//    {
//    
//                                 if ( ++i >= MAX_REC15 )
//    {
//                                    i = 0;
//    }
//                                 map_REC_UPDATE_MVB.ix = i;
//                                 map_REC_UPDATE_MVB.val[ i ] = type;
//    }
//    
//    }else{
//    
//                           /*
//                           WTB trace UPDATE mode
//                           */
//                              i = map_REC_UPDATE_WTB.ix;
//    
//                              if ( map_REC_UPDATE_WTB.val[ i ] != type )
//    {
//    
//                                 if ( ++i >= MAX_REC31 )
//    {
//                                    i = 0;
//    }
//                                 map_REC_UPDATE_WTB.ix = i;
//                                 map_REC_UPDATE_WTB.val[ i ] = type;
//    }
//    
//    }
//    }
//                     break;
//    
//    }
//    }
//    
//#endif
//            return;
//    }


	  /*
	     err_rec
	   */
/* <ATR:07> */
//         void err_rec(void)
//    {
//    
//            /*
//            Writes the errorr.code and the error.info into the event recorder.
//            */
//            unsigned char i;
//            unsigned char err_txt[26], aval[ 4 ];
//    
//            err_txt[ 0 ] = *"*";
//            err_txt[ 1 ] = *"*";
//            err_txt[ 2 ] = *"*";
//            err_txt[ 3 ] = *"e";
//            err_txt[ 4 ] = *"r";
//            err_txt[ 5 ] = *"r";
//            err_txt[ 6 ] = *":";
//            err_txt[ 7 ] = *" ";
//    
//            /* convert result into ascii format */
//            hex_asc( p_map_vw->code, aval );
//    
//            for ( i=0; i<4; i++)
//    {
//               err_txt[ i+8 ] = aval[ i ];
//    }
//            err_txt[ 12 ] = *",";
//            err_txt[ 13 ] = *" ";
//            err_txt[ 14 ] = *"i";
//            err_txt[ 15 ] = *"n";
//            err_txt[ 16 ] = *"f";
//            err_txt[ 17 ] = *"o";
//            err_txt[ 18 ] = *":";
//            err_txt[ 19 ] = *" ";
//    
//            /* convert result into ascii format */
//            hex_asc( p_map_vw->info, aval );
//    
//            for ( i=0; i<4; i++)
//    {
//               err_txt[ i+20 ] = aval[ i ];
//    }
//            err_txt[ 24 ] = 0;
//            err_txt[ 25 ] = 0;
//    
//            recorder( R_ERR );
//    
//            return;
//    }


/*
   ascend_range
 */

void 
ascend_range (char max)
{
	short i;

	for (i = 0; i < max; i++)
		NADI.vehicle[i].RangeNo = i + 1;
}


/*
   descend_range
 */

void 
descend_range (char max)
{
	short i;

	for (i = 0; i < max; i++)
		NADI.vehicle[i].RangeNo = max - i;
}


/*
   build_vrno
 */

void 
build_vrno (void)
{

	/*
	   building the valid vehicle range numbers within NADI.
	 */
	char i, j, max;

	max = NADI.state.VehicleNo;

	/* complete NADI state with ScopeOfRangeNo. */
	NADI.state.ScopeOfRangeNo = MAP_ABSOLUTE;

	/*
	   Description: DYNAMIC ATTRIBUTES

	   bit 0.0   vehicle in operation
	   bit 0.1   vehicle is leading
	   bit 0.2   vehicle contains the train bus master
	   bit 0.3   vehicle is traction leader (train bus controlled)

	   bit 0.4   vehicle is traction follower (train bus controlled)
	   bit 0.5   vehicle is traction leader (not train bus controlled)
	   bit 0.6   vehicle is traction folower (not train bus controlled)
	   bit 0.7   traction unit of the vehicle is active

	   bit 1.0   last vehicle
	   bit 1.x   x = 1..7 reserved

	   bit 2.x   x = 0..7 reserved
	 */


	/*
	   RULE 1 {Leading vehicle at one end of the train, bit 0.1 }
	 */

	if ((NADI.vehicle[0].DynAttr[0] & 0x02) != 0) {
		ascend_range (max);
		recorder (0x31);
		return;
	} else {
		if ((NADI.vehicle[max - 1].DynAttr[0] & 0x02) != 0) {
			descend_range (max);
			recorder (0x32);
			return;
		}
	}


	/*
	   RULE 2 {Leading vehicle within the train, not bit 0.1}
	 */

	j = 0;

	for (i = 0; i < max; i++) {
		if ((NADI.vehicle[i].DynAttr[0] & 0x02) != 0) {
			j = i + 1;
			break;
		}
	}

	if (j != 0) {

		/* Leading car found */
		i = max - j;			/* nr of vehicles till last member */
		j--;					/* nr of vehicles till first member */

		if (i > j) {
			ascend_range (max);
			recorder (0x33);
			return;
		} else {
			if (i < j) {
				descend_range (max);
				recorder (0x34);
				return;
			}
		}
	}
	/*
	   RULE 3 {Not rule 1 or 2, but traction leader at one end of the train bit 0.3}
	 */

	if ((NADI.vehicle[0].DynAttr[0] & 0x08) != 0) {
		ascend_range (max);
		recorder (0x35);
		return;
	} else {
		if ((NADI.vehicle[max - 1].DynAttr[0] & 0x08) != 0) {
			descend_range (max);
			recorder (0x36);
			return;
		}
	}


	/*
	   RULE 4 {Not rule 1-3}
	 */

	/*
	   r u l e  4
	   Range number relative to the vehicle. It depends on the direction
	   of the operator console.
	   Changed at 08.02.95: it was urgend necessary, that the numbering
	   in each node is the same. With L_DIRECTION there has been a different
	   numbering in somme nodes (agreed with AEG: Mr. Bahrs, Mr. Lehmann,
	   with ABB-CH: Mr. Schomisch, Mr. Nivergelt, ABB-D: Mr. Herrmann)
	 */

	ascend_range (max);
	NADI.state.ScopeOfRangeNo = MAP_RELATIVE;

	recorder (0x37);

	return;
}
