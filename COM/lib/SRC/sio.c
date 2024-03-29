/****************************************************************/
/* 2004/nov/11,gio 15:15                \SWISV\LIB\SRC\sio.c    */
/****************************************************************/
/*          Gestione dello Standard I/O                         */
/****************************************************************/
/*
    $Date: 2004/11/11 14:48:10 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*==============================================================*/
/* Includes */
#include "sio.h"
#include "pi_sys.h"


/*==============================================================*/
/* Globals */
static SioGetFuncPtr sio_get_func = 0;  /* current pointer to the get function */
static SioPutFuncPtr sio_put_func = 0;  /* current pointer to the put function */


/*==============================================================*/
/* Buffer I/O implementation */

/*--------------------------------------------------------------*/
SioGetFuncPtr sio_set_get(SioGetFuncPtr get_func_ptr)
{
    SioGetFuncPtr   t;

    pi_disable();
    t               = sio_get_func;
    sio_get_func    = get_func_ptr;
    pi_enable();

    return t;
}

/*--------------------------------------------------------------*/
SioPutFuncPtr sio_set_put(SioPutFuncPtr put_func_ptr)
{
    SioPutFuncPtr t;

    pi_disable();
    t               = sio_put_func;
    sio_put_func    = put_func_ptr;
    pi_enable();

    return t;
}

/*--------------------------------------------------------------*/
void sio_get(char *buf, short req_num, short timeout, short *act_num)
{
    if (sio_get_func)
    {
        (*sio_get_func)(buf, req_num, timeout, act_num);
    }
    else
    {
        pi_wait(timeout);
        *act_num    = 0;
    }
}

/*--------------------------------------------------------------*/
void sio_put(const char *buf, short req_num)
{
    if (sio_put_func)
    {
        (*sio_put_func)(buf, req_num);
    }
}


/*==============================================================*/
/* Character I/O implementation */

/*--------------------------------------------------------------*/
char sio_poll_key(short timeout)
{
    unsigned char   c;
    short           act_num;

    do
    {
        sio_get(&c, 1, timeout, &act_num);
        if (!act_num)
        {
            return 0;
        }
    } while (c == 0x11);

    return c;
}

/*--------------------------------------------------------------*/
char sio_get_key(void)
{
//    return sio_poll_key(PI_FOREVER);
    return sio_poll_key(1);
}

/*--------------------------------------------------------------*/
char sio_put_char(char c)
{
    /* add a CR before a LF */
    if (c == 10)
    {
        sio_put_char(13);
    }

    sio_put(&c, 1);
    return c;
}


/* ****************************************************************************
    $Log: sio.c,v $
    Revision 1.2  2004/11/11 14:48:10  mungiguerrav
    2004/nov/11,gio 15:15          Napoli       Mungi
    - Si aggiungono keyword CVS e breve storia documentata
    - Si sostituiscono i TAB con 4 spazi
    - Si applica regole di codifica per incolonnamento e parentesi
    - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2000/01/20 09:23:20     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2000/01/20 09:23:20     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data � arretrata con un artificio
01.01   2000/gen/20,gio             Napoli              Busco,Balbi,Mungi
        - Si elimina blocco su ingresso caratteri > 0x80 in sio_poll_key

 ------------------------   Prima di CVS    ------------------------
01.00x  1997/xxx/XX,xxx             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

