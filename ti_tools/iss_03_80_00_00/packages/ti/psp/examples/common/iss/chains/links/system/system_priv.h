/** ==================================================================
 *  @file   system_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/system/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SYSTEM_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_PRIV_H_

#include <ti/sysbios/knl/Clock.h>
#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/examples/common/iss/chains/links/system_config.h>
#include <ti/psp/examples/common/iss/chains/links/cameraLink.h>
#include <ti/psp/examples/common/iss/chains/links/displayLink.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/examples/utility/vpsutils_tsk.h>
#include <ti/psp/examples/utility/vpsutils_buf.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

#define SYSTEM_LINK_FRAMES_PER_CH   (4)

#define SYSTEM_CMD_CREATE          (0x00000000)
#define SYSTEM_CMD_START           (0x00000001)
#define SYSTEM_CMD_STOP            (0x00000002)
#define SYSTEM_CMD_DELETE          (0x00000003)
#define SYSTEM_CMD_NEW_DATA        (0x00000004)

#define NULL_LINK_TSK_PRI                (2)
#define DUP_LINK_TSK_PRI                 (2)
#define VIDEO_SRC_LINK_TSK_PRI           (2)
#define SYSTEM_TSK_PRI                   (2)
#define DISPLAY_LINK_TSK_PRI             (4)
#define GRPX_LINK_TSK_PRI             (4)
#define SCALAR_LINK_TSK_PRI              (6)
#define DEI_LINK_TSK_PRI                 (8)
#define SW_MS_LINK_TSK_PRI               (8)
#define CAMERA_LINK_TSK_PRI             (10)
#define NSF_LINK_TSK_PRI                 (12)

#define SYSTEM_DEFAULT_TSK_STACK_SIZE    (32*KB)
#define SYSTEM_TSK_STACK_SIZE            (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define CAMERA_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define NSF_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DEI_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SCALAR_LINK_TSK_STACK_SIZE       (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define GRPX_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DISPLAY_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DISPLAY_HWMS_LINK_TSK_STACK_SIZE (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define VIDEO_SRC_LINK_TSK_STACK_SIZE    (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SW_MS_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define NULL_LINK_TSK_STACK_SIZE         (4*KB)
#define DUP_LINK_TSK_STACK_SIZE          (4*KB)

#define SYSTEM_VIP_0    (0)
#define SYSTEM_VIP_1    (1)
#define SYSTEM_VIP_MAX  (2)

/**
 * \brief Channel information
 *  Place holder to store the channel information.
 *  A destination LINK could use this information to configure itself to accept
 *  frame specified the source
 *  A Source LINK should specify the configurations of the all the channels that
 *  it will output
 *  A m2m LINK should specify the configurations of the all the channels that
 *  it will output
 */
typedef struct {
    FVID2_DataFormat dataFormat;
    Vps_VpdmaMemoryType memType;
    /** Tiled / non-tiled */
    UInt16 width;
    UInt16 height;
    UInt32 pitch[FVID2_MAX_PLANES];
    FVID2_ScanFormat scanFormat;
} System_LinkChInfo;

/**
 * \brief LINKs output queue information
 *  Specifies a place holder that describe the output information of the LINK
 */
