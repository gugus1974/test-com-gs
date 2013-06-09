/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1992 ABB Verkehrssysteme AG
|                     
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAT_TFER
|
|    Document     :
|
|    Abstract     :   Class 4 Mastership Transfer
|
|    Remarks      :
|
|    Dependencies :
|
|    Accepted     :
|
 ------------------------------------------------------------------------------
|
|    HISTORY:
|
|    Vers  	 Dept.   YY-MM-DD  State    Name    Reference
|    ---- 	 ------  --------  -------  ------- ---------
|      1  	 BAEI-2  93-02-09  created  Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef		BAT_TFER_H
#define 	BAT_TFER_H




/*
 ---------------------------------------------------------------------------ƒ
|
|   Constants
|
 ----------------------------------------------------------------------------
*/

#define			BAT_RANGE			16


/*
 ---------------------------------------------------------------------------ƒ
|
|   Prototypes
|
 ----------------------------------------------------------------------------
*/
/*
void 			bat_open			(struct BAT_STR_DTA ** 		p_bat_dta);
*/

void			bat_init			(void);

short           bat_create          (struct BAM_STR_CFG_PT *  p_pt);
void       		bat_bld_kc4_mf     	(void);

void       		bat_task     	    (void);

unsigned int    bat_rank            (void);

void bat_pass_on_mastership  (unsigned short not_used2);

void bat_accept_mastership   (unsigned short not_used2);


/*
 ---------------------------------------------------------------------------ƒ
|
|   M O D I F I C A T I O N S		BAT_TFER.H
|   - - - - - - - - - - - - -       ----------
|
 ----------------------------------------------------------------------------


	name of object modified		Nr:	.2  	dated:	9?-??-??

    description of what was done to object

 ----------------------------------------------------------------------------

*/

#endif
