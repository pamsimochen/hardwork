/** ==================================================================
 *  @file   testPlatform.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/tvs_mtis_est/test/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                           DSPS R&D Center
 *                     Video and Image Processing
 *
 *         Copyright (c) 2004 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *                    TI INTERNAL - TI PROPRIETARY
 *
 *
 *  Contact: Aziz Umit Batur     <batur@ti.com>
 *
 *=======================================================================
 *
 *  File: testPlatform.c
 *
 *=======================================================================
 *
 *  Revision 4.5 (2-April-2009)
 *
 =======================================================================*/

/* Test platform functions are intended for the PC platform */

#include <WTSD_DucatiMMSW/alg/tvs_mtis_est/msp_tvs_mtis_est.h>
#include<WTSD_DucatiMMSW/platform/osal/timm_osal_trace.h>
#include "../inc/globalDefs.h"
// #include "../../inc/TI_VidStab.h"
#include "../inc/testPlatform.h"
#include "../../debugUtils.h"
/* 
 * #ifdef IS_WINDOWS #include "win.h" #endif
 * 
 * #ifdef IS_WINDOWS void doFrameRate(TPstruct* tp); #endif */

/*=======================================================================
 *
 * Name:        computeBoundarySignals()
 *
 * Description: Simulates the BSC computation
 *
 * Input:
 *   vs:        Handle for the VS structure
 *
 * Returns:
 *
 * Effects:
 *              Updates the boundary signals
 *
 =======================================================================*/

/* ===================================================================
 *  @func     computeBoundarySignals                                               
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
void computeBoundarySignals(TPstruct * tp)
{
    Word32 vp, hp, horzInd, vertInd, nbs, vertLoc, horzLoc;

    // Byte str1[500];
    Byte *currFrm = tp->frmBuffer;                         // Pointer to
                                                           // current frame
    Byte *bscBufferCurr = tp->bscBufferCurr;

    Byte temp;

    uWord16 *bounSignalV, *bounSignalH;

    bounSignalV = (uWord16 *) bscBufferCurr;
    bounSignalH =
        ((uWord16 *) bscBufferCurr) + (tp->bsc_row_vct * tp->bsc_row_vnum);

    for (hp = 0; hp < (tp->bsc_col_vct * tp->bsc_col_hnum); hp++)
    {
        bounSignalH[hp] = 0;
    }

    for (nbs = 0; nbs < tp->bsc_col_vct; nbs++)
    {
        vertLoc =
            tp->bsc_col_vpos + (nbs * tp->bsc_col_vnum * tp->bsc_col_vskip);
        for (vp = vertLoc;
             vp < (vertLoc + (tp->bsc_col_vnum * tp->bsc_col_vskip));
             vp += tp->bsc_col_vskip)
        {
            horzInd = 0;
            for (hp = tp->bsc_col_hpos;
                 hp <
                 (tp->bsc_col_hpos + (tp->bsc_col_hnum * tp->bsc_col_hskip));
                 hp += tp->bsc_col_hskip)
            {
                bounSignalH[(nbs * tp->bsc_col_hnum) + horzInd] +=
                    currFrm[(vp * tp->horzImgSizeBeforeBSC) + hp];
                horzInd++;
            }
        }
    }

    for (hp = 0; hp < (tp->bsc_row_vct * tp->bsc_row_vnum); hp++)
    {
        bounSignalV[hp] = 0;
    }

    for (nbs = 0; nbs < tp->bsc_row_vct; nbs++)
    {
        horzLoc =
            tp->bsc_row_hpos + (nbs * tp->bsc_row_hnum * tp->bsc_row_hskip);
        for (hp = horzLoc;
             hp < (horzLoc + (tp->bsc_row_hnum * tp->bsc_row_hskip));
             hp += tp->bsc_row_hskip)
        {
            vertInd = 0;
            for (vp = tp->bsc_row_vpos;
                 vp <
                 (tp->bsc_row_vpos + (tp->bsc_row_vnum * tp->bsc_row_vskip));
                 vp += tp->bsc_row_vskip)
            {
                temp = currFrm[(vp * tp->horzImgSizeBeforeBSC) + hp];
                bounSignalV[(nbs * tp->bsc_row_vnum) + vertInd] +=
                    (uWord16) temp;
                vertInd++;
            }
        }
    }

#ifdef WRITE_DEBUG
    // strcpy(str1, OUTPUT_PATH);
    // strcat(str1, "bounSignal.usf");
    writeUShortArrayBinary("../data/bounSignal.usf", bounSignalH,
                           tp->bsc_col_hnum * tp->bsc_col_vct, 1,
                           (Word16) (tp->frameNo == 0));
    writeUShortArrayBinary("../data/bounSignal.usf", bounSignalV,
                           tp->bsc_row_vnum * tp->bsc_row_vct, 0,
                           (Word16) (tp->frameNo == 0));
#endif

    return;
}

/*============================================================================
 *
 * Name: VS_motionCompensate()
 *
 * Description:    Crops the stabilized window
 *
 * Input:
 *    inpFrmBuff   Pointer to input frame
 *    outFrmBuff   Pointer to output frame
 *    inpImgSizeV  Number of rows of the input frame
 *    inpImgSizeH  Number of columns of the input frame
 *    bounLines    Number of rows at the top and bottom of the frame for
 *				   motion compensation
 *    bounPels     Number of columns at the left and right edges of the
 *                 frame for motion compensation
 *    ROy          Vertical coordinate of the upper left corner of the
 *                 subwindow that will be cropped
 *    ROx          Horizontal coordinate of the upper left corner of the
 *                 subwindow that will be cropped
 *
 *
 * Comments:       inpImgSizeH should be an EVEN NUMBER
 *                 Input frame should be in YCbCr422 format (YCbYCrYCbYCr...)
 *                 ROx and ROy should be at pel resolution
 ============================================================================*/

