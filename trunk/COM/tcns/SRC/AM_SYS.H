/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Included "atr_sys.h"                                                                */
/*==============================================================================================*/

/* <ATR:01> */
#include "atr_sys.h"


/*
********************************************************************************
*                                                                              *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland     *
*    COPYRIGHT    :   (c) 1990 ABB Verkehrssysteme AG                          *
*                                                                              *
********************************************************************************
*                                                                              *
*    PROJECT      :   MicOS Version 2                                            *
*                                                                              *
*    SUBPROJECT   :   Message Data (Application Layer)                           *
*                                                                              *
********************************************************************************
*                                                                              *
*    MODULE       :   am_sys.h                                                   *
*                                                                              *
*    DOCUMENT     :                                                            *
*                                                                              *
*    ABSTRACT     :   Definition of constants and prototypes Message data (AM)   *
*                                                                              *
*    REMARKS      :   Indirect call table is defined                             *
*                                                                              *
*    DEPENDENCIES : -                                                          *
*                                                                              *
*    ACCEPTED     :                                                            *
*                                                                              *
********************************************************************************
*                                                                              *
*    HISTORY:                                                                  *
*                                                                              *
*    Version  YY-MM-DD  Name   Dept.   Ref   Status                            *
*    -------  --------  -----  ------  ----  -------                           *
*     1.0     92-03-20  Mr     BAEI-2   --   created                           *
*@JDP 1.1     92-11-17  EH     CRBC-1   --   added am_invoke_requ,             *
*@JDP                                        AM_INVOKE_CONF, AM_REPLIER_STATES,*
*@JDP                                        am_get_replier_status             *   
*@JDP: lines containing "@JDP" must be deleted to get the JDP version          *
*     1.2     93-11-08  Mr     BATC     --   char types changed to unsigned    *
*     1.3     93-09-01  EH     CRBC1    --   AM_ALL_VEH, AM_GROUP_BIT added,   *
*                                            am_nm_config modified             *
*     1.4     93-11-18  EH     CRBC1    --   multicast protocol !!!            *
*     3.1     94-03-08  MSch   BATC     --   Version fixed to 3.1              *
*     4.0     94-08-03  EH     CRBC1    --   AM_ADDRESS statt AM_NET_ADDR      *
*     4.1     95-01-30  FL     BATL     --   Defintions of direct mapping and  *
*                                            indirect mapping over the the jump*
*                                            table inserted. To choose this two*
*                                            cases, the compiler switch (O_ICM)*
*                                            (incirect call mechanism) must be *
*                                            set or unset.                     *
*     4.2     95-02-28  FL     BATL     __   am_v_get_status changed in        *
*                                            am_m_v_get_status because of the  *
*                                            additional parameter traffic      *
*                                            store id.                         *
*     4.3     95-08-03  Fl      BATL     --  inserted MESSENGER_ID             *
*     4.4     95-09-18  Fl      BATL     __  inserted am_read_messenger_status *
*                                            for Network Management (LME)      *
*     4.5     95-09-18  Fl      BATL     --  inserted DEFINES, TYPES and       *
*                                            FUNCTIONS for station directory   *
*                                            access                            *
*     4.6     95-09-18  Fl      BATL     --  am_disp_show_busses inserted      *
*     4.7     95-10-09  FL      BATL     --  ins. am_write_messenger_control   *
*     4.8     95-15-11  FL      BATL     --  AM_ST_PHYA -> LINK_ADDRESS        *
*                                            AM_ST_INFO in new AM_ST_ENTRY     *
*                                            moved                             *
********************************************************************************
*/

#ifndef     AM_SYS_H
#define     AM_SYS_H

#if defined (O_ICM) && ! defined (O_USE_VARIABLE)
  #if defined (O_EKR)|| defined (O_PC)          /* target specific */
    #include "bc_sys.h"
  #else
    #include "vcos_sys.h"
  #endif
#endif

/*
*****************************************
* basic constants, types and functions  *
*****************************************
*/

typedef unsigned long  CARDINAL32;

/*
*****************************************
* global constants                      *
*****************************************
*/

/* The link layer function lm_v_get_status has get one parameter more (traffic store id) */

#if defined (O_ICM)
#define am_v_get_status(a,b,c) am_m_v_get_status(0,a,b,c)
#endif


/* maximal number of simultaneous send connections */

