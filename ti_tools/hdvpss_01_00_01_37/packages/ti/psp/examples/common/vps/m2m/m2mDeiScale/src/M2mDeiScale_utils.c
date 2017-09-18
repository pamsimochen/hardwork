/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file M2mDeiScale_utils.c
 *
 *  \brief VPS DEI memory to memory driver example utility file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/std.h>
#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_app.h>

#include "M2mDeiScale_utils.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* String to print the instance */
static char *AppM2mDeiInstName[] =
{
#ifdef TI_816X_BUILD
    "DeiHqSc1Wb0",
    "DeiSc2Wb1",
    "DeiHqSc3Vip0",
    "DeiSc4Vip1",
    "DeiHqSc1Sc3Wb0Vip0",
    "DeiSc2Sc4Wb1Vip1"
#else
    "DeiSc1Wb0",
    "DeiSc3Vip0",
    "DeiSc1Sc3Wb0Vip0",
    "Sc2Wb1",
    "Sc4Vip1",
    "Sc2Sc4Wb1Vip1",
#endif
};

/* String to print the format */
static char *AppM2mDeiFmtName[] =
{
    "yuyv422",
    "nv12",
    "nv16"
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_m2mDeiAllocBuf
 *  Allocates input and output buffers depending on format and memory mode.
 */
Int32 App_m2mDeiAllocBuf(App_M2mDeiObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    UInt32              inBufSizeY, inBufSizeCbCr;
    UInt32              outBufSizeDei, outBufSizeYVip, outBufSizeCbCrVip;
    UInt32              cInHeight, cOutVipHeight;
    UInt8              *tempPtr;
    UInt32              fmtIndex, width;

    appObj->inLoadBuf = NULL;
    appObj->outSaveBufDei = NULL;
    appObj->outSaveBufVip = NULL;
    for (cnt = 0u; cnt < appObj->numInBuffers; cnt++)
    {
        appObj->inBufY[cnt] = NULL;
        appObj->inBufCbCr[cnt] = NULL;
    }
    for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
    {
        appObj->outBufDei[cnt] = NULL;
        appObj->outBufYVip[cnt] = NULL;
        appObj->outBufCbCrVip[cnt] = NULL;
    }

    /* Calculate pitch and make them aligned to VPS_BUFFER_ALIGNMENT.
     * Also calculate the input/output buffer size depending on format */
    if ((FVID2_DF_YUV420SP_UV == appObj->inDataFmt) ||
        (FVID2_DF_YUV422SP_UV == appObj->inDataFmt))
    {
        if (VPS_VPDMA_MT_TILEDMEM == appObj->inMemType)
        {
            appObj->inPitchY = VPSUTILS_TILER_CNT_8BIT_PITCH;
            appObj->inPitchCbCr = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }
        else
        {
            appObj->inPitchY =
                VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT);
            appObj->inPitchCbCr =
                VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT);
        }
        inBufSizeY = VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT) *
            appObj->inHeight;
        if (FVID2_DF_YUV420SP_UV == appObj->inDataFmt)
        {
            inBufSizeCbCr = inBufSizeY / 2u;
            cInHeight = appObj->inHeight / 2u;
        }
        else
        {
            inBufSizeCbCr = inBufSizeY;
            cInHeight = appObj->inHeight;
        }
    }
    else
    {
        appObj->inPitchY =
            VpsUtils_align(appObj->inWidth * 2u, VPS_BUFFER_ALIGNMENT);
        appObj->inPitchCbCr = 0u;
        inBufSizeY = VpsUtils_align(appObj->inWidth * 2u, VPS_BUFFER_ALIGNMENT)
            * appObj->inHeight;
        inBufSizeCbCr = 0u;
        cInHeight = 0u;
    }
    appObj->outPitchDei =
        VpsUtils_align(appObj->outWidthDei * 2u, VPS_BUFFER_ALIGNMENT);
    outBufSizeDei =
        VpsUtils_align(appObj->outWidthDei * 2u, VPS_BUFFER_ALIGNMENT) *
        appObj->outHeightDei;
    if ((FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip) ||
        (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip))
    {
        if (VPS_VPDMA_MT_TILEDMEM == appObj->outMemTypeVip)
        {
            appObj->outPitchYVip = VPSUTILS_TILER_CNT_8BIT_PITCH;
            appObj->outPitchCbCrVip = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }
        else
        {
            appObj->outPitchYVip =
                VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT);
            appObj->outPitchCbCrVip =
                VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT);
        }
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
        {
            outBufSizeCbCrVip = outBufSizeYVip / 2u;
            cOutVipHeight = appObj->outHeightVip / 2u;
        }
        else
        {
            outBufSizeCbCrVip = outBufSizeYVip;
            cOutVipHeight = appObj->outHeightVip;
        }
    }
    else
    {
        appObj->outPitchYVip =
            VpsUtils_align(appObj->outWidthVip * 2u, VPS_BUFFER_ALIGNMENT);
        appObj->outPitchCbCrVip = 0u;
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip * 2u, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = 0u;
        cOutVipHeight = 0u;
    }

    /* Allocate input load and output save memory for all the buffers at one
     * go so that memory is allocated contiguously. */
    appObj->inLoadBuf = VpsUtils_memAlloc(
                            (appObj->numInBuffers *
                                (inBufSizeY + inBufSizeCbCr)),
                            VPS_BUFFER_ALIGNMENT);
    if (NULL == appObj->inLoadBuf)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        retVal = FVID2_EALLOC;
    }

    if (VpsUtils_appM2mDeiIsWbInst(appObj->drvInst))
    {
        appObj->outSaveBufDei = VpsUtils_memAlloc(
                                    (appObj->numOutBuffers * outBufSizeDei),
                                    VPS_BUFFER_ALIGNMENT);
        if (NULL == appObj->outSaveBufDei)
        {
            Vps_printf(
                "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = FVID2_EALLOC;
        }
        else
        {
            if (Vps_platformIsEvm())
            {
                memset(
                    appObj->outSaveBufDei,
                    0u,
                    (appObj->numOutBuffers * outBufSizeDei));
            }
        }
    }

    if (VpsUtils_appM2mDeiIsVipInst(appObj->drvInst))
    {
        appObj->outSaveBufVip = VpsUtils_memAlloc(
                                    (appObj->numOutBuffers *
                                        (outBufSizeYVip + outBufSizeCbCrVip)),
                                    VPS_BUFFER_ALIGNMENT);
        if (NULL == appObj->outSaveBufVip)
        {
            Vps_printf(
                "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            retVal = FVID2_EALLOC;
        }
        else
        {
            if (Vps_platformIsEvm())
            {
                memset(
                    appObj->outSaveBufVip,
                    0u,
                    (appObj->numOutBuffers *
                        (outBufSizeYVip + outBufSizeCbCrVip)));
            }
        }
    }

    /* Allocate input buffers from tiler memory or assign pointer depending on
     * whether the input is tiled or non tiled. */
    if (VPS_VPDMA_MT_TILEDMEM == appObj->inMemType)
    {
        for (cnt = 0u; cnt < appObj->numInBuffers; cnt++)
        {
            appObj->inBufY[cnt] = (UInt8 *) VpsUtils_tilerAlloc(
                                                VPSUTILS_TILER_CNT_8BIT,
                                                appObj->inWidth,
                                                appObj->inHeight);
            if (NULL == appObj->inBufY[cnt])
            {
                Vps_printf(
                    "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                retVal = FVID2_EALLOC;
                break;
            }
            if (inBufSizeCbCr > 0u)
            {
                appObj->inBufCbCr[cnt] = (UInt8 *) VpsUtils_tilerAlloc(
                                                       VPSUTILS_TILER_CNT_16BIT,
                                                       appObj->inWidth,
                                                       cInHeight);
                if (NULL == appObj->inBufCbCr[cnt])
                {
                    Vps_printf(
                        "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                    retVal = FVID2_EALLOC;
                    break;
                }
            }
        }
    }
    else
    {
        tempPtr = appObj->inLoadBuf;
        for (cnt = 0u; cnt < appObj->numInBuffers; cnt++)
        {
            appObj->inBufY[cnt] = tempPtr;
            tempPtr += inBufSizeY;
            if (inBufSizeCbCr > 0u)
            {
                appObj->inBufCbCr[cnt] = tempPtr;
                tempPtr += inBufSizeCbCr;
            }
        }
    }

    /* Assign DEI writeback output buffers from out0 save buffer */
    if (VpsUtils_appM2mDeiIsWbInst(appObj->drvInst))
    {
        tempPtr = appObj->outSaveBufDei;
        for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
        {
            appObj->outBufDei[cnt] = tempPtr;
            tempPtr += outBufSizeDei;
        }
    }

    /* Allocate DEI-VIP output buffers if needed */
    if (VpsUtils_appM2mDeiIsVipInst(appObj->drvInst))
    {
        /* Allocate VIP output buffers from tiler memory or assign pointer
           depending on whether the output is tiled or non tiled. */
        if (VPS_VPDMA_MT_TILEDMEM == appObj->outMemTypeVip)
        {
            for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
            {
                appObj->outBufYVip[cnt] = (UInt8 *) VpsUtils_tilerAlloc(
                                                        VPSUTILS_TILER_CNT_8BIT,
                                                        appObj->outWidthVip,
                                                        appObj->outHeightVip);
                if (NULL == appObj->outBufYVip[cnt])
                {
                    Vps_printf(
                        "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                    retVal = FVID2_EALLOC;
                    break;
                }
                if (outBufSizeCbCrVip > 0u)
                {
                    appObj->outBufCbCrVip[cnt] = (UInt8 *)
                        VpsUtils_tilerAlloc(
                            VPSUTILS_TILER_CNT_16BIT,
                            appObj->outWidthVip,
                            cOutVipHeight);
                    if (NULL == appObj->outBufCbCrVip[cnt])
                    {
                        Vps_printf(
                            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                        retVal = FVID2_EALLOC;
                        break;
                    }
                }
            }
        }
        else
        {
            tempPtr = appObj->outSaveBufVip;
            for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
            {
                appObj->outBufYVip[cnt] = tempPtr;
                tempPtr += outBufSizeYVip;
                if (outBufSizeCbCrVip > 0u)
                {
                    appObj->outBufCbCrVip[cnt] = tempPtr;
                    tempPtr += outBufSizeCbCrVip;
                }
            }
        }
    }

    /* Free the allocated memory if error occurs */
    if (FVID2_SOK != retVal)
    {
        App_m2mDeiFreeBuf(appObj);
    }
    else
    {
        /* Print buffer address information */
        Vps_printf("%s: DEI In Buffer Address    : 0x%p, Size: 0x%p\n",
            APP_NAME, appObj->inLoadBuf,
            appObj->numInBuffers * (inBufSizeY + inBufSizeCbCr));
        if (NULL != appObj->outSaveBufDei)
        {
            Vps_printf("%s: DEI-WB Out Buffer Address: 0x%p, Size: 0x%p\n",
                APP_NAME, appObj->outSaveBufDei,
                appObj->numOutBuffers * outBufSizeDei);
        }
        if (NULL != appObj->outSaveBufVip)
        {
            Vps_printf("%s: VIP Out Buffer Address   : 0x%p, Size: 0x%p\n",
                APP_NAME, appObj->outSaveBufVip,
                appObj->numOutBuffers * (outBufSizeYVip + outBufSizeCbCrVip));
        }
        if (NULL != appObj->outSaveBufDei)
        {
            fmtIndex = 0u;
            width = appObj->outPitchDei / 2u;
            Vps_printf(
                "saveRaw(0, 0x%.8x, "
                "\"C:\\\\dei%sCh%dWbOut_%s_prog_packed_%d_%d.tigf\", "
                "%d, 32, true);\n",
                appObj->outSaveBufDei, AppM2mDeiInstName[appObj->drvInst],
                appObj->numCh, AppM2mDeiFmtName[fmtIndex], width,
                appObj->outHeightDei,
                (appObj->numOutBuffers * outBufSizeDei)/4);
        }
        if (NULL != appObj->outSaveBufVip)
        {
            fmtIndex = 0u;
            width = appObj->outPitchYVip / 2u;
            if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
            {
                fmtIndex = 1u;
                width = appObj->outPitchYVip;
            }
            if (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip)
            {
                fmtIndex = 2u;
                width = appObj->outPitchYVip;
            }
            Vps_printf(
                "saveRaw(0, 0x%.8x, "
                "\"C:\\\\dei%sCh%dVipOut_%s_prog_packed_%d_%d.tigf\", "
                "%d, 32, true);\n",
                appObj->outSaveBufVip, AppM2mDeiInstName[appObj->drvInst],
                appObj->numCh, AppM2mDeiFmtName[fmtIndex], width,
                appObj->outHeightVip,
                (appObj->numOutBuffers *
                    (outBufSizeYVip + outBufSizeCbCrVip))/4);
        }
    }

    return (retVal);
}



/**
 *  App_m2mDeiFreeBuf
 *  Frees the allocated input and output buffers.
 */
Int32 App_m2mDeiFreeBuf(App_M2mDeiObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    UInt32              inBufSizeY, inBufSizeCbCr;
    UInt32              outBufSizeDei, outBufSizeYVip, outBufSizeCbCrVip;

    /* Calculate the input/output buffer size depending on format */
    if (FVID2_DF_YUV420SP_UV == appObj->inDataFmt)
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT) *
            appObj->inHeight;
        inBufSizeCbCr = inBufSizeY / 2u;
    }
    else if (FVID2_DF_YUV422SP_UV == appObj->inDataFmt)
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT) *
            appObj->inHeight;
        inBufSizeCbCr = inBufSizeY;
    }
    else
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth * 2u, VPS_BUFFER_ALIGNMENT)
            * appObj->inHeight;
        inBufSizeCbCr = 0u;
    }
    outBufSizeDei =
        VpsUtils_align(appObj->outWidthDei * 2u, VPS_BUFFER_ALIGNMENT) *
        appObj->outHeightDei;
    if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = outBufSizeYVip / 2u;
    }
    else if (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip)
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = outBufSizeYVip;
    }
    else
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip * 2u, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = 0u;
    }

    /* Free input load and output save buffers. */
    if (NULL != appObj->inLoadBuf)
    {
        VpsUtils_memFree(
            appObj->inLoadBuf,
            (appObj->numInBuffers * (inBufSizeY + inBufSizeCbCr)));
    }
    if (NULL != appObj->outSaveBufDei)
    {
        VpsUtils_memFree(
            appObj->outSaveBufDei,
            (appObj->numOutBuffers * outBufSizeDei));
    }
    if (NULL != appObj->outSaveBufVip)
    {
        VpsUtils_memFree(
            appObj->outSaveBufVip,
            (appObj->numOutBuffers * (outBufSizeYVip + outBufSizeCbCrVip)));
    }

    /* Free tiled buffers if already allocated */
    VpsUtils_tilerFreeAll();

    /* Reset the buffer address */
    appObj->inLoadBuf = NULL;
    appObj->outSaveBufDei = NULL;
    appObj->outSaveBufVip = NULL;
    for (cnt = 0u; cnt < appObj->numInBuffers; cnt++)
    {
        appObj->inBufY[cnt] = NULL;
        appObj->inBufCbCr[cnt] = NULL;
    }
    for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
    {
        appObj->outBufDei[cnt] = NULL;
        appObj->outBufYVip[cnt] = NULL;
        appObj->outBufCbCrVip[cnt] = NULL;
    }

    return (retVal);
}



