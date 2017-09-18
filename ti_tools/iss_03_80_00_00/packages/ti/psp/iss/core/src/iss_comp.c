/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file iss_comp.c
*
* This file contains routines for
*
* @path iss/drivers/...
*
* @rev 01.00
*/
/* --------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 25-March-2009 Georgi Hristov *! *
 * =========================================================================== */

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>
#include <ti/psp/iss/hal/iss/isp/isp.h>
#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>
#include <ti/psp/iss/hal/iss/ccp2/ccp2.h>
#include <ti/psp/iss/hal/iss/csi2/csi2.h>
#include <ti/psp/iss/core/isp_msp.h>
#include <ti/psp/iss/core/inc/isp_bsc.h>
#include "../inc/iss_drv.h"
#include "../inc/iss_comp.h"
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <ti/psp/iss/hal/iss/isp/isif/inc/isif.h>          // isif_2dlsc_sof_int_enable();

#ifdef TARGET_ZEBU
#define ISS_RSZ_BLANK         14
#else
#define ISS_RSZ_BLANK         0
#endif

#define ISS_DRV_RSZ_END_ISR         ISP_RSZ_INT_DMA        // ISP_RSZ_INT_EOF0
                                                           // //ISP_RSZ_INT_LAST_PIX
#define ISS_DRV_IPIPEIF_END_ISR     ISP_IPIPEIF_IRQ
#define ISS_DRV_H3A_END_ISR         ISP_H3A_INT
#define ISS_DRV_IPIPE_INT_BSC       ISP_IPIPE_INT_BSC
#define ISS_DRV_ISIF_VD_ISR         ISP_ISIF_INT_0
#define ISS_DRV_ISIF_2DLSC_ISR      ISP_ISIF_INT_3
#define ISS_DRV_IPIPE_INT_HST       ISP_IPIPE_INT_HST

#define ISS_DRV_DENOM               1000

extern isif_regs_ovly isif_reg;
extern csi2_regs_ovly csi2A_regs;
extern iss_regs_ovly iss_regs;
extern isp_regs_ovly isp_regs;

extern void Iss_Ar0330EnableStreaming();
extern void Iss_IMX140_EnableStreaming();

#ifdef USE_MIPI_MODE

 static csi2_ctx_cfg_t csi2ImageCtxDefaultParams = {
    CSI2_DISABLED_BIT,                                     // byteswap
    CSI2_DISABLED_BIT,                                     // generic
    1,                                                     // fec_number
    0,                                                     // count
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT,
                                                           // //EOF_EN
    CSI2_DISABLED_BIT,                                     // EOL_EN
    CSI2_DISABLED_BIT,                                     // CS_EN
    CSI2_DISABLED_BIT,                                     // COUNT_UNLOCK
    CSI2_DISABLED_BIT,                                     // VP_FORCE
    CSI2_DISABLED_BIT,                                     // LINE_MODULO

    CSI2_USER_RAW8,                                        /* pixel format of
                                                            * USER_DEFINED
                                                            * 0:RAW6; 1:RAW7;
                                                            * 2:RAW8 */
    CSI2_CHANNEL_0,                                        /* Channel
                                                            * Id:0,1,2,3 */
    CSI2_SIMPLE_DPCM,                                      /* 0:Advanced;
                                                            * 1:Simple */
    // CSI2_RAW10_EXP16, /*Data format selection.*/
    CSI2_RAW12_VP,                                            /* Data format
                                                            * selection. */

    0x0,                                                   /* alpha */
    200,                                                   /* line number */

    0x00,                                                  /* 8-bit
                                                            * interrupt-bit
                                                            * mask */

};

 static csi2_ctrl_cfg_t csi2CtrlDefaultParamsVpOnly = {
    CSI2_FIFO_6_BY_8,                                      // mflag_level
                                                           // level_h;
    CSI2_FIFO_4_BY_8,                                      // mflag_level
                                                           // level_l;
    CSI2_SIZE_16x64,                                       // burst_size
    CSI2_OCPCLK,                                           // video_clock_mode;
    CSI2_DISABLE_STREAMING,                                // streaming_mode;
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT,//
                                                           // non_posted_write;
    CSI2_ENABLED_BIT,                                      // vp_only_enable;
    CSI2_DISABLED_BIT,                                     // dbg_enable;
    CSI2_NATIVE_MIPI,                                      // endianness
                                                           // little_big_endian;

    CSI2_DISABLED_BIT,                                     // ecc_enable;
    CSI2_DISABLED_BIT                                      // secure_mode;
};

 static csi2_complexio_cfg_t csi2ComplexioCfgDefaultParams = {

    CSI2_ENABLED_BIT,                                      /* Controls the
                                                            * reset of the
                                                            * complex IO--
                                                            * 0:Reset active;
                                                            * 1:reset
                                                            * de-asserted */
    CSI2_ON,                                               /* 0:CSI2_OFF,
                                                            * 1:CSI2_ON,
                                                            * 2:CSI2_ULP */
    CSI2_AUTO,                                             /* Automatic
                                                            * switch between
                                                            * CSI2_ULP and
                                                            * CSI2_ON
                                                            * states--0:disable;
                                                            * 1:enable */

    CSI2_PLUS_MINUS,                                       /* data lane and
                                                            * clock lane
                                                            * polarity 0:+/-
                                                            * pin order;
                                                            * 1:-/+ pin order
                                                            */
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,

    CSI2_NOT_USED,                                         /* data lane and
                                                            * clock lane
                                                            * positions:
                                                            * 1,2,3,4,5 */
    CSI2_NOT_USED,
    CSI2_POSITION_3,
    CSI2_POSITION_2,
    CSI2_POSITION_1
};

 /*static csi2_timing_cfg_t csi2TimingDefaultParams = {
    CSI2_DISABLED_BIT,                                     // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ;
    0x1FFFu,                                               // stop_state_counter;

    CSI2_DISABLED_BIT,                                     // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ;
    0x1FFFu                                                // stop_state_counter;
};*/

static csi2_phy_cfg_t csi2PhysicalDefaultParams = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x0000000A,
    0x0000000B,
    0x0000000C,
    0x0000000D,
    0x0000000E,
    0x0000000F
};

#endif

static const isp_interrupt_id_t isp_interrupt_id[] = {
    ISS_DRV_RSZ_END_ISR, ISS_DRV_IPIPEIF_END_ISR, ISS_DRV_H3A_END_ISR,
        ISS_DRV_IPIPE_INT_BSC,
    ISS_DRV_ISIF_VD_ISR, ISS_DRV_ISIF_2DLSC_ISR, ISS_DRV_IPIPE_INT_HST,
        ISP_IPIPEIF_IRQ
};

static const ISP_DMA_ID_T IPIPE_DMA_REQ_LUT[] = {
    ISP_IPIPE_DPC_1, ISP_IPIPE_LAST_PIX, ISP_IPIPE_DPC_0, ISP_IPIPE_HIST,
        ISP_IPIPE_BSC
};

static ISS_INTERRUPT_ID_T iss_interrupt_id[] = {
    ISS_INTERRUPT_CCP2_IRQ8
};

iss_drv_config_t *dbg_iss_drv_config;

