/*=============================================================================================*/
/* NCDB configuration	    														           */
/* Header file (ncdb.h) 															           */
/* 																					           */
/* Version 2.1																		           */
/*																					           */
/* (c)1996/8 Ansaldo Trasporti														           */
/* Written by Giuseppe Carnevale													           */
/*=============================================================================================*/


#ifndef _NCDB_
#define _NCDB_


/*=============================================================================================*/
/* Macros */

#define BIT(x) (1 << x)


/*=============================================================================================*/
/* Constants */

/* Element ID to use in NSDB for an embedded NCDB */
#define NC_NCDB_IN_NSDB_ELE_ID	0x3468

/* Current NC version */
#define NC_VERSION				0

/* Object IDs */
#define NC_START_OF_DB_OBJ_ID	0x1000	/* start of DB */
#define NC_END_OF_DB_OBJ_ID		0xFFFF	/* end of DB */
#define NC_TCN_CONF_OBJ_ID		0x2000	/* TCN configuration root */
#define NC_INVALID_START_OBJ_ID	0x0010	/* start of DB in the wrong endian mode */

/* Traffic store IDs */
#define NC_MVB_TS_ID			0
#define NC_WTB_TS_ID			1

/* Flags for the configuration */
#define NC_CONF_8_BIT_FTF		BIT(0)

/* Flags for the stations */
#define NC_ST_IS_MVB_BA			BIT(0)			/* the station is a MVB Bus Administrator      */
#define NC_ST_NEEDS_NADI		BIT(1)			/* the station needs the NADI from the gateway */
#define NC_ST_IS_GATEWAY		BIT(2)			/* enables the gateway interface               */
#define NC_ST_MVB_BA_PRIO_OFF	3				/* offset of the MVB BA priority field         */
#define NC_ST_MVB_BA_PRIO_MASK	(BIT(3)|BIT(4))	/* mask for the MVB BA priority field          */
#define NC_ST_MVB_BA_PRIO_0		0				/* this BA has a low priority                  */
#define NC_ST_MVB_BA_PRIO_1		BIT(3)			/* this BA has a medium-low priority           */
#define NC_ST_MVB_BA_PRIO_2		BIT(4)			/* this BA has a medium-high priority          */
#define NC_ST_MVB_BA_PRIO_3		(BIT(3)|BIT(4))	/* this BA has a high priority                 */

/* Flags for the functions */
#define NC_FUNC_NO_WTB			BIT(0)	/* the function doesn't have to be declared on WTB */

/* Special functions (defined to avoid am_sys.h inclusion) */
#define NC_ROUTER_FCT			251		/* must be equal to AM_ROUTER_FCT */

/* MVB datasets sizes */
#define NC_MVB_DS_02_BYTES		0x0000
#define NC_MVB_DS_04_BYTES		0x1000
#define NC_MVB_DS_08_BYTES		0x2000
#define NC_MVB_DS_16_BYTES		0x3000
#define NC_MVB_DS_32_BYTES		0x4000

/* WTB datasets sizes */
#define NC_WTB_DS_PERIOD_1		0x00
#define NC_WTB_DS_PERIOD_2		0x01
#define NC_WTB_DS_PERIOD_4		0x02
#define NC_WTB_DS_PERIOD_8		0x03
#define NC_WTB_DS_PERIOD_16		0x04
#define NC_WTB_DS_PERIOD_32		0x05
#define NC_WTB_DS_PERIOD_64		0x06
#define NC_WTB_DS_PERIOD_128	0x07
#define NC_WTB_DS_PERIOD_MASK	0x07

/* Miscellanea */
#define NC_NO_FRESHNESS			0xFFFF	/* dataset has no max freshness */

/* PDM marshalling types */
#define NC_PDM_INTERNAL    		0x1000
#define NC_PDM_EXPORT      		0x2001
#define NC_PDM_IMPORT      		0x4002

/* PDM action flags */
#define NC_PDM_SOURCE			0x00	/* pv_eid = source var id, param1 = opt, param2 = bit offset */
#define NC_PDM_FUNC				0x01	/* pv_eid = dest var id, param1 = opt, param2 = function */
#define NC_PDM_COPY				0x02	/* pv_eid = dest var id, param1 = opt, param2 = extra_opt */
#define NC_PDM_DFLT_VALUE 		0x03	/* pv_eid = dest var id, param2 = byte offset */

