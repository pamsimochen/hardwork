/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  CPIS layer prototypes                                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*     NAME                                                                 */
/*        cpisVision.h --                                                   */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the vision functions belonging to the                          */
/*        CoProcessor InfraStructure (CPIS)'s vision library                */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.1  6 June , 2011                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _CPISLIBVISION_H
#define _CPISLIBVISION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <tistdtypes.h>

/* Structure used as input parameter to CPIS_UpdateEWRMeanVar16()
   CPIS_UpdateEWRMeanVar16() updates the exponential running mean and variance values over background pixels in a frame
   using the formulas: 
   updatedMean = (1-weightMean) × previousMean + weightMean × (newestData<<7)
   updatedVar = (1 - weightVar) × previousVar + weightVar × ((newestData<<7) - previousMean)^2

   A bitmap of the foreground is passed to the function so the update is selectively applied
   to background pixels only.  
 */
typedef struct {
    Uint16 weightMean; /* SQ0.15 format used to calculate updatedMean = (1-weightMean) × previousMean + weightMean × newestData*/
    Uint16 weightVar;  /* SQ0.15 format used to calculate updatedVar = (1 - weightVar) × previousVar + weightVar × (newestData - previousMean)^2 */
} CPIS_UpdateEWRMeanVar16Parms;

/* Structure used as input parameter to CPIS_segmentFG()
   CPIS_segmentFG() segments the foreground from the background.
 */
typedef struct {
    Int16 thresholdCameraNoiseSQ15_0; /* Camera noise threshold below which a pixel would be discarded from being classified as foreground */
    Int16 thresholdVarFactorS7_8; /* Square of the number of standard deviation above which a pixel would be considered as foreground */
    Int16 thresholdGlobalS15_0; /* Threshold used to classify a pixel as foreground in the inter-frame difference technique */
    Uint8 motionHistImgDecay; /* Number frames a pixel must remain motionless before being categorized as stable */
    Uint8 enableCompensation; /* 1: Enable brightness compensation, 0: Disable brightness compensation */
} CPIS_SegmentFGParms;

/* Structure used as input parameter to CPIS_fgSegment()
   CPIS_fgSegment() segments the foreground from the background.
 */
typedef struct {
    Int16 thresholdCameraNoiseSQ15_0; /* Camera noise threshold below which a pixel would be discarded from being classified as foreground */
    Int16 thresholdVarFactorS7_8; /* Square of the number of standard deviation above which a pixel would be considered as foreground */
    Int16 thresholdGlobalS15_0; /* Threshold used to classify a pixel as foreground in the inter-frame difference technique */
    Uint8 motionHistImgDecay;
} CPIS_FgSegmentParms;

/* Structure used as input parameter to CPIS_initMeanVarMHI()
   CPIS_initMeanVarMHI() Initializes the Mean, Var, Motion Histogram.
   This API also returns the sume of the Luma pixels values for each block.
 */
typedef struct {
    Int16 scdThrCamNoise; /* Variance init value which is set to SCD_THRESHOLD_CAMERA_NOISE_SQ15_0*/
} CPIS_InitMeanVarMHIParms;

typedef struct {
    Uint16 motionThreshold;
} CPIS_ImDiffIntlvParms;

/* Structure output by function CPIS_segmentFG() for every block processed
   CPIS_segmentFG() segments the foreground from the background.
   Each members of the structure are summation values collected for a particular processing block.
   These values can be converted to average values by dividing by the nubmer of pixels per block.
   Partitioning of a frame into processing block is determined by the processing block size.
   For instance if frame is of size 288x192 and processing block is 32 x 8
   then the frame is partitioned into a grid of 9 x 24 blocks.
   For each block a structure CPIS_SegmentFG_stat is output.
   base.dstBuf[4].ptr points to the arrays of structure CPIS_SegmentFG_stat 
 */
