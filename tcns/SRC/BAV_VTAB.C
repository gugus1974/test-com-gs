/*   BAV_VTAB.C
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1995 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAV_VTAB
|
|    Document     :
|
|    Abstract     :   Vector Table for Version Info and Indirect Call Functions
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      1   BATL    95-04-26  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#include "bax_incl.h"
#include "bam_main.h"
#include "bad_diag.h"

/* new page
 ----------------------------------------
  constant vector table for indirect call
 ----------------------------------------
*/

const TYPE_BA_IND_CALL ba_ind_call_table =
{
 {BA_VERSION, BA_RELEASE, BA_UPDATE, BA_EVOLUTION},
  bam_init,
  bam_create,
  bam_task,
  bam_main,
  bam_diag
};

TYPE_BA_IND_CALL * ba_pc_ptr_to_table;

/*
 ----------------------------------------------------------------------------

    file name: BAV_VTAB.C 	   Evol-Nr:	1	dated:	95-04-26

    Increment of LP_EVOLUTION

    File Created
 ----------------------------------------------------------------------------
*/
