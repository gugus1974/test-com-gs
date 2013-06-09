/*



*/
/**************************************************************************
 * filename : LL.H                                                         
 * by       : C.Leeb CHCRC C1 / M.Becker AHE/STL3                          
 * date     : 11.07.94                                                     
 *                                                                         
 * contains internals of link layer WTB V1.9                               
 *                                                                         
 **************************************************************************
 * 4.0-3  30.07.96 cs    new events MAC_BM_CloseSwitch_Req and              
 *                       BM_MAC_CloseSwitch_Con                             
 * 4.1-24 18.10.96 cs    change of Type_SaveData
 * 4.1-41 27.01.97 cs    additional consistency check for topography
 * 4.1-44 27.01.97 cs    MAC/BMI/TBM_ERROR now as DIAG_xxx_ERR in diag.h 
 **************************************************************************/
#ifndef LL_H
#define LL_H
/* #define  TRUE    0x01        4.0 */
/* #define  FALSE   0x00        4.0 */
/* messagenumbers for MAC, BM, TRP, TMR, ACC, MMI, MSI */
/*
 structure of symbolic messagenames:   M_DST_SRC_TYPE
                     
                    M_     message
                    DST_   destination
                    SRC_   source
                    TYPE   message type

 used abbreviations :
                    BM     bus manager
                    BMI    bus management interface
                    MAC    medium access control
                    MMI    MAC-messenger interface
                    TRP    transport instance (messenger-interface)
    */
