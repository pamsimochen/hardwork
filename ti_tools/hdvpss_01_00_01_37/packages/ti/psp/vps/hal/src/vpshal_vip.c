/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vip.c
 *
 *  \brief VPS Vip Parser HAL Source file.
 *  This file implements the HAL APIs of the VPS VIP Parser.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/cslr/cslr_vps_vip_parser.h>
#include <ti/psp/cslr/cslr_hd_vps_vpdma.h>

#ifdef TI_816X_BUILD
#include <ti/psp/cslr/soc_TI816x.h>
#endif

#ifdef TI_814X_BUILD
#include <ti/psp/cslr/soc_TI814x.h>
#endif

#ifdef TI_8107_BUILD
#include <ti/psp/cslr/soc_TI8107.h>
#endif

#include <ti/psp/vps/hal/vpshal_vip.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Maximum number of VIP Port instances */
#define VPSHAL_VIP_MAX_PORT_INST    (4u)
/** \brief Maximum number of VIP instances */
#define VPSHAL_VIP_MAX_PER_INST     (2u)

/** \brief Maximum value supported for all the crop parameters - 11-bits */
#define VPSHAL_VIP_MAX_CROP_VALUE   (2047u)
/** \brief Maximum value supported for crop channel number - 4-bits */
#define VPSHAL_VIP_MAX_CROP_CH_NUM  (15u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_VipInfo
 *  \brief Structure containing Instance Information for each VIP Instance
 */
typedef struct
{
    VpsHal_VipInst            instId;
    /**< Vip Instance */

    VpsHal_VipPort            portId;
    /**< Vip Port */

    UInt32                    openCnt;
    /**< Keep track of opening of instance */

    CSL_VpsVipRegsOvly        regs;
    /**< Base address of Vip Register */
    UInt32 virtRegOffset[1];
    /**< Array of indexes into the Overlay memory where virtual registers are
         to be configured */
    UInt32 *regOffset[1];
    /**< Array of physical address of the register configured through
         VPDMA register overlay. */
    UInt32 regOvlySize;
} VpsHal_VipInfo;


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief Instance variable to store information about each vip instance.
 */
static VpsHal_VipInfo  VipPortObj[VPSHAL_VIP_MAX_PORT_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_vipInit
 *  \brief VIP HAL init function.
 *  This function should be called before calling any of VIP HAL API's.
 *
 *  \param numInstances       Number of instances to initialize.
 *  \param initParams         Pointer to structure VpsHal_VipInstParam.
 *  \param arg                Not used currently. Meant for future purpose.
 *
 *  \return                   success    0
 *                            failure    error value.
 */
Int VpsHal_vipInit(UInt32 numInstances,
                   VpsHal_VipInstParams *initParams,
                   Ptr arg)
{
    Int instCnt, ret = 0;
    UInt32 interruptMask = 0x0;

    GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_VIP_MAX_PORT_INST);
    GT_assert( GT_DEFAULT_MASK, initParams != NULL);

    for(instCnt = 0u; instCnt < numInstances; instCnt++)
    {
        VipPortObj[instCnt].openCnt = 0;
        VipPortObj[instCnt].regs =
        (CSL_VpsVipRegsOvly)initParams[instCnt].baseAddress;

        /* Disable all VIP interrupts except overflow interrupts as driver is
         * not handling any of other interrupts */
        interruptMask = 0x0;
        interruptMask |=
            (CSL_VPS_VIP_PARSER_FIQ_STATUS_ASYNC_FIFO_PRTA_STATUS_MASK        |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_ASYNC_FIFO_PRTB_STATUS_MASK         |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_ANC_STATUS_MASK    |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_CHROMA_STATUS_MASK |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_LUMA_STATUS_MASK   |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_ANC_STATUS_MASK    |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_CHROMA_STATUS_MASK |
            CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_LUMA_STATUS_MASK);
        interruptMask = ~interruptMask;
        VipPortObj[instCnt].regs->FIQ_MASK = interruptMask;
        VipPortObj[instCnt].regs->FIQ_CLEAR = 0;
        VipPortObj[instCnt].regs->FIQ_CLEAR = interruptMask;

        /* Set immediately stop bits */
        VipPortObj[instCnt].regs->XTRA6_PORT_A = 0xFFFFFFFFu;
        VipPortObj[instCnt].regs->XTRA7_PORT_B = 0xFFFFFFFFu;

    }

    return(ret);

}

/**
 *  VpsHal_vipDeInit
 *  \brief VIP HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg      Not used currently. Meant for future purpose
 *
 *  \return         success    0
 *                  failure    error value
 */
Int VpsHal_vipDeInit(Ptr arg)
{
    return(0);
}

/**
 *  VpsHal_vipOpen
 *  \brief Returns the handle to the requested Vip instance.
 *  It is allowed to open only one handle for each vip Instance.
 *
 *  This function should be called prior to calling any of the VIP HAL
 *  configuration APIs.
 *
 *  VpsHal_vipInit should be called prior to this
 *
 *  \param vipInst       Vip Instance
 *  \param vipPort       Vip Port

 *  \return              success     VIP handle
 *                       failure     NULL
 */
VpsHal_VipHandle VpsHal_vipOpen(VpsHal_VipInst vipInst,
                                VpsHal_VipPort vipPort)
{
    UInt32                cookie;
    VpsHal_VipHandle      handle = NULL, ret = NULL;
    VpsHal_VipInfo        *instInfo = NULL;
    UInt                  instIndex = (VPSHAL_VIP_MAX_PER_INST *
                              vipInst + vipPort);

    GT_assert( GT_DEFAULT_MASK, instIndex < VPSHAL_VIP_MAX_PORT_INST);

    instInfo = &(VipPortObj[instIndex]);

    /* Enable global interrupts */
    cookie = Hwi_disable();
    if (0 == instInfo->openCnt)
    {
        instInfo->instId = vipInst;
        instInfo->portId = vipPort;
        instInfo->openCnt++;
        handle = (VpsHal_VipHandle)(instInfo);
        ret = handle;
    }
    /* Enable global interrupts */
    Hwi_restore(cookie);

    return(ret);
}

