/*   LPX_LIST.C
 ----------------------------------------------------------------------------

     Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG

 ----------------------------------------------------------------------------

     Project      :   MicOS
     Subproject   :   Process Data

 ----------------------------------------------------------------------------

     File         :   LPX_LIST.C   -   Input structure creation functions

     Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3
                      [2]: Link Layer Process Specification
                      [3]: Link Layer Process Design

     Abstract     :   The low level interface defines structures which
                      are built using these functions
     Remarks      :

     Dependencies :

     Accepted     :

     History      :   see end of file

 ----------------------------------------------------------------------------
*/

#define   LPX_LIST_C


#if defined (O_PCMCIA)
#define O_LE
#endif

#include  "lp_sys.h"            /*   LP user interface                     */
#include  "pi_sys.h"            /*   PIL                                   */

#include  "lpc_blck.h"          /*   control block                         */
#include  "lpi_ifce.h"          /*   LP import interface                   */
#include  "lpl_4tcn.h"          /*   TS access routines                    */


/* new page
 ----------------------------------------------------------------------------
|
| name          STR_LPX_RECYCLE
|
|
 ------------   A B S T R A C T   -------------------------------------------


  This structure is used to build a chain of allocated pointers, which must
  all be returned to the pool (p_alcd means p_allocated) upon request.

  The static pointer p_root is the root pointer to the chain of allocated
  pointers. Only one chain is supported (here, for lpx lists)

*/

struct          STR_LPX_RECYCLE
{
        struct  STR_LPX_RECYCLE *       p_next;
        void *                          p_alcd;
};


static  void *  p_root;


/*  new page
 ----------------------------------------------------------------------------
|
|   name        lpx_gen_ds_x
|
|       (out)   p_ds_x                  p to str_lp_ds_x structure
|   (in )       p_ds_name               p to a ds_name variable
|
|   ret value   -
|
 ------------   A B S T R A C T   -------------------------------------------


  All memory is supplied by the caller.

  The contents of the ds_name are translated into str_lp_ds_x structure info
  using data from the control block.

  See user's manual for definition of str_lp_ds_x.

  Requirements: The traffic store was successfully configured.

*/

void            lpx_gen_ds_x    (struct STR_LP_DS_X *           p_x,
                                 const struct STR_DS_NAME *     p_ds_name)
{
void *          p_cb;
LC_TYPE_TS_PIT *pb_pit;
unsigned short  prt_indx;

    if (lpc_open (p_ds_name->ts_id, &p_cb) == LPS_OK)
    {
        pb_pit   = lpc_get_pb_pit (p_cb);
        lpi_get_prt_indx_via_pvn (p_cb, &prt_indx, pb_pit, p_ds_name->prt_addr);

        p_x->p_pcs =   ((unsigned char *) lpc_get_pb_pcs (p_cb)) +
                                            lpl_gen_pcs_offset (p_cb, prt_indx);
        p_x->p_prt =   ((unsigned char *) lpc_get_pb_prt (p_cb)) +
                                            lpl_gen_prt_offset (p_cb, prt_indx);
        p_x->p_frc =   ((unsigned char *) lpc_get_pb_frc (p_cb)) +
                                            lpl_gen_prt_offset (p_cb, prt_indx);

        lpl_get_prt_size (p_cb, &p_x->prt_sze,  p_x->p_pcs);
    }
}

/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_gen_pv_x
|
|       (out)   p_pv_x                  p to str_lp_pv_x structure
| (in )         p_pv_set                p to str_lp_pv_set
| (in )         count                   str_lp_pv_set count
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  All memory is supplied by the caller.

  The pv_format and p_variable contents of the pv_set are translated into
  str_lp_pv_x structure info.

  The type is extended by two bits of the size info to form the real type,
  except when the type is an array type.

  The size is converted to "element count".
  The byte offset is corrected for types which require a swapped byte offset.

  See user's manual for definition of str_lp_pv_x.

*/

