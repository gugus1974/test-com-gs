/*=============================================================================================*/
/* TCN configuration	    														           */
/* Implementation file (conf_tcn.c)													           */
/* 																					           */
/* Version 2.1																		           */
/*																					           */
/* (c)1996 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/* 29.11.2001    ver 2.01           Schiavo                                                    */
/*   - si agg. func nc_mvb_map_cfg()                                                           */
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include <string.h>

#include "conf_tcn.h"
#include "recorder.h"
#include "c_utils.h"
#include "tcn_util.h"

#ifdef ATR_WTB
#include "pdm_lcfg.h"
#include "pdm_main.h"
#include "ll.h"
#endif


/*=============================================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(3)
#endif


/*=============================================================================================*/
/* Macros */

#define ptr2idx(ptr, base) ((char*)ptr - (char*)base)

/* signal a missing element id */
#define eid_error(eid)															\
	nse_record(__FILE__, __LINE__, NSE_EVENT_TYPE_ERROR, NSE_ACTION_CONTINUE,	\
			   sizeof(NcEleID), (UINT8*)&eid)


/*=============================================================================================*/
/* Typedefs */

typedef struct BA_STR_CONFIGURATION BA_TYPE_CONFIGURATION;
typedef struct BA_STR_CYC_IX_PTR_TAB BA_TYPE_CYC_IX_PTR_TAB;

typedef union {						/**** Generic dataset structure (MVB or WTB) ***************/
	MvbDsEntry	mvb;				/* entry for a MVB dataset                                 */
	WtbDsEntry	wtb;				/* entry for a WTB dataset                                 */
} DsEntry;


/*=============================================================================================*/
/* Variables */

char						debug_cfg = 1;	/* if TRUE we are in debug mode */

static const AppStEntry		*app_st_conf;	/* application configuration structure pointer */

static const NcObjHeader	*extern_db;		/* extern NCDB */
static const NcObjHeader	*db;			/* NCDB */

static const NcTcnConf		*tcn_conf;		/* saved ptrs inside the configuration */
static const StEntry		*st;
static unsigned short		st_num;
static const FuncEntry		*func;
static unsigned short		func_num;
static const PdmEntry		*pdm;
static unsigned short		pdm_num;
static const MvbDsEntry		*mvb_ds;
static unsigned short		mvb_ds_num;
static const WtbDsEntry		*wtb_ds;
static unsigned short		wtb_ds_num;


/*=============================================================================================*/
/* List handling routines */

/*---------------------------------------------------------------------------------------------*/
/* Finds an element in a list */

#define find_list_elem(x, y, z) _find_list_elem((const char*)x, sizeof(*x), y, z)

static const char *_find_list_elem(const char *listP, short elem_size, short elem_num,
								   NcEleID elem_id)
{
	if (!listP) return 0;

	while (elem_num-- > 0 && *(NcEleID*)listP) {
		if (*(NcEleID*)listP == elem_id) return listP;
		listP += elem_size;
 	}

	return 0;
}


/*=============================================================================================*/
/* TCN configuration search routines */

