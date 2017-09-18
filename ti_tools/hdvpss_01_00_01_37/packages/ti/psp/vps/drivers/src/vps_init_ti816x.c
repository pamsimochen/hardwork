/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_init_ti816x.c
 *
 * \brief Vps initialization file for TI816X platform
 * This file implements Vps initialization and de-initialization APIs
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>
#ifdef PROFILE
#include <ti/sysbios/knl/Clock.h>
#endif

#include <ti/psp/cslr/soc_TI816x.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>

#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/cslr/cslr_vps_vip_parser.h>
#include <ti/psp/cslr/cslr_hd_vps_vpdma.h>
#include <ti/psp/cslr/cslr_hd_vps.h>

#include <ti/psp/platforms/vps_platform.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/vps/vps_m2mComp.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/common/vps_utils.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_chrus.h>
#include <ti/psp/vps/hal/vpshal_cig.h>
#include <ti/psp/vps/hal/vpshal_comp.h>
#include <ti/psp/vps/hal/vpshal_compr.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_dcompr.h>
#include <ti/psp/vps/hal/vpshal_dei.h>
#include <ti/psp/vps/hal/vpshal_deih.h>
#include <ti/psp/vps/hal/vpshal_drn.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_ede.h>
#include <ti/psp/vps/hal/vpshal_nf.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_vcomp.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/hal/vpshal_cproc.h>
#include <ti/psp/vps/hal/vpshal_rf.h>
#include <ti/psp/vps/hal/vpshal_onChipEncOps.h>

#include <ti/psp/vps/drivers/display/vps_dlm.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/drivers/vps_init.h>

#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>

#include <ti/psp/vps/hal/vpshal_hdvenc.h>
#include <ti/psp/vps/hal/vpshal_sdvenc.h>

#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/vps/core/vpscore_deiWrbkPath.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/vpscore_graphics.h>
#include <ti/psp/vps/core/vpscore_scWrbkPath.h>
#include <ti/psp/vps/core/vpscore_secPath.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/core/vpscore_vip.h>
#include <ti/psp/vps/core/vpscore_vip.h>
#include <ti/psp/vps/core/vpscore_cscCig.h>

#include <ti/psp/vps/drivers/display/vpsdrv_display.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSc.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/vps/drivers/capture/vpsdrv_capture.h>
#include <ti/psp/vps/drivers/m2mNsf/vpsdrv_m2mNsf.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mDei.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mComp.h>
#include <ti/psp/vps/drivers/display/vpsdrv_graphics.h>

#define __DEBUG__

#ifdef __DEBUG__
#define VPSI_PRINT1(a)          Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a);
#define VPSI_PRINT2(a, b)       Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b);
#define VPSI_PRINT3(a, b, c)    Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b, c);
#define VPSI_PRINT4(a, b, c, d) Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b, c, d);

#define VPSI_ENTER_FUNC         Vps_printf("%s (%d): E", __FUNCTION__,      \
                                __LINE__);
#define VPSI_EXIT_FUNC          Vps_printf("%s (%d): L", __FUNCTION__,      \
                                __LINE__);
#else
#define VPSI_PRINT1(a)
#define VPSI_PRINT2(a, b)
#define VPSI_PRINT3(a, b, c)
#define VPSI_PRINT4(a, b, c, d)
#define VPSI_ENTER_FUNC
#define VPSI_EXIT_FUNC
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VCORE_BP_MAX_HANDLE             (4u)
#define VCORE_DEI_MAX_HANDLE            (4u)
#define VCORE_DWP_MAX_HANDLE            (4u)
#define VCORE_GRPX_MAX_HANDLE           (4u)

#define DDRV_NUM_INST                   (5u)
#define DDRV_DEI_NUM_INST               (2u)
#define GDRV_GRPX_NUM_INST              (3u)
#define MDRV_DEI_NUM_INST               (6u)
#define MDRV_COMP_NUM_INST              (2u)


#define RF_DAC_CTRL_REG_ADDR            (CSL_TI816x_CTRL_MODULE_BASE + \
                                            0x0684)
#define HD_DAC_CTRL_REG_ADDR            (CSL_TI816x_CTRL_MODULE_BASE + \
                                            0x0660)
#define SD_DAC_CTRL_REG_ADDR            (CSL_TI816x_CTRL_MODULE_BASE + \
                                            0x0670)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

static Int32 vpsiHalInit(void);
static Int32 vpsiHalDeInit(void);
static Int32 vpsiCoreInit(void);
static Int32 vpsiCoreDeInit(void);
static Int32 vpsiCommonInit(void);
static Int32 vpsiCommonDeInit(void);
static Int32 vpsiDCtrlInit(void);
static Int32 vpsiDCtrlDeInit(void);
static Int32 vpsiDriverInit(void);
static Int32 vpsiDriverDeInit(void);

static Int32 vpsiHalOpen(void);
static Int32 vpsiHalClose(void);

Int32 RemoteDebug_init(void);
Int32 RemoteDebug_deInit(void);

/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

VpsHal_Handle ScHandle[CSL_VPS_SC_PER_CNT] = {NULL, NULL, NULL, NULL, NULL};
VpsHal_Handle CprocHandle = NULL;
VpsHal_Handle VcompHandle = NULL;
VpsHal_Handle EdeHandle = NULL;
VpsHal_Handle CompHandle = NULL;
VpsHal_Handle CscHandles[CSL_VPS_CSC_PER_CNT] = {NULL, NULL, NULL, NULL, NULL};
VpsHal_Handle CigHandle = NULL;
VpsHal_Handle ChrusHandle[CSL_VPS_CHRUS_PER_CNT] = {NULL, NULL, NULL, NULL};
VpsHal_VipHandle VipHandle[CSL_VPS_VIP_PORT_PER_CNT] = {NULL, NULL, NULL, NULL};
VpsHal_Handle DrnHandle[CSL_VPS_DRN_PER_CNT] = {NULL, NULL};
VpsHal_Handle DeiHandle[CSL_VPS_DEI_PER_CNT] = {NULL};
VpsHal_Handle DeihHandle[CSL_VPS_DEI_HQ_PER_CNT] = {NULL};
VpsHal_Handle ComprHandle[CSL_VPS_COMPR_PER_CNT] = {NULL, NULL};
VpsHal_Handle DcomprHandle[CSL_VPS_DCOMPR_PER_CNT] = {NULL, NULL, NULL, NULL, NULL};
VpsHal_Handle NfHandle = NULL;
VpsHal_Handle SdVencHandle = NULL;
VpsHal_Handle HdVencHandles[CSL_VPS_HD_VENC_PER_CNT] = {NULL, NULL, NULL};
VpsHal_Handle GrpxHandles[CSL_VPS_GRPX_PER_CNT] = {NULL,NULL,NULL};

CSL_VpsRegsOvly  gVpsHal_regsVps   = (CSL_VpsRegsOvly)CSL_TI816x_VPS_BASE;
CSL_VpsVpdmaRegsOvly  gVpsHal_regsVpdma = (CSL_VpsVpdmaRegsOvly)CSL_VPS_VPDMA_0_REGS;
CSL_VpsVipRegsOvly    gVpsHal_regsVip0  = (CSL_VpsVipRegsOvly)CSL_VPS_VIP0_PARSER_0_REGS;
CSL_VpsVipRegsOvly    gVpsHal_regsVip1  = (CSL_VpsVipRegsOvly)CSL_VPS_VIP1_PARSER_0_REGS;
CSL_VpsNfRegsOvly     gVpsHal_regsNf    = (CSL_VpsNfRegsOvly)CSL_VPS_NF_0_REGS;

/* Chroma Upsampler Init Time Parameters */
VpsHal_ChrusInstParams chrusInstParams[CSL_VPS_CHRUS_PER_CNT] = {
    {VPSHAL_CHRUS_INST_PRI0, CSL_VPS_CHR_US_PRI0_0_REGS},
    {VPSHAL_CHRUS_INST_AUX, CSL_VPS_CHR_US_AUX_0_REGS},
    {VPSHAL_CHRUS_INST_SEC0, CSL_VPS_CHR_US_SEC0_0_REGS},
    {VPSHAL_CHRUS_INST_SEC1, CSL_VPS_CHR_US_SEC1_0_REGS}
};

/* CIG Init Time Parameters */
VpsHal_CigInitParams cigInstParams[CSL_VPS_CIG_PER_CNT] = {
    {VPSHAL_CIG_INST_0, CSL_VPS_CIG_0_REGS}
};

/* Blender Init Parameters */
VpsHal_CompInstParams blendInstParams[CSL_VPS_COMP_PER_CNT] = {
    {VPSHAL_COMP_INST_0, CSL_VPS_COMP_0_REGS}
};
/* COMPR Init time Parameters */
VpsHal_ComprInitParams comprInitParams[CSL_VPS_COMPR_PER_CNT] = {
    {VPSHAL_COMPR_INST_PRI_TOP, CSL_VPS_COMPR_PRI_TOP_0_REGS},
    {VPSHAL_COMPR_INST_PRI_BOT, CSL_VPS_COMPR_PRI_BOT_0_REGS},
    {VPSHAL_COMPR_INST_AUX, CSL_VPS_COMPR_AUX_0_REGS},
};
/* DCOMPR Init time Parameters */
VpsHal_DcomprInitParams dcomprInitParams[CSL_VPS_DCOMPR_PER_CNT] = {
    {VPSHAL_DCOMPR_INST_PRI_FLD1, CSL_VPS_DECOMPR_PRI_FLD1_0_REGS},
    {VPSHAL_DCOMPR_INST_PRI_FLD2, CSL_VPS_DECOMPR_PRI_FLD2_0_REGS},
    {VPSHAL_DCOMPR_INST_PRI_FLD3, CSL_VPS_DECOMPR_PRI_FLD3_0_REGS},
    {VPSHAL_DCOMPR_INST_AUX_FLD1, CSL_VPS_DECOMPR_AUX_FLD1_0_REGS},
    {VPSHAL_DCOMPR_INST_AUX_FLD2, CSL_VPS_DECOMPR_AUX_FLD2_0_REGS},
};
/* CSC Init Time Parameters */
VpsHal_CscInstParams cscInstParams[CSL_VPS_CSC_PER_CNT] = {
    {VPSHAL_CSC_INST_HD1, CSL_VPS_CSC_HD1_0_REGS},
    {VPSHAL_CSC_INST_SD, CSL_VPS_CSC_SD_0_REGS},
    {VPSHAL_CSC_INST_WB2, CSL_VPS_CSC_WB2_0_REGS},
    {VPSHAL_CSC_INST_VIP0, CSL_VPS_CSC_VIP0_0_REGS},
    {VPSHAL_CSC_INST_VIP1, CSL_VPS_CSC_VIP1_0_REGS}
};

/* DEI Init Time Parameters */
VpsHal_DeiInitParams deiInstParams[CSL_VPS_DEI_PER_CNT] = {
    {VPSHAL_DEI_INST_0, CSL_VPS_DEI_0_REGS}
};
/* DEI_H Init Time Parameters */
VpsHal_DeihInitParams deihInstParams[CSL_VPS_DEI_HQ_PER_CNT] = {
    {VPSHAL_DEIH_INST_0, CSL_VPS_DEI_HQ_0_REGS}
};
/* DRN Init Time Parameters */
VpsHal_DrnInstParams drnInstParams[CSL_VPS_DRN_PER_CNT] = {
    {VPSHAL_DRN_INST_PRI, CSL_VPS_DRN_PRI_0_REGS},
    {VPSHAL_DRN_INST_AUX, CSL_VPS_DRN_AUX_0_REGS}
};
/* VPS Init Time Parameters */
VpsHal_VpsInstParams vpsInstParams[CSL_VPS_PER_CNT] = {
    {CSL_VPS_INTC_0_REGS}
};
/* EDE Init Time Parameters */
VpsHal_EdeInitParams edeInstParams[CSL_VPS_EDE_PER_CNT] = {
    {VPSHAL_EDE_INST_0, CSL_VPS_EDE_0_REGS}
};
/* HDVENC Init Time Parameters */
VpsHal_HdVencInstParams hdvencInstParams[CSL_VPS_HD_VENC_PER_CNT] = {
    {VPSHAL_HDVENC_HDMI, CSL_VPS_HDMI_VENC_0_REGS, 0u, 0u},
    {VPSHAL_HDVENC_DVO2, CSL_VPS_DVO2_VENC_0_REGS, 0u, 0u},
    {VPSHAL_HDVENC_HDCOMP, CSL_VPS_HDCOMP_VENC_0_REGS, 0u, 0u}
};

/* NF Init Time Parameters */
VpsHal_NfInstParams nfInstParams[CSL_VPS_NF_PER_CNT] = {
    {VPSHAL_NF_INST_0, CSL_VPS_NF_0_REGS}
};

/* SC Init Time Parameters */
VpsHal_ScInstParams scInstParams[CSL_VPS_SC_PER_CNT] = {
    {VPSHAL_SC_INST_1, CSL_VPS_SC_1_REGS, VPSHAL_VPDMA_CONFIG_DEST_SC1, TRUE},
    {VPSHAL_SC_INST_2, CSL_VPS_SC_2_REGS, VPSHAL_VPDMA_CONFIG_DEST_SC2, FALSE},
    {VPSHAL_SC_INST_5, CSL_VPS_SC_5_REGS, VPSHAL_VPDMA_CONFIG_DEST_SC5, FALSE},
    {VPSHAL_SC_INST_3, CSL_VPS_SC_3_REGS, VPSHAL_VPDMA_CONFIG_DEST_SC3, FALSE},
    {VPSHAL_SC_INST_4, CSL_VPS_SC_4_REGS, VPSHAL_VPDMA_CONFIG_DEST_SC4, FALSE}
};

/* SDVENC Init Time Parameters */
VpsHal_SdVencInstParams sdvencInstParams[CSL_VPS_SD_VENC_PER_CNT] = {
    {VPSHAL_SDVENC_INST_0, CSL_VPS_SD_VENC_0_REGS, 0x01u,\
        VPS_DC_ENCODER_RF}
};

/* VCOMP Init Time Parameters */
VpsHal_VcompInstParams vcompInstParams[CSL_VPS_VCOMP_PER_CNT] = {
    {VPSHAL_VCOMP_INST_0, CSL_VPS_VCOMP_0_REGS}
};

/* VIP Init Time Parameters */
VpsHal_VipInstParams vipInstParams[CSL_VPS_VIP_PORT_PER_CNT] = {
    {CSL_VPS_VIP0_PARSER_0_REGS},
    {CSL_VPS_VIP0_PARSER_0_REGS},
    {CSL_VPS_VIP1_PARSER_0_REGS},
    {CSL_VPS_VIP1_PARSER_0_REGS}
};

/* VPDMA Init Time Parameters */
VpsHal_VpdmaInstParams  vpdmaInstParams = {CSL_VPS_VPDMA_0_REGS,
        CSL_TI816x_VPS_BASE};

/*GRPX Init Time Parameters */
VpsHal_GrpxInstParams grpxInstParams[CSL_VPS_GRPX_PER_CNT] = {
    {VPSHAL_GRPX_INST_0},
    {VPSHAL_GRPX_INST_1},
    {VPSHAL_GRPX_INST_2}
};
/* cproc Init Time Parameters */
VpsHal_CprocInstParams cprocInstParams[CSL_VPS_CPROC_PER_CNT] = {
    {VPSHAL_CPROC_INST_0, CSL_VPS_CPROC_0_REGS}
};
Vcore_BpInitParams bpInitParams[] = {
    {VCORE_BP_INST_0, VCORE_BP_MAX_HANDLE, {VPSHAL_VPDMA_CHANNEL_BP0},
        VPSHAL_VPDMA_PATH_BP0},
    {VCORE_BP_INST_1, VCORE_BP_MAX_HANDLE, {VPSHAL_VPDMA_CHANNEL_BP1},
        VPSHAL_VPDMA_PATH_BP1}
};

