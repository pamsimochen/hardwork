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
/*        cpisImgProc.h --                                                  */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the image processing functions belonging to the                */
/*        CoProcessor InfraStructure (CPIS)'s image processing library      */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.1  6 June , 2011                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _CPISIMGPROC_H
#define _CPISIMGPROC_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <tistdtypes.h>
/* 
    Maximum processing block size for different functions. 
    For a given <functionName>: 
    procBlockSize.width x procBlockSize.height < MAX_<functionName>_BLOCKSIZE
*/

#define MAX_ALPHABLEND_GLOBAL_ALPHA_BLOCKSIZE   (2*1024/PLAT_DIV)
#define MAX_ALPHABLEND_BLOCKSIZE                (1638/PLAT_DIV)
#define MAX_COLORSPCCONV_BLOCKSIZE              (744/PLAT_DIV)
#define MAX_ROTATION_BLOCKSIZE                  (2048/PLAT_DIV)   
#define MAX_FILLMEM_BLOCKSIZE                   (8188/PLAT_DIV)   
#define MAX_ARRAYOP_BLOCKSIZE                   (4096/PLAT_DIV)
#define MAX_ARRAYSCALAROP_BLOCKSIZE             (8192/PLAT_DIV)
#define MAX_ARRAYCONDWRITE_BLOCKSIZE            (2730/PLAT_DIV)
#define MAX_YCBCRPACK_BLOCKSIZE                 (1364/PLAT_DIV)
#define MAX_YCBCRUNPACK_BLOCKSIZE               (1364/PLAT_DIV)
#define MAX_MATMUL_BLOCKSIZE                    (4096/PLAT_DIV)
#define MAX_SUM_BLOCKSIZE                       (8188/PLAT_DIV)
#define MAX_SUMCFA_BLOCKSIZE                    (8176/PLAT_DIV)
#define MAX_LUT_BLOCKSIZE                       (8192/PLAT_DIV)
#define MAX_LUT_SIZE                            (32768/PLAT_DIV)
#define MAX_BLKAVERAGE_BLOCKSIZE                (8188/PLAT_DIV)
#define MAX_MEDIANFILTER_ROW_BLOCKSIZE          (8192/PLAT_DIV)
#define MAX_MEDIANFILTER_COL_BLOCKSIZE          (8192/PLAT_DIV)
#define MAX_FILTER_BLOCKSIZE                    (8192/PLAT_DIV)
#define MAX_RGBPACK_BLOCKSIZE                   (1638/PLAT_DIV)
#define MAX_RGBUNPACK_BLOCKSIZE                 (1638/PLAT_DIV)
#define MAX_MEDIAN2D_BLOCKSIZE                  (8192/PLAT_DIV)
#define MAX_SOBEL_BLOCKSIZE                     (4096/PLAT_DIV)
#define MAX_PYRAMID_BLOCKSIZE                   (8192/PLAT_DIV)
#define MAX_AFFINE_BLOCKSIZE                    (5000/PLAT_DIV)
#define MAX_CFA_BLOCKSIZE                       (2730/PLAT_DIV)
#define MAX_SAD_BLOCKSIZE                       (8192/PLAT_DIV)
#define MAX_SAD_TEMPLATESIZE                    (32768/PLAT_DIV)

/* 
    The below enums represent the various color space types that 
    are supported by the YCbCrPack routine
*/
typedef enum {
    CPIS_444_16BIT_TO_422_8BIT=0,
    CPIS_422_16BIT_TO_422_8BIT,
    CPIS_420_16BIT_TO_422_8BIT,
    CPIS_422V_16BIT_TO_422_8BIT,
    CPIS_444_16BIT_TO_444_8BIT,
    CPIS_422_16BIT_TO_444_8BIT,
    CPIS_420_16BIT_TO_444_8BIT,
    CPIS_422V_16BIT_TO_444_8BIT,

    CPIS_444_8BIT_TO_422_8BIT=0x8000,
    CPIS_422_8BIT_TO_422_8BIT,
    CPIS_420_8BIT_TO_422_8BIT,
    CPIS_422V_8BIT_TO_422_8BIT,
    CPIS_444_8BIT_TO_444_8BIT,
    CPIS_422_8BIT_TO_444_8BIT,
    CPIS_420_8BIT_TO_444_8BIT,
    CPIS_422V_8BIT_TO_444_8BIT

} CPIS_ColorSpacePack;

