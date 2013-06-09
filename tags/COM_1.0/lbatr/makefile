#****************************************************************/
# 2004/nov/30,mar 17:35                 \SWISV\LBATR\makefile   */
#****************************************************************/
# $Date: 2004/12/10 14:25:09 $          $Revision: 1.4 $
# $Author: mungiguerrav $


!include    \SWISV\LBATR\incfile
!include    \SWISV\LIB\incfile
!include    \SWISV\TCNS\incfile


main:   chkrel  makeHw  lbatr


#-----------------------------  Ripulisce directory oggetto  ----------------------------
del:
    del     OBJ7L\*.obj
    del     A\HW\OBJ7L\*.obj

    del     OBJ7L\*.lst
    del     A\HW\OBJ7L\*.lst


#-----------------------------  Crea directory oggetto  ---------------------------------
md:
    md      OBJ7L
    md      A\HW\OBJ7L


#-----------------------------  Operazioni principali  ----------------------------------
chkrel:
    checkrel check


makeHw:
    cd      \SWISV\LBATR\A\HW
    make
    cd      \SWISV\LBATR


lbatr:  OBJ7L\atr_log.obj   OBJ7L\c_utils.obj   OBJ7L\crc.obj       OBJ7L\hw_mon.obj    \
        OBJ7L\lbatrvrs.obj  OBJ7L\xilinx.obj    OBJ7L\xmodem.obj


#-----------------------------  Compilazioni    -----------------------------------------
OBJ7L\lbatrvrs.obj:                                                                     \
        SRC\lbatrvrs.c  $(I_versio)     SRC\lbatrst.inc
    uca     lbatrvrs
#----------------------------------------------------------------------------------------
OBJ7L\atr_log.obj:                                                                      \
        SRC\atr_log.c   $(I_atr_log)    $(I_str_hw)     $(I_pi_sys)     $(I_c_utils)
    uca     atr_log
#----------------------------------------------------------------------------------------
OBJ7L\c_utils.obj:                                                                      \
        SRC\c_utils.c   $(I_c_utils)
    uca     c_utils
#----------------------------------------------------------------------------------------
OBJ7L\crc.obj:                                                                          \
        SRC\crc.c       $(I_crc)
    uca     crc
#----------------------------------------------------------------------------------------
OBJ7L\hw_mon.obj:                                                                       \
        SRC\hw_mon.c    $(I_hw_mon)     $(I_atr_hw)     $(I_pi_sys)     $(I_c_utils)    \
                        $(I_ushell)     $(I_am29f040)   $(I_crc)        $(I_strParse)
    uca     hw_mon
#----------------------------------------------------------------------------------------
OBJ7L\xilinx.obj:                                                                       \
        SRC\xilinx.c    $(I_pi_sys)     $(I_c_utils)    $(I_xilinx)
    uca     xilinx
#----------------------------------------------------------------------------------------
OBJ7L\xmodem.obj:                                                                       \
        SRC\xmodem.c    $(I_xmodem)     $(I_crc)        $(I_pi_sys)     $(I_sio)
    uca     xmodem
#----------------------------------------------------------------------------------------


#****************************************************************************/
# $Log: makefile,v $
# Revision 1.4  2004/12/10 14:25:09  mungiguerrav
# 2004/nov/30,mar 17:35           Napoli      Mungi
# - Si aggiungono      include incfile di LBATR,LIB,TCNS
#   necessari alla gestione degli include a piu' livelli
# - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,
#   definite negli incfile
#
# Revision 1.3  2004/08/31 16:32:47  mungiguerrav
# 2004/ago/31,mar 18:25        Napoli          Mungi\Busco
# - Si corregge riferimento a SWISV\LIB\SRC\versio.h
#
# Revision 1.2  2004/08/31 16:17:48  mungiguerrav
# 2004/ago/31,mar 18:15        Napoli          Mungi\Busco
# - Si effettua Merge da branch makefile
#
# Revision 1.1.2.4  2004/08/31 15:36:00  mungiguerrav
# 2004/ago/31,mar 17:35        Napoli          Mungi\Busco
# - Si abolisce secondo parametro di comando checkrel, per adeguarsi a
#   nuova gestione che legge tale TAG in lbAtrvrs.c
# - Si adegua gestione  del:  a Win98 e WinXP
#
    