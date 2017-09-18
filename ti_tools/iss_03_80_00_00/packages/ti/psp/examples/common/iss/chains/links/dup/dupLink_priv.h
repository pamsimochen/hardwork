/** ==================================================================
 *  @file   dupLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/dup/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _DUP_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DUP_LINK_PRIV_H_

#include <ti/psp/examples/common/iss/chains/links/dupLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>

#define DUP_LINK_OBJ_MAX   (2)

#define DUP_LINK_MAX_FRAMES_PER_OUT_QUE		(SYSTEM_LINK_FRAMES_PER_CH*16*2)

typedef struct {
    UInt32 tskId;

    VpsUtils_TskHndl tsk;

    DupLink_CreateParams createArgs;

    UInt32 getFrameCount;
    UInt32 putFrameCount;

    System_LinkInfo *pInTskInfo;

    System_LinkInfo info;

    VpsUtils_BufHndl outFrameQue[DUP_LINK_MAX_OUT_QUE];

    FVID2_Frame frames[DUP_LINK_MAX_OUT_QUE * DUP_LINK_MAX_FRAMES_PER_OUT_QUE];

    System_FrameInfo frameInfo[DUP_LINK_MAX_OUT_QUE *
                               DUP_LINK_MAX_FRAMES_PER_OUT_QUE];

    Semaphore_Handle lock;

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList[DUP_LINK_MAX_OUT_QUE];

} DupLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
