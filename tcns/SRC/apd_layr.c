/*   APD_RTP15.C
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
#if defined (O_960)
#define  LPI_MODULE_NAME "apd_layr"
#endif


#if defined (O_PCMCIA)
#define O_LE
#endif

#include "apd_incl.h"
#include "lp_sys.h"

#include "pi_sys.h"
#include "lpc_blck.h"
#include "lpi_ifce.h"
#include "lpl_4tcn.h"

#define  TYPE_PD_RESULT TYPE_APD_RESULT


/*
 ----------------------------------------------------------------------------
|
|   apd_init
|
  ------------   A B S T R A C T   -------------------------------------------

    Initialises the static data.
    Calls the init function of all known link layers.

*/

struct APD_STR_DTA   apd_dta;

TYPE_PD_RESULT      _apd_init         (void)
{
    lpi_memset (&apd_dta, 0, sizeof (apd_dta));

    return (APD_OK);
}


/*
 ----------------------------------------------------------------------------
|
|   apd_connect
|
  ------------   A B S T R A C T   -------------------------------------------

    connects the traffic store id to a link layer via the traffic store type

*/

TYPE_PD_RESULT     apd_connect_mll (unsigned int     ts_id,
                                     int     ll_type)
{
TYPE_PD_RESULT     result = APD_OK;

    if (ts_id <= APD_TS_ID_MAX)
    {
        if ((ll_type == APD_LL_TCN) || (ll_type == APD_LL_FIP))
        {
            apd_dta.ll_type[ts_id] = ll_type;
        }
        else
        {
            result = APD_RANGE;
        }
    }
    else
    {
        result = APD_UNKNOWN_TS;
    }
    return (result);
}


/*
 ----------------------------------------------------------------------------

    name          apd_put_variable

    return value  APD_OK or error codes as described by user's manual

 ----------------------------------------------------------------------------

    One variable (incl. error and validity) is copied from the application
    instance to the traffic store at the location defined by pv name.

    The internal settings are
        "with_force      = ACTIVE" and
        "multiple_access = ACTIVE"

*/

TYPE_PD_RESULT _apd_put_variable (const struct STR_APD_PV_DESCRIPTOR * p_pvd)
{
TYPE_PD_RESULT result;
                                        #if defined (O_MLL)
char        ll_type = apd_dta.ll_type[p_pvd->pv_name.ts_id];

    if      (ll_type == APD_LL_FIP) { result = APD_RANGE; }
    else if (ll_type != APD_LL_TCN) { result = APD_RANGE; }
    else
                                        #endif
    {
    char    pv_set[lpd_sizeof_pv_set(2)];

        if ((result = lpd_gen_pv_set ((struct STR_APD_PV_SET *) &pv_set[0],
                                       p_pvd, NULL, 1)) == APD_OK)
        {
            ((struct STR_APD_PV_SET *) pv_set)->multiple_access = APD_ACTIVE;

            result = apd_put_set_fast ((struct STR_APD_PV_SET *)&pv_set[0]);
        }
    }
    return (result);
}


/*
 ----------------------------------------------------------------------------

    name          apd_get_variable

    return value  APD_OK or error codes as described by user's manual

 ----------------------------------------------------------------------------

    One variable (incl. error and validity) is copied from the traffic store
    location defined by pv name to the application instance.

    The internal settings are
        "with_force = ACTIVE"

*/

TYPE_PD_RESULT _apd_get_variable(const struct STR_APD_PV_DESCRIPTOR * p_pvd,
                                  unsigned short * p_refresh                 )
{
TYPE_PD_RESULT result;
                                        #if defined (O_MLL)
char        ll_type = apd_dta.ll_type[p_pvd->pv_name.ts_id];

    if      (ll_type == APD_LL_FIP) { result = APD_RANGE; }
    else if (ll_type != APD_LL_TCN) { result = APD_RANGE; }
    else
                                        #endif
    {
    char    pv_set[lpd_sizeof_pv_set(2)];

        if ((result = lpd_gen_pv_set ((struct STR_APD_PV_SET *) &pv_set[0],
                                       p_pvd, p_refresh, 1)) == LPS_OK)
        {
            result = apd_get_set_fast ((struct STR_APD_PV_SET *)&pv_set[0]);
        }
    }
    return (result);
}


