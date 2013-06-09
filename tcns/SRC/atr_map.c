/*==============================================================================================*/
/* UIC Mapping Server patches file													            */
/* Implementation file (atr_map.c)													            */
/* 																					            */
/* Version 1.0																		            */
/*			 																		            */
/* (c)1998 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes  */

#include <stdio.h>
#include "pi_sys.h"
#ifdef ATR_WTB
#include "bmi.h"
#include "atr_pdm.h"
#endif
#include "am_sys.h"
#include "map.h"
#include "c_utils.h"
#include "ushell.h"


/*==============================================================================================*/
/* Globals */

unsigned short MAP_SHOW_NADI = 0;	/* print the NADI: 1=x/yNADI 2=NADI (one shoot) */
unsigned short MAP_TRACE_ON  = 1;	/* enables a more detailed tracing              */

int map_debug_flag = 0;				/* if 1 enables the printouts                   */

#ifdef ATR_WTB
void (*map_user_ls_Report)(l_report) = 0;	/* user WTB Link Layer report function */
#endif


/*==============================================================================================*/
/* Routines */

/*----------------------------------------------------------------------------------------------*/
/* Write service for the Mapping Server (used only on the gateway) */

#ifdef ATR_WTB

void write_serv(const char *s)
{
	if (map_debug_flag) printf("%s\n", s);
}

#endif


/*----------------------------------------------------------------------------------------------*/
/* Calculate the PDM mode from the vehicle descriptor */

#ifdef ATR_WTB

unsigned short node_mode_from_vhdes(map_VehicleDescr *vhdes)
{
	if (vhdes->DynAttr[0] & (0x02 | 0x08)) return PDM_LEADING;
	if (vhdes->PermAttr[0] & 0x01) return PDM_LED;
	return PDM_WAGGON;
}

#endif


/*==============================================================================================*/
/* Debug routines */

/*----------------------------------------------------------------------------------------------*/
/* UIC Mapping Server NADI display */

static const CMessageDef m_nadi_validity[] =
	{ {MAP_NADI_INVALID, "MAP_NADI_INVALID"}, {MAP_NADI_VALID, "MAP_NADI_VALID"}, {0, 0} };

static const CMessageDef m_nadi_mdirect[] =
	{ {MAP_DIREC_UNKNOWN, "MAP_DIREC_UNKNOWN"}, {MAP_DIREC_1, "MAP_DIREC_1"},
	  {MAP_DIREC_2, "MAP_DIREC_2"}, {0, 0} };

static const CMessageDef m_nadi_scope[] =
	{ {MAP_RELATIVE, "MAP_RELATIVE"}, {MAP_ABSOLUTE, "MAP_ABSOLUTE"}, {0, 0} };

static const CMessageDef m_nadi_tcntype[] =
	{ {0x02 /*L_STRONG*/, "L_STRONG"}, {0x03 /*L_WEAK*/, "L_WEAK"}, {0x01 /*L_SLAVE*/, "L_SLAVE"}, {0, 0} };

static void bitset_dump(const char *b, int n_bits)
{
	int	 i;
	char c;

	for (i = 0, c = 1; i < n_bits; i++) {
		if (*b & c) printf("%d ", i+1);
		if (!(c <<= 1)) c = 1, b++;
	}
}

