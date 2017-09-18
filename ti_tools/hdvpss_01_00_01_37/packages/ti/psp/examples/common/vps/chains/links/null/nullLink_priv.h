/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _NULL_LINK_PRIV_H_
#define _NULL_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/nullLink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>

#define NULL_LINK_OBJ_MAX   (3)

typedef struct
{
    UInt32 tskId;

    VpsUtils_TskHndl tsk;

    NullLink_CreateParams createArgs;

    UInt32 receviedFrameCount;

} NullLink_Obj;


#endif
