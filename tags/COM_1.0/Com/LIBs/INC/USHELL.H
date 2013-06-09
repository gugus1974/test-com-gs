/********************************************************************/
/* 2004/nov/12,ven 12:15                \SWISV\LIB\SRC\ushell.h     */
/********************************************************************/
/*                  Gestione linea di Comando                       */
/********************************************************************/
/*
    $Date: 2004/11/12 11:46:44 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef _USHELL_
#define _USHELL_

/*==================================================================*/
/* Includes */
#include "strparse.h"


/*==================================================================*/
/* Typedefs */
/* shell command function prototype */
typedef short (*ShellCmdFct)(short argc, char *argv[]);


/*==================================================================*/
/* Globals */
extern short    ushell_sema;            /* semaphore that could be used by commands */
                                        /* to sync with other tasks                 */


/*==================================================================*/
/* Prototypes */
short           ushell_init( void );
short           ushell( const char *prompt );
short           ushell_register( const char  *cmd,
                                 const char  *arg_desc,
                                 const char  *cmd_desc,
                                 ShellCmdFct fct );
short           ushell_exec( const char *s );
void            ushell_cmdon(const char *s);
void            ushell_cmdoff(const char *s);


unsigned char   get_line( char          *line,
                          unsigned char n,
                          char          use_history_flag );

#endif


/* ****************************************************************************
    $Log: ushell.h,v $
    Revision 1.2  2004/11/12 11:46:44  mungiguerrav
    2004/nov/12,ven 12:15          Napoli       Mungi
        - Si aggiungono keyword CVS e storia recente invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2000/10/27 14:58:40     accardol
    - Inserimento in CVS della versione in lib07n
    Revision 1.1.1.1    2000/10/27 14:58:40     accardol
    - Inserimento in CVS in branch STP della versione in lib07n
        La data è arretrata con un artificio
01.22   2000/ott/27,ven             Napoli              Busco
        - Si agg. prototipi ushell_cmdon(..) e ushell_cmdoff(..)

 ------------------------   Prima di CVS    ------------------------
01.21   2000/feb/07,lun             Napoli              Mungi
        - Si riporta il prot. di ushell_register da void a short
          compatibile con TCNS del 1999/apr/14
01.20   2000/feb/05,sab             Napoli              Mungi
        - Si riporta il prot. di ushell_register da short a void
          compatibile con STAR\TCNS del 1998/giu/24
01.10   1999/lug/13,mar             Napoli              Carnevale
        - L'addio ....                  (con ushell_sema !!!!)
01.00   1996/xxx/XX,xxx             Napoli              Carnevale
        - Creazione
*/