typedef struct {
    Uint32 sumModelPixel; /* sum of all the values in the background's MEAN model */
    Uint32 sumCurFrmPixel; /* sum of all the pixels' value in the current frame */
    Int32 sumStableBGPixel; /* sum of the pixel's values that are classified as stable */
    Uint32 sumStableBGPixelCnt; /* number of pixel's values that are classified as stable */
    Uint32 sumBGSubCnt; /* number of pixels that are classified as foreground by the background subtraction algorithm */
    Uint32 sumIFDCnt; /* number of pixels whose inter-frame differences exceed threshold thresholdGlobalS15_0 */  
    Int32 sumStableBGPriorPixel; /* sum of all the values in the background's MEAN model, prior to compensation */
    Uint32 sumStableBGPriorPixelCnt; /* number of pixel's values that are classified as stable, prior to compensation */
    Uint32 sumBGSubPriorCnt; /* number of pixels that are classified as foreground by the background subtraction algorithm, prior to compensation */
} CPIS_SegmentFG_stat;


/* Structure output by function CPIS_fgSegment() for every block processed
   CPIS_fgSegmentFG() segments the foreground from the background.
   Each members of the structure are summation values collected for a particular processing block.
   These values can be converted to average values by dividing by the nubmer of pixels per block.
   Partitioning of a frame into processing block is determined by the processing block size.
   For instance if frame is of size 288x192 and processing block is 32 x 8
   then the frame is partitioned into a grid of 9 x 24 blocks.
   For each block a structure CPIS_fgSegment_stat is output.
   base.dstBuf[3].ptr points to the arrays of structure CPIS_fgSegment_stat
 */
typedef struct {
    Uint32 sumModelPixel; /* sum of all the values in the background's MEAN model */
    Uint32 sumCurFrmPixel; /* sum of all the pixels' value in the current frame */
    Uint32 numMHImaskPixels; /* number of pixels that are not required to update in MHI*/
    Uint32 sumBGSubCnt; /* number of pixels that are classified as foreground by the background subtraction algorithm */
    Uint32 sumIFDCnt; /* number of pixels whose inter-frame differences exceed threshold thresholdGlobalS15_0 */
    Uint32 dummy1;
    Uint32 dummy2;
    Uint32 dummy3;
} CPIS_FgSegment_stat;

typedef struct {
    Uint16 dummy; /* Really there is no specific parameters needed for this function */
} CPIS_CountNumONpixelsParms;

/* Structure used by function CPIS_calcSobelEdgeMap()
   This function calculates sobel edge map of source frame and also counts number of
   pixels belonging to strong edges.
 */
typedef struct {
    Uint16 qShift;          /* Set to 3*/
    Uint16 edgeThreshold;   /* threshold used to filter out edges */
    Uint8 onEdgeVal;        /* value assigned to pixels that belong to strong sobel edge value */
    Uint8 offEdgeVal;       /* value assigned to pixels that belong to weak sobel edge value */
    Uint8 noEdgeMap;        /* when enabled gives only the number of sobel edges.*/
} CPIS_CalcSobelEdgeMapParms;

typedef struct {
    Uint16 motionT;
    Uint16 bgSubT;
} CPIS_TamperImageScanParms;

typedef struct {
    Uint32 totalPos;
    Uint32 truePos;
    Uint32 fgCount;
    Uint32 sumPixelValue;
} CPIS_TamperImageScan_stat;

typedef enum {
    DO_NOT_COMPUTE= 0, /* Do nothing, no magnitude is produced */
    SUM_ABS_XY, /* Compute magnitude of gradient as Gmag = (|Gx| + |Gy|) */
    SUM_SQUARE_XY, /* Compute magnitude as Gmag = (Gx^2 + Gy^2) */
    SQRT_SUM_SQUARE_XY /* Compute magnitude as Gmag = Sqrt((Gx^2 + Gy^2)) Not currently supported */
} CPIS_MagnitudeType;

