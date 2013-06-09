#****************************************************************/
#* 2005/ott/03,lun 12:00                 \SWISV\TCNS\makefile   */
#****************************************************************/
# $Date: 2005/10/11 09:17:34 $          $Revision: 1.15 $
# $Author: mungiguerrav $


!include    \SWISV\LIB\incfile
!include    \SWISV\LBATR\incfile
!include    \SWISV\TCNS\incfile


DDEA = A\DEA\OBJ7L
DDEV = A\DEV\OBJ7L
DSTA = A\STA\OBJ7L
DSTB = A\STB\OBJ7L


main:   chkrel  makeSchede  tcns


#-----------------------------  Ripulisce directory oggetto  ----------------------------
del:
    if exist OJINL\*.inL        del     OJINL\*.inL

    if exist $(DDEA)\*.obj      del     $(DDEA)\*.obj
    if exist $(DDEV)\*.obj      del     $(DDEV)\*.obj
    if exist $(DSTA)\*.obj      del     $(DSTA)\*.obj
    if exist $(DSTB)\*.obj      del     $(DSTB)\*.obj

    if exist $(DDEA)\*.lst      del     $(DDEA)\*.lst
    if exist $(DDEV)\*.lst      del     $(DDEV)\*.lst
    if exist $(DSTA)\*.lst      del     $(DSTA)\*.lst
    if exist $(DSTB)\*.lst      del     $(DSTB)\*.lst

    if exist $(DDEA)\*.lib      del     $(DDEA)\*.lib
    if exist $(DDEV)\*.lib      del     $(DDEV)\*.lib
    if exist $(DSTA)\*.lib      del     $(DSTA)\*.lib
    if exist $(DSTB)\*.lib      del     $(DSTB)\*.lib

    lib166 CREATE   $(DDEA)\tcns.lib
    lib166 CREATE   $(DDEV)\tcns.lib
    lib166 CREATE   $(DSTA)\tcns.lib
    lib166 CREATE   $(DSTB)\tcns.lib

#-----------------------------  Crea directory oggetto  ---------------------------------
md:
    if not exist OJINL          md      OJINL
    if not exist $(DDEA)        md      $(DDEA)
    if not exist $(DDEV)        md      $(DDEV)
    if not exist $(DSTA)        md      $(DSTA)
    if not exist $(DSTB)        md      $(DSTB)

    if exist $(DDEA)\*.lib      del     $(DDEA)\*.lib
    if exist $(DDEV)\*.lib      del     $(DDEV)\*.lib
    if exist $(DSTA)\*.lib      del     $(DSTA)\*.lib
    if exist $(DSTB)\*.lib      del     $(DSTB)\*.lib

    lib166 CREATE   $(DDEA)\tcns.lib
    lib166 CREATE   $(DDEV)\tcns.lib
    lib166 CREATE   $(DSTA)\tcns.lib
    lib166 CREATE   $(DSTB)\tcns.lib


#-----------------------------  Operazioni principali  ----------------------------------
chkrel:
    checkrel check

makeSchede:
    cd      A\DEA
    make
    cd      ..\DEV
    make
    cd      ..\STA
    make
    cd      ..\STB
    make
    cd      \SWISV\TCNS


tcns:           \
        OJINL\tcnsvrs.inL       \
        OJINL\malloc.inL        \
        OJINL\pi_rtx.inL        \
        OJINL\pi_rtx_a.inL      \
#-------------- TCN/JDP/LM
        OJINL\lm_layer.inL      \
        OJINL\lm_v_lay.inL      \
#-------------- TCN/JDP/MD
        OJINL\am_call.inL       \
        OJINL\am_direc.inL      \
        OJINL\am_group.inL      \
        OJINL\am_layer.inL      \
        OJINL\am_reply.inL      \
        OJINL\lx_param.inL      \
        OJINL\md_devic.inL      \
        OJINL\md_lists.inL      \
        OJINL\mm_pool.inL       \
        OJINL\nm_disp.inL       \
        OJINL\nm_layer.inL      \
        OJINL\nm_stadi.inL      \
        OJINL\tm_i_lay.inL      \
        OJINL\tm_m_lay.inL      \
#-------------- TCN/JDP/NMA
        OJINL\ma_main.inL       \
        OJINL\ma_mvb.inL        \
        OJINL\maclock.inL       \
        OJINL\madomain.inL      \
        OJINL\mamesse.inL       \
        OJINL\mastatio.inL      \
        OJINL\matasks.inL       \
        OJINL\mavariab.inL      \
        OJINL\mm_util.inL       \
#-------------- TCN/JDP/MAP_10
        OJINL\atr_map.inL       \
        OJINL\map_npv.inL       \
        OJINL\map_ui.inL        \
#-------------- TCN/ATR
        OJINL\atr_nma.inL       \
        OJINL\conf_tcn.inL      \
        OJINL\mvbc_red.inL      \
        OJINL\tcn_mon.inL       \
        OJINL\tcn_util.inL      \
#-------------- TCN/JDP/LC
        OJINL\lc_layer.inL      \
        OJINL\lc_intr.inL       \
        OJINL\lc_wait.inL       \
#-------------- TCN/JDP/BA
        OJINL\bav_vtab.inL      \
        OJINL\bap_poll.inL      \
        OJINL\bat_tfer.inL      \
        OJINL\bam_main.inL      \
        OJINL\bad_diag.inL      \
        OJINL\bal_link.inL      \
        OJINL\bah_hwre.inL      \
        OJINL\bac_cons.inL      \
        OJINL\baa_auxi.inL      \
#-------------- TCN/JDP/PD
        OJINL\lpa_as2c.inL      \
        OJINL\lp_indt.inL       \
        OJINL\lpc_blck.inL      \
        OJINL\lpl_lacs.inL      \
        OJINL\lpx_list.inL      \
        OJINL\lpd_extd.inL      \
        OJINL\lp_init.inL       \
        OJINL\lp_var.inL        \
        OJINL\lp_tmo.inL        \
        OJINL\lp_frce.inL       \
        OJINL\lp_clus.inL       \
        OJINL\lp_ffld.inL       \
        OJINL\apd_layr.inL


#---------------------------------   Inserimenti in tcns.lib    ---------------------------------------------
OJINL\tcnsvrs.inL:                                                                                          \
  $(DDEA)\tcnsvrs.obj  $(DDEV)\tcnsvrs.obj  $(DSTA)\tcnsvrs.obj  $(DSTB)\tcnsvrs.obj
    tcnRep  tcnsvrs
#------------------------------------------------------------------------------------------------------------
OJINL\malloc.inL:                                                                                           \
  $(DDEA)\malloc.obj   $(DDEV)\malloc.obj   $(DSTA)\malloc.obj   $(DSTB)\malloc.obj
    tcnRep  malloc
#------------------------------------------------------------------------------------------------------------
OJINL\pi_rtx.inL:                                                                                           \
  $(DDEA)\pi_rtx.obj   $(DDEV)\pi_rtx.obj   $(DSTA)\pi_rtx.obj   $(DSTB)\pi_rtx.obj
    tcnRep  pi_rtx
