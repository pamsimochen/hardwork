/** ==================================================================
 *  @file   jpeg_dec.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/test/simcop/test_algo/jpeg_dec/jpeg_decode/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "jpeg_dec.h"
#include "Image_Buffer.h"

/* module handles are defined here and exposed to the lower layers */
CSL_SimcopHandle Simcophndl;

CSL_DctHandle Dcthndl;

CSL_SimcopDmaHandle SimcopDmahndl;

CSL_VlcdjHandle Vlcdhndl;

Uint16 NUMMCUPERBLOCK;

Bool bMtcr2OcpErrIrqOccured, bOcpErrIrqOccured, bVlcdjDecodeErrIrqOccured,
    bDoneIrqOccured;

/* ===================================================================
 *  @func     set_huff                                               
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
void set_huff(const unsigned short int *pHuffTab)
{
    int i = 0;

    int j = 0;

    unsigned short int *intptr;

    intptr = (unsigned short int *) HUFTAB_ADDRESS;
    j = 8 * 256;

    for (i = 0; i < j; i++)
    {
        *intptr = pHuffTab[i];
        intptr++;
    }

}

/* ===================================================================
 *  @func     set_quant                                               
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
void set_quant(const unsigned short int *pQuantTab)
{
    int i = 0;

    int j = 0;

    unsigned short int *quantptr;

    quantptr = (unsigned short int *) QUANTAB_ADDRESS;
    j = 8 * 32;

    for (i = 0; i < j; i++)
    {
        *quantptr = pQuantTab[i];
        quantptr++;
    }
}

/* ===================================================================
 *  @func     reset_buf                                               
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
void reset_buf(unsigned int *p)
{
    int i;

    for (i = 0; i < 1024; i++)
        *p++ = 0xABCDDEAD;
}

/* ===================================================================
 *  @func     read_hex_short                                               
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
void read_hex_short(char *filename, unsigned short *image, /* allocated
                                                            * before calling
                                                            * this function */
                    int width, int height)                 /* width and
                                                            * height provided 
                                                            * by caller */
{
    int i;

    FILE *ff;

    /* open a file for reading */
    ff = fopen(filename, "r");

    for (i = 0; i < height * width; i++)
        fscanf(ff, "%hx", &image[i]);

    fclose(ff);
}

/* ===================================================================
 *  @func     read_hex_char                                               
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
void read_hex_char(char *filename, unsigned char *image,   /* allocated
                                                            * before calling
                                                            * this function */
                   int width, int height)                  /* width and
                                                            * height provided 
                                                            * by caller */
{
    int i;

    FILE *ff;

    short tmp_short;

    /* open a file for reading */
    ff = fopen(filename, "r");

    for (i = 0; i < height * width; i++)
    {
        fscanf(ff, "%hx", &tmp_short);
        image[i] = tmp_short;
    }

    fclose(ff);

}

/* ===================================================================
 *  @func     getNMCU                                               
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
Uint16 getNMCU(JPEGParams * JpegParam)                     /* per block and
                                                            * per row and per 
                                                            * col */
{
    if (YUV420 == JpegParam->Format && JpegParam->Isplanar == FALSE)
    {
        JpegParam->uNumMCUPerBlock = MAX_MCUSPERBLK_YUV420;
        JpegParam->YBlkCount =
            (JpegParam->uImageHeight / MCU_HEIGHT_IN_PIXELS_YUV420);
    }
    else if (YUV422 == JpegParam->Format && JpegParam->Isplanar == FALSE)
    {
        JpegParam->uNumMCUPerBlock = MAX_MCUSPERBLK_YUV422;
        JpegParam->YBlkCount =
            (JpegParam->uImageHeight / MCU_HEIGHT_IN_PIXELS_YUV422);
    }
    else if (JpegParam->Isplanar == TRUE)
    {
        JpegParam->uNumMCUPerBlock = MAX_MCUSPERBLK_SEQUENTIAL_MODE;
        JpegParam->YBlkCount =
            (JpegParam->uImageHeight / MCU_HEIGHT_IN_PIXEL_SEQUENTIAL_MODE);
    }
    if (JpegParam->Isplanar == FALSE)

    {
        while (JpegParam->uNumMCUPerBlock)
        {
            if ((JpegParam->uImageWidth %
                 (JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_PIXELS)) == 0)
                break;
            JpegParam->uNumMCUPerBlock--;
        }

        JpegParam->XBlkCount =
            ((JpegParam->uImageWidth) /
             (MCU_WIDTH_IN_PIXELS * JpegParam->uNumMCUPerBlock));
    }

    else

    {
        if (JpegParam->YuvComp == Y || JpegParam->Format == YUV444)
        {
            while (JpegParam->uNumMCUPerBlock)
            {
                if ((JpegParam->uImageWidth %
                     (JpegParam->uNumMCUPerBlock *
                      MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE)) == 0)
                    break;
                JpegParam->uNumMCUPerBlock--;
            }
            JpegParam->XBlkCount =
                ((JpegParam->uImageWidth) /
                 (MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE *
                  JpegParam->uNumMCUPerBlock));
        }

        else

        {

            while (JpegParam->uNumMCUPerBlock)
            {
                if (((JpegParam->uImageWidth / 2) %
                     (JpegParam->uNumMCUPerBlock *
                      MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE)) == 0)
                    break;
                JpegParam->uNumMCUPerBlock--;
            }
            JpegParam->XBlkCount =
                ((JpegParam->uImageWidth) /
                 (MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE *
                  JpegParam->uNumMCUPerBlock * 2));
        }

        if (JpegParam->Format == YUV420)
        {
            if (JpegParam->YuvComp != Y)
            {
                // JpegParam->XBlkCount/=2;
                JpegParam->YBlkCount /= 2;
            }

        }

    }

    return (JpegParam->uNumMCUPerBlock);
}