void            lpx_gen_pv_x    (       struct STR_LP_PV_X *    p_x,
                                 const  struct STR_LP_PV_SET *  p_pv_set,
                                        unsigned short          count)
{
void * p_cb;
    lpc_open (lp_get_pvn_dsn(&p_pv_set->pv_name, ts_id), &p_cb);

    while (count-- > 0)
    {
        p_x->p_appl      =  p_pv_set->p_var;

        p_x->size        =  lp_get_pvn_pvf (&p_pv_set->pv_name, size);
        p_x->type        =  lp_get_pvn_pvf (&p_pv_set->pv_name, type) +
                            ((p_x->size << 4) & 0x30);
        p_x->bit_offset  =  lp_get_pvn_pvf (&p_pv_set->pv_name, bit_offset)
                         &  LPL_MASK_BYTE_OFFSET_IN_BIT_OFFSET;
        p_x->byte_offset =  lp_get_pvn_pvf (&p_pv_set->pv_name, bit_offset)
                         >> LPL_SHIFT_BIT_OFFSET_TO_BYTE_OFFSET;

        {
        unsigned int prt_offset;

            if (lpc_get_ts_type(p_cb) == LP_TS_TYPE_D)
            {
                prt_offset = (lp_get_pvn_dsn(&p_pv_set->pv_name, prt_addr) & 3) << 5;
                p_x->byte_offset += prt_offset;
            }
        }

        switch (p_x->type & 0xF)
        {
                case    LP_TYPE_BOOL_1:
                case    LP_TYPE_BOOL_2:
                case    LP_TYPE_BCD_4:
                case    LP_TYPE_BITSET:
                case    LP_TYPE_CARD:
                case    LP_TYPE_SIGN:

                        #if defined (O_LE)
                        if (p_x->size == LP_SIZE_8)
                        {
                            if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
                            {
                                /* do nothing */
                            }
                            else
                            {
                                p_x->byte_offset = (p_x->byte_offset & 1) == 0 ?
                                                    p_x->byte_offset + 1 :
                                                    p_x->byte_offset - 1 ;
                            }
                        }
                        #endif

                break;

                case    LP_TYPE_A8_ODD:

                        #if defined (O_LE)
                        if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
                        {
                                /* do nothing */
                        }
                        else
                        {
                                p_x->byte_offset = (p_x->byte_offset & 1) == 0 ?
                                                    p_x->byte_offset + 1 :
                                                    p_x->byte_offset - 1 ;
                        }
                        #endif
                        p_x->type =  p_x->type & 0xf;
                        p_x->size = (p_x->size << 1) + 1;

                break;

                case    LP_TYPE_A8_EVEN:

                        p_x->type =  p_x->type & 0xf;
                        p_x->size = (p_x->size << 1) + 2;

                break;

                case    LP_TYPE_A16_CARD:
                case    LP_TYPE_A16_SIGN:

                        p_x->type =  p_x->type & 0xf;
                        p_x->size += 1;

                break;

                case    LP_TYPE_A32_CARD:
                case    LP_TYPE_A32_SIGN:

                        p_x->type =  p_x->type & 0xf;
                        p_x->size = (p_x->size >> 1) + 1;

                break;

                default:
                break;
        }
        p_pv_set++;
        p_x++;
    }
}

/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_gen_ds_set
|
| (in ) (out)   p_ds_set                p to str_lp_ds_set structure
|       (out)   p_pv_x                  p to str_lp_pv_x structure
| (in )         p_pv_set                p to str_lp_pv_set ARRAY
| (in )         p_refresh               p to the refresh variable
| (in )         pv_set_cnt              count pv_set
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  All memory is supplied by the caller.

  The pv_set parameters are translated into the ds_set structure information
        ds_x, ds_y and pv_x
  by calling the function lpx_gen_ds_x, lpx_gen_pv_x and by filling in the
  ds_y info itself.

  The default values for the put and get functionality is
                - with force
                - put update not as total port update

*/

void            _lpx_gen_ds_set (struct STR_LP_DS_SET *         p_ds_set,
                                 struct STR_LP_PV_X   *         p_pv_x,
                                 const struct STR_LP_PV_SET *   p_pv_set,
                                 unsigned short *               p_refresh,
                                 unsigned short                 pv_set_cnt)
{
struct  STR_DS_NAME ds_name;

        ds_name.ts_id    = lp_get_pvn_dsn (&p_pv_set->pv_name, ts_id);
        ds_name.prt_addr = lp_get_pvn_dsn (&p_pv_set->pv_name, prt_addr);

        lpx_gen_ds_x (&p_ds_set->ds_x, &ds_name);
        lpx_gen_pv_x (p_pv_x, p_pv_set, pv_set_cnt);

        p_ds_set->ds_x.p_refresh = p_refresh;
        p_ds_set->ds_x.pv_x_cnt  = pv_set_cnt;
        p_ds_set->p_pv_x         = p_pv_x;

        p_ds_set->ds_y.ts_id = ds_name.ts_id;

        {
        void * p_cb;
            if (lpc_open (ds_name.ts_id, &p_cb) == LPS_OK)
            {
                if (lpc_get_hw_type(p_cb) == LP_HW_TYPE_HDLC)
                {
                    p_ds_set->ds_y.with_frce = LP_VAR_WITH_FRCE;
                    p_ds_set->ds_y.with_else = LP_VAR_WITHOUT;
                }
                else
                {
                    p_ds_set->ds_y.with_frce = LP_VAR_WITH_FRCE;
                    p_ds_set->ds_y.with_else = LP_VAR_WITH_ELSE;
                }
            }
        }
}




/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_gen_clus_ts
|
| (in )         ts_id                   traffic store identifier
| (in )         ds_count                count of lists STR_LP_CLUS_DS
|
| return value  ptr                     allocated pointer
|
 ------------   A B S T R A C T   -------------------------------------------


  Memory is taken from the pool and remembered in the recycle chain.

  Memory for a structure STR_LP_CLUS_TS is allocated from the memory pool and
  the parameters ts_id and ds_count are written to it. The pointer to the
  next STR_LP_CLUS_DS must be filled in by the caller.

  !!!           Better Code!
                if (lpx_remember (&p_root, p_clts) != LPS_OK)
                {
                        pi_free (p_clts);
                        p_clts = NULL;
                }

