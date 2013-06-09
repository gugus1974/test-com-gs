/*   LPD_EXTD.C
 ----------------------------------------------------------------------------

     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland

     COPYRIGHT    :   (c) 1994, 1995 ABB Verkehrssysteme AG

     Project      :   RTP Process Data

     Document     :   [1]: IEC 9 332 Part 2: Real Time Protocols
                      [2]: 3EHT 420 087: TCN-JDP RTP Application Layer PD
                                                     User's Manual

     History      :   see end of file

 ----------------------------------------------------------------------------
*/
#if !defined (O_RTPP)

#if defined (O_960)
#define  LPI_MODULE_NAME "lpd_extd"
#endif

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "apd_incl.h"
#include "lp_sys.h"
#include "pi_sys.h"
#include "lpc_blck.h"
#include "lpl_4tcn.h"
#include "lpi_ifce.h"
#include "lp_ffld.h"            /*   Constants for modifying bit mask       */


static struct LPD_STR_DTA
{
    int       c_subscribed;
}             lpd_dta;



void   lpd_init          (void);



void   lpd_init          (void)
{
    lpi_memset (&lpd_dta, 0, sizeof (struct LPD_STR_DTA));
}

TYPE_APD_RESULT _lpd_gen_pv_set (struct STR_APD_PV_SET * p_pv_set,
                                    const struct STR_APD_PV_DESCRIPTOR * p_pv_descr,
                                    unsigned short *            p_refresh,
								    int                         c_pv_descr)
{
void *          p_cb;
LC_TYPE_TS_PIT *pb_pit;
unsigned short           prt_indx;
unsigned int    ts_id    = p_pv_descr->pv_name.ts_id;
unsigned short  prt_addr = p_pv_descr->pv_name.prt_addr;
TYPE_APD_RESULT result = APD_OK;

	if (lpc_open (ts_id, &p_cb) == LPS_OK)
	{
	    pb_pit   = lpc_get_pb_pit (p_cb);
	    lpi_get_prt_indx_via_pvn (p_cb, &prt_indx, pb_pit, prt_addr);

        p_pv_set->p_refresh = p_refresh;
        p_pv_set->p_pv_list = &p_pv_set->pv_list[0];

	    p_pv_set->p_pcs = ((unsigned char *) lpc_get_pb_pcs (p_cb)) +
                          lpl_gen_pcs_offset (p_cb, prt_indx);
		p_pv_set->p_prt = ((unsigned char *) lpc_get_pb_prt (p_cb)) +
						  lpl_gen_prt_offset (p_cb, prt_indx);
                                       #if defined (O_PCMCIA)
		p_pv_set->p_frc = ((unsigned char *) lpc_get_pb_frc_shadow (p_cb)) +
						  lpl_gen_prt_offset (p_cb, prt_indx);
                                       #else
		p_pv_set->p_frc = ((unsigned char *) lpc_get_pb_frc (p_cb)) +
						  lpl_gen_prt_offset (p_cb, prt_indx);
                                       #endif
		lpl_get_prt_size (p_cb, &p_pv_set->prt_sze, p_pv_set->p_pcs);

        p_pv_set->c_pv_list = lpd_gen_pv_list (&p_pv_set->pv_list[0],
                                               p_pv_descr,
                                               c_pv_descr);
        p_pv_set->ts_id  = p_pv_descr->pv_name.ts_id;

        p_pv_set->force_active    = LP_VAR_WITH_FRCE;

        p_pv_set->multiple_access = LP_VAR_WITHOUT;
	}
    else
    {
        result = APD_UNKNOWN_TS;
    }
    return (result);
}


void   _lpd_mod_pv_set      (struct STR_APD_PV_SET *       p_pv_set ,
                             char                      force_active ,
	                         char                   multiple_access)
{

        p_pv_set->force_active    = force_active;
        p_pv_set->multiple_access = multiple_access;


}



