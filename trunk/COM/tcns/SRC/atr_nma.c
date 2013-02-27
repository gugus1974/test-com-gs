/************************************************************************/
/*Ven.18.feb.2000       v.1.40              \SW\TCN\ATR\atr_mna.c       */
/************************************************************************/
/* ATR TCN Network Management extensions                                */
/* (c)1997 Ansaldo Trasporti                                            */
/* Written by Giuseppe Carnevale                                        */
/*======================================================================*/


/*======================================================================*/
/* Includes */

#include <string.h>

#include "atr_nma.h"
#include "pi_sys.h"
#include "atr_hw.h"
#include "conf_tcn.h"
#include "sio.h"
#include "ushell.h"
#include "xmodem.h"


/*======================================================================*/
/* Pragmas */

#ifdef __C166__
#pragma WL(0)
#endif


/*======================================================================*/
/* JDP externals not in includes */

extern char *manufact_name;
extern char *device_type;
extern char *station_name;
extern char station_identifier;

#define MAX_TNM_PDU             1024       /* largest size of data in frame */
#define MM_CMD_NOT_EXECUTED     35         /* service failed */

typedef int (*MA_SERVICE_CALL)(const char *in, char *out, int *out_len_p);
typedef int (*MA_SERVICE_CLOSE)(void);

extern int ma_init(void);
extern int ma_subscribe(    unsigned short      cmd,
                            unsigned char       sif_code,
                            MA_SERVICE_CALL     ma_service_call,
                            MA_SERVICE_CLOSE    ma_service_close,
                            void                *service_descr  );


/*======================================================================*/
/* NCDB management */

/*----------------------------------------------------------------------*/
short start_ext_ncdb_update(void)
{
	short err = 0;	/* error code */

    /* open the writing deleting the previous contents  */
    /* and skipping the first header                    */
	Check(hw_ext_data_open(TRUE));
	Check(hw_ext_data_write(0, sizeof(NcObjHeader)));

error:
	return err;
}


/*----------------------------------------------------------------------*/
short write_ext_ncdb_object(const NcObjHeader *header)
{
	short 			err = 0;									/* error code  */
    unsigned short  data_len = sizeof(*header) + header->size;  /* data length */

	/* write the data */
	Check(hw_ext_data_write((void*)header, data_len));

error:
	/* close on failure */
	if (err) hw_ext_data_close();
	return err;
}


/*----------------------------------------------------------------------*/
short end_ext_ncdb_update(void)
{
	short 			err = 0;	/* error code  */
	NcObjHeader		header;		/* header */

	/* close the file */
	Check(hw_ext_data_close());

	/* write the first header reopening the file without erase */
	Check(hw_ext_data_open(FALSE));
	header.oid  = NC_START_OF_DB_OBJ_ID;
	header.size = 0;
	Check(write_ext_ncdb_object(&header));
	Check(hw_ext_data_close());

error:
	/* close on failure */
	if (err) hw_ext_data_close();
	return err;
}


/*======================================================================*/
/* XMODEM NCDB transfer */

/*----------------------------------------------------------------------*/

#define OBJ_BUF_SIZE	4096	/* this is the max object size we can transfer */

static FilterNcdbObjFptr filter_ncdb_obj_fptr;	/* NCDB objects filter function */
static char				 *obj_buf;				/* object data buffer           */
static int				 obj_pos;				/* our position in the obj_buf  */

static short write_ext_obj_pkt( long index,
                                char *pkt   )
{
	short err = XMODEM_OK;	/* error code */
    int   obj_tot_size;     /* total size (including header) of the */
                            /* object we are considering            */

	/* append the received data */
	memmove(&obj_buf[obj_pos], pkt, XMODEM_PACKET_SIZE);
	obj_pos += XMODEM_PACKET_SIZE;

	/* check that we have a new object (not the trailer) */
    while (    obj_pos >= sizeof(NcObjHeader)
            && ((NcObjHeader*)obj_buf)->oid != NC_END_OF_DB_OBJ_ID
            && (obj_tot_size = ((NcObjHeader*)obj_buf)->size + sizeof(NcObjHeader)) <= obj_pos
          )
    {
		/* check for endian compatibility */
        if (((NcObjHeader*)obj_buf)->oid == NC_INVALID_START_OBJ_ID)
            Check(XMODEM_NCDB_FORMAT_ERR);

		/* call the filter function if specified */
        if (filter_ncdb_obj_fptr)
            (*filter_ncdb_obj_fptr)((NcObjHeader*)obj_buf);

		/* write the object in the buffer if it isn't the NCDB header */
        if ( ((NcObjHeader*)obj_buf)->oid != NC_START_OF_DB_OBJ_ID )
            if (write_ext_ncdb_object((void*)obj_buf))
                Check(XMODEM_NCDB_WRITE_ERR);
		
		/* remove the object from the buffer */
		obj_pos -= obj_tot_size;
        if (obj_pos)
            memmove(obj_buf, &obj_buf[obj_tot_size], obj_pos);
	}

	/* check for overflows in the object buffer */
    if (obj_pos + XMODEM_PACKET_SIZE > OBJ_BUF_SIZE)
        Check(XMODEM_NCDB_MEMORY_ERR);

error:
	return err;
}

