/*************************************************************************
**    Copyright (C) Siemens AG 1993 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       Process Data Marshalling functions

**************************************************************************
**    AUTHOR::     Arnold (Ar)    
                   Heindel (Hei)  
**    CREATED::    05.08.93
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:06 $
      previous history:
                   date      sign   remark
                   05.08.93  Ar     start of development
                   24.08.93  Ar     adaption to changed interface to config.
                   08.10.93  Ar     minor changes for the boolean functions
                   18.11.93  Kra    mem.h replaced by stddef.h
                   18.11.93  Kra    static definition of convert_frac_to 
                                    modif'd

      $Log: PDM_FUNC.C,v $
      Revision 1.1.2.1  2006/12/01 10:40:06  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

*************************************************************************/

#define PDM_MODULE	/* access to PDM internal definitions */
#define PDM_FUNC_C_	/* braindead IC960 compiler           */

#ifdef O_960
#pragma noalign
#endif

#include "pdmdebug.h"
#include "pdmtypes.h"
#include "pdm_cp.h"



#define pdm_cast_xx_yy(mnemo1, mnemo2, type1, type2) \
void pdm_cast_##mnemo1##_##mnemo2(void *p_v1, void *p_v2, UNSIGNED8 dummy)\
{\
     *(type2 *)p_v2 = *(type1 *)p_v1; \
}

#define pdm_cast_R32_yy(mnemo2, type2) \
void pdm_cast_R32_##mnemo2(void *p_v1, void *p_v2, UNSIGNED8 dummy)\
{\
     *(type2 *)p_v2 = (type2)*(REAL32 *)p_v1; \
}

#define pdm_cast_xx_R32(mnemo1, type1) \
void pdm_cast_##mnemo1##_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy)\
{\
     *(REAL32 *)p_v2 = (REAL32)*(type1 *)p_v1; \
}



/* cast functions */
pdm_cast_xx_yy( U4, U32,    UNSIGNED8, UNSIGNED32)
pdm_cast_xx_yy( U8, U32,    UNSIGNED8, UNSIGNED32)
pdm_cast_xx_yy(U16, U32,   UNSIGNED16, UNSIGNED32)
pdm_cast_xx_yy(U32, U32,   UNSIGNED32, UNSIGNED32)
pdm_cast_xx_yy( I8, I32,     INTEGER8,  INTEGER32)
pdm_cast_xx_yy(I16, I32,    INTEGER16,  INTEGER32)
pdm_cast_xx_yy(I32, I32,    INTEGER32,  INTEGER32)

pdm_cast_xx_yy(U32,  U4, UNSIGNED32,  UNSIGNED8)
pdm_cast_xx_yy(U32,  U8, UNSIGNED32,  UNSIGNED8)
pdm_cast_xx_yy(U32, U16, UNSIGNED32, UNSIGNED16)
pdm_cast_xx_yy(I32,  I8,  INTEGER32,   INTEGER8)
pdm_cast_xx_yy(I32, I16,  INTEGER32,  INTEGER16)


void
pdm_cast_R32_U4(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(UNSIGNED8 *)p_v2 = (UNSIGNED8)*(REAL32 *)p_v1 & 0xf;
}


pdm_cast_R32_yy( U8,  UNSIGNED8)
pdm_cast_R32_yy(U16, UNSIGNED16)
pdm_cast_R32_yy(U32, UNSIGNED32)
pdm_cast_R32_yy( I8,   INTEGER8)
pdm_cast_R32_yy(I16,  INTEGER16)
pdm_cast_R32_yy(I32,  INTEGER32)
pdm_cast_R32_yy(R32,     REAL32)

pdm_cast_xx_R32( U4,  UNSIGNED8)
pdm_cast_xx_R32( U8,  UNSIGNED8)
pdm_cast_xx_R32(U16, UNSIGNED16)
pdm_cast_xx_R32(U32, UNSIGNED32)
pdm_cast_xx_R32( I8,   INTEGER8)
pdm_cast_xx_R32(I16,  INTEGER16)
pdm_cast_xx_R32(I32,  INTEGER32)

void
pdm_cast_FI2_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(REAL32 *)p_v2 = (REAL32)*(INTEGER16 *)p_v1 / (REAL32)0x4000;
}

void
pdm_cast_FU4_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(REAL32 *)p_v2 = (REAL32)*(UNSIGNED16 *)p_v1 / (REAL32)0x4000;
}

void
pdm_cast_FI8_R32(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(REAL32 *)p_v2 = (REAL32)*(INTEGER16 *)p_v1 / (REAL32)0x1000;
}

void
pdm_cast_R32_FI2(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(INTEGER16 *)p_v2 = (INTEGER16)(*(REAL32 *)p_v1 * 16384.0);
}