int    lpd_gen_pv_list  (struct STR_APD_PV_LIST *              p_pv_list,
						 const  struct STR_APD_PV_DESCRIPTOR * p_pv_descr,
						 int                                   c_pv_descr)
{
void * p_cb;
int    c_total = c_pv_descr;

	lpc_open (p_pv_descr->pv_name.ts_id, &p_cb);

	while (c_pv_descr-- > 0)
	{
		p_pv_list->p_appl      =  p_pv_descr->p_variable;

		p_pv_list->size        =  p_pv_descr->pv_name.size;
		p_pv_list->type        =  p_pv_descr->pv_name.type +
							      ((p_pv_list->size << 4) & 0x30);
		p_pv_list->bit_offset  =  p_pv_descr->pv_name.bit_offset
						       &  LPL_MASK_BYTE_OFFSET_IN_BIT_OFFSET;
		p_pv_list->byte_offset =  p_pv_descr->pv_name.bit_offset
				      	 	   >> LPL_SHIFT_BIT_OFFSET_TO_BYTE_OFFSET;

		switch (p_pv_list->type & 0xF)
		{

				case    LP_TYPE_BOOL_1:
				case    LP_TYPE_BOOL_2:
				case    LP_TYPE_BCD_4:
				case    LP_TYPE_BITSET:
				case    LP_TYPE_CARD:
				case    LP_TYPE_SIGN:

						#if defined (O_LE)
						if (p_pv_list->size == LP_SIZE_8)
						{
							if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
							{
								/* do nothing */
							}
							else
							{
								p_pv_list->byte_offset =
                                        (p_pv_list->byte_offset & 1) == 0 ?
										 p_pv_list->byte_offset + 1 :
										 p_pv_list->byte_offset - 1 ;
							}
						}
						#endif

				break;


				case    LP_TYPE_A8_EVEN:

						p_pv_list->type =  p_pv_list->type & 0xf;
						p_pv_list->size = (p_pv_list->size << 1) + 2;
                break;

				case    LP_TYPE_A8_ODD:

						p_pv_list->type =  p_pv_list->type & 0xf;
						p_pv_list->size = (p_pv_list->size << 1) + 1;

						#if defined (O_LE)
							if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
							{
								/* do nothing */
							}
							else
							{
								p_pv_list->byte_offset =
                                        (p_pv_list->byte_offset & 1) == 0 ?
										 p_pv_list->byte_offset + 1 :
										 p_pv_list->byte_offset - 1 ;
							}
						#endif
                break;

				case    LP_TYPE_A16_CARD:
				case    LP_TYPE_A16_SIGN:

						p_pv_list->type =  p_pv_list->type & 0xf;
						p_pv_list->size += 1;

				break;

				case    LP_TYPE_A32_CARD:
				case    LP_TYPE_A32_SIGN:

						p_pv_list->type =  p_pv_list->type & 0xf;
						p_pv_list->size = (p_pv_list->size >> 1) + 1;

				break;

				default:
				break;
		}

		p_pv_list++;

        if  ((p_pv_descr->pv_name.ctl_offset != 0x3FF) &&
             (p_pv_descr->p_validity != NULL))
        {
		    p_pv_list->p_appl      =  p_pv_descr->p_validity;
            p_pv_list->size        =  LP_SIZE_BITS;
		    p_pv_list->type        =  LP_TYPE_BOOL_2;
		    p_pv_list->bit_offset  =  p_pv_descr->pv_name.ctl_offset
						           &  LPL_MASK_BYTE_OFFSET_IN_BIT_OFFSET;
		    p_pv_list->byte_offset =  p_pv_descr->pv_name.ctl_offset
				      	 	       >> LPL_SHIFT_BIT_OFFSET_TO_BYTE_OFFSET;

			#if defined (O_LE)
			if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC){/* do nothing */}
            else
			{
				p_pv_list->byte_offset = (p_pv_list->byte_offset & 1) == 0 ?
									      p_pv_list->byte_offset + 1 :
										  p_pv_list->byte_offset - 1 ;
			}
			#endif

            c_total++;
		    p_pv_list++;
        }
		p_pv_descr++;
	}

    return (c_total);
}