short xmodem_ext_ncdb_rcv(FilterNcdbObjFptr filter_ncdb_obj_func)
{
	short	err; 	/* error code       */

	/* save the filter function pointer */
	filter_ncdb_obj_fptr = filter_ncdb_obj_func;

	/* allocate the objects buffer */
	obj_buf = pi_alloc(OBJ_BUF_SIZE);
    if (!obj_buf)
        Check(XMODEM_NCDB_MEMORY_ERR);
	obj_pos = 0;

	/* do the NCDB transfer */
	Check(start_ext_ncdb_update());
    err = xmodem_crc_rcv(write_ext_obj_pkt);
	end_ext_ncdb_update();

error:
    if (obj_buf)
        pi_free(obj_buf);
    if (err)
    {  /* erase what we could have written till now */
        if (start_ext_ncdb_update() == ATR_NMA_OK)
            end_ext_ncdb_update();
	}
	return err;
}


/*======================================================================*/
/* NMA interface implementation */

/*----------------------------------------------------------------------*/
static int read_ncdb_call(  const char  *in,
                            char        *out,
                            int         *out_len_p  )
{
	short			err;
	void			*p_element;
	unsigned short	element_size;

	Check(nc_get_db_element(*(unsigned short*)in, &p_element, &element_size));
	p_element = (char *)p_element - sizeof(NcObjHeader);
	element_size += sizeof(NcObjHeader);
    if (element_size > MAX_TNM_PDU)
        Check(MM_CMD_NOT_EXECUTED);

	pi_copy8(out, p_element, element_size);
	*out_len_p += element_size;

error:
	return err ? MM_CMD_NOT_EXECUTED : 0;
}


/*----------------------------------------------------------------------*/
static int start_ncdb_update_call(  const char  *in,
                                    char        *out,
                                    int         *out_len_p  )
{
    if (start_ext_ncdb_update())
        return MM_CMD_NOT_EXECUTED;
    else
        return 0;
}


/*----------------------------------------------------------------------*/
static int write_ncdb_call( const char  *in,
                            char        *out,
                            int         *out_len_p  )
{
    if (write_ext_ncdb_object((void*)in))
        return MM_CMD_NOT_EXECUTED;
    else
        return 0;
}


/*----------------------------------------------------------------------*/
static int end_ncdb_update_call(    const char  *in,
                                    char        *out,
                                    int         *out_len_p  )
{
    if (end_ext_ncdb_update())
        return MM_CMD_NOT_EXECUTED;
    else
        return 0;
}


/*----------------------------------------------------------------------*/
static char	 *out_buff = 0;
static short out_buff_pos = 0;

static void put_in_buff(    const char  *buf,
                            short       req_num )
{
    for (; req_num > 0; req_num--, buf++)
    {
        if (*buf != 13)
        {
			out_buff[out_buff_pos++] = *buf;
            if (out_buff_pos == MAX_TNM_PDU)
            {
  				out_buff_pos--;
   	 			return;
   	   		}
 		}
  	}
}

static int shell_call(  const char  *in,
                        char        *out,
                        int         *out_len_p  )
{
	short 			err;
	SioPutFuncPtr	old_put = sio_set_put(put_in_buff);

	/* allocate the output buffer */
	out_buff = pi_alloc(MAX_TNM_PDU);
    if (!out_buff)
        Check(MM_CMD_NOT_EXECUTED);
	out_buff_pos = 0;

	/* execute the command and copy the result */
	Check(ushell_exec(in));
	putchar(0);
	pi_copy8(out, out_buff, out_buff_pos);
	*out_len_p += out_buff_pos;
	
error:
	/* cleanup */
	sio_set_put(old_put);
	pi_free(out_buff);
	return err ? MM_CMD_NOT_EXECUTED : 0;
}


