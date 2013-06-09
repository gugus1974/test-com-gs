/* LPC_BLCK.H
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Link Process                                            
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     File Name    :   LPC_BLCK.H 		(Control Block)               
                                                                              
     Document     :   see source file                                         
                                                                              
     Abstract     :         "                                                 
                                                                              
     Remarks      :         "                                                 
                                                                              
     Dependencies :         "                                                 
                                                                              
     Accepted     :         "                                                 
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     HISTORY:                                                                 
                                                                              
     Version    YY-MM-DD     Name           Dept.      Reference     Status   
     -------    --------     ----------     ------     ---------     -------  
       0.1      91-08-23     W. Marsden     BAEI-2         --        created  
                                                                              
 ---------------------------------------------------------------------------- 
*/

#ifndef   LPC_BLCK_H
#define   LPC_BLCK_H


#ifndef        LP_SYS_H
    #error: LP_SYS.H must be included prior to LP_CB.H
#endif


/*
 --------------------------------------------------------------------------- 
| 
| constant      LPC_SIZE
| 
 ------------   A B S T R A C T   ------------------------------------------ 
  
  
  This constant is used to define the maximum number of traffic stores which 
  are supported by link layer process. The traffic stores are identified by the 
  variable "ts_id".

*/

#define		LPC_SIZE	(LP_TS_ID_MAX + 1)

/* new page
 --------------------------------------------------------------------------- 
|
| constant      LP_...
|
 ------------   A B S T R A C T   ------------------------------------------ 


		LPC_FREE		CB Index (TS_ID) is free for use
		LPC_IN_USE              CB Index (TS_ID) in use
		LPC_ERROR               CB Index (TS_ID) in use, an eror
					was encountered during configuration
*/

#define		LPC_IRPT_FREE		0
#define		LPC_IRPT_IN_USE		1

#define		LPC_TS_FREE		0
#define		LPC_TS_IN_USE		1
#define		LPC_TS_ERROR		2

#define		LPC_STATE_MAX		3

#define		LPC_NSWP		LP_TS_TYPE_C

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_chk_prt_addr
|
| param. in     p_cb            	handle supplied by lpc_ts_access
|               prt_addr		port address
|
|        out    -
|
| return value  status			LPS_OK
|               			LPS_RANGE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function compares the port address with the highest supported port
  address to determine the range state.

*/

#define		lpc_chk_prt_addr(p_cb, prt_addr)  \
		((prt_addr <= ((TYPE_LP_CB *) p_cb)->prt_addr_max) ? LPS_OK : LPS_RANGE)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_chk_prt_addr                                           */


/* new page
 ----------------------------------------------------------------------------- 
| 
| macrofunction lpc_chk_prt_indx
|
| param. in     p_cb            	handle supplied by lpc_ts_access
|               prt_indx		port index
|
|        out    -
|
| return value  status			LPS_OK
|               			LPS_RANGE
|
 ------------   A B S T R A C T   -------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function compares the port index with the highest permitted port index
  to determine the range state.

*/

#define		lpc_chk_prt_indx(p_cb, prt_indx)  \
    ((prt_indx <= ((TYPE_LP_CB *) p_cb)->prt_indx_max) ? LPS_OK : LPS_RANGE)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_chk_prt_indx					  */


/* new page
 ----------------------------------------------------------------------------
|
| macrofunction lpc_get_ts_id
|
| param. in     p_cb			handle provided by lpc_ts_access
|
| return value  ts_id
|                                       LP_HW_TYPE_WTB
 ------------   A B S T R A C T   -------------------------------------------

  Reads the traffic store id belonging to the p_cb handle

*/

#define		lpc_get_ts_id(p_cb) \
		    ((((TYPE_LP_CB *) p_cb)->ts_id))


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.5  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_hw_type                                           */


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_hw_type
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  hw_type                 LP_HW_TYPE_BAP_1
|                                       LP_HW_TYPE_WTB
|					LP_HW_TYPE_MIC_1
|					LP_HW_TYPE_MIC_2
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the hardware_type associated with the target traffic
  store.

*/