/* ===================================================================
 *  @func     get_handle                                               
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
Jpeg_enc_status get_handle()
{
    CSL_SimcopObj *pSimcopObj;

    CSL_DctObj *pDctObj;

    CSL_SimcopDmaObj *pSimcopDmaObj;

    CSL_VlcdjObj *pVlcdObj;

    Jpeg_enc_status status = JPEG_ENC_SOK;

    /* Get SIMCOP handle */
    pSimcopObj = (CSL_SimcopObj *) malloc(sizeof(CSL_SimcopObj));
    pSimcopObj->openMode = CSL_EXCLUSIVE;
    pSimcopObj->uid = CSL_SIMCOP_0_UID;                    /* @todo ? */
    pSimcopObj->xio = CSL_SIMCOP_0_XIO;
    pSimcopObj->regs = (CSL_SimcopRegsOvly) SIMCOP_START_ADDRESS;
    pSimcopObj->perNum = CSL_SIMCOP_0;
    {
        CSL_Status status;

        Simcophndl =
            CSL_simcopOpen(pSimcopObj, pSimcopObj->perNum, pSimcopObj->openMode,
                           &status);
        if (CSL_SOK != status)
        {
            printf("Usage: Error in getting handle to Simcop instance\n");
            exit(-1);
        }
    }

    /* Get SIMCOP DMA handle */
    pSimcopDmaObj = (CSL_SimcopDmaObj *) malloc(sizeof(CSL_SimcopDmaObj));
    pSimcopDmaObj->openMode = CSL_EXCLUSIVE;
    pSimcopDmaObj->uid = CSL_COPDMA_CHA0_UID;              /* @todo ? */
    pSimcopDmaObj->xio = CSL_COPDMA_CHA0_XIO;
    pSimcopDmaObj->regs = (CSL_SimcopDmaRegsOvly) SIMCOPDMA_START_ADDRESS;
    pSimcopDmaObj->perNum = CSL_COPDMA_0;
    {
        CSL_Status status;

        SimcopDmahndl =
            CSL_simcopDmaOpen(pSimcopDmaObj, pSimcopDmaObj->perNum,
                              pSimcopDmaObj->openMode, &status);
        if (CSL_SOK != status)
        {
            printf("Usage: Error in getting handle to Simcop DMA instance\n");
            exit(-1);
        }
    }

    /* Get DCT handle */
    pDctObj = (CSL_DctObj *) malloc(sizeof(CSL_DctObj));
    pDctObj->openMode = CSL_EXCLUSIVE;
    pDctObj->uid = CSL_DCT_0_UID;                          /* @todo ? */
    pDctObj->xio = CSL_DCT_0_XIO;
    pDctObj->regs = (CSL_DctRegsOvly) DCT_START_ADDRESS;
    pDctObj->perNum = CSL_DCT_0;
    {
        CSL_Status status;

        Dcthndl =
            CSL_dctOpen(pDctObj, pDctObj->perNum, pDctObj->openMode, &status);
        if (CSL_SOK != status)
        {
            printf("Usage: Error in getting handle to DCT instance\n");
            goto EXIT;
        }
    }

    /* Get VLCDJ handle */
    pVlcdObj = (CSL_VlcdjObj *) malloc(sizeof(CSL_VlcdjObj));
    pVlcdObj->openMode = CSL_EXCLUSIVE;
    pVlcdObj->uid = CSL_VLCDJ_0_UID;                       /* @todo ? */
    pVlcdObj->xio = CSL_VLCDJ_0_XIO;
    pVlcdObj->regs = (CSL_VlcdjRegsOvly) VLCDJ_START_ADDRESS;
    pVlcdObj->perNum = CSL_VLCDJ_0;
    {
        CSL_Status status;

        Vlcdhndl =
            CSL_VlcdjOpen(pVlcdObj, pVlcdObj->perNum, pVlcdObj->openMode,
                          &status);
        if (CSL_SOK != status)
        {
            printf("Usage: Error in getting handle to VLCDj instance\n");
            goto EXIT;
        }
    }

  EXIT:
    return status;

}

/* ===================================================================
 *  @func     DMA_to_Ping                                               
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
static void DMA_to_Ping(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_SIMCOP_DMA; /* Attach 
                                                                                 * buffers 
                                                                                 * to 
                                                                                 * accelerators */
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_EFGHABCD;  /* Configure 
                                                                         * buffer 
                                                                         * offsets */
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;   /* Enable 
                                                                     * syncing 
                                                                     * of
                                                                     * modules 
                                                                     * needed 
                                                                     * with
                                                                     * HWSeq */
}