*/

void * _lpx_gen_clus_ts (unsigned short         ts_id,
                         unsigned short         ds_count)
{

    struct  STR_LP_CLUS_TS * p_clts;

    if ((p_clts = pi_alloc (sizeof (struct STR_LP_CLUS_TS))) != NULL)
    {
        p_clts->ts_id     = ts_id;
        p_clts->ds_count = ds_count;

        lpx_remember (&p_root, p_clts);
    }
    return (p_clts);
}


/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_gen_clus_ds
|
| (in )         p_ds_name               structure ds_name
| (in )         p_pv_set                pointer to pv_set array
| (in )         p_refresh               pointer where to write time info
| (in )         pv_set_count            array count of pv_sets
|
| return value  ptr                     allocated pointer
|
 ------------   A B S T R A C T   -------------------------------------------


  Memory is taken from the pool and remembered in the recycle chain.

  Memory for a structure STR_LP_CLUS_DS and pv_set_count structures of
  STR_LP_PV_X is allocated from the memory pool.

  The parameters are all filled in.

  The pointer to the next STR_LP_CLUS_DS is set to NULL. The caller must
  modify this pointer when ajoining to the cluster list.

*/

void *  _lpx_gen_clus_ds   (const  struct STR_DS_NAME *     p_ds_name,
                            const  struct STR_LP_PV_SET *   p_pv_set,
                            unsigned short *                p_refresh,
                            unsigned short                  pv_set_cnt)
{
    struct STR_LP_PV_X    * p_vl;
    struct STR_LP_CLUS_DS * p_pl = NULL;


    if ((p_vl = pi_alloc ((unsigned short)(pv_set_cnt * sizeof (struct STR_LP_PV_X))))  == NULL)
    {
    }
    else if ((p_pl = pi_alloc (sizeof (struct STR_LP_CLUS_DS))) == NULL)
    {
        pi_free ((void*)p_vl);
        p_vl = NULL;
    }
    else
    {
        lpx_gen_ds_x (&p_pl->ds_x, p_ds_name);
        p_pl->ds_x.p_refresh = p_refresh;
        p_pl->ds_x.pv_x_cnt = pv_set_cnt;

        p_pl->p_dta_lst = p_vl;
        p_pl->p_nxt_lst = NULL;

        lpx_gen_pv_x    (p_vl,
                         p_pv_set,
                         pv_set_cnt);

        lpx_remember    (&p_root, p_pl);
        lpx_remember    (&p_root, p_vl);

    }
    return (p_pl);
}

/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_recycle
|
| (in )         p_recycle               pointer to root structure
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  The route structure contains the pointers to an allocated memory block
  and to the next "recyclable" structure.
  The chain is terminated by a structure containing to "NULL" pointers.

  Modification 3.04.95: Error in While loop could lead to crash,
*/

void lpx_recycle (void ** p_root)
{
    struct  STR_LPX_RECYCLE * p_rc;
    struct  STR_LPX_RECYCLE * p_rc_sub;

    p_rc =  ((struct STR_LPX_RECYCLE *)* p_root);

    while (p_rc != NULL)
    {
        p_rc_sub = p_rc;
        p_rc     = p_rc->p_next;

        pi_free (p_rc_sub->p_alcd);
        pi_free ((void*)p_rc_sub);
    }

    *p_root = NULL;
}

/* new page
 ----------------------------------------------------------------------------
|
| name          lpx_remember
|
| (in )         p_root          pointer to root structure
|
| return value  -
|
 ------------   A B S T R A C T   -------------------------------------------


  The root points to an allocated memory block containing the allocated
  pointer and a pointer to the next remembered structure in the chain.

  The chain is terminated by a remembered pointer and a NULL to the next
  chain element.

*/

TYPE_PD_RESULT  lpx_remember            (void **        p_root,
                                         void *         the_pointer)

{
short   result = LPS_OK;

struct  STR_LPX_RECYCLE * p_rc;

        if ((p_rc = pi_alloc (sizeof (struct STR_LPX_RECYCLE))) != NULL)
        {
                p_rc->p_next    = (struct STR_LPX_RECYCLE *) *p_root;
                p_rc->p_alcd    = the_pointer;
                *p_root         = p_rc;
        }
        else
        {
                result = LPS_ERROR;
        }

        return (result);
}

/*
 ----------------------------------------------------------------------------
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------

    file name: LPX_LIST.C               Nr:     1       dated:  92-10-05

    name of object modified             Nr:     ?       dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

    file name: LPX_LIST.C               Nr:     2       dated:  95-04-03

    lpx_recycle                         Nr:     ?       dated:  9?-??-??

    do while modified to while do, condition new: p_rc != NULL


*/