#------------------------------------------------------------------------------------------------------------
OJINL\pi_rtx_a.inL:                                                                                         \
  $(DDEA)\pi_rtx_a.obj $(DDEV)\pi_rtx_a.obj $(DSTA)\pi_rtx_a.obj $(DSTB)\pi_rtx_a.obj
    tcnRepA pi_rtx_a
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/LM
#------------------------------------------------------------------------------------------------------------
OJINL\lm_layer.inL:                                                                                         \
  $(DDEA)\lm_layer.obj $(DDEV)\lm_layer.obj $(DSTA)\lm_layer.obj $(DSTB)\lm_layer.obj
    tcnRep  lm_layer
#------------------------------------------------------------------------------------------------------------
OJINL\lm_v_lay.inL:                                                                                         \
  $(DDEA)\lm_v_lay.obj $(DDEV)\lm_v_lay.obj $(DSTA)\lm_v_lay.obj $(DSTB)\lm_v_lay.obj
    tcnRep  lm_v_lay
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/MD
#------------------------------------------------------------------------------------------------------------
OJINL\am_call.inL:                                                                                          \
  $(DDEA)\am_call.obj  $(DDEV)\am_call.obj  $(DSTA)\am_call.obj  $(DSTB)\am_call.obj
    tcnRep  am_call
#------------------------------------------------------------------------------------------------------------
OJINL\am_direc.inL:                                                                                         \
  $(DDEA)\am_direc.obj $(DDEV)\am_direc.obj $(DSTA)\am_direc.obj $(DSTB)\am_direc.obj
    tcnRep  am_direc
#------------------------------------------------------------------------------------------------------------
OJINL\am_group.inL:                                                                                         \
  $(DDEA)\am_group.obj $(DDEV)\am_group.obj $(DSTA)\am_group.obj $(DSTB)\am_group.obj
    tcnRep  am_group
#------------------------------------------------------------------------------------------------------------
OJINL\am_layer.inL:                                                                                         \
  $(DDEA)\am_layer.obj $(DDEV)\am_layer.obj $(DSTA)\am_layer.obj $(DSTB)\am_layer.obj
    tcnRep  am_layer
#------------------------------------------------------------------------------------------------------------
OJINL\am_reply.inL:                                                                                         \
  $(DDEA)\am_reply.obj $(DDEV)\am_reply.obj $(DSTA)\am_reply.obj $(DSTB)\am_reply.obj
    tcnRep  am_reply
#------------------------------------------------------------------------------------------------------------
OJINL\lx_param.inL:                                                                                         \
  $(DDEA)\lx_param.obj $(DDEV)\lx_param.obj $(DSTA)\lx_param.obj $(DSTB)\lx_param.obj
    tcnRep  lx_param
#------------------------------------------------------------------------------------------------------------
OJINL\md_devic.inL:                                                                                         \
  $(DDEA)\md_devic.obj $(DDEV)\md_devic.obj $(DSTA)\md_devic.obj $(DSTB)\md_devic.obj
    tcnRep  md_devic
#------------------------------------------------------------------------------------------------------------
OJINL\md_lists.inL:                                                                                         \
  $(DDEA)\md_lists.obj $(DDEV)\md_lists.obj $(DSTA)\md_lists.obj $(DSTB)\md_lists.obj
    tcnRep  md_lists
#------------------------------------------------------------------------------------------------------------
OJINL\mm_pool.inL:                                                                                          \
  $(DDEA)\mm_pool.obj  $(DDEV)\mm_pool.obj  $(DSTA)\mm_pool.obj  $(DSTB)\mm_pool.obj
    tcnRep  mm_pool
#------------------------------------------------------------------------------------------------------------
OJINL\nm_disp.inL:                                                                                          \
  $(DDEA)\nm_disp.obj  $(DDEV)\nm_disp.obj  $(DSTA)\nm_disp.obj  $(DSTB)\nm_disp.obj
    tcnRep  nm_disp
#------------------------------------------------------------------------------------------------------------
OJINL\nm_layer.inL:                                                                                         \
  $(DDEA)\nm_layer.obj $(DDEV)\nm_layer.obj $(DSTA)\nm_layer.obj $(DSTB)\nm_layer.obj
    tcnRep  nm_layer
#------------------------------------------------------------------------------------------------------------
OJINL\nm_stadi.inL:                                                                                         \
  $(DDEA)\nm_stadi.obj $(DDEV)\nm_stadi.obj $(DSTA)\nm_stadi.obj $(DSTB)\nm_stadi.obj
    tcnRep  nm_stadi
#------------------------------------------------------------------------------------------------------------
OJINL\tm_i_lay.inL:                                                                                         \
  $(DDEA)\tm_i_lay.obj $(DDEV)\tm_i_lay.obj $(DSTA)\tm_i_lay.obj $(DSTB)\tm_i_lay.obj
    tcnRep  tm_i_lay
#------------------------------------------------------------------------------------------------------------
OJINL\tm_m_lay.inL:                                                                                         \
  $(DDEA)\tm_m_lay.obj $(DDEV)\tm_m_lay.obj $(DSTA)\tm_m_lay.obj $(DSTB)\tm_m_lay.obj
    tcnRep  tm_m_lay
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/NMA
#------------------------------------------------------------------------------------------------------------
OJINL\ma_main.inL:                                                                                          \
  $(DDEA)\ma_main.obj  $(DDEV)\ma_main.obj  $(DSTA)\ma_main.obj  $(DSTB)\ma_main.obj
    tcnRep  ma_main
#------------------------------------------------------------------------------------------------------------
OJINL\ma_mvb.inL:                                                                                           \
 $(DDEA)\ma_mvb.obj    $(DDEV)\ma_mvb.obj   $(DSTA)\ma_mvb.obj   $(DSTB)\ma_mvb.obj
    tcnRep  ma_mvb
#------------------------------------------------------------------------------------------------------------
OJINL\maclock.inL:                                                                                          \
  $(DDEA)\maclock.obj  $(DDEV)\maclock.obj  $(DSTA)\maclock.obj  $(DSTB)\maclock.obj
    tcnRep  maclock
#------------------------------------------------------------------------------------------------------------
OJINL\madomain.inL:                                                                                         \
  $(DDEA)\madomain.obj $(DDEV)\madomain.obj $(DSTA)\madomain.obj $(DSTB)\madomain.obj
    tcnRep  madomain
#------------------------------------------------------------------------------------------------------------
OJINL\mamesse.inL:                                                                                          \
  $(DDEA)\mamesse.obj  $(DDEV)\mamesse.obj  $(DSTA)\mamesse.obj  $(DSTB)\mamesse.obj
    tcnRep  mamesse
#------------------------------------------------------------------------------------------------------------
OJINL\mastatio.inL:                                                                                         \
  $(DDEA)\mastatio.obj $(DDEV)\mastatio.obj $(DSTA)\mastatio.obj $(DSTB)\mastatio.obj
    tcnRep  mastatio
#------------------------------------------------------------------------------------------------------------
OJINL\matasks.inL:                                                                                          \
  $(DDEA)\matasks.obj  $(DDEV)\matasks.obj  $(DSTA)\matasks.obj  $(DSTB)\matasks.obj
    tcnRep  matasks
