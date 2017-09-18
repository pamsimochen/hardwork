/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Jpeg_enc.h
*
* This File contains declarations for Jpeg_enc.c
*
* @path 
*
* @rev 0.1
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 23-Sep 2008 Geetha Ravindran: Initial Release
*!
*========================================================================= */

#ifndef _JPEG_DEC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _JPEG_DEC_H
#include "csl_simcop.h"
// #include "csl_rot.h"
#include "csl_dct.h"
#include "simcop.h"
#include "csl_vlcd.h"
#include "csl_simcop_dma.h"

// #include "ISS_Mem_Map.h"
#define PIPELINE 1

#define BITSTRBUF_BANK_SIZE 2048
#define BITSTRBUF_SIZE 4096
#define BDMA_TRANSFER_SIZE BITSTRBUF_BANK_SIZE
#define SIMCOP_DMA_MAX_XCNT 511
#define HUFTAB_ADDRESS     CSL_HUFFMAN_TABLES              // 0x50022000
#define QUANTAB_ADDRESS    CSL_QUANT_TABLES                // 0x50027000
#define BBM_BANK_SIZE 2048
#define RESET0 0

#define MCU_WIDTH_IN_PIXELS 16
#define MCU_WIDTH_IN_PIXEL_YUV420_YU422 16
#define MCU_WIDTH_IN_BYTES_YUV422 32
#define MCU_WIDTH_IN_BYTES_YUV420 16
#define MCU_HEIGHT_IN_PIXELS_YUV422 8
#define MCU_HEIGHT_IN_PIXELS_YUV420 16
#define MAX_MCUSPERBLK_YUV422 16
#define MAX_MCUSPERBLK_YUV420 8                            /* Max is 10 but
                                                            * optimal is 8 */
#define MAX_MCUSPERBLK_SEQUENTIAL_MODE 64
#define MCU_WIDTH_IN_PIXEL_SEQUENTIAL_MODE 8
#define MCU_HEIGHT_IN_PIXEL_SEQUENTIAL_MODE  8
#define BYTES_TO_PIXEL_RATIO_YUV422 2
#define MCU_SIZE_IN_BYTES_SEQUENTIAL_MODE 64

/* STATUS CODES */
#define JPEG_ENC_SOK                 (1)                   /* Success */
#define JPEG_ENC_FAILED           (-1)                     /* Generic failure 
                                                            */
#define JPEG_ENC_NOTSUPPORTED (-2)                         /* Format (YUV,
                                                            * Height, Width
                                                            * or QF not
                                                            * Supported */
#define JPEG_ENC_INVPARAMS        (-3)
#define JPEG_ENC_INITFAILED        (-4)
#define JPEG_ENC_NOMEMORY        (-5)
#define JPEG_ENC_INVALIDHANDLE  (-6)
#define JPEG_ENC_DEINITFAILED    (-7)

typedef enum {
    PIPE_UP,
    PIPE,
    PIPE_DOWN
} PipelineStageType;

typedef enum {
    YUV420 = 0,
    YUV422 = 1,
    YUV444 = 2
} Jpeg_Input_Formats;

typedef enum {
    ROTATE_0_DEGREE = 0,
    ROTATE_90_DEGREE = 1,
    ROTATE_180_DEGREE = 2,
    ROTATE_270_DEGREE = 3,
    DATA_SHIFT = 4,
    HORIZANTAL_CIRCULAR_SHIFT = 5
} Jpeg_Rotation_type;

typedef enum {
    Y,
    U,
    V
} YuvComponentType;

typedef struct {
    Uint16 uImageWidth;                                    // 128p.M
                                                           // limitation
    Uint16 uImageHeight;                                   // 16p.N
                                                           // limitation
    Jpeg_Input_Formats Format;
    Uint16 *pHuffTab;
    Uint16 *pQuantTab;                                     // 
    Uint8 *pInput;                                         // Input buffer
                                                           // (SDRAM)
    Uint8 *pOutput;                                        // Output buffer
                                                           // (SDRAM)
    Uint32 nOutBytes;                                      // bitstream
                                                           // output size
    Uint32 nOupBufsize;
#if PIPELINE
    PipelineStageType PipelineStage;
#endif
    Uint16 uNumMCUPerBlock;
    Uint32 XBlkCount;
    Uint32 YBlkCount;
    Bool Isplanar;
    YuvComponentType YuvComp;
} JPEGParams;

typedef Int16 Jpeg_enc_status;

extern Bool bMtcr2OcpErrIrqOccured, bOcpErrIrqOccured,
    bVlcdjDecodeErrIrqOccured, bDoneIrqOccured;

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
void set_huff(const unsigned short int *pHuffTab);

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
void set_quant(const unsigned short int *pQuantTab);

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
void reset_buf(unsigned int *p);

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
void read_hex_short(char *filename, unsigned short *image, int width,
                    int height);

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
void start_hwseq(JPEGParams * JpegParam);

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
Jpeg_enc_status get_handle();

/* ===================================================================
 *  @func     DMA_Config                                               
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
Jpeg_enc_status DMA_Config(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     DCT_Config                                               
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
Jpeg_enc_status DCT_Config(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     VLCDJ_Config                                               
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
Jpeg_enc_status VLCDJ_Config(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     SIMCOP_Config                                               
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
Jpeg_enc_status SIMCOP_Config(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     JPEG_config                                               
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
Jpeg_enc_status JPEG_config(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     JPEG_init                                               
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
Jpeg_enc_status JPEG_init();

/* ===================================================================
 *  @func     JPEG_run                                               
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
Jpeg_enc_status JPEG_run(JPEGParams * JpegParam);

/* ===================================================================
 *  @func     JPEG_deinit                                               
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
Jpeg_enc_status JPEG_deinit();

#define JPEG_EXIT_IF(_Cond,_ErrorCode) { \
    if ((_Cond)) { \
        status = _ErrorCode; \
        printf ("Error :: %s : %s : %d :: Exiting because : %s\n", \
                __FILE__, __FUNCTION__, __LINE__, #_Cond); \
        goto EXIT; \
    } \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
