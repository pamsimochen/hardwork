/** ==================================================================
 *  @file   issdrv_captureApi.c
 *
 *  @path   /ti/psp/iss/drivers/capture/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>
#include <ti/psp/iss/hal/iss/bte/bte.h>



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* none */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

extern bte_regs_ovly bte_reg;


/* ========================================================================== */
/*                          Function Defination                               */
/* ========================================================================== */

Int32 IssCdrv_configureBteContext(bteHandle *handleBTE, int streamNumber, int width, int height, int format, int rotationMode)
{
    bte_config_t *contextHandle;
    int i;
    int pitch;

    GT_assert(GT_DEFAULT_MASK, handleBTE != NULL);

    GT_assert(GT_DEFAULT_MASK, streamNumber < handleBTE->contextNumber/2);

    i = streamNumber * 2;
    contextHandle = &handleBTE->bte_config[i];

    /* First context */
    if(isYUV422ILEFormat(format) == 1)
    {
        pitch = width * 2;
    }
    else
    {
        pitch = width;
    }

    contextHandle->context_end_addr = ((DIV_UP(contextHandle->context_start_addr + pitch, 16) - 1) << CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT) +
        ((height - 1) << CSL_BTE_BTE_CONTEXT_END_0_Y_SHIFT);

    if (rotationMode < 4)
    {
        if(isYUV422ILEFormat(format) == 1)
            contextHandle->context_ctrl.grid = 0x2;
        else
            contextHandle->context_ctrl.grid = 0x0;
    }
    else
    {
        contextHandle->context_ctrl.grid = 0x1;
    }

    /* 4 lines */
    contextHandle->context_ctrl.trigger_threshold = pitch * 3 / 16 + 2;

    /* Second context for YUV420 */
    if(isYUV422ILEFormat(format) == 0)
    {
        contextHandle++;

        contextHandle->context_end_addr = ((DIV_UP(contextHandle->context_start_addr + pitch, 16) - 1) << CSL_BTE_BTE_CONTEXT_END_0_X_SHIFT) +
            ((height/2 - 1) << CSL_BTE_BTE_CONTEXT_END_0_Y_SHIFT);

        if (rotationMode < 4)
        {
            contextHandle->context_ctrl.grid = 0x2;
        }
        else
        {
            contextHandle->context_ctrl.grid = 0x1;
        }

        contextHandle->context_ctrl.trigger_threshold = pitch * 3 / 16 + 2;
    }

    return 0;
}

Int32 IssCdrv_startBteContext(bte_config_t *contextHandle, uint32 tilerAddress)
{
    GT_assert(GT_DEFAULT_MASK, contextHandle != NULL);

    contextHandle->frame_buffer_addr = tilerAddress;

    contextHandle->context_ctrl.start = 1;

    GT_assert(GT_DEFAULT_MASK, bte_config_context_ctrl(contextHandle) == BTE_SUCCESS);

    return 0;
}

Int32 IssCdrv_stopBteContext(bte_config_t *contextHandle)
{
    GT_assert(GT_DEFAULT_MASK, contextHandle != NULL);

    contextHandle->context_ctrl.start = 0;
    contextHandle->context_ctrl.flush = 0;
    contextHandle->context_ctrl.stop = 1;

    GT_assert(GT_DEFAULT_MASK, bte_config_ctrl(&contextHandle->context_ctrl, contextHandle->context_num) == BTE_SUCCESS);

    contextHandle->context_ctrl.start = 0;
    contextHandle->context_ctrl.flush = 1;
    contextHandle->context_ctrl.stop = 0;

    GT_assert(GT_DEFAULT_MASK, bte_config_ctrl(&contextHandle->context_ctrl, contextHandle->context_num) == BTE_SUCCESS);

    while ((bte_reg->BTE_HL_IRQSTATUS_RAW & (CSL_BTE_BTE_HL_IRQSTATUS_RAW_IRQ_CTX0_DONE_MASK << (contextHandle->context_num))) == 0);

    return 0;
}

UInt32 IssCdrv_getBteCtxNum()
{
	return (2 << BTE_FEXT(bte_reg->BTE_HL_HWINFO, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_SHIFT, CSL_BTE_BTE_HL_HWINFO_CONTEXTS_MASK));
}

Void IssCdrv_setBteBwLimiter(UInt32 bwLimit)
{
    BTE_SET32(bte_reg->BTE_CTRL, bwLimit, CSL_BTE_BTE_CTRL_BW_LIMITER_SHIFT,
              CSL_BTE_BTE_CTRL_BW_LIMITER_MASK);
}

Void IssCdrv_setBteCtrlPosted(UInt32 enable)
{
	if (enable)
    	BTE_SET32(bte_reg->BTE_CTRL, 1, CSL_BTE_BTE_CTRL_POSTED_SHIFT,
              CSL_BTE_BTE_CTRL_POSTED_MASK);
}
