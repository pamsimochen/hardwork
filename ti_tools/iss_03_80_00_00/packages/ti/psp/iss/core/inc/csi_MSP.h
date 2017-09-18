/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file camera_xxx.h
*
* XXX: Description
*
* @path iss/drivers/drv_camera/inc
*
* @rev nn.mm
*/
/* -------------------------------------------------------------------------- */
#ifndef __NEW_CSI_MSP__H
#define __NEW_CSI_MSP__H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* User code goes here */
/* ------compilation control switches --------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/core/inc/csi2rx_api.h>
/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#define CAM_CSI_BUFF        (2)
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/* ======================================================================= */
/**
 * CAM_CSI2_LANE_POSITION_T enumeration for selection of position of the channel
 *
 * @param CAM_CSI2_NOT_USED channel is not used
 * @param CAM_CSI2_POSITION_1 channel is at lane 1
 * @param CAM_CSI2_POSITION_2 channel is at lane 2
 * @param CAM_CSI2_POSITION_3 channel is at lane 3
 * @param CAM_CSI2_POSITION_4 channel is at lane 4
 * @param CAM_CSI2_POSITION_5 channel is at lane 5
 */
/* ======================================================================= */
typedef enum
{
    CAM_CSI2_NOT_USED=0,
    CAM_CSI2_POSITION_1=1,
    CAM_CSI2_POSITION_2=2,
    CAM_CSI2_POSITION_3=3,
    CAM_CSI2_POSITION_4=4,
    CAM_CSI2_POSITION_5=5
}CAM_CSI2_LANE_POSITION_T;

/* ======================================================================= */
/**
 * CAM_CSI2_LANE_POLARITY_ORDER_T enumeration for selection of polarity order of the channel
 *
 * @param CAM_CSI2_PLUS_MINUS  +/- polarity
 * @param CAM_CSI2_MINUS_PLUS  -/+ polarity
 */
/* ======================================================================= */

typedef enum 
{
    CAM_CSI2_PLUS_MINUS=0,
    CAM_CSI2_MINUS_PLUS=1
}CAM_CSI2_LANE_POLARITY_ORDER_T;


/* ======================================================================= */
/**
 * CAM_CSI2_POWER_SWITCH_MODE_T enumeration for selection of mode of power state change of CSI2
 *
 * @param CAM_CSI2_MANUAL manually power state is changed from CSI2_ON->CSI2_ULP
 * @param CAM_CSI2_AUTO     power state changed automatically to CSI2_ULP when all the channels are idle
 */
/* ======================================================================= */

typedef enum 
{
    CAM_CSI2_MANUAL=0,
    CAM_CSI2_AUTO=1
}CAM_CSI2_POWER_SWITCH_MODE_T;

/* ======================================================================= */
/**
 * CAM_CSI2_POWER_COMMAND_T  enumeration for selection of power state of CSI2
 *
 * @param CAM_CSI2_OFF  power-off state
 * @param CAM_CSI2_ON   power-on state
 * @param CAM_CSI2_ULP  ultra-low-power state
 */
/* ======================================================================= */
typedef enum
{
    CAM_CSI2_OFF=0,
    CAM_CSI2_ON=1,
    CAM_CSI2_ULP=2	
}CAM_CSI2_POWER_COMMAND_T;

/* ======================================================================= */
/**
 * CAM_CSI2_CONTROL_FEATURE_BIT_T enumeration for selectin of status of the bit
 *
 * @param CAM_CSI2_DISABLED_BIT  disables the bit
 * @param CAM_CSI2_ENABLED_BIT	  enables the bit
 */
/* ======================================================================= */
typedef enum
{
    CAM_CSI2_DISABLED_BIT=0,
    CAM_CSI2_ENABLED_BIT=1
}CAM_CSI2_CONTROL_FEATURE_BIT_T;


typedef struct 
{
    /*Controls the reset of the complex IO-- 0:Reset active; 1:reset de-asserted */
    CAM_CSI2_CONTROL_FEATURE_BIT_T reset_ctrl;

    /* power state--0:CSI2_OFF, 1:CSI2_ON, 2:CSI2_ULP*/
    CAM_CSI2_POWER_COMMAND_T power_state; 		

    /*Automatic switch between CSI2_ULP and CSI2_ON states--0:disable; 1:enable*/
    CAM_CSI2_POWER_SWITCH_MODE_T power_mode;		

    /*data lane and clock lane polarity    0:+/- pin order; 1:-/+ pin order */		
    CAM_CSI2_LANE_POLARITY_ORDER_T data4_polarity;		
    CAM_CSI2_LANE_POLARITY_ORDER_T data3_polarity;
    CAM_CSI2_LANE_POLARITY_ORDER_T data2_polarity;
    CAM_CSI2_LANE_POLARITY_ORDER_T data1_polarity;
    CAM_CSI2_LANE_POLARITY_ORDER_T clock_polarity;	

    /*data lane and clock lane positions: 1,2,3,4,5*/
    CAM_CSI2_LANE_POSITION_T data4_position;		
    CAM_CSI2_LANE_POSITION_T data3_position;
    CAM_CSI2_LANE_POSITION_T data2_position;
    CAM_CSI2_LANE_POSITION_T data1_position;
    CAM_CSI2_LANE_POSITION_T clock_position;
}CAM_CSI_complexio_cfg;