#------------------------------------------------------------------------------------------------------------
OJINL\mavariab.inL:                                                                                         \
  $(DDEA)\mavariab.obj $(DDEV)\mavariab.obj $(DSTA)\mavariab.obj $(DSTB)\mavariab.obj
    tcnRep  mavariab
#------------------------------------------------------------------------------------------------------------
OJINL\mm_util.inL:                                                                                          \
  $(DDEA)\mm_util.obj  $(DDEV)\mm_util.obj  $(DSTA)\mm_util.obj  $(DSTB)\mm_util.obj
    tcnRep  mm_util
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/MAP_10
#------------------------------------------------------------------------------------------------------------
OJINL\atr_map.inL:                                                                                          \
  $(DDEA)\atr_map.obj  $(DDEV)\atr_map.obj  $(DSTA)\atr_map.obj  $(DSTB)\atr_map.obj
    tcnRep  atr_map
#------------------------------------------------------------------------------------------------------------
OJINL\map_npv.inL:                                                                                          \
  $(DDEA)\map_npv.obj  $(DDEV)\map_npv.obj  $(DSTA)\map_npv.obj  $(DSTB)\map_npv.obj
    tcnRep  map_npv
#------------------------------------------------------------------------------------------------------------
OJINL\map_ui.inL:                                                                                           \
  $(DDEA)\map_ui.obj   $(DDEV)\map_ui.obj   $(DSTA)\map_ui.obj   $(DSTB)\map_ui.obj
    tcnRep  map_ui
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/ATR
#------------------------------------------------------------------------------------------------------------
OJINL\atr_nma.inL:                                                                                          \
  $(DDEA)\atr_nma.obj  $(DDEV)\atr_nma.obj  $(DSTA)\atr_nma.obj  $(DSTB)\atr_nma.obj
    tcnRep  atr_nma
#------------------------------------------------------------------------------------------------------------
OJINL\conf_tcn.inL:                                                                                         \
  $(DDEA)\conf_tcn.obj $(DDEV)\conf_tcn.obj $(DSTA)\conf_tcn.obj $(DSTB)\conf_tcn.obj
    tcnRep  conf_tcn
#------------------------------------------------------------------------------------------------------------
OJINL\mvbc_red.inL:                                                                                         \
  $(DDEA)\mvbc_red.obj $(DDEV)\mvbc_red.obj $(DSTA)\mvbc_red.obj $(DSTB)\mvbc_red.obj
    tcnRep  mvbc_red
#------------------------------------------------------------------------------------------------------------
OJINL\tcn_mon.inL:                                                                                          \
  $(DDEA)\tcn_mon.obj  $(DDEV)\tcn_mon.obj  $(DSTA)\tcn_mon.obj  $(DSTB)\tcn_mon.obj
    tcnRep  tcn_mon
#------------------------------------------------------------------------------------------------------------
OJINL\tcn_util.inL:                                                                                         \
  $(DDEA)\tcn_util.obj $(DDEV)\tcn_util.obj $(DSTA)\tcn_util.obj $(DSTB)\tcn_util.obj
    tcnRep  tcn_util
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/LC
#------------------------------------------------------------------------------------------------------------
OJINL\lc_layer.inL:                                                                                         \
  $(DDEA)\lc_layer.obj $(DDEV)\lc_layer.obj $(DSTA)\lc_layer.obj $(DSTB)\lc_layer.obj
    tcnRep  lc_layer
#------------------------------------------------------------------------------------------------------------
OJINL\lc_intr.inL:                                                                                          \
  $(DDEA)\lc_intr.obj  $(DDEV)\lc_intr.obj  $(DSTA)\lc_intr.obj  $(DSTB)\lc_intr.obj
    tcnRep  lc_intr
#------------------------------------------------------------------------------------------------------------
OJINL\lc_wait.inL:                                                                                          \
  $(DDEA)\lc_wait.obj  $(DDEV)\lc_wait.obj  $(DSTA)\lc_wait.obj  $(DSTB)\lc_wait.obj
    tcnRep  lc_wait
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/BA
#------------------------------------------------------------------------------------------------------------
OJINL\bav_vtab.inL:                                                                                         \
  $(DDEA)\bav_vtab.obj $(DDEV)\bav_vtab.obj $(DSTA)\bav_vtab.obj $(DSTB)\bav_vtab.obj
    tcnRep  bav_vtab
#------------------------------------------------------------------------------------------------------------
OJINL\bap_poll.inL:                                                                                         \
  $(DDEA)\bap_poll.obj $(DDEV)\bap_poll.obj $(DSTA)\bap_poll.obj $(DSTB)\bap_poll.obj
    tcnRep  bap_poll
#------------------------------------------------------------------------------------------------------------
OJINL\bat_tfer.inL:                                                                                         \
  $(DDEA)\bat_tfer.obj $(DDEV)\bat_tfer.obj $(DSTA)\bat_tfer.obj $(DSTB)\bat_tfer.obj
    tcnRep  bat_tfer
#------------------------------------------------------------------------------------------------------------
OJINL\bam_main.inL:                                                                                         \
  $(DDEA)\bam_main.obj $(DDEV)\bam_main.obj $(DSTA)\bam_main.obj $(DSTB)\bam_main.obj
    tcnRep  bam_main
#------------------------------------------------------------------------------------------------------------
OJINL\bad_diag.inL:                                                                                         \
  $(DDEA)\bad_diag.obj $(DDEV)\bad_diag.obj $(DSTA)\bad_diag.obj $(DSTB)\bad_diag.obj
    tcnRep  bad_diag
#------------------------------------------------------------------------------------------------------------
OJINL\bal_link.inL:                                                                                         \
  $(DDEA)\bal_link.obj $(DDEV)\bal_link.obj $(DSTA)\bal_link.obj $(DSTB)\bal_link.obj
    tcnRep  bal_link
#------------------------------------------------------------------------------------------------------------
OJINL\bah_hwre.inL:                                                                                         \
  $(DDEA)\bah_hwre.obj $(DDEV)\bah_hwre.obj $(DSTA)\bah_hwre.obj $(DSTB)\bah_hwre.obj
    tcnRep  bah_hwre
#------------------------------------------------------------------------------------------------------------
OJINL\bac_cons.inL:                                                                                         \
  $(DDEA)\bac_cons.obj $(DDEV)\bac_cons.obj $(DSTA)\bac_cons.obj $(DSTB)\bac_cons.obj
    tcnRep  bac_cons
#------------------------------------------------------------------------------------------------------------
OJINL\baa_auxi.inL:                                                                                         \
  $(DDEA)\baa_auxi.obj $(DDEV)\baa_auxi.obj $(DSTA)\baa_auxi.obj $(DSTB)\baa_auxi.obj
    tcnRep  baa_auxi
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/PD
#------------------------------------------------------------------------------------------------------------
OJINL\lpa_as2c.inL:                                                                                         \
  $(DDEA)\lpa_as2c.obj $(DDEV)\lpa_as2c.obj $(DSTA)\lpa_as2c.obj $(DSTB)\lpa_as2c.obj
    tcnRep  lpa_as2c
#------------------------------------------------------------------------------------------------------------
OJINL\lp_indt.inL:                                                                                          \
  $(DDEA)\lp_indt.obj  $(DDEV)\lp_indt.obj  $(DSTA)\lp_indt.obj  $(DSTB)\lp_indt.obj
    tcnRep  lp_indt