/*
 ----------------------------------------------------------------------------

    name          apd_put_set

    return value  APD_OK or error codes as described by user's manual

 ----------------------------------------------------------------------------

    A cluster of variables (incl. error and validity) are copied from the
    application instances to the traffic store PORT defined by the
    "pv_set" list. The internal setting of "multiple_access" configures
    whether the WHOLE PORT or only PART OF THE PORT is updated. Setting
    "multiple_access=APD_ACTIVE" is slower, but allows a partial port update.

*/


TYPE_PD_RESULT _apd_put_set (const struct STR_APD_PV_DESCRIPTOR * p_pv_descr,
                             int                                  c_pv_descr)
{
    TYPE_PD_RESULT result  = APD_OK;

#if defined (O_MLL)
    char ll_type = apd_dta.ll_type[p_pv_descr->pv_name.ts_id];

    if (ll_type == APD_LL_FIP)
    {
        result = APD_RANGE;
    }
    else if (ll_type != APD_LL_TCN)
    {
        result = APD_RANGE;
    }
    else
#endif
    {
        struct  STR_APD_PV_SET * p_pv_set;

        if ((p_pv_set = (struct STR_APD_PV_SET *) pi_alloc (lpd_sizeof_pv_set(c_pv_descr))) != NULL)
        {
            if ((result = lpd_gen_pv_set (p_pv_set, p_pv_descr,
                                          NULL, c_pv_descr)) == LPS_OK)
            {
                apd_put_set_fast (p_pv_set);
                pi_free          ((void*)p_pv_set);
            }
        }
        else
        {
            result = APD_MEMORY;
        }
    }
    return (result);
}


/*
 ----------------------------------------------------------------------------

    name          apd_get_set

    return value  APD_OK or error codes as described by user's manual

 ----------------------------------------------------------------------------

    A cluster of variables (incl. error and validity) are copied from the
    traffic store PORT defined by the "pv_set" list to the application
    instances. The user may read a discretionary amount of variables from
    the target port.

*/

TYPE_PD_RESULT _apd_get_set (const struct STR_APD_PV_DESCRIPTOR * p_pv_descr,
                             unsigned short *                     p_refresh ,
                             int                                  c_pv_descr)
{
    TYPE_PD_RESULT result  = APD_OK;

#if defined (O_MLL)
    char ll_type = apd_dta.ll_type[p_pv_descr->pv_name.ts_id];

    if (ll_type == APD_LL_FIP)
    {
        result = APD_RANGE;
    }
    else if (ll_type != APD_LL_TCN)
    {
        result = APD_RANGE;
    }
    else
#endif
    {
        struct  STR_APD_PV_SET * p_pv_set;
        if ((p_pv_set = (struct STR_APD_PV_SET *) pi_alloc (lpd_sizeof_pv_set(c_pv_descr))) != NULL)
        {
            if ((result = lpd_gen_pv_set (p_pv_set, p_pv_descr,
                                          p_refresh, c_pv_descr)) == LPS_OK)
            {
                apd_get_set_fast (p_pv_set);
                pi_free          ((void*)p_pv_set);
            }
        }
        else
        {
            result = APD_MEMORY;
        }
    }
    return (result);
}


/*
 ----------------------------------------------------------------------------

    name          apd_put_set_fast

    return value  APD_OK or error codes as described by user's manual

 ----------------------------------------------------------------------------

    A cluster of variables (incl. error and validity) are copied from the
    application instances to the traffic store PORT defined by the
    "pv_set" list. The internal setting of "multiple_access" configures
    whether the WHOLE PORT or only PART OF THE PORT is updated. Setting
    "multiple_access=APD_ACTIVE" is slower, but allows a partial port update.

*/

TYPE_PD_RESULT _apd_put_set_fast (const struct STR_APD_PV_SET * p_pv_set)
{
        return (lp_put_data_set ((struct STR_LP_DS_SET *) p_pv_set));
}