/* ===================================================================
 *  @func     issDrvEnableDma
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
ISP_RETURN issDrvEnableDma(IPIPE_DMA_REQ_T ipipe_dma_req)
{
    ISP_RETURN result;

    result =
        isp_common_enable_dma(IPIPE_DMA_REQ_LUT[ipipe_dma_req], ISP_DMA_ENABLE);
    return (result);
}

/* ===================================================================
 *  @func     issDrvDisableDma
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
ISP_RETURN issDrvDisableDma(IPIPE_DMA_REQ_T ipipe_dma_req)
{
    ISP_RETURN result;

    result =
        isp_common_enable_dma(IPIPE_DMA_REQ_LUT[ipipe_dma_req],
                              ISP_DMA_DISABLE);
    return (result);
}

/* ===================================================================
 *  @func     issCropIpipeLsc
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
ISP_RETURN issCropIpipeLsc(ipipe_cfg_t * ipipe_cfg,
                           ipipe_size_cfg_t * ipipe_size_cfg)
{
    ipipe_cfg->lsc_params->h_offset += ipipe_size_cfg->ipipe.sizes.h_start;
    ipipe_cfg->lsc_params->v_offset += ipipe_size_cfg->ipipe.sizes.v_start;
    return (ISP_SUCCESS);
}

/* ===================================================================
 *  @func     issDrvRegisterIspCallback
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
ISP_RETURN issDrvRegisterIspCallback(MSP_IssInterruptIdT irq_id,
                                     Interrupt_Handle_T * handle)
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (NULL == handle)
    {
        return ISP_RESOURCE_UNAVAILABLE;
    }
    retval |= isp_hook_int_handler(isp_interrupt_id[irq_id], handle);

    return retval;
}

/* ===================================================================
 *  @func     issDrvUnregisterIspCallback
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
ISP_RETURN issDrvUnregisterIspCallback(MSP_IssInterruptIdT irq_id,
                                       Interrupt_Handle_T * handle)
{
    ISP_RETURN retval = ISP_SUCCESS;

    isp_disable_interrupt(isp_interrupt_id[irq_id]);
    retval |= isp_unhook_int_handler(isp_interrupt_id[irq_id], handle);

    return retval;
}

/* ===================================================================
 *  @func     issDrvEnableIspIrq
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
ISP_RETURN issDrvEnableIspIrq(MSP_IssInterruptIdT irq_id)
{
    return isp_enable_interrupt(isp_interrupt_id[irq_id]);
}

/* ===================================================================
 *  @func     issDrvDisableIspIrq
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
ISP_RETURN issDrvDisableIspIrq(MSP_IssInterruptIdT irq_id)
{
    return isp_disable_interrupt(isp_interrupt_id[irq_id]);
}

/* ===================================================================
 *  @func     issDrvRegisterIssCallback
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
ISP_RETURN issDrvRegisterIssCallback(MSP_IssInterruptIdT irq_id,
                                     iss_callback_info_t * irq_handle)
{
    if (NULL == irq_handle)
    {
        return ISP_INVALID_INPUT;
    }
    irq_id -= MSP_ISS_DRV_ISS_IRQ_START;
    return iss_register_interrupt_handler(iss_interrupt_id[irq_id],
                                          irq_handle->cbk_func,
                                          irq_handle->arg1, irq_handle->arg2);
}

/* ===================================================================
 *  @func     issDrvUnregisterIssCallback
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
ISP_RETURN issDrvUnregisterIssCallback(MSP_IssInterruptIdT irq_id)
{
    ISP_RETURN retval = ISP_SUCCESS;

    irq_id -= MSP_ISS_DRV_ISS_IRQ_START;
    retval |= iss_disable_interrupt(iss_interrupt_id[irq_id]);
    retval |= iss_unregister_interrupt_handler(iss_interrupt_id[irq_id]);

    return retval;
}

/* ===================================================================
 *  @func     issDrvEnableIssIrq
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
ISP_RETURN issDrvEnableIssIrq(MSP_IssInterruptIdT irq_id)
{
    irq_id -= MSP_ISS_DRV_ISS_IRQ_START;
    return iss_enable_interrupt(iss_interrupt_id[irq_id]);
}

/* ===================================================================
 *  @func     issDrvDisableIssIrq
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
ISP_RETURN issDrvDisableIssIrq(MSP_IssInterruptIdT irq_id)
{
    irq_id -= MSP_ISS_DRV_ISS_IRQ_START;
    return iss_disable_interrupt(iss_interrupt_id[irq_id]);
}

/* ===================================================================
 *  @func     issDrvDisableIssModuleIrq
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
ISP_RETURN issDrvDisableIssModuleIrq(MSP_IssInterruptIdT irq_id)
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (MSP_ISS_DRV_CCP_LCM == irq_id)
    {
        retval |= CCP2_clear_irq_status(EOF_IRQ);
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvSetVdLineNumber
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
ISP_RETURN issDrvSetVdLineNumber(uint32 vd_id, uint32 line_number)
{
    switch (vd_id)
    {
        case 0:
            isif_config_vd0_line_number(line_number);
            break;
        case 1:
            isif_config_vd1_line_number(line_number);
            break;
        case 2:
            isif_config_vd2_line_number(line_number);
            break;
        default:
            return ISP_INVALID_INPUT;
    }
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issDrvInit
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
ISP_RETURN issDrvInit(void)
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval |= ccp2_init();

    retval |= ccp2_open();

    retval |= ipipeif_init();

    retval |= ipipeif_open();

    retval |= isif_init();

    retval |= isif_open();

    retval |= ipipe_init();

    retval |= ipipe_open();

    retval |= rsz_init();

    retval |= rsz_open();

    retval |= h3a_init();

    retval |= h3a_open();

    retval |= isp_common_enable_clk(ISP_BL_CLK);

    return retval;

}

/* ===================================================================
 *  @func     issDrvDeinit
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
ISP_RETURN issDrvDeinit(void)
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval |= ipipe_close();

    retval |= ipipeif_close();

    retval |= rsz_close();

    retval |= isif_close();

    retval |= ccp2_close();

    retval |= h3a_close();

    // Added to enable power standby mode for ISP
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_ENABLE);

    isp_common_disable_clk(ISP_BL_CLK);

    return retval;
}

/* ===================================================================
 *  @func     issDrvCheckMaxWidth
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
ISP_RETURN issDrvCheckMaxWidth(config_sizes_t * config_sizes)
{
    if ((config_sizes->out_size_x > RSZ_MAX_LINE_SIZE))
    {
        return ISP_INVALID_INPUT;
    }
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issDrvCalcCropDims
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
ISP_RETURN issDrvCalcCropDims(config_sizes_t * config_sizes)
{
    uint32 img_crop_height;

    uint32 img_crop_width;

    img_crop_width =
        ((config_sizes->in_size_x * ISS_CROP_NO_ZOOM) /
         config_sizes->crop_x) & ~1;
    img_crop_height =
        ((config_sizes->in_size_y * ISS_CROP_NO_ZOOM) /
         config_sizes->crop_y) & ~1;

    if ((config_sizes->out_size_x > (RSZ_UPSCALE_MAX * img_crop_width)) ||
        (config_sizes->out_size_y > (RSZ_UPSCALE_MAX * img_crop_height)) ||
        ((config_sizes->out_size_x * RSZ_DOWNSCALE_MAX) < img_crop_width) ||
        ((config_sizes->out_size_y * RSZ_DOWNSCALE_MAX) < img_crop_height))
    {
        return ISP_INVALID_INPUT;
    }
    config_sizes->in_size_x = img_crop_width;
    config_sizes->in_size_y = img_crop_height;

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     ipipe_set_sizes_ccp2_crop
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
static void ipipe_set_sizes_ccp2_crop(iss_config_params_t * config_params,
                                      ipipe_size_cfg_t * ipipe_size_cfg,
                                      uint32 fReadCcp2)
{
    uint32 img_crop_height;

    uint32 img_crop_width;

    int img_crop_center;

    uint32 blanking = 0;

    uint32 resize_ratio_x, resize_ratio_y;

    uint32 resize_ratioA, resize_ratioB;

    uint32 img_start_x, img_start_y;

    config_sizes_t *config_sizes = &config_params->config_sizes;

    img_crop_width =
        ((config_sizes->in_size_x * ISS_CROP_NO_ZOOM) /
         config_sizes->crop_x) & ~1;
    img_crop_height =
        ((config_sizes->in_size_y * ISS_CROP_NO_ZOOM) /
         config_sizes->crop_y) & ~1;

    if (fReadCcp2)
    {
        config_sizes->in_size_x += ISS_RSZ_BLANK;
        config_sizes->in_size_y += 2;
        blanking = ISS_RSZ_BLANK;
    }

    img_crop_center = ((config_sizes->in_size_x - blanking) / 2);
    img_crop_center += config_sizes->in_start_x;
    // img_crop_center += ((img_crop_center * config_sizes->in_start_x) /
    // ISS_CROP_NO_ZOOM);
    if (img_crop_center < (img_crop_width / 2))
    {
        img_start_x = 0;
    }
    else
    {
        img_start_x = (img_crop_center - (img_crop_width / 2)) & ~1;
        // if(img_start_x + img_crop_width > (config_sizes->in_size_x -
        // blanking)) {
        // img_start_x = (config_sizes->in_size_x - blanking) -
        // img_crop_width;
        // }
    }
    img_crop_center = ((config_sizes->in_size_y) / 2);
    img_crop_center += config_sizes->in_start_y;
    // img_crop_center += ((img_crop_center * config_sizes->in_start_y) /
    // ISS_CROP_NO_ZOOM);
    if (img_crop_center < (img_crop_height / 2))
    {
        img_start_y = 0;
    }
    else
    {
        img_start_y = (img_crop_center - (img_crop_height / 2)) & ~1;
        // if(img_start_y + img_crop_height > config_sizes->in_size_y) {
        // img_start_y = config_sizes->in_size_y - img_crop_height;
        // }
    }

    ipipe_size_cfg->ipipe_if.sizes.h_size = config_sizes->in_size_x;
    ipipe_size_cfg->ipipe_if.sizes.v_size = config_sizes->in_size_y;

    ipipe_size_cfg->ipipe_if.sizes.h_size_crop = img_crop_width;
    ipipe_size_cfg->ipipe_if.sizes.v_size_crop = img_crop_height;

    if (fReadCcp2)
    {
        ipipe_size_cfg->ipipe_if.sizes.h_size_crop += (ISS_RSZ_BLANK + 4);
        ipipe_size_cfg->ipipe_if.sizes.v_size_crop += 4;
    }

    ipipe_size_cfg->ipipe_if.sizes.h_start = img_start_x;
    ipipe_size_cfg->ipipe_if.sizes.v_start = img_start_y;

    ipipe_size_cfg->ipipe_if.sizes.ppln = config_sizes->in_size_ppln;

    memcpy(&ipipe_size_cfg->isif.sizes, &ipipe_size_cfg->ipipe_if.sizes,
           sizeof(in_frame_sizes_t));

    ipipe_size_cfg->isif.sizes.h_start = 0;
    ipipe_size_cfg->isif.sizes.v_start = 0;
    ipipe_size_cfg->isif.sizes.h_size = ipipe_size_cfg->isif.sizes.h_size_crop;
    ipipe_size_cfg->isif.sizes.v_size = ipipe_size_cfg->isif.sizes.v_size_crop;

    memcpy(&ipipe_size_cfg->ipipe.sizes, &ipipe_size_cfg->isif.sizes,
           sizeof(in_frame_sizes_t));

    ipipe_size_cfg->clock_div = CCP2_FRACK_CLOCK_MAX;

    if (IPIPE_OUT_FORMAT_UNUSED != config_params->out_format)
    {
        memcpy(&ipipe_size_cfg->rszA.in_sizes, &ipipe_size_cfg->isif.sizes,
               sizeof(in_frame_sizes_t));

        ipipe_size_cfg->rszA.in_sizes.h_start = 0;
        ipipe_size_cfg->rszA.in_sizes.h_size_crop = img_crop_width;
        if (fReadCcp2)
        {
            ipipe_size_cfg->rszA.in_sizes.h_start += 18;
            ipipe_size_cfg->rszA.in_sizes.h_size_crop = img_crop_width - 2;
        }
        if (IPIPE_OUT_FORMAT_BAYER != config_params->out_format)
        {
            ipipe_size_cfg->rszA.in_sizes.v_start = 2;
            ipipe_size_cfg->rszA.in_sizes.v_size_crop = img_crop_height - 4;
        }
        else
        {
            ipipe_size_cfg->rszA.in_sizes.v_start = 0;
            ipipe_size_cfg->rszA.in_sizes.v_size_crop = img_crop_height;
            ipipe_size_cfg->rszA.in_sizes.h_start = 0;
            ipipe_size_cfg->rszA.in_sizes.h_size_crop = img_crop_width;
        }

        ipipe_size_cfg->rszA.out_sizes.h_size = config_sizes->out_size_x;
        ipipe_size_cfg->rszA.out_sizes.v_size = config_sizes->out_size_y;
        ipipe_size_cfg->rszA.out_sizes.bpln = config_sizes->out_size_bpln;
        ipipe_size_cfg->rszA.outC_ppln = config_sizes->out_size_bplnC;

        resize_ratio_x =
            ipipe_size_cfg->rszA.out_sizes.h_size * ISS_DRV_DENOM /
            ipipe_size_cfg->rszA.in_sizes.h_size_crop;
        resize_ratio_y =
            ipipe_size_cfg->rszA.out_sizes.v_size * ISS_DRV_DENOM /
            ipipe_size_cfg->rszA.in_sizes.v_size_crop;

        if (resize_ratio_x < ISS_DRV_DENOM)
        {
            resize_ratio_x = ISS_DRV_DENOM;
        }
        if (resize_ratio_y < ISS_DRV_DENOM)
        {
            resize_ratio_y = ISS_DRV_DENOM;
        }
        resize_ratioA = resize_ratio_x * resize_ratio_y;
        // for formats different than ARGB8888 (4 Bytes for pixel) max. pixel
        // clock is 200MHz. Since bandwith limitation
        // for ARGB8888 max. pixel clock should be limited to 160MHz.
        if (IPIPE_OUT_FORMAT_RGB888 == config_params->out_format)
        {
            ipipe_size_cfg->clock_div =
                (uint32) ((((uint64) CCP2_FRACK_CLOCK_MAX * 160) / 200 *
                           (uint64) (ISS_DRV_DENOM * ISS_DRV_DENOM)) /
                          resize_ratioA);
        }
        else
        {
            ipipe_size_cfg->clock_div =
                (uint32) (((uint64) CCP2_FRACK_CLOCK_MAX *
                           (uint64) (ISS_DRV_DENOM * ISS_DRV_DENOM)) /
                          resize_ratioA);
        }
    }

    if (IPIPE_OUT_FORMAT_UNUSED != config_params->out_formatB)
    {
        memcpy(&ipipe_size_cfg->rszB.in_sizes, &ipipe_size_cfg->isif.sizes,
               sizeof(in_frame_sizes_t));

        ipipe_size_cfg->rszB.in_sizes.h_start = 0;
        if (fReadCcp2)
        {
            ipipe_size_cfg->rszB.in_sizes.h_start += 16;
        }
        ipipe_size_cfg->rszB.in_sizes.v_start = 2;

        ipipe_size_cfg->rszB.in_sizes.h_size_crop = img_crop_width;
        ipipe_size_cfg->rszB.in_sizes.v_size_crop = img_crop_height;

        ipipe_size_cfg->rszB.out_sizes.h_size = config_sizes->out_sizeB_x;
        ipipe_size_cfg->rszB.out_sizes.v_size = config_sizes->out_sizeB_y;
        ipipe_size_cfg->rszB.out_sizes.bpln = config_sizes->out_sizeB_bpln;
        ipipe_size_cfg->rszB.outC_ppln = config_sizes->out_sizeB_bplnC;

        resize_ratio_x =
            ipipe_size_cfg->rszB.out_sizes.h_size * ISS_DRV_DENOM /
            ipipe_size_cfg->rszB.in_sizes.h_size_crop;
        resize_ratio_y =
            ipipe_size_cfg->rszB.out_sizes.v_size * ISS_DRV_DENOM /
            ipipe_size_cfg->rszB.in_sizes.v_size_crop;
        if (resize_ratio_x < ISS_DRV_DENOM)
        {
            resize_ratio_x = ISS_DRV_DENOM;
        }
        if (resize_ratio_y < ISS_DRV_DENOM)
        {
            resize_ratio_y = ISS_DRV_DENOM;
        }
        resize_ratioB = resize_ratio_x * resize_ratio_y;

        if ((IPIPE_OUT_FORMAT_UNUSED == config_params->out_format) ||
            (resize_ratioB > resize_ratioA))
        {
            // TODO:
            // TODO second cast should be to uint64???
            // TODO what about RGB888 ?
            ipipe_size_cfg->clock_div =
                (uint32) (((uint64) CCP2_FRACK_CLOCK_MAX *
                           (uint32) (ISS_DRV_DENOM * ISS_DRV_DENOM)) /
                          resize_ratioB);
        }
    }
    // slow down CCP read speed if user request it
    // if(ipipe_size_cfg->clock_div >
    // (config_params->config_sizes.clockPerscents * CCP2_FRACK_CLOCK_MAX /
    // 1000)) {
    if (config_params->config_sizes.clockPerscents < MSP_ISS_SPEED_MAX)
    {
        ipipe_size_cfg->clock_div =
            (config_params->config_sizes.clockPerscents *
             ipipe_size_cfg->clock_div / 1000);
    }

    // TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_DRIVERS, "clock divider: %d",
    // ipipe_size_cfg->clock_div);
    config_sizes->in_size_x = img_crop_width;
    config_sizes->in_size_y = img_crop_height;

}

/* ===================================================================
 *  @func     issDrvGetDivider
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
ISP_RETURN issDrvGetDivider(iss_config_params_t * config_params)
{
    ipipe_size_cfg_t ipipe_size_cfg;                       // temp

    memset(&ipipe_size_cfg, 0, sizeof(ipipe_size_cfg));

    // here we should get divider dpending on crop sizes but no on user
    // request
    config_params->config_sizes.clockPerscents = 1000;

    ipipe_set_sizes_ccp2_crop(config_params, &ipipe_size_cfg, 1);
    config_params->config_sizes.clockPerscents =
        ipipe_size_cfg.clock_div * 1000 / CCP2_FRACK_CLOCK_MAX;
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     ipipe_set_sizes
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
static void ipipe_set_sizes(iss_config_params_t * config_params,
                            ipipe_size_cfg_t * ipipe_size_cfg, uint8 YC_C_sel)
{
    uint32 img_crop_height;

    uint32 img_crop_width;

    int img_crop_center;

    uint32 blanking = 0;

    uint32 resize_ratio_x, resize_ratio_y;

    uint32 resize_ratioA, resize_ratioB;

    img_crop_width =
        ((config_params->config_sizes.in_size_x * ISS_CROP_NO_ZOOM) /
         config_params->config_sizes.crop_x) & ~1;
    img_crop_height =
        ((config_params->config_sizes.in_size_y * ISS_CROP_NO_ZOOM) /
         config_params->config_sizes.crop_y) & ~1;
    // img_crop_width -= 16;

    ipipe_size_cfg->ipipe_if.sizes.h_size = config_params->config_sizes.in_size_x;  // 3328;//img_width;
    ipipe_size_cfg->ipipe_if.sizes.v_size = config_params->config_sizes.in_size_y;  // 2448;//img_height;

    ipipe_size_cfg->ipipe_if.sizes.h_size_crop = config_params->config_sizes.in_size_x; // 3328;//img_width;
    ipipe_size_cfg->ipipe_if.sizes.v_size_crop = config_params->config_sizes.in_size_y; // 2448;//img_height;

    ipipe_size_cfg->ipipe_if.sizes.h_start = 0;
    ipipe_size_cfg->ipipe_if.sizes.v_start = 0;

    ipipe_size_cfg->ipipe_if.sizes.ppln =
        config_params->config_sizes.in_size_ppln;

    memcpy(&ipipe_size_cfg->isif.sizes, &ipipe_size_cfg->ipipe_if.sizes,
           sizeof(in_frame_sizes_t));

    memcpy(&ipipe_size_cfg->ipipe.sizes, &ipipe_size_cfg->ipipe_if.sizes,
           sizeof(in_frame_sizes_t));

    img_crop_center = ((config_params->config_sizes.in_size_x - blanking) / 2);
    // FIXME: img_crop_center += (config_params->config_sizes.in_start_x);
    if (IPIPE_OUT_FORMAT_UNUSED != config_params->out_format)
    {
        memcpy(&ipipe_size_cfg->rszA.in_sizes, &ipipe_size_cfg->ipipe_if.sizes,
               sizeof(in_frame_sizes_t));

        if (img_crop_center < (img_crop_width / 2))
        {
            ipipe_size_cfg->rszA.in_sizes.h_start = 0;
        }
        else
        {
            ipipe_size_cfg->rszA.in_sizes.h_start =
                (img_crop_center - (img_crop_width / 2)) & ~3;
            if (ipipe_size_cfg->rszA.in_sizes.h_start + img_crop_width >
                (config_params->config_sizes.in_size_x - blanking))
            {
                ipipe_size_cfg->rszA.in_sizes.h_start =
                    (config_params->config_sizes.in_size_x - blanking) -
                    img_crop_width;
            }
        }
        if (YC_C_sel)
        {
            ipipe_size_cfg->rszA.in_sizes.h_start += 1;
            img_crop_width -= 2;
        }
        img_crop_center = ((ipipe_size_cfg->rszA.in_sizes.v_size) / 2);
        // FIXME: img_crop_center +=
        // (config_params->config_sizes.in_start_y);
        if (img_crop_center < (img_crop_height / 2))
        {
            ipipe_size_cfg->rszA.in_sizes.v_start = 2;
        }
        else
        {
            ipipe_size_cfg->rszA.in_sizes.v_start =
                (img_crop_center - (img_crop_height / 2)) & ~1;
            if (ipipe_size_cfg->rszA.in_sizes.v_start + img_crop_height >
                ipipe_size_cfg->rszA.in_sizes.v_size)
            {
                ipipe_size_cfg->rszA.in_sizes.v_start =
                    ipipe_size_cfg->rszA.in_sizes.v_size - img_crop_height;
            }
        }
        if (YC_C_sel)
        {
            ipipe_size_cfg->rszA.in_sizes.v_start += 1;
        }
        ipipe_size_cfg->rszA.in_sizes.h_size_crop = img_crop_width;
        ipipe_size_cfg->rszA.in_sizes.v_size_crop = img_crop_height;

        ipipe_size_cfg->rszA.out_sizes.h_size =
            config_params->config_sizes.out_size_x;
        ipipe_size_cfg->rszA.out_sizes.v_size =
            config_params->config_sizes.out_size_y;
        ipipe_size_cfg->rszA.out_sizes.bpln =
            config_params->config_sizes.out_size_bpln;
        ipipe_size_cfg->rszA.outC_ppln =
            config_params->config_sizes.out_size_bplnC;

    }

    if (IPIPE_OUT_FORMAT_UNUSED != config_params->out_formatB)
    {
        memcpy(&ipipe_size_cfg->rszB.in_sizes, &ipipe_size_cfg->ipipe_if.sizes,
               sizeof(in_frame_sizes_t));

        if (img_crop_center < (img_crop_width / 2))
        {
            ipipe_size_cfg->rszB.in_sizes.h_start = 0;
        }
        else
        {
            ipipe_size_cfg->rszB.in_sizes.h_start =
                (img_crop_center - (img_crop_width / 2)) & ~3;
            if (ipipe_size_cfg->rszB.in_sizes.h_start + img_crop_width >
                (config_params->config_sizes.in_size_x - blanking))
            {
                ipipe_size_cfg->rszB.in_sizes.h_start =
                    (config_params->config_sizes.in_size_x - blanking) -
                    img_crop_width;
            }
        }
        if (YC_C_sel)
        {
            ipipe_size_cfg->rszB.in_sizes.h_start += 1;
        }
        img_crop_center = ((ipipe_size_cfg->rszB.in_sizes.v_size) / 2);
        // FIXME: img_crop_center +=
        // (config_params->config_sizes.in_start_y);
        if (img_crop_center < (img_crop_height / 2))
        {
            ipipe_size_cfg->rszB.in_sizes.v_start = 2;
        }
        else
        {
            ipipe_size_cfg->rszB.in_sizes.v_start =
                (img_crop_center - (img_crop_height / 2)) & ~1;
            if (ipipe_size_cfg->rszB.in_sizes.v_start + img_crop_height >
                ipipe_size_cfg->rszB.in_sizes.v_size)
            {
                ipipe_size_cfg->rszB.in_sizes.v_start =
                    ipipe_size_cfg->rszB.in_sizes.v_size - img_crop_height;
            }
        }
        if (YC_C_sel)
        {
            ipipe_size_cfg->rszB.in_sizes.v_start += 2;
        }
        ipipe_size_cfg->rszB.in_sizes.h_size_crop = img_crop_width;
        ipipe_size_cfg->rszB.in_sizes.v_size_crop = img_crop_height;

        ipipe_size_cfg->rszB.out_sizes.h_size =
            config_params->config_sizes.out_sizeB_x;
        ipipe_size_cfg->rszB.out_sizes.v_size =
            config_params->config_sizes.out_sizeB_y;
        ipipe_size_cfg->rszB.out_sizes.bpln =
            config_params->config_sizes.out_sizeB_bpln;
        ipipe_size_cfg->rszB.outC_ppln =
            config_params->config_sizes.out_sizeB_bplnC;

        resize_ratio_x =
            ipipe_size_cfg->rszB.out_sizes.h_size * 10 /
            ipipe_size_cfg->rszB.in_sizes.h_size_crop;
        resize_ratio_y =
            ipipe_size_cfg->rszB.out_sizes.v_size * 10 /
            ipipe_size_cfg->rszB.in_sizes.v_size_crop;
        resize_ratioB = resize_ratio_x * resize_ratio_y;

        resize_ratioA =
            (ipipe_size_cfg->rszA.out_sizes.h_size * 10 /
             ipipe_size_cfg->rszA.in_sizes.h_size_crop) *
            (ipipe_size_cfg->rszA.out_sizes.v_size * 10 /
             ipipe_size_cfg->rszA.in_sizes.v_size_crop);

        if ((IPIPE_OUT_FORMAT_UNUSED == config_params->out_format) ||
            (resize_ratioB > resize_ratioA))
        {
            ipipe_size_cfg->clock_div = 100 * resize_ratioB / 100;
        }

    }
    // memcpy(&ipipe_size_cfg->rszB.in_sizes, &ipipe_size_cfg->rszA.in_sizes,
    // sizeof(in_frame_sizes_t));

    // op_rszB_img_width = (img_width/8 );
    // op_rszB_img_height = (img_height/8);

    // if(resize_ratioB > 100) {
    // ipipe_size_cfg->clock_div = ipipe_size_cfg->clock_div * resize_ratio_x
    // / 100;
    // }

    config_params->config_sizes.in_size_x = img_crop_width;
    config_params->config_sizes.in_size_y = img_crop_height;
    /*
     * if(config_params->config_sizes.out_format == IPIPE_OUT_FORMAT_YUV422)
     * { ipipe_size_cfg->rszA.format = RSZ_YUV422_OP;
     * ipipe_size_cfg->rszB.format = RSZ_YUV422_OP; } else {
     * ipipe_size_cfg->rszA.format = RSZ_YUV420_YC_OP;
     * ipipe_size_cfg->rszB.format = RSZ_YUV420_YC_OP; } */
}