/* 
    The below enums represent the various color space types that 
    are supported by the YCbCrUnPack routine
*/

typedef enum {
    CPIS_422_TO_444_8BIT=0x8000,
    CPIS_422_TO_422_8BIT,
    CPIS_422_TO_420_8BIT,
    CPIS_444_TO_444_8BIT,
    CPIS_444_TO_422_8BIT,
    CPIS_444_TO_420_8BIT,

    CPIS_422_TO_444_16BIT=0x0000,
    CPIS_422_TO_422_16BIT,
    CPIS_422_TO_420_16BIT,
    CPIS_444_TO_444_16BIT,
    CPIS_444_TO_422_16BIT,
    CPIS_444_TO_420_16BIT

} CPIS_ColorSpaceUnpack;

/* The conditions supprted by the array conditional write API */
typedef enum {
    CPIS_WR_ZERO=0, 
    CPIS_WR_NOTZERO,     
    CPIS_WR_SAT,     
    CPIS_WR_NOTSAT     
} CPIS_WriteMode;

typedef enum {
    CPIS_TOP2BOTTOM,
    CPIS_BOTTOM2TOP,
    CPIS_LEFT2RIGHT,
    CPIS_RIGHT2LEFT
} CPIS_FilterDir;

typedef enum {
    CPIS_USE_BOUNDARY,
    CPIS_USE_PASSED_VALUES
} CPIS_FilterInitialMode;

/* ColorSpaceConv API params */
typedef struct {
    Int16 matrix[9];
    Uint32 qShift;
    Int16 preOffset[3];     /* offset to add to each component before matrix multiplication */
    Int16 postOffset[3];    /* offset to add to each component after matrix multiplication */
    Int16 signedInput[3];
    Int16 signedOutput[3];
    CPIS_ColorDsMode colorDsMode;  /* color downsampling mode */
} CPIS_ColorSpcConvParms;

/* AlphaBlendig API params */
typedef struct {
    /* 
        if set to 1 then alpha value specified next is used for entire image
        otherwise if 0, use alpha plane passed as src[1]
    */
    Uint16 useGlobalAlpha; 
    Uint16 alphaValue;  /* global alpha value, 0-255, 255 let see foreground, 0 let see background */
	Uint16 qShift;
	Int16 matrix[9];
    CPIS_Buffer background;
    CPIS_Buffer backgroundUV;
    CPIS_Format backgroundFormat;
} CPIS_AlphaBlendParms;

/* Rotation API params */
typedef struct {
    Int16 angle;
} CPIS_RotationParms;

/* FillMem API params */
typedef struct {
    Uint8 * constData; // On _SIMCOP must be 16 bytes aligned
} CPIS_FillMemParms;