/**
 *  App_m2mDeiCopyToTilerMem
 *  Copies the input video to the tiler memory space if input memory mode
 *  to test is tiled.
 */
Void App_m2mDeiCopyToTilerMem(App_M2mDeiObj *appObj)
{
    UInt32              cnt;
    UInt32              inBufSizeY, inBufSizeCbCr, cHeight;
    UInt8              *tempPtr;

    /* Calculate the input buffer size depending on format */
    if (FVID2_DF_YUV420SP_UV == appObj->inDataFmt)
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT) *
            appObj->inHeight;
        inBufSizeCbCr = inBufSizeY / 2u;
        cHeight = appObj->inHeight / 2u;
    }
    else if (FVID2_DF_YUV422SP_UV == appObj->inDataFmt)
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT) *
            appObj->inHeight;
        inBufSizeCbCr = inBufSizeY;
        cHeight = appObj->inHeight;
    }
    else
    {
        inBufSizeY = VpsUtils_align(appObj->inWidth * 2u, VPS_BUFFER_ALIGNMENT)
            * appObj->inHeight;
        inBufSizeCbCr = 0u;
        cHeight = 0u;
    }

    /* Copy the input buffer to tiled input buffer if input memory mode
     * is tiled */
    if ((NULL != appObj->inLoadBuf) &&
        (VPS_VPDMA_MT_TILEDMEM == appObj->inMemType))
    {
        Vps_printf("%s: Copying input buffer (0x%p) to tiler space...\n",
            APP_NAME, appObj->inLoadBuf);
        tempPtr = appObj->inLoadBuf;
        for (cnt = 0u; cnt < appObj->numInBuffers; cnt++)
        {
            VpsUtils_tilerCopy(
                VPSUTILS_TILER_COPY_FROM_DDR,
                (UInt32) appObj->inBufY[cnt],
                appObj->inWidth,
                appObj->inHeight,
                tempPtr,
                VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT));
            tempPtr += inBufSizeY;
            if (NULL != appObj->inBufCbCr[cnt])
            {
                VpsUtils_tilerCopy(
                    VPSUTILS_TILER_COPY_FROM_DDR,
                    (UInt32) appObj->inBufCbCr[cnt],
                    appObj->inWidth,
                    cHeight,
                    tempPtr,
                    VpsUtils_align(appObj->inWidth, VPS_BUFFER_ALIGNMENT));
                tempPtr += inBufSizeCbCr;
            }
        }
        Vps_printf("%s: DDR to tiler copy done\n", APP_NAME);
    }

    return;
}



