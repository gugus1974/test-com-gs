/*************************************************************************
**        Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       General definitions
                   Header file for main controlling programm of PDM
**************************************************************************
**    AUTHOR::     Krafft (kra)
                   Heindel (Hei)
**    CREATED::    16.08.93
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $
                   MODTIME::  $Date: 2006/12/01 10:40:07 $
      previous history:
                   date      sign   remark
                   16.08.93  kra    start of development
                   19.11.93  Kra    include file "ctrl.h" of WTB introduced
                   10.02.95  Kra    Modifications for new WTB LL version 1.9

      $Log: PDM_MAIN.H,v $
      Revision 1.1.2.1  2006/12/01 10:40:07  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

**************************************************************************/

#ifndef PDM_MAIN_H_
#define PDM_MAIN_H_


/* The following constant is defined in ctrl.h and used to avoid
 * duplicate inclusion of header file
 */
#ifndef L_BUSY
#include "bmi.h"
#endif

#include "pdmtypes.h"

#ifndef PC_TEST
//#include "hardinit.def"
#define ASSERT(x)

#else
/* length of system tick */
#define RMK_TICK  10
#include <assert.h>
#define ASSERT(x) assert(x)
#endif


/* ---------------------- PDM external interface ---------------------- */

/*--------------------error messages-----------------------------------------*/
#define PDM_OK            0
#define PDM_ERR           0x0001
#define PDM_INVAL_PV_N    0x0002
#define PDM_INVAL_PD_TYPE 0x0003
#define PDM_ERR_NOT_FOUND 0x0010
#define PDM_ERR_NO_MEM    0x0011
#define PDM_ERR_CALL_SEQ  0x0012
#define PDM_ERR_CONF_MODE 0x0013
#define PDM_WAR_DEF_USED  0x0101
#define PDM_ERR_UNKNOWN   0x1fff


/*-------------- marshalling mode identifier -------------------------------*/
#define PDM_DEFAULT_MODE    0x0000
#define PDM_DO_NOT_MARSH    0xffff

/*--------- constants of operational status of internal/export/import ------*/
#define PDM_INIT     0x00
#define PDM_ENABLED  0x01
#define PDM_DISABLED 0x02





/*-------------------- General TYPEs ----------------------------------------*/

typedef struct STR_PDM_STATUS
{
  UNSIGNED16    pdm_sw_version;   /* version of PDM software                 */
  UNSIGNED16    mdb_version;      /* version of marshalling data base        */
  unsigned char initialised;      /* pdm_init already called? TRUE/FALSE     */
  unsigned char configured;       /* pdm_config already called?              */
  unsigned char wtb_ts_created;   /* traffic store of wtb created?           */
  unsigned char topo_indicated;   /* wtb topography indicated?               */
  UNSIGNED16    ex_status;        /* export status of operation              */
  UNSIGNED16    ex_mode;          /* actual export marshalling mode          */
  UNSIGNED16    ex_cycle;         /* marshalling cycle                       */
  UNSIGNED16    ex_result;        /* last result of marshaling config        */
  UNSIGNED16    in_status;        /* internal status                         */
  UNSIGNED16    in_mode;          /* actual internal marshalling mode        */
  UNSIGNED16    in_cycle;         /* marshalling cycle                       */
  UNSIGNED16    in_result;        /* last result of marshaling config        */
  UNSIGNED16    im_status;        /* import status of operation              */
  UNSIGNED16    im_mode;          /* actual import marshalling mode          */
  UNSIGNED16    im_cycle;         /* marshalling cycle                       */
  UNSIGNED16    im_result;        /* last result of marshaling config        */
} T_PDM_STATUS;


/*----------- prototypes of module pdm_main -------------------------------*/
int pdm_init (void);

int pdm_config (void * p_pdm_database,
                UNSIGNED16 internal_mode,
                UNSIGNED16 export_mode,
                UNSIGNED16 import_mode);

void pdm_get_status (T_PDM_STATUS * p_pdm_st);

void pdm_wtb_topo_ind (Type_Topography *p_topography);

void pdm_wtb_ts_crea_ind (unsigned char wtb_ts_id);

int config_marsh(UNSIGNED16 type);

int pdm_reconfig_marsh(UNSIGNED16 type, UNSIGNED16 mode);

void pdm_main (void);

short pdm_wtb_put_variable (const TYPE_PV_NAME_48  pv_name,
                            void  *p_var,
			    UNSIGNED8 *p_chk,
                            const UNSIGNED16 pd_type);


/* ---------------------- PDM internal interface ---------------------- */
#ifdef PDM_MODULE

/* shadow buffer length for WTB source port */
#define PDM_SHADOW_BUF_LEN 128

#define PDM_MARSH_TYPES 3         /* internal, export, import */

#ifndef PDM_MAIN_C_
extern T_PDM_COPY_LIST pdm_copy_list[PDM_MARSH_TYPES];


extern UNSIGNED16 pdm_ftf_size;  /* number of bytes (1 or 2) for FTF
				  * see TCN Norm Part 4, Ch. 7.5.1 (2 bytes)
				  * but it conflicts with UIC leaflet 556
				  * (1 byte) default for PDM is 1 byte.
				  */

/* global vars to store WTB topography data */
extern unsigned char  wtb_tsid;
extern Type_Topography * p_wtb_topo;

/* shadow buffer for WTB source port */
extern UNSIGNED8 pdm_shadow_buf[PDM_SHADOW_BUF_LEN];
#endif

#ifdef PCTEST
void pd_marshall (void);
#endif
#endif

#endif /* PDM_MAIN_H_ */