#define		lpc_get_hw_type(p_cb) \
		    ((unsigned short) (((TYPE_LP_CB *) p_cb)->hw_type))


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.5  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_hw_type                                           */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pb_pit
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -                       pointer to base of port comm & status
|
| return value  pb_pit
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the base pointer to the port index table.

  This function will generally be used by the "put" and "get" functions.

*/

#define		lpc_get_pb_pit(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_pit)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pb_pit                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pb_pcs
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -                       pointer to base of port comm & status
|
| return value  pb_pcs
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the base pointer to the port communication and status
  registers.

  This function will generally be used by the "put" and "get" functions.

*/

#define		lpc_get_pb_pcs(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_pcs)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pb_pcs                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pb_prt
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  pb_prt
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the pointer to the data buffer base.

  This function will generally be used by the "put" and "get" functions.

*/

#define		lpc_get_pb_prt(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_prt)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-27  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pb_prt                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pb_frc
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out
|
| return value  pb_frc
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the pointer to the force table. This function will
  generally be used by the "put" and "get" as well as by the "force" and 
  "unforce" functions.

*/

#define		lpc_get_pb_frc(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_frc)

#define		lpc_get_pb_frc_shadow(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_frc_shadow)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0    BAEI-2  91-09-27  created  LPDBR1.TXT  1.3  Marsden      --
|  1    BATC    95-12-06  enhancd                   Marsden  new _shadow
 ----------------------------------------------------------------------------
End of function lpc_get_pb_frc                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pb_mwrd
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out
|
| return value  pb_frc
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the pointer to the magic word. It is only used by
  mailbox slaves in the cluster function.

*/

#define		lpc_get_pb_mwd(p_cb) \
			(((TYPE_LP_CB *) p_cb)->pb_mwd)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-10-30  created                   Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pb_mwd   */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pit_type
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  pit_type
|
 ------------   A B S T R A C T   -------------------------------------------


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the pit type (LP_PIT_TYPE_A, _B) for the port index
  table of the target traffic store.

  !!! Attention: relies on odd constants for 8 bit and even constants for
  16 bit pit types!

*/


#define		lpc_get_pit_type(p_cb) 	\
			(lpc_get_ts_type(p_cb) & 1)


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pit_type                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pcs_type
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  pcs_type
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the pcs type (LP_PCS_TYPE_A,B,C) for the pcs array
  of the target traffic store.

*/

#define		lpc_get_pcs_type(p_cb) \
			(lpc_get_ts_type (p_cb))

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_pcs_type                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_prt_type
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  prt_type
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the prt type (only one type defined) for the port data
  array of the target traffic store.

*/

#define		lpc_get_prt_type(p_cb) 	LP_PRT_TYPE_A


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_prt_type                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_ts_owner
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  ts_owner                LP_CFG_TS_OWNED / LP_TS_CFG_ALIEN
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the traffic store owner. The caller must obey the
  magic word mechanism for traffic stores owned by alien devices

*/

#define		lpc_get_ts_owner(p_cb)         \
			(((TYPE_LP_CB *) p_cb)->ts_owner)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_ts_type                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_ts_type
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  prt_type
|
 ------------   A B S T R A C T   -------------------------------------------


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the traffic store type (A, B, C)

*/

#define		lpc_get_ts_type(p_cb)         \
			(((TYPE_LP_CB *) p_cb)->ts_type)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ----------------------------------------------------------------------------
End of function lpc_get_ts_type                                          */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_pcs_handle
|
| param. in     p_cb			handle supplied by lpc_ts_access
|        out    -
|
| return value  the size of the pcs in powers of 2
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function returns the size of the pcs in powers of 2. The caller is
  lpl_gen_pcs_offset.

  This value is used in lpl_gen_pcs_p to generate two separate pcs pointers
  for use in dp ram applications. 

*/

#define		lpc_get_pcs_size_in_pwr_2(p_cb) 	\
			(((TYPE_LP_CB *) p_cb)->pcs_power_of_2)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_power_of_2                                        */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_prt_addr_max
|
| param. in     p_cb			handle supplied by lpc_ts_access
|
|        out    -
|
| return value  prt_addr_max
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The highest supported port address within the traffic store is returned.

*/