/* ===================================================================
 *  @func     DMA_to_Pong                                               
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
static void DMA_to_Pong(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_FGHABCDE;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     DCT_to_Ping                                               
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
static void DCT_to_Ping(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_EF;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_CDGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_to_Pong                                               
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
static void DCT_to_Pong(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_FG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_ABCD;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_to_Ping                                               
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
static void VLCDJE_to_Ping(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_CDGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_to_Pong                                               
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
static void VLCDJE_to_Pong(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_ABCD;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DMA_on_A                                               
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
static void DMA_on_A(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_ABCDEFGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     ROT_B_to_E                                               
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
static void ROT_B_to_E(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_ROT_A_O;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotIOfst = ROTIN_BCDA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotOOfst = ROTOUTLDCOUT_EFGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_F_to_C                                               
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
static void DCT_F_to_C(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_FG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_CDGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_on_D                                               
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
static void VLCDJE_on_D(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_DGHA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DMA_on_D                                               
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
static void DMA_on_D(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_DEFGHABC;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     ROT_A_to_F                                               
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
static void ROT_A_to_F(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_ROT_A_O;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotIOfst = ROTIN_ABCD;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotOOfst = ROTOUTLDCOUT_FGHE;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_E_to_B                                               
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
static void DCT_E_to_B(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_EF;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_BCDG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_on_C                                               
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
static void VLCDJE_on_C(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_CDGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DMA_on_C                                               
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
static void DMA_on_C(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_CDEFGHAB;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     ROT_D_to_E                                               
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
static void ROT_D_to_E(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_ROT_A_O;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotIOfst = ROTIN_DABC;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotOOfst = ROTOUTLDCOUT_EFGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_F_to_A                                               
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
static void DCT_F_to_A(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_FG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_ABCD;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_on_B                                               
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
static void VLCDJE_on_B(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_BCDG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DMA_on_B                                               
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
static void DMA_on_B(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffB =
        BUFSWITCHIMGABCDCOEFFA_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_BCDEFGHA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     ROT_C_to_F                                               
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
static void ROT_C_to_F(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_ROT_A_O;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotIOfst = ROTIN_CDAB;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotOOfst = ROTOUTLDCOUT_FGHE;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_E_to_D                                               
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
static void DCT_E_to_D(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffD =
        BUFSWITCHIMGABCDCOEFFA_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_EF;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_DGHA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_on_A                                               
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
static void VLCDJE_on_A(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffA =
        BUFSWITCHIMGABCDCOEFFA_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_ABCD;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DMA_on_F                                               
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
static void DMA_on_F(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_SIMCOP_DMA;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaOfst = DMAOFST_FGHABCDE;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DmaSync = DMASYNC_CH0;
}

/* ===================================================================
 *  @func     ROT_C_to_E                                               
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
static void ROT_C_to_E(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffC =
        BUFSWITCHIMGABCDCOEFFA_ROT_A_I;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_ROT_A_O;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotIOfst = ROTIN_CDAB;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotOOfst = ROTOUTLDCOUT_EFGH;
    simcopsetup->HwseqCtrl.HwSeqStep[step].RotSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_F_to_G                                               
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
static void DCT_F_to_G(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffF = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffG = BUFSWITCHIMGGH_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_FG;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_GHAB;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_E_to_H                                               
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
static void DCT_E_to_H(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffE = BUFSWITCHIMGEF_DCT_S;
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffG = BUFSWITCHIMGGH_DCT_F;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSOfst = DCTSNSF_EF;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctFOfst = DCTFVLCDJ_HABC;
    simcopsetup->HwseqCtrl.HwSeqStep[step].DctSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     VLCDJE_on_G                                               
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
static void VLCDJE_on_G(CSL_SimcopHwSetupCtrl * simcopsetup, Uint8 step)
{
    simcopsetup->HwseqCtrl.HwSeqStep[step].ImBuffG = BUFSWITCHIMGGH_VLCDJ_IO;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjIOOfst = DCTFVLCDJ_GHAB;
    simcopsetup->HwseqCtrl.HwSeqStep[step].VlcdjSync = SYNC_ENABLED;
}

/* ===================================================================
 *  @func     DCT_Config_Decode                                               
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
Jpeg_enc_status DCT_Config_Decode(JPEGParams * JpegParam)
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    CSL_DctHwSetup *dctsetup;

    dctsetup = (CSL_DctHwSetup *) malloc(sizeof(CSL_DctHwSetup));
    memset(dctsetup, 0, sizeof(CSL_DctHwSetup));
    if (NULL == dctsetup)
        printf("Failed to allocate memory for dctsetup structure\n");

    dctsetup->Autogating = CSL_SIMCOP_AUTOGATING_ON;
    dctsetup->HwEnable = CSL_SIMCOP_HW_DISABLE;
    dctsetup->IntEnable = CSL_SIMCOP_HW_INT_ENABLE;
    dctsetup->TrgSrcType = CSL_SIMCOP_HW_TRIG_HWSTART_SIGNAL;
    if (JpegParam->Format == YUV420 && JpegParam->Isplanar == FALSE)
        dctsetup->SpFmtType = CSL_SIMCOP_DCT_SPDATA_YUV420;
    else if (JpegParam->Format == YUV422 && JpegParam->Isplanar == FALSE)
        dctsetup->SpFmtType = CSL_SIMCOP_DCT_SPDATA_YUV422;
    else if (JpegParam->Isplanar == TRUE)
        dctsetup->SpFmtType = CSL_SIMCOP_DCT_SPDATA_SEQBLK;
    dctsetup->nMcus = JpegParam->uNumMCUPerBlock - 1;
    dctsetup->nModeType = CSL_SIMCOP_MODE_IDCT;

    status = CSL_dctHwSetup(Dcthndl, dctsetup);
    free(dctsetup);
    return status;

}

/* ===================================================================
 *  @func     VLCDJ_Config_Decode                                               
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
Jpeg_enc_status VLCDJ_Config_Decode(JPEGParams * JpegParam)
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    CSL_VlcdHwSetup *vlcdsetup;

    CSL_HuffQuantAccType valueb1, valuea, valueb2;

    /* set up */

    vlcdsetup = (CSL_VlcdHwSetup *) malloc(sizeof(CSL_VlcdHwSetup));
    memset(vlcdsetup, 0, sizeof(CSL_VlcdHwSetup));
    if (NULL == vlcdsetup)
        printf("Failed to allocate memory for vlcdsetup structure\n");
    /* VLCD REV */
    if (JpegParam->Isplanar == FALSE ||
        (JpegParam->Isplanar == TRUE && JpegParam->YuvComp == Y))
    {
        vlcdsetup->nRevId = 0x10;

        /* VLCDJ CTRL settings */

        vlcdsetup->nEnable = CSL_SIMCOP_HW_DISABLE;
        vlcdsetup->nModeType = CSL_SIMCOP_MODE_VLCD_DECODE;
        vlcdsetup->nTrgSrcType = CSL_SIMCOP_HW_TRIG_HWSTART_SIGNAL;
        vlcdsetup->nAutogating = CSL_SIMCOP_AUTOGATING_ON;
        vlcdsetup->nIntenDone = CSL_SIMCOP_HW_INT_ENABLE;
        vlcdsetup->nIntenErr = CSL_RESET;
        vlcdsetup->nRben = CSL_RESET;
        // <change> vlcdsetup->nClrrb = CSL_SET;
        vlcdsetup->nClrrb = CSL_RESET;
        /* VLCDJ_CFG */
        /* vlcdsetup->nEncNmcus = 0;//JpegParam->uNumMCUPerBlock -1;
         * vlcdsetup->nEncRlocen =0;//CSL_SET; vlcdsetup->nEncRsten =0; */
        /* if(JpegParam->Format == YUV420) vlcdsetup->nEncFmtType =
         * CSL_SIMCOP_VLCD_ENCODE_YUV420; else if (JpegParam->Format ==
         * YUV422) vlcdsetup->nEncFmtType = CSL_SIMCOP_VLCD_ENCODE_YUV422; */
        /* VLCDJ BSPPTR */
        vlcdsetup->pEncBitptr = 8;
        vlcdsetup->pEncByteptr = RESET0;

        /* VLCDJE CBUF */
        vlcdsetup->pEncCbufStart = RESET0;
        vlcdsetup->pEncCbufEnd = 0xC00;
        /* VLCDJE RSTCFG */
        vlcdsetup->nEncRstInc = 0x1;
        vlcdsetup->nEncRstInit = 0;
        vlcdsetup->nEncRstIntrvl = 1;
        vlcdsetup->nEncRstPhase = 0;
        vlcdsetup->nEncRstofst = (Uint32) & JpegParam->pOutput[0];
        vlcdsetup->nEncDcpred[0] = RESET0;
        vlcdsetup->nEncDcpred[1] = RESET0;
        vlcdsetup->nEncDcpred[2] = RESET0;
        vlcdsetup->pEncDct = RESET0;
        vlcdsetup->pEncQmr = RESET0;
        vlcdsetup->pEncRstptr = RESET0;
        vlcdsetup->pEncVlctbl = RESET0;

        vlcdsetup->nDecNmcus = JpegParam->uNumMCUPerBlock - 1;
        vlcdsetup->nDecRsten = CSL_SET;
        vlcdsetup->pDecBitptr = 8;
        vlcdsetup->pDecByteptr = RESET0;
        vlcdsetup->pDecCbufStart = RESET0;
        vlcdsetup->pDecCbufEnd = 0xC00;
        vlcdsetup->pDecDct = RESET0;
        vlcdsetup->pDecQmr = RESET0;

        vlcdsetup->pDecCtrltbl = RESET0;
        if (JpegParam->Format == YUV420 && JpegParam->Isplanar == FALSE)
            vlcdsetup->nDecFmtType = CSL_SIMCOP_VLCD_DECODE_YUV420;
        else if (JpegParam->Format == YUV422 && JpegParam->Isplanar == FALSE)
            vlcdsetup->nDecFmtType = CSL_SIMCOP_VLCD_DECODE_YUV422;
        else if (JpegParam->Isplanar == TRUE)
            vlcdsetup->nDecFmtType = CSL_SIMCOP_VLCD_DECODE_SEQBLK;

        vlcdsetup->nDecDcpred[0] = RESET0;
        vlcdsetup->nDecDcpred[1] = RESET0;
        vlcdsetup->nDecDcpred[2] = RESET0;

        vlcdsetup->pDecDcdtbl[0] = 0x78;
        vlcdsetup->pDecDcdtbl[1] = 0x94;
        vlcdsetup->pDecDcdtbl[2] = 0x200;
        vlcdsetup->pDecDcdtbl[3] = 0x21c;

        status = CSL_VlcdjHwSetup(Vlcdhndl, vlcdsetup);
    }

    else if (JpegParam->Isplanar == TRUE && JpegParam->YuvComp != Y)
    {
        if (JpegParam->YuvComp == U)
        {
            vlcdsetup->pDecQmr = 128;
            vlcdsetup->pDecCtrltbl = 60;
            vlcdsetup->pDecDcdtbl[0] = 0x200;
            vlcdsetup->pDecDcdtbl[1] = 0x21c;
        }
        CSL_VlcdjGetHwStatus(Vlcdhndl, CSL_VLCDJD_QUERY_BYTEPTR,
                             &vlcdsetup->pDecByteptr);
        CSL_VlcdjGetHwStatus(Vlcdhndl, CSL_VLCDJD_QUERY_GETBITPTR,
                             &vlcdsetup->pDecBitptr);

        if (vlcdsetup->pDecBitptr < 8)
            vlcdsetup->pDecByteptr += 11;                  /* scan header for 
                                                            * 1 component =
                                                            * 10, +1 for
                                                            * partial byte */
        else
            vlcdsetup->pDecByteptr += 10;

        vlcdsetup->pDecBitptr = 8;

        if (vlcdsetup->pDecByteptr >= BITSTRBUF_SIZE)
            vlcdsetup->pDecByteptr -= BITSTRBUF_SIZE;
        vlcdsetup->nDecNmcus = JpegParam->uNumMCUPerBlock - 1;
        vlcdsetup->nDecDcpred[0] = RESET0;
        vlcdsetup->nDecDcpred[1] = RESET0;
        vlcdsetup->nDecDcpred[2] = RESET0;

        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETSETBITPTR,
                           &vlcdsetup->pDecBitptr);
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETBYTEPTR,
                           &vlcdsetup->pDecByteptr);
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETNMCUS,
                           &vlcdsetup->nDecNmcus);
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETDCPREDY,
                           &vlcdsetup->nDecDcpred[0]);
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETDCPREDU,
                           &vlcdsetup->nDecDcpred[1]);
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETDCPREDV,
                           &vlcdsetup->nDecDcpred[2]);

        if (JpegParam->YuvComp == U)
        {
            CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETQMR,
                               &vlcdsetup->pDecQmr);
            CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETVLCTBL,
                               &vlcdsetup->pDecCtrltbl);
            CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETDEC_HUFF0,
                               &vlcdsetup->pDecDcdtbl[0]);
            CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJD_CMD_SETDEC_HUFF1,
                               &vlcdsetup->pDecDcdtbl[1]);
        }

    }

    /* get the status of whereHUFF and Quant tables are attached and store
     * the values in b1and b2 */
    CSL_simcopGetHWStatus(Simcophndl, CSL_SIMCOP_QUERY_HUFF, &valueb1);
    CSL_simcopGetHWStatus(Simcophndl, CSL_SIMCOP_QUERY_QUANT, &valueb2);

    valuea = HUFFQUANTACC_COPROCESSOR;
    CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_HUFF, &valuea);
    CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_QUANT, &valuea);

    set_huff(JpegParam->pHuffTab);
    set_quant(JpegParam->pQuantTab);

    /* Restore the values stored in b1 and b2 */
    CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_HUFF, &valueb1);
    CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_QUANT, &valueb2);

    free(vlcdsetup);
    return status;

}