/**
 *  App_m2mDeiCopyFromTilerMem
 *  Copies the processed output video from tiler memory space to output memory
 *  when output mode to test is tiled.
 */
Void App_m2mDeiCopyFromTilerMem(App_M2mDeiObj *appObj)
{
    UInt32              cnt;
    UInt32              outBufSizeYVip, outBufSizeCbCrVip, cHeight;
    UInt8              *tempPtr;

    /* Calculate the output buffer size depending on format */
    if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = outBufSizeYVip / 2u;
        cHeight = appObj->outHeightVip / 2u;
    }
    else if (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip)
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = outBufSizeYVip;
        cHeight = appObj->outHeightVip;
    }
    else
    {
        outBufSizeYVip =
            VpsUtils_align(appObj->outWidthVip * 2u, VPS_BUFFER_ALIGNMENT) *
            appObj->outHeightVip;
        outBufSizeCbCrVip = 0u;
        cHeight = 0u;
    }

    /* Copy the processed output buffer from tiler memory to input buffer if
     * output memory mode is tiled */
    if ((NULL != appObj->outSaveBufVip) &&
        (VPS_VPDMA_MT_TILEDMEM == appObj->outMemTypeVip))
    {
        Vps_printf(
            "%s: Copying tiled output buffer to save buffer (0x%p)...\n",
            APP_NAME, appObj->outSaveBufVip);
        tempPtr = appObj->outSaveBufVip;
        for (cnt = 0u; cnt < appObj->numOutBuffers; cnt++)
        {
            if (NULL != appObj->outBufYVip[cnt])
            {
                VpsUtils_tilerCopy(
                    VPSUTILS_TILER_COPY_TO_DDR,
                    (UInt32) appObj->outBufYVip[cnt],
                    appObj->outWidthVip,
                    appObj->outHeightVip,
                    tempPtr,
                    VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT));
                tempPtr += outBufSizeYVip;
            }
            if (NULL != appObj->outBufCbCrVip[cnt])
            {
                VpsUtils_tilerCopy(
                    VPSUTILS_TILER_COPY_TO_DDR,
                    (UInt32) appObj->outBufCbCrVip[cnt],
                    appObj->outWidthVip,
                    cHeight,
                    tempPtr,
                    VpsUtils_align(appObj->outWidthVip, VPS_BUFFER_ALIGNMENT));
                tempPtr += outBufSizeCbCrVip;
            }
        }
        Vps_printf("%s: Tiler to DDR copy done\n", APP_NAME);
    }

    return;
}



