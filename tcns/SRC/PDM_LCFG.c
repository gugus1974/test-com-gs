/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> fixed copy list memory deallocation                                                 */
/* <ATR:02> min() macro defined only if missing                                                 */
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
**    TASK::       Configuration of Marshalling lists out of Data Base
                   Routines to configure Marshalling lists for the copy module
                   out of the user defined database.
                   (generation of Datasets ...)
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
                   13.08.93  Kra    start of development         
                   18.11.93  Kra    wrong definition of inval32[] replaced 
                   23.02.94  Kra    3 errors wrong end of loop controled by
                                    variable num_vars deleted in procedures:
                                     gen_fct_for_pdm_cp_in()
                                     gen_dst_for_pdm_cp_ex()
                                     gen_dst_for_pdm_cp_im()
                   01.03.94  Kra    Type of export destination dataset (to put
                                    shadow port of pdm) changed to array
                   18.04.94  Kra    ini_s_dir can handle number of nodes = 0 
                                    now


      $Log: PDM_LCFG.c,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

*************************************************************************/

#define PDM_MODULE	/* access to PDM internal definitions */

/*
 * APD BUG WORKAROUND:
 * ACCESS TO WTB FAILS, BECAUSE APD MANGLES THE PORT ADDRESS (DIV BY 4)
 */
#define APD_BUG
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#ifdef O_960
#pragma noalign
#endif

#include <stddef.h>
#include <string.h>

#include "lp_sys.h"
#include "apd_incl.h"
#include "pi_sys.h"
#include "pdmdebug.h"
#include "pdmtypes.h"
#include "pdm_lcfg.h"
#include "pdm_cp.h"
#include "pdm_main.h"

#include "recorder.h"

#ifndef O_166
#define pdm_alloc pi_alloc 
/* <ATR:01> */
// #define pdm_free pi_free
#define pdm_free(x) do {pi_free(x); x = 0;} while (0)
#endif

/*------------local valid defines and makros---------------------------------*/
#define NEXTSUBLIST(p_x) ((UNSIGNED8 *)(p_x) + \
			  ((T_MDB_SUB_HDR *)(p_x))->length)

#define RECORD(p_msg) (nse_record("PDMLCFG",__LINE__,   \
                                  NSE_EVENT_TYPE_ERROR,\
                                  NSE_ACTION_CONTINUE, \
                                  (strlen(p_msg)+1),   \
                                  (UINT8 *)p_msg))


/* NOTE: macro min evaluates arguments more than once */
/* <ATR:02> */
#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif

/************** l_cfg internal definitions ***********************************/

/*---------------- local definitions ----------------------------------------*/

/* constants for coding and decoding extended WTB topography */
#define NODE_NOT_PRESENT 0xff


#define MAX_NODE_ADDR   63
#define MAX_NODE_NUM    32

typedef struct S_FTF_INFO {
    UNSIGNED16  ftf;		/* Frame type field (up to 16 bits) */
    UNSIGNED16  n_vehicles;	/* # of ports (i.e. vehicles) with this FTF */
    UNSIGNED16  n_vars;		/* # of vars with this FTF */
    UNSIGNED16  i_pvdescr;	/* index in pv descriptor array 
				 * for first configured variable
				 */
} T_FTF_INFO;

typedef struct
{
  unsigned short n_vehicles;
  unsigned short wtb_ts_id;                     /* traffic store id of WTB  */
  unsigned short ftf_list[MAX_NODE_ADDR+1];      /* present ftf fields       */
  T_FTF_INFO     ftf_info[MAX_NODE_NUM+1];      /* FTF info */
} T_PDM_TOPO;



T_PDM_TOPO pdm_topo; /* PDM's knowledge of topography */

/* universal invalid bits buffer */
STATIC UNSIGNED8 invalid_value[128] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
STATIC UNSIGNED8 invalid_chk = PDM_CHK_INVALID;
STATIC UNSIGNED8 empty_srcdir[PDM_S_DIR_LEN] = {0};



/* structure to count variables dependend of type to 
 * compute variable buffer space 
 */
typedef struct SPACE_INFO {
    /* space for all ports */
    unsigned short size_4;	/* space requirement for 4-byte objects  */
    unsigned short size_2;	/* space requirement for 2-byte objects  */
    unsigned short size_1;	/* space requirement for 1-byte objects  */
    unsigned short chks_offset;	/* space offset for check variables */
} SPACE_INFO;

typedef struct PORT_INFO {
    /* per port */
    unsigned short vars;	/* count variables       */
    unsigned short chks;	/* count unique check variables */
} PORT_INFO;



/* ---------------------- function prototypes ---------------------- */

UNSIGNED8 
bytesize(TYPE_PV_NAME_48 pvname);



#ifdef DEBUG
void
dump_pvdescr(TYPE_APD_PV_DESCRIPTOR *p_pvdescr, int vars)
{
    int k;
    for (k = 0; k < vars; k++) {
	pr_err("var %3d: p_var 0x%08x p_chk 0x%08x pvname (%2d, %4d, %3d, %4d, x%02x, %4d)\n", 
	       k, p_pvdescr->p_variable, p_pvdescr->p_validity,
	       p_pvdescr->pv_name.ts_id,
	       p_pvdescr->pv_name.prt_addr,
	       p_pvdescr->pv_name.size,
	       p_pvdescr->pv_name.bit_offset,
	       p_pvdescr->pv_name.type,
	       p_pvdescr->pv_name.ctl_offset);
	p_pvdescr++;
    }
}

void
dump_pvset(TYPE_APD_PV_SET *p_pvset, int n)
{
    int k;
    struct STR_APD_PV_LIST *p_pv_list = p_pvset->p_pv_list;

    for (k = 0; k < n; k++) {
	pr_err("var %3d: p_appl 0x%08x, type x%02x, size %2d, byteoff %3d, bitoff %d\n",
	       k, p_pv_list->p_appl, p_pv_list->type, p_pv_list->size,
	       p_pv_list->byte_offset, p_pv_list->bit_offset);
	p_pv_list++;
    }
}

    
void
dump_func_info(T_PDM_FUNC_INFO *func_info, int  n_funcs)
{
    int i, j;
    T_PDM_FUNC_ARG *p_arg;
    extern char *func2name();
    extern char *cast2name();

    pr_err("\n");
    for (i = 0; i < n_funcs; i++) {
	p_arg = func_info->p_args;
	pr_err("func %2d: %s (0x%08x) (\n", 
	       i, func2name( func_info->function), func_info->function);
	for (j = 0; j < (int)func_info->n_args; j++) {
	    pr_err("    p_var 0x%08x, p_chk 0x%08x, %c %4d %s\n",
		   p_arg->p_var, p_arg->p_chk, 
		   (p_arg->bit_pos & PDM_CALC_NEGATED) ? 'I' : ' ', 
		   p_arg->bit_pos & ~PDM_CALC_NEGATED,
		   cast2name(p_arg->f_up_cast));
	    p_arg++;
	}
	pr_err(") returns p_var 0x%08x, p_chk 0x%08x, size %3d, type x%02x %s\n",
	       func_info->result.p_var, func_info->result.p_chk, 
	       func_info->result.size, func_info->result.type,
	       cast2name(func_info->result.f_down_cast));
	func_info++;
    }
}

void
dump_ftf_info(T_FTF_INFO *p_ftf_info) 
{
    T_FTF_INFO *pfi;

    pr_err("FTF info array\n");
    pr_err("     ftf  n_vehicles  n_vars  i_pvdescr\n");
    for (pfi = p_ftf_info; pfi->ftf != PDM_FTF_INVALID; pfi++) {
	pr_err("  0x%04x%12d%8d%11d\n", 
	       pfi->ftf, pfi->n_vehicles, pfi->n_vars, 
	       pfi->i_pvdescr);
    }
}

void
dump_func_ref2idx(UNSIGNED16 *p_func_ref2idx, UNSIGNED16 n) 
{
    UNSIGNED16 i;
    
    pr_err("\nFunction reference to index converter\n");
    pr_err(" ref   index\n");
    for (i = 0; i < n; i++) {
	pr_err("%4d  %6d\n", i, p_func_ref2idx[i]);
    }
}
#endif



/*new page*/
/******************************************************************************
*
*   name:         calc_size_in_bytes
*
*   function:     Uses type and size of a TCN variable to calculate its size in
*                 bytes.
*                 within this procedure, PDM takes care of alignment
*                 constrains of the used CPU (or compiler).
*
*   input:        type:       TCN variable type
*                 size:       TCN variable size
*
*   used globals: none
*
*   output:       none
*
*   return value: Size of the variable in bytes.
*
******************************************************************************/
UNSIGNED8 
bytesize(TYPE_PV_NAME_48 pvname)
{
    UNSIGNED8 size = PVN48GETSIZE(pvname);

    /* for data types and sizes refer to TCN norm part 2, ch. 2.3.1.7 
     * the constants come from the norm
     */
    /* counters are incremented in steps of object size (1, 2 or 4 bytes) */
    /* the cases of the switch are left by return statementsfor ease of
     * implementation
     */
    switch (PVN48GETTYPE(pvname)) {
      case LP_TYPE_A8_ODD:
	return (size * 2) + 1;

      case LP_TYPE_A8_EVEN:
      case LP_TYPE_A16_CARD:
      case LP_TYPE_A16_SIGN:
      case LP_TYPE_A32_CARD:
      case LP_TYPE_A32_SIGN:
	return (size * 2) + 2;

      default:
	return (size == 0) ? 1 : (size * 2);
    }
    /*NOTREACHED*/
}

