/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* Updated the interface with the lpl_4tcn file                                                 */
/*==============================================================================================*/


/* LPA_AS2C.H
ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º                                                                             º
º    Property of  :   ABB Verkehrssyteme AG, CH-8050 Zrich / Switzerland     º
º    COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                         º
º                                                                             º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                             º
º    Project      :   MicOS                                                   º
º    Subproject   :   Link Process                                            º
º                                                                             º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                             º
º    File         :   LPA_AS2C.H   Header File for TCN C version of ASM moduleº
º                                                                             º
º    Document     :   [1]: IEC WG 22-5, Upper Layers, Version 1.3             º
º                     [2]: Link Layer Process Specification                   º
º                     [3]: Link Layer Process Design                          º
º                                                                             º
º    Abstract     :   Prototypes of functions and macros which can also       º
º                     be implemented in ASM to enhance efficiency.            º
º    Remarks      :                                                           º
º                                                                             º
º    Dependencies :                                                           º
º                                                                             º
º    Accepted     :                                                           º
º                                                                             º
ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹
º                                                                             º
º    HISTORY:                                                                 º
º                                                                             º
º    Version    YY-MM-DD     Name           Dept.      Reference     Status   º
º    -------    --------     ----------     ------     ---------     -------  º
º       1       92-10-19     W. Marsden     BAEI-2         --                 º
º                                                                             º
ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
*/

#ifndef        	LPA_AS2C_H
#define        	LPA_AS2C_H


/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ Prototypes of LPA_AS2C.C
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  Source found in module LPA_AS2C.C

*/

void		_lpa_unpack (const struct STR_LP_PAC_0 *	p_pac_0);
void		_lpa_pack   (const struct STR_LP_PAC_0 *	p_pac_0);

void		lpa_memset		(void *		p_dest,
					 unsigned short	value,
					 unsigned short	size);

void		lpa_copy		(void *		p_dest,
					 void *		p_srce,
					 unsigned short	size);

		#if defined (O_TCN) || defined (O_PC) || defined (LPD_RP2)

void		lpa_put_frcd_prt	(void *  	p_prt,
					 void *  	p_frc,
					 unsigned int   size);

void		lpa_get_frcd_prt	(void *	 	p_tmp,
					 void *  	p_prt,
					 void *  	p_frc,
					 unsigned int   size);
void		lpa_tb_put_frcd_prt	(void *  	p_prt,
					 void *  	p_frc,
					 unsigned int   size);

void		lpa_tb_get_frcd_prt	(void *	 	p_tmp,
					 void *  	p_prt,
					 void *  	p_frc,
					 unsigned int   size);
		#endif


#if defined (O_TCN) || defined (O_PC) || defined (O_RP) || defined (O_KHW) || defined (LPD_RP2)

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_add_pge_offset_rd
³
³ (in )         p_pcs
³               
³ return value  offset in bytes
³ 
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  
  The byte offset between the base of the target port (page 0) and the active
  page for reading traffic store data is added to port pointer.

  !!!Attention: Disable interrupts before using this function and reenable them
		interrupts only after completing the data set read operation.

*/

#define		lpa_add_pge_offset_rd(p_cb, p_pcs, p_prt)       \
		{	unsigned short p_sizeof;						\
 			lpl_get_pge_offset_rd(p_cb, &p_sizeof, p_pcs);	\
 			p_prt = (void*)((char *)p_prt + p_sizeof);		}


/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_tb_add_pge_offset_rd
³
³ (in )         p_pcs
³               
³ return value  offset in bytes
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  The byte offset between the base of the target port (page 0) and the active
  page for reading TRAIN BUS traffic store data is added to port pointer.

  !!!Attention: Disable interrupts before using this function and reenable them
		interrupts only after completing the data set read operation.

*/

#define		lpa_tb_add_pge_offset_rd(p_cb, p_pcs, p_prt)		\
		{	unsigned short p_sizeof;                            \
 			lpl_tb_get_pge_offset_rd(p_cb, &p_sizeof, p_pcs);	\
 			p_prt = (void*)((char *) p_prt + p_sizeof);			}


/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_add_pge_offset_wt
³
³ (in )         p_pcs
³
³ return value  offset in bytes
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  The byte offset between the base of the target port (page 0) and the passive
  page for writing traffic store data (page 0 or page 1) is added to port ptr.

*/

#define		lpa_add_pge_offset_wt(p_cb, p_pcs, p_prt)       	\
		{   unsigned short p_sizeof;                            \
 		    lpl_get_pge_offset_wt(p_cb, &p_sizeof, p_pcs);      \
  			p_prt = (void*)((char *) p_prt + p_sizeof);			}



/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_tb_add_pge_offset_wt
³
³ (in )         p_pcs
³
³ return value  offset in bytes
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  The byte offset between the base of the target port (page 0) and the passive
  page for writing traffic store data OF TYPE TRAIN BUS is added to port ptr.

*/

#define		lpa_tb_add_pge_offset_wt(p_cb, p_pcs, p_prt)       	\
		{   unsigned short p_sizeof;                            \
 		    lpl_tb_get_pge_offset_wt(p_cb, &p_sizeof, p_pcs);	\
  			p_prt = (void*)((char *) p_prt + p_sizeof);			}



/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_tgl_prt_page
³
³ (in )         p_pcs
³               
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  toggles the page pointer of the PCS 

*/

#define		lpa_tgl_prt_page(p_cb, p_pcs)       \
			{lpl_tgl_prt_page (p_cb, p_pcs);}

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ macrofunction	lpa_get_ts_seg
³
³ param. in     p_pcs
³        out    -
³               
³ return value  offset in bytes
³ 
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

  
  This function can be used to separate the offset and the segment in
  '86 applications. No use in pure ANSI-C application.

  The ds_x parameters are copied to local variables

*/

#define		lpa_get_ts_seg(p_pcs, p_prt, p_frc, p_clus_ds) /* no func */

#define		lpa_copy_ds_x(p_pcs, p_prt, p_frc, prt_sze, p_ds_x)  \
{		 	p_pcs   = p_ds_x->ds_x.p_pcs; \
			p_prt   = p_ds_x->ds_x.p_prt; \
			p_frc   = p_ds_x->ds_x.p_frc; \
			prt_sze = p_ds_x->ds_x.prt_sze; }




/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_swap_bitset_16
³
³ (in )         p_dest
³ (in )         p_srce
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  A 16 bit value is copied, simultaneously swapping the high and the low byte

*/

/* #define	lpa_swap_bitset_16(p_dest, p_srce) 							  \ */
/* 	{ *(char *) ((char *) p_dest + 1) = *(char *) ((char *) p_srce) ; 	  \ */
/* 	  *(char *) ((char *) p_dest)     = *(char *) ((char *) p_srce + 1);}   */

#define	lpa_swap_bitset_16(p_dest, p_srce) \
	*(unsigned short*)p_dest= ((*(unsigned short*)p_srce) << 8) | \
							  ((*(unsigned short*)p_srce) >> 8);

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_swap_long_32
³
³ (in )         p_dest
³ (in )         p_srce
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  A 32 bit value is copied, simultaneously swapping the high and the low word


*/

#define	lpa_swap_long_32(p_dest, p_srce) \
	{ *(short*) ((short *) p_dest + 1) = *(short *) ((short *) p_srce) ; \
	  *(short*) ((short *) p_dest)     = *(short *) ((short *) p_srce + 1);}


/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_copy_16
³
³ param. in     p_dest
³               p_srce
³		count
³
³        out    -
³               
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  This macro is a temporary realisation of pi_copy16 ().

*/

#if defined (O_TCN) || defined (O_PC)
    #define lpa_copy_16(p_dest, p_srce, count) \
            { short *p_tmp_dest, *p_tmp_srce; \
              count++; \
              p_tmp_dest = (short *) p_dest; \
              p_tmp_srce = (short *) p_srce; \
              while (--count != 0) \
              { *(p_tmp_dest++) = *(p_tmp_srce++); }}
#endif

/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_swap_16
³
³ param. in     p_dest
³               p_srce
³		count
³
³        out    -
³               
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
  
  This function copies an array of characters, simultaneously swapping the
  high and low bytes

*/

#define	lpa_swap_16(p_dest, p_srce, count) \
	{ count++; while (--count != 0) \
	  { 	*(unsigned short*)p_dest= ((*(unsigned short*)p_srce) << 8) | \
							  		  ((*(unsigned short*)p_srce) >> 8);  \
	    p_srce = (char *) p_srce + 2; p_dest = (char *) p_dest + 2; }}




/* new page
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³
³ name          lpa_swap_32
³
³ (in )         p_dest
³ (in )         p_srce
³ (in )         count
³
³ return value  -
³
ÀÄÄÄÄÄÄÄÄÄÄÄÄ   A B S T R A C T   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


  This function copies an array of long, simultaneously swapping the
  high and low words

*/

#define	lpa_swap_32(p_dest, p_srce, count) \
	{ count++; while (--count != 0) \
	  { *(short *) ((short *) p_dest + 1) = *(short *) ((short *) p_srce);\
	    *(short *) ((short *) p_dest) = *(short *) ((short *) p_srce + 1);\
	    p_srce = (char *) p_srce + 4; p_dest = (char *) p_dest + 4; }}

#endif

#endif

/*
 ---------------------------------------------------------------------------Ä
|
|   M O D I F I C A T I O N S           LPA_AS2C.H
|   - - - - - - - - - - - - -           ---------
|
 ----------------------------------------------------------------------------

								Nr: 2   dated:  94-01-14

	Switch order changed, O_PC with O_TCN (no longer O_EKR)

 ----------------------------------------------------------------------------

								Nr: 3   dated:  94-01-27

	New function lpa_tb_add_pge_offset_rd()

*/
