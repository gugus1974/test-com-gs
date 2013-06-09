/*
*****************************************************************************
*                                                                           *
*    Property of  :   ABB Verkehrssysteme AG, CH-8050 Z*rich / Switzerland  *
*    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                       *
*                                                                           *
*****************************************************************************
*                                                                           *
*    PROJECT      : IEC TCN                                                 *
*                                                                           *
*    SUBPROJECT   : Link-Layer Message (LM)                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
*    MODULE       : LM_LAYER.H                                              *
*                                                                           *
*    DOCUMENT     :                                                         *
*                                                                           *
*    ABSTRACT     : Functions for link layers message acces.                *
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
*     Version  YY-MM-DD  Name         Dept.    Ref   Comments               *
*    -------  --------  ----------   ------   ----  --------------------    *
*    0.1      91-06-18  E. Nantier   BAEI-2    --   created                 *
*    0.2      91-09-10  E. Nantier   BAEI-2    --   modified for new lm     *
*                                                   name LM and no more LK  *
*    0.3      91-10-18  WHK          BAEI-3    --                           *
*             Prototypes has been modified to conform C.                    *
*    0.4      92-02-14  Mr           BAEI-2    --   modified enable/disable *
*    0.5      92-03-02  E. Hoepli    CRBC1     --   new names lm_t/v_       *
*                                                                           *
*    4.1.0.0  93-08-16  zur Bonsen   BATC      --   Adaption on MVBC        *
*    4.1.0.1  94-05-26  zur Bonsen   BATC      --   char -> unsigned char   *
*    4.2.0.0  94-11-11  zur Bonsen   BATC      --   (no chgs in this file)  *
*    4.3.0.0  95-01-31  zur Bonsen   BATL      --   removed non-ASCII chars *
*    4.3.0.1  95-01-31  zur Bonsen   BATL      --   updated    lm_head.h    *
*    4.3.0.2  95-02-28  zur Bonsen   BATL      --   introduced lm_m_t_      *
*                                                   func. names, see below  *
*    4.3.0.3  95-04-09  zur Bonsen   BATL      --   LM_IEC_PROTOCOL intro-  *
*						    duced.		    *
*    4.3.1.0  95-09-15  zur Bonsen   BATL      --   lm_m_v_get_own_phya and *
*						    lm_m_v_get_src_phya     *
*                                                   have been added.        *
*                                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* Updates in 4.3.0.2:                                                       *
*                                                                           *
* This file contains following function name groups:                        *
*                                                                           *
* lm_v_...        = BAP Link Layer Message Functions to MVB                 *
*                   (Target HW can support only one MVB interface only)     *
*                                                                           *
* lm_m_v_...      = MVBC Link Layer Message Functions to MVB                *
*                   (Multiple MVB I/F supported, identified by 1st param)   *
*                                                                           *
* lm_t_...        = WTB Link Layer Functions as currently provided          *
*                                                                           *
* lm_m_t_...      = WTB Link Layer Functions with additional dummy para-    *
*                   meter to identify interface.  As of now, this para-     *
*                   meter is ignored.  The lm_m_t_... functions will call   *
*                   lm_t_... functions inside LM_LAYER.C.                   *
*                                                                           *
* Following functions do not contain "lm_m_v/t_" but at least "lm_v/t_"     *
* prefixes.  Reason:  These functions need to be called just once and       *
* cover all MVB or WTB interfaces at once.                                  *
*                                                                           *
* If you compile for a BAP-target:                                          *
*                                                                           *
* - Make sure you use Link Layer I/F software for BAP, not MVBC.            *
* - Set compiler switch O_BAP                                               *
* - lm_m_v_... functions will be created in LM_LAYER.C.                     *
*                                                                           *
*                                                                           *
* If you compile for an MVBC-target:                                        *
*                                                                           *
* - Make sure you use Link Layer I/F software for MVBC, not BAP.            *
* - Make sure compiler switch O_BAP is not set                              *
* - lm_m_v_... functions are directly available in MVBC LM software         *
*                                                                           *
* If you compile for a WTB-target:                                          *
*                                                                           *
* - The lm_m_t_... functions will be created in LM_LAYER.C where these      *
*   ones call up lm_t_... functions directly.                               *
*                                                                           *
*                                                                           *
* !!! ATTENTION !!!                                                         *
* -----------------                                                         *
*                                                                           *
* The upper layer message data software (network layer up to application    *
* layer must use lm_m_v_... and lm_m_t_... functions, and no longer         *
* lm_v_... and lm_t_... functions.  Reason:  Support for multiple MVB's     *
* and, at a much later time, multiple WTB's.                                *
*                                                                           *
*****************************************************************************
*/

