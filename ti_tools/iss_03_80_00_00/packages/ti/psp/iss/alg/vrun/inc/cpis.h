/** ==================================================================
 *  @file   cpis.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/vrun/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ======================================================================== */
/* TEXAS INSTRUMENTS, INC.  */
/* */
/* CPIS Signal Processing Library */
/* */
/* This library contains proprietary intellectual property of Texas */
/* Instruments, Inc.  The library and its source code are protected by */
/* various copyrights, and portions may also be protected by patents or */
/* other legal protections.  */
/* */
/* This software is licensed for use with Texas Instruments OMAP4 */
/* based application processors.  This license was provided to you prior */
/* to installing the software.  You may review this license by consulting */
/* the file TI_license.PDF which accompanies the files in this library.  */
/* */
/* --------------------------------------------------------------------------------------------- 
 */
/* */
/* NAME */
/* cpis.h -- cpis header file */
/* */
/* DESCRIPTION */
/* This file defines all the APIs that directly called by the app */
/* on the SIMCOP computation unit and also the GPP apis */
/* using natural C implementation */
/* */
/* REV */
/* version 0.0.1: June 16, 2010 */
/* Initial version */
/* */
/* version 0.0.2: July 23, 2010 */
/* Extends support for multiple handles and cloning along vertical direction */
/* */
/* ---------------------------------------------------------------------------------------------- 
 */
/* Copyright (c) 2010 Texas Instruments, Incorporated.  */
/* All Rights Reserved.  */
/* ========================================================================= */

#ifndef _CPISLIB_H
#define _CPISLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CCS_PROJECT

#include "tistdtypes.h"
#include "imx_run.h"

#else

#include "../../ip_run/inc/imx_run.h"
#include "../../jpeg_enc/inc/msp.h"


#define TIMM_OSAL_MallocExtn MSP_MallocExtn
#define TIMMOSAL_MEM_SEGMENT_EXT (0)

#endif

//#define DISABLE_SRC_INIT
#define DISABLE_DST_INIT
    //#define CMD_COEFF_MEM_INIT

#define CPIS_MAX_SRC_CHANNELS 4
#define CPIS_MAX_DST_CHANNELS 4

#define CPIS_MAX_HANDLES 8

/* Error symbols used by the library */
#define CPIS_INIT_ERROR             1
#define CPIS_NOTINIT_ERROR          2
#define CPIS_UNPACK_ERROR           3
#define CPIS_NOSUPPORTFORMAT_ERROR  4
#define CPIS_NOSUPPORTDIM_ERROR     5
#define CPIS_PACK_ERROR             6
#define CPIS_MAXNUMFUNCREACHED      7
#define CPIS_OUTOFMEM               8
#define CPIS_NOSUPPORTANGLE_ERROR   9
#define CPIS_NOSUPPORTOP_ERROR     10

/* Various data formats supported by the library */
    typedef enum {
        CPIS_YUV_420P=0, /* Planar symbols must be listed first */
        CPIS_YUV_422P,
        CPIS_YUV_444P,
        CPIS_YUV_411P,
        CPIS_YUV_422VP, /* Vertical subsampling */
        CPIS_RGB_P,
        CPIS_BAYER_P,
        CPIS_YUV_422IBE,
        CPIS_YUV_422ILE,
        CPIS_RGB_555,
        CPIS_RGB_565,
        CPIS_BAYER,
        CPIS_YUV_444IBE,
        CPIS_YUV_444ILE,
        CPIS_RGB_888,
        CPIS_YUV_GRAY,
        CPIS_8BIT,
        CPIS_16BIT,
        CPIS_32BIT,
        CPIS_64BIT,
        CPIS_U8BIT,
        CPIS_U16BIT,
        CPIS_U32BIT,
        CPIS_U64BIT,
        CPIS_1BIT,
        CPIS_YUV_420SP
    } CPIS_Format;

/* Structures used to convey information regarding the various blocks */
    typedef struct {
        Uint32 width; /* In number of pixels */
        Uint32 height; /* In number of lines */
    } CPIS_Size;

    typedef struct {
        Uint8 *ptr;
        Uint32 stride; /* In number of bytes */
    } CPIS_Buffer;