UNSIGNED8 
bitsize(TYPE_PV_NAME_48 pvname)
{
    UNSIGNED8 size = PVN48GETSIZE(pvname);

    /* for data types and sizes refer to TCN norm part 2, ch. 2.3.1.7 
     * the constants come from the norm
     */
    /* counters are incremented in steps of object size (1, 2 or 4 bytes) */
    /* the cases of the switch are left by return statementsfor ease of
     * implementation
     */
    switch (PVN48GETTYPE(pvname)) {
      case LP_TYPE_BOOL_1:
	return 1;

      case LP_TYPE_BOOL_2:
	return 2;

      case LP_TYPE_BCD_4:
	return 4;

      case LP_TYPE_A8_ODD:
	return (size * 16) + 8;

      case LP_TYPE_A8_EVEN:
      case LP_TYPE_A16_CARD:
      case LP_TYPE_A16_SIGN:
      case LP_TYPE_A32_CARD:
      case LP_TYPE_A32_SIGN:
	return (size * 16) + 16;

      default:
	return (size == 0) ? 8 : (size * 16);
    }
    /*NOTREACHED*/
}



/*new page*/
/******************************************************************************
*
*   function:     Uses type and size of a TCN variable to calculate its size in
*                 bytes and increment the counters
*
*   input:        PV_name
*
*   output:       counters
*
******************************************************************************/
STATIC UNSIGNED8 
add_space_for_pvname(TYPE_PV_NAME_48 pvname, SPACE_INFO *space_info)
{
    UNSIGNED8 size = PVN48GETSIZE(pvname);
    
    /* for data types and sizes refer to TCN norm part 2, ch. 2.3.1.7 
     * the constants come from the norm
     */
    /* counters are incremented in steps of object size (1, 2 or 4 bytes) */
    /* the cases of the switch are left by return statementsfor ease of
     * implementation
     */
    switch (PVN48GETTYPE(pvname)) {
      case LP_TYPE_A8_ODD:
	space_info->size_1 += (size * 2) + 1;
	return 1;
      case LP_TYPE_A8_EVEN:
	space_info->size_1 += (size * 2) + 2;
	return 1;
	
      case LP_TYPE_A16_CARD:
      case LP_TYPE_A16_SIGN:
	space_info->size_2 += (size + 1) * 2;
	return 2;
	
      case LP_TYPE_A32_CARD:
      case LP_TYPE_A32_SIGN:
	space_info->size_4 += (size + 1) * 2;
	return 4;
	
      default:
	switch (size) {
	  case 0:
	    space_info->size_1++;
	    return 1;
	  case 1:
	    space_info->size_2 += 2;
	    return 2;
	  case 2:
	    space_info->size_4 += 4;
	    return 4;
	  case 3:
	  case 4:
	    space_info->size_4 += 8;
	    return 4;
	  default:
	    /* desaster */
	    ASSERT(0);
	    return 0;
	}
	/*NOTREACHED*/
    }
    /*NOTREACHED*/
}



STATIC UNSIGNED8 
add_space_for_pvname_n(TYPE_PV_NAME_48 pvname, 
			   SPACE_INFO *space_info,
			   UNSIGNED16 n)
{
    UNSIGNED8 size = PVN48GETSIZE(pvname);
    
    /* similar to function add_space_for_pvname() but multiplies space
     * requirements according to n. This is used for import
     * marshalling calculations for n ports with same FTF
     */
    /* for data types and sizes refer to TCN norm part 2, ch. 2.3.1.7 
     * the constants come from the norm
     */
    /* counters are incremented in steps of object size (1, 2 or 4 bytes) */
    /* the cases of the switch are left by return statementsfor ease of
     * implementation
     */
    switch (PVN48GETTYPE(pvname)) {
      case LP_TYPE_A8_ODD:
	space_info->size_1 += ((size * 2) + 1) * n;
	return 1;
      case LP_TYPE_A8_EVEN:
	space_info->size_1 += ((size * 2) + 2) * n;
	return 1;
	
      case LP_TYPE_A16_CARD:
      case LP_TYPE_A16_SIGN:
	space_info->size_2 += (size + 1) * n * 2;
	return 2;
	
      case LP_TYPE_A32_CARD:
      case LP_TYPE_A32_SIGN:
	space_info->size_4 += (size + 1) * n * 2;
	return 4;
	
      default:
	switch (size) {
	  case 0:
	    space_info->size_1 += n;
	    return 1;
	  case 1:
	    space_info->size_2 += n * 2;
	    return 2;
	  case 2:
	    space_info->size_4 += n * 4;
	    return 4;
	  case 3:
	  case 4:
	    space_info->size_4 += n * 8;
	    return 4;
	  default:
	    /* desaster */
	    ASSERT(0);
	    return 0;
	}
	/*NOTREACHED*/
    }
    /*NOTREACHED*/
}



STATIC UNSIGNED8 
pv_alignment(TYPE_PV_NAME_48 pvname)
{
    /* for data types and sizes refer to TCN norm part 2, ch. 2.3.1.7 
     * the constants come from the norm
     */
    switch (PVN48GETTYPE(pvname)) {
      case LP_TYPE_A8_ODD:
      case LP_TYPE_A8_EVEN:
	return 1;
	
      case LP_TYPE_A16_CARD:
      case LP_TYPE_A16_SIGN:
	return 2;
	
      case LP_TYPE_A32_CARD:
      case LP_TYPE_A32_SIGN:
	return 4;
	
      default:
	switch (PVN48GETSIZE(pvname)) {
	  case 0:
	    return 1;
	  case 1:
	    return 2;
	  case 2:
	  case 3:
	  case 4:
	    return 4;
	  default:
	    /* desaster */
	    ASSERT(0);
	    return 0;
	}
	/*NOTREACHED*/
    }
    /*NOTREACHED*/
}

/******************************************************************************
*
*   name:          analyse_wtb_ts
*
*   function:      Analyses traffic store of the WTB.
*                  The delivered WTB topography is extended to have fast access
*                  to the needed data (configuration of copy lists)
*
*   input:         pointer to extended topography
*
*   used globals:  p_wtb_topo
*                  wtb_tsid
*
*   output:        initialised and valid extended topography
*
*   return value:  none
*
******************************************************************************/
STATIC void 
analyse_wtb_ts(void)
{
    unsigned int i,j;
    unsigned int port, tcn_addr;
    UNSIGNED16 ftfield = 0;
    struct STR_APD_PV_DESCRIPTOR pv_descr;

    /* init PV_NAME to get access to FTF */
    pv_descr.pv_name.type = LP_TYPE_CARD;
    pv_descr.pv_name.size = (pdm_ftf_size == 1) ? LP_SIZE_8 : LP_SIZE_16;
    pv_descr.pv_name.bit_offset = 0;
    pv_descr.pv_name.ctl_offset = 0x3ff;
    pv_descr.pv_name.ts_id = wtb_tsid;
    pv_descr.p_variable = &ftfield;
    pv_descr.p_validity = NULL;

    /* initialise pdm topo structure */
    pdm_topo.n_vehicles = 0;
    for (i = 0; i <= MAX_NODE_ADDR; i++) {
	pdm_topo.ftf_list[i] = PDM_FTF_INVALID;
    }
    for (i = 0; i <= MAX_NODE_NUM; i++) {
	pdm_topo.ftf_info[i].ftf = PDM_FTF_INVALID;
	pdm_topo.ftf_info[i].n_vehicles = 0;
	pdm_topo.ftf_info[i].n_vars = 0;
	pdm_topo.ftf_info[i].i_pvdescr = 0;
    }
    
    pdm_topo.wtb_ts_id = wtb_tsid;

    pdm_topo.n_vehicles = p_wtb_topo->number_of_nodes;

    tcn_addr = p_wtb_topo->top_address; /* get the first TCN address */

    /* analyse traffic store and fill topo structure */
    for (i = 0; i < pdm_topo.n_vehicles; i++) {
	if (tcn_addr != p_wtb_topo->node_address) {
	    port = tcn_addr; 
	
	    /* get frame type field */
	    pv_descr.pv_name.prt_addr =  port;
#ifdef APD_BUG
	    pv_descr.pv_name.prt_addr <<= 2;
#endif
	    
	    if (apd_get_variable(&pv_descr, NULL) == LPS_OK) {
		/* search entry in FTF info */
		for (j = 0; 
		     pdm_topo.ftf_info[j].ftf != PDM_FTF_INVALID
		     &&
		     pdm_topo.ftf_info[j].ftf != ftfield;
		     j++) /*NOP*/;
		pdm_topo.ftf_info[j].ftf = ftfield;
		/* increment port count for FTF */
		pdm_topo.ftf_info[j].n_vehicles++;
		pdm_topo.ftf_list[port] = ftfield;
	    }
	}
	/* get next tcn node address */
	if(--tcn_addr == 0) {
	    /* NOTE: addresses are in order 2, 1, 63, 62, ... */
	    tcn_addr = 63;
	}
    }

}



/*
 * get pointer to FTF info for a given FTF
 */
STATIC T_FTF_INFO *
search_ftf_info(UNSIGNED16 ftf)
{
    T_FTF_INFO *p_info;

    /* search entry in FTF info */
    p_info = pdm_topo.ftf_info;
    while (p_info->ftf != PDM_FTF_INVALID && p_info->ftf != ftf) {
	p_info++;
    }
    return (p_info->ftf == ftf) ? p_info : NULL;
}

