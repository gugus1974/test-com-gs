/*************************************************************************
**        Copyright (C) Siemens AG 1996 All Rights Reserved. Confidential    
**========================================================================
**
**    PROJECT::    TCN Train Communication Network
                   JDP Joint Development Project

**    MODULE::     PDM Process Data Marshalling

**    WORKFILE::   $Workfile$
**************************************************************************
**    TASK::       PDM specific data types

**************************************************************************
**    AUTHOR::     Heindel (Hei)  
**    CREATED::    29.01.96
**========================================================================
**    HISTORY::    AUTHOR::   $Author: schiavor $
                   REVISION:: $Revision: 1.1.2.1 $  
                   MODTIME::  $Date: 2006/12/01 10:40:07 $
      $Log: PDMTYPES.H,v $
      Revision 1.1.2.1  2006/12/01 10:40:07  schiavor
      01/12/2006                         Napoli                            SchiavoR
      Primo inserimento per utilizzo libreria nel GTW

**************************************************************************/

/*-------------------- General TYPEs ----------------------------------------*/
#ifndef PDMTYPES_H_
#define PDMTYPES_H_   /* define to avoid duplicate types */
#include "tcntypes.h"

#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef TRUE
#define TRUE (0 == 0)
#endif

/* date structure in configuration data base */
typedef struct {
    UNSIGNED8    prod_day;
    UNSIGNED8    prod_month;
    UNSIGNED16   prod_year;
} T_PDM_DATE;

#endif