/**************************************************************************************/
/************************** module specific configuration Start here ******************/
/**************************************************************************************/

/* ===================================================================
 *  @func     DMA_Config_Decode                                               
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
Jpeg_enc_status DMA_Config_Decode(JPEGParams * JpegParam)
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    CSL_SimcopDmaHwSetup *DmaSetup = NULL;

    DmaSetup = malloc(sizeof(CSL_SimcopDmaHwSetup));
    if (DmaSetup == NULL)
        printf("Failed to allocate memory for DmaSetup structure\n");

    memset(DmaSetup, 0, sizeof(CSL_SimcopDmaHwSetup));

    DmaSetup->DmaCtrl.TagCnt = 3;
    DmaSetup->DmaSysConfig.StandByMode = CSL_DMA_SYSCONFIG_STANDBYMODE_SMART0;
    DmaSetup->EOILineNum = CSL_DMA_IRQEOI_LINE_NUMBER_LINE1;

    if (JpegParam->Format == YUV420 && JpegParam->Isplanar == FALSE)
    {
        DmaSetup->DmaChannelCtrl[0].SetupChan = TRUE;
        DmaSetup->DmaChannelCtrl[0].SMemAddr = (Uint32) JpegParam->pOutput;
        DmaSetup->DmaChannelCtrl[0].SMemOfst = JpegParam->uImageWidth;
        DmaSetup->DmaChannelCtrl[0].BufAddr = IBuffer_A_Ofst;
        DmaSetup->DmaChannelCtrl[0].BufOfst =
            (MCU_WIDTH_IN_BYTES_YUV420 * JpegParam->uNumMCUPerBlock);
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.YNUM =
            MCU_HEIGHT_IN_PIXELS_YUV420;
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.XNUM =
            JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.YSTEP =
            MCU_HEIGHT_IN_PIXELS_YUV420;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.XSTEP =
            JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[0].Frame.YCNT = JpegParam->YBlkCount;
        DmaSetup->DmaChannelCtrl[0].Frame.XCNT = JpegParam->XBlkCount;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Dir =
            CSL_DMA_CHAN_CTRL_DIR_IBUFTOSYS;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStart =
            CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN0;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStop =
            CSL_DMA_CHAN_CTRL_HWSTOP_DISABLED;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Linked =
            CSL_DMA_CHAN_CTRL_LINKED_CHAN1;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.SWTrigger =
            CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Enable =
            CSL_DMA_CHAN_CTRL_ENABLE_ENABLE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.BlockDone.IRQEnableSet = TRUE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.FrameDone.IRQEnableSet = TRUE;

        DmaSetup->DmaChannelCtrl[1].SetupChan = TRUE;
        DmaSetup->DmaChannelCtrl[1].SMemAddr =
            (Uint32) ((JpegParam->pOutput) +
                      (JpegParam->uImageHeight * JpegParam->uImageWidth));
        DmaSetup->DmaChannelCtrl[1].SMemOfst = JpegParam->uImageWidth;
        DmaSetup->DmaChannelCtrl[1].BufAddr =
            IBuffer_A_Ofst +
            (JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_BYTES_YUV420 *
             MCU_HEIGHT_IN_PIXELS_YUV420);
        DmaSetup->DmaChannelCtrl[1].BufOfst =
            (MCU_WIDTH_IN_BYTES_YUV420 * JpegParam->uNumMCUPerBlock);
        DmaSetup->DmaChannelCtrl[1].ChanBlkSize.YNUM =
            MCU_HEIGHT_IN_PIXELS_YUV420 / 2;
        DmaSetup->DmaChannelCtrl[1].ChanBlkSize.XNUM =
            JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[1].ChanBlkStep.YSTEP =
            MCU_HEIGHT_IN_PIXELS_YUV420 / 2;
        DmaSetup->DmaChannelCtrl[1].ChanBlkStep.XSTEP =
            JpegParam->uNumMCUPerBlock * MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[1].Frame.YCNT = JpegParam->YBlkCount;
        DmaSetup->DmaChannelCtrl[1].Frame.XCNT = JpegParam->XBlkCount;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.Dir =
            CSL_DMA_CHAN_CTRL_DIR_IBUFTOSYS;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.HWStart =
            CSL_DMA_CHAN_CTRL_HWSTART_DISABLED;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.HWStop =
            CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN0;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.Linked =
            CSL_DMA_CHAN_CTRL_LINKED_DISABLED;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.SWTrigger =
            CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT;
        DmaSetup->DmaChannelCtrl[1].ChanCtrl.Enable =
            CSL_DMA_CHAN_CTRL_ENABLE_ENABLE;
        DmaSetup->DmaChannelCtrl[1].IRQConfig.BlockDone.IRQEnableSet = TRUE;
        DmaSetup->DmaChannelCtrl[1].IRQConfig.FrameDone.IRQEnableSet = TRUE;
    }
    else if (JpegParam->Format == YUV422 && JpegParam->Isplanar == FALSE)
    {
        DmaSetup->DmaChannelCtrl[0].SetupChan = TRUE;
        DmaSetup->DmaChannelCtrl[0].SMemAddr = (Uint32) JpegParam->pOutput;
        DmaSetup->DmaChannelCtrl[0].SMemOfst =
            JpegParam->uImageWidth * BYTES_TO_PIXEL_RATIO_YUV422;
        DmaSetup->DmaChannelCtrl[0].BufAddr = IBuffer_A_Ofst;
        DmaSetup->DmaChannelCtrl[0].BufOfst =
            (MCU_WIDTH_IN_BYTES_YUV422 * JpegParam->uNumMCUPerBlock);
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.YNUM =
            MCU_HEIGHT_IN_PIXELS_YUV422;
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.XNUM =
            JpegParam->uNumMCUPerBlock * BYTES_TO_PIXEL_RATIO_YUV422 *
            MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.YSTEP =
            MCU_HEIGHT_IN_PIXELS_YUV422;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.XSTEP =
            JpegParam->uNumMCUPerBlock * BYTES_TO_PIXEL_RATIO_YUV422 *
            MCU_WIDTH_IN_PIXEL_YUV420_YU422;
        DmaSetup->DmaChannelCtrl[0].Frame.YCNT = JpegParam->YBlkCount;
        DmaSetup->DmaChannelCtrl[0].Frame.XCNT = JpegParam->XBlkCount;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Dir =
            CSL_DMA_CHAN_CTRL_DIR_IBUFTOSYS;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStart =
            CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN0;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStop =
            CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN0;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Linked =
            CSL_DMA_CHAN_CTRL_LINKED_DISABLED;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.SWTrigger =
            CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Enable =
            CSL_DMA_CHAN_CTRL_ENABLE_ENABLE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.BlockDone.IRQEnableSet = TRUE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.FrameDone.IRQEnableSet = TRUE;
    }

    else if (JpegParam->Isplanar == TRUE)
    {
        DmaSetup->DmaChannelCtrl[0].SetupChan = TRUE;
        DmaSetup->DmaChannelCtrl[0].BufAddr = IBuffer_A_Ofst;
        DmaSetup->DmaChannelCtrl[0].BufOfst =
            (MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE * JpegParam->uNumMCUPerBlock);
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.YNUM = 1;  // MCU_HEIGHT_IN_PIXEL_SEQUENTIAL_MODE;
        DmaSetup->DmaChannelCtrl[0].ChanBlkSize.XNUM = JpegParam->uNumMCUPerBlock * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE;  // JpegParam->uNumMCUPerBlock 
                                                                                                                        // * 
                                                                                                                        // MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.YSTEP = 1; // MCU_HEIGHT_IN_PIXEL_SEQUENTIAL_MODE;
        DmaSetup->DmaChannelCtrl[0].ChanBlkStep.XSTEP = JpegParam->uNumMCUPerBlock * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE; // JpegParam->uNumMCUPerBlock 
                                                                                                                        // * 
                                                                                                                        // MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE;
        DmaSetup->DmaChannelCtrl[0].Frame.YCNT = JpegParam->YBlkCount;
        DmaSetup->DmaChannelCtrl[0].Frame.XCNT = JpegParam->XBlkCount;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Dir =
            CSL_DMA_CHAN_CTRL_DIR_IBUFTOSYS;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStart =
            CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN0;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.HWStop =
            CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN0;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Linked =
            CSL_DMA_CHAN_CTRL_LINKED_DISABLED;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.SWTrigger =
            CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT;
        DmaSetup->DmaChannelCtrl[0].ChanCtrl.Enable =
            CSL_DMA_CHAN_CTRL_ENABLE_ENABLE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.BlockDone.IRQEnableSet = TRUE;
        DmaSetup->DmaChannelCtrl[0].IRQConfig.FrameDone.IRQEnableSet = TRUE;

        if (JpegParam->YuvComp != Y && JpegParam->Format == YUV420)
        {
            if (JpegParam->YuvComp == U)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              (JpegParam->uImageWidth *
                               JpegParam->uImageHeight));
            else if (JpegParam->YuvComp == V)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              ((JpegParam->uImageWidth *
                                JpegParam->uImageHeight * 5) / 4));
            DmaSetup->DmaChannelCtrl[0].SMemOfst =
                JpegParam->uImageWidth / 2 * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE /
                8;
        }
        else if (JpegParam->YuvComp != Y && JpegParam->Format == YUV422)
        {
            if (JpegParam->YuvComp == U)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              (JpegParam->uImageWidth *
                               JpegParam->uImageHeight));
            else if (JpegParam->YuvComp == V)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              ((JpegParam->uImageWidth *
                                JpegParam->uImageHeight * 3) / 2));
            DmaSetup->DmaChannelCtrl[0].SMemOfst =
                JpegParam->uImageWidth / 2 * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE /
                8;
        }
        else if (JpegParam->YuvComp != Y && JpegParam->Format == YUV444)
        {
            if (JpegParam->YuvComp == U)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              (JpegParam->uImageWidth *
                               JpegParam->uImageHeight));
            else if (JpegParam->YuvComp == V)
                DmaSetup->DmaChannelCtrl[0].SMemAddr =
                    (Uint32) (JpegParam->pOutput +
                              ((JpegParam->uImageWidth *
                                JpegParam->uImageHeight * 2)));
            DmaSetup->DmaChannelCtrl[0].SMemOfst =
                JpegParam->uImageWidth * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE / 8;
        }
        else if (JpegParam->YuvComp == Y)
        {

            DmaSetup->DmaChannelCtrl[0].SMemAddr = (Uint32) JpegParam->pOutput;
            DmaSetup->DmaChannelCtrl[0].SMemOfst =
                JpegParam->uImageWidth * MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE / 8;

        }

    }

    if ((JpegParam->YuvComp == Y && JpegParam->Isplanar == TRUE) ||
        JpegParam->Isplanar == FALSE)
    {
        DmaSetup->DmaChannelCtrl[2].SetupChan = TRUE;
    }
    else
        DmaSetup->DmaChannelCtrl[2].SetupChan = FALSE;

    DmaSetup->DmaChannelCtrl[2].SMemAddr = (Uint32) JpegParam->pInput;
    DmaSetup->DmaChannelCtrl[2].SMemOfst =
        BDMA_TRANSFER_SIZE * SIMCOP_DMA_MAX_XCNT;
    DmaSetup->DmaChannelCtrl[2].BufAddr = BitStrBuff_Ofst;
    DmaSetup->DmaChannelCtrl[2].BufOfst = BDMA_TRANSFER_SIZE;
    DmaSetup->DmaChannelCtrl[2].ChanBlkSize.YNUM = 1;
    DmaSetup->DmaChannelCtrl[2].ChanBlkSize.XNUM = BITSTRBUF_BANK_SIZE;
    DmaSetup->DmaChannelCtrl[2].ChanBlkStep.YSTEP = 1;
    DmaSetup->DmaChannelCtrl[2].ChanBlkStep.XSTEP = BITSTRBUF_BANK_SIZE;
    DmaSetup->DmaChannelCtrl[2].Frame.YCNT = 1;            // (JpegParam->nOupBufsize/(BITSTRBUF_BANK_SIZE*SIMCOP_DMA_MAX_XCNT));
    DmaSetup->DmaChannelCtrl[2].Frame.XCNT = SIMCOP_DMA_MAX_XCNT;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.Dir = CSL_DMA_CHAN_CTRL_DIR_SYSTOIBUF;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.HWStart =
        CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN1;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.HWStop =
        CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN1;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.Linked =
        CSL_DMA_CHAN_CTRL_LINKED_DISABLED;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.SWTrigger =
        CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT;
    DmaSetup->DmaChannelCtrl[2].ChanCtrl.Enable =
        CSL_DMA_CHAN_CTRL_ENABLE_ENABLE;
    DmaSetup->DmaChannelCtrl[2].IRQConfig.BlockDone.IRQEnableSet = TRUE;
    DmaSetup->DmaChannelCtrl[2].IRQConfig.FrameDone.IRQEnableSet = TRUE;

    status = CSL_simcopDmaHwSetup(SimcopDmahndl, DmaSetup);
    free(DmaSetup);
    return status;

}

/* ===================================================================
 *  @func     SIMCOP_Config_decode                                               
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
Jpeg_enc_status SIMCOP_Config_decode(JPEGParams * JpegParam)
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    CSL_SimcopHwSetupCtrl *simcopsetup;

    Uint16 NumBlks;

    Uint8 NumStepsinSequence;

    Uint8 step;

    Bool bPipeExecuted;

    if (JpegParam->Isplanar == FALSE)
    {
        if (JpegParam->Format == YUV420)
            NumBlks =
                (JpegParam->uImageHeight * JpegParam->uImageWidth) /
                (MCU_WIDTH_IN_PIXEL_YUV420_YU422 * MCU_HEIGHT_IN_PIXELS_YUV420 *
                 JpegParam->uNumMCUPerBlock);
        else if (JpegParam->Format == YUV422)
            NumBlks =
                (JpegParam->uImageHeight * JpegParam->uImageWidth) /
                (MCU_WIDTH_IN_PIXEL_YUV420_YU422 * MCU_HEIGHT_IN_PIXELS_YUV422 *
                 JpegParam->uNumMCUPerBlock);
    }
    else
    {
        NumBlks =
            (JpegParam->uImageHeight * JpegParam->uImageWidth) /
            (MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE *
             MCU_HEIGHT_IN_PIXEL_SEQUENTIAL_MODE * JpegParam->uNumMCUPerBlock);
        if (JpegParam->YuvComp != Y)

        {
            if (JpegParam->Format != YUV444)

            {
                if (JpegParam->Format == YUV420)
                    NumBlks /= 4;
                else
                    NumBlks /= 2;
            }
        }

    }

#if PIPELINE
    NumStepsinSequence = 2;
    bPipeExecuted = ((NumBlks - 1) / NumStepsinSequence == 0) ? FALSE : TRUE;
#else
    NumStepsinSequence = 3;
#endif
    simcopsetup =
        (CSL_SimcopHwSetupCtrl *) malloc(sizeof(CSL_SimcopHwSetupCtrl));
    if (NULL == simcopsetup)
        printf("Failed to allocate memory for SimcopHwSetup structure\n");
    /* Set values to reset value to make sure that unused register fields
     * don't have random values */
    memset(simcopsetup, 0, sizeof(CSL_SimcopHwSetupCtrl));

    simcopsetup->StandbyMode = STANDBY_SMART;
    simcopsetup->LdcCtrl.LdcRTagCnt = 6;
    simcopsetup->LdcCtrl.LdcRTagOfst = 6;
    /* Set values of used registers */
    simcopsetup->ClkCtrl.Dma = CLKCTRL_ENABLE_REQ;
    simcopsetup->ClkCtrl.Dct = CLKCTRL_ENABLE_REQ;
    simcopsetup->ClkCtrl.Vlcdj = CLKCTRL_ENABLE_REQ;