#define  NO_EVENT                       0x0000
#define  MAC_BM_Stop_Req                0x0001
#define  MAC_BM_StartMaster_Cmd         0x0002
#define  MAC_BM_Configure_Req           0x0003
#define  MAC_BM_Detect_Req              0x0004
#define  MAC_BM_Name_Req                0x0005
#define  MAC_BM_StartSlave_Cmd          0x0006
#define  MAC_BM_Inclusion_Req           0x0007
#define  MAC_BM_Inhibit_Req             0x0008
#define  MAC_BM_Unname_Req              0x0009
#define  MAC_BM_SetStrong_Cmd           0x000A
#define  MAC_BM_SetWeak_Cmd             0x000B
#define  MAC_BM_Sleep_Cmd               0x000C
#define  MAC_BM_CancelSleep_Cmd         0x000D
#define  MAC_BM_ChgNodeDesc_Cmd         0x000E
#define  MAC_BM_Inhibit_Cmd             0x000F
#define  MAC_BM_Allow_Cmd               0x0010
#define  MAC_BM_ChgUserReport_Cmd       0x0011
#define  MAC_BM_CloseSwitch_Req         0x0012             /*4.0-3*/
#define  BM_MAC_Stop_Con                0x0020
#define  BM_MAC_Configure_Con           0x0021
#define  BM_MAC_Detect_Con              0x0022
#define  BM_MAC_Name_Con                0x0023
#define  BM_MAC_Topo_Ind                0x0024
#define  BM_MAC_Inclusion_Con           0x0025
#define  BM_MAC_Inhibit_Con             0x0026
#define  BM_MAC_Unname_Con              0x0027
#define  BM_MAC_SlaveStop_Ind           0x0028
#define  BM_MAC_MasterStop_Ind          0x0029
#define  BM_MAC_WTBNodes_Ind            0x002A
#define  BM_MAC_MasterSMC_Ind           0x002B
#define  BM_MAC_MasterYield_Ind         0x002C
#define  BM_MAC_MasterLengthening_Ind   0x002D
#define  BM_MAC_MasterEndNodeFail_Ind   0x002E
#define  BM_MAC_CloseSwitch_Con         0x002F             /*4.0-3*/
/* signals from DRV to MAC */
#define  UDF_DIR1                       0x0030
#define  UDF_DIR2                       0x0031
#define  SC_DIR12                       0x0032
#define  UDF_TIMER1                     0x0034
#define  UDF_TIMER2                     0x0035
/* BMI -> BM signals */
#define  BM_BMI_Configure               0x0041
#define  BM_BMI_SetSlave                0x0042
#define  BM_BMI_SetWeak                 0x0043
#define  BM_BMI_SetStrong               0x0044
#define  BM_BMI_StartNaming             0x0045
#define  BM_BMI_Remove                  0x0046
#define  BM_BMI_SetSleep                0x0047
#define  BM_BMI_CancelSleep             0x0048
#define  BM_BMI_ChgNodeDesc             0x0049
#define  BM_BMI_Inhibit                 0x004A
#define  BM_BMI_Allow                   0x004B
#define  BM_BMI_ChgUserReport           0x004C
/* BM -> BMI signals */
#define  BMI_BM_Report                  0x0050
/* BM -> MMI signals */
#define  MMI_BM_Report                  0x0060
#define STRONG_STRENGTH 128
#define WEAK_STRENGTH     2
#define SLAVE_STRENGTH    0
/* DetectionResult */
#define  MAC_RESULT_DET_YIELD           0x00
#define  MAC_RESULT_DET_INSIST          0x01
#define  MAC_RESULT_DET_SMC             0x02
#define  MAC_RESULT_DET_NOTHING         0x03
/* InclusionResult */
#define  MAC_RESULT_INC_INCLUDED        0x00
#define  MAC_RESULT_INC_INHIBIT         0x01
#define  MAC_RESULT_INC_DISRUPT         0x02
/* NameResult */
#define  MAC_RESULT_NAME_OK             0x00
#define  MAC_RESULT_NAME_FAILURE        0x01
#define  MAC_RESULT_NAME_YIELD          0x02
#define  MAC_RESULT_NAME_AGAIN          0x03
/* MasterStopReason */
#define  MAC_MASTER_STOP_FAILURE        0x00    /* could not distribute Topo */
#define  MAC_MASTER_STOP_POLLLISTOVF    0x01    /* no way, doesn't fit*/
/* SlaveStopReason */
#define  MAC_SLAVE_STOP_ENDNODEFAIL    0x00
#define  MAC_SLAVE_STOP_UNNAMED        0x01 
/* MasterRank */
#define  WEAK           0x00
#define  STRONG         0x01
/* Bit rate in bits/s; used in BuildPollList*/
/* #define BITRATE         1000000  /* 1 MBit/sec */
extern unsigned long BITRATE;     /* G. Carnevale 98/09/07: global variable */
/* Timeouts and BASICCYCLE Duration  in ms */
#define BASIC_CYCLE     25 
/* A == Attention flag   C == Change flag   I == Inhibit flag */
#define A_BIT    0x20
#define C_BIT    0x10
#define I_BIT    0x08
#define RI_BIT   0x10
#define ACI_MASK (~(A_BIT | C_BIT | I_BIT))
/* programmnumbers for ZAS */
#define  PNR_BMI     1  /* bus manager interface */
#define  PNR_BM      2  /* bus manager */
#define  PNR_MAC     3  /* medium access control MAC */
#define  PNR_MMI     4  /* MMI */
/* queuenumbers for ZAS */
#define  WSNR_BMI   0  /* number for bus manager interface */
#define  WSNR_BM    0  /* number for bus manager */
#define  WSNR_MAC   0  /* number for MAC */
#define  WSNR_MMI   0  /* number for MMI */
/* reserved timer numbers */
#define MAC_TIMER1   0   /* used by MAC */
#define MAC_TIMER2   1   /* used by MAC */
typedef struct
{
  unsigned short pnr ;
  unsigned short enr ;
  unsigned short p1  ;
  unsigned short p2  ;
  void *         p34 ;
} Type_ZASElement ;
typedef struct
{
#ifdef O_NVM_EEPROM    /*4.1-24*/
  unsigned short   vers;
  unsigned short   id;
#endif
  unsigned char    MT_H__YP;
  unsigned char    MT_L;
  Type_WTBStatus   WTB_status;
  Type_Topography  topo;
  unsigned char    main_direction;
  unsigned char    last_node_addr;
  unsigned char    my_str;
  unsigned short   checksum;
}
Type_SaveData;
typedef struct
{
   Type_Topography *    p_valid_topo ;
   Type_Topography      topo_a ;
   Type_Topography      topo_b ;
   UNSIGNED8            valid_topo_counter ;   /*4.1-41*/
}
Type_TopographyBuffer;
typedef struct
{
   unsigned long        basic_period_cnt ;
   unsigned char        inaug_cnt ;
   unsigned char        topo_cnt ;
   unsigned long        NoTMD ;
   unsigned long        NoRMD ;
}
Type_MACStatistic;
/* prototypes */
void tbm_init(Type_WTBStatus *,
              Type_WTBNodes  *,
              Type_TopographyBuffer *,
              Type_MACStatistic *,
              unsigned char     *) ;
void mac_init(Type_WTBStatus *,
              Type_WTBNodes  *,
              Type_MACStatistic *) ;
/* binding MMI -> BMI */
void bmi_bind_mmi             ( void (*status_indication) (int result) );
void bmi_md_status_ind        ( unsigned short);
#endif
/* END OF FILE */