/* Base paramters common to all APIs */
    typedef struct {
        CPIS_Format srcFormat[CPIS_MAX_SRC_CHANNELS];
        CPIS_Buffer srcBuf[CPIS_MAX_SRC_CHANNELS];
        CPIS_Format dstFormat[CPIS_MAX_DST_CHANNELS];
        CPIS_Buffer dstBuf[CPIS_MAX_DST_CHANNELS];
        CPIS_Size   roiSize;
        CPIS_Size   procBlockSize;
        Uint16      numInput;
        Uint16      numOutput;
        Uint8       dmaOnly;
    } CPIS_BaseParms;

    typedef void *CPIS_Handle;

/* Sobel API params */
#define ANGLE_NO_EDGE   15
#define M_FACTOR        64

/* Double Thresholding API params */
#define POSSIBLE_EDGE 127
#define NON_EDGE             0

    typedef struct {
        Uint8 minTh1;
        Uint8 minTh2;
        Uint8 ver;
    } CPIS_SobelXTRParms;

/* XTR Multi Threshold API Parameters */
    typedef struct {
        Uint8 highTh1;
        Uint8 lowTh1;
        Uint8 highTh2;
        Uint8 lowTh2;
        Uint8 highTh3;
        Uint8 lowTh3;
    } CPIS_MultiThParms;

/* XTR Neighbor Threshold API Parameters */
    typedef struct {
        Uint8 highTh;
        Uint8 lowTh;
        Uint8 neighborTh;
    } CPIS_NeighborThParms;

/* Structures to convey the type of mask and morphology operation */
    typedef enum {
        CPIS_DILATION = 0,
        CPIS_EROSION  = 1
    } CPIS_MorphOp;

/* Morphology API Parameters */
    typedef struct {
        CPIS_Size    maskSize;
        Uint8       *maskPtr;
        CPIS_MorphOp opType;
    } CPIS_MorphologyParms;

/* Horizontal Recursive filter API params */
    typedef struct {
        Int8 *scratch_buf;
        Int16 scratchBufStride;
        Int16 weight;
    } CPIS_RecFilterHorParms;

/* Vertical Recursive filter API params */
    typedef struct {
        Int8 *scratch_buf;
        Int16 scratchBufStride;
        Int16 weight;
    } CPIS_RecFilterVerParms;

/* Integral Image8 API Parameters */
    typedef struct {
        Uint8 blkHeightCol;
        Uint8 blkWidthRow;
    } CPIS_IntegralImage8Parms;

/* UYVYint to RGBpl conversion API params */
    typedef struct {
        Int16 r_luma;
        Int16 g_luma;
        Int16 b_luma;
        Int16 r_cr;
        Int16 g_cr;
        Int16 b_cr;
        Int16 r_cb;
        Int16 g_cb;
        Int16 b_cb;
        Uint8 yuv444Out;
    } CPIS_UYVYint2YUV444plRGBplParms;

/* Image Convolution API Parameters */
    typedef struct {
        CPIS_Size maskSize;
        Uint8     maskShift;
        Int16    *maskPtr;
    } CPIS_ImgConvParms;

/* Double Thresholding API Parameters */
    typedef struct {
        Uint8 lowThresh;
        Uint8 highThresh;
    } CPIS_DoubleThreshParms;

/* Non Max Suppression API params */
    typedef struct {
        Int16  threshold;
        Uint16 maskWidth;
        Uint16 maskHeight;
    } CPIS_NonMaxSuppParms;

    typedef struct {
        Uint8 UpDwnSample; // 0 for U, V upsample, 1 for Y downsample
    } CPIS_NV12toYUV444plParms;

/* YCbCr to Hue conversion API params */
    typedef struct {
        Uint8 hueAddition;
        Uint8 minHueTh;
        Uint8 maxHueTh;
        Uint8 minLumTh;
        Uint8 maxLumTh;
        Uint8 maskVal;
    } CPIS_YCbCr2HueParms;

