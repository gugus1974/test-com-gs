/****************************************************************/
/* 2004/nov/08,lun 12:20                \SWISV\LIB\SRC\logMac.h */
/****************************************************************/
/*          DEFINIZIONE   M A C R O  di  LOGICA                 */
/****************************************************************/
/*
    $Date: 2004/11/08 11:39:40 $          $Revision: 1.3 $
    $Author: mungiguerrav $
*/


#define LEG(A)          Acc = (A)
#define AND(A)          Acc = Acc && (A)
#define OR(A)           Acc = Acc || (A)
#define STA1(A)         A   = Acc
#define STA0(A)         A   = !Acc

#define ONTD_NEW(A,V)   static unsigned short CN_##A = V, XN_##A = V
#define ONTD(A)         if (!Acc)               \
                            CN_##A = XN_##A;    \
                        else if (CN_##A)        \
                        {                       \
                            CN_##A--;           \
                            Acc=FALSO;          \
                        }

#define OFTD_NEW(A,V)   static unsigned short CF_##A = V, XF_##A = V
#define OFTD(A)         if ( Acc)               \
                            CF_##A = XF_##A;    \
                        else if (CF_##A)        \
                        {                       \
                            CF_##A--;           \
                            Acc=VERO;           \
                        }

#define COMPIST_NEW(A,SI,SS)    static short SI_##A=SI;       \
                                static short SS_##A=SS;       \
                                static ABOOL    ST_##A=FALSO
#define COMPIST(A,PAR)  if (ST_##A)             \
                        {                       \
                            if (PAR<=SI_##A)    \
                                ST_##A=FALSO;   \
                        }                       \
                        else                    \
                        {                       \
                            if (PAR>=SS_##A)    \
                                ST_##A=VERO;    \
                        }

#define RISING_NEW(A)     static unsigned short RISINGST_##A = 0
#define RISING(A)         if (Acc)                  \
                          {                         \
                              Acc = !RISINGST_##A;  \
                              RISINGST_##A = 1;     \
                          }                         \
                          else                      \
                              RISINGST_##A = 0;     \

#define FALLING_NEW(A)    static unsigned short FALLINGST_##A = 0
#define FALLING(A)        if (!Acc)                 \
                          {                         \
                              Acc = FALLINGST_##A;  \
                              FALLINGST_##A = 0;    \
                          }                         \
                          else                      \
                          {                         \
                              Acc = 0;              \
                              FALLINGST_##A = 1;    \
                          }                         \



/* ****************************************************************************
    $Log: logMac.h,v $
    Revision 1.3  2004/11/08 11:39:40  mungiguerrav
    2004/nov/08,lun 12:20          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.2        2004/06/15 11:35:26     buscob
01.05   2003/nov/21,ven             Napoli              Nappo
        - Per poter utilizzare il nuovo compilatore Borland C++ Builder 6 si convertono
          tutti gli int in short o long a seconda dell'uso.

    Revision 1.1        2001/12/14 12:04:46     accardol
    - Inserimento in CVS della versione in lib07m
    Revision 1.1.1.1    2001/12/14 12:04:46     accardol
    - Inserimento in CVS in branch STP della versione in lib07m
        La data è arretrata con un artificio
01.04   2001/dic/14,ven             Napoli              Mungi
        - Si sostituisce    BOOLEAN     con     ABOOL

 ------------------------   Prima di CVS    ------------------------
01.03   2000/feb/01,mar             Napoli              Busco
        - Si elim. bug in FALLING
01.02   1999/dic/16,gio             Napoli              Busco/Mungi
        - Si agg. macro RISING_NEW, RISING, FALLING_NEW, FALLING
01.01   1995/ott/19,gio             Napoli              Mungi
        - Si agg. macro OFTD e OFTD_NEW
01.00   1995/apr/20,gio             Napoli              Mungi
        - Creazione
*/