#if !PIPELINE
    step = 0;
    simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
    VLCDJE_to_Ping(simcopsetup, step);
    simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_1;   /* Link with
                                                                 * Next Step */

    step = 1;
    simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
    DCT_to_Ping(simcopsetup, step);
    simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_2;

    step = 2;
    simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
    DMA_to_Ping(simcopsetup, step);
    simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_0;

    simcopsetup->HwseqCtrl.HwSeqStepCounter = (NumBlks * NumStepsinSequence);
#else
    switch (JpegParam->PipelineStage)
    {
        case PIPE_UP:
            step = 0;
            simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
            VLCDJE_to_Ping(simcopsetup, step);
            simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_1;   /* Link 
                                                                         * with 
                                                                         * Next 
                                                                         * Step */

            step = 1;
            simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
            VLCDJE_to_Pong(simcopsetup, step);
            DCT_to_Ping(simcopsetup, step);
            simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_0;

            simcopsetup->HwseqCtrl.HwSeqStepCounter =
                NumBlks > NumStepsinSequence ? NumStepsinSequence : NumBlks;

            break;
        case PIPE:

            step = 0;
            simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
            VLCDJE_to_Ping(simcopsetup, step);
            DCT_to_Pong(simcopsetup, step);
            DMA_to_Ping(simcopsetup, step);
            simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_1;

            step = 1;
            simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
            VLCDJE_to_Pong(simcopsetup, step);
            DCT_to_Ping(simcopsetup, step);
            DMA_to_Pong(simcopsetup, step);
            simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_0;

            simcopsetup->HwseqCtrl.HwSeqStepCounter =
                NumBlks > NumStepsinSequence ? NumBlks - NumStepsinSequence : 0;
            // if (JpegParam->YuvComp == U)
            // simcopsetup->HwseqCtrl.HwSeqStepCounter =2;

            break;
        case PIPE_DOWN:
            if (!(NumBlks % NumStepsinSequence))
            {
                step = 0;
                simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
                DCT_to_Pong(simcopsetup, step);
                DMA_to_Ping(simcopsetup, step);

                simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_1;

                step = 1;
                simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
                DMA_to_Pong(simcopsetup, step);
                simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_0;
            }
            else
            {
                step = 0;
                simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;

                DCT_to_Ping(simcopsetup, step);
                if (bPipeExecuted)
                    DMA_to_Pong(simcopsetup, step);
                simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_1;

                step = 1;
                simcopsetup->HwseqCtrl.HwSeqStep[step].SetupStep = TRUE;
                DMA_to_Ping(simcopsetup, step);
                simcopsetup->HwseqCtrl.HwSeqStep[step].Next = NEXTSTEP_0;
            }

            simcopsetup->HwseqCtrl.HwSeqStepCounter = NumStepsinSequence;
            break;
        default:
            break;
    }
