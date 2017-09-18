/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_hdvenc.c
 *
 * \brief VPS HDVENC HAL Source file.
 * This file implements the HAL APIs of the VPS HDVENC.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/cslr/cslr_vps_hd_venc_d.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/src/vpshalCscDefaults.h>
#include <ti/psp/vps/hal/vpshal_hdvenc.h>
#include <ti/psp/cslr/cslr_vps_hd_venc_a.h>
#include <ti/psp/cslr/cslr_vps_hd_venc_d.h>
#include <ti/psp/vps/hal/src/vpshalHdVencDefaults.h>
#include <ti/psp/vps/vps_cfgCsc.h>
#include <ti/psp/vps/hal/vpshal_onChipEncOps.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/**
 *  \brief Adjust Vblank and active video lines for progressive
 *  mode to workaround the 8-pixel shift issue. This will ensure that the
 *  VENC will always pull in one extra line from Blender and hence will flush
 *  8 pixel wide Blender Async FIFO every frame.
 *  Caution: This will result in VENC underflow bit getting set for every frame.
 */
#define VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE

//#define VPSHAL_VENC_SWAP_COLORS_FOR_VGA_OUT_ON_TI813X

/**
 * \brief Offset of the gamma correction memories in the Venc register map
 */
#define VPSHAL_HDVENC_GAMMA_REGS_OFFSET (0x1000)

/* Masks and Shifts for the gamma correction */
#define CSL_VPS_HD_VENC_A_GAMMA_LOW_COEFF_MASK      (0x000003FF)
#define CSL_VPS_HD_VENC_A_GAMMA_LOW_COEFF_SHIFT     (0x00000000)
#define CSL_VPS_HD_VENC_A_GAMMA_MID_COEFF_MASK      (0x0000FFC0)
#define CSL_VPS_HD_VENC_A_GAMMA_MID_COEFF_SHIFT     (0x0000000A)
#define CSL_VPS_HD_VENC_A_GAMMA_HIGH_COEFF_MASK     (0x3FF00000)
#define CSL_VPS_HD_VENC_A_GAMMA_HIGH_COEFF_SHIFT    (0x00000014)

#define HDVENC_HOFFSET                              (0u)

/*
 *  \brief Enums for the configuration overlay register offsets - used as array
 *  indices to register offset array. These enums defines the registers
 *  that will be used to form the configuration register overlay.
 *  This in turn is used by client drivers to configure registers using
 *  VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this
 *  enum and the initial value of NUM_REG_IDX macro should be modified
 *  accordingly. And make sure that the values of these enums are in
 *  ascending order and it starts from 0.
 */
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    VpsHal_HdVencOutputColorSpace           outColorSpace;
    /**< Y_RGBn: Color space setting. This bit defines the color space used by
         the analog video output.
         It will be used to configure the scaling functions on different channels
         for different analog display color spaces. */
    VpsHal_HdVencDvoFmt                     fmt;
    /**< DVO_FMT: These three bits define the format of digital video
    output port.*/
    VpsHal_HdVencDataSource                 dataSource;
    /**< DVO_CS: These two bits select the video data source for DVO.*/
    VpsHal_HdVencCscMode                    cscMode;
    /**< Color space conversion mode. Based on this Hal will configure the
    default CSC coefficients */
    VpsHal_HdVencCscCoeff                   cscCoeff;
    /**< Pointer to the coefficients.  If this is null Hal will program the
    default coefficients according to the conversion selected. */
    struct
    {
        UInt32                                  dac0PwDn;
        /**< PWD_0: DAC0 power down bit. */
        UInt32                                  dac1PwDn;
        /**< PWD_1: DAC1 power down bit. */
        UInt32                                  dac2PwDn;
        /**< PWD_2: DAC2 power down bit. */
        UInt32                                  dacInvertData;
        /**< INVT: When this bit is set, all the data to DAC0, DAC1,
        and DAC2 will be inverted */
        UInt32                                  dac0RfPdn;
        /**< DAC_RF0: This bit will turn on or off the internal voltage
        reference of DAC0/1/2.*/
        UInt32                                  dac1RfPdn;
        /**< DAC_RF1: This bit will turn on or off the internal voltage
        reference of DAC3/4/5.*/
    } analogCfg;
    struct
    {
        UInt32                                  invtfidPol;
        /**< I_DVO_F: This bit controls the polarity of DVO_FID signal. */
        UInt32                                  invtVsPol;
        /**< I_DVO_V: This bit controls the polarity of DVO_VS signal.*/
        UInt32                                  invtHsPol;
        /**< I_DVO_H: This bit controls the polarity of DVO_HS signal. */
        UInt32                                  invtActsPol;
        /**< I_DVO_A: This bit controls the polarity of DVO_ACTVID signal. */
        VpsHal_HdVenc444to422                   decimationMethod;
        /**< S_422: When the DVO output is in 4:2:2 YCbCr format, this bit
        controls the CbCr format.*/
        UInt32                                  dvoOff;
        /**< DVO_OFF: This bit can force the DVO to 0s */
    } dvoCfg;
} HdVenc_OutputCfg;

typedef struct
{
    FVID2_ModeInfo mInfo;
    UInt32         dispMode;
    /**< Display mode select - 480i, 480p, 1080i, 720p, 576i or 576p. */
    UInt32         osdFidSt1;
    /**< Defines the starting location of the first field for OSD timing. */
    UInt32         osdFidSt2;
    /**< Defines the starting location of the second field for OSD timing.
         This is used only in interlaced mode. */
} HdVenc_StdModeInfo;

typedef struct
{
    UInt32                   instId;
    UInt8                    openCnt;
    CSL_VpsHdVencdRegsOvly   regsOvly;
    UInt32                   currModeIdx;
    volatile UInt32         *gRegBase;
    VpsHal_HdVencConfig      vencCfg;
    Vps_DcOutputInfo         output;
    Vps_DcModeInfo           mode;

    /* On-Chip encoder specifics */
    Vps_OnChipEncoderOps    enc[VPSHAL_HDVENC_MAX_NUM_OF_ONCHIP_ENC];
    /**< Operations for all the on-chip encoder that are paired with this
         instance of venc */
    UInt32                  noOfEncs;
    /**< Number of on-chip encoder that this instance of venc supports. */
    HdVenc_OutputCfg        outputCfg;
    /**< Output Configuration */
    Int32                   contrast;
    Int32                   brightness;
    Int32                   saturation;
    Int32                   hue;
} HdVenc_Obj;

typedef struct
{
    Int32 m[VPSHAL_HDVENC_CSC_NUM_COEFF][VPSHAL_HDVENC_CSC_NUM_COEFF];
    Int32 a[VPSHAL_HDVENC_CSC_NUM_COEFF];
} VpsHal_hdVencCscQFormCoeff;

HdVenc_StdModeInfo HdVencStdModeInfo[] = {
    {{FVID2_STD_1080P_60, 1920, 1080, FVID2_SF_PROGRESSIVE, 148500, 60,
        88, 148, 44, 4, 36, 5}, 3, 8, 0},
    {{FVID2_STD_1080P_50, 1920, 1080, FVID2_SF_PROGRESSIVE, 74250, 50,
        528, 148, 44, 4, 36, 5}, 3, 7, 0},
    {{FVID2_STD_1080P_30, 1920, 1080, FVID2_SF_PROGRESSIVE, 74250, 30,
        88, 148, 44, 4, 36, 5}, 3, 4, 0},
    {{FVID2_STD_720P_60, 1280, 720, FVID2_SF_PROGRESSIVE, 74250, 60,
        110, 220, 40, 5, 20, 5}, 3, 6, 0},
    {{FVID2_STD_720P_50, 1280, 720, FVID2_SF_PROGRESSIVE, 74250, 50,
        440, 220, 40, 5, 20, 5}, 3, 5, 0},
    {{FVID2_STD_XGA_60, 1024, 768, FVID2_SF_PROGRESSIVE, 65000, 60,
        24, 160, 136, 3, 29, 6}, 1, 6, 0},
    {{FVID2_STD_XGA_75, 1024, 768, FVID2_SF_PROGRESSIVE, 78750, 75,
        16, 176, 96, 1, 28, 3}, 1, 8, 0},
    {{FVID2_STD_SXGA_60, 1280, 1024, FVID2_SF_PROGRESSIVE, 108000, 60,
        48, 248, 112, 1, 38, 3}, 1, 8, 0},
    {{FVID2_STD_SXGA_75, 1280, 1024, FVID2_SF_PROGRESSIVE, 135000, 75,
        16, 248, 144, 1, 38, 3}, 1, 10, 0},
    {{FVID2_STD_UXGA_60, 1600, 1200, FVID2_SF_PROGRESSIVE, 162000, 60,
        64, 304, 192, 1, 46, 3}, 1, 9, 0},
    {{FVID2_STD_1080I_60, 1920, 1080, FVID2_SF_INTERLACED, 74250, 30,
        88, 148, 44, 2, 15, 5}, 2, 4, 4},
    {{FVID2_STD_1080I_50, 1920, 1080, FVID2_SF_INTERLACED, 74250, 30,
        528, 148, 44, 2, 15, 5}, 2, 4, 4},
    {{FVID2_STD_480P, 720, 480, FVID2_SF_PROGRESSIVE, 27000, 60,
        16, 60, 62, 9, 30, 6}, 1, 4, 4},
    {{FVID2_STD_576P, 720, 576, FVID2_SF_PROGRESSIVE, 27000, 50,
        12, 68, 64, 5, 39, 5}, 5, 4, 4},
};

struct
{
    Int32 m[3][3][3];
} static vpsHalHsvCoeff = {
    {
        {{602, 422, 35}, {116,  -116, 299}, {306, -306, -335}},
        {{601, -601, -1075}, {116, 908, -207}, {306, -306, 1280}},
        {{601, -601, 337}, {116, -116, -508}, {306, 718, 172}}
    }
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void hdVencProgramShiftScale(const HdVenc_Obj *hdVencObj,
                                    const VpsHal_HdVencAnalogConfig *aCfg);
static Void hdVencProgramCscCoeff(const HdVenc_Obj *hdVencObj,
                                  const HdVenc_OutputCfg *cfg);
static UInt32 hdVencMakeCscReg(UInt16 coeff1, UInt16 coeff2);
static Void hdVmakeCfg0Reg(CSL_VpsHdVencdRegsOvly  regsOvly, const VpsHal_HdVencConfig *cfg);
static UInt32 hdVencMakeShiftReg(UInt16 scale, UInt16 shift, UInt16 blLevel);
static Void hdVencProgramGamCoeff(const HdVenc_Obj *hdVencObj,
                                  const VpsHal_HdVencConfig *cfg);
static Void hdVgetCfg0Reg(UInt32 regVal, VpsHal_HdVencConfig *cfg);
static Void VpsHal_hdVencPrintConfig(VpsHal_HdVencConfig *vencCfg);
static Int32 vpsHalHdVencSetOutputCfg(HdVenc_Obj *hdVencObj);
static Void hdVencSetPolarity(Vps_DcOutputInfo *output,
                              HdVenc_OutputCfg *outputCfg);
static Int32 VpsHal_hdVencSetControl(HdVenc_Obj *hdVencObj, Vps_DcVencControl *ctrl);
static Int32 VpsHal_hdVencGetControl(HdVenc_Obj *hdVencObj, Vps_DcVencControl *ctrl);
static Void hdVencSetModeTiming(HdVenc_Obj *hdVencObj,
                                FVID2_ModeInfo *tInfo);
static Void hdVencSetFidOffset(HdVenc_Obj *hdVencObj,
                               UInt32 FidOffset0,
                               UInt32 FidOffset1);
static Void hdVencSetDefaults(HdVenc_Obj *hdVencObj);

#if 0
static Void hdVencSetDvoConfig(HdVenc_Obj *hdVencObj,
                               FVID2_ModeInfo *mInfo);
static Void hdVencSetOsdConfig(HdVenc_Obj *hdVencObj,
                               FVID2_ModeInfo *mInfo);
static Void hdVencSetAnalogConfig(HdVenc_Obj *hdVencObj,
                                  FVID2_ModeInfo *mInfo);
#endif


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static HdVenc_Obj HdVencObj[VPSHAL_HDVENC_MAX_INST];

/**
 *  \brief Default Value of Color Space Conversion coefficients
 */
static VpsHal_HdVencCscCoeff CscDefaultCoeff[VPSHAL_HDVENC_CSC_NUM_MODE] =
{
    /* SDTV R2Y Video Data Range */
    {
        {{0x259, 0x75, 0x132},
        {0x1EA5, 0x20B, 0x1F50},
        {0x1E4A, 0x1FAB, 0x20B},},
        {0x0, 0x200, 0x200}
    },
    /* SDTV Y2R Video Data Range */
    {
        {{0x400, 0x1EA7, 0x1D35},
        {0x400, 0x6EF, 0x1FFE},
        {0x400, 0x0, 0x57D},},
        {0x210, 0xC88, 0xD40}
    },
    /* SDTV R2Y Graphics Data Range */
    {
        {{0x204, 0x64, 0x107},
        {0x1ED6, 0x1C2, 0x1F68},
        {0x1E87, 0x1FB7, 0x1C2},},
        {0x40, 0x200, 0x200}
    },
    /* SDTV Y2R Graphics Data Range */
    {
        {{0x4A8, 0x1E6F, 0x1CBF},
        {0x04A8, 0x0812, 0x1FFF},
        {0x04A8, 0x1FFE, 0x0662},},
        {0x220, 0xBAC, 0xC84}
    },
     /* HDTV R2Y Video data Range */
    {
        {{0x2DC, 0x4A, 0xDA},
        {0x1E6C, 0x20C, 0x1F88},
        {0x1E24, 0x1FD0, 0x20C},},
        {0x0, 0x200, 0x200}
    },
    /* HDTV Y2R Video data range */
    {
        {{0x400, 0x1F45, 0x1E2B},
        {0x400, 0x742, 0x0},
        {0x400, 0x0, 0x0629},},
        {0x148, 0xC60, 0xCEC}
    },
    /* HDTV R2Y Graphics data Range */
    {
        {{0x275, 0x3f, 0xBB},
        {0x1EA5, 0x1C2, 0x1F99},
        {0x1E67, 0x1FD7, 0x1C2},},
        {0x40, 0x200, 0x200}
    },
    /* HDTV Y2R grahics data range */
    {
        {{0x4A8, 0x1F26, 0x1DDE},
        {0x4A8, 0x0873, 0x0},
        {0x4A8, 0x0, 0x072C},},
        {0x134, 0xB7c, 0xC20}
    },
    /* CSC coefficients for RGB to RGB mode */
    {
        {
            /* A0,    B0,      C0 */
            {0x0400u, 0x0000u, 0x0000u},
            /* A1,    B1,      C1 */
            {0x0000u, 0x0400u, 0x0000u},
            /* A2,    B2,      C2 */
            {0x0000u, 0x0000u, 0x0400u}
        },
        /* D0,    D1,      D2 */
        {0x0000u, 0x0000u, 0x0000u}
    }
};

VpsHal_hdVencCscQFormCoeff CscQFormCoeff[VPSHAL_HDVENC_CSC_NUM_MODE] = {
    /* SDTV R2Y Video Data Range */
    {
        {
            {601, 117, 306},
            {-347, 523, -176},
            {-438, -85, 523}
        },
        {0, 512, 512}
    },
    /* SDTV Y2R Video Data Range */
    {
        {
            {1024, -345, -715},
            {1024, 1775, -2},
            {1024, 0, 1405}
        },
        {528, -888, -704}
    },
    /* SDTV R2Y Graphics Data Range */
    {
        {
            {516, 100, 263},
            {-298, 450, -152},
            {-377, -73, 450}
        },
        {64, 512, 512}
    },
    /* SDTV Y2R Graphics Data Range */
    {
        {
            {1192, -401, -833},
            {1192, 2066, -1},
            {1192, -2, 1634},
        },
        {544, -1108, -892}
    },
     /* HDTV R2Y Video data Range */
    {
        {
            {732, 74, 218},
            {-404, 524, -120},
            {-476, -48, 524}
        },
        {0, 512, 512}
    },
    /* HDTV Y2R Video data range */
    {
        {
            {1024, -187, -469},
            {1024, 1858, 0},
            {1024, 0, 1577},
        },
        {328, -928, -788}
    },
    /* HDTV R2Y Graphics data Range */
    {
        {
            {629, 63, 187},
            {-347, 450, -103},
            {-409, -41, 450}
        },
        {64, 512, 512}
    },
    /* HDTV Y2R grahics data range */
    {
        {
            {1192, -218, -546},
            {1192, 2163, 0},
            {1192, 0, 1836}
        },
        {308, -1156, -992}
    },
    {
        {
            {0x0400, 0x0000, 0x0000},
            {0x0000, 0x0400, 0x0000},
            {0x0000, 0x0000, 0x0400}
        },
        {0x0000, 0x0000, 0x0000}
    }
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_hdvencInit
 *  \brief Initializes HDVENC objects, gets the register overlay offsets for
 *  HDVENC registers.
 *  This function should be called before calling any of HDVENC HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int VpsHal_hdvencInit(UInt32 numInstances,
                      const VpsHal_HdVencInstParams *initParams,
                      Ptr arg)
{
    Int                     instCnt;
    Int                     ret = 0;
    UInt32                  encCnt = 0;
    HdVenc_Obj              *instInfo = NULL;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_HDVENC_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt ++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_HDVENC_MAX_INST));

        /* Initialize Instance Specific Parameters */
        instInfo = &(HdVencObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regsOvly =
                (CSL_VpsHdVencdRegsOvly ) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;

        /* TODO: Ideally this gRegBase should come from the CSLR */
        instInfo->gRegBase = (volatile UInt32*)
            (UInt32)initParams[instCnt].baseAddress +
                                VPSHAL_HDVENC_GAMMA_REGS_OFFSET;
        /* Bydefault 1080p-60 mode is set in the venc */
        instInfo->currModeIdx = 0u;

        /* Paired encoder details */
        instInfo->noOfEncs = initParams[instCnt].pairedEncCnt;
        for (encCnt = 0x0; encCnt < initParams[instCnt].pairedEncCnt; encCnt++)
        {
            instInfo->enc[encCnt].encId = initParams[instCnt].encIds[encCnt];
            instInfo->enc[encCnt].getModeEnc = NULL;
            instInfo->enc[encCnt].setModeEnc = NULL;
            instInfo->enc[encCnt].startEnc = NULL;
            instInfo->enc[encCnt].stopEnc = NULL;
            instInfo->enc[encCnt].controlEnc = NULL;
            instInfo->enc[encCnt].handle = NULL;
        }

        /* Set the Defaults */
        instInfo->output.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
        instInfo->output.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
        instInfo->output.dataFormat = FVID2_DF_YUV444P;

        instInfo->outputCfg.dvoCfg.invtfidPol = FALSE;
        instInfo->outputCfg.dvoCfg.invtVsPol = FALSE;
        instInfo->outputCfg.dvoCfg.invtHsPol = FALSE;
        instInfo->outputCfg.dvoCfg.invtActsPol = FALSE;
        instInfo->outputCfg.dvoCfg.decimationMethod =
            VPSHAL_HDVENC_444TO422_ALTERNATEPIXEL;
        instInfo->outputCfg.dvoCfg.dvoOff = FALSE;
        VpsHal_hdVencSetOutput((VpsHal_Handle)instInfo, &instInfo->output);

        /* Stopping Venc at the time of init */
        VpsHal_hdvencStopVenc((VpsHal_Handle)instInfo);

        instInfo->brightness = 0;
        instInfo->contrast = 128;
        instInfo->saturation = 128;
        instInfo->hue = 31;
    }

    return (ret);
}



