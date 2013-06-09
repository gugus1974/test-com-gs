/*************************************************************************
**        Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       copy routines for Process Data Marshalling 

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
                   24.08.93  Ar     Adaption to changed interface to config.
                   04.10.93  Ar     Error field in DS set to the correct
                                    value
                   20.10.93  Ar     see below
                   18.11.93  Kra    mem.h replaced by stddef.h

      $Log: PDM_CP.C,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

*************************************************************************/

#define PDM_MODULE	/* access to PDM internal definitions */
#define PDM_CP_C_	/* braindead IC960 compiler           */

#ifdef O_960
#pragma noalign
#endif

#include <stddef.h>
#include <string.h>
#include "apd_incl.h"
#include "pdmdebug.h"
#include "pdmtypes.h"
#include "pdm_cp.h"
#include "pdm_main.h"


#define IS_FTF_OK(p_from_ds)  ((p_from_ds)->dir.from.frame_type == (p_from_ds)->dir.from.rcv_frame_type)

union U_pdm_invalid_float pdm_invalid_float = {0xffffffffL};


/******************************************************************************
*
*   name:          pdm_set_var_invalid
*
*   function:      sets a variable to invalid
*
******************************************************************************/
void pdm_set_var_invalid_2(void *p_var, void *p_chk, 
			   UNSIGNED8 type, UNSIGNED8 size)
{
    if (p_chk != NULL) {
	*(UNSIGNED8 *)p_chk = PDM_CHK_INVALID;
    }
    else {
	switch (type) {
	  case LP_TYPE_A8_ODD :
	    size = (size * 2) + 1;
	    break;

	  case LP_TYPE_A8_EVEN:
	  case LP_TYPE_A16_CARD:
	  case LP_TYPE_A16_SIGN:
	  case LP_TYPE_A32_CARD:
	  case LP_TYPE_A32_SIGN:
	    size = (size * 2) + 2;
	    break;
	    
	  default:
	    size *= 2;
	    if (size == 0) {
		size = 1;
	    }
	    break;
	}
	switch (size) {
	  case 1:
	    *(UNSIGNED8 *)(p_var) = 0xff;
	    break;
	  case 2:
	    *(UNSIGNED16 *)(p_var) = 0xffff;
	    break;
	  case 4:
	    *(UNSIGNED32 *)(p_var) = 0xffffffff;
	    break;
	  default:
	    memset((UNSIGNED8 *)p_var, 0xff, size);
	    break;
	}
    }
}

void pdm_set_var_invalid(TYPE_APD_PV_DESCRIPTOR *p_pvdescr)
{
    pdm_set_var_invalid_2(p_pvdescr->p_variable,
			  p_pvdescr->p_validity,
			  p_pvdescr->pv_name.type,
			  p_pvdescr->pv_name.size);
}

/*
 *   The function set_ds_invalid    sets a complete DS to invalid
 *
 *   arguments:        p_ds: pointer to the DS
 */
STATIC void 
set_ds_invalid(T_PDM_DATASET * p_ds)
{
    int             i;
    TYPE_APD_PV_DESCRIPTOR *  p_pvdescr;
    
    p_pvdescr = p_ds->p_pvdescr;
    
    /* set all variables within a DS to invalid */
    for (i = p_ds->vars; i > 0; i--) {
	pdm_set_var_invalid(p_pvdescr);
	p_pvdescr++;
    }
}


/*
 *   The function check_freshness checks the freshness of all variables 
 *   within a DS. A variable with expired freshness is set to invalid.
 *
 *   arguments:    p_from_ds: pointer to the DS
 */
STATIC void 
check_freshness(T_PDM_DATASET * p_from_ds)
{
    unsigned short ds_freshness;
    int i;
    unsigned short * p_var_freshness;
    TYPE_APD_PV_DESCRIPTOR *p_pvdescr;
    
    /* each variable has an individual freshnes assigned, compare
     * this value to the freshness of the dataset.
     */
    ds_freshness = p_from_ds->dir.from.ds_freshness;
    p_pvdescr = p_from_ds->p_pvdescr;
    p_var_freshness = p_from_ds->dir.from.p_var_freshness;
    
    for (i = p_from_ds->vars; i > 0; i--, p_pvdescr++, p_var_freshness++) {
	if (ds_freshness > *p_var_freshness) {
	    pdm_set_var_invalid(p_pvdescr);
	}
    }
}



/*
 *   The function set_all_ds_of_ts_invalid sets all data sets in a traffic
 *   store to invalid, if one data set of the traffic store is invalid. This 
 *   is because it is assumed that if one DS is invalid because of an invalid
 *   frame type field all other DS are also invalid.
 *
 *   arguments:         p_copy_list: pointer to a complete copy list
 */
