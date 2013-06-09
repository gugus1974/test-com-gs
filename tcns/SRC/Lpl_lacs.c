/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Fixed _lp_pv_size() for Big Endian processors                                       */
/*==============================================================================================*/


/*   LPL_LACS.C
 ----------------------------------------------------------------------------

     Property of  :   ABB Verkehrssyteme AG, CH-8050 Z걊ich / Switzerland
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG

 ----------------------------------------------------------------------------

     Project      :   MicOS
     Subproject   :   Process Data

 ----------------------------------------------------------------------------

     File         :   LPL_LACS.C   -   Link Process Lowest Level of Access

     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3
                      [2]: Link Layer Process Specification
                      [3]: Link Layer Process Design

     Abstract     :   Traffic Store Access Routines

     Remarks      :

     Dependencies :

     Accepted     :

 ----------------------------------------------------------------------------

     HISTORY      :   See end of file

 ----------------------------------------------------------------------------

        !!! Contains Case Tables in functions   lpl_put_prt_access
                                                lpl_get_prt_access
*/

#define  LPL_LACS_C
#define  LPI_MODULE_NAME "lp_lacs"


#if defined (O_PCMCIA)
#define O_LE
#endif

#include "lp_sys.h"             /*   LP user interface                      */
#include "pi_sys.h"

#include "lpi_ifce.h"           /*   LP import interface                    */
#include "lpl_4tcn.h"           /*   TS access routines                     */
#include "lpa_as2c.h"           /*   possibly tuned by ASM implementation   */
#include "lpc_blck.h"           /*   control block functionality            */

#include "lp_bits.h"            /*   Bit manipulation functions             */




/*
 ----------------------------------------------------------------------------
|
| globals
|
 ------------   A B S T R A C T   -------------------------------------------
  module global variables
*/


/*
 ----------------------------------------------------------------------------
|
| hardware dependant includes
|
 ------------   A B S T R A C T   -------------------------------------------

  !!! These files are only included for certain hardwares

#ifdef    O_PCW3
#include  "hipcwf.h"
#endif

*/

/* new page
 ----------------------------------------------------------------------------
|
| local MACROS  P_DEST_T_... P_SRCE_T_..., P_PCS
|
 ------------   A B S T R A C T   -------------------------------------------


  Local Macros to abbreviate the cast operating to pointers.

*/

#define P_DEST_T_B1  (*((TYPE_LP_BOOL_1  *) ((char *) p_dest)))
#define P_DEST_T_B2  (*((TYPE_LP_BOOL_2  *) ((char *) p_dest)))
#define P_DEST_T_BCD (*((TYPE_LP_BCD_4   *) ((char *) p_dest)))
#define P_DEST_T_FLD (*((TYPE_LP_FLD_8   *) ((char *) p_dest)))

#define P_SRCE_T_B1  (*((TYPE_LP_BOOL_1  *) ((char *) p_srce)))
#define P_SRCE_T_B2  (*((TYPE_LP_BOOL_2  *) ((char *) p_srce)))
#define P_SRCE_T_BCD (*((TYPE_LP_BCD_4   *) ((char *) p_srce)))
#define P_SRCE_T_FLD (*((TYPE_LP_FLD_8   *) ((char *) p_srce)))

#define P_PCS           ((TYPE_LPL_PCS *) p_pcs)

#if defined (O_LE)

#define lpl_copy_16_sb      lpa_swap_bitset_16
#define lpl_put_a16_sb      lpa_swap_16

#define lpl_copy_32_sb_sw(p_dest, p_srce)   {                            \
      *(char *) ((char *) p_dest + 3) = *(char *) ((char *) p_srce    ); \
      *(char *) ((char *) p_dest + 2) = *(char *) ((char *) p_srce + 1); \
      *(char *) ((char *) p_dest + 1) = *(char *) ((char *) p_srce + 2); \
      *(char *) ((char *) p_dest    ) = *(char *) ((char *) p_srce + 3); }