static short nadi_cmd(short argc, char *argv[])
{
	int					i;			/* generic index                      */
	map_TrainTopography	*map_topo;	/* current Mapping Server topography  */
	char				ts[12];		/* temporary string                   */

	argv = argv;
	if (argc != 1) return -1;

	if (map_GetTrainTopography(0, &map_topo) != MAP_OK) {
		printf("NADI not available.\n");
		return 0;
	}

	printf("version        : %u\n", map_topo->state.version);
	printf("validity       : %s\n", c_fetch_message(m_nadi_validity, map_topo->state.validity, ts, 0));
	printf("mdirect        : %s\n", c_fetch_message(m_nadi_mdirect, map_topo->state.mdirect, ts, 0));
	printf("ScopeOfRangeNo : %s\n", c_fetch_message(m_nadi_scope, map_topo->state.ScopeOfRangeNo, ts, 0));
	printf("\n");

	for (i = 0; i < map_topo->state.VehicleNo; i++) {
		printf("Node %2u%c (TCN %02u %s), ID : %02X%02X%02X%02X%02X%02X, No : %u, NcV : %d\n",
				map_topo->vehicle[i].RangeNo,
				map_topo->state.ownaddr == map_topo->vehicle[i].NodeAddr ? '*' : ' ',
				map_topo->vehicle[i].NodeAddr,
				c_fetch_message(m_nadi_tcntype,    map_topo->vehicle[i].TCNType, ts, 0),
				map_topo->vehicle[i].VTy,          map_topo->vehicle[i].Owner,
				map_topo->vehicle[i].VehicleId[0], map_topo->vehicle[i].VehicleId[1],
				map_topo->vehicle[i].VehicleId[2], map_topo->vehicle[i].VehicleId[3],
				map_topo->vehicle[i].VehicleNo,    map_topo->vehicle[i].NcV);

		printf("Functions : ");
		bitset_dump((char*)map_topo->vehicle[i].Functions, sizeof(map_topo->vehicle[i].Functions)*8);
		printf("\n");
		printf("PermAttr  : ");
		bitset_dump((char*)map_topo->vehicle[i].PermAttr, sizeof(map_topo->vehicle[i].PermAttr)*8);
		printf("\n");
		printf("PAttrFS   : ");
		bitset_dump((char*)map_topo->vehicle[i].PAttrFS, sizeof(map_topo->vehicle[i].PAttrFS)*8);
		printf("\n");
		printf("DynAttr   : ");
		bitset_dump((char*)map_topo->vehicle[i].DynAttr, sizeof(map_topo->vehicle[i].DynAttr)*8);
		printf("\n\n");
	}

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
/* NADI's dynamic attributes toggling */

static void vh_des_conf(void *ref, map_result status)
{
	ref = ref;
	status = status;

	/* report the update result on error and signal that we can go on */
	if (status) printf("Update result = %d\n", status);
    pi_post_semaphore(ushell_sema, 0);
}

static short nadi_dyn_cmd(short argc, char *argv[])
{
	map_VehicleDescr	*vh_des_p, vh_des;
	int					pos, val;

	/* get the parameters */
	if (argc < 2 || argc > 3) return -1;
	pos = parse_number(argv[1]) - 1;
	if (argc == 3) val = parse_number(argv[2]);
	else val = -1;

	/* get the old vehicle descriptor */
	map_GetOwnVhDescriptor(0, &vh_des_p);
	vh_des = *vh_des_p;

	/* get the value if the toggling is requested */
	if (val == -1) val = !c_get_bit(vh_des.DynAttr, pos);

	/* set the new value */
	if (val) c_set_bit(vh_des.DynAttr, pos);
	else c_clear_bit(vh_des.DynAttr, pos);

	/* fix the node strenght */
	vh_des.TCNType = 3 /*L_WEAK*/;

	/* set the new vehicle descriptor */
    map_SetVehicleDescriptor(0, &vh_des, vh_des_conf, 0);

	/* wait for completion */
    pi_pend_semaphore(ushell_sema, PI_FOREVER, 0);

	return 0;
}


/*----------------------------------------------------------------------------------------------*/
/* Installs the UIC Mapping Server monitor(s) commands */

void map_monitor(void)
{
	ushell_register("nadi", "", "Print the NADI", nadi_cmd);
	ushell_register("nadi_dyn", "<number> [<value>]", "Change a NADI dynamic attribute", nadi_dyn_cmd);
}


/*----------------------------------------------------------------------------------------------*/


/*==============================================================================================*/