/*new page*/
/******************************************************************************
*
*   name:         find_mode_in_db
*
*   function:     searching of the data base for
*                 marshalling mode (e.g. leading vehicle) and
*                 marshalling type (e.g. internal marshalling) and returns
*                 the pointer to the right list of the data base or
*
*   input:        marshalling type
*                 marshalling mode
*
*   used globals: none
*
*   output:       pointer to data base position of the found list or NULL
*
*   return value: Error: PDM_OK (list found)
*                        PDM_ERR_NOT_FOUND (no list available)
*
******************************************************************************/
STATIC int 
find_mode_in_db (UNSIGNED16 type, UNSIGNED16 mode,
                     void * p_db, void ** p_db_act, UNSIGNED16 * p_cycle)
{
    /*search until marshalling mode found or end of data base*/
    *p_db_act = p_db;
    for(;;) {
	/* calculate pointer to next marshalling list element
	 * (first element is the database header which is not interesting here)
	 */
	*p_db_act = (void *)((UNSIGNED8 *)*p_db_act +
			     ((T_MDB_SUB_HDR *)*p_db_act)->length);
	
	if (((T_MDB_SUB_HDR*)*p_db_act)->type == type
	    &&
	    ((T_MDB_LIST_HDR*)*p_db_act)->mode == mode) {
	    *p_cycle = ((T_MDB_LIST_HDR *)*p_db_act)->cycle;
	    return PDM_OK;
	}

	/*end of data base reached? => list not found*/
	if (((T_MDB_SUB_HDR*)*p_db_act)->type == PDM_END_OF_DB    
	    ||
	    (UNSIGNED8 *)*p_db_act >=  
	    (UNSIGNED8 *)p_db + ((T_MDB_DB_HDR *)p_db)->total_length_of_db) {
	    *p_db_act = (T_MDB_LIST_HDR *)NULL;
	    return PDM_ERR_NOT_FOUND;
	}
    }
    /*NOTREACHED*/
}

/*
 * Free a copy list
 */
STATIC void  
pdm_free_cpl(T_PDM_COPY_LIST *p_cpl)
{
    unsigned short i;
    unsigned short n_from_ports = p_cpl->n_from_ports;
    unsigned short n_to_ports = p_cpl->n_to_ports;

/* <ATR:01> */
//  if (p_cpl->configured == FALSE) {
//	    return;
//  }
    if (p_cpl->p_var_buf) pdm_free(p_cpl->p_var_buf);
    if (p_cpl->p_from_ds) {
	for (i = 0; i < n_from_ports; i++) {
	    if (p_cpl->p_from_ds[i].p_ds) {
		pdm_free(p_cpl->p_from_ds[i].p_ds);
	    }
	}
	pdm_free(p_cpl->p_from_ds);
    }
    if (p_cpl->p_from_apd_pv_descr) pdm_free(p_cpl->p_from_apd_pv_descr);
    if (p_cpl->p_fresh) pdm_free(p_cpl->p_fresh);
    if (p_cpl->p_func_info) pdm_free(p_cpl->p_func_info);
    if (p_cpl->p_func_args) pdm_free(p_cpl->p_func_args);
    if (p_cpl->p_to_ds) {
	for (i = 0; i < n_to_ports; i++) {
	    if (p_cpl->p_to_ds[i].p_ds) {
		pdm_free(p_cpl->p_to_ds[i].p_ds);
	    }
	}
	pdm_free(p_cpl->p_to_ds);
    }
    p_cpl->configured = FALSE;
}

STATIC void
gen_pv_set(UNSIGNED16 dir, 
	   T_PDM_DATASET *p_pds, 
	   TYPE_APD_PV_DESCRIPTOR *p_pvdescr, 
	   PORT_INFO *port_info)
{
#ifdef DEBUG
    PDM_Dbg(("\nlpd_gen_pv_set(): \n"));
    dump_pvdescr(p_pvdescr, port_info->vars);
#endif
    lpd_gen_pv_set(p_pds->p_ds,
		   p_pvdescr,
		   dir == PDM_FROM ? &p_pds->dir.from.ds_freshness : NULL,
		   port_info->vars);
    p_pds->vars = port_info->vars;

#ifdef DEBUG
    PDM_Dbg(("p_pv_set: ts_id 0x%04x, prt_size %3d, c_pv_list %3d\n",
	     p_pds->p_ds->ts_id,
	     p_pds->p_ds->prt_sze,
	     p_pds->p_ds->c_pv_list));
    dump_pvset(p_pds->p_ds, p_pds->p_ds->c_pv_list);
#endif
}


/* copy PV_Name from 48 bit unsigned int to APD bit field structure */
STATIC void
copy_pvname(struct STR_APD_PV_NAME *apd_pv_name,
	    TYPE_PV_NAME_48 pdm_pv_name)
{
    apd_pv_name->ts_id       = PVN48GETTSID(pdm_pv_name);
    apd_pv_name->prt_addr    = PVN48GETPORT(pdm_pv_name);
    apd_pv_name->size        = PVN48GETSIZE(pdm_pv_name);
    apd_pv_name->bit_offset  = PVN48GETVAROFF(pdm_pv_name);
    apd_pv_name->type        = PVN48GETTYPE(pdm_pv_name);
    apd_pv_name->ctl_offset  = PVN48GETCHKOFF(pdm_pv_name);
}


STATIC void
set_pvdescr(T_PDM_COPY_LIST *p_cpl,
	    TYPE_APD_PV_DESCRIPTOR *p_pv, 
	    T_MDB_FROMVAR *p_from_vars,
	    SPACE_INFO *space_info)
{
    /* copy PV_Name */
    p_pv->pv_name.ts_id       = PVN48GETTSID(p_from_vars->pv_name);
    p_pv->pv_name.prt_addr    = PVN48GETPORT(p_from_vars->pv_name);
    p_pv->pv_name.size        = PVN48GETSIZE(p_from_vars->pv_name);
    p_pv->pv_name.bit_offset  = PVN48GETVAROFF(p_from_vars->pv_name);
    p_pv->pv_name.type        = PVN48GETTYPE(p_from_vars->pv_name);
    p_pv->pv_name.ctl_offset  = PVN48GETCHKOFF(p_from_vars->pv_name);
    
    /* get variable alignment, size and check variable offsets */
    if (PVN48GETCHKOFF(p_from_vars->pv_name) != PDM_CHKVARVOID
	&& p_from_vars->overlay == PDM_CHKUNIQUE) {
	p_pv->p_validity = (UNSIGNED8 *)p_cpl->p_var_buf + space_info->chks_offset;
	space_info->chks_offset++;
    }
    else {
	/* do not bother the lp with overlayed check variables, they are
	 * fetched only once.
	 */
	p_pv->p_validity = NULL;
    }
    
    switch (pv_alignment(p_from_vars->pv_name)) {
      case 1:
	p_pv->p_variable = (UNSIGNED8 *)p_cpl->p_var_buf + space_info->size_1;
	break;
      case 2:
	p_pv->p_variable = (UNSIGNED16 *)
	    ((UNSIGNED8 *)p_cpl->p_var_buf + space_info->size_2);
	break;
      case 4:
	p_pv->p_variable = (UNSIGNED32 *)
	    ((UNSIGNED8 *)p_cpl->p_var_buf + space_info->size_4);
	break;
      default:
	ASSERT(0);
	break;
    }
    /* increment offset in variable buffer */
    add_space_for_pvname(p_from_vars->pv_name, space_info);
}

/* in the pv description are some NULL pointers for configured check
 * variables to optimize data access by lp. This NULL pointers have
 * to be replaced by valid addresses.
 */
void
replace_nullptrs(T_MDB_FROMVAR *p_from_vars, 
		 TYPE_APD_PV_DESCRIPTOR *p_from_apd_pv_descr,
		 UNSIGNED16 n_from_vars) 
{
    UNSIGNED16 i;
    UNSIGNED16 ref;
    TYPE_APD_PV_DESCRIPTOR *p_pvdescr;
    
    p_pvdescr = p_from_apd_pv_descr;
    for (i = 0; i < n_from_vars; i++) {
	if ((ref = p_from_vars->overlay) != PDM_CHKUNIQUE) {
	    switch (ref & (PDM_CHKOVERLAYCHK | PDM_CHKOVERLAYVAR)) {
	      case PDM_CHKOVERLAYCHK:
		p_pvdescr->p_validity = 
		    p_from_apd_pv_descr[ref & PDM_REFMASK].p_validity;
		break;
	      case PDM_CHKOVERLAYVAR:
		p_pvdescr->p_validity = 
		    p_from_apd_pv_descr[ref & PDM_REFMASK].p_variable;
		break;
	      default:
		ASSERT(0);
		break;
	    }
	}
	p_pvdescr++;
	p_from_vars++;
    }
}

STATIC int
set_f_up_cast(F_CAST *p_cast, UNSIGNED16 calc_type, UNSIGNED16 sizetype)
{
    if (calc_type == PDM_CALC_R32) {
	switch (sizetype) {
	  case  PDM_U4: *p_cast = pdm_cast_U4_R32;	break;
	  case  PDM_U8: *p_cast = pdm_cast_U8_R32;	break;
	  case PDM_U16: *p_cast = pdm_cast_U16_R32;	break;
	  case PDM_U32: *p_cast = pdm_cast_U32_R32;	break;
	  case  PDM_I8: *p_cast = pdm_cast_I8_R32;	break;
	  case PDM_I16: *p_cast = pdm_cast_I16_R32;	break;
	  case PDM_I32: *p_cast = pdm_cast_I32_R32;	break;
	  case PDM_FI2: *p_cast = pdm_cast_FI2_R32;	break;
	  case PDM_FU4: *p_cast = pdm_cast_FU4_R32;	break;
	  case PDM_FI8: *p_cast = pdm_cast_FI8_R32;	break;
	  default: 		       			return FALSE;
	}
    }
    else {
	switch (sizetype) {
	  case  PDM_U4: *p_cast = pdm_cast_U4_U32;	break;
	  case  PDM_U8: *p_cast = pdm_cast_U8_U32;	break;
	  case PDM_U16: *p_cast = pdm_cast_U16_U32;	break;
	  case PDM_U32: *p_cast = pdm_cast_U32_U32;	break;
	  case  PDM_I8: *p_cast = pdm_cast_I8_I32;	break;
	  case PDM_I16: *p_cast = pdm_cast_I16_I32;	break;
	  case PDM_I32: *p_cast = pdm_cast_I32_I32;	break;

	  case  PDM_B1: *p_cast = pdm_cast_B1_B1;	break;
	  case  PDM_B2: *p_cast = pdm_cast_B2_B1;	break;
	  case  PDM_B8: 
	  case PDM_B16: 
	  case PDM_B32: *p_cast = pdm_cast_BSET_B1;	break;
	  default: 		       			return FALSE;
	}
    }
    return TRUE;
}