#------------------------------------------------------------------------------------------------------------
OJINL\lpc_blck.inL:                                                                                         \
  $(DDEA)\lpc_blck.obj $(DDEV)\lpc_blck.obj $(DSTA)\lpc_blck.obj $(DSTB)\lpc_blck.obj
    tcnRep  lpc_blck
#------------------------------------------------------------------------------------------------------------
OJINL\lpl_lacs.inL:                                                                                         \
  $(DDEA)\lpl_lacs.obj $(DDEV)\lpl_lacs.obj $(DSTA)\lpl_lacs.obj $(DSTB)\lpl_lacs.obj
    tcnRep  lpl_lacs
#------------------------------------------------------------------------------------------------------------
OJINL\lpx_list.inL:                                                                                         \
  $(DDEA)\lpx_list.obj $(DDEV)\lpx_list.obj $(DSTA)\lpx_list.obj $(DSTB)\lpx_list.obj
    tcnRep  lpx_list
#------------------------------------------------------------------------------------------------------------
OJINL\lpd_extd.inL:                                                                                         \
  $(DDEA)\lpd_extd.obj $(DDEV)\lpd_extd.obj $(DSTA)\lpd_extd.obj $(DSTB)\lpd_extd.obj
    tcnRep  lpd_extd
#------------------------------------------------------------------------------------------------------------
OJINL\lp_init.inL:                                                                                          \
  $(DDEA)\lp_init.obj  $(DDEV)\lp_init.obj  $(DSTA)\lp_init.obj  $(DSTB)\lp_init.obj
    tcnRep  lp_init
#------------------------------------------------------------------------------------------------------------
OJINL\lp_var.inL:                                                                                           \
  $(DDEA)\lp_var.obj   $(DDEV)\lp_var.obj   $(DSTA)\lp_var.obj   $(DSTB)\lp_var.obj
    tcnRep  lp_var
#------------------------------------------------------------------------------------------------------------
OJINL\lp_tmo.inL:                                                                                           \
  $(DDEA)\lp_tmo.obj   $(DDEV)\lp_tmo.obj   $(DSTA)\lp_tmo.obj   $(DSTB)\lp_tmo.obj
    tcnRep  lp_tmo
#------------------------------------------------------------------------------------------------------------
OJINL\lp_frce.inL:                                                                                          \
  $(DDEA)\lp_frce.obj  $(DDEV)\lp_frce.obj  $(DSTA)\lp_frce.obj  $(DSTB)\lp_frce.obj
    tcnRep  lp_frce
#------------------------------------------------------------------------------------------------------------
OJINL\lp_clus.inL:                                                                                          \
  $(DDEA)\lp_clus.obj  $(DDEV)\lp_clus.obj  $(DSTA)\lp_clus.obj  $(DSTB)\lp_clus.obj
    tcnRep  lp_clus
#------------------------------------------------------------------------------------------------------------
OJINL\lp_ffld.inL:                                                                                          \
  $(DDEA)\lp_ffld.obj  $(DDEV)\lp_ffld.obj  $(DSTA)\lp_ffld.obj  $(DSTB)\lp_ffld.obj
    tcnRep  lp_ffld
#------------------------------------------------------------------------------------------------------------
OJINL\apd_layr.inL:                                                                                         \
  $(DDEA)\apd_layr.obj $(DDEV)\apd_layr.obj $(DSTA)\apd_layr.obj $(DSTB)\apd_layr.obj
    tcnRep  apd_layr
#------------------------------------------------------------------------------------------------------------



#--------------------------------------    Compilazioni    --------------------------------------------------
$(DDEA)\tcnsvrs.obj  $(DDEV)\tcnsvrs.obj  $(DSTA)\tcnsvrs.obj  $(DSTB)\tcnsvrs.obj:                         \
        SRC\tcnsvrs.c   $(I_versio).h   SRC\tcnsst.inc
    uca     tcnsvrs
#------------------------------------------------------------------------------------------------------------
$(DDEA)\malloc.obj   $(DDEV)\malloc.obj   $(DSTA)\malloc.obj   $(DSTB)\malloc.obj:                          \
        SRC\malloc.c    $(I_malloc).h
    uca     malloc
#------------------------------------------------------------------------------------------------------------
$(DDEA)\pi_rtx.obj   $(DDEV)\pi_rtx.obj   $(DSTA)\pi_rtx.obj   $(DSTB)\pi_rtx.obj:                          \
        SRC\pi_rtx.c    $(I_pi_rtx).h   $(I_malloc).h   $(I_recorder).h
    uca     pi_rtx
#------------------------------------------------------------------------------------------------------------
$(DDEA)\pi_rtx_a.obj $(DDEV)\pi_rtx_a.obj $(DSTA)\pi_rtx_a.obj $(DSTB)\pi_rtx_a.obj:                        \
        SRC\pi_rtx_a.a66
    uca     pi_rtx_a
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/LM
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lm_layer.obj $(DDEV)\lm_layer.obj $(DSTA)\lm_layer.obj $(DSTB)\lm_layer.obj:                        \
        SRC\lm_layer.c  $(I_lm_layer).h
    uca     lm_layer
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lm_v_lay.obj $(DDEV)\lm_v_lay.obj $(DSTA)\lm_v_lay.obj $(DSTB)\lm_v_lay.obj:                        \
        SRC\lm_v_lay.c  $(I_basetype).h $(I_md_layer).h $(I_lm_layer).h $(I_mvbc).h     $(I_lm_head).h
    uca     lm_v_lay
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/MD
#------------------------------------------------------------------------------------------------------------
$(DDEA)\am_call.obj  $(DDEV)\am_call.obj  $(DSTA)\am_call.obj  $(DSTB)\am_call.obj:                         \
        SRC\am_call.c   $(I_nm_stadi).h $(I_pi_sys).h   $(I_md_ifce).h  $(I_am_sys).h   $(I_basetype).h     \
                        $(I_md_layer).h $(I_nm_layer).h $(I_tm_layer).h $(I_am_sub).h
    uca     am_call
#------------------------------------------------------------------------------------------------------------
$(DDEA)\am_direc.obj $(DDEV)\am_direc.obj $(DSTA)\am_direc.obj $(DSTB)\am_direc.obj:                        \
        SRC\am_direc.c  $(I_am_sys).h
    uca     am_direc
#------------------------------------------------------------------------------------------------------------
$(DDEA)\am_group.obj $(DDEV)\am_group.obj $(DSTA)\am_group.obj $(DSTB)\am_group.obj:                        \
        SRC\am_group.c  $(I_am_sys).h   $(I_am_group).h
    uca     am_group
#------------------------------------------------------------------------------------------------------------
$(DDEA)\am_layer.obj $(DDEV)\am_layer.obj $(DSTA)\am_layer.obj $(DSTB)\am_layer.obj:                        \
        SRC\am_layer.c  $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h $(I_nm_layer).h $(I_tm_layer).h     \
                        $(I_pi_sys).h
    uca     am_layer
