/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file TriDisplay_grpxTest.c
 *
 *  \brief Triple display example file for GRPX planes.
 *
 *  This will start the GRPX plane displays and keeps displaying the
 *  TI logo on top of the video planes.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/examples/common/vps/display/triDisplay/src/TriDisplay.h>
#include <ti/psp/examples/common/vps/display/triDisplay/src/Rgb888Input.h>


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

static Int32 App_dispGrpxCreate(App_DisplayObj *appObj);
static Int32 App_dispGrpxDelete(App_DisplayObj *appObj);
static Int32 App_dispGrpxCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 App_dispGrpxErrCbFxn(FVID2_Handle handle,
                                  Ptr appData,
                                  Void *errList,
                                  Ptr reserved);
static Void App_dispGrpxUpdateFrmList(App_DisplayObj *appObj,
                                      FVID2_FrameList *fList);
static Void App_dispGrpxSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);
static Void App_dispGrpxSetParams(App_DisplayObj *appObj,
                                  Vps_GrpxParamsList *grpxPrmList,
                                  Vps_GrpxRtParams *grpxRtPrms);
static Int32 App_dispGrpxInitVariables(App_DisplayObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Display application object per Grpx instance. */
static App_DisplayObj AppDispGrpxObjs[NUM_GRPX_DISPLAYS];

/* Input buffer for the grpx display path. Align the buffers properly. */
#pragma DATA_ALIGN(AppGrpxBuffer, VPS_BUFFER_ALIGNMENT)
static UInt32 AppGrpxBuffer[TOTAL_NUM_GRPX_BUFFERS]
                           [GRPX_BUFFER_SIZE / sizeof(UInt32)];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_dispInitGrpx
 *  GRPX plane init function.
 */
Int32 App_dispInitGrpx(App_DispComboObj *comboObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              dispCnt;
    App_DisplayObj     *appObj;

    /* Copy the input image */
    memcpy(&AppGrpxBuffer[0u][0u], Rgb888Input, sizeof(AppGrpxBuffer));

    /* Initialize the test parameters */
    for (dispCnt = 0u; dispCnt < NUM_GRPX_DISPLAYS; dispCnt++)
    {
        appObj = &AppDispGrpxObjs[dispCnt];
        appObj->driverId = FVID2_VPS_DISP_GRPX_DRV;
        appObj->driverInst = VPS_DISP_INST_GRPX0 + dispCnt;
        appObj->inBuf = (UInt8 *) &AppGrpxBuffer[0u][0u];
        appObj->inFrmSize = GRPX_BUFFER_SIZE;
        appObj->numInBuf = TOTAL_NUM_GRPX_BUFFERS;
        appObj->windowWidth = GRPX_WIDTH;
        appObj->windowHeight = GRPX_HEIGHT;
        appObj->scanFormat = FVID2_SF_PROGRESSIVE;
        appObj->mosaicInfo.numCol = 1u;
        appObj->mosaicInfo.numRow = 1u;
        appObj->mosaicInfo.numWindows =
            appObj->mosaicInfo.numCol * appObj->mosaicInfo.numRow;
        if (VPS_DISP_INST_GRPX2 == appObj->driverInst)
        {
            appObj->frameWidth = SD_FRAME_WIDTH;
            appObj->frameHeight = SD_FRAME_HEIGHT;
        }
        else
        {
            appObj->frameWidth = HD_FRAME_WIDTH;
            appObj->frameHeight = HD_FRAME_HEIGHT;
        }
    }

    /* Create and start each of the GRPX planes */
    for (dispCnt = 0u; dispCnt < NUM_GRPX_DISPLAYS; dispCnt++)
    {
        if (FALSE == comboObj->isGrpxActive[dispCnt])
        {
            continue;
        }
        retVal |= App_dispGrpxCreate(&AppDispGrpxObjs[dispCnt]);
    }

    return (retVal);
}



/**
 *  App_dispDeInitGrpx
 *  GRPX plane deinit function.
 */
Int32 App_dispDeInitGrpx(App_DispComboObj *comboObj)
{
    Int32       retVal = FVID2_SOK;
    UInt32      dispCnt;

    /* Stop and delete each of the GRPX planes */
    for (dispCnt = 0u; dispCnt < NUM_GRPX_DISPLAYS; dispCnt++)
    {
        if (FALSE == comboObj->isGrpxActive[dispCnt])
        {
            continue;
        }
        retVal |= App_dispGrpxDelete(&AppDispGrpxObjs[dispCnt]);
    }

    return (retVal);
}



/**
 *  App_dispGrpxCreate
 *  GRPX display create function.
 */
static Int32 App_dispGrpxCreate(App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  fListCnt;
    FVID2_Format            fmt;
    FVID2_FrameList        *fList;
    FVID2_CbParams          cbParams;
    Vps_GrpxCreateParams    createPrms =
    {
        VPS_VPDMA_MT_NONTILEDMEM,
        VPS_GRPX_STREAMING_MODE,
        FALSE
    };
    Vps_GrpxParamsList      grpxPrmList;
    Vps_GrpxRtParams        grpxRtPrms;

    /* Initialize and allocate handles */
    retVal = App_dispGrpxInitVariables(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Open driver */
    cbParams.cbFxn = App_dispGrpxCbFxn;
    cbParams.errCbFxn = App_dispGrpxErrCbFxn;
    cbParams.errList = &appObj->errFrameList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             appObj->driverId,
                             appObj->driverInst,
                             &createPrms,
                             NULL,
                             &cbParams);
    if (NULL == appObj->fvidHandle)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }
    Vps_printf("%s: Grpx Driver Opened for Instance %d!!\n",
        APP_NAME, appObj->driverInst);

    /* Set the required format */
    App_dispGrpxSetFormat(appObj, &fmt);
    retVal = FVID2_setFormat(appObj->fvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    App_dispGrpxSetParams(appObj, &grpxPrmList, &grpxRtPrms);
    retVal = FVID2_control(
                appObj->fvidHandle,
                IOCTL_VPS_SET_GRPX_PARAMS,
                &grpxPrmList,
                NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n",__FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Prime Buffers */
    for (fListCnt = 0u; fListCnt < NUM_PRIMING; fListCnt++)
    {
        /* Update the buffer address */
        fList = &appObj->frameList[fListCnt];
        App_dispGrpxUpdateFrmList(appObj, fList);

        /* Queue buffers to driver */
        retVal = FVID2_queue(appObj->fvidHandle, fList, 0u);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Start driver */
    Vps_printf("%s: Starting Grpx Driver %d...\n",
        APP_NAME, appObj->driverInst);
    retVal = FVID2_start(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    return (retVal);
}



/**
 *  App_dispGrpxDelete
 */
static Int32 App_dispGrpxDelete(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    FVID2_FrameList    *fList;

    /* Stop driver */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Grpx Driver %d Stopped!!\n",
        APP_NAME, appObj->driverInst);

    /* Dequeue all the driver held buffers */
    fList = &appObj->frameList[0u];
    while (1)
    {
        /* Dequeue buffers from driver */
        retVal = FVID2_dequeue(
                     appObj->fvidHandle,
                     fList,
                     0u,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    /* Close driver */
    retVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    Vps_printf("%s: Grpx Display %d Test Successful!!\n",
        APP_NAME, appObj->driverInst);

    return (retVal);
}



/**
 *  App_dispGrpxCbFxn
 *  Driver callback function.
 */
static Int32 App_dispGrpxCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32               retVal;
    FVID2_FrameList    *fList;
    App_DisplayObj     *appObj = (App_DisplayObj *) appData;

    /* Use the same frame list for all request */
    fList = &appObj->frameList[0u];

    /* Dequeue buffer from driver */
    retVal = FVID2_dequeue(handle, fList, 0u, FVID2_TIMEOUT_NONE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    appObj->completedFrames++;

    /* Update the buffer address */
    App_dispGrpxUpdateFrmList(appObj, fList);

    /* Queue the updated frame list back to the driver */
    retVal = FVID2_queue(handle, fList, 0u);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    return (retVal);
}



/**
 *  App_dispGrpxErrCbFxn
 *  Driver error callback function.
 */
static Int32 App_dispGrpxErrCbFxn(FVID2_Handle handle,
                                  Ptr appData,
                                  Void *errList,
                                  Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}



/**
 *  App_dispGrpxUpdateFrmList
 *  Updates the frame list with the next request buffer information.
 */
static Void App_dispGrpxUpdateFrmList(App_DisplayObj *appObj,
                                      FVID2_FrameList *fList)
{
    UInt32          index, frmCnt;
    FVID2_Frame    *frame;

    /* Fill the next set of buffers */
    fList->numFrames = appObj->mosaicInfo.numWindows;
    for (frmCnt = 0u; frmCnt < appObj->mosaicInfo.numWindows; frmCnt++)
    {
        if (appObj->numInBuf == appObj->bufferIndex)
        {
            appObj->bufferIndex = 0u;
        }
        index = appObj->bufferIndex;

        frame = fList->frames[frmCnt];
        if (FVID2_SF_PROGRESSIVE == appObj->scanFormat)
        {
            frame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
        }
        else
        {
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *) (appObj->inBuf +
                            (index * appObj->inFrmSize) + GRPX_BUFFER_PITCH);
        }
        appObj->bufferIndex++;
    }

    appObj->submittedFrames++;

    return;
}



/**
 *  App_dispGrpxSetFormat
 *  Sets the required/default format.
 */
static Void App_dispGrpxSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt)
{
    fmt->channelNum = 0u;
    fmt->width = appObj->windowWidth;
    fmt->height = appObj->windowHeight;
    fmt->pitch[FVID2_RGB_ADDR_IDX] = GRPX_BUFFER_PITCH;
    fmt->scanFormat = appObj->scanFormat;
    if (FVID2_SF_INTERLACED == appObj->scanFormat)
    {
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = TRUE;
    }
    else
    {
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    }
    fmt->dataFormat = FVID2_DF_RGB24_888;
    fmt->bpp = FVID2_BPP_BITS24;
    fmt->reserved = NULL;

    return;
}



/**
 *  App_dispGrpxSetParams
 *  Sets the required Grpx parameters.
 */
static Void App_dispGrpxSetParams(App_DisplayObj *appObj,
                                  Vps_GrpxParamsList *grpxPrmList,
                                  Vps_GrpxRtParams *grpxRtPrms)
{
    grpxPrmList->numRegions = 1u;
    grpxPrmList->gParams = grpxRtPrms;
    grpxPrmList->clutPtr = NULL;
    grpxPrmList->scParams = NULL;

    grpxRtPrms->regId = 0u;
    grpxRtPrms->format = FVID2_DF_RGB24_888;
    grpxRtPrms->pitch[FVID2_RGB_ADDR_IDX] = GRPX_BUFFER_PITCH;
    grpxRtPrms->rotation = VPS_MEM_0_ROTATION;
    grpxRtPrms->scParams = NULL;
    grpxRtPrms->stenPtr = NULL;
    grpxRtPrms->stenPitch = 0u;

    grpxRtPrms->regParams.regionWidth = appObj->windowWidth;
    grpxRtPrms->regParams.regionHeight = appObj->windowHeight;
    grpxRtPrms->regParams.regionPosX =
        (appObj->frameWidth - appObj->windowWidth) / 2u;
    grpxRtPrms->regParams.regionPosY = 0u;
    grpxRtPrms->regParams.dispPriority = 1u;
    grpxRtPrms->regParams.firstRegion = TRUE;
    grpxRtPrms->regParams.lastRegion = TRUE;
    grpxRtPrms->regParams.scEnable = FALSE;
    grpxRtPrms->regParams.stencilingEnable = FALSE;
    grpxRtPrms->regParams.bbEnable = FALSE;
    grpxRtPrms->regParams.bbAlpha = 0u;
    grpxRtPrms->regParams.blendAlpha = 0xFF;
    grpxRtPrms->regParams.blendType = VPS_GRPX_BLEND_REGION_GLOBAL;
    grpxRtPrms->regParams.transEnable = TRUE;
    grpxRtPrms->regParams.transType = VPS_GRPX_TRANS_3BIT_MASK;
    grpxRtPrms->regParams.transColorRgb24 = 0x00000000u;    /* Black */
}



/**
 *  App_dispGrpxInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Int32 App_dispGrpxInitVariables(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              fListCnt, frmCnt;
    FVID2_Frame        *frame;
    FVID2_FrameList    *fList;

    appObj->bufferIndex = 0u;
    appObj->submittedFrames = 0u;
    appObj->completedFrames = 0u;

    /* Init frame list */
    for (fListCnt = 0u; fListCnt < NUM_FRAMELIST; fListCnt++)
    {
        fList = &appObj->frameList[fListCnt];
        fList->numFrames = appObj->mosaicInfo.numWindows;
        fList->perListCfg = NULL;
        fList->reserved = NULL;
        for (frmCnt = 0u; frmCnt < appObj->mosaicInfo.numWindows; frmCnt++)
        {
            frame = &appObj->frames[fListCnt][frmCnt];
            fList->frames[frmCnt] = frame;
            frame->channelNum = 0u;
            frame->appData = NULL;
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->reserved = NULL;
            frame->fid = FVID2_FID_FRAME;
        }
    }

    /* Init error frame list */
    memset(&appObj->errFrameList, 0u, sizeof (appObj->errFrameList));

    return (retVal);
}