/* YCbCr to RGB conversion API params */
    typedef struct  {
        Int16 r_luma;
        Int16 g_luma;
        Int16 b_luma;
        Int16 r_cr;
        Int16 g_cr;
        Int16 b_cr;
        Int16 r_cb;
        Int16 g_cb;
        Int16 b_cb;
        Int16 scaleDown;
    } CPIS_YCbCr2RGBParms;

    typedef struct {
        Uint16 threshold;
        Uint8  d_type;           //0 -> byte; 1 -> short
        Uint8  op;               //0 -> <= ; 1 -> >=
        Uint8  thr_value_select; //0 -> min/max; 1 -> threshold
    }CPIS_ImgThresParms;

    typedef struct {

        Int32  sat_high; /* Upper threshold, always a positive value, maximum is 32767 */
        Int32  sat_high_set; /* Value to set to if upper threshold exceeded */
        Int32  sat_low; /* Lower threshold, in general should be positive or zero value */
        Int32  sat_low_set; /* Value to set to if lower threshold exceeded */
        Uint16 K;
        Uint16 nplus1;
        Uint8 *tempBuf1;
        Uint8 *tempBuf2;
        Uint8 *tempBuf3;
        Uint8 *tempBuf4;
        Uint8 *ScratchBuf;

    }CPIS_HarrisCornersParms;

    typedef struct {

        Uint16 K; /* Q.15, typically 32767*0.04= 1310 */
        Int32  sat_high; /* Upper threshold, always a positive value, maximum is 32767 */
        Int32  sat_high_set; /* Value to set to if upper threshold exceeded */
        Int32  sat_low; /* Lower threshold, in general should be positive or zero value */
        Int32  sat_low_set; /* Value to set to if lower threshold exceeded */
        Uint8 *scratchBuf;

    } CPIS_ComputeCornersParms;

    typedef struct {

        Uint16 nplus1;

    }CPIS_ComputeBlockMaximaParms;

    typedef struct {

        Uint16 nplus1;

    }CPIS_NmsStep1Parms;

/* Sobel API params */
    typedef struct {
        //Uint16 qShift;
        //Int32 sat_high;
        //Int32 sat_high_set;
        //Int32 sat_low;
        //Int32 sat_low_set;
        Uint8 filterW;
        Uint8 filterH;
    } CPIS_SobelParms;

    typedef enum {
        DO_NOT_COMPUTE= 0, /* Do nothing, no magnitude is produced */
        SUM_ABS_XY, /* Compute magnitude of gradient as Gmag = (|Gx| + |Gy|) */
        SUM_SQUARE_XY, /* Compute magnitude as Gmag = (Gx^2 + Gy^2) */
        SQRT_SUM_SQUARE_XY /* Compute magnitude as Gmag = Sqrt((Gx^2 + Gy^2)) Not currently supported */
    } CPIS_MagnitudeType;

    typedef struct {
        Uint16             x_qShift; /* Amount of shift applied to Gx */
        Int32              x_sat_high; /* higher bound limit used for saturation  of Gx */
        Int32              x_sat_high_set; /* higher bound set value used for staturation of Gx */
        Int32              x_sat_low; /* lower bound limit used for saturation  of Gx */
        Int32              x_sat_low_set; /* lower bound set value used for staturation of Gx */
        Uint16             y_qShift; /* Amount of shift applied to Gy */
        Int32              y_sat_high; /* higher bound limit used for saturation  of Gy */
        Int32              y_sat_high_set; /* higher bound set value used for staturation of Gy */
        Int32              y_sat_low; /* lower bound limit used for saturation  of Gy */
        Int32              y_sat_low_set; /* lower bound set value used for staturation of Gy */
        CPIS_MagnitudeType magnitudeType; /* Method used to compute the magnitude of the gradient Gmag */
        Uint16             magn_qShift; /* Amount of shift applied to Gmag */
        Int32              magn_sat_high; /* higher bound limit used for saturation  of Gmag */
        Int32              magn_sat_high_set; /* higher bound set value used for staturation of Gmag */
        Int32              magn_sat_low; /* lower bound limit used for saturation  of Gmag */
        Int32              magn_sat_low_set; /* lower bound set value used for staturation of Gy */

    }CPIS_XYGradientsAndMagnitudeParms;

