/*   LPM_4MPB.C
 ----------------------------------------------------------------------------

     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland

     COPYRIGHT    :   (c) 1994, 1995 ABB Verkehrssysteme AG

     Project      :   RTP Process Data

     Document     :   [1]: IEC 9 332 Part 2: Real Time Protocols
                      [2]: 3EHT 420 087: TCN-JDP RTP Application Layer PD
                                                     User's Manual

     History      :   see end of file

 ----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>

#if defined (O_PCMCIA)
#define O_LE
#endif
#include "lp_sys.h"
#include "apd_incl.h"


/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Static Variable                                                     */
/*                                                                      */
/*----------------------------------------------------------------------*/

static struct LMP_STR_DTA
{
    unsigned short io_addr;
}   lpm_dta;

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Mapping Definitions                                                 */
/*                                                                      */
/*----------------------------------------------------------------------*/

#define LPM_BCR_OFFS_SKRL     0x08
#define LPM_BCR_OFFS_SATN     0x12
#define LPM_BCR_OFFS_ERLN     0x10


#define MFR_OFFSET     0x980
#define PIT_OFFSET     0x1000
#define PCS_OFFSET     0xC00
#define PRT_OFFSET     0x2000

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Auxiliary Functions                                                 */
/*                                                                      */
/*----------------------------------------------------------------------*/

#if defined (_MSC_VER)

#include <conio.h>

#define lpm_outbyte(prt,val)   \
	{unsigned short the_port = lpm_dta.io_addr + (prt);\
	 _outp(the_port,val); }

#define lpm_inbyte(p_val, prt) \
	 {unsigned short the_port = lpm_dta.io_addr + (prt);\
	   *p_val = _inp(the_port); }

#elif defined (__TURBOC__)

#define lpm_outbyte(prt_addr, val)	 \
{unsigned short port_addr = lpm_dta.io_addr + (prt_addr);     \
  {\
 asm PUSH AX; \
 asm PUSH DX; \
 asm MOV DX, port_addr;\
 asm MOV AL, val;\
 asm OUT DX, AL;\
 asm POP DX;    \
 asm POP AX;    \
  }\
}

#define lpm_inbyte(p_val, prt_addr)       \
{unsigned short port_addr = lpm_dta.io_addr + (prt_addr);     \
   {\
 asm PUSH AX; \
 asm PUSH BX; \
 asm PUSH DX; \
 asm PUSH ES; \
 asm LES BX, p_val;\
 asm MOV DX, port_addr;\
 asm IN  AL, DX;\
 asm MOV ES: [BX], AL;\
 asm POP ES; \
 asm POP DX; \
 asm POP BX; \
 asm POP AX; \
    }\
}
#endif

/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Access to MPB functions only after the "Erlaubnisbit" is set        */
/*  This function returns, when the Erlaubnisbit is active              */
/*  The caller must do his BAP access straight away                     */
/*  Further BAP accesses only after new call to this function !         */
/*                                                                      */
/*----------------------------------------------------------------------*/

#define lpm_mpb_wait()              \
        { unsigned char  *p_tmp;     \
          unsigned char   tmp;       \
         p_tmp = &tmp;              \
          tmp   = 0;                 \
          while (tmp != 0xFF) { lpm_inbyte (p_tmp, LPM_BCR_OFFS_ERLN); } }




/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Use this function to write to a BAP register at "offset" 0...F      */
/*  in the BAP address range                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/

void  bcr_put  (unsigned int   offset,
                unsigned char  value)
{
   lpm_mpb_wait ();
   lpm_outbyte (offset, value);
}


/*----------------------------------------------------------------------*/
/*                                                                      */
/*  Use this function to read from  BAP register at "offset" 0...F      */
/*  in the BAP address range                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/

void  bcr_get  (unsigned char * p_value,
                unsigned int    offset)
{
   lpm_mpb_wait();
   lpm_inbyte (p_value, offset);
}


void            lpm_mpb_close (void)
{

   bcr_put (LPM_BCR_OFFS_SKRL, 0x08);
   bcr_put (LPM_BCR_OFFS_SKRL, 0x00);

}

TYPE_APD_RESULT lpm_mpb_init     (unsigned short	 offs_bcr)
{
int  result = LPS_OK;
unsigned char tmp;

   lpm_dta.io_addr = offs_bcr;

   bcr_put (LPM_BCR_OFFS_SKRL, 0x08);
   bcr_put (LPM_BCR_OFFS_SKRL, 0x78);
   bcr_put (LPM_BCR_OFFS_SATN, 0x01);          /* Init SATN */
   bcr_get (&tmp, LPM_BCR_OFFS_SKRL);

   if (tmp != 0x78) { result = LPS_ERROR; }

      return (result);
}



/*----------------------------------------------------------------------*/
/* iNTEL Pointer Conversion Structure                                   */
/*----------------------------------------------------------------------*/


union UNN_ITL_PTR
{
  unsigned short shrt_ptr[2];
  void *	 p_void;
};


/*----------------------------------------------------------------------*/
/*  Extract MPB parameters from DOS envionment/MPBSETUP.DAT             */
/*----------------------------------------------------------------------*/


#define  SEARCH_FILE_LEN 20

struct MPB_INFO
{
   unsigned char  filename  [8];
   unsigned char  sep1      [1];
   unsigned char  version   [1];
   unsigned char  sep2      [1];
   unsigned char  release   [1];
   unsigned char  sep3      [2];
   unsigned short bap          ;
   unsigned short ts           ;
   unsigned short intr         ;
};

/*

  ----------------------------------------------------------------------

    lpm_read_mpb_setup

  ----   Abstract  -----------------------------------------------------

    Reads the C:\MPB\MPBSETUP.DAT file

*/



TYPE_APD_RESULT lpm_read_mpb_setup  (void **         p_ts,
                                     unsigned int *bap_ioaddr,
                                     unsigned int *mpb_intr_nr)
{
char           str[] = "C:\\MPB\\MPBSETUP.DAT";
FILE            *stream;
struct MPB_INFO  mpb_info;
short            done = 0;
union UNN_ITL_PTR ret_val;
unsigned short ts_startseg;

    stream = fopen (str, "rt");

    if (stream != NULL)         /* check if all values are read ...  */
	{
	    if (fread (&mpb_info, SEARCH_FILE_LEN, 1, stream) == 1)
        {
             ts_startseg = mpb_info.ts;       /* take MPB config from file    */
            *bap_ioaddr  = mpb_info.bap;
            *mpb_intr_nr = mpb_info.intr;

            printf ("File C:\\MPB\\MPBSETUP.DAT found. ");
            done	    = 0;
        }
        else
        {
             ts_startseg = 0xE000;		  /* take "default" values	  */
            *bap_ioaddr  = 0x200;
            *mpb_intr_nr = 0x7;

            printf ("File c:\\mpb\\mpbsetup.dat NOT FOUND. ");
             done	    = 1;
        }
        fclose (stream);
    }

    printf ("p_ts %X0000, BAPoff 0x%X\n", ts_startseg, *bap_ioaddr);

    ret_val.shrt_ptr[1] = ts_startseg;
    ret_val.shrt_ptr[0] = 0;
    *p_ts = ret_val.p_void;

    return (done);
}



