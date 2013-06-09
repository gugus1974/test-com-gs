/*==============================================================================================*/
/* ATR header file for Process Data Marshalling extensions                                      */
/*                                                                                              */
/* (c)1998 Ansaldo Trasporti	            													*/
/* Written by Giuseppe Carnevale                                                                */
/*==============================================================================================*/

#ifndef _ATR_PDM_
#define _ATR_PDM_


/*==============================================================================================*/
/* Includes */

#include "tcntypes.h"
#include "bmi.h"


/*==============================================================================================*/
/* Defines */

/* PDM commands for pdm_control() */
#define PDM_STOP            0       /* stop marshalling                                        */
#define PDM_START           1       /* start marshalling                                       */
#define PDM_CONFIG_INTERNAL 2       /* control internal marshalling                            */

/* PDM predefined marshalling modes */
#define PDM_LEADING         1       /* leading vehicle, e.g. traction leader */
#define PDM_LED             2       /* led vehicle, e.g. traction follower   */
#define PDM_WAGGON          3       /* waggon vehicle                        */
#define PDM_SINGLE_NODE     0       /* only one vehicle in composition       */


/*==============================================================================================*/
/* Prototypes */

void pdm_new_wtb_topo(Type_Topography *p_topography);
void pdm_set_node_mode_mask(unsigned short mode_and, unsigned short mode_or);
unsigned short pdm_node_mode_mask(unsigned short mode);

char pdm_set_node_mode(unsigned short mode);
void pdm_start_node(void);

int pdm_control(UNSIGNED8 control, UNSIGNED8 mode);


/*==============================================================================================*/

#endif

