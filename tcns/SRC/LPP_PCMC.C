/*   LPP_PCMC.C
 ---------------------------------------------------------------------------- 
                                                                              
     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         

 ---------------------------------------------------------------------------- 
                                                                              
     Project      :   MicOS                                                   
     Subproject   :   Process Data                                            
                                                                              
 ---------------------------------------------------------------------------- 

     File         :   LPP_PCMC.C   -   Link Process Lowest Level of Access    
                                                                              
     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             
                      [2]: Link Layer Process Specification                   
                      [3]: Link Layer Process Design                          

     Abstract     :   Traffic Store Access Routines

     Remarks      :                                                           
                                                                              
     Dependencies :                                                           

     Accepted     :                                                           
                                                                              
 ---------------------------------------------------------------------------- 
                                                                              
     HISTORY      :   See end of file

 ----------------------------------------------------------------------------

*/

#define  LPI_MODULE_NAME "lpp_pcmc"

#ifdef   O_PCMCIA

#include "lp_sys.h"             /*   LP user interface                      */

#include "lpi_ifce.h"           /*   LP import interface                    */
#include "lpp_pcmc.h"
#include "lpc_blck.h"

#include "pcnode.h"



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          pcmcia_lpi_put_ts_short
³               pcmcia_lpi_get_ts_short
³               pcmcia_lpi_put_ts_array
³               pcmcia_lpi_get_ts_array
³
³
³ return value  PCMCIA service error code
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  This routines writes and reads the TS.
  PCMCIA Node implementation.

*/


  void lpp_put_ts_short   (void *    p_cb,
                           void *    p_ts,
                           void *    p_ap)
  {
  int rs;

      rs = P_WriteNum16TS (lpc_get_ts_id (p_cb),
                           (int*) p_ap, (unsigned long) p_ts, 1);

      lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "put_ts_short");
  }


  void lpp_get_ts_short    (void *    p_cb,
                            void *    p_ap,
                            void *    p_ts)
  {
  unsigned int dummy;
  int rs;

      rs = P_ReadNum16TS (lpc_get_ts_id (p_cb),
                          (unsigned long) p_ts, (int*) p_ap, 1, &dummy);

      lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR,LPI_DU_RESET, "get_ts_short");

  }


  void lpp_put_ts_array   (void *           p_cb,
                           void *           p_ts,
                           void *           p_ap,
                           unsigned short   size_x_2)
  {
  int rs;

	  rs = P_WriteNum16TS (lpc_get_ts_id (p_cb),
                           (int * ) p_ap, (unsigned long) p_ts,
                           (unsigned int) size_x_2);

      lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "put_ts_array");
  }


  void lpp_get_ts_array   (void *           p_cb,
                           void *           p_ap,
                           void *           p_ts,
                           unsigned short   size_x_2)
  {
  int rs;
  unsigned int dummy;

	  rs = P_ReadNum16TS (lpc_get_ts_id (p_cb),
                          (unsigned long) p_ts,
                          (int * ) p_ap, (unsigned int) size_x_2, &dummy);

      lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "get_ts_array");
  }


  void lpp_set_ts_array   (void *           p_cb,
                           void *           p_ts,
                           unsigned short   value,
                           unsigned short   size_of)
  {
  int rs;

      rs = P_FillByteTS  (lpc_get_ts_id (p_cb),
                          value, (unsigned long) p_ts, size_of);

      lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "get_ts_array");
  }


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lp_put_pit
³               lp_get_pit
³
³ return value  PCMCIA service error code
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  This routines writes and reads the PIT.
  PCMCIA Node implementation.

