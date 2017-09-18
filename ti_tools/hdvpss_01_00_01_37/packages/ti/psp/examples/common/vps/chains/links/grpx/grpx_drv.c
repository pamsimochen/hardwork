/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file grpxLink_drv.c
 *
 *  \brief This will start the GRPX plane displays and keeps displaying the
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
#include <ti/psp/examples/common/vps/chains/links/grpx.h>
#include <ti/psp/examples/common/vps/chains/links/grpx/grpx_priv.h>
#include <ti/psp/examples/common/vps/chains/links/grpx/Rgb888Input.h>
#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>

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
                                      FVID2_FrameList *fList, UInt32 fListCnt);
static Void App_dispGrpxSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);
static Void App_dispGrpxSetParams(App_DisplayObj *appObj,
                                  Vps_GrpxParamsList *grpxPrmList,
                                  Vps_MultiWinParams *multiWinPrm);
static Int32 App_dispGrpxInitVariables(App_DisplayObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Display application object per Grpx instance. */
static App_DisplayObj AppDispGrpxObjs[NUM_GRPX_DISPLAYS];

/* Input buffer for the grpx display path. Align the buffers properly. */
static UInt8 *g_pAppGrpxBuffer[NUM_GRPX_DISPLAYS];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  GrpxLink_init
 *  GRPX plane init function.
 */
Int32 Grpx_init(void)
{
    Int32 i, k;
    UInt8 *pDst, *pSrc;

    /* alloc and copy icon to grpx buffer */
    for(i=0; i<NUM_GRPX_DISPLAYS; i++)
    {
        /* alloc buffer */
        g_pAppGrpxBuffer[i] = VpsUtils_memAlloc(GRPX_BUFFER_SIZE, VPS_BUFFER_ALIGNMENT);
        GT_assert( GT_DEFAULT_MASK, g_pAppGrpxBuffer[i]!=NULL);

        /* clear with 0's */
        memset(g_pAppGrpxBuffer[i], 0, GRPX_BUFFER_SIZE);

        /* copy icon to correct position inside buffer */
        pDst = g_pAppGrpxBuffer[i] + GRPX_BUFFER_PITCH*GRPX_STARTY + GRPX_STARTX*GRPX_BPP;
        pSrc = (UInt8*)Rgb888Input;

        if(i==NUM_GRPX_DISPLAYS-1)
        {
            pDst = g_pAppGrpxBuffer[i] + GRPX_BUFFER_PITCH*GRPX_STARTY + (GRPX_WIDTH - GRPX_ICON_WIDTH - GRPX_STARTX)*GRPX_BPP;
        }

        for(k=0; k<GRPX_ICON_HEIGHT; k++)
        {
            memcpy(pDst, pSrc, GRPX_ICON_WIDTH*GRPX_ICON_BPP);
            pSrc += GRPX_ICON_WIDTH*GRPX_ICON_BPP;
            pDst += GRPX_BUFFER_PITCH;
        }
    }

    return (FVID2_SOK);
}

/**
 *  GrpxLink_deInit
 *  GRPX plane deinit function.
 */
Int32 Grpx_deInit(void)
{
    Int32 i;

    for(i=0; i<NUM_GRPX_DISPLAYS; i++)
    {
        VpsUtils_memFree(g_pAppGrpxBuffer[i], GRPX_BUFFER_SIZE);
    }

    return (FVID2_SOK);
}

Int32 Grpx_start(UInt32 linkId)
{
    Int32               retVal = FVID2_SOK;
    UInt32              driverInst;
    App_DisplayObj     *appObj = NULL;
    Int32               bufId=0;

    switch (linkId)
    {
        case SYSTEM_LINK_ID_GRPX_0:
        {
            driverInst = VPS_DISP_INST_GRPX0;
            appObj = &AppDispGrpxObjs[0u];
            bufId = 0;
        }
        break;

        case SYSTEM_LINK_ID_GRPX_1:
        {
            driverInst = VPS_DISP_INST_GRPX1;
            appObj = &AppDispGrpxObjs[1u];
            bufId = 1;
        }
        break;

        case SYSTEM_LINK_ID_GRPX_2:
        {
            driverInst = VPS_DISP_INST_GRPX2;
            appObj = &AppDispGrpxObjs[2u];
            bufId = 2;
        }
        break;

        default:
        {
            retVal = FVID2_EFAIL;
        }
        break;
    }

    if (FVID2_SOK == retVal)
    {
        /* Initialize the test parameters */
        appObj->driverId = FVID2_VPS_DISP_GRPX_DRV;
        appObj->driverInst = driverInst;
        appObj->inBuf = (UInt8 *) g_pAppGrpxBuffer[bufId];
        appObj->inFrmSize = GRPX_BUFFER_SIZE;
        appObj->numInBuf = 1u;
        appObj->windowWidth = GRPX_WIDTH;
        appObj->windowHeight = GRPX_HEIGHT;
        appObj->scanFormat = FVID2_SF_PROGRESSIVE;
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

        /* Create and start each of the GRPX planes */
        retVal = App_dispGrpxCreate(appObj);
    }

    return (retVal);
}

Int32 Grpx_stop(UInt32 linkId)
{
    Int32               retVal = FVID2_SOK;
    App_DisplayObj     *appObj = NULL;

    switch (linkId)
    {
        case SYSTEM_LINK_ID_GRPX_0:
        {
            appObj = &AppDispGrpxObjs[0u];
        }
        break;

        case SYSTEM_LINK_ID_GRPX_1:
        {
            appObj = &AppDispGrpxObjs[1u];
        }
        break;

        case SYSTEM_LINK_ID_GRPX_2:
        {
            appObj = &AppDispGrpxObjs[2u];
        }
        break;

        default:
        {
            retVal = FVID2_EFAIL;
        }
        break;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = App_dispGrpxDelete(appObj);
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
        FALSE,
    };
    Vps_GrpxParamsList      grpxPrmList;
    Vps_MultiWinParams      multiWinPrm;


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
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Opened instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif

    /* Set the required format */
    App_dispGrpxSetFormat(appObj, &fmt);
    retVal = FVID2_setFormat(appObj->fvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    App_dispGrpxSetParams(appObj, &grpxPrmList, &multiWinPrm);

    if(NUM_REGIONS>1)
    {
        Vps_LayoutId layoutId;

        retVal = FVID2_control(
                    appObj->fvidHandle,
                    IOCTL_VPS_CREATE_LAYOUT,
                    &multiWinPrm,
                    &layoutId
                    );
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error %d @ line %d\n",__FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

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
        fList = &appObj->frameList;
        App_dispGrpxUpdateFrmList(appObj, fList, fListCnt);

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
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Starting instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif
    retVal = FVID2_start(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Started instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif

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
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Stopping instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Stopped instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif

    /* Dequeue all the driver held buffers */
    fList = &appObj->frameList;
    while (1u)
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

    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: %s: Closed instance %d !!!\n",
        Clock_getTicks(), APP_NAME, appObj->driverInst);
    #endif

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
    fList = &appObj->frameList;

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
    App_dispGrpxUpdateFrmList(appObj, fList, 0);

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
                                      FVID2_FrameList *fList, UInt32 fListCnt)
{
    UInt32          index, regId;
    FVID2_Frame    *frame;

    /* Fill the next set of buffers */
    fList->numFrames = NUM_REGIONS;
    if (appObj->bufferIndex >= appObj->numInBuf)
    {
        appObj->bufferIndex = 0u;
    }
    index = appObj->bufferIndex;


    for(regId=0; regId<NUM_REGIONS; regId++)
    {
        frame = &appObj->frames[fListCnt*NUM_REGIONS + regId];

        fList->frames[regId] = frame;

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
    }

    appObj->bufferIndex++;
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
    if(GRPX_BPP==3)
    {
        fmt->dataFormat = FVID2_DF_RGB24_888;
        fmt->bpp = FVID2_BPP_BITS24;
    }
    else
    {
        fmt->dataFormat = FVID2_DF_ARGB32_8888;
        fmt->bpp = FVID2_BPP_BITS32;
    }
    fmt->reserved = NULL;

    return;
}



/**
 *  App_dispGrpxSetParams
 *  Sets the required Grpx parameters.
 */
static Void App_dispGrpxSetParams(App_DisplayObj *appObj,
                                  Vps_GrpxParamsList *grpxPrmList,
                                  Vps_MultiWinParams *multiWinPrm
                                  )
{
    UInt32 regId;
    Vps_GrpxRtParams *grpxRtPrms;
    Vps_WinFormat    *winFmt;

    grpxPrmList->numRegions = NUM_REGIONS;
    grpxPrmList->gParams = &appObj->grpxRtPrms[0];
    grpxPrmList->clutPtr = NULL;
    grpxPrmList->scParams = NULL;

    multiWinPrm->channelNum = 0;
    multiWinPrm->numWindows = grpxPrmList->numRegions;
    multiWinPrm->winFmt     = &appObj->winFormat[0];

    for(regId=0; regId<grpxPrmList->numRegions; regId++)
    {
        grpxRtPrms = &appObj->grpxRtPrms[regId];
        winFmt     = &appObj->winFormat[regId];

        grpxRtPrms->regId = regId;

        if(GRPX_BPP==3)
            grpxRtPrms->format = FVID2_DF_RGB24_888;
        else
            grpxRtPrms->format = FVID2_DF_ARGB32_8888;

        grpxRtPrms->pitch[FVID2_RGB_ADDR_IDX] = GRPX_BUFFER_PITCH;
        grpxRtPrms->rotation = VPS_MEM_0_ROTATION;
        grpxRtPrms->scParams = NULL;
        grpxRtPrms->stenPtr = NULL;
        grpxRtPrms->stenPitch = 0u;

        grpxRtPrms->regParams.regionWidth = appObj->windowWidth;
        if(regId==(grpxPrmList->numRegions-1))
           grpxRtPrms->regParams.regionHeight = appObj->windowHeight - REGION_HEIGHT*regId;
        else
           grpxRtPrms->regParams.regionHeight = REGION_HEIGHT;

        grpxRtPrms->regParams.regionPosX = 0;
        grpxRtPrms->regParams.regionPosY = REGION_HEIGHT*regId;

        grpxRtPrms->regParams.dispPriority = 1u;
        grpxRtPrms->regParams.firstRegion = FALSE;
        grpxRtPrms->regParams.lastRegion = FALSE;

        if(regId==0)
            grpxRtPrms->regParams.firstRegion = TRUE;
        if(regId==(grpxPrmList->numRegions-1))
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

        winFmt->winStartX = grpxRtPrms->regParams.regionPosX;
        winFmt->winStartY = grpxRtPrms->regParams.regionPosY;
        winFmt->winWidth  = grpxRtPrms->regParams.regionWidth;
        winFmt->winHeight = grpxRtPrms->regParams.regionHeight;
        winFmt->pitch[FVID2_RGB_ADDR_IDX]  = grpxRtPrms->pitch[FVID2_RGB_ADDR_IDX];
        winFmt->dataFormat = grpxRtPrms->format;

        winFmt->bpp = FVID2_BPP_BITS16;
        if(winFmt->dataFormat==FVID2_DF_RGB24_888)
            winFmt->bpp = FVID2_BPP_BITS24;
        if(winFmt->dataFormat==FVID2_DF_ARGB32_8888)
            winFmt->bpp = FVID2_BPP_BITS32;

        winFmt->priority = 0;

        #ifdef SYSTEM_DEBUG
        Vps_printf(" %d: %s: %d: Region %d: Size %d x %d, Position (%d, %d) !!!\n",
            Clock_getTicks(), APP_NAME, appObj->driverInst,
            grpxRtPrms->regId,
            grpxRtPrms->regParams.regionWidth,
            grpxRtPrms->regParams.regionHeight,
            grpxRtPrms->regParams.regionPosX,
            grpxRtPrms->regParams.regionPosY
        );

        #endif

        GT_assert( GT_DEFAULT_MASK,
            grpxRtPrms->regParams.regionPosY+grpxRtPrms->regParams.regionHeight <= appObj->windowHeight
            );

        GT_assert( GT_DEFAULT_MASK,
            grpxRtPrms->regParams.regionPosX+grpxRtPrms->regParams.regionWidth <= appObj->windowWidth
            );

    }
}



/**
 *  App_dispGrpxInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Int32 App_dispGrpxInitVariables(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              fCnt, regId;
    FVID2_Frame        *frame;
    FVID2_FrameList    *fList;

    appObj->bufferIndex = 0u;
    appObj->submittedFrames = 0u;
    appObj->completedFrames = 0u;

    /* Init frame list */
    memset(&appObj->frameList, 0u, sizeof (appObj->frameList));
    fList = &appObj->frameList;
    fList->numFrames = NUM_REGIONS;

    /* Init frame */
    memset(&appObj->frames, 0u, sizeof (appObj->frames));
    for (fCnt = 0u; fCnt < NUM_PRIMING; fCnt++)
    {
        for(regId=0; regId<NUM_REGIONS; regId++)
        {
            frame = &appObj->frames[fCnt*NUM_REGIONS + regId];
            frame->channelNum = regId;
            frame->fid = FVID2_FID_FRAME;
        }
    }

    /* Init error frame list */
    memset(&appObj->errFrameList, 0u, sizeof (appObj->errFrameList));

    return (retVal);
}


