/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Modified PDM_S_DIR_TYPE and PDM_S_DIR_SIZE to aviod using the unsupp. BITSET64 type */
/*==============================================================================================*/


/*************************************************************************
**        Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       Definitions for Marshalling list configuration
                   Header file for Configuration of Marshalling lists
                   out of user defined Data Base

**************************************************************************
**    AUTHOR::     Krafft (kra)    
                   Heindel (Hei)  
**    CREATED::    13.08.93
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:06 $
      previous history:
                   date      sign   remark
                   13.08.93  kra    start of development         

      $Log: PDM_LCFG.H,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

*************************************************************************/


#ifndef PDM_LCFG_H_
#define PDM_LCFG_H_

#include "lp_sys.h"
#include "pdmtypes.h"

#ifdef PDM_MODULE
#include "pdm_cp.h"
#endif


#define PDM_SW_VERSION 0x0001	/* checked against database version to
				 * verify proper format
				 */


/*****************************************************************************/
/*********************** general definitions  ********************************/
/*****************************************************************************/

/* check variable definitions */
#define PDM_CHKVARVOID          0x3ff  /* void check variable value */
#define PDM_CHKMASK             0x6000 /* mask chk overlay bits */ 
#define PDM_CHKUNIQUE           0x6000
                       /* no or unique or first of overlayed check variables */
#define PDM_CHKOVERLAYVAR       0x4000 /* chk var overlayed by var */
#define PDM_CHKOVERLAYCHK       0x2000 /* chk var overlayed by chk var */
#define PDM_FUNC                0x8000 /* function result reference marker */
#define PDM_REFMASK             0x0fff /* mask reference */
#define PDM_FLAGMASK            (~PDM_REFMASK) /* mask flags */

#define PDM_DEFAULT_VALUE       0x4000 /* variable comes from default buffer */


/*-------------- Source directory definitions -------------------------------*/
/* the source directory heads all mvb ports which are specified along with
 * copy type CP_S_DIR. each bit set to one specifies a valid process variable
 * within (and right behind the directory) the frame. the number of bits set
 * specifies the number of valid process variables within the frame
 */

/* PV_NAME of source directory */
/* <ATR:01> */
// #define PDM_S_DIR_TYPE       		LP_TYPE_BITSET
// #define PDM_S_DIR_SIZE       		LP_SIZE_64
#define PDM_S_DIR_TYPE       		0x0F
#define PDM_S_DIR_SIZE       		0x03
#define PDM_S_DIR_OFFSET     		0
#define PDM_S_DIR_VAR_OFFSET 		64
#define PDM_S_DIR_CHK_OFFSET 		64
#define PDM_S_DIR_VAR_OFFSET_CHK 	0

/* length of source directory in octets */
#define PDM_S_DIR_LEN 8



/*****************************************************************************/
/********** constant definitions to decode Data Base *************************/
/*****************************************************************************/

/*------------- defines for configuration of lists --------------------------*/

/* definitions to decode data base list header */
#define PDM_DB_HEADER   0x0101

/* the last nibble of PDM_INTERNAL, PDM_EXPORT and PDM_IMPORT builds an index
 * int the copy list data structure array. DONT CHANGE THESE BITS!
 */
#define PDM_INTERNAL    0x1000
#define PDM_EXPORT      0x2001
#define PDM_IMPORT      0x4002

#define PDM_FROM        0x0100
#define PDM_FUNCTION    0x0200
#define PDM_TO          0x0400
#define PDM_DEFAULT_VAL 0x0800
#define PDM_END_OF_DB   0x00f0

#define PDM_NEGATED 0x0001

/*definitions to decode contents of db lists*/
#define PDM_DONT_CARE_WORD   (UNSIGNED16)0xffff
#define PDM_DONT_CARE_OCTET  (UNSIGNED8)0xff
#define PDM_NO_FRESHNESS     (UNSIGNED16)0xffff


/*-------- function identifier definition (valid for all) -------------------*/
#define PDM_FUNC_AND   0	/* boolean and */
#define PDM_FUNC_OR    1	/* boolean or */
#define PDM_FUNC_XOR   2	/* boolean exclusive or */
#define PDM_FUNC_MIN   3	/* signed, unsigned, float min */
#define PDM_FUNC_MAX   4	/* signed, unsigned, float max */
#define PDM_FUNC_CONV  5	/* type conversion */
#define PDM_FUNC_SUM   6	/* signed, unsigned, float sum */
#define PDM_FUNC_SELECT 7	/* signed, unsigned select */

/* function calculation types */
#define PDM_UNSIGNED  0x08	/* calculate on basis unsigned values */
#define PDM_REAL      0x10	/* calculate on basis float values */


/* copy types for import */
#define PDM_NORM      0x01
#define PDM_S_DIR     0x02
#define PDM_ROUTE     0x04



/*****************************************************************************/
/************** TYPE definitions to decode Data Base *************************/
/*****************************************************************************/