typedef struct {
    Uint16 x_qShift; /* Amount of shift applied to Gx */
    Int32 x_sat_high; /* higher bound limit used for saturation  of Gx */
    Int32 x_sat_high_set; /* higher bound set value used for staturation of Gx */
    Int32 x_sat_low; /* lower bound limit used for saturation  of Gx */
    Int32 x_sat_low_set; /* lower bound set value used for staturation of Gx */
    Uint16 y_qShift; /* Amount of shift applied to Gy */
    Int32 y_sat_high; /* higher bound limit used for saturation  of Gy */
    Int32 y_sat_high_set; /* higher bound set value used for staturation of Gy */
    Int32 y_sat_low; /* lower bound limit used for saturation  of Gy */
    Int32 y_sat_low_set; /* lower bound set value used for staturation of Gy */
    CPIS_MagnitudeType magnitudeType; /* Method used to compute the magnitude of the gradient Gmag */
    Uint16 magn_qShift; /* Amount of shift applied to Gmag */
    Int32 magn_sat_high; /* higher bound limit used for saturation  of Gmag */
    Int32 magn_sat_high_set; /* higher bound set value used for staturation of Gmag */
    Int32 magn_sat_low; /* lower bound limit used for saturation  of Gmag */
    Int32 magn_sat_low_set; /* lower bound set value used for staturation of Gy */
} CPIS_XyGradientsAndMagnitudeParms;

/* NMS params */
Int32 CPIS_getNMSBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

typedef struct {
    Int16  threshold; /* threshold on the value for non maximum suppression */
    Uint16 maskWidth; /* mask filter width */
    Uint16 maskHeight; /* mask filter height */
} CPIS_NMSParms;

typedef struct {
    Int16 hueAddition; /* constant added to the Hue output */
} CPIS_YCbCr2HueParms;

typedef struct {
    Int16 minHueTh; /* minimum Threshold of Hue for assigning maskVal in mask plane */
    Int16 maxHueTh; /* maximum Threshold of Hue for assigning maskVal in mask plane. */
    Int16 minLumTh; /* minimum Threshold of Y(Luma) for assigning maskVal in mask plane. */
    Int16 maxLumTh; /* maximum Threshold of Y(Luma) for assigning maskVal in mask plane. */
    Int16 maskVal; /* Value to set in the mask plane when Hue and Luma in specified threshold limits. */
} CPIS_Hue2MaskParms;

#define POSSIBLE_EDGE 127
#define NON_EDGE      0

typedef struct {
    Uint8 lowThresh;
    Uint8 highThresh;
} CPIS_doubleThresholdingParms;

typedef enum {
    CPIS_DILATION = 0,
    CPIS_EROSION  = 1
} CPIS_MorphOp;

typedef struct {
	 Uint32 maskWidth; /* mask filter width */
	 Uint32 maskHeight;
     Uint8  *maskPtr;
     CPIS_MorphOp opType;
} CPIS_morphologyParms;

/* Integral image params */
typedef struct {
    Uint16 *scratch; /* scratch buffer of size 2*2*base->roiSize.width * base->roiSize.height */
    Uint16 stage;   /* Image integral stage: 1 or 2 */
} CPIS_IntegralImageParms;

/* Col integral image params, dummy structure */
typedef struct {
    Uint16 dummy; /* not used */
} CPIS_IntegralImageColParms;

/* Row integral image params, dummy structure */
typedef struct {
    Uint16 dummy; /* not used */
} CPIS_IntegralImageRowParms;

/* computecorners params */
typedef struct {

    Bool isInputXYgradient; /* FALSE: input is expected to be one 8-bit grayscale image, TRUE: input is one X gradient plane and one Y gradient plane */
    Uint16 gradientBitDepth; /* gradient bit depth can be 8,9,10,11 up to 16, internally used to right shift the square of the gradient
                                 if gradient bit depth is 11 then square of gradient will be right shifted by 2*(gradientBitDepth-8)= 6 */
    Uint16 K; /* Q.15, typically 32767*0.04= 1310 */
    Int32 sat_high; /* Upper threshold, always a positive value, maximum is 32767 */
    Int32 sat_high_set; /* Value to set to if upper threshold exceeded */
    Int32 sat_low; /* Lower threshold, in general should be positive or zero value */
    Int32 sat_low_set; /* Value to set to if lower threshold exceeded */

} CPIS_ComputeCornersParms;