/* ===================================================================
 *  @func     VS_motionCompensate                                               
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
void VS_motionCompensate(Byte * inpFrmBuff, Byte * outFrmBuff,
                         Word16 inpImgSizeV, Word16 inpImgSizeH,
                         Word16 bounLines, Word16 bounPels, Word16 ROy,
                         Word16 ROx)
{
    Byte *p_inBuff, *p_outBuff;

    Word16 ROxIsEven, tmp, vp, hp;

    // Compute the size of the cropped image
    Word16 outImgSizeV = inpImgSizeV - 2 * bounLines;

    Word16 outImgSizeH = inpImgSizeH - 2 * bounPels;

    // Initialize the pointers to the input and output buffers
    p_inBuff = inpFrmBuff + ROy * 2 * inpImgSizeH + ROx * 2;
    p_outBuff = outFrmBuff;

    // Check whether ROx is even or odd
    if (((ROx >> 1) << 1) == ROx)
        ROxIsEven = 1;
    else
        ROxIsEven = 0;

    // Crop the subwindow when ROx is even
    if (ROxIsEven == 1)
    {

        for (vp = 0; vp < outImgSizeV; vp++)
        {
            for (hp = 0; hp < outImgSizeH; hp++)
            {

                // Copy the Y value
                *p_outBuff++ = *p_inBuff++;

                // Copy the Cb/Cr value
                *p_outBuff++ = *p_inBuff++;
            }
            p_inBuff += inpImgSizeH * 2 - 2 * outImgSizeH;
        }
    }

    // Crop the subwindow when ROx is odd (In this case, interpolate Cb and
    // Cr)
    if (ROxIsEven == 0)
    {

        for (vp = 0; vp < outImgSizeV; vp++)
        {
            for (hp = 0; hp < outImgSizeH; hp++)
            {

                // Copy the Y value
                *p_outBuff++ = *p_inBuff++;

                // Interpolate the Cb/Cr value
                tmp = *(p_inBuff - 2);
                tmp += *(p_inBuff + 2);
                *p_outBuff++ = (Byte) (tmp >> 1);
                p_inBuff++;
            }
            p_inBuff += inpImgSizeH * 2 - 2 * outImgSizeH;
        }
    }

    return;
}

/*=======================================================================
 *
 * Name: VS_compensateMotion()
 *
 * Description: Reads out the stabilized window and stores it.
 *              The readout location is assumed to be at quarter-pel
 *              resolution.
 *
 * Input:
 *   vsOP: Pointer to the frame output parameters structure of VS
 *   tp:   Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     VS_compensateMotion                                               
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
void VS_compensateMotion(MSP_TVS_MTIS_FRAME_OUT * vsOP, TPstruct * tp)
{
    Byte *tmp;

    tp->ROx = vsOP->sROh;
    tp->ROy = vsOP->sROv;

    tp->MVscaleY[tp->frameNo] = vsOP->usVertStatus;
    tp->MVscaleX[tp->frameNo] = vsOP->usHorzStatus;

    if (tp->MCres == 2)
    {
        tp->ROxHist[tp->frameNo] = vsOP->sROh - 4 * tp->bounPels;
        tp->ROyHist[tp->frameNo] = vsOP->sROv - 4 * tp->bounLines;
    }
    else if (tp->MCres == 1)
    {
        tp->ROxHist[tp->frameNo] = vsOP->sROh - 2 * tp->bounPels;
        tp->ROyHist[tp->frameNo] = vsOP->sROv - 2 * tp->bounLines;
    }
    else
    {
        tp->ROxHist[tp->frameNo] = vsOP->sROh - tp->bounPels;
        tp->ROyHist[tp->frameNo] = vsOP->sROv - tp->bounLines;
    }

    /* if (tp->frameNo>0) { if (tp->MCres==2) { tp->ROx =
     * tp->ROxHist[tp->frameNo-1] + 4*tp->bounPels; tp->ROy =
     * tp->ROyHist[tp->frameNo-1] + 4*tp->bounLines; } else if (tp->MCres==1) 
     * { tp->ROx = tp->ROxHist[tp->frameNo-1] + 2*tp->bounPels; tp->ROy =
     * tp->ROyHist[tp->frameNo-1] + 2*tp->bounLines; } else { tp->ROx =
     * tp->ROxHist[tp->frameNo-1] + tp->bounPels; tp->ROy =
     * tp->ROyHist[tp->frameNo-1] + tp->bounLines; } } */

    /* if (tp->frameNo>1) { if (tp->MCres==2) { tp->ROx =
     * tp->ROxHist[tp->frameNo-1] + 4*tp->bounPels; tp->ROy =
     * tp->ROyHist[tp->frameNo-1] + 4*tp->bounLines; } else if (tp->MCres==1) 
     * { tp->ROx = tp->ROxHist[tp->frameNo-1] + 2*tp->bounPels; tp->ROy =
     * tp->ROyHist[tp->frameNo-1] + 2*tp->bounLines; } else { tp->ROx =
     * 2*tp->ROxHist[tp->frameNo-1] - tp->ROxHist[tp->frameNo-2] +
     * tp->bounPels; tp->ROy = 2*tp->ROyHist[tp->frameNo-1] -
     * tp->ROyHist[tp->frameNo-2] + tp->bounLines; } } */

    // storeStabilizedFrame(tp,0);

    // Switch the frame buffers
    tmp = tp->frmBuffer;
    tp->frmBuffer = tp->frmBufferPrev;
    tp->frmBufferPrev = tmp;

    // Switch the bsc buffers
    tmp = tp->bscBufferCurr;
    tp->bscBufferCurr = tp->bscBufferPrev;
    tp->bscBufferPrev = tmp;

    return;
}

