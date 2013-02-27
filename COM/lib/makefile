#****************************************************************/
# 2004/dic/23,gio 16:45                 \SWISV\LIB\makefile     */
#****************************************************************/
# $Date: 2004/12/23 16:13:19 $          $Revision: 1.4 $
# $Author: mungiguerrav $


!include    \SWISV\LIB\incfile


main:   chkrel  lib


#-----------------------------  Ripulisce directory oggetto  ----------------------------
del:
    if exist OBJ7M\*.obj    del     OBJ7M\*.obj
    if exist OBJ7L\*.obj    del     OBJ7L\*.obj
    if exist OBJL\*.obj     del     OBJL\*.obj
    if exist OBJD\*.obj     del     OBJD\*.obj

    if exist OBJ7M\*.lst    del     OBJ7M\*.lst
    if exist OBJ7L\*.lst    del     OBJ7L\*.lst
#   if exist OBJL\*.lst     del     OBJL\*.lst              Borland non crea .lst
    if exist OBJD\*.lst     del     OBJD\*.lst

#-----------------------------  Crea directory oggetto  ---------------------------------
md:
    md      OBJ7M
    md      OBJ7L
    md      OBJL
    md      OBJD


#-----------------------------  Operazioni principali  ----------------------------------
chkrel:
    checkrel check

lib:    OBJ7M\libvrs.obj        OBJ7L\libvrs.obj        OBJL\libvrs.obj     \
                                OBJ7L\am29Dev.obj                           \
        OBJ7M\am29f040.obj      OBJ7L\am29f040.obj      OBJL\am29f040.obj   \
        OBJ7M\consio.obj        OBJ7L\consio.obj        OBJL\consio.obj     \
        OBJ7M\consolle.obj      OBJ7L\consolle.obj      OBJL\consolle.obj   \
        OBJ7M\cputmg.obj        OBJ7L\cputmg.obj        OBJL\cputmg.obj     \
                                OBJ7L\diag.obj                              \
        OBJ7M\diDrv.obj         OBJ7L\diDrv.obj         OBJL\diDrv.obj      \
        OBJ7M\diDrvCmd.obj      OBJ7L\diDrvCmd.obj      OBJL\diDrvCmd.obj   \
        OBJ7M\doDrv.obj         OBJ7L\doDrv.obj         OBJL\doDrv.obj      \
        OBJ7M\doDrvCmd.obj      OBJ7L\doDrvCmd.obj      OBJL\doDrvCmd.obj   \
        OBJ7M\dp8570a.obj       OBJ7L\dp8570a.obj       OBJL\dp8570a.obj    \
                                OBJ7L\dp_comm1.obj                          \
        OBJ7M\dp_comm2.obj      OBJ7L\dp_comm2.obj      OBJL\dp_comm2.obj   \
        OBJ7M\dp_mbx.obj        OBJ7L\dp_mbx.obj        OBJL\dp_mbx.obj     \
        OBJ7M\dp_ram.obj        OBJ7L\dp_ram.obj        OBJL\dp_ram.obj     \
        OBJ7M\energy.obj        OBJ7L\energy.obj        OBJL\energy.obj     \
        OBJ7M\hw16x.obj         OBJ7L\hw16x.obj                             \
                                OBJ7L\mdvDev.obj                            \
        OBJ7M\memd.obj          OBJ7L\memd.obj          OBJL\memd.obj       \
        OBJ7M\msf.obj           OBJ7L\msf.obj           OBJL\msf.obj        \
        OBJ7M\nvramDev.obj      OBJ7L\nvramDev.obj      OBJL\nvramDev.obj   \
        OBJ7M\pardb.obj         OBJ7L\pardb.obj         OBJL\pardb.obj      \
        OBJ7M\query.obj         OBJ7L\query.obj         OBJL\query.obj      \
        OBJ7M\queryCmd.obj      OBJ7L\queryCmd.obj      OBJL\queryCmd.obj   \
        OBJ7M\ramDev.obj        OBJ7L\ramDev.obj        OBJL\ramDev.obj     \
        OBJ7M\ritar.obj         OBJ7L\ritar.obj         OBJL\ritar.obj      \
        OBJ7M\rtcCmd.obj        OBJ7L\rtcCmd.obj        OBJL\rtcCmd.obj     \
                                OBJ7L\sio.obj                               \
        OBJ7M\spartan.obj       OBJ7L\spartan.obj       OBJL\spartan.obj    \
        OBJ7M\stream.obj        OBJ7L\stream.obj        OBJL\stream.obj     \
        OBJ7M\strmCmd.obj       OBJ7L\strmCmd.obj       OBJL\strmCmd.obj    \
        OBJ7M\strParse.obj      OBJ7L\strParse.obj      OBJL\strParse.obj   \
        OBJ7M\tempo.obj         OBJ7L\tempo.obj         OBJL\tempo.obj      \
        OBJ7M\timeDate.obj      OBJ7L\timeDate.obj      OBJL\timeDate.obj   \
        OBJ7M\trace.obj         OBJ7L\trace.obj         OBJL\trace.obj      \
        OBJ7M\trcbk.obj         OBJ7L\trcbk.obj         OBJL\trcbk.obj      \
                                OBJ7L\ushell.obj                            \
        OBJ7M\vardb.obj         OBJ7L\vardb.obj         OBJL\vardb.obj      \
        OBJ7M\vardbCmd.obj      OBJ7L\vardbCmd.obj      OBJL\vardbCmd.obj   \
        OBJ7M\versio.obj        OBJ7L\versio.obj        OBJL\versio.obj     \
        OBJ7M\view.obj          OBJ7L\view.obj          OBJL\view.obj       \
                                                                            \
            OBJD\dsptmg.obj                 OBJL\dsptmg.obj                 \
            OBJD\frepick.obj                OBJL\frepick.obj