#define AM_MAX_SEND_CONN                2

#define am_buffer_free( addr, size)     pi_free(addr)

#define AM_MAX_CREDIT     7      /* maximal credit for message transmission */

#define AM_ROUTER_FCT  (unsigned char)251
        /* 251 decimal, this function is always
           located on the gateway */

#define AM_SERVICE_FCT (unsigned char)253
        /* 253 decimal, service replier function,
           this is the only replier function which
        may use physical addressing */

#define AM_MANAGER_FCT (unsigned char)254
        /* 254 decimal, service caller function,
           this is the only caller function which
           may use physical addressing */

#define AM_UNKNOWN_DEVICE (unsigned char) 255

#define AM_SAME_DEVICE    (unsigned char) 0
        /* my generic device address, myself for every device */

#define AM_PHYS_ADDR      (unsigned char) 0x80
        /* physical addressing bit in vehicles */

#define AM_SAME_VEHICLE   (unsigned char) 0
        /* my generic vehicle, myself for every vehicle */

#define AM_GROUP_BIT      (unsigned char) 0x40
        /* vehicles with bit 6 set are groups, multicast */

#define AM_ALL_VEH        (unsigned char) 0xFF
        /* this vehicle group contains all vehicles, broadcast */

#define AM_ANY_TC         (unsigned char) 0
        /* pass-all value for topo count, is always valid */

#define AM_MAX_TC         (unsigned char) 0x7F
        /* maximal value for 7-bit topo count */

/* error code definitions: -------------------------------------------- */
#define AM_MAX_ERR   31

/* The following error codes are defined for the type 'AM_RESULT'.
   Application specific error codes must be higher than 'AM_MAX_ERR'. */
#define AM_OK                    0
#define AM_FAILURE               1
#define AM_BUS_ERR               2
#define AM_REM_CONN_OVF          3
#define AM_CONN_TMO_ERR          4
#define AM_SEND_TMO_ERR          5
#define AM_REPLY_TMO_ERR         6
#define AM_ALIVE_TMO_ERR         7
#define AM_NO_LOC_MEM_ERR        8
#define AM_NO_REM_MEM_ERR        9
#define AM_REM_CANC_ERR         10
#define AM_ALREADY_USED         11
#define AM_ADDR_FMT_ERR         12
#define AM_NO_REPLY_EXP_ERR     13
#define AM_NR_OF_CALLS_OVF      14
#define AM_REPLY_LEN_OVF        15
#define AM_DUPL_LINK_ERR        16
#define AM_MY_DEV_UNKNOWN_ERR   17
#define AM_NO_READY_INST_ERR    18
#define AM_NR_OF_INST_OVF       19
#define AM_CALL_LEN_OVF         20
#define AM_UNKNOWN_DEST_ERR     21
#define AM_INAUG_ERR            22
#define AM_TRY_LATER_ERR        23
#define AM_DEST_NOT_REG_ERR     24
#define AM_GW_DEST_NOT_REG_ERR  25  
#define AM_GW_SRC_NOT_REG_ERR   26
#define AM_REPEAT_TMO_ERR       27


  /* constants for am_v_get_status */

#define AM_NO_RESET             0x0000
#define AM_RECV_ACTIVE          0x0001
#define AM_SEND_ACTIVE          0x0002
#define AM_RECV_OVF             0x0004
#define AM_IRQ_WITHOUT_MESSAGE	0x0008
#define AM_INDICATE_MESSAGE     0x0010

  /* some special bus numbers for the stadi directory */

#define     AM_UNKNOWN_BUS_NR   255
#define     AM_MY_VB_NR         254   /* my own VB if there is just 1 VB */
#define     AM_TB_NR              1   /* this is always the bus_nr for TB */                                                                            
#define     AM_MAX_BUSSES         3   /* maximal number of busses/station */


/*
*************************************
* types                             *
*************************************
*/

typedef unsigned char    AM_RESULT;


typedef struct {
unsigned char        function;
unsigned char        device;
} AM_DIR_ENTRY;


 /* Network address format.
    'vehicle' includes the 'AM_PHYS_ADDR' bit if physical addressing mode
    is used, the value 'AM_SAME_VEHICLE' always means the own vehicle.
    'fct_or_sta' is either a logical function or a station depending on the
    addressing mode indicated in 'vehicle'.
    'next_sta' is the station address of the next hop towards the entity
    represented by the other fields.
    'tc' is the topo count which specifies the time interval during which
    'vehicle' is valid.  
*/
typedef struct {
unsigned char     vehicle;        /* physical or logical addressing */
unsigned char     fct_or_sta;     /* station if phys., function if log. */
unsigned char     next_sta;       /* next station used on the way */
unsigned char     tc;             /* topo count for which vehicle is valid */
} AM_ADDRESS;