void
pdm_cast_R32_FU4(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(UNSIGNED16 *)p_v2 = (UNSIGNED16)(*(REAL32 *)p_v1 * 16384.0);
}

void
pdm_cast_R32_FI8(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(INTEGER16 *)p_v2 = (INTEGER16)(*(REAL32 *)p_v1 * 4096.0);
}

void
pdm_cast_B1_B1(void *p_v1, void *p_v2, UNSIGNED8 bit_pos)
{
    *(UNSIGNED8 *)p_v2 = 
	(*(UNSIGNED8 *)p_v1 & 0x1) ^
	    ((bit_pos & PDM_CALC_NEGATED) != 0);
}

void
pdm_cast_B2_B1(void *p_v1, void *p_v2, UNSIGNED8 bit_pos)
{
    *(UNSIGNED8 *)p_v2 = 
	(*(UNSIGNED8 *)p_v1 == BOOLEAN2_TRUE) ^
	    ((bit_pos & PDM_CALC_NEGATED) != 0);
}

void
pdm_cast_B1_B2(void *p_v1, void *p_v2, UNSIGNED8 dummy)
{
    *(UNSIGNED8 *)p_v2 = (*(UNSIGNED8 *)p_v1) ? BOOLEAN2_TRUE  : BOOLEAN2_FALSE; 
}

void
pdm_cast_BSET_B1(void *p_v1, void *p_v2, UNSIGNED8 bit_pos)
{
    UNSIGNED8 bpos = (bit_pos & ~PDM_CALC_NEGATED);
    UNSIGNED8 neg =  (bit_pos & PDM_CALC_NEGATED) != 0;
    *(UNSIGNED8 *)p_v2 = 
	((*((UNSIGNED8 *)p_v1 + (bpos >> 3)) >> (bpos & 0x7)) & 0x1) ^ neg;
}


void 
pdm_invalid_8(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	*(UNSIGNED8 *)p_chk = PDM_CHK_INVALID;
    }
    else {
	*(UNSIGNED8 *)p_var = 0xff;
    }
}

void 
pdm_invalid_16(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	*(UNSIGNED8 *)p_chk = PDM_CHK_INVALID;
    }
    else {
	*(UNSIGNED16 *)p_var = 0xffff;
    }
}

void 
pdm_invalid_32(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	*(UNSIGNED8 *)p_chk = PDM_CHK_INVALID;
    }
    else {
	*(UNSIGNED32 *)p_var = 0xffffffff;
    }
}

/* validity check functions */
#define pdm_isvalid_xx(mnemo,type,hex) \
int pdm_isvalid_##mnemo(void *p_var, void *p_chk) \
{ \
    if (p_chk != NULL) { \
	return *(UNSIGNED8 *)p_chk == PDM_CHK_VALID || \
	    *(UNSIGNED8 *)p_chk == PDM_CHK_FORCE; \
    } \
    return *(type *)p_var != (hex); \
}

pdm_isvalid_xx( 4,  UNSIGNED8, 0xfU)
pdm_isvalid_xx( 8,  UNSIGNED8, 0xffU)
pdm_isvalid_xx(16,  UNSIGNED16, 0xffffU)
pdm_isvalid_xx(32,  UNSIGNED32, 0xffffffffU)

int pdm_isvalid_1(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	return *(UNSIGNED8 *)p_chk == PDM_CHK_VALID ||
	    *(UNSIGNED8 *)p_chk == PDM_CHK_FORCE;
    }
    return TRUE;
}

int pdm_isvalid_2(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	return *(UNSIGNED8 *)p_chk == PDM_CHK_VALID ||
	    *(UNSIGNED8 *)p_chk == PDM_CHK_FORCE;
    }
    return *(UNSIGNED8 *)p_var == PDM_CHK_VALID ||
	*(UNSIGNED8 *)p_var == PDM_CHK_FORCE;
}

int pdm_isvalid_64(void *p_var, void *p_chk)
{
    if (p_chk != NULL) {
	return *(UNSIGNED8 *)p_chk == PDM_CHK_VALID ||
	    *(UNSIGNED8 *)p_chk == PDM_CHK_FORCE;
    }
    return *(UNSIGNED32 *)p_var != 0xffffffffU && 
	*((UNSIGNED32 *)p_var + 1) != 0xffffffffU; \
}

int pdm_never_valid(void *p_var, void *p_chk)
{
    return FALSE;
}


/* generic function to do all PDM built-in functions */

#define U32BUFLEN 8  /* length of argument/result buffer in UNSIGNED32 units */