#-----------------------------  Compilazioni    -----------------------------------------
OBJ7M\libvrs.obj        OBJ7L\libvrs.obj        OBJL\libvrs.obj:                        \
        SRC\libvrs.c    $(I_versio)     SRC\libst.inc
    uca     libvrs
#----------------------------------------------------------------------------------------
OBJ7L\am29Dev.obj:                                                                      \
        SRC\am29Dev.c   $(I_am29f040)   $(I_am29Dev)
    uca     am29Dev
#----------------------------------------------------------------------------------------
OBJ7M\am29f040.obj      OBJ7L\am29f040.obj      OBJL\am29f040.obj:                      \
        SRC\am29f040.c  $(I_am29f040)
    uca     am29f040
#----------------------------------------------------------------------------------------
OBJ7M\consio.obj        OBJ7L\consio.obj        OBJL\consio.obj:                        \
        SRC\consio.c    $(I_consio)     $(I_query)
    uca     consio
#----------------------------------------------------------------------------------------
OBJ7M\consolle.obj      OBJ7L\consolle.obj      OBJL\consolle.obj:                      \
        SRC\consolle.c  $(I_consolle)   $(I_query)      $(I_consio)
    uca     consolle
#----------------------------------------------------------------------------------------
OBJ7M\cputmg.obj        OBJ7L\cputmg.obj        OBJL\cputmg.obj:                        \
        SRC\cputmg.c    $(I_libisv)     $(I_consolle)   $(I_cputmg)
    uca     cputmg
#----------------------------------------------------------------------------------------
OBJ7L\diag.obj:                                                                         \
        SRC\diag.c      $(I_consolle)   $(I_stream)     $(I_query)      $(I_pi_rtx)     \
                        $(I_strt_tcn)   $(I_diag)
    uca     diag
#----------------------------------------------------------------------------------------
OBJ7M\diDrv.obj         OBJ7L\diDrv.obj         OBJL\diDrv.obj:                         \
        SRC\diDrv.c     $(I_consolle)   $(I_vardb)      $(I_diDrv)
    uca     diDrv
#----------------------------------------------------------------------------------------
OBJ7M\diDrvCmd.obj      OBJ7L\diDrvCmd.obj      OBJL\diDrvCmd.obj:                      \
        SRC\diDrvCmd.c  $(I_consolle)   $(I_diDrv)      $(I_diDrvCmd)
    uca     diDrvCmd
#----------------------------------------------------------------------------------------
OBJ7M\doDrv.obj         OBJ7L\doDrv.obj         OBJL\doDrv.obj:                         \
        SRC\doDrv.c     $(I_consolle)   $(I_vardb)      $(I_doDrv)
    uca     doDrv
#----------------------------------------------------------------------------------------
OBJ7M\doDrvCmd.obj      OBJ7L\doDrvCmd.obj      OBJL\doDrvCmd.obj:                      \
        SRC\doDrvCmd.c  $(I_consolle)   $(I_doDrv)      $(I_doDrvCmd)
    uca     doDrvCmd
#----------------------------------------------------------------------------------------
OBJ7M\dp8570a.obj       OBJ7L\dp8570a.obj       OBJL\dp8570a.obj:                       \
        SRC\dp8570a.c   $(I_dp8570a)
    uca     dp8570a
#----------------------------------------------------------------------------------------
OBJ7L\dp_comm1.obj:                                                                     \
        SRC\dp_comm1.c  $(I_dp_comm)    $(I_lp_sys)     $(I_am_sys)     $(I_pi_sys)
    uca     dp_comm1