/* some special types for the stadi directory */

typedef unsigned short LINK_ADDRESS;
/* 16 bit link address consists of a concatenation of the bus_id and
   station_address */

typedef struct {
  LINK_ADDRESS   link_address;
  unsigned char  final_station;
  unsigned char  next_station;
  unsigned short bus_id;
} AM_ST_ENTRY;

/****************************************************
*  special types for LME (Layer Management Entity)  *
****************************************************/

/* Structure contains information about the status of the messenger */

struct STR_AM_RD_MSNGR_STATUS
{
  unsigned long messages_sent;
  unsigned long messages_received;
  unsigned long packets_sent;
  unsigned long packet_retries;
  unsigned long multicast_retries;
  unsigned char messenger_id[32];
  unsigned char send_time_out;
  unsigned char ack_time_out;
  unsigned char alive_time_out;
  unsigned char credit;
  unsigned char packet_size;
  unsigned char instances;
  unsigned char multicast_window;
};

struct STR_AM_WT_MSNGR_CTRL
{
  unsigned char send_time_out;
  unsigned char ack_time_out;
  unsigned char alive_time_out;
  unsigned char credit;
  unsigned char packet_size;
  unsigned char packet_lifetime;
  unsigned char multicast_window;
};

 /* A function of type AM_CALL_CONF is called at the caller site after recep-
    tion of an expected reply message or an error and signals call completion.
    If any error occurred then 'status' is set !== 'AM_OK', otherwise
    'status' returns the value which has been specified for the reply
    by the replier function.
    'in_msg_addr' returns the address which was passed with 'am_call_requ'
    or the address of a buffer which was allocated by the messenger if any.
    ( may be NULL, i.e. the messenger has not allocated a buffer! )
    and 'in_msg_size' is set to 0 in case of an error or if just a status
    ( and no user data ) has been specified for the reply. */
typedef void ( * AM_CALL_CONF )( /* caller_fct */ unsigned char,
                                 /* ext_ref */ void *,
                                 /* replier */ const AM_ADDRESS *,
                                 /* in_msg_addr - may be NULL */ void *,
                                 /* in_msg_size */ CARDINAL32,
                                 /* status */ AM_RESULT );
                                                                 
 /* A function of type AM_INVOKE_CONF is called at the caller site     
    after transmission of a call message or an error and signals       
    completion of an invoke request (call without reply message).      
    If any error occurred then 'status' is set !== 'AM_OK'. */
typedef void ( * AM_INVOKE_CONF )( /* caller_fct */ unsigned char, 
                                   /* ext_ref */ void *,
                                   /* replier */ const AM_ADDRESS *,
                                   /* status */ AM_RESULT );       
                                                                    
typedef enum                                                        
{                                                                   
  AM_RS_BOUND=0, AM_RS_UNBOUND, AM_RS_UNBINDING,                    
  AM_RS_READY, AM_RS_RECEIVING, AM_RS_RPI_INEXEC, AM_RS_RPC_INEXEC, 
  AM_RS_SENDING, AM_RS_CLOSING                                      
} AM_REPLIER_STATES;                                                
                                                                    
 /* A function of this type is called at the replier site after reception
    of an incoming call message for which there is a ready instance. 
    It signals receive completion. */
typedef void ( * AM_RCV_CONF )( /* replier_fct */ unsigned char,
                                /* caller */ const AM_ADDRESS *,
                                /* in_msg_addr */ void *,
                                /* in_msg_size */ CARDINAL32,
                                /* ext_ref */ void *);
  
 /* A function of this type is called at the replier site after trans-
    mission of a reply message and signals reply completion. */
typedef void ( * AM_REPLY_CONF )( /* replier_fct */ unsigned char,
                                  /* ext_ref */ void * );


/*****************************************************************************

                                 Prototypes

*****************************************************************************/

void  _am_version_read       (short *, short *, short *, short *);
void  _am_init               (void);
void  _am_nm_config
        (unsigned char, unsigned short,   
         unsigned short, AM_RESULT *);    