STATIC F_VALID
set_f_is_valid(UNSIGNED16 sizetype)
{
    switch (sizetype) {
      case  PDM_B1:
	return pdm_isvalid_1;
      case  PDM_B2:
	return pdm_isvalid_2;
      case  PDM_U4:
	return pdm_isvalid_4;
      case  PDM_I8:
      case  PDM_U8:
      case  PDM_B8:
	return pdm_isvalid_8;
      case PDM_U16:
      case PDM_I16:
      case PDM_B16:
      case PDM_FI2:
      case PDM_FU4:
      case PDM_FI8:
	return pdm_isvalid_16;
      case PDM_U32:
      case PDM_I32:
	return pdm_isvalid_32;
      case PDM_B64:
	return pdm_isvalid_64;
      default: 		
	return pdm_never_valid;
    }
    /*NOTREACHED*/
}

STATIC F_INVALID
set_f_invalidate(UNSIGNED16 sizetype)
{
    switch (sizetype) {
      case  PDM_B1:
      case  PDM_B2:
      case  PDM_U4:
      case  PDM_I8:
      case  PDM_U8:
      case  PDM_B8:
	return pdm_invalid_8;
      case PDM_U16:
      case PDM_I16:
      case PDM_B16:
      case PDM_FI2:
      case PDM_FU4:
      case PDM_FI8:
	return pdm_invalid_16;
      case PDM_B32:
      case PDM_R32:
      case PDM_U32:
      case PDM_I32:
	return pdm_invalid_32;
      default: 		
	return pdm_invalid_8; /* do not do so much harm */
    }
    /*NOTREACHED*/
}
   
STATIC int
set_f_down_cast(F_CAST *p_cast, UNSIGNED16 sizetype)
{
    /* in p_cast is the function calculation type remembered */
    if (*p_cast == pdm_cast_R32_R32) {
	switch (sizetype) {
	  case  PDM_U4: *p_cast = pdm_cast_R32_U4;	break;
	  case  PDM_U8: *p_cast = pdm_cast_R32_U8;	break;
	  case PDM_U16: *p_cast = pdm_cast_R32_U16;	break;
	  case PDM_U32: *p_cast = pdm_cast_R32_U32;	break;
	  case  PDM_I8: *p_cast = pdm_cast_R32_I8;	break;
	  case PDM_I16: *p_cast = pdm_cast_R32_I16;	break;
	  case PDM_I32: *p_cast = pdm_cast_R32_I32;	break;
	  case PDM_FI2: *p_cast = pdm_cast_R32_FI2;	break;
	  case PDM_FU4: *p_cast = pdm_cast_R32_FU4;	break;
	  case PDM_FI8: *p_cast = pdm_cast_R32_FI8;	break;
	  default: 	*p_cast = (F_CAST)set_f_invalidate(sizetype);	break;
	}
    }
    else {
	switch (sizetype) {
	  case  PDM_U4: *p_cast = pdm_cast_U32_U4;	break;
	  case  PDM_U8: *p_cast = pdm_cast_U32_U8;	break;
	  case PDM_U16: *p_cast = pdm_cast_U32_U16;	break;
	  case PDM_U32: *p_cast = pdm_cast_U32_U32;	break;
	  case  PDM_I8: *p_cast = pdm_cast_I32_I8;	break;
	  case PDM_I16: *p_cast = pdm_cast_I32_I16;	break;
	  case PDM_I32: *p_cast = pdm_cast_I32_I32;	break;

	  case  PDM_B1: *p_cast = pdm_cast_B1_B1;	break;
	  case  PDM_B2: *p_cast = pdm_cast_B1_B2;	break;
	  default: 	*p_cast = (F_CAST)set_f_invalidate(sizetype);	break;
	}
    }
    return TRUE;
}

STATIC void
init_sdir(TYPE_APD_PV_DESCRIPTOR *p_pv, 
	  UNSIGNED16 ts_id, 
	  UNSIGNED16 prt_addr,
	  UNSIGNED8  *buf,
	  UNSIGNED16 ftf,
	  UNSIGNED16 *tcn,
	  UNSIGNED16 parts)
{
    UNSIGNED16 i;
    UNSIGNED16 idx = *tcn;

    p_pv->p_validity = NULL; /* source dir has no check variable */
    p_pv->p_variable = buf;
    p_pv->pv_name.ts_id       = ts_id;
    p_pv->pv_name.prt_addr    = prt_addr;
    p_pv->pv_name.size        = PDM_S_DIR_SIZE;
    p_pv->pv_name.bit_offset  = PDM_S_DIR_OFFSET;
    p_pv->pv_name.type        = PDM_S_DIR_TYPE;
    p_pv->pv_name.ctl_offset  = PDM_CHKVARVOID;
    for (i = 0; i < parts; i++) {
	while(pdm_topo.ftf_list[idx] != ftf) {
	    idx++;
	    ASSERT(idx <= MAX_NODE_ADDR);
	}
	buf[idx >> 3] |= 1 << (idx & 0x7);
	idx++;
    }
#ifdef DEBUG
    pr_err("\n0x%08x: (%2d) ", buf, parts);
    for (i = 0; i < 64; i++) {
	pr_err("%d", (buf[i >> 3] & 1 << (i & 0x7)) != 0);
    }
    pr_err("\n");
#endif
    *tcn = idx;
}