/**
 *  App_m2mDeiProgramScCoeff
 *  Programs the scalar coefficient.
 */
Int32 App_m2mDeiProgramScCoeff(App_M2mDeiObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    Vps_ScCoeffParams   coeffPrms;
    Vps_ScLazyLoadingParams lazyLoadingParams;

    coeffPrms.hScalingSet = VPS_SC_DS_SET_8_16;
    coeffPrms.vScalingSet = VPS_SC_DS_SET_8_16;
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;

    /* Program DEI scalar coefficient - Always used */
    Vps_printf("%s: Programming DEI Scalar coefficients ...\n", APP_NAME);
    retVal = FVID2_control(
                 appObj->fvidHandle,
                 IOCTL_VPS_SET_COEFFS,
                 &coeffPrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Programming of coefficients failed\n", APP_NAME);
        return (retVal);
    }
    Vps_printf("%s: Programming DEI Scalar coefficients Done\n", APP_NAME);

    lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;
#ifdef TI_816X_BUILD
    lazyLoadingParams.enableLazyLoading = TRUE;
    lazyLoadingParams.enableFilterSelect = TRUE;
#else
    lazyLoadingParams.enableLazyLoading = FALSE;
    lazyLoadingParams.enableFilterSelect = FALSE;
#endif
    retVal = FVID2_control(
                 appObj->fvidHandle,
                 IOCTL_VPS_SC_SET_LAZY_LOADING,
                 &lazyLoadingParams,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s:Enabling Lazy Loading failed for scalar ID %d\n",
            APP_NAME, VPS_M2M_DEI_SCALAR_ID_DEI_SC);
    }

    /* Program the second scalar coefficient if needed */
    if (VpsUtils_appM2mDeiIsVipInst(appObj->drvInst))
    {
        if (FALSE == appObj->chPrms[0u].vipScCfg->bypass)
        {
            /* Program VIP scalar coefficients */
            Vps_printf(
                "%s: Programming VIP Scalar coefficients ...\n", APP_NAME);
            coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
            retVal = FVID2_control(
                         appObj->fvidHandle,
                         IOCTL_VPS_SET_COEFFS,
                         &coeffPrms,
                         NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf(
                    "%s: Programming of coefficients failed\n", APP_NAME);
                return (retVal);
            }
            Vps_printf(
                "%s: Programming VIP Scalar coefficients Done\n", APP_NAME);

            lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
            retVal = FVID2_control(
                         appObj->fvidHandle,
                         IOCTL_VPS_SC_SET_LAZY_LOADING,
                         &lazyLoadingParams,
                         NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s:Enabling Lazy Loading failed for scalar ID %d\n",
                    APP_NAME, VPS_M2M_DEI_SCALAR_ID_VIP_SC);
            }
        }
    }

    return (retVal);
}
