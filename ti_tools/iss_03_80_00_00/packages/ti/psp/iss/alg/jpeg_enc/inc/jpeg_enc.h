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
* @path /alg/jpeg_enc/inc/
*
* @rev 0.1
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 23-Sep 2008 Geetha Ravindran: Initial Release
*!
*! 16-Apr-2009 Phanish: Resource Manager Integration.
*!
*========================================================================= */

#ifndef _JPEG_ENC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _JPEG_ENC_H

#include <ti/psp/iss/hal/iss/simcop/simcop_global/csl_simcop.h>
#include <ti/psp/iss/hal/iss/simcop/dct/csl_dct.h>
#include <ti/psp/iss/hal/iss/simcop/rot/csl_rot.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
#include <ti/psp/iss/hal/iss/simcop/vlcdj/csl_vlcd.h>
#include <ti/psp/iss/hal/iss/simcop/dma/csl_simcop_dma.h>
#include "../msp_jpege.h"

#define MAX_JPEG_HEADER_SIZE 65536
#define MAX_INPUT_SIZE (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * BYTES_TO_PIXEL_RATIO_YUV422)
#define MAX_OUTPUT_SIZE (BITSTRBUF_BANK_SIZE * SIMCOP_DMA_MAX_XCNT * 9)

#define PIPELINE 1

#define DEFAULT_BYTE_ALIGN 4
#define SIXTEEN_BYTE_ALGIN 16

#define EXIF_SAVE_SUPPORT 0
#define EXIF_THMB_SUPPORT 0
#define JFIF_THMB_SUPPORT 0
/* DEBUG MACROS */
#define LOG_RST_LOCATION_ARRAY 0
#define JPEG_THUMBNAIL_ROTATION_SUPPORTED 1
#if (1 == JPEG_THUMBNAIL_ROTATION_SUPPORTED)
#define ROTATED_THUMBNAIL_HEADER_LEN (589+6)
#endif
#define THUMBNAIL_HEADER_LEN (589)

#define MAX_IMAGE_WIDTH 5088
#define MAX_IMAGE_HEIGHT 3392

#define LQFACTOR 0
#define HQFACTOR 95

#define BITSTRBUF_BANK_SIZE 2048
#define BDMA_TRANSFER_SIZE BITSTRBUF_BANK_SIZE
#define SIMCOP_DMA_MAX_XCNT 511
#define RESET0 0

#define VLCDJE_BIT_PTR_RESET_VALUE 8
#define MCU_WIDTH_IN_PIXELS 16
#define MCU_WIDTH_IN_PIXEL_YUV420_YU422 16
#define MCU_WIDTH_IN_BYTES_YUV422 32
#define MCU_WIDTH_IN_BYTES_YUV420 16
#define MCU_HEIGHT_IN_PIXELS_YUV422 8
#define MCU_HEIGHT_IN_PIXELS_YUV420 16
#define MAX_MCUSPERBLK_YUV422 16
#define MAX_MCUSPERBLK_YUV420 8                            /* Max is 10 but
                                                            * optimal is 8 */
#define MAX_MCUPERBLK_ROT_PIPELINE 4
#define BYTES_TO_PIXEL_RATIO_YUV420 1.5
#define BYTES_TO_PIXEL_RATIO_YUV422 2
#define MAX_BYTES_TO_PIXEL_RATIO BYTES_TO_PIXEL_RATIO_YUV422
#define JPEG_ENCODE_HSS_SLICE_SIZE_420 MCU_HEIGHT_IN_PIXELS_YUV420
#define JPEG_ENCODE_HSS_SLICE_SIZE_422 MCU_HEIGHT_IN_PIXELS_YUV422
#define NUMBER_OF_LINES_PER_IPIPE_BUF 16
#define NUMBER_OF_YUV422_SLICES_PER_IPIPE_BUF 2
#define NUMBER_OF_IPIPE_BUFS 2

#define HMEM_WIDTH 8
#define HMEM_HEIGHT 256
#define HMEM_NUM_ENTRIES (HMEM_WIDTH * HMEM_HEIGHT)        /* 4096 bytes =
                                                            * 2048 short */

#define ENC_QMEM_WIDTH 8
#define ENC_QMEM_HEIGHT 16
#define ENC_QMEM_NUM_ENTRIES (ENC_QMEM_WIDTH * ENC_QMEM_HEIGHT)

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
    NO_HEADER,
    JFIF,
    EXIF
} JpegHeaderType;

typedef enum {
    YUV420 = 0,
    YUV422 = 1,
    YUV444 = 2
} Jpeg_Input_Formats;

typedef enum {
    ROTATE_0_DEGREE = 0,
    ROTATE_90_DEGREE = 90,
    ROTATE_180_DEGREE = 180,
    ROTATE_270_DEGREE = 270,
    DATA_SHIFT = 4,
    HORIZANTAL_CIRCULAR_SHIFT = 5
} Jpeg_Rotation_type;

typedef enum {
    HQ = 0,
    HSS = 1
} Jpeg_Usecase;

typedef struct {
    Uint32 ulValue[20];
    Uint32 ulNumOfEntries;
} tSimcopAddrVal;

typedef struct {
    Uint32 ulSliceSizeYUV420_Y;
    Uint32 ulSliceSizeYUV420_UV;
    Uint32 ulSliceSizeYUV422_Y;
    Uint32 ulMCUWidthBy2;
    Uint32 ulIncrementBlockAddrY;
    Uint32 ulIncrementBlockAddrUV;

    Uint32 ulIncrementSmemAddr_Chan0;
    Uint32 ulIncrementSmemAddr_Chan1;
    Uint32 ulImageWidthInBytes;
} tOneTimeCalculations;

typedef struct {
    Uint32 ulImageWidth;                                   // 128p.M
                                                           // limitation
    Uint32 ulImageHeight;                                  // 16p.N
                                                           // limitation
    Uint32 ulImageStrideHorizontal;
    Uint32 ulCaptureWidth;
    Uint32 ulCaptureHeight;
    JpegHeaderType HeaderFormat;
#if EXIF_SAVE_SUPPORT
    MOD_EXIF_INFO_SUPPORTED *pExifData;
    Uint16 usThumbnailImageWidth;
    Uint16 usThumbnailImageHeight;
    Uint8 *pThumbnailUncompressed;
    Uint8 *pThumbnail;
    Uint16 usThumbnailSize;
#endif
    Jpeg_Input_Formats Format;
    Uint8 uImageQualityFactor;
    Uint8 pQuantTab[ENC_QMEM_NUM_ENTRIES];
    Uint16 *pHuffTab;                                      // [HMEM_NUM_ENTRIES];
    Jpeg_Rotation_type Rotation_angle;
    Uint8 *pInput;                                         // Input buffer
                                                           // (SDRAM)
    Uint16 pInverseQuantTab[ENC_QMEM_NUM_ENTRIES];
    Uint8 *pOutput;                                        // Output buffer
                                                           // (SDRAM)
    Uint32 *pOutputRstLocations;                           // Restart Marker
                                                           // Location Array
                                                           // buffer (SDRAM)
    Uint32 *pOutputRstLocationsVirtual;                    // Restart Marker
                                                           // Location Array
                                                           // buffer (SDRAM)
                                                           // used by Ducati
    Uint32 nOutBytes;                                      // bitstream
                                                           // output size
    Uint32 nOupBufsize;
#if PIPELINE
    PipelineStageType PipelineStage;
#endif
    Uint16 uNumMCUPerBlock;
    Int rst_int;
    Int16 usRstInit;
    Uint8 *pOutput_Rotated;
    Jpeg_Usecase Usecase;
    Uint32 XBlkCount;
    Uint32 YBlkCount;
    Uint32 CurrentSliceNumber;
    Uint16 uNumberofProcessingSlices;
    Uint32 uProcessingSliceHeight;
    Uint32 ulSliceHt;
    Bool bReconfigWithinSlice;
    Uint32 ulReconfigBlks;
    Bool bGoodMultipleOfMCUs;
    Bool bRestartEnable;
    Uint32 ulRestartInterval;

    /* Output Params */
    MSP_U32 *pOutputRstLocAndSize;                         // Restart Marker
                                                           // Location and
                                                           // Size Array
                                                           // buffer (SDRAM)
                                                           // to be used by
                                                           // Ducati to
                                                           // rearrange MCUs
    Uint8 *pComment;

    /* To maintain/store Configuration tables */
    tSimcopAddrVal stSimcopAddrVal_PipeUp, stSimcopAddrVal_PipeUpReConfig;
    tSimcopAddrVal stSimcopAddrVal_Pipe;
    tSimcopAddrVal stSimcopAddrVal_PipeDown, stSimcopAddrVal_PipeDownReConfig;
    tSimcopAddrVal stSimcopDma_PipeUp, stSimcopDma_Pipe, stSimcopDma_PipeDown,
        stSimcopDma_PipeUpReconfig;
    tSimcopAddrVal stSimcopRot_PipeUp, stSimcopRot_PipeUpReconfig;

    MSP_U32 nSdmaChannel;
    /* Module handles are defined here and exposed to the lower layers */
    CSL_SimcopHandle Simcophndl;
    CSL_DctHandle Dcthndl;
    CSL_SimcopDmaHandle SimcopDmahndl;
    CSL_VlcdjHandle Vlcdhndl;
    CSL_RotHandle Rothndl;

    // void* pGeneric;
    tOneTimeCalculations *stCalculationVariables;
} JPEGParams;

typedef Int32 Jpeg_enc_status;

/* ===================================================================
 *  @func     set_huff_enc                                               
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
void set_huff_enc(const Uint16 * pHuffTab, CSL_SimcopHandle hSimcop);

/* ===================================================================
 *  @func     set_enc_quant                                               
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
void set_enc_quant(const Uint16 * pQuantTab, CSL_SimcopHandle hSimcop);

/* ===================================================================
 *  @func     reset_buf_enc                                               
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
void reset_buf_enc(CSL_SimcopBufferType buf, CSL_SimcopHandle);

/* ===================================================================
 *  @func     gen_qmat                                               
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
void gen_qmat(int q_factor, unsigned char qmat[128]);

/* ===================================================================
 *  @func     getNMCU_enc                                               
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
void getNMCU_enc(JPEGParams * JpegParam);

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
 *  @func     ROT_Config                                               
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
Jpeg_enc_status ROT_Config(JPEGParams * JpegParam);

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
Jpeg_enc_status JPEG_init(JPEGParams * JpegParam, RM_HANDLE pRMHandle);

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
Jpeg_enc_status JPEG_config(JPEGParams * JpegParam, RM_HANDLE pRMHandle);

/* ===================================================================
 *  @func     JPEG_ReleaseResource                                               
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
Jpeg_enc_status JPEG_ReleaseResource(JPEGParams * JpegParam,
                                     RM_HANDLE pRMHandle);
/* ===================================================================
 *  @func     JPEG_AcquireResource                                               
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
Jpeg_enc_status JPEG_AcquireResource(JPEGParams * JpegParam,
                                     RM_HANDLE pRMHandle);
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
Jpeg_enc_status JPEG_deinit(JPEGParams * JpegParam, RM_HANDLE pRMHandle);

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
Jpeg_enc_status JPEG_run(JPEGParams * JpegParam, void *);

/* ===================================================================
 *  @func     ROT_Rearrange                                               
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
Jpeg_enc_status ROT_Rearrange(JPEGParams * JpegParam, MSP_U32);

#define JPEG_EXIT_IF(_Cond,_ErrorCode) { \
    if ((_Cond)) { \
        status = _ErrorCode; \
        goto EXIT; \
    } \
}

#define JPEG_EXIT_IF_N(_Cond,_ErrorCode,N)  if ((_Cond)) { \
    status = _ErrorCode; \
    goto EXIT_##N; \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