#define		lpc_get_prt_addr_max(p_cb) \
			(((TYPE_LP_CB *) p_cb)->prt_addr_max)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_prt_addr_max                                      */


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_prt_indx_max
|
| param. in     p_cb			handle supplied by lpc_ts_access
|
|        out    -
|
| return value  prt_indx_max
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The highest supported port (dock) index within the traffic store is returned.

*/

#define		lpc_get_prt_indx_max(p_cb) \
			(((TYPE_LP_CB *) p_cb)->prt_indx_max)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_prt_indx_max                                      */


/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_get_state
|
| param. in     p_cb			handle supplied by lpc_ts_access
|
|        out    -
|
| return value  state			LPC_FREE
|					LPC_IN_USE
|                                       LPC_ERROR
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The state of the traffic store initialisation is returned.
  
*/

#define		lpc_get_state(p_cb) \
			(((TYPE_LP_CB *) p_cb)->state)

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.4  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_get_state                                             */

/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_put_hw_type
|
| param. in     p_cb			handle provided by lpc_ts_access
|               hw_type	
|               
|        out    -
|               
| return value  result			LPS_OK
|					LPS_RANGE
|		
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The hardware type for the target traffic store is stored in the control
  block

*/

#define		lpc_put_hw_type(p_cb, my_hw_type) 		\
			{ ((TYPE_LP_CB *) p_cb)->hw_type  = my_hw_type; }


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created  LPDBR1.TXT  1.5  Marsden      --
 ----------------------------------------------------------------------------
End of function lpc_put_hw_type                                           */


/* new page
 ----------------------------------------------------------------------------
|
| function      lpc_put_p_ts_cfg
|
 ------------   A B S T R A C T   -------------------------------------------


   The pointer to the traffic store configuration is stored/retrieved for
   future reference.

*/

#define     lpc_put_p_ts_cfg(p_cb, par_p_ts_cfg)        \
			{ ((TYPE_LP_CB *) p_cb)->p_ts_cfg = par_p_ts_cfg; }

#define     lpc_get_p_ts_cfg(p_cb, par_pp_ts_cfg)        \
			{ *par_pp_ts_cfg = ((TYPE_LP_CB *) (p_cb))->p_ts_cfg; }



/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_put_pb_pit	put base pointer port index table
|
| param. in     p_cb			handle provided by lpc_ts_access
|               pb_pit			base pointer port index table
|               pit_type		LP_PIT_8, LP_PIT_16
|               mem_type                LPC_STATIC, LPC_DYNAMIC
|
|        out    -
|               
| return value  result			LPS_OK
|					LPS_RANGE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The base pointer to a traffic store resource is saved together with the
  information, whether the resource was allocated by the memory utility or
  statically configured.

  The type of port index table (8 or 16 bit port address) is saved.

*/

#define lpc_put_pb_pit(p_cb, p_pit) \
	{ ((struct STR_LP_CB *) p_cb)->pb_pit = p_pit; }

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_pb_pit  */


/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpc_put_pb_pcs	put base pointer port commu & status
|
| param. in     p_cb			handle provided by lpc_ts_access
|               pb_pcs			base pointer port commun & status reg
|               mem_type                LPC_STATIC, LPC_DYNAMIC
|
|        out    -
|               
| return value  result			LPS_OK
|					LPS_RANGE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The base pointer to a traffic store resource is saved together with the
  information, whether the resource was allocated by the memory utility or
  statically configured.

*/

#define lpc_put_pb_pcs(p_cb, p_pcs) \
	{ ((struct STR_LP_CB *) p_cb)->pb_pcs = p_pcs; }

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_pb_pcs                                            */


/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpc_put_pb_prt	put base pointer port data buffer
|
| param. in     p_cb			handle provided by lpc_ts_access
|               pb_prt			base pointer
|
|        out    -
|               
| return value  result			LPS_OK
|					LPS_RANGE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The base pointer to a traffic store resource is saved together with the
  information, whether the resource was allocated by the memory utility or
  statically configured.

*/

#define lpc_put_pb_prt(p_cb, p_prt) \
	{ ((struct STR_LP_CB *) p_cb)->pb_prt = p_prt; }

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_pb_prt                                            */