Vcore_DeiInitParams deiInitParams[] = {
    {VCORE_DEI_INST_PRI,
     VCORE_DEI_MAX_HANDLE,
     TRUE,
     {
         VPSHAL_VPDMA_CHANNEL_PRI_FLD1_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD1_CHROMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD2_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD2_CHROMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD3_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD3_CHROMA,

         VPSHAL_VPDMA_CHANNEL_PRI_MV0,
         VPSHAL_VPDMA_CHANNEL_PRI_MV1,

         VPSHAL_VPDMA_CHANNEL_PRI_MVSTM,

         VPSHAL_VPDMA_CHANNEL_PRI_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_CHROMA,

         VPSHAL_VPDMA_CHANNEL_PRI_FLD_WR_TOP_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD_WR_TOP_CHROMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD_WR_BOT_LUMA,
         VPSHAL_VPDMA_CHANNEL_PRI_FLD_WR_BOT_CHROMA,

         VPSHAL_VPDMA_CHANNEL_PRI_MV_OUT,
         VPSHAL_VPDMA_CHANNEL_PRI_MVSTM_OUT,
     },
     {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
     VPSHAL_VPDMA_PATH_PRI},
    {VCORE_DEI_INST_AUX,
     VCORE_DEI_MAX_HANDLE,
     FALSE,
     {
         VPSHAL_VPDMA_CHANNEL_AUX_FLD1_LUMA,
         VPSHAL_VPDMA_CHANNEL_AUX_FLD1_CHROMA,
         VPSHAL_VPDMA_CHANNEL_AUX_FLD2_LUMA,
         VPSHAL_VPDMA_CHANNEL_AUX_FLD2_CHROMA,
         VPSHAL_VPDMA_CHANNEL_INVALID,
         VPSHAL_VPDMA_CHANNEL_INVALID,

         VPSHAL_VPDMA_CHANNEL_AUX_MV,
         VPSHAL_VPDMA_CHANNEL_INVALID,

         VPSHAL_VPDMA_CHANNEL_INVALID,

         VPSHAL_VPDMA_CHANNEL_AUX_LUMA,
         VPSHAL_VPDMA_CHANNEL_AUX_CHROMA,
         VPSHAL_VPDMA_CHANNEL_AUX_FLD_WR_LUMA,
         VPSHAL_VPDMA_CHANNEL_AUX_FLD_WR_CHROMA,
         VPSHAL_VPDMA_CHANNEL_INVALID,
         VPSHAL_VPDMA_CHANNEL_INVALID,

         VPSHAL_VPDMA_CHANNEL_AUX_MV_OUT,
         VPSHAL_VPDMA_CHANNEL_INVALID
     },
     {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
     VPSHAL_VPDMA_PATH_AUX}
};

Vcore_DwpInitParams dwpInitParams[] = {
    {VCORE_DWP_INST_WB0, VCORE_DWP_MAX_HANDLE, VPSHAL_VPDMA_CHANNEL_WB0},
    {VCORE_DWP_INST_WB1, VCORE_DWP_MAX_HANDLE, VPSHAL_VPDMA_CHANNEL_WB1}
};

Vcore_SecInitParams secInitParams[] =
{
    {VCORE_SEC_INST_0, {VPSHAL_VPDMA_CHANNEL_SEC0_LUMA,
     VPSHAL_VPDMA_CHANNEL_SEC0_CHROMA}, 4u, NULL,
     VPSHAL_VPS_CLKC_SEC0, VPSHAL_VPDMA_PATH_SEC0},
    {VCORE_SEC_INST_1, {VPSHAL_VPDMA_CHANNEL_SEC1_LUMA,
     VPSHAL_VPDMA_CHANNEL_SEC1_CHROMA}, 4u, NULL,
     VPSHAL_VPS_CLKC_SEC1, VPSHAL_VPDMA_PATH_SEC1}
};

Vcore_SwpInitParams swpInitParams =
{
    VCORE_SWP_INST_WB2, VPSHAL_VPDMA_CHANNEL_WB2, 4u, NULL
};

Vcore_CscCigInitParams cscCigInitParams =
{
    VCORE_CSC_CIG_INST_0, VCORE_CSC_CIG_NUM_INST,
    NULL, NULL, NULL, NULL
};

/* Grpx core driver init parameters*/
Vcore_GrpxInitParams gprxInitParams[] =
{
    {VCORE_GRPX_INST_0, VPSHAL_VPDMA_CHANNEL_GRPX0,
        VPSHAL_VPDMA_CHANNEL_STENCIL0, VPSHAL_VPDMA_CHANNEL_CLUT0,
        VPSHAL_VPDMA_PATH_GRPX0, VPSHAL_VPDMA_PATH_GRPX0_STENC,
        VEM_EE_GRPX0, VCORE_GRPX_MAX_HANDLE,NULL},
    {VCORE_GRPX_INST_1, VPSHAL_VPDMA_CHANNEL_GRPX1,
        VPSHAL_VPDMA_CHANNEL_STENCIL1, VPSHAL_VPDMA_CHANNEL_CLUT1,
        VPSHAL_VPDMA_PATH_GRPX1, VPSHAL_VPDMA_PATH_GRPX1_STENC,
        VEM_EE_GRPX1, VCORE_GRPX_MAX_HANDLE,NULL},
    {VCORE_GRPX_INST_2, VPSHAL_VPDMA_CHANNEL_GRPX2,
        VPSHAL_VPDMA_CHANNEL_STENCIL2, VPSHAL_VPDMA_CHANNEL_CLUT2,
        VPSHAL_VPDMA_PATH_GRPX2, VPSHAL_VPDMA_PATH_GRPX2_STENC,
        VEM_EE_GRPX2, VCORE_GRPX_MAX_HANDLE,NULL}
};

Vcore_VipInitParams vipInitParams[] =
{
    {VCORE_VIP_INST_SEC0, 4u, VPSHAL_VPS_VIP_INST_VIP0, {NULL, NULL},
        VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT, 1u},
    {VCORE_VIP_INST_SEC1, 4u, VPSHAL_VPS_VIP_INST_VIP1, {NULL, NULL},
        VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT, 1u},
};

VpsDdrv_InitPrms displayDrvInitPrms[DDRV_NUM_INST] =
{
    {VPS_DISP_INST_BP0, VRM_RESOURCE_BP0_PATH, DC_NODE_BP0,
     VCORE_BP_INST_0, NULL},
    {VPS_DISP_INST_BP1, VRM_RESOURCE_BP1_PATH, DC_NODE_BP1,
     VCORE_BP_INST_1, NULL},
    {VPS_DISP_INST_SEC1, VRM_RESOURCE_SEC1_PATH, DC_NODE_SEC1,
     VCORE_SEC_INST_1, NULL},
    {VPS_DISP_INST_MAIN_DEIH_SC1, VRM_RESOURCE_PRI_VIDEO_PATH, DC_NODE_PRI,
     VCORE_DEI_INST_PRI, NULL},
    {VPS_DISP_INST_AUX_DEI_SC2, VRM_RESOURCE_AUX_VIDEO_PATH, DC_NODE_AUX,
     VCORE_DEI_INST_AUX, NULL}
};

VpsMdrv_ScInitParams scMdrvInitParams[VPS_M2M_SC_INST_MAX] =
{
    {
        VPS_M2M_INST_SEC0_SC5_WB2,
        {
            VRM_RESOURCE_SEC0_PATH, VRM_RESOURCE_SC5_WB2_PATH,
            VRM_RESOURCE_INVALID, VRM_RESOURCE_INVALID
        },
        VCORE_SEC_INST_0, VCORE_SWP_INST_WB2,
        NULL, NULL
    },
    {
        VPS_M2M_INST_BP0_SC5_WB2,
        {
            VRM_RESOURCE_BP0_PATH, VRM_RESOURCE_SC5_WB2_PATH,
            VRM_RESOURCE_INVALID, VRM_RESOURCE_INVALID
        },
        VCORE_BP_INST_0, VCORE_SWP_INST_WB2,
        NULL, NULL
    },
    {
        VPS_M2M_INST_BP1_SC5_WB2,
        {
            VRM_RESOURCE_BP1_PATH, VRM_RESOURCE_SC5_WB2_PATH,
            VRM_RESOURCE_INVALID, VRM_RESOURCE_INVALID
        },
        VCORE_BP_INST_1, VCORE_SWP_INST_WB2,
        NULL, NULL
    },
    {
        VPS_M2M_INST_SEC0_SC3_VIP0,
        {
            VRM_RESOURCE_SEC0_PATH, VRM_RESOURCE_SEC0_MUX,
            VRM_RESOURCE_INVALID, VRM_RESOURCE_INVALID
        },
        VCORE_SEC_INST_0, VCORE_VIP_INST_SEC0,
        NULL, NULL
    },
    {
        VPS_M2M_INST_SEC1_SC4_VIP1,
        {
            VRM_RESOURCE_SEC1_PATH, VRM_RESOURCE_SEC1_MUX,
            VRM_RESOURCE_INVALID, VRM_RESOURCE_INVALID
        },
        VCORE_SEC_INST_1, VCORE_VIP_INST_SEC1,
        NULL, NULL
    }
};

VpsDdrv_GrpxInitParams   grpxDispInitParams[GDRV_GRPX_NUM_INST] =
{
     {VPS_DISP_INST_GRPX0, VRM_RESOURCE_GRPX0_PATH, DC_NODE_G0, 0u, NULL},
     {VPS_DISP_INST_GRPX1, VRM_RESOURCE_GRPX1_PATH, DC_NODE_G1, 1u, NULL},
     {VPS_DISP_INST_GRPX2, VRM_RESOURCE_GRPX2_PATH, DC_NODE_G2, 2u, NULL},
};

VpsMdrv_DeiInitParams deiMdrvInitParams[MDRV_DEI_NUM_INST] =
{
    {
        VPS_M2M_INST_MAIN_DEIH_SC1_WB0,
        VPSHAL_VPS_SEC_INST_0,
        {
            VRM_RESOURCE_PRI_VIDEO_PATH,
            VRM_RESOURCE_WB0_PATH,
            VRM_RESOURCE_INVALID,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_PRI, VCORE_DWP_INST_WB0, VCORE_VIP_INST_SEC0,
        },
        {
            NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_AUX_DEI_SC2_WB1,
        VPSHAL_VPS_SEC_INST_1,
        {
            VRM_RESOURCE_AUX_VIDEO_PATH,
            VRM_RESOURCE_WB1_PATH,
            VRM_RESOURCE_INVALID,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_AUX, VCORE_DWP_INST_WB1, VCORE_VIP_INST_SEC1,
        },
        {
            NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_MAIN_DEIH_SC3_VIP0,
        VPSHAL_VPS_SEC_INST_0,
        {
            VRM_RESOURCE_PRI_VIDEO_PATH,
            VRM_RESOURCE_WB0_PATH,
            VRM_RESOURCE_SEC0_MUX,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_PRI, VCORE_DWP_INST_WB0, VCORE_VIP_INST_SEC0,
        },
        {
            NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_AUX_DEI_SC4_VIP1,
        VPSHAL_VPS_SEC_INST_1,
        {
            VRM_RESOURCE_AUX_VIDEO_PATH,
            VRM_RESOURCE_WB1_PATH,
            VRM_RESOURCE_SEC1_MUX,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_AUX, VCORE_DWP_INST_WB1, VCORE_VIP_INST_SEC1,
        },
        {
            NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0,
        VPSHAL_VPS_SEC_INST_0,
        {
            VRM_RESOURCE_PRI_VIDEO_PATH,
            VRM_RESOURCE_WB0_PATH,
            VRM_RESOURCE_SEC0_MUX,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_PRI, VCORE_DWP_INST_WB0, VCORE_VIP_INST_SEC0,
        },
        {
            NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1,
        VPSHAL_VPS_SEC_INST_1,
        {
            VRM_RESOURCE_AUX_VIDEO_PATH,
            VRM_RESOURCE_WB1_PATH,
            VRM_RESOURCE_SEC1_MUX,
            VRM_RESOURCE_INVALID
        },
        {
            VCORE_DEI_INST_AUX, VCORE_DWP_INST_WB1, VCORE_VIP_INST_SEC1,
        },
        {
            NULL, NULL, NULL
        }
    }
};

VpsMdrv_CompInitParams compMdrvInitParams[MDRV_COMP_NUM_INST] =
{
    {
        VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB,
        {
            VRM_RESOURCE_BP0_PATH,
            VRM_RESOURCE_INVALID, //VRM_RESOURCE_GRPX0_PATH,
            VRM_RESOURCE_SC5_WB2_PATH,
            VRM_RESOURCE_INVALID,
            VRM_RESOURCE_HD1_PATH /*VRM_RESOURCE_HDMI_BLEND?*/
        },
        {
            VCORE_BP_INST_0, NULL /*VPS_DISP_INST_GRPX0*/, VCORE_SWP_INST_WB2, NULL, VCORE_CSC_CIG_INST_0
        },
        {
            NULL, NULL, NULL, NULL, NULL
        }
    },
    {
        VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB,
        {
            VRM_RESOURCE_BP1_PATH,
            VRM_RESOURCE_INVALID, //VRM_RESOURCE_GRPX1_PATH,
            VRM_RESOURCE_SC5_WB2_PATH,
            VRM_RESOURCE_INVALID,
            VRM_RESOURCE_HD1_PATH /*VRM_RESOURCE_HDMI_BLEND?*/
        },
        {
            VCORE_BP_INST_1, VPS_DISP_INST_GRPX1, VCORE_SWP_INST_WB2, NULL, VCORE_CSC_CIG_INST_0
        },
        {
            NULL, NULL, NULL, NULL, NULL
        }
    }
    //TODO: Need to add for other flavours of Blender driver instances; Also update MDRV_COMP_NUM_INST above accordingly
};

Dc_InitParams dcInitParams = {
    {NULL},
    {
        {DC_NODE_HDMI_VENC, VPS_DC_VENC_HDMI, NULL,
            VPSHAL_VPS_CLKC_HDMI_DVO1, VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO1},
        {DC_NODE_HDCOMP_VENC, VPS_DC_VENC_HDCOMP, NULL,
            VPSHAL_VPS_CLKC_HDCOMP, VPSHAL_VPS_VENC_OUT_PIX_CLK_INVALID},
        {DC_NODE_DVO2_VENC, VPS_DC_VENC_DVO2, NULL,
            VPSHAL_VPS_CLKC_DVO2, VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2},
        {DC_NODE_SD_VENC, VPS_DC_VENC_SD, NULL,
            VPSHAL_VPS_CLKC_SDVENC, VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2},
    },
    {
        {VPS_DC_VENC_HDMI, VPS_DC_CLKSRC_VENCD_DIV2},
        {VPS_DC_VENC_DVO2, VPS_DC_CLKSRC_VENCD_DIV2},
        {VPS_DC_VENC_HDCOMP, VPS_DC_CLKSRC_VENCA}
    },
    3
};


static VpsHal_RfInstParams rfInitParam[CSL_VPS_RF_PER_CNT] =
                                                {VPSHAL_RF_INST_SD_VENC,
                                                 CSL_VPS_NTSC_RF_0_REGS,
                                                 VPS_DC_VENC_SD,
                                                 VPS_DC_ENCODER_RF};

static Vps_OnChipEncoderOps rfEncOps[VPSHAL_SDVENC_MAX_NUM_OF_ONCHIP_ENC] =
                                        {VPS_DC_ENCODER_RF, NULL,
                                         NULL,
                                         NULL,
                                         VpsHal_rfStart,
                                         VpsHal_rfStop,
                                         VpsHal_rfControl};

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

Int32 Vps_init(Ptr arg)
{
    Int32 retVal = 0;

    /* Call Utils Init */
#ifdef PROFILE
    Vps_printf(" %d: VpsUtils_init >>>\r\n", Clock_getTicks());
#endif
    retVal = VpsUtils_init(NULL);
    if (0 != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }
    if (0 == retVal)
    {
        retVal = RemoteDebug_init();
    }
#ifdef PROFILE
    Vps_printf(" %d: VpsUtils_init <<<\r\n", Clock_getTicks());
#endif

#ifdef PROFILE
    Vps_printf(" %d: vpsiHalInit >>>\r\n", Clock_getTicks());
#endif
    if (0 == retVal)
    {
        retVal = vpsiHalInit();
    }
#ifdef PROFILE
    Vps_printf(" %d: vpsiHalInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
    Vps_printf(" %d: vpsiCommonInit >>>\r\n", Clock_getTicks());
#endif
    if (0 == retVal)
    {
        retVal = vpsiCommonInit();
    }
#ifdef PROFILE
    Vps_printf(" %d: vpsiCommonInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
    Vps_printf(" %d: vpsiDCtrlInit >>>\r\n", Clock_getTicks());
#endif
    if (0 == retVal)
    {
        retVal = vpsiDCtrlInit();
    }
#ifdef PROFILE
    Vps_printf(" %d: vpsiDCtrlInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
    Vps_printf(" %d: vpsiCoreInit >>>\r\n", Clock_getTicks());
#endif
    if (0 == retVal)
    {
        retVal = vpsiCoreInit();
    }
#ifdef PROFILE
    Vps_printf(" %d: vpsiCoreInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
    Vps_printf(" %d: vpsiDriverInit >>>\r\n", Clock_getTicks());
#endif
    if (0 == retVal)
    {
        retVal = vpsiDriverInit();
    }
#ifdef PROFILE
    Vps_printf(" %d: vpsiDriverInit <<<\r\n", Clock_getTicks());
#endif
    return (retVal);
}

Int32 Vps_deInit(Ptr arg)
{
    Int32 retVal = 0;

    retVal |= vpsiDriverDeInit();
    retVal |= vpsiCoreDeInit();
    retVal |= vpsiDCtrlDeInit();
    retVal |= vpsiCommonDeInit();
    retVal |= vpsiHalDeInit();
    retVal |= RemoteDebug_deInit();
    retVal |= VpsUtils_deInit(NULL);

    return (retVal);
}

static Int32 vpsiHalInit(void)
{
    Int32                   retVal = 0;
    VpsHal_VpdmaVersion     vpdmaVer;
    extern CSL_VpsRegsOvly  VpsRegOvly;

    /* Call VPS init function */
    retVal = VpsHal_vpsInit(vpsInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    VpsHal_vpsClkcModuleEnableAll();

    /* Configure VENC Clocks, Since VpsRegOvly is global
     * variable in VPS hal, it can be accessed here */
    VpsRegOvly->CLKC_VENC_CLKSEL = 0xC000E;

    /* Select the required firmware version */
#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
 #ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
  #ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
    vpdmaVer = VPSHAL_VPDMA_VER_1AE;
  #else
    vpdmaVer = VPSHAL_VPDMA_VER_1AD;
  #endif /* VPS_CFG_VPDMA_MOSIAC_USE_SOT */
 #else
    vpdmaVer = VPSHAL_VPDMA_VER_1A3;
 #endif  /* VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE */
#else
    vpdmaVer = VPSHAL_VPDMA_VER_195;
#endif  /* VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS */

    /* Use 0x1B2 firmware for TI816x PG2.0 */
    if (Vps_platformGetCpuRev() >= VPS_PLATFORM_CPU_REV_2_0)
    {
        vpdmaVer = VPSHAL_VPDMA_VER_1B2;
    }

    /* Call VPDMA init function */
    retVal = VpsHal_vpdmaInit(&vpdmaInstParams, vpdmaVer, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Initialize CHRUS HAL */
    retVal = VpsHal_chrusInit(CSL_VPS_CHRUS_PER_CNT,
                              chrusInstParams,
                              NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CIG init Function */
    retVal = VpsHal_cigInit(CSL_VPS_CIG_PER_CNT, cigInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call Blend init function */
    retVal = VpsHal_compInit(CSL_VPS_COMP_PER_CNT, blendInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call COMPR init function */
    retVal = VpsHal_comprInit(CSL_VPS_COMPR_PER_CNT, comprInitParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CSC init function */
    retVal = VpsHal_cscInit(CSL_VPS_CSC_PER_CNT, cscInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DCOMPR init function */
    retVal = VpsHal_dcomprInit(CSL_VPS_DCOMPR_PER_CNT, dcomprInitParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DEI init function */
    retVal = VpsHal_deiInit(CSL_VPS_DEI_PER_CNT, deiInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DEI_H init function */
    retVal = VpsHal_deihInit(CSL_VPS_DEI_HQ_PER_CNT, deihInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DRN init function */
    retVal = VpsHal_drnInit(CSL_VPS_DRN_PER_CNT, drnInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call EDE init function */
    retVal = VpsHal_edeInit(CSL_VPS_EDE_PER_CNT, edeInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call HDVENC init function */
    retVal = VpsHal_hdvencInit(CSL_VPS_HD_VENC_PER_CNT, hdvencInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call NF init function */
    retVal = VpsHal_nfInit(CSL_VPS_NF_PER_CNT, nfInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call Scalar init function */
    retVal = VpsHal_scInit(CSL_VPS_SC_PER_CNT, scInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call SDVENC init function */
    retVal = VpsHal_sdvencInit(CSL_VPS_SD_VENC_PER_CNT, sdvencInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call VCOMP init function */
    retVal = VpsHal_vcompInit(CSL_VPS_VCOMP_PER_CNT, vcompInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call VIP init function */
    retVal = VpsHal_vipInit(CSL_VPS_VIP_PORT_PER_CNT, vipInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }
    /* Call GRPX init function*/
    retVal = VpsHal_grpxInit(CSL_VPS_GRPX_PER_CNT, grpxInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CPROC init function */
    retVal = VpsHal_cprocInit(CSL_VPS_CPROC_PER_CNT, cprocInstParams, NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Initialize RF Hal */
    retVal = VpsHal_rfInit(CSL_VPS_RF_PER_CNT, &rfInitParam[0], NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (retVal);
}

static Int32 vpsiHalDeInit(void)
{
    Int32 retVal = 0;

    /* Call VIP de-init function */
    retVal = VpsHal_vipDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call VCOMP de-init function */
    retVal = VpsHal_vcompDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call SDVENC de-init function */
    retVal = VpsHal_sdvencDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call SC de-init function */
    retVal = VpsHal_scDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call NF de-init function */
    retVal = VpsHal_nfDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call HDVENC de-init function */
    retVal = VpsHal_hdvencDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call EDE de-init function */
    retVal = VpsHal_edeDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DRN de-init function */
    retVal = VpsHal_drnDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DEI_H de-init function */
    retVal = VpsHal_deihDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DEI de-init function */
    retVal = VpsHal_deiDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CSC de-init function */
    retVal = VpsHal_cscDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call COMPR de-init function */
    retVal = VpsHal_comprDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call DCOMPR de-init function */
    retVal = VpsHal_dcomprDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call COMP de-init function */
    retVal = VpsHal_compDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CIG de-init function */
    retVal = VpsHal_cigDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CSC de-init function */
    retVal = VpsHal_chrusDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /** Call GRPX de-init function    */
    retVal = VpsHal_grpxDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call VPDMA de-init function */
    retVal = VpsHal_vpdmaDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call VPS de-init function */
    retVal = VpsHal_vpsDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call CPROC de-init function */
    retVal = VpsHal_cprocDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* DeInitialize RF */
    retVal = VpsHal_rfDeInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (retVal);
}

static Int32 vpsiCommonInit(void)
{
    Int32 retVal = 0;

    /* Call Vrm Init */
    retVal = Vrm_init(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call Vem init */
    retVal = Vem_init(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    #if defined(VPS_MODULE_INCLUDE_DISPLAY)

    /* Call Dlm Init */
    retVal = VpsDlm_init(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_NSF) || defined(VPS_MODULE_INCLUDE_M2M_DEI) || defined(VPS_MODULE_INCLUDE_M2M_SC)

    /* Call Dlm Init */
    retVal = Mlm_init(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    #endif

    return (retVal);
}

static Int32 vpsiCommonDeInit(void)
{
    Int32 retVal = 0;

    #if defined(VPS_MODULE_INCLUDE_M2M_NSF) || defined(VPS_MODULE_INCLUDE_M2M_DEI) || defined(VPS_MODULE_INCLUDE_M2M_SC)

    retVal = Mlm_deInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_DISPLAY)

   /* Call Dlm Init */
    retVal = VpsDlm_deInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }
    #endif

    /* Call Vem init */
    retVal = Vem_deInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    /* Call Vrm Init */
    retVal = Vrm_deInit(NULL);
    if (0 != retVal)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (retVal);
}

static Int32 vpsiDCtrlInit(void)
{
    Int32 retVal = 0;

    retVal = vpsiHalOpen();
    if (0 == retVal)
    {
        #if defined(VPS_MODULE_INCLUDE_DISPLAY)

        dcInitParams.halHandle[VDC_AUX_CSC_IDX] =
            CscHandles[VPSHAL_CSC_INST_HD1];
        dcInitParams.halHandle[VDC_SD_CSC_IDX] =
            CscHandles[VPSHAL_CSC_INST_SD];
        dcInitParams.halHandle[VDC_VCOMP_IDX] = VcompHandle;
        dcInitParams.halHandle[VDC_EDE_IDX] = EdeHandle;
        dcInitParams.halHandle[VDC_CIG_IDX] = CigHandle;
        dcInitParams.halHandle[VDC_COMP_IDX] = CompHandle;
        dcInitParams.halHandle[VDC_CPROC_IDX] = CprocHandle;

        dcInitParams.outNode[0u].vencHalHandle = HdVencHandles[0u];
        dcInitParams.outNode[1u].vencHalHandle = HdVencHandles[1u];
        dcInitParams.outNode[2u].vencHalHandle = HdVencHandles[2u];
        dcInitParams.outNode[3u].vencHalHandle = SdVencHandle;

        /* Call DC init function */
        retVal = Dc_init(&dcInitParams, NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }

        #endif
    }

    if (0 != retVal)
    {
        vpsiHalClose();
    }

    return (retVal);
}

static Int32 vpsiDCtrlDeInit(void)
{
    Int32 retVal = 0;

    #if defined(VPS_MODULE_INCLUDE_DISPLAY)

    retVal = Dc_deInit(NULL);

    #endif
    retVal = vpsiHalClose();
    if (0 != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
    }

    return (retVal);
}

static Int32 vpsiHalOpen(void)
{
    Int32   retVal = 0;
    UInt32  instCnt;

    /* Open the CPROC */
    CprocHandle = VpsHal_cprocOpen(VPSHAL_CPROC_INST_0);
    if (NULL == CprocHandle)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        retVal = -1;
    }

    /* Open the VCOMP */
    VcompHandle = VpsHal_vcompOpen(VPSHAL_VCOMP_INST_0);
    if (NULL == VcompHandle)
    {
        VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        retVal = -1;
    }

    /* Open the CSC Instance and get the handle to them */
    if (0 == retVal)
    {
        CscHandles[VPSHAL_CSC_INST_SD] = VpsHal_cscOpen(VPSHAL_CSC_INST_SD);
        if (NULL == CscHandles[VPSHAL_CSC_INST_SD])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        CscHandles[VPSHAL_CSC_INST_HD1] = VpsHal_cscOpen(VPSHAL_CSC_INST_HD1);
        if (NULL == CscHandles[VPSHAL_CSC_INST_HD1])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        CscHandles[VPSHAL_CSC_INST_WB2] = VpsHal_cscOpen(VPSHAL_CSC_INST_WB2);
        if (NULL == CscHandles[VPSHAL_CSC_INST_WB2])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        CscHandles[VPSHAL_CSC_INST_VIP0] = VpsHal_cscOpen(VPSHAL_CSC_INST_VIP0);
        if (NULL == CscHandles[VPSHAL_CSC_INST_VIP0])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        CscHandles[VPSHAL_CSC_INST_VIP1] = VpsHal_cscOpen(VPSHAL_CSC_INST_VIP1);
        if (NULL == CscHandles[VPSHAL_CSC_INST_VIP1])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        /* Open the COMP Instance and get the handle to them */
        CompHandle = VpsHal_compOpen(VPSHAL_COMP_INST_0, NULL);
        if (NULL == CompHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        CigHandle = VpsHal_cigOpen(VPSHAL_CIG_INST_0);
        if (NULL == CigHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        EdeHandle = VpsHal_edeOpen(VPSHAL_EDE_INST_0);
        if (NULL == EdeHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        /* Get RF-DAC out of reset.  Register is in PRCM module */
         * (volatile UInt32 *)(RF_DAC_CTRL_REG_ADDR) = 0x1;

        rfEncOps[0u].handle = VpsHal_rfOpen(VPSHAL_RF_INST_SD_VENC);
        if (NULL == rfEncOps[0u].handle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        /* Get SD-DAC out of reset.  Register is in PRCM module */
         * (volatile UInt32 *)(SD_DAC_CTRL_REG_ADDR) = 0x1;

        SdVencHandle = VpsHal_sdvencOpen(VPSHAL_SDVENC_INST_0);
        if (NULL == SdVencHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    /*
     * Since RF is on-chip encoder and it is paired with this SD Venc, let the
     * venc know that there is an on-chip encoder paired with it.
     */
    if (0 == retVal)
    {
        retVal = VpsHal_sdvencInitEncOps(SdVencHandle, rfEncOps);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        /* Get HD-DAC out of reset.  Register is in PRCM module */
        * (volatile UInt32 *)(HD_DAC_CTRL_REG_ADDR) = 0x1;

        HdVencHandles[VPSHAL_HDVENC_HDMI] =
                                VpsHal_hdvencOpen(VPSHAL_HDVENC_HDMI);
        if (NULL == HdVencHandles[VPSHAL_HDVENC_HDMI])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        HdVencHandles[VPSHAL_HDVENC_DVO2] =
                                VpsHal_hdvencOpen(VPSHAL_HDVENC_DVO2);
        if (NULL == HdVencHandles[VPSHAL_HDVENC_DVO2])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        HdVencHandles[VPSHAL_HDVENC_HDCOMP] =
                                VpsHal_hdvencOpen(VPSHAL_HDVENC_HDCOMP);
        if (NULL == HdVencHandles[VPSHAL_HDVENC_HDCOMP])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        DeiHandle[VPSHAL_DEI_INST_0] =
            VpsHal_deiOpen(VPSHAL_DEI_INST_0);
        if (NULL == DeiHandle[VPSHAL_DEI_INST_0])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        DeihHandle[VPSHAL_DEIH_INST_0] =
            VpsHal_deihOpen(VPSHAL_DEIH_INST_0);
        if (NULL == DeiHandle[VPSHAL_DEIH_INST_0])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        DrnHandle[VPSHAL_DRN_INST_PRI] =
            VpsHal_drnOpen(VPSHAL_DRN_INST_PRI);
        if (NULL == DrnHandle[VPSHAL_DRN_INST_PRI])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        DrnHandle[VPSHAL_DRN_INST_AUX] =
            VpsHal_drnOpen(VPSHAL_DRN_INST_AUX);
        if (NULL == DrnHandle[VPSHAL_DRN_INST_AUX])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ChrusHandle[VPSHAL_CHRUS_INST_SEC0] =
            VpsHal_chrusOpen(VPSHAL_CHRUS_INST_SEC0);
        if (NULL == ChrusHandle[VPSHAL_CHRUS_INST_SEC0])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        ChrusHandle[VPSHAL_CHRUS_INST_SEC1] =
            VpsHal_chrusOpen(VPSHAL_CHRUS_INST_SEC1);
        if (NULL == ChrusHandle[VPSHAL_CHRUS_INST_SEC1])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        ChrusHandle[VPSHAL_CHRUS_INST_PRI0] =
            VpsHal_chrusOpen(VPSHAL_CHRUS_INST_PRI0);
        if (NULL == ChrusHandle[VPSHAL_CHRUS_INST_PRI0])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }
    if (0 == retVal)
    {
        ChrusHandle[VPSHAL_CHRUS_INST_AUX] =
            VpsHal_chrusOpen(VPSHAL_CHRUS_INST_AUX);
        if (NULL == ChrusHandle[VPSHAL_CHRUS_INST_AUX])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ScHandle[VPSHAL_SC_INST_5] = VpsHal_scOpen(VPSHAL_SC_INST_5);
        if (NULL == ScHandle[VPSHAL_SC_INST_5])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ScHandle[VPSHAL_SC_INST_1] = VpsHal_scOpen(VPSHAL_SC_INST_1);
        if (NULL == ScHandle[VPSHAL_SC_INST_1])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ScHandle[VPSHAL_SC_INST_2] = VpsHal_scOpen(VPSHAL_SC_INST_2);
        if (NULL == ScHandle[VPSHAL_SC_INST_2])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ScHandle[VPSHAL_SC_INST_3] = VpsHal_scOpen(VPSHAL_SC_INST_3);
        if (NULL == ScHandle[VPSHAL_SC_INST_3])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        ScHandle[VPSHAL_SC_INST_4] = VpsHal_scOpen(VPSHAL_SC_INST_4);
        if (NULL == ScHandle[VPSHAL_SC_INST_4])
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        for (instCnt = 0u; instCnt < CSL_VPS_COMPR_PER_CNT; instCnt++)
        {
            ComprHandle[instCnt] = VpsHal_comprOpen(instCnt);
            if (NULL == ComprHandle[instCnt])
            {
                Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                retVal = -1;
                break;
            }
        }
    }

    if (0 == retVal)
    {
        for (instCnt = 0u; instCnt < CSL_VPS_DCOMPR_PER_CNT; instCnt++)
        {
            DcomprHandle[instCnt] = VpsHal_dcomprOpen(instCnt);
            if (NULL == DcomprHandle[instCnt])
            {
                Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                retVal = -1;
                break;
            }
        }
    }

   if(0 == retVal)
   {
       for(instCnt = 0; instCnt < CSL_VPS_GRPX_PER_CNT; instCnt++)
       {
           GrpxHandles[instCnt] = VpsHal_grpxOpen((VpsHal_GrpxInst)instCnt);
           if(NULL == GrpxHandles[instCnt])
           {
               Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
               retVal = -1;
               break;
           }
       }
   }
    return (retVal);
}

static Int32 vpsiHalClose(void)
{
    Int32 retVal = 0;
    UInt32 instCnt;

    if (NULL != CprocHandle)
    {
        retVal = VpsHal_cprocClose(CprocHandle);
        if (0 != retVal)
        {
            VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            CprocHandle = NULL;
        }
    }

    if (NULL != VcompHandle)
    {
        retVal = VpsHal_vcompClose(VcompHandle);
        if (0 != retVal)
        {
            VPSI_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            VcompHandle = NULL;
        }
    }

    if (NULL != CscHandles[VPSHAL_CSC_INST_SD])
    {
        /* Open the CSC Instance and get the handle to them */
        retVal = VpsHal_cscClose(CscHandles[VPSHAL_CSC_INST_SD]);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            CscHandles[VPSHAL_CSC_INST_SD] = NULL;
        }
    }

    if (NULL != CscHandles[VPSHAL_CSC_INST_HD1])
    {
        retVal = VpsHal_cscClose(CscHandles[VPSHAL_CSC_INST_HD1]);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            CscHandles[VPSHAL_CSC_INST_HD1] = NULL;
        }
    }

    if (NULL != CompHandle)
    {
        /* Close the COMP Instance and get the handle to them */
        retVal = VpsHal_compClose(CompHandle);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            CompHandle = NULL;
        }
    }

    if (NULL != CigHandle)
    {
        retVal = VpsHal_cigClose(CigHandle);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            CigHandle = NULL;
        }
    }

    if (NULL != EdeHandle)
    {
        retVal = VpsHal_edeClose(EdeHandle);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            EdeHandle = NULL;
        }
    }

    if (rfEncOps[0u].handle != NULL)
    {
        retVal = VpsHal_rfClose(rfEncOps[0u].handle);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            rfEncOps[0u].handle = NULL;
        }
    }

    if (NULL != SdVencHandle)
    {
        retVal = VpsHal_sdvencClose(SdVencHandle);
        if (0 != NULL)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            SdVencHandle = NULL;
        }
    }

    if (NULL != HdVencHandles[VPSHAL_HDVENC_HDMI])
    {
        retVal = VpsHal_hdvencClose(HdVencHandles[VPSHAL_HDVENC_HDMI]);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            HdVencHandles[VPSHAL_HDVENC_HDMI] = NULL;
        }
    }

    if (NULL != HdVencHandles[VPSHAL_HDVENC_DVO2])
    {
        retVal = VpsHal_hdvencClose(HdVencHandles[VPSHAL_HDVENC_DVO2]);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            HdVencHandles[VPSHAL_HDVENC_DVO2] = NULL;
        }
    }

    if (NULL != HdVencHandles[VPSHAL_HDVENC_HDCOMP])
    {
        retVal = VpsHal_hdvencClose(HdVencHandles[VPSHAL_HDVENC_HDCOMP]);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
        else
        {
            HdVencHandles[VPSHAL_HDVENC_HDCOMP] = NULL;
        }
    }

    for(instCnt = 0u; instCnt < CSL_VPS_GRPX_PER_CNT; instCnt++)
    {
        if(NULL != GrpxHandles[instCnt])
        {
            retVal = VpsHal_grpxClose(GrpxHandles[instCnt]);
            if (0 != retVal)
            {
                Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            }
            else
            {
                GrpxHandles[instCnt] = NULL;
            }
        }
   }
    return (retVal);
}

static Int32 vpsiCoreInit(void)
{
    Int32 retVal = 0;
    UInt32 instCnt;

    Vcore_vipResInit();

    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    deiInitParams[0u].halHandle[VCORE_DEI_CHRUS0_IDX] =
        ChrusHandle[VPSHAL_CHRUS_INST_PRI0];
    deiInitParams[0u].halHandle[VCORE_DEI_CHRUS1_IDX] = NULL;
    deiInitParams[0u].halHandle[VCORE_DEI_CHRUS2_IDX] = NULL;
    deiInitParams[0u].halHandle[VCORE_DEI_DRN_IDX] =
        DrnHandle[VPSHAL_DRN_INST_PRI];
    deiInitParams[0u].halHandle[VCORE_DEI_DEI_IDX] =
        DeihHandle[VPSHAL_DEIH_INST_0];
    deiInitParams[0u].halHandle[VCORE_DEI_SC_IDX] =
        ScHandle[VPSHAL_SC_INST_1];
    deiInitParams[0u].halHandle[VCORE_DEI_COMPR0_IDX] =
        ComprHandle[VPSHAL_COMPR_INST_PRI_TOP];
    deiInitParams[0u].halHandle[VCORE_DEI_COMPR1_IDX] =
        ComprHandle[VPSHAL_COMPR_INST_PRI_BOT];
    deiInitParams[0u].halHandle[VCORE_DEI_DCOMPR0_IDX] =
        DcomprHandle[VPSHAL_DCOMPR_INST_PRI_FLD1];
    deiInitParams[0u].halHandle[VCORE_DEI_DCOMPR1_IDX] =
        DcomprHandle[VPSHAL_DCOMPR_INST_PRI_FLD2];
    deiInitParams[0u].halHandle[VCORE_DEI_DCOMPR2_IDX] =
        DcomprHandle[VPSHAL_DCOMPR_INST_PRI_FLD3];

    deiInitParams[1u].halHandle[VCORE_DEI_CHRUS0_IDX] =
        ChrusHandle[VPSHAL_CHRUS_INST_AUX];
    deiInitParams[1u].halHandle[VCORE_DEI_CHRUS1_IDX] = NULL;
    deiInitParams[1u].halHandle[VCORE_DEI_CHRUS2_IDX] = NULL;
    deiInitParams[1u].halHandle[VCORE_DEI_DRN_IDX] =
        DrnHandle[VPSHAL_DRN_INST_AUX];
    deiInitParams[1u].halHandle[VCORE_DEI_DEI_IDX] =
        DeiHandle[VPSHAL_DEI_INST_0];
    deiInitParams[1u].halHandle[VCORE_DEI_SC_IDX] =
        ScHandle[VPSHAL_SC_INST_2];
    deiInitParams[1u].halHandle[VCORE_DEI_COMPR0_IDX] =
        ComprHandle[VPSHAL_COMPR_INST_AUX];
    deiInitParams[1u].halHandle[VCORE_DEI_COMPR1_IDX] = NULL;
    deiInitParams[1u].halHandle[VCORE_DEI_DCOMPR0_IDX] =
        DcomprHandle[VPSHAL_DCOMPR_INST_AUX_FLD1];
    deiInitParams[1u].halHandle[VCORE_DEI_DCOMPR1_IDX] =
        DcomprHandle[VPSHAL_DCOMPR_INST_AUX_FLD2];
    deiInitParams[1u].halHandle[VCORE_DEI_DCOMPR2_IDX] = NULL;

    if (0 == retVal)
    {
        retVal = Vcore_deiInit(2u, deiInitParams, NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    if (0 == retVal)
    {
        retVal = Vcore_dwpInit(VCORE_DWP_NUM_INST, dwpInitParams, NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    if (0 == retVal)
    {
        vipInitParams[0].halHandle[VCORE_VIP_SC_IDX] = ScHandle[VPSHAL_SC_INST_3];
        vipInitParams[0].halHandle[VCORE_VIP_CSC_IDX] = CscHandles[VPSHAL_CSC_INST_VIP0];
        vipInitParams[1].halHandle[VCORE_VIP_SC_IDX] = ScHandle[VPSHAL_SC_INST_4];
        vipInitParams[1].halHandle[VCORE_VIP_CSC_IDX] = CscHandles[VPSHAL_CSC_INST_VIP1];
        retVal = Vcore_vipInit(2u, vipInitParams, NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_DISPLAY) || defined(VPS_MODULE_INCLUDE_M2M_COMP)

    if (0 == retVal)
    {
        retVal = Vcore_bpInit(VCORE_BP_INST_MAX, bpInitParams, NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_DISPLAY) || defined(VPS_MODULE_INCLUDE_M2M_SC)

    secInitParams[0].chrusHandle = ChrusHandle[VPSHAL_CHRUS_INST_SEC0];
    secInitParams[1].chrusHandle = ChrusHandle[VPSHAL_CHRUS_INST_SEC1];
    if (0 == retVal)
    {
        retVal = Vcore_secInit(2, &secInitParams[0], NULL);
        if (0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_SC)  || defined(VPS_MODULE_INCLUDE_M2M_COMP)

    swpInitParams.scHandle = ScHandle[VPSHAL_SC_INST_5];
    if (0 == retVal)
    {
        retVal = Vcore_swpInit(1, &swpInitParams, NULL);
        if (retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_GRPX)  || defined(VPS_MODULE_INCLUDE_M2M_COMP)

    /*init grpx core dirver*/
    for(instCnt = 0u; instCnt < CSL_VPS_GRPX_PER_CNT; instCnt++)
    {
        gprxInitParams[instCnt].grpxHandle = GrpxHandles[instCnt];
    }
    if(0 == retVal)
    {
        retVal = Vcore_grpxInit(CSL_VPS_GRPX_PER_CNT, &gprxInitParams[0],NULL);
        if(0 != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_COMP)

    cscCigInitParams.cscHandle     = CscHandles[VPSHAL_CSC_INST_HD1];
    cscCigInitParams.cigHandle     = CigHandle;
    cscCigInitParams.blendHdmiHandle = CompHandle;
    cscCigInitParams.cscWrbkHandle = CscHandles[VPSHAL_CSC_INST_WB2];

    if (0 == retVal)
    {
        retVal = Vcore_cscCigInit(1, &cscCigInitParams, NULL);
        if (retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        }
    }

    #endif

    return (retVal);
}

static Int32 vpsiCoreDeInit(void)
{
    Int32 retVal = 0;


    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    retVal = Vcore_deiDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    retVal |= Vcore_dwpDeInit(NULL);
    retVal |= Vcore_vipDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_DISPLAY) || defined(VPS_MODULE_INCLUDE_M2M_COMP)

    retVal |= Vcore_bpDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_DISPLAY) || defined(VPS_MODULE_INCLUDE_M2M_SC)

    retVal |= Vcore_secDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_SC) || defined(VPS_MODULE_INCLUDE_M2M_COMP)

    retVal |= Vcore_swpDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_GRPX)

    retVal |= Vcore_grpxDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_COMP) || defined(VPS_MODULE_INCLUDE_DISPLAY)

    retVal |= Vcore_cscCigDeInit(NULL);

    #endif

    Vcore_vipResDeInit();

    return (retVal);
}

static Int32 vpsiDriverInit(void)
{
    Int32               retVal = 0;
    const Vcore_Ops    *bpCoreOps;
    const Vcore_Ops    *secCoreOps, *swpCoreOps;
    const Vcore_Ops    *grpxCoreOps;
    const Vcore_Ops    *deiCoreOps, *dwpCoreOps, *vipCoreOps;
    const Vcore_Ops    *cscCigCoreOps;

    bpCoreOps = Vcore_bpGetCoreOps();
    secCoreOps = Vcore_secGetCoreOps();
    swpCoreOps  = Vcore_swpGetCoreOps();
    deiCoreOps = Vcore_deiGetCoreOps();
    dwpCoreOps = Vcore_dwpGetCoreOps();
    vipCoreOps = Vcore_vipGetCoreOps();
    cscCigCoreOps = Vcore_cscCigGetCoreOps();

    #if defined(VPS_MODULE_INCLUDE_DISPLAY)

    displayDrvInitPrms[0].coreOps = bpCoreOps;
    displayDrvInitPrms[1].coreOps = bpCoreOps;
    displayDrvInitPrms[2].coreOps = secCoreOps;
    displayDrvInitPrms[3].coreOps = deiCoreOps;
    displayDrvInitPrms[4].coreOps = deiCoreOps;
    retVal = VpsDdrv_init(DDRV_NUM_INST, displayDrvInitPrms, NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_SC)

    if (0 == retVal)
    {
        scMdrvInitParams[0].inCoreOps = secCoreOps;
        scMdrvInitParams[0].outCoreOps = swpCoreOps;
        scMdrvInitParams[1].inCoreOps = bpCoreOps;
        scMdrvInitParams[1].outCoreOps = swpCoreOps;
        scMdrvInitParams[2].inCoreOps = bpCoreOps;
        scMdrvInitParams[2].outCoreOps = swpCoreOps;
        scMdrvInitParams[3].inCoreOps = secCoreOps;
        scMdrvInitParams[3].outCoreOps = vipCoreOps;
        scMdrvInitParams[4].inCoreOps = secCoreOps;
        scMdrvInitParams[4].outCoreOps = vipCoreOps;
        retVal = VpsMdrv_scInit(5u, scMdrvInitParams, NULL);
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    if (0 == retVal)
    {
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_WB0].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_WB0].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = dwpCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_WB0].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = NULL;

        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_WB1].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_WB1].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = dwpCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_WB1].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = NULL;

        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC3_VIP0].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC3_VIP0].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = NULL;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC3_VIP0].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = vipCoreOps;

        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC4_VIP1].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC4_VIP1].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = NULL;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC4_VIP1].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = vipCoreOps;

        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = dwpCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = vipCoreOps;

        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1].
                            coreOps[VPSMDRV_DEI_DEI_CORE_IDX] = deiCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1].
                            coreOps[VPSMDRV_DEI_DWP_CORE_IDX] = dwpCoreOps;
        deiMdrvInitParams[VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1].
                            coreOps[VPSMDRV_DEI_VIP_CORE_IDX] = vipCoreOps;
        retVal = VpsMdrv_deiInit(MDRV_DEI_NUM_INST, &deiMdrvInitParams[0], NULL);
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_CAPTURE_VIP)

    if (0 == retVal) {
      retVal = Vps_captInit();
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_NSF)

    if (0 == retVal) {
      retVal = Vps_nsfInit();
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_GRPX)

    if(0 == retVal)
    {
        grpxCoreOps = Vcore_grpxGetCoreOps();
        grpxDispInitParams[0].coreOps = grpxCoreOps;
        grpxDispInitParams[1].coreOps = grpxCoreOps;
        grpxDispInitParams[2].coreOps = grpxCoreOps;
        retVal = VpsDdrv_grpxInit(3u, &grpxDispInitParams[0], NULL);
    }

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_COMP)

    if (0 == retVal)
    {
        grpxCoreOps = Vcore_grpxGetCoreOps();
        compMdrvInitParams[VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_IN_VIDEO_IDX]  = bpCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_IN_GRPX_IDX]   = NULL; //grpxCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_OUT_WRBK0_IDX] = swpCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_OUT_WRBK1_IDX] = NULL;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_PROCESS_PATH_IDX] = cscCigCoreOps;

        compMdrvInitParams[VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_IN_VIDEO_IDX]  = bpCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_IN_GRPX_IDX]   = NULL;//grpxCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_OUT_WRBK0_IDX] = swpCoreOps;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_OUT_WRBK1_IDX] = NULL;
        compMdrvInitParams[VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB].
                            coreOps[VPSMDRV_COMP_CORE_PROCESS_PATH_IDX] = cscCigCoreOps;
        //TODO: Need to add for other flavours of Blender driver instances;

        retVal = VpsMdrv_compInit(MDRV_COMP_NUM_INST, &compMdrvInitParams[0], NULL);
    }

    #endif

    return (retVal);
}

static Int32 vpsiDriverDeInit(void)
{
    Int32 retVal = 0;

    #if defined(VPS_MODULE_INCLUDE_DISPLAY)

    retVal = VpsDdrv_deInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_SC)

    retVal |= VpsMdrv_scDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_DEI)

    retVal |= VpsMdrv_deiDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_CAPTURE_VIP)

    retVal |= Vps_captDeInit();

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_NSF)

    retVal |= Vps_nsfDeInit();

    #endif

    #if defined(VPS_MODULE_INCLUDE_GRPX)

    retVal |= VpsDdrv_grpxDeInit(NULL);

    #endif

    #if defined(VPS_MODULE_INCLUDE_M2M_COMP)

    retVal |= VpsMdrv_compDeInit(NULL);

    #endif

    return (retVal);
}