/**
 *  VpsHal_vipClose
 *  \brief Closes the Vip HAL
 *  VpsHal_vipInit and Vip_open should be called prior to this.
 *
 *  \param handle       Valid handle returned by VpsHal_vipOpen function.
 *
 *  \return             success    0
 *                      failure    error value
 */
Int VpsHal_vipClose(VpsHal_VipHandle handle)
{
    Int             ret = -1;
    UInt32          cookie;
    VpsHal_VipInfo  *instInfo;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    instInfo = (VpsHal_VipInfo *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (instInfo->openCnt > 0)
    {
        instInfo->openCnt--;
        ret = 0;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (ret);
}


/**
 *  VpsHal_vipSetVipConfig
 *  \brief Sets the entire Vip configuration to the actual Vip Registers.
 *
 *  enter VpsHal_vipInit and VpsHal_vipOpen should be called prior to this.
 *
 *  \param vipInst             Vip Instance
 *  \param vipConfig           Pointer to VpsHal_VipConfig structure.
 *                             This parameter should be non-NULL.

 *  \return                    success    0
 *                             failure    error value
 */
Int VpsHal_vipSetVipConfig(VpsHal_VipInst vipInst,
                           const VpsHal_VipConfig *vipConfig)
{
    Int                 ret = 0;
    UInt32              tempRegs = 0;
    VpsHal_VipInfo      instInfo;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != vipConfig);
    GT_assert( GT_DEFAULT_MASK, VPSHAL_VIP_INST_MAX != vipInst);

    instInfo = VipPortObj[2*vipInst];

    tempRegs = (((UInt32)vipConfig->vipConfig.clipActive) <<
        CSL_VPS_VIP_PARSER_MAIN_CLIP_ACTIVE_SHIFT) &
        CSL_VPS_VIP_PARSER_MAIN_CLIP_ACTIVE_MASK;

    tempRegs |= (((UInt32)vipConfig->vipConfig.clipBlank) <<
        CSL_VPS_VIP_PARSER_MAIN_CLIP_BLNK_SHIFT) &
        CSL_VPS_VIP_PARSER_MAIN_CLIP_BLNK_MASK;

    tempRegs |= (vipConfig->intfMode <<
        CSL_VPS_VIP_PARSER_MAIN_DATA_INTERFACE_MODE_SHIFT) &
        CSL_VPS_VIP_PARSER_MAIN_DATA_INTERFACE_MODE_MASK;

    instInfo.regs->MAIN = tempRegs;

    return(ret);
}

/**
 *  VpsHal_vipGetVipConfig
 *  \brief Gets the entire Vip configuration from the actual Vip registers.
 *
 *  \param  vipInst          Valid handle returned by VpsHal_vipOpen function.
 *  \param  vipConfig        Pointer to VpsHal_VipConfig structure to be filled
 *                           with register configurations. This parameter
 *                           should be non-NULL.
 *
 *  \return                  success     0
 *                           failure     error value
 */
Int VpsHal_vipGetVipConfig(VpsHal_VipInst vipInst,
                           VpsHal_VipConfig *vipConfig)
{
    Int                 ret = 0;
    UInt32              tempRegs = 0;
    VpsHal_VipInfo      instInfo;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != vipConfig);
    GT_assert( GT_DEFAULT_MASK, VPSHAL_VIP_INST_MAX != vipInst);

    instInfo = VipPortObj[2*vipInst];
    tempRegs = instInfo.regs->MAIN;

    vipConfig->vipConfig.clipActive = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_MAIN_CLIP_ACTIVE_MASK) >>
        CSL_VPS_VIP_PARSER_MAIN_CLIP_ACTIVE_SHIFT);

    vipConfig->vipConfig.clipBlank = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_MAIN_CLIP_BLNK_MASK) >>
        CSL_VPS_VIP_PARSER_MAIN_CLIP_BLNK_SHIFT);

    vipConfig->intfMode = (VpsHal_VipIntfMode)( (tempRegs &
        CSL_VPS_VIP_PARSER_MAIN_DATA_INTERFACE_MODE_MASK) >>
        CSL_VPS_VIP_PARSER_MAIN_DATA_INTERFACE_MODE_SHIFT );

    return(ret);
 }

/**
 *  VpsHal_vipSetPorConfig
 *  \brief Sets the entire port configuration
 *
 *  \param  handle            Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param  portConfig        Pointer to VpsHal_VipConfig structure.
 *                            This parameter should be non-NULL.
 *
 *  \return                   success    0
 *                            failure    error value
 */
