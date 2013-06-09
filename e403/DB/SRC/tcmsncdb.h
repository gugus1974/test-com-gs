/*=============================================================================================*/
/* Metro Madrid TCN definitions                                                                */
/* Header file (mad_ncdb.h)																	   */
/* 																							   */
/* Version 1.1																				   */
/*                          																   */
/*=============================================================================================*/

#ifndef TCMS_NCDB_INCLUDED
#define TCMS_NCDB_INCLUDED

#include "ncdb.h"


#define NUM_STATIO(statio)  (statio-0xF0000000)


/*==============================================================================================*/
/* Stations definitions */

#define CCUM_ST_ID   0xF0000003   /* Central Control Unit Active         */
#define CCUS_ST_ID   0xF0000007   /* Central Control Unit Standby        */

#define RIOAM_ST_ID  0xF0000004   /* Local Control Unit in car M Active  */
#define RIOAS_ST_ID  0xF0000008   /* Local Control Unit in car M Standby */

#define RIOBM_ST_ID  0xF0000005   /* Local Control Unit in car R Active  */
#define RIOBS_ST_ID  0xF0000009   /* Local Control Unit in car R Standby */

#define RIOCM_ST_ID  0xF0000006   /* Local Control Unit in car S Active  */
#define RIOCS_ST_ID  0xF000000A   /* Local Control Unit in car S Standby */

#define IDU1_ST_ID   0xF000000B   /* Integrated Diagnostic Unit Cab A    */
#define IDU2_ST_ID   0xF000000C   /* Integrated Diagnostic Unit Cab B    */

#define TCU1_ST_ID   0xF000000D   /* Traction Control Unit 1             */
#define TCU2_ST_ID   0xF000000E   /* Traction Control Unit 2             */

#define CGS1_ST_ID   0xF0000010   /* Aux Control unit 1                  */
#define CGS2_ST_ID   0xF0000011   /* Aux Control unit 2                  */
#define CGS3_ST_ID   0xF0000012   /* Aux Control unit 3                  */

#define BCU_ST_ID    0xF0000016   /* Brake Control unit 1                */

#define ERROR_ST_ID  0xF0000000

#define CCUS_FCT 128

#endif