/*----------------------------------------------------------------------*/
extern const char application_version[];
static char station_name_string[32];
short atr_nma_init(void)
{
	short err;

	/* fill the station identification fields */
	manufact_name = "Ansaldo Trasporti S.p.A.";
	device_type   = (char*)application_version;
    sprintf(    station_name_string,
                "%s/%s",
                atr_hw_name,
                nc_get_app_st_conf()->txt_name);
	station_name  = station_name_string;
	station_identifier = 0;

	/* initialize the network management */
	Check(ma_init());
	Check(ma_subscribe(0, ATR_NMA_READ_NCDB_SIF,         read_ncdb_call,         0, "read_ncdb"));
	Check(ma_subscribe(0, ATR_NMA_START_NCDB_UPDATE_SIF, start_ncdb_update_call, 0, "start_ncdb_update"));
	Check(ma_subscribe(0, ATR_NMA_WRITE_NCDB_SIF,        write_ncdb_call,        0, "write_ncdb"));
	Check(ma_subscribe(0, ATR_NMA_END_NCDB_UPDATE_SIF,   end_ncdb_update_call,   0, "end_ncdb_update"));
	Check(ma_subscribe(0, ATR_NMA_SHELL_SIF, 		     shell_call,             0, "shell"));

error:
	return err;
}


/*----------------------------------------------------------------------*/
static short		call_semaphore = -1;
static AM_RESULT	call_status;
static char			*call_in_msg;
static CARDINAL32	call_in_msg_size;

static void call_conf(  unsigned char       caller_fct,
                        char                *ext_ref,
                        const AM_ADDRESS    *replier,
                        char                *in_msg_addr,
                        CARDINAL32          in_msg_size,
                        AM_RESULT           status      )
{
	/* save the results and free the caller */
	call_status = status;
	call_in_msg = in_msg_addr;
	call_in_msg_size = in_msg_size;
	pi_post_semaphore(call_semaphore, 0);
}

short atr_nma_call( unsigned        char veh,
                    unsigned        char sta,
                    unsigned        char tc,
                    unsigned        char sif,
                    const char      *data_out,
                    unsigned short  data_out_len,
                    char            **data_in,
                    unsigned short  *data_in_len_p  )
{
	short		err = 0;		/* error code */
	char		*p = 0;			/* temporary buffer pointer */
	AM_ADDRESS	dest_net_addr;	/* destination adress */
	
	/* create the semaphore at the first call */
	if (call_semaphore == -1) {
		call_semaphore = pi_create_semaphore(0, PI_ORD_FCFS, &err);
		Check(err);
	}

	/* create the output buffer */
	p = pi_alloc(2 + data_out_len);
	if (!p) Check(MM_CMD_NOT_EXECUTED);
	p[0] = 0x17;	// !!! should be 0x02; left to the old value for compatibility
	p[1] = sif;
	pi_copy8((void*)(p + 2), (void*)data_out, data_out_len);

	/* send the request to the destination */
	dest_net_addr.vehicle = AM_PHYS_ADDR | veh;
	dest_net_addr.fct_or_sta = sta;
	dest_net_addr.next_sta = sta;
	dest_net_addr.tc = tc;
    am_call_requ(   254,
                    &dest_net_addr,
                    p,
                    2+data_out_len,
                    0,
                    2+MAX_TNM_PDU,
                    0,
                    call_conf,
                    0               );
    pi_pend_semaphore(  call_semaphore,
                        PI_FOREVER,
                        0           );
	pi_free(p);
	Check(call_status);
	
	if (data_in) {
		/* remove the Network Management header */
		p = pi_alloc(call_in_msg_size - 2);
		if (!p) {
			pi_free(call_in_msg);
			Check(MM_CMD_NOT_EXECUTED);
 		}
        pi_copy8(   (void*)p,
                    (void*)(call_in_msg + 2),
                    call_in_msg_size - 2        );
		
		/* save the results */
		*data_in = p;
	}
	pi_free(call_in_msg);

    if (data_in_len_p)
        *data_in_len_p = call_in_msg_size - 2;

error:
	return err;
}


/*----------------------------------------------------------------------*/