/******************************************************************************
*
*   BUILD_COPY_LIST
*
******************************************************************************/
int 
build_copy_list(T_PDM_COPY_LIST *p_cpl, void *p_data_base)
{
    unsigned int i, j;
    UNSIGNED16 n_from_ports, n_from_vars;
    UNSIGNED16 n_funcs, n_func_args;
    UNSIGNED16 n_func_infos, n_func_info_args;
    UNSIGNED16 n_to_ports, n_to_vars;
    UNSIGNED16 *p_fr;
    UNSIGNED16 *p_fresh;
    void * p_db_pos;
    T_MDB_FROMVAR * p_from_vars;
    T_MDB_TOVAR * p_to_vars;
    T_MDB_TOVAR_IM * p_to_vars_im;
    T_MDB_FUNC_HDR * p_func_cfg;    /*pointer to act. function list in db */
    T_MDB_FUNC_ARG * p_func;         /*pointer to input array of functions*/
    T_MDB_FROMVAR_HDR * p_from_cfg;  /*pointer to from-variable list in db*/
    T_MDB_FROMVAR_HDR * pfc;         
    T_MDB_TOVAR_HDR * p_to_cfg;      /*pointer to act. dest. list */
    T_MDB_SUB_HDR * p_dflts_cfg;
    UNSIGNED8        *p_dflts;      /*pointer to default value list */

    unsigned int curr_port;                /* for each FROM/TO variables */
    unsigned int newdsname, lastdsname;  /* detect change from port to port */
    SPACE_INFO space_info;        /* space requirements for vars and checks */
    PORT_INFO *from_port_info;    /* # of vars and checks per FROM variable  */
    PORT_INFO *to_port_info;      /* # of vars and checks per TO variable */
    PORT_INFO *p_info;            /* loop variable for port info */
    unsigned int chk_space;       /* space for all unique check vars over all
				     ports */
    unsigned int var_space;       /* space for all variables */
    unsigned int srcdir_space;       /* space for all variables */
    TYPE_APD_PV_DESCRIPTOR * p_to_apd_pv_descr;
    TYPE_APD_PV_DESCRIPTOR *p_pvdescr;
    TYPE_APD_PV_DESCRIPTOR *p_pv;
    T_PDM_FUNC_ARG *p_arg;
    UNSIGNED16 ref;                   /* variable reference */
    UNSIGNED16 flags;                 /* variable flags */
    int result;
    T_FTF_INFO *p_ftf_info;
    UNSIGNED16 *func_ref2idx = NULL; /* convert function result reference to
				      * index in copy list function array
				      */
    UNSIGNED16 user_mode = p_cpl->mode;
    UNSIGNED16 marsh_type = p_cpl->type;

    if (p_cpl->status == PDM_ENABLED) {
	/* stop timer */
        pi_disable_timeout(p_cpl->timeout_id);
    }

    
    if (user_mode == PDM_DO_NOT_MARSH) {
	/* copy list is disabled because of mode */
	p_cpl->status = PDM_DISABLED;
	return PDM_OK;
    }

    /* clean previous copy list */
    pdm_free_cpl(p_cpl);

    /* find user mode in database and get position within db */
    /*-------------------------------------------------------*/
    if ((result = find_mode_in_db(marsh_type, user_mode,
				  p_data_base, 
				  &p_db_pos, &p_cpl->cycle)) != PDM_OK){
	RECORD("User_mode not found");
	goto BUILDERR;
    }
    
    if (marsh_type == PDM_IMPORT) {
	analyse_wtb_ts();
    }

    /* set pointer for FROM, FUNC and TO list in NSDB (local vars for speed) */
    /*-----------------------------------------------------------------------*/
    p_from_cfg = (T_MDB_FROMVAR_HDR *)((UNSIGNED8 *)p_db_pos +
					   sizeof(T_MDB_LIST_HDR));
    if (marsh_type == PDM_IMPORT) {
	n_from_ports = 0;
	for (pfc = p_from_cfg; 
	     (pfc->hdr.type & PDM_FROM) == PDM_FROM;
	     pfc = (T_MDB_FROMVAR_HDR *)(NEXTSUBLIST(pfc))) {
	    if ((p_ftf_info = search_ftf_info(pfc->uu.ftf)) != NULL) {
		n_from_ports += p_ftf_info->n_vehicles;
		p_ftf_info->n_vars = pfc->n_vars + 1;
	    }
	}
    }
    else {
	n_from_ports = p_from_cfg->uu.n_ports;
	n_from_vars = p_from_cfg->n_vars;
	pfc = (T_MDB_FROMVAR_HDR *)(NEXTSUBLIST(p_from_cfg));
    }

    p_cpl->n_from_ports = n_from_ports;
	
    p_func_cfg = (T_MDB_FUNC_HDR *)(pfc);
    n_funcs    = p_func_cfg->n_funcs;
    n_func_args    = p_func_cfg->n_args;
    if (marsh_type == PDM_IMPORT) {
	UNSIGNED16 n;
	n_func_infos = 0;
	n_func_info_args = 0;
	i = 0;
	p_func = (T_MDB_FUNC_ARG *)(p_func_cfg + 1);
	while (i < n_func_args + n_funcs) {
	    if (p_func->uu.func.ref & PDM_FUNC) {
		if (p_func->uu.func.flags & PDM_S_DIR) {
		    p_func++;
		    i++;
		    if ((p_ftf_info = search_ftf_info(p_func->uu.arg.ftf)) != NULL) {
			n = p_ftf_info->n_vehicles;
		    }
		    else  {
			n = 1;
		    }
		    n_func_info_args += n;
		    n_func_infos += n ;
		}
		else {
		    n_func_infos++;
		}
	    }
	    else {
		if ((p_ftf_info = search_ftf_info(p_func->uu.arg.ftf)) != NULL) {
		    n = p_ftf_info->n_vehicles;
		}
		else  {
		    n = 1;
		}
		n_func_info_args += n;
	    }
	    p_func++;
	    i++;
	}
    }
    else {
	n_func_infos = n_funcs;
	n_func_info_args = n_func_args;
    }

    /* allocate function reference converter */
    if (p_func_cfg->n_funcs != 0 
	&& (func_ref2idx = pdm_alloc((p_func_cfg->n_funcs) * sizeof(*func_ref2idx))) == NULL) {
	goto NOMEM;
    }

    /* pointer to-variable sublist */
    p_to_cfg = (T_MDB_TOVAR_HDR *)(NEXTSUBLIST(p_func_cfg));
    n_to_ports = p_to_cfg->n_ports;

    p_dflts_cfg = (T_MDB_SUB_HDR *)(NEXTSUBLIST(p_to_cfg));
    /* pointer default value sublist */
    if (p_dflts_cfg->type & PDM_DEFAULT_VAL) {
	p_dflts = (UNSIGNED8 *)p_dflts_cfg + sizeof(*p_dflts_cfg);
	PDM_Dbg(("default buffer at address 0x%08x\n", p_dflts))
    }
    else {
	p_dflts = NULL;
    }

    /* init long term buffer pointers */
    /*--------------------------------*/
    p_cpl->p_var_buf = NULL;
    p_cpl->p_from_ds = NULL;
    p_cpl->p_fresh = NULL;
    p_cpl->p_func_info = NULL;
    p_cpl->p_func_args = NULL;
    p_cpl->p_to_ds = NULL;

    /* init temporary buffer pointers */
    p_cpl->p_from_apd_pv_descr = NULL;
    p_to_apd_pv_descr = NULL;

    /* allocate both data structures for FROM and TO variables info at once */
    if ((from_port_info = pdm_alloc((n_from_ports + n_to_ports) * 
				    sizeof(PORT_INFO))) == NULL) {
	goto NOMEM;
    }

    /* initialize port info with zero */
    memset (from_port_info, 0, (n_from_ports + n_to_ports) * 
				    sizeof(PORT_INFO));
    to_port_info = from_port_info + n_from_ports;

    /* initialize variable and check space info with zero */
    space_info.size_1 = 0;
    space_info.size_2 = 0;
    space_info.size_4 = 0;
    

    /* compute space requirements for FROM variables */
    /*-----------------------------------------------*/
    curr_port = 0;
    if (marsh_type == PDM_IMPORT) {
	pfc = p_from_cfg; 
	while ((pfc->hdr.type & PDM_FROM) == PDM_FROM) {
	    if ((p_ftf_info = search_ftf_info(pfc->uu.ftf)) != NULL) {
		p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)pfc +
						sizeof(T_MDB_FROMVAR_HDR));
		chk_space = 0;
		for (i = 0; i < pfc->n_vars; i++) {
		    add_space_for_pvname_n(p_from_vars->pv_name, &space_info,
					   p_ftf_info->n_vehicles);
		    if (PVN48GETCHKOFF(p_from_vars->pv_name) != PDM_CHKVARVOID &&
			p_from_vars->overlay == PDM_CHKUNIQUE) {
			/* add a chk var for the lp pv description */
			chk_space++;
		    }
		    p_from_vars++;
		}
		for (i = 0; i < p_ftf_info->n_vehicles; i++) {
		    /* ftf and n variables */
		    from_port_info[curr_port + i].vars = pfc->n_vars + 1;
		    from_port_info[curr_port + i].chks = chk_space;
		}
		curr_port += p_ftf_info->n_vehicles;
	    }
	    pfc = (T_MDB_FROMVAR_HDR *)(NEXTSUBLIST(pfc));
	}
    }
    else {
	p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)p_from_cfg +
					       sizeof(T_MDB_FROMVAR_HDR));
	lastdsname = PVN48GETDSNAME(p_from_vars->pv_name);
	for (i = 0; i < n_from_vars; i++) {
	    if ((newdsname = PVN48GETDSNAME(p_from_vars->pv_name)) != lastdsname) {
		lastdsname = newdsname;
		curr_port++;
		ASSERT(curr_port < n_from_ports);
	    }
	    add_space_for_pvname(p_from_vars->pv_name, &space_info);
	    /* a var for the lp pv description */
	    from_port_info[curr_port].vars++;
	    if (PVN48GETCHKOFF(p_from_vars->pv_name) != PDM_CHKVARVOID &&
		p_from_vars->overlay == PDM_CHKUNIQUE) {
		/* and a chk var for the lp pv description */
		from_port_info[curr_port].chks++;
	    }
	    p_from_vars++;
	}
    }
    
    /* compute space requirements for TO variables and function results */
    /*------------------------------------------------------------------*/
    srcdir_space = 0;
    curr_port = 0;
    n_to_vars = p_to_cfg->n_vars;
    if (marsh_type == PDM_IMPORT) {
	int rest;
	UNSIGNED16 parts;
	UNSIGNED16 n_vehicles;
	UNSIGNED16 n_srcdir;


	p_to_vars_im = (T_MDB_TOVAR_IM *)((UNSIGNED8 *)p_to_cfg +
					    sizeof(T_MDB_TOVAR_HDR));
    
	lastdsname = PVN48GETDSNAME(p_to_vars_im->pv_name);
	for (i = 0; i < n_to_vars; i++) {
	    if ((newdsname = PVN48GETDSNAME(p_to_vars_im->pv_name)) != lastdsname) {
		lastdsname = newdsname;
		curr_port++;
		ASSERT(curr_port < n_to_ports);
	    }
	    if (p_to_vars_im->flags & (PDM_NORM | PDM_ROUTE)) {
		to_port_info[curr_port].vars++;
		if (PVN48GETCHKOFF(p_to_vars_im->pv_name) != PDM_CHKVARVOID) {
		    /* and a chk var for the lp pv description */
		    to_port_info[curr_port].chks++;
		}
		if (p_to_vars_im->ref & PDM_FUNC) {
		    /* add space for function results */
		    add_space_for_pvname(p_to_vars_im->pv_name, &space_info);
		}
	    }
	    else if (p_to_vars_im->flags & PDM_S_DIR) {
		p_ftf_info = search_ftf_info(p_to_vars_im->ftf);
		if (p_ftf_info != NULL) {
		    n_vehicles = p_ftf_info->n_vehicles;
		    if (PVN48GETCHKOFF(p_to_vars_im->pv_name) != PDM_CHKVARVOID) {
			parts = 32 / bytesize(p_to_vars_im->pv_name);
			n_srcdir = 0; /* only one source directory */
			/* and n_vehicles check variables written as variables */
			to_port_info[curr_port].vars = 1 + n_vehicles;
			space_info.size_1 += n_vehicles;
			to_port_info[curr_port].chks = 0;
			curr_port++;
			/* reserve space for one source dir and for check var list */
			srcdir_space += PDM_S_DIR_LEN;
		    }
		    else {
			parts = 24 / bytesize(p_to_vars_im->pv_name);
			n_srcdir = 1; /* source directory in each port */
		    }
		    rest = n_vehicles;
		    while (rest > 0) {
			to_port_info[curr_port].vars = min(parts, rest) + n_srcdir;
			to_port_info[curr_port].chks = 0;
			srcdir_space += n_srcdir * PDM_S_DIR_LEN; /* reserve space if needed */
			if ((rest -= parts) > 0) {
			    curr_port++;
			}
		    }
		    if (p_to_vars_im->ref & PDM_FUNC) {
			/* add space for function results */
			add_space_for_pvname_n(p_to_vars_im->pv_name, &space_info,
					       n_vehicles);
		    }
		}
		else {
		    /* only the source directory (no bits set) and nothing else */
		    to_port_info[curr_port].vars = 1;
		}
	    }

	    p_to_vars_im++;
	}
    }
    else {
	/* space req. for TO variables (PDM_INTERN, PDM_EXPORT) */
	p_to_vars = (T_MDB_TOVAR *)((UNSIGNED8 *)p_to_cfg +
					    sizeof(T_MDB_TOVAR_HDR));
    
	lastdsname = PVN48GETDSNAME(p_to_vars->pv_name);
	for (i = 0; i < n_to_vars; i++) {
	    if ((newdsname = PVN48GETDSNAME(p_to_vars->pv_name)) != lastdsname) {
		lastdsname = newdsname;
		curr_port++;
		ASSERT(curr_port < n_to_ports);
	    }
	    if (p_to_vars->ref & PDM_FUNC) {
		/* add space for function results */
		add_space_for_pvname(p_to_vars->pv_name, &space_info);
	    }
	    /* a var for the lp pv description */
	    to_port_info[curr_port].vars++;
	    if (PVN48GETCHKOFF(p_to_vars->pv_name) != PDM_CHKVARVOID) {
		/* and a chk var for the lp pv description */
		to_port_info[curr_port].chks++;
	    }
	    p_to_vars++;
	}
    }


    /* calculate storage requirements for complete variable buffer */
    /*-------------------------------------------------------------*/
    p_info = from_port_info;
    chk_space = 0;
    n_from_vars = 0;
    n_to_vars = 0;
    for (i = 0; i < n_from_ports; i++) {
	chk_space += p_info->chks;
	n_from_vars += p_info->vars;
	p_info++;
    }
    for (i = 0; i < n_to_ports; i++) {
	chk_space += p_info->chks;
	n_to_vars += p_info->vars;
	p_info++;
    }
    var_space = space_info.size_4 + space_info.size_2 + space_info.size_1 ;

    PDM_Dbg(("var_space %d, chk_space %d, srcdir_space %d\n", 
	     var_space, chk_space, srcdir_space));

    /* allocate buffers */
    /*------------------*/
    if ((p_cpl->p_var_buf = pdm_alloc(srcdir_space + 
				      var_space + chk_space)) == NULL
	||
	(p_cpl->p_from_ds = pdm_alloc(n_from_ports *
				    sizeof(T_PDM_DATASET))) == NULL
	||
	(p_cpl->p_from_apd_pv_descr = pdm_alloc(n_from_vars *
					 sizeof(TYPE_APD_PV_DESCRIPTOR))) == NULL
	||
	(p_cpl->p_fresh = pdm_alloc(n_from_vars *
					   sizeof(unsigned short))) == NULL
	||
	(n_funcs > 0 
	 &&
	 (p_cpl->p_func_info = pdm_alloc(n_func_infos *
				       sizeof(T_PDM_FUNC_INFO))) == NULL
	 )
	||
	(n_func_args > 0 
	 &&
	 (p_cpl->p_func_args = pdm_alloc(n_func_info_args *
				       sizeof(T_PDM_FUNC_ARG))) == NULL
	 )
	||
	(p_cpl->p_to_ds = pdm_alloc(n_to_ports *
				    sizeof(T_PDM_DATASET))) == NULL
	||
	/* temporary buffers */
	(p_to_apd_pv_descr = pdm_alloc(n_to_vars *
				       sizeof(TYPE_APD_PV_DESCRIPTOR))) == NULL
	) {
	goto NOMEM;
    }

    /* clear variable buffer */
    memset(p_cpl->p_var_buf, 0, srcdir_space + var_space + chk_space);

    /* initialize pointers for every FROM / TO variable */
    for (i = 0; i < n_from_ports; i++) {
	p_cpl->p_from_ds[i].p_ds = NULL;
    }
    for (i = 0; i < n_to_ports; i++) {
	p_cpl->p_to_ds[i].p_ds = NULL;
    }

    /* allocate buffers for FROM lpd_gen_pv_set() */
    PDM_Dbg(("Allocate mem for FROM pv descriptor\n"));
    for (i = 0; i < n_from_ports; i++) {
	PDM_Dbg(("port %3d: vars %3d, chks %3d\n", i, 
		 from_port_info[i].vars, from_port_info[i].chks));
	if ((p_cpl->p_from_ds[i].p_ds = 
	     pdm_alloc(sizeof(TYPE_APD_PV_SET) +  
		       (from_port_info[i].vars + from_port_info[i].chks - 1) *
		       (sizeof(TYPE_APD_PV_LIST)))) == NULL) {
	    goto NOMEM;
	}
    }

    /* allocate buffers for TO lpd_gen_pv_set() */
    PDM_Dbg(("Allocate mem for TO pv descriptor\n"));
    for (i = 0; i < n_to_ports; i++) {
	PDM_Dbg(("port %3d: vars %3d, chks %3d\n", i, 
		 to_port_info[i].vars, to_port_info[i].chks));
	if ((p_cpl->p_to_ds[i].p_ds = 
	     pdm_alloc(sizeof (TYPE_APD_PV_SET) +  
		       (to_port_info[i].vars + to_port_info[i].chks - 1) *
		       (sizeof (TYPE_APD_PV_LIST)))) == NULL) {
	    goto NOMEM;
	}
    }

    /* use space infos as offsets in variable buffer */
    space_info.size_1 = srcdir_space + space_info.size_2 + space_info.size_4;
    space_info.size_2 = srcdir_space + space_info.size_4;
    space_info.size_4 = srcdir_space;
    space_info.chks_offset = srcdir_space + var_space;
    srcdir_space = 0;

    PDM_Dbg(("\np_variable buffer 0x%08x\n", p_cpl->p_var_buf));
    PDM_Dbg(("    offset size 1 0x%08x\n", (UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_1));
    PDM_Dbg(("    offset size 2 0x%08x\n", (UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_2));
    PDM_Dbg(("    offset size 4 0x%08x\n", (UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_4));
    PDM_Dbg(("    offset chks   0x%08x\n", (UNSIGNED8 *)p_cpl->p_var_buf + space_info.chks_offset));

    /* generate lp access data structures for FROM variables */
    /*-------------------------------------------------------*/
    p_pv = p_pvdescr = p_cpl->p_from_apd_pv_descr;
    p_fr = p_fresh = p_cpl->p_fresh;
    if (marsh_type == PDM_IMPORT) {
	pfc = p_from_cfg; 
	curr_port = 0;
	while ((pfc->hdr.type & PDM_FROM) == PDM_FROM) {
	    if ((p_ftf_info = search_ftf_info(pfc->uu.ftf)) != NULL) {
		UNSIGNED16 prt_addr = 0;
		while (prt_addr < 64 && pdm_topo.ftf_list[prt_addr] != pfc->uu.ftf) {
		    prt_addr++;
		}
		p_ftf_info->i_pvdescr = p_pvdescr - p_cpl->p_from_apd_pv_descr;
		for (i = 0; i < p_ftf_info->n_vehicles; i++) {
		    ASSERT(prt_addr < 64);
		    p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)pfc +
					    sizeof(T_MDB_FROMVAR_HDR));
		    
		    p_pv->pv_name.ts_id       = PVN48GETTSID(p_from_vars->pv_name);
		    p_pv->pv_name.prt_addr    = prt_addr;
#ifdef APD_BUG
		    p_pv->pv_name.prt_addr <<= 2;
#endif
		    p_pv->pv_name.size        = (pdm_ftf_size == 1) ? LP_SIZE_8 : LP_SIZE_16;
		    p_pv->pv_name.bit_offset  = 0;
		    p_pv->pv_name.type        = LP_TYPE_CARD;
		    p_pv->pv_name.ctl_offset  = PDM_CHKVARVOID;
		    p_pv->p_variable          = 
			&p_cpl->p_from_ds[curr_port].dir.from.rcv_frame_type;
		    p_pv->p_validity          = NULL;
		    p_pv++;
		    for (j = 0; j < pfc->n_vars; j++) {
			set_pvdescr(p_cpl, p_pv, p_from_vars, &space_info);
			*p_fr = p_from_vars->freshness;
			p_pv->pv_name.prt_addr = prt_addr; 
#ifdef APD_BUG
			p_pv->pv_name.prt_addr <<= 2;
#endif
			p_from_vars++;
			p_pv++;
			p_fr++;
		    }
		    prt_addr++;
		    while (prt_addr < 64 && pdm_topo.ftf_list[prt_addr] != pfc->uu.ftf) {
			prt_addr++;
		    }
		    gen_pv_set(PDM_FROM, &p_cpl->p_from_ds[curr_port], 
			       p_pvdescr, &from_port_info[curr_port]);
		    p_cpl->p_from_ds[curr_port].p_pvdescr = p_pvdescr;
		    p_cpl->p_from_ds[curr_port].dir.from.p_var_freshness = p_fresh;
		    p_cpl->p_from_ds[curr_port].dir.from.frame_type = pfc->uu.ftf;
		    
		    p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)pfc +
						    sizeof(T_MDB_FROMVAR_HDR));
		    replace_nullptrs(p_from_vars, p_pvdescr + 1, pfc->n_vars);
		    
		    p_pvdescr = p_pv;
		    p_fresh = p_fr;

		    curr_port++;
		}
	    }
	    pfc = (T_MDB_FROMVAR_HDR *)(NEXTSUBLIST(pfc));
	}
    }
    else {
	/* lp access data structures for FROM variables (PDM_INTERN, PDM_EXPORT) */
	p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)p_from_cfg +
						sizeof(T_MDB_FROMVAR_HDR));
	curr_port = 0;
	lastdsname = PVN48GETDSNAME(p_from_vars->pv_name);
	for (i = 0; i < n_from_vars; i++) {
	    if ((newdsname = PVN48GETDSNAME(p_from_vars->pv_name)) != lastdsname) {
		lastdsname = newdsname;
		/* generate lp access structures for current dataset */
		gen_pv_set(PDM_FROM, &p_cpl->p_from_ds[curr_port], 
			   p_pvdescr, &from_port_info[curr_port]);
		p_cpl->p_from_ds[curr_port].p_pvdescr = p_pvdescr;
		p_cpl->p_from_ds[curr_port].dir.from.p_var_freshness = p_fresh;
		p_cpl->p_from_ds[curr_port].dir.from.frame_type = PDM_FTF_INVALID;
		p_cpl->p_from_ds[curr_port].dir.from.rcv_frame_type = PDM_FTF_INVALID;
		p_pv = (p_pvdescr += from_port_info[curr_port].vars);
		p_fr = (p_fresh += from_port_info[curr_port].vars);
		curr_port++;
		ASSERT(curr_port < n_from_ports);
	    }
	
	    set_pvdescr(p_cpl, p_pv, p_from_vars, &space_info);
	    *p_fr = p_from_vars->freshness;
	    p_from_vars++;
	    p_pv++;
	    p_fr++;
	}
	/* generate lp access structures for last dataset */
	gen_pv_set(PDM_FROM, &p_cpl->p_from_ds[curr_port], 
		   p_pvdescr, &from_port_info[curr_port]);
	p_cpl->p_from_ds[curr_port].p_pvdescr = p_pvdescr;
	p_cpl->p_from_ds[curr_port].dir.from.p_var_freshness = p_fresh;
	p_cpl->p_from_ds[curr_port].dir.from.frame_type = PDM_FTF_INVALID;
	p_cpl->p_from_ds[curr_port].dir.from.rcv_frame_type = PDM_FTF_INVALID;
	p_from_vars = (T_MDB_FROMVAR *)((UNSIGNED8 *)p_from_cfg +
					sizeof(T_MDB_FROMVAR_HDR));
	replace_nullptrs(p_from_vars, p_cpl->p_from_apd_pv_descr, n_from_vars);
    }

    if (marsh_type == PDM_IMPORT) {
	int i = 0;
    }