#------------------------------------------------------------------------------------------------------------
$(DDEA)\am_reply.obj $(DDEV)\am_reply.obj $(DSTA)\am_reply.obj $(DSTB)\am_reply.obj:                        \
        SRC\am_reply.c  $(I_pi_sys).h   $(I_md_ifce).h  $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h     \
                        $(I_nm_layer).h $(I_tm_layer).h $(I_am_sub).h
    uca     am_reply
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lx_param.obj $(DDEV)\lx_param.obj $(DSTA)\lx_param.obj $(DSTB)\lx_param.obj:                        \
        SRC\lx_param.c  $(I_basetype).h $(I_md_layer).h $(I_lm_layer).h $(I_am_sys).h   $(I_nm_disp).h      \
                        $(I_md_ifce).h
    uca     lx_param
#------------------------------------------------------------------------------------------------------------
$(DDEA)\md_devic.obj $(DDEV)\md_devic.obj $(DSTA)\md_devic.obj $(DSTB)\md_devic.obj:                        \
        SRC\md_devic.c  $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h
    uca     md_devic
#------------------------------------------------------------------------------------------------------------
$(DDEA)\md_lists.obj $(DDEV)\md_lists.obj $(DSTA)\md_lists.obj $(DSTB)\md_lists.obj:                        \
        SRC\md_lists.c  $(I_pi_sys).h   $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h
    uca     md_lists
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mm_pool.obj  $(DDEV)\mm_pool.obj  $(DSTA)\mm_pool.obj  $(DSTB)\mm_pool.obj:                         \
        SRC\mm_pool.c   $(I_pi_sys).h   $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h $(I_mm_pool).h
    uca     mm_pool
#------------------------------------------------------------------------------------------------------------
$(DDEA)\nm_disp.obj  $(DDEV)\nm_disp.obj  $(DSTA)\nm_disp.obj  $(DSTB)\nm_disp.obj:                         \
        SRC\nm_disp.c   $(I_basetype).h $(I_md_layer).h $(I_nm_disp).h
    uca     nm_disp
#------------------------------------------------------------------------------------------------------------
$(DDEA)\nm_layer.obj $(DDEV)\nm_layer.obj $(DSTA)\nm_layer.obj $(DSTB)\nm_layer.obj:                        \
        SRC\nm_layer.c  $(I_md_ifce).h  $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h $(I_lx_param).h     \
                        $(I_mm_pool).h  $(I_pi_sys).h   $(I_nm_layer).h $(I_nm_disp).h  $(I_nm_stadi).h     \
                        $(I_am_group).h
    uca     nm_layer
#------------------------------------------------------------------------------------------------------------
$(DDEA)\nm_stadi.obj $(DDEV)\nm_stadi.obj $(DSTA)\nm_stadi.obj $(DSTB)\nm_stadi.obj:                        \
        SRC\nm_stadi.c  $(I_nm_stadi).h $(I_am_sys).h   $(I_pi_sys).h   $(I_md_ifce).h
    uca     nm_stadi
#------------------------------------------------------------------------------------------------------------
$(DDEA)\tm_i_lay.obj $(DDEV)\tm_i_lay.obj $(DSTA)\tm_i_lay.obj $(DSTB)\tm_i_lay.obj:                        \
        SRC\tm_i_lay.c  $(I_pi_sys).h   $(I_md_ifce).h  $(I_am_sys).h   $(I_basetype).h $(I_md_layer).h     \
                        $(I_nm_layer).h $(I_mm_pool).h  $(I_tm_layer).h $(I_tm_sub).h
    uca     tm_i_lay
#------------------------------------------------------------------------------------------------------------
$(DDEA)\tm_m_lay.obj $(DDEV)\tm_m_lay.obj $(DSTA)\tm_m_lay.obj $(DSTB)\tm_m_lay.obj:                        \
        SRC\tm_m_lay.c  $(I_traceenb).h $(I_pi_sys).h   $(I_md_ifce).h  $(I_am_sys).h   $(I_basetype).h     \
                        $(I_md_layer).h $(I_nm_layer).h $(I_mm_pool).h  $(I_tm_layer).h $(I_tm_sub).h
    uca     tm_m_lay
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/NMA
#------------------------------------------------------------------------------------------------------------
$(DDEA)\ma_main.obj  $(DDEV)\ma_main.obj  $(DSTA)\ma_main.obj  $(DSTB)\ma_main.obj:                         \
        SRC\ma_main.c   $(I_tcntypes).h $(I_am_sys).h   $(I_tnm).h
    uca     ma_main
#------------------------------------------------------------------------------------------------------------
$(DDEA)\ma_mvb.obj   $(DDEV)\ma_mvb.obj   $(DSTA)\ma_mvb.obj   $(DSTB)\ma_mvb.obj:                          \
        SRC\ma_mvb.c    $(I_tcntypes).h $(I_tnm).h      $(I_lc_sys).h   $(I_bax_incl).h
    uca     ma_mvb
#------------------------------------------------------------------------------------------------------------
$(DDEA)\maclock.obj  $(DDEV)\maclock.obj  $(DSTA)\maclock.obj  $(DSTB)\maclock.obj:                         \
        SRC\maclock.c   $(I_tcntypes).h $(I_tnm).h
    uca     maclock
#------------------------------------------------------------------------------------------------------------
$(DDEA)\madomain.obj $(DDEV)\madomain.obj $(DSTA)\madomain.obj $(DSTB)\madomain.obj:                        \
        SRC\madomain.c  $(I_tcntypes).h $(I_tnm).h
    uca     madomain
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mamesse.obj  $(DDEV)\mamesse.obj  $(DSTA)\mamesse.obj  $(DSTB)\mamesse.obj:                         \
        SRC\mamesse.c   $(I_tcntypes).h $(I_am_sys).h   $(I_am_group).h $(I_tnm).h
    uca     mamesse
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mastatio.obj $(DDEV)\mastatio.obj $(DSTA)\mastatio.obj $(DSTB)\mastatio.obj:                        \
        SRC\mastatio.c  $(I_tcntypes).h $(I_lc_sys).h   $(I_am_sys).h   $(I_tnm).h      $(I_pi_sys).h
    uca     mastatio
#------------------------------------------------------------------------------------------------------------
$(DDEA)\matasks.obj  $(DDEV)\matasks.obj  $(DSTA)\matasks.obj  $(DSTB)\matasks.obj:                         \
        SRC\matasks.c   $(I_tcntypes).h $(I_tnm).h
    uca     matasks
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mavariab.obj $(DDEV)\mavariab.obj $(DSTA)\mavariab.obj $(DSTB)\mavariab.obj:                        \
        SRC\mavariab.c  $(I_tcntypes).h $(I_tnm).h      $(I_lp_sys).h   $(I_apd_incl).h $(I_tcn_util).h
    uca     mavariab
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mm_util.obj  $(DDEV)\mm_util.obj  $(DSTA)\mm_util.obj  $(DSTB)\mm_util.obj:                         \
        SRC\mm_util.c   $(I_tcntypes).h $(I_tnm).h
    uca     mm_util
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/MAP_10
#------------------------------------------------------------------------------------------------------------
$(DDEA)\atr_map.obj  $(DDEV)\atr_map.obj  $(DSTA)\atr_map.obj  $(DSTB)\atr_map.obj:                         \
        SRC\atr_map.c   $(I_pi_sys).h   $(I_am_sys).h   $(I_map).h      $(I_c_utils).h  $(I_ushell).h
    uca     atr_map
