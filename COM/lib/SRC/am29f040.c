/********************************************************************/
/* 2004/nov/30,mar 15:05                \SWISV\LIB\SRC\am29f040.c   */
/********************************************************************/
/*                  Am29F040 write routines                         */
/********************************************************************/
/*
    $Date: 2006/12/05 14:00:36 $          $Revision: 1.3.4.1 $
    $Author: schiavor $
*/


/*==================================================================*/
/* Includes */
#include "am29f040.h"

/*==================================================================*/
/* Pragmas */
#ifdef __C166__
#pragma WL(3)
#endif

/*==================================================================*/
/* Macros */
#ifdef __C166__
    #define XHUGE xhuge
#else
    #define XHUGE
#endif

/*==================================================================*/
/* Constants */
#define KEY1    0x5555
#define KEY2    0x2AAA

/*==================================================================*/
/* Macros */
#ifdef O_LE
    #define read_word(p) ( ((unsigned short)*(((unsigned char*)p)+1) << 8) | *(unsigned char*)p )
#else
    #define read_word(p) ( *(unsigned short*)p )
#endif

#define am29f040(base, offset) *(((volatile unsigned short XHUGE *)(base)) + (offset))


/*==================================================================*/
/* Private routines */

/*------------------------------------------------------------------*/
/* Wait for an operation to complete */
static short am29f040_wait(void *base, long offset, short value)
{
    short t;
//    err = am29f040_wait(base, 0, 0xFFFF);

    do
    {
        t   = am29f040(base, offset);
        if ((t & 0x0080) == (value & 0x0080))
        {
            goto pass1;                                 /* CLAMOROSO !!!!!!!!   */
        }
    } while ((t & 0x0020) != 0x0020);

    if ((t & 0x0080) == (value & 0x0080))
    {
        goto pass1;                                     /* CLAMOROSO !!!!!!!!   */
    }

    return AM29F040_FAILURE_ERR;

pass1:
    do
    {
        t   = am29f040(base, offset);

        if ((t & 0x8000) == (value & 0x8000))
        {
            goto pass2;                                 /* CLAMOROSO !!!!!!!!   */
        }
    } while ((t & 0x2000) != 0x2000);

    if ((t & 0x8000) == (value & 0x8000))
    {
        goto pass2;                                     /* CLAMOROSO !!!!!!!!   */
    }

    return AM29F040_FAILURE_ERR;

pass2:
    return AM29F040_OK;
}


/*------------------------------------------------------------------*/
/* Check that at the specified address there is an Am29F040 */
short am29f040_check_code(void *base)
{
    /* send the autoselect command */
    am29f040(base, 0)       = 0xF0F0;
    am29f040(base, KEY1)    = 0xAAAA;
    am29f040(base, KEY2)    = 0x5555;
    am29f040(base, KEY1)    = 0x9090;

    /* check the manufacture code and Am29F040 device code */
    if (am29f040(base, 0) != 0x0101 || am29f040(base, 1) != 0xA4A4)
    {
        return AM29F040_FAILURE_ERR;
    }

    /* reset to read mode and return */
    am29f040(base, 0)       = 0xF0F0;
    return AM29F040_OK;
}


/*------------------------------------------------------------------*/
/* Chip erase */

short am29f040_chip_erase(void *base)
{
    short err;                              /* error code */

    /* send chip erase command */
    am29f040(base, 0)       = 0xF0F0;//reset
    am29f040(base, KEY1)    = 0xAAAA;//KEY1= 5555
    am29f040(base, KEY2)    = 0x5555;//KEY2= 2AAA
    am29f040(base, KEY1)    = 0x8080;
    am29f040(base, KEY1)    = 0xAAAA;
    am29f040(base, KEY2)    = 0x5555;
    am29f040(base, KEY1)    = 0x1010;

    /* wait for erase completion */
    err = am29f040_wait(base, 0, 0xFFFF);

    /* reset */
    am29f040(base, 0)       = 0xF0F0;

    return err;
}


/*------------------------------------------------------------------*/
/* Sectore erase */
short am29f040_sector_erase(void *base, short sector)
{
/*sector =0..7 
se sector= 7 => base + offset= 470000h
*/

    short   err;                            /* error code */
    long    offset  = ((long)sector) << 16;

    /* send sector erase command */
    am29f040(base, 0)       = 0xF0F0;
    am29f040(base, KEY1)    = 0xAAAA;
    am29f040(base, KEY2)    = 0x5555;
    am29f040(base, KEY1)    = 0x8080;
    am29f040(base, KEY1)    = 0xAAAA;
    am29f040(base, KEY2)    = 0x5555;
    am29f040(base, offset)  = 0x3030;

    /* wait for erase completion */
    err = am29f040_wait(base, offset, 0xFFFF);

    /* reset */
    am29f040(base, 0)       = 0xF0F0;

    return err;
}


/*------------------------------------------------------------------*/
/* Buffer writing */
short am29f040_write(void *base, long offset, void *buff, long buff_len)
{
    short       err = AM29F040_OK;          /* error code */
    short XHUGE *p  = buff;                 /* current ptr in the buffer */
    short       t;                          /* temp value */

    /* convert the offset and the buff_len to words */
    offset      /= 2;
    buff_len    /= 2;

    /* reset */
    am29f040(base, 0)       = 0xF0F0;

    while (buff_len)
    {
        /* read the word to write (only once, it could change) */
        t   = read_word(p);

        /* start the byte program */
        am29f040(base, KEY1)    = 0xAAAA;
        am29f040(base, KEY2)    = 0x5555;
        am29f040(base, KEY1)    = 0xA0A0;
        am29f040(base, offset)  = t;

        /* wait for write completion */
        err = am29f040_wait(base, offset, t);
        if (err)
        {
            goto error;                                 /* CLAMOROSO !!!!!!!!   */
        }

        /* go to the next word */
        offset++;
        p++;
        buff_len--;
    }

error:
    /* reset */
    am29f040(base, 0)       = 0xF0F0;

    return err;
}



/* ****************************************************************************
    $Log: am29f040.c,v $
    Revision 1.3.4.1  2006/12/05 14:00:36  schiavor
    05/12/2006                         Napoli                            SchiavoR
    si rende compatibile con il sw del gtw

    Revision 1.3  2004/12/13 10:02:43  mungiguerrav
    2004/nov/30,mar 15:05           Napoli      Mungi
    - Per rendere possibile la compilazione a scopo di debug anche in BorlandC,
      si definisce XHUGE pari a huge nel caso non sia definito __C166__

    Revision 1.2  2004/11/12 09:59:56  mungiguerrav
    2004/nov/12,ven 08:45          Napoli       Mungi
        - Si aggiungono keyword CVS e breve storia documentata
        - Si sostituiscono i TAB con 4 spazi
        - Si applica regole di codifica per incolonnamento e parentesi
        - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1999/05/17 07:53:24     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1999/05/17 07:53:24     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data � arretrata con un artificio
01.10   1999/mag/17,lun             Napoli              Carnevale
        - Modifiche non documentate

 ------------------------   Prima di CVS    ------------------------
01.01   1997/xxx/XX,xxx             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

