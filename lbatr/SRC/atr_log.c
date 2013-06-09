/********************************************************************/
/* 2004/nov/26,ven 08:40                \SWISV\LBATR\SRC\atr_log.c  */
/********************************************************************/
/*                  Logging functions                               */
/********************************************************************/
/*
    $Date: 2004/11/26 14:16:43 $          $Revision: 1.4 $
    $Author: mungiguerrav $
*/


/*==========================================================================*/
/* Includes */

#include "atr_log.h"

#include <stdio.h>

#include "atr_hw.h"
#include "pi_sys.h"
#include "c_utils.h"

#if defined(ATR_WTB) && !defined(UMS)
extern void write_serv (const char *s);
#endif


/*==========================================================================*/
/* Variables */

static const CMessageDef    m_event_type[]   =
{
    NSE_EVENT_TYPE_ERROR,    "ERROR",
    NSE_EVENT_TYPE_WARNINGS, "WARNING",
    NSE_EVENT_TYPE_SPECIAL,  "SPECIAL",
    NSE_EVENT_TYPE_UNKNOWN,  "UNKNOWN",
    NSE_EVENT_TYPE_LOST,     "LOST",
    0,                       0
};

static const CMessageDef    m_event_action[]   =
{
    NSE_ACTION_CONTINUE, "CONTINUE",
    NSE_ACTION_SUSPEND,  "SUSPEND",
    NSE_ACTION_HALT,     "HALT",
    NSE_ACTION_REBOOT,   "REBOOT",
    0,                   0
};


/*==========================================================================*/
/* Implementation */

/*--------------------------------------------------------------------------*/
void dump_ticks(unsigned long ticks)
{
    int h =  ticks / (60*60*1000L / PI_TICK);
    int m = (ticks / (   60*1000L / PI_TICK)) % 60;
    int s = (ticks / (      1000L / PI_TICK)) % 60;
    int c = (ticks / (        10L / PI_TICK)) % 100;

    printf("%d:%02d:%02d.%02d", h, m, s, c);
}


/*--------------------------------------------------------------------------*/
void dump_nse_record(char *module_name, UINT16 line_number, int event_type, int event_action,
                     UINT8 list_length, UINT8 *p_list)
{
    short   i;
    char    ts[12];
    char    is_ascii = FALSE;

#if defined(ATR_WTB) && !defined(UMS)
    if (event_type == NSE_EVENT_TYPE_SPECIAL) write_serv((char*)p_list);
    else {
#endif

        if (event_type == NSE_EVENT_TYPE_SPECIAL || (list_length && !p_list[list_length-1]))
        {
            for (i = 0; i < list_length-1; i++)
            {
                if (p_list[i] < ' ' || p_list[i] > '~')
                {
                    break;
                }
            }

            if (i == list_length-1)
            {
                is_ascii = TRUE;
                printf("\n>>> (");
                dump_ticks(pi_ticks_elapsed());
                printf(") '%s'", p_list);
            }
        }

        if (!is_ascii)
        {
            printf("\n*** NSE_RECORD: file='%s', line=%u, type=%s, action=%s",
                   module_name, line_number,
                   c_fetch_message(m_event_type, event_type, ts, 0),
                   c_fetch_message(m_event_action, event_action, ts, 0));

            if (list_length)
            {
                printf(", data=0x");
#ifdef O_LE
                for (i = list_length - 1; i >= 0; i--)
                {
                    printf("%02x", (short)p_list[i]);
                }
#else
                for (i = 0; i < list_length; i++)
                {
                    printf("%02x", (short)p_list[i]);
                }
#endif
            }
        }

        putchar('\n');
#if defined(ATR_WTB) && !defined(UMS)
    }
#endif
}


/*--------------------------------------------------------------------------*/
int atr_record(char *module_name, UINT16 line_number, int event_type, int event_action,
               UINT8 list_length, UINT8 *p_list, unsigned long ticks, char *text)
{
    if (text && *text)
    {
        printf("\n>>> (");
        dump_ticks(ticks);
        printf(") %s @ %s:%u\n", text, module_name, line_number);
    }
    else
    {
        dump_nse_record(module_name, line_number, event_type, event_action, list_length, p_list);
    }

    if      (event_action == NSE_ACTION_HALT)
    {
        hw_halt();
    }
    else if (event_action == NSE_ACTION_REBOOT)
    {
        hw_reset();
    }
    else if (event_action == NSE_ACTION_SUSPEND)
    {
        pi_task_exit();
    }

    return NSE_ERR_OK;
}


/* ****************************************************************************
    $Log: atr_log.c,v $
    Revision 1.4  2004/11/26 14:16:43  mungiguerrav
    2004/nov/26,ven 08:40           Napoli      Mungi
    - Si aggiungono keyword CVS e storia invertita
    - Si sostituiscono TAB con 4 spazi
    - Si applicano le regole di codifica sulle parentesi


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.3        2004/08/31 16:13:25     mungiguerrav
    2004/ago/31,mar 18:10        Napoli          Mungi\Busco
    - Si ripristina la versione originale di lbAtr05c, abolendo la deviazione
    Busco del 2002/11/04 non testata sufficientemente

    Revision 1.2        2002/11/04 13:34:53     buscob
01.01   2002/nov/04,lun             Napoli              Busco
        - Si agg. delay tra la stampa nse_record ed il blocco/reset della scheda
        - Si memorizza il reset code

    Revision 1.1        2002/10/22 14:38:47     buscob
    - Inserimento in CVS
01.0n   1998/set/11,ven             Napoli              Carnevale
        - Modifiche non documentate
01.00   1998/mmm/gg                 Napoli              Carnevale
        - Creazione non documentata
*/