#ifndef LM_LAYER_H
#define LM_LAYER_H

#ifndef MD_OK
        #include "md_layer.h"
#endif

#include "lc_sys.h"      /* Mr : for phy_get_address */


#define  LM_SEND_WITH_HAMSTER    0
#define  LM_SEND_NO_HAMSTER      1

#define  LM_OLD_PROTOCOL         0x00           /* do not change value! */
#define  LM_IEC_PROTOCOL         0x80           /* do not change value! */

/* !!! Attention  Do not use LM_OLD_PROTOCOL */

/*
*****************************************************************************
* MVBC Message Queue Data Structure                                         *
*****************************************************************************
*/

    /* Necessary support to install message queues in MVBC Traffic Store */
    /* Structure is ignored for BAP-based target systems 		 */

    struct LM_STR_FRAGMENTS
    {
        void                    *p_tm_fragment;
        struct LM_STR_FRAGMENTS *p_next;
        unsigned short           size;
    };


/*
*****************************************************************************
* Global Initialization functions - each function to be called only once    *
*****************************************************************************
*/

    void    lm_v_system_init( void ) ;
    void    lm_t_system_init( void ) ;


/*
*****************************************************************************
* Dedicated MVBC Function (Dummy function called for BAP subsystems)        *
*****************************************************************************
*/


/* New LM function for MVBC-based targets: lm_m_v_config() */


    #ifndef O_BAP

        short   lm_m_v_config( unsigned short,
                               struct LM_STR_FRAGMENTS *,
                               struct LM_STR_FRAGMENTS *,
                               struct LM_STR_FRAGMENTS * );

        #define lm_v_config(a,b,c) lm_m_v_config(0,a,b,c)

    #else

        #define lm_m_v_config( a,b,c,d ) {;}

        #define lm_v_config( a,b,c ) {;}

    #endif

/*
*****************************************************************************
* Functions, calls must be made for every existing MVB/WTB subsystem        *
*****************************************************************************
*/


/* Initialization/Activation */
/* ------------------------- */

    void    lm_m_v_init( unsigned short dummy,
                         void (* receive_confirm)(void),
                         void (* get_pack)(void **, MD_PACKET ** ),
                         void ** ident,
                         void (* put_pack)( MD_PACKET * ),
                         void (* status_indicate)(int ) ) ;

    void    lm_m_t_init( unsigned short dummy,
                         void (* receive_confirm)(void),
                         void (* get_pack)(void **, MD_PACKET ** ),
                         void ** ident,
                         void (* put_pack)( MD_PACKET * ),
                         void (* status_indicate)(int ) ) ;



/* Get Bus Status */
/* -------------- */

    void     lm_m_v_get_status(      unsigned short,
                                     unsigned short,
                                     unsigned short,
                                     unsigned short * ) ;

    void     lm_m_t_get_status(      unsigned short,
                                     unsigned short,
                                     unsigned short,
                                     unsigned short * ) ;



/* Send Request */
/* ------------ */

    void     lm_m_v_send_request(    unsigned short,
                                     unsigned char,
                                     unsigned char,
                                     MD_PACKET *,
                                     int * ) ;

    void     lm_m_t_send_request(    unsigned short,
                                     unsigned char,
                                     unsigned char,
                                     MD_PACKET *,
                                     int * ) ;



/* Send Queue Flush */
/* ---------------- */

    void     lm_m_v_send_queue_flush(unsigned short ) ;

    void     lm_m_t_send_queue_flush(unsigned short ) ;



/* Receive Poll */
/* ------------ */

    void     lm_m_v_receive_poll(    unsigned short,
                                     unsigned char *,
                                     unsigned char *,
                                     MD_PACKET **,
                                     int * ) ;

    void     lm_m_t_receive_poll(    unsigned short,
                                     unsigned char *,
                                     unsigned char *,
                                     MD_PACKET **,
                                     int * ) ;


/* Get address of own device */
/* ------------------------- */

    unsigned short lm_m_v_get_own_phya( unsigned short,
                                        int * );

    unsigned short lm_m_t_get_own_phya( unsigned short,
                                        int * );


/* Get phyical address of source device */
/* ------------------------------------ */

    unsigned short lm_m_v_get_src_phya( MD_PACKET * ,
                                        int * );

    unsigned short lm_m_t_get_src_phya( MD_PACKET * ,
                                        int * );



/* Miscellaneous */
/* ------------- */

#define phy_get_address lc_get_device_address




#endif /* #ifndef MD_LAYER_H */