#define lpl_put_a32_sb_sw(p_dest, p_srce, count) {                       \
      count++; while (--count != 0)                                      \
      {                                                                  \
      *(char *) ((char *) p_dest + 3) = *(char *) ((char *) p_srce    ); \
      *(char *) ((char *) p_dest + 2) = *(char *) ((char *) p_srce + 1); \
      *(char *) ((char *) p_dest + 1) = *(char *) ((char *) p_srce + 2); \
      *(char *) ((char *) p_dest    ) = *(char *) ((char *) p_srce + 3); \
    p_srce = (char *) p_srce + 4; p_dest = (char *) p_dest + 4;    } }
#else

#define lpl_copy_16_sb(p_dest, p_srce)    {                              \
      *((unsigned short *) (p_dest)) = *((unsigned short *) (p_srce)); }

#define lpl_put_a16_sb      pi_copy16

#define lpl_copy_32_sb_sw(p_dest, p_srce) {                              \
      *((unsigned long *) (p_dest)) = *((unsigned long *) (p_srce)); }

#define lpl_put_a32_sb_sw(p_dest, p_srce, count) {                       \
    pi_copy16 (p_dest, p_srce, (count<<1));}

#endif

/*  new page
 ----------------------------------------------------------------------------
|
| type          TYPE_LPL_
|
 ------------   A B S T R A C T   -------------------------------------------


  Local types are defined, because conflicts between little and big endian
  representations require some swapping within types.

*/

typedef         TYPE_LP_8               TYPE_LPL_8;
typedef         TYPE_LP_16              TYPE_LPL_16;

typedef struct
{
        TYPE_LPL_16     low;
        TYPE_LPL_16     hgh;

}                                       TYPE_LPL_32;

typedef struct
{
        TYPE_LPL_16     low;
        TYPE_LPL_16     hgh;
        TYPE_LPL_16     ticks;
}                                       TYPE_LPL_TIMEDATE_48;


/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_chk_prt_size
|
| (in )         prt_indx
| (in )         prt_size
|
| return value  result          LPS_OK
|                               LPS_CONFIG      Size > 32 bytes
|                                               Size odd
|                                               Size incompatible to hw_type
|
 ------------   A B S T R A C T   -------------------------------------------


  The port size is checked for valid. BAP_1 traffic store types support only
  the standard sizes (2, 4, 8, 16, 32 bytes). There are restrictions
  concerning the placing of larger sized ports. These restrictions are
  verified.

*/


