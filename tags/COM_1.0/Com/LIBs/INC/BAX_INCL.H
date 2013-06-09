/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1994 ABB Verkehrssysteme AG
|
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BA_INCL.H
|
|    Document     :
|
|    Abstract     :   Interface Header File
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
|    Vers   Dept.   YY-MM-DD  State    Name    Reference
|    ----   ------  --------  -------  ------- ---------
|      0    BATC    94-01-01  created  Marsden
|    1147   Ma      New lc_subscribe_emf ()
 ------------------------------------------------------------------------------
*/

#ifndef     BA_INCL_H
#define     BA_INCL_H

/*  new page
 ----------------------------------------------------------------------------
|
|   Version Information
|
 ----------------------------------------------------------------------------
*/


typedef struct { unsigned short item16[100];} TYPE_USHRT_A100;
typedef struct { unsigned char  bite[50][2]; } TYPE_CHAR_A50X2;


/*  new page
 ----------------------------------------------------------------------------
|
|   contents
|
 ----------------------------------------------------------------------------

    structures

    constants

    functions



*/


/*  new page
 ----------------------------------------------------------------------------
|
|   constants
|
 ----------------------------------------------------------------------------
*/

#ifndef     RET_OK
#define     RET_OK              0
#endif

#ifndef     NULL
#define     NULL                ((void *) 0L)
#endif


#define     BA_ERROR           -1
#define     BA_E               BA_ERROR


#define     BA_CA_CNT           5

#define	    BA_PL_ETH_N_ETL_Y 	0x8000
#define	    BA_PL_ETH_Y_ETL_N 	0x4000
#define	    BA_PL_ETH_Y_ETL_Y 	0xC000

#define     BA_SCAN_KWN_DVC     0
#define     BA_SCAN_ALL_DVC     1


/*  new page
 ----------------------------------------------------------------------------
|
|   structures
|
 ----------------------------------------------------------------------------
*/

struct       BA_STR_CFG_SL
{
    unsigned char  c_low;
    unsigned char  c_hgh;
};

struct       BA_STR_CFG_SCN
{
    unsigned char  c_slot[1024];
};

struct       BA_STR_CFG_KC3
{
    unsigned short kc3_mf[256];
};

struct       BA_STR_CFG_KC4
{
    unsigned short kc4_mf[16];
};

struct       BA_STR_CFG_CTL
{
    unsigned short mf[256];
};

/*  new page
 ----------------------------------------------------------------------------
|
|   structure   Configuration Interface
|
 ----------------------------------------------------------------------------


*/

struct      BA_STR_CONFIGURATION
{
    unsigned short  nu0;                /* not used                     */
    unsigned short  mtvc;               /* master transfer version ctrl */
    unsigned short  t_reply_max;        /* 42 or 64 us                  */
    unsigned short  c_periods_in_macro; /* periods per macro (1024/512) */
    unsigned short  eps;                /* event poll strategy          */
    unsigned short  basic_period;       /* in us!                       */
    unsigned short  c_macros_in_turn;   /* macro cycles per turn        */
    unsigned short  scan_strategy;      /* 0/1: known/all devices       */
    unsigned char   nu1[8];             /* not used                     */
    unsigned short  i_kdl;              /* index p device addr list     */
    unsigned short  i_nu1;              /* not used - was i_rql         */
    unsigned short  i_ctl;              /* index p cyclic transm list   */
    unsigned short  i_bal;              /* index p bus admin list       */
    unsigned short  i_snl;              /* index scan list              */
    unsigned short  i_eol;              /* index p end of list mark     */
};

#define     BA_STR_BA_CFG    BA_STR_CONFIGURATION
#define     BA_STR_BA_CFG_1  BA_STR_CONFIGURATION

/*  new page
 ----------------------------------------------------------------------------
|
|   structure   Cyclic Index Pointer Table
|
 ----------------------------------------------------------------------------

    i_cl        index cycle list, 11 pointers to 11 cycle lists
    i_sl        index split list,  5 pointers to  5 split lists

*/

struct       BA_STR_CYC_IX_PTR_TAB
{
    unsigned short      i_cl[11];
    unsigned short      i_sl[5];
};

struct       BA_STR_SCAN_RESULT
{
    struct   BA_STR_SCAN_RSLT_MBR
    {
        unsigned short      rsp;
        unsigned short      dsw;
    }        sn_rslt[256];
};

struct       BA_STR_POLL_STAT
{
    unsigned long       c_event[256];
};

#define BA_RSP_OK           0
#define BA_RSP_MISSING      1
#define BA_RSP_ERROR        2
#define BA_RSP_NOT_POLLED   3