/*---------------------------------------------------------------------------------------------*/
static short select_st(NcEleID st_eid, const StEntry **st_h)
{
	short 				err = NC_OK;	/* error code */
	const StEntry		*st_p;			/* generic station entry in the conf */

	/* find the requested station */
	st_p = (const StEntry *)find_list_elem(st, st_num, st_eid);
	if (!st_p) Check(NC_WRONG_CONF_ERR);

	/* save the results */
	if (st_h) *st_h = st_p;

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
static short select_mvb_ds(NcEleID mvb_ds_eid, const MvbDsEntry **mvb_ds_h,
						   const PvEntry **mvb_pv_h, unsigned short *mvb_pv_num_p)
{
	short 				err;			/* error code */
	const MvbDsEntry	*mvb_ds_p;		/* generic MVB dataset entry in the conf */
	const PvEntry		*mvb_pv;		/* process variables object */
	unsigned short		mvb_pv_num;     /* number of process variables in the object */

	/* find the requested MVB dataset */
	mvb_ds_p = (const MvbDsEntry *)find_list_elem(mvb_ds, mvb_ds_num, mvb_ds_eid);
	if (!mvb_ds_p) Check(NC_WRONG_CONF_ERR);

	/* find the pv object */
	Check(nc_get_db_element(mvb_ds_p->pv_oid, (void**)&mvb_pv, &mvb_pv_num));
	mvb_pv_num /= sizeof(PvEntry);

	/* save the results */
	if (mvb_ds_h) *mvb_ds_h = mvb_ds_p;
	if (mvb_pv_h) *mvb_pv_h = mvb_pv;
	if (mvb_pv_num_p) *mvb_pv_num_p = mvb_pv_num;

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
static short select_mvb_pv(NcEleID mvb_pv_eid, const PvEntry *mvb_pv, unsigned short mvb_pv_num,
						   const PvEntry **mvb_pv_h)
{
	short 				err = NC_OK;	/* error code */
	const PvEntry		*mvb_pv_p;		/* generic MVB process variable entry in the conf */

	/* find the requested MVB process variable */
	mvb_pv_p = (const PvEntry *)find_list_elem(mvb_pv, mvb_pv_num, mvb_pv_eid);
	if (!mvb_pv_p) Check(NC_WRONG_CONF_ERR);

	/* save the results */
	if (mvb_pv_h) *mvb_pv_h = mvb_pv_p;

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/


/*=============================================================================================*/
/* Generic private routines */

/*---------------------------------------------------------------------------------------------*/
/* Find an object in an NCDB */

static short nc_get_db_element1(const NcObjHeader *p, unsigned short id, const void **p_element,
								unsigned short *element_size)
{
	/* exit if the DB is not present */
	if (!p) return NC_NOT_FOUND_ERR;

 	/* return NULL if no id is specified */
 	if (!id) {
		if (p_element) *p_element = 0;
		if (element_size) *element_size = 0;
 		return NC_OK;
 	}

	/* end if we find the end of DB */
	while (p->oid != NC_END_OF_DB_OBJ_ID) {

		/* check if the current element is the searched one */
		if (p->oid == id) {
 			if (p_element) *p_element = p+1;
   			if (element_size) *element_size = p->size;
   	 		return NC_OK;
 		}

		/* go to the next one */
  		p = (const NcObjHeader*)(((char*)p) + sizeof(NcObjHeader) + p->size);
	}

	return NC_NOT_FOUND_ERR;
}


/*---------------------------------------------------------------------------------------------*/
/* Generate a PV_NAME from the selected MVB process variable */

static void gen_mvb_pv_name(const MvbDsEntry *mvb_ds_p, const PvEntry *mvb_pv_p,
							struct STR_PV_NAME *pvn)
{
	pvn->type = mvb_pv_p->var_type;
	pvn->size = mvb_pv_p->var_size;
	pvn->bit_offset = mvb_pv_p->var_offset;
	pvn->prt_addr = mvb_ds_p->mf & 0x0FFF;
	pvn->ts_id = NC_MVB_TS_ID;
	/* WARNING: add the check variable offset */
}


/*---------------------------------------------------------------------------------------------*/
/* Generate a PV_NAME_48 from the selected MVB process variable */

#ifdef ATR_WTB

static void gen_mvb_pv_name48(const MvbDsEntry *mvb_ds_p, const PvEntry *pv_p,
							  TYPE_PV_NAME_48 *pvn)
{
	const PvEntry *chk_pv_p = 0;	/* ptr to the check variable */

	/* fill the PV_NAME_48 fields */
	PVN48SETTSID(*pvn, NC_MVB_TS_ID);
	PVN48SETPORT(*pvn, mvb_ds_p->mf & 0x0FFF);
	PVN48SETVAROFF(*pvn, pv_p->var_offset);
	PVN48SETSIZE(*pvn, pv_p->var_size);
	PVN48SETTYPE(*pvn, pv_p->var_type);

	/* set the check variable offset (if any) */
	if (pv_p->chk_eid) nc_get_mvb_el(mvb_ds_p->eid, pv_p->chk_eid, 0, &chk_pv_p, 0, 0, 0);
    if (chk_pv_p) PVN48SETCHKOFF(*pvn, chk_pv_p->var_offset);
    else PVN48SETCHKOFF(*pvn, PDM_CHKVARVOID);
}

#endif


/*---------------------------------------------------------------------------------------------*/
/* Generate a PV_NAME_48 from the selected WTB process variable */

#ifdef ATR_WTB

static void gen_wtb_pv_name48(const WtbDsEntry *wtb_ds_p, const PvEntry *pv_p,
							  TYPE_PV_NAME_48 *pvn)
{
	const PvEntry *chk_pv_p = 0;	/* ptr to the check variable */

	/* fill the PV_NAME_48 fields */
	PVN48SETTSID(*pvn, NC_WTB_TS_ID);
	PVN48SETPORT(*pvn, 0);
	PVN48SETVAROFF(*pvn, pv_p->var_offset);
	PVN48SETSIZE(*pvn, pv_p->var_size);
	PVN48SETTYPE(*pvn, pv_p->var_type);

	/* set the check variable offset (if any) */
	if (pv_p->chk_eid) nc_get_wtb_el(wtb_ds_p->eid, pv_p->chk_eid, 0, &chk_pv_p);
    if (chk_pv_p) PVN48SETCHKOFF(*pvn, chk_pv_p->var_offset);
    else PVN48SETCHKOFF(*pvn, PDM_CHKVARVOID);
}

#endif


/*=============================================================================================*/
/* Configuration handling */

/*---------------------------------------------------------------------------------------------*/
short nc_set_app_st_conf(const AppStEntry *app_st_list, NcEleID st_eid)
{
	app_st_conf = (const AppStEntry *)find_list_elem(app_st_list, 9999, st_eid);
	if (!app_st_conf) return NC_NOT_FOUND_ERR;
	return NC_OK;
}


/*---------------------------------------------------------------------------------------------*/
short nc_set_db(const void *extern_nc_db, const void *nc_db)
{
	short 				err;			/* error code */

	/* set the DB ptrs */
	extern_db = extern_nc_db;
	if (extern_db && extern_db->oid != NC_START_OF_DB_OBJ_ID) extern_db = 0;
	db = nc_db;
	if (db && db->oid != NC_START_OF_DB_OBJ_ID) db = 0;

	/* set the ptrs to the main objects */
	Check(nc_get_db_element(NC_TCN_CONF_OBJ_ID, (void**)&tcn_conf, 0));
	Check(nc_get_db_element(tcn_conf->st_oid, (void**)&st, &st_num));
	st_num /= sizeof(StEntry);
	Check(nc_get_db_element(tcn_conf->func_oid, (void**)&func, &func_num));
	func_num /= sizeof(FuncEntry);
	Check(nc_get_db_element(tcn_conf->pdm_oid, (void**)&pdm, &pdm_num));
	pdm_num /= sizeof(PdmEntry);
	Check(nc_get_db_element(tcn_conf->mvb_ds_oid, (void**)&mvb_ds, &mvb_ds_num));
	mvb_ds_num /= sizeof(MvbDsEntry);
	Check(nc_get_db_element(tcn_conf->wtb_ds_oid, (void**)&wtb_ds, &wtb_ds_num));
	wtb_ds_num /= sizeof(WtbDsEntry);

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short nc_get_db_element(unsigned short id, const void **p_element, unsigned short *element_size)
{
	short err;	/* error code */

	if (extern_db) err = nc_get_db_element1(extern_db, id, p_element, element_size);
	else err = NC_NOT_FOUND_ERR;
	if (err && db) err = nc_get_db_element1(db, id, p_element, element_size);

	return err;
}


/*=============================================================================================*/
/* Utility routines */

/*---------------------------------------------------------------------------------------------*/
short nc_mvb_address(NcEleID st_eid, unsigned short *address)
{
	short 				err = NC_OK;	/* error code */
	const StEntry		*st_p;			/* generic station entry in the conf */

	/* 0 is supposed to be our station */
	if (!st_eid) st_eid = app_st_conf->st_eid;

	/* find the requested station */
	st_p = (const StEntry *)find_list_elem(st, st_num, st_eid);
	if (!st_p) Check(NC_WRONG_CONF_ERR);

	/* save its MVB address */
	if (address) *address = st_p->mvb_address;

error:
	return err;
}


/*=============================================================================================*/
/* MVB traffic store configuration */

/*---------------------------------------------------------------------------------------------*/
short nc_mvb_ts_cfg(TYPE_LP_TS_CFG **ts_cfg_h)
{
	short				err = 0;			/* error code */

	short				ds_idx;				/* dataset index */

	const MvbDsEntry	*mvb_ds_p;			/* generic MVB dataset entry in the conf */
	const AppDsEntry	*app_ds_p;			/* generic application dataset ptr */

	TYPE_LP_TS_CFG		*ts_cfg;			/* traffic store configuration */
	TYPE_LP_PRT_CFG 	*prt_cfg = 0;		/* port configuration */

	short				pi_max = 0;			/* port index max */
	short				pa_max = 0;			/* port address max */
	short				pa_num = 0;			/* port address number */

	/* memory allocation */
	ts_cfg = (TYPE_LP_TS_CFG*)pi_alloc(sizeof(TYPE_LP_TS_CFG));
	if (!ts_cfg) Check(NC_MEM_ERR);
	prt_cfg = (TYPE_LP_PRT_CFG*)pi_alloc(mvb_ds_num * sizeof(*prt_cfg));
	if (!prt_cfg) Check(NC_MEM_ERR);

	for (ds_idx = 0; !nc_get_idx_mvb_ds(ds_idx, &mvb_ds_p, &app_ds_p); ds_idx++) {

		/* skip the dataset if we are not interested in it */
		if (!debug_cfg && !app_ds_p) continue;

		/* configure the port type */
		if (mvb_ds_p->src_station_eid == app_st_conf->st_eid) prt_cfg[pa_num].type = LP_CFG_SRCE;
		else prt_cfg[pa_num].type = LP_CFG_SINK;

		/* fill the other port fields */
		prt_cfg[pa_num].prt_addr = mvb_ds_p->mf & 0x0FFF;
 		if (!app_ds_p || !(app_ds_p->flags & NC_APP_DS_SAME_PI)) pi_max += 4;
		prt_cfg[pa_num].prt_indx = pi_max;
		switch (mvb_ds_p->mf & 0xF000) {
 			case 0x0000: prt_cfg[pa_num].size = LP_CFG_02_BYTES; break;
 			case 0x1000: prt_cfg[pa_num].size = LP_CFG_04_BYTES; break;
 			case 0x2000: prt_cfg[pa_num].size = LP_CFG_08_BYTES; break;
 			case 0x3000: prt_cfg[pa_num].size = LP_CFG_16_BYTES; break;
 			case 0x4000: prt_cfg[pa_num].size = LP_CFG_32_BYTES; break;
		}

		/* update the max port address used */
		if (prt_cfg[pa_num].prt_addr > pa_max) pa_max = prt_cfg[pa_num].prt_addr;

		/* increment the number of port addresses used */
		pa_num++;
	}

	/* build the traffic store configuration */
	ts_cfg->pb_pit       = 0;
	ts_cfg->pb_pcs       = 0;
	ts_cfg->pb_prt       = 0;
	ts_cfg->pb_frc       = 0;
	ts_cfg->pb_def       = 0;
	ts_cfg->pb_mwd       = 0;
	ts_cfg->ownership    = LP_CFG_TS_OWNED;
	ts_cfg->ts_type      = LP_TS_TYPE_B;
	ts_cfg->prt_addr_max = pa_max | LP_PRT_ADDR_MAX_MASK;
	ts_cfg->prt_indx_max = pi_max | LP_PRT_INDX_MAX_MASK;
	ts_cfg->prt_count    = pa_num;
	ts_cfg->p_prt_cfg    = prt_cfg;

	/* save the results and exit */
	*ts_cfg_h = ts_cfg;
	return err;

error:
	/* deallocate the memory on error */
	if (ts_cfg) pi_free((void*)ts_cfg);
	if (prt_cfg) pi_free((void*)prt_cfg);
	return err;
}


/*=============================================================================================*/
/* Dataset configuration */

/*---------------------------------------------------------------------------------------------*/
short nc_mvb_ds_cfg(void)
{
	short					err = 0;		/* error code */

	const MvbDsEntry		*mvb_ds_p;		/* generic MVB dataset entry in the conf */
	const PvEntry			*mvb_pv;		/* process variables object */
	unsigned short			mvb_pv_num;     /* number of process variables in the object */
	const PvEntry			*mvb_pv_p;		/* generic MVB process variable entry in the conf */

	const AppDsEntry		*app_ds_p;		/* generic application dataset ptr */
	const AppPvEntry		*app_pv_p;		/* generic application process variable ptr */

	struct STR_LP_PV_X		*pv_x;			/* ptr to array of pv descriptions */
	struct STR_LP_PV_SET	*pv_set;		/* ptr to array of pv sets */
	struct STR_LP_PV_SET	*pv_set_p;		/* generic pv set pointer */

	short					app_pv_num;		/* number of application process variables */

	/* loop for all the application's used MVB datasets */
	for (app_ds_p = (const AppDsEntry*)app_st_conf->app_ds_list; app_ds_p->ds_eid != 0;
		 app_ds_p++) if (app_ds_p->p_ds_set) {

		/* find the dataset in the configuration */
		err = select_mvb_ds(app_ds_p->ds_eid, &mvb_ds_p, &mvb_pv, &mvb_pv_num);
 		if (err) {
			if (app_ds_p->flags & NC_APP_DS_CAN_MISS) {
				err = 0;
				continue;
			}
			else {
		 		eid_error(app_ds_p->ds_eid);
 				Check(err);
 			}
 		}

		/* count the number of process variables used by the application */
		for (app_pv_num = 0, app_pv_p = (const AppPvEntry *)app_ds_p->app_pv_list;
 			 app_pv_p->pv_eid; app_pv_p++) if (app_pv_p->p_data) app_pv_num++;

		/* memory allocation */
		pv_x = (struct STR_LP_PV_X *)pi_alloc(app_pv_num * sizeof(*pv_x));
		if (!pv_x) Check(NC_MEM_ERR);
		pv_set = (struct STR_LP_PV_SET *)pi_alloc(app_pv_num * sizeof(*pv_set));
		if (!pv_set) Check(NC_MEM_ERR);

        /* create the pv sets */
		for (app_pv_p = (const AppPvEntry *)app_ds_p->app_pv_list, pv_set_p = pv_set;
 			 app_pv_p->pv_eid != 0; app_pv_p++) if (app_pv_p->p_data) {

			/* find the process variable in the configuration */
  			err = select_mvb_pv(app_pv_p->pv_eid, mvb_pv, mvb_pv_num, &mvb_pv_p);
	 		if (err) {
				if (app_ds_p->flags & NC_APP_DS_CAN_MISS) {
					app_pv_num--;
					err = 0;
					continue;
				}
				else {
		 			eid_error(app_pv_p->pv_eid);
 					Check(err);
 				}
	 		}

			/* fill the pv set fields and go to the next one */
   	  		gen_mvb_pv_name(mvb_ds_p, mvb_pv_p, &pv_set_p->pv_name);
   	   		pv_set_p->p_var = app_pv_p->p_data;
   	   		pv_set_p++;
 		}

		/* generate the dataset */
		lp_gen_ds_set(app_ds_p->p_ds_set, pv_x, pv_set, app_ds_p->p_refresh, app_pv_num);

		/* fix the dataset flags */
 		if (debug_cfg || (app_ds_p->flags & NC_APP_DS_FORCE))
  			app_ds_p->p_ds_set->ds_y.with_frce = LP_VAR_WITH_FRCE;
  		else app_ds_p->p_ds_set->ds_y.with_frce = LP_VAR_WITHOUT;
 		if (debug_cfg || (app_ds_p->flags & NC_APP_DS_ELSE))
  			app_ds_p->p_ds_set->ds_y.with_else = LP_VAR_WITH_ELSE;
  		else app_ds_p->p_ds_set->ds_y.with_else = LP_VAR_WITHOUT;

		/* deallocate the pv set memory */
		pi_free((char*)pv_set);
	}

error:
	return err;
}
short nc_mvb_map_cfg(void)
{
    const StEntry           *st_p;                  /* generic station entry in the conf */

    select_st(app_st_conf->st_eid, &st_p);
    if (st_p->flags & NC_ST_NEEDS_NADI)
        return(1);
    else
        return(0);
}


/*=============================================================================================*/
/* Bus administrator configuration */

/*---------------------------------------------------------------------------------------------*/
/* Extracts the cycle and subcycle informations */

static void get_cyc_sub(unsigned short cyc_sub, short *cycb, short *sub)
{
	short		   i;
	unsigned short t;

	for (i = 15, t = BIT(15); !(cyc_sub & t); t >>= 1, i--);

	if (cycb) *cycb = i;
	if (sub) *sub = cyc_sub ^ BIT(i);
}


/*---------------------------------------------------------------------------------------------*/
/* Put a value in the split list */

static void put_split(BA_TYPE_CYC_IX_PTR_TAB *ixP, short cyc_sub, unsigned char count)
{
	short			cycb, sub;
	unsigned char	*cP;
	short			off;

    get_cyc_sub(cyc_sub, &cycb, &sub);
	if (cycb == 0) return;

	cP = (unsigned char*)ixP + ixP->i_sl[(cycb-1) / 2];
	off = sub*2 + ((cycb-1) & 1);
	cP[off] = count;
	if (cycb & 1) cP[BIT(cycb)*2 + off] = count;
}


/*---------------------------------------------------------------------------------------------*/
/* Output the mfs included in the min<=cyc_sub<max range */

static short *put_mf(short min, short max, short *mfP, BA_TYPE_CYC_IX_PTR_TAB *ixP)
{
	short			cur = min;				/* current cyc_sub value */
	short			nxt;					/* next cyc_sub value */
	unsigned char	mf_count;				/* mf counter */
	unsigned short	i;						/* index */

	while (cur < max) {
		nxt = 0x7FFF;
 		mf_count = 0;
		for (i = 0; i < mvb_ds_num; i++) {
    		if (mvb_ds[i].cyc_sub == cur) {
      			*mfP++ = mvb_ds[i].mf;
       			mf_count++;
    		}
	    	else if (mvb_ds[i].cyc_sub > cur && mvb_ds[i].cyc_sub < nxt) nxt = mvb_ds[i].cyc_sub;
     	}
     	if (mf_count) put_split(ixP, cur, mf_count);
     	cur = nxt;
	}

	return mfP;
}


/*---------------------------------------------------------------------------------------------*/
short nc_mvb_ba_cfg(struct BA_STR_CONFIGURATION **ba_cfg_h)
{
	short					err = 0;				/* error code */

	const StEntry			*st_p;					/* generic station entry in the conf */

	unsigned short			i;						/* generic index */
	short					bus_admin_dev_num = 0;	/* number of bus administrator devices */
	BA_TYPE_CONFIGURATION	*confP = 0;				/* configuration pointer */
	unsigned char			*cP, *cP2;				/* generic (current) char pointer */
	BA_TYPE_CYC_IX_PTR_TAB	*ixP;					/* cycle index pointer table */

	/* find our station in the configuration */
	select_st(app_st_conf->st_eid, &st_p);

	/* exit if we are not supposed to be Bus Administrators */
	if (!(st_p->flags & NC_ST_IS_MVB_BA)) goto exit;

	/* calculate the bus_admin_dev_num counter */
	for (i = 0; i < st_num; i++) if (st[i].flags & NC_ST_IS_MVB_BA) bus_admin_dev_num++;

	/* allocate the memory */
	confP = (BA_TYPE_CONFIGURATION*)pi_alloc(sizeof(*confP) + st_num*2 + sizeof(*ixP) +
											 mvb_ds_num*2 + (4+16+64+256+1024)*2 +
   	  				 						 bus_admin_dev_num*2 + 1024);
	if (!confP) Check(NC_MEM_ERR);

	/* fill-in the configuration fields */
	confP->nu0                = 0;            				/* not used                     */
	confP->mtvc               = tcn_conf->mtvc |			/* master transfer version ctrl */
		((st_p->flags & NC_ST_MVB_BA_PRIO_MASK) >> NC_ST_MVB_BA_PRIO_OFF);
	confP->t_reply_max        = tcn_conf->t_reply_max;		/* 42 or 64 us                  */
	confP->c_periods_in_macro = 1024;						/* periods per macro (1024/512) */
	confP->eps                = tcn_conf->ev_poll_strategy;	/* event poll strategy          */
	confP->basic_period       = tcn_conf->basic_period;		/* in us!                       */
	confP->c_macros_in_turn   = tcn_conf->c_macros_in_turn;	/* macro cycles per turn        */
	confP->scan_strategy      = tcn_conf->scan_strategy;    /* 0/1: known/all devices       */
	pi_zero16((int*)&confP->nu1, 4);		    			/* not used                     */

	cP = (unsigned char *)(confP + 1);

	/* build the known devices list */
	confP->i_kdl = ptr2idx(cP, confP);          /* index p device addr list     */
	for (i = 0; i < st_num; i++, cP += 2) *(short*)cP = st[i].mvb_address | 0xF000;

	confP->i_nu1 = ptr2idx(cP, confP);          /* not used                     */

	/* build the cycle index table */
	confP->i_ctl = ptr2idx(cP, confP);          /* index p cyclic transm list   */
    ixP = (BA_TYPE_CYC_IX_PTR_TAB*)cP;
	cP += sizeof(BA_TYPE_CYC_IX_PTR_TAB);
	i = mvb_ds_num*2;							/* @@@ workaround for C166 bug */
	cP2 = cP + i;
	pi_zero16((int*)cP2, 4+16+64+256+1024);
	ixP->i_sl[0] = ptr2idx(cP2, ixP); cP2 += 4*2;
	ixP->i_sl[1] = ptr2idx(cP2, ixP); cP2 += 16*2;
	ixP->i_sl[2] = ptr2idx(cP2, ixP); cP2 += 64*2;
	ixP->i_sl[3] = ptr2idx(cP2, ixP); cP2 += 256*2;
	ixP->i_sl[4] = ptr2idx(cP2, ixP); cP2 += 1024*2;
	for (i = 0; i <= 10; i++) {
		ixP->i_cl[i] = ptr2idx(cP, ixP);
		cP = (unsigned char *)put_mf(BIT(i), BIT(i+1), (short*)cP, ixP);
	}
	cP = cP2;

	/* build the bus administrators devices list */
	confP->i_bal = ptr2idx(cP, confP);          /* index bus admin addr list    */
	for (i = 0; i < st_num; i++) if (st[i].flags & NC_ST_IS_MVB_BA) {
		*(short*)cP = st[i].mvb_address | 0x8000;
 		cP += 2;
	}

	/* build the scan list */
	confP->i_snl = ptr2idx(cP, confP);			/* index scan list              */
    for (i = 0; i < 1024; i++, cP++) *cP = ((i % 8) == 7) ? 1 : 0;

	/* mark the end of configuration */
	confP->i_eol = ptr2idx(cP, confP);          /* index p end of list mark     */

exit:
	/* save the results and exit */
	*ba_cfg_h = confP;
	return err;

error:
	/* deallocate the memory on error */
	if (confP) pi_free((void*)confP);
	return err;
}


/*=============================================================================================*/
/* Sink timeout supervision configuration */

/*---------------------------------------------------------------------------------------------*/
short nc_tmo_cfg(TYPE_LP_TMO_CLUS **tmo_cfg_h, unsigned char bottom_address, unsigned char top_address)
{
	short					err = 0;			/* error code */

	short					ds_idx;				/* dataset index */

	const MvbDsEntry		*mvb_ds_p;			/* generic MVB dataset entry in the conf */
	const AppDsEntry		*app_ds_p;			/* generic application dataset ptr */

	short					mvb_prt_count = 0;	/* MVB port count for sink timeout sup. */
	short					wtb_prt_count;		/* WTB port count for sink timeout sup. */
	TYPE_LP_TMO_CLUS		*tmo_clus;	  		/* s.t.s. header */
	TYPE_LP_TMO_CLUS_TS		*tmo_clus_ts;		/* s.t.s. traffic store header */
	TYPE_DS_NAME			*ds_names;	  		/* dataset names for sink timeout sup. */
	char					*cP;				/* current pointer in the configuration */
	int						i;					/* generic index */

	/* calculate the MVB prt_count for sink timeout supervision */
	for (ds_idx = 0; !nc_get_idx_mvb_ds(ds_idx, &mvb_ds_p, &app_ds_p); ds_idx++)
		if (debug_cfg || (app_ds_p && (app_ds_p->flags & NC_APP_DS_TMO))) mvb_prt_count++;

	/* calculate the WTB prt_count for sink timeout supervision */
	if (bottom_address && top_address) {
		if (bottom_address > 32) wtb_prt_count = (64 - bottom_address) + top_address + 1;
		else wtb_prt_count = top_address - bottom_address + 2;
	}
	else wtb_prt_count = 0;

 	/* allocate the memory for the timeout data */
 	tmo_clus = (TYPE_LP_TMO_CLUS*)pi_alloc(sizeof(TYPE_LP_TMO_CLUS) +
				(mvb_prt_count ? sizeof(TYPE_LP_TMO_CLUS_TS) : 0) +
				(wtb_prt_count ? sizeof(TYPE_LP_TMO_CLUS_TS) : 0) +
				(mvb_prt_count + wtb_prt_count) * sizeof(TYPE_DS_NAME));
 	if (!tmo_clus) Check(NC_MEM_ERR);

 	/* initialize the header and the current pointer */
	tmo_clus->ts_count = 0;
	tmo_clus->reserved = 0;
 	cP = (char*)(tmo_clus+1);

	if (mvb_prt_count) {

		/* set the structures' pointers */
		tmo_clus_ts = (TYPE_LP_TMO_CLUS_TS *)cP;
		cP += sizeof(TYPE_LP_TMO_CLUS_TS);
		ds_names = (TYPE_DS_NAME *)cP;
		cP += mvb_prt_count * sizeof(TYPE_DS_NAME);

		/* add the traffic store to the list */
		tmo_clus->p_clus_ts[tmo_clus->ts_count++] = tmo_clus_ts;

		/* fill the traffic store header */
		tmo_clus_ts->ts_id     = NC_MVB_TS_ID;
		tmo_clus_ts->prt_count = mvb_prt_count;
		tmo_clus_ts->p_ds_name = ds_names;

		/* fill the ds_names array */
		for (ds_idx = 0; !nc_get_idx_mvb_ds(ds_idx, &mvb_ds_p, &app_ds_p); ds_idx++)
   			if (debug_cfg || (app_ds_p && (app_ds_p->flags & NC_APP_DS_TMO))) {
				ds_names->prt_addr = mvb_ds_p->mf & 0x0FFF;
				ds_names->ts_id = NC_MVB_TS_ID;
				ds_names++;
			}
	}

	if (wtb_prt_count) {

		/* set the structures' pointers */
		tmo_clus_ts = (TYPE_LP_TMO_CLUS_TS *)cP;
		cP += sizeof(TYPE_LP_TMO_CLUS_TS);
		ds_names = (TYPE_DS_NAME *)cP;
		cP += wtb_prt_count * sizeof(TYPE_DS_NAME);

		/* add the traffic store to the list */
		tmo_clus->p_clus_ts[tmo_clus->ts_count++] = tmo_clus_ts;

		/* fill the traffic store header */
		tmo_clus_ts->ts_id     = NC_WTB_TS_ID;
		tmo_clus_ts->prt_count = wtb_prt_count;
		tmo_clus_ts->p_ds_name = ds_names;

		/* fill the ds_names array */
		if (bottom_address > 32) for (i = bottom_address; i < 64; i++, ds_names++) {
			ds_names->prt_addr = i;
			ds_names->ts_id = NC_WTB_TS_ID;
		}
		for (i = 0; i <= top_address; i++, ds_names++) {
			ds_names->prt_addr = i;
			ds_names->ts_id = NC_WTB_TS_ID;
		}
	}

	/* set the timeout configuration */
	*tmo_cfg_h = tmo_clus;

error:
	return err;
}


/*=============================================================================================*/
/* Messenger function directory configuration */

/*---------------------------------------------------------------------------------------------*/
short nc_func_cfg(AM_DIR_ENTRY **am_dir_h, short *am_dir_size)
{
	short					err = 0;			/* error code */
	const StEntry			*st_p;				/* generic station entry in the conf */
	AM_DIR_ENTRY			*am_dir;			/* function directory */
	unsigned short			i;					/* index */

 	/* allocate the memory for the function directory */
 	am_dir = (AM_DIR_ENTRY*)pi_alloc(func_num * sizeof(AM_DIR_ENTRY));
 	if (!am_dir) Check(NC_MEM_ERR);

	/* create the function directory */
	for (i = 0; i < func_num; i++) {
		am_dir[i].function = func[i].func;
		Check(select_st(func[i].st_eid, &st_p));
		am_dir[i].device = st_p->mvb_address;
	}

	/* save the results */
	*am_dir_h = am_dir;
	*am_dir_size = func_num;

error:
	return err;
}


/*=============================================================================================*/
/* MVBC redundancy routines */

/*---------------------------------------------------------------------------------------------*/
short nc_mvbc_red_cfg(MvbcRedConfHeader **mvbc_red_cfg_h)
{
	short				err = 0;	/* error code                         */
	MvbcRedConfHeader	*cfg = 0;	/* configuration header pointer       */
	MvbcRedDevEntry		*cfg_dev;	/* configuration device array pointer */
	const StEntry		*st_p;		/* generic station entry in the conf  */
	int					i, j;		/* generic indexes                    */
	short				mf;			/* master frame of a dataset          */
	short				cyc_sub;	/* cycle+subcycle of a dataset        */
	short				b_cyc_sub;	/* biggest cycle+subcycle found       */

	/* find our station in the configuration and exit if we are not supposed to be Bus Administrators */
	select_st(app_st_conf->st_eid, &st_p);
	if (!(st_p->flags & NC_ST_IS_MVB_BA)) goto exit;

	/* allocate the configuration structure */
 	cfg = (MvbcRedConfHeader*)pi_alloc(sizeof(MvbcRedConfHeader) + st_num * sizeof(MvbcRedDevEntry));
 	if (!cfg) Check(NC_MEM_ERR);
 	cfg_dev = (MvbcRedDevEntry*)(cfg + 1);

	/* loop for each station in the configuration */
	for (i = 0, b_cyc_sub = 0; i < st_num; i++) {

		/* find the fastest dataset produced by the station */
		for (j = 0, mf = 0xF000 | st[i].mvb_address, cyc_sub = 4096; j < mvb_ds_num; j++) {
			if (mvb_ds[j].src_station_eid == st[i].eid && mvb_ds[j].cyc_sub < cyc_sub) {
				mf      = mvb_ds[j].mf;
				cyc_sub = mvb_ds[j].cyc_sub;
			}
		}

		/* fill the device's record */
		cfg_dev[i].mvb_address = st[i].mvb_address;
		cfg_dev[i].mf          = mf;

		/* update the biggest cyc_sub field found */
		if (cyc_sub > b_cyc_sub) b_cyc_sub = cyc_sub;
	}

	/* fill the configuration header */
	get_cyc_sub(b_cyc_sub, &b_cyc_sub, 0);
	cfg->timeout = (1 << b_cyc_sub) * 4;
	cfg->n_dev   = st_num;

exit:
	/* save the result */
	if (mvbc_red_cfg_h) *mvbc_red_cfg_h = cfg;

error:
	return err;
}


/*=============================================================================================*/
/* Process Data Marshalling routines */

#ifdef ATR_WTB

/*---------------------------------------------------------------------------------------------*/

typedef struct {						/**** Information about a list of variables ************/
	char				ts_id;			/* traffic store id                                    */
	short				d_ds_num; 		/* number of different datasets definitions            */
	const DsEntry		**d_ds_list;	/*   list of different datasets                        */
	short				*d_ds_c_list;	/*   count of pvs for each dataset (*)                 */
	short				num;	 		/* number of process variables definitions             */
	const PvEntry		**pv_list;		/*   list of process variables                         */
	const DsEntry		**ds_list;		/*   list of associated datasets                       */
	unsigned short		*chkov_list;	/*   list of check variables PDM overlay info (*)      */
	short				*pdm_idx_list;	/*   index of the pv for the PDM ("from" and func.)    */
} VarInfo;

typedef struct {						/**** Info collected for each marshalling list *********/
	const PdmActEntry	*pdm_act;		/* ptr to the PDM actions object                       */
	unsigned short		pdm_act_num;	/* number of elements in the PDM actions object        */
	const void			*dflt_value;	/* ptr to the default values object                    */
	unsigned short		dflt_value_size;/* number of bytes in the default values object        */
	short				*act_pv_list;	/* index for the corresponding pv from the act list    */
	VarInfo				from;			/* information about the "from" variables              */
	VarInfo				to;				/* information about the "to" variables                */
	short				extra_s_dir_ds;	/* number of extra dataset required for the s-dir copy */
	short				func_num; 		/* number of "functions" definitions                   */
	short				func_arg_num; 	/* number of arguments to the "functions"              */
	unsigned short		list_len;		/* length of the PDM marshalling list                  */
} MarshListInfo;


/*---------------------------------------------------------------------------------------------*/
static short pdm_var_info_sort(VarInfo *vi_p)
{
	short				err = 0;			/* error code                                    */
	short				i, j, k, ix, iy;    /* generic indexes and counters                  */
	short				n = 0;				/* number of process variables actually included */
	const PvEntry		**pv = 0;			/*   list of process variables                   */
	const DsEntry		**ds = 0;			/*   list of associated datasets                 */

	/* allocate the sorted lists */
	if (!C_array_new(vi_p->d_ds_c_list, vi_p->d_ds_num)) Check(NC_MEM_ERR);
	pi_zero8((void*)vi_p->d_ds_c_list, vi_p->d_ds_num * sizeof(*vi_p->d_ds_c_list));
	if (!C_array_new(pv, vi_p->num)) Check(NC_MEM_ERR);
	if (!C_array_new(ds, vi_p->num)) Check(NC_MEM_ERR);
	if (!C_array_new(vi_p->chkov_list, vi_p->num)) Check(NC_MEM_ERR);

	/* sort the different datasets by port address for MVB or FTF for WTB (not really needed) */
	for (i = 0; i < vi_p->d_ds_num; i++) {
		for (j = i+1; j < vi_p->d_ds_num; j++) {
			if (vi_p->ts_id == NC_MVB_TS_ID &&
  			    (vi_p->d_ds_list[i]->mvb.mf & 0x0FFF) > (vi_p->d_ds_list[j]->mvb.mf & 0x0FFF) ||
			    vi_p->ts_id == NC_WTB_TS_ID && vi_p->d_ds_list[i]->wtb.ftf > vi_p->d_ds_list[j]->wtb.ftf) {
				const DsEntry *t = vi_p->d_ds_list[i];
  				vi_p->d_ds_list[i] = vi_p->d_ds_list[j];
   				vi_p->d_ds_list[j] = t;
   			}
		}
	}

	/* loop for each different dataset */
	for (i = 0; i < vi_p->d_ds_num; i++) {

		/* save the first index for this dataset */
 		ix = n;

		/* collect all the process variables without a check variable */
		for (j = 0; j < vi_p->num; j++) {
 			if (vi_p->ds_list[j] == vi_p->d_ds_list[i] && !vi_p->pv_list[j]->chk_eid) {
 				pv[n] = vi_p->pv_list[j];
 				ds[n] = vi_p->ds_list[j];
   				vi_p->chkov_list[n] = PDM_CHKUNIQUE;
   				n++;
   	   			vi_p->d_ds_c_list[i]++;
   	  			vi_p->ds_list[j] = 0;
			}
  		}

		/* collect all the process variables with an unique check variable */
		for (j = 0; j < vi_p->num; j++) {
 			if (vi_p->ds_list[j] == vi_p->d_ds_list[i]) {
				for (k = ix; k < n; k++)
   					if (vi_p->pv_list[j]->chk_eid == pv[k]->eid ||
   	   					vi_p->pv_list[j]->chk_eid == pv[k]->chk_eid) break;
				if (k == n) {
	 				pv[n] = vi_p->pv_list[j];
 					ds[n] = vi_p->ds_list[j];
   					vi_p->chkov_list[n] = PDM_CHKUNIQUE;
   					n++;
	   	   			vi_p->d_ds_c_list[i]++;
	   	  			vi_p->ds_list[j] = 0;
   	   			}
			}
  		}

		/* collect all the process variables with a check variable overlayed with a variable */
		for (j = 0; j < vi_p->num; j++) {
 			if (vi_p->ds_list[j] == vi_p->d_ds_list[i]) {
				for (k = ix; k < n; k++) {
   					if (vi_p->pv_list[j]->chk_eid == pv[k]->eid) {
		 				pv[n] = vi_p->pv_list[j];
 						ds[n] = vi_p->ds_list[j];
   						vi_p->chkov_list[n] = PDM_CHKOVERLAYVAR | k;
   						n++;
		   	   			vi_p->d_ds_c_list[i]++;
	   	  				vi_p->ds_list[j] = 0;
       				    break;
					}
   	   			}
			}
  		}

		/* collect all the process variables with a check variable orverlayed with a check variable */
		for (j = 0; j < vi_p->num; j++) {
 			if (vi_p->ds_list[j] == vi_p->d_ds_list[i]) {
				for (k = ix; k < n; k++) {
   					if (vi_p->pv_list[j]->chk_eid == pv[k]->chk_eid) {
		 				pv[n] = vi_p->pv_list[j];
 						ds[n] = vi_p->ds_list[j];
   						vi_p->chkov_list[n] = PDM_CHKOVERLAYCHK | k;
   						n++;
		   	   			vi_p->d_ds_c_list[i]++;
	   	  				vi_p->ds_list[j] = 0;
       				    break;
					}
   	   			}
			}
  		}

	}

	/* change the old process variable lists */
	pi_free((void*)vi_p->pv_list);
	pi_free((void*)vi_p->ds_list);
	vi_p->pv_list = pv;
	vi_p->ds_list = ds;

	return err;

error:
	pi_free((void*)pv);
	pi_free((void*)ds);
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short nc_pdm_cfg(void **pdm_cfg_h, unsigned short *ftf_size)
{
	const StEntry		*st_p;				/* generic station entry in the conf */
	MarshListInfo		*mli = 0;			/* marshalling list info */
	short				err = 0;			/* error code */
	short				i, j, k, ix, iy;   	/* generic indexes and counters */
	void				*pdm_cfg = 0;		/* allocated PDM configuration table */
	unsigned short		pdm_cfg_len;		/* length of the PDM configuration table */
	char				*cP;				/* ptr used to move inside the PDM table */
	VarInfo				*vi_p;				/* generic ptr to a var info structure */
	const DsEntry		*ds_p;				/* generic dataset entry in the conf */
	const PvEntry		*pv_p;				/* generic process variable entry in the conf */

	/* find our station in the configuration and exit if we are not supposed to be a gateway */
	select_st(app_st_conf->st_eid, &st_p);
	if (!(st_p->flags & NC_ST_IS_GATEWAY)) goto error;

	/* set the FTF size */
	if (ftf_size) *ftf_size = (tcn_conf->flags & NC_CONF_8_BIT_FTF) ? 1 : 2;

	/* allocate and zero the marshalling list info storage */
	mli = (void*)pi_alloc(pdm_num * sizeof(*mli));
	if (!mli) Check(NC_MEM_ERR);
	pi_zero8((void*)mli, pdm_num * sizeof(*mli));

	/* initialize the total length of the PDM configuration table */
	pdm_cfg_len = sizeof(T_MDB_DB_HDR) + sizeof(T_MDB_SUB_HDR);

	/* do a preflight to collect all the relevant information */
	for (i = 0; i < pdm_num; i++) {

		/* find the action and default values objects */
		Check(nc_get_db_element(pdm[i].pdm_act_oid, (void**)&mli[i].pdm_act, &mli[i].pdm_act_num));
		mli[i].pdm_act_num /= sizeof(PdmActEntry);
		Check(nc_get_db_element(pdm[i].dflt_value_oid, &mli[i].dflt_value, &mli[i].dflt_value_size));

		/* initialize the "from" and "to" traffic stores ids */
  		if (pdm[i].type == NC_PDM_IMPORT) mli[i].from.ts_id = NC_WTB_TS_ID, mli[i].to.ts_id = NC_MVB_TS_ID;
   		else if (pdm[i].type == NC_PDM_EXPORT) mli[i].from.ts_id = NC_MVB_TS_ID, mli[i].to.ts_id = NC_WTB_TS_ID;
   		else mli[i].from.ts_id = NC_MVB_TS_ID, mli[i].to.ts_id = NC_MVB_TS_ID;

  		/* allocate the lists storage */
   		if (!C_array_new(mli[i].act_pv_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].from.d_ds_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].from.pv_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].from.ds_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
		if (!C_array_new(mli[i].from.pdm_idx_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].to.d_ds_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].to.pv_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
   		if (!C_array_new(mli[i].to.ds_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);
		if (!C_array_new(mli[i].to.pdm_idx_list, mli[i].pdm_act_num)) Check(NC_MEM_ERR);

		/* count the entries in each list */
		for (j = 0, k = 0; j < mli[i].pdm_act_num; j++)
		{

			if (mli[i].pdm_act[j].act == NC_PDM_SOURCE) {
  				vi_p = &mli[i].from;	/* it is a "from" variable */
				k++;	/* increment the count of source variables for this "operation" */
   	 		}
			else
			{
  				vi_p = &mli[i].to;		/* it is a "to" variable */

				/* if the result is a function update the number of functions & arguments */
				if (mli[i].pdm_act[j].act == NC_PDM_FUNC)
				{
   					mli[i].func_num++;
   					mli[i].func_arg_num += k;
   	   			}

				/* if it is a source directory copy calculate the number of required ports */
				if (mli[i].pdm_act[j].act == NC_PDM_FUNC && mli[i].pdm_act[j].param1 == NC_PDM_FUNC_S_DIR ||
   					mli[i].pdm_act[j].act == NC_PDM_COPY && mli[i].pdm_act[j].param1 == NC_PDM_COPY_S_DIR) {

					short 				size;
				   	struct STR_PV_NAME	pvn;

					/* get the destination variable (it must be a MVB variable) */
   					Check(nc_get_mvb_el(0, mli[i].pdm_act[j].pv_eid, 0, &pv_p, 0, 0, &pvn));

					/* calculate the number of dataset required to store it */
   					if (pv_p->var_size == LP_SIZE_BITS && (pv_p->var_type == LP_TYPE_BOOL_1 ||
   	   					pv_p->var_type == LP_TYPE_BOOL_2 || pv_p->var_type == LP_TYPE_BCD_4)) size = 1;
   	   		        else
   	   		        {
						size = lp_pv_size(&pvn);
	   	   				if (!pv_p->var_offset)
	   	   				{
	   	   					//printf("size pv %d\n",size);
	   	   					size = (mli[i].pdm_act[j].param2 + (32 / size) - 1) / (32 / size) + 1;
	   	   					//printf("size %d\n",size);
   		   			    }
   		   				else size = (mli[i].pdm_act[j].param2 + (24 / size) - 1) / (24 / size);
   	    		    }

					/* update the count of extra datasets required */
					mli[i].extra_s_dir_ds += size - 1;
					//printf("mli[i].extra_s_dir_ds %d\n",mli[i].extra_s_dir_ds);
				}

				k = 0;	/* reset the number of arguments, we are going to process a new operation */
   	 		}

			/* get the information about the specified variable */
  			if (vi_p->ts_id == NC_WTB_TS_ID)
				Check(nc_get_wtb_el(0, mli[i].pdm_act[j].pv_eid, (WtbDsEntry**)&ds_p, &pv_p));
 			else Check(nc_get_mvb_el(0, mli[i].pdm_act[j].pv_eid, (MvbDsEntry**)&ds_p, &pv_p, 0, 0, 0));

			/* check if the dataset is already known */
  			C_array_find(ds_p, vi_p->d_ds_list, vi_p->d_ds_num, ix);
   			if (ix == -1) vi_p->d_ds_list[vi_p->d_ds_num++] = ds_p;

			/* check if the process variable is already known */
  			C_array_find(pv_p, vi_p->pv_list, vi_p->num, ix);
   			if (ix == -1)
   			{
   	 			vi_p->pv_list[vi_p->num] = pv_p;
   	 			vi_p->ds_list[vi_p->num] = ds_p;
   	   			vi_p->num++;
   	   		    //printf("vip_num %d\n",vi_p->num);
   	   		}
		}

		/* make order in the variables */
		Check(pdm_var_info_sort(&mli[i].from));
		Check(pdm_var_info_sort(&mli[i].to));

		/* build the list that gives the pv index from the action index */
		for (j = 0; j < mli[i].pdm_act_num; j++) {

			/* select if it is a source or destination variable */
			if (mli[i].pdm_act[j].act == NC_PDM_SOURCE) vi_p = &mli[i].from;
			else vi_p = &mli[i].to;

			/* find the variable in the list */
			for (k = 0; k < vi_p->num; k++) {
				if (vi_p->pv_list[k]->eid == mli[i].pdm_act[j].pv_eid) {
					mli[i].act_pv_list[j] = k;
					break;
				}
			}
		}

		/* build the PDM index list for the "from" variables */
		for (j = 0, k = 0; j < mli[i].from.num; j++, k++) {
			if (pdm[i].type == NC_PDM_IMPORT && j > 0 && mli[i].from.ds_list[j] != mli[i].from.ds_list[j-1])
				k = 0;
			mli[i].from.pdm_idx_list[j] = k;
		}

		/* build the PDM index list for the "to" variables associated to a function */
		for (j = 0, k = 0; j < mli[i].pdm_act_num; j++)
			if (mli[i].pdm_act[j].act == NC_PDM_FUNC) mli[i].to.pdm_idx_list[mli[i].act_pv_list[j]] = k++;

  		/* calculate the list len */
   		mli[i].list_len = sizeof(T_MDB_LIST_HDR) +
   						  sizeof(T_MDB_FROMVAR_HDR) * (pdm[i].type == NC_PDM_IMPORT ? mli[i].from.d_ds_num : 1) +
   	   	   				  mli[i].from.num * sizeof(T_MDB_FROMVAR) +
   				  		  sizeof(T_MDB_FUNC_HDR) +
		   				  mli[i].func_num * sizeof(T_MDB_FUNC_ARG) +
   	     				  sizeof(T_MDB_TOVAR_HDR) +
   	                      mli[i].to.num * (pdm[i].type == NC_PDM_IMPORT ? sizeof(T_MDB_TOVAR_IM) :
   	                      												  sizeof(T_MDB_TOVAR)) +
				   	      sizeof(T_MDB_SUB_HDR) + mli[i].dflt_value_size;

		/* update the total length of the PDM configuration table */
		pdm_cfg_len += mli[i].list_len;
	}

	/* allocate the storage for the PDM configuration table */
 	pdm_cfg = (void*)pi_alloc(pdm_cfg_len);
	if (!pdm_cfg) Check(NC_MEM_ERR);
	cP = pdm_cfg;

	/* fill the head of the database */
	{
		T_MDB_DB_HDR *p = (void*)cP;
 		cP += sizeof(*p);

  		p->hdr.type                   = PDM_DB_HEADER;
   		p->hdr.length                 = sizeof(*p);
   		p->total_length_of_db         = pdm_cfg_len;
   		p->application_id             = 0;
   		p->vehicle_appl_id            = 0;
   		p->db_version                 = 0;
   		p->production_date.prod_day   = 0;
   		p->production_date.prod_month = 0;
   		p->production_date.prod_year  = 0;
	}

	/* create the tables */
	for (i = 0; i < pdm_num; i++) {

		/* fill the head of the marshalling list */
		{
			T_MDB_LIST_HDR *p = (void*)cP;
 			cP += sizeof(*p);

  			p->hdr.type   = pdm[i].type;
   			p->hdr.length = mli[i].list_len;
   			p->mode       = pdm[i].mode;
   			p->cycle      = pdm[i].cycle;
		}

		/* fill the "from" part for the import type */
		if (pdm[i].type == NC_PDM_IMPORT) for (j = 0; j < mli[i].from.d_ds_num; j++) {

			/* fill the header of the "from" variables list */
			{
				T_MDB_FROMVAR_HDR *p = (void*)cP;
 				cP += sizeof(*p);

  				p->hdr.type   = pdm[i].type | PDM_FROM;
   				p->hdr.length = sizeof(*p) + mli[i].from.d_ds_c_list[j] * sizeof(T_MDB_FROMVAR);
   				p->uu.ftf     = mli[i].from.d_ds_list[j]->wtb.ftf;
   				p->n_vars     = mli[i].from.d_ds_c_list[j];
			}

			/* fill the "from" variables list */
			{
				T_MDB_FROMVAR *p = (void*)cP;
 				cP += mli[i].from.d_ds_c_list[j] * sizeof(*p);

				for (k = 0; k < mli[i].from.num; k++)
   					if (mli[i].from.ds_list[k] == mli[i].from.d_ds_list[j]) {
						p->ref = mli[i].from.pdm_idx_list[k];
						gen_wtb_pv_name48(&mli[i].from.ds_list[k]->wtb, mli[i].from.pv_list[k], &p->pv_name);
   	  					p->overlay = mli[i].from.chkov_list[k];
   	   					p->freshness = (mli[i].from.ds_list[k]->wtb.refresh_tmo ?
   	   								    mli[i].from.ds_list[k]->wtb.refresh_tmo : PDM_NO_FRESHNESS);
   	   		        	p++;
	 				}
   			}
		}

		/* fill the "from" part for the other types */
 		else {

			/* fill the header of the "from" variables list */
			{
				T_MDB_FROMVAR_HDR *p = (void*)cP;
 				cP += sizeof(*p);

  				p->hdr.type   = pdm[i].type | PDM_FROM;
   				p->hdr.length = sizeof(*p) + mli[i].from.num * sizeof(T_MDB_FROMVAR);
   				p->uu.n_ports = mli[i].from.d_ds_num;
   				p->n_vars     = mli[i].from.num;
			}

			/* fill the "from" variables list */
			{
				T_MDB_FROMVAR *p = (void*)cP;
 				cP += mli[i].from.num * sizeof(*p);

				for (k = 0; k < mli[i].from.num; k++, p++) {
					p->ref = mli[i].from.pdm_idx_list[k];
   					gen_mvb_pv_name48(&mli[i].from.ds_list[k]->mvb, mli[i].from.pv_list[k], &p->pv_name);
   	  				p->overlay = mli[i].from.chkov_list[k];
   	   				p->freshness = (mli[i].from.ds_list[k]->mvb.refresh_tmo ?
   	   								mli[i].from.ds_list[k]->mvb.refresh_tmo : PDM_NO_FRESHNESS);
	 			}
   			}
   	 	}

		/* fill the header of the "functions" list */
		{
			T_MDB_FUNC_HDR *p = (void*)cP;
 			cP += sizeof(*p);

  			p->hdr.type   = pdm[i].type | PDM_FUNCTION;
   			p->hdr.length = sizeof(*p) + (mli[i].func_num + mli[i].func_arg_num) * sizeof(T_MDB_FUNC_ARG);
   			p->n_funcs    = mli[i].func_num;
   			p->n_args     = mli[i].func_arg_num;
		}

		/* fill the "functions" list */
		{
			T_MDB_FUNC_ARG *p = (void*)cP;
			cP += (mli[i].func_num + mli[i].func_arg_num) * sizeof(*p);

			/* loop for all the functions */
			for (j = 0; j < mli[i].func_num; j++) {

				/* find it in the list of the actions */
				for (k = 0, ix = 0; k < mli[i].pdm_act_num; k++) {
					if (mli[i].pdm_act[k].act == NC_PDM_FUNC &&
						mli[i].to.pdm_idx_list[mli[i].act_pv_list[k]] == j) {

						/* save the function entry */
   						p->uu.func.ref   = j | PDM_FUNC;
   						p->uu.func.id    = mli[i].pdm_act[k].param2;
   						p->uu.func.flags = mli[i].pdm_act[k].param1;
   	   					p++;

						/* save the source entries */
						for (; ix < k; ix++) {
	   						p->uu.arg.ref = mli[i].from.pdm_idx_list[mli[i].act_pv_list[ix]];
       						if (pdm[i].type == NC_PDM_IMPORT)
								p->uu.arg.ftf = mli[i].from.ds_list[mli[i].act_pv_list[ix]]->wtb.ftf;
       		        	    else p->uu.arg.ftf = 0;
	   						p->uu.arg.flags   = mli[i].pdm_act[ix].param1;
	   						p->uu.arg.bit_pos = mli[i].pdm_act[ix].param2;
   	   						p++;
						}

						/* process the next function */
						break;
					}
					if (mli[i].pdm_act[k].act != NC_PDM_SOURCE) ix = k + 1;
				}
			}
		}

		/* fill the "to" part for the import type */
		if (pdm[i].type == NC_PDM_IMPORT)
		{

			/* fill the header of the "to" variables list */
			{
				T_MDB_TOVAR_HDR *p = (void*)cP;
 				cP += sizeof(*p);

  				p->hdr.type   = pdm[i].type | PDM_TO;
   				p->hdr.length = sizeof(*p) + mli[i].to.num * sizeof(T_MDB_TOVAR_IM);
   				p->n_ports    = mli[i].to.d_ds_num + mli[i].extra_s_dir_ds;
   				p->n_vars     = mli[i].to.num;
   				//printf(" IMPORT TO p->n_ports %d\n",p->n_ports);
   				//printf(" IMPORT TO p->n_vars %d\n",p->n_vars);
			}

			/* fill the "to" variables list */
			{
				T_MDB_TOVAR_IM *p = (void*)cP;
 				cP += mli[i].to.num * sizeof(*p);

				/* loop for all the "to" variables */
				for (j = 0; j < mli[i].to.num; j++, p++) {

					/* find the variable in the actions list */
   					for (k = 0; k < mli[i].pdm_act_num; k++)
			          	if (mli[i].pdm_act[k].act != NC_PDM_SOURCE && mli[i].act_pv_list[k] == j) break;

					if (mli[i].pdm_act[k].act == NC_PDM_FUNC) {
						p->ref = mli[i].to.pdm_idx_list[j] | PDM_FUNC;
						p->ftf = 0;
						p->flags = mli[i].pdm_act[k].param1;
   					}
					else if (mli[i].pdm_act[k].act == NC_PDM_COPY) {
						p->ref = mli[i].from.pdm_idx_list[mli[i].act_pv_list[k-1]];
						p->ftf = mli[i].from.ds_list[mli[i].act_pv_list[k-1]]->wtb.ftf;
						p->flags = mli[i].pdm_act[k].param1;
					}
					else {
						p->ref = mli[i].pdm_act[k].param2 | PDM_DEFAULT_VALUE;
						p->ftf = 0;
						p->flags = PDM_NORM;
   					}
				   	gen_mvb_pv_name48(&mli[i].to.ds_list[j]->mvb, mli[i].to.pv_list[j], &p->pv_name);

				   	/* for the source directory copy with check variables we have to fix the port address */
					if ((mli[i].pdm_act[k].act == NC_PDM_FUNC && mli[i].pdm_act[k].param1 == NC_PDM_FUNC_S_DIR ||
   						 mli[i].pdm_act[k].act == NC_PDM_COPY && mli[i].pdm_act[k].param1 == NC_PDM_COPY_S_DIR) &&
   						!mli[i].to.pv_list[j]->var_offset) {
   						PVN48SETPORT(p->pv_name, PVN48GETPORT(p->pv_name) - 1);
					}
				}
   			}
		}

		/* fill the "to" part for the other types */
 		else {

			/* fill the header of the "to" variables list */
			{
				T_MDB_TOVAR_HDR *p = (void*)cP;
 				cP += sizeof(*p);

  				p->hdr.type   = pdm[i].type | PDM_TO;
   				p->hdr.length = sizeof(*p) + mli[i].to.num * sizeof(T_MDB_TOVAR);
   				p->n_ports    = mli[i].to.d_ds_num;
   				p->n_vars     = mli[i].to.num;
			}

			/* fill the "to" variables list */
			{
				T_MDB_TOVAR *p = (void*)cP;
 				cP += mli[i].to.num * sizeof(*p);

				/* loop for all the "to" variables */
				for (j = 0; j < mli[i].to.num; j++, p++) {

					/* find the variable in the actions list */
   					for (k = 0; k < mli[i].pdm_act_num; k++)
			          	if (mli[i].pdm_act[k].act != NC_PDM_SOURCE && mli[i].act_pv_list[k] == j) break;

					if (mli[i].pdm_act[k].act == NC_PDM_FUNC)
						p->ref = mli[i].to.pdm_idx_list[j] | PDM_FUNC;
					else if (mli[i].pdm_act[k].act == NC_PDM_COPY)
						p->ref = mli[i].from.pdm_idx_list[mli[i].act_pv_list[k-1]];
					else p->ref = mli[i].pdm_act[k].param2 | PDM_DEFAULT_VALUE;
				   	if (pdm[i].type == NC_PDM_EXPORT)
				   		 gen_wtb_pv_name48(&mli[i].to.ds_list[j]->wtb, mli[i].to.pv_list[j], &p->pv_name);
				   	else gen_mvb_pv_name48(&mli[i].to.ds_list[j]->mvb, mli[i].to.pv_list[j], &p->pv_name);
				}
   			}
		}

		/* fill the header of the "default values" list */
		{
			T_MDB_SUB_HDR *p = (void*)cP;
 			cP += sizeof(*p);

  			p->type    = pdm[i].type | PDM_DEFAULT_VAL;
   			p->length  = sizeof(*p) + mli[i].dflt_value_size;
		}

		/* fill the "default values" list */
		{
			pi_copy8(cP, mli[i].dflt_value, mli[i].dflt_value_size);
 			cP += mli[i].dflt_value_size;
 		}
	}

	/* fill the tail of the database */
	{
		T_MDB_SUB_HDR *p = (void*)cP;
 		cP += sizeof(*p);

  		p->type   = PDM_END_OF_DB;
   		p->length = sizeof(*p);
	}

/* !!! debug check !!! */ if ((char*)cP - (char*)pdm_cfg != pdm_cfg_len) pi_fail();

error:
	/* free the allocated temporary storage */
	if (mli) {
		for (i = 0; i < pdm_num; i++) {
			pi_free((void*)mli[i].act_pv_list);
			pi_free((void*)mli[i].from.d_ds_list);
			pi_free((void*)mli[i].from.d_ds_c_list);
			pi_free((void*)mli[i].from.pv_list);
			pi_free((void*)mli[i].from.ds_list);
			pi_free((void*)mli[i].from.chkov_list);
			pi_free((void*)mli[i].from.pdm_idx_list);
			pi_free((void*)mli[i].to.d_ds_list);
			pi_free((void*)mli[i].to.d_ds_c_list);
			pi_free((void*)mli[i].to.pv_list);
			pi_free((void*)mli[i].to.ds_list);
			pi_free((void*)mli[i].to.chkov_list);
			pi_free((void*)mli[i].to.pdm_idx_list);
 		}
		pi_free((void*)mli);
 	}

	/* free the PDM configuration table on error */
	if (err && pdm_cfg) {
		pi_free((void*)pdm_cfg);
 		pdm_cfg = 0;
	}

	/* save the results */
	if (pdm_cfg_h) *pdm_cfg_h = pdm_cfg;

	return err;
}


/*---------------------------------------------------------------------------------------------*/

#endif


/*=============================================================================================*/
/* WTB traffic store configuration */

#ifdef ATR_WTB

/*---------------------------------------------------------------------------------------------*/
short nc_wtb_ts_cfg(TYPE_LP_TS_CFG **ts_cfg_h)
{
	short				err = 0;			/* error code */
	short				i;					/* generic index */
	TYPE_LP_TS_CFG		*ts_cfg;			/* traffic store configuration */
	TYPE_LP_PRT_CFG 	*prt_cfg = 0;		/* port configuration */

	/* memory allocation */
	ts_cfg = (TYPE_LP_TS_CFG*)pi_alloc(sizeof(TYPE_LP_TS_CFG));
	if (!ts_cfg) Check(NC_MEM_ERR);
	prt_cfg = (TYPE_LP_PRT_CFG*)pi_alloc(128 * sizeof(*prt_cfg));
	if (!prt_cfg) Check(NC_MEM_ERR);

	/* configure the ports */
	for (i = 0; i < 64; i++) {

		/* multicast port */
		prt_cfg[i].prt_addr = i;
		prt_cfg[i].prt_indx = i;
		prt_cfg[i].size     = LP_CFG_128_BYTES;
		prt_cfg[i].type     = (i == 0 ? LP_CFG_SRCE : LP_CFG_SINK);

		/* unicast port (not supported) */
		prt_cfg[i + 64].prt_addr = 0x80 + i;
		prt_cfg[i + 64].prt_indx = 64 + i;
		prt_cfg[i + 64].size     = 0;
		prt_cfg[i + 64].type     = (i == 0 ? LP_CFG_SRCE : LP_CFG_SINK);
	}

	/* build the traffic store configuration */
	ts_cfg->pb_pit       = 0;
	ts_cfg->pb_pcs       = 0;
	ts_cfg->pb_prt       = 0;
	ts_cfg->pb_frc       = 0;
	ts_cfg->pb_def       = 0;
	ts_cfg->pb_mwd       = 0;
	ts_cfg->ownership    = LP_CFG_TS_OWNED;
	ts_cfg->ts_type      = LP_TS_TYPE_D;
	ts_cfg->prt_addr_max = (0x80 + 63) | LP_PRT_ADDR_MAX_MASK;
	ts_cfg->prt_indx_max = 127 | LP_PRT_INDX_MAX_MASK;
	ts_cfg->prt_count    = 128;
	ts_cfg->p_prt_cfg    = prt_cfg;

	/* save the results and exit */
	*ts_cfg_h = ts_cfg;
	return err;

error:
	/* deallocate the memory on error */
	if (ts_cfg) pi_free((void*)ts_cfg);
	if (prt_cfg) pi_free((void*)prt_cfg);
	return err;
}


/*---------------------------------------------------------------------------------------------*/

#endif


/*=============================================================================================*/
/* WTB node configuration */

#ifdef ATR_WTB

/*---------------------------------------------------------------------------------------------*/
short nc_node_cfg(unsigned short export_mode, Type_NodeDescriptor *node_descriptor)
{
	static PdmActEntry	*pdm_act;		/* export PDM actions           */
	unsigned short		pdm_act_num;	/* number of export PDM actions */
	const WtbDsEntry	*ds_p = 0;		/* dataset to be exported       */
	unsigned short		i, j;			/* generic counters             */
	unsigned short		err = 0;		/* error code                   */

	/* search the PDM export dataset in the configuration */
	for (i = 0; i < pdm_num; i++) if (pdm[i].type == NC_PDM_EXPORT && pdm[i].mode == export_mode) {
		Check(nc_get_db_element(pdm[i].pdm_act_oid, (void**)&pdm_act, &pdm_act_num));
		pdm_act_num /= sizeof(PdmActEntry);
		for (j = 0; j < pdm_act_num; j++) if (pdm_act[j].act != NC_PDM_SOURCE) {
			Check(nc_get_wtb_el(0, pdm_act[j].pv_eid, &ds_p, 0));
			break;
		}
		if (j == pdm_act_num) Check(NC_WRONG_CONF_ERR);
		break;
	}
	if (i == pdm_num) Check(NC_WRONG_CONF_ERR);

	/* fill the node descriptor fields and the PDM FTF size information */
	node_descriptor->node_frame_size       = ds_p ? ds_p->size : 128;
	node_descriptor->node_period           = ds_p ? (ds_p->flags & NC_WTB_DS_PERIOD_MASK) :
													MAX_NODE_PERIOD;
	if (tcn_conf->flags & NC_CONF_8_BIT_FTF) {
		node_descriptor->node_key.node_type    = ds_p ? ds_p->ftf : 0;
		node_descriptor->node_key.node_version = 0;
	}
	else {
		node_descriptor->node_key.node_type    = ds_p ? ((ds_p->ftf >> 8) & 0xFF) : 0;
		node_descriptor->node_key.node_version = ds_p ? (ds_p->ftf & 0xFF) : 0;
	}

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/

#endif


/*=============================================================================================*/
/* UIC Mapping Server configuration */

#ifdef ATR_WTB

/*---------------------------------------------------------------------------------------------*/
#ifdef UMS

short nc_ums_cfg(ums_NodeConfigurationTable *ums_cfg,
				 UWTM_NODE_CONFIG_TABLE_T *uwtm_cfg, UIMCS_T_CONFIGURATION *uimcs_cfg)
{
	int i;	/* generic index */

	/* clear everything just to be sure... */
	pi_zero8(ums_cfg, sizeof(*ums_cfg));
	pi_zero8(uwtm_cfg, sizeof(*uwtm_cfg));
	pi_zero8(uimcs_cfg, sizeof(*uimcs_cfg));

	/* fill the fields for UMS */
	ums_cfg->basic_period          = 25;
	ums_cfg->node_frame_size       = 128;	// ???
	ums_cfg->node_period           = 1;		// ???
	ums_cfg->uic_ind               = 1;
	ums_cfg->uic_version           = 1;
	ums_cfg->uic_subversion        = 1;
	ums_cfg->tcn_node_type         = L_WEAK;
	ums_cfg->max_veh_count         = 32;
	ums_cfg->max_num_nodes         = 32;
	ums_cfg->num_NADI_entries      = 32;
	ums_cfg->veh_management        = tcn_conf->vehicle_desc[1-1];
	ums_cfg->veh_owner             = tcn_conf->vehicle_desc[2-1];
	ums_cfg->national_version      = tcn_conf->vehicle_desc[3-1];
	ums_cfg->applicationID         = tcn_conf->vehicle_desc[4-1];
	ums_cfg->KFT_used              = 0;
	ums_cfg->num_of_controlled_veh = tcn_conf->vehicle_desc[5-1];
	pi_copy8(ums_cfg->ts_attr, &tcn_conf->vehicle_desc[18-1], sizeof(ums_cfg->ts_attr));
	for (i = 0; i < 6; i++) {
		pi_copy8(ums_cfg->veh_conf[i].veh_attr, &tcn_conf->vehicle_desc[46-1 + i*14],
				 sizeof(ums_cfg->veh_conf[i].veh_attr));
		pi_copy8(ums_cfg->veh_conf[i].uic_number, &tcn_conf->vehicle_desc[41-1 + i*14],
				 sizeof(ums_cfg->veh_conf[i].uic_number));
	}

	/* fill the fields for UWTM */
	uwtm_cfg->wtb_config.transmission_rate                     = BITRATE / 1000;
	uwtm_cfg->wtb_config.basic_period                          = ums_cfg->basic_period;
	uwtm_cfg->wtb_config.fritting_disabled                     = 0;
	uwtm_cfg->wtb_config.node_descriptor.node_frame_size       = ums_cfg->node_frame_size;
	uwtm_cfg->wtb_config.node_descriptor.node_period           = ums_cfg->node_period;
	uwtm_cfg->wtb_config.node_descriptor.node_key.node_type    = ums_cfg->uic_ind;
	uwtm_cfg->wtb_config.node_descriptor.node_key.node_version = ums_cfg->uic_version;
	uwtm_cfg->wtb_config.poll_md_when_idle                     = 0;
	uwtm_cfg->wtb_config.sink_port_count                       = ums_cfg->max_veh_count;
	uwtm_cfg->wtb_config.source_port_count                     = 1;
	uwtm_cfg->wtb_config.port_size                             = ums_cfg->node_frame_size;
	uwtm_cfg->wtb_config.p_traffic_store                       = 0;
	uwtm_cfg->wtb_config.Report                                = 0;
	uwtm_cfg->wtb_config.ErrorReport                           = 0;
	uwtm_cfg->omode                                            = ums_cfg->tcn_node_type;
	uwtm_cfg->single_node                                      = ums_cfg->KFT_used & 0x02;

	/* fill the fields for UIMCS */
	uimcs_cfg->mc_requests_max = 3;     /* max. number of stored multicast requests */
	uimcs_cfg->open_calls_max  = 32;    /* max. number of open calls */
	uimcs_cfg->reply_len_max   = 1024;  /* max. length of reply message accepted */
	uimcs_cfg->reply_timeout   = 10000; /* max. timeout for replier */

	return 0;
}

#endif


/*---------------------------------------------------------------------------------------------*/
#ifndef UMS

short nc_map_cfg(map_NodeConfigurationTable *map_cfg)
{
	/* This table tells how to map UIC-556 v12 static attributes into v10 ones */
	static const struct {
		struct { unsigned char	byte, bit; } uic_v12;
		unsigned char						 uic_v10_attr;
	} stat_attr_corr[] = {
		{ /*38*/ {20,0}, 1  }, { /*39*/ {20,1}, 2  }, { /*44*/ {20,4}, 3  }, { /*45*/ {20,5}, 3  },
		{ /*46*/ {20,6}, 3  }, { /*44*/ {20,4}, 4  }, { /*45*/ {20,5}, 5  }, { /*46*/ {20,6}, 6  },
		{ /*50*/ {21,2}, 7  }, { /*51*/ {21,3}, 8  }, { /* 1*/ {46,0}, 9  }, { /* 2*/ {46,1}, 10 },
		{ /* 3*/ {46,2}, 11 }, { /* 4*/ {46,3}, 12 }, { /* 5*/ {46,4}, 13 }, { /* 6*/ {46,5}, 14 },
		{ /* 7*/ {46,6}, 15 }, { /* 8*/ {46,7}, 16 }, { /* 9*/ {47,0}, 17 }, { /*10*/ {47,1}, 18 },
		{ /*11*/ {47,2}, 19 }, { /*12*/ {47,3}, 20 }, { /*13*/ {47,4}, 21 }, { /*14*/ {47,5}, 22 },
		{ /*15*/ {47,6}, 23 }, { /*16*/ {47,7}, 24 }, { /*17*/ {18,0}, 25 }, { /*18*/ {48,0}, 26 },
		{ /*19*/ {18,1}, 27 }, { /*20*/ {48,1}, 28 }, { /*40*/ {20,2}, 29 }, { /*41*/ {20,3}, 30 },
		{ /*42*/ {48,6}, 31 }, { /*43*/ {48,7}, 32 }, { /*85*/ {25,5}, 33 }
	};

	int   i;			/* generic index */
	short err = 0;		/* error code    */

	/* fill the vehicle descriptor */
	pi_zero8(&map_cfg->vehicle_descriptor, sizeof(map_cfg->vehicle_descriptor));
	map_cfg->vehicle_descriptor.VehicleNo    = *(unsigned short *)&tcn_conf->vehicle_desc[46-1];
	map_cfg->vehicle_descriptor.Owner        = tcn_conf->vehicle_desc[2-1];
	map_cfg->vehicle_descriptor.VehicleId[0] = tcn_conf->vehicle_desc[42-1];
	map_cfg->vehicle_descriptor.VehicleId[1] = tcn_conf->vehicle_desc[43-1];
	map_cfg->vehicle_descriptor.VehicleId[2] = tcn_conf->vehicle_desc[44-1];
	map_cfg->vehicle_descriptor.VehicleId[3] = tcn_conf->vehicle_desc[45-1];
	for (i = 0; i < sizeof(stat_attr_corr) / sizeof(*stat_attr_corr); i++) {
		if ((tcn_conf->vehicle_desc[stat_attr_corr[i].uic_v12.byte - 1] >>
				stat_attr_corr[i].uic_v12.bit) & 1)
			map_cfg->vehicle_descriptor.PermAttr[(stat_attr_corr[i].uic_v10_attr - 1) / 8] |=
				1 << ((stat_attr_corr[i].uic_v10_attr - 1) % 8);
	}
	map_cfg->vehicle_descriptor.NcV          = tcn_conf->vehicle_desc[5-1];
	for (i = 0; i < func_num; i++) {
		if (!(func[i].flags & NC_FUNC_NO_WTB))
			map_cfg->vehicle_descriptor.Functions[(func[i].func - 1) / 8] |=
				1 << ((func[i].func - 1) % 8);
	}
	map_cfg->vehicle_descriptor.TCNType      = L_WEAK;
	map_cfg->vehicle_descriptor.VTy          = tcn_conf->vehicle_desc[41-1];
	for (i = 0; i < sizeof(map_cfg->vehicle_descriptor.PAttrFS); i++)
		map_cfg->vehicle_descriptor.PAttrFS[i] = tcn_conf->owner_data[i];
	map_cfg->vehicle_descriptor.PMax = tcn_conf->owner_data[i];

	/* allocate space for all the devices (wasting some space) */
	map_cfg->device_list.number = 0;
	map_cfg->device_list.device = (AM_NET_ADDR*)pi_alloc(st_num * sizeof(*map_cfg->device_list.device));
	if (!map_cfg->device_list.device) Check(NC_MEM_ERR);

	/* find all the devices that need the NADI */
	for (i = 0; i < st_num; i++) if (st[i].flags & NC_ST_NEEDS_NADI) {
		map_cfg->device_list.device[map_cfg->device_list.number].vehicle  = AM_SAME_VEHICLE;
		map_cfg->device_list.device[map_cfg->device_list.number].function = MAP_FUNCTION_NR;
		map_cfg->device_list.device[map_cfg->device_list.number].device   = st[i].mvb_address;
		map_cfg->device_list.number++;
	}

	/* fill the known WTB LL configuration info */
	map_cfg->configuration.basic_period = 25;
	map_cfg->configuration.individual_period = 0;
	pi_zero8(&map_cfg->configuration.node_descriptor, sizeof(map_cfg->configuration.node_descriptor));
	map_cfg->configuration.ts_descriptor = 0;

error:
	return err;
}

#endif


/*---------------------------------------------------------------------------------------------*/

#endif


/*=============================================================================================*/
/* Debug support routines */

/*---------------------------------------------------------------------------------------------*/
const AppStEntry *nc_get_app_st_conf(void)
{
	return app_st_conf;
}


/*---------------------------------------------------------------------------------------------*/
void nc_get_st(const StEntry **st_h, unsigned short *st_num_p)
{
	if (st_h) *st_h = st;
	if (st_num_p) *st_num_p = st_num;
}


/*---------------------------------------------------------------------------------------------*/
short nc_get_idx_mvb_ds(short ds_idx, const MvbDsEntry **mvb_ds_h, const AppDsEntry **app_ds_h)
{
	short				err = 0;			/* error code */

	/* check if the dataset index is out of range and save its pointer */
	if (ds_idx >= mvb_ds_num) Check(NC_INVALID_INDEX_ERR);
	if (mvb_ds_h) *mvb_ds_h = mvb_ds + ds_idx;

	/* find the dataset name if required */
	if (app_ds_h) *app_ds_h = (const AppDsEntry*)find_list_elem(app_st_conf->app_ds_list, 9999,
																mvb_ds[ds_idx].eid);
error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
short nc_get_idx_mvb_pv(short ds_idx, short pv_idx,
						const MvbDsEntry **mvb_ds_h, const PvEntry **mvb_pv_h,
						const AppDsEntry **app_ds_h, const AppPvEntry **app_pv_h,
   	  					struct STR_PV_NAME *pvn)
{
	short				err;				/* error code */

	const MvbDsEntry	*mvb_ds_p;			/* generic configuration dataset ptr */

	const PvEntry		*mvb_pv;	   		/* process variables object */
	unsigned short		mvb_pv_num;     	/* number of process variables in the object */

	const AppDsEntry	*app_ds_p;			/* generic application dataset ptr */
	const AppPvEntry	*app_pv_p;			/* generic application process variable ptr */

	/* find the dataset */
	Check(nc_get_idx_mvb_ds(ds_idx, &mvb_ds_p, &app_ds_p));

	/* get the pv object */
	Check(nc_get_db_element(mvb_ds_p->pv_oid, (void**)&mvb_pv, &mvb_pv_num));
	mvb_pv_num /= sizeof(PvEntry);

	/* check if the process variable index is out of range */
	if (pv_idx >= mvb_pv_num) Check(NC_INVALID_INDEX_ERR);

	/* find the process variable name */
	if (app_ds_p) app_pv_p = (const AppPvEntry*)find_list_elem(app_ds_p->app_pv_list, 9999,
															   mvb_pv[pv_idx].eid);
   	else app_pv_p = 0;

	/* save the ptrs */
	if (mvb_ds_h) *mvb_ds_h = mvb_ds_p;
	if (mvb_pv_h) *mvb_pv_h = mvb_pv + pv_idx;
	if (app_ds_h) *app_ds_h = app_ds_p;
	if (app_pv_h) *app_pv_h = app_pv_p;

	/* generate the PV_NAME for the selected process variable */
	if (pvn) gen_mvb_pv_name(mvb_ds + ds_idx, mvb_pv + pv_idx, pvn);

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Find a MVB dataset or process variable using its element ids */

short nc_get_mvb_el(NcEleID mvb_ds_eid, NcEleID mvb_pv_eid,
					const MvbDsEntry **mvb_ds_h, const PvEntry **mvb_pv_h,
					const AppDsEntry **app_ds_h, const AppPvEntry **app_pv_h,
   	   				struct STR_PV_NAME *pvn)
{
	short 				err = NC_OK;	/* error code */
	short				i;				/* generic integer */
	const MvbDsEntry	*mvb_ds_p;		/* generic MVB dataset entry in the conf */
	const PvEntry		*mvb_pv;		/* process variable object */
	unsigned short		mvb_pv_num;     /* number of process variables in the object */
	const PvEntry		*mvb_pv_p = 0;	/* generic MVB process variable entry in the conf */
	const AppDsEntry	*app_ds_p;		/* generic application dataset ptr */

	if (mvb_ds_eid) {

		/* find the specified dataset */
		mvb_ds_p = (const MvbDsEntry *)find_list_elem(mvb_ds, mvb_ds_num, mvb_ds_eid);
 		if (!mvb_ds_p) Check(NC_NOT_FOUND_ERR);

		if (mvb_pv_eid) {

  			/* get the pv object */
			Check(nc_get_db_element(mvb_ds_p->pv_oid, (void**)&mvb_pv, &mvb_pv_num));
			mvb_pv_num /= sizeof(PvEntry);

			/* find the requested process variable */
			mvb_pv_p = (const PvEntry *)find_list_elem(mvb_pv, mvb_pv_num, mvb_pv_eid);
  			if (!mvb_pv_p) Check(NC_NOT_FOUND_ERR);
		}
	}
	else {

		/* loop in all the MVB datasets */
		for (i = 0, mvb_ds_p = mvb_ds; i < mvb_ds_num; i++, mvb_ds_p++) {

			/* find the pv object */
			Check(nc_get_db_element(mvb_ds_p->pv_oid, (void**)&mvb_pv, &mvb_pv_num));
			mvb_pv_num /= sizeof(PvEntry);

			/* find the requested MVB process variable */
			mvb_pv_p = (const PvEntry *)find_list_elem(mvb_pv, mvb_pv_num, mvb_pv_eid);
			if (mvb_pv_p) break;
 		}
  		if (i == mvb_ds_num) Check(NC_NOT_FOUND_ERR);

	}

	/* save the results */
	if (mvb_ds_h) *mvb_ds_h = mvb_ds_p;
   	if (mvb_pv_h) *mvb_pv_h = mvb_pv_p;
	if (app_ds_h || app_pv_h) {
		app_ds_p = (const AppDsEntry*)find_list_elem(app_st_conf->app_ds_list, 9999,
													 mvb_ds_p->eid);
		if (app_ds_h) *app_ds_h = app_ds_p;
		if (app_pv_h) *app_pv_h = (const AppPvEntry*)find_list_elem(app_ds_p->app_pv_list, 9999,
															   		mvb_pv_eid);
	}
	if (pvn) gen_mvb_pv_name(mvb_ds_p, mvb_pv_p, pvn);

error:
	return err;
}


/*---------------------------------------------------------------------------------------------*/
/* Find a WTB dataset or process variable using its index */

#ifdef ATR_WTB

short nc_get_idx_wtb_pv(short ds_idx, short pv_idx,
					   	const WtbDsEntry **wtb_ds_h, const PvEntry **wtb_pv_h,
					   	TYPE_PV_NAME_48 *pvn)
{
	short				err = 0;			/* error code */
	const PvEntry		*wtb_pv;	   		/* process variables object */
	unsigned short		wtb_pv_num;     	/* number of process variables in the object */

	/* check if the dataset index is out of range and save its pointer */
	if (ds_idx >= wtb_ds_num) Check(NC_INVALID_INDEX_ERR);
	if (wtb_ds_h) *wtb_ds_h = wtb_ds + ds_idx;

	if (wtb_pv_h || pvn) {
		/* get the pv object */
		Check(nc_get_db_element(wtb_ds[ds_idx].pv_oid, (void**)&wtb_pv, &wtb_pv_num));
		wtb_pv_num /= sizeof(PvEntry);

		/* check if the process variable index is out of range and save its pointer */
		if (pv_idx >= wtb_pv_num) Check(NC_INVALID_INDEX_ERR);
		if (wtb_pv_h) *wtb_pv_h = wtb_pv + pv_idx;

		/* create the pv-name if required */
		if (pvn) gen_wtb_pv_name48(wtb_ds + ds_idx, wtb_pv + pv_idx, pvn);
	}

error:
	return err;
}

#endif


/*---------------------------------------------------------------------------------------------*/
/* Find a WTB dataset or process variable using its element ids */

#ifdef ATR_WTB

short nc_get_wtb_el(NcEleID wtb_ds_eid, NcEleID wtb_pv_eid,
					const WtbDsEntry **wtb_ds_h, const PvEntry **wtb_pv_h)
{
	short 				err = NC_OK;	/* error code */
	short				i;				/* generic integer */
	const WtbDsEntry	*wtb_ds_p;		/* generic WTB dataset entry in the conf */
	const PvEntry		*wtb_pv;		/* process variable object */
	unsigned short		wtb_pv_num;     /* number of process variables in the object */
	const PvEntry		*wtb_pv_p = 0;	/* generic WTB process variable entry in the conf */

	if (wtb_ds_eid) {

		/* find the specified dataset */
		wtb_ds_p = (const WtbDsEntry *)find_list_elem(wtb_ds, wtb_ds_num, wtb_ds_eid);
 		if (!wtb_ds_p) Check(NC_NOT_FOUND_ERR);

		if (wtb_pv_eid) {

  			/* get the pv object */
			Check(nc_get_db_element(wtb_ds_p->pv_oid, (void**)&wtb_pv, &wtb_pv_num));
			wtb_pv_num /= sizeof(PvEntry);

			/* find the requested process variable */
			wtb_pv_p = (const PvEntry *)find_list_elem(wtb_pv, wtb_pv_num, wtb_pv_eid);
  			if (!wtb_pv_p) Check(NC_NOT_FOUND_ERR);
		}
	}
	else {

		/* loop in all the MVB datasets */
		for (i = 0, wtb_ds_p = wtb_ds; i < wtb_ds_num; i++, wtb_ds_p++) {

			/* find the pv object */
			Check(nc_get_db_element(wtb_ds_p->pv_oid, (void**)&wtb_pv, &wtb_pv_num));
			wtb_pv_num /= sizeof(PvEntry);

			/* find the requested MVB process variable */
			wtb_pv_p = (const PvEntry *)find_list_elem(wtb_pv, wtb_pv_num, wtb_pv_eid);
			if (wtb_pv_p) break;
 		}
  		if (i == wtb_ds_num) Check(NC_NOT_FOUND_ERR);

	}

	/* save the results */
	if (wtb_ds_h) *wtb_ds_h = wtb_ds_p;
   	if (wtb_pv_h) *wtb_pv_h = wtb_pv_p;

error:
	return err;
}

#endif


/*---------------------------------------------------------------------------------------------*/