int             _lpl_chk_prt_size       (void *         p_cb,
                     unsigned short prt_indx,
                     unsigned short size)
{
int     result = LPS_OK;


    if (lpc_get_ts_type(p_cb) == LP_TS_TYPE_D)
    {
    result = lpl_chk_prt_size_type_d(p_cb, size) == LPS_OK ?
                           result : LPS_CONFIG ;
    }
    else
    {
    unsigned short tmp_size = size & LP_CFG_XX_BYTES_MSK;

    result = ((tmp_size == LP_CFG_02_BYTES) ||
          (tmp_size == LP_CFG_04_BYTES) ||
          (tmp_size == LP_CFG_08_BYTES) ||
          (tmp_size == LP_CFG_16_BYTES) ||
          (tmp_size == LP_CFG_32_BYTES)) ? result : LPS_CONFIG;

    result = ((tmp_size > LP_CFG_08_BYTES) && (prt_indx & 1) != 0) ||
         ((tmp_size > LP_CFG_16_BYTES) && (prt_indx & 3) != 0) ||
          (tmp_size > LP_CFG_32_BYTES) ? LPS_CONFIG : result;
    }

    return (result);
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-03  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_chk_prt_size                                            */

/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_clr_pit
|
| (in )         p_cb                    Control Block Handle
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  The port address table is cleared. This will have the affect of closing
  all ports in this traffic store.

  Requirement: lp_create () must have created a valid port index table.

*/

void            _lpl_clr_pit            (void *         p_cb)

{
void *          pb_pit;
unsigned short  pit_size;

    pb_pit = lpc_get_pb_pit (p_cb);
                                       #if !defined (O_PCMCIA)
    if ((pb_pit ) != NULL)
                                       #endif
    {

        pit_size = lpl_gen_pit_offset (p_cb, lpc_get_prt_addr_max (p_cb) + 1);
        lpi_set_ts_array (p_cb, pb_pit, 0, pit_size);
    }
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-11-09  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_clr_pit  */


/* new page
 ----------------------------------------------------------------------------
|
| function      lpl_put_def_values
|
| (out)         pb_prt
| (in )         pb_def
| (in )         dck_max
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------

  pb_def is the base pointer to the default value buffer. The default value
  buffer is a one paged version of the traffic store port data buffer
  containing the start up values for all traffic store bus variables.

  The contents of pb_def are copied into both pages of pb_prt. The contents
  are copied in blocks of 32 bytes (one 16 word port).

  The definition of prt_indx_max ((prt_indx_max + 1) MOD 4 = 0) guarantees,
  that sufficient memory is reserved within the traffic store for the last
  block.

*/

void _lpl_put_def_values (void *  p_cb,
                          void *  p_tmp_def)
{
    int              i;                      /* loop control for port sets   */
    LC_TYPE_TS_PRT * p_prt;
    LC_TYPE_TS_PGE * p_def = p_tmp_def;

    i       = (lpc_get_prt_indx_max (p_cb) + 1) >> 2;
    p_prt   =  lpc_get_pb_prt (p_cb);

    while   (i-- > 0)
    {
        pi_copy16  ((void*)&p_prt[0][0],
                    (void*)p_def,
                    sizeof (LC_TYPE_TS_PGE) >> 1);

        pi_copy16  ((void*)&p_prt[0][1],
                    (void*)p_def,
                    sizeof (LC_TYPE_TS_PGE) >> 1);

        p_def++;
        p_prt++;
    }
}

/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-10-07  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_put_def_values                                          */


/* new page
 ----------------------------------------------------------------------------
|
| function      lpl_put_prt_size
|
| (in )         p_pcs
| (in )         prt_size                size in bytes
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  The port size is written to the pcs. BAP_1 traffic store types support only
  the standard sizes (2, 4, 8, 16, 32 bytes). Other types of traffic store
  support any even size between 2 and 64 bytes.

  The port size is converted into function codes. The function codes are
  defined as follows:

        0 -  2 bytes
        1 -  4 bytes
        2 -  8 bytes
        3 - 16 bytes
        4 - 32 bytes
        7 - 2, 4, 6, 8, ...., 64 bytes

  This function code is written into the fcode field. A second field for
  the actual size (size_fcode_7) is used when fcode 7 specifies that a
  non standard size is used.

*/

void            _lpl_put_prt_size       (void *         p_cb,
                                         void *         p_pcs,
                                         unsigned short size)
{
int     fcode   = 0;
    p_cb = p_cb;
    if (size == LP_CFG_02_BYTES_DSW)
    {
        fcode = LPL_PCS_FCODE_F;
    }
    else
    {
        size = size >> 1;

        while ((size != 1) && (size != 0))
        {
                size = size >> 1;
                fcode++;
        }
    }
    lpl_put_prt_fcode (p_cb, p_pcs, fcode);

}

void            _lpl_tb_put_prt_size    (void *         p_pcs,
                                         unsigned short size)
{
    lpl_put_prt_fcode (NULL, p_pcs, LPL_PCS_FCODE_6);
    ((TYPE_LPL_PCS_D *) p_pcs)->size = size;
}


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|  0.1  BAEI-2  91-09-03  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_put_prt_size                                            */


/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_put_prt_access
|
| (out)         p_dest                  Pointer to destination
| (in )         p_srce                  Pointer to source
| (in )         pv_format
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------

  This routine copies the bus variable between the traffic store and the
  application.

  The source pointer points to the application variable.
  The destination pointer points to the base of the invalid page of the port
  data buffer.

  The port pointer is adjusted by adding the byte offset derived from the
  bit offset (pv_format) to the pointer.

  If coherence problems for the target variable can arise, the caller must
  lock the task switching mechanism or prevent interrupts from coming through.

*/

void            _lpl_put_pv_x           (void *                 p_port,
                                         struct STR_LP_PV_X *   p_pv_x,
                                         unsigned short         var_count)


{
register void * p_dest;
         void * p_srce;
int             i;


    while (var_count-- != 0)
    {
        p_dest = (void *) (((char *) p_port) + p_pv_x->byte_offset);
        p_srce = p_pv_x->p_appl;

        switch (p_pv_x->type)
        {

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BOOL_1:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bool_1 (&P_DEST_T_FLD,
                                 p_pv_x->bit_offset,
                                 lp_get_bool_1 (P_SRCE_T_B1));

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BOOL_2:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bool_2  (&P_DEST_T_FLD,
                                  p_pv_x->bit_offset,
                                  lp_get_bool_2 (P_SRCE_T_B2));

        break;

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BCD_4:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bcd_4   (&P_DEST_T_FLD,
                                  p_pv_x->bit_offset,
                                  lp_get_bcd_4 (P_SRCE_T_BCD));

        break;

/* ------------------------------------------------------------------------ */

        case LPL_BITSET_8:
        case LPL_CARD_8:
        case LPL_SIGN_8:

/* ------------------------------------------------------------------------ */


             *(char  *) ((char *) p_dest) = *(char *) p_srce;

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BITSET_16:

/* ------------------------------------------------------------------------ */

                 lpa_swap_bitset_16 (p_dest, p_srce);

        break;

/* ------------------------------------------------------------------------ */

        case LPL_CARD_16:
        case LPL_SIGN_16:
        case LPL_FRAC_200_16:
        case LPL_FRAC_400_16:
        case LPL_FRAC_800_16:

/* ------------------------------------------------------------------------ */

             *(short *) p_dest = *(short *) p_srce;

        break;


/* new page
   ------------------------------------------------------------------------ */

        case LPL_TIMEDATE_48:           /* Swap the high and low 16 bits    */

/* ------------------------------------------------------------------------ */

             ((TYPE_LPL_TIMEDATE_48 *) p_dest)->ticks =
             ((TYPE_LPL_TIMEDATE_48 *) p_srce)->ticks;

/*      !!! NO BREAK - fall into LPL_TIMEDATE_48 continued to copy rest     */



/* ------------------------------------------------------------------------ */

/* !!!  case LPL_TIMEDATE_48 continued, must not be moved                   */
        case LPL_BITSET_32:             /* Swap the high and low 16 bits    */
        case LPL_REAL_32:
        case LPL_CARD_32:
        case LPL_SIGN_32:

/* ------------------------------------------------------------------------ */

                 lpa_swap_long_32 (p_dest, p_srce);

        break;


/* new page
   ------------------------------------------------------------------------ */

        case LP_TYPE_A8_ODD:

/* ------------------------------------------------------------------------ */
#if defined (O_LE)
                *(char *) ((char *) p_dest) = *(char *) ((char *) p_srce);
                if (p_pv_x->size > 1)
                {
                    p_dest = ((char *) p_dest) + 2;
                    p_srce = ((char *) p_srce) + 1;
                }
                else
                {
                    break;
                }
#endif
/*
   ------------------------------------------------------------------------
        !!!  No break from odd array 8!                                     */
        case LP_TYPE_A8_EVEN:

/* ------------------------------------------------------------------------ */

#if defined (O_LE)
                i = (p_pv_x->size) >> 1;
                lpa_swap_16 (p_dest, p_srce, i);
#else
                pi_copy8 (p_dest, p_srce, p_pv_x->size);
#endif
        break;


/* ------------------------------------------------------------------------ */

        case LP_TYPE_A16_SIGN:
        case LP_TYPE_A16_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             lpa_copy_16 (p_dest, p_srce, i);

        break;


/* ------------------------------------------------------------------------ */

        case LP_TYPE_A32_SIGN:
        case LP_TYPE_A32_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             lpa_swap_32 (p_dest, p_srce, i);

        break;



/* new page
   ------------------------------------------------------------------------ */

        default:

/* ------------------------------------------------------------------------ */

            lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "unknown data type"); /*$H. unknown data type */

        break;

        }

        p_pv_x++;

   }

}


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  92-10-05  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_put_prt_access */




/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_get_prt_access
|
| (in )         p_port                  Pointer to active data port page
| (in )         p_pv_x                  Pointer to input list
|
|        out    -
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  This routine copies the bus variable between the traffic store and the
  application.

  The source pointer points to the base of a buffer containing a copy of the
  traffic store port merged with the force variables from the force table.
  The destination pointer points to the application variable.

  The port pointer is adjusted by adding the byte offset derived from the
  bit offset (pv_format) to the pointer.

*/

void            _lpl_get_pv_x           (void *                 p_port,
                                         struct STR_LP_PV_X *   p_pv_x,
                                         unsigned short         var_count)


{

register void * p_dest;
         void * p_srce;
int             i;

    while (var_count-- != 0)
    {
        p_dest = p_pv_x->p_appl;
        p_srce = (void *) (((char *) p_port) + p_pv_x->byte_offset);

        switch (p_pv_x->type)
        {

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BOOL_1:

/* ------------------------------------------------------------------------ */

                 lp_put_bool_1     (&P_DEST_T_B1,
                                    lp_get_fld_bool_1 (P_SRCE_T_FLD,
                                                       p_pv_x->bit_offset));

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BOOL_2:

/* ------------------------------------------------------------------------ */

                 lp_put_bool_2     (&P_DEST_T_B2,
                                    lp_get_fld_bool_2 (P_SRCE_T_FLD,
                                                       p_pv_x->bit_offset));

        break;

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BCD_4:

/* ------------------------------------------------------------------------ */

                 lp_put_bcd_4     (&P_DEST_T_BCD,
                                    lp_get_fld_bcd_4  (P_SRCE_T_FLD,
                                                       p_pv_x->bit_offset));
        break;

/* ------------------------------------------------------------------------ */

        case LPL_BITSET_8:
        case LPL_CARD_8:
        case LPL_SIGN_8:

/* ------------------------------------------------------------------------ */


             *(char  *) ((char *) p_dest) = *(char *) p_srce;

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BITSET_16:

/* ------------------------------------------------------------------------ */

                 lpa_swap_bitset_16 (p_dest, p_srce);

        break;

/* ------------------------------------------------------------------------ */

        case LPL_CARD_16:
        case LPL_SIGN_16:
        case LPL_FRAC_200_16:
        case LPL_FRAC_400_16:
        case LPL_FRAC_800_16:

/* ------------------------------------------------------------------------ */

             *(short *) p_dest = *(short *) p_srce;

        break;


/* new page
   ------------------------------------------------------------------------ */

        case LPL_TIMEDATE_48:           /* Swap the high and low 16 bits    */

/* ------------------------------------------------------------------------ */

             ((TYPE_LPL_TIMEDATE_48 *) p_dest)->ticks =
             ((TYPE_LPL_TIMEDATE_48 *) p_srce)->ticks;

/*      !!! NO BREAK - fall into LPL_TIMEDATE_48 continued to copy rest     */



/* ------------------------------------------------------------------------ */

/* !!!  case LPL_TIMEDATE_48 continued, must not be moved                   */
        case LPL_BITSET_32:             /* Swap the high and low 16 bits    */
        case LPL_REAL_32:
        case LPL_CARD_32:
        case LPL_SIGN_32:

/* ------------------------------------------------------------------------ */

                 lpa_swap_long_32 (p_dest, p_srce);

        break;


/* new page
   ------------------------------------------------------------------------ */

        case LP_TYPE_A8_ODD:

/* ------------------------------------------------------------------------ */

            #if defined (O_LE)
                *(char *) ((char *) p_dest) = *(char *) ((char *) p_srce);
                if (p_pv_x->size > 1)
                {
                    p_dest = ((char *) p_dest) + 1;
                    p_srce = ((char *) p_srce) + 2;
                }
                else
                {
                    break;
                }
            #else
            /*  fall into case "type even" for pi_copy8 */
            #endif
/*
   ------------------------------------------------------------------------
        !!!  No break from odd array 8!                                     */
        case LP_TYPE_A8_EVEN:

/* ------------------------------------------------------------------------ */

             #if defined (O_LE)
                i = (p_pv_x->size) >> 1;
                lpa_swap_16 (p_dest, p_srce, i);
             #else
                pi_copy8 (p_dest, p_srce, p_pv_x->size);
             #endif
        break;


/* ------------------------------------------------------------------------ */

        case LP_TYPE_A16_SIGN:
        case LP_TYPE_A16_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             pi_copy16 (p_dest, p_srce, i);

        break;


/* ------------------------------------------------------------------------ */

        case LP_TYPE_A32_SIGN:
        case LP_TYPE_A32_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             lpa_swap_32 (p_dest, p_srce, i);

        break;



/* new page
   ------------------------------------------------------------------------ */

        default:

/* ------------------------------------------------------------------------ */

            lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "unknown data type");   /*$H. unknown data type */

        break;
        }

        p_pv_x++;

   }

}


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BAEI-2  92-10-05  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_put_prt_access */