TYPE_PD_RESULT _apd_get_set_fast (const struct STR_APD_PV_SET * p_pv_set)
{

        return (lp_get_data_set ((struct STR_LP_DS_SET *) p_pv_set));
}


TYPE_PD_RESULT _apd_put_cluster (const struct STR_APD_PV_CLUSTER * p_clus)
{
int i;
TYPE_PD_RESULT result = APD_OK;
int             c_ds_clus = p_clus->c_pv_set;

    for (i = 0; i < c_ds_clus; i++)
    {
        if (lp_put_data_set ((struct STR_LP_DS_SET *) (p_clus->p_pv_set[i]))
            != APD_OK ) { result = APD_ERROR; }
    }
    return (result);
}


TYPE_PD_RESULT _apd_get_cluster (const struct STR_APD_PV_CLUSTER * p_clus)
{
int i;
TYPE_PD_RESULT result = APD_OK;
int             c_ds_clus = p_clus->c_pv_set;

    for (i = 0; i < c_ds_clus; i++)
    {
        if (lp_get_data_set ((struct STR_LP_DS_SET *) (p_clus->p_pv_set[i]))
            != APD_OK ) { result = APD_ERROR; }
    }
    return (result);
}



TYPE_PD_RESULT _apd_force_variable(const struct STR_APD_PV_DESCRIPTOR * p_pvd)
{
TYPE_PD_RESULT result;
char validity = APD_CHK_IS_FRCD;
struct STR_APD_PV_DESCRIPTOR my_pvd = *p_pvd;
                                        #if defined (O_MLL)
char        ll_type = apd_dta.ll_type[p_pvd->pv_name.ts_id];

    if      (ll_type == APD_LL_FIP) { result = APD_RANGE; }
    else if (ll_type != APD_LL_TCN) { result = APD_RANGE; }
    else
                                        #endif
    {
            my_pvd.p_validity = &validity;
            result  = lpd_force_variable (&my_pvd);
    }
    return (result);
}


TYPE_PD_RESULT _apd_unforce_variable (const struct STR_APD_PV_NAME * p_pvn)
{
TYPE_PD_RESULT result;
                                        #if defined (O_MLL)
char        ll_type = apd_dta.ll_type[p_pvn->ts_id];

    if      (ll_type == APD_LL_FIP) { result = APD_RANGE; }
    else if (ll_type != APD_LL_TCN) { result = APD_RANGE; }
    else
                                        #endif
    {
        result = lpd_unforce_variable (p_pvn);
    }

    return (result);
}


TYPE_PD_RESULT _apd_unforce_all (unsigned int ts_id)
{
TYPE_PD_RESULT result;
                                        #if defined (O_MLL)
char        ll_type = apd_dta.ll_type[ts_id];

    if      (ll_type == APD_LL_FIP) { result = APD_RANGE; }
    else if (ll_type != APD_LL_TCN) { result = APD_RANGE; }
    else
                                        #endif
    {
        result = _lp_unfrc_all ((unsigned short) ts_id);
    }
    return (result);
}

                                       #if !defined (O_BAP)

TYPE_PD_RESULT _apd_subscribe (struct STR_APD_DS_NAME *  p_ds_name,
                                void (*proc) (unsigned short)      ,
                                unsigned short            param    )
{
TYPE_PD_RESULT result = APD_RANGE;
unsigned short  tmp_ds_name = *(unsigned short *) p_ds_name;
int ix;

    for (ix = 0; ix < APD_C_SUBSCRIBERS; ix++)
    {
        if (apd_dta.subscribed[ix].in_use == APD_PASSIVE)
        {
            apd_dta.subscribed[ix].in_use  = APD_ACTIVE;
            apd_dta.subscribed[ix].ds_name = tmp_ds_name;
            apd_dta.subscribed[ix].param   = param      ;
            apd_dta.subscribed[ix].p_func  = proc;
            result = lpd_subscribe (p_ds_name, proc, param, ix);
            break;
        }
    }

    return (result);
}