void  _am_announce_device
       (unsigned int, unsigned int, unsigned int, unsigned short);
void  _am_insert_dir_entries (AM_DIR_ENTRY * , unsigned int);
void  _am_remove_dir_entries (AM_DIR_ENTRY * , unsigned int);
void  _am_get_dir_entry      (unsigned char, unsigned char *);
void  _am_clear_dir          (void);
void  _am_send_requ                                                  /* @JDP */
      (unsigned char, const AM_ADDRESS *, void *, CARDINAL32,         /* @JDP */
       void *, CARDINAL32, unsigned int, AM_CALL_CONF, AM_INVOKE_CONF, /* @JDP */
       void * );                                                       /* @JDP */
/*   @JDP
void  _am_call_requ
    (unsigned char, const AM_ADDRESS *, void *, CARDINAL32,
    void *, CARDINAL32, unsigned int, AM_CALL_CONF, void * );
@JDP*/
void  _am_call_cancel
      (unsigned char, const AM_ADDRESS *, AM_RESULT *);
void  _am_bind_replier
      (unsigned char, AM_RCV_CONF, AM_REPLY_CONF, AM_RESULT *);
void  _am_unbind_replier     (unsigned char replier_fct);
void  _am_rcv_requ
      (unsigned char, void *, CARDINAL32, void *, AM_RESULT *);
AM_REPLIER_STATES _am_get_replier_status (unsigned char, void *);  
void  _am_reply_requ
      (unsigned char, void *, CARDINAL32, void *, AM_RESULT);
void  _am_rcv_cancel
      (unsigned char, void *, AM_RESULT *);
AM_RESULT _am_set_current_tc (unsigned char);   /* EH 14-Jul-1994 */

/* the messenger is normally called over the indirect jump table, but it
   is also possible to start it direct from the OS or if necessary from the
   applications */

/* the messenger must be started as a cyclic task which has lower priority
   than the timeout process. */

void tm_messenger (void);

/*    special functions for stadi management.                           */
/*    The station directory is necessary if there is more than 1 VB     */
/*    in a vehicle, or if the physical address is not identical with    */
/*    the station address. A station address is unique within a         */
/*    vehicle.                                                          */

AM_RESULT _am_st_write (AM_ST_ENTRY *, unsigned int);
   /* Writes (inserts) a number of entries into the STADI.
   Returns AM_OK on success, or AM_FAILURE if some entries of the list
   cannot be written because they do not pass the checks.
   */

void _am_st_read (AM_ST_ENTRY *, unsigned int);
   /* Reads a number of entries from the STADI. */


void _am_disp_show_busses (unsigned char *,	unsigned char *);
   /* Returns a list of connected bus numbers. nr_of_busses specifies the
   number of valid entries in bus_nr list and is limited to AM_MAX_BUSSES.
   The TB is always the last entry in the list if it is connected.
   This information is provided by the bus dispatcher module.
   */

/****************************************************************************
*                 special functions for network management                  *
*                                                                           *
*  !! These function are reserved for the Network Management and            *
*     it is not allowed to use the functions for Message Data Applications  *
*****************************************************************************/

int _am_read_messenger_status(struct STR_AM_RD_MSNGR_STATUS *p_stat);
  /* Retrieve the status of the messenger and its statistic counters */

int _am_write_messenger_control(struct STR_AM_WT_MSNGR_CTRL *p_stat);
   /* Set parameters in the messenger */

/******************************************************************************

      Structure Declaration for Indirect Call Table

*******************************************************************************/