#ifdef DEBUG
    if (marsh_type == PDM_IMPORT) {
	dump_ftf_info(pdm_topo.ftf_info);
    }
    PDM_Dbg(("\ncomplete p_cpl->p_from_apd_pv_descr after chk ptr repair\n"));
    dump_pvdescr(p_cpl->p_from_apd_pv_descr, n_from_vars);
#endif



    /* build data structures for function arguments and results */
    /*----------------------------------------------------------*/
    if (n_func_args > 0) {
	int n = 0;
	int curr_func;
	UNSIGNED16 sizetype;
	UNSIGNED16 calc_type;
	T_PDM_FUNC_INFO *p_curr_fct;

	p_func = (T_MDB_FUNC_ARG *)(p_func_cfg + 1);
	p_arg = p_cpl->p_func_args;
	curr_func = 0;
	i = 0;
	while (i < n_func_args + n_funcs) {
	    if (p_func->uu.func.ref & PDM_FUNC) {
		func_ref2idx[p_func->uu.func.ref & PDM_REFMASK] = curr_func;
		calc_type = (p_func->uu.func.flags & PDM_REAL) 
		    ? PDM_CALC_R32
		    : (p_func->uu.func.flags & PDM_UNSIGNED) 
			? PDM_CALC_U32 
                        : PDM_CALC_I32;
		if (p_func->uu.func.flags & PDM_S_DIR) {
		    T_MDB_FUNC_ARG * p_func_a;
		    p_func_a = p_func + 1;
		    if ((p_ftf_info = search_ftf_info(p_func_a->uu.arg.ftf)) != NULL) {
			n = p_ftf_info->n_vehicles;
			p_pvdescr = &p_cpl->p_from_apd_pv_descr[p_ftf_info->i_pvdescr + 
							 p_func_a->uu.arg.ref + 1];
			for (j = 0; j < n; j++) {
			    p_curr_fct = &p_cpl->p_func_info[curr_func];
			    p_curr_fct->function = 
				pdm_func_table[p_func->uu.func.id][calc_type];
			    p_curr_fct->n_args = 1;
			    p_curr_fct->p_args = p_arg;
			    p_curr_fct->result.f_down_cast = (calc_type == PDM_CALC_R32) 
				? pdm_cast_R32_R32 : pdm_cast_I32_I32;
			    p_arg->p_var = p_pvdescr->p_variable;
			    p_arg->p_chk = p_pvdescr->p_validity;
			    sizetype = PDM_FOLD_SIZE_TYPE(p_pvdescr->pv_name.size,
							  p_pvdescr->pv_name.type);
			    p_arg->f_is_valid = set_f_is_valid(sizetype);
			    if (set_f_up_cast(&p_arg->f_up_cast, calc_type, sizetype) == FALSE) {
				p_arg->f_is_valid = pdm_never_valid;
			    }
			    p_arg->bit_pos = p_func_a->uu.arg.bit_pos;
			    if (p_func_a->uu.arg.flags & PDM_NEGATED) {
				p_arg->bit_pos |= PDM_CALC_NEGATED;
			    }
			    p_pvdescr += p_ftf_info->n_vars;
			    curr_func++;
			    p_arg++;
			}
		    }
		    i++;
		    p_func++; 		/* the argument is already processed */
		}
		else {
		    p_curr_fct = &p_cpl->p_func_info[curr_func];
		    curr_func++;
		    p_curr_fct->function = 
			pdm_func_table[p_func->uu.func.id][calc_type];
		    p_curr_fct->n_args = 0;
		    p_curr_fct->p_args = p_arg;
		}
		/* function result structure is set, when TO variables
		 * are analysed
		 */
	    }
	    else {
		UNSIGNED16 k, n, m;
		if (marsh_type == PDM_IMPORT) {
		    if ((p_ftf_info = search_ftf_info(p_func->uu.arg.ftf)) != NULL) {
			n = p_ftf_info->n_vehicles;
			m = p_ftf_info->n_vars;
			p_pvdescr = &p_cpl->p_from_apd_pv_descr[p_ftf_info->i_pvdescr + 
							 p_func->uu.arg.ref + 1];
		    }
		    else {
			n = 0;
		    }
		}
		else {
		    n = 1;
		    m = 0;
		    p_pvdescr = &p_cpl->p_from_apd_pv_descr[p_func->uu.arg.ref];
		}
		for (k = 0; k < n; k++) {
		    p_curr_fct->n_args++;
		    p_arg->p_var = p_pvdescr->p_variable;
		    p_arg->p_chk = p_pvdescr->p_validity;
		    sizetype = PDM_FOLD_SIZE_TYPE(p_pvdescr->pv_name.size,
						  p_pvdescr->pv_name.type);
		    p_arg->f_is_valid = set_f_is_valid(sizetype);
		    if (set_f_up_cast(&p_arg->f_up_cast, calc_type, sizetype) == FALSE) {
			p_arg->f_is_valid = pdm_never_valid;
		    }
		    p_arg->bit_pos = p_func->uu.arg.bit_pos;
		    if (p_func->uu.arg.flags & PDM_NEGATED) {
			p_arg->bit_pos |= PDM_CALC_NEGATED;
		    }
		    p_arg++;
		    p_pvdescr += m;
		}
	    }
	    p_func++;
	    i++;
	}
	p_cpl->n_funcs = curr_func;
    }
    else {
	p_cpl->n_funcs = 0;
    }

