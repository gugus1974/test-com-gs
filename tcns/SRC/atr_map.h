/*==============================================================================================*/
/* ATR header file for UIC Mapping Server                                                       */
/*                                                                                              */
/* (c)1998 Ansaldo Trasporti	            													*/
/* Written by Giuseppe Carnevale                                                                */
/*==============================================================================================*/

#ifndef _ATR_MAP_
#define _ATR_MAP_

/*==============================================================================================*/
/* Patches sections */

/*----------------------------------------------------------------------------------------------*/
/* Compilation switches */

#define PDM			/* the UIC Mapping Server supports the PDM     */
#define TERM_OUT	/* enable the terminal output via write_serv() */
#define O_960		/* swear we are an i960 board (...)            */

/*----------------------------------------------------------------------------------------------*/
/* Includes */

/* required headers */
#include "recorder.h"
#include "old_sys.h"
#ifdef ATR_WTB
#include "bmi.h"
#endif
#include "map.h"
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------------------------*/
/* Prototypes */

/* UIC Mapping server print function */
void write_serv (const char *);


/*----------------------------------------------------------------------------------------------*/
/* Macros */

/* debug functions not implemented */
#define err_rec()
#define recorder(x)
// #define recorder(x) if (x != 0xA8 && x != 0x01 && x != 0x02) printf("(%02X)\n", (unsigned int)x); else
// #define recorder(x) if (x != 0xA8 && x != 0x01 && x != 0x02) printf("(%s %d %02X)\n", __FILE__, __LINE__, (unsigned int)x); else


/*----------------------------------------------------------------------------------------------*/
/* Little Endian support */

#ifdef O_LE

#define conv_be_short(x)            x = ((((x) & 0x00FF) << 8) | (((x) >> 8) & 0x00FF))
#define conv_be_s_tgm_id(x)         conv_be_short((x).code)
#define conv_be_VehicleDescr(x)     conv_be_short((x).VehicleNo)
#define conv_be_VehicleGroupList(x) conv_be_short((x).length)
#define conv_be_TrainTopography(x)  {                                               \
	int i;                                                                          \
	conv_be_short((x).state.version);                                               \
	for (i = 0; i < MAP_MAX_NO_VEH; i++) conv_be_VehicleDescr((x).vehicle[i]);      \
}

#else

#define conv_be_s_tgm_id(x)
#define conv_be_VehicleDescr(x)
#define conv_be_VehicleGroupList(x)
#define conv_be_TrainTopography(x)

#endif


/*----------------------------------------------------------------------------------------------*/
/* E-telegrams packing and unpacking macros for MVB stations */

#ifndef ATR_WTB

#define pack_uic_et(t,l)   if (*(l) > 6) memmove(((char*)(t)) + 5, ((char*)(t)) + 6, --*(l) - 5); else
#define unpack_uic_et(t,l) if (*(l) > 5) memmove(((char*)(t)) + 6, ((char*)(t)) + 5, ++*(l) - 6); else

#endif


/*==============================================================================================*/
/* Extras sections */

#ifdef ATR_WTB
unsigned short node_mode_from_vhdes(map_VehicleDescr *vhdes);
#endif

void map_monitor(void);


/*----------------------------------------------------------------------------------------------*/

#endif