STATIC void 
set_all_ds_of_ts_invalid(T_PDM_COPY_LIST * p_copy_list)
{
    int ds_num;
    int i, j;
    unsigned short ts_id;
    T_PDM_DATASET * p_from_ds;
    T_PDM_DATASET * p_temp_from_ds;

    ds_num = p_copy_list->n_from_ports;
    p_from_ds = p_copy_list->p_from_ds;

    for (i = 0; i < ds_num; i++, p_from_ds++) {
	if (p_from_ds->error == PDM_ERR_INVALID_FTF) {
	    /* we found a ds with an invalid frame type field, now search for
	     * all datasets with the same traffic store id and set this datasets
	     * also to invalid.
	     */
	    ts_id = p_from_ds->p_ds->ts_id;
	    p_temp_from_ds = p_copy_list->p_from_ds;
	    for (j = 0; j < ds_num; j++, p_temp_from_ds++) {
		if ((p_temp_from_ds->p_ds->ts_id == ts_id)  &&
		    (p_temp_from_ds->error == LPS_OK) ) {
		    p_temp_from_ds->dir.from.rcv_frame_type = PDM_FTF_INVALID;
		}
	    }
	}
    }
    /* now all datasets which shall be set to invalid are marked set them now
     * to invalid.
     */
    p_from_ds = p_copy_list->p_from_ds;
    for (i = 0; i < ds_num; i++, p_from_ds++) {
	if ((p_from_ds->dir.from.frame_type != PDM_FTF_INVALID)        &&
	    (p_from_ds->dir.from.frame_type != p_from_ds->dir.from.rcv_frame_type) &&
	    (p_from_ds->error == LPS_OK)                          ) {
	    set_ds_invalid(p_from_ds);
	}
    }
}



/*
 *   The function pdm_copy  executes the copying between the different TS. The type
 *   of copying is described in the copy list
 *
 *   arguments:     p_copy_list: list describing what has to be copied.
 *
 *   returns:       error code of the last error which happend. For a detailed
 *                  error report you have to look into the  error values
 *                  contained within the copy list.
 */
int 
pdm_copy(T_PDM_COPY_LIST * p_copy_list)
{
    int i;
    int error;
    int ds_error;
    BOOLEAN ftf_ok;
    T_PDM_DATASET    *p_from_ds;
    T_PDM_DATASET    *p_to_ds;
    T_PDM_FUNC_INFO  *p_func;
    
    if (p_copy_list->status != PDM_ENABLED) {
	return PDM_OK;
    }
    
    /* get all required data sets */
    p_from_ds = p_copy_list->p_from_ds;
    ftf_ok = TRUE;
    error = LPS_OK;

    for (i = p_copy_list->n_from_ports; i > 0; i--) {
	/* the following cast is dangerous, but the incompatible
	 * interfaces of RTP and APD force this (see apd_layr.c).
	 */
	ds_error = lp_get_data_set((struct STR_LP_DS_SET *)p_from_ds->p_ds);
	p_from_ds->error = ds_error;
	if (ds_error != LPS_OK) {
	    set_ds_invalid(p_from_ds);
	}
	else if (IS_FTF_OK(p_from_ds)) {
	    check_freshness(p_from_ds);
	}
	else {
	    ftf_ok = FALSE;
	    ds_error = PDM_ERR_INVALID_FTF;
	    set_ds_invalid(p_from_ds);
	}

	if ((ds_error != LPS_OK) && (error != PDM_ERR_INVALID_FTF)  )  {
	    /* if error is already set to PDM_INVALID_FTF, leave it at this
	     * value
	     */
	    error = ds_error;
	}
	p_from_ds++;
    }
    if (!ftf_ok) {
	set_all_ds_of_ts_invalid(p_copy_list);
    }

    /* apply the function to the from DS, the functions are even called
     * if the input is invalid. This is neccessary to get the correct
     * value for 'invalid'.
     */
    p_func = p_copy_list->p_func_info;
    for (i = p_copy_list->n_funcs; i > 0; i--) {
	(p_func->function ? p_func->function : pdm_void_func)
	    (p_func->n_args, p_func->p_args, &p_func->result);
	p_func++;
    }

    /* all complete for putting the DS into the target DS */
    p_to_ds = p_copy_list->p_to_ds;
    for (i = p_copy_list->n_to_ports; i > 0; i--) {
	if (p_copy_list->shadow_buf != NULL) {
	    /* NOTE: use of an internal RTP interface! */
	    lp_put_pv_x(p_copy_list->shadow_buf,
			(struct STR_LP_PV_X *)p_to_ds->p_ds->p_pv_list,
			p_to_ds->vars);  /* this is used for export only  */
	}
	/* about the dangerous cast, see comment above */
	ds_error = lp_put_data_set((struct STR_LP_DS_SET *)p_to_ds->p_ds);
	p_to_ds->error = ds_error;
	if ((ds_error != LPS_OK)  && (error != PDM_ERR_INVALID_FTF)) {
	    error = ds_error;
	}

#if 0
	if ((ds_error == LPS_OK)  && (p_to_ds->dir.to.n_defaults != 0)) {
	    /* The default values are located at the end of the list. Reduction
	     * of the number of variables within the dataset causes the default
	     * values to be set only once.
	     */
	    if (p_copy_list->shadow_buf == NULL) {
		p_to_ds->vars -= p_to_ds->dir.to.n_defaults;
	    }
	    else {
		p_to_ds->vars -= p_to_ds->dir.to.n_defaults;
	    }

	    p_to_ds->dir.to.n_defaults = 0;
	}
#endif
	p_to_ds++;
    }
    return(error);
}