/* new page
 ---------------------------------------------------------------------------- 
| 
| function      lpc_put_pb_frc	put base pointer force table
|
| param. in     p_cb			handle provided by lpc_ts_access
|               pb_frc			base pointer
|               mem_type                LPC_STATIC, LPC_DYNAMIC
|
|        out    -
|               
| return value  result			LPS_OK
|					LPS_RANGE
|               
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The base pointer to a traffic store resource is saved together with the
  information, whether the resource was allocated by the memory utility or
  statically configured.

*/

#define lpc_put_pb_frc(p_cb, p_frc) \
	{ ((struct STR_LP_CB *) p_cb)->pb_frc = p_frc; }

#define lpc_put_pb_frc_shadow(p_cb, p_frc) \
	{ ((struct STR_LP_CB *) p_cb)->pb_frc_shadow = p_frc; }

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.3  Marsden      --
|  1    BATC    95-12-06  enhancd                   Marsden  new _shadow
 ----------------------------------------------------------------------------
End of function lpc_put_pb_frc                                            */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_put_pb_mwd
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out
|
| return value  pb_frc
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function saves the pointer to the magic word.

*/

#define		lpc_put_pb_mwd(p_cb, p_mwd) \
		    { ((struct STR_LP_CB *) p_cb)->pb_mwd = p_mwd; }


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-10-30  created                   Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_pb_mwd   */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_put_ts_owner
|
| param. in     p_cb			handle supplied by lpc_ts_access
|		swap
|        out    -			LPC_NSWP / LPC_YSWP
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  The function sets the ownership (own or alien traffic store). An access to
  an alien traffic store is regulated by the magic word mechanism.

*/

#define		lpc_put_ts_owner(p_cb, value) 		\
			{ ((TYPE_LP_CB *) p_cb)->ts_owner = value; }

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  92-08-10  created              1.3  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_put_ts_owner */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_sink_in_use
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port receive update. This procedure tells the control block, that
  the procedure has been allocated for the target traffic store.

*/

#define		lpc_irpt_sink_in_use(p_cb) \
			{lpc_irpt_sink_modifd (p_cb, LPC_IRPT_IN_USE);}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_sink_in_use				            */

/* new page
 ----------------------------------------------------------------------------
|
| macrofunction lpc_irpt_sink_free
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port receive update. This procedure tells the control block, that
  the procedure has been removed (desubscribed) for the target traffic store.

*/

#define		lpc_irpt_sink_free(p_cb) \
			{lpc_irpt_sink_modifd (p_cb, LPC_IRPT_FREE);}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_sink_free          				    */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_sink_query
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  result			LPC_IRPT_FREE
|					LPC_IRPT_IN_USE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port receive update. This procedure queries the control block,
  whether a procedure has already been subscribed for the target traffic store.

*/

#define		lpc_irpt_sink_query(p_cb) \
		    (((TYPE_LP_CB *) p_cb)->irpt_sink)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_sink_query                                         */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_sink_modifd
|
| param. in     p_cb			handle provided by lpc_ts_access
|               value
|        out    -
|
| return value  result			LPC_IRPT_FREE
|					LPC_IRPT_IN_USE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port receive update. This procedure announces to the control block,
  whether a procedure has already been subscribed for the target traffic store.

*/

#define		lpc_irpt_sink_modifd(p_cb, value) \
		    {((TYPE_LP_CB *) p_cb)->irpt_sink = value;}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_sink_query                                         */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_srce_in_use
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port update. This procedure tells the control block, that the
  procedure has been allocated for the target traffic store.

*/

#define		lpc_irpt_srce_in_use(p_cb) \
			{lpc_irpt_srce_modifd (p_cb, LPC_IRPT_IN_USE);}

/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_srce_in_use				            */
/* new page
 ----------------------------------------------------------------------------
|
| macrofunction lpc_irpt_srce_free
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  -
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port send update. This procedure tells the control block, that
  the procedure has been removed (desubscribed) for the target traffic store.

*/

#define		lpc_irpt_srce_free(p_cb) \
			{lpc_irpt_srce_modifd (p_cb, LPC_IRPT_FREE);}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_srce_free   					    */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_srce_query