struct       BA_STR_DIAGNOSIS
{
    unsigned long             c_scan_update;
    unsigned long             c_error_frame;
    unsigned long             c_ref_frame;
    unsigned long             c_poll_event_error;
    unsigned long             c_poll_rsp_total;
    struct BA_STR_POLL_STAT * p_poll_rsp;
    unsigned long             c_poll_rsp;
    unsigned long             s_poll_rsp;
    struct BA_STR_SCAN_RESULT * p_scan_rslt_cl_1;
    unsigned long             c_scan_rslt_cl_1;
    unsigned long             s_scan_rslt_cl_1;
    struct BA_STR_SCAN_RESULT * p_scan_rslt_cl_2p;
    unsigned long             c_scan_rslt_cl_2p;
    unsigned long             s_scan_rslt_cl_2p;
};

/*  new page
 ----------------------------------------------------------------------------
|
|   prototypes
|
 ----------------------------------------------------------------------------

    Two interfaces are available during configuration:
    either :  bam_init () - bam_create () - bam_task ()
    or     :  bam_main ()

    One interface is available to network management:

    bam_diag ()

*/

void    bam_init   (void);

int     bam_create (unsigned int bus_id,
                    struct BA_STR_CONFIGURATION *  p_ba_cfg);

void    bam_task   (unsigned int bus_id                    );

void    bam_main   (unsigned int bus_id,
                    struct BA_STR_CONFIGURATION *  p_ba_cfg);

void    bam_diag   (unsigned int bus_id,
                    void  (*prc_notified) (struct BA_STR_DIAGNOSIS * p_diag));




/*
 ----------------------------------------------------------------------------
|
|       Structure Declaration for Indirect Call Table
|
 ----------------------------------------------------------------------------
*/

typedef struct STR_BA_IND_CALL
{
 unsigned char                       ba_version_info[4];

 void            (*pba_init  )       (       void);

 int             (*pba_create)       (unsigned int bus_id,
                                      struct BA_STR_CONFIGURATION * p_cfg);

 void            (*pba_task  )       (unsigned int bus_id                );

 void            (*pba_main  )       (unsigned int	bus_id,
                                      struct BA_STR_CONFIGURATION * p_cfg);

 void            (*pba_diag  )       (unsigned int bus_id,
                                      void  (*prc_notified)
                                     (struct BA_STR_DIAGNOSIS * p_diag)  );

} TYPE_BA_IND_CALL;

/*
 ----------------------------------------------------------------------------
|
|       Function Definitions
|
 ----------------------------------------------------------------------------
*/

                                    #if defined (O_ICM)

extern const TYPE_BA_IND_CALL       ba_ind_call_table;

                                        #if defined (O_USE_VARIABLE)
    extern   TYPE_BA_IND_CALL *     ba_pc_ptr_to_table;
    #define  PBA_IND_CALL           ba_pc_ptr_to_table
                                        #else
    #define  PBA_IND_CALL           (*(TYPE_BA_IND_CALL * *) BA_ADR_TABLE)
                                        #endif

#define ba_connect()           { PBA_IND_CALL = (void *) &ba_ind_call_table; }

#define ba_init                    (* (PBA_IND_CALL->pba_init            ))
#define ba_create                  (* (PBA_IND_CALL->pba_create          ))
#define ba_subscribe_diagnosis     (* (PBA_IND_CALL->pba_diag            ))
#define ba_main                    (* (PBA_IND_CALL->pba_main            ))
#define ba_task                    (* (PBA_IND_CALL->pba_task            ))

                                    #else

#define ba_connect()               /* */

#define	ba_init                     bam_init
#define	ba_create                   bam_create
#define ba_subscribe_diagnosis      bam_diag
#define ba_main                     bam_main
#define ba_task                     bam_task

                                    #endif

/*
 ----------------------------------------------------------------------------

    Version

 ----------------------------------------------------------------------------

*/

#define BA_VERSION      1
#define BA_RELEASE      2

#define BA_UPDATE       4
#define BA_EVOLUTION    7


                                   #if defined (O_BAP)
#define ba_get_version() \
"Name=BA BAP  V:1.0.1-D:95-XX-XX-C: Released for integration      "
                                   #else
#define ba_get_version() \
"Name=BA MVBC V:1.0.1-D:95-XX-XX-C: Released for integration      "
                                   #endif

#endif

/*
 ----------------------------------------------------------------------------

        M O D I F I C A T I O N S
        - - - - - - - - - - - - -

 ----------------------------------------------------------------------------

        V: 0    date: 94-10-24   by: W. Marsden, BATC Turgi

 ----------------------------------------------------------------------------


*/