Int VpsHal_vipSetPortConfig(VpsHal_VipHandle handle,
                            const VpsHal_VipPortConfig *portConfig)
{
    Int                 ret = 0;
    volatile UInt32     tempRegs = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != portConfig);

    instInfo = (VpsHal_VipInfo *) handle;

    tempRegs|=(((UInt32)portConfig->embConfig.errCorrEnable) <<
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_FVH_ERR_CORRECTION_ENABLE_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_FVH_ERR_CORRECTION_ENABLE_MASK;

    tempRegs|= (portConfig->embConfig.srcNumPos <<
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_2X4X_SRCNUM_POS_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_2X4X_SRCNUM_POS_MASK;

    tempRegs|= (portConfig->disConfig.fidSkewPostCnt <<
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_POSTCOUNT_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_POSTCOUNT_MASK;

    tempRegs|= (portConfig->disConfig. fidSkewPreCnt <<
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_PRECOUNT_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_PRECOUNT_MASK;

    tempRegs|= (portConfig->disConfig.lineCaptureStyle <<
        CSL_VPS_VIP_PARSER_PORT_A_USE_ACTVID_HSYNC_N_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_USE_ACTVID_HSYNC_N_MASK;

    tempRegs|= (portConfig->disConfig.fidDetectMode <<
       CSL_VPS_VIP_PARSER_PORT_A_FID_DETECT_MODE_SHIFT) &
       CSL_VPS_VIP_PARSER_PORT_A_FID_DETECT_MODE_MASK;

    tempRegs|= (portConfig->disConfig.actvidPol <<
        CSL_VPS_VIP_PARSER_PORT_A_ACTVID_POLARITY_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_ACTVID_POLARITY_MASK;

    tempRegs|= (portConfig->disConfig.vsyncPol <<
        CSL_VPS_VIP_PARSER_PORT_A_VSYNC_POLARITY_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_VSYNC_POLARITY_MASK;

    tempRegs|= (portConfig->disConfig.hsyncPol <<
        CSL_VPS_VIP_PARSER_PORT_A_HSYNC_POLARITY_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_HSYNC_POLARITY_MASK;

    tempRegs|= (portConfig->pixClkEdgePol <<
        CSL_VPS_VIP_PARSER_PORT_A_PIXCLK_EDGE_POLARITY_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_PIXCLK_EDGE_POLARITY_MASK;

    tempRegs|= (((UInt32)portConfig->invertFidPol) <<
        CSL_VPS_VIP_PARSER_PORT_A_FID_POLARITY_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_FID_POLARITY_MASK;

    tempRegs|= (((UInt32)portConfig->enablePort) <<
        CSL_VPS_VIP_PARSER_PORT_A_ENABLE_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK;

    tempRegs|= (((UInt32)portConfig->clrAsyncFifoRd) <<
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_RD_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_RD_MASK;

    tempRegs|= (((UInt32)portConfig->clrAsyncFifoWr) <<
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_WR_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_WR_MASK;

    tempRegs|= (portConfig->ctrlChanSel <<
        CSL_VPS_VIP_PARSER_PORT_A_CTRL_CHAN_SEL_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_CTRL_CHAN_SEL_MASK;

    tempRegs|= (portConfig->syncType <<
        CSL_VPS_VIP_PARSER_PORT_A_SYNC_TYPE_SHIFT) &
        CSL_VPS_VIP_PARSER_PORT_A_SYNC_TYPE_MASK;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        tempRegs |= (portConfig->discreteBasicMode <<
            CSL_VPS_VIP_PARSER_PORT_A_DISC_SYNC_BASIC_MODE_SHIFT) &
            CSL_VPS_VIP_PARSER_PORT_A_DISC_SYNC_BASIC_MODE_MASK;

        instInfo->regs->PORT_A= tempRegs;
    }
    else
    {
        tempRegs |= (portConfig->discreteBasicMode <<
            CSL_VPS_VIP_PARSER_PORT_B_DISC_SYNC_BASIC_MODE_SHIFT) &
            CSL_VPS_VIP_PARSER_PORT_B_DISC_SYNC_BASIC_MODE_MASK;

        instInfo->regs->PORT_B = tempRegs;
    }

    tempRegs = 0;
    tempRegs|= (portConfig->ancChSel8b <<
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_ANC_CHAN_SEL_8B_SHIFT) &
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_ANC_CHAN_SEL_8B_MASK;

    tempRegs|= (portConfig->embConfig.isMaxChan3Bits <<
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_TVP5158_CHAN_ID_TYPE_SHIFT) &
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_TVP5158_CHAN_ID_TYPE_MASK;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        instInfo->regs->XTRA_PORT_A = tempRegs;
    }
    else
    {
        instInfo->regs->XTRA_PORT_B = tempRegs;
    }

#ifdef NOT_YET
    /* KC: Discrete sync, disable the VIP parser to look for DE during the
       vblank period - dirty workaround */
    instInfo->regs->PORT_A |= (1<<22);
#endif
    return(ret);
}


/**
 *  VpsHal_vipGetPortConfig
 *  \brief Gets the entire Port configuration from the actual Vip registers.
 *
 *  \param handle            Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param portConfig        Pointer to VpsHal_VipPortConfig structure.
 *                           This parameter should be non-NULL.
 *
 *  \return                  success    0
 *                           failure    error value
 */
Int VpsHal_vipGetPortConfig(VpsHal_VipHandle handle,
                            VpsHal_VipPortConfig *portConfig)
{
    Int                 ret = 0;
    UInt32              tempRegs = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != portConfig);

    instInfo = (VpsHal_VipInfo *) handle;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        tempRegs = instInfo->regs->PORT_A;
    }
    else
    {
         tempRegs = instInfo->regs->PORT_B;
    }

    portConfig->embConfig.errCorrEnable = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_FVH_ERR_CORRECTION_ENABLE_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_FVH_ERR_CORRECTION_ENABLE_SHIFT);

    portConfig->embConfig.srcNumPos = (Vps_VipSrcNumPos)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_2X4X_SRCNUM_POS_MASK)>>
        CSL_VPS_VIP_PARSER_PORT_A_ANALYZER_2X4X_SRCNUM_POS_SHIFT);

    portConfig->disConfig.fidSkewPostCnt = (tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_POSTCOUNT_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_POSTCOUNT_SHIFT;

    portConfig->disConfig.fidSkewPreCnt = (tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_PRECOUNT_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_FID_SKEW_PRECOUNT_SHIFT;

    portConfig->disConfig.lineCaptureStyle =
        (Vps_VipLineCaptureStyle)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_USE_ACTVID_HSYNC_N_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_USE_ACTVID_HSYNC_N_SHIFT);

    portConfig->disConfig.fidDetectMode = (Vps_VipFidDetectMode)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_FID_DETECT_MODE_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_FID_DETECT_MODE_SHIFT);

    portConfig->disConfig.actvidPol =(Vps_VipPolarity)( (tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_ACTVID_POLARITY_MASK)>>
        CSL_VPS_VIP_PARSER_PORT_A_ACTVID_POLARITY_SHIFT);

    portConfig->disConfig.vsyncPol = (Vps_VipPolarity)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_VSYNC_POLARITY_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_VSYNC_POLARITY_SHIFT);

    portConfig->disConfig.hsyncPol = (Vps_VipPolarity)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_HSYNC_POLARITY_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_HSYNC_POLARITY_SHIFT);

    portConfig->pixClkEdgePol = (Vps_VipPixClkEdgePol)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_PIXCLK_EDGE_POLARITY_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_PIXCLK_EDGE_POLARITY_SHIFT);

    portConfig->invertFidPol = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_FID_POLARITY_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_FID_POLARITY_SHIFT);

    portConfig->enablePort = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_ENABLE_SHIFT);

    portConfig->clrAsyncFifoRd = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_RD_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_RD_SHIFT);

    portConfig->clrAsyncFifoWr = (UInt32)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_WR_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_CLR_ASYNC_FIFO_WR_SHIFT);

    portConfig->ctrlChanSel = (Vps_VipCtrlChanSel)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_CTRL_CHAN_SEL_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_CTRL_CHAN_SEL_SHIFT);

    portConfig->syncType = (VpsHal_VipSyncType)((tempRegs &
        CSL_VPS_VIP_PARSER_PORT_A_SYNC_TYPE_MASK) >>
        CSL_VPS_VIP_PARSER_PORT_A_SYNC_TYPE_SHIFT);

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        tempRegs = instInfo->regs->XTRA_PORT_A;
    }
    else
    {
        tempRegs = instInfo->regs->XTRA_PORT_B;
    }

    portConfig->ancChSel8b = (Vps_VipAncChSel8b)((tempRegs &
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_ANC_CHAN_SEL_8B_MASK) >>
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_ANC_CHAN_SEL_8B_SHIFT);

    portConfig->embConfig.isMaxChan3Bits = (tempRegs &
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_TVP5158_CHAN_ID_TYPE_MASK) >>
        CSL_VPS_VIP_PARSER_XTRA_PORT_A_TVP5158_CHAN_ID_TYPE_SHIFT;

    return(ret);
}

