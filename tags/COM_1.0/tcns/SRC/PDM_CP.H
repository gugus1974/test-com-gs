/*************************************************************************
**        Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       copy routines definitions

**************************************************************************
**    AUTHOR::     Arnold (Ar)    
                   Heindel (Hei)  
**    CREATED::    05.08.93
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:06 $
      previous history:
                   date      sign   remark
                   05.08.93  Ar     start of development
                   23.08.93  Ar     Interface to config. changed
                   30.08.93  Ar     -----------"----------------
                   08.11.93  Ar     Changes for real

      $Log: PDM_CP.H,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

**************************************************************************/

#ifndef PDM_CP_H_
#define PDM_CP_H_

#include "lp_sys.h"
#include "pdm_func.h"

/* the frame type field FTF shall be defined as Cardinal16, so that
 * 0xffff is also the invalid value according to TCN Working Draft
 * see TC9/22 Part 4 Ch. 7.5.1, Sept. 1995
 * But UIC has only 1 byte FTF
 */  
#define PDM_FTF_DONT_CARE_2 0xffff    
#define PDM_FTF_DONT_CARE 0xff
#define PDM_FTF_INVALID      (pdm_ftf_size == 1 ? 0xff : 0xffff)


#define PDM_ERR_INVALID_FTF 0xF0   /* error number should not interfere
                                    * with error numbers of link layer
				    */   


/*** limits and invalid values for the different datatypes of TCN ****/

#define PDM_INVALID_FRAC_200       (TYPE_LP_FRAC_200)0xffff
#define PDM_INVALID_FRAC_400       (TYPE_LP_FRAC_400)0xffff
#define PDM_INVALID_FRAC_800       (TYPE_LP_FRAC_800)0xffff
#define PDM_MAX_FRAC_200           (TYPE_LP_FRAC_200)0x7fff
#define PDM_MIN_FRAC_200           (TYPE_LP_FRAC_200)0x8000
#define PDM_MAX_FRAC_400           (TYPE_LP_FRAC_400)0xffff
#define PDM_MIN_FRAC_400           (TYPE_LP_FRAC_400)0x0000
#define PDM_MAX_FRAC_800           (TYPE_LP_FRAC_800)0x7fff
#define PDM_MIN_FRAC_800           (TYPE_LP_FRAC_800)0x8000

#define PDM_MAX_FRAC_200_FL   ((float)((TYPE_LP_FRAC_200)0x7fff) / (float)((TYPE_LP_FRAC_200)0x4000))
#define PDM_MIN_FRAC_200_FL   (-2.0)
#define PDM_MAX_FRAC_400_FL   ((float)((TYPE_LP_FRAC_400)0xffff) / (float)((TYPE_LP_FRAC_400)0x4000))
#define PDM_MIN_FRAC_400_FL   0.0
#define PDM_MAX_FRAC_800_FL   ((float)((TYPE_LP_FRAC_800)0x7fff) / (float)((TYPE_LP_FRAC_800)0x1000))
#define PDM_MIN_FRAC_800_FL   (-8.0)
#define PDM_MAX_CARD_32_FL    (float)PDM_MAX_CARD_32
#define PDM_MIN_CARD_32_FL    0.0
#define PDM_MAX_SIGN_32_FL    (float)PDM_MAX_SIGN_32
#define PDM_MIN_SIGN_32_FL    (float)PDM_MIN_SIGN_32


#define PDM_MAX_SIGN_8       (TYPE_LP_SIGN_8)0x7f
#define PDM_MIN_SIGN_8       (TYPE_LP_SIGN_8)0x80
#define PDM_MAX_SIGN_16      (TYPE_LP_SIGN_16)0x7fff
#define PDM_MIN_SIGN_16      (TYPE_LP_SIGN_16)0x8000
#define PDM_MAX_SIGN_32      (TYPE_LP_SIGN_32)0x7fffffffL
#define PDM_MIN_SIGN_32      (TYPE_LP_SIGN_32)0x80000000L
#define PDM_INVALID_SIGN_8   (TYPE_LP_SIGN_8)0xff
#define PDM_INVALID_SIGN_16  (TYPE_LP_SIGN_16)0xffff
#define PDM_INVALID_SIGN_32  (TYPE_LP_SIGN_32)0xffffffffL


