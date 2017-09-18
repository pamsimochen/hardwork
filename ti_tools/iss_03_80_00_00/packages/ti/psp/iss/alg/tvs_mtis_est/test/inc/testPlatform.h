/** ==================================================================
 *  @file   testPlatform.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/tvs_mtis_est/test/inc/                                                  
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
 *  File: testPlatform.h
 *  
 *=======================================================================
 *
 *  Revision 4.5 (2-April-2009)
 *
 =======================================================================*/

/* Intended for the PC platform */

#ifndef _TEST_PLATFORM_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _TEST_PLATFORM_INCLUDED_
#include "globalDefs.h"
#include <WTSD_DucatiMMSW/platform/osal/timm_osal_interfaces.h>

#define INPUT_PATH "..\\..\\..\\..\\..\\alg\\tvs_mtis_est\\test\\test_vectors\\input\\"
#define OUTPUT_PATH "..\\..\\..\\..\\..\\alg\\tvs_mtis_est\\test\\test_vectors\\output\\"
#define REFOUTPUT_PATH "..\\..\\..\\..\\..\\alg\\tvs_mtis_est\\test\\test_vectors\\ref_output\\"

// #include "TI_VidStab.h"

#define MAX_FRAME_SIZE_FOR_DISPLAY_V 1200                  // 480
// #define MAX_FRAME_SIZE_FOR_DISPLAY_H 1920//640
#define MAX_FRAME_SIZE_FOR_DISPLAY_H 2048                  // 640

#define VSTAB_Malloc(_size, _bytesalign) \
  TIMM_OSAL_MallocExtn(_size, TIMM_OSAL_TRUE, _bytesalign, TIMMOSAL_MEM_SEGMENT_EXT, NULL)

#define TVS_MTIS_Malloc(SIZE, BYTEALIGNMENT)      VSTAB_Malloc(SIZE, BYTEALIGNMENT)
#define TVS_MTIS_Free(PTR)        if(PTR != NULL) TIMM_OSAL_Free(PTR)
#define TVS_MTISMemset(PTR, SIZE, VAL)   TIMM_OSAL_Memset(PTR, SIZE, VAL)
#define TVS_MTIS_Memcpy(PTR_Dst, PTR_Src, USIZE)   TIMM_OSAL_Memcpy(PTR_Dst, PTR_Src, USIZE)

#define TVS_MTIS_ASSERT   TVS_MTIS_PARAMCHECK

// #define TIMM_OSAL_TraceFunction printf
#define TVS_MTIS_PARAMCHECK(C,V)  if (!(C)) { \
    tErr = V;\
    TIMM_OSAL_TraceFunction("ERROR: in %s [%s] :: %d \n", __FILE__,__FUNCTION__, __LINE__);\
    goto EXIT; \
}

typedef struct {
    char inputFile[500];
    char outputFile[500];
    Word16 HorzImgSizeBeforeBSC;
    Word16 VertImgSizeBeforeBSC;
    Word16 HorzImgSizeAfterIpipe;
    Word16 VertImgSizeAfterIpipe;
    Word32 StartFrameNum;
    Word32 LastFrameNum;
    Word16 BoundaryPixelsH;
    Word16 BoundaryPixelsV;
    Word16 SearchRangePixelsH;
    Word16 SearchRangePixelsV;
} VstabTestParamType;

