/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Defined REG                                                                         */
/* <ATR:02> Excluded lpa_asxx.h and lpl_lacs.h                                                  */
/* <ATR:03> Fixed lpa_get_frcd_prt() for MVBC                                                   */
/* <ATR:04> Fixed lpa_put_frcd_prt() for MVBC                                                   */
/*==============================================================================================*/


/*   LPA_AS2C.C
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º                                                                            º
º    Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland    º
º    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                        º
º                                                                            º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                            º
º    Project      :   MicOS                                                  º
º    Subproject   :   Process Data                                           º
º                                                                            º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                            º
º    File         :   LPA_AS2C.C   Assembler-C translation of ASM procedures º
º                                                                            º
º    Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3            º
º                     [2]: Link Layer Process Specification                  º
º                     [3]: Link Layer Process Design                         º
º                                                                            º
º    Abstract     :   Reserve Procedures, copy of original, tuned asm        º
º                     procedures to be used on targets with no inline asm    º
º                                                                            º
º    Remarks      :   Not employed in EKR, RP or KHW                         º
º                                                                            º
º    Dependencies :                                                          º
º                                                                            º
º    Accepted     :                                                          º
º                                                                            º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                            º
º    Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Name     Reference         º
º    ----  ------  --------  -------  ----------  -------  ---------         º
º      1   BAEI-2  92-10-12  created              Marsden      --            º
º      2   BATC    93-06-15  modified             Marsden      --            º
º                                                                            º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
*/

#define  LPA_AS2C_C

#include "lp_sys.h"		/*   LP user interface                      */
#include "pi_sys.h"		/*   PIL                                    */
/* <ATR:02> */
// #include "lpa_asxx.h"		/*   possible asm functionality export      */
// #include "lpl_lacs.h"
#include "lpl_4tcn.h"

#if 	!(defined (O_TCN) || defined (O_PC) || defined (LPD_RP2))
#error: This is a TCN File or a PC!
#endif

/* <ATR:01> */
#define REG

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ 	module name for event recorder
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

	The name of the module is stored here for the event recorder
*/

#if defined (O_TCN)

static char lp_local_module_name[] = "lpa_a2c";

#endif

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_get_frcd_prt
³
³ (out)         p_tmp 
³ (in )         p_prt
³ (in )         p_frc
³ (in )         size
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  

  The process data is merged with the force table using a logical algorithm
  to produce a traffic store image which takes forced variables into account.

  This procedure is called by the cluster copy routines. A further evaluation
  of the force status is not necessary. A future BAP should support this
  algorithm.

  To speed up the algorithm, integer pointers are used.

  !!!Attention: size must be adjusted, when integer differs in size to 16
		The compiler will announce this case via error message

  The algorithm:

      temp  = (port AND (NOT frc_mask)) OR (frc_var)

  is used.

*/

/* <ATR:03> */
void		lpa_get_frcd_prt	(void *	 		p_tmp,
					 			 void *  		p_prt,
					 			 void *  		p_frc,
					 			 unsigned int   size)
{
	short *p_int_frc = p_frc;
    short *p_int_tmp = p_tmp;
    short *p_int_prt = p_prt;

	while (size-- != 0) {
		*p_int_tmp++ = (*p_int_prt++ & ~*(p_int_frc + 16)) | *p_int_frc;
		 p_int_frc++;
	}

/*
REG short *	p_reg_frc = p_frc;
    short *  	p_int_tmp = p_tmp;
    short *  	p_int_prt = p_prt;


	while (size-- != 0)
	{
		*p_int_tmp++ = (*p_int_prt++ & ~*(p_reg_frc))
					     |  *(p_reg_frc + 16);
		 p_reg_frc++;
	}
*/
}


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
³ ----  ------  --------  -------  ----------  ---  -------  ---------
³   1   BAEI-2  91-09-20  created  LPDBR2.TXT  2.1  Marsden      --   
³   2   BAEI-2  92-10-05  modif'd                   Marsden      --   
³   3   BATC    93-06-15  modif'd                   Marsden      --   
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
End of function lpa_get_frcd_prt  */

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_put_frcd_prt
³
³ (in - out)    p_prt
³ (in )         p_frc
³ (in )         size
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  

  The process data is merged with the force table using a logical algorithm
  to produce a traffic store image which takes forced variables into account.

  This procedure is called by the cluster copy routines. A further evaluation
  of the force status is not necessary. A future BAP should support this
  algorithm.

  To speed up the algorithm, integer pointers are used.

  !!!Attention: size must be adjusted, when integer differs in size to 16
		The compiler will announce this case via error message

  The algorithm:

      port  = (port AND (NOT frc_mask)) OR (frc_var)

  is used.