#----------------------------------------------------------------------------------------
OBJ7M\dp_comm2.obj      OBJ7L\dp_comm2.obj      OBJL\dp_comm2.obj:                      \
        SRC\dp_comm2.c  $(I_dp_comm)
    uca     dp_comm2
#----------------------------------------------------------------------------------------
OBJ7M\dp_mbx.obj        OBJ7L\dp_mbx.obj        OBJL\dp_mbx.obj:                        \
        SRC\dp_mbx.c    $(I_dp_mbx)
    uca     dp_mbx
#----------------------------------------------------------------------------------------
OBJ7M\dp_ram.obj        OBJ7L\dp_ram.obj        OBJL\dp_ram.obj:                        \
        SRC\dp_ram.c    $(I_dp_ram)
    uca     dp_ram
#----------------------------------------------------------------------------------------
OBJ7M\energy.obj        OBJ7L\energy.obj        OBJL\energy.obj:                        \
        SRC\energy.c    $(I_energy)
    uca     energy
#----------------------------------------------------------------------------------------
OBJ7M\hw16x.obj         OBJ7L\hw16x.obj:                                                \
        SRC\hw16x.c
    uca     hw16x
#----------------------------------------------------------------------------------------
OBJ7L\mdvDev.obj:                                                                       \
        SRC\mdvDev.c    $(I_am29f040)   $(I_mdvDev)
    uca     mdvDev
#----------------------------------------------------------------------------------------
OBJ7M\memd.obj          OBJ7L\memd.obj          OBJL\memd.obj:                          \
        SRC\memd.c      $(I_consolle)
    uca     memd
#----------------------------------------------------------------------------------------
OBJ7M\msf.obj           OBJ7L\msf.obj           OBJL\msf.obj:                           \
        SRC\msf.c       $(I_msf)
    uca     msf
#----------------------------------------------------------------------------------------
OBJ7M\nvramDev.obj      OBJ7L\nvramDev.obj      OBJL\nvramDev.obj:                      \
        SRC\nvramDev.c  $(I_consolle)   $(I_nvramDev)
    uca     nvramDev
#----------------------------------------------------------------------------------------
OBJ7M\pardb.obj         OBJ7L\pardb.obj         OBJL\pardb.obj:                         \
        SRC\pardb.c     $(I_stream)     $(I_pardb)      $(I_consolle)   $(I_strParse)
    uca     pardb
#----------------------------------------------------------------------------------------
OBJ7M\query.obj         OBJ7L\query.obj         OBJL\query.obj:                         \
        SRC\query.c     $(I_query)
    uca     query
#----------------------------------------------------------------------------------------
OBJ7M\queryCmd.obj      OBJ7L\queryCmd.obj      OBJL\queryCmd.obj:                      \
        SRC\queryCmd.c  $(I_consolle)   $(I_queryCmd)   $(I_query)
    uca     queryCmd
#----------------------------------------------------------------------------------------
OBJ7M\ramDev.obj        OBJ7L\ramDev.obj        OBJL\ramDev.obj:                        \
        SRC\ramDev.c    $(I_ramDev)
    uca     ramDev
#----------------------------------------------------------------------------------------
OBJ7M\ritar.obj         OBJ7L\ritar.obj         OBJL\ritar.obj:                         \
        SRC\ritar.c     $(I_ritar)
    uca     ritar
#----------------------------------------------------------------------------------------
OBJ7M\rtcCmd.obj        OBJ7L\rtcCmd.obj        OBJL\rtcCmd.obj:                        \
        SRC\rtcCmd.c    $(I_dp8570a)    $(I_consolle)   $(I_rtcCmd)   $(I_strParse)
    uca     rtcCmd
#----------------------------------------------------------------------------------------
OBJ7L\sio.obj:                                                                          \
        SRC\sio.c       $(I_sio)        $(I_pi_sys)
    uca     sio
#----------------------------------------------------------------------------------------
OBJ7M\spartan.obj       OBJ7L\spartan.obj       OBJL\spartan.obj:                       \
        SRC\spartan.c   $(I_hw16x)      $(I_spartan)
    uca     spartan
#----------------------------------------------------------------------------------------
OBJ7M\stream.obj        OBJ7L\stream.obj        OBJL\stream.obj:                        \
        SRC\stream.c    $(i_libisv)     $(I_stream)     $(I_consolle)
    uca     stream
#----------------------------------------------------------------------------------------
OBJ7M\strmCmd.obj       OBJ7L\strmCmd.obj       OBJL\strmCmd.obj:                       \
        SRC\strmCmd.c   $(I_consolle)   $(I_stream)     $(I_strmCmd)
    uca     strmCmd