STATIC void 
generic_func(UNSIGNED16      n_args,
	     T_PDM_FUNC_ARG *p_args,
	     T_PDM_FUNC_RES *p_result,
	     F_OP           op)
{
    UNSIGNED16 i;
    UNSIGNED32 result[U32BUFLEN];
    UNSIGNED32 arg[U32BUFLEN];

    if (p_result->p_var == NULL) {
	/* function result is not used, do not call the function at all */
	return;
    }

    /* check validity of first argument */
    if (n_args == 0 || p_args->f_is_valid(p_args->p_var, p_args->p_chk) == FALSE) {
	goto INVALID;
    }

    /* cast argument to function calculation type */
    p_args->f_up_cast(p_args->p_var, result, p_args->bit_pos);

    /* for each of the remaining arguments */
    for (i = 1;  i < n_args; i++) {
	p_args++;
	/* check validity of argument */
	if (p_args->f_is_valid(p_args->p_var, p_args->p_chk) == FALSE) {
	    goto INVALID;
	}
	/* do calculation */
	p_args->f_up_cast(p_args->p_var, arg, p_args->bit_pos);
	if (op == NULL || op(result, arg) == FALSE) {
	    goto INVALID;
	}
    }

    /* cast result to function type */
    p_result->f_down_cast(result, p_result->p_var, 0);
    if (p_result->p_chk != NULL) {
	*(UNSIGNED8 *)p_result->p_chk = PDM_CHK_VALID;
    }
    return;

  INVALID:
    /* invalid or calculation error */
    p_result->f_invalidate(p_result->p_var, p_result->p_chk);
    return;
}    


/* PDM built-in functions */

#define PDM_F_MINMAX(name, type, op) \
STATIC int name(void *result, void *arg) \
{ \
    if(*(type *)arg op *(type *)result) { \
	*(type *)result = *(type *)arg; \
    } \
    return TRUE; \
} \
STATIC void pdm_##name(UNSIGNED16      n_args, \
		       T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result) \
{ \
    generic_func(n_args, p_args, p_result, name); \
}

PDM_F_MINMAX(umin, UNSIGNED32, <)
PDM_F_MINMAX(imin,  INTEGER32, <)
PDM_F_MINMAX(rmin,     REAL32, <)
PDM_F_MINMAX(umax, UNSIGNED32, >)
PDM_F_MINMAX(imax,  INTEGER32, >)
PDM_F_MINMAX(rmax,     REAL32, >)


#define PDM_F_SUM(name, type) \
STATIC int name(void *result, void *arg) \
{ \
    *(type *)result += *(type *)arg; \
    return TRUE; \
} \
STATIC void pdm_##name(UNSIGNED16      n_args, \
		       T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result) \
{ \
    generic_func(n_args, p_args, p_result, name); \
}

PDM_F_SUM(usum, UNSIGNED32)
PDM_F_SUM(isum,  INTEGER32)
PDM_F_SUM(rsum,     REAL32)

	
#define PDM_F_BOOL(name, type, op) \
STATIC int name(void *result, void *arg) \
{ \
    *(type *)result op *(type *)arg; \
    return TRUE; \
} \
STATIC void pdm_##name(UNSIGNED16      n_args, \
		       T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result) \
{ \
    generic_func(n_args, p_args, p_result, name); \
}

PDM_F_BOOL(and, UNSIGNED8, &=)
PDM_F_BOOL( or, UNSIGNED8, |=)
PDM_F_BOOL(xor, UNSIGNED8, ^=)

STATIC void 
pdm_conv(UNSIGNED16      n_args,
	 T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result)
{
    generic_func(n_args, p_args, p_result, NULL); 
}

STATIC void 
pdm_select(UNSIGNED16      n_args,
	   T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result)
{
    UNSIGNED32 index;
    UNSIGNED32 result[U32BUFLEN];

    if (p_result->p_var == NULL) {
	/* function result is not used, do not call the function at all */
	return;
    }

    /* check validity of first argument */
    if (n_args == 0 || p_args->f_is_valid(p_args->p_var, p_args->p_chk) == FALSE) {
	goto INVALID;
    }

    /* cast argument to function calculation type */
    p_args->f_up_cast(p_args->p_var, result, 0);

    index = *(UNSIGNED32 *)result;

    if (index < 0 || index > n_args - 2) {
	goto INVALID;
    }

    /* select argument */
    p_args += (index + 1);
    
    /* check validity of argument */
    if (p_args->f_is_valid(p_args->p_var, p_args->p_chk) == FALSE) {
	goto INVALID;
    }

    /* cast result to function type */
    p_args->f_up_cast(p_args->p_var, result, p_args->bit_pos);
    p_result->f_down_cast(result, p_result->p_var, 0);
    if (p_result->p_chk != NULL) {
	*(UNSIGNED8 *)p_result->p_chk = PDM_CHK_VALID;
    }
    return;

  INVALID:
    /* invalid or calculation error */
    p_result->f_invalidate(p_result->p_var, p_result->p_chk);
    return;
}    


/*ARGSUSED*/
void 
pdm_void_func(UNSIGNED16      n_args,
	      T_PDM_FUNC_ARG *p_args, T_PDM_FUNC_RES *p_result)
{
    if (p_result != NULL) {
	p_result->f_invalidate(p_result->p_var, p_result->p_chk);
    }
}


F_PDM_FUNC pdm_func_table[PDM_NUMBER_OF_FUNC][PDM_CALC_TYPES] = {
    {pdm_and, NULL, NULL},            /* PDM_FUNC_AND  0 */
    {pdm_or, NULL, NULL},             /* PDM_FUNC_OR   1 */
    {pdm_xor, NULL, NULL},            /* PDM_FUNC_XOR  2 */
    {pdm_imin, pdm_umin, pdm_rmin},   /* PDM_FUNC_MIN  3 */
    {pdm_imax, pdm_umax, pdm_rmax},   /* PDM_FUNC_MAX  4 */
    {pdm_conv, pdm_conv, pdm_conv},   /* PDM_FUNC_CONV 5: do only the casts 
				       * and no calculation */
    {pdm_isum, pdm_usum, pdm_rsum},   /* PDM_FUNC_SUM  6 */
    {pdm_select, pdm_select, NULL},   /* PDM_FUNC_SELECT 7 */
};

#ifdef DEBUG
static struct F2N {
    F_PDM_FUNC func;
    char *name;
} func2name_tab[] = {
    { pdm_and, "AND   "},
    {  pdm_or, "OR    "},
    { pdm_xor, "XOR   "},
    {pdm_imin, "MIN(I)"},
    {pdm_umin, "MIN(U)"},
    {pdm_rmin, "MIN(R)"},
    {pdm_imax, "MAX(I)"},
    {pdm_umax, "MAX(U)"},
    {pdm_rmax, "MAX(R)"},
    {pdm_conv, "CONV  "},
    {pdm_select, "SEL  "},
};

char *
func2name(F_PDM_FUNC func)
{
    struct F2N *f;

    for (f = func2name_tab; f->func; f++) {
	if (f->func == func) {
	    return f->name;
	}
    }
    return "NULL  ";
}

static struct C2N {
    F_CAST cast;
    char *name;
} cast2name_tab[] = {
    { pdm_cast_U4_U32,  "U4_U32"},
    { pdm_cast_U8_U32,  "U8_U32"},
    { pdm_cast_U16_U32, "U16_U32"},
    { pdm_cast_U32_U32, "U32_U32"},
    { pdm_cast_I8_I32,  "I8_I32"},
    { pdm_cast_I16_I32, "I16_I32"},
    { pdm_cast_I32_I32, "I32_I32"},
    { pdm_cast_U32_U4,  "U32_U4"},
    { pdm_cast_U32_U8,  "U32_U8"},
    { pdm_cast_U32_U16, "U32_U16"},
    { pdm_cast_I32_I8,  "I32_I8"},
    { pdm_cast_I32_I16, "I32_I16"},
    { pdm_cast_R32_U4,  "R32_U4"},
    { pdm_cast_R32_U8,  "R32_U8"},
    { pdm_cast_R32_U16, "R32_U16"},
    { pdm_cast_R32_U32, "R32_U32"},
    { pdm_cast_R32_I8,  "R32_I8"},
    { pdm_cast_R32_I16, "R32_I16"},
    { pdm_cast_R32_I32, "R32_I32"},
    { pdm_cast_R32_R32, "R32_R32"},
    { pdm_cast_U4_R32,  "U4_R32"},
    { pdm_cast_U8_R32,  "U8_R32"},
    { pdm_cast_U16_R32, "U16_R32"},
    { pdm_cast_U32_R32, "U32_R32"},
    { pdm_cast_I8_R32,  "I8_R32"},
    { pdm_cast_I16_R32, "I16_R32"},
    { pdm_cast_I32_R32, "I32_R32"},
    { pdm_cast_FI2_R32, "FI2_R32"},
    { pdm_cast_FU4_R32, "FU4_R32"},
    { pdm_cast_FI8_R32, "FI8_R32"},
    { pdm_cast_R32_FI2, "R32_FI2"},
    { pdm_cast_R32_FU4, "R32_FU4"},
    { pdm_cast_R32_FI8, "R32_FI8"},
    { pdm_cast_B1_B1,   "B1_B1"},
    { pdm_cast_B2_B1,   "B2_B1"},
    { pdm_cast_BSET_B1, "BSET_B1"},
    { pdm_cast_B1_B2,   "B1_B2"},
    { NULL,             "????  "},
};

char *
cast2name(F_CAST cast)
{
    struct C2N *c;

    for (c = cast2name_tab; c->cast; c++) {
	if (c->cast == cast) {
	    return c->name;
	}
    }
    return "????  ";
}
#endif