#endif

    simcopsetup->BufAccCtrl.Quant = HUFFQUANTACC_VLCDJ_READ;    // Attach
                                                                // quant and
                                                                // huff to
                                                                // Cop
    simcopsetup->BufAccCtrl.Huff = HUFFQUANTACC_VLCDJ_READ;

    simcopsetup->BbmCtrl.BbmSyncChan = 1;                  // DMA Sync
                                                           // Channel 1 used
                                                           // by BBM
    // <change> simcopsetup->BbmCtrl.BitStream = BSACC_PINGPONG_DEC;
    // //Bitstream buffer used by HW for JPEG encode
    if ((JpegParam->PipelineStage == PIPE_UP) &&
        (JpegParam->YuvComp == Y || JpegParam->Isplanar == FALSE))
        simcopsetup->BbmCtrl.BitStream = BSACC_COP_COP;
    else
        simcopsetup->BbmCtrl.BitStream = BSACC_PINGPONG_DEC;

    simcopsetup->BbmCtrl.BitStreamXferSize = BSTRANSFERSIZE_2048_BYTES; // Bitstream 
                                                                        // transfer 
                                                                        // size 
                                                                        // to 
                                                                        // SDRAM 
                                                                        // 

    simcopsetup->Irq[0].SetupIrq = TRUE;
    simcopsetup->Irq[1].SetupIrq = TRUE;
    simcopsetup->Irq[2].SetupIrq = TRUE;
    simcopsetup->Irq[3].SetupIrq = TRUE;

    simcopsetup->IrqMode[0] = IRQMODE_ONEOF;
    simcopsetup->IrqMode[1] = IRQMODE_ONEOF;
    simcopsetup->IrqMode[2] = IRQMODE_ONEOF;
    simcopsetup->IrqMode[3] = IRQMODE_ONEOF;

    simcopsetup->Irq[0].DoneIrq.IRQEnableSet = IRQENABLESET_ENABLE_INTERRUPT;
    simcopsetup->Irq[1].OcpErrIrq.IRQEnableSet = IRQENABLESET_ENABLE_INTERRUPT;
    simcopsetup->Irq[2].VlcdjDecodeErrIrq.IRQEnableSet =
        IRQENABLESET_ENABLE_INTERRUPT;
    simcopsetup->Irq[3].Mtcr2OcpErrIrq.IRQEnableSet =
        IRQENABLESET_ENABLE_INTERRUPT;

    status = CSL_ESYS_INVPARAMS;
    status = CSL_simcopHwSetup(Simcophndl, simcopsetup);

  EXIT:
    return status;
}