/**
 *  VpsHal_hdvencDeInit
 *  \brief Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int VpsHal_hdvencDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_hdvencOpen
 *  \brief This function should be called prior to calling any of the HDVENC HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param sdvencInst       Requested HDVENC instance
 */
VpsHal_Handle VpsHal_hdvencOpen(UInt32 sdvencInst)
{
    Int                     cnt;
    UInt32                  cookie;
    VpsHal_Handle           handle = NULL;
    HdVenc_Obj             *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_HDVENC_MAX_INST; cnt++)
    {
        instInfo = &(HdVencObj[cnt]);
        /* Return the matching instance handle */
        if (sdvencInst == instInfo->instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == instInfo->openCnt)
            {
                handle = (VpsHal_Handle) (instInfo);
                instInfo->openCnt ++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return (handle);
}

/**
 *  VpsHal_hdvencClose
 *  \brief This functions closes the sdvenc handle and after call to this
 *  function, sdvenc handle is not valid. It also decrements the reference
 *  counter.
 *
 *  \param handle       Requested HDVENC instance
 */
Int VpsHal_hdvencClose(VpsHal_Handle handle)
{
    Int                 ret = -1;
    UInt32              cookie;
    HdVenc_Obj         *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    instInfo = (HdVenc_Obj *) handle;

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
 *  VpsHal_hdVencGetMode
 *  \brief This functions to get the mode.
 *
 *  \param handle         Requested HDVENC instance
 *  \param mode           Mode information parameters to be returned.
 */
Int32 VpsHal_hdVencGetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode)
{
    HdVenc_Obj              *hdVencObj = NULL;
    UInt32                  vencId;
    UInt32                  encCnt;
    Int32                   retVal  =   VPS_SOK;
    Vps_DcModeInfo          encMode;
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != mode));

    hdVencObj = (HdVenc_Obj *)handle;
    vencId = mode->vencId;
    VpsUtils_memcpy(mode,
                    &hdVencObj->mode,
                    sizeof(Vps_DcModeInfo));

    if (VPS_SOK == retVal)
    {
        for(encCnt = 0x0; encCnt < hdVencObj->noOfEncs; encCnt++)
        {
            if (hdVencObj->enc[encCnt].getModeEnc != NULL)
            {
                retVal = hdVencObj->enc[encCnt].getModeEnc (
                                hdVencObj->enc[encCnt].handle,
                                &encMode);
                if (VPS_SOK != retVal)
                {
                    /* Let the apps know the reason */
                    break;
                }
            }
        }
    }
    mode->vencId = vencId;
    return (retVal);

}

/**
 * VpsHal_hdVencSetMode
 * \brief This function is used to set the mode on the HDVENC
 *
 * \param handle          Requested HDVENC instance
 *
 * \param mode            Mode information
 */
Int32 VpsHal_hdVencSetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode)
{

    HdVenc_Obj              *hdVencObj = NULL;
    Int32                    retVal = VPS_EFAIL;
    UInt32                   modeCnt, numStdModes;
    VpsHal_HdVencConfig     *vencCfg = NULL;
    UInt32                   encCnt;
    CSL_VpsHdVencdRegsOvly   regsOvly = NULL;
    FVID2_ModeInfo          *mInfo = NULL;
    UInt32                   dispMode, osdFidSt1, osdFidSt2;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != mode));

    hdVencObj = (HdVenc_Obj *)handle;
    regsOvly = hdVencObj->regsOvly;

    /* Copy first the default parameters */
    vencCfg = &hdVencObj->vencCfg;
    vencCfg->hdVencMode = mode->mInfo.standard;
    if (FVID2_STD_CUSTOM != vencCfg->hdVencMode)
    {
        numStdModes = sizeof(HdVencStdModeInfo) / sizeof(HdVenc_StdModeInfo);

        for (modeCnt = 0u; modeCnt < numStdModes; modeCnt ++)
        {
            if (vencCfg->hdVencMode == HdVencStdModeInfo[modeCnt].mInfo.standard)
            {
                mInfo = &(HdVencStdModeInfo[modeCnt].mInfo);
                dispMode = HdVencStdModeInfo[modeCnt].dispMode;
                osdFidSt1 = HdVencStdModeInfo[modeCnt].osdFidSt1;
                osdFidSt2 = HdVencStdModeInfo[modeCnt].osdFidSt2;
                break;
            }
        }
    }
    else
    {
        dispMode = mode->mode;
        mInfo = &(mode->mInfo);

        /* Assume FLD delay fix for custom mode as a nominal value of 4 lines */
        osdFidSt1 = 4u;
        osdFidSt2 = 4u;
    }

    if (NULL != mInfo)
    {
        hdVencObj->vencCfg.scanFormat = mInfo->scanFormat;
        hdVencObj->vencCfg.bypass2XUpSampling = TRUE;
        hdVencObj->vencCfg.gammaCoeffs = NULL;
        hdVencObj->vencCfg.dispMode = (VpsHal_HdVDispMode)dispMode;
        hdVencObj->vencCfg.bypassGammaCor = TRUE;
        hdVencObj->vencCfg.rdGammaCorMem = FALSE;
        hdVencObj->vencCfg.selfTest = FALSE;

        VpsUtils_memcpy(
            &hdVencObj->vencCfg.analogConfig,
            &gHdVencDefCfg.analogConfig,
            sizeof(hdVencObj->vencCfg.analogConfig));
        VpsUtils_memcpy(
            &hdVencObj->vencCfg.dvoConfig,
            &gHdVencDefCfg.dvoConfig,
            sizeof(hdVencObj->vencCfg.dvoConfig));
        VpsUtils_memcpy(
            &hdVencObj->vencCfg.osdConfig,
            &gHdVencDefCfg.osdConfig,
            sizeof(hdVencObj->vencCfg.osdConfig));
        VpsUtils_memcpy(
            &hdVencObj->vencCfg.mvConfig,
            &gHdVencDefCfg.mvConfig,
            sizeof(hdVencObj->vencCfg.mvConfig));
        VpsUtils_memcpy(
            &hdVencObj->vencCfg.vbiConfig,
            &gHdVencDefCfg.vbiConfig,
            sizeof(hdVencObj->vencCfg.vbiConfig));

        hdVencSetDefaults(hdVencObj);

        if ((FVID2_DF_RGB24_888 == hdVencObj->output.dataFormat) &&
            ((VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC == hdVencObj->output.dvoFmt) ||
                (VPS_DC_DVOFMT_DOUBLECHAN_DISCSYNC == hdVencObj->output.dvoFmt)))
        {
            hdVencObj->vencCfg.analogConfig.scale0Coeff = 0xb00;
            hdVencObj->vencCfg.analogConfig.shift0Coeff = 0;
            hdVencObj->vencCfg.analogConfig.blank0Level = 0;

            hdVencObj->vencCfg.analogConfig.scale1Coeff = 0xb00;
            hdVencObj->vencCfg.analogConfig.shift1Coeff = 0;
            hdVencObj->vencCfg.analogConfig.blank1Level = 0;

            hdVencObj->vencCfg.analogConfig.scale2Coeff = 0xb00;
            hdVencObj->vencCfg.analogConfig.shift2Coeff = 0;
            hdVencObj->vencCfg.analogConfig.blank2Level = 0;

            /*
                SOG was not removed 100% when VENC is configured as discreted sync output due to
                min and max SYNC pulse was non-zero, which may affect VGA output.
            */
            hdVencObj->vencCfg.analogConfig.syncHighLevelAmp = 0;
            hdVencObj->vencCfg.analogConfig.syncLowLevelAmp = 0;
        }

        hdVencSetModeTiming(hdVencObj, mInfo);
        hdVencSetFidOffset(hdVencObj, osdFidSt1, osdFidSt2);

        /* Set the Video Mode in the register */
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_DM_SEL_MASK;
        regsOvly->CFG0 |= ((dispMode <<
                CSL_VPS_HD_VENC_A_CFG0_DM_SEL_SHIFT) &
                    CSL_VPS_HD_VENC_A_CFG0_DM_SEL_MASK);

        /* Set the default parameters according to mode selected */
        retVal = VpsHal_hdVencSetConfig(handle, &hdVencObj->vencCfg);
    }

    /* Copy various frame params for app usage like height, width etc
     * based on timing parameters set for e.g. 1080P60 width is 1920 and
     * height is 1080. etc.
     */
    if (VPS_SOK == retVal)
    {
        hdVencObj->mode.mode = dispMode;
        VpsUtils_memcpy(&(hdVencObj->mode.mInfo),
                        mInfo,
                        sizeof(FVID2_ModeInfo));
    }

    /* For all the paired on-chip encoders - update the new mode */
    if (VPS_SOK == retVal)
    {
        for(encCnt = 0u; encCnt < hdVencObj->noOfEncs; encCnt++)
        {
            if (hdVencObj->enc[encCnt].setModeEnc != NULL)
            {
                retVal = hdVencObj->enc[encCnt].setModeEnc(
                                hdVencObj->enc[encCnt].handle,
                                mode);
            }
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }
    return retVal;
}

/**
 *  VpsHal_hdVencSetOutput
 *  \brief This function is used to configure ouptut type for HD Venc
 *
 *  \param handle         Requested HDVENC instance
 *
 *  \param output         Output format
 */
