/** ==================================================================
 *  @file   swMsLink_layouts.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/swMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/examples/common/iss/chains/links/swMsLink.h>

/* ===================================================================
 *  @func     SwMsLink_getLayoutInfo                                               
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
Int32 SwMsLink_getLayoutInfo(SwMsLink_LayoutParams * layoutParams,
                             SwMsLink_LayoutInfo * layoutInfo, UInt32 outPitch)
{
    UInt16 outWidth, outHeight;

    UInt16 widthAlign, heightAlign;

    SwMsLink_LayoutWinInfo *winInfo;

    UInt16 winId, row, col;

    widthAlign = VPS_BUFFER_ALIGNMENT / 2;
    heightAlign = 2;

    SwMsLink_getOutSize(layoutParams->outRes, &outWidth, &outHeight);

    layoutInfo->layoutId = layoutParams->outLayoutMode;

    switch (layoutParams->outLayoutMode)
    {
        case SYSTEM_LAYOUT_MODE_1CH:
            layoutInfo->numWin = 1;

            winId = 0;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->startX = 0;
            winInfo->startY = 0;
            winInfo->width = outWidth;
            winInfo->height = outHeight;
            winInfo->bypass = FALSE;
            winInfo->channelNum = layoutParams->win2ChMap[winId];
            break;

        case SYSTEM_LAYOUT_MODE_4CH:
            layoutInfo->numWin = 4;

            for (row = 0; row < 2; row++)
            {
                for (col = 0; col < 2; col++)
                {
                    winId = row * 2 + col;

                    winInfo = &layoutInfo->winInfo[winId];

                    winInfo->width = VpsUtils_align(outWidth / 2, widthAlign);
                    winInfo->height =
                        VpsUtils_align(outHeight / 2, heightAlign);
                    winInfo->startX = winInfo->width * col;
                    winInfo->startY = winInfo->height * row;
                    winInfo->bypass = FALSE;
                    winInfo->channelNum = layoutParams->win2ChMap[winId];
                }
            }

            break;

        case SYSTEM_LAYOUT_MODE_16CH:
            layoutInfo->numWin = 16;

            for (row = 0; row < 4; row++)
            {
                for (col = 0; col < 4; col++)
                {
                    winId = row * 4 + col;

                    winInfo = &layoutInfo->winInfo[winId];

                    winInfo->width = VpsUtils_align(outWidth / 4, widthAlign);
                    winInfo->height =
                        VpsUtils_align(outHeight / 4, heightAlign);
                    winInfo->startX = winInfo->width * col;
                    winInfo->startY = winInfo->height * row;
                    winInfo->bypass = TRUE;
                    winInfo->channelNum = layoutParams->win2ChMap[winId];
                }
            }

            break;

        case SYSTEM_LAYOUT_MODE_8CH:
            layoutInfo->numWin = 8;

            for (row = 0; row < 2; row++)
            {
                for (col = 0; col < 2; col++)
                {
                    winId = row * 2 + col;

                    winInfo = &layoutInfo->winInfo[winId];

                    winInfo->width =
                        VpsUtils_align((outWidth * 2) / 5, widthAlign);
                    winInfo->height =
                        VpsUtils_align(outHeight / 2, heightAlign);
                    winInfo->startX = winInfo->width * col;
                    winInfo->startY = winInfo->height * row;
                    winInfo->bypass = FALSE;
                    winInfo->channelNum = layoutParams->win2ChMap[winId];
                }
            }

            for (row = 0; row < 4; row++)
            {
                winId = 4 + row;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width = layoutInfo->winInfo[0].width / 2;
                winInfo->height = layoutInfo->winInfo[0].height / 2;
                winInfo->startX = layoutInfo->winInfo[0].width * 2;
                winInfo->startY = winInfo->height * row;
                winInfo->bypass = FALSE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];
            }
            break;

        case SYSTEM_LAYOUT_MODE_5CH_PLUS_1CH:
            layoutInfo->numWin = 6;

            winId = 0;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width = VpsUtils_align((outWidth * 2) / 3, widthAlign);
            winInfo->height = VpsUtils_align((outHeight * 2) / 3, heightAlign);
            winInfo->startX = 0;
            winInfo->startY = 0;
            winInfo->bypass = FALSE;
            winInfo->channelNum = layoutParams->win2ChMap[winId];

            for (row = 0; row < 2; row++)
            {
                winId = 1 + row;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width =
                    VpsUtils_align(layoutInfo->winInfo[0].width / 2,
                                   widthAlign);
                winInfo->height =
                    VpsUtils_align(layoutInfo->winInfo[0].height / 2,
                                   heightAlign);
                winInfo->startX = layoutInfo->winInfo[0].width;
                winInfo->startY = winInfo->height * row;
                winInfo->bypass = TRUE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];

            }

            for (col = 0; col < 3; col++)
            {
                winId = 3 + col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width =
                    VpsUtils_align(layoutInfo->winInfo[0].width / 2,
                                   widthAlign);
                winInfo->height =
                    VpsUtils_align(layoutInfo->winInfo[0].height / 2,
                                   heightAlign);
                winInfo->startX = winInfo->width * col;
                winInfo->startY = layoutInfo->winInfo[0].height;
                winInfo->bypass = TRUE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];
            }
            break;

        case SYSTEM_LAYOUT_MODE_7CH_PLUS_1CH:
            layoutInfo->numWin = 8;

            winId = 0;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width = VpsUtils_align((outWidth) / 4, widthAlign) * 3;
            winInfo->height = VpsUtils_align((outHeight) / 4, heightAlign) * 3;
            winInfo->startX = 0;
            winInfo->startY = 0;
            winInfo->bypass = FALSE;
            winInfo->channelNum = layoutParams->win2ChMap[winId];

            for (row = 0; row < 3; row++)
            {
                winId = 1 + row;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width =
                    VpsUtils_align(outWidth - layoutInfo->winInfo[0].width,
                                   widthAlign);
                winInfo->height =
                    VpsUtils_align(layoutInfo->winInfo[0].height / 3,
                                   heightAlign);
                winInfo->startX = layoutInfo->winInfo[0].width;
                winInfo->startY = winInfo->height * row;
                winInfo->bypass = TRUE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];

            }

            for (col = 0; col < 4; col++)
            {
                winId = 4 + col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width = VpsUtils_align(outWidth / 4, widthAlign);
                winInfo->height =
                    VpsUtils_align(outHeight - layoutInfo->winInfo[0].height,
                                   heightAlign);
                winInfo->startX = winInfo->width * col;
                winInfo->startY = layoutInfo->winInfo[0].height;
                winInfo->bypass = TRUE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];
            }
            break;

        case SYSTEM_LAYOUT_MODE_1CH_PLUS_2CH_PIP:
            layoutInfo->numWin = 3;

            winId = 0;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->startX = 0;
            winInfo->startY = 0;
            winInfo->width = outWidth;
            winInfo->height = outHeight;
            winInfo->bypass = FALSE;
            winInfo->channelNum = layoutParams->win2ChMap[winId];

            for (col = 0; col < 2; col++)
            {
                winId = 1 + col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width = VpsUtils_align(outWidth / 4, widthAlign);
                winInfo->height = VpsUtils_align(outHeight / 4, heightAlign);

                if (col == 0)
                {
                    winInfo->startX = VpsUtils_align(outWidth / 20, widthAlign);
                }
                else
                {
                    winInfo->startX =
                        VpsUtils_align(outWidth - winInfo->width -
                                       outWidth / 20, widthAlign);
                }

                winInfo->startY =
                    VpsUtils_align(outHeight - winInfo->height - outHeight / 20,
                                   heightAlign);
                winInfo->bypass = TRUE;
                winInfo->channelNum = layoutParams->win2ChMap[winId];
            }
            break;

    }

    /* if a channel repeats in different window, then mark the repeated
     * window as having invalid CH */
    memset(layoutInfo->ch2WinMap, SYSTEM_SW_MS_INVALID_ID,
           sizeof(layoutInfo->ch2WinMap));
    for (winId = 0; winId < layoutInfo->numWin; winId++)
    {
        winInfo = &layoutInfo->winInfo[winId];

        /* assuming YUV422I data */
        winInfo->bufAddrOffset =
            winInfo->startY * outPitch + winInfo->startX * 2;

        if (winInfo->channelNum > SYSTEM_SW_MS_MAX_CH_ID)
        {
            winInfo->channelNum = SYSTEM_SW_MS_INVALID_ID;
        }
        if (winInfo->channelNum != SYSTEM_SW_MS_INVALID_ID)
        {
            if (layoutInfo->ch2WinMap[winInfo->channelNum] ==
                SYSTEM_SW_MS_INVALID_ID)
                layoutInfo->ch2WinMap[winInfo->channelNum] = winId;
            else
                winInfo->channelNum = SYSTEM_SW_MS_INVALID_ID;
        }
    }

    return 0;
}

/* ===================================================================
 *  @func     SwMsLink_getOutSize                                               
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
Int32 SwMsLink_getOutSize(UInt32 outRes, UInt16 * width, UInt16 * height)
{
    switch (outRes)
    {
        case SYSTEM_DISPLAY_RES_MAX:
            *width = 1920;
            *height = 1200;
            break;

        case SYSTEM_DISPLAY_RES_720P60:
            *width = 1280;
            *height = 720;
            break;

        default:
        case SYSTEM_DISPLAY_RES_1080I60:
        case SYSTEM_DISPLAY_RES_1080P60:
        case SYSTEM_DISPLAY_RES_1080P30:
            *width = 1920;
            *height = 1080;
            break;

        case SYSTEM_DISPLAY_RES_NTSC:
            *width = 720;
            *height = 480;
            break;

        case SYSTEM_DISPLAY_RES_PAL:
            *width = 720;
            *height = 576;
            break;

    }
    return 0;
}