/*=======================================================================
 *
 * Name: InitTP()
 *
 * Description: Initializes the test platform
 *
 * Input:
 *   vsCP: Pointer to the VS input creation parameters structure
 *   tp:   Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     InitTP                                               
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
void InitTP(MSP_HANDLE hMSP, MSP_TVS_MTIS_CREATE_PARAM * vsCP, TPstruct * tp)
// Initialize the test platform (assumed to be the PC platform)
{
    unsigned long vsCO;

    MSP_TVS_MTIS_BSC_FRAME_PARAMS stFrameBSCParams;

    tp->lines = vsCP->sVertImgSizeBeforeBSC;
    tp->pels = vsCP->sHorzImgSizeBeforeBSC;

    tp->ROpels = tp->pels - 2 * vsCP->sBounPels;
    tp->ROlines = tp->lines - 2 * vsCP->sBounLines;
    tp->bounPels = vsCP->sBounPels;
    tp->bounLines = vsCP->sBounLines;
    tp->MCres = 0;

    tp->hpLines = 2 * tp->lines - 1;
    tp->hpPels = 2 * tp->pels - 1;
    tp->qpLines = 2 * tp->hpLines - 1;
    tp->qpPels = 2 * tp->hpPels - 1;

    tp->frameNo = tp->numFirstFrame;

    if (tp->inputType & FILE_IO)
    {
        // Open the input sequence file for reading
        if ((tp->inSeqFile = fopen(tp->inSeqFileName, "rb")) == NULL)
        {
#ifndef NO_PRINTF
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                               "\nError when opening file: %s",
                               tp->inSeqFileName);
#endif
            exit(-1);
        }
        tp->inSeqFilePos = 0;
    }

    if (tp->outputType & FILE_IO)
    {
        // Delete the output sequence file before writing
        if ((tp->outSeqFile = fopen(tp->outSeqFileName, "wb")) == NULL)
        {
#ifndef NO_PRINTF
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                               "\nError when deleting file: %s",
                               tp->outSeqFileName);
#endif
            exit(-1);
        }
        fclose(tp->outSeqFile);
    }
    /* 
     * #ifdef IS_WINDOWS if (tp->outputType & WIN_IO) { tp->targetTime =
     * timeGetTime() + 1000/tp->frameRate; //Adds (one frame) startup time if 
     * (tp->outputType & DISP_IN_ORG_FRAME) initDisplay(tp->pels,tp->lines);
     * else initDisplay(tp->ROpels,tp->ROlines); } #endif //IS_WINDOWS */
    stFrameBSCParams.bCurrentSettings = MSP_FALSE;
    MSP_query(hMSP, MSP_TVS_MTIS_QUERY_BSCPARAMS, (MSP_PTR) & stFrameBSCParams);
    /* tp->bsc_col_hnum = vsCO->bsc_col_hnum; tp->bsc_row_vct =
     * vsCO->bsc_row_vct; tp->bsc_row_vpos = vsCO->bsc_row_vpos;
     * tp->bsc_row_vnum = vsCO->bsc_row_vnum; tp->bsc_row_vskip =
     * vsCO->bsc_row_vskip; tp->bsc_row_hpos = vsCO->bsc_row_hpos;
     * tp->bsc_row_hnum = vsCO->bsc_row_hnum; tp->bsc_row_hskip =
     * vsCO->bsc_row_hskip; tp->bsc_row_shf = vsCO->bsc_row_shf;
     * tp->bsc_col_vct = vsCO->bsc_col_vct; tp->bsc_col_vpos =
     * vsCO->bsc_col_vpos; tp->bsc_col_vnum = vsCO->bsc_col_vnum;
     * tp->bsc_col_vskip = vsCO->bsc_col_vskip; tp->bsc_col_hpos =
     * vsCO->bsc_col_hpos; tp->bsc_col_hnum = vsCO->bsc_col_hnum;
     * tp->bsc_col_hskip = vsCO->bsc_col_hskip; tp->bsc_col_shf =
     * vsCO->bsc_col_shf; */
    TVS_MTIS_Memcpy(&tp->bsc_row_vct, (MSP_PTR) & stFrameBSCParams.stBSCParams,
                    sizeof(MSP_TVS_MTIS_BSC_PARAMS));

    tp->horzImgSizeBeforeBSC = vsCP->sHorzImgSizeBeforeBSC;
    tp->vertImgSizeBeforeBSC = vsCP->sVertImgSizeBeforeBSC;

    return;
}

/* ===================================================================
 *  @func     CloseTP                                               
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
void CloseTP(TPstruct * tp)
// Close the test platform
{
    if ((tp->inputType & FILE_IO) && tp->inSeqFile != NULL)
    {
        fclose(tp->inSeqFile);
    }
    return;
}

/*=======================================================================
 *
 * Name: getNewFrame()
 *
 * Description: Reads a new frame from a file
 *
 * Input:
 *   tp: Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *   Switches the current and previuos frame pointers in test platform
 *   structure.
 *
 =======================================================================*/

/* ===================================================================
 *  @func     getNewFrame                                               
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
void getNewFrame(TPstruct * tp)
{
    int i, j;

    Byte *p_currFrm, *p_interlacedFrame;

    if (tp->inputType & FILE_IO)
    {
        if (tp->inputType & INTERLACED)
        {
            ReadImage(tp, tp->interlacedFrame, tp->lines * tp->pels * 4);

            p_interlacedFrame = tp->interlacedFrame;
            p_currFrm = tp->frmBuffer;
            for (i = 0; i < tp->lines; i++)
            {
                for (j = 0; j < tp->pels; j++)
                {                                          // use pel ind
                    *p_currFrm++ = *p_interlacedFrame;
                    p_interlacedFrame += 2;
                }
                p_interlacedFrame += 2 * tp->pels;
            }
        }
        else
        {

            // tp->frameNo = 0;

            ReadImage(tp, tp->frmBuffer, tp->lines * tp->pels);

            /* for (i=0; i<tp->lines; i++) { for (j=0; j<tp->pels; j++) {
             * tp->frmBuffer[i*tp->pels+j] = 1; } } */

        }
    }

    return;
}