typedef struct {

    Uint16 nplus1; /* maxima are searched in block dimensions of nplus1 x nplus1 */

}CPIS_ComputeBlockMaximaParms;



typedef struct {

    Uint16 nplus1; /* if NMS neighborhood is (2n+1)*(2n+1) then nplus1= n+1 */

}CPIS_FastNmsStep1Parms;

/*
 * This enum type defines different choice of downsampling
 * supported by CPIS_twoLevelsPyramid().
 */
#define CPIS_MAX_NUM_PYR_LEVEL 2

typedef enum {
    NO_DOWNSAMPLING=0, /* no downsampling, full scale intensity image is kept, which is subsequently processed by sobel gradient if enabled */
    GAUSSIAN3x3_NO_DOWNSAMPLING, /* gaussian 3x3 applied but no downsampling performed */
    GAUSSIAN3x3_DOWNSAMPLING2x2 /* gaussian 3x3 applied followed by downsampling */
} CPIS_DownsampleType;

typedef struct {
    CPIS_DownsampleType dsType; /* Specify downsampling type */
    Bool outputIntensity;      /* Set to TRUE, if you want the intensity image after downsampling to be sent out */
    Bool outputXYSobelGradient; /* Set to TRUE, if you want the sobel X and Y gradients after downsampling to be sent out */
    CPIS_Format gradientFormat; /* gradient type CPIS_INT8, CPIS_UINT8, CPIS_INT16, CPIS_UINT16 */
    Uint8 grad_shift;   /* shift used during gradient calculation is src is 8-bit and gradient format is 16-bit set it to 0 */
    Int32 grad_sat_high; /* For gradient, upper threshold, always a positive value, maximum is 32767 */
    Int32 grad_sat_high_set; /* For gradient, value to set to if upper threshold exceeded */
    Int32 grad_sat_low; /* For gradient, Lower threshold, in general should be positive or zero value */
    Int32 grad_sat_low_set; /* For gradient, value to set to if lower threshold exceeded */

    CPIS_Buffer intensityBuffer; /* Pointer, stride of the output intensity image, must be same format as source buffer  */
    CPIS_Buffer gradXBuffer; /* Pointer, stride of the X component of the sobel gradient output */
    CPIS_Buffer gradYBuffer; /* Pointer, stride of the Y component of the sobel gradient output */
} CPIS_PyramidLevel;

/*
 * Structure used by CPIS_twoLevelsPyramid() function
 * which produces two levels of gaussian pyramid at once with
 * optional downscaling and generation of sobel X and Y gradients
 * for each level.
 */
typedef struct {
    CPIS_PyramidLevel level[2];
} CPIS_TwoLevelsPyramidParms;

/* 
   CPIS_UpdateEWRMeanVar16() updates the exponential running mean and variance values over background pixel in a frame
   using the formulas: 
   updatedMean = (1-weightMean) × previousMean + weightMean × (newestData<<7)
   updatedVar = (1 - weightVar) × previousVar + weightVar × ((newestData<<7) - previousMean)^2 

   A bitmap of the foreground is passed to the function so the update is selectively applied
   to background pixels only. 
 */
Int32 CPIS_updateEWRMeanVar16(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_UpdateEWRMeanVar16Parms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the background update algorithm based on roi's width and roi's height) */
Int32 CPIS_getUpdateEWRMeanVar16blockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_segmentFG(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SegmentFGParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the segment foreground algorithm based on roi's width and roi's height) */
Int32 CPIS_getSegmentFGblockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_fgSegment(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FgSegmentParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the segment foreground algorithm based on roi's width and roi's height) */
Int32 CPIS_getInitMeanVarMHIBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_initMeanVarMHI(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_InitMeanVarMHIParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the segment foreground algorithm based on roi's width and roi's height) */
Int32 CPIS_getFgSegmentBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_imDiffIntlv(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ImDiffIntlvParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the image difference algorithm based on roi's width and roi's height) */
Int32 CPIS_getImDiffIntlvBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);
Int32 CPIS_tamperImageScan(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_TamperImageScanParms *params,
        CPIS_ExecType execType
);