#------------------------------------------------------------------------------------------------------------
$(DDEA)\map_npv.obj  $(DDEV)\map_npv.obj  $(DSTA)\map_npv.obj  $(DSTB)\map_npv.obj:                         \
        SRC\map_npv.c   $(I_atr_map).h  $(I_am_sys).h   $(I_pi_sys).h   $(I_map).h      $(I_map_ums).h
    uca     map_npv
#------------------------------------------------------------------------------------------------------------
$(DDEA)\map_ui.obj   $(DDEV)\map_ui.obj   $(DSTA)\map_ui.obj   $(DSTB)\map_ui.obj:                          \
        SRC\map_ui.c    $(I_atr_map).h  $(I_am_sys).h   $(I_pi_sys).h   $(I_map).h      $(I_map_ums).h      \
                        $(I_map_bmi).h
    uca     map_ui
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/ATR
#------------------------------------------------------------------------------------------------------------
$(DDEA)\atr_nma.obj  $(DDEV)\atr_nma.obj  $(DSTA)\atr_nma.obj  $(DSTB)\atr_nma.obj:                         \
        SRC\atr_nma.c   $(I_atr_nma).h  $(I_pi_sys).h   $(I_atr_hw).h   $(I_conf_tcn).h $(I_sio).h          \
                        $(I_ushell).h   $(I_xmodem).h
    uca     atr_nma
#------------------------------------------------------------------------------------------------------------
$(DDEA)\conf_tcn.obj $(DDEV)\conf_tcn.obj $(DSTA)\conf_tcn.obj $(DSTB)\conf_tcn.obj:                        \
        SRC\conf_tcn.c  $(I_conf_tcn).h $(I_recorder).h $(I_c_utils).h  $(I_tcn_util).h
    uca     conf_tcn
#------------------------------------------------------------------------------------------------------------
$(DDEA)\mvbc_red.obj $(DDEV)\mvbc_red.obj $(DSTA)\mvbc_red.obj $(DSTB)\mvbc_red.obj:                        \
        SRC\mvbc_red.c  $(I_pi_sys).h   $(I_lc_sys).h   $(I_lp_sys).h   $(I_mvbc).h     $(I_lpc_blck).h     \
                        $(I_lpl_4tcn).h $(I_atr_hw).h   $(I_c_utils).h  $(I_mvbc_red).h
    uca     mvbc_red
#------------------------------------------------------------------------------------------------------------
$(DDEA)\tcn_mon.obj  $(DDEV)\tcn_mon.obj  $(DSTA)\tcn_mon.obj  $(DSTB)\tcn_mon.obj:                         \
        SRC\tcn_mon.c   $(I_pi_sys).h   $(I_atr_hw).h   $(I_bax_incl).h $(I_md_layer).h $(I_sio).h          \
                        $(I_mvbc).h     $(I_lp_sys).h   $(I_apd_incl).h $(I_am_sys).h   $(I_conf_tcn).h     \
                        $(I_atr_nma).h  $(I_xmodem).h   $(I_mvbc_red).h $(I_tcn_util).h $(I_tcn_mon).h      \
                        $(I_ushell).h   $(I_c_utils).h  $(I_mvbcinc).inc $(I_bai_ifce).h
    uca     tcn_mon
#------------------------------------------------------------------------------------------------------------
$(DDEA)\tcn_util.obj $(DDEV)\tcn_util.obj $(DSTA)\tcn_util.obj $(DSTB)\tcn_util.obj:                        \
        SRC\tcn_util.c  $(I_pi_sys).h   $(I_lc_sys).h   $(I_lp_sys).h   $(I_tcn_util).h
    uca     tcn_util
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/LC
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lc_layer.obj $(DDEV)\lc_layer.obj $(DSTA)\lc_layer.obj $(DSTB)\lc_layer.obj:                        \
        SRC\lc_layer.c  $(I_lc_sys).h   $(I_mvbc).h     $(I_lc_tgt).h   $(I_lc_head).h
    uca     lc_layer
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lc_intr.obj  $(DDEV)\lc_intr.obj  $(DSTA)\lc_intr.obj  $(DSTB)\lc_intr.obj:                         \
        SRC\lc_intr.c   $(I_lc_sys).h   $(I_mvbc).h     $(I_lc_tgt).h   $(I_lc_head).h
    uca     lc_intr
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lc_wait.obj  $(DDEV)\lc_wait.obj  $(DSTA)\lc_wait.obj  $(DSTB)\lc_wait.obj:                         \
        SRC\lc_wait.c   $(I_lc_sys).h   $(I_mvbc).h     $(I_lc_tgt).h   $(I_lc_head).h
    uca     lc_wait
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/BA
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bav_vtab.obj $(DDEV)\bav_vtab.obj $(DSTA)\bav_vtab.obj $(DSTB)\bav_vtab.obj:                        \
        SRC\bav_vtab.c  $(I_bax_incl).h $(I_bam_main).h $(I_bad_diag).h
    uca     bav_vtab
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bap_poll.obj $(DDEV)\bap_poll.obj $(DSTA)\bap_poll.obj $(DSTB)\bap_poll.obj:                        \
        SRC\bap_poll.c  $(I_baa_auxi).h $(I_bac_cons).h $(I_bad_diag).h $(I_bah_hwre).h $(I_bai_ifce).h     \
                        $(I_bal_link).h $(I_bap_poll).h $(I_bax_incl).h
    uca     bap_poll
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bat_tfer.obj $(DDEV)\bat_tfer.obj $(DSTA)\bat_tfer.obj $(DSTB)\bat_tfer.obj:                        \
        SRC\bat_tfer.c  $(I_baa_auxi).h $(I_bad_diag).h $(I_bah_hwre).h $(I_bai_ifce).h $(I_bal_link).h     \
                        $(I_bam_main).h $(I_bat_tfer).h $(I_bax_incl).h
    uca     bat_tfer
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bam_main.obj $(DDEV)\bam_main.obj $(DSTA)\bam_main.obj $(DSTB)\bam_main.obj:                        \
        SRC\bam_main.c  $(I_pi_sys).h   $(I_baa_auxi).h $(I_bad_diag).h $(I_bai_ifce).h $(I_bah_hwre).h     \
                        $(I_bah_tcfg).h $(I_bal_link).h $(I_bam_main).h $(I_bap_poll).h $(I_bat_tfer).h     \
                        $(I_bax_incl).h
    uca     bam_main
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bad_diag.obj $(DDEV)\bad_diag.obj $(DSTA)\bad_diag.obj $(DSTB)\bad_diag.obj:                        \
        SRC\bad_diag.c  $(I_bax_incl).h $(I_baa_auxi).h $(I_bad_diag).h $(I_bah_hwre).h $(I_bai_ifce).h     \
                        $(I_bal_link).h $(I_bam_main).h $(I_bap_poll).h
    uca     bad_diag
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bal_link.obj $(DDEV)\bal_link.obj $(DSTA)\bal_link.obj $(DSTB)\bal_link.obj:                        \
        SRC\bal_link.c  $(I_pi_sys).h   $(I_baa_auxi).h $(I_bad_diag).h $(I_bah_hwre).h $(I_bah_tcfg).h     \
                        $(I_bai_ifce).h $(I_bal_link).h $(I_bam_main).h $(I_bap_poll).h $(I_bat_tfer).h     \
                        $(I_bax_incl).h $(I_baf_fram).h
    uca     bal_link
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bah_hwre.obj $(DDEV)\bah_hwre.obj $(DSTA)\bah_hwre.obj $(DSTB)\bah_hwre.obj:                        \
        SRC\bah_hwre.c  $(I_lc_sys).h   $(I_baa_auxi).h $(I_bah_hwre).h $(I_bah_ebac).h $(I_bai_ifce).h     \
                        $(I_bal_link).h $(I_bax_incl).h
    uca     bah_hwre