/**
 *  VpsHal_vipGetStatus
 *  \brief Get the status of Video Source.
 *
 *  \param handle            Valid handle returned by VpsHal_vipOpen function.
 *  \param sid               Source Id Number.
 *  \param vipStatus         Pointer to VpsHal_VipStatus structure.
 *                           This parameter should be non-NULL
 *
 *  \return                  success     0
 *                           failure     error value.
 */
Int VpsHal_vipGetSrcStatus(UInt32 vpdmaCh,
                        VpsHal_VipStatus *vipStatus)
{
    Int32                ret = 0, vipInstId, vipPortId, sid;
    VpsHal_VipInfo      *instInfo = NULL;

    if(vpdmaCh >= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0
        &&
        vpdmaCh <= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC15
    )
    {
        vipInstId = VPSHAL_VIP_INST_0;
        vipPortId = VPSHAL_VIP_PORT_A;
        sid = vpdmaCh - VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0;
    } else
    if(vpdmaCh >= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC0
        &&
        vpdmaCh <= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC15
    )
    {
        vipInstId = VPSHAL_VIP_INST_0;
        vipPortId = VPSHAL_VIP_PORT_B;
        sid = vpdmaCh - VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC0;
    } else
    if(vpdmaCh >= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0
        &&
        vpdmaCh <= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC15
    )
    {
        vipInstId = VPSHAL_VIP_INST_1;
        vipPortId = VPSHAL_VIP_PORT_A;
        sid = vpdmaCh - VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0;
    } else
    if(vpdmaCh >= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC0
        &&
        vpdmaCh <= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC15
    )
    {
        vipInstId = VPSHAL_VIP_INST_1;
        vipPortId = VPSHAL_VIP_PORT_B;
        sid = vpdmaCh - VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC0;
    }
    else
    {
        /* Invalid VPDMA channel */
        GT_assert(GT_DEFAULT_MASK, FALSE);
    }


    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != vipStatus);
    /*Check for source id*/
    GT_assert( GT_DEFAULT_MASK, sid < 16);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX + vipPortId];

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        vipStatus->isVideoDetect =
        (UInt32)((instInfo->regs->PORT_A_VDET_VEC >> sid) & 0x00000001u);

        vipStatus->prevFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_A_SRC_FID >>
            (2 * sid)) & 0x00000001u);

        vipStatus->curFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_A_SRC_FID >>
            (2 * sid + 1)) & 0x00000001u);

        vipStatus->prevEncFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_A_ENC_FID >>
            (2 * sid)) & 0x00000001u);

        vipStatus->curEncFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_A_ENC_FID >>
            (2 * sid + 1)) & 0x00000001u);

        vipStatus->height =
            (UInt16)((instInfo->regs->OUTPUT_PORT_A_SRC_SIZE[sid] &
             0x000007FFu) >>0x00000000u );

        vipStatus->width =
            (UInt16)((instInfo->regs->OUTPUT_PORT_A_SRC_SIZE[sid] &
            0x07FF0000u) >> 0x00000010u);
    }
    else
    {
        vipStatus->isVideoDetect =
        (UInt32)((instInfo->regs->PORT_B_VDET_VEC >> sid) & 0x00000001u);

        vipStatus->prevFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_B_SRC_FID >>
            (2 * sid)) & 0x00000001u);

        vipStatus->curFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_B_SRC_FID >>
            (2 * sid + 1)) & 0x00000001u);

        vipStatus->prevEncFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_B_ENC_FID >>
            (2 * sid)) & 0x00000001u);

        vipStatus->curEncFid =
            (Vps_VipFid)((instInfo->regs->OUTPUT_PORT_B_ENC_FID >>
            (2 * sid + 1)) & 0x00000001u);

        vipStatus->height =
            (UInt16)((instInfo->regs->OUTPUT_PORT_B_SRC_SIZE[sid] &
             0x000007FFu) >>0x00000000u );

        vipStatus->width =
            (UInt16)((instInfo->regs->OUTPUT_PORT_B_SRC_SIZE[sid] &
             0x07FF0000u) >> 0x00000010u);
    }

    return(ret);
}