/*-------------- general types of database ----------------------------------*/

/* each element in the data base has the following header */
typedef struct {
    UNSIGNED16 type;                 /* sub list type                   */
    UNSIGNED16 length;               /* length of list in octets 
				      * (inclusive header)              */
} T_MDB_SUB_HDR;



/* first element of the data base is the data base header */
typedef struct {
    T_MDB_SUB_HDR hdr;               /* type (data base header) and length of
				      * db header                        */
    UNSIGNED16    total_length_of_db;/* length of database in octets     */
    UNSIGNED16    application_id;    /* identifier for application 
				      * e.g. UIC, VDV                    */
    UNSIGNED16    vehicle_appl_id;   /* identifier for the vehicle 
				      * application e.g. leading vehicle */
    UNSIGNED16    db_version;        /* version of data base             */
    T_PDM_DATE    production_date;   /* date of production of data base  */
} T_MDB_DB_HDR;



/* The following list header has same layout for each list type Export, 
 * Import, Internal and on the top of it.
 * There is a list for each different user mode for the types Export, 
 * Import or Internal.
 */
typedef struct {
    T_MDB_SUB_HDR hdr;   /* import, export or internal and length in 
			  * octets                                       */
    UNSIGNED16    mode;  /* user mode                                    */
    UNSIGNED16    cycle; /* milliseconds                                 */
} T_MDB_LIST_HDR;


/*---------------------------------------------------------------------------
 * marshalling configuration
 *---------------------------------------------------------------------------
 */

/* marshalling FROM variable sublist header */
typedef struct {
    T_MDB_SUB_HDR hdr;       /* FROM variable list and length in octets  */
    union {
	UNSIGNED16 n_ports;  /* number of FROM ports (internal, export)  */
	UNSIGNED16 ftf;      /* FTF of WTB port  (import only)           */
    } uu;
    UNSIGNED16 n_vars;       /* number of FROM vars for this port / FTF  */
} T_MDB_FROMVAR_HDR;



/* FROM variable list element type */
typedef struct {
    UNSIGNED16        ref;        /* reference of variable               */
    TYPE_PV_NAME_48   pv_name;    /* PV_NAME of variable                 */
    UNSIGNED16        overlay;    /* overlay information for check var   */
    UNSIGNED16        freshness;  /* allowed freshnes for  e a c h  
				   * variable                            */
} T_MDB_FROMVAR;



/* function sublist header */
typedef struct {
    T_MDB_SUB_HDR hdr;     /* function list and length in octets         */
    UNSIGNED16 n_funcs;    /* number of functions to be executed         */
    UNSIGNED16 n_args;     /* number of all arguments to all functions   */
} T_MDB_FUNC_HDR;



/* function/argument list element type */
typedef struct {
    union {
	struct {
	    UNSIGNED16  ref;     /* reference from argument into
				  * from-variable list                   */
	    UNSIGNED16  ftf;     /* frame type field for wtb varibales 
				  * (import only)                        */
	    UNSIGNED16  flags;   /* argument negated */
	    UNSIGNED16  bit_pos; /* bit position in a bitset variable, 
				  * ignored for other types              */
	} arg;
	struct {
	    UNSIGNED16  ref;     /* reference of function result for 
				  * use in to-variable list              */
	    UNSIGNED16  id;      /* identifies function to be executed   */
	    UNSIGNED16  flags;   /* PDM_NORM, PDM_S_DIR, 
				  * PDM_UNSIGNED, PDM_REAL               */
	} func;
    } uu;
} T_MDB_FUNC_ARG;

/* TO port sublist header */
typedef struct {
    T_MDB_SUB_HDR hdr;     /* to-variable list type and length in octets */
    UNSIGNED16 n_ports;    /* number of ports                            */
    UNSIGNED16 n_vars;     /* number of to-variables                     */
} T_MDB_TOVAR_HDR;


/* TO variable list element type */
typedef struct {
    UNSIGNED16      ref;       /* reference of from-variable or function 
				* result                                 */
    TYPE_PV_NAME_48 pv_name;   /* PV_NAME of to-variable                 */
} T_MDB_TOVAR;

/* NOTE:
 * T_MDB_TOVAR and T_MDB_TOVAR_IM share the first 2 
 * components, but are not a union to optimize space requirements
 */
typedef struct {
    UNSIGNED16      ref;       /* reference of from-variable or function 
				* result                                 */ 
    TYPE_PV_NAME_48 pv_name;   /* PV_NAME of to-variable                 */
    UNSIGNED16      ftf;       /* frame type of from-variable            */ 
    UNSIGNED16      flags;     /* CP_NORM, CP_S_DIR                      */
} T_MDB_TOVAR_IM;



#ifdef PDM_MODULE

/*****************************************************************************/
/***         Module internal Interface of config copy lists     **************/
/*****************************************************************************/

extern int
build_copy_list(T_PDM_COPY_LIST *p_cpl, void *p_data_base);
#endif


#endif
