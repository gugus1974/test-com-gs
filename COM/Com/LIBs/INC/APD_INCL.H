/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
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

#if defined (__cplusplus)
 extern "C"
 {
#endif


#ifndef APD_INCL_H
#define APD_INCL_H

#if defined (O_ICM) && !defined (O_USE_VARIABLE)
#include "vcos_sys.h"
#endif

typedef         unsigned int TYPE_PD_BITFIELD;

/*
 ----------------------------------------------------------------------------

    DS_NAME                            Ref: IEC Part 2, Chapter 2.3.1.2

 ----------------------------------------------------------------------------

*/

typedef struct          STR_APD_DS_NAME
{
#if defined (O_LE)
    TYPE_PD_BITFIELD    prt_addr: 12;
    TYPE_PD_BITFIELD    ts_id   : 4;
#else
    TYPE_PD_BITFIELD    ts_id   : 4;
    TYPE_PD_BITFIELD    prt_addr: 12;
#endif
}                       TYPE_APD_DS_NAME;



/*
 ----------------------------------------------------------------------------

    PV_NAME                            Ref: IEC Part 2, Chapter 2.3.1.2

 ----------------------------------------------------------------------------

*/

#if  defined (O_LE)
#define apd_init_dsn(ts_id, prt_addr) prt_addr, ts_id
#define apd_init_pvn(ts_id, prt_addr, size, bit_o, type, ctl_o) \
        ctl_o, type, bit_o, size, prt_addr, ts_id
#else
#define apd_init_dsn(ts_id, prt_addr) ts_id, prt_addr
#define apd_init_pvn(ts_id, prt_addr, size, bit_o, type, ctl_o) \
        ts_id, prt_addr, size, bit_o, type, ctl_o
#endif

typedef struct          STR_APD_PV_NAME
{
#if defined (O_LE)
    TYPE_PD_BITFIELD    ctl_offset  :10;
    TYPE_PD_BITFIELD    type        : 6;
    TYPE_PD_BITFIELD    bit_offset  :10;
    TYPE_PD_BITFIELD    size        : 6;
    TYPE_PD_BITFIELD    prt_addr    : 12;
    TYPE_PD_BITFIELD    ts_id       : 4;
#else
    TYPE_PD_BITFIELD    ts_id       : 4;
    TYPE_PD_BITFIELD    prt_addr    : 12;
    TYPE_PD_BITFIELD    size        : 6;
    TYPE_PD_BITFIELD    bit_offset  :10;
    TYPE_PD_BITFIELD    type        : 6;
    TYPE_PD_BITFIELD    ctl_offset  :10;
#endif
}                       TYPE_APD_PV_NAME;

/*
 ----------------------------------------------------------------------------

    Constants                          Ref: IEC Part 2, Chapter 2.3.2

 ----------------------------------------------------------------------------
*/

typedef int                 TYPE_APD_RESULT;
                                       #if !defined PD_T_RESULT
#define PD_T_RESULT
typedef TYPE_APD_RESULT     TYPE_PD_RESULT;
                                       #endif

#define APD_OK              0
#define APD_PRT_PASSIVE     1
#define APD_ERROR           2
#define APD_CONFIG          3
#define APD_MEMORY          4
#define APD_UNKNOWN_TS      5
#define APD_RANGE           6
#define APD_DATA_TYPE       7

#define APD_TS_ID_MAX       15
#define APD_C_SUBSCRIBERS   5

#define APD_LL_TCN          0
#define APD_LL_FIP          1

#define APD_PASSIVE         0
#define APD_ACTIVE          1

#define APD_CHK_IS_ERROR    0
#define APD_CHK_IS_OK       1
#define APD_CHK_IS_FRCD     2
#define APD_CHK_IS_UDFND    3

/*
 ----------------------------------------------------------------------------

    Cluster Structures                  Ref: IEC Part 2, Chapter 2.3.4

 ----------------------------------------------------------------------------
*/



struct  STR_APD_PV_LIST
{
    void *                  p_appl;
    unsigned char           type;
    unsigned char           size;
    unsigned char           byte_offset;
    unsigned char           bit_offset;
};


struct  STR_APD_PV_SET
{
    struct STR_APD_PV_LIST *p_pv_list;
    unsigned short *        p_refresh;
    void *                  p_pcs;
    void *                  p_prt;
    void *                  p_frc;
    unsigned char           prt_sze;
    unsigned char           c_pv_list;
    unsigned short          ts_id;
    unsigned char           force_active;
    unsigned char           multiple_access;
    struct STR_APD_PV_LIST  pv_list[1];
};

struct  STR_APD_PV_CLUSTER
{
    unsigned short          ts_id;
    unsigned char           c_pv_set;
    struct STR_APD_PV_SET * p_pv_set[1];
};

struct STR_APD_PV_DESCRIPTOR
{
    struct STR_APD_PV_NAME  pv_name;
    void *                  p_variable;
    void *                  p_validity;
};


/*
 ----------------------------------------------------------------------------

    Cluster Boolean Pack Structures

 ----------------------------------------------------------------------------

    A FUPLA feature

    PAC_2:     Each member of the PAC_2 array has a pointer to the boolean
               and a mask marking the bit position with a "1" in the bitset.

    PAC_1:
*/


struct STR_APD_PAC_2
{
    char *                 p_bool;
    unsigned int           b_mask;
};

struct STR_APD_PAC_1
{
    struct STR_APD_PAC_2 * p_array_of_pac_2;
    unsigned short *       p_bitset;
    unsigned int           c_bools_in_bitset;
};

struct STR_APD_PAC_0
{
    struct STR_APD_PAC_1 * p_array_of_pac_1;
    unsigned int           c_pac_1;
};




/*
 ----------------------------------------------------------------------------

    Static Structure

 ----------------------------------------------------------------------------

    For each traffic store id, a link layer type can be connected.
    At the moment, only the TCN link layer is known. The FIP link layer
    is planned.

*/

struct APD_STR_DTA
{
    struct
    {
        unsigned short in_use ;
        unsigned short ds_name;
        unsigned short param  ;
        void (*p_func) (unsigned short);
    }   subscribed[APD_C_SUBSCRIBERS];
    char ll_type[16];
};


/*
 ----------------------------------------------------------------------------

    Prototypes              Ref: IEC Part 2, Chapter 2.3.3

 ----------------------------------------------------------------------------
*/

/*  Initialisation   */

TYPE_PD_RESULT _apd_init            (void                                  );

TYPE_PD_RESULT  apd_connect_mll     (unsigned int     ts_id,
                                      int              ll_type                       );
/*  put and get variable   */

TYPE_PD_RESULT _apd_put_variable    (const struct STR_APD_PV_DESCRIPTOR * p);

TYPE_PD_RESULT _apd_get_variable    (const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh);

/*  put and get data set   */

TYPE_PD_RESULT _apd_put_set         (const struct STR_APD_PV_DESCRIPTOR * p ,
                                      int                           c_pv_set);

TYPE_PD_RESULT _apd_put_set_fast    (const struct STR_APD_PV_SET *        p);

TYPE_PD_RESULT _apd_get_set         (const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh ,
                                      int                           c_pv_set);

TYPE_PD_RESULT _apd_get_set_fast    (const struct STR_APD_PV_SET *        p);


/*  put and get clusters   */

TYPE_PD_RESULT _apd_put_cluster     (const struct STR_APD_PV_CLUSTER *    p);

TYPE_PD_RESULT _apd_get_cluster     (const struct STR_APD_PV_CLUSTER *    p);


/*  Force Mechanism        */

TYPE_PD_RESULT _apd_force_variable  (const struct STR_APD_PV_DESCRIPTOR * p);

TYPE_PD_RESULT _apd_unforce_variable(const struct STR_APD_PV_NAME *  p_pv_n);

TYPE_PD_RESULT _apd_unforce_all     (unsigned                     int ts_id);


/*  Subscription           */

TYPE_PD_RESULT _apd_subscribe       (struct STR_APD_DS_NAME *     p_ds_name ,
                                      void (*proc)          (unsigned short) ,
                                      unsigned short               param   );

TYPE_PD_RESULT _apd_desubscribe     (struct STR_APD_DS_NAME *     p_ds_name);

TYPE_PD_RESULT _apd_pass_subscription(struct STR_APD_DS_NAME*     p_dsn_old ,
                                      struct STR_APD_DS_NAME *     p_dsn_new);

/*  Boolean to Bitset Conversion  */

void            _apd_pack            (const struct STR_APD_PAC_0 *   p_pac_0);

void            _apd_unpack          (const struct STR_APD_PAC_0 *   p_pac_0);


/*  Externally Available Link Layer Extensions   */

TYPE_PD_RESULT _lpd_gen_pv_set      (struct STR_APD_PV_SET *       p_pv_set ,
                                      const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh ,
						              int                           c_pv_set);

void            _lpd_mod_pv_set      (struct STR_APD_PV_SET *       p_pv_set ,
                                      char                      force_active ,
						              char                   multiple_access);


/*  Internally Used Link Layer Extensions   */

TYPE_PD_RESULT lpd_gen_pv_list      (struct STR_APD_PV_LIST *     p_pv_list ,
							          const struct STR_APD_PV_DESCRIPTOR * p ,
								      int                           c_pv_set);

TYPE_PD_RESULT lpd_force_variable   (const struct STR_APD_PV_DESCRIPTOR * p);

TYPE_PD_RESULT lpm_mpb_init	     (unsigned short	            offs_bcr);

void		    lpm_mpb_close	     (void                                  );

TYPE_PD_RESULT lpm_read_mpb_setup   (void **                           p_ts ,
                                      unsigned int   *            bap_ioaddr ,
                                      unsigned int   *           mpb_intr_nr);


TYPE_PD_RESULT lpd_unforce_variable (const struct STR_APD_PV_NAME * p_pv_nm);

TYPE_PD_RESULT lpd_subscribe        (struct STR_APD_DS_NAME *    p_ds_name ,
                                      void (*proc)           (unsigned short),
                                      unsigned short                   param ,
                                      int    ix                             );

TYPE_PD_RESULT lpd_desubscribe      (struct STR_APD_DS_NAME *    p_ds_name ,
                                      int    ix                             );

TYPE_PD_RESULT lpd_pass_subscription(struct STR_APD_DS_NAME * p_ds_name_old ,
                                      struct STR_APD_DS_NAME * p_ds_name_new);


#define lpd_sizeof_pv_set(c_pv_set) \
   (unsigned short) ( (sizeof (struct STR_APD_PV_SET) +    \
                      (sizeof (struct STR_APD_PV_LIST) * (2*c_pv_set))) )



/*
 ----------------------------------------------------------------------------
|
|       Structure Declaration for Indirect Call Table
|
 ----------------------------------------------------------------------------
*/

typedef struct STR_APD_IND_CALL
{
 unsigned char                       apd_version_info[4];

 TYPE_PD_RESULT (*papd_init)        (       void);

 TYPE_PD_RESULT (*papd_put_variable)(const struct STR_APD_PV_DESCRIPTOR * p);

 TYPE_PD_RESULT (*papd_get_variable)(const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh);

 TYPE_PD_RESULT (*papd_put_set     )(const struct STR_APD_PV_DESCRIPTOR * p ,
                                      int                           c_pv_set);

 TYPE_PD_RESULT (*papd_put_set_fast)(const struct STR_APD_PV_SET *        p);

 TYPE_PD_RESULT (*papd_get_set     )(const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh ,
                                      int                           c_pv_set);

 TYPE_PD_RESULT (*papd_get_set_fast)(const struct STR_APD_PV_SET *        p);

 TYPE_PD_RESULT (*papd_put_cluster )(const struct STR_APD_PV_CLUSTER *    p);

 TYPE_PD_RESULT (*papd_get_cluster )(const struct STR_APD_PV_CLUSTER *    p);

 TYPE_PD_RESULT (*papd_force_var   )(const struct STR_APD_PV_DESCRIPTOR * p);

 TYPE_PD_RESULT (*papd_unforce_var )(const struct STR_APD_PV_NAME *  p_pv_n);

 TYPE_PD_RESULT (*papd_unforce_all )(unsigned                     int ts_id);

 TYPE_PD_RESULT (*papd_subscribe   )(struct STR_APD_DS_NAME *     p_ds_name ,
                                      void (*proc)  (unsigned short param  ) ,
                                      unsigned short              the_param );

 TYPE_PD_RESULT (*papd_desubscribe )(struct STR_APD_DS_NAME *     p_ds_name);

 TYPE_PD_RESULT (*papd_pass_subscr )(struct STR_APD_DS_NAME * p_ds_name_old ,
                                      struct STR_APD_DS_NAME * p_ds_name_new);

 void            (*papd_pack        )(const struct STR_APD_PAC_0 *   p_pac_0);

 void            (*papd_unpack      )(const struct STR_APD_PAC_0 *   p_pac_0);

 TYPE_PD_RESULT (*plpd_gen_pv_set  )(struct STR_APD_PV_SET *       p_pv_set ,
                                      const struct STR_APD_PV_DESCRIPTOR * p ,
                                      unsigned short *             p_refresh ,
						              int                           c_pv_set);

 void            (*plpd_mod_pv_set  )(struct STR_APD_PV_SET *       p_pv_set ,
						              char                      force_active ,
                                      char                   multiple_access);

} TYPE_PD_IND_CALL;


/*
 ----------------------------------------------------------------------------
|
|       Function Definitions
|
 ----------------------------------------------------------------------------
*/

#if defined (O_ICM)

extern const TYPE_PD_IND_CALL      pd_ind_call_table;

#if defined (O_USE_VARIABLE)
    extern   TYPE_PD_IND_CALL *    pd_pc_ptr_to_table;
    #define  PPD_IND_CALL          pd_pc_ptr_to_table
#else
    #define  PPD_IND_CALL          (*(TYPE_PD_IND_CALL * *) PD_ADR_TABLE)
#endif

#define apd_connect() { PPD_IND_CALL = (void *) &pd_ind_call_table; }

#define apd_init                    (* (PPD_IND_CALL->papd_init            ))
#define apd_put_variable            (* (PPD_IND_CALL->papd_put_variable    ))
#define apd_get_variable            (* (PPD_IND_CALL->papd_get_variable    ))
#define apd_put_set                 (* (PPD_IND_CALL->papd_put_set         ))
#define apd_put_set_fast            (* (PPD_IND_CALL->papd_put_set_fast    ))
#define apd_get_set                 (* (PPD_IND_CALL->papd_get_set         ))
#define apd_get_set_fast            (* (PPD_IND_CALL->papd_get_set_fast    ))
#define apd_put_cluster             (* (PPD_IND_CALL->papd_put_cluster     ))
#define apd_get_cluster             (* (PPD_IND_CALL->papd_get_cluster     ))
#define apd_force_variable          (* (PPD_IND_CALL->papd_force_var       ))
#define apd_unforce_variable        (* (PPD_IND_CALL->papd_unforce_var     ))
#define apd_unforce_all             (* (PPD_IND_CALL->papd_unforce_all     ))
#define apd_subscribe               (* (PPD_IND_CALL->papd_subscribe       ))
#define apd_desubscribe             (* (PPD_IND_CALL->papd_desubscribe     ))
#define apd_pass_subscription       (* (PPD_IND_CALL->papd_pass_subscr     ))
#define apd_pack                    (* (PPD_IND_CALL->papd_pack            ))
#define apd_unpack                  (* (PPD_IND_CALL->papd_unpack          ))

#define lpd_gen_pv_set              (* (PPD_IND_CALL->plpd_gen_pv_set      ))
#define lpd_mod_pv_set              (* (PPD_IND_CALL->plpd_mod_pv_set      ))

#else

#define apd_connect()              /* */

#define	apd_init                    _apd_init
#define apd_put_variable            _apd_put_variable
#define apd_get_variable            _apd_get_variable
#define apd_put_set                 _apd_put_set
#define apd_put_set_fast            _apd_put_set_fast
#define apd_get_set                 _apd_get_set
#define apd_get_set_fast            _apd_get_set_fast
#define apd_put_cluster             _apd_put_cluster
#define apd_get_cluster             _apd_get_cluster
#define apd_force_variable          _apd_force_variable
#define apd_unforce_variable        _apd_unforce_variable
#define apd_unforce_all             _apd_unforce_all
#define apd_subscribe               _apd_subscribe
#define apd_desubscribe             _apd_desubscribe
#define apd_pass_subscription       _apd_pass_subscription
#define apd_pack                    _apd_pack
#define apd_unpack                  _apd_unpack

#define lpd_gen_pv_set              _lpd_gen_pv_set
#define lpd_mod_pv_set              _lpd_mod_pv_set

#endif




/*
 ----------------------------------------------------------------------------

    Version

 ----------------------------------------------------------------------------

*/

#define PD_VERSION      1
#define PD_RELEASE      1

#define PD_UPDATE       3
#define PD_EVOLUTION    4


#if defined (O_BAP)
#define apd_get_version() \
"Name=RTP-APD BAP  V:1.1.1-D:95-XX-XX-C: Released for integration      "
#else
#define apd_get_version() \
"Name=RTP-APD MVBC V:1.1.1-D:95-XX-XX-C: Released for integration      "
#endif

#endif


#if defined (__cplusplus)
 }
#endif



/*
 ----------------------------------------------------------------------------

        M O D I F I C A T I O N S
        - - - - - - - - - - - - -

 ----------------------------------------------------------------------------


        V: 0    date: 94-10-24   by: W. Marsden, BATC Turgi

           1    date: 95-08-10   by: W. Marsden, BATC Turgi 
                                 apd_subscribe:   New parameter "param" 
                                 apd_subscribe:   ds_name passed by address
                                 apd_desubscribe: ds_name passed by address
           2    date: 95-09-26   by: W. Marsden, BATC Turgi 
                                 STR_APD_PV_CLUSTER  struct STR_APD_PV_SET *
                                 instead of          struct STR_APD_PV_LIST *
 ----------------------------------------------------------------------------


*/