/* APIs supported by the library */
    Int32 CPIS_init();

    Int32 CPIS_deInit();

    Int32 CPIS_start(CPIS_Handle handle);

    Int32 CPIS_wait(CPIS_Handle handle);

    Int32 CPIS_delete(CPIS_Handle handle);

    void CPIS_UpdateAddresses(CPIS_Handle cpisHandle,  CPIS_BaseParms *base);

    IMX_run *CPIS_gethandle(void *ptrhandle);

    IMX_run *CPIS_addhandle(void *ptrhandle);

    void CPIS_addUpdateAddressFn(void *ptrhandle, void *fn);

    void CPIS_SIMCOP_Reconfigure();

    void CPIS_CMD_COEFF_MEM_Init();

    Int32 CPIS_clubHandle(void *cpisHandle);


    Int32 CPIS_sobelAngle(CPIS_Handle *ptrhandle,
                          CPIS_BaseParms *base,
                          CPIS_SobelXTRParms *params);

    Int32 CPIS_multiThreshold(CPIS_Handle *ptrhandle,
                              CPIS_BaseParms *base,
                              CPIS_MultiThParms *params);

    Int32 CPIS_neighborThreshold(CPIS_Handle *ptrhandle,
                                 CPIS_BaseParms *base,
                                 CPIS_NeighborThParms *paramsNeighbor);

    Int32 CPIS_morphology(CPIS_Handle *ptrhandle,
                          CPIS_BaseParms *base,
                          CPIS_MorphologyParms *paramsMorph);

    Int32 CPIS_recFilterHor(CPIS_Handle *ptrhandle,
                            CPIS_BaseParms *base,
                            CPIS_RecFilterHorParms *params);

    Int32 CPIS_recFilterVer(CPIS_Handle *ptrhandle,
                            CPIS_BaseParms *base,
                            CPIS_RecFilterVerParms *params);

    Int32 CPIS_integralImage8(CPIS_Handle *ptrhandle,
                              CPIS_BaseParms *base,
                              CPIS_IntegralImage8Parms *paramsII8);

    Int32 CPIS_UYVYint2YUV444plRGBpl(CPIS_Handle *ptrhandle,
                                     CPIS_BaseParms *base,
                                     CPIS_UYVYint2YUV444plRGBplParms *params);

    Int32 CPIS_NV12toYUV444pl(CPIS_Handle *ptrhandle,
                              CPIS_BaseParms *base,
                              CPIS_NV12toYUV444plParms *params);

    Int32 CPIS_YCbCr2Hue(CPIS_Handle *ptrhandle,
                         CPIS_BaseParms *base,
                         CPIS_YCbCr2HueParms *params);

    Int32 CPIS_YCbCr2RGBpl(CPIS_Handle *ptrhandle,
                           CPIS_BaseParms *base,
                           CPIS_YCbCr2RGBParms *params);

    Int32 CPIS_imageConvolution(CPIS_Handle *ptrhandle,
                                CPIS_BaseParms *base,
                                CPIS_ImgConvParms *paramsImgConv);

    Int32 CPIS_doubleThresholding(CPIS_Handle *ptrhandle,
                                  CPIS_BaseParms *base,
                                  CPIS_DoubleThreshParms *paramsDoubleThresh);

    Int32 CPIS_ExtractLumaFromUYVYint(CPIS_Handle *ptrhandle,
                                      CPIS_BaseParms *base);

    Int32 CPIS_UYVYint2YUV420pl(CPIS_Handle *ptrhandle,
                                CPIS_BaseParms *base);

    Int32 CPIS_nonMaxSupp(CPIS_Handle *ptrhandle,
                          CPIS_BaseParms *base,
                          CPIS_NonMaxSuppParms *params);

    Int32 CPIS_cannyNonMaxSupp(CPIS_Handle *ptrhandle,
                               CPIS_BaseParms *base);

    Int32 CPIS_ImageMin(CPIS_Handle *ptrhandle,
                        CPIS_BaseParms *base);

    Int32 CPIS_ImageMax(CPIS_Handle *ptrhandle,
                        CPIS_BaseParms *base);

    Int32 CPIS_ImageThreshold(CPIS_Handle *ptrhandle,
                              CPIS_BaseParms *base,
                              CPIS_ImgThresParms *params);

    Int32 CPIS_HarrisCorners(CPIS_Handle *ptrhandle,
                             CPIS_BaseParms *base,
                             CPIS_HarrisCornersParms *params);

    Int32 CPIS_ComputeCorners (CPIS_Handle *ptrhandle,
                               CPIS_BaseParms *base,
                               CPIS_ComputeCornersParms *params);

    Int32 CPIS_ComputeBlockMaxima (CPIS_Handle *ptrhandle,
                                   CPIS_BaseParms *base,
                                   CPIS_ComputeBlockMaximaParms *params);

    Int32 CPIS_NmsStep1(CPIS_Handle *ptrhandle,
                        CPIS_BaseParms *base,
                        CPIS_NmsStep1Parms *params);

    Int32 CPIS_sobelMN(CPIS_Handle        *ptrhandle,
                       CPIS_BaseParms     *base,
                       CPIS_SobelParms    *params);

    Int32 CPIS_XYGradientsAndMagnitude (CPIS_Handle *ptrhandle,
                                        CPIS_BaseParms *base,
                                        CPIS_XYGradientsAndMagnitudeParms *params);

    Int32 GPP_CPIS_sobelAngle(CPIS_BaseParms *baseRef,
                              CPIS_SobelXTRParms *params);

    Int32 GPP_CPIS_multiThreshold(CPIS_BaseParms *baseRef,
                                  CPIS_MultiThParms *params);

    Int32 GPP_CPIS_neighborThreshold(CPIS_BaseParms *baseNeighborRef,
                                     CPIS_NeighborThParms *paramsNeighbor);

    Int32 GPP_CPIS_morphology(CPIS_BaseParms *baseMorphRef,
                              CPIS_MorphologyParms *paramsMorph);

    Int32 GPP_CPIS_recFilterHor(CPIS_BaseParms *baseRef,
                                CPIS_RecFilterHorParms *params);

    Int32 GPP_CPIS_recFilterVer(CPIS_BaseParms *baseRef,
                                CPIS_RecFilterVerParms *params);

    Int32 GPP_CPIS_integralImage8(CPIS_BaseParms *baseII8Ref,
                                  CPIS_IntegralImage8Parms *paramsII8);

    Int32 GPP_CPIS_UYVYint2YUV444plRGBpl(CPIS_BaseParms *baseRef,
                                         CPIS_UYVYint2YUV444plRGBplParms *params);

    Int32 GPP_CPIS_NV12toYUV444pl(CPIS_BaseParms *base,
                                  CPIS_NV12toYUV444plParms *params);

    Int32 GPP_CPIS_YCbCr2Hue(CPIS_BaseParms *base,
                             CPIS_YCbCr2HueParms *params);

    Int32 GPP_CPIS_YCbCr2RGBpl(CPIS_BaseParms *base,
                               CPIS_YCbCr2RGBParms *params);

    Int32 GPP_CPIS_imageConvolution(CPIS_BaseParms *baseImgConvRef,
                                    CPIS_ImgConvParms *paramsImgConv);

    Int32 GPP_CPIS_doubleThresholding(CPIS_BaseParms *baseDoubleThreshRef,
                                      CPIS_DoubleThreshParms *paramsDoubleThresh);

    Int32 GPP_CPIS_ExtractLumaFromUYVYint(CPIS_BaseParms *base);

    Int32 GPP_CPIS_UYVYint2YUV420pl(CPIS_BaseParms *base);

    Int32 GPP_CPIS_nonMaxSupp(CPIS_BaseParms *base,
                              CPIS_NonMaxSuppParms *params);

    Int32 GPP_CPIS_cannyNonMaxSupp(CPIS_BaseParms *base);

    Int32 GPP_CPIS_ImageThreshold(CPIS_BaseParms *base,
                                  CPIS_ImgThresParms *params);

    Int32 GPP_CPIS_ImageMin(CPIS_BaseParms *base);

    Int32 GPP_CPIS_ImageMax(CPIS_BaseParms *base);

    Int32 GPP_CPIS_HarrisCorners(CPIS_BaseParms *base,
                                 CPIS_HarrisCornersParms *params);

    Int32 GPP_CPIS_ComputeCorners (CPIS_BaseParms *base,
                                   CPIS_ComputeCornersParms *params);

    Int32 GPP_CPIS_ComputeBlockMaxima (CPIS_BaseParms *base,
                                       CPIS_ComputeBlockMaximaParms *params);

    Int32 GPP_CPIS_NmsStep1(CPIS_BaseParms *base,
                            CPIS_NmsStep1Parms *params);

    Int32 GPP_CPIS_NmsStep2(CPIS_BaseParms *base);

    Int32 GPP_CPIS_sobelMN(CPIS_BaseParms     *base,
                           CPIS_SobelParms *params);

    Int32 GPP_CPIS_XYGradientsAndMagnitude (CPIS_BaseParms *base,
                                            CPIS_XYGradientsAndMagnitudeParms *params);

#ifdef __cplusplus
        }
#endif

#endif /* #define _CPISLIB_H */

/* ======================================================================== */
/*                                 End of file                                                                                                                     */
/* --------------------------------------------------------------------------------------------  */
/*            Copyright (c) 2010 Texas Instruments, Incorporated.                                                                            */
/*                           All Rights Reserved.                                                                                                            */
/* ======================================================================== */