/*
#if defined (O_TCN)
*/
#if (1)
/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_tb_put_pv_x
|
| (out)         p_dest                  Pointer to destination
| (in )         p_srce                  Pointer to source
| (in )         pv_format
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------

  This routine copies the bus variable between the traffic store and the
  application.

  The source pointer points to the application variable.
  The destination pointer points to the base of the invalid page of the port
  data buffer.

  The port pointer is adjusted by adding the byte offset derived from the
  bit offset (pv_format) to the pointer.

  If coherence problems for the target variable can arise, the caller must
  lock the task switching mechanism or prevent interrupts from coming through.

  The difference between this and the _vb_ function is the data swap. The
  VB bus controller transmits the bytes 1-0-3-2-5-4 and the TB bus controller
  transmits the bytes 0-1-2-3-4-...

*/

void            _lpl_tb_put_pv_x        (void *                 p_port,
                                         struct STR_LP_PV_X *   p_pv_x,
                                         unsigned short         var_count)

{
register void * p_dest;
         void * p_srce;
int             i;


    while (var_count-- != 0)
    {
        p_dest = (void *) (((char *) p_port) + p_pv_x->byte_offset);
        p_srce = p_pv_x->p_appl;

        switch (p_pv_x->type)
        {

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BOOL_1:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bool_1 (&P_DEST_T_FLD,
                                 p_pv_x->bit_offset,
                                 lp_get_bool_1 (P_SRCE_T_B1));

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BOOL_2:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bool_2  (&P_DEST_T_FLD,
                                  p_pv_x->bit_offset,
                                  lp_get_bool_2 (P_SRCE_T_B2));

        break;

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BCD_4:

/* ------------------------------------------------------------------------ */

             lp_put_fld_bcd_4   (&P_DEST_T_FLD,
                                  p_pv_x->bit_offset,
                                  lp_get_bcd_4 (P_SRCE_T_BCD));

        break;

/* ------------------------------------------------------------------------ */

        case LPL_BITSET_8:
        case LPL_CARD_8:
        case LPL_SIGN_8:

/* ------------------------------------------------------------------------ */


             *(char  *) ((char *) p_dest) = *(char *) p_srce;

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BITSET_16:

/* ------------------------------------------------------------------------ */

             *(short *) p_dest = *(short *) p_srce;

        break;

/* ------------------------------------------------------------------------ */

        case LPL_CARD_16:
        case LPL_SIGN_16:
        case LPL_FRAC_200_16:
        case LPL_FRAC_400_16:
        case LPL_FRAC_800_16:

/* ------------------------------------------------------------------------ */

             lpl_copy_16_sb (p_dest, p_srce);

        break;


/* new page
   ------------------------------------------------------------------------ */

        case LPL_TIMEDATE_48:           /* Swap the high and low 16 bits    */

/* ------------------------------------------------------------------------ */

        {
        void * p_tmp_dst = &(((TYPE_LPL_TIMEDATE_48 *) p_dest)->ticks);
        void * p_tmp_src = &(((TYPE_LPL_TIMEDATE_48 *) p_srce)->ticks);
             lpl_copy_16_sb (p_tmp_dst, p_tmp_src);
        }


/*      !!! NO BREAK - fall into LPL_TIMEDATE_48 continued to copy rest     */


/* ------------------------------------------------------------------------ */

/* !!!  case LPL_TIMEDATE_48 continued, must not be moved                   */
        case LPL_REAL_32:
        case LPL_CARD_32:
        case LPL_SIGN_32:

/* ------------------------------------------------------------------------ */

             lpl_copy_32_sb_sw (p_dest, p_srce);

        break;

/* ------------------------------------------------------------------------ */

        case LPL_BITSET_32:

/* ------------------------------------------------------------------------ */

             *(long *) p_dest = *(long *) p_srce;

        break;

/* new page
   ------------------------------------------------------------------------ */

        case LP_TYPE_A8_ODD:
        case LP_TYPE_A8_EVEN:

/* ------------------------------------------------------------------------ */

             pi_copy8 (p_dest, p_srce, p_pv_x->size);

        break;

/* ------------------------------------------------------------------------ */

        case LP_TYPE_A16_SIGN:
        case LP_TYPE_A16_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             lpl_put_a16_sb (p_dest, p_srce, i);

        break;


/* ------------------------------------------------------------------------ */

        case LP_TYPE_A32_SIGN:
        case LP_TYPE_A32_CARD:

/* ------------------------------------------------------------------------ */

             i = p_pv_x->size;
             lpl_put_a32_sb_sw (p_dest, p_srce, i);

        break;



/* new page
   ------------------------------------------------------------------------ */

        default:

/* ------------------------------------------------------------------------ */

            lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "unknown data type"); /*$H. unknown data type */

        break;

        }

        p_pv_x++;

   }

}