/**
 *  VpsHal_vipIsVideoDetect
 *  \brief Check if proper video sync is found.
 *  This function return whether video sync is found properly or not.
 *
 *  \param  handle           Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param  sid              Source Id Number.
 *
 *  \return                  success    1   proper video sync is found
 *                           failure    0   proper video sync is not found
 */
UInt32 VpsHal_vipIsVideoDetect(VpsHal_VipHandle handle,
                             UInt16 sid)
{
    UInt32              ret = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    /*Check for source id*/
    GT_assert( GT_DEFAULT_MASK, sid < 16);

    instInfo = (VpsHal_VipInfo *) handle;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        ret = (UInt32)((instInfo->regs->PORT_A_VDET_VEC >> sid) & 0x00000001u);
    }
    else
    {
        ret = (UInt32)((instInfo->regs->PORT_B_VDET_VEC >> sid) & 0x00000001u);
    }
    return(ret);
}

Int32 VpsHal_vipInstDisable(VpsHal_VipInst vipInstId)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    regVal = CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK;

    instInfo->regs->PORT_A &= ~regVal;
    instInfo->regs->PORT_B &= ~regVal;

    return 0;
}

Int32 VpsHal_vipInstResetFIFO(VpsHal_VipInst vipInstId, Bool assertReset)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    regVal =  CSL_VPS_VIP_PARSER_PORT_B_CLR_ASYNC_FIFO_WR_MASK
            | CSL_VPS_VIP_PARSER_PORT_B_CLR_ASYNC_FIFO_RD_MASK;

    if(assertReset)
    {
        instInfo->regs->PORT_A |= regVal;
        instInfo->regs->PORT_B |= regVal;
    }
    else
    {
        instInfo->regs->PORT_A &= ~regVal;
        instInfo->regs->PORT_B &= ~regVal;
    }

    return 0;
}

Int32 VpsHal_vipInstPortClearFIQ(VpsHal_VipInst vipInstId)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId * VPSHAL_VIP_PORT_MAX];
    regVal   = instInfo->regs->FIQ_STATUS;

    /* Write '0' followed by '1' to clear the interrupt */
    instInfo->regs->FIQ_CLEAR = 0;
    instInfo->regs->FIQ_CLEAR = regVal;

    return 0;
}


Int32 VpsHal_vipInstPortIsOverflow(VpsHal_VipInst vipInstId, volatile UInt32 *isOverflowPortA, volatile UInt32 *isOverflowPortB)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    *isOverflowPortA = FALSE;
    *isOverflowPortB = FALSE;

    regVal = instInfo->regs->FIQ_STATUS;

    if(regVal & CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_LUMA_STATUS_MASK)
    {
        *isOverflowPortA = TRUE;

        /* Write '0' followed by '1' to clear the interrupt */
        instInfo->regs->FIQ_CLEAR = 0;
        instInfo->regs->FIQ_CLEAR = CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_LUMA_STATUS_MASK;
    }
    if(regVal & CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_LUMA_STATUS_MASK)
    {
        *isOverflowPortB = TRUE;

        /* Write '0' followed by '1' to clear the interrupt */
        instInfo->regs->FIQ_CLEAR = 0;
        instInfo->regs->FIQ_CLEAR = CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_LUMA_STATUS_MASK;
    }

    return 0;
}

/*
 *  This API will detect if there is an overflow, disable
 *  the parser if overflow has occured. This API is added
 *  to disable VIP parser as soon as the overflow has occured
 *  This is only called from the Overflow ISR
 */
void VpsHal_vipInstChkOverflowAndDisable(VpsHal_VipInst vipInstId,
                        Bool isBlockRst)
{

    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;
    volatile UInt32 maskVal;
    UInt32 isOverflowPortA;
    UInt32 isOverflowPortB;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    maskVal = CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK;

    isOverflowPortA = FALSE;
    isOverflowPortB = FALSE;

    regVal = instInfo->regs->FIQ_STATUS;

    if(regVal & CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTA_LUMA_STATUS_MASK)
    {
        isOverflowPortA = TRUE;
    }
    if(regVal & CSL_VPS_VIP_PARSER_FIQ_STATUS_OUTPUT_FIFO_PRTB_LUMA_STATUS_MASK)
    {
        isOverflowPortB = TRUE;
    }

    if(TRUE == isBlockRst)
    {
         if((TRUE == isOverflowPortA) ||
            (TRUE == isOverflowPortB))
         {
            isOverflowPortA = TRUE;
            isOverflowPortB = TRUE;
         }

    }

    /*Disable the VIP parser Ports if required*/

    if(TRUE == isOverflowPortA)
    {
           instInfo->regs->PORT_A &= ~maskVal;
    }

    if(TRUE == isOverflowPortB)
    {
           instInfo->regs->PORT_B &= ~maskVal;
    }

}