/* ArrayOperation API params */
typedef struct {
    Uint16 qShift;
    CPIS_Operation operation;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_ArrayOpParms;

/* ArrayScalarOperation API params */
typedef struct {
    Uint16 qShift;
    CPIS_Operation operation;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 mask[2][2];
} CPIS_ArrayScalarOpParms;

/* ArrayConditionalWrite API params */
typedef struct {
    Uint16 qShift;
    CPIS_WriteMode writeMode;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_ArrayCondWriteParms;

/* YCbCrPack API params */
typedef struct {
    Uint16 qShift;
    CPIS_ColorSpacePack colorSpace;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 scale;
} CPIS_YCbCrPackParms;

/* YCbCrUnPack API params */
typedef struct {
    Uint16 qShift;
    CPIS_ColorSpaceUnpack colorSpace;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 scale;
} CPIS_YCbCrUnpackParms;

/* MatrixMutiply API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 matWidth;
    Int16 matHeight;
    void * matPtr;
    CPIS_Format matFormat;
} CPIS_MatMulParms;

/* Sum API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 * scalarPtr;
    CPIS_Format scalarFormat;
} CPIS_SumParms;

/* Sum CFA API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 * scalarPtr;
    CPIS_Format scalarFormat;
} CPIS_SumCFAParms;

/* LUT API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 * lutPtr;
    CPIS_Format lutFormat;
    Int16 numLUT;
    Int16 LUTSize;
} CPIS_LUTParms;

/* Blk Average API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_BlkAverageParms;

/* Median Filter Row API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 median_size;
} CPIS_MedianFilterRowParms;

/* Median Filter Col API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 median_size;
} CPIS_MedianFilterColParms;

/* Filter API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 coeff_width;
    Int16 coeff_height;
    Int16 * coeffPtr;
    CPIS_Format coeffFormat;
} CPIS_FilterParms;

/* RGBPack API params */
typedef struct {
    Int8 reserved; /* Not used */
} CPIS_RGBPackParms;

/* RGBUnpack API params */
typedef struct {
    Int8 reserved; /* Not used */
} CPIS_RGBUnpackParms;

/* Vertical/Horizontal recursive filter params */
typedef struct {
    CPIS_FilterDir direction;
    CPIS_FilterInitialMode initialMode;
    CPIS_Buffer initialValues;
    Uint16 alpha;
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_RecursiveFilterParms;

/* Median 2D Filter */
typedef struct {
    Int16 filterWidth;
    Int16 filterHeight;
} CPIS_Median2DParms;

/* Sobel API params */
typedef struct {
    Uint16 qShift;
    Bool separateXYplanes; /* TRUE if output needs to be separated into two planes: one for sobel-X and the other other one for Sobel Y */
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_SobelParms;

/* Pyramid API params */
typedef struct {
    Uint16 gaussFilterSize; /* Can be 3, 5, 7 */
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_PyramidParms;


/* 
Affine transform params 


 X      m0 m1     x   tx
 Y   =  m2 m3  *  y + ty


 */
#define CPIS_AFFINE_PRIVATE_VAR_SIZE 12

typedef struct {
    Uint8 *privateVars; /* Pointer to scratch of size 12 bytes */ 
    Uint8 *scratch; /* point to a region of size scratchSize bytes */
    Uint32 scratchSize; /* size of scratch in bytes, obtained by calling  _CPIS_affineTransformGetSize */
    Int16 skipOutside; /* skip processing of region outside of the ROI */
    Int16 m0;
    Int16 m1;
    Int16 m2;
    Int16 m3;
    Int16 tx;
    Int16 ty;
    Int16 m0inv;  /* Coefficient of the inverse matrix */
    Int16 m1inv;
    Int16 m2inv;
    Int16 m3inv;
    Int16 txinv;
    Int16 tyinv;
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;

} CPIS_AffineTransformParms;

typedef struct {
    Uint16 width;
    Uint16 height;
    Uint16 stride;
    Uint16 blockWidth;
    Uint16 blockHeight;
} CPIS_AffineTransformOutputROI;

/* CFA API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
} CPIS_CFAParms;

/* Sum-of-absolute differences API params */
typedef struct {
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    Int16 *templatePtr; /* Pointer to template */
    CPIS_Format templateFormat; /* format of each data element composing the template. Can be CPIS_U8BIT, CPIS_8BIT, CPIS_16BIT, CPIS_U16BIT .  */
    Int16 loadTemplate; /* Enable(1)/Disable(0) pre-loading of template into coefficient memory */
    Uint16 loadTemplateStride;
    CPIS_Size templateRoiSize; /* Size of the template's ROI */
    CPIS_Size templateSize;     /* Size of the template */
    Int16 templateStartOfst; /* Offset from location pointed by templatePtr to the first data point of the template */
} CPIS_SadParms;

/* Multi downscale by N/8 API params */
typedef struct {
    Uint8 numOutputs; /* Number of output frames, each of different resize ratio, that need to be produced by the function, max is 8 */
    Uint8 Nh[8]; /* for each output frame specify the numerator 'Nh' of Nh/8 of the horizontal resizing ratio that is applied to it */
    Uint8 Nv[8]; /* for each output frame specify the numerator 'Nv' of Nv/8 of the vertical resizing ratio that is applied to it */
} CPIS_MultiDownScaleBy8Parms;

typedef struct {
    Int16 scalar0;
    Int16 scalar1;
    CPIS_Operation operation;
    Uint16 qShift;
    Int32 sat_high;
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set; 
} CPIS_DeInterleaveParms;

typedef struct {
    Int8 scalarY;
    Int8 scalarUV;
} CPIS_SemiPlanarToPlanarParms;

typedef enum {
    ROW,
    COL
} CPIS_FFT_Dir;

typedef enum {
    FORWARD= 0,
    INVERSE,
    FORWARD_REAL_INPUT,
    INVERSE_REAL_OUTPUT
} CPIS_FFT_Op;

/* FFT function's parameter structure 
   This function calculates the 1-D FFT or inverse FFT for 32, 64, 128, 256, 512, 1024 complex points 
   The input and output bit resolution is 16-bits and radix-2 FFT is used.
 */

typedef struct {
    CPIS_FFT_Dir dir; /* Direction of the 1D FFT: row or columnn */
    Uint16 numPoints; /* Must be a power of 2, maximum value is 1024 */
    CPIS_FFT_Op FFT_op; /* Set to one of the 4 options listed in CPIS_FFT_Op */
    /* shiftPattern describes the bit shift pattern for all FFT's stage.
    The number of stages is equal to log2(numPoints). Hence for 256 FFT, there
    are 8 stages. At every stage, stage, there is normally a dynamic range expansion
    of 1 bit. In order to avoid overflow due to that expansion from 16-bits to 17 bits, 
    there should be a 1-bit right shif at every stage. 
    However if it is known that the output dynamic range is less than 16 bits then it implies
    some stages don't overflow and fewer bit-shifts are required. 

    Here are a few examples of shiftPattern:
       0xFF= 11111111b means that for every stages of 256 FFT, a right shift occur.
       0xFFFF= 11111111 11111111b means that for every stages of 1024 FFT, a right shift occur.
       0x0F= 00001111b means that only the first 4 stages are right shifted

    The number of bits that are effectively taken into account depend
    on the number of stages in the FFT which is log2(numPoints).
    For a 1024 points FFT, 10 bits are taken into account.
    For a 256 points FFT, 8 bits are taken into account.
    By default, it is strongly recommended to set shiftPattern=0xFFFF
    in order to avoid overflow for any settings of numPoints .
    There also might not be any precision benefit in limiting the number of right shift
    because these bits are the least significant bit that was rounded off 
    during the twiddle factor multiplication of each stage and hence contain quantization error.

     */
    Uint16 shiftPattern; 
    Int32 sat_high; /* use this to saturate highest value */
    Int32 sat_high_set; /* use this to saturate highest value */
    Int32 sat_low; /* use this to saturate lowest value */
    Int32 sat_low_set; /* use this to saturate lowest value */
} CPIS_FFTParms;

typedef struct {
    Uint16 validFlag;
    Uint16 inputBlockWidth;
    Uint16 inputBlockWidthDiv2;
    Uint16 inputBlockHeight;
    Uint16 inBlock_x;
    Uint16 inBlock_y;
    Uint16 outBlock_x;
    Uint16 outBlock_y;
} CPIS_RemapBlockInfo;

typedef struct {
    float *srcMap;    /* Input arg of CPIS_converMap()only: srcMap points to an input array of (x,y) coordinates in float */
    CPIS_Size mapDim; /* Input arg : width and height of the coordinate map pointed by the above srcMap, which are also the destination ROI's width and height */
    CPIS_Size srcImageDim; /* Input arg: Dimensions of the source image, doesn't need to be same as mapDim and usually are larger */
    CPIS_Format srcFormat; /* Input arg: source format: CPIS_U8BIT, CPIS_U16BIT, CPIS_YUV_422ILE */
    CPIS_Size outputBlockDim; /* Input arg: output block dimensions,
                                 which must divide mapDim.width and mapDim.height . Width must be multiple of 16 bytes*/
    void *srcMap1; /* Input arg: pointer to block-partitioned map of indexes in Q format used by TLU and bilinear interpolation
                          size must be:
                          For CPIS_YUV_420SP:
                         (sizeof(CPIS_RemapBlockInfo) + 5*outputBlockDim.width*outputBlockDim.height) *( 2  + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height))
                          For all other formats:
                         (sizeof(CPIS_RemapBlockInfo) + 4*outputBlockDim.width*outputBlockDim.height) *( 2  + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height))
     */
    Uint32 srcMap1ByteLen; /* Input arg: For CPIS_YUV_420SP format, must be at least (sizeof(CPIS_RemapBlockInfo) + 5*outputBlockDim.width*outputBlockDim.height) *( 2  + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height)) */
    /* For all other format must be at least (sizeof(CPIS_RemapBlockInfo) + 4*outputBlockDim.width*outputBlockDim.height) *( 2  + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height)) */
    void *srcMap2; /* Input arg: mut be 128 bytes aligned for cache purpose, pointer to map of dma paramaters for all the blocks.  */
    Uint32 srcMap2ByteLen; /* Input arg: For CPIS_YUV_420SP, must be at least  2* 16 * ( 2 + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height))
                                         For all other format, it must be at least 16 * ( 2 + (mapDim.width * mapDim.height) / (outputBlockDim.width * outputBlockDim.height)) */
    Uint8 qShift; /* Input arg : Number of fractional bits used to represent Q-numbers pointed by srcMap1 */
    CPIS_Size maxInputBlockDim; /* Output arg of CPIS_convertmap(), input arg of CPIS_remap() : maximum input block width and height */
    Uint32 maxInputBlockSize; /* Output arg of CPIS_convertMap(), input arg of CPIS_remap(): maximum input block size in bytes */
} CPIS_RemapMaps;

typedef enum {
    NN, // nearest neighbour
    BILINEAR //bilinear interpolation
} CPIS_Interpolation;

/* Structure used as input parameter to CPIS_remap()
 */
typedef struct {
    CPIS_Interpolation interpolationLuma; /* interpolation method: bilinear or NN for luma part or for monochrom plane*/
    CPIS_Interpolation interpolationChroma; /* chroma interpolation method: bilinear or NN */
    Uint8 rightShift; // optional right shift to convert from 16-bit to 8-bit for instance
    Int32 sat_high; // saturation applied after right shift
    Int32 sat_high_set;
    Int32 sat_low;
    Int32 sat_low_set;
    CPIS_RemapMaps maps;
} CPIS_RemapParms;

typedef struct {
    Int16 weight;
} CPIS_recursiveFilterHorzParms;

typedef struct {
    Int16 weight;
} CPIS_recursiveFilterVertParms;

Int32 CPIS_getColorSpcConvBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_colorSpcConv(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ColorSpcConvParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_alphaBlend(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_AlphaBlendParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getAlphaBlendBlockDim(CPIS_Format srcFormat, Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_alphaBlendRGB(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_AlphaBlendParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getRotationBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_rotation(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_RotationParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getFillMemBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_fillMem(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FillMemParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getArrayOpBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_arrayOp(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ArrayOpParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getArrayScalarOpBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_arrayScalarOp(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ArrayScalarOpParms *params,
        CPIS_ExecType execType
);


Int32 CPIS_arrayCondWrite(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ArrayCondWriteParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getYCbCrPackBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_YCbCrPack(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_YCbCrPackParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getYCbCrUnPackBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_YCbCrUnpack(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_YCbCrUnpackParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getMatMulBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_matMul(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_MatMulParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_sum(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SumParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_sumCFA(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SumCFAParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getLUTBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_table_lookup(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_LUTParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_blkAverage(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_BlkAverageParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_medianFilterRow(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_MedianFilterRowParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_medianFilterCol(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_MedianFilterColParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getFilterBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_filter(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FilterParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_RGBPack(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_RGBPackParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_RGBUnpack(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_RGBUnpackParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_recursiveFilter(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_RecursiveFilterParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getMedian2DBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_median2D(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_Median2DParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getSobelBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_sobel(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SobelParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getPyramidBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_pyramid(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_PyramidParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_affineTransformGetSize(CPIS_BaseParms *base,
        CPIS_AffineTransformParms *params, CPIS_AffineTransformOutputROI *outputROI );

Int32 CPIS_affineTransform(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_AffineTransformParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getSadBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_sad(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SadParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_multiDownScaleBy8(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_MultiDownScaleBy8Parms *params,
        CPIS_ExecType execType
);

Int32 CPIS_deInterleave(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_DeInterleaveParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getDeInterleaveBlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_semiPlanarToPlanar(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_SemiPlanarToPlanarParms *params,
        CPIS_ExecType execType
);

/* FFT function interface */
Int32 CPIS_fft(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_FFTParms *params,
        CPIS_ExecType execType
);

/* Function to get the optimum block dimensions of the FFT */
Int32 CPIS_getFFTBlockDim(CPIS_Format srcFormat, CPIS_Format dstFormat, Uint32 roiWidth, Uint32 roiHeight, CPIS_FFTParms *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_getSemiPlanarToPlanarBlockDim(Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_getMultiDownScaleBy8BlockDim(CPIS_Format srcFormat, CPIS_MultiDownScaleBy8Parms *params, Uint32 roiWidth, Uint32 roiHeight, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_setSadTemplateOffset(CPIS_Handle *handle, Uint16 templateStartOfst);

Int32 CPIS_loadRecursiveFilterInitialValues(CPIS_Handle *handle, void *src, CPIS_Format format, Uint32 stride);

Int32 CPIS_setRecursiveFilterAlphaCoef(CPIS_Handle *handle, Uint16 alpha);

/*
 * CPIS_convertMap()
 *
 * dstMapStruct points to a structure CPIS_RemapMaps
 */
Int32 CPIS_convertMap(CPIS_RemapMaps *maps);

Int32 CPIS_initRemap();
/* CPIS_remap() function interface */
Int32 CPIS_remap(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_RemapParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getMap1AndMap2byteLen(CPIS_RemapMaps *maps);

Uint32 CPIS_getRemapScatterGatherBufSize(CPIS_BaseParms *base, CPIS_RemapParms *params);

Int32 CPIS_getRecFiltHorzK1BlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_recursiveFilterHorz_K1(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_recursiveFilterHorzParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getRecFiltHorzK2BlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_recursiveFilterHorz_K2(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_recursiveFilterHorzParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_getRecFiltVerK1BlockDim(CPIS_BaseParms *base, void *params, Uint32 *blockWidth, Uint32 *blockHeight);

Int32 CPIS_recursiveFilterVert_K1(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_recursiveFilterVertParms *params,
        CPIS_ExecType execType
);

Int32 CPIS_recursiveFilterVert_K2(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_recursiveFilterVertParms *params,
        CPIS_ExecType execType
);

#ifdef __cplusplus
}
#endif

#endif /* #define _CPISIMGPROC_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