#------------------------------------------------------------------------------------------------------------
$(DDEA)\bac_cons.obj $(DDEV)\bac_cons.obj $(DSTA)\bac_cons.obj $(DSTB)\bac_cons.obj:                        \
        SRC\bac_cons.c  $(I_baa_auxi).h $(I_bac_cons).h $(I_bap_poll).h
    uca     bac_cons
#------------------------------------------------------------------------------------------------------------
$(DDEA)\baa_auxi.obj $(DDEV)\baa_auxi.obj $(DSTA)\baa_auxi.obj $(DSTB)\baa_auxi.obj:                        \
        SRC\baa_auxi.c  $(I_baa_auxi).h $(I_bac_cons).h $(I_bai_ifce).h $(I_bam_main).h
    uca     baa_auxi
#------------------------------------------------------------------------------------------------------------
#-------------- TCN/JDP/PD
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lpa_as2c.obj $(DDEV)\lpa_as2c.obj $(DSTA)\lpa_as2c.obj $(DSTB)\lpa_as2c.obj:                        \
        SRC\lpa_as2c.c  $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpl_4tcn).h
    uca     lpa_as2c
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_indt.obj  $(DDEV)\lp_indt.obj  $(DSTA)\lp_indt.obj  $(DSTB)\lp_indt.obj:                         \
        SRC\lp_indt.c   $(I_apd_incl).h $(I_lp_sys).h
    uca     lp_indt
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lpc_blck.obj $(DDEV)\lpc_blck.obj $(DSTA)\lpc_blck.obj $(DSTB)\lpc_blck.obj:                        \
        SRC\lpc_blck.c  $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpa_as2c).h $(I_lpi_ifce).h $(I_lpl_4tcn).h     \
                        $(I_lpc_blck).h
    uca     lpc_blck
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lpl_lacs.obj $(DDEV)\lpl_lacs.obj $(DSTA)\lpl_lacs.obj $(DSTB)\lpl_lacs.obj:                        \
        SRC\lpl_lacs.c  $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpi_ifce).h $(I_lpl_4tcn).h $(I_lpa_as2c).h     \
                        $(I_lpc_blck).h $(I_lp_bits).h
    uca     lpl_lacs
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lpx_list.obj $(DDEV)\lpx_list.obj $(DSTA)\lpx_list.obj $(DSTB)\lpx_list.obj:                        \
        SRC\lpx_list.c  $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpc_blck).h $(I_lpi_ifce).h $(I_lpl_4tcn).h
    uca     lpx_list
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lpd_extd.obj $(DDEV)\lpd_extd.obj $(DSTA)\lpd_extd.obj $(DSTB)\lpd_extd.obj:                        \
        SRC\lpd_extd.c  $(I_apd_incl).h $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpc_blck).h $(I_lpl_4tcn).h     \
                        $(I_lpi_ifce).h $(I_lp_ffld).h
    uca     lpd_extd
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_init.obj  $(DDEV)\lp_init.obj  $(DSTA)\lp_init.obj  $(DSTB)\lp_init.obj:                         \
        SRC\lp_init.c   $(I_lp_sys).h   $(I_lc_sys).h  $(I_pi_sys).h    $(I_lpc_blck).h $(I_lpi_ifce).h     \
                        $(I_lpl_4tcn).h $(I_lp_tmo).h  $(I_lp_irpt).h
    uca     lp_init
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_var.obj   $(DDEV)\lp_var.obj   $(DSTA)\lp_var.obj   $(DSTB)\lp_var.obj:                          \
        SRC\lp_var.c    $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpl_4tcn).h $(I_lpc_blck).h $(I_lpi_ifce).h     \
                        $(I_lpa_as2c).h $(I_lp_ffld).h
    uca     lp_var
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_tmo.obj   $(DDEV)\lp_tmo.obj   $(DSTA)\lp_tmo.obj   $(DSTB)\lp_tmo.obj:                          \
        SRC\lp_tmo.c    $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpl_4tcn).h $(I_lpc_blck).h $(I_lpi_ifce).h     \
                        $(I_lp_tmo).h
    uca     lp_tmo
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_frce.obj  $(DDEV)\lp_frce.obj  $(DSTA)\lp_frce.obj  $(DSTB)\lp_frce.obj:                         \
        SRC\lp_frce.c   $(I_lp_sys).h   $(I_lc_sys).h   $(I_pi_sys).h   $(I_lpi_ifce).h $(I_lpl_4tcn).h     \
                        $(I_lpc_blck).h $(I_lp_ffld).h
    uca     lp_frce
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_clus.obj  $(DDEV)\lp_clus.obj  $(DSTA)\lp_clus.obj  $(DSTB)\lp_clus.obj:                         \
        SRC\lp_clus.c   $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpi_ifce).h $(I_lpl_4tcn).h $(I_lpc_blck).h     \
                        $(I_lpa_as2c).h
    uca     lp_clus
#------------------------------------------------------------------------------------------------------------
$(DDEA)\lp_ffld.obj  $(DDEV)\lp_ffld.obj  $(DSTA)\lp_ffld.obj  $(DSTB)\lp_ffld.obj:                         \
        SRC\lp_ffld.c   $(I_lp_sys).h   $(I_lpl_4tcn).h $(I_lp_ffld).h
    uca     lp_ffld
#------------------------------------------------------------------------------------------------------------
$(DDEA)\apd_layr.obj $(DDEV)\apd_layr.obj $(DSTA)\apd_layr.obj $(DSTB)\apd_layr.obj:                        \
        SRC\apd_layr.c  $(I_apd_incl).h $(I_lp_sys).h   $(I_pi_sys).h   $(I_lpc_blck).h $(I_lpi_ifce).h     \
                        $(I_lpl_4tcn).h
    uca     apd_layr
#------------------------------------------------------------------------------------------------------------