#define PDM_MAX_CARD_8      (TYPE_LP_CARD_8)0xffU
#define PDM_MIN_CARD_8      (TYPE_LP_CARD_8)0U
#define PDM_MAX_CARD_16     (TYPE_LP_CARD_8)0xffffU
#define PDM_MIN_CARD_16     (TYPE_LP_CARD_8)0U
#define PDM_MAX_CARD_32     (TYPE_LP_CARD_32)0xffffffffUL
#define PDM_MIN_CARD_32     (TYPE_LP_CARD_32)0U
#define PDM_INVALID_CARD_8  (TYPE_LP_CARD_8)0xffU
#define PDM_INVALID_CARD_16 (TYPE_LP_CARD_16) 0xffffU
#define PDM_INVALID_CARD_32 (TYPE_LP_CARD_32)0xffffffffUL


union U_pdm_invalid_float {
    unsigned long l;
    float f;
};
#ifndef PDM_CP_C_
extern union U_pdm_invalid_float pdm_invalid_float;
#endif

#define PDM_INVALID_REAL  (pdm_invalid_float.f)
                                   /* has to be NOT A NUMBER from IEEE */


#define PDM_CHK_ERROR  	 0	/* check variable for error variable */
#define PDM_CHK_VALID  	 1	/* check variable for valid variable */
#define PDM_CHK_FORCE    2	/* check variable for forced variable */
#define PDM_CHK_INVALID  3	/* check variable for invalid variable */


typedef struct STR_APD_PV_SET    TYPE_APD_PV_SET;
typedef struct STR_APD_PV_LIST   TYPE_APD_PV_LIST;
typedef struct STR_APD_PV_DESCRIPTOR    TYPE_APD_PV_DESCRIPTOR;


/* data structure for reading data from the source Traffic Store */

typedef struct STR_PDM_DATASET     {
    TYPE_APD_PV_SET * p_ds;     /* description of the dataset           */
    TYPE_APD_PV_DESCRIPTOR *p_pvdescr; 
                                /* pointer in array of pv descriptors */
    int      error;             /* error while reading this ds          */
    int      vars;              /* number of vars of port */
    union {
	struct {
	    UNSIGNED16 *p_var_freshness;  /* Pointer to an array with freshness
					   * for each variable specified in DS */
	    UNSIGNED16 rcv_frame_type;    /* received frame type field. The frame
					   * type field must occur in the p_pv_x
					   * list of the DS and p_appl must point
					   * to rcv_frame_type.                */
	    UNSIGNED16 frame_type;        /* desired frame type                   */
	    UNSIGNED16 ds_freshness;      /* memory space for the freshness value */
	} from;
	struct {
	    UNSIGNED16 n_defaults; /* number of default values within DS */
	} to;
    } dir;
} T_PDM_DATASET;


/* data structure describing the whole copy process for one data set */

typedef struct STR_PDM_COPY_LIST {
    UNSIGNED16         configured;      /* configuration flag                */
    UNSIGNED16         status;          /* export status of operation        */
    UNSIGNED16         type;            /* actual marshalling type           */
    UNSIGNED16         mode;            /* actual marshalling mode           */
    UNSIGNED16         cycle;           /* marshalling cycle                 */
    UNSIGNED16         result;          /* last result of marshalling config */
    UNSIGNED16         timeout_id;      /* marshalling timeout               */
    UNSIGNED16         n_from_ports;    /* number of source DS to be read    */
    UNSIGNED16         n_funcs;         /* num of functions to be executed   */
    UNSIGNED16         n_to_ports;      /* number of output ds               */
    T_PDM_DATASET      *p_from_ds;      /* array of FROM DS                  */
    TYPE_APD_PV_DESCRIPTOR *p_from_apd_pv_descr; /* APD descriptor           */
    T_PDM_FUNC_INFO    *p_func_info;    /* array of functions                */
    T_PDM_DATASET      *p_to_ds;        /* description of the TO DS          */
    void               *p_var_buf;      /* pointer to variable buffer        */
    unsigned short     *p_fresh;        /* pointer to allowed freshness of each
					 * variable 
					 */
    UNSIGNED8          *shadow_buf;     /* WTB TS write shadow buffer        
					 * buffer for entire port only used
					 * for export (otherwise = NULL)      
					 */
    T_PDM_FUNC_ARG     *p_func_args;
} T_PDM_COPY_LIST;



extern void pdm_set_var_invalid(TYPE_APD_PV_DESCRIPTOR *p_pvdescr);

extern int pdm_copy(T_PDM_COPY_LIST * p_copy_list);

#endif