int     lpd_force_variable (const struct STR_APD_PV_DESCRIPTOR *  p_pv_descr)
{
int     result = LPS_OK;

void *  p_cb;
LC_TYPE_TS_PRT * p_frc;
struct  STR_LP_PV_X     * p_pv_x;
char    pv_set[lpd_sizeof_pv_set(2)];
struct  STR_APD_PV_SET * p_pv_set = (struct STR_APD_PV_SET *) &pv_set[0];


                                        #if !defined (O_BAP)
TM_TYPE_SERVICE_AREA *  p_sa;
unsigned short		    is_prt_addr;      /* in service port address */
unsigned short		    my_prt_addr;      /* target port address     */
                                        #endif

  if ((result = lpd_gen_pv_set ( p_pv_set, p_pv_descr, NULL, 1)) == APD_OK)
  {
                                        #if !defined (O_BAP)
    LC_TYPE_TS_PCS    * p_pcs    = p_pv_set->p_pcs;
    unsigned int	    pcs_type;
                                        #endif
    p_cb = lpc_turbo_open (p_pv_set->ts_id);

                                        #if !defined (O_BAP)
    lpl_get_prt_type (p_cb, &pcs_type, p_pcs);

	lpl_put_prt_type (p_cb, p_pcs, LPL_PCS_PASSIVE);
	lpl_frc_enable (p_cb, p_pcs, LC_PCS_VAL_FE_SET);
                                            #if !defined (O_PCMCIA)
	p_sa        = lpi_lc_get_p_sa ();

	is_prt_addr = lpi_lc_get_mfr_pa (p_sa);
	my_prt_addr = p_pv_descr->pv_name.prt_addr;

	if (my_prt_addr == is_prt_addr)             /* wait till over */
	{
		lpi_lc_wait (20);
	}
                                            #endif
                                        #endif
    p_frc  = p_pv_set->p_frc;
    p_pv_x =  (struct STR_LP_PV_X *) &(p_pv_set->pv_list[0]);

    lpc_frc_flag_set (p_cb);

    lpi_lock ();
										#if defined (O_TCN)
	if  (lpc_get_hw_type (p_cb) == LP_HW_TYPE_HDLC)
	{
		_lpl_tb_put_pv_x (&p_frc[0][LPI_FRC_DTA], p_pv_x, p_pv_set->c_pv_list);

        p_pv_set->pv_list[0].p_appl = (void *) &lp_frc_field;
        p_pv_set->pv_list[1].p_appl = (void *) &lp_frc_field;

		_lpl_tb_put_pv_x (&p_frc[0][LPI_FRC_MSK_WTB], p_pv_x, p_pv_set->c_pv_list);
	}
	else
										#endif
    {
                                        #if !defined (O_BAP)
     void * p_temp_0;
     void * p_temp_1;
     LC_TYPE_TS_PRT * p_prt;

        p_temp_0 = p_pv_set->pv_list[0].p_appl;
        p_temp_1 = p_pv_set->pv_list[1].p_appl;
        p_prt    = p_pv_set->p_prt;
                                        #endif
        _lpl_put_pv_x (&p_frc[0][LPI_FRC_DTA], p_pv_x, p_pv_set->c_pv_list);

        p_pv_set->pv_list[0].p_appl = (void *) &lp_frc_field;
        p_pv_set->pv_list[1].p_appl = (void *) &lp_frc_field;

        _lpl_put_pv_x (&p_frc[0][LPI_FRC_MSK], p_pv_x, p_pv_set->c_pv_list);
                                        #if defined (O_PCMCIA)
        {
         LC_TYPE_TS_PRT * p_frc_ts;
            p_frc_ts = lpc_get_pb_frc (p_cb);
            lpi_put_ts_array (p_cb, p_frc_ts, p_frc, sizeof (LC_TYPE_TS_PRT));
        }
                                        #endif

                                        #if !defined (O_BAP) && !defined (O_PCMCIA)
        if (pcs_type == LPL_PCS_SINK)
        {
            p_pv_set->pv_list[0].p_appl = p_temp_0;
            p_pv_set->pv_list[1].p_appl = p_temp_1;

		    _lpl_put_pv_x (&p_prt[0][0], p_pv_x, p_pv_set->c_pv_list);
            _lpl_put_pv_x (&p_prt[0][1], p_pv_x, p_pv_set->c_pv_list);
        }
										#endif
                                        #if !defined (O_BAP)
        lpl_put_prt_type (p_cb, p_pcs, pcs_type);
										#endif

    }
	lpi_unlock ();
  }
  return (result);
}


TYPE_APD_RESULT lpd_unforce_variable (const struct STR_APD_PV_NAME * p_pv_name)
{
void * p_cb;
TYPE_APD_RESULT result;
char    pv_set[lpd_sizeof_pv_set(2)];
struct  STR_APD_PV_SET * p_pv_set = (struct STR_APD_PV_SET *) &pv_set[0];
struct	STR_APD_PV_DESCRIPTOR pv_descr;

    pv_descr.pv_name    = *p_pv_name;
    pv_descr.p_variable = (void *) &lp_unfrc_field;
    pv_descr.p_validity = (void *) &lp_unfrc_field;

    if ((result = lpd_gen_pv_set (p_pv_set,
                                  &pv_descr, NULL, 1)) == APD_OK)
    {
    struct STR_LP_PV_X * p_pv_x = (struct STR_LP_PV_X *) p_pv_set->p_pv_list;
    LC_TYPE_TS_PRT * p_frc  = p_pv_set->p_frc;
        lpi_lock ();

        p_cb = lpc_turbo_open (p_pv_set->ts_id);
        if  (lpc_get_hw_type (p_cb) == LP_HW_TYPE_HDLC)
	    {
	    char * p_mask = (char *) p_pv_set->p_frc;

		    _lpl_tb_put_pv_x (p_mask, p_pv_x, p_pv_set->c_pv_list);
		    p_mask = p_mask + LPL_SIZEOF_TB_PRT_PGE;
		    _lpl_tb_put_pv_x (p_mask, p_pv_x, p_pv_set->c_pv_list);

        }
	    else
        {
            _lpl_put_pv_x (&p_frc[0][LPI_FRC_MSK], p_pv_x, p_pv_set->c_pv_list);
            _lpl_put_pv_x (&p_frc[0][LPI_FRC_DTA], p_pv_x, p_pv_set->c_pv_list);
                                        #if defined (O_PCMCIA)
            {
             LC_TYPE_TS_PRT * p_frc_ts;
                p_frc_ts = lpc_get_pb_frc (p_cb);
                lpi_put_ts_array (p_cb, p_frc_ts, p_frc,
                                  sizeof (LC_TYPE_TS_PRT));
            }
                                        #endif

        }

	    lpi_unlock ();
    }

    return (result);
}