|
| param. in     p_cb			handle provided by lpc_ts_access
|
|        out    -
|
| return value  result			LPC_IRPT_FREE
|					LPC_IRPT_IN_USE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port send update. This procedure queries the control block,
  whether a procedure has already been subscribed for the target traffic store.

*/

#define		lpc_irpt_srce_query(p_cb) \
		    (((TYPE_LP_CB *) p_cb)->irpt_srce)


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_srce_query                                         */

/* new page
 ---------------------------------------------------------------------------- 
|
| macrofunction lpc_irpt_srce_modifd
|
| param. in     p_cb			handle provided by lpc_ts_access
|               value
|        out    -
|
| return value  result			LPC_IRPT_FREE
|					LPC_IRPT_IN_USE
|
 ------------   A B S T R A C T   ------------------------------------------- 


  The parameter (handle) p_cb is provided by lpc_ts_access (ts_id) to
  identify the target traffic store.

  A user procedure may be subscribed for each traffic store which is called
  upon a port receive update. This procedure announces to the control block,
  whether a procedure has already been subscribed for the target traffic store.

*/

#define		lpc_irpt_srce_modifd(p_cb, value) \
		    {((TYPE_LP_CB *) p_cb)->irpt_srce = value;}


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-25  created  LPDBR1.TXT  1.2  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_irpt_srce_modifd                                      */



/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_recycle
|
| (in )         p_cb
|
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  pieces of memory for the traffic store may have been allocated from the
  pool. This memory is returned.

*/

#define	lpc_recycle(p_cb) \
	lpx_recycle (&(((struct STR_LP_CB *) p_cb)->p_root));


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_remember */


/* new page
 ---------------------------------------------------------------------------- 
|
| function      lpc_remember
|
| (in )         p_cb
| (in )         p_alcd
|
| return value  -
| 
 ------------   A B S T R A C T   ------------------------------------------- 


  A piece of memory for the traffic store was allocated from the pool. This
  memory is remembered, so that it can be returned later.

*/

#define	lpc_remember(p_cb, p_alcd) \
	lpx_remember (&(((struct STR_LP_CB *) p_cb)->p_root), p_alcd);


/*
 ---------------------------------------------------------------------------- 
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-08-20  created  LPDBR1.TXT  1.1  Marsden      --
 ---------------------------------------------------------------------------- 
End of function lpc_remember */


/* new page
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_access
|
 ----------------------------------------------------------------------------- 
*/

void *		lpc_turbo_open		(unsigned short	ts_id);

short    	lpc_open    		(unsigned int   ts_id,
		         			     void * * 	    p_cb);


/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_init
|
 ----------------------------------------------------------------------------- 
*/

void            lpc_init          	(void);

/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_clear_ts
|
 -----------------------------------------------------------------------------
*/


void            lpc_clear_ts		(void * 	p_cb);

/*
 -----------------------------------------------------------------------------
| 
| prototype     lpc_frc_flag_clr
|
 -----------------------------------------------------------------------------
*/

void        	lpc_frc_flag_clr	(void * 	p_cb);


/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_frc_flag_set
|
 ----------------------------------------------------------------------------- 
*/

void        	lpc_frc_flag_set	(void * 	p_cb);


/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_frc_flag_sum in lpindt.h
|
 ----------------------------------------------------------------------------- 
*/

#define		lpc_frc_flag_sum()	_lpc_frc_flag_sum ()


/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_kill_ts
|
 ----------------------------------------------------------------------------- 
*/

void            lpc_kill_ts		(void * 	    p_cb);

/*
 -----------------------------------------------------------------------------
|
| prototype     lpc_put_state
|
 ----------------------------------------------------------------------------- 
*/

void		lpc_put_state		(void         *  p_cb,
               					 short   		 state);



/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lpc_put_max
|
 -----------------------------------------------------------------------------
*/

void 		lpc_put_max      	(void *	     	p_cb,
					             unsigned short	prt_addr_max,
					             unsigned short prt_indx_max);

/*
 -----------------------------------------------------------------------------
|
| prototype     lpc_put_ts_type
|
 -----------------------------------------------------------------------------
*/

short         lpc_put_ts_type	    (void *		    p_cb,
                 				 unsigned short	ts_type);


#endif
