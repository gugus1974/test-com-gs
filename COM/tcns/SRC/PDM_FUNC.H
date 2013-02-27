/*************************************************************************
**        Copyright (C) Siemens AG 1996 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       function processing

**************************************************************************
**    AUTHOR::     Heindel (Hei)  
**    CREATED::    29.02.96
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:06 $
      $Log: PDM_FUNC.H,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

**************************************************************************/

#ifndef PDM_FUNC_H_
#define PDM_FUNC_H_


/* Type and size in one object */
#define PDM_TYPEBITS 6              /* number of bits of size encoding */
#define PDM_FOLD_SIZE_TYPE(size,type) (((size) << PDM_TYPEBITS) | (type))
#define PDM_B1    PDM_FOLD_SIZE_TYPE(  LP_SIZE_BITS, LP_TYPE_BOOL_1)
#define PDM_B2    PDM_FOLD_SIZE_TYPE(  LP_SIZE_BITS, LP_TYPE_BOOL_2)
#define PDM_B8    PDM_FOLD_SIZE_TYPE(     LP_SIZE_8, LP_TYPE_BITSET)
#define PDM_B16   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_BITSET)
#define PDM_B32   PDM_FOLD_SIZE_TYPE(    LP_SIZE_32, LP_TYPE_BITSET)
#define PDM_B64   PDM_FOLD_SIZE_TYPE(    LP_SIZE_64, LP_TYPE_BITSET)
#define PDM_I8    PDM_FOLD_SIZE_TYPE(     LP_SIZE_8, LP_TYPE_SIGN)
#define PDM_I16   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_SIGN)
#define PDM_I32   PDM_FOLD_SIZE_TYPE(    LP_SIZE_32, LP_TYPE_SIGN)
#define PDM_U4    PDM_FOLD_SIZE_TYPE(     LP_SIZE_8, LP_TYPE_BCD_4)
#define PDM_U8    PDM_FOLD_SIZE_TYPE(     LP_SIZE_8, LP_TYPE_CARD)
#define PDM_U16   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_CARD)
#define PDM_U32   PDM_FOLD_SIZE_TYPE(    LP_SIZE_32, LP_TYPE_CARD)
#define PDM_R32   PDM_FOLD_SIZE_TYPE(    LP_SIZE_32, LP_TYPE_REAL)
#define PDM_FI2   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_FRAC_200)
#define PDM_FU4   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_FRAC_400)
#define PDM_FI8   PDM_FOLD_SIZE_TYPE(    LP_SIZE_16, LP_TYPE_FRAC_800)


/* data structures for functions */

/* The functions can calculate on the basis of UNSIGNED32, INTEGER32 
 * and REAL32 
 */
#define PDM_CALC_TYPES 	3
#define PDM_CALC_I32 	0
#define PDM_CALC_U32 	1
#define PDM_CALC_R32 	2

#define PDM_CALC_NEGATED 0x80

typedef int  (*F_VALID)(void *p_var, void *p_val);
typedef void (*F_INVALID)(void *p_var, void *p_val);
typedef void (*F_CAST)(void *p_v1, void *p_v2, UNSIGNED8 bit_pos);

/*
 * A function gets the result (pointed to by p_result) from the previous 
 * computation and an argument (pointed to by p_result). It does some 
 * calculation and puts the result back.
 * If some error occurs the function should return FALSE to invalidate
 * the result and abort further calculation. Otherwise the function should 
 * return TRUE.
 */
typedef int  (*F_OP)(void *p_result, void *p_arg);


typedef struct STR_PDM_FUNC_ARG {
    void          *p_var;               /* location of function input */
    BOOLEAN       *p_chk;               /* location of associated check var */
    F_VALID       f_is_valid;           /* check validity of argument */
    F_CAST        f_up_cast;            /* cast arg to processable type */
    UNSIGNED8     bit_pos;              /* pit position inside bit set
					 * and flag if bit should be inverted
					 * before processing PDM_CALC_NEGATED
					 */
} T_PDM_FUNC_ARG;


typedef struct STR_PDM_FUNC_RES {
    void          *p_var;              /* location of function result      */
    BOOLEAN       *p_chk;              /* location of associated check var */
    F_CAST        f_down_cast;         /* cast result to desired type      */
    F_INVALID     f_invalidate;        /* invalidate result value          */
    
    UNSIGNED16    bytesize;	       /* size in bytes                    */
    unsigned char size;                /* size of result                   */
    unsigned char type;                /* type of result                   */
} T_PDM_FUNC_RES;


typedef void (*F_PDM_FUNC)(UNSIGNED16      n_args,
			   T_PDM_FUNC_ARG *p_args,
			   T_PDM_FUNC_RES *p_result);

typedef struct STR_PDM_FUNC_INFO {
    F_PDM_FUNC          function;   /* function to be called */
    UNSIGNED16          n_args;     /* number of arguments                */
    T_PDM_FUNC_ARG     *p_args;     /* function arguments (array)         */
    T_PDM_FUNC_RES      result;     /* return value                       */
} T_PDM_FUNC_INFO;


#define PDM_NUMBER_OF_FUNC 15

#ifndef PDM_FUNC_C_
extern F_PDM_FUNC pdm_func_table[PDM_NUMBER_OF_FUNC][PDM_CALC_TYPES];
#endif

/* PDM cast functions */
extern void pdm_cast_U4_U32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U8_U32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U16_U32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U32_U32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I8_I32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I16_I32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I32_I32(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_U32_U4(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U32_U8(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U32_U16(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I32_I8(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I32_I16(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_R32_U4(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_U8(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_U16(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_U32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_I8(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_I16(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_I32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_U4_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U8_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U16_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_U32_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I8_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I16_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_I32_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_FI2_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_FU4_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_FI8_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_R32_FI2(void *p_v1, void *p_v2, UNSIGNED8 dummy);


extern void pdm_cast_R32_FU4(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_R32_FI8(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern void pdm_cast_B1_B1(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_B2_B1(void *p_v1, void *p_v2, UNSIGNED8 dummy);
extern void pdm_cast_BSET_B1(void *p_v1, void *p_v2, UNSIGNED8 bit_pos);
extern void pdm_cast_B1_B2(void *p_v1, void *p_v2, UNSIGNED8 dummy);

extern int pdm_isvalid_1(void *p_v1, void *p_v2);
extern int pdm_isvalid_2(void *p_v1, void *p_v2);
extern int pdm_isvalid_4(void *p_v1, void *p_v2);
extern int pdm_isvalid_8(void *p_v1, void *p_v2);
extern int pdm_isvalid_16(void *p_v1, void *p_v2);
extern int pdm_isvalid_32(void *p_v1, void *p_v2);
extern int pdm_isvalid_64(void *p_v1, void *p_v2);
extern int pdm_never_valid(void *p_v1, void *p_v2);

extern void pdm_invalid_8(void *p_v1, void *p_v2);
extern void pdm_invalid_16(void *p_v1, void *p_v2);
extern void pdm_invalid_32(void *p_v1, void *p_v2);


extern void pdm_void_func(UNSIGNED16      n_args,
			  T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result);

#endif