TYPE_APD_RESULT lpd_subscribe   (struct STR_APD_DS_NAME * p_ds_name,
                                 void   (*p_proc) (unsigned short),
                                 unsigned short   param,
                                 int    index)
{
void * p_cb;
LC_TYPE_TS_PIT * pb_pit;
char * p_pcs;

TYPE_APD_RESULT result;
unsigned short  prt_indx;
                                       #if defined (O_PD_TEST)
    param  = param;
    p_proc = p_proc;
                                       #endif

    result  = lpc_open (p_ds_name->ts_id, &p_cb);

    if (result == LPS_OK)
    {
        if (lpc_get_hw_type (p_cb) == LP_HW_TYPE_MVBC)
        {
           	pb_pit   = lpc_get_pb_pit   (p_cb);
	        lpi_get_prt_indx (p_cb, &prt_indx, pb_pit, p_ds_name->prt_addr);

	        p_pcs    = ((char *) lpc_get_pb_pcs (p_cb)) +
				                     lpl_gen_pcs_offset (p_cb, prt_indx);

	        if (lpi_lc_int_connect ((void (*)(unsigned short))p_proc,
                                   index, LC_KERNEL, param) == LC_OK)
            {
   		        lpi_put_pcs_bap  (p_pcs, LC_PCS_MSK_IEX,
                                        LC_PCS_OFF_IEX, index);
   		        lpi_put_pcs_mvbc (p_cb, p_pcs, LC_PCS_MSK_DTI,
                                               LC_PCS_OFF_DTI, (index + 1));
            }
            else
            {
                lpi_hamster_uncond (LPI_DU_WARN, LPI_DU_CONT,
                                    "lc int connect error"); /*$H. lc int connect error */
                result = APD_ERROR;
            }
        }
        else
        {
            lpi_hamster_uncond (LPI_DU_WARN, LPI_DU_CONT,
                                "wrong hw type for subscription"); /*$H. wrong hwtype */
            result = APD_ERROR;
        }
    }
    else
    {
        lpi_hamster_uncond (LPI_DU_WARN, LPI_DU_CONT, "unkown ts id"); /*$H. unknown ts_id */
        result = APD_UNKNOWN_TS;
    }

	return (result);
}



TYPE_APD_RESULT lpd_desubscribe       (struct STR_APD_DS_NAME * p_ds_name,
                                       int    index)
{
void * p_cb;
LC_TYPE_TS_PIT * pb_pit;
char * p_pcs;

TYPE_APD_RESULT result;
unsigned short  prt_indx;
                                       #if defined (O_PD_TEST)
        index = index;
                                       #endif
        result  = lpc_open (p_ds_name->ts_id, &p_cb);

        if (result == LPS_OK)
        {
            if (lpc_get_hw_type (p_cb) == LP_HW_TYPE_MVBC)
            {
              	pb_pit   = lpc_get_pb_pit   (p_cb);
		        lpi_get_prt_indx (p_cb, &prt_indx, pb_pit,
                                  p_ds_name->prt_addr);

		        p_pcs    = ((char *) lpc_get_pb_pcs (p_cb)) +
					                     lpl_gen_pcs_offset (p_cb, prt_indx);

   		        lpi_put_pcs_bap  (p_pcs, LC_PCS_MSK_IEX, LC_PCS_OFF_IEX,
                                                        LC_PCS_VAL_IEX_CLR);
   		        lpi_put_pcs_mvbc (p_cb, p_pcs,    LC_PCS_MSK_DTI,
                                  LC_PCS_OFF_DTI, LC_PCS_VAL_DTI_CLR);
		        lpi_lc_int_disconnect (index);
            }
            else { result = APD_ERROR; }
        }
        else { result = APD_UNKNOWN_TS; }

		return (result);

}

#endif



/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------


   V1:   95-12-06:  lpd_force / unforce: new shadow force table for pcmcia

*/