/* PDM param1 */
#define NC_PDM_SOURCE_NEGATED	0x01	/* the source variable needs to be inverted */
#define NC_PDM_FUNC_NORM		0x01	/* use the normal copy */
#define NC_PDM_FUNC_S_DIR		0x02	/* use the source directory copy (only with CONV) */
#define NC_PDM_FUNC_UNSIGNED	0x08	/* convert the function arguments to unsigned */
#define NC_PDM_FUNC_REAL		0x10    /* convert the function arguments to real */
#define NC_PDM_COPY_NORM		0x01	/* use the normal copy */
#define NC_PDM_COPY_S_DIR		0x02	/* use the source directory copy; param2 = n_vehicles */

/* PDM param2 */
#define NC_PDM_FUNC_AND 		0x0000
#define NC_PDM_FUNC_OR 			0x0001
#define NC_PDM_FUNC_XOR 		0x0002
#define NC_PDM_FUNC_MIN 		0x0003
#define NC_PDM_FUNC_MAX 		0x0004
#define NC_PDM_FUNC_CONV 		0x0005
#define NC_PDM_FUNC_SUM 		0x0006
#define NC_PDM_FUNC_SELECT 		0x0007


/*---------------------------------------------------------------------------------------------*/
/* Useful macros */

#define NC_BOOLEAN1			0x00, 0x00
#define NC_ANTIVALENT2		0x01, 0x00
#define NC_BCD4				0x02, 0x00
#define NC_ENUM4			0x02, 0x00
#define NC_BITSET8			0x04, 0x00
#define NC_UNSIGNED8		0x05, 0x00
#define NC_ENUM8			0x05, 0x00
#define NC_INTEGER8			0x06, 0x00
#define NC_CHARACTER8		0x07, 0x00

#define NC_BITSET16			0x04, 0x01
#define NC_UNSIGNED16		0x05, 0x01
#define NC_ENUM16			0x05, 0x01
#define NC_INTEGER16		0x06, 0x01
#define NC_BIPOLAR16_14		0x08, 0x01
#define NC_UNIPOLAR16_14	0x09, 0x01
#define NC_BIPOLAR16_12		0x0A, 0x01

#define NC_REAL32			0x03, 0x02
#define NC_BITSET32			0x04, 0x02
#define NC_UNSIGNED32		0x05, 0x02
#define NC_ENUM32			0x05, 0x02
#define NC_INTEGER32		0x06, 0x02

#define NC_TIMEDATE48		0x02, 0x03

/* types not yet implemented */
/*/ #define NC_BITSET64		0x04, 0x04
// #define NC_UNSIGNED64	0x05, 0x04
// #define NC_INTEGER64		0x06, 0x04
*/
#define NC_BITSET64		NC_ARRAY_OF_WORD8(8)

#define NC_ARRAY_OF_WORD8(n)		((n) & 1 ? 0x07 : 0x0F), ((n)-1)/2
#define NC_ARRAY_OF_UNSIGNED16(n)	0x0D, ((n)-1)
#define NC_ARRAY_OF_INTEGER16(n)	0x0E, ((n)-1)
#define NC_ARRAY_OF_UNSIGNED32(n)	0x0B, ((n)-1)
#define NC_ARRAY_OF_INTEGER32(n)	0x0C, ((n)-1)

#define NC_OBJ_DEF(x, y)				\
	NcObjHeader	header_##y##; 			\
	x			data_##y##[y##_NUM]



/*=============================================================================================*/
/* Typedefs */

/*---------------------------------------------------------------------------------------------*/
/* Generic TCN configuration */

typedef unsigned short NcObjID;
typedef unsigned long  NcEleID;


/*---------------------------------------------------------------------------------------------*/

typedef struct NcObjHeader {				/**** NC object header *****************************/
	NcObjID				oid;				/* object id                                       */
	unsigned short 		size;				/* object size                                     */
	/* object data follows */
} NcObjHeader;


/*---------------------------------------------------------------------------------------------*/

typedef struct NcVersion {					/**** NC version identifier ************************/
	unsigned long		signature_1;		/* characters of the signature (the company)       */
	unsigned long		signature_2;		/* characters of the signature (the person)        */
	unsigned char		ver_1;				/* BCD coded first digits of the version           */
	unsigned char		ver_2;				/* BCD coded second digits of the version          */
	unsigned short		ver_3;				/* extra characters of the version                 */
	unsigned char		day;				/* BCD coded day number                            */
	unsigned char		month;				/* BCD coded month number                          */
	unsigned short		year;				/* BCD coded year number                           */
} NcVersion;


/*---------------------------------------------------------------------------------------------*/