Int32 VpsHal_hdVencSetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output)
{
    Int32                       retVal          = VPS_SOK;
    HdVenc_Obj                 *hdVencObj      = NULL;
    HdVenc_OutputCfg           *outputCfg;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != output));

    hdVencObj = (HdVenc_Obj *)handle;
    outputCfg = &hdVencObj->outputCfg;

    /* Depending on the venc, set the output */
    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        outputCfg->analogCfg.dacInvertData = FALSE;
        switch (output->aFmt)
        {
            case VPS_DC_A_OUTPUT_COMPOSITE:
                outputCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN;
                /* Turn ON only one DAC as its a single channnel output */
                outputCfg->analogCfg.dac0PwDn = FALSE;
                outputCfg->analogCfg.dac1PwDn = TRUE;
                outputCfg->analogCfg.dac2PwDn = TRUE;
                /* Turn on the reference voltage to the DAC */
                outputCfg->analogCfg.dac0RfPdn = FALSE;
                break;
            case VPS_DC_A_OUTPUT_SVIDEO:

                /*
                    Sync on Green should be disabled once we select
                    component output with Discrete sync. By default
                    hal was configuring for emb sync for component output.
                    Now HAL takes from dvofmt.
                */
                if (output->dvoFmt == VPS_DC_DVOFMT_DOUBLECHAN_DISCSYNC)
                    outputCfg->fmt =
                        VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC;
                else
                    outputCfg->fmt =
                        VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN;

                /* Turn ON only one DAC as its a single channnel output */
                outputCfg->analogCfg.dac0PwDn = FALSE;
                outputCfg->analogCfg.dac1PwDn = FALSE;
                outputCfg->analogCfg.dac2PwDn = TRUE;
                /* Turn on the reference voltage to the DAC */
                outputCfg->analogCfg.dac0RfPdn = FALSE;
                break;
            case VPS_DC_A_OUTPUT_COMPONENT:

                /*
                    Sync on Green should be disabled once we select
                    component output with Discrete sync. By default
                    hal was configuring for emb sync for component output.
                    Now HAL takes from dvofmt.
                */
                if (output->dvoFmt == VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC)
                    outputCfg->fmt =
                        VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC;
                else
                    outputCfg->fmt =
                        VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC;

                /* Turn ON only one DAC as its a single channnel output */
                outputCfg->analogCfg.dac0PwDn = FALSE;
                outputCfg->analogCfg.dac1PwDn = FALSE;
                outputCfg->analogCfg.dac2PwDn = FALSE;
                /* Turn on the reference voltage to the DAC */
                outputCfg->analogCfg.dac0RfPdn = FALSE;
                break;
            default :
                retVal = VPS_EINVALID_PARAMS;
        }
    }
    else if (VPSHAL_HDVENC_HDMI == hdVencObj->instId ||
                VPSHAL_HDVENC_DVO2 == hdVencObj->instId)
    {
        switch (output->dvoFmt)
        {
            case VPS_DC_DVOFMT_SINGLECHAN:
                outputCfg->fmt =
                    VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN;
                break;
            case VPS_DC_DVOFMT_DOUBLECHAN:
                outputCfg->fmt =
                    VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN;
                break;
            case VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC:
                outputCfg->fmt =
                    VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC;
                break;
            case VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC:
                outputCfg->fmt =
                    VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC;
                break;
            case VPS_DC_DVOFMT_DOUBLECHAN_DISCSYNC:
                outputCfg->fmt =
                    VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC;
                break;
            default:
                retVal = VPS_EINVALID_PARAMS;
        }
    }
    else
    {
        retVal = VPS_EINVALID_PARAMS;
    }
    if (output->dataFormat == FVID2_DF_RGB24_888)
    {
        outputCfg->outColorSpace = VPSHAL_HDVENC_COLORSPACE_RGB;
        outputCfg->cscMode = VPSHAL_HDVENC_CSCMODE_R2R;
        outputCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_COLORSPC;
    }
    else if ((FVID2_DF_YUV444P == output->dataFormat) ||
             (FVID2_DF_YUV422SP_UV == output->dataFormat))
    {
        outputCfg->outColorSpace = VPSHAL_HDVENC_COLORSPACE_YUV;
        outputCfg->cscMode = VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_R2Y;
        outputCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_COLORSPC;
    }
    else
    {
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Copy Current CSC Coefficients */
        VpsUtils_memcpy(&outputCfg->cscCoeff,
                        &CscDefaultCoeff[outputCfg->cscMode],
                        sizeof(VpsHal_HdVencCscCoeff));

        /*program polarity*/
        hdVencSetPolarity(output, outputCfg);
        /* Program CSC Coefficients */
#if defined(TI_8107_BUILD) && defined(VPSHAL_VENC_SWAP_COLORS_FOR_VGA_OUT_ON_TI813X)
    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        memset(&outputCfg->cscCoeff, 0, sizeof(outputCfg->cscCoeff));

        outputCfg->cscCoeff.mulCoeff[2][1] = 
            outputCfg->cscCoeff.mulCoeff[1][0] = 
                outputCfg->cscCoeff.mulCoeff[0][2] = 0x400;
    }
#endif
        hdVencProgramCscCoeff(hdVencObj, outputCfg);
        retVal = vpsHalHdVencSetOutputCfg(hdVencObj);

        if (FVID2_SOK == retVal)
        {
            VpsUtils_memcpy(&hdVencObj->output,
                            output,
                            sizeof(Vps_DcOutputInfo));
        }
    }

    return retVal;
}




/**
 * VpsHal_hdVencGetOutput
 * \brief This function is used to get the output set on the venc.
 *
 * \param handle        Requested HDVENC instance
 *
 * \param mode          Output format to be returned
 */
Void VpsHal_hdVencGetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output)
{
    HdVenc_Obj                 *hdVencObj      = NULL;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != output));

    hdVencObj = (HdVenc_Obj *)handle;

    VpsUtils_memcpy(output,
                    &hdVencObj->output,
                    sizeof(Vps_DcOutputInfo));
}

/**
 * VpsHal_hdVencSetConfig
 * \brief This function sets the HD venc registers according to the
 * parameters passed by the calling function
 * \param handle            Requested HDVENC handle
 * \param vencCfg           Configuration to be passed by calling function.
 */