/*=======================================================================
 *
 * Name: storeStabilizedFrame()
 *
 * Description: Computes the stabilized frame, and stores it in a file or
 *              displays it on screen.
 *
 * Input:
 *   tp:         Handle for the test platform
 *   doNotWrite: 1->do not write to file
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     storeStabilizedFrame                                               
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
void storeStabilizedFrame(TPstruct * tp, short doNotWrite)
{
    Word32 i, j;

    Byte *p_stabFrm, *p_prevFrm, *currFrm, *p_currFrm, *pN_currFrm;

    Word16 ROx_tmp, ROy_tmp;

    Word16 ROx, ROy, pels, lines, ROlines, ROpels;

    Word32 pixPerFrame;

    Word16 pel0, line0, pel1, line1, pel2, line2;

    if ((tp->lines > MAX_FRAME_SIZE_FOR_DISPLAY_V) ||
        (tp->pels > MAX_FRAME_SIZE_FOR_DISPLAY_H))
    {
#ifndef	NO_PRINTF
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\nError: Frame is too large to display!\n");
#endif
        exit(0);
    }

    line0 = tp->lines / 2;
    pel0 = tp->pels / 2;
    line1 = 2 * line0 - 1;
    pel1 = 2 * pel0 - 1;
    line2 = 2 * line1 - 1;
    pel2 = 2 * pel1 - 1;

    if (tp->MCres == 1)
    {
        interpolateFrm(tp->frmBuffer, tp->HPintFrame, tp->lines, tp->pels);
        if (tp->outputType & YUV420)
        {
            interpolateFrm(tp->frmBuffer + tp->lines * tp->pels,
                           tp->HPintFrame + tp->hpLines * tp->hpPels, line0,
                           pel0);
            interpolateFrm(tp->frmBuffer + tp->lines * tp->pels + line0 * pel0,
                           tp->HPintFrame + tp->hpLines * tp->hpPels +
                           line1 * pel1, line0, pel0);
        }
    }

    if (tp->MCres == 2)
    {
        interpolateFrm(tp->frmBuffer, tp->HPintFrame, tp->lines, tp->pels);
        interpolateFrm(tp->HPintFrame, tp->QPintFrame, tp->hpLines, tp->hpPels);
        if (tp->outputType & YUV420)
        {
            interpolateFrm(tp->frmBuffer + tp->lines * tp->pels,
                           tp->HPintFrame + tp->hpLines * tp->hpPels, line0,
                           pel0);
            interpolateFrm(tp->frmBuffer + tp->lines * tp->pels + line0 * pel0,
                           tp->HPintFrame + tp->hpLines * tp->hpPels +
                           line1 * pel1, line0, pel0);
            interpolateFrm(tp->HPintFrame + tp->hpLines * tp->hpPels,
                           tp->QPintFrame + tp->qpLines * tp->qpPels, line1,
                           pel1);
            interpolateFrm(tp->HPintFrame + tp->hpLines * tp->hpPels +
                           line1 * pel1,
                           tp->QPintFrame + tp->qpLines * tp->qpPels +
                           line2 * pel2, line1, pel1);
        }
    }

    if ((tp->outputType & FILE_IO) || (tp->outputType & WIN_IO))
    {
        if (tp->outputType & DISP_IN_ORG_FRAME)
        {
            if (tp->outputType & GRAYSCALE)
            {
                p_stabFrm = tp->stabFrm;

                // Copy a block of samples
                p_prevFrm = tp->frmBuffer;
                for (i = 0; i < tp->pels * tp->lines; i++)
                {
                    *p_stabFrm++ = *p_prevFrm++;
                }

                if (tp->MCres == 2)
                {
                    ROy_tmp = tp->ROy / 4;
                    ROx_tmp = tp->ROx / 4;
                }
                else if (tp->MCres == 1)
                {
                    ROy_tmp = tp->ROy / 2;
                    ROx_tmp = tp->ROx / 2;
                }
                else
                {
                    ROy_tmp = tp->ROy;
                    ROx_tmp = tp->ROx;
                }

                // Mark the window
                p_stabFrm = tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp;
                for (i = 0; i < tp->ROpels; i++)
                {
                    *p_stabFrm++ = 220;
                }
                p_stabFrm =
                    tp->stabFrm + (ROy_tmp + tp->ROlines - 1) * tp->pels +
                    ROx_tmp;
                for (i = 0; i < tp->ROpels; i++)
                {
                    *p_stabFrm++ = 220;
                }
                p_stabFrm = tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp;
                for (i = 0; i < tp->ROlines; i++)
                {
                    *p_stabFrm = 220;
                    p_stabFrm += tp->pels;
                }
                p_stabFrm =
                    tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp + (tp->ROpels -
                                                                  1);
                for (i = 0; i < tp->ROlines; i++)
                {
                    *p_stabFrm = 220;
                    p_stabFrm += tp->pels;
                }
            }
            else if (tp->outputType & YUV420)
            {
                p_stabFrm = tp->stabFrm;

                // Copy a block of samples
                p_prevFrm = tp->frmBuffer;
                for (i = 0; i < tp->pels * tp->lines * 3 / 2; i++)
                {
                    *p_stabFrm++ = *p_prevFrm++;
                }

                if (tp->MCres == 2)
                {
                    ROy_tmp = tp->ROy / 4;
                    ROx_tmp = tp->ROx / 4;
                }
                else if (tp->MCres == 1)
                {
                    ROy_tmp = tp->ROy / 2;
                    ROx_tmp = tp->ROx / 2;
                }
                else
                {
                    ROy_tmp = tp->ROy;
                    ROx_tmp = tp->ROx;
                }

                // Mark the window
                p_stabFrm = tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp;
                for (i = 0; i < tp->ROpels; i++)
                {
                    *p_stabFrm++ = 220;
                }
                p_stabFrm =
                    tp->stabFrm + (ROy_tmp + tp->ROlines - 1) * tp->pels +
                    ROx_tmp;
                for (i = 0; i < tp->ROpels; i++)
                {
                    *p_stabFrm++ = 220;
                }
                p_stabFrm = tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp;
                for (i = 0; i < tp->ROlines; i++)
                {
                    *p_stabFrm = 220;
                    p_stabFrm += tp->pels;
                }
                p_stabFrm =
                    tp->stabFrm + ROy_tmp * tp->pels + ROx_tmp + (tp->ROpels -
                                                                  1);
                for (i = 0; i < tp->ROlines; i++)
                {
                    *p_stabFrm = 220;
                    p_stabFrm += tp->pels;
                }
            }
        }
        else
        {
            if (tp->outputType & GRAYSCALE)
            {
                // Copy the samples to the stabFrm
                p_stabFrm = tp->stabFrm;
                if (tp->MCres == 2)
                {
                    p_prevFrm =
                        tp->QPintFrame + tp->ROy * (tp->qpPels) + tp->ROx;
                    for (j = 0; j < tp->ROlines; j++)
                    {
                        for (i = 0; i < tp->ROpels; i++)
                        {
                            *p_stabFrm++ = *p_prevFrm;
                            p_prevFrm += 4;
                        }
                        p_prevFrm += 4 * (4 * tp->pels - 3) - tp->ROpels * 4;
                    }
                }
                else if (tp->MCres == 1)
                {
                    p_prevFrm =
                        tp->HPintFrame + tp->ROy * (tp->hpPels) + tp->ROx;
                    for (j = 0; j < tp->ROlines; j++)
                    {
                        for (i = 0; i < tp->ROpels; i++)
                        {
                            *p_stabFrm++ = *p_prevFrm;
                            p_prevFrm += 2;
                        }
                        p_prevFrm += 4 * tp->pels - 2 - tp->ROpels * 2;
                    }
                }
                else
                {
                    p_prevFrm = tp->frmBuffer + tp->ROy * tp->pels + tp->ROx;
                    for (j = 0; j < tp->ROlines; j++)
                    {
                        for (i = 0; i < tp->ROpels; i++)
                        {
                            *p_stabFrm++ = *p_prevFrm++;
                        }
                        p_prevFrm += tp->pels - tp->ROpels;
                    }
                }
            }
            else if (tp->outputType & YUV420)
            {
                p_stabFrm = tp->stabFrm;
                if (tp->inputType & GRAYSCALE)
                {
                    if (tp->MCres == 0)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->pels;
                        lines = tp->lines;
                        currFrm = tp->frmBuffer;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm++;
                            }
                            p_currFrm += pels - ROpels;
                        }

                        p_currFrm =
                            currFrm + pixPerFrame + (ROy / 2) * (pels / 2) +
                            ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }

                        p_currFrm =
                            currFrm + pixPerFrame + pixPerFrame / 4 +
                            (ROy / 2) * (pels / 2) + ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }
                    }
                    else if (tp->MCres == 1)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->hpPels;
                        lines = tp->hpLines;
                        currFrm = tp->HPintFrame;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm;
                                p_currFrm += 2;
                            }
                            p_currFrm += 2 * pels - 2 * ROpels;
                        }

                        p_currFrm =
                            currFrm + pixPerFrame + (ROy / 2) * (pels / 2) +
                            ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }

                        p_currFrm =
                            currFrm + pixPerFrame + pixPerFrame / 4 +
                            (ROy / 2) * (pels / 2) + ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }
                    }
                    else if (tp->MCres == 2)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->qpPels;
                        lines = tp->qpLines;
                        currFrm = tp->QPintFrame;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm;
                                p_currFrm += 4;
                            }
                            p_currFrm += 4 * pels - 4 * ROpels;
                        }
                        p_currFrm =
                            currFrm + pixPerFrame + (ROy / 2) * (pels / 2) +
                            ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }

                        p_currFrm =
                            currFrm + pixPerFrame + pixPerFrame / 4 +
                            (ROy / 2) * (pels / 2) + ROx / 2;
                        for (j = 0; j < ROlines / 2; j++)
                        {
                            for (i = 0; i < ROpels / 2; i++)
                            {
                                *p_stabFrm++ = 128;
                            }
                            p_currFrm += pels / 2 - ROpels / 2;
                        }
                    }
                }
                else
                {
                    if (tp->MCres == 0)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->pels;
                        lines = tp->lines;
                        currFrm = tp->frmBuffer;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm++;
                            }
                            p_currFrm += pels - ROpels;
                        }

                        // Copying Cb and Cr samples requires some
                        // interpolation depending on ROx and ROy

                        if (ROx & 0x0001)
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx odd, ROy odd
                                p_currFrm =
                                    currFrm + pixPerFrame +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * (pels / 2) +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm++;
                                        pN_currFrm++;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2 + 1) * (pels / 2) + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm++;
                                        pN_currFrm++;
                                        // increment?
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }
                            }
                            else
                            {
                                // ROx odd, ROy even
                                p_currFrm =
                                    currFrm + pixPerFrame +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame +
                                    (ROy / 2) * (pels / 2) + ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm++ + *pN_currFrm++) / 2;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2) * (pels / 2) + ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm++ + *pN_currFrm++) / 2;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }

                            }
                        }
                        else
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx even, ROy odd

                                p_currFrm =
                                    currFrm + pixPerFrame +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * (pels / 2) +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm++) +
                                             (*pN_currFrm++)) / 2;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2 + 1) * (pels / 2) + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm++) +
                                             (*pN_currFrm++)) / 2;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                    pN_currFrm += pels / 2 - ROpels / 2;
                                }
                            }
                            else
                            {
                                // ROx even, ROy even

                                p_currFrm =
                                    currFrm + pixPerFrame +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm++;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + pixPerFrame / 4 +
                                    (ROy / 2) * (pels / 2) + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm++;
                                    }
                                    p_currFrm += pels / 2 - ROpels / 2;
                                }
                            }
                        }
                    }
                    else if (tp->MCres == 1)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->hpPels;
                        lines = tp->hpLines;
                        currFrm = tp->HPintFrame;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm;
                                p_currFrm += 2;
                            }
                            p_currFrm += 2 * pels - 2 * ROpels;
                        }

                        // Copying Cb and Cr samples requires some
                        // interpolation depending on ROx and ROy

                        if (ROx & 0x0001)
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx odd, ROy odd
                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel1 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * pel1 +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2) * pel1 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2 + 1) * pel1 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }
                            }
                            else
                            {
                                // ROx odd, ROy even
                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel1 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel1 +
                                    ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm) / 2;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2) * pel1 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2) * pel1 + ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm) / 2;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }

                            }
                        }
                        else
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx even, ROy odd

                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel1 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * pel1 +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm) + (*pN_currFrm)) / 2;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2) * pel1 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2 + 1) * pel1 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm) + (*pN_currFrm)) / 2;
                                        p_currFrm += 2;
                                        pN_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                    pN_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }
                            }
                            else
                            {
                                // ROx even, ROy even

                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel1 +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm;
                                        p_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line1 * pel1 +
                                    (ROy / 2) * pel1 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm;
                                        p_currFrm += 2;
                                    }
                                    p_currFrm += 2 * pel1 - 2 * (ROpels / 2);
                                }
                            }
                        }
                    }
                    else if (tp->MCres == 2)
                    {
                        ROx = tp->ROx;
                        ROy = tp->ROy;
                        pels = tp->qpPels;
                        lines = tp->qpLines;
                        currFrm = tp->QPintFrame;
                        ROlines = tp->ROlines;
                        ROpels = tp->ROpels;
                        pixPerFrame = pels * lines;

                        // Copy a block of luma samples
                        p_currFrm = currFrm + ROy * pels + ROx;
                        for (j = 0; j < ROlines; j++)
                        {
                            for (i = 0; i < ROpels; i++)
                            {
                                *p_stabFrm++ = *p_currFrm;
                                p_currFrm += 4;
                            }
                            p_currFrm += 4 * pels - 4 * ROpels;
                        }

                        // Copying Cb and Cr samples requires some
                        // interpolation depending on ROx and ROy

                        if (ROx & 0x0001)
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx odd, ROy odd
                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel2 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * pel2 +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + pel2 * line2 +
                                    (ROy / 2) * pel2 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + pel2 * line2 +
                                    (ROy / 2 + 1) * pel2 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm +
                                             *(p_currFrm + 1) + *(pN_currFrm +
                                                                  1)) / 4;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }
                            }
                            else
                            {
                                // ROx odd, ROy even
                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel2 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel2 +
                                    ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm) / 2;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line2 * pel2 +
                                    (ROy / 2) * pel2 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + line2 * pel2 +
                                    (ROy / 2) * pel2 + ROx / 2 + 1;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            (*p_currFrm + *pN_currFrm) / 2;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }

                            }
                        }
                        else
                        {
                            if (ROy & 0x0001)
                            {
                                // ROx even, ROy odd

                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * pel2 +
                                    ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2 +
                                                             1) * pel2 +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm) + (*pN_currFrm)) / 2;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line2 * pel2 +
                                    (ROy / 2) * pel2 + ROx / 2;
                                pN_currFrm =
                                    currFrm + pixPerFrame + line2 * pel2 +
                                    (ROy / 2 + 1) * pel2 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ =
                                            ((*p_currFrm) + (*pN_currFrm)) / 2;
                                        p_currFrm += 4;
                                        pN_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                    pN_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }
                            }
                            else
                            {
                                // ROx even, ROy even

                                p_currFrm =
                                    currFrm + pixPerFrame + (ROy / 2) * (pel2) +
                                    ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm;
                                        p_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }

                                p_currFrm =
                                    currFrm + pixPerFrame + line2 * pel2 +
                                    (ROy / 2) * pel2 + ROx / 2;
                                for (j = 0; j < ROlines / 2; j++)
                                {
                                    for (i = 0; i < ROpels / 2; i++)
                                    {
                                        *p_stabFrm++ = *p_currFrm;
                                        p_currFrm += 4;
                                    }
                                    p_currFrm += 4 * pel2 - 4 * (ROpels / 2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (tp->outputType & FILE_IO)
    {
        if (!doNotWrite)
        {
            if (tp->outputType & DISP_IN_ORG_FRAME)
            {
                WriteImage(tp, tp->stabFrm, tp->outSeqFileName,
                           tp->lines * tp->pels);
            }
            else
            {
                WriteImage(tp, tp->stabFrm, tp->outSeqFileName,
                           tp->ROlines * tp->ROpels);
            }
        }
    }
    /* 
     * #ifdef IS_WINDOWS if (tp->outputType & WIN_IO) {
     * 
     * if (tp->outputType & GRAYSCALE) { tp->dispFrm[0] = tp->stabFrm;
     * //other two dimensions are not used because of grayscale //Show the
     * image doFrameRate(tp);
     * 
     * if (tp->outputType & DISP_IN_ORG_FRAME) { displayImage(tp,
     * tp->dispFrm, tp->pels, tp->lines); } else { displayImage(tp,
     * tp->dispFrm, tp->ROpels, tp->ROlines); } } else if (tp->outputType &
     * YUV420) { doFrameRate(tp); if (tp->outputType & DISP_IN_ORG_FRAME) {
     * tp->dispFrm[0] = tp->stabFrm; tp->dispFrm[1] = tp->stabFrm +
     * tp->pels*tp->lines; tp->dispFrm[2] = tp->stabFrm + tp->pels*tp->lines
     * + tp->pels*tp->lines/4; displayImage(tp, tp->dispFrm, tp->pels,
     * tp->lines); } else { tp->dispFrm[0] = tp->stabFrm; tp->dispFrm[1] =
     * tp->stabFrm + tp->ROpels*tp->ROlines; tp->dispFrm[2] = tp->stabFrm +
     * tp->ROpels*tp->ROlines + tp->ROpels*tp->ROlines/4;
     * displayImage(tp,tp->dispFrm,tp->ROpels,tp->ROlines); } }
     * 
     * } #endif //IS_WINDOWS */

    return;
}

