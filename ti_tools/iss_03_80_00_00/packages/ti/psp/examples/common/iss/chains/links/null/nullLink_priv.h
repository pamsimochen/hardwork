/** ==================================================================
 *  @file   nullLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/null/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _NULL_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _NULL_LINK_PRIV_H_

#include <ti/psp/examples/common/iss/chains/links/nullLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>

#define NULL_LINK_OBJ_MAX   (3)

typedef struct {
    UInt32 tskId;

    VpsUtils_TskHndl tsk;

    NullLink_CreateParams createArgs;

    UInt32 receviedFrameCount;

} NullLink_Obj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