*/

/* <ATR:04> */
void		lpa_put_frcd_prt	(void *  		p_prt,
					 			 void *  		p_frc,
					 			 unsigned int   size)
{
	short *p_int_frc = p_frc;
	short *p_int_prt = p_prt;

	while (size-- != 0)
	{
		*p_int_prt++ = (*p_int_prt & ~*(p_int_frc + 16)) | *p_int_frc;
		 p_int_frc++;
	}

/*
REG short *	p_reg_frc = p_frc;
REG short *  	p_reg_prt = p_prt;

	while (size-- != 0)
	{
		*p_reg_prt++ = (*p_reg_prt & ~*(p_reg_frc))
					   |  *(p_reg_frc + 16);
		 p_reg_frc++;
	}
*/
}


/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_tb_put_frcd_prt
³
³ (in - out)    p_prt
³ (in )         p_frc
³ (in )         size
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  The process data is merged with the force table using a logical algorithm
  to produce a traffic store image which takes forced variables into account.

  This procedure is called by the cluster copy routines. A further evaluation
  of the force status is not necessary. A future BAP should support this
  algorithm.

  To speed up the algorithm, integer pointers are used.

  !!!Attention: size must be adjusted, when integer differs in size to 16
		The compiler will announce this case via error message

  The algorithm:

	  port  = (port AND (NOT frc_mask)) OR (frc_var)

  is used.

*/

void		lpa_tb_put_frcd_prt	(void *  	p_prt,
					 void *  	p_frc,
					 unsigned int   size)
{
REG short *	p_reg_frc = p_frc;
REG short *  	p_reg_prt = p_prt;


	while (size-- != 0)
	{
		*p_reg_prt++ = (*p_reg_prt & ~*(p_reg_frc))
					   |  *(p_reg_frc + (LPL_SIZEOF_TB_PRT_PGE/2));
		 p_reg_frc++;
	}
}


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
³ ----  ------  --------  -------  ----------  ---  -------  ---------
³   1   BAEI-2  91-09-20  created  LPDBR2.TXT  2.1  Marsden      --   
³   2   BAEI-2  92-10-05  modif'd                   Marsden      --   
³   3   BATC    93-06-15  modif'd                   Marsden      --   
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
End of function lpa_put_frcd_prt  */


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_pack
³
³ (in )         p_pack			structure STR_LP_PAC_0
³               
³ return value  -
³ 
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  
   The Pack list defined by the structure STR_LP_PAC_0 and STR_LP_PAC_1
   is  used to find the bitset 16 variables which are packed from 1 bit
   (bit 0) characters.


*/

union	UNN_LPI_NEAR
{
	void *		p_;
	unsigned short	offst;
};

#define lpi_off_2_ptr(p_b,indx) {((union UNN_LPI_NEAR *) p_b)->offst = indx;}

void		 _lpa_pack	(const struct STR_LP_PAC_0 *	p_pac_0)

{
int		 pac_1_count;
int		 pac_2_count;
unsigned short * p_bitset;
unsigned char  * p_bool;

struct	 STR_LP_PAC_1 *	p_pac_1;
struct	 STR_LP_PAC_2 *	p_pac_2;

	#if defined (O_EKR) || defined (O_PC) || defined (O_TCN)
	p_pac_1 = (struct STR_LP_PAC_1 *) p_pac_0;	/*   load the data segment  */
	p_pac_2 = (struct STR_LP_PAC_2 *) p_pac_0;	/*   load the data segment  */
	p_bitset= (unsigned short *)      p_pac_0;
	p_bool  = (unsigned char  *)      p_pac_0;
						#endif
	lpi_off_2_ptr (&p_pac_1, p_pac_0->i_pac_1);

	pac_1_count = p_pac_0->pac_1_count;

	while 	(pac_1_count-- > 0)
	{
		lpi_off_2_ptr (&p_pac_2, p_pac_1->i_pac_2);
		lpi_off_2_ptr (&p_bitset, p_pac_1->i_bitset);

		*p_bitset   = 0;

		pac_2_count = p_pac_1->bit_count;

		while 	(pac_2_count-- > 0)
		{
			lpi_off_2_ptr (&p_bool, p_pac_2->i_bool);
			(*p_bitset) <<= p_pac_2->boff;
			 *p_bitset |= (*p_bool & 1);
			p_pac_2++;
		}
		p_pac_1++;
	}
}


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
³ ----  ------  --------  -------  ----------  ---  -------  ---------
³  0.1  BAEI-2  92-10-06  created                   Marsden      --
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
End of function lpa_pack */