/* Return optimum block dimensions used by the segment foreground algorithm based on roi's width and roi's height) */
Int32 CPIS_getTamperImageScanBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

/* Function CPIS_calcSobelEdgeMap()

   Description:

   This function calculates sobel map of source frame and also counts number of
   pixels belonging to edges.

   Source frame is 8-bit or 16-bit.
   Destination frame pointed by base.dstBuf[0].ptr is the sobel edge map, which is basically
   a frame filled with 8-bit values equal to:
   - params->onEdgeVal if pixel's sobel value strictly greater than  params->edgeThreshold
   - params->offEdgeVal otherwise

   Sobel value is calculated as follow:
   Sobel= F(H, V)
   where H is the horizontal sobel value produced by the filter:
       1 0 -1			
   H=  2 0 -2		
	   1 0 -1		

   V is the vertical sobel value produced by the filter:

       1  2  1
   V=  0  0  0
      -1 -2 -1

   F(H,V) is a TI proprietary function that outputs the strength of an edge based on H and V.
   qShift is used during computation of H and V and its recommended value is 3.

   The function also outputs to the location pointed by base.dstBufPtr[1].ptr 
   the number of pixels that belong to edges as a 32 bit unsigned integer.

   If either base.procBlockSize.width or procBlockSize.height is set to 0, 
   the function will try to find the best fit for these parameters using the information provided
   by the source format, base.roiSize.width and base.roiSize.height . 
   Also if you want to know in advance to what values these parameters will be set to, 
   use the function CPIS_getCalcSobelEdgeMapBlockDim().
 */
Int32 CPIS_calcSobelEdgeMap(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_CalcSobelEdgeMapParms *params,
        CPIS_ExecType execType
);

/*
    Return the optimum base.procBlockSize.width and base.procBlockSize.height that work along with the specified
    roiWidth, roiHeight and srcFormat.
 */