typedef struct NcTcnConf {					/**** TCN configuration ****************************/
	short				version;			/* version of the configuration structure          */
	short				min_version;		/* min version to be used to read this conf        */

	NcObjID				st_oid;				/* object id of the list of stations               */
	NcObjID				func_oid;			/* function location object id                     */
	NcObjID				pdm_oid;			/* process data marshalling object id              */

	unsigned short		flags;				/* generic configuration flags                     */

	NcObjID				mvb_ds_oid;			/* object id of the list of MVB datasets           */
	unsigned short		mtvc;				/* master transfer version ctrl                    */
	unsigned short		t_reply_max;		/* reply timeout in us (42 or 64)                  */
	unsigned short		basic_period;		/* basic period in us                              */
	unsigned short		ev_poll_strategy;	/* event poll strategy                             */
	unsigned short		c_macros_in_turn;	/* macro cycles per turn                           */
	unsigned short		scan_strategy;		/* 0/1: known/all devices                          */

	NcVersion			version_id;			/* version identifier                              */

	NcObjID				wtb_ds_oid;			/* object id of the list of WTB datasets           */
	unsigned char		vehicle_desc_hdr[4];/* UIC-556 v.12 vehicle descriptor header          */
	unsigned char		vehicle_desc[124];	/* UIC-556 v.12 vehicle descriptor                 */
	unsigned char		owner_data[32];		/* data specific to the vehicle owner              */
	
} NcTcnConf;


/*---------------------------------------------------------------------------------------------*/

typedef struct StEntry {					/**** Station entry ********************************/
	NcEleID				eid;				/* station identifier                              */
	unsigned short		mvb_address;		/* MVB device address                              */
	unsigned short		flags;				/* flags                                           */
	NcObjID				user_oid;			/* station user data object id                     */
	short				unused;				/*                                                 */
} StEntry;


/*---------------------------------------------------------------------------------------------*/

typedef struct FuncEntry {					/**** Function entry *******************************/
	NcEleID				st_eid;				/* station identifier                              */
	unsigned char		func;				/* function                                        */
	unsigned char		flags;				/* flags                                           */
	short				unused;				/*                                                 */
} FuncEntry;


/*---------------------------------------------------------------------------------------------*/

typedef struct MvbDsEntry {					/**** MVB dataset entry ****************************/
	NcEleID				eid;             	/* MVB dataset identifier                          */
	NcEleID				src_station_eid;	/* id of the source station                        */
	short				mf;             	/* master frame                                    */
	short				cyc_sub;        	/* cycle bit + subcycle number                     */
	unsigned short		refresh_tmo;    	/* refresh timeout                                 */
	NcObjID				pv_oid;				/* object id of the list of process variables      */
} MvbDsEntry;

typedef struct PvEntry {					/**** Process variable entry (WTB or MVB) **********/
	NcEleID				eid;				/* process variable identifier                     */
	char				var_type;			/* variable type                                   */
	char				var_size;			/* variable size                                   */
	short				var_offset;			/* variable bit offset                             */
	NcEleID				chk_eid;			/* check variable identifier                       */
} PvEntry;


/*---------------------------------------------------------------------------------------------*/

typedef struct WtbDsEntry {					/**** WTB dataset entry ****************************/
	NcEleID				eid;				/* WTB dataset identifier                          */
	unsigned short		ftf;				/* Frame Type Field (PDType)                       */
	unsigned short		refresh_tmo;		/* refresh timeout                                 */
	NcObjID				pv_oid;				/* object id of the list of process variables      */
	unsigned char		size;				/* dataset size in bytes                           */
	unsigned char		flags;				/* dataset flags                                   */
} WtbDsEntry;


/*---------------------------------------------------------------------------------------------*/

typedef struct PdmEntry {					/**** Process Data Marshalling entry ***************/
	unsigned short		type;				/* PDM type                                        */
	unsigned short		mode;				/* PDM mode                                        */
	unsigned short		cycle;				/* PDM cycle in [ms]                               */
	unsigned short		unused;
	NcObjID				pdm_act_oid;		/* object id of the actions list                   */
	NcObjID				dflt_value_oid;		/* object id of the default values definitions     */
} PdmEntry;

typedef struct PdmActEntry {				/**** PDM action entry *****************************/
	NcEleID				pv_eid;				/* source or destination process variable id       */
	unsigned char		act;				/* action to perform                               */
	unsigned char		param1;				/* first parameter                                 */
	unsigned short		param2;				/* second parameter                                */
} PdmActEntry;


/*=============================================================================================*/
void ncdbext_init(const void **data);

#endif