#ifdef DEBUG
    if (marsh_type == PDM_IMPORT) {
	dump_func_ref2idx(func_ref2idx, p_func_cfg->n_funcs);
    }
#endif

    /* generate lp access data structures for TO variables */
    /*-----------------------------------------------------*/
    p_to_vars = (T_MDB_TOVAR *)((UNSIGNED8 *)p_to_cfg +
					   sizeof(T_MDB_TOVAR_HDR));
    p_to_vars_im = (T_MDB_TOVAR_IM *)p_to_vars;
    p_pv = p_pvdescr = p_to_apd_pv_descr;
    curr_port = 0;
    lastdsname = PVN48GETDSNAME(p_to_vars->pv_name);

    for (i = 0; i < p_to_cfg->n_vars; i++) {
	TYPE_PV_NAME_48 pv_name = 
	    (marsh_type == PDM_IMPORT) 
		? p_to_vars_im->pv_name : p_to_vars->pv_name;

	if ((newdsname = PVN48GETDSNAME(pv_name)) != lastdsname) {
	    lastdsname = newdsname;
	    /* generate lp access structures for this dataset */
	    gen_pv_set(PDM_TO, &p_cpl->p_to_ds[curr_port], 
		       p_pvdescr, &to_port_info[curr_port]);
	    p_pv = (p_pvdescr += to_port_info[curr_port].vars);
	    curr_port++;
	    ASSERT(curr_port < n_to_ports);
	}
	
	if (marsh_type == PDM_IMPORT) {
	    flags = p_to_vars_im->ref & PDM_FLAGMASK;
	    ref   = p_to_vars_im->ref & PDM_REFMASK;
	}
	else {
	    flags = p_to_vars->ref & PDM_FLAGMASK;
	    ref   = p_to_vars->ref & PDM_REFMASK;
	}
	if (marsh_type != PDM_IMPORT || (p_to_vars_im->flags & PDM_NORM)) {
	    /* copy PV_Name */
	    copy_pvname(&p_pv->pv_name, pv_name);

	    /* get variable alignment, size and check variable offsets */
	    if ((flags & PDM_FUNC)) {
		T_PDM_FUNC_RES *res;
		UNSIGNED16 idx;
		UNSIGNED16 sizetype;
		
		idx = (marsh_type == PDM_IMPORT) ? func_ref2idx[ref] : ref;
		res = &p_cpl->p_func_info[idx].result;
		/* allocate space for function result */
		if (PVN48GETCHKOFF(pv_name) != PDM_CHKVARVOID) {
		    p_pv->p_validity = (UNSIGNED8 *)p_cpl->p_var_buf + space_info.chks_offset;
		    space_info.chks_offset++;
		}
		else {
		    p_pv->p_validity = NULL;
		}
		
		switch (pv_alignment(pv_name)) {
		  case 1:
		    p_pv->p_variable = (UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_1;
		    break;
		  case 2:
		    p_pv->p_variable = (UNSIGNED16 *)
			((UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_2);
		    break;
		  case 4:
		    p_pv->p_variable = (UNSIGNED32 *)
			((UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_4);
		    break;
		  default:
		    ASSERT(0);
		    break;
		}
		/* increment offset in variable buffer */
		add_space_for_pvname(pv_name, &space_info);
		/* now fill in the function result infos */
		res->p_var = p_pv->p_variable;
		res->p_chk = p_pv->p_validity;
		res->size  = p_pv->pv_name.size;
		res->type  = p_pv->pv_name.type;
		sizetype = PDM_FOLD_SIZE_TYPE(p_pv->pv_name.size,
					      p_pv->pv_name.type);
		res->f_invalidate = set_f_invalidate(sizetype);
		set_f_down_cast(&res->f_down_cast, sizetype);
	    }
	    else if (flags & PDM_DEFAULT_VALUE) {
		if (p_dflts != NULL) {
		    p_pv->p_variable = p_dflts + ref;
		    p_pv->p_validity = NULL;
		}
		else {
		    /* if no default value available, set it to invalid */
		    p_pv->p_variable = invalid_value;
		    p_pv->p_validity = &invalid_chk;
		}
	    }
	    else {
		TYPE_APD_PV_DESCRIPTOR *p_from_pvdescr;
		if (marsh_type == PDM_IMPORT) {
		    if ((p_ftf_info = search_ftf_info(p_to_vars_im->ftf)) != NULL) {
			p_from_pvdescr = &p_cpl->p_from_apd_pv_descr[p_ftf_info->i_pvdescr + ref + 1];
			p_pv->p_variable = p_from_pvdescr->p_variable;
			p_pv->p_validity = p_from_pvdescr->p_validity;
		    }
		    else {
			p_pv->p_variable = invalid_value;
			p_pv->p_validity = &invalid_chk;
		    }
		}
		else {
		    p_from_pvdescr = &p_cpl->p_from_apd_pv_descr[ref];
		    p_pv->p_variable = p_from_pvdescr->p_variable;
		    p_pv->p_validity = p_from_pvdescr->p_validity;
		}
	    }
	    p_pv++;
	}
	else {
	    /* Source Directory format */
	    int rest;
	    UNSIGNED16 parts;
	    UNSIGNED16 n_vehicles;
	    UNSIGNED16 n_srcdir;
	    UNSIGNED16 j;
	    UNSIGNED16 k;
	    UNSIGNED16 incr_offset;
	    UNSIGNED16 tcn = 0;
	    UNSIGNED16 ftf = p_to_vars_im->ftf;
	    UNSIGNED16 end;
	    UNSIGNED16 port = PVN48GETPORT(p_to_vars_im->pv_name);
	    UNSIGNED16 tsid = PVN48GETTSID(p_to_vars_im->pv_name);
	    UNSIGNED16 bit_offset;
	    TYPE_APD_PV_DESCRIPTOR *p_from_pvdescr;

	    /* first variable is always the source directory */
	    if ((p_ftf_info = search_ftf_info(ftf)) != NULL) {
		T_PDM_FUNC_INFO *p_func_info;

		n_vehicles = p_ftf_info->n_vehicles;
		incr_offset = bitsize(pv_name);
		if (PVN48GETCHKOFF(pv_name) != PDM_CHKVARVOID) {
		    parts = 32 / bytesize(pv_name);
		    n_srcdir = 0; /* only one source directory */
		}
		else {
		    parts = 24 / bytesize(pv_name);
		    n_srcdir = 1; /* source directory in each port */
		}
		if (n_srcdir == 0) {
		    /* source directory */
		    init_sdir(p_pv, tsid, port,
			      (UNSIGNED8 *)p_cpl->p_var_buf + srcdir_space,
			      ftf, &tcn, n_vehicles);
		    srcdir_space += PDM_S_DIR_LEN;
		    p_pv++;
		    
		    /* check var list */
		    if (flags & PDM_FUNC) {
			p_func_info = &p_cpl->p_func_info[func_ref2idx[ref]];
		    }
		    else {
			p_from_pvdescr = &p_cpl->p_from_apd_pv_descr[p_ftf_info->i_pvdescr + ref + 1];
		    }
		    for (k = 0; k < n_vehicles; k++) {
			p_pv->pv_name.ts_id       = tsid;
			p_pv->pv_name.prt_addr    = port;
			p_pv->pv_name.size        = LP_SIZE_BITS;
			p_pv->pv_name.bit_offset  = PDM_S_DIR_CHK_OFFSET + k * 2;
			p_pv->pv_name.type        = LP_TYPE_BOOL_2;
			p_pv->pv_name.ctl_offset  = PDM_CHKVARVOID; 
			if (flags & PDM_FUNC) {

			    p_pv->p_variable = 
				(UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_1;
			    space_info.size_1++;
			    p_func_info->result.p_chk = p_pv->p_variable;
			    p_func_info++;
			}
			else {
			    /* the check variable is treated as a normal 
			     * variable in the check var list 
			     */
			    p_pv->p_variable = p_from_pvdescr->p_validity;
			    p_from_pvdescr += p_ftf_info->n_vars;
			}
			p_pv->p_validity = NULL;
			p_pv++;
		    }
		    port++;

		    /* generate pv set */
		    gen_pv_set(PDM_TO, &p_cpl->p_to_ds[curr_port], 
			       p_pvdescr, &to_port_info[curr_port]);
		    p_pv = (p_pvdescr += to_port_info[curr_port].vars);
		    curr_port++;
		}

		rest = n_vehicles;
		j = 0;
		if (flags & PDM_FUNC) {
		    p_func_info = &p_cpl->p_func_info[func_ref2idx[ref]];
		}
		else {
		    p_from_pvdescr = &p_cpl->p_from_apd_pv_descr[p_ftf_info->i_pvdescr + ref + 1];
		}
		while (rest > 0) {
		    end = min(rest, parts);
		    if (n_srcdir != 0) {
			init_sdir(p_pv, tsid, port,
				  (UNSIGNED8 *)p_cpl->p_var_buf + srcdir_space,
				  ftf, &tcn, end);
			srcdir_space += PDM_S_DIR_LEN;
			p_pv++;
			bit_offset = PDM_S_DIR_VAR_OFFSET;
		    }
		    else {
			bit_offset = PDM_S_DIR_VAR_OFFSET_CHK;
		    }
		    for (k = 0; k < end; k++) {
			if (flags & PDM_FUNC) {
			    UNSIGNED16 sizetype;
			    /* now fill in the function result infos */
			    p_func_info->result.size  = PVN48GETSIZE(pv_name);
			    p_func_info->result.type  = PVN48GETTYPE(pv_name);
			    sizetype = PDM_FOLD_SIZE_TYPE(PVN48GETSIZE(pv_name),
							  PVN48GETTYPE(pv_name));
			    p_func_info->result.f_invalidate = set_f_invalidate(sizetype);
			    set_f_down_cast(&p_func_info->result.f_down_cast, sizetype);

			    switch (pv_alignment(pv_name)) {
			      case 1:
				p_func_info->result.p_var = 
				    (UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_1;
				break;
			      case 2:
				p_func_info->result.p_var = (UNSIGNED16 *)
				    ((UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_2);
				break;
			      case 4:
				p_func_info->result.p_var = (UNSIGNED32 *)
				    ((UNSIGNED8 *)p_cpl->p_var_buf + space_info.size_4);
				break;
			      default:
				ASSERT(0);
				break;
			    }
			    /* increment offset in variable buffer */
			    add_space_for_pvname(pv_name, &space_info);
			    p_pv->p_variable = p_func_info->result.p_var;
			    p_func_info++;
			}
			else {
			    p_pv->p_variable = p_from_pvdescr->p_variable;
			    p_from_pvdescr += p_ftf_info->n_vars;
			}				
			p_pv->p_validity = NULL;
			copy_pvname(&p_pv->pv_name, pv_name);
			p_pv->pv_name.bit_offset = bit_offset;
			p_pv->pv_name.prt_addr   = port;
			bit_offset += incr_offset;
			p_pv++;
		    }
		    if ((rest -= parts) > 0) {
			lastdsname = PVN48GETDSNAME(pv_name);
			/* generate lp access structures for this dataset */
			gen_pv_set(PDM_TO, &p_cpl->p_to_ds[curr_port], 
				   p_pvdescr, &to_port_info[curr_port]);
			p_pv = (p_pvdescr += to_port_info[curr_port].vars);
			curr_port++;
			ASSERT(curr_port < n_to_ports);
		    }
		    port++;
		}
	    }
	    else {
		UNSIGNED16 dummy;
		/* only the source directory (no bits set) and nothing else */
		init_sdir(p_pv, tsid, port, empty_srcdir, 0, &dummy, 0);
		p_pv++;
	    }
	}

	if (marsh_type == PDM_IMPORT) {
	    p_to_vars_im++;
	}
	else {
	    p_to_vars++;
	}
    }
    /* generate lp access structures for last dataset */
    gen_pv_set(PDM_TO, &p_cpl->p_to_ds[curr_port], 
	       p_pvdescr, &to_port_info[curr_port]);
    curr_port++;

    p_cpl->n_to_ports = curr_port;

	/* <ATR:01> remove the unused but allocated TO storage space */
	for (i = curr_port; i < n_to_ports; i++) pdm_free(p_cpl->p_to_ds[i].p_ds);


#ifdef DEBUG
    /* now is the function result info inserted */
    dump_func_info(p_cpl->p_func_info, n_func_infos);
#endif

    /* clean up */
    /*----------*/
    if (func_ref2idx) pdm_free(func_ref2idx);
    pdm_free(from_port_info);
    pdm_free(p_to_apd_pv_descr);

    p_cpl->status = PDM_ENABLED;
    pi_enable_timeout(p_cpl->timeout_id, ((p_cpl->cycle + RMK_TICK/2U)/RMK_TICK));

    if (p_cpl->shadow_buf) {
	memset(p_cpl->shadow_buf, PDM_INVALID_CARD_8, 
	       PDM_SHADOW_BUF_LEN * sizeof(p_cpl->shadow_buf[0]));
    }


    return PDM_OK;


    /*----------------------- ERROR --------------------------------*/

  NOMEM:

    RECORD("No memory");
    if (func_ref2idx) pdm_free(func_ref2idx);
    if (from_port_info) pdm_free(from_port_info);
    if (p_to_apd_pv_descr) pdm_free(p_to_apd_pv_descr);
    pdm_free_cpl(p_cpl);
    result = PDM_ERR_NO_MEM;

  BUILDERR:
    p_cpl->status = PDM_DISABLED;
    p_cpl->result = result;
    return result;
}