#----------------------------------------------------------------------------------------
OBJ7M\strParse.obj      OBJ7L\strParse.obj      OBJL\strParse.obj:                      \
        SRC\strParse.c  $(I_strParse)
    uca     strParse
#----------------------------------------------------------------------------------------
OBJ7M\tempo.obj         OBJ7L\tempo.obj         OBJL\tempo.obj:                         \
        SRC\tempo.c     $(I_tempo)
    uca     tempo
#----------------------------------------------------------------------------------------
OBJ7M\timeDate.obj      OBJ7L\timeDate.obj      OBJL\timeDate.obj:                      \
        SRC\timeDate.c  $(I_timeDate)
    uca     timeDate
#----------------------------------------------------------------------------------------
OBJ7M\trace.obj         OBJ7L\trace.obj         OBJL\trace.obj:                         \
        SRC\trace.c     $(I_consio)     $(I_consolle)   $(I_vardb)     $(I_trace)       \
                        $(I_stream)
    uca     trace
#----------------------------------------------------------------------------------------
OBJ7M\trcbk.obj         OBJ7L\trcbk.obj         OBJL\trcbk.obj:                         \
        SRC\trcbk.c     $(I_consolle)   $(I_vardb)     $(I_trcbk)
    uca     trcbk
#----------------------------------------------------------------------------------------
OBJ7L\ushell.obj:                                                                       \
        SRC\ushell.c    $(I_ushell)     $(I_sio)        $(I_pi_sys)     $(I_query)
    uca     ushell
#----------------------------------------------------------------------------------------
OBJ7M\vardb.obj         OBJ7L\vardb.obj         OBJL\vardb.obj:                         \
        SRC\vardb.c     $(I_vardb)      $(I_query)
    uca     vardb
#----------------------------------------------------------------------------------------
OBJ7M\vardbCmd.obj      OBJ7L\vardbCmd.obj      OBJL\vardbCmd.obj:                      \
        SRC\vardbCmd.c  $(I_consolle)   $(I_vardbCmd)   $(I_vardb)
    uca     vardbCmd
#----------------------------------------------------------------------------------------
OBJ7M\versio.obj        OBJ7L\versio.obj        OBJL\versio.obj:                        \
        SRC\versio.c    $(I_consolle)   $(I_query)      $(I_versio)
    uca     versio
#----------------------------------------------------------------------------------------
OBJ7M\view.obj          OBJ7L\view.obj          OBJL\view.obj:                          \
        SRC\view.c      $(I_consolle)   $(I_vardb)
    uca     view
#----------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------
OBJD\dsptmg.obj                 OBJL\dsptmg.obj:                                        \
        HCD\dsptmg.c    $(I_libdsp)     $(I_dsptmg)
    uca     dsptmg
#----------------------------------------------------------------------------------------
OBJD\frepick.obj                OBJL\frepick.obj:                                       \
        HCD\frepick.c   $(I_frepick)
    uca     frepick
#----------------------------------------------------------------------------------------


#****************************************************************************/
# $Log: makefile,v $
# Revision 1.4  2004/12/23 16:13:19  mungiguerrav
# 2004/dic/23,gio 16:45           Napoli      Mungi
# - Per evitare messaggi di errore, nel target 'del' si aggiunge il test    if exist  DIR\*.xxx
#
# Revision 1.3  2004/12/13 10:13:38  mungiguerrav
# 2004/nov/30,mar 13:15           Napoli      Mungi
# - Utilizzando i target multipli, si unificano le chiamate di compilazione per tutti gli
#   oggetti derivati da un solo sorgente
# - Si agg.   include incfile di LIB,  necessario alla gestione degli include a piu' livelli
# - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,  definite negli incfile
#
# Revision 1.2  2004/08/31 14:25:14  mungiguerrav
# 2004/ago/31,mar 16:25        Napoli          Mungi\Busco
# - Si effettua Merge da branch makefile
#
# Revision 1.1.2.6  2004/08/31 14:11:07  mungiguerrav
# 2004/ago/31,mar 16:10        Napoli          Mungi\Busco
# - Si abolisce secondo parametro di comando checkrel, per adeguarsi a
#   nuova gestione che legge tale TAG in libvrs.c
# - Si adegua gestione  del:  sia a Win98 che WinXP
#
# Revision 1.1.2.5  2004/07/22 16:38:00  buscob
# 2004/07/22,gio 18:05    Napoli      Busco/Mungi
# - Si agg. dipendenza dagli include di libvrs
#
# Revision 1.1.2.8  2004/07/20 17:54:15  mungiguerrav
