/*
 ----------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 Zrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|
 ----------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ----------------------------------------------------------------------------
|
|    File Name    :   BAA_AUXI
|
|    Document     :
|
|    Abstract     :   Auxiliary Functions
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ----------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  Dept.   YY-MM-DD  State    Name    Reference
|    ----  ------  --------  -------  ------- ---------
|      1   BAEI-2  93-02-09  created  Marsden
|
 ----------------------------------------------------------------------------
*/

#define   BAA_AUXI_C

#include "baa_auxi.h"
#include "bac_cons.h"
#include "bai_ifce.h"
#include "bam_main.h"


/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_p_dta           static data access function
|
 ------------   A B S T R A C T   -------------------------------------------


    static data declaration and debug access function

*/

static  struct  BAA_STR_DTA baa_dta;


void            baa_open            (struct BAA_STR_DTA **  p_baa_dta)
{
        *p_baa_dta = &baa_dta;
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        baa_init
|
 ------------   A B S T R A C T   -------------------------------------------


    The internal variables are initialised.
    The electrical bus length is assumed to be maximum.

*/

void            baa_init            (void)

{

	bai_memset (&baa_dta, 0, sizeof (baa_dta));

	baa_ebl_put (BAA_PDC_6);
}


/*  new page
 ----------------------------------------------------------------------------
|
|   name        baa_create
|
 ------------   A B S T R A C T   -------------------------------------------

    The electrical bus length is set to the value configured by the
    application engineer.

*/

short           baa_create          (struct BAM_STR_CFG_PT *  p_pt)
{
short   result;

		result = baa_ebl_put (p_pt->p_div->ebl);

		baa_dta.ebl.ix_table =  baa_dta.ebl.class - 1;

        return (result);
}

/*  new page
 ----------------------------------------------------------------------------
|
|   name        baa_p_2_abs
|
 ------------   A B S T R A C T   -------------------------------------------

    The intel segment:offset representation of a pointer is converted to
    the absolute address

    The absolute address is converted to the intel segment:offset
    representation of a pointer

    A long offset is added to an intel pointer by first converting the
    pointer to absolute, adding the offset and converting the result
    back to intel pointer representation.

*/

#if defined (O_SEG)

union BAA_UNN_DTA   baa_p_2_abs     (void *     ptr)

{
union BAA_UNN_DTA   dta;
union BAA_UNN_DTA   temp;

		dta.p_vd[0] = ptr;
		temp.us[1]  = dta.us[1] >> 12;
		temp.us[0]  = dta.us[1] << 4;
		dta.us[1]   = 0;

		temp.ul[0]  += dta.ul[0];

		return (temp);
}


void *          baa_abs_2_p     (unsigned long  address)

{

union BAA_UNN_DTA   temp;

        temp.ul[0]  = address;
        temp.us[1] <<= 12;
        temp.us[1] += temp.us[0] >> 4;
        temp.us[0] &= 0xF;

        return (temp.p_vd[0]);
}

void *          baa_p_arith     (void *     ptr, unsigned long l_offset)
{
union BAA_UNN_DTA   temp;

    temp = baa_p_2_abs (ptr);

    temp.ul[0] += l_offset;

    return (baa_abs_2_p (temp.ul[0]));
}

#endif

/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_pd
|
 ------------   A B S T R A C T   -------------------------------------------

    The electrical bus length is converted into propagation delay classes.
    This is relevant for the BAP-BAC implementation

*/

short baa_ebl_put (unsigned short ebl)
{
//    unsigned short	class;
    signed short  result = RET_OK;

	if (ebl <= BAA_PDC_1)
	{
//        class = 1;
    }
	else if (ebl <= BAA_PDC_2)
	{
//        class = 2;
    }
    else if (ebl <= BAA_PDC_3)
    {
//        class = 3;
    }
	else if (ebl <= BAA_PDC_4)
	{
//	    class = 4;
    }
	else if (ebl <= BAA_PDC_5)
	{
//	    class = 5;
    }
	else if (ebl <= BAA_PDC_6)
	{
//	    class = 6;
	}
	else
	{
//	    class = 7;
	    result = BA_E;
	}

	baa_dta.ebl.class = 6;
    return (result);
}

/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_pd
|
 ------------   A B S T R A C T   -------------------------------------------

    The propagation delay class configured by the application engineer is
    returned to the caller.

*/

void            baa_ebl_get  (unsigned short *   class)
{
		*class = baa_dta.ebl.class;
}



/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_pd
|
 ------------   A B S T R A C T   -------------------------------------------

     The time reserved for a telegram (master and slave frame) on the bus
     is dependant upon the size of the slave frame (ix_size) and of the
     propagation delay class.

     This function returns the reserved time for a telegram of ix_size
     in fcode representation 0 .... 4. If the fcode is larger than 4,
     it is assumed that the caller means a one word telegram.

*/

unsigned short  baa_pd_in_us        (unsigned short ix_size)

{
    if (ix_size <= 4)
    {
		return (baa_pdt.us[baa_dta.ebl.ix_table][ix_size]);
    }
    else
    {
		return (baa_pdt.us[baa_dta.ebl.ix_table][0]);
    }

}


/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_pd
|
 ------------   A B S T R A C T   -------------------------------------------

    The propagation delay table [class index][fcode ] has values for each
    class. This function returns the class index which is valid for this
    configuration.

*/

unsigned short  baa_pd_ix_table     (void)

{
		return (baa_dta.ebl.ix_table);
}

/* new page
 ----------------------------------------------------------------------------
|
|   name        baa_ix2p            "Index to pointer"
|
 -----------    A B S T R A C T     -----------------------------------------

    The BVLCONV pointers are somewhat strange. This routine converts these
    bizarre Stammtischpointers into conventional "C" pointers.

    Not used pointers are converted to NULL.

    Don't complain, it's not my fault!

*/

void            baa_ix2p            (struct BAA_STR_P_A16 * p_a16,
                                     void *                 p_base,
                                     unsigned short         ix,
                                     unsigned short         ix_next)

{
    if ((p_a16->cnt = (ix_next - ix) / 2) == 0)
    {
        p_a16->ptr = NULL;
    }
    else
    {
        p_a16->ptr = (unsigned short*) (((char *) p_base) + ix);
    }
}



/* new page
 -----------------------------------------------------------------------------
|
|   name        baa_delay
|
 ------------   A B S T R A C T   --------------------------------------------

    count to the parameter and returns. It wastes time.

*/

void            baa_delay           (unsigned count)

{
int i = 0;

    while (count-- != 0)
    {
        i = i + count;
    }
    i = i;
}


/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S       BAA_AUXI.C
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


    name of object modified     Nr: .2      dated:  9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/
