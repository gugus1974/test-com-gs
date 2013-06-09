/* LP_BITS.H
 ----------------------------------------------------------------------------- 
                                                                               
     Property of  :   ABB Verkehrssyteme AG, CH-8050 ZÅrich / Switzerland      
     COPYRIGHT    :   (c) 1991 ABB Verkehrssysteme AG                          
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     Project      :   MicOS                                                    
     Subproject   :   Link Layer Process                                       
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     File         :   lp_bits.h                                                
                                                                               
     Document     :   [1] IEC WG 22-5 "Upper Layers", Version 1.3              
                      [2] Link Layer Process Specification                     
                      [3] Link Layer Process Design                            
                                                                               
     Abstract     :   Support macros to manipulate bits in bitfields           
                                                                               
     Remarks      :                                                            
                                                                               
     Dependencies :                                                            
                                                                               
     Accepted     :                                                            
                                                                               
 ----------------------------------------------------------------------------- 
                                                                               
     HISTORY:                                                                  
                                                                               
     Version    YY-MM-DD     Name           Dept.      Reference     Status    
     -------    --------     ----------     ------     ---------     -------   
        1       91-01-31     M.Rîthlin      BAEI-2                   created   
        2       91-03-06     N. Gatz        BAEI-2         --        modified  
        3       91-09-20     W. Marsden     BAEI-2         --        modified  
                                                                               
 ----------------------------------------------------------------------------- 
*/

#ifndef        LP_BITS_H
#define        LP_BITS_H


/*
 ----------------------------------------------------------------------------- 
| 
| constant      LP_UNMASK_....
| 
 ------------   A B S T R A C T   -------------------------------------------- 
 
 
  These constants are used to unmask only the bits defined for the type
  accessed. 

  The constants are employed in the functions

		lp_put_bits
		lp_get_bits
		lp_put_field
		lp_get_field

  
*/


#define 	LP_UNMASK_BOOL_1		1
	
#define 	LP_UNMASK_BOOL_2		3

#define 	LP_UNMASK_BCD_4			0xF




/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_f..._bit
| 
 ----------------------------------------------------------------------------- 

  the lp_f... functions return the bitfield which has been manipulated
  by the operation described by the name

*/


#define lp_fset_bit(item, bit_nr)   (item |   (0x1 << bit_nr))
#define lp_fclr_bit(item, bit_nr)   (item &  ~(0x1 << bit_nr))
#define lp_ftst_bit(item, bit_nr)   (item &   (0x1 << bit_nr))


/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_..._bit
| 
 ----------------------------------------------------------------------------- 

  the lp_... manipulate the bitfield directly

*/


#define lp_set_bit(item, bit_nr)    {item |=  (0x1 << bit_nr);}
#define lp_clr_bit(item, bit_nr)    {item &= ~(0x1 << bit_nr);}


/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_..._bool_..
|
 ----------------------------------------------------------------------------- 


  The target item is always a pointer type. Each function is diverted to
  a function which performs the same task for boolean, antivalent and bcd_4.
  The difference is the value of LP_UNMASK_...

	lp_get_bool_1 reads the boolean value
	lp_put_bool_1 writes the boolean value
	lp_get_fld_bool_1 reads the boolean value from within a field
	lp_put_fld_bool_1 writes the boolean value into a field

*/


#define lp_get_bool_1(item) \
		lp_get_bits  (item, LP_UNMASK_BOOL_1)

#define lp_put_bool_1(item, val) \
		lp_put_bits  (item, val, LP_UNMASK_BOOL_1)

#define lp_put_fld_bool_1(item, bit_nr, val) \
		lp_put_field (item, bit_nr, val, LP_UNMASK_BOOL_1)

#define lp_get_fld_bool_1(item, bit_nr) \
		lp_get_field (item, bit_nr, LP_UNMASK_BOOL_1)


/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_..._antivalent
| 
 ----------------------------------------------------------------------------- 


  The target item is always a pointer type. Each function is diverted to
  a function which performs the same task for boolean, antivalent and bcd_4.
  The difference is the value of LP_UNMASK_...

	lp_get_bool_2 reads the antivalent value
	lp_put_bool_2 writes the antivalent value
	lp_get_fld_bool_2 reads the antivalent value from within a field
	lp_put_fld_bool_2 writes the antivalent value into a field

*/

#define lp_get_bool_2(item) \
		lp_get_bits  (item, LP_UNMASK_BOOL_2)

#define lp_put_bool_2(item, val) \
		lp_put_bits  (item, val, LP_UNMASK_BOOL_2)

#define lp_put_fld_bool_2(item, bit_nr, val) \
		lp_put_field (item, bit_nr, val, LP_UNMASK_BOOL_2)

#define lp_get_fld_bool_2(item, bit_nr) \
		lp_get_field (item, bit_nr, LP_UNMASK_BOOL_2)

/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_..._bcd4
| 
 ----------------------------------------------------------------------------- 


  The target item is always a pointer type. Each function is diverted to
  a function which performs the same task for boolean, antivalent and bcd_4.
  The difference is the value of LP_UNMASK_...

	lp_get_bcd_4 reads the bcd4 value
	lp_put_bcd_4 writes the bcd4 value
	lp_get_fld_bcd_4 reads the bcd4 value from within a field
	lp_put_fld_bcd_4 writes the bcd4 value into a field

*/

#define lp_get_bcd_4(item) \
		lp_get_bits  (item, LP_UNMASK_BCD_4)

#define lp_put_bcd_4(item, val) \
		lp_put_bits  (item, val, LP_UNMASK_BCD_4)

#define lp_put_fld_bcd_4(item, bit_nr, val)  		\
		lp_put_field (item, bit_nr, val, LP_UNMASK_BCD_4)

#define lp_get_fld_bcd_4(item, bit_nr)	  		\
		lp_get_field (item, bit_nr, LP_UNMASK_BCD_4)


/*
 ----------------------------------------------------------------------------- 
| 
| macrofunction lp_
| 
 ----------------------------------------------------------------------------- 


  These functions perform bit operations upon pointer type variables of the
  parameter type dependant size.

	lp_get_bits reads the first xx bits of the pointer variable
	lp_put_bits writes the first xx bits to the pointer variable
	lp_get_field reads xx bits from within a field
	lp_put_field writes xx bits into a field

*/


#define 	lp_put_field(item, bit_nr, val, mask)               \
		{*(item) = (*(item) & ~(mask            << bit_nr)) \
                                    | (((val) & (mask)) << bit_nr);}

#define 	lp_get_field(item, bit_nr, mask) \
			((item >> bit_nr) & mask)

#define 	lp_get_bits(item, mask) \
			(item & mask)

#define	 	lp_put_bits(item, val, mask)  \
			{*item = (val & mask);}


/*
 ----------------------------------------------------------------------------- 
| 
| prototype     lp_get_bit_offset
|
 ----------------------------------------------------------------------------- 
*/

int		lp_get_bit_offset 	(unsigned 	bit_field,
					 unsigned * 	bit_offset);


/*
 ----------------------------------------------------------------------------- 
| 
| Version 0.2   Modifications
| 
 ----------------------------------------------------------------------------- 


  1)            

  2)		

*/

#endif