Int32 VpsHal_vipInstPortEnable(VpsHal_VipInst vipInstId, VpsHal_VipPort vipPortId, Bool enable, UInt32 *cfgOvlyPtr)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    regVal =  CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK;

    if(enable)
    {
        /* clear FIQ bits before enable */
        instInfo->regs->FIQ_CLEAR = 0xFFFFFFFF;
        instInfo->regs->FIQ_CLEAR = 0;

        if (NULL == cfgOvlyPtr)
        {
            if(vipPortId==VPSHAL_VIP_PORT_A)
                instInfo->regs->PORT_A |= regVal;
            else
                instInfo->regs->PORT_B |= regVal;
        }
        else
        {
            if(vipPortId==VPSHAL_VIP_PORT_A)
                regVal |= instInfo->regs->PORT_A;
            else
                regVal |= instInfo->regs->PORT_B;

            *(((UInt32*) cfgOvlyPtr) + 4) = regVal;
        }
    }
    else
    {
        if (NULL == cfgOvlyPtr)
        {
            if(vipPortId==VPSHAL_VIP_PORT_A)
                instInfo->regs->PORT_A &= ~regVal;
            else
                instInfo->regs->PORT_B &= ~regVal;
        }
        else
        {
            if(vipPortId==VPSHAL_VIP_PORT_A)
                regVal = instInfo->regs->PORT_A & (~regVal);
            else
                regVal = instInfo->regs->PORT_B & (~regVal);

            *(((UInt32*) cfgOvlyPtr) + 4) = regVal;
        }
    }

    return 0;
}

UInt32 VpsHal_vipInstPortEnableGetCfgOvlySize(VpsHal_VipInst vipInstId, VpsHal_VipPort vipPortId)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    return (instInfo->regOvlySize);
}

Void VpsHal_vipInstPortEnableCreateOvly(VpsHal_VipInst vipInstId,
                                        VpsHal_VipPort vipPortId,
                                        UInt32 *cfgOvlyPtr)
{
    VpsHal_VipInfo  *instInfo;
    volatile UInt32 regVal;

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    instInfo = &VipPortObj[vipInstId*VPSHAL_VIP_PORT_MAX];

    if(vipPortId==VPSHAL_VIP_PORT_A)
    {
        instInfo->regOffset[0] = (UInt32 *) &(instInfo->regs->PORT_A);
    }
    else
    {
        instInfo->regOffset[0] = (UInt32 *) &(instInfo->regs->PORT_B);
    }

    instInfo->regOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                            instInfo->regOffset,
                            1,
                            instInfo->virtRegOffset);

    VpsHal_vpdmaCreateRegOverlay(instInfo->regOffset, 1, (Ptr)cfgOvlyPtr);
}

Int32 VpsHal_vipPortEnable(VpsHal_VipHandle handle, UInt32 enable)
{
    Int                 ret = 0;
    UInt32              tempRegs = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    instInfo = (VpsHal_VipInfo *) handle;

    tempRegs = CSL_VPS_VIP_PARSER_PORT_A_ENABLE_MASK;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
      if(enable)
        instInfo->regs->PORT_A |= tempRegs;
      else
        instInfo->regs->PORT_A &= ~tempRegs;
    }
    else
    {
      if(enable)
        instInfo->regs->PORT_B |= tempRegs;
      else
        instInfo->regs->PORT_B &= ~tempRegs;
    }

    return(ret);
}

Int32 VpsHal_vipPortReset(VpsHal_VipHandle handle, UInt32 enable)
{
    Int                 ret = 0;
    UInt32              tempRegs = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    instInfo = (VpsHal_VipInfo *) handle;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        tempRegs = CSL_VPS_VIP_PARSER_PORT_A_SW_RESET_MASK;
        if(enable)
        {
            instInfo->regs->PORT_A |= tempRegs;
        }
        else
        {
            instInfo->regs->PORT_A &= ~tempRegs;
        }
    }
    else
    {
        tempRegs = CSL_VPS_VIP_PARSER_PORT_B_SW_RESET_MASK;
        if(enable)
        {
            instInfo->regs->PORT_B |= tempRegs;
        }
        else
        {
            instInfo->regs->PORT_B &= ~tempRegs;
        }
    }

    return (ret);
}

UInt32 VpsHal_vipPortGetCompleteStatus(VpsHal_VipHandle handle)
{
    UInt32               retVal = 0;
    VpsHal_VipInfo      *instInfo = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    instInfo = (VpsHal_VipInfo *) handle;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        if (instInfo->regs->FIQ_STATUS &
                CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_A_CFG_DISABLE_COMPLETE_MASK)
        {
            retVal = 1u;
        }
        else
        {
            retVal = 0u;
        }
    }
    else
    {
        if (instInfo->regs->FIQ_STATUS &
                CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_B_CFG_DISABLE_COMPLETE_MASK)
        {
            retVal = 1u;
        }
        else
        {
            retVal = 0u;
        }
    }

    return (retVal);
}

Void VpsHal_vipPortClearCompleteStatus(VpsHal_VipHandle handle)
{
    VpsHal_VipInfo      *instInfo = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    instInfo = (VpsHal_VipInfo *) handle;

    if(instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        instInfo->regs->FIQ_CLEAR |=
            CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_A_CFG_DISABLE_COMPLETE_MASK;
        instInfo->regs->FIQ_CLEAR &=
            ~CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_A_CFG_DISABLE_COMPLETE_MASK;
    }
    else
    {
        instInfo->regs->FIQ_CLEAR |=
            CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_B_CFG_DISABLE_COMPLETE_MASK;
        instInfo->regs->FIQ_CLEAR &=
            ~CSL_VPS_VIP_PARSER_FIQ_STATUS_PORT_B_CFG_DISABLE_COMPLETE_MASK;
    }
}