/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_unpack
³
³ (in )         p_unpack
³               
³ return value  -
³ 
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  
   The unpack list defined by the structure STR_LP_PAC_0 and STR_LP_PAC_1
   is  used to find the bitset 16 variables which are unpacked into 1 bit
   (bit 0) characters.


*/

void		_lpa_unpack	(const struct STR_LP_PAC_0 *	p_pac_0)
{

int		 pac_1_count;
int		 pac_2_count;
unsigned short * p_bitset;
unsigned char  * p_bool;

struct	 STR_LP_PAC_1 *	p_pac_1;
struct	 STR_LP_PAC_2 *	p_pac_2;

	#if defined (O_EKR) || defined (O_PC) || defined (O_TCN)

	p_pac_1 = (struct STR_LP_PAC_1 *) p_pac_0;	/*   load the data segment  */
	p_pac_2 = (struct STR_LP_PAC_2 *) p_pac_0;	/*   load the data segment  */
	p_bitset= (unsigned short *)      p_pac_0;
	p_bool  = (unsigned char  *)      p_pac_0;
	#endif
	lpi_off_2_ptr (&p_pac_1, p_pac_0->i_pac_1);

	pac_1_count = p_pac_0->pac_1_count;

	while 	(pac_1_count-- > 0)
	{
		lpi_off_2_ptr (&p_pac_2, p_pac_1->i_pac_2);
		lpi_off_2_ptr (&p_bitset, p_pac_1->i_bitset);

		pac_2_count = p_pac_1->bit_count;

		while 	(pac_2_count-- > 0)
		{
			lpi_off_2_ptr (&p_bool, p_pac_2->i_bool);

			(*p_bitset) >>= (p_pac_2->boff) - 1;
			 *p_bool = *((unsigned char *) p_bitset) & 1;
			(*p_bitset) >>= 1;
			 p_pac_2++;
		}
		p_pac_1++;
	}

}


/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³ Vers  Dept.   YY-MM-DD  Status   Debug Rep.  Ctr  Name     Reference
³ ----  ------  --------  -------  ----------  ---  -------  ---------
³  0.1  BAEI-2  92-10-06  created                   Marsden      --
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
End of function lpa_unpack */

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_memset
³
³ (out)         p_dest
³ (in )         value			- value in 16 bits
³ (in )		size			- in bytes
³               
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  Writes the parameter "value" to the memory buffer of size

  Does not support byte alignment!

*/

void		lpa_memset		(void *		p_dest,
					 unsigned short	value,
					 unsigned short	size)
{
unsigned short 	count;
unsigned short *p_trgt;

	p_trgt = (unsigned short *) p_dest;
	count  = size >> 1;

	while (count--  > 0)
	{
		*p_trgt++ = value;
	}
}
/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_copy
³
³ (out)         p_dest
³ (in )         p_srce			- value in 16 bits
³ (in )		size 			- in bytes
³               
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  A copy function. Does not support byte alignment!

*/

void		lpa_copy		(void *		p_trgt,
					 void *		p_from,
					 unsigned short	size)
{
unsigned short 	count;
unsigned short *p_dest;
unsigned short *p_srce;

	p_dest = (unsigned short *) p_trgt;
	p_srce = (unsigned short *) p_from;

	count  = size >> 1;

	while (count--  > 0)
	{
		*p_dest++ = *p_srce++;
	}
}

/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S           LP_AS2C.C
|   - - - - - - - - - - - - -           ---------
|
 ----------------------------------------------------------------------------

    lp_get_frcd_prt             Nr: .3  dated:  93-06-15

    Autoincrement separated from operation where pointer used more than once

 ----------------------------------------------------------------------------

    lp_put_frcd_prt             Nr: .3  dated:  93-06-15

    Autoincrement separated from operation where pointer used more than once

 ----------------------------------------------------------------------------

                                Nr: .4  dated:  94-01-14

    Switch O_PC added to allow compile for PC

 ----------------------------------------------------------------------------

*/
