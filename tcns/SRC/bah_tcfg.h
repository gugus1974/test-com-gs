/*==============================================================================================*/
/* ATR modifications by Giuseppe Carnevale marked with <ATR:xx>                                 */
/*                                                                                              */
/* <ATR:01> Defined BAH_TS_BASE for ATR boards                                                  */
/*==============================================================================================*/


/*
 ------------------------------------------------------------------------------
|
|    Property of  :   ABB Verkehrssysteme AG, CH-8050 ZÅrich / Switzerland
|    COPYRIGHT    :   (c) 1994 ABB Verkehrssysteme AG
|
 ------------------------------------------------------------------------------
|
|    Project      :   T C N   B U S   A D M I N I S T R A T O R
|
 ------------------------------------------------------------------------------
|
|    File Name    :   BAH_TCFG.H
|
|    Document     :
|
|    Abstract     :   Target Configuration, must be filled out for each
|                     migration separately
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
|    Vers   Dept.   YY-MM-DD  State     Name    Reference
|    ----   ------  --------  -------   ------- ---------
|      0    BATC    94-01-01  created   Marsden
|
 ------------------------------------------------------------------------------
*/

#ifndef         BAH_TCFG_H
#define         BAH_TCFG_H


/* new page
 -----------------------------------------------------------------------------
|
|   name        POINTER CONFIGURATION
|
 ------------   A B S T R A C T   --------------------------------------------

    The location of traffic store elements is required.

    An example for the first prototype hardware is given. The traffic store
    in this example is based at 0x40000. The pointer representation is
    intel segment:offset.

    Please fill in your parameters in the "else" column (no new switch!)
    Use the pointer representation understood by your compiler!

*/

#if !defined (BX_SIMU) && !defined (O_BAP)
#if    defined (O_EKR)

#define BAH_TS_BASE          0x40000000L
#elif defined (O_AIP)
#define BAH_TS_BASE          0x00D00000L
#elif defined (O_VCH)
#define BAH_TS_BASE          0x01000000L

/* <ATR:01> */
#elif defined (_ATR_SYS_)
#define BAH_TS_BASE          TM0_BASE

#else
#error fill in your traffic store map here
#endif
#endif


#endif            /* End of File */
