/****************************************************************************************/
/* 2004/nov/05,ven 17:50                                        \SWISV\LIB\SRC\energy.h */
/*                                                                                      */
/* Module: ENERGY                                                                       */
/* Description: Calcolo energia assorbita e restituita                                  */
/*                                                                          HEADER FILE */
/****************************************************************************************/
/*
    $Date: 2004/11/05 16:55:12 $          $Revision: 1.2 $
    $Author: mungiguerrav $
*/


#ifndef ENERGY_INCLUDED
#define ENERGY_INCLUDED


/****************************************************************************************/
/* Symbol definitions                                                                   */
/****************************************************************************************/
#define ENERGY_TCAMPOK      0               /* Passo di campionamento valido            */
#define ENERGY_TCAMPFAIL    1               /* Passo di campionamento troppo piccolo    */

#define ENERGY_TMSMIN       10              /* Passo di campionamento minimo in msec    */


/****************************************************************************************/
/* Type definitions                                                                     */
/****************************************************************************************/
struct energy_counter
{
    unsigned short  enerAss;
    unsigned short  enerRes;
    unsigned long   sumPotAss;
    unsigned long   sumPotRes;
};


/****************************************************************************************/
/* Functions prototypes                                                                 */
/****************************************************************************************/
unsigned short energy_init(unsigned short tms);
void           energy_calc(long pot, struct energy_counter *pt);


#endif


/* ****************************************************************************
    $Log: energy.h,v $
    Revision 1.2  2004/11/05 16:55:12  mungiguerrav
    2004/nov/05,ven 17:50          Napoli       Mungi
    - Si aggiungono keyword CVS e storia invertita


 ----------------   Fase transitoria tra *.mod e CVS    ------------
    Revision 1.1        2003/02/12 10:19:22     accardol
    - Inserimento in CVS della versione in lib07n
01.06   2003/feb/12,mer             Napoli              Accardo
        - Si agg. def. struttura
                    struct energy_counter
                    {
                        unsigned short  enerAss;
                        unsigned short  enerRes;
                        unsigned long   sumPotAss;
                        unsigned long   sumPotRes;
                    };
        - Si sost. prot.    void energy_calc(long pot)
          con               void energy_calc(long pot, struct energy_counter *pt)

 ------------------------   Prima di CVS    ------------------------
01.05   2003/feb/11,mar             Napoli              Accardo
        - Si elim. prot.    void energy_calcAss(long pot)
01.04   2003/feb/10,lun             Napoli              Accardo
        - Si sost. prototipi:
                void energy_calc   (short tens, short corr)     con     void energy_calc   (long pot)
                void energy_calcAss(short tens, short corr)     con     void energy_calcAss(long pot)
01.03   2003/feb/10,lun             Napoli              Accardo
        - Si agg. prot. void energy_calcAss(short tens, short corr);
01.02   2003/gen/27,lun             Napoli              Accardo
        - Si modifica   unsigned short energy_init(float tCalc)
          in            unsigned short energy_init(float tCalc, unsigned short enerAssIni, unsigned short enerResIni)
01.01   2003/gen/24,ven             Napoli              Accardo
        - Si modifica   void            energy_init()
          in            unsigned short  energy_init(float tCalc)
01.00   2003/gen/22,mer             Napoli              Accardo
        - Creazione
*/