typedef struct
{
    unsigned char amd_version[4];
    void  ( *pam_init)        (void);
    void  ( *pam_nm_config)
             (unsigned char, unsigned short,   
              unsigned short, AM_RESULT *);    
    void  ( *pam_announce_device)
          (unsigned int, unsigned int, unsigned int, unsigned short);
    void  ( *pam_insert_dir_entries) (AM_DIR_ENTRY * , unsigned int);
    void  ( *pam_remove_dir_entries) (AM_DIR_ENTRY * , unsigned int);
    void  ( *pam_get_dir_entry)      (unsigned char, unsigned char *);
    void  ( *pam_clear_dir)          (void);
    void  ( *pam_send_requ)                                        /* @JDP */
          (unsigned char, const AM_ADDRESS *, void *, CARDINAL32,  /* @JDP */
          void *, CARDINAL32, unsigned int,                        /* @JDP */
          AM_CALL_CONF, AM_INVOKE_CONF, void * );              /* @JDP */
/* @JDP
    void  ( *pam_call_requ)
          (unsigned char, const AM_ADDRESS *, void *, CARDINAL32,
          void *, CARDINAL32, unsigned int, AM_CALL_CONF, void * );
@JDP */
    void  ( *pam_call_cancel)
          (unsigned char, const AM_ADDRESS *, AM_RESULT *);
    void  ( *pam_bind_replier)
          (unsigned char, AM_RCV_CONF, AM_REPLY_CONF, AM_RESULT *);
    void  ( *pam_unbind_replier)     (unsigned char replier_fct);
    void  ( *pam_rcv_requ)
          (unsigned char, void *, CARDINAL32, void *, AM_RESULT *);
    void  ( *pam_reply_requ)
          (unsigned char, void *, CARDINAL32, void *, AM_RESULT);
    void  ( *pam_rcv_cancel)
          (unsigned char, void *, AM_RESULT *);
    void  ( *pam_m_v_get_status)
          (unsigned short, unsigned short, unsigned short, unsigned short *);
    void  ( *pam_tm_messenger) (void);
    AM_REPLIER_STATES ( *pam_get_replier_status) (unsigned char, void *);
    AM_RESULT ( *pam_set_current_tc) (unsigned char);

    AM_RESULT ( *pam_st_write ) (const AM_ST_ENTRY entries[],
                                 unsigned int nr_of_entries);
    void ( *pam_st_read) (AM_ST_ENTRY entries[], unsigned int nr_of_entries);
    void ( *pam_disp_show_busses) (unsigned char *nr_of_busses,
                                   unsigned char bus_nr[AM_MAX_BUSSES]);
    int  ( *pam_read_messenger_status) (struct STR_AM_RD_MSNGR_STATUS *);
    int  ( *pam_write_messenger_control) (struct STR_AM_WT_MSNGR_CTRL *);
} TYPE_AM_IND_CALL;

/* !!! indirect jump table not yet supported for multicast protocol! */
void am_multicast_requ (unsigned char caller_fct,
                        const AM_ADDRESS *replier,
                        void *out_msg_addr,
                        unsigned int out_msg_size,
                        AM_INVOKE_CONF confirm,
                        void *ext_ref);
void am_multicast_init (void);
/* !!! */



/*  !!!! Switch O_ICM must be set if calls over the jump table will be used */

#if defined (O_ICM)

extern const TYPE_AM_IND_CALL am_ind_call_table;

#if defined (O_USE_VARIABLE) || defined (AM_USE_VARIABLE)
  extern  TYPE_AM_IND_CALL*         am_pc_ptr_to_table;
  #define PAM_IND_CALL              am_pc_ptr_to_table
#else
  #define PAM_IND_CALL              (*(TYPE_AM_IND_CALL * *) AM_ADR_TABLE)
#endif

#define am_init                    (* (PAM_IND_CALL->pam_init))
#define am_nm_config               (* (PAM_IND_CALL->pam_nm_config))
#define am_announce_device         (* (PAM_IND_CALL->pam_announce_device))
#define am_insert_dir_entries      (* (PAM_IND_CALL->pam_insert_dir_entries))
#define am_remove_dir_entries      (* (PAM_IND_CALL->pam_remove_dir_entries))
#define am_get_dir_entry           (* (PAM_IND_CALL->pam_get_dir_entry))
#define am_clear_dir               (* (PAM_IND_CALL->pam_clear_dir))
#define am_call_requ(fct, r, out_a, out_s, in_a, in_s, tmo, cc, ref) /* @JDP */ \
    (* (PAM_IND_CALL->pam_send_requ))                                /* @JDP */ \
      (fct, r, out_a, out_s, in_a, in_s, tmo, cc, (AM_INVOKE_CONF) NULL, ref) /* @JDP */
/* @JDP
#define am_call_requ               (* (PAM_IND_CALL->pam_call_requ))
@JDP */
#define am_invoke_requ(fct, r, out_a, out_s, ic, ref) /* @JDP */ \
    (* (PAM_IND_CALL->pam_send_requ))                 /* @JDP */ \
      (fct, r, out_a, out_s, NULL, 0, 0, (AM_CALL_CONF) NULL, ic, ref) /* @JDP */