Int32 VpsHal_vipSetVipActCropCfg(VpsHal_VipHandle handle,
                                 const VpsHal_VipCropConfig *vipHalCropCfg)
{
    Int32              retVal = VPS_SOK;
    VpsHal_VipInfo     *instInfo;
    UInt32              tempRegsX = 0, tempRegsY = 0;

    GT_assert(GT_DEFAULT_MASK, (NULL != handle));
    GT_assert(GT_DEFAULT_MASK, (NULL != vipHalCropCfg));

    instInfo = (VpsHal_VipInfo *) handle;

    /* Check for valid range */
    if ((vipHalCropCfg->channelNum > VPSHAL_VIP_MAX_CROP_CH_NUM) ||
        (vipHalCropCfg->vipCropCfg.cropStartX > VPSHAL_VIP_MAX_CROP_VALUE) ||
        (vipHalCropCfg->vipCropCfg.cropStartY > VPSHAL_VIP_MAX_CROP_VALUE) ||
        (vipHalCropCfg->vipCropCfg.cropWidth > VPSHAL_VIP_MAX_CROP_VALUE) ||
        (vipHalCropCfg->vipCropCfg.cropHeight > VPSHAL_VIP_MAX_CROP_VALUE))
    {
        retVal = FVID2_EOUT_OF_RANGE;
    }

    if (retVal == VPS_SOK)
    {
        tempRegsX = (vipHalCropCfg->channelNum <<
                        CSL_VPS_VIP_PARSER_X_CROP_CFG_SRC_NUM_CFG_SHIFT);
        tempRegsX |= (vipHalCropCfg->vipCropCfg.cropStartX <<
                        CSL_VPS_VIP_PARSER_X_CROP_CFG_SKIP_NUM_PIX_SHIFT);
        tempRegsX |= (vipHalCropCfg->vipCropCfg.cropWidth <<
                        CSL_VPS_VIP_PARSER_X_CROP_CFG_USE_NUM_PIX_SHIFT);
        if (TRUE == vipHalCropCfg->vipCropEnable)
        {
            tempRegsX |= CSL_VPS_VIP_PARSER_X_CROP_CFG_BYPASS_CROP_MASK;
        }

        tempRegsY = (vipHalCropCfg->vipCropCfg.cropStartY <<
                        CSL_VPS_VIP_PARSER_Y_CROP_CFG_SKIP_NUM_LINES_SHIFT);
        tempRegsY |= (vipHalCropCfg->vipCropCfg.cropHeight <<
                        CSL_VPS_VIP_PARSER_Y_CROP_CFG_USE_NUM_LINES_SHIFT);

        if (instInfo->portId == VPSHAL_VIP_PORT_A)
        {
            instInfo->regs->PORT_A_ACT_X_CROP_CFG = tempRegsX;
            instInfo->regs->PORT_A_ACT_Y_CROP_CFG = tempRegsY;
        }
        else
        {
            instInfo->regs->PORT_B_ACT_X_CROP_CFG = tempRegsX;
            instInfo->regs->PORT_B_ACT_Y_CROP_CFG = tempRegsY;
        }
    }

    return (retVal);
}

Int32 VpsHal_vipGetVipActCropCfg(VpsHal_VipHandle handle,
                                 VpsHal_VipCropConfig *vipHalCropCfg)
{
    UInt32      retVal = VPS_SOK;
    VpsHal_VipInfo     *instInfo;
    UInt32              tempRegsX = 0, tempRegsY = 0;

    GT_assert(GT_DEFAULT_MASK, (NULL != handle));
    GT_assert(GT_DEFAULT_MASK, (NULL != vipHalCropCfg));

    instInfo = (VpsHal_VipInfo *) handle;

    if (instInfo->portId == VPSHAL_VIP_PORT_A)
    {
        tempRegsX = instInfo->regs->PORT_A_ACT_X_CROP_CFG;
        tempRegsY = instInfo->regs->PORT_A_ACT_Y_CROP_CFG;
    }
    else
    {
        tempRegsX = instInfo->regs->PORT_B_ACT_X_CROP_CFG;
        tempRegsY = instInfo->regs->PORT_B_ACT_Y_CROP_CFG;
    }

    vipHalCropCfg->channelNum =
        ((tempRegsX & CSL_VPS_VIP_PARSER_X_CROP_CFG_SRC_NUM_CFG_MASK) >>
            CSL_VPS_VIP_PARSER_X_CROP_CFG_SRC_NUM_CFG_SHIFT);
    vipHalCropCfg->vipCropCfg.cropStartX =
        ((tempRegsX & CSL_VPS_VIP_PARSER_X_CROP_CFG_SKIP_NUM_PIX_MASK) >>
            CSL_VPS_VIP_PARSER_X_CROP_CFG_SKIP_NUM_PIX_SHIFT);
    vipHalCropCfg->vipCropCfg.cropWidth =
        ((tempRegsX & CSL_VPS_VIP_PARSER_X_CROP_CFG_USE_NUM_PIX_MASK) >>
            CSL_VPS_VIP_PARSER_X_CROP_CFG_USE_NUM_PIX_SHIFT);
    if (tempRegsX & CSL_VPS_VIP_PARSER_X_CROP_CFG_BYPASS_CROP_MASK)
    {
        vipHalCropCfg->vipCropEnable = TRUE;
    }
    else
    {
        vipHalCropCfg->vipCropEnable = FALSE;
    }

    vipHalCropCfg->vipCropCfg.cropStartY =
        ((tempRegsY & CSL_VPS_VIP_PARSER_Y_CROP_CFG_SKIP_NUM_LINES_MASK) >>
            CSL_VPS_VIP_PARSER_Y_CROP_CFG_SKIP_NUM_LINES_SHIFT);
    vipHalCropCfg->vipCropCfg.cropHeight =
        ((tempRegsY & CSL_VPS_VIP_PARSER_Y_CROP_CFG_USE_NUM_LINES_MASK) >>
            CSL_VPS_VIP_PARSER_Y_CROP_CFG_USE_NUM_LINES_SHIFT);

    return (retVal);
}