typedef enum {
    MSP_CSI2_HOOK_LINENUM_ISR,
    MSP_CSI2_UNHOOK_LINENUM_ISR,
    MSP_CSI2_ENABLE_LINENUM_ISR,
    MSP_CSI2_DISABLE_LINENUM_ISR,
    MSP_CSI2_HOOK_FRAME_END_ISR,
    MSP_CSI2_UNHOOK_FRAME_END_ISR,
    MSP_CSI2_ENABLE_FRAMEEND_ISR,
    MSP_CSI2_DISABLE_FRAMEEND_ISR,
    MSP_CSI2_ENABLE_FRAMESTART_ISR,
    MSP_CSI2_CONFIG_PING_PONG,
    MSP_CSI2_CONFIG_ENABLE_CONTEXT,
    MSP_CSI2_CONFIG_DISABLE_CONTEXT,
    MSP_CSI2_CONFIG_RESET,
    MSP_CSI2_CONFIG_CTX,
    MSP_CSI2_CONFIG_RX,
    MSP_CSI2_CONFIG_INIT,
    MSP_CSI2_CONFIG_TRANSCODE,
    MSP_CSI2_CONFIG_INTLINENUM,
    MSP_CSI2_CONFIG_HOOK_FIFO_OVF,
    MSP_CSI2_CONFIG_UNHOOK_FIFO_OVF,
    MSP_CSI2_CONFIG_ENABLE_FIFO_OVF_ISR,
    MSP_CSI2_CONFIG_DISABLE_FIFO_OVF_ISR,
    MSP_CSI2_CONFIG_USER_PARAMS_COMPLEXIO,

    MSP_CSI2_HOOK_START_FRAME_ISR,
    MSP_CSI2_UNHOOK_START_FRAME_ISR,
    MSP_CSI2_ENABLE_START_FRAME_ISR,
    MSP_CSI2_DISABLE_START_FRAME_ISR

}MSP_CSI2CfgIndex;

typedef struct{
    IssCsi2rxIsrHandleT tIsrHandle;
    MSP_U32 nVsize;
}IssCsi2rxIsrHandleLineNumT;

typedef struct CSI2_IsrHandle{
    IssCsi2rxIsrHandleLineNumT          tCsi2IsrHdl;
    MSP_CSI2CfgIndex                    eIrqIdx;
    MSP_PTR                             pNext;
}CSI2_IsrHandle;

typedef enum{
    CAM_CSI_VALID_NO = 0<<0,
    CAM_CSI_VALID_CAOMPLEXIO = 1<<0
//TODO
}CAM_CSI_VALIDCFG;

typedef struct{
    MSP_U32 nValid;
    MSP_PTR *pPrm;
//TODO
}CAM_CSI_UserCfg;


typedef struct {
    //MSP_PTR devHandle;
    MSP_BOOL inUse;
    IssCSI2DevT     eCsiInstance;
    csi2_cfg_t      tIssCsi2Config;
    CAM_CSI_complexio_cfg tComplexIO;
    //CAM_CSI_UserCfg tUserCfg;
    CSI2_IsrHandle* pCsi2IsrHdlFirst;
    //CAM_Module*     pModulsCsi;
}MSP_CsiDev;

typedef struct{
    IssCSI_BufferIdx buff_idx;
    MSP_PTR pBuff;
}IssCsi2rxConfigPingPong;

typedef struct {
    IssCSI2DevT eCsiInstance;
}CsiCfgParams;

typedef enum{
    MSP_CSI2_GET_LATEST_BUFFER,
}MSP_CsiQueryIndexT;

typedef struct {
    MSP_PTR                     pCsiBuffs[CAM_CSI_BUFF]; //attached to csi buffers
    MSP_U32                     nextWorkBuffer;
    MSP_U32                     numWorkBuffers;
    MSP_U32                     bufferLineSizeBytes;    // Line size of buffer
}ContextBuffPrm;

typedef struct {
    ContextBuffPrm              nContextBuffPrm[CAM_CSI_NUM_CTX]; //8 csi contexts
    MSP_U32                     nContextIdx;
    //MSP_PTR                     pCsiBuffs[CAM_CSI_BUFF]; //attached to csi buffers
    MSP_U8                      numLanes;
    MSP_U32                     csi2_speed_MHzs;
    IssCsi2rxTranscodeParams_t  transcode[CAM_CSI_NUM_CTX];
    issCsi2rxCallbackT          callback; //TODO
}CAM_CSI_Configuration;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAMERA_XXX_H */