*/


  void lpp_put_pit  (void *            p_cb,
                     LC_TYPE_TS_PIT *  p_pit,
                     unsigned short    prt_addr,
                     unsigned short    prt_indx)
  {
  int rs;

	   rs = P_WriteNum16TS (lpc_get_ts_id (p_cb),
                            (int * ) &prt_indx,
                            ((unsigned long) p_pit) + 2 * prt_addr, 1);

       lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "put_pit");
  }


  void lpp_get_pit (void *             p_cb,
                    unsigned short *   p_ix,
                    LC_TYPE_TS_PIT *   p_pit,
                    unsigned short     prt_addr)
  {
  int rs;
  unsigned int number;  // number of values read

	   rs = P_ReadNum16TS (lpc_get_ts_id (p_cb),
                           ((unsigned long) p_pit) + 2 * prt_addr,
                           (int *) p_ix, 1, &number);

       lpi_hamster_cond (rs != LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "get_pit");
  }



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lp_put_pcs
³               lp_get_pcs
³               lp_tgl_pcs
³               lp_inc_pcs
³
³ return value  PCMCIA service error code
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  This routines writes, reads, toggles and increments the PCS.
  PCMCIA Node implementation.

*/


  void lpp_put_pcs   (void *           p_cb,
                      void *           p_pcs,
                      unsigned short   mask,
                      unsigned short   offset,
                      unsigned short   value)
  {
  int rs;
  unsigned short buff;
  int            pcs_and_offs;
  unsigned int   number;
  unsigned int   ts_id = lpc_get_ts_id (p_cb);
  unsigned long  o_pcs = (unsigned long) p_pcs + (offset / 16) * 2;

  	   rs = P_ReadNum16TS (ts_id, o_pcs, &pcs_and_offs, 1, &number);

       if (rs == SUCCESS)
       {
	     buff = (mask & (value << (offset%16))) | ( (pcs_and_offs) & ~mask);
	     rs   = P_FillNum16TS (ts_id, buff, o_pcs, 1);
       }
       lpi_hamster_cond (rs != LPP_OK, LPI_DU_ERR,LPI_DU_RESET, "put_pcs");
  }




  void lpp_get_pcs  (void *           p_cb,
                     unsigned int *   p_val,
                     void *           p_pcs,
                     unsigned short   mask,
                     unsigned short   offset)
  {
  int rs;
  unsigned int number;

	   rs = P_ReadNum16TS (lpc_get_ts_id (p_cb),
                           (unsigned long) p_pcs + (offset / 16) * 2,
                           (int *) p_val, 1, &number);
	   *p_val = (*p_val & mask) >> (offset % 16);

       lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "get_pcs");
  }



  void lpp_tgl_pcs  (void *             p_cb,
                     void *             p_pcs,
                     unsigned short     mask,
                     unsigned short     offset)
  {
  int rs;
  int buff;
  unsigned int  number;
  unsigned int  ts_id = lpc_get_ts_id (p_cb);
  unsigned long o_pcs = (unsigned long) p_pcs + (offset / 16) * 2;

	rs = P_ReadNum16TS (ts_id, o_pcs, &buff, 1, &number);
	if (number==1)
	{
        buff=buff ^ mask;
        rs = P_FillNum16TS (ts_id, buff, o_pcs, 1);
	}
    lpi_hamster_cond (rs!=LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "tgl_pcs");
  }




  void  lpp_inc_pcs   (void *           p_cb,
                       void *           p_pcs,
                       unsigned short   mask,
                       unsigned short   offset)
  {

  int rs;
  int buff;
  unsigned int number;
  unsigned int  ts_id = lpc_get_ts_id (p_cb);
  unsigned long o_pcs = (unsigned long) p_pcs + (offset / 16) * 2;
  mask=mask;  // to avoid compiler warning

	   rs = P_ReadNum16TS (ts_id, o_pcs, &buff, 1, &number);
	   if (number == 1)
	   {
		 buff = buff +1;
		 rs   = P_FillNum16TS (ts_id, buff, o_pcs, 1);
	   }
       lpi_hamster_cond (rs != LPP_OK, LPI_DU_ERR, LPI_DU_RESET, "inc_pcs");
  }



/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lp_set_node
³
³ globals       node   actual  node no
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  This routin set the actual node no.
  PCMCIA Node implementation.

*/




#endif /* O_PCMCIA */

/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------


    description of what was done to object (not functionality!)

*/