Int32 VpsHal_vipPrintDebugInfo()
{
    CSL_VpsVipRegsOvly vipRegs;
    CSL_VpsVpdmaRegsOvly vpdmaRegs;

    Vps_printf(" \r\n");
    Vps_printf(" VIP and VPDMA registers, \r\n");

    vipRegs = (CSL_VpsVipRegsOvly)CSL_VPS_VIP0_PARSER_0_REGS;

    Vps_printf(" VIP0 : FIQ_STATUS  : 0x%08x = 0x%08x\r\n", &vipRegs->FIQ_STATUS, vipRegs->FIQ_STATUS);

    vipRegs = (CSL_VpsVipRegsOvly)CSL_VPS_VIP1_PARSER_0_REGS;

    Vps_printf(" VIP1 : FIQ_STATUS  : 0x%08x = 0x%08x\r\n", &vipRegs->FIQ_STATUS, vipRegs->FIQ_STATUS);

    vpdmaRegs = (CSL_VpsVpdmaRegsOvly)CSL_VPS_VPDMA_0_REGS;

    Vps_printf(" VPDMA: LIST_BUSY   : 0x%08x = 0x%08x\r\n", &vpdmaRegs->LIST_STAT_SYNC, vpdmaRegs->LIST_STAT_SYNC);

    VpsHal_vpdmaPrintPerfMonRegs();

    Vps_printf(" \r\n");

    return 0;
}

/**
 *  VpsHal_vipGetFidRegAddr
 *  \brief Function used to return the physical address of FID register.
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */
Int32 VpsHal_vipGetFidRegAddr(VpsHal_VipHandle handle,
                              volatile UInt32 **reg,
                              UInt32 *mask,
                              UInt32 *shift)
{
    VpsHal_VipInfo        *instInfo = NULL;

    instInfo = (VpsHal_VipHandle)(handle);

    if ( instInfo->portId == VPSHAL_VIP_PORT_A )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_A_SRC_FID);
        *mask =
        CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC_FID_PRTA_SRC0_CURR_SOURCE_FID_MASK;
        *shift =
        CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC_FID_PRTA_SRC0_CURR_SOURCE_FID_SHIFT;
    }
    else if ( instInfo->portId == VPSHAL_VIP_PORT_B )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_B_SRC_FID);
        *mask =
        CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC_FID_PRTB_SRC0_CURR_SOURCE_FID_MASK;
        *shift =
        CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC_FID_PRTB_SRC0_CURR_SOURCE_FID_SHIFT;
    }
    else
    {
        return (-1);
    }

    return ( 0x0 );
}

/**
 *  VpsHal_vipGetSrcHeightRegAddr
 *  \brief Function used to return physical address of source height register.
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */
Int32 VpsHal_vipGetSrcHeightRegAddr(VpsHal_VipHandle handle,
                                  volatile UInt32 **reg,
                                  UInt32 *mask,
                                  UInt32 *shift)
{
    VpsHal_VipInfo        *instInfo = NULL;

    instInfo = (VpsHal_VipHandle)(handle);

    if ( instInfo->portId == VPSHAL_VIP_PORT_A )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_A_SRC_SIZE[0]);
        *mask =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC0_SIZE_PRTA_SRC0_HEIGHT_MASK;
        *shift =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC0_SIZE_PRTA_SRC0_HEIGHT_SHIFT;

    }
    else if ( instInfo->portId == VPSHAL_VIP_PORT_B )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_B_SRC_SIZE[0]);
        *mask =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC0_SIZE_PRTB_SRC0_HEIGHT_MASK;
        *shift =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC0_SIZE_PRTB_SRC0_HEIGHT_SHIFT;
    }
    else
    {
        return (-1);
    }

    return ( 0x0 );
}

/**
 *  VpsHal_vipGetSrcWidthRegAddr
 *  \brief Function used to return the physical address of source width register.
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */
Int32 VpsHal_vipGetSrcWidthRegAddr(VpsHal_VipHandle handle,
                                  volatile UInt32 **reg,
                                  UInt32 *mask,
                                  UInt32 *shift)
{
    VpsHal_VipInfo        *instInfo = NULL;

    instInfo = (VpsHal_VipHandle)(handle);

    if ( instInfo->portId == VPSHAL_VIP_PORT_A )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_A_SRC_SIZE[0]);
        *mask =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC0_SIZE_PRTA_SRC0_WIDTH_MASK;
        *shift =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_A_SRC0_SIZE_PRTA_SRC0_WIDTH_SHIFT;

    }
    else if ( instInfo->portId == VPSHAL_VIP_PORT_B )
    {
        *reg = &(instInfo->regs->OUTPUT_PORT_B_SRC_SIZE[0]);
        *mask =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC0_SIZE_PRTB_SRC0_WIDTH_MASK;
        *shift =
            CSL_VPS_VIP_PARSER_OUTPUT_PORT_B_SRC0_SIZE_PRTB_SRC0_WIDTH_SHIFT;
    }
    else
    {
        return (-1);
    }

    return ( 0x0 );
}