TYPE_PD_RESULT _apd_desubscribe (struct STR_APD_DS_NAME *  p_ds_name)
{
TYPE_PD_RESULT result = APD_ERROR;
unsigned short  tmp_ds_name = *(unsigned short *) p_ds_name;
int ix;

    for (ix = 0; ix < APD_C_SUBSCRIBERS; ix++)
    {
        if (apd_dta.subscribed[ix].ds_name == tmp_ds_name)
        {
            apd_dta.subscribed[ix].in_use = APD_PASSIVE;
            result = lpd_desubscribe (p_ds_name, ix);
            break;
        }
    }
    return (result);
}


TYPE_PD_RESULT _apd_pass_subscription (struct STR_APD_DS_NAME * p_ds_name_old,
                                        struct STR_APD_DS_NAME * p_ds_name_new)
{
TYPE_PD_RESULT  result = APD_ERROR;
unsigned short  tmp_ds_name_old = *(unsigned short *) p_ds_name_old;
unsigned short  tmp_ds_name_new = *(unsigned short *) p_ds_name_new;
int ix;

    for (ix = 0; ix < APD_C_SUBSCRIBERS; ix++)
    {
        if ((apd_dta.subscribed[ix].ds_name == tmp_ds_name_old) &&
            (apd_dta.subscribed[ix].in_use  == APD_ACTIVE))
        {
            lpd_desubscribe (p_ds_name_old, ix);
            result = lpd_subscribe (p_ds_name_new,
                                    apd_dta.subscribed[ix].p_func,
                                    apd_dta.subscribed[ix].param, ix);
            apd_dta.subscribed[ix].ds_name = tmp_ds_name_new;
            break;
        }
    }
    return (result);
}

                                       #else
                                       #endif
/*
 ----------------------------------------------------------------------------

    name          apd_pack

    return value  -

 ----------------------------------------------------------------------------

   The Pack list defined by the structure STR_LP_PAC_0 and STR_LP_PAC_1
   defines the booleans which are to be packed into a bitset 16

*/

void     _apd_pack (const struct STR_APD_PAC_0 *    p_pac_0)

{
int      c_pac_1;
int      c_pac_2;

struct   STR_APD_PAC_1 * p_pac_1;
struct   STR_APD_PAC_2 * p_pac_2;
unsigned short bitset;

    p_pac_1 = p_pac_0->p_array_of_pac_1;
    c_pac_1 = p_pac_0->c_pac_1;

    while (c_pac_1-- > 0)
    {
        p_pac_2 = p_pac_1->p_array_of_pac_2;
        c_pac_2 = p_pac_1->c_bools_in_bitset;
        bitset  = 0;

        while (c_pac_2-- > 0)
        {
            if ((*p_pac_2->p_bool & 1) == 1)
            {
                bitset |=  p_pac_2->b_mask;
            }
            p_pac_2++;
        }
        *(p_pac_1->p_bitset) = bitset;
        p_pac_1++;
    }
}

/*
 ----------------------------------------------------------------------------

    name          apd_unpack

    return value  -

 ----------------------------------------------------------------------------

   The Pack list defined by the structure STR_LP_PAC_0 and STR_LP_PAC_1
   defines the booleans which are to be unpacked from bitset 16s into
   individual booleans (char)

*/

void     _apd_unpack (const struct STR_APD_PAC_0 *    p_pac_0)

{
int      c_pac_1;
int      c_pac_2;
unsigned short bitset;

struct   STR_APD_PAC_1 * p_pac_1;
struct   STR_APD_PAC_2 * p_pac_2;

    p_pac_1 = p_pac_0->p_array_of_pac_1;
    c_pac_1 = p_pac_0->c_pac_1;

    while (c_pac_1-- > 0)
    {
        p_pac_2 =  p_pac_1->p_array_of_pac_2;
        bitset  = *p_pac_1->p_bitset;
        c_pac_2 =  p_pac_1->c_bools_in_bitset;

        while (c_pac_2-- > 0)
        {
           *p_pac_2++->p_bool = ((bitset & p_pac_2->b_mask) == 0) ? 0 : 1;
        }
        p_pac_1++;
    }
}



/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S
|   - - - - - - - - - - - - -
|
 ----------------------------------------------------------------------------


*/