#define am_call_cancel             (* (PAM_IND_CALL->pam_call_cancel))
#define am_bind_replier            (* (PAM_IND_CALL->pam_bind_replier))
#define am_unbind_replier          (* (PAM_IND_CALL->pam_unbind_replier))
#define am_rcv_requ                (* (PAM_IND_CALL->pam_rcv_requ))
#define am_get_replier_status      (* (PAM_IND_CALL->pam_get_replier_status))
#define am_reply_requ              (* (PAM_IND_CALL->pam_reply_requ))
#define am_rcv_cancel	             (* (PAM_IND_CALL->pam_rcv_cancel))
#define am_m_v_get_status          (* (PAM_IND_CALL->pam_m_v_get_status))
#define am_tm_messenger            (* (PAM_IND_CALL->pam_tm_messenger))
#define am_set_current_tc          (* (PAM_IND_CALL->pam_set_current_tc))
#define am_st_write                (* (PAM_IND_CALL->pam_st_write))
#define am_st_read                 (* (PAM_IND_CALL->pam_st_read))
#define am_disp_show_busses        (* (PAM_IND_CALL->pam_disp_show_busses))
#define am_read_messenger_status   (* (PAM_IND_CALL->pam_read_messenger_status))
#define am_write_messenger_control (*(PAM_IND_CALL->pam_write_messenger_control))

/*
*************************************
* connect AM                        *
*************************************
*/

#define am_connect() PAM_IND_CALL = (void *) &am_ind_call_table

#else /* Direct mapping of the functions are used */

/* In this case, if the user uses am_connect() the function does nothing, because
   no indirect jump_table is available */

#define am_connect()      /* */

#define am_version_read            _am_version_read
#define am_init                    _am_init
#define am_announce_device         _am_announce_device
#define am_nm_config               _am_nm_config
#define am_insert_dir_entries      _am_insert_dir_entries
#define am_remove_dir_entries      _am_remove_dir_entries
#define am_get_dir_entry           _am_get_dir_entry
#define am_clear_dir               _am_clear_dir
#define am_send_requ               _am_send_requ
/*@JDP*/#define am_call_requ(fct, r, out_a, out_s, in_a, in_s, tmo, cc, ref) \
       _am_send_requ(fct, r, out_a, out_s, in_a, in_s, tmo, cc, (AM_INVOKE_CONF) NULL, ref) /* @JDP */
/* @JDP
#define am_call_requ               _am_call_requ
@JDP */
#define am_invoke_requ(fct, r, out_a, out_s, ic, ref)  \
       _am_send_requ(fct, r, out_a, out_s, NULL, 0, 0, (AM_CALL_CONF) NULL, ic, ref)
#define am_call_cancel             _am_call_cancel
#define am_bind_replier            _am_bind_replier
#define am_unbind_replier          _am_unbind_replier
#define am_rcv_requ                _am_rcv_requ
#define am_reply_requ              _am_reply_requ
#define am_rcv_cancel	             _am_rcv_cancel
#define am_get_replier_status      _am_get_replier_status
#define am_set_current_tc          _am_set_current_tc
#define am_st_write                _am_st_write
#define am_st_read                 _am_st_read
#define am_disp_show_busses        _am_disp_show_busses
#define am_read_messenger_status   _am_read_messenger_status
#define am_write_messenger_control _am_write_messenger_control

#if defined (O_EKR) || defined (O_PC)
#define am_m_v_get_status          lm_v_get_status
#else
#define am_m_v_get_status          lm_m_v_get_status
#endif
#define am_tm_messenger             tm_messenger
#endif  /* else */

/*
************************************* 
* Version Verification Mechanism    *
*************************************
*/


#define AM_VERSION          4 /* if you update, update also MESSENGER_ID */
#define AM_RELEASE          2 /* if you update, update also MESSENGER_ID */

#define MESSENGER_ID        "RTP_MD-V4.2-15.11.95\0"
#define MESSENGER_ID_SIZE    sizeof(MESSENGER_ID)



#if defined (O_ICM) && !defined (O_USE_VARIABLE)
  #define am_version()    bc_version_verify ( BC_SUBPROJECT_AM, \
                                              AM_VERSION,       \
                                              AM_RELEASE );
#else
  #define am_version()    0
#endif

#endif                                 /* AM_SYS_H */
