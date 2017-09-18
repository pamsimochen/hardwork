/** ==================================================================
 *  @file   displayLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/display/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _DISPLAY_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DISPLAY_LINK_PRIV_H_

#include <ti/psp/examples/common/iss/chains/links/displayLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>
#include <ti/psp/examples/utility/vpsutils_tsk.h>
#define DISPLAY_LINK_BLANK_FRAME_CHANNEL_NUM     (0xFFFF)

#define DISPLAY_LINK_BLANK_FRAME_INIT_QUE        (6)

#define DISPLAY_LINK_OBJ_MAX (3)

#define DISPLAY_LINK_CMD_DO_DEQUE                (0x0500)

typedef struct {
    UInt32 tskId;

    VpsUtils_TskHndl tsk;

    DisplayLink_CreateParams createArgs;

    System_LinkInfo *pInTskInfo;

    System_LinkQueInfo inQueInfo;

    Semaphore_Handle lock;
    Semaphore_Handle complete;

    UInt32 curDisplayChannelNum;
    UInt32 queueCount;

    FVID2_Handle displayHndl;

    Vps_DispCreateParams displayCreateArgs;
    Vps_DispCreateStatus displayCreateStatus;
    FVID2_Format displayFormat;
    UInt32 displayInstId;

    FVID2_Frame blankFrame;

    UInt32 dequeCount;
    UInt32 cbCount;

    UInt32 totalTime;

    UInt32 startTime;
    UInt32 prevTime;
    UInt32 minCbTime;
    UInt32 maxCbTime;
    Uint32 lastCbTime;

} DisplayLink_Obj;

/* ===================================================================
 *  @func     DisplayLink_drvCreate                                               
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
Int32 DisplayLink_drvCreate(DisplayLink_Obj * pObj,
                            DisplayLink_CreateParams * pPrm);
/* ===================================================================
 *  @func     DisplayLink_drvStart                                               
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
Int32 DisplayLink_drvStart(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvProcessData                                               
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
Int32 DisplayLink_drvProcessData(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvStop                                               
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
Int32 DisplayLink_drvStop(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvDelete                                               
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
Int32 DisplayLink_drvDelete(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvLock                                               
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
Int32 DisplayLink_drvLock(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvUnlock                                               
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
Int32 DisplayLink_drvUnlock(DisplayLink_Obj * pObj);

/* ===================================================================
 *  @func     DisplayLink_drvSwitchCh                                               
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
Int32 DisplayLink_drvSwitchCh(DisplayLink_Obj * pObj, UInt32 channelNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