Int VpsHal_hdVencSetConfig(VpsHal_Handle handle, VpsHal_HdVencConfig *vencCfg)
{
    Int32 retVal = VPS_SOK;
    HdVenc_Obj              *hdVencObj = NULL;
    CSL_VpsHdVencdRegsOvly  regsOvly = NULL;
    UInt32                  peakValueHigh, peakValueLow;
    UInt32                  pbPrShiftingHigh, pbPrShiftingLow;
    UInt32                  VencEnableDelayCountHigh, VencEnableDelayCountLow;
    UInt32                  backPorchPeakValueHigh, backPorchPeakValueLow;

    const                   VpsHal_HdVencAnalogConfig *aCfg;
    const                   VpsHal_HdVencDvoConfig *dCfg;
    const                   VpsHal_HdVencOsdConfig *oCfg;
    const                   VpsHal_HdVencMvConfig *mCfg;


    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != vencCfg));

    aCfg = &vencCfg->analogConfig;
    dCfg = &vencCfg->dvoConfig;
    oCfg = &vencCfg->osdConfig;
    mCfg = &vencCfg->mvConfig;

    hdVencObj = (HdVenc_Obj *)handle;
    regsOvly = hdVencObj->regsOvly;

    if (VPS_SOK == retVal)
    {
        hdVmakeCfg0Reg(regsOvly, vencCfg);

        /* Program the CSC coefficients */
        /* program the remaining bits of the CFG1 to CFG6 registers. */
        peakValueHigh = ((mCfg->peakingValue & 0x700) >> 8);
        regsOvly->CFG5 |= ((peakValueHigh <<
                            CSL_VPS_HD_VENC_A_CFG5_MV_PK_HI2_SHIFT) &
                            CSL_VPS_HD_VENC_A_CFG5_MV_PK_HI2_MASK);

        peakValueLow = (mCfg->peakingValue & 0xFF);
        regsOvly->CFG6 |= ((peakValueLow <<
                            CSL_VPS_HD_VENC_A_CFG6_MV_PK_LOW8_SHIFT) &
                            CSL_VPS_HD_VENC_A_CFG6_MV_PK_LOW8_MASK);

        hdVencProgramShiftScale(hdVencObj, &vencCfg->analogConfig);
        /* Program the remaining bits of the CFG7 to CFG9 registers */
        regsOvly->CFG10 = 0;
        regsOvly->CFG10 = ((aCfg->totalPixelsPerLine <<
                                CSL_VPS_HD_VENC_A_CFG10_PIXELS_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG10_PIXELS_MASK) |
                            ((aCfg->totalLinesPerFrame <<
                                CSL_VPS_HD_VENC_A_CFG10_LINES_SHIFT)  &
                                CSL_VPS_HD_VENC_A_CFG10_LINES_MASK) |
                            ((aCfg->clampPeriod  <<
                                CSL_VPS_HD_VENC_A_CFG10_CLAMP_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG10_CLAMP_MASK);
        regsOvly->CFG11 = 0;
        regsOvly->CFG11 = ((aCfg->vbiLineEnd0  <<
                                CSL_VPS_HD_VENC_A_CFG11_V_BLA1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG11_V_BLA1_MASK) |
                            ((aCfg->vbiLineend1 <<
                                CSL_VPS_HD_VENC_A_CFG11_V_BLA2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG11_V_BLA2_MASK) |
                            ((aCfg->equPulseWidth <<
                                CSL_VPS_HD_VENC_A_CFG11_EQ_WTH_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG11_EQ_WTH_MASK);
        regsOvly->CFG12 = 0;
        regsOvly->CFG12 = ((aCfg->hbiPixelEnd <<
                                CSL_VPS_HD_VENC_A_CFG12_H_BLANK_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG12_H_BLANK_MASK) |
                            ((aCfg->numActivePixels <<
                                CSL_VPS_HD_VENC_A_CFG12_ACT_PIX_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG12_ACT_PIX_MASK) |
                            ((aCfg->hsWidth <<
                                CSL_VPS_HD_VENC_A_CFG12_HS_WTH_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG12_HS_WTH_MASK);
        regsOvly->CFG13 = 0;
        pbPrShiftingHigh = ((aCfg->pbPrShifting & 0xF00) >> 8);
        VencEnableDelayCountHigh = ((aCfg->VencEnableDelayCount & 0xF00) >> 8);
        regsOvly->CFG13 = ((aCfg->lastActiveLineFirstField <<
                                CSL_VPS_HD_VENC_A_CFG13_END_F1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG13_END_F1_MASK) |
                            ((oCfg->HbiSignalStartPixel <<
                                CSL_VPS_HD_VENC_A_CFG13_OSD_HBI_ST_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG13_OSD_HBI_ST_MASK) |
                            ((pbPrShiftingHigh <<
                                CSL_VPS_HD_VENC_A_CFG13_CBCR_S2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG13_CBCR_S2_MASK) |
                            ((VencEnableDelayCountHigh <<
                                CSL_VPS_HD_VENC_A_CFG13_DELAY_VENC_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG13_DELAY_VENC_MASK);

        regsOvly->CFG14 = 0;
        pbPrShiftingLow = (aCfg->pbPrShifting & 0xFF);
        regsOvly->CFG14 = ((aCfg->syncHighLevelAmp  <<
                                CSL_VPS_HD_VENC_A_CFG14_SYNC_H_SHIFT)&
                                CSL_VPS_HD_VENC_A_CFG14_SYNC_H_MASK) |
                            ((aCfg->syncLowLevelAmp <<
                                CSL_VPS_HD_VENC_A_CFG14_SYNC_L_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG14_SYNC_L_MASK) |
                            ((pbPrShiftingLow <<
                                CSL_VPS_HD_VENC_A_CFG14_CBCR_S1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG14_CBCR_S1_MASK);
        regsOvly->CFG15 = 0;
        regsOvly->CFG15 = ((dCfg->firstActivePixelLine <<
                                CSL_VPS_HD_VENC_A_CFG15_DVO_AVST_H_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG15_DVO_AVST_H_MASK) |
                            ((dCfg->activeVideoLineWidth <<
                                CSL_VPS_HD_VENC_A_CFG15_DVO_AVD_HW_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG15_DVO_AVD_HW_MASK) |
                            ((dCfg->hsWidth <<
                                CSL_VPS_HD_VENC_A_CFG15_DVO_HS_WD_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG15_DVO_HS_WD_MASK);

        regsOvly->CFG16 = 0;
        backPorchPeakValueLow = mCfg->backPorchPeakValue & 0xFF;
        regsOvly->CFG16 = ((dCfg->hsStartPixel <<
                                CSL_VPS_HD_VENC_A_CFG16_DVO_HS_ST_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG16_DVO_HS_ST_MASK) |
                            ((dCfg->activeLineStart0 <<
                                CSL_VPS_HD_VENC_A_CFG16_DVO_AVST_V1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG16_DVO_AVST_V1_MASK) |
                            ((backPorchPeakValueLow <<
                                CSL_VPS_HD_VENC_A_CFG16_BP_PK_L_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG16_BP_PK_L_MASK);
        regsOvly->CFG17 = 0;
        backPorchPeakValueHigh = ((mCfg->backPorchPeakValue & 0x300) >> 0x8);
        regsOvly->CFG17 = ((dCfg->activeLineStart1 <<
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVST_V2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVST_V2_MASK) |
                            ((dCfg->activeVideoLines0 <<
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVD_VW1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVD_VW1_MASK) |
                            ((backPorchPeakValueHigh <<
                                CSL_VPS_HD_VENC_A_CFG17_BP_PK_H_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG17_BP_PK_H_MASK);
        regsOvly->CFG18 = 0;
        regsOvly->CFG18 = ((dCfg->activeVideoLines1 <<
                                CSL_VPS_HD_VENC_A_CFG18_DVO_AVD_VW2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG18_DVO_AVD_VW2_MASK) |
                            ((dCfg->vsStartLine0 <<
                                CSL_VPS_HD_VENC_A_CFG18_DVO_VS_ST1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG18_DVO_VS_ST1_MASK) |
                            ((dCfg->vsWidth0 <<
                                CSL_VPS_HD_VENC_D_CFG18_DVO_VS_WD1_SHIFT) &
                                CSL_VPS_HD_VENC_D_CFG18_DVO_VS_WD1_MASK);
        regsOvly->CFG19 = 0;
        regsOvly->CFG19 = ((dCfg->vsStartLine1 <<
                                CSL_VPS_HD_VENC_A_CFG19_DVO_VS_ST2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG19_DVO_VS_ST2_MASK) |
                            ((dCfg->fidToggleLine0 <<
                                CSL_VPS_HD_VENC_A_CFG19_DVO_FID_ST1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG19_DVO_FID_ST1_MASK) |
                            ((dCfg->vsWidth1 <<
                                CSL_VPS_HD_VENC_A_CFG19_DVO_VS_WD2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG19_DVO_VS_WD2_MASK);
        regsOvly->CFG20 = 0;
        VencEnableDelayCountLow = aCfg->VencEnableDelayCount & 0xFF;
        regsOvly->CFG20 = ((dCfg->fidToggleLine1 <<
                                CSL_VPS_HD_VENC_A_CFG20_DVO_FID_ST2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG20_DVO_FID_ST2_MASK) |
                            ((oCfg->fidToggleLine1 <<
                                CSL_VPS_HD_VENC_A_CFG20_OSD_FID_ST2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG20_OSD_FID_ST2_MASK) |
                            ((VencEnableDelayCountLow <<
                                CSL_VPS_HD_VENC_A_CFG20_DELAY_VENC_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG20_DELAY_VENC_MASK);
        regsOvly->CFG21 = 0;
        regsOvly->CFG21 = ((oCfg->lastNonActivePixelInLine <<
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVST_H_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVST_H_MASK) |
                            ((oCfg->activeVideoLineWidth <<
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVD_HW_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVD_HW_MASK) |
                            ((oCfg->hsWidth <<
                                CSL_VPS_HD_VENC_A_CFG21_OSD_HS_WD_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG21_OSD_HS_WD_MASK);
        regsOvly->CFG22 = 0;
        regsOvly->CFG22 = ((oCfg->hsStartPixel <<
                                CSL_VPS_HD_VENC_A_CFG22_OSD_HS_ST_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG22_OSD_HS_ST_MASK) |
                            ((oCfg->activeLineStart0 <<
                                CSL_VPS_HD_VENC_A_CFG22_OSD_AVST_V1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG22_OSD_AVST_V1_MASK) |
                            ((mCfg->mv100mvAgcPulseYSection <<
                                CSL_VPS_HD_VENC_A_CFG22_P_100MV_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG22_P_100MV_MASK);
        regsOvly->CFG23 = 0;
        regsOvly->CFG23 = ((oCfg->activeLineStart1 <<
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVST_V2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVST_V2_MASK) |
                            ((oCfg->activeVideoLines0 <<
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVD_VW1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVD_VW1_MASK) |
                            ((mCfg->mv450mvAgcPulseXSection <<
                                CSL_VPS_HD_VENC_A_CFG23_P_450MV_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG23_P_450MV_MASK);
        regsOvly->CFG24 = 0;
        regsOvly->CFG24 = ((oCfg->activeVideoLines1 <<
                                CSL_VPS_HD_VENC_A_CFG24_OSD_AVD_VW2_SHIFT)  &
                                CSL_VPS_HD_VENC_A_CFG24_OSD_AVD_VW2_MASK) |
                            ((oCfg->vsStartLine0 <<
                                CSL_VPS_HD_VENC_A_CFG24_OSD_VS_ST1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG24_OSD_VS_ST1_MASK) |
                            ((oCfg->vsWidth0 <<
                                CSL_VPS_HD_VENC_A_CFG24_OSD_VS_WD1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG24_OSD_VS_WD1_MASK);
        regsOvly->CFG25 = 0;
        regsOvly->CFG25 = ((oCfg->vsStartLine1 <<
                                CSL_VPS_HD_VENC_A_CFG25_OSD_VS_ST2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG25_OSD_VS_ST2_MASK) |
                            ((oCfg->fidToggleLine0 <<
                                CSL_VPS_HD_VENC_A_CFG25_OSD_FID_ST1_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG25_OSD_FID_ST1_MASK) |
                            ((oCfg->vsWidth1 <<
                                CSL_VPS_HD_VENC_A_CFG25_OSD_VS_WD2_SHIFT) &
                                CSL_VPS_HD_VENC_A_CFG25_OSD_VS_WD2_MASK);
        /* Copy the configuration so that it can be returned during the
        getconfig API */
        VpsUtils_memcpy(&hdVencObj->vencCfg,
                        vencCfg,
                        sizeof(VpsHal_HdVencConfig));
        if (NULL != vencCfg->gammaCoeffs)
        {
            hdVencProgramGamCoeff(hdVencObj, vencCfg);
        }
    }
    return (retVal);
}


/**
 * VpsHal_hdvencInitEncOps
 * \brief Initializes venc with capabilities of paired on-chip encoders
 *
 * \param handle    Handle to HD Venc HAL
 * \param encOps    Capabilities of the encoder
 */
Int32 VpsHal_hdvencInitEncOps(VpsHal_Handle handle,Vps_OnChipEncoderOps *encOps)
{
    Int32       rtnValue    =   VPS_EBADARGS;
    UInt32      noOfEncs    =   0x0;
    HdVenc_Obj  *hdVencObj  =   NULL;
    UInt32  cookie;
    GT_assert(VpsHalTrace, (NULL != encOps));
    GT_assert(VpsHalTrace, (NULL != handle));

    /*
     * 1. Validate the arguments, including venc ID.
     * 2. Validate the venc status - If streaming - return error
     * 3. Copy the ops supplied - Under interrupt protection
     */
    while (TRUE)
    {
        if (encOps == NULL)
        {
            break;
        }
        hdVencObj = (HdVenc_Obj *)handle;
        for (   noOfEncs = 0x0;
                noOfEncs < VPSHAL_HDVENC_MAX_NUM_OF_ONCHIP_ENC;
                noOfEncs++)
        {
            if (hdVencObj->enc[noOfEncs].encId == encOps[noOfEncs].encId)
            {
                /* Exclusive access */
                cookie = Hwi_disable();
                /* Found the match - atlast, copy the ops */
                hdVencObj->enc[noOfEncs].getModeEnc =
                                                    encOps[noOfEncs].getModeEnc;
                hdVencObj->enc[noOfEncs].setModeEnc =
                                                    encOps[noOfEncs].setModeEnc;
                hdVencObj->enc[noOfEncs].startEnc   = encOps[noOfEncs].startEnc;
                hdVencObj->enc[noOfEncs].stopEnc    = encOps[noOfEncs].stopEnc;
                hdVencObj->enc[noOfEncs].controlEnc =
                                                    encOps[noOfEncs].controlEnc;
                hdVencObj->enc[noOfEncs].handle     = encOps[noOfEncs].handle;
                Hwi_restore(cookie);
                rtnValue    =   VPS_SOK;
                break;
            } /* Matching encoder IDs */
            else
            {
                rtnValue = VPS_EBADARGS;
                break;
            }
        } /* All supported encoder on this venc */
        break;
    }
    return (rtnValue);
}

/**
 * VpsHal_hdVencPrintConfig
 * \brief Prints the configuration.
 * \param handle            Requested HDVENC handle
 * \param vencCfg           Configuration to be passed by calling function.
 */
static Void VpsHal_hdVencPrintConfig(VpsHal_HdVencConfig *vencCfg)
{

    VpsHal_HdVencAnalogConfig   *aCfg;
    VpsHal_HdVencDvoConfig      *dCfg;
    VpsHal_HdVencOsdConfig      *oCfg;

    aCfg = &vencCfg->analogConfig;
    dCfg = &vencCfg->dvoConfig;
    oCfg = &vencCfg->osdConfig;

    GT_0trace(VpsHalTrace, GT_DEBUG,"{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dispMode =              = %d,\n", vencCfg->dispMode);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".scanFormat =            = %d,\n", vencCfg->scanFormat);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".bypass2XUpSampling      = %d,\n", vencCfg->bypass2XUpSampling);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".bypassCsc               = %d,\n", vencCfg->bypassCsc);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".bypassGammaCor          = %d,\n", vencCfg->bypassGammaCor);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".rdGammaCorMem           = %d,\n", vencCfg->rdGammaCorMem);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".selfTest                = %d,\n", vencCfg->startEncoder);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".cscMode                 = %d,\n\n", vencCfg->cscMode);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".outColorSpace           = %d,\n", aCfg->outColorSpace);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dac0PwDn                = %d,\n", aCfg->dac0PwDn);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dac1PwDn                = %d,\n", aCfg->dac1PwDn);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dac2PwDn                = %d,\n", aCfg->dac2PwDn);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dacInvertData           = %d,\n", aCfg->dacInvertData);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dac0RfPdn               = %d,\n", aCfg->dac0RfPdn);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dac1RfPdn               = %d,\n", aCfg->dac1RfPdn);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".scale0Coeff             = %d,\n", aCfg->scale0Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".shift0Coeff             = %d,\n", aCfg->shift0Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".blank0Level             = %d,\n", aCfg->blank0Level);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".scale1Coeff             = %d,\n", aCfg->scale1Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".shift1Coeff             = %d,\n", aCfg->shift1Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".blank1Level             = %d,\n", aCfg->blank1Level);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".scale2Coeff             = %d,\n", aCfg->scale2Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".shift2Coeff             = %d,\n", aCfg->shift2Coeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".blank2Level             = %d,\n", aCfg->blank2Level);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".totalPixelsPerLine      = %d,\n", aCfg->totalPixelsPerLine);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".totalLinesPerFrame      = %d,\n", aCfg->totalLinesPerFrame);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".clampPeriod             = %d,\n", aCfg->clampPeriod);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vbiLineEnd0             = %d,\n", aCfg->vbiLineEnd0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vbiLineend1             = %d,\n", aCfg->vbiLineend1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".equPulseWidth           = %d,\n", aCfg->equPulseWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hbiPixelEnd             = %d,\n", aCfg->hbiPixelEnd);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".numActivePixels         = %d,\n", aCfg->numActivePixels);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hsWidth                 = %d,\n", aCfg->hsWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".lastActiveLineFirstField = %d,\n", aCfg->lastActiveLineFirstField);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".pbPrShifting            = %d,\n", aCfg->pbPrShifting);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".VencEnableDelayCount    = %d,\n", aCfg->VencEnableDelayCount);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".syncHighLevelAmp        = %d,\n", aCfg->syncHighLevelAmp);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".syncLowLevelAmp         = %d\n", aCfg->syncLowLevelAmp);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "},\n\n");

    GT_0trace(VpsHalTrace, GT_DEBUG, "{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dataSource              = %d,\n", dCfg->dataSource);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".fmt                     = %d,\n", dCfg->fmt);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".invtfidPol              = %d,\n", dCfg->invtfidPol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".invtVsPol               = %d,\n", dCfg->invtVsPol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".invtHsPol               = %d,\n", dCfg->invtHsPol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".invtActsPol             = %d,\n", dCfg->invtActsPol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".decimationMethod        = %d,\n", dCfg->decimationMethod);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dvoOff                  = %d,\n", dCfg->dvoOff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsFidLineStart          = %d,\n", dCfg->vsFidLineStart);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".jeidaFmt                = %d,\n", dCfg->jeidaFmt);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".firstActivePixelLine    = %d,\n", dCfg->firstActivePixelLine);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLineWidth    = %d,\n", dCfg->activeVideoLineWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hsWidth                 = %d,\n", dCfg->hsWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hsStartPixel            = %d,\n", dCfg->hsStartPixel);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeLineStart0        = %d,\n", dCfg->activeLineStart0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeLineStart1        = %d,\n", dCfg->activeLineStart1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLines0       = %d,\n", dCfg->activeVideoLines0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLines1       = %d,\n", dCfg->activeVideoLines1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsStartLine0            = %d,\n", dCfg->vsStartLine0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsStartLine1            = %d,\n", dCfg->vsStartLine1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsWidth0                = %d,\n", dCfg->vsWidth0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsWidth1                = %d,\n", dCfg->vsWidth1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".fidToggleLine0          = %d,\n", dCfg->fidToggleLine0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".fidToggleLine1          = %d\n", dCfg->fidToggleLine1);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "},\n\n");

    GT_0trace(VpsHalTrace, GT_DEBUG,  "{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".invtfidPol              = %d,\n", oCfg->invtfidPol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".HbiSignalStartPixel     = %d,\n", oCfg->HbiSignalStartPixel);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".lastNonActivePixelInLine = %d,\n", oCfg->lastNonActivePixelInLine);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLineWidth    = %d,\n", oCfg->activeVideoLineWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hsWidth                 = %d,\n", oCfg->hsWidth);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".hsStartPixel            = %d,\n", oCfg->hsStartPixel);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeLineStart0        = %d,\n", oCfg->activeLineStart0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeLineStart1        = %d,\n", oCfg->activeLineStart1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLines0       = %d,\n", oCfg->activeVideoLines0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".activeVideoLines1       = %d,\n", oCfg->activeVideoLines1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsStartLine0            = %d,\n", oCfg->vsStartLine0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsStartLine1            = %d,\n", oCfg->vsStartLine1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsWidth0                = %d,\n", oCfg->vsWidth0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".vsWidth1                = %d,\n", oCfg->vsWidth1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".fidToggleLine0          = %d,\n", oCfg->fidToggleLine0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".fidToggleLine1          = %d\n", oCfg->fidToggleLine1);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "},\n\n");

    GT_0trace(VpsHalTrace, GT_DEBUG,
                "{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".macroVisionEnable       = %d\n", 0);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "},\n\n");

    GT_0trace(VpsHalTrace, GT_DEBUG, "{\n");
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".enableVbiLine0[0]       =%d,\n", 0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".enableVbiLine0[1]       =%d,\n", 0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".enableVbiLine1[0]       =%d,\n", 0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".enableVbiLine1[1]       =%d,\n", 0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".enableExternalVbi       =%d\n",  0);
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "}\n\n");
    GT_0trace(VpsHalTrace, GT_DEBUG,
                "}\n\n");
}

/**
 * VpsHal_hdVencGetConfig
 * \brief Get the configuration.
 * \param handle            Requested HDVENC handle
 * \param vencCfg           Venc configuration to be filled by driver
 */
Void VpsHal_hdVencGetConfig(VpsHal_Handle handle, VpsHal_HdVencConfig *vencCfg)
{
    HdVenc_Obj  *hdVencObj = NULL;
    CSL_VpsHdVencdRegsOvly  regsOvly = NULL;
    volatile UInt32         regValue;
    UInt32                  peakValueHigh, peakValueLow;
    UInt32                  pbPrShiftingHigh, pbPrShiftingLow;
    UInt32                  VencEnableDelayCountHigh, VencEnableDelayCountLow;
    UInt32                  backPorchPeakValueHigh, backPorchPeakValueLow;
    VpsHal_HdVencAnalogConfig   *aCfg;
    VpsHal_HdVencDvoConfig      *dCfg;
    VpsHal_HdVencOsdConfig      *oCfg;
    VpsHal_HdVencMvConfig       *mCfg;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != vencCfg));

    aCfg = &vencCfg->analogConfig;
    dCfg = &vencCfg->dvoConfig;
    oCfg = &vencCfg->osdConfig;
    mCfg = &vencCfg->mvConfig;

    hdVencObj = (HdVenc_Obj *)handle;
    regsOvly = hdVencObj->regsOvly;

    regValue =  regsOvly->CFG0;

    hdVgetCfg0Reg(regValue, vencCfg);

    peakValueHigh = ((regsOvly->CFG5 & CSL_VPS_HD_VENC_A_CFG5_MV_PK_HI2_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG5_MV_PK_HI2_SHIFT);

    peakValueLow =  ((regsOvly->CFG6 & CSL_VPS_HD_VENC_A_CFG6_MV_PK_LOW8_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG6_MV_PK_LOW8_SHIFT);

    mCfg->peakingValue = (peakValueHigh << 8) |  peakValueLow;

    aCfg->scale0Coeff = ((regsOvly->CFG7 & CSL_VPS_HD_VENC_A_CFG7_SCALE0_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG7_SCALE0_SHIFT);
    aCfg->shift0Coeff = ((regsOvly->CFG7 & CSL_VPS_HD_VENC_A_CFG7_SHIFT0_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG7_SHIFT0_SHIFT);
    aCfg->blank0Level = ((regsOvly->CFG7 & CSL_VPS_HD_VENC_A_CFG7_BL_0_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG7_BL_0_SHIFT);

    aCfg->scale1Coeff = ((regsOvly->CFG8 & CSL_VPS_HD_VENC_A_CFG8_SCALE1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG8_SCALE1_SHIFT);
    aCfg->shift1Coeff = ((regsOvly->CFG8 & CSL_VPS_HD_VENC_A_CFG8_SHIFT1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG8_SHIFT1_SHIFT);
    aCfg->blank1Level = ((regsOvly->CFG8 & CSL_VPS_HD_VENC_A_CFG8_BL_1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG8_BL_1_SHIFT);

    aCfg->scale2Coeff = ((regsOvly->CFG9 & CSL_VPS_HD_VENC_A_CFG9_SCALE2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG9_SCALE2_SHIFT);
    aCfg->shift2Coeff = ((regsOvly->CFG9 & CSL_VPS_HD_VENC_A_CFG9_SHIFT2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG9_SHIFT2_SHIFT);
    aCfg->blank2Level = ((regsOvly->CFG9 & CSL_VPS_HD_VENC_A_CFG9_BL_2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG9_BL_2_SHIFT);

    aCfg->totalPixelsPerLine = ((regsOvly->CFG10 &
                                    CSL_VPS_HD_VENC_A_CFG10_PIXELS_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG10_PIXELS_SHIFT);
    aCfg->totalLinesPerFrame = ((regsOvly->CFG10 &
                                    CSL_VPS_HD_VENC_A_CFG10_LINES_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG10_LINES_SHIFT);
    aCfg->clampPeriod = ((regsOvly->CFG10 & CSL_VPS_HD_VENC_A_CFG10_CLAMP_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG10_CLAMP_SHIFT);

    aCfg->vbiLineEnd0 = ((regsOvly->CFG11 & CSL_VPS_HD_VENC_A_CFG11_V_BLA1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG11_V_BLA1_SHIFT);
    aCfg->vbiLineend1 = ((regsOvly->CFG11 & CSL_VPS_HD_VENC_A_CFG11_V_BLA2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG11_V_BLA2_SHIFT);
    aCfg->equPulseWidth = ((regsOvly->CFG11 &
                                CSL_VPS_HD_VENC_A_CFG11_EQ_WTH_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG11_EQ_WTH_SHIFT);

    aCfg->hbiPixelEnd = ((regsOvly->CFG12 &
                            CSL_VPS_HD_VENC_A_CFG12_H_BLANK_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG12_H_BLANK_SHIFT);
    aCfg->numActivePixels = ((regsOvly->CFG12 &
                                CSL_VPS_HD_VENC_A_CFG12_ACT_PIX_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG12_ACT_PIX_SHIFT);
    aCfg->hsWidth = ((regsOvly->CFG12 & CSL_VPS_HD_VENC_A_CFG12_HS_WTH_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG12_HS_WTH_SHIFT);

    pbPrShiftingHigh = ((regsOvly->CFG13 &
                            CSL_VPS_HD_VENC_A_CFG13_CBCR_S2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG13_CBCR_S2_SHIFT);
    aCfg->lastActiveLineFirstField = ((regsOvly->CFG13 &
                                    CSL_VPS_HD_VENC_A_CFG13_END_F1_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG13_END_F1_SHIFT);
    oCfg->HbiSignalStartPixel = ((regsOvly->CFG13 &
                                CSL_VPS_HD_VENC_A_CFG13_OSD_HBI_ST_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG13_OSD_HBI_ST_SHIFT);
    VencEnableDelayCountHigh = ((regsOvly->CFG13 &
                                CSL_VPS_HD_VENC_A_CFG13_DELAY_VENC_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG13_DELAY_VENC_SHIFT);

    pbPrShiftingLow = ((regsOvly->CFG14 & CSL_VPS_HD_VENC_A_CFG14_CBCR_S1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG14_CBCR_S1_SHIFT);
    aCfg->syncHighLevelAmp = ((regsOvly->CFG14 &
                                    CSL_VPS_HD_VENC_A_CFG14_SYNC_H_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG14_SYNC_H_SHIFT);
    aCfg->syncLowLevelAmp = ((regsOvly->CFG14 &
                                    CSL_VPS_HD_VENC_A_CFG14_SYNC_L_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG14_SYNC_L_SHIFT);

    aCfg->pbPrShifting = pbPrShiftingLow | (pbPrShiftingHigh << 8);

    dCfg->firstActivePixelLine = ((regsOvly->CFG15 &
                                  CSL_VPS_HD_VENC_A_CFG15_DVO_AVST_H_MASK)
                                  >> CSL_VPS_HD_VENC_A_CFG15_DVO_AVST_H_SHIFT);
    dCfg->activeVideoLineWidth = ((regsOvly->CFG15 &
                                  CSL_VPS_HD_VENC_A_CFG15_DVO_AVD_HW_MASK)
                                  >> CSL_VPS_HD_VENC_A_CFG15_DVO_AVD_HW_SHIFT);
    dCfg->hsWidth = ((regsOvly->CFG15 &
                        CSL_VPS_HD_VENC_A_CFG15_DVO_HS_WD_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG15_DVO_HS_WD_SHIFT);

    dCfg->hsStartPixel = ((regsOvly->CFG16 &
                            CSL_VPS_HD_VENC_A_CFG16_DVO_HS_ST_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG16_DVO_HS_ST_SHIFT);
    dCfg->activeLineStart0 = ((regsOvly->CFG16 &
                                CSL_VPS_HD_VENC_A_CFG16_DVO_AVST_V1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG16_DVO_AVST_V1_SHIFT);
    backPorchPeakValueLow =  ((regsOvly->CFG16 &
                                CSL_VPS_HD_VENC_A_CFG16_BP_PK_L_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG16_BP_PK_L_SHIFT);

    dCfg->activeLineStart1 = ((regsOvly->CFG17 &
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVST_V2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG17_DVO_AVST_V2_SHIFT);
    dCfg->activeVideoLines0 = ((regsOvly->CFG17 &
                                CSL_VPS_HD_VENC_A_CFG17_DVO_AVD_VW1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG17_DVO_AVD_VW1_SHIFT);
    backPorchPeakValueHigh = ((regsOvly->CFG17 &
                                CSL_VPS_HD_VENC_A_CFG17_BP_PK_H_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG17_BP_PK_H_SHIFT);

    mCfg->backPorchPeakValue = backPorchPeakValueLow |
                                    (backPorchPeakValueHigh) << 0x8;

    dCfg->activeVideoLines1 = ((regsOvly->CFG18 &
                                CSL_VPS_HD_VENC_A_CFG18_DVO_AVD_VW2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG18_DVO_AVD_VW2_SHIFT);
    dCfg->vsStartLine0 = ((regsOvly->CFG18 &
                            CSL_VPS_HD_VENC_A_CFG18_DVO_VS_ST1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG18_DVO_VS_ST1_SHIFT);
    dCfg->vsWidth0 = ((regsOvly->CFG18 &
                        CSL_VPS_HD_VENC_A_CFG18_DVO_VS_WD1_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG18_DVO_VS_WD1_SHIFT);

    dCfg->vsStartLine1 = ((regsOvly->CFG19 &
                            CSL_VPS_HD_VENC_A_CFG19_DVO_VS_ST2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG19_DVO_VS_ST2_SHIFT);
    dCfg->fidToggleLine0 = ((regsOvly->CFG19 &
                                CSL_VPS_HD_VENC_A_CFG19_DVO_FID_ST1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG19_DVO_FID_ST1_SHIFT);
    dCfg->vsWidth1  = ((regsOvly->CFG19 &
                            CSL_VPS_HD_VENC_A_CFG19_DVO_VS_WD2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG19_DVO_VS_WD2_SHIFT);

    dCfg->fidToggleLine1 = ((regsOvly->CFG20 &
                                CSL_VPS_HD_VENC_A_CFG20_DVO_FID_ST2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG20_DVO_FID_ST2_SHIFT);
    oCfg->fidToggleLine1 = ((regsOvly->CFG20 &
                                CSL_VPS_HD_VENC_A_CFG20_OSD_FID_ST2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG20_OSD_FID_ST2_SHIFT);
    VencEnableDelayCountLow = ((regsOvly->CFG20 &
                                CSL_VPS_HD_VENC_A_CFG20_DELAY_VENC_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG20_DELAY_VENC_SHIFT);

    aCfg->VencEnableDelayCount = VencEnableDelayCountLow |
                                    (VencEnableDelayCountHigh & 8);

    oCfg->lastNonActivePixelInLine = ((regsOvly->CFG21 &
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVST_H_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG21_OSD_AVST_H_SHIFT);
    oCfg->activeVideoLineWidth = ((regsOvly->CFG21 &
                                CSL_VPS_HD_VENC_A_CFG21_OSD_AVD_HW_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG21_OSD_AVD_HW_SHIFT);
    oCfg->hsWidth = ((regsOvly->CFG21 &
                        CSL_VPS_HD_VENC_A_CFG21_OSD_HS_WD_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG21_OSD_HS_WD_SHIFT);

    oCfg->hsStartPixel = ((regsOvly->CFG22 &
                            CSL_VPS_HD_VENC_A_CFG22_OSD_HS_ST_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG22_OSD_HS_ST_SHIFT);
    oCfg->activeLineStart0 = ((regsOvly->CFG22 &
                                CSL_VPS_HD_VENC_A_CFG22_OSD_AVST_V1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG22_OSD_AVST_V1_SHIFT);
    mCfg->mv100mvAgcPulseYSection = ((regsOvly->CFG22 &
                                      CSL_VPS_HD_VENC_A_CFG22_P_100MV_MASK)
                                      >> CSL_VPS_HD_VENC_A_CFG22_P_100MV_SHIFT);

    oCfg->activeLineStart1 = ((regsOvly->CFG23 &
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVST_V2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG23_OSD_AVST_V2_SHIFT);
    oCfg->activeVideoLines0 = ((regsOvly->CFG23 &
                                CSL_VPS_HD_VENC_A_CFG23_OSD_AVD_VW1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG23_OSD_AVD_VW1_SHIFT);
    mCfg->mv450mvAgcPulseXSection = ((regsOvly->CFG23 &
                                      CSL_VPS_HD_VENC_A_CFG23_P_450MV_MASK)
                                      >> CSL_VPS_HD_VENC_A_CFG23_P_450MV_SHIFT);

    oCfg->activeVideoLines1 = ((regsOvly->CFG24 &
                                CSL_VPS_HD_VENC_A_CFG24_OSD_AVD_VW2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG24_OSD_AVD_VW2_SHIFT);
    oCfg->vsStartLine0 = ((regsOvly->CFG24 &
                            CSL_VPS_HD_VENC_A_CFG24_OSD_VS_ST1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG24_OSD_VS_ST1_SHIFT);

    oCfg->vsWidth0 = ((regsOvly->CFG24 &
                        CSL_VPS_HD_VENC_A_CFG24_OSD_VS_WD1_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG24_OSD_VS_WD1_SHIFT);

    oCfg->vsStartLine1 = ((regsOvly->CFG25 &
                            CSL_VPS_HD_VENC_A_CFG25_OSD_VS_ST2_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG25_OSD_VS_ST2_SHIFT);
    oCfg->fidToggleLine0 = ((regsOvly->CFG25 &
                                CSL_VPS_HD_VENC_A_CFG25_OSD_FID_ST1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG25_OSD_FID_ST1_SHIFT);
    oCfg->vsWidth1 = ((regsOvly->CFG25 &
                        CSL_VPS_HD_VENC_A_CFG25_OSD_VS_WD2_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG25_OSD_VS_WD2_SHIFT);

    /* Set the parameters which cannot be read from registers */
    vencCfg->cscCoefficients = NULL;
    vencCfg->gammaCoeffs = NULL;
    vencCfg->cscMode = hdVencObj->vencCfg.cscMode;

    VpsHal_hdVencPrintConfig(vencCfg);
}

/**
 *  VpsHal_hdvencIoctl
 *  \brief This functions controls the VENC and other paired encoders. For now
 *         there are no VENC specific control operations that could be performed
 *
 *         If paired encoder (such as RF transmitter) for this VENC exists,
 *         the paired encoders control function would be called.
 *
 *  \param VpsHal_Handle    Handle to the venc
 *  \param cmd              Control command
 *  \param cmdArgs          Arguments associated with the command
 *  \param cmdStatusArgs    Result after execution of the command
 */
Int32 VpsHal_hdvencIoctl (  VpsHal_Handle   handle,
                            UInt32          cmd,
                            Ptr             cmdArgs,
                            Ptr             cmdStatusArgs)
{
    Int32       retVal      =   VPS_SOK;
    HdVenc_Obj  *hdVencObj  =   NULL;
    UInt32      encCnt;

    GT_assert(VpsHalTrace, (NULL != handle));

    hdVencObj = (HdVenc_Obj *)handle;

    switch (cmd)
    {
        case IOCTL_VPS_DCTRL_SET_VENC_CONTROL:
        {
            Vps_DcVencControl *ctrl = (Vps_DcVencControl *)cmdArgs;

            retVal = VpsHal_hdVencSetControl(hdVencObj, ctrl);

            break;
        }

        case IOCTL_VPS_DCTRL_GET_VENC_CONTROL:
        {
            Vps_DcVencControl *ctrl = (Vps_DcVencControl *)cmdArgs;

            retVal = VpsHal_hdVencGetControl(hdVencObj, ctrl);

            break;
        }

        default:
            for(encCnt = 0x0; encCnt < hdVencObj->noOfEncs; encCnt++)
            {
                if (hdVencObj->enc[encCnt].controlEnc != NULL)
                {
                    retVal = hdVencObj->enc[encCnt].controlEnc (
                                    hdVencObj->enc[encCnt].handle,
                                    cmd,
                                    cmdArgs,
                                    cmdStatusArgs);
                    if (VPS_SOK != retVal)
                    {
                        /* Let the apps know the reason */
                        break;
                    }
                }
            }
            break;
    }

    return (retVal);
}

static Int32 VpsHal_hdVencGetControl(HdVenc_Obj *hdVencObj, Vps_DcVencControl *ctrl)
{
    Int32 retVal = VPS_SOK;

    GT_assert(VpsHalTrace, (NULL != hdVencObj));
    GT_assert(VpsHalTrace, (NULL != ctrl));

    if (VPS_DC_CTRL_BRIGHTNESS == ctrl->control)
    {
        ctrl->level = hdVencObj->brightness + 129;
    }
    else if (VPS_DC_CTRL_CONTRAST == ctrl->control)
    {
        ctrl->level = hdVencObj->contrast;
    }
    else if (VPS_DC_CTRL_SATURATION == ctrl->control)
    {
        ctrl->level = hdVencObj->saturation;
    }
    else if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        ctrl->level = hdVencObj->hue;
    }
    else
    {
        retVal = VPS_EINVALID_PARAMS;
    }

    return (retVal);
}

/**
 *  VpsHal_hdVencSetControl
 *  \brief Single function used to set controls like brightness, contrast
 *         using CSC of the VENC.
 *
 *  \param handle          Requested HDVENC instance
 */
static Int32 VpsHal_hdVencSetControl(HdVenc_Obj *hdVencObj, Vps_DcVencControl *ctrl)
{
    Int32                   retVal = VPS_SOK;
    Int32                   d;
    UInt32                  cnt1, cnt2;
    UInt32                  mode;
    HdVenc_OutputCfg       *outputCfg;
    CSL_VpsHdVencdRegsOvly  regsOvly;
    Int32                   temp, tempHue, tempMul;

    GT_assert(VpsHalTrace, (NULL != hdVencObj));
    GT_assert(VpsHalTrace, (NULL != ctrl));

    outputCfg = &hdVencObj->outputCfg;
    regsOvly = hdVencObj->regsOvly;
    GT_assert(VpsHalTrace, (NULL != regsOvly));

    if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        if ((ctrl->level < 1) || (ctrl->level > 61))
        {
            GT_0trace(VpsHalTrace, GT_ERR, "level out of range!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }
    else
    {
        if ((ctrl->level < 1) || (ctrl->level > 256))
        {
            GT_0trace(VpsHalTrace, GT_ERR, "level out of range!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }
    if (VPS_DC_CTRL_BRIGHTNESS == ctrl->control)
    {
        hdVencObj->brightness = ctrl->level - 129;
    }
    else if (VPS_DC_CTRL_CONTRAST == ctrl->control)
    {
        hdVencObj->contrast = ctrl->level;
    }
    else if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        hdVencObj->hue = ctrl->level;
    }
    else
    {
        hdVencObj->saturation = ctrl->level;
    }

    mode = outputCfg->cscMode;
    if (VPS_SOK == retVal)
    {
        switch (outputCfg->cscMode)
        {
            case VPSHAL_HDVENC_CSCMODE_R2R:
            {
                Int32 vsu, vsw;

                VpsUtils_memset(&outputCfg->cscCoeff,
                                0u,
                                sizeof(outputCfg->cscCoeff));

                tempHue = hdVencObj->hue - 31;

                tempMul = 1;
                if (tempHue < 0)
                {
                    tempMul = (-1);
                    tempHue = tempHue * tempMul;
                }

                vsu = VpsHalHdVencCosineTable[tempHue] *
                        (hdVencObj->saturation / 128);
                vsw = tempMul * VpsHalHdVencSineTable[tempHue] *
                        (hdVencObj->saturation / 128);
                for (cnt1 = 0u; cnt1 < VPSHAL_HDVENC_CSC_NUM_COEFF;  cnt1 ++)
                {
                    for (cnt2 = 0u; cnt2 < VPSHAL_HDVENC_CSC_NUM_COEFF; cnt2 ++)
                    {
                        temp = vpsHalHsvCoeff.m[cnt1][cnt2][0] +
                                    (vpsHalHsvCoeff.m[cnt1][cnt2][1] * vsu) / 10000 +
                                    (vpsHalHsvCoeff.m[cnt1][cnt2][1] * vsw) / 10000;

                        outputCfg->cscCoeff.mulCoeff[cnt1][cnt2] = (UInt16)temp;
                    }

                    outputCfg->cscCoeff.mulCoeff[cnt1][cnt1] =
                        (outputCfg->cscCoeff.mulCoeff[cnt1][cnt1] *
                            hdVencObj->contrast) / 128;

                    outputCfg->cscCoeff.addCoeff[cnt1] =
                        (UInt16)hdVencObj->brightness;
                }

                break;
            }

            case VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_R2Y:
            case VPSHAL_HDVENC_CSCMODE_SDTV_GRAPHICS_R2Y:
            case VPSHAL_HDVENC_CSCMODE_HDTV_VIDEO_R2Y:
            case VPSHAL_HDVENC_CSCMODE_SDTV_VIDEO_R2Y:
            {
                tempHue = hdVencObj->hue - 31;

                tempMul = 1;
                if (tempHue < 0)
                {
                    tempMul = (-1);
                    tempHue = tempHue * tempMul;
                }
                for (cnt1 = 0u; cnt1 < VPSHAL_HDVENC_CSC_NUM_COEFF;  cnt1 ++)
                {
                    for (cnt2 = 0u; cnt2 < VPSHAL_HDVENC_CSC_NUM_COEFF; cnt2 ++)
                    {
                        temp = CscQFormCoeff[mode].m[cnt1][cnt2] * hdVencObj->contrast / 128;

                        if (cnt1 > 0)
                        {
                            temp = temp * hdVencObj->saturation / 128;
                        }

                        if (1u == cnt1)
                        {
                            temp = (temp * VpsHalHdVencCosineTable[tempHue] +
                                        temp * VpsHalHdVencSineTable[tempHue]) *
                                            tempMul;
                            temp /= 10000;
                        }
                        if (2u == cnt1)
                        {
                            temp = (temp * VpsHalHdVencCosineTable[tempHue] -
                                        temp * VpsHalHdVencSineTable[tempHue]) *
                                            tempMul;
                            temp /= 10000;
                        }

                        outputCfg->cscCoeff.mulCoeff[cnt1][cnt2] = (UInt16) temp;
                    }
                }

                if ((VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_R2Y == mode) ||
                    (VPSHAL_HDVENC_CSCMODE_SDTV_GRAPHICS_R2Y == mode))
                {
                    d = 64 + hdVencObj->brightness;

                    outputCfg->cscCoeff.addCoeff[0] = (UInt16) d;
                    outputCfg->cscCoeff.addCoeff[1] = 512u;
                    outputCfg->cscCoeff.addCoeff[2] = 512u;
                }
                else
                {
                    d = 64 - ((hdVencObj->contrast*64)/10);

                    d = d + hdVencObj->brightness;

                    outputCfg->cscCoeff.addCoeff[0] = (UInt16) d;
                    outputCfg->cscCoeff.addCoeff[1] = 512u;
                    outputCfg->cscCoeff.addCoeff[2] = 512u;
                }
                break;
            }
        }

        hdVencProgramCscCoeff(hdVencObj, outputCfg);
    }

    return (retVal);
}

/** \brief Program the gamma correction coefficients */
static Void hdVencProgramGamCoeff(const HdVenc_Obj *hdVencObj,
                           const VpsHal_HdVencConfig *cfg)
{
    int coeffCount;
    UInt32 coeffLow, coeffMid, coeffHigh;
    UInt16 *coeffPtrCh0, *coeffPtrCh1, *coeffPtrCh2;

    coeffPtrCh0 = &cfg->gammaCoeffs->coeffs[0][0];
    coeffPtrCh1 = &cfg->gammaCoeffs->coeffs[1][0];
    coeffPtrCh2 = &cfg->gammaCoeffs->coeffs[2][0];
    for (coeffCount = 0; coeffCount < 1024; coeffCount++)
    {
       coeffLow =  ((coeffPtrCh0[coeffCount] <<
                        CSL_VPS_HD_VENC_A_GAMMA_LOW_COEFF_SHIFT) &
                            CSL_VPS_HD_VENC_A_GAMMA_LOW_COEFF_MASK);
       coeffMid =  ((coeffPtrCh1[coeffCount] <<
                        CSL_VPS_HD_VENC_A_GAMMA_MID_COEFF_SHIFT) &
                            CSL_VPS_HD_VENC_A_GAMMA_MID_COEFF_MASK);
       coeffHigh = ((coeffPtrCh2[coeffCount] <<
                        CSL_VPS_HD_VENC_A_GAMMA_HIGH_COEFF_SHIFT) &
                            CSL_VPS_HD_VENC_A_GAMMA_HIGH_COEFF_MASK);
        hdVencObj->gRegBase[coeffCount] = coeffLow | coeffMid | coeffHigh;
    }
}
/** \brief Program shift and scaling coefficients */
static Void hdVencProgramShiftScale(const HdVenc_Obj *hdVencObj,
                             const VpsHal_HdVencAnalogConfig *aCfg)
{
    CSL_VpsHdVencdRegsOvly   regsOvly = hdVencObj->regsOvly;

    regsOvly->CFG7 = hdVencMakeShiftReg(aCfg->scale0Coeff, aCfg->shift0Coeff,
                        aCfg->blank0Level);
    regsOvly->CFG8 = hdVencMakeShiftReg(aCfg->scale1Coeff, aCfg->shift1Coeff,
                        aCfg->blank1Level);
    regsOvly->CFG9 = hdVencMakeShiftReg(aCfg->scale2Coeff, aCfg->shift2Coeff,
                        aCfg->blank2Level);
}

static Void hdVencProgramCscCoeff(const HdVenc_Obj *hdVencObj,
                                  const HdVenc_OutputCfg *cfg)
{
    CSL_VpsHdVencdRegsOvly   regsOvly = hdVencObj->regsOvly;
    const VpsHal_HdVencCscCoeff   *coeff = NULL;

    coeff = &cfg->cscCoeff;
    regsOvly->CFG1 = hdVencMakeCscReg(coeff->mulCoeff[0][0],
                                         coeff->mulCoeff[0][1]);
    regsOvly->CFG2 = hdVencMakeCscReg(coeff->mulCoeff[0][2],
                                         coeff->mulCoeff[1][0]);
    regsOvly->CFG3 = hdVencMakeCscReg(coeff->mulCoeff[1][1],
                                         coeff->mulCoeff[1][2]);
    regsOvly->CFG4 = hdVencMakeCscReg(coeff->mulCoeff[2][0],
                                         coeff->mulCoeff[2][1]);
    regsOvly->CFG5 = ((coeff->mulCoeff[2][2] << CSL_VPS_HD_VENC_A_CFG5_C2_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG5_C2_MASK) |
                     ((coeff->addCoeff[0] << CSL_VPS_HD_VENC_A_CFG5_D0_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG5_D0_MASK);

    regsOvly->CFG6 = ((coeff->addCoeff[1] << CSL_VPS_HD_VENC_A_CFG6_D1_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG6_D1_MASK) |
                     ((coeff->addCoeff[2] << CSL_VPS_HD_VENC_A_CFG6_D2_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG6_D2_MASK);

}
static UInt32 hdVencMakeCscReg(UInt16 coeff1, UInt16 coeff2)
{
    UInt32 tempReg;

    /* Since all coefficients masks are same, onle masks for A0 and
     * B0 are used */
    tempReg = (coeff1 << CSL_VPS_HD_VENC_A_CFG1_A0_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG1_A0_MASK;
    tempReg |= (coeff2 << CSL_VPS_HD_VENC_A_CFG1_B0_SHIFT) &
                        CSL_VPS_HD_VENC_A_CFG1_B0_MASK;
    return (tempReg);
}

static UInt32 hdVencMakeShiftReg(UInt16 scale, UInt16 shift, UInt16 blLevel)
{
    UInt32 temp = 0;

    temp = (UInt32) (scale | (shift << CSL_VPS_HD_VENC_A_CFG7_SHIFT0_SHIFT) |
            blLevel << CSL_VPS_HD_VENC_A_CFG8_BL_1_SHIFT);
    return (temp);
}

static Void hdVmakeCfg0Reg(CSL_VpsHdVencdRegsOvly  regsOvly,
                           const VpsHal_HdVencConfig *cfg)
{
    //const VpsHal_HdVencAnalogConfig *aCfg = &cfg->analogConfig;
    const VpsHal_HdVencDvoConfig *dCfg = &cfg->dvoConfig;
    //const VpsHal_HdVencOsdConfig *oCfg = &cfg->osdConfig;
    const VpsHal_HdVencMvConfig *mCfg  = &cfg->mvConfig;

     regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_DM_SEL_MASK;
     regsOvly->CFG0 |= ((cfg->dispMode <<
             CSL_VPS_HD_VENC_A_CFG0_DM_SEL_SHIFT) &
                 CSL_VPS_HD_VENC_A_CFG0_DM_SEL_MASK);

    if (FVID2_SF_PROGRESSIVE == cfg->scanFormat)
    {
        regsOvly->CFG0 &= (~CSL_VPS_HD_VENC_A_CFG0_I_PN_MASK);
    }
    else
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_I_PN_MASK;
    }
    if (TRUE == cfg->bypass2XUpSampling)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_BYPS_2X_MASK;
    }
    else
    {
        regsOvly->CFG0 &= (~CSL_VPS_HD_VENC_A_CFG0_BYPS_2X_MASK);
    }
    if (TRUE == cfg->bypassGammaCor)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_BYPS_GC_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_BYPS_GC_MASK;
    }
    if (TRUE == cfg->rdGammaCorMem)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_RD_MEM_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~ CSL_VPS_HD_VENC_A_CFG0_RD_MEM_MASK;
    }
    if (TRUE == cfg->selfTest)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_STEST_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_STEST_MASK;
    }
    if ( VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE == dCfg->vsFidLineStart)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_HF_LINE_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_HF_LINE_MASK;
    }
    if (TRUE == dCfg->jeidaFmt)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_JED_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_JED_MASK;
    }
    /* Encoder is stopped while configuring */
    regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_START_MASK;
    if (TRUE == mCfg->macroVisionEnable)
    {
        regsOvly->CFG0 |= CSL_VPS_HD_VENC_A_CFG0_CMV_MASK;
    }
    else
    {
        regsOvly->CFG0 &= ~CSL_VPS_HD_VENC_A_CFG0_CMV_MASK;
    }
}

static Void hdVgetCfg0Reg(UInt32 regValue, VpsHal_HdVencConfig *cfg)
{
    VpsHal_HdVencAnalogConfig *aCfg = &cfg->analogConfig;
    VpsHal_HdVencDvoConfig *dCfg = &cfg->dvoConfig;
    VpsHal_HdVencOsdConfig *oCfg = &cfg->osdConfig;
    VpsHal_HdVencMvConfig *mCfg  = &cfg->mvConfig;

    switch ((regValue & CSL_VPS_HD_VENC_A_CFG0_DM_SEL_MASK) >>
        CSL_VPS_HD_VENC_A_CFG0_DM_SEL_SHIFT)
    {
        case VPSHAL_HDVENC_DMODE_480I :
            cfg->dispMode = VPSHAL_HDVENC_DMODE_480I;
        break;
        case VPSHAL_HDVENC_DMODE_480P:
            cfg->dispMode = VPSHAL_HDVENC_DMODE_480P;
        break;
        case VPSHAL_HDVENC_DMODE_1080I:
            cfg->dispMode = VPSHAL_HDVENC_DMODE_1080I;
        break;
        case VPSHAL_HDVENC_DMODE_720P:
            cfg->dispMode = VPSHAL_HDVENC_DMODE_720P;
        break;
        case VPSHAL_HDVENC_DMODE_576I:
            cfg->dispMode = VPSHAL_HDVENC_DMODE_576I;
        break;
        case VPSHAL_HDVENC_DMODE_576P:
            cfg->dispMode = VPSHAL_HDVENC_DMODE_576P;
        break;

    }
    if (0 ==
        ((regValue & CSL_VPS_HD_VENC_A_CFG0_I_PN_MASK)
            >> CSL_VPS_HD_VENC_A_CFG0_I_PN_SHIFT))
    {
        cfg->scanFormat = FVID2_SF_PROGRESSIVE;
    }
    else
    {
        cfg->scanFormat = FVID2_SF_INTERLACED;
    }
    cfg->bypass2XUpSampling = ((regValue & CSL_VPS_HD_VENC_A_CFG0_BYPS_2X_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_BYPS_2X_SHIFT);

    cfg->bypassCsc = ((regValue & CSL_VPS_HD_VENC_A_CFG0_BYPS_CS_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_BYPS_CS_SHIFT);

    aCfg->outColorSpace = (VpsHal_HdVencOutputColorSpace)
                            ((regValue & CSL_VPS_HD_VENC_A_CFG0_Y_RGBN_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_Y_RGBN_SHIFT);

    aCfg->dac0PwDn = ((regValue & CSL_VPS_HD_VENC_A_CFG0_PWD_0_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_PWD_0_SHIFT);
    aCfg->dac1PwDn = ((regValue & CSL_VPS_HD_VENC_A_CFG0_PWD_1_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_PWD_1_SHIFT);
    aCfg->dac2PwDn = ((regValue & CSL_VPS_HD_VENC_A_CFG0_PWD_2_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_PWD_2_SHIFT);

    aCfg->dacInvertData = ((regValue & CSL_VPS_HD_VENC_A_CFG0_INVT_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_INVT_SHIFT);

    switch ((regValue & CSL_VPS_HD_VENC_A_CFG0_DVO_CS_MASK) >>
            CSL_VPS_HD_VENC_A_CFG0_DVO_CS_SHIFT)
    {
        case VPSHAL_HDVENC_OUTPUTSRC_DATAMGR:
            dCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_DATAMGR;
        break;
        case VPSHAL_HDVENC_OUTPUTSRC_GAMMACRT:
            dCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_GAMMACRT;
        break;
        case VPSHAL_HDVENC_OUTPUTSRC_COLORSPC:
            dCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_COLORSPC;
        break;
    }

    cfg->bypassGammaCor = ((regValue & CSL_VPS_HD_VENC_A_CFG0_BYPS_GC_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_BYPS_GC_SHIFT);


    cfg->rdGammaCorMem = ((regValue & CSL_VPS_HD_VENC_A_CFG0_RD_MEM_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_RD_MEM_SHIFT);

    cfg->selfTest = ((regValue & CSL_VPS_HD_VENC_A_CFG0_STEST_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_STEST_SHIFT);

    switch ((regValue & CSL_VPS_HD_VENC_A_CFG0_DVO_FMT_MASK)
                >> CSL_VPS_HD_VENC_A_CFG0_DVO_FMT_SHIFT)
    {
        case VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN :
            dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN;
        break;
        case VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN :
            dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN;
        break;
        case VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC :
            dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC;
        break;
        case VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC :
            dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC;
        break;
        case VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC :
            dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC;
        break;
    }
    aCfg->dac0RfPdn = ((regValue & CSL_VPS_HD_VENC_A_CFG0_DAC_RF0_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_DAC_RF0_SHIFT);

    aCfg->dac1RfPdn = ((regValue & CSL_VPS_HD_VENC_A_CFG0_DAC_RF1_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_DAC_RF1_SHIFT);

    oCfg->invtfidPol = ((regValue & CSL_VPS_HD_VENC_A_CFG0_IVT_FID_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_IVT_FID_SHIFT);

    dCfg->invtfidPol = ((regValue & CSL_VPS_HD_VENC_A_CFG0_I_DVO_F_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_I_DVO_F_SHIFT);

    dCfg->invtVsPol = ((regValue & CSL_VPS_HD_VENC_A_CFG0_I_DVO_V_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_I_DVO_V_SHIFT);

    dCfg->invtHsPol = ((regValue & CSL_VPS_HD_VENC_A_CFG0_I_DVO_H_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_I_DVO_H_SHIFT);

    dCfg->invtActsPol = ((regValue & CSL_VPS_HD_VENC_A_CFG0_I_DVO_A_MASK)
                            >> CSL_VPS_HD_VENC_A_CFG0_I_DVO_A_SHIFT);

    dCfg->decimationMethod = (VpsHal_HdVenc444to422)
                                ((regValue & CSL_VPS_HD_VENC_A_CFG0_S_422_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_S_422_SHIFT);

    dCfg->dvoOff = ((regValue & CSL_VPS_HD_VENC_A_CFG0_DVO_OFF_MASK)
                                >> CSL_VPS_HD_VENC_A_CFG0_DVO_OFF_SHIFT);

    if (VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE ==
        (regValue & CSL_VPS_HD_VENC_A_CFG0_HF_LINE_MASK) >>
            CSL_VPS_HD_VENC_A_CFG0_HF_LINE_SHIFT)
    {
        dCfg->vsFidLineStart = VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE;
    }
    else
    {
        dCfg->vsFidLineStart = VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING;
    }
    dCfg->jeidaFmt = ((regValue & CSL_VPS_HD_VENC_A_CFG0_JED_MASK)
                        >> CSL_VPS_HD_VENC_A_CFG0_JED_SHIFT);

    mCfg->macroVisionEnable = ((regValue & CSL_VPS_HD_VENC_A_CFG0_CMV_MASK)
                                    >> CSL_VPS_HD_VENC_A_CFG0_CMV_SHIFT);
}


static Void hdVencSetPolarity(Vps_DcOutputInfo *output,
                              HdVenc_OutputCfg *outputCfg)
{;
    GT_assert(VpsHalTrace, (NULL != outputCfg));
    GT_assert(VpsHalTrace, (NULL != output));

    if (VPS_DC_POLARITY_ACT_HIGH == output->dvoFidPolarity)
    {
        outputCfg->dvoCfg.invtfidPol = FALSE;
    }
    else
    {
        outputCfg->dvoCfg.invtfidPol = TRUE;
    }
    if (VPS_DC_POLARITY_ACT_HIGH == output->dvoVsPolarity)
    {
        outputCfg->dvoCfg.invtVsPol = FALSE;
    }
    else
    {
        outputCfg->dvoCfg.invtVsPol = TRUE;
    }
    if (VPS_DC_POLARITY_ACT_HIGH == output->dvoHsPolarity)
    {
        outputCfg->dvoCfg.invtHsPol = FALSE;
    }
    else
    {
        outputCfg->dvoCfg.invtHsPol = TRUE;
    }
    if (VPS_DC_POLARITY_ACT_HIGH == output->dvoActVidPolarity)
    {
        outputCfg->dvoCfg.invtActsPol = FALSE;
    }
    else
    {
        outputCfg->dvoCfg.invtActsPol = TRUE;
    }
}


/** Function to the configuration related to output settings in hdvenc */
static Int32 vpsHalHdVencSetOutputCfg(HdVenc_Obj *hdVencObj)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      cfg0Reg;
    CSL_VpsHdVencdRegsOvly      regsOvly = NULL;
    HdVenc_OutputCfg           *outputCfg;

    regsOvly = hdVencObj->regsOvly;
    outputCfg = &hdVencObj->outputCfg;

    GT_assert(VpsHalTrace, (NULL != regsOvly));

    cfg0Reg = regsOvly->CFG0;

    cfg0Reg &= ~CSL_VPS_HD_VENC_A_CFG0_BYPS_CS_MASK;
    if (VPSHAL_HDVENC_OUTPUTSRC_COLORSPC != outputCfg->dataSource)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_BYPS_CS_MASK;
    }
#if defined(TI_8107_BUILD) && defined(VPSHAL_VENC_SWAP_COLORS_FOR_VGA_OUT_ON_TI813X)
    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        cfg0Reg &= ~CSL_VPS_HD_VENC_A_CFG0_BYPS_CS_MASK;
    }
#endif

    cfg0Reg &= ~CSL_VPS_HD_VENC_A_CFG0_Y_RGBN_MASK;
    if (VPSHAL_HDVENC_COLORSPACE_YUV == outputCfg->outColorSpace)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_Y_RGBN_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_DVO_FMT_MASK);
    cfg0Reg |= ((outputCfg->fmt <<
                    CSL_VPS_HD_VENC_A_CFG0_DVO_FMT_SHIFT) &
                    CSL_VPS_HD_VENC_A_CFG0_DVO_FMT_MASK);

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_DVO_CS_MASK);

    cfg0Reg |= (outputCfg->dataSource <<
                    CSL_VPS_HD_VENC_A_CFG0_DVO_CS_SHIFT) &
                    CSL_VPS_HD_VENC_A_CFG0_DVO_CS_MASK;

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_PWD_0_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_PWD_1_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_PWD_2_MASK);
    if (TRUE == outputCfg->analogCfg.dac0PwDn)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_PWD_0_MASK;
    }
    if (TRUE == outputCfg->analogCfg.dac1PwDn)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_PWD_1_MASK;
    }
    if (TRUE == outputCfg->analogCfg.dac2PwDn)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_PWD_2_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_INVT_MASK);
    if (TRUE == outputCfg->analogCfg.dacInvertData)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_INVT_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_DAC_RF0_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_DAC_RF1_MASK);
    if (TRUE == outputCfg->analogCfg.dac0RfPdn)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_DAC_RF0_MASK;
    }
    if (TRUE == outputCfg->analogCfg.dac1RfPdn)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_DAC_RF1_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_IVT_FID_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_I_DVO_F_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_I_DVO_V_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_I_DVO_H_MASK |
                 CSL_VPS_HD_VENC_A_CFG0_I_DVO_A_MASK);
    if (TRUE == outputCfg->dvoCfg.invtfidPol)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_IVT_FID_MASK;
    }
    if (TRUE == outputCfg->dvoCfg.invtfidPol)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_I_DVO_F_MASK;
    }
    if (TRUE == outputCfg->dvoCfg.invtVsPol)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_I_DVO_V_MASK;
    }
    if (TRUE == outputCfg->dvoCfg.invtHsPol)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_I_DVO_H_MASK;
    }
    if (TRUE == outputCfg->dvoCfg.invtActsPol)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_I_DVO_A_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_S_422_MASK);
    if (VPSHAL_HDVENC_444TO422_ALTERNATEPIXEL == outputCfg->dvoCfg.decimationMethod)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_S_422_MASK;
    }

    cfg0Reg &= ~(CSL_VPS_HD_VENC_A_CFG0_DVO_OFF_MASK);
    if (TRUE == outputCfg->dvoCfg.dvoOff)
    {
        cfg0Reg |= CSL_VPS_HD_VENC_A_CFG0_DVO_OFF_MASK;
    }

    regsOvly->CFG0 = cfg0Reg;

    return (retVal);
}



/**
 *  VpsHal_hdvencStartVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencStartVenc(VpsHal_Handle handle)
{
    HdVenc_Obj              *hdVencObj = NULL;
    CSL_VpsHdVencdRegsOvly   regsOvly = NULL;
    Int32                   retVal = VPS_SOK;
    UInt32                  encCnt;

   GT_assert(VpsHalTrace, (NULL != handle));

    hdVencObj = (HdVenc_Obj *)handle;
    regsOvly = hdVencObj->regsOvly;

    regsOvly->CFG0 |= CSL_VPS_HD_VENC_D_CFG0_START_MASK;

    for(encCnt = 0x0; encCnt < hdVencObj->noOfEncs; encCnt++)
    {
        if (hdVencObj->enc[encCnt].startEnc != NULL)
        {
            retVal = hdVencObj->enc[encCnt].startEnc (
                            hdVencObj->enc[encCnt].handle,
                            NULL);
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }
    return (retVal);
}



/**
 *  VpsHal_hdvencStopVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencStopVenc(VpsHal_Handle handle)
{
    HdVenc_Obj              *hdVencObj = NULL;
    CSL_VpsHdVencdRegsOvly   regsOvly = NULL;
    Int32                   retVal = VPS_SOK;
    UInt32                  encCnt;

    GT_assert(VpsHalTrace, (NULL != handle));

    hdVencObj = (HdVenc_Obj *)handle;
    regsOvly = hdVencObj->regsOvly;

    regsOvly->CFG0 &= (~CSL_VPS_HD_VENC_D_CFG0_START_MASK);

    for(encCnt = 0x0; encCnt < hdVencObj->noOfEncs; encCnt++)
    {
        if (hdVencObj->enc[encCnt].stopEnc != NULL)
        {
            retVal = hdVencObj->enc[encCnt].stopEnc (
                            hdVencObj->enc[encCnt].handle,
                            NULL);
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }
    return (retVal);
}


/**
 *  VpsHal_hdvencResetVenc
 *  \brief This functions to reset the mode in the HdVenc.
 *
 *  \param handle         Requested HDVENC instance
 */
Int VpsHal_hdvencResetVenc(VpsHal_Handle handle)
{
    CSL_VpsHdVencdRegsOvly   regsOvly = NULL;
    Int32                    retVal = VPS_SOK;
    volatile UInt32          delay = 1000;

    GT_assert(VpsHalTrace, (NULL != handle));

    regsOvly = ((HdVenc_Obj *)handle)->regsOvly;

    regsOvly->CFG10 = 0;

    return (retVal);
}



static Void hdVencSetDefaults(HdVenc_Obj *hdVencObj)
{
    VpsHal_HdVencOsdConfig      *oCfg;
    VpsHal_HdVencAnalogConfig   *aCfg;
    VpsHal_HdVencDvoConfig      *dCfg;

    aCfg = &hdVencObj->vencCfg.analogConfig;
    oCfg = &hdVencObj->vencCfg.osdConfig;
    dCfg = &hdVencObj->vencCfg.dvoConfig;

    aCfg->outColorSpace = VPSHAL_HDVENC_COLORSPACE_YUV;
    aCfg->dac0PwDn = FALSE;
    aCfg->dac1PwDn = FALSE;
    aCfg->dac2PwDn = FALSE;
    aCfg->dacInvertData = FALSE;
    aCfg->dac0RfPdn = FALSE;
    aCfg->dac1RfPdn = FALSE;
    aCfg->scale0Coeff = 0xc77;
    aCfg->shift0Coeff = 0xc0;
    aCfg->blank0Level = 0x184;
    aCfg->scale1Coeff = 0xc30;
    aCfg->shift1Coeff = 0xc0;
    aCfg->blank1Level = 0x1c0;
    aCfg->scale2Coeff = 0xc30;
    aCfg->shift2Coeff = 0xc0;
    aCfg->blank2Level = 0x1c0;
    aCfg->totalPixelsPerLine = 0;
    aCfg->totalLinesPerFrame = 0;
    aCfg->clampPeriod = 0x84;
    aCfg->vbiLineEnd0 = 0;
    aCfg->vbiLineend1 = 0x245;
    aCfg->equPulseWidth = 0x3F;
    aCfg->hbiPixelEnd = 0;
    aCfg->numActivePixels = 0;
    aCfg->hsWidth = 0x3F;
    aCfg->lastActiveLineFirstField = 0;
    aCfg->pbPrShifting = 0;
    aCfg->VencEnableDelayCount = 0;
    aCfg->syncHighLevelAmp = 0x338;
    aCfg->syncLowLevelAmp = 0x038;

    dCfg->dataSource = VPSHAL_HDVENC_OUTPUTSRC_COLORSPC;
    dCfg->fmt = VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC;
    dCfg->invtfidPol = FALSE;
    dCfg->invtVsPol = FALSE;
    dCfg->invtHsPol = FALSE;
    dCfg->invtActsPol = FALSE;
    dCfg->decimationMethod = VPSHAL_HDVENC_444TO422_DECIMATION;
    dCfg->dvoOff = FALSE;
    dCfg->vsFidLineStart = VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING;
    dCfg->jeidaFmt = FALSE;
    dCfg->firstActivePixelLine = 0;
    dCfg->activeVideoLineWidth = 0;
    dCfg->hsWidth = 4;
    dCfg->hsStartPixel = 1;
    dCfg->activeLineStart0 = 1;
    dCfg->activeLineStart1 = 0;
    dCfg->activeVideoLines0 = 0;
    dCfg->activeVideoLines1 = 0;
    dCfg->vsStartLine0 = 1;
    dCfg->vsStartLine1 = 1;
    dCfg->vsWidth0 = 1;
    dCfg->vsWidth1 = 1;
    dCfg->fidToggleLine0 = 1;
    dCfg->fidToggleLine1 = 0;

    oCfg->invtfidPol = FALSE;
    oCfg->HbiSignalStartPixel = 0;
    oCfg->lastNonActivePixelInLine = 0;
    oCfg->activeVideoLineWidth = 0;
    oCfg->hsWidth = 4;
    oCfg->hsStartPixel = 1;
    oCfg->activeLineStart0 = 1;
    oCfg->activeLineStart1 = 1;
    oCfg->activeVideoLines0 = 0;
    oCfg->activeVideoLines1 = 0;
    oCfg->vsStartLine0 = 1;
    oCfg->vsStartLine1 = 1;
    oCfg->vsWidth0 = 1;
    oCfg->vsWidth1 = 1;
    oCfg->fidToggleLine0 = 0;
    oCfg->fidToggleLine1 = 0;
}



static Void hdVencSetModeTiming(HdVenc_Obj *hdVencObj,
                                FVID2_ModeInfo *tInfo)
{
    VpsHal_HdVencOsdConfig      *oCfg;
    VpsHal_HdVencAnalogConfig   *aCfg;
    VpsHal_HdVencDvoConfig      *dCfg;

    aCfg = &hdVencObj->vencCfg.analogConfig;
    oCfg = &hdVencObj->vencCfg.osdConfig;
    dCfg = &hdVencObj->vencCfg.dvoConfig;

    aCfg->totalPixelsPerLine = tInfo->hFrontPorch +
                               tInfo->hBackPorch +
                               tInfo->hSyncLen +
                               tInfo->width;
    aCfg->clampPeriod = 132;
    aCfg->equPulseWidth = tInfo->hSyncLen;
    aCfg->vbiLineEnd0 = tInfo->vFrontPorch +
                        tInfo->vBackPorch +
                        tInfo->vSyncLen - 1u;
    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        aCfg->vbiLineEnd0 -= 1;
    #endif

    aCfg->vbiLineend1 = (2 * (tInfo->vFrontPorch +
                        tInfo->vBackPorch +
                        tInfo->vSyncLen)) +
                        (tInfo->height / 2);
    aCfg->numActivePixels = tInfo->width + 8;
    aCfg->hsWidth = tInfo->hSyncLen;
    aCfg->hbiPixelEnd = tInfo->hSyncLen + tInfo->hBackPorch -
                            HDVENC_HOFFSET - 3u;

    if ((VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC ==
            hdVencObj->outputCfg.fmt) ||
        (VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN_DISCSYNC ==
            hdVencObj->outputCfg.fmt))
    {
        dCfg->firstActivePixelLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 0u;
        if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
        {
            oCfg->lastNonActivePixelInLine = tInfo->hSyncLen +
                                             tInfo->hBackPorch -
                                             HDVENC_HOFFSET - 5u;
        }
        else
        {
            oCfg->lastNonActivePixelInLine = tInfo->hSyncLen +
                                             tInfo->hBackPorch -
                                             HDVENC_HOFFSET - 8u;
        }
    }
    else
    {
        dCfg->firstActivePixelLine = tInfo->hSyncLen +
                                     tInfo->hFrontPorch +
                                     tInfo->hBackPorch;
        if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
        {
            oCfg->lastNonActivePixelInLine = tInfo->hSyncLen +
                                             tInfo->hBackPorch -
                                             HDVENC_HOFFSET - 5u;
        }
        else
        {
            oCfg->lastNonActivePixelInLine = dCfg->firstActivePixelLine - 8u;
        }
    }

#if 0
    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        dCfg->firstActivePixelLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 0u;
        oCfg->lastNonActivePixelInLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 5u;
    }
    else
    {
        if ((VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN ==
                    hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN ==
                    hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC ==
                    hdVencObj->outputCfg.fmt))
        {
            dCfg->firstActivePixelLine = aCfg->totalPixelsPerLine -
                                            tInfo->width - HDVENC_HOFFSET - 0u;
            oCfg->lastNonActivePixelInLine = dCfg->firstActivePixelLine - 8u;
        }
        else
        {
            dCfg->firstActivePixelLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                                HDVENC_HOFFSET - 0u;
            oCfg->lastNonActivePixelInLine = tInfo->hSyncLen +
                                                tInfo->hBackPorch -
                                                HDVENC_HOFFSET - 8u;
        }
    }
#endif

    /* Whenever gamma correction is not bypassed, the DVO output will be
       delayed by one pixel resulting in a wrong pixel at left and clipping of
       a pixel at right. To correct this problem, We need to make an
       adjustment to OSD_AVST_H */
    if (FALSE == hdVencObj->vencCfg.bypassGammaCor)
    {
        oCfg->lastNonActivePixelInLine -= 1;
    }

    dCfg->hsStartPixel = 0;
    dCfg->activeVideoLineWidth = tInfo->width;
    oCfg->activeVideoLineWidth = tInfo->width;
    dCfg->hsWidth = tInfo->hSyncLen;
    dCfg->activeLineStart0 = tInfo->vFrontPorch + tInfo->vBackPorch +
                                tInfo->vSyncLen;

    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        dCfg->activeLineStart0 -= 1;
    #endif

    if(FVID2_SF_PROGRESSIVE == tInfo->scanFormat)
    {
        aCfg->totalLinesPerFrame = tInfo->vFrontPorch +
                                   tInfo->vBackPorch +
                                   tInfo->vSyncLen +
                                   tInfo->height;
        dCfg->activeVideoLines0 = tInfo->height;
        aCfg->lastActiveLineFirstField = tInfo->vFrontPorch +
                                         tInfo->vBackPorch +
                                         tInfo->vSyncLen +
                                         tInfo->height - 1u;
        oCfg->activeVideoLines0 = tInfo->height;

        #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
        oCfg->activeVideoLines0 += 1;
        #endif
    }
    else
    {
        aCfg->totalLinesPerFrame = (2 * (tInfo->vFrontPorch +
                                   tInfo->vBackPorch +
                                   tInfo->vSyncLen)) +
                                   tInfo->height + 1u;
        aCfg->lastActiveLineFirstField = (tInfo->vFrontPorch +
                                         tInfo->vBackPorch +
                                         tInfo->vSyncLen) +
                                         (tInfo->height >> 1) - 1u;
        dCfg->activeVideoLines0 = tInfo->height >> 1u;
        dCfg->activeVideoLines1 = tInfo->height >> 1u;
        oCfg->activeVideoLines0 = tInfo->height >> 1u;
        oCfg->activeVideoLines1 = tInfo->height >> 1u;
        dCfg->activeLineStart1 = (2 * (tInfo->vFrontPorch +
                                 tInfo->vBackPorch +
                                 tInfo->vSyncLen)) +
                                (tInfo->height >> 1u);
        dCfg->vsStartLine1 = ((tInfo->vFrontPorch +
                              tInfo->vBackPorch +
                              tInfo->vSyncLen +
                              tInfo->height) / 2) +
                              tInfo->vSyncLen +
                              tInfo->vBackPorch;
    }
    dCfg->vsStartLine0 = tInfo->vFrontPorch;
    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        dCfg->vsStartLine0 -= 1;
    #endif
    dCfg->vsWidth0 = tInfo->vSyncLen;
    dCfg->vsWidth1 = tInfo->vSyncLen;
    dCfg->fidToggleLine1 = tInfo->vFrontPorch +
                             tInfo->vBackPorch +
                             tInfo->vSyncLen +
                             tInfo->vFrontPorch +
                             (tInfo->height >> 1u);
    oCfg->fidToggleLine1 = tInfo->vFrontPorch +
                             tInfo->vBackPorch +
                             tInfo->vSyncLen +
                             tInfo->vFrontPorch+
                             (tInfo->height >> 1u);
    oCfg->hsWidth = tInfo->hSyncLen;
    oCfg->activeLineStart0 = tInfo->vFrontPorch + tInfo->vBackPorch +
                                        tInfo->vSyncLen;

    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        oCfg->activeLineStart0 -= 1;
    #endif

    oCfg->activeLineStart1 = (2 * (tInfo->vFrontPorch +
                             tInfo->vBackPorch +
                             tInfo->vSyncLen)) +
                            (tInfo->height >> 1u);
    oCfg->vsWidth0 = tInfo->vSyncLen;
    oCfg->vsWidth1 = tInfo->vSyncLen;
    oCfg->vsStartLine1 = (tInfo->vFrontPorch +
                         tInfo->vBackPorch +
                         tInfo->vSyncLen +
                         tInfo->height - 1) / 2;

    if ((FVID2_STD_1080I_60 == tInfo->standard) ||
        (FVID2_STD_1080I_50 == tInfo->standard))
    {
        hdVencObj->vencCfg.dvoConfig.vsFidLineStart =
                    VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE;
    }
    else
    {
        hdVencObj->vencCfg.dvoConfig.vsFidLineStart =
                    VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING;
    }
}



static Void hdVencSetFidOffset(HdVenc_Obj *hdVencObj,
                               UInt32 fidOffset0,
                               UInt32 fidOffset1)
{
    VpsHal_HdVencOsdConfig      *oCfg;

    oCfg = &hdVencObj->vencCfg.osdConfig;

    oCfg->fidToggleLine0 += fidOffset0;
    oCfg->fidToggleLine1 += fidOffset1;
}



#if 0
/* To calculate the registers setting for the OSD interface based on
 * timing parameters.  For future use when venc will support cusotm
 * timings.
 */
static Void hdVencSetOsdConfig(HdVenc_Obj *hdVencObj,
                               FVID2_ModeInfo *tInfo)
{
    VpsHal_HdVencOsdConfig      *oCfg;

    oCfg = &hdVencObj->vencCfg.osdConfig;

    /* Horizontal Configuration */
    oCfg->HbiSignalStartPixel = 0u;

    oCfg->activeVideoLineWidth = tInfo->width;
    oCfg->hsWidth = tInfo->hSyncLen;
    oCfg->hsStartPixel = 1;

    /* Vertical Configuration */
    oCfg->activeLineStart0 = tInfo->vFrontPorch + tInfo->vBackPorch +
                                        tInfo->vSyncLen;

    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        oCfg->activeLineStart0 -= 1;
    #endif

    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        oCfg->lastNonActivePixelInLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 5u;
    }
    else
    {
        if ((VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN ==
                    hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN ==
                    hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC ==
                    hdVencObj->outputCfg.fmt))
        {
            oCfg->lastNonActivePixelInLine =
                tInfo->hSyncLen + tInfo->hBackPorch + tInfo->hFrontPorch -
                    HDVENC_HOFFSET - 8u;
        }
        else
        {
            oCfg->lastNonActivePixelInLine =
                tInfo->hSyncLen + tInfo->hBackPorch - HDVENC_HOFFSET - 8u;
        }
    }
    /* Whenever gamma correction is not bypassed, the DVO output will be
       delayed by one pixel resulting in a wrong pixel at left and clipping of
       a pixel at right. To correct this problem, We need to make an
       adjustment to OSD_AVST_H */
    if (FALSE == hdVencObj->vencCfg.bypassGammaCor)
    {
        oCfg->lastNonActivePixelInLine -= 1;
    }

    if (FVID2_SF_INTERLACED == tInfo->scanFormat)
    {
        oCfg->activeLineStart1 = tInfo->vFrontPorch +
                                 tInfo->vBackPorch +
                                 tInfo->vSyncLen +
                                 tInfo->vFrontPorch +
                                 tInfo->vBackPorch +
                                 tInfo->vSyncLen +
                                (tInfo->height >> 1u);
        oCfg->activeVideoLines0 = tInfo->height >> 1u;
        oCfg->activeVideoLines1 = tInfo->height >> 1u;

        oCfg->vsStartLine0 = tInfo->vFrontPorch;
        oCfg->vsStartLine1 = tInfo->vFrontPorch +
                             tInfo->vBackPorch +
                             tInfo->vSyncLen +
                             tInfo->vFrontPorch+
                             (tInfo->height >> 1u);
        oCfg->vsWidth0 = tInfo->vSyncLen;
        oCfg->vsWidth1 = tInfo->vSyncLen;

        oCfg->fidToggleLine0 = oCfg->vsStartLine0;
        oCfg->fidToggleLine1 = oCfg->vsStartLine1;
    }
    else
    {
        oCfg->activeLineStart1 = 0u;
        oCfg->activeVideoLines0 = tInfo->height;

        #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
        oCfg->activeVideoLines0 += 1;
        #endif

        oCfg->activeVideoLines1 = 0u;
        oCfg->vsStartLine0 = 1u;
        oCfg->vsStartLine1 = 0u;
        oCfg->vsWidth0 = tInfo->vSyncLen;
        oCfg->vsWidth1 = 0u;
    }
}

/* To calculate the registers setting for the Analog interface based on
 * timing parameters.  For future use when venc will support cusotm
 * timings.
 */
static Void hdVencSetAnalogConfig(HdVenc_Obj *hdVencObj,
                                  FVID2_ModeInfo *tInfo)
{
    VpsHal_HdVencAnalogConfig   *aCfg;

    aCfg = &hdVencObj->vencCfg.analogConfig;

    /* Horizontal Configuration */
    aCfg->totalPixelsPerLine = tInfo->hFrontPorch +
                               tInfo->hBackPorch +
                               tInfo->hSyncLen +
                               tInfo->width;
    aCfg->hbiPixelEnd = tInfo->hSyncLen + tInfo->hBackPorch - 1u;
    aCfg->numActivePixels = tInfo->width;
    aCfg->equPulseWidth = aCfg->hsWidth = tInfo->hSyncLen;

    /* Vertical Configuration */
    aCfg->vbiLineEnd0 = tInfo->vFrontPorch +
                        tInfo->vBackPorch +
                        tInfo->vSyncLen - 1u;

    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        aCfg->vbiLineEnd0 -= 1;
    #endif

    aCfg->vbiLineend1 = tInfo->vFrontPorch +
                        tInfo->vBackPorch +
                        tInfo->vSyncLen +
                        (tInfo->height / 2) +
                        tInfo->vFrontPorch +
                        tInfo->vBackPorch +
                        tInfo->vSyncLen + 1u
                        - 1u;

    if (FVID2_SF_INTERLACED == tInfo->scanFormat)
    {
        aCfg->totalLinesPerFrame = tInfo->vFrontPorch +
                                   tInfo->vBackPorch +
                                   tInfo->vSyncLen +
                                   tInfo->vFrontPorch +
                                   tInfo->vBackPorch +
                                   tInfo->vSyncLen +
                                   tInfo->height + 1u;
        aCfg->lastActiveLineFirstField = tInfo->vFrontPorch +
                                         tInfo->vBackPorch +
                                         tInfo->vSyncLen +
                                         (tInfo->height >> 1) - 1u;
    }
    else
    {
        aCfg->totalLinesPerFrame = tInfo->vFrontPorch +
                                   tInfo->vBackPorch +
                                   tInfo->vSyncLen +
                                   tInfo->height;
        aCfg->lastActiveLineFirstField = tInfo->vFrontPorch +
                                         tInfo->vBackPorch +
                                         tInfo->vSyncLen +
                                         tInfo->height - 1u;
    }

    aCfg->clampPeriod = tInfo->hFrontPorch +
                        tInfo->hBackPorch +
                        tInfo->hSyncLen - 1;
}

/* To calculate the registers setting for the DVO interface based on
 * timing parameters.  For future use when venc will support cusotm
 * timings.
 */
static Void hdVencSetDvoConfig(HdVenc_Obj *hdVencObj,
                               FVID2_ModeInfo *tInfo)
{
    VpsHal_HdVencDvoConfig      *dCfg;

    dCfg = &hdVencObj->vencCfg.dvoConfig;

    /* Horizontal Configuration */
    if (VPSHAL_HDVENC_HDCOMP == hdVencObj->instId)
    {
        dCfg->firstActivePixelLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 0u;
    }
    else
    {
        if ((VPSHAL_HDVENC_DVOFMT_10BIT_SINGLECHAN == hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_DOUBLECHAN == hdVencObj->outputCfg.fmt) ||
            (VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC == hdVencObj->outputCfg.fmt))
        {
            dCfg->firstActivePixelLine =
                tInfo->hSyncLen + tInfo->hBackPorch + tInfo->hFrontPorch -
                    HDVENC_HOFFSET - 0u;
        }
        else
        {
            dCfg->firstActivePixelLine = tInfo->hSyncLen + tInfo->hBackPorch -
                                            HDVENC_HOFFSET - 0u;
        }
    }
    dCfg->activeVideoLineWidth = tInfo->width;
    dCfg->hsWidth = tInfo->hSyncLen;
    dCfg->hsStartPixel = 0;

    /* Vertical Parameters */
    dCfg->activeLineStart0 = tInfo->vFrontPorch + tInfo->vBackPorch +
                                tInfo->vSyncLen;

    #ifdef VPSHAL_HDVENC_ADJUST_TIMING_FOR_SHIFT_ISSUE
    if(tInfo->scanFormat==FVID2_SF_PROGRESSIVE)
        dCfg->activeLineStart0 -= 1;
    #endif

    if (FVID2_SF_INTERLACED == tInfo->scanFormat)
    {
        dCfg->activeLineStart1 = tInfo->vFrontPorch +
                                 tInfo->vBackPorch +
                                 tInfo->vSyncLen +
                                 tInfo->vFrontPorch +
                                 tInfo->vBackPorch +
                                 tInfo->vSyncLen +
                                (tInfo->height >> 1u);
        dCfg->activeVideoLines0 = tInfo->height >> 1u;
        dCfg->activeVideoLines1 = tInfo->height >> 1u;

        dCfg->vsStartLine0 = tInfo->vFrontPorch;

        dCfg->vsStartLine1 = tInfo->vFrontPorch +
                             tInfo->vBackPorch +
                             tInfo->vSyncLen +
                             tInfo->vFrontPorch +
                             (tInfo->height >> 1u);
        dCfg->vsWidth0 = tInfo->vSyncLen;
        dCfg->vsWidth1 = tInfo->vSyncLen;
        dCfg->fidToggleLine0 = dCfg->vsStartLine0;
        dCfg->fidToggleLine1 = dCfg->vsStartLine1;
    }
    else
    {
        dCfg->activeLineStart1 = 0u;
        dCfg->activeVideoLines0 = tInfo->height;
        dCfg->activeVideoLines1 = 0u;
        dCfg->vsStartLine0 = tInfo->vFrontPorch;
        dCfg->vsStartLine1 = 0u;
        dCfg->vsWidth0 = tInfo->vSyncLen;
        dCfg->vsWidth1 = 0u;
        dCfg->fidToggleLine0 = 1u;
        dCfg->fidToggleLine1 = 0u;
    }

    if ((FVID2_STD_1080I_60 == tInfo->standard) ||
        (FVID2_STD_1080I_50 == tInfo->standard))
    {
        dCfg->vsFidLineStart =
                    VPSHAL_HDVENC_HFLINE_START_LINE_MIDDLE;
    }
    else
    {
        dCfg->vsFidLineStart =
                    VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING;
    }
}
#endif