/* ===================================================================
 *  @func     start_hwseq                                               
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
void start_hwseq(JPEGParams * JpegParam)
{
    CSL_HwSeqStartStopType value1;

    Uint16 data1 = 6;

    value1 = HWSEQSTARTSTOP_DO;
    // <change> trigger first DMA to BSMEM
    if (JpegParam->PipelineStage == PIPE_UP)
    {
        CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_BITSTREAM, &data1);

        data1 = 1;
        CSL_VlcdjHwControl(Vlcdhndl, CSL_VLCDJ_CMD_CONFIGBANK, &data1);
    }

    CSL_simcopHwControl(Simcophndl, CSL_SIMCOP_CMD_SET_HW_SEQ_START, &value1);
}

/* ===================================================================
 *  @func     Ducati_Config                                               
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
void Ducati_Config()
{
    ducati_interrupt_enable();
    iss_interrupt_enable();
}

/* ===================================================================
 *  @func     JPEG_init_decode                                               
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
Jpeg_enc_status JPEG_init_decode()
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    reset_buf((unsigned int *) 0x50021000);
    reset_buf((unsigned int *) 0x50028000);
    reset_buf((unsigned int *) 0x50029000);
    reset_buf((unsigned int *) 0x5002A000);
    reset_buf((unsigned int *) 0x5002B000);
    reset_buf((unsigned int *) 0x5002C000);
    reset_buf((unsigned int *) 0x5002D000);
    reset_buf((unsigned int *) 0x5002E000);
    reset_buf((unsigned int *) 0x5002F000);

    // Populate input image in SDRAM

    JPEG_EXIT_IF((get_handle()) != JPEG_ENC_SOK, JPEG_ENC_INVALIDHANDLE);

    JPEG_EXIT_IF((CSL_simcopInit(Simcophndl) != JPEG_ENC_SOK), JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_VlcdjInit(Vlcdhndl) != JPEG_ENC_SOK), JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_dctInit(Dcthndl) != JPEG_ENC_SOK), JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_simcopDmaInit(SimcopDmahndl) != JPEG_ENC_SOK),
                 JPEG_ENC_FAILED);

  EXIT:
    return status;

}

/* ===================================================================
 *  @func     JPEG_config_decode                                               
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
Jpeg_enc_status JPEG_config_decode(JPEGParams * JpegParam)
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    JpegParam->uNumMCUPerBlock = getNMCU(JpegParam);
    NUMMCUPERBLOCK = JpegParam->uNumMCUPerBlock;
    /* Configure Individual modules - these functions are defined in thsi
     * file itself */
