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
|    File Name    :   BAF_FRAM.H
|
|    Document     :
|                     
|    Abstract     :   Functions with Hardware Access
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
|    Vers  	Dept.   YY-MM-DD  State    	Name    Reference
|    ----  	------  --------  -------  	------- ---------
|      0   	BAEI-2  93-01-18  created  	Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef 		BAF_FRAM_H
#define 		BAF_FRAM_H

#if         	defined (BX_SIMU)
#if defined (O_BAP)
#define			baf_bll_tmr1_bm_cfgd	    bal_tmr1_bm_cfgd
#define			baf_bll_tmr1_ra		    bal_tmr1_ra
#define			baf_bll_dma_empty	    bal_dma_empty
#define			baf_bll_dma_empty_dummy     bal_dma_empty_dummy
#else
#define			baf_bll_dma_empty	    bal_mvbc_amfx
#define			baf_bll_dma_empty_dummy     bal_dma_empty_dummy
#endif
#elif defined   (O_EKR)
#if   defined (O_BAP)
void		    baf_bll_tmr1_bm_cfgd	(void);
void		    baf_bll_tmr1_ra     	(void);
void		    baf_bll_dma_empty		(void);
void		    baf_bll_dma_empty_dummy	(void);
#else
#define 	    baf_bll_tmr1_bm_cfgd()      {;}
#define		    baf_bll_tmr1_ra()           {;}
#define		    baf_bll_dma_empty()         {;}
#define		    baf_bll_dma_empty_dummy()   {;}

#endif
#endif
#endif
