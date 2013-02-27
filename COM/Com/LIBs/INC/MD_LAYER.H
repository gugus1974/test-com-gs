/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : Multifunction Vehicle Bus                               *
*    SUBPROJECT   : Message data layer for Bus communication	            *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : MD_LAYER.H                                              *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : Functions for the global message data communication     *
*                                                                           *
*    REMARKS      :                                                         *
*                                                                           *
*    DEPENDENCIES :                                                         *
*                                                                           *
*    ACCEPTED     :                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
*    HISTORY:                                                               *
*                                                                           *
*    Version  YY-MM-DD  Name         Dept.    Ref   Comments                *
*    -------  --------  ----------   ------   ----  --------------------    *
*    0.1      91-06-24  E. Nantier   BAEI-2    --   created                 *
*    0.2      91-09-10  E. Nantier   BAEI-2    --   modified for new struct *
*                                                   in norme document       *
*    0.3      91-10-07  E. Hoepli    CRBC-1    --   extended                *
*    0.4      92-03-17  E. Hoepli    CRBC-1    --   modified for gateway    *
*    0.5      93-12-08  R. Muenger   BATC      --   redefined link struct   *
*    0.6      94-07-18  E. Hoepli    CRBC-1    --   md_my_tc added          *
*    0.7      94-01-31  G. z. Bonsen BATL      --   verification at LM and  *
*						    removed non-ASCII chars *
*    1.0      95-03-23  R. Flum      BATL      --   modified struct         *
*                                                   TM_LINK_ID.             *
*                                                   Struct MD_NET_PACKET    *
*                                                   changed, because of     *
*                                                   alignment problems      *
*****************************************************************************
*/
#ifndef MD_LAYER_H
    #define MD_LAYER_H


#include "basetype.h"

/*
*****************************************
*         defines                       *
*****************************************
*/

#define MD_OK               0
#define MD_FAILURE          1
#define MD_INAUGURATION     2
#define MD_READY            0
#define MD_REJECT           1
#define MD_QUEUEVOID		1
#define MD_PENDING         	2
#define MD_SENT         	3
#define MD_FLUSHED         	4

#define MD_HIGH_PRIO		1

#ifdef WAG
  #define MD_DATA_SIZE      123
#else
  #define MD_DATA_SIZE		22
#endif

#define MD_NO_RESET             0x0000
#define MD_RECV_ACTIVE          0x0001
#define MD_SEND_ACTIVE          0x0002
#define MD_RECV_OVF             0x0004
#define MD_IRQ_WITHOUT_MESSAGE	0x0008
#define MD_INDICATE_MESSAGE     0x0010


/*
*****************************************
*         Types                         *
*****************************************
*/

#if defined (O_960)
    #pragma noalign str_port_packet
    #pragma noalign str_link_packet
    #pragma noalign str_mvb_link_packet
    #pragma noalign str_wtb_link_packet
    #pragma noalign str_unn_version
#endif


typedef struct str_mvb_link_packet {
    unsigned char   mode_hdest;  /* mode (1) and upper 4 dest bits */
    unsigned char   dest;        /* destination address            */
    unsigned char   prot_hsrc;   /* prot and upper 4 source bits   */
    unsigned char   src;         /* source address                 */
} MD_MVB_LINK_PACKET ;

typedef struct str_wtb_link_packet {
        unsigned char   reserved;
	unsigned char   dest;
	unsigned char   llc;
	unsigned char   src;
} MD_WTB_LINK_PACKET;

typedef struct str_link_packet {
        union unn_version {
                MD_MVB_LINK_PACKET      mvb;
                MD_WTB_LINK_PACKET      wtb; 
        } version;
	unsigned char   size;
	unsigned char   final_vehicle;
	unsigned char   final_fct_or_dev;
	unsigned char   origin_vehicle;
	unsigned char   origin_fct_or_dev;
	SET8            mtc;
	unsigned char	data[ MD_DATA_SIZE ] ;
} MD_LINK_PACKET;


typedef struct str_port_packet {
	MD_LINK_PACKET	lk ;
	} MD_PORT_PACKET ;

typedef struct {
    void           *next ;
    void           *identifier ;
    unsigned char   control ;
    unsigned char   status ;
	MD_PORT_PACKET	port ;
	} MD_PACKET;

/* types for common list handling: */
typedef struct MD_LIST_HEADER {
struct MD_LIST_HEADER	* next;
struct MD_LIST_HEADER	* prev;
} MD_LIST_HEADER;
/* MD_LIST_HEADER - header of a list element, reserved for use by this 
   module only. The root of a list must be a pointer to this structure
   and each list element must have this header as its first field. */





typedef struct str_link_id{
    unsigned char   my_fct_or_dev;
    unsigned char   vehicle;
    /* bit 7 indicates physical addressing */
    unsigned char   fct_or_dev;
    unsigned char   link_dev;
    /* 'link_dev' is the device address on the own vehicle. It is the gateway
    for remote functions. */
    }TM_LINK_ID;
/*
*****************************************
*         Functions                     *
*****************************************
*/

/* functions for common list handling: */
void md_init_ele ( MD_LIST_HEADER * ele );
/* initializes the header of an element, does not allocate memory */

void md_put_ele ( MD_LIST_HEADER * root, MD_LIST_HEADER * ele );
/* inserts at bottom. 
   no action if the element is NULL or already connected in a list. */

void md_get_ele ( MD_LIST_HEADER * root, MD_LIST_HEADER * * ele );
/* removes from top. Returns 'ele' = NULL if the list is empty. */

void md_cancel_ele ( MD_LIST_HEADER * ele );
/* removes from any position of any list.
   no action if the element is NULL or not connected in a list. */

void md_init_list( MD_LIST_HEADER * * root );
/* initializes a list with no elements */

extern unsigned char md_my_vehicle;
extern unsigned char md_my_device;
extern unsigned char md_my_tc;

#define md_get_my_device()           md_my_device
/* returns my unique device address or 'AM_UNKNOWN_DEVICE' if device is
   unknown */

#define md_get_my_vehicle()          md_my_vehicle
/* returns my unique vehicle address or 'AM_SAME_VEHICLE' if vehicle is
   unknown */

#define md_set_my_device(my_dev)     { md_my_device = my_dev; }
/* Defines my unique device address. Must be called during initialization. */

#define md_set_my_vehicle(my_veh)    { md_my_vehicle = my_veh; }
/* Defines my unique vehicle address. Must be called during initialization. */

#define md_get_my_tc()               md_my_tc
/* returns my topo count or 'AM_ANY_TC' if topo count is unknown */
 
#define md_get_my_veh_and_tc( my_veh, my_tc) \
{ pi_disable(); *(my_veh) = md_my_vehicle; *(my_tc) = md_my_tc; pi_enable();}
/* reads my vehicle and my topo count in a consistent way. */

#define md_set_my_veh_and_tc( my_veh, my_tc) \
{ pi_disable(); md_my_vehicle = (my_veh); md_my_tc = (my_tc); pi_enable();}
/* Assigns my vehicle and my topo count in a consistent way. */

#endif