/* ===================================================================
 *  @func     ipipe_set_isif_sizes
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
static void ipipe_set_isif_sizes(config_sizes_t * config_sizes,
                                 ipipe_size_cfg_t * ipipe_size_cfg)
{

    ipipe_size_cfg->ipipe_if.sizes.h_size = config_sizes->in_size_x;    // 3328;//img_width;
    ipipe_size_cfg->ipipe_if.sizes.v_size = config_sizes->in_size_y;    // 2448;//img_height;

    ipipe_size_cfg->ipipe_if.sizes.h_size_crop = config_sizes->in_size_x;   // 3328;//img_width;
    ipipe_size_cfg->ipipe_if.sizes.v_size_crop = config_sizes->in_size_y;   // 2448;//img_height;

    ipipe_size_cfg->ipipe_if.sizes.h_start = 0;
    ipipe_size_cfg->ipipe_if.sizes.v_start = 0;

    ipipe_size_cfg->ipipe_if.sizes.ppln = config_sizes->in_size_ppln;

    memcpy(&ipipe_size_cfg->isif.sizes, &ipipe_size_cfg->ipipe_if.sizes,
           sizeof(in_frame_sizes_t));

    memcpy(&ipipe_size_cfg->ipipe.sizes, &ipipe_size_cfg->ipipe_if.sizes,
           sizeof(in_frame_sizes_t));
}

/* ===================================================================
 *  @func     issDrvSetNew2DLscTable
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
ISP_RETURN issDrvSetNew2DLscTable(void *new_table_ptr,
                                  iss_config_processing_t * iss_config,
                                  iss_drv_config_t * iss_drv_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    isif_2dlsc_cfg_t *isif_2dlsc_cfg = &iss_drv_config->lsc2d_cfg;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    IssIsifCopyAndSetLscTable((uint32) new_table_ptr, isif_2dlsc_cfg,
                              iss_config, ipipe_size_cfg);
    return (retval);
}

/* ===================================================================
 *  @func     issDrvRawToYuvConfig
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
ISP_RETURN issDrvRawToYuvConfig(iss_config_params_t * config_params,
                                iss_drv_config_t * iss_drv_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

#ifdef USE_MIPI_MODE
	Int32 csiDDRspeedMhz = 400;
    csi2_cfg_t pIssCsi2Config;
    CSI2_DEVICE_T dev = CSI2_DEVICE_A;
    csi2_ctx_transcode_cfg_t transcode;

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
    {
        return retval;
    }
    retval = csi2_open();
	
	//Enable CSI module clocks
    iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_ENABLE);

    //Reset the CSI2 receiver (globlal reset) and wait for RESET_DONE.
    retval |= csi2_reset(dev, 0);
		
	pIssCsi2Config.master_standby_mode = CSI2_NO_STANDBY;            // CSI2_SMART_STANDBY;
    pIssCsi2Config.auto_idle_ocp_clock = CSI2_FREE_RUNNING;          // CSI2_AUTO_IDLE;
    pIssCsi2Config.context_set_flag |= (0x01u << 0);
    pIssCsi2Config.ctx_cfg_ptr_array[0] = &csi2ImageCtxDefaultParams;
    pIssCsi2Config.control = &csi2CtrlDefaultParamsVpOnly;
    (pIssCsi2Config.ctx_cfg_ptr_array[0])->vp_force = CSI2_ENABLED_BIT;

    pIssCsi2Config.complexio_cfg_1 = &csi2ComplexioCfgDefaultParams;
    pIssCsi2Config.complexio_cfg_2 = &csi2ComplexioCfgDefaultParams;

    csi2ComplexioCfgDefaultParams.data1_position = CSI2_POSITION_2;
    csi2ComplexioCfgDefaultParams.data2_position = CSI2_POSITION_3; //CSI2_NOT_USED;//
    csi2ComplexioCfgDefaultParams.data3_position = CSI2_POSITION_4; //CSI2_NOT_USED; //
    csi2ComplexioCfgDefaultParams.data4_position = CSI2_POSITION_5; //CSI2_NOT_USED; //

    pIssCsi2Config.complexio_irq_enable_1 = 0x00000000u;
    pIssCsi2Config.complexio_irq_enable_2 = 0x00000000u;

//    pIssCsi2Config.timing_io = &csi2TimingDefaultParams;

    // set proper timing on CSI D-PHY
    pIssCsi2Config.physical = &csi2PhysicalDefaultParams;
    pIssCsi2Config.physical->phy_00 = ((((12500 * csiDDRspeedMhz + 1000000 / 2) / 1000000) - 1) & 0xFF) << 8;
    pIssCsi2Config.physical->phy_00 |= ((((90000 * csiDDRspeedMhz + 1000000 / 2) / 1000000) + 3) & 0xFF);
				
	retval = csi2_config(dev, &pIssCsi2Config);

	//Start Streaming
#ifdef IMGS_MICRON_AR0330
	Iss_Ar0330EnableStreaming();
#elif defined IMGS_SONY_IMX140
	Iss_IMX140_EnableStreaming();
#endif
	csi2_wait_phy_reset();


    transcode.transcode_mode = CSI2_DISABLE_TRANSCODE;
    transcode.hcount = 1920;
    transcode.hskip = 0;
    transcode.vcount = 1080;
    transcode.vskip = 0;
    retval |= csi2_config_context_transcode(dev, 0, &transcode);

    *(unsigned int *)0x55051004 = 0x0c;//ISIF_MODESET hdpol vdpol netive

    iss_regs->ISS_CTRL &= ~0x0000000C;
    csi2A_regs->CSI2_CTX_CTRL2_0 &= 0x000003FF;
    csi2A_regs->CSI2_CTX_CTRL2_0 |= 0x0000012C;
    csi2A_regs->CSI2_CTRL |= 0x00000800;
    isp_regs ->ISP5_CTRL |= 0x00C00000;

    retval = csi2_enable_context(dev, 0);
    retval = csi2_start(dev);

#endif

    iss_config_processing_t *iss_config = config_params->iss_config;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    if (RSZ_MAX_LINE_SIZE < config_params->config_sizes.out_size_x)
    {
        return ISP_FAILURE;
    }
    if (IPIPE_PROC_COMPR_NO != config_params->f_compres_in)
    {
        ipipe_size_cfg->bytes_per_pixel = 1;
    }
    else
    {
        ipipe_size_cfg->bytes_per_pixel = 2;
    }
    ipipe_set_sizes_ccp2_crop(config_params, ipipe_size_cfg, 1);

    ipipe_size_cfg->rszA.format = config_params->out_format;
    ipipe_size_cfg->rszB.format = config_params->out_formatB;

    switch (config_params->iss_config->vp_device)
    {
        case IPIPE_VP_DEV_CSIA:
            retval |= iss_isp_input_config(ISS_ISP_INPUT_CSI2_A);
            break;

        case IPIPE_VP_DEV_CSIB:
            retval |= iss_isp_input_config(ISS_ISP_INPUT_CSI2_B);
            break;

        case IPIPE_VP_DEV_CCP:
            retval |= iss_isp_input_config(ISS_ISP_INPUT_CCCP2);
            break;

        case IPIPE_VP_DEV_PI:
            retval |= iss_isp_input_config(ISS_ISP_INPUT_PI);
            break;

        default:
            return ISP_FAILURE;
    }

    isp_common_sync_ctrl_cfg(ISP_SYNC_ENABLE);
    isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_DISABLE);
    isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_DMA_CLK);
    isp_common_vbusm_id_cfg(0xF);
    isp_common_posted_write_cfg(ISP_POSTED_WRT_ENABLE);
    isp_common_vbusm_priority_cfg(0x00);

    if ((IPIPE_VP_DEV_CSIA != config_params->iss_config->vp_device) &&
        (IPIPE_VP_DEV_CSIB != config_params->iss_config->vp_device))
    {
        isp_common_ip_port_cfg(ISP_INPUT_PORT_CCP2_PARALLEL);
    }
    else
    {
        isp_common_ip_port_cfg(ISP_INPUT_PORT_CSI2);
    }

    // if((IPIPE_VP_DEV_CSIA != config_params->iss_config->vp_device) &&
    // (IPIPE_VP_DEV_CSIB != config_params->iss_config->vp_device)) {
    iss_sync_detect_config(ISS_SYNC_VS_RISING_EDGE);
    // }

    retval |=
        issIpipeifPreviewParamsInit(&iss_drv_config->ipipeif_cfg,
                                    config_params->iss_config);

    retval |= ipipeif_config(&iss_drv_config->ipipeif_cfg);

    retval |=
        IssIsifParamsInitPreview(&iss_drv_config->isif_cfg,
                                 &ipipe_size_cfg->isif.sizes, config_params);

    retval |= isif_config(&iss_drv_config->isif_cfg);

    // ///////////////// need to config 2D LSC //////////////////////////
    iss_drv_config->lsc2d_cfg.enable = 0;
    if (iss_config->f_proc_isif_validity & PROC_ISIF_VALID_2DLSC)
    {
        if (iss_config->lsc_2d->enable)
        {
            IssIsifCopyAndSetLscTable((uint32) config_params->
                                      lsc_2d_table_buffer,
                                      &iss_drv_config->lsc2d_cfg, iss_config,
                                      ipipe_size_cfg);
            if ((ISS_CROP_NO_ZOOM * 8) < config_params->config_sizes.crop_x)
            {
                iss_drv_config->lsc2d_cfg.enable = 0;
            }
        }
    }
    // ///////////// CCP_Rx config ////////////////////////
    // retval |= issCcp2PreviewParamsInit(&iss_drv_config->ccp_cfg,
    // ipipe_size_cfg->clock_div, CCP2_OUTPUT_VP);
    // //
    // retval |= ccp2_config(&iss_drv_config->ccp_cfg);
    // //
    // retval |= issCcp2PreviewLcmInit(&iss_drv_config->ccp_lcm_cfg,
    // &ipipe_size_cfg->ipipe_if.sizes, 0, config_params->f_compres_in,
    // CCP2_OUTPUT_VP);
    // retval |= CCP2_Read_from_Memory_Config(&iss_drv_config->ccp_lcm_cfg,
    // MEMORY_READ);
    if (config_params->VidstabEnb)
    {

        // Enable Boundary Signal Calculator
        // config_params->iss_config->bsc->enable = DRV_BSC_ENABLE_ON;
        // config_params->iss_config->bsc->mode = DRV_BSC_MODE_SINGLE;
        // config_params->iss_config->bsc->col_sample =
        // DRV_BSC_SAMPLING_ENABLED;
        // config_params->iss_config->bsc->row_sample =
        // DRV_BSC_SAMPLING_ENABLED;
        // config_params->iss_config->bsc->element = DRV_BSC_ELEMENT_Y;
        // config_params->iss_config->f_proc_ipipe_validity |=
        // PROC_IPIPE_VALID_BSC;
    }

    retval |=
        issIpipePreviewParamsInit(&iss_drv_config->ipipe_cfg, config_params,
                                  &ipipe_size_cfg->ipipe.sizes);

    if (iss_config->f_proc_ipipe_validity & PROC_IPIPE_VALID_LSC)
    {
        issCropIpipeLsc(&iss_drv_config->ipipe_cfg, ipipe_size_cfg);
    }
    retval |= ipipe_config(&iss_drv_config->ipipe_cfg);

    retval |=
        issRszPreviewParamsInit(&iss_drv_config->rsz_config, config_params,
                                ipipe_size_cfg);

    retval |= rsz_config(&iss_drv_config->rsz_config);

    if (ISP_SUCCESS == retval)
    {
        ipipe_size_cfg->f_flip_A = config_params->f_flip;
        ipipe_size_cfg->f_mirror_A = config_params->f_mirror;
        ipipe_size_cfg->f_flip_B = config_params->f_flip_B;
        ipipe_size_cfg->f_mirror_B = config_params->f_mirror_B;
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvRawToRawConfig
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
ISP_RETURN issDrvRawToRawConfig(iss_config_params_t * config_params,
                                iss_drv_config_t * iss_drv_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    if (IPIPE_PROC_COMPR_NO != config_params->f_compres_in)
    {
        ipipe_size_cfg->bytes_per_pixel = 1;
    }
    else
    {
        ipipe_size_cfg->bytes_per_pixel = 2;
    }
    // ipipe_set_sizes_ccp2_crop(config_params, ipipe_size_cfg, 1);
    ipipe_size_cfg->clock_div = CCP2_FRACK_CLOCK_MAX;
    ipipe_size_cfg->ipipe_if.sizes.h_size =
        config_params->config_sizes.in_size_x;
    ipipe_size_cfg->ipipe_if.sizes.h_size_crop =
        ipipe_size_cfg->ipipe_if.sizes.h_size;
    ipipe_size_cfg->ipipe_if.sizes.h_start = 0;
    ipipe_size_cfg->ipipe_if.sizes.v_size =
        config_params->config_sizes.in_size_y;
    ipipe_size_cfg->ipipe_if.sizes.v_size_crop =
        ipipe_size_cfg->ipipe_if.sizes.v_size;
    ipipe_size_cfg->ipipe_if.sizes.v_start = 0;
    ipipe_size_cfg->ipipe_if.sizes.ppln =
        config_params->config_sizes.in_size_ppln;

    ipipe_size_cfg->rszA.format = config_params->out_format;
    ipipe_size_cfg->rszB.format = config_params->out_formatB;

    // ///////////// CCP_Rx config ////////////////////////
    retval |=
        issCcp2PreviewParamsInit(&iss_drv_config->ccp_cfg,
                                 ipipe_size_cfg->clock_div, CCP2_OUTPUT_OCP);

    retval |= ccp2_config(&iss_drv_config->ccp_cfg);

    retval |= issCcp2PreviewLcmInit(&iss_drv_config->ccp_lcm_cfg,
                                    &ipipe_size_cfg->ipipe_if.sizes,
                                    config_params->config_sizes.out_size_bpln,
                                    config_params->f_compres_in,
                                    CCP2_OUTPUT_OCP);

    retval |=
        CCP2_Read_from_Memory_Config(&iss_drv_config->ccp_lcm_cfg, MEMORY_READ);

    if (ISP_SUCCESS == retval)
    {
        ipipe_size_cfg->f_flip_A = config_params->f_flip;
        ipipe_size_cfg->f_mirror_A = config_params->f_mirror;
        ipipe_size_cfg->f_flip_B = config_params->f_flip_B;
        ipipe_size_cfg->f_mirror_B = config_params->f_mirror_B;
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvCcp2SetSize
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
ISP_RETURN issDrvCcp2SetSize(uint32 h_size, uint32 v_size)
{
    in_frame_sizes_t sizes;

    ccp2_lcm_enable_t lcm_enable;

    ISP_RETURN retval = ISP_SUCCESS;

    sizes.h_size = h_size;
    sizes.h_size_crop = h_size;
    sizes.h_start = 0;
    sizes.ppln = h_size;
    sizes.v_size = v_size;
    sizes.v_size_crop = v_size;
    sizes.v_start = 0;

    retval |=
        issCcp2PreviewLcmInit(&lcm_enable, &sizes, 0, IPIPE_PROC_COMPR_NO,
                              CCP2_OUTPUT_VP);

    retval |= CCP2_Read_from_Memory_Config(&lcm_enable, MEMORY_READ);

    return retval;
}

/* ===================================================================
 *  @func     issDrvIsifSetSize
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
ISP_RETURN issDrvIsifSetSize(uint32 h_size, uint32 v_size)
{
    isif_input_sizes_t in_size;

    in_size.hd_width = h_size;
    in_size.ppln_hs_interval = h_size;
    in_size.vd_width = v_size;
    in_size.lpfr_vs_interval = v_size;

    isif_config_in_sizes(&in_size);

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issDrvYuvToYuvConfig
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
ISP_RETURN issDrvYuvToYuvConfig(iss_config_params_t * config_params,
                                iss_drv_config_t * iss_drv_config)
{

    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    ipipe_size_cfg->rszA.format = config_params->out_format;
    ipipe_size_cfg->rszB.format = config_params->out_formatB;
    ipipe_size_cfg->rsz_ip_port = config_params->iss_config->rsz_ip_port_sel;

    if (IPIPE_IN_FORMAT_YUV420 == config_params->in_format)
    {

        if (IPIPE_IN_COMP_Y == config_params->in_comp)
        {
            ipipe_set_sizes(config_params, ipipe_size_cfg, 1);
            retval |=
                issIpipeifYuv420InputParamsInit(&iss_drv_config->ipipeif_cfg,
                                                ipipe_size_cfg, 1);
        }
        else
        {
            ipipe_set_sizes(config_params, ipipe_size_cfg, 1);
            retval |=
                issIpipeifYuv420InputParamsInit(&iss_drv_config->ipipeif_cfg,
                                                ipipe_size_cfg, 0);
        }
    }
    else
    {
        ipipe_set_sizes(config_params, ipipe_size_cfg, 0);
        retval |=
            issIpipeifYuvInputParamsInit(&iss_drv_config->ipipeif_cfg,
                                         ipipe_size_cfg);
    }
    retval |= ipipeif_config(&iss_drv_config->ipipeif_cfg);

    if (RSZ_IP_IPIPE == config_params->iss_config->rsz_ip_port_sel)
    {

        retval |=
            issIpipePreviewParamsInit(&iss_drv_config->ipipe_cfg, config_params,
                                      &ipipe_size_cfg->ipipe.sizes);

        if (config_params->iss_config->
            f_proc_ipipe_validity & PROC_IPIPE_VALID_LSC)
        {
            issCropIpipeLsc(&iss_drv_config->ipipe_cfg, ipipe_size_cfg);
        }

        retval |= ipipe_config(&iss_drv_config->ipipe_cfg);
    }
    retval |=
        issRszPreviewParamsInit(&iss_drv_config->rsz_config, config_params,
                                ipipe_size_cfg);

    retval |= rsz_config(&iss_drv_config->rsz_config);

    retval |= issSetRszInputPort(config_params->iss_config->rsz_ip_port_sel);

    if (IPIPE_IN_FORMAT_YUV420 == config_params->in_format)
    {
        if (IPIPE_IN_COMP_C == config_params->in_comp)
        {
            rsz_cfg_yuv420_op(RESIZER_A, RSZ_YUV420_C_OP_ONLY);
        }
    }
    return retval;

}

/* ===================================================================
 *  @func     issDrvMemToMemConfig
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
ISP_RETURN issDrvMemToMemConfig(iss_config_params_t * config_params,
                                iss_drv_config_t * iss_drv_config)
{
    ISP_RETURN result;

    if (IPIPE_IN_FORMAT_BAYER == config_params->in_format)
    {
        if (IPIPE_OUT_FORMAT_BAYER != config_params->out_format)
        {
            result = issDrvRawToYuvConfig(config_params, iss_drv_config);
        }
        else
        {
            result = issDrvRawToRawConfig(config_params, iss_drv_config);
        }
    }
    else
    {
        result = issDrvYuvToYuvConfig(config_params, iss_drv_config);
    }
    dbg_iss_drv_config = iss_drv_config;
    return (result);
}

/* ===================================================================
 *  @func     issDrvYuvToYuvStart
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
ISP_RETURN issDrvYuvToYuvStart(iss_drv_config_t * iss_drv_config,
                               void *in_buffer, void *out_buff_rszA,
                               void *out_buff_rszB, void *out_buff_rszA_C,
                               void *out_buff_rszB_C)
{

    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    if (ipipe_size_cfg->rszA.format != IPIPE_OUT_FORMAT_UNUSED)
    {
        if ((ipipe_size_cfg->rszA.format == IPIPE_OUT_FORMAT_YUV420) &&
            (out_buff_rszA_C != NULL))
        {
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV420_Y_OP, out_buff_rszA,
                                    ipipe_size_cfg->rszA.out_sizes.bpln);
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV420_C_OP, out_buff_rszA_C,
                                    ipipe_size_cfg->rszA.outC_ppln);
        }
        else
        {
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV422_RAW_RGB_OP,
                                    out_buff_rszA,
                                    ipipe_size_cfg->rszA.out_sizes.bpln);
        }
    }
    if (ipipe_size_cfg->rszB.format != IPIPE_OUT_FORMAT_UNUSED)
    {
        if ((ipipe_size_cfg->rszB.format == IPIPE_OUT_FORMAT_YUV420) &&
            (out_buff_rszB_C != NULL))
        {
            if (out_buff_rszB != NULL && out_buff_rszB_C == NULL)
            {
                return ISP_FAILURE;
            }
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV420_Y_OP, out_buff_rszB,
                                    ipipe_size_cfg->rszB.out_sizes.bpln);
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV420_C_OP, out_buff_rszB_C,
                                    ipipe_size_cfg->rszB.outC_ppln);
        }
        else
        {
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV422_RAW_RGB_OP,
                                    out_buff_rszB,
                                    ipipe_size_cfg->rszB.out_sizes.bpln);
        }
    }

    ipipeif_set_sdram_in_addr((uint32) in_buffer);

    if (ipipe_size_cfg->rszA.format != IPIPE_OUT_FORMAT_UNUSED)
    {
        retval |= rsz_submodule_start(RSZ_RUN, RESIZER_A);
    }
    if ((ipipe_size_cfg->rszB.format != IPIPE_OUT_FORMAT_UNUSED) &&
        (out_buff_rszB != NULL))
    {
        retval |= rsz_submodule_start(RSZ_RUN, RESIZER_B);
    }
    retval |= isp_enable_interrupt(ISS_DRV_RSZ_END_ISR);

    retval |= rsz_start(RSZ_RUN);
    if (RSZ_IP_IPIPE == ipipe_size_cfg->rsz_ip_port)
    {
        retval |= ipipe_start(IPIPE_RUN);
    }
    retval |= ipipeif_start(IPIPEIF_RUN);
    return retval;

}

/* ===================================================================
 *  @func     check_lsc_prefetch
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
void check_lsc_prefetch(void)
{
    int tmp = 100000;

    while ((!isif_2dlsc_is_prefetched()) && (tmp--)) ;
    if (!(tmp > 0))
    {
        // TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, "LSC prefetch
        // failed\n");
    }
}

/* ===================================================================
 *  @func     issTriggerRawCapture
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
ISP_RETURN issTriggerRawCapture(Int32 * raw_out_buff, isif_start_t enable)
{
    int status = ISP_SUCCESS;

    isif_config_sdram_address((uint32) raw_out_buff);

    switch (enable)
    {
        case ISIF_START_WRITE:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_ENABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            break;

        case ISIF_STOP_WRITE:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_DISABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            break;

        default:
            status = ISP_INVALID_INPUT;
    }

    return (status);
}

/* ===================================================================
 *  @func     issDrvRawToYuvStart
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
ISP_RETURN issDrvRawToYuvStart(iss_drv_config_t * iss_drv_config,
                               void *in_buffer, void *out_buff_rszA,
                               void *out_buff_rszB, void *out_buff_rszA_C,
                               void *out_buff_rszB_C, void *raw_out_buff)
{

    ISP_RETURN retval = ISP_SUCCESS;

    uint32 out_sizeA = 0, out_sizeB = 0;

    uint32 out_sizeA_C = 0, out_sizeB_C = 0;

    uint8 *pInBuffer;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    if (IPIPE_OUT_FORMAT_UNUSED != ipipe_size_cfg->rszA.format)
    {
        if (ipipe_size_cfg->f_flip_A)
        {
            out_sizeA =
                ipipe_size_cfg->rszA.out_sizes.bpln *
                (ipipe_size_cfg->rszA.out_sizes.v_size);
            out_sizeA_C =
                ipipe_size_cfg->rszA.outC_ppln *
                (ipipe_size_cfg->rszA.out_sizes.v_size / 2);
        }
        if ((ipipe_size_cfg->rszA.format == IPIPE_OUT_FORMAT_YUV420) &&
            (out_buff_rszA_C != NULL))
        {
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV420_Y_OP,
                                    ((uint8 *) out_buff_rszA + out_sizeA),
                                    ipipe_size_cfg->rszA.out_sizes.bpln);
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV420_C_OP,
                                    ((uint8 *) out_buff_rszA_C + out_sizeA_C),
                                    ipipe_size_cfg->rszA.outC_ppln);
        }
        else
        {
            retval |=
                issSetRszOutAddress(RESIZER_A, RSZ_YUV422_RAW_RGB_OP,
                                    ((uint8 *) out_buff_rszA + out_sizeA),
                                    ipipe_size_cfg->rszA.out_sizes.bpln);
        }
    }

    if (IPIPE_OUT_FORMAT_UNUSED != ipipe_size_cfg->rszB.format)
    {
        if (ipipe_size_cfg->f_flip_B)
        {
            out_sizeB =
                ipipe_size_cfg->rszB.out_sizes.bpln *
                (ipipe_size_cfg->rszB.out_sizes.v_size);
            out_sizeB_C =
                ipipe_size_cfg->rszB.outC_ppln *
                (ipipe_size_cfg->rszB.out_sizes.v_size / 2);
        }
        if ((ipipe_size_cfg->rszB.format == IPIPE_OUT_FORMAT_YUV420) &&
            (out_buff_rszB_C != NULL))
        {
            if (out_buff_rszB != NULL && out_buff_rszB_C == NULL)
            {
                return ISP_FAILURE;
            }
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV420_Y_OP,
                                    ((uint8 *) out_buff_rszB + out_sizeB),
                                    ipipe_size_cfg->rszB.out_sizes.bpln);
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV420_C_OP,
                                    ((uint8 *) out_buff_rszB_C + out_sizeB_C),
                                    ipipe_size_cfg->rszB.outC_ppln);
        }
        else
        {
            retval |=
                issSetRszOutAddress(RESIZER_B, RSZ_YUV422_RAW_RGB_OP,
                                    ((uint8 *) out_buff_rszB + out_sizeB),
                                    ipipe_size_cfg->rszB.out_sizes.bpln);
        }
    }

    pInBuffer =
        (uint8 *) in_buffer +
        (ipipe_size_cfg->ipipe_if.sizes.v_start *
         ipipe_size_cfg->ipipe_if.sizes.ppln * ipipe_size_cfg->bytes_per_pixel);
    pInBuffer +=
        ((ipipe_size_cfg->ipipe_if.sizes.h_start *
          ipipe_size_cfg->bytes_per_pixel / 32) * 32);
    // retval |= CCP2_Lcm_Set_Read_Address((void*)pInBuffer);

    // extern csi2_regs_ovly csi2A_regs;
    // {//write to the read location to see is there an exeption
    // //stop CSI to see the result
    // csi2A_regs->CSI2_CTRL &= ~0x00000001;
    // *((uint32 *)(pInBuffer)) = 0xBABACECA;
    // }
    // {//make an exeption
    // *((uint32 *)(0x04000000)) = 0xBABACECA;
    // }

    if ((ipipe_size_cfg->rszA.format != IPIPE_OUT_FORMAT_UNUSED) &&
        (ipipe_size_cfg->rszA.format != IPIPE_OUT_FORMAT_BAYER))
    {
        retval |= rsz_submodule_start(RSZ_RUN, RESIZER_A);
    }

    if ((ipipe_size_cfg->rszB.format != IPIPE_OUT_FORMAT_BAYER) &&
        (ipipe_size_cfg->rszB.format != IPIPE_OUT_FORMAT_UNUSED)
        && (out_buff_rszB != NULL))
    {
        retval |= rsz_submodule_start(RSZ_RUN, RESIZER_B);
    }

    isif_config_sdram_address((uint32) raw_out_buff);

    retval |= isp_enable_interrupt(ISS_DRV_RSZ_END_ISR);

    retval |= rsz_start(RSZ_RUN);
    retval |= ipipe_start(IPIPE_RUN);
    if (NULL != raw_out_buff)
    {
        retval |= isif_start(ISIF_START_WRITE);
    }
    else
    {
        retval |= isif_start(ISIF_START_SYNC);
    }
    // retval |= ipipeif_start(IPIPEIF_RUN);
    if ((iss_drv_config->lsc2d_cfg.enable))
    {
        retval |= isif_config_2d_lsc(&iss_drv_config->lsc2d_cfg);
        isif_2dlsc_sof_int_enable(iss_drv_config->lsc2d_cfg.enable);
        isif_2dlsc_enable(iss_drv_config->lsc2d_cfg.enable);
        check_lsc_prefetch();
    }
    // retval |= Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, EOF_IRQ);
    // retval |= ccp2_start(MEMORY_READ);

    return retval;

}

/* ===================================================================
 *  @func     issDrvRawToRawStart
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
ISP_RETURN issDrvRawToRawStart(iss_drv_config_t * iss_drv_config,
                               void *in_buffer, void *out_buff)
{

    ISP_RETURN retval = ISP_SUCCESS;

    uint8 *pInBuffer;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    pInBuffer =
        (uint8 *) in_buffer +
        (ipipe_size_cfg->ipipe_if.sizes.v_start *
         ipipe_size_cfg->ipipe_if.sizes.ppln * ipipe_size_cfg->bytes_per_pixel);
    pInBuffer +=
        ((ipipe_size_cfg->ipipe_if.sizes.h_start *
          ipipe_size_cfg->bytes_per_pixel / 32) * 32);

    retval |= CCP2_Lcm_Set_Read_Address((void *) pInBuffer);

    // retval |= CCP2_Lcm_Set_Write_Address(out_buff);
    // TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, "CCP2_LCM_Set_Write
    // Called !!!!");

    retval |= Configure_CCP2_Lcm_Interrupt(CONTROL_ENABLE, EOF_IRQ);

    retval |= ccp2_start(MEMORY_READ);

    return retval;

}

/* ===================================================================
 *  @func     issDrvWbUpdate
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
ISP_RETURN issDrvWbUpdate(iss_config_processing_t * iss_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_wb_cfg_t *wb_params;

    if (iss_config->f_proc_ipipe_validity & PROC_IPIPE_VALID_WB)
    {
        iss_config->f_proc_ipipe_validity &= ~PROC_IPIPE_VALID_WB;
        wb_params = (ipipe_wb_cfg_t *) iss_config->wb;
        retval = ipipe_config_wbal(wb_params);
    }
    return retval;
}

rsz_yc_min_max iss_rsz_yc_min_max[2] = {
    {0, 255, 0, 255}
    ,
    {16, 235, 16, 240}
    ,
};

/* ===================================================================
 *  @func     issDrvSetYuvRange
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
ISP_RETURN issDrvSetYuvRange(IPIPE_YUV_RANGE eRange)
{
    return rsz_cfg_yc_max_min(&iss_rsz_yc_min_max[eRange]);
}

/* ===================================================================
 *  @func     issDrvH3aConfig
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
ISP_RETURN issDrvH3aConfig(iss_config_processing_t * iss_config,
                           void *aewb_buff, void *af_buff,
                           iss_drv_config_t * iss_drv_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    /*
     * h3a_cfg_t h3a_cfg; ipipeif_h3a_input_cfg_t h3a_input_cfg; */
    retval = h3a_config_aewb_op_addr((uint32) aewb_buff);
    retval |= h3a_config_af_op_addr((uint32) af_buff);

    if (PROC_H3A_VALID_NO == iss_config->f_h3a_validity)
    {
        return ISP_SUCCESS;
    }
    retval |=
        h3aParamsInitPreview(&iss_drv_config->h3a_cfg, iss_config, af_buff,
                             aewb_buff);

    if (H3A_FEATURE_DISABLE == iss_config->h3a_common_params->h3a_decim_enable)
    {
        issIpipeifH3aInputParamsInit(&iss_drv_config->h3a_input_cfg, 0,
                                     IPIPEIF_DOWNSCALE_NOM);
    }
    else
    {
        issIpipeifH3aInputParamsInit(&iss_drv_config->h3a_input_cfg, 0,
                                     IPIPEIF_DOWNSCALE_NOM * 2);
    }
    if (iss_config->h3a_common_params->ipiprif_ave_filt_en ==
        H3A_FEATURE_DISABLE)
    {
        iss_drv_config->h3a_input_cfg.avg_filter_enable =
            (IPIPEIF_AVGFILT_T) CSL_IPIPEIF_RSZ2_AVGFILT2_OFF;
    }
    ipipeif_h3a_input_config(&iss_drv_config->h3a_input_cfg);
    iss_drv_config->h3a_cfg.h3a_common_cfg->reserved = 0;  // some patch
                                                           // ?????
                                                           // 0xdc000001
    retval |= h3a_config(&iss_drv_config->h3a_cfg);

    return retval;
}

