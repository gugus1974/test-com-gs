/********************************************************************/
/* 2004/nov/26,ven 08:50                \SWISV\LBATR\SRC\atr_log.h  */
/********************************************************************/
/*                  Logging functions                               */
/********************************************************************/
/*
    $Date: 2004/11/26 14:17:31 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _ATR_LOG_
#define _ATR_LOG_

/*==========================================================================*/
/* Includes */
#include "pi_sys.h"
#include "recorder.h"


/*==========================================================================*/
/* Macros */

#define atr_log_warning(message, data, data_len) \
    atr_record(__FILE__, __LINE__, NSE_EVENT_TYPE_WARNINGS, NSE_ACTION_CONTINUE, data_len, data, \
               pi_ticks_elapsed(), message)
#define atr_log_error(message, data, data_len) \
    atr_record(__FILE__, __LINE__, NSE_EVENT_TYPE_ERROR, NSE_ACTION_CONTINUE, data_len, data, \
               pi_ticks_elapsed(), message)
#define atr_log_failure(message, data, data_len) \
    atr_record(__FILE__, __LINE__, NSE_EVENT_TYPE_ERROR, NSE_ACTION_HALT, data_len, data, \
               pi_ticks_elapsed(), message)


/*==========================================================================*/
/* Prototypes */

void    dump_ticks(     unsigned long   ticks);

void    dump_nse_record(char            *module_name,
                        UINT16          line_number,
                        int             event_type,
                        int             event_action,
                        UINT8           list_length,
                        UINT8           *p_list);

int     atr_record(     char            *module_name,
                        UINT16          line_number,
                        int             event_type,
                        int             event_action,
                        UINT8           list_length,
                        UINT8           *p_list,
                        unsigned long   ticks,
                        char            *text);

#endif


/* ****************************************************************************
    $Log: atr_log.h,v $
    Revision 1.2  2004/11/26 14:17:31  mungiguerrav
    2004/nov/26,ven 08:50           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2002/10/22 14:38:47     buscob
    - Inserimento in CVS
01.0x   1998/giu/11,gio             Napoli              Carnevale
        - Modifiche non documentate
01.00   1998/mmm/gg                 Napoli              Carnevale
        - Creazione non documentata
*/