#*=*=************************************************************************
# $Log: makefile,v $
# Revision 1.15  2005/10/11 09:17:34  mungiguerrav
# 2005/ott/03,lun 12:00           Napoli      Mungi
# - Per la mancata segnalazione delle variabili $(I_xxx) vuote,
#   in ogni uso di una variabile, si aggiunge l'estensione dell'ultimo file:
#   si scoprono e correggono quindi due  (I_tnm.h)  in  (I_tnm).h
# - Si ordinano gli include dal piu' lontano al piu' vicino, in modo che
#   per eventuali ridefinizioni di variabili (cosa comunque da evitare),
#   valga la definizione piu' vicina
#
# Revision 1.14  2005/03/30 13:29:56  mungiguerrav
# 2005/mar/30,mer 14:10           Napoli      Mungi
# - In mancanza di risposte alla mail del 2005/03/08,
#   si eliminano tutti i riferimenti alla scheda STR
#
# Revision 1.13  2005/03/09 15:10:30  mungiguerrav
# 2005/mar/09,mer 14:40           Napoli      Mungi
# - Si inseriscono i test:
#     - exist         prima di cancellare gli oggetti
#     - not exist     prima di creare directory OBJxx
#
# Revision 1.12  2004/12/13 12:43:47  mungiguerrav
# 2004/dic/10,ven 14:00           Napoli      Mungi
# - Dopo una breve fase sperimentale, a cui purtroppo non ha potuto partecipare Nappo,
#   e dopo la relativa cancellazione in Comandi, si sostituisce tcnRep1  con  tcnRep
#
# Revision 1.11  2004/12/13 12:40:14  mungiguerrav
# 2004/dic/02,gio 10:00           Napoli      Mungi
# - Per consentire di distinguere il momento della compilazione da quello
#   dell'inserimento in XXX\OBJ7L\tcns.lib:
#     - Si agg.   gestione della directory  OJINL con i file *.inL, che indicano
#       per ogni oggetto l'effettivo inserimento in tcns.lib: essi sono creati da
#       tcnRep1 al momento dell'inserimento
#     - Dopo la compilazione uca *, si abolisce  l'inserimento immediato tcnRep *
#     - Per gli oggetti creati da sorgenti assembler, si utilizza tcnRepA *
#
# Revision 1.10  2004/12/13 12:29:09  mungiguerrav
# 2004/dic/01,mer 10:00           Napoli      Mungi
# - Si agg.   include incfile di TCNS, LIB,  LBATR, necessari alla gestione degli include a piu' livelli
# - Nei prerequisiti dei target oggetto si aggiungono le variabili di tipo I_xxxx,  definite negli incfile
#
# Revision 1.9  2004/11/24 19:58:19  mungiguerrav
# 2004/nov/19,ven 13:55           Napoli      Mungi
# - BugFix:   dopo prime prove con Nappo su applicativo Los Angeles, si scopre
#             la mancanza dei riferimenti per pi_rtx_a, spostato da A\XXX\SRC
#             a   SRC     (come malloc e pi_rtx)
#
# Revision 1.8  2004/11/24 19:53:05  mungiguerrav
# 2004/nov/16,mar 15:00           Napoli      Mungi
# - Si completa il raggruppamento di compilazione per righe
# - Si aboliscono tutti i riferimenti dedicati alle schede DEV, STA, STB, STR,
#   assorbiti nella righe di compilazione con target multiplo
#
# Revision 1.7  2004/11/24 19:47:43  mungiguerrav
# 2004/nov/16,mar 13:20           Napoli      Mungi
# - Si continua il raggruppamento di compilazione per righe fino a
#   TCN/JDP/PD escluso
#   (ho dovuto spezzare il lavoro in due parti per capire le posizioni dei file,
#    perché l'area TCN/JDP/PD non era indicata nei makefile precedenti)
# - Si continuano ad abolire i riferimenti nelle successive linee dedicate
#   alle schede DEV, STA, STB, STR
#
# Revision 1.6  2004/11/24 19:33:34  mungiguerrav
# 2004/nov/16,mar 10:30           Napoli      Mungi
# - Si definisce la variabile inc_atr_sys per tutti i file A\XXX\SRC\atr_sys.h
# - Si unificano in unico gruppo le variabili di include (ossia si eliminano le
#   variabili _v, _a, _b, _r che Formato aveva creato per le singole schede
#   DEv, STa, STb, STr per gestire la sola differenze dei file atr_sys.h
# - Si semplifica la definizione delle variabili di include usando SRC\
#   al posto di \swisv\tcns\src\
# - Si passa al raggruppamento per righe degli oggetti di un solo sorgente
# - La stessa riga è poi usata per il comando di compilazione: la sostituzione
#   è effettuata fino a TCN/JDP/NMA escluso
# - Sono anche aboliti i relativi riferimenti nelle successive linee dedicate
#   alle schede DEV, STA, STB, STR
#
# Revision 1.5  2004/11/24 19:15:31  mungiguerrav
# 2004/nov/15,lun 13:30           Napoli      Mungi
# - Si definiscono le variabili   DXXX        con le dir oggetto
# - Si corregge la posizione di pi_rtx.h  da   A\XXX\SRC  a   SRC
# - Si richiama il target makeSchede prima di tcns e non per ogni singola scheda
# - Si aggiungono i file malloc e pi_rtx (che prima erano nelle dir A\XXX\SRC di scheda)
# - Si eliminano i target intermedi di tcns (ossia dea dev sta stb str)
# - Si introduce batch  tcnRep   per il comando   lib166 REPLACE
# - Si provano a raggruppare gli oggetti provenienti da un unico
#   sorgente per colonne, ma poi nella compilazione si deve usare
#   il raggruppamento per riga: questo fa pensare !
#
# Revision 1.4  2004/11/24 18:42:22  mungiguerrav
# 2004/nov/12,ven 16:30           Napoli      Formato
# - Si aggiungono le variabili per le le dipendenze dai file di include di qualunque livello
# - Si inseriscono tali dipendenze per tutti i file oggetto
# - Si aggiunge la creazione dei file A\XXX\OBJ7L\tcns.lib vuoti alla fine
#   delle esecuzioni 'md' e 'del'
# - Si elimina la creazione e riempimento totale obbligato dei file A\XXX\OBJ7L\tcns.lib
#   alla fine di ogni compilazione, anche di un solo file
# - Si aggiunge il riempimento dei file A\XXX\OBJ7L\tcns.lib per ogni singolo file
#   compilato, usando il comando REPLACE (non indicato nel manuale Keil !!!)
#
# Revision 1.3  2004/10/25 14:34:50  mungiguerrav
# 2004/ott/25,lun 16:30          Napoli       Mungi
# - Si sostituiscono i riferimenti ad  hw_sta  con  hw_stb
#
# Revision 1.2  2004/09/01 14:50:19  mungiguerrav
# 2004/set/01,mer 16:45        Napoli          Mungi\Busco
# - Si crea il makefile per la compilazione e creazione automatica di tcns.lib
#   per ogni scheda registrata (DEA,DEV,STA,STB,STR)
# - Si inserisce gestione    checkrel check   per gestione tag e release tag
#   in sostituzione delle foto di libreria
#
# Revision 1.1.2.3  2004/09/01 13:39:13  mungiguerrav
# 2004/set/01,mer 15:00        Napoli          Mungi\Busco
# - Si aggiunge gestione comando checkrel
# - Si aggiunge compilazione e inserimento in tcns.lib di tcnsvrs.obj
# - Si adegua gestione  del:  a Win98 e WinXP
#
#
#
# Si inseriscono file makefile superiore e inferiore
# - Il makefile superiore gestisce    checkrel check
# - Si aboliscono i file *.obj e relative directory dalla gestione CVS
#
# Revision 1.1.2.2  2004/07/15 16:37:03  buscob
# 2004/07/15,gio 15:15     Napoli      Busco\Nappo
# - Creazione per nuova tcns con sorgenti
