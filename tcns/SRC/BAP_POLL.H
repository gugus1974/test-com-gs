/*   BAP_POLL.H
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|                     
|    File Name    :   BAP_POLL.H
|                     
|    Document     :
|                     
|    Abstract     :   MVB Event Poll Task
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|                     
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      0   BAEI-2  93-01-11  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef     BAP_POLL_H
#define     BAP_POLL_H


#include	"baa_auxi.h"
#include	"bal_link.h"

#if defined (BX_SIMU)

#define		BAP_TIME_16					BSP_TIME_16


#endif

/*  new page
 ----------------------------------------------------------------------------
|
|   constants     BAP_T
|
 ----------------------------------------------------------------------------

	These constants specify the minimum time left in a slot to do

	either event execution or poll
	or
	both event execution and poll

*/


#define		BAP_T_1					300




#define 	bap_dta_h_nxt(p_nxt_func, p_q, nxt_mf)	\
			{ bap_dta.hgh.p_nxt = p_nxt_func; p_q->n_pf = nxt_mf;}

#define 	bap_dta_l_nxt(p_nxt_func, p_q, nxt_mf)	\
			{ bap_dta.low.p_nxt = p_nxt_func; p_q->n_pf = nxt_mf;}

struct      BAP_STR_TRANS_TAB
{
			unsigned short  mfa[3];
};

struct      BAP_STR_IND_IND_CALL
{
			int   (**p_iic) (struct BAL_STR_Q * p_q_sf);
			int   (*p_ic) (void);
};

struct		BAP_STR_SF_CHK
{
			unsigned short temp;
};


struct   BAP_STR_MEM_ET
{
	     int   (*p_nxt)        (struct BAL_STR_Q *);
		 int   (**p_p_frontier)(struct BAL_STR_Q *);
		 unsigned short *		p_n_mf;
		 unsigned short  		n_mf;
		   signed short			rslt;
		 unsigned short			sf_log;
         struct
         {
              unsigned short    c_mf;
              unsigned short    n_mf;
         }                      gl_ins;    /* global insert after 2 events */
};

extern	struct  BAP_STR_IND_IND_CALL    bap_gbl;

#define     bap_poll                (** (bap_gbl.p_iic))
#define		bap_send_mf				(*  (bap_gbl.p_ic))


struct		BAP_STR_DBUG
{
			unsigned short empty;
};

struct      BAP_STR_DTA
{
	const   struct  BAP_STR_TRANS_TAB * p_tt;
			struct  BAP_STR_DBUG        dbug;
			  signed int  				hgh_rslt_glob_act;
			  signed int  				hgh_rslt_glob_pas;
			unsigned short				poll_strategy;
			unsigned short  			low_cont_mf;
			struct  BAP_STR_MEM_ET  	low;
			struct  BAP_STR_MEM_ET  	hgh;
};


#define		BAP_SF_SENT					1
#define		BAP_SF_PEND					0

#define     BAP_FRST_MF_GRP_POLL        0xDFFC


#define     BAP_FCODE_MASK              0xF000

#define     BAP_FCODE_15_TIME           1
#define     BAP_FCODE_12_TIME           (BAP_FCODE_15_TIME + 1)


/*  new page
 ------------------------------------------------------------------------------
|
|   name          switch table constants
|
 ------------------------------------------------------------------------------
*/

#define     BAP_CASE_ETH_STRT           0
#define     BAP_CASE_ETH_GLOB           1
#define     BAP_CASE_ETH_POLL_GBL       2
#define     BAP_CASE_ETH_POLL_GRP       3
#define     BAP_CASE_ETH_EXCT           4

#define     BAP_CASE_ETL_STRT           5
#define     BAP_CASE_ETL_GLOB           6
#define     BAP_CASE_ETL_POLL_GBL       7
#define     BAP_CASE_ETL_POLL_GRP       8
#define     BAP_CASE_ETL_EXCT           9
#define     BAP_CASE_ETL_CONT           10

#define     BAP_CASE_ESC                11
#define     BAP_CASE_ERROR              11



/*  new page
 ------------------------------------------------------------------------------
|
|   name          return values poll state
|
 ------------------------------------------------------------------------------
*/

#define     BAP_ET_N                0
#define     BAP_ET_Y                1

#define     BAP_ETH_N_ETL_N         0x0
#define     BAP_ETH_Y_ETL_N         0x4
#define     BAP_ETH_N_ETL_Y         0x8
#define     BAP_ETH_Y_ETL_Y         0xC


#define		BAP_POLL_STPD_AT_GLOBAL	0x1
#define		BAH_END_OF_SLOT			0x2

/*  new page
 ------------------------------------------------------------------------------
|
|   name          commands for poll param
|
 ------------------------------------------------------------------------------
*/

#define     BAP_E_T_BIT             0x0010
#define     BAP_ETH                 0x0000
#define     BAP_ETL                 0x0010

#define     BAP_S_R_BIT             0x0200
#define     BAP_RPLY                0x0000
#define     BAP_SLNC                0x0200

#define     BAP_S_C_BIT             0x0100
#define     BAP_CONT                0x0000
#define     BAP_STRT                0x0100

#define     BAP_CONT_RPLY           (BAP_CONT + BAP_RPLY)
#define     BAP_STRT_RPLY           (BAP_STRT + BAP_RPLY)

#define     BAP_CONT_SLNC           (BAP_CONT + BAP_SLNC)
#define     BAP_STRT_SLNC           (BAP_STRT + BAP_SLNC)

#define     BAP_ET_FCODE            0x9000

#define     BAP_ETH_MF_CONT_RPLY    (BAP_ET_FCODE + BAP_ETH + BAP_CONT_RPLY)
#define     BAP_ETH_MF_STRT_RPLY    (BAP_ET_FCODE + BAP_ETH + BAP_STRT_RPLY)
#define     BAP_ETH_MF_CONT_SLNC    (BAP_ET_FCODE + BAP_ETH + BAP_CONT_SLNC)
#define     BAP_ETH_MF_STRT_SLNC    (BAP_ET_FCODE + BAP_ETH + BAP_STRT_SLNC)

#define     BAP_ETL_MF_CONT_RPLY    (BAP_ET_FCODE + BAP_ETL + BAP_CONT_RPLY)
#define     BAP_ETL_MF_STRT_RPLY    (BAP_ET_FCODE + BAP_ETL + BAP_STRT_RPLY)
#define     BAP_ETL_MF_CONT_SLNC    (BAP_ET_FCODE + BAP_ETL + BAP_CONT_SLNC)
#define     BAP_ETL_MF_STRT_SLNC    (BAP_ET_FCODE + BAP_ETL + BAP_STRT_SLNC)


/*  new page
 ------------------------------------------------------------------------------
|
|   prototypes
|
 ------------------------------------------------------------------------------
*/

void 			bap_open			(struct BAP_STR_DTA ** 	p_bap_dta);

void        	bap_init        	(void);

void            bap_create          (unsigned short     strategy,
                                     unsigned short     no_second_poll);

short        	bap_reset      	    (void);

void			bap_etl_cont		(struct BAL_STR_Q *		p_q_sf);


#if defined     (BX_SIMU)

#include        "bsp_poll.h"

void *          bap_p_trans         (void);

#define         bap_param           bsp_param
#define         bap_rcve_sf         bsp_rcve_sf
#define         bap_send_sf         bsp_send_sf
#define         bap_execute         bsp_execute


#endif


#endif