/*
 ----------------------------------------------------------------------------
| Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
| ----  ------  --------  -------  ----------  ---  -------  ---------
|   1   BATC    94-02-04  created                   Marsden      --
 ----------------------------------------------------------------------------
End of function lpl_tb_put_pv_x */



/* new page
 ----------------------------------------------------------------------------
|
| name          lpl_tb_get_pv_x
|
| (in )         p_port                  Pointer to active data port page
| (in )         p_pv_x                  Pointer to input list
|
|        out    -
|
| return value  result
|
 ------------   A B S T R A C T   -------------------------------------------


  This routine copies the bus variable between the traffic store and the
  application.

  The source pointer points to the base of a buffer containing a copy of the
  traffic store port merged with the force variables from the force table.
  The destination pointer points to the application variable.

  The port pointer is adjusted by adding the byte offset derived from the
  bit offset (pv_format) to the pointer.

*/

void            _lpl_tb_get_pv_x        (void *                 p_port,
                                         struct STR_LP_PV_X *   p_pv_x,
                                         unsigned short         var_count)


{

register void * p_dest;
         void * p_srce;
int             i;

    while (var_count-- > 0)
    {
        p_dest = p_pv_x->p_appl;
        p_srce = (void *) (((char *) p_port) + p_pv_x->byte_offset);

        switch (p_pv_x->type)
        {

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BOOL_1:

/* ------------------------------------------------------------------------ */

                 lp_put_bool_1     (&P_DEST_T_B1,
                                    lp_get_fld_bool_1 (P_SRCE_T_FLD,
                                    p_pv_x->bit_offset));

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BOOL_2:

/* ------------------------------------------------------------------------ */

                 lp_put_bool_2     (&P_DEST_T_B2,
                                    lp_get_fld_bool_2 (P_SRCE_T_FLD,
                                    p_pv_x->bit_offset));

        break;

/* new page
   ------------------------------------------------------------------------ */

        case LPL_BCD_4:

/* ------------------------------------------------------------------------ */

                 lp_put_bcd_4     (&P_DEST_T_BCD,
                                   lp_get_fld_bcd_4  (P_SRCE_T_FLD,
                                   p_pv_x->bit_offset));
        break;

/* ------------------------------------------------------------------------ */

        case LPL_BITSET_8:
        case LPL_CARD_8:
        case LPL_SIGN_8:

/* ------------------------------------------------------------------------ */


             *(char  *) ((char *) p_dest) = *(char *) p_srce;

        break;


/* ------------------------------------------------------------------------ */

        case LPL_BITSET_16:

/* ------------------------------------------------------------------------ */

             *(short *) p_dest = *(short *) p_srce;

        break;

/* ------------------------------------------------------------------------ */

        case LPL_CARD_16:
        case LPL_SIGN_16:
        case LPL_FRAC_200_16:
        case LPL_FRAC_400_16:
        case LPL_FRAC_800_16:

/* ------------------------------------------------------------------------ */

             lpl_copy_16_sb (p_dest, p_srce);

        break;


/* new page
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        case LPL_TIMEDATE_48:           /* Swap the high and low 16 bits    */

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/


             lpl_copy_16_sb (&(((TYPE_LPL_TIMEDATE_48 *) p_dest)->ticks),
                             &(((TYPE_LPL_TIMEDATE_48 *) p_srce)->ticks));


/*      !!! NO BREAK - fall into LPL_TIMEDATE_48 continued to copy rest     */


/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

/* !!!  case LPL_TIMEDATE_48 continued, must not be moved                   */
        case LPL_REAL_32:
        case LPL_CARD_32:
        case LPL_SIGN_32:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

             lpl_copy_32_sb_sw (p_dest, p_srce);

        break;

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        case LPL_BITSET_32:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

             *(long *) p_dest = *(long *) p_srce;

        break;

/* new page
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        case LP_TYPE_A8_ODD:
        case LP_TYPE_A8_EVEN:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

             pi_copy8 (p_dest, p_srce, p_pv_x->size);

        break;

/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        case LP_TYPE_A16_SIGN:
        case LP_TYPE_A16_CARD:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

             i = p_pv_x->size;
             lpl_put_a16_sb (p_dest, p_srce, i);

        break;


/*旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        case LP_TYPE_A32_SIGN:
        case LP_TYPE_A32_CARD:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

             i = p_pv_x->size;
             lpl_put_a32_sb_sw (p_dest, p_srce, i);

        break;


/* new page
  旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커*/

        default:

/*읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸*/

            lpi_hamster_uncond (LPI_DU_ERR, LPI_DU_RESET, "unknown data type"); /*$H. unknown data type */

        break;

        }

        p_pv_x++;

   }

}