Int32 CPIS_getCalcSobelEdgeMapBlockDim(CPIS_Format srcFormat, Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_countNumONpixels(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_CountNumONpixelsParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getCountNumONpixelsBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_getXyGradientsAndMagnitudeBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);


/* 2-D Gradient filtering

The first order 3×3 gradient filter calculates the first derivative in both the horizontal and vertical directions,
Gx and Gy, respectively. So for the image pixel I(x,y), we calculate the gradients as shown here:
Gx = I(x+1,y) - I(x-1,y)
Gy = I(x,y+1) - I(x,y-1)
The gradient magnitude can be calculated in three ways depending on the paramater
params->magnitudeType:
- params->magnitudeType== DO_NOT_COMPUTE : no magnitude values are produced
- params->magnitudeType== SUM_ABS_XY : magnitude values are computed using the formula
Gmag = (|Gx| + |Gy|)
- params->magnitudeType== SUM_SQUARE_XY : magnitude values are computed using the formula
Gmag= Gx^2 + Gy^2
- params->magnitudeType== SQRT_SUM_SQUARE_XY : Not supported currently
 */
Int32 CPIS_xyGradientsAndMagnitude(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_XyGradientsAndMagnitudeParms *params,
        CPIS_ExecType execType
);

/* Non-maximum Suppression
 *
 * CPIS_NMS API performs non maximum suppression on the given input image. This function compares the value of each input pixel against its neighbors. It returns an unsigned char image in which 255 was written in place of maxima positions. For an output pixel to be "on" (numerical value=255), the input pixel value must be both:
 *  - greater than or equal to its neighbors’ values
 *  - greater than the minimum threshold
 *  If the above conditions are not met simultaneously, the output will be 0.
 *
 */
Int32 CPIS_NMS(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_NMSParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_YCbCr2Hue(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_YCbCr2HueParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getYCbCr2HueBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_Hue2Mask(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_Hue2MaskParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getHue2MaskBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_getDoubleThreshBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_doubleThresholding(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_doubleThresholdingParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getMorphologyBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_morphology(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_morphologyParms *params,
        CPIS_ExecType execType
);

/*
 * CPIS_integralImageColWise() API takes unsigned byte input data and produces an unsigned 32-bit output data
 * that adds all the input pixels along all the previous rows to obtain the column-wise integral sum of the image for the current pixel location.
 * For instance, if s(m,n) denotes the input pixel at location col=m and row=n then, the output o(m,n) is expressed as:
 * o(m,n) = s(m,0) + s(m, 1) + ….. + s(m, n-1)
 * The complete integral image can be obtained by applying row-wise CPIS_integralImageRowWise() to the output of this column-wise function.
 *
 */

Int32 CPIS_getIntImageColBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_integralImageColWise(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_IntegralImageColParms *params,
        CPIS_ExecType execType
);

/*
 * CPIS_integralImageRowWise() API takes unsigned byte input data and produces an unsigned 32-bit output data
 * that adds all the input pixels along all the previous columns to obtain the row-wise integral sum of the image for the current pixel location.
 * For instance, if s(m,n) denotes the input pixel at location col=m and row=n then, the output o(m,n) is expressed as:
 * o(m,n) = s(0,n) + s(1, n) + ….. + s(m-1, n)
 * The complete integral image can be obtained by applying col-wise CPIS_integralImageColWise() to the output of this row-wise function.
 *
 */

Int32 CPIS_getIntImageRowBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_integralImageRowWise(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_IntegralImageRowParms *params,
        CPIS_ExecType execType
);

/* ComputeCorners */
Int32 CPIS_computeCorners(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ComputeCornersParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getComputeCornersBorderDim(Bool isInputXYgradient, Uint16 procBlockWidth, Uint16 procBlockHeight, Uint16 *borderWidth, Uint16 *borderHeight);

Int32 CPIS_getComputeCornersBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_fastNmsStep1(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FastNmsStep1Parms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getFastNmsStep1BlockDim(
        CPIS_BaseParms *base, 
		void *params, 
		Uint32 *blockWidth, 
		Uint32 *blockHeight);

Int32 CPIS_fastNmsStep2(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ExecType execType
);

Int32 CPIS_getCompBlkMaximaBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_computeBlockMaxima(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ComputeBlockMaximaParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_twoLevelsPyramid(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_TwoLevelsPyramidParms *params,
        CPIS_ExecType execType
);


/* OSD AlphaBlendig API params */
typedef struct {
    /*
        if set to 1 then alpha value specified next is used for entire image
        otherwise if 0, use alpha plane passed as src[1]
     */
    Uint16 useGlobalAlpha;
    Uint16 alphaValue;  /* global alpha value, 0-255, 255 let see foreground, 0 let see background */
    CPIS_Buffer background;
    CPIS_Buffer backgroundUV;
    CPIS_Format backgroundFormat;
    Uint8 colorKey[2];          /* color keys for luma and chroma */
    Bool transparencyEnable; /* transparency enable flag */	
    Bool dirtyBit;              /* dirtyBit from resource manager
                                 *  0 : when SIMCOP not used by other clients
                                 *  1 : when SIMCOP used by other clients  */
} CPIS_OSD_AlphaBlendParms;


Int32 CPIS_osd_alphaBlend(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_OSD_AlphaBlendParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getTwoLevelsPyramidBlockAndBorderDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight, Uint16 *borderWidth, Uint16 *borderHeight);

#ifdef __cplusplus
}
#endif

#endif /* #define _CPISLIBCUST_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