#if !PIPELINE
    Ducati_Config();
    JPEG_EXIT_IF((SIMCOP_Config_decode(JpegParam) != JPEG_ENC_SOK), JPEG_ENC_INVPARAMS);    // calculation 
                                                                                            // of 
                                                                                            // total 
                                                                                            // count 
                                                                                            // HWSEQ 
                                                                                            // could 
                                                                                            // be 
                                                                                            // done 
                                                                                            // with 
                                                                                            // in 
                                                                                            // this 
                                                                                            // fucntion 
                                                                                            // 
    JPEG_EXIT_IF((DMA_Config_Decode(JpegParam) != JPEG_ENC_SOK), JPEG_ENC_INVPARAMS);   // calculation 
                                                                                        // of 
                                                                                        // xCNT& 
                                                                                        // yCNT 
                                                                                        // could 
                                                                                        // be 
                                                                                        // done 
                                                                                        // with 
                                                                                        // in 
                                                                                        // this 
                                                                                        // function.
    JPEG_EXIT_IF((DCT_Config_Decode(JpegParam) != JPEG_ENC_SOK),
                 JPEG_ENC_INVPARAMS);
    JPEG_EXIT_IF((VLCDJ_Config_Decode(JpegParam) != JPEG_ENC_SOK),
                 JPEG_ENC_INVPARAMS);
#else
    JPEG_EXIT_IF((SIMCOP_Config_decode(JpegParam) != JPEG_ENC_SOK), JPEG_ENC_INVPARAMS);    // calculation 
                                                                                            // of 
                                                                                            // total 
                                                                                            // count 
                                                                                            // HWSEQ 
                                                                                            // could 
                                                                                            // be 
                                                                                            // done 
                                                                                            // with 
                                                                                            // in 
                                                                                            // this 
                                                                                            // fucntion 
                                                                                            // 
    switch (JpegParam->PipelineStage)
    {
        case PIPE_UP:
            Ducati_Config();
            JPEG_EXIT_IF((DMA_Config_Decode(JpegParam) != JPEG_ENC_SOK), JPEG_ENC_INVPARAMS);   // calculation 
                                                                                                // of 
                                                                                                // xCNT& 
                                                                                                // yCNT 
                                                                                                // could 
                                                                                                // be 
                                                                                                // done 
                                                                                                // with 
                                                                                                // in 
                                                                                                // this 
                                                                                                // function. 
                                                                                                // 
            JPEG_EXIT_IF((DCT_Config_Decode(JpegParam) != JPEG_ENC_SOK),
                         JPEG_ENC_INVPARAMS);
            JPEG_EXIT_IF((VLCDJ_Config_Decode(JpegParam) != JPEG_ENC_SOK),
                         JPEG_ENC_INVPARAMS);
            break;
    }
#endif
    /* DCT config should be called only during Pipe up of all components in
     * planar mode */

  EXIT:
    return status;

}

/* Kick start JPEG encoding & waits for the completion then returns */
/* when interrupt concept comes in this might need to chage */
/* ===================================================================
 *  @func     JPEG_run_decode                                               
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
Jpeg_enc_status JPEG_run_decode(JPEGParams * JpegParam)
{

    Uint16 data, SetStepCount, CurrentStepCount;

    Uint32 numbytes;

    Jpeg_enc_status status = JPEG_ENC_SOK;

    static Uint16 PipeBx, PipeBy;

    /* Kick start HWSEQ */
    start_hwseq(JpegParam);
    while (bDoneIrqOccured == FALSE) ;
    bDoneIrqOccured = FALSE;
    CSL_simcopGetHWStatus(Simcophndl, CSL_SIMCOP_QUERY_HW_SEQ_STEP_COUNTER,
                          &SetStepCount);
    CSL_simcopGetHWStatus(Simcophndl,
                          CSL_SIMCOP_QUERY_HW_SEQ_STEP_COUNTER_CURRENT,
                          &CurrentStepCount);
    if (SetStepCount == CurrentStepCount)
    {
        printf("%d steps done\n", CurrentStepCount);
        status = JPEG_ENC_SOK;
    }
    else
    {
        printf("HWSeq Steps NOT completed\n");
        status = JPEG_ENC_FAILED;
        goto EXIT;
    }

  EXIT:
    return status;

}

/* ===================================================================
 *  @func     JPEG_deinit_decode                                               
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
Jpeg_enc_status JPEG_deinit_decode()
{

    Jpeg_enc_status status = JPEG_ENC_SOK;

    JPEG_EXIT_IF((CSL_VlcdjClose(Vlcdhndl) != JPEG_ENC_SOK), JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_dctClose(Dcthndl) != JPEG_ENC_SOK), JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_simcopDmaClose(SimcopDmahndl) != JPEG_ENC_SOK),
                 JPEG_ENC_FAILED);
    JPEG_EXIT_IF((CSL_simcopClose(Simcophndl) != JPEG_ENC_SOK),
                 JPEG_ENC_FAILED);
  EXIT:
    return status;

}