#if 0
#ifdef IS_WINDOWS

/*=======================================================================
 *
 * Name: displayFinalStabSequence()
 *
 * Description: Displays the final stabilized sequence using previously
 *              computed readout positions
 *
 * Input:
 *   tp: Handle for the test platform.
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     displayFinalStabSequence                                               
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
void displayFinalStabSequence(TPstruct * tp)
{
    int c, c2;

    printf
        ("\n[Type 'o' to see original video] [Type 's' to see stabilized video] >");
    c = getchar();
    c2 = getchar();

    while (c != 'n')
    {
        tp->inSeqFilePos = 0;
        fseek(tp->inSeqFile, 0, SEEK_SET);
        tp->frameNo = tp->numFirstFrame;

        tp->targetTime = timeGetTime() + 1000 / tp->frameRate;

        // Read the first frame
        getNewFrame(tp);

        // Initialize with the neutral readout location
        tp->ROx = tp->bounPels;
        tp->ROy = tp->bounLines;
        if (tp->MCres == 1)
        {
            tp->ROx = 2 * tp->ROx;
            tp->ROy = 2 * tp->ROy;
        }
        else if (tp->MCres == 2)
        {
            tp->ROx = 4 * tp->ROx;
            tp->ROy = 4 * tp->ROy;
        }

        // Show the first frame
        storeStabilizedFrame(tp, 1);

        tp->frameNo += tp->frameStep;

        // Main loop
        while (tp->frameNo <= tp->numLastFrame)
        {

            getNewFrame(tp);
            if (tp->MCres == 2)
            {
                tp->ROx = 4 * tp->bounPels + tp->ROxHist[tp->frameNo];
                tp->ROy = 4 * tp->bounLines + tp->ROyHist[tp->frameNo];
            }
            else if (tp->MCres == 1)
            {
                tp->ROx = 2 * tp->bounPels + tp->ROxHist[tp->frameNo];
                tp->ROy = 2 * tp->bounLines + tp->ROyHist[tp->frameNo];
            }
            else
            {
                tp->ROx = tp->bounPels + tp->ROxHist[tp->frameNo];
                tp->ROy = tp->bounLines + tp->ROyHist[tp->frameNo];
            }
            if (c == 'o')
            {
                tp->ROx = tp->bounPels;
                tp->ROy = tp->bounLines;
                if (tp->MCres == 1)
                {
                    tp->ROx = 2 * tp->ROx;
                    tp->ROy = 2 * tp->ROy;
                }
                else if (tp->MCres == 2)
                {
                    tp->ROx = 4 * tp->ROx;
                    tp->ROy = 4 * tp->ROy;
                }
            }

            // Show the previous frame
            storeStabilizedFrame(tp, 1);

            tp->frameNo += tp->frameStep;
        }

        printf
            ("\n[Type 'o' to see original video] [Type 's' to see stabilized video] >");
        c = getchar();
        c2 = getchar();
    }

    return;
}

#endif                                                     // IS_WINDOWS
#endif

/*=======================================================================
 *
 * Name: ReadImage()
 *
 * Description: Reads a frame from a file. Modified from TI MPEG-4 encoder.
 *
 * Input:
 *   tp:               Handle for the test platform
 *   image:            Pointer to the image location
 *   numItemsPerFrame: Number of pixels in the frame
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     ReadImage                                               
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
void ReadImage(TPstruct * tp, Byte * image, Word32 numItemsPerFrame)
{
    Word32 *file_pos = &(tp->inSeqFilePos);

    Word32 frame_no = tp->frameNo;

    FILE *inFile = tp->inSeqFile;

    Word32 status;

    // Skip the header
    if (*file_pos == 0)
    {
        status = read_bytes(image, DEF_HEADER_LENGTH, inFile);
    }

    // Skip to the appropriate frame_no
    if (tp->inputType & YUV420)
    {
        fseek(inFile, (frame_no - (*file_pos)) * numItemsPerFrame * 3 / 2,
              SEEK_CUR);
        status = read_bytes(image, numItemsPerFrame * 3 / 2, inFile);
    }
    else
    {
        fseek(inFile, (frame_no - (*file_pos)) * numItemsPerFrame, SEEK_CUR);
        status = read_bytes(image, numItemsPerFrame, inFile);
    }

    *file_pos = frame_no + 1;

    // Handle reading error
    if (tp->inputType & YUV420)
    {
        if (status != (3 * numItemsPerFrame / 2))
        {
#ifndef NO_PRINTF
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                               "iofun.c ReadImage status=%d, should be=%d\n",
                               (int) status, numItemsPerFrame);
#endif
            exit(-1);
        }
    }
    else
    {
        if (status != numItemsPerFrame)
        {
#ifndef NO_PRINTF
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                               "iofun.c ReadImage status=%d, should be=%d\n",
                               (int) status, numItemsPerFrame);
#endif
            exit(-1);
        }
    }

    return;
}

/*=======================================================================
 *
 * Name: read_bytes()
 *
 * Description: Reads a certain number of pixels from the file
 *
 * Input:
 *   image:  Pointer to the image buffer
 *   nbytes: Number of pixels to be read
 *   inFile: Pointer to the input file
 *
 * Returns:
 *   The number of bytes successfully read.
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     read_bytes                                               
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
Word32 read_bytes(Byte * image, Word32 nbytes, FILE * inFile)
{
    Word32 status;

    status = fread(image, 1, nbytes, inFile);

    return status;
}

/*=======================================================================
 *
 * Name: WriteImage()
 *
 * Description: Writes a frame to a file. Modified from TI MPEG-4 Encoder.
 *
 * Input:
 *   tp:               Handle for the test platform
 *   image:            Pointer to the image buffer
 *   filename:         Name of the output file
 *   numItemsPerFrame: Number of pixels per frame
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     WriteImage                                               
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
void WriteImage(TPstruct * tp, Byte * image, char *filename,
                Word32 numItemsPerFrame)
{
    FILE *f_out;

    /* Opening file */
    if ((f_out = fopen(filename, "ab")) == NULL)
    {
#ifndef NO_PRINTF
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS, "%s%s\n",
                           "Error in opening file: ", filename);
