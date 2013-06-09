2004/ago/31,mar 16:00              \SWISV\LIB\history.mod
****************************************************************
            Storia globale del modulo LIB
****************************************************************
$Date: 2005/01/25 16:43:00 $          $Revision: 1.3 $
$Author: mungiguerrav $

****************************************************************************/
$Log: history.mod,v $
Revision 1.3  2005/01/25 16:43:00  mungiguerrav
----------------------------------------------------------------------
        Release: rel_06a    2005/01/25   17:18:05
- Solo per i file:          atr_hw.h,   atr_log.c,.h
    - Si aggiungono keyword CVS e storia invertita, abolendo i .mod
    - Si sostituiscono [TAB] con [4 spazi] per
    - Si applicano le regole di codifica sulle parentesi
  Gli altri file hanno ancora i formati Carnevale,
  ma si spera a poco a poco di regolarizzarli.
  Ad esempio molti file non rispettano la regola del divieto dell'underscore "_"

- makefile.     +       A\HW\makefile.
    - Si aggiungono      include incfile di LBATR\A\HW,LBATR,LIB
      necessari alla gestione degli include a piu' livelli

- incfile.      +       A\HW\incfile.
    - Si crea per permettere ai makefile di gestire gli include a piu' livelli:
      in questi file sono create le variabili con gli include a cascata dei
      file header presenti nella dir:
            SRC             per     incfile.
            A\HW\SRC        per     A\HW\incfile.
----------------------------------------------------------------------

Revision 1.2  2004/08/31 17:01:32  mungiguerrav
2004/ago/31,mar 18:55        Napoli          Mungi\Busco
- REL_06
- Si inseriscono file makefile superiore e inferiore
- Il makefile superiore gestisce    checkrel check
- Si aboliscono i file *.obj e relative directory dalla gestione CVS

Revision 1.7  2004/08/31 14:53:39  mungiguerrav


****************************************************************
Per la storia precedente di LBATR, vedi foto lbAtr05c del 2001/12/18,mar