/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
� Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
� ----  ------  --------  -------  ----------  ---  -------  ---------
�   1   BATC    94-02-04  created                   Marsden      --
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
End of function lpl_tb_get_pv_x */

#endif


/* new page
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
�
� name          lp_pv_size
�
� (in )         pv_name
�
� return value  size                    size of variable in bytes
�
읕컴컴컴컴컴�   A B S T R A C T   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸


  This routine decodes the type and size information to make the variable size
  in bytes.

*/


TYPE_PD_RESULT  _lp_pv_size     (const  struct STR_PV_NAME *    p_pv_name)

{

short   size = -1;

        if (lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A8_EVEN)
            size = (lp_get_pvn_pvf(p_pv_name, size) + 1) * 2;
        else if (lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A8_ODD)
            size = (lp_get_pvn_pvf(p_pv_name, size) * 2) + 1;
/* <ATR:01> */
// size = ((TYPE_LPL_PV_FORMAT_EXT_SIZE *) p_pv_name)->ext_size + 1;
        else if ((lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A16_SIGN) ||
                 (lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A16_CARD) ||
                 (lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A32_SIGN) ||
                 (lp_get_pvn_pvf(p_pv_name, type) == LP_TYPE_A32_CARD))
        {
                size = (lp_get_pvn_pvf(p_pv_name, size) + 1) << 1;
        }

        else
        {
                switch  (lp_get_pvn_pvf(p_pv_name, size))
                {

                        case    LP_SIZE_BITS:

                                size = 1;
                                break;

                        case    LP_SIZE_16:

                                size = 2;
                                break;

                        case    LP_SIZE_32:

                                size = 4;
                                break;

                        case    LP_SIZE_48:

                                size = 6;
                                break;

                        case    LP_SIZE_64:

                                size = 8;
                                break;

                        default:

                                size = -1;
                                break;
                }
        }
        return (size);

}



/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
� Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
� ----  ------  --------  -------  ----------  ---  -------  ---------
�   1   BAEI-2  92-06-28  created                   Marsden      --
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
End of function lp_pv_size */

/*
 ---------------------------------------------------------------------------�
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LPL_LACS.C               Nr:     2       dated:  94-02-04

    lpl_tb_put_pv_x
    lpl_tb_get_pv_x

    new functions for Train bus because different swap from bus controller

 ----------------------------------------------------------------------------

    file name: LPL_LACS.C               Nr:     2       dated:  9?-??-??

    name of object modified

    description of what was done to object (not functionality!)

*/
