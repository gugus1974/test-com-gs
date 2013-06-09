/*
 ----------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|
 ----------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ----------------------------------------------------------------------------
|
|    File Name    :   BAM_MAIN.H
|
|    Document     :
|
|    Abstract     :   Bus Administrator State Control
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ----------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      1   BAEI-2  93-02-10  created  Marsden
|
 ----------------------------------------------------------------------------
*/

#ifndef		BAM_MAIN_H
#define 	BAM_MAIN_H

#include    "bax_incl.h"

/* 	new page
 ----------------------------------------------------------------------------
|
| 	name      	BAM_UNN_STATE
|
 ------------   A B S T R A C T   -------------------------------------------

	The state components are
		"active / passive"
		"master / redundant"

	The constants BAM_IX_MR and BAM_IX_AP are used to index the components.

*/

union			BAM_UNN_STATE
{
		char	sptd[2];
		short	both;
};


/* 	new page
 ----------------------------------------------------------------------------
|
| 	structures
|
 ----------------------------------------------------------------------------
*/

struct      BAM_STR_CFG_DIV
{
    unsigned short checksum;
    unsigned short c_version_update;
    unsigned short c_slot;
    unsigned short fi_slot;          /* !!! can be removed later */
    unsigned short fx_slot;          /* !!! can be removed later */
    unsigned short c_kc3;
    unsigned short c_kc4;
    unsigned short ebl;
    unsigned short pl_strategy;
    unsigned short pl_no_second_poll;
    unsigned short sn_strategy;
    unsigned short basic_cycle;
    unsigned short bmt_cycle;
    unsigned short own_dvc_addr;
    unsigned short sys_c_tick;
};

struct      BAM_STR_CFG_CYC
{
    unsigned short       c_0001ms;
    TYPE_CHAR_A50X2 *    p_c_0002;
    TYPE_CHAR_A50X2 *    p_c_0008;
    TYPE_CHAR_A50X2 *    p_c_0032;
    TYPE_CHAR_A50X2 *    p_c_0128;
    TYPE_CHAR_A50X2 *    p_c_0512;
    TYPE_USHRT_A100 *    p_0001ms;
    TYPE_USHRT_A100 *    p_0002ms;
    TYPE_USHRT_A100 *    p_0004ms;
    TYPE_USHRT_A100 *    p_0008ms;
    TYPE_USHRT_A100 *    p_0016ms;
    TYPE_USHRT_A100 *    p_0032ms;
    TYPE_USHRT_A100 *    p_0064ms;
    TYPE_USHRT_A100 *    p_0128ms;
    TYPE_USHRT_A100 *    p_0256ms;
    TYPE_USHRT_A100 *    p_0512ms;
    TYPE_USHRT_A100 *    p_1024ms;
};

struct      BAM_STR_CFG_PT
{
    void *                     p_cpt;
    struct  BAM_STR_CFG_DIV *  p_div;      /* diverse information           */
    struct  BAM_STR_CFG_CYC *  p_cyc;
    struct  BA_STR_CFG_SCN *   p_scn;      /* configurator scan allocation  */
    struct  BA_STR_CFG_KC4 *   p_kc4;      /* known master list             */
    struct  BA_STR_CFG_KC3 *   p_kc3;      /* known slave list              */
};

struct			BAM_STR_DTA
{
    unsigned long               init_key;
	short						state;
	short						sema_id;
	unsigned short              mtvc;
    struct BAM_STR_CFG_PT       pt_cfg;
    struct BAM_STR_CFG_CYC      ba_cyc;
    struct BAM_STR_CFG_DIV      ba_cfg;
};

/* 	new page
 ----------------------------------------------------------------------------
|
| 	constants
|
 ----------------------------------------------------------------------------
*/

#define		BAM_RA				1                       /*  State Redundant */
#define		BAM_BM				2                       /*  State Master    */
#define		BAM_NCFG			4       				/*  State Passive   */
#define		BAM_CFGD			8                       /*  State Active    */

#define		BAM_RA_NCFG			(BAM_RA + BAM_NCFG)
#define		BAM_RA_CFGD			(BAM_RA + BAM_CFGD)
#define		BAM_BM_NCFG			(BAM_BM + BAM_NCFG)
#define		BAM_BM_CFGD			(BAM_BM + BAM_CFGD)

#define 	BAM_C_64MS 			64 / 1.3
#define		BAM_RA_T_WAIT_CFGD	2						/*  Req Spec: 4.1.4 */
#define		BAM_RA_T_WAIT_NCFG	17						/*  Req Spec: 4.1.4 */

#define		bam_get_state(p_bam)	(p_bam->u_state.both)


/* 	new page
 ----------------------------------------------------------------------------
|
| 	prototypes
|
 ----------------------------------------------------------------------------
*/

void 		bam_open			(struct BAM_STR_DTA ** 		p_bam);

void		bam_st_2_ra			(unsigned short				state);

void		bam_st_2_bm 		(unsigned short				state);

void		bam_st_trans_di_ext	(unsigned short				state);

void        bam_task_act        (void);

#if defined (BX_SIMU)
#define     bam_task_rdy()      {bad_task(); bat_task();}
#else
#if defined (O_BAP)
#define     bam_task_rdy()      {;} /* called via rslt = 1 */
#else
#define     bam_task_rdy        bam_task_act
#endif
#endif

#define			bam_st_trans_di_int(param)			\
				{ 	bai_disable ();					\
					bam_st_trans_di_ext(param);		\
					bai_enable ();			 	}

#if defined (O_BAP)
#define bam_lc_config() RET_OK
#else
short   bam_lc_config (void);
#endif



#endif