// Test platform structure: Contains parameters and data for the PC platform
// where VS is tested.
typedef struct {

    // File I/O
    Word32 inputType;
    Word32 outputType;
    char inSeqFileName[500];
    char outSeqFileName[500];
    char RO_FileName[500];
    Word16 RO_fileExists;
    char SC_FileName[500];
    Word16 SC_fileExists;
    FILE *inSeqFile;
    Word32 inSeqFilePos;
    FILE *outSeqFile;
    Word32 numFirstFrame;
    Word32 numLastFrame;
    Word32 frameStep;
    Word32 frameNo;

    Word16 lines;
    Word16 pels;
    Word16 ROpels;
    Word16 ROlines;
    Word16 bounPels;
    Word16 bounLines;

    Word16 hpLines;
    Word16 hpPels;
    Word16 qpLines;
    Word16 qpPels;

    Word16 ROx;
    Word16 ROy;

    Byte *frmBuffer;
    Byte *frmBufferPrev;

    Word16 MCres;
    /* 
     * //Display #ifdef IS_WINDOWS DWORD targetTime; unsigned char
     * display_frame_mem[MAX_FRAME_SIZE_FOR_DISPLAY_V*MAX_FRAME_SIZE_FOR_DISPLAY_H*3/2]; 
     * Byte* dispFrm[3]; #endif */
    /* Byte
     * interlacedFrame[MAX_FRAME_SIZE_FOR_DISPLAY_V*MAX_FRAME_SIZE_FOR_DISPLAY_H*3/2]; 
     * Byte
     * stabFrm[MAX_FRAME_SIZE_FOR_DISPLAY_V*MAX_FRAME_SIZE_FOR_DISPLAY_H*3/2]; 
     * Word32 frameRate; Word16 ROxHist[4000]; Word16 ROyHist[4000]; Word16
     * MVxHist[4000]; Word16 MVyHist[4000]; uWord16 MVscaleY[4000]; uWord16
     * MVscaleX[4000]; Byte HPintFrame[1843200]; //[480*640*4*3/2];
     * 
     * Byte QPintFrame[7372800]; //[480*640*16*3/2]; */
    Byte *interlacedFrame;
    Byte *stabFrm;
    Word32 frameRate;
    Word16 *ROxHist;
    Word16 *ROyHist;
    Word16 *MVxHist;
    Word16 *MVyHist;
    uWord16 *MVscaleY;
    uWord16 *MVscaleX;
    Byte *HPintFrame;                                      // [480*640*4*3/2]; 
                                                           // 

    Byte *QPintFrame;                                      // [480*640*16*3/2];
    // Byte QPintFrame[1]; //Allocate QPintFrame for quarter pel compensation

    Byte *bscBufferCurr;
    Byte *bscBufferPrev;

    Word16 horzImgSizeBeforeBSC;
    Word16 vertImgSizeBeforeBSC;
    Word32 bsc_row_vct;
    Word32 bsc_row_vpos;
    Word32 bsc_row_vnum;
    Word32 bsc_row_vskip;
    Word32 bsc_row_hpos;
    Word32 bsc_row_hnum;
    Word32 bsc_row_hskip;
    Word32 bsc_row_shf;
    Word32 bsc_col_vct;
    Word32 bsc_col_vpos;
    Word32 bsc_col_vnum;
    Word32 bsc_col_vskip;
    Word32 bsc_col_hpos;
    Word32 bsc_col_hnum;
    Word32 bsc_col_hskip;
    Word32 bsc_col_shf;

} TPstruct;

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
void VS_compensateMotion(MSP_TVS_MTIS_FRAME_OUT * vs, TPstruct * tp);

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
void InitTP(MSP_HANDLE hMSP, MSP_TVS_MTIS_CREATE_PARAM * vsCP, TPstruct * tp);

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
void CloseTP(TPstruct * tp);

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
void getNewFrame(TPstruct * tp);

/* 
 * #ifdef IS_WINDOWS void displayFinalStabSequence(TPstruct* tp); #endif */
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
void storeStabilizedFrame(TPstruct * tp, Word16 doNotWrite);

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
void ReadImage(TPstruct * tp, Byte * image, Word32 numItemsPerFrame);

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
void WriteImage(TPstruct * tp, Byte * image, char *filename, Word32 numOfPix);

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
Word32 read_bytes(Byte * image, Word32 nbytes, FILE * inFile);

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
Word32 fwrite_bytes(Byte * image, Word32 nbytes, FILE * f_out);

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
void interpolateFrm(Byte * frm, Byte * intFrm, Word16 lines, Word16 pels);

Word32 fwrite_bytes128(Byte * image, Word32 nbytes, FILE * f_out);

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
void computeBoundarySignals(TPstruct * tp);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