#endif
        exit(-1);
    }

    if ((tp->inputType & GRAYSCALE) && (tp->outputType & GRAYSCALE))
    {
        fwrite_bytes(image, numItemsPerFrame, f_out);
    }
    else if ((tp->inputType & GRAYSCALE) && (tp->outputType & YUV420))
    {
        fwrite_bytes(image, numItemsPerFrame, f_out);
        fwrite_bytes128(image, numItemsPerFrame / 2, f_out);
    }
    else if ((tp->inputType & YUV420) && (tp->outputType & YUV420))
    {
        fwrite_bytes(image, numItemsPerFrame * 3 / 2, f_out);
    }

    fclose(f_out);

    return;
}

/*=======================================================================
 *
 * Name: fwrite_bytes()
 *
 * Description: Writes a certain number of pixels from a file.
 *              Modified from TI MPEG-4 Encoder.
 *
 * Input:
 *   image:  Pointer to the image buffer
 *   nbytes: Number of pixels to be written
 *   f_out:  Pointer to the output file
 *
 * Returns:
 *   The number of bytes successfully written.
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     fwrite_bytes                                               
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
Word32 fwrite_bytes(Byte * image, Word32 nbytes, FILE * f_out)
{
#define WRITE_BLOCK 64
#define WRITE_BLOCK_LOG_2 6

    Word32 status;

    Word32 i, j, numBlocks;

    Word32 numRemBytes;

    Byte *p_image;

    unsigned char c[WRITE_BLOCK];

    numBlocks = nbytes >> WRITE_BLOCK_LOG_2;
    numRemBytes = nbytes - numBlocks * WRITE_BLOCK;
    p_image = image;
    status = 0;
    for (i = 0; i < numBlocks; i++)
    {
        for (j = 0; j < WRITE_BLOCK; j++)
        {
            c[j] = (unsigned char) (*p_image++);
        }
        status += fwrite(c, sizeof(char), WRITE_BLOCK, f_out);
    }

    if (numRemBytes != 0)
    {
        for (j = 0; j < numRemBytes; j++)
        {
            c[j] = (unsigned char) (*p_image++);
        }
        status += fwrite(c, sizeof(char), numRemBytes, f_out);
    }

    return status;
}

/*=======================================================================
 *
 * Name: fwrite_bytes128()
 *
 * Description: Writes a certain number of pixels (128) to a file.
 *              Used for grayscale to yuv conversion.
 *              Modified from TI MPEG-4 Encoder.
 *
 * Input:
 *   image:  Pointer to the image buffer
 *   nbytes: Number of pixels to be read
 *   f_out:  Pointer to the input file
 *
 * Returns:
 *   The number of bytes successfully written.
 *
 * Side effects:
 *
 =======================================================================*/