/* ===================================================================
 *  @func     issDrvH3aStart
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
ISP_RETURN issDrvH3aStart(void)
{
    ISP_RETURN retval = ISP_SUCCESS;
    retval |= h3a_aeawb_enable(H3A_FEATURE_ENABLE);
    retval |= h3a_af_enable(H3A_FEATURE_ENABLE);
    return retval;
}

/* ===================================================================
 *  @func     issDrvH3aStop
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
ISP_RETURN issDrvH3aStop(void)
{
    h3a_af_enable(H3A_FEATURE_DISABLE);
    return h3a_aeawb_enable(H3A_FEATURE_DISABLE);
}

/* ===================================================================
 *  @func     issDrvRawStatisticsConfig
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
ISP_RETURN issDrvRawStatisticsConfig(iss_config_params_t * config_params,
                                     iss_drv_config_t * iss_drv_config)
{

    ISP_RETURN retval = ISP_SUCCESS;

    ISS_ISP_INPUT_SEL_T vp_dev;

    IPIPE_PROC_COMPRESSION f_compres_out = config_params->f_compres_out;

    iss_config_processing_t *iss_config = config_params->iss_config;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    ISP_IP_PORT_T ip_port;

    ipipe_set_isif_sizes(&config_params->config_sizes, ipipe_size_cfg);

    switch (config_params->iss_config->vp_device)
    {
        case IPIPE_VP_DEV_CSIA:
            vp_dev = ISS_ISP_INPUT_CSI2_A;
            ip_port = ISP_INPUT_PORT_CSI2;
            break;
        case IPIPE_VP_DEV_CSIB:
            vp_dev = ISS_ISP_INPUT_CSI2_B;
            ip_port = ISP_INPUT_PORT_CSI2;
            break;
        case IPIPE_VP_DEV_CCP:
            vp_dev = ISS_ISP_INPUT_CCCP2;
            ip_port = ISP_INPUT_PORT_CCP2_PARALLEL;
            break;
        default:
            vp_dev = ISS_ISP_INPUT_CSI2_A;
            ip_port = ISP_INPUT_PORT_CSI2;
            break;
    }
    retval |= iss_isp_input_config(vp_dev);
    isp_common_ip_port_cfg(ip_port);

    retval |=
        IssIsifParamsInitCaptureInMemory(&iss_drv_config->isif_cfg, iss_config,
                                         &ipipe_size_cfg->isif.sizes,
                                         f_compres_out);

    iss_drv_config->lsc2d_cfg.enable = 0;
    if (iss_config->f_proc_isif_validity & PROC_ISIF_VALID_2DLSC)
    {
        if (iss_config->lsc_2d->enable)
        {
            IssIsifCopyAndSetLscTable((uint32) config_params->
                                      lsc_2d_table_buffer,
                                      &iss_drv_config->lsc2d_cfg, iss_config,
                                      ipipe_size_cfg);
            if ((ISS_CROP_NO_ZOOM * 8) < config_params->config_sizes.crop_x)
            {
                iss_drv_config->lsc2d_cfg.enable = 0;
            }
        }
    }
    retval |= isif_config(&iss_drv_config->isif_cfg);

    retval |=
        issIpipeifPreviewParamsInit(&iss_drv_config->ipipeif_cfg, iss_config);

    retval |= ipipeif_config(&iss_drv_config->ipipeif_cfg);

    retval |=
        issIpipeCaptureParamsInitNoWrite(&iss_drv_config->ipipe_cfg, iss_config,
                                         &ipipe_size_cfg->ipipe.sizes);

    if (iss_config->f_proc_ipipe_validity & PROC_IPIPE_VALID_LSC)
    {
        issCropIpipeLsc(&iss_drv_config->ipipe_cfg, ipipe_size_cfg);
    }

    retval |= ipipe_config(&iss_drv_config->ipipe_cfg);

    return retval;

}

/* ===================================================================
 *  @func     issDrvCCPRawStatisticsConfig
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
ISP_RETURN issDrvCCPRawStatisticsConfig(iss_config_params_t * config_params,
                                        iss_drv_config_t * iss_drv_config)
{

    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    in_frame_sizes_t *frame_size = &ipipe_size_cfg->ipipe_if.sizes;

    retval |= issDrvRawStatisticsConfig(config_params, iss_drv_config);

    retval |= issDrvCcp2SetSize(frame_size->h_size, frame_size->v_size);

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issDrvIpipeProcessingConfig
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
ISP_RETURN issDrvIpipeProcessingConfig(iss_config_processing_t * iss_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_cfg_t ipipe_cfg;

    retval |= issIpipeConfigProcessing(&ipipe_cfg, iss_config);
    retval |= ipipe_config_filters(&ipipe_cfg);

    return retval;
}

/* ===================================================================
 *  @func     issDrvProcessingConfig
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
ISP_RETURN issDrvProcessingConfig(iss_config_processing_t * iss_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    ipipe_cfg_t ipipe_cfg;

    isif_cfg_t isif_cfg;

    if (iss_config->f_proc_ipipe_validity)
    {
        retval |= issIpipeConfigProcessing(&ipipe_cfg, iss_config);
        retval |= ipipe_config_filters(&ipipe_cfg);
    }
    if (iss_config->f_proc_isif_validity)
    {
        IssIsifConfigFilters(&isif_cfg, iss_config);
        retval |= isif_config(&isif_cfg);
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvDarkToRawConfig
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
ISP_RETURN issDrvDarkToRawConfig(iss_config_params_t * config_params,
                                 iss_drv_config_t * iss_drv_config)
{

    ipipe_cfg_t test_ipipe_cfg;

    ipipeif_path_cfg_t test_ipipeif_cfg;

    isif_cfg_t test_isif_cfg;

    ISS_ISP_INPUT_SEL_T vp_dev;

    ISP_RETURN retval = ISP_SUCCESS;

    iss_config_processing_t *iss_config = config_params->iss_config;

    IPIPE_PROC_COMPRESSION f_compres_out = config_params->f_compres_out;

    IPIPE_PROC_COMPRESSION f_compres_in = config_params->f_compres_in;

    ipipe_size_cfg_t *ipipe_size_cfg = &iss_drv_config->ipipe_size_cfg;

    ipipe_set_sizes(config_params, ipipe_size_cfg, 0);

    switch (config_params->iss_config->vp_device)
    {
        case IPIPE_VP_DEV_CSIA:
            vp_dev = ISS_ISP_INPUT_CSI2_A;
            break;
        case IPIPE_VP_DEV_CSIB:
            vp_dev = ISS_ISP_INPUT_CSI2_B;
            break;
        default:
            vp_dev = ISS_ISP_INPUT_CSI2_A;
            break;
    }
    retval |= iss_isp_input_config(vp_dev);
    isp_common_ip_port_cfg(ISP_INPUT_PORT_CSI2);

    retval |=
        IssIsifParamsInitCaptureInMemory(&test_isif_cfg, iss_config,
                                         &ipipe_size_cfg->isif.sizes,
                                         f_compres_out);

    retval |= isif_config(&test_isif_cfg);

    // retval |= issIpipeifPreviewParamsInit(&test_ipipeif_cfg);
    retval |=
        issIpipeifDarkFrameParamsInit(&test_ipipeif_cfg, iss_config,
                                      &ipipe_size_cfg->ipipe_if.sizes,
                                      f_compres_in);

    retval |= ipipeif_config(&test_ipipeif_cfg);

    retval |=
        issIpipeCaptureParamsInitNoWrite(&test_ipipe_cfg, iss_config,
                                         &ipipe_size_cfg->ipipe.sizes);

    if (iss_config->f_proc_ipipe_validity & PROC_IPIPE_VALID_LSC)
    {
        issCropIpipeLsc(&iss_drv_config->ipipe_cfg, ipipe_size_cfg);
    }

    retval |= ipipe_config(&test_ipipe_cfg);

    return retval;

}

/* ===================================================================
 *  @func     issDrvMemToMemStart
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
ISP_RETURN issDrvMemToMemStart(iss_drv_config_t * iss_drv_config,
                               void *in_buffer, void *out_buff_rsza,
                               void *out_buff_rszb, void *out_buff_rsza_c,
                               void *out_buff_rszb_c, IPIPE_IN_FORMAT eInFormat,
                               void *raw_out_buff)
{
    ISP_RETURN retval;

    if (IPIPE_IN_FORMAT_BAYER == eInFormat)
    {
        if (IPIPE_OUT_FORMAT_BAYER !=
            iss_drv_config->ipipe_size_cfg.rszA.format)
        {
            retval =
                issDrvRawToYuvStart(iss_drv_config, in_buffer, out_buff_rsza,
                                    out_buff_rszb, out_buff_rsza_c,
                                    out_buff_rszb_c, raw_out_buff);
        }
        else
        {
            retval =
                issDrvRawToRawStart(iss_drv_config, in_buffer, out_buff_rsza);
        }
    }
    else
    {
        retval =
            issDrvYuvToYuvStart(iss_drv_config, in_buffer, out_buff_rsza,
                                out_buff_rszb, out_buff_rsza_c,
                                out_buff_rszb_c);
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvVpToIsifStart
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
ISP_RETURN issDrvVpToIsifStart(iss_drv_config_t * iss_drv_config,
                               void *out_raw_buffer)
{
    ISP_RETURN retval = ISP_SUCCESS;

    isif_config_sdram_address((uint32) out_raw_buffer);

    retval |= ipipe_start(IPIPE_RUN);
    // retval |= ipipeif_start(IPIPEIF_RUN);
    if (NULL != out_raw_buffer)
    {
        retval |= isif_start(ISIF_START_WRITE);
    }
    else
    {
        retval |= isif_start(ISIF_START_SYNC);
    }

    if ((iss_drv_config->lsc2d_cfg.enable))
    {
        retval |= isif_config_2d_lsc(&iss_drv_config->lsc2d_cfg);
        isif_2dlsc_sof_int_enable(iss_drv_config->lsc2d_cfg.enable);
        isif_2dlsc_enable(iss_drv_config->lsc2d_cfg.enable);
        check_lsc_prefetch();
    }

    return retval;
}

/* ===================================================================
 *  @func     issDrvCcpToIsifStart
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
ISP_RETURN issDrvCcpToIsifStart(iss_drv_config_t * iss_drv_config,
                                void *in_buffer, void *out_raw_buffer)
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval |= CCP2_Lcm_Set_Read_Address(in_buffer);

    retval |= issDrvVpToIsifStart(iss_drv_config, out_raw_buffer);

    retval |= CCP2_clear_irq_status(EOF_IRQ);

    retval |= ccp2_start(MEMORY_READ);

    return retval;
}

/* ===================================================================
 *  @func     issDrvDarkToRawStart
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
ISP_RETURN issDrvDarkToRawStart(void *out_buffer, void *dfs_address)
{

    ISP_RETURN retval = ISP_SUCCESS;

    retval = isif_config_sdram_address((uint32) out_buffer);
    retval |= ipipeif_set_sdram_in_addr((uint32) dfs_address);

    retval |= isp_enable_interrupt(ISS_DRV_IPIPEIF_END_ISR);

    retval |= ipipe_start(IPIPE_SINGLE);
    retval |= isif_start(ISIF_START_WRITE);
    retval |= ipipeif_start(IPIPEIF_SINGLE);

    return retval;
}

/* ===================================================================
 *  @func     issDrvProcessingStop
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
ISP_RETURN issDrvProcessingStop(void)
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval |= isp_disable_interrupt(ISS_DRV_RSZ_END_ISR);
    retval |= isp_disable_interrupt(ISS_DRV_IPIPEIF_END_ISR);
    retval |= Configure_CCP2_Lcm_Interrupt(CONTROL_DISABLE, EOF_IRQ);

    retval |= ipipe_start(IPIPE_STOP);
    retval |= isif_start(ISIF_STOP);
    retval |= ipipeif_start(IPIPEIF_STOP);
    retval |= rsz_submodule_start(RSZ_STOP, RESIZER_A);
    retval |= rsz_submodule_start(RSZ_STOP, RESIZER_B);
    retval |= rsz_start(RSZ_STOP);
    retval |= ccp2_disableLCM();

    return retval;
}

/* ===================================================================
 *  @func     issDrvStopIsif
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
ISP_RETURN issDrvStopIsif(uint32 dummy_frames, uint32 width, uint32 height,
                          void *in_buffer)
{
    ISP_RETURN retval = ISP_SUCCESS;

    int i;

    retval |= ipipe_start(IPIPE_STOP);
    retval |= isif_start(ISIF_STOP);
    // retval |= ipipeif_start(IPIPEIF_STOP);

    if (dummy_frames)
    {
        retval |= iss_isp_input_config(ISS_ISP_INPUT_CCCP2);
        retval |= isp_common_ip_port_cfg(ISP_INPUT_PORT_CCP2_PARALLEL);

        retval |= issDrvCcp2SetSize(width, height);
        retval |= issDrvIsifSetSize(width, height);
        retval |= CCP2_Lcm_Set_Read_Address(in_buffer);

        for (i = 0; i < dummy_frames; i++)
        {
            retval |= CCP2_clear_irq_status(EOF_IRQ);

            retval |= isif_start(ISIF_START_SYNC);
            retval |= ccp2_start(MEMORY_READ);

            while (!CCP2_get_irq_status(EOF_IRQ)) ;
            retval |= CCP2_clear_irq_status(EOF_IRQ);

            retval |= isif_start(ISIF_STOP);
            retval |= ccp2_disableLCM();
        }
    }
    return retval;
}

/* ===================================================================
 *  @func     issDrvCcpToIsifStop
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
ISP_RETURN issDrvCcpToIsifStop(void)
{
    ISP_RETURN retval = ISP_SUCCESS;

    retval |= issDrvStopIsif(0, 0, 0, NULL);

    while (!CCP2_get_irq_status(EOF_IRQ)) ;
    retval |= CCP2_clear_irq_status(EOF_IRQ);

    return retval;
}