typedef struct {
    UInt16 numCh;
    /**< No of channel that would be sent out */
    System_LinkChInfo chInfo[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Each channels configurations */

} System_LinkQueInfo;

/**
 * \brief LINKs information
 *  Specifies a place holder that describe the LINK information
 */
typedef struct {
    UInt16 numQue;
    /**< Number of output queue that a LINK supports */
    System_LinkQueInfo queInfo[SYSTEM_MAX_OUT_QUE];
    /**< Each queue configurations */
} System_LinkInfo;

typedef struct {
    Iss_CaptRtParams cameraRtParams;
    UInt32 captureChannelNum;
    UInt32 cameraChannelNum;
    UInt32 nsfChannelNum;
    UInt32 displayHwMsRepeatCount;

    Uint32 dupCount;
    FVID2_Frame *pDupOrgFrame;

} System_FrameInfo;

typedef System_LinkInfo *(*System_GetLinkInfoCb) (VpsUtils_TskHndl * pTsk);

typedef Int32(*System_LinkGetOutputFramesCb) (VpsUtils_TskHndl * pTsk,
                                              UInt16 queId,
                                              FVID2_FrameList * pFrameList);
typedef Int32(*System_LinkPutEmptyFramesCb) (VpsUtils_TskHndl * pTsk,
                                             UInt16 queId,
                                             FVID2_FrameList * pFrameList);

/**
 * \brief LINK Instance Info
 *  Each of the LINKs are expected to register with "system" with the following
 *  information.
 *  Using these links the system would form a chain with mutiple LINKs
 */
typedef struct {
    VpsUtils_TskHndl *pTsk;

    System_LinkGetOutputFramesCb linkGetFullFrames;
    /**< Function expected to be called by the LINK to get the input frames */
    System_LinkPutEmptyFramesCb linkPutEmptyFrames;
    /**< Function expected to be called by the LINK to return received frame,
         Once processed by the LINK */
    System_GetLinkInfoCb getLinkInfo;
    /**<  Function that returns the LINKs output channel configurations */
} System_LinkObj;

/**
 * \brief System Task Descriptor
 *
 */
typedef struct {
    VpsUtils_MbxHndl mbx;
    Task_Handle tsk;
    Task_FuncPtr chainsMainFunc;
    System_LinkObj linkObj[SYSTEM_LINK_ID_MAX];
    FVID2_Handle fvidDisplayCtrl;
    Vps_DcConfig displayCtrlCfg;
    FVID2_Handle systemDrvHandle;
    Vps_SystemVPllClk vpllCfg;

    FVID2_Handle hdmiHandle;

    UInt32 displayRes;
    Bool tilerEnable;

    /* locks and flags for VIP when doing reset */
    Semaphore_Handle vipLock[SYSTEM_VIP_MAX];
    Bool vipResetFlag[SYSTEM_VIP_MAX];

    volatile Bool haltExecution;

    UInt32 displayUnderflowCount[4];

} System_Obj;

extern System_Obj gSystem_obj;

/* ===================================================================
 *  @func     System_getLinkInfo                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
System_LinkInfo *System_getLinkInfo(UInt32 linkId);

/* ===================================================================
 *  @func     System_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_init();

/* ===================================================================
 *  @func     System_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_deInit();

/* ===================================================================
 *  @func     System_getLinksFullFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_getLinksFullFrames(UInt32 linkId, UInt16 queId,
                                FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     System_putLinksEmptyFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_putLinksEmptyFrames(UInt32 linkId, UInt16 queId,
                                 FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     System_sendLinkCmd                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd);

/* ===================================================================
 *  @func     System_registerLink                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_registerLink(UInt32 linkId, System_LinkObj * pTskObj);

/* ===================================================================
 *  @func     System_hdmiCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_hdmiCreate(UInt32 displayRes, Vps_PlatformBoardId boardId);

/* ===================================================================
 *  @func     System_hdmiStart                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_hdmiStart(UInt32 displayRes, Vps_PlatformBoardId boardId);

/* ===================================================================
 *  @func     System_hdmiStop                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_hdmiStop();

/* ===================================================================
 *  @func     System_hdmiDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_hdmiDelete();

/* ===================================================================
 *  @func     System_dispSetPixClk                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_dispSetPixClk();

/* ===================================================================
 *  @func     System_lockVip                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_lockVip(UInt32 vipInst);

/* ===================================================================
 *  @func     System_unlockVip                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_unlockVip(UInt32 vipInst);

/* ===================================================================
 *  @func     System_setVipResetFlag                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_setVipResetFlag(UInt32 vipInst);

/* ===================================================================
 *  @func     System_clearVipResetFlag                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Bool System_clearVipResetFlag(UInt32 vipInst);

/* ===================================================================
 *  @func     System_displayUnderflowPrint                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_displayUnderflowPrint(Bool runTimePrint, Bool clearAll);

/* ===================================================================
 *  @func     System_displayUnderflowCheck                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_displayUnderflowCheck(Bool clearAll);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
