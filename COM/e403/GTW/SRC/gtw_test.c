/*=============================================================================================*/
/* Gateway debug routines              														   */
/* 																							   */
/* Version 1.0																				   */
/*																							   */
/* (c)1999 Ansaldo Trasporti																   */
/* Written by Giuseppe Carnevale                                    		                   */
/* 31.03.2000 Balbi/Iusto Aggiunti comandi shell suspend e halt  (ATR 01)                      */     
/*=============================================================================================*/


/*=============================================================================================*/
/* Includes */

#include "pi_sys.h"
#include "am_sys.h"
#include "ushell.h"


/*=============================================================================================*/
/* Shell commands */

/*---------------------------------------------------------------------------------------------*/

#ifdef NEVER

#include "\windows\desktop\zlib.src\zlib.h"

static short xx_cmd(short argc, char *argv[])
{
	int   err = 0, i;
	Bytef *source = 0, *dest = 0, *dest2 = 0;
	uLongf sourceLen, destLen, dest2Len;

	argc = argc;
	argv = argv;

	if (argc < 2) return -1;

	sourceLen = parse_number(argv[1]);
	destLen = sourceLen + sourceLen/10 + 12;
	dest2Len = 2048;

	source = (Bytef*)pi_alloc(sourceLen);
	if (!source) Check(-9999);
	dest   = (Bytef*)pi_alloc(destLen);
	if (!dest) Check(-9998);
	dest2  = (Bytef*)pi_alloc(dest2Len);
	if (!dest2) Check(-9998);

	if (sourceLen == 8) {
		source[0] = 0xF0;
		source[1] = 0xF1;
		source[2] = 0x38;
		source[3] = 0x8D;
		source[4] = 0xFB;
		source[5] = 0xF0;
		source[6] = 0xD1;
		source[7] = 0xA2;
	}

	printf("Source:\n");
	dump_buffer(FALSE, (unsigned char*)source, sourceLen);
	printf("\n");

	if (argc == 3) Check(compress2(dest, &destLen, source, sourceLen, parse_number(argv[2])));
	else Check(compress(dest, &destLen, source, sourceLen));

	printf("Compressed:\n");
	dump_buffer(FALSE, (unsigned char*)dest, destLen);
	printf("\n");

	Check(uncompress(dest2, &dest2Len, dest, destLen));

	if (dest2Len != sourceLen) printf("Length error!\n");
	for (i = 0; i < sourceLen; i++) if (source[i] != dest2[i]) {
		printf("Compression error!!:\n");
		dump_buffer(FALSE, (unsigned char*)dest2, dest2Len);
		printf("\n");
		break;
	}

error:
	if (source) pi_free(source);
	if (dest) pi_free(dest);
	if (dest2) pi_free(dest2);
	if (err) printf("Got error: %d\n", err);
	return 0;
}

#else

static short xx_cmd(short argc, char *argv[])
{
	return 0;
}

#endif

/*---------------------------------------------------------------------------------------------*/

#define MAX_MSG_LEN 1024

static char	*buf = (char*)0x100;	/* pointer to the message      */
static int	mess_len;				/* message length              */

static int  abort_request = FALSE;	/* the user requested to stop  */
static int  calls_in_progress = 0;	/* number of calls in progress */

static void call_conf(unsigned char caller_fct, char *ext_ref, const AM_ADDRESS *replier,
					  char *in_msg_addr, CARDINAL32 in_msg_size, AM_RESULT status)
{
	caller_fct = caller_fct;
	ext_ref = ext_ref;
	replier = replier;	

	if (status) printf("\ncall_conf: status=%d\n", (short)status);

    if (in_msg_addr) am_buffer_free(in_msg_addr, in_msg_size);
    
    if (!abort_request)
		am_call_requ(caller_fct, replier, buf, mess_len, 0, MAX_MSG_LEN, 0, call_conf, ext_ref);
	else calls_in_progress--;
}

static void rcv_conf(unsigned char replier_fct, const AM_ADDRESS *caller, char *in_msg_addr,
					      CARDINAL32 in_msg_size, char *ext_ref)
{
    am_reply_requ(replier_fct, 0, 0, ext_ref, AM_OK);
    printf("\n<MESSAGE> to function %d from vehicle %d, function %d, station %d, topography %d:\n",
    	   replier_fct, caller->vehicle, caller->fct_or_sta, caller->next_sta, caller->tc);
    if (!ext_ref) dump_buffer(FALSE, (void*)in_msg_addr, in_msg_size);
    else {
		short i;
		printf(":: ");
 		for (i = 0; i < in_msg_size; i++) printf("%X ", in_msg_addr[i]);
		printf("::\n");
    }
    am_buffer_free(in_msg_addr, in_msg_size);
}

static void reply_conf(unsigned char replier_fct, char *ext_ref)
{
    AM_RESULT		status;			/* am error code */
    am_rcv_requ(replier_fct, 0, MAX_MSG_LEN, ext_ref, &status);
}


static short yy_cmd(short argc, char *argv[])
{
	AM_ADDRESS	dest_net_addr;			/* destination adress         */
	char		our_fct;				/* caller function            */
	int			n;						/* number of concurrent calls */
	int			i;						/* index                      */

	dest_net_addr.vehicle    = parse_number(argv[1]);
	dest_net_addr.fct_or_sta = parse_number(argv[2]);
	dest_net_addr.next_sta   = AM_UNKNOWN_DEVICE;
	dest_net_addr.tc         = AM_ANY_TC;

	n        = parse_number(argv[3]);
	our_fct  = parse_number(argv[4]);
	mess_len = parse_number(argv[5]);

	/* start all the calls */
	abort_request = FALSE;
	calls_in_progress = n;
	for (i = 0; i < n; i++) {
		am_call_requ(our_fct, &dest_net_addr, buf, mess_len, 0, MAX_MSG_LEN, 0, call_conf, i);
		dest_net_addr.fct_or_sta++;
	}

	/* wait for a keypress */
	while(!sio_poll_key(10));
	abort_request = TRUE;

	/* wait for the calls ending */
	while (calls_in_progress) pi_wait(10);

	return 0;
}


/*---------------------------------------------------------------------------------------------*/
static short sleep_cmd(short argc, char *argv[])
{
//	d_sleep();
}

static short hw_cmd(short argc, char *argv[])
{
	hw_halt();
}

static short suspe_cmd(short argc, char *argv[])
{
	pi_task_exit();
}

/*---------------------------------------------------------------------------------------------*/

/*=============================================================================================*/
/* Entry point */

void gtw_test_install(void)
{
	ushell_register("xx", "", "xx", xx_cmd);
	ushell_register("yy", "", "yy", yy_cmd);

	ushell_register("sleep", "", "Force the GTW in sleep mode", sleep_cmd);
	ushell_register("suspend", "", "Attiva Routine pi_task_exit", suspe_cmd); // ATR (01)
	ushell_register("halt", "", "Force the GTW in hardware fault", hw_cmd);   // ATR (01)

}

/*=============================================================================================*/

