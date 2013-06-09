/*



*/
/* 
IMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM;
: modul    : mac messenger interface                                          :
:                                                                             :
: file     : mmi.h                                                            :
: date     : 07.08.92                                                         :
: reference: TCN norm - REAL TIME PROTOCOLS                                   :
:            link-layer internal interfaces                                   :
: author   : ABB Henschel WAG/NTL 2 Gg                                        :
:                                                                             :
: nderungen : 01.02.94 Kvr/Br  - MD_PACKET2 in MD_PACKET umbenannt           :
:                               - char nach unsigned char                     :
-              11.04.96 Kvr     - PACKET stuctures removed (am_sys.h) included-
-                                 for MD_PACKET                               -
HMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM<
*/
#include "am_sys.h"
/* functions for application at messenger-modul */
void lm_t_system_init (void);
void lm_m_t_init(unsigned short bus_id,
                 void (*rec_confirm)    (void),
                 void (*get_a_packet)   (void** owner, MD_PACKET **a_packet),
                 void **owner,
                 void (*put_a_packet)   (MD_PACKET *a_packet),
                 void (*status_changed) ( /*unsigned short*/ int result)
          );
void lm_m_t_get_status(unsigned short bus_id,
                       unsigned short /*char*/ status_selector,
                       unsigned short /*char*/ status_reset,
                       unsigned short /*char*/ *status_result);
void lm_m_t_send_request( unsigned short bus_id,
                          unsigned char sourcedevice,
                          unsigned char destinationdevice,
                          MD_PACKET *packet_to_send2,
                          /*unsigned short*/ int *result );
void lm_m_t_send_queue_flush( unsigned short bus_id);
void lm_m_t_receive_poll ( unsigned short bus_id,
                           unsigned char  *sourcedevice,
                           unsigned char  *destinationdevice,
                           MD_PACKET         **receive_packet2,
                           /*unsigned short*/   int *result );
/* functions for MAC at MMI-modul at MAC side */ 
void          mac_md_system_init   (void);
void*         md_request_to_send   (unsigned char *destination );
void          md_receive_indicate  (void *receive_frame );
unsigned char md_check_for_data    (void);
void          md_send_confirm      (void);
void          md_status_indicate   (unsigned short result) ;
void          md_free_packets      (void);
