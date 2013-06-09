/***************************************************************************/
/*2001.ven.07.set      v.1.01         \SW\COM\PRJs\GTW\SRC\gtw_util.c      */
/*                                                                         */
/* Module: gtw_util                                                        */
/* Description:Gateway utilities                                           */
/*                                                    IMPLEMENTATION FILE  */
/***************************************************************************/
/***************************************************************************/


/*=============================================================================================*/
/* Include files */


#include "lc_sys.h"
#include "lp_sys.h"
#include "pi_sys.h"
#include "am_sys.h"
#include "pdm_main.h"
#include "pdm_lcfg.h"
#include "map.h"

#include "conf_tcn.h"
#include "c_utils.h"
#include "xmodem.h"
#include "atr_nma.h"
#include "crc.h"
#include "x25642.h"

#include "gtw_appl.h"
#include "gtw_util.h"


/*=============================================================================================*/
/* Mapping server utilities */

/*---------------------------------------------------------------------------------------------*/
/* TCN to UIC address conversion */

unsigned char tcn2uic(map_TrainTopography *map_topo, unsigned char tcn_address)
{
	int	i;

	for (i = 0; i < map_topo->state.VehicleNo; i++)
		if (map_topo->vehicle[i].NodeAddr == tcn_address) return map_topo->vehicle[i].RangeNo;

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* UIC to TCN address conversion */

unsigned char uic2tcn(map_TrainTopography *map_topo, unsigned char uic_address)
{
	int	i;

	for (i = 0; i < map_topo->state.VehicleNo; i++)
		if (map_topo->vehicle[i].RangeNo == uic_address) return map_topo->vehicle[i].NodeAddr;

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
/* TCN to ORD address conversion */
/* 33 34 .. 62 63 01 02 .. 31 32 */
/* 00 01 .. 29 30 31 32 .. 61 62 */

unsigned char tcn2ord(unsigned char tcn_address)
{
	return (tcn_address + 30) % 63;
}


/*---------------------------------------------------------------------------------------------*/
/* Driving direction (relative direction of trainset's direction 1) */

char get_driving_direction(map_TrainTopography *map_topo)
{
	unsigned char	my_TCN_address;	/* our address                                */
	char			node_orient;	/* our orientation relative to the TCN master */
	char			driving_dir;	/* the determined driving direction           */

	/* we can determine this information only if we have a valid NADI */
	if (map_topo->state.validity != MAP_NADI_VALID) return 0;

	/* get the input variables */
	my_TCN_address = map_topo->state.ownaddr;
	node_orient    = map_topo->state.mdirect;

	/* fix the node orientation if we are the TCN master or return unknown */
	if (node_orient == 0) {
		 if (my_TCN_address == 1) node_orient = 1;
		 else return 0;
	}

	/* is the TCN master oriented like UIC ? */
	if (tcn2ord(uic2tcn(map_topo,1)) < tcn2ord(uic2tcn(map_topo,2))) {
		if (node_orient == 1) driving_dir = 1;			/* same dir if I have the TCN dir        */
		else                  driving_dir = 2;
	}
	else {
		if (node_orient == 2) driving_dir = 1;			/* same dir if I don't have the TCN dir  */
		else                  driving_dir = 2;
	}

	return driving_dir;
}


/*=============================================================================================*/
/* EEPROM management                                                                           */
/*                                                                                             */
/*      +------------------------------------------------------------+                         */
/* 0000 | 8 * Gateway configuration structures (16 bytes)            | 007F                    */
/*      +------------------------------------------------------------+                         */
/* 0080 | Unused                                                     | 1FFF                    */
/*      +------------------------------------------------------------+                         */
/*                                                                                             */

/*---------------------------------------------------------------------------------------------*/

#define CONF_ADDRESS	0	/* base address for the gateway configuration structures */
#define CONF_NUM		8	/* number of configuration structures                    */


/*---------------------------------------------------------------------------------------------*/
short get_conf(GtwApplConf *conf, int *pos)
{
	short			err;			/* error code               */
	GtwApplConf 	c[CONF_NUM];	/* configuration structures */
	int				i, j;			/* generic indexes          */
	unsigned short	crc;			/* calculated CRC           */
	unsigned long	max_boot;		/* maximum boot count       */

	/* load the configuration structures */
	Check(x25642_read(CONF_ADDRESS, (void*)c, sizeof(c)));

	/* find the most recent one */
	for (i = 0, j = -1, max_boot = 0; i < CONF_NUM; i++) {
		crc = crc_16(crc_table_0x1021, 0, (void*)&c[i], sizeof(*c) - 2);
		if (crc == c[i].crc && c[i].reboot_count > max_boot) {
			max_boot = c[i].reboot_count;
			j = i;
		}
	}

	/* save its pointer and index */
	if (conf) {
		if (j == -1) pi_zero8(conf, sizeof(*conf));
		else *conf = c[j];
	}
	if (pos) *pos = j;

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short set_conf(GtwApplConf *conf)
{
	short	err;	/* error code          */
	int		i;		/* configuration index */

	/* calculate the CRC */
	conf->crc = crc_16(crc_table_0x1021, 0, (void*)conf, sizeof(*conf) - 2);

	/* write the new configuration */
	Check(get_conf(0, &i));
	Check(x25642_write(CONF_ADDRESS + ((i + 1) % CONF_NUM) * sizeof(*conf), (void*)conf, sizeof(*conf)));

error:
	return err;
}


/*=============================================================================================*/
/* Gateway NCDB configuration management */

static const CMessageDef m_ncdb_upload_error[] = {
	{XMODEM_NCDB_WRITE_ERR,     "Errore nella scrittura su FLASH dell'NCDB"},
	{XMODEM_NCDB_FORMAT_ERR,    "Errore nel formato dell'NCDB"},
	{XMODEM_NCDB_MEMORY_ERR,    "Memoria esaurita"},
	{XMODEM_MEM_ERR,    		"Memoria esaurita"},
	{XMODEM_CANCEL_ERR, 		"Trasferimento XMODEM abortito"},
	{XMODEM_RETRY_ERR,  		"Troppi errori nel trasferimento XMODEM"},
	{XMODEM_BUFFER_ERR,  		"Errore di dimensione del buffer nel trasferimento XMODEM"},
	{XMODEM_ORDER_ERR,  		"Pacchetti fuori ordine nel trasferimento XMODEM"},
	{0, 0}
};

static unsigned long 	vid;		/* vehicle id                              */
static unsigned short	red_addr;	/* MVB address of the redundant unit       */
static short			red_err;	/* last error code from the redundant unit */

void filter_ncdb(NcObjHeader *header)
{
	/* fix the vehicle id */
	if (header->oid == NC_TCN_CONF_OBJ_ID) {
		NcTcnConf *conf = (NcTcnConf*)(header + 1);
		*(unsigned long *)&conf->vehicle_desc[42-1] |= vid;
	}

	/* send the object to the other station (if it isn't the NCDB header) */
	if (red_addr && !red_err && header->oid != NC_START_OF_DB_OBJ_ID)
		red_err = atr_nma_call(AM_SAME_VEHICLE, red_addr, AM_ANY_TC, ATR_NMA_WRITE_NCDB_SIF,
							   (void*)header, sizeof(*header) + header->size, 0, 0);
}


short carica_ncdb_cmd(short argc, char *argv[])
{
	unsigned short	my_addr = 0;	/* our MVB address                   */
	short			err;			/* error code                        */
	char			s[32];			/* temporary string                  */

	if (argc != 1) return -1;

	/* find the MVB addresses of the two gateways */
	nc_mvb_address(0, &my_addr);
	red_addr = 0;
	red_err = 0;
	if (nc_mvb_address(GTWS_ST_ID, &red_addr) == NC_OK) {
		if (red_addr == my_addr) nc_mvb_address(GTWM_ST_ID, &red_addr);
	}

	printf("\n\n*** Configurazione Gateway TCN/TCN* - Ansaldo Trasporti S.p.A. ***\n\n");

	printf("Inserisci il numero di serie del veicolo : ");
	if (!get_line(s, sizeof(s), FALSE)) return 0;
	sscanf(s, "%lx", &vid);

	/* start the remote NCDB update */
	if (red_addr && !red_err) {
		red_err = atr_nma_call(AM_SAME_VEHICLE, red_addr, AM_ANY_TC, ATR_NMA_START_NCDB_UPDATE_SIF, 0, 0, 0, 0);
		if (red_err) printf("\n*** Errore nel collegamento MVB con l'altra unita' ***\n");
	}

	printf("\nAvvia il trasferimento XMODEM/CRC dell'NCDB...\n");
	err = xmodem_ext_ncdb_rcv(filter_ncdb);
	printf("\n\n");

	/* dump the result */
	if (err) printf("ERRORE: %s\n\n", c_fetch_message(m_ncdb_upload_error, err, s, 0));

	/* end the remote NCDB update */
	if (red_addr && !red_err)
		red_err = atr_nma_call(AM_SAME_VEHICLE, red_addr, AM_ANY_TC, ATR_NMA_END_NCDB_UPDATE_SIF, 0, 0, 0, 0);
	if (red_err) {
		printf("*** ATTENZIONE: non e' stato possibile aggiornare l'NCDB dell'altra ***\n");
		printf("***             unita' a causa di un errore.                        ***\n\n");
	}

	if (err) printf("*** Configurazione fallita ***\n");
	else printf("Configurazione completata.\n");

	return 0;
}


/*=============================================================================================*/
/* Set the gateway configuration parameters */

short configura_cmd(short argc, char *argv[])
{
	GtwApplConf		gtw_conf;	/* gateway configuration structure   */
	char			s[32];		/* temporary string                  */
	char			*sP;		/* temporary string pointer          */
	unsigned long 	vid;		/* vehicle id                        */

	if (argc > 2) return -1;
	if (argc == 1) {
		printf("Inserisci il numero di serie del veicolo : ");
		if (!get_line(s, sizeof(s), FALSE)) {
			printf("*** Configurazione abortita ***\n");
			return 0;
		}
		sP = s;
	}
	else sP = argv[1];

	/* read the board configuration */
	if (get_conf(&gtw_conf, 0)) {
		printf("*** Errore nel caricamento della configurazione ***\n");
		return 0;
	}

	/* put the vehicle ID in the configuration */
	sscanf(sP, "%lx", &vid);
	gtw_conf.veh_serial[0] = (vid >> 24) & 0xFF;
	gtw_conf.veh_serial[1] = (vid >> 16) & 0xFF;
	gtw_conf.veh_serial[2] = (vid >>  8) & 0xFF;
	gtw_conf.veh_serial[3] = (vid >>  0) & 0xFF;
	gtw_conf.reboot_count++;

	if (set_conf(&gtw_conf)) printf("*** Errore nell'aggiornamento della configurazione ***\n");
	else printf("Nuovo numero di serie del veicolo : %lX\n", vid);

	return 0;
}
/*=============================================================================================*/
/* Convert BCD -> BIN */

/*unsigned char bcd2bin(unsigned short valbcd)
{

    return((valbcd >> 4) * 10 + (valbcd & 0x0F));
}
*/
/*=============================================================================================*/
/* Convert BIN -> BCD */

unsigned short bin2bcd(unsigned char valbin)
{
    return(((valbin/10) << 4) + (valbin%10));
}
/*
void binaryToBCD(unsigned long binaryNumber,unsigned char *bcdNumber) {
    unsigned char index;
    for (index=0; i<numberOfDigits, index++)
    {
        if (binaryNumber!=0)
        {
            bcdNumber[index] = binaryNumber%10;
            binaryNumber = binaryNumber/10;
        }
    }
)


unsigned short bin2bcd2(unsigned char valbin)
{
    return(((valbin/10) << 4) + (valbin%10));
}
*/
/*=============================================================================================*/