Word32 fwrite_bytes128(Byte * image, Word32 nbytes, FILE * f_out)
{
#define WRITE_BLOCK 64
#define WRITE_BLOCK_LOG_2 6

    Word32 status;

    Word32 i, j, numBlocks;

    Word32 numRemBytes;

    unsigned char c[WRITE_BLOCK];

    numBlocks = nbytes >> WRITE_BLOCK_LOG_2;
    numRemBytes = nbytes - numBlocks * WRITE_BLOCK;
    status = 0;
    for (i = 0; i < numBlocks; i++)
    {
        for (j = 0; j < WRITE_BLOCK; j++)
        {
            c[j] = (unsigned char) (128);
        }
        status += fwrite(c, sizeof(char), WRITE_BLOCK, f_out);
    }

    if (numRemBytes != 0)
    {
        for (j = 0; j < numRemBytes; j++)
        {
            c[j] = (unsigned char) (128);
        }
        status += fwrite(c, sizeof(char), numRemBytes, f_out);
    }

    return status;
}

/*=======================================================================
 *
 * Name: interpolateFrm()
 *
 * Description: Interpolates a frame by 2.
 *
 * Input:
 *   frm:    Pointer to the image buffer
 *   intFrm: Pointer to the interpolated image
 *   lines:  Number of lines of the image
 *   pels:   Number of pels of the image
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

/* ===================================================================
 *  @func     interpolateFrm                                               
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
void interpolateFrm(Byte * frm, Byte * intFrm, short lines, short pels)
{
    Byte *p_frm, *p_intFrm;

    short vp, hp;

    // Horizontal interpolation
    p_frm = frm;
    p_intFrm = intFrm;
    for (vp = 0; vp < lines; vp++)
    {
        for (hp = 0; hp < pels - 1; hp++)
        {
            *p_intFrm++ = *p_frm++;
            *p_intFrm++ =
                (Byte) (((short) (*(p_frm - 1)) + (short) (*p_frm)) / 2);
        }
        *p_intFrm++ = *p_frm++;
        p_intFrm += pels * 2 - 1;
    }

    // Vertical interpolation
    p_frm = frm;
    p_intFrm = intFrm + 2 * pels - 1;
    for (vp = 0; vp < lines - 1; vp++)
    {
        for (hp = 0; hp < pels; hp++)
        {
            *p_intFrm =
                (Byte) (((short) (*p_frm) + (short) (*(p_frm + pels))) / 2);
            p_intFrm += 2;
            p_frm++;
        }
        p_intFrm += pels * 2 - 2;
    }

    // Diagonal interpolation
    p_frm = frm;
    p_intFrm = intFrm + 2 * pels;
    for (vp = 0; vp < lines - 1; vp++)
    {
        for (hp = 0; hp < pels - 1; hp++)
        {
            *p_intFrm =
                (Byte) (((short) (*p_frm) + (short) (*(p_frm + pels)) +
                         (short) (*(p_frm + 1)) +
                         (short) (*(p_frm + pels + 1))) / 4);
            p_intFrm += 2;
            p_frm++;
        }
        p_intFrm += pels * 2;
        p_frm++;
    }

    return;
}

#if 0
#ifdef IS_WINDOWS

/*=======================================================================
 *
 * Name: doFrameRate()
 *
 * Description: Adjusts the frame rate of the display.
 *              Modified from TI MPEG-4 encoder.
 *
 * Input:
 *   tp: Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/

// Modified from TI MPEG-4 Encoder
/* ===================================================================
 *  @func     doFrameRate                                               
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
void doFrameRate(TPstruct * tp)
{
    DWORD currentTime;

    int diffTime;

    /* Compute desired frame rate */
    if (tp->frameRate <= 0)
        return;

    /* this is where we want to be */
    tp->targetTime += 1000 / tp->frameRate;

    /* this is where we are */
    currentTime = timeGetTime();

    diffTime = tp->targetTime - currentTime;

    // printf("\n%d",diffTime);

    /* See if we are already lagging behind */
    if (diffTime < 0)
    {
        // fprintf(stderr,"\nCould not achieve target display frame
        // rate!\n");
        tp->targetTime = currentTime;
        return;
    }

    /* Spin for awhile */
    Sleep(diffTime);

    /* this is not a very accurate timer */
}

#endif
#endif
