/*==============================================================================================*/
/* Process Data Marshalling patches file											            */
/* Implementation file (atr_pdm.c)													            */
/* 																					            */
/* Version 1.0																		            */
/*																					            */
/* (c)1998 Ansaldo Trasporti														            */
/* Written by Giuseppe Carnevale													            */
/*==============================================================================================*/


/*==============================================================================================*/
/* Includes */

#include "atr_pdm.h"
#include "pi_sys.h"
#include "lp_sys.h"
#include "pdm_main.h"
#include "pdm_lcfg.h"
#include "pd_jdp.h"
#include "bmi.h"
#include "conf_tcn.h"


/*==============================================================================================*/
/* Globals */

#ifndef UMS
extern char       		pd_jdp_topo_counter;	/* the topo_counter that enables the WTB TS */
#endif

static unsigned short	node_mode     = PDM_DO_NOT_MARSH;	/* the actual node operating mode   */
static unsigned short	node_mode_and = 0xFFFF;				/* value to be ANDed to the mode    */
static unsigned short	node_mode_or  = 0x0000;				/* value to be ORed to the mode     */
static unsigned char  	valid_topo_counter;					/* the current topo_counter value   */


/*==============================================================================================*/
/* Private routines */

/*----------------------------------------------------------------------------------------------*/
/* Change the exported frame size */

static void set_export_pdframe_size(UNSIGNED8 size)
{
    type_mvb_descriptor		*pdjdp_ts_descriptor;	/* the WTB TS descriptor      */

    /* get access to the WTB TS */
    if (lpc_open(WTB_TS_ID, &pdjdp_ts_descriptor) != LPS_OK) return;

    /* set the new port size */
    pdjdp_ts_descriptor->pb_pcs[pdjdp_ts_descriptor->pb_pit[0]].size = size;
}


/*----------------------------------------------------------------------------------------------*/
/* Invalidate the dataset of the specified node  */

static void invalidate_wtb_ts_node(UNSIGNED8 address, UNSIGNED8 node_type, UNSIGNED8 node_version)
{
    type_mvb_descriptor		*pdjdp_ts_descriptor;	/* the WTB TS descriptor      */
    unsigned short			act_port_index;			/* its port index             */
    struct STR_TB_PCS		*p_pcs;					/* its Port Control Structure */
	struct STR_TB_PRT_PGE	*p_dck_pge;				/* the port page to write     */
	int						write_pge;				/* page number to write       */
	int						i;						/* generic index              */

    /* get access to the WTB TS */
    if (lpc_open(WTB_TS_ID, &pdjdp_ts_descriptor) != LPS_OK) return;

	/* find out the PCS */
	act_port_index = pdjdp_ts_descriptor->pb_pit[address];
	p_pcs = pdjdp_ts_descriptor->pb_pcs + act_port_index;

	/* we cannot be interrupted by the WTB LL */
	pi_disable();

	/* find out the page to write */
	write_pge = p_pcs->page == 0 ? 1 : 0;
	p_dck_pge = pdjdp_ts_descriptor->pb_prt[act_port_index].pge + write_pge;

	/* write the data to the port */
	p_dck_pge->tb_dck[0] = node_type;
	p_dck_pge->tb_dck[1] = node_version;
	pi_zero8(&p_dck_pge->tb_dck[2], 126);

	/* update the PCS */
	p_pcs->size = 128;
	p_pcs->tack = -1;
	p_pcs->page = write_pge;

	/* enable the others */
	pi_enable();
}

/*----------------------------------------------------------------------------------------------*/
/* Initialize the WTB TS according to the new topography (if known...) */

static void initialize_wtb_ts(const Type_Topography *topo)
{
	int					i;			/* generic index */
	UNSIGNED8			address;

	/* invalidate all the WTB TS */
	for (i = 1; i < 64; i++) invalidate_wtb_ts_node(i, 0xFF, 0xFF);

	/* set the conected nodes FTF */
	for (i = 0, address = topo->bottom_address; i < topo->number_of_nodes;
			i++, address = address == 63 ? 1 : address + 1) {
#ifndef UMS
		invalidate_wtb_ts_node(address, topo->NK[i].node_type, topo->NK[i].node_version);
#else
		invalidate_wtb_ts_node(address, 0x21, 0x01); /* !!! */
#endif
	}
}


/*==============================================================================================*/
/* Public routines */

/*----------------------------------------------------------------------------------------------*/
/* The new WTB topography indication routine for the PDM (use this insted of pdm_wtb_topo_ind) */

