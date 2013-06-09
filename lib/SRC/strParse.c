/********************************************************************/
/* 2004/nov/11,gio 15:45                \SWISV\LIB\SRC\strParse.c   */
/********************************************************************/
/*              String parsing utility routines                     */
/********************************************************************/
/*
    $Date: 2004/11/12 11:14:07 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


/*==================================================================*/
/* Includes */
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "strparse.h"


/*==================================================================*/
/* Macros */
#ifdef __C166__
    #define XHUGE xhuge
#else
    #define XHUGE
#endif


/*==================================================================*/
/* Utility routines */

/*------------------------------------------------------------------*/
/* Dump a buffer */

#define BYTES_PER_ROW 16

void dump_buffer(char absolute_addr, const unsigned char *buf, short size)
{
    short                       i, j, k;
    unsigned char               c;
    const unsigned char XHUGE   *p = buf;

    for (i = 0; i <= (size-1) / BYTES_PER_ROW; i++)
    {
        if (absolute_addr)
        {
            printf("%08lX:", (long)p + (long)(i * BYTES_PER_ROW));
        }
        else
        {
            printf("%04x:", i * BYTES_PER_ROW);
        }

        for (j = 0; j < BYTES_PER_ROW; j++)
        {
            k   = i*BYTES_PER_ROW + j;
            if (k < size)
            {
                printf(" %02X", p[k]);
            }
            else
            {
                printf("   ");
            }
        }
        printf(" '");

        for (j = 0; j < BYTES_PER_ROW; j++)
        {
            k   = i*BYTES_PER_ROW + j;
            if (k < size)
            {
                c   = p[k];
                if (c >= ' ')
                {
                    printf("%c", c);
                }
                else
                {
                    printf(".");
                }
            }
            else
            {
                printf(" ");
            }
        }
        printf("'\n");
    }
}

/*------------------------------------------------------------------*/
/* Parse a buffer */

short parse_buffer(const char *s, unsigned char *buf, short el_size)
{
    unsigned char       *p = buf;       /* current buffer pos   */
    unsigned long       v;              /* hex value            */
    const char          *ns;            /* new string ptr       */
    short               j;              /* string length        */

    while (*s)
    {
        if (*s == '\"')
        {
            /* calculate the string length */
            ns      = (const char *)strchr(++s, '\"');
            if (ns)
            {
                j = ns - s;
            }
            else
            {
                j = strlen(s);
            }

            /* copy the string into the buffer */
            memcpy(p, s, j);
            p += j;

            /* find the new string position */
            s += j;
            if (*s == '\"')
            {
                s++;
            }

            while (*s == ' ')
            {
                s++;
            }
        }
        else
        {
            if      (toupper(*s) == 'S')
            {
                el_size     = 1;
                s++;
            }
            else if (toupper(*s) == 'W')
            {
                el_size     = 2;
                s++;
            }
            else if (toupper(*s) == 'L')
            {
                el_size     = 4;
                s++;
            }

            /* convert into a number */
            sscanf(s, "%lx", &v);

            /* save the number in the buffer */
            if      (el_size == 1)
            {
                *p  = v;
            }
            else if (el_size == 2)
            {
                *(short*)p = v;
            }
            else if (el_size == 4)
            {
                *(long*)p = v;
            }

            p += el_size;

            /* find the new string position */
            do
            {
                s++;
            } while (*s && *s != ' ');

            while (*s == ' ')
            {
                s++;
            }
        }
    }
    return p - buf;
}

/*------------------------------------------------------------------*/
/* Parse a number */

long parse_number(const char *s)
{
    long    l = 0;

    if      (s[0] == '$')
    {
        sscanf(s+1, "%lx", &l);
    }
    else if (s[0] == '0' && toupper(s[1]) == 'X')
    {
        sscanf(s+2, "%lx", &l);
    }
    else
    {
        sscanf(s, "%ld", &l);
    }

    return l;
}


/* ****************************************************************************
    $Log: strParse.c,v $
    Revision 1.2  2004/11/12 11:14:07  mungiguerrav
    2004/nov/11,gio 15:45          Napoli       Mungi
        - Si aggiungono keyword CVS e breve storia documentata
        - Si sostituiscono i TAB con 4 spazi
        - Si applica regole di codifica per incolonnamento e parentesi
        - Non si applicano ancora regole di codifica per nomi routine e variabili


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        1999/05/17 07:52:36     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    1999/05/17 07:52:36     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.10   1999/mag/17,lun             Napoli              Carnevale
        - Modifiche non documentate

 ------------------------   Prima di CVS    ------------------------
01.01   1997/xxx/XX,xxx             Napoli              Carnevale
        - Modifiche non documentate
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