void pdm_new_wtb_topo(Type_Topography *p_topography)
{
	/* save the topo counter value */
	valid_topo_counter = p_topography->topo_counter;

	initialize_wtb_ts(p_topography);
	pdm_wtb_topo_ind(p_topography);
}


/*----------------------------------------------------------------------------------------------*/
/* Set the mask values for the node mode */

void pdm_set_node_mode_mask(unsigned short mode_and, unsigned short mode_or)
{
	/* just save them for later use */
	node_mode_and = mode_and;
	node_mode_or = mode_or;
}


/*----------------------------------------------------------------------------------------------*/
/* Mask the specified node mode */

unsigned short pdm_node_mode_mask(unsigned short mode)
{
	return (mode & node_mode_and) | node_mode_or;
}


/*==============================================================================================*/
/* Public routines for Mapping Server 10 */

#ifndef UMS

/*----------------------------------------------------------------------------------------------*/
/* Sets the new operating mode for the node. Returns TRUE if the WTB topography will change */

char pdm_set_node_mode(unsigned short mode)
{
	static char			first_run = FALSE;
	Type_NodeDescriptor	node_descriptor;

	/* mask the requested mode */
	mode = pdm_node_mode_mask(mode);

	/* get the node descriptor from the mode */
	if (nc_node_cfg(mode, &node_descriptor) != NC_OK) {
		mode = 0;
		if (nc_node_cfg(mode, &node_descriptor) != NC_OK) {
			node_descriptor.node_frame_size       = 0;
			node_descriptor.node_period           = 7;
			node_descriptor.node_key.node_type    = 0xFF;
			node_descriptor.node_key.node_version = 0xFF;
		}
	}

	/* if the requested mode is the current one don't do anything */
	if (mode == node_mode) return FALSE;

	/* set the new mode */
	node_mode = mode;

	/* immediately stop the WTB TS */
	pd_jdp_topo_counter = 0;

	/* configure the export for the new mode and stop the import */

    //pdm_reconfig_marsh(PDM_INTERNAL, mode); //mod. per LOSA

    pdm_reconfig_marsh(PDM_EXPORT, mode);
    pdm_reconfig_marsh(PDM_IMPORT, PDM_DO_NOT_MARSH);

	/* set the new exported frame size */
    set_export_pdframe_size(node_descriptor.node_frame_size);

	/* start the topography update */
	if (!first_run) first_run = TRUE;
	else ls_t_ChgNodeDesc(node_descriptor);

	/* the WTB will be re-inaugurated */
	return TRUE;
}


/*----------------------------------------------------------------------------------------------*/
/* Enables the WTB dataset transmission. Must be called when the data is valid */

void pdm_start_node(void)
{
	/* enable the WTB TS */
	pd_jdp_topo_counter = valid_topo_counter;

	/* configure the import for the new mode */
   	pdm_reconfig_marsh(PDM_IMPORT, node_mode);
}

/*----------------------------------------------------------------------------------------------*/

#endif


/*==============================================================================================*/
/* Public routines for Mapping Server 12 */

#ifdef UMS

/*----------------------------------------------------------------------------------------------*/
/* The new pdm_control() routine for the "old" PDMs */

int pdm_control(UNSIGNED8 control, UNSIGNED8 mode)
{
	Type_NodeDescriptor	node_descriptor;

    switch (control) {

    	case PDM_START:
    		if (nc_node_cfg(mode, &node_descriptor) == NC_OK) {
    			node_mode = mode;
		        set_export_pdframe_size(node_descriptor.node_frame_size);
//				ls_t_ChgNodeDesc(node_descriptor);
    		    pdm_reconfig_marsh(PDM_EXPORT, node_mode);
    		    pdm_reconfig_marsh(PDM_IMPORT, node_mode);
    		    /* pdm_reconfig_marsh(PDM_ROUTING, mode); */
    		    break;
    		}

	    case PDM_STOP:
   			node_mode = PDM_DO_NOT_MARSH;
        	set_export_pdframe_size(0);
        	pdm_reconfig_marsh(PDM_EXPORT, node_mode);
        	pdm_reconfig_marsh(PDM_IMPORT, node_mode);
        	/* pdm_reconfig_marsh(PDM_ROUTING, PDM_DO_NOT_MARSH); */
        	break;

	    case PDM_CONFIG_INTERNAL:
    	    pdm_reconfig_marsh(PDM_INTERNAL, mode);
        	break;

	    default:
    	    break;
	}

	return (PDM_OK);
}


/*----------------------------------------------------------------------------------------------*/

#endif


/*==============================================================================================*/

