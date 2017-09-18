/* ==============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file ip_run.h
 *
 * This header file defines the public interface of IP_RUN module.
 * 
 *
 * @path  $(DUCATIVOB)\alg\ip_run\inc\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 25-09-2009 Sanish Mahadik: Added PIPE_DOWN stage in IP_Run_Task
 *! 03-08-2009 Sanish Mahadik: Changed to add task-based mechanism 
 *! 25-05-2009 Swami: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef _IP_RUN_H_
#define _IP_RUN_H_
//#define IP_RUN_TASK_BASED
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

   /* User code goes here */
   /* ------compilation control switches ----------------------------------------*/
   /****************************************************************
    * INCLUDE FILES
    ****************************************************************/
   /* ----- system and platform files --------------------------*/
   /*-------program files ----------------------------------------*/
#include <stdio.h>

#ifdef CCS_PROJECT

#include "Simcop.h"
#include "csl_simcop.h"
#include "csl_iMX4.h"
#include "csl_simcop_dma.h"
#include "csl_ldc.h"
#include "csl_nsf.h"
#include <stdlib.h>
#include "timm_osal_memory.h"
#include "timm_osal_trace.h"

#else

#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
#include <ti/psp/iss/hal/iss/simcop/simcop_global/csl_simcop.h>
#include <ti/psp/iss/hal/iss/simcop/imx/csl_imx4.h>
#include <ti/psp/iss/hal/iss/simcop/dma/csl_simcop_dma.h>
#include <ti/psp/iss/hal/iss/simcop/ldc/csl_ldc.h>
#include <ti/psp/iss/hal/iss/simcop/nsf/csl_nsf.h>
#include <ti/psp/iss/timmosal/inc/timm_osal_memory.h>
#include <ti/psp/iss/timmosal/inc/timm_osal_trace.h>

#endif

   /*--------------------------- END -------------------------------*/
   /****************************************************************
    * EXTERNAL REFERENCES NOTE : only use if not found in header file
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

   typedef TIMM_OSAL_S8 IP_RUN_RET_TYPE;
   /*--------function prototypes --------------------------------*/
   /*--------------------------- END -------------------------------*/
   /****************************************************************
    * PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data declarations ----------------------------------*/
   /*--------macros -----------------------------------------------*/
#define MAX_PORTS 8
#define HWSEQ_MAX_STEPS 4
#define DEFAULT_BYTE_ALIGN 4
#define SIXTEEN_BYTE_ALIGN 16
#define THIRTYTWO_BYTE_ALIGN 32
#define TASK_IP_RUN_STACK (15 * 1024)
#define TASK_IP_RUN_PRIORITY 15

   /*Ratio of how many bytes are required per pixel for different formats*/
#define BYTES_TO_PIXEL_RATIO_YUV420 1.5
#define BYTES_TO_PIXEL_RATIO_YUV420_Y 1
#define BYTES_TO_PIXEL_RATIO_YUV420_UV 0.5
#define BYTES_TO_PIXEL_RATIO_YUV422 2

#define DMA_RASTER_IN 1
#define DMA_RASTER_OUT (1<<1)


   /*--------------------------- END ------------------------------*/

   typedef struct {
       TIMM_OSAL_U8 numSteps;
       TIMM_OSAL_U8 curStep;
       TIMM_OSAL_U32 irqStatus[4];
       TIMM_OSAL_U32 dmaStatus[4];
   } IP_RUN_Wait_Struct;

/* ========================================================================== */
/** IP_RUN_BOOL enumeration for Boolean variable for IP_RUN
 *
 * @param IP_RUN_FALSE    : False condition
 * @param IP_RUN_TRUE     : True condition
 */
/* ========================================================================== */
typedef enum {
    IP_RUN_FALSE = 0,
    IP_RUN_TRUE = 1
}IP_RUN_BOOL;





   /*ERROR STATUS CODES*/
#define IP_RUN_SOK                 (0)         /* Success */
#define IP_RUN_FAILED           (-1)        /* Generic failure */
#define IP_RUN_NOTSUPPORTED (-2) /* Format (YUV, Height, Width or QF not Supported*/
#define IP_RUN_INVPARAMS        (-3)    /*invalid parameters*/
#define IP_RUN_INITFAILED        (-4)    /*initialization failed*/
#define IP_RUN_NOMEMORY        (-5)    /*no memory available*/
#define IP_RUN_INVALIDHANDLE  (-6)    /*invalid handle passed*/
#define IP_RUN_DEINITFAILED    (-7)    /*deinitialization failed*/
#define IP_RUN_CONFIGFAILED   (-8)   /* configuration failed*/


/* ======================================================================= */
/*! \struct  DmaTferStruct
    * Structure which captures the DMA information for each channel such as DDR address, block-size and strides.
    *
    * @param ddrAddr         : DMA transfer base address in DDR
    * @param ddrWidth        : Data width in DDR, in bytes
    * @param ddr_X_jump   : Offset to next block, in bytes
    * @param ddr_Y_jump   : Offset to next row of blocks, in units of number of lines
    * @param imgBufAddr    : Starting address in image buffer
    * @param imgBufWidth   : Data width in image buffer in bytes
    * @param blockWidth      : Block width in bytes
    * @param blockHeight     : Block height in bytes 
    */
/* ======================================================================= */
typedef struct {
    TIMM_OSAL_U32 ddrAddr;
    TIMM_OSAL_U32 ddrWidth;
    TIMM_OSAL_S16 ddr_X_jump;
    TIMM_OSAL_S16 ddr_Y_jump;
    TIMM_OSAL_U32 imgBufAddr;
    TIMM_OSAL_U32 imgBufWidth;
    TIMM_OSAL_U16 blockWidth;
    TIMM_OSAL_U16 blockHeight;
    TIMM_OSAL_U16 raster_order;
}DmaTferStruct;



/* ======================================================================= */
/*! \struct  port
 * Structure which captures the port information (which is a node in the pipeline).
    *
    * @param accl         : Accelerator which is present at this port 
    * @param stage       : Stage of this port in the complete pipeline (starting from 0)
    * @param buf_step   : For each step (0 to HWSEQ_MAX_STEPS-1), image buffers to be connected to given accelerator
    */
/* ======================================================================= */
typedef struct {
    TIMM_OSAL_U16 accl;
    TIMM_OSAL_U16 stage;
    TIMM_OSAL_U16 buf_step[HWSEQ_MAX_STEPS];
}port;




/*! \struct  pipe_order
   *! \struct  pipe_order
    * Structure which captures pipeline information except for the Static buffer information (which include
    * work memory for NSF and LDC, IMX command memories and IMX coefficient memories).  
    *
    * @param acclr         : Array of ports  
    * @param nxt_step   : Next step information for each step
    *
* ======================================================================= */
typedef struct {
    port          acclr[MAX_PORTS];
    TIMM_OSAL_U16 nxt_step[HWSEQ_MAX_STEPS];
}pipe_order;



/* ======================================================================= */
   /*! \struct  pipeline 
    * Structure which captures pipeline information completely including the Static buffer information (which include
    * work memory for NSF and LDC, IMX command memories and IMX coefficient memories).  Command memory
    * location for each step can be different and algorithm can execute different IMX code in each step.
    *
    * @param num_stages         : No. of stages in the pipeline
    * @param IMXA_W              : Coefficient memory for IMX-A
    * @param IMXB_W              : Coefficient memory for IMX-B
    * @param IMXA_CMD          : Command memory location which stores the IMX code for IMX-A
    * @param IMXB_CMD          : Command memory location which stores the IMX code for IMX-B
    * @param LDC_W                : Work memory for LDC
    * @param NSF_W                : Work memory for NSF
    * @param pipeorder            : Structure to capture the Pipeline information
    */
/* ======================================================================= */
typedef struct {
    TIMM_OSAL_S16  num_stages;
    TIMM_OSAL_S16  IMXA_W;
    TIMM_OSAL_S16  IMXB_W;
    TIMM_OSAL_U16 *IMXA_CMD[HWSEQ_MAX_STEPS];
    TIMM_OSAL_U16 *IMXB_CMD[HWSEQ_MAX_STEPS];
    TIMM_OSAL_S16  LDC_W;
    TIMM_OSAL_S16  NSF_W;
#ifdef BUILD_FOR_OMAP5
        TIMM_OSAL_S16  NSF3_W;
#endif
    pipe_order    *pipeorder;
}pipeline;



/*LDC parameters*/

/* ======================================================================= */
/* LdcNsfDataFormatType - DataFormatType for LDC
    *
    * @param DATA_FORMAT_YCBCR422   : YUV 422 data format.
    * @param DATA_FORMAT_BAYER         :  Bayer data format
    * @param DATA_FORMAT_YCBCR420 : YUV 420 data format 
    * 
    */
/* ======================================================================= */
typedef enum {
    DATA_FORMAT_YCBCR422 = 0,
    DATA_FORMAT_BAYER = 1,
    DATA_FORMAT_YCBCR420 = 2
}LdcNsfDataFormatType;



typedef enum {
    IP_RUN_VNF_OUPUT_COMPLETE  = 0,
    IP_RUN_VNF_SLICE_COMPLETE = 1
}IP_RUN_VNF_COMPLETE_TYPE;
/* ======================================================================= */
/* IP_RUN_LDC_LUMA_INTERPOL_TYPE - Interpolation type for the LDC module. It can be
 *  bilinear or bicubic.
 *
 * @param IP_RUN_LDC_YINTERPOLATION_BICUBIC   : Bilinear interpolation. This will be faster.
 * @param IP_RUN_LDC_YINTERPOLATION_BILINEAR  : Bicubic interpolation, which will be more accurate but performance
 *                                                                               drops by 50% as compared to Bilinear mode.
 *
 */
/* ======================================================================= */
typedef enum {
    IP_RUN_LDC_YINTERPOLATION_BICUBIC  = 0,
    IP_RUN_LDC_YINTERPOLATION_BILINEAR = 1
}IP_RUN_LDC_LUMA_INTERPOL_TYPE;





   /* ======================================================================= */
   /* IP_RUN_BAYER_INIT_COLOR - Bayer format Init color for LDC
    *
    * @param IP_RUN_BAYER_INITCOLOR_RED                   : Red
    * @param IP_RUN_BAYER_INITCOLOR_GREENRED         : Green-red
    * @param IP_RUN_BAYER_INITCOLOR_GREENBLUE        : Green-blue
    * @param IP_RUN_BAYER_INITCOLOR_BLUE                  : Blue
    * @param IP_RUN_BAYER_INITCOLOR_DISABLED          : Disabled
    * 
    */
   /* ======================================================================= */
typedef enum {
    IP_RUN_BAYER_INITCOLOR_RED       = 0,
    IP_RUN_BAYER_INITCOLOR_GREENRED  = 1,
    IP_RUN_BAYER_INITCOLOR_GREENBLUE = 2,
    IP_RUN_BAYER_INITCOLOR_BLUE      = 3,
    IP_RUN_BAYER_INITCOLOR_DISABLED =4
}IP_RUN_BAYER_INIT_COLOR;






   /* ======================================================================= */
   /* IP_RUN_AffineParams -  Affine transform parameters for LDC
    *  @param unAffineA : Affine transwarp A 
    *  @param unAffineB : Affine transwarp B
    *  @param unAffineC : Affine transwarp C
    *  @param unAffineD : Affine transwarp D
    *  @param unAffineE : Affine transwarp E
    *  @param unAffineF : Affine transwarp F
    *  --       --     --                   --     --     --       --       --
    * | h_affine  | = | unAffineA   unAffineB | * | h_input |  +  | unAffineC |
    * | v_affine  |   | unAffineD   unAffineE |   | v_input |     | unAffineF |
    *  --       --     --                   --     --     --       --       --
    * where (h_affine, v_affine) denotes the tranformed coordinates after applying affine transformation
    *       (h_input, v_input) denotes the coordinates of the input pixels (can be distorted or distortion corrected pixels)
    * 
    * If s & r denote the desired scaling factor and rotation angle that needs to be applied via affine transform to the input,
    * 
    * unAffineA = s * cos(r); unAffineB = s * sin(r); unAffineD = -s * sin(r); unAffineE = s * cos(r);
    * unAffineC = h_0 - h_0 * A - v_0 * B; unAffineF = v_0 - h_0 * D - v_0 * E
    * 
    * where (h_0, v_0) denotes the coordinates of the lens center
    */
/* ======================================================================= */
typedef struct {
    TIMM_OSAL_U16 unAffineA;
    TIMM_OSAL_U16 unAffineB;
    TIMM_OSAL_U16 unAffineC;
    TIMM_OSAL_U16 unAffineD;
    TIMM_OSAL_U16 unAffineE;
    TIMM_OSAL_U16 unAffineF;
}IP_RUN_AffineParams;



/* ======================================================================= */
/* LdcNsfDataFormatType - DataFormatType for LDC
 *
 * @param BAYER_FORMAT_UNPACK12   : UNPACK12 Bayer data format.
 * @param BAYER_FORMAT_PACK12       : PACK12  Bayer data format
 * @param BAYER_FORMAT_PACK8         : PACK8  Bayer data format
 * @param BAYER_FORMAT_ALAW8         :ALAW8  Bayer data format
 *
 */
/* ======================================================================= */
typedef enum {
    BAYER_FORMAT_UNPACK12 = 0,
    BAYER_FORMAT_PACK12 = 1,
    BAYER_FORMAT_PACK8 = 2,
    BAYER_FORMAT_ALAW8 = 3,
    BAYER_FORMAT_DISABLED =4
}LdcBayerFormatType;







/*! \struct  pipeline
 * Structure which captures LDC configuration parameters.
 *
 * @param DataFormat                : LDC data-format
 * @param BayerFormat              : Bayer data format
 * @param FrameHeight               : Frame height in no of lines
 * @param FrameWidth                : Frame width in no of pixels
 * @param start_y                       : Start location for output in Y direction
 * @param start_x                       : Start location for output in X direction
 * @param ldclut                         : Look-up table pointer
 * @param pixelpad                     : Padding for fetching the input pixel data
 * @param lens_centre_v0            : Lens centre Y-coordinate
 * @param lens_centre_h0            : Lens centre X-coordinate
 * @param FrameWidthInBytes      : Frame width in bytes
 * @param pInput                        : Pointer to input data
 * @param pOutput                      : Pointer to output data
 * @param obw                           : block-width for computation
 * @param obh                            : block-height for computation
 * @param ldc_outline_offst         : Outline offset for LDC data
 * @param ldc_outline_offst_chroma: Outline chroma offset for LDC data
 * @param ldc_inline_offst             : Input line offset for LDC data
 * @param ldc_inline_offst_chroma : Input chroma line offset for LDC data
 * @param ldc_kvl                         : Vertical left magnification factor
 * @param ldc_khr                         : Horizontal right magnification factor
 * @param ldc_kvu                        : Vertical upper magnification factor
 * @param ldc_khl                          : Horizontal left magnification factor
 * @param ldc_rth                          : Radial threshold in no. of pixels
 * @param ldc_backmap                 : Right shift no. of bits
 * @param intpoltype                      : Luma-interpolation type
 * @param initcolour                       : Initial color for BAYER data
 * @param affineParams                 : Affine parameters
 */
/* ======================================================================= */
typedef struct {
    LdcNsfDataFormatType          DataFormat;
    LdcBayerFormatType            BayerFormat;
    TIMM_OSAL_U16                 FrameHeight;
    TIMM_OSAL_U16                 FrameWidth;
    TIMM_OSAL_U16                 start_y;
    TIMM_OSAL_U16                 start_x;
    TIMM_OSAL_U16                *ldclut;
    TIMM_OSAL_U16                 pixelpad;
    TIMM_OSAL_U16                 lens_centre_v0;
    TIMM_OSAL_U16                 lens_centre_h0;
    TIMM_OSAL_U16                 FrameWidthInBytes;
    TIMM_OSAL_U8                 *pInput;
    TIMM_OSAL_U8                 *pOutput;
    TIMM_OSAL_U16                 obw;
    TIMM_OSAL_U16                 obh;
    IP_RUN_BOOL                   LDC_FUNC_Enable;
    TIMM_OSAL_U16                 ldc_outline_offst;
    TIMM_OSAL_U16                 ldc_outline_offst_chroma;
    TIMM_OSAL_U16                 ldc_inline_offst;
    TIMM_OSAL_U16                 ldc_inline_offst_chroma;
    TIMM_OSAL_U16                 ldc_kvl;
    TIMM_OSAL_U16                 ldc_khr;
    TIMM_OSAL_U16                 ldc_kvu;
    TIMM_OSAL_U16                 ldc_khl;
    TIMM_OSAL_U16                 ldc_rth;
    TIMM_OSAL_U8                  ldc_backmap;
    IP_RUN_LDC_LUMA_INTERPOL_TYPE intpoltype;
    IP_RUN_BAYER_INIT_COLOR       initcolour;
    IP_RUN_AffineParams           affineParams;
}LDCParams;



/*NSF Parameters*/

/* ======================================================================= */
/* NsfSftSlope - NSF smooth parameter value
 *
 * @param NSF_SFT_SLP_0                : parameter value 0
 * @param NSF_SFT_SLP_1_BY_8       : parameter value 1
 * @param NSF_SFT_SLP_1_BY_4       : parameter value 2
 * @param NSF_SFT_SLP_1_BY_2       : parameter value 3
 *
 */
/* ======================================================================= */
typedef enum {
    NSF_SFT_SLP_0=0,
    NSF_SFT_SLP_1_BY_8=1,
    NSF_SFT_SLP_1_BY_4=2,
    NSF_SFT_SLP_1_BY_2=3
} NsfSftSlope;



/* ======================================================================= */
/*! \struct  NsfParams
 * Structure which captures NSF configuration parameters.
 *
 * @param DataFormat                : NSF data-format
 * @param luma_en                     : Enable filtering of Y-component
 * @param chroma_en                 : Enable filtering of UV-component
 * @param LastStripWidthBytes     : Strip-width of last strip in bytes
 * @param BlockWidthInPixels       : Block-width in pixels for a vertical strip
 * @param BlockHeight                 : Block-height of computational block
 * @param sft_luma_chroma         : Parameter value for Smoothing parameter
 * @param sft_luma                      : Enable smoothing for Y
 * @param sft_chroma                  : Enable smoothing for UV
 * @param threshold_param          : Filtering parameter enable/default/user-specified
 * @param shd_param                  : Shading correction parameter enable/default/user-specified
 * @param edge_param                : Edge enhancement parameter enable/default/user-specified
 * @param desat_param               : Desaturation parameter enable/default/user-specified
 * @param threshold_val               : User-specified Filtering parameters
 * @param shd_val                       : User-specified shading correction parameters
 * @param edge_val                     : User-specified edge enhancement parameters
 * @param desat_val                     : User-specified desaturation parameters
 */
/* ======================================================================= */
typedef struct {
    LdcNsfDataFormatType DataFormat;
    IP_RUN_BOOL          luma_en;
    IP_RUN_BOOL          chroma_en;
    TIMM_OSAL_U16        LastStripWidthBytes;                          /*This parameter is for last strip of NSF processing; will inform DMA valid strip-width to be output*/
    TIMM_OSAL_U16        BlockWidthInPixels;
    TIMM_OSAL_U16        BlockHeight;
    NsfSftSlope          sft_luma_chroma;
    IP_RUN_BOOL          sft_luma;
    IP_RUN_BOOL          sft_chroma;
    IP_RUN_BOOL          threshold_param;
    IP_RUN_BOOL          shd_param;
    IP_RUN_BOOL          edge_param;
    IP_RUN_BOOL          desat_param;
    TIMM_OSAL_S16       *threshold_val;
    TIMM_OSAL_S16       *shd_val;
    TIMM_OSAL_S16       *edge_val;
    TIMM_OSAL_S16       *desat_val;
}NsfParams;



#ifdef BUILD_FOR_OMAP5

/* ======================================================================= */
/* Nsf3BayerFormatType - DataFormatType for NSF3
 *
 * @param BAYER_FORMAT_8_BIT        : 8 BIT Bayer data format
 * @param BAYER_FORMAT_ALAW_8       : ALAW8 Bayer data format
 * @param BAYER_FORMAT_10_BIT       : 10 BIT Bayer data format
 * @param BAYER_FORMAT_12_BIT       : 12 BIT Bayer data format
 * @param BAYER_FORMAT_14_BIT       : 14 BIT Bayer data format
 * @param BAYER_FORMAT_16_BIT       : 16 BIT Bayer data format
 *
 */
/* ======================================================================= */
    typedef enum {
        BAYER_FORMAT_8_BIT = 0,
        BAYER_FORMAT_ALAW_8 = 1,
        BAYER_FORMAT_10_BIT = 2,
        BAYER_FORMAT_12_BIT = 3,
        BAYER_FORMAT_14_BIT = 4,
        BAYER_FORMAT_16_BIT = 5,
    }Nsf3BayerFormatType;



/* ======================================================================= */
/*! \struct  Nsf3Params
 * Structure which captures NSF3 configuration parameters.
 *
 * @param DataFormat                : NSF3 data-format
 * @param LastStripWidthBytes       : Strip-width of last strip in bytes
 * @param BlockWidthInPixels        : Block-width in pixels for a vertical strip
 * @param BlockHeight               : Block-height of computational block
 * @param threshold_param           : Noise threshold parameter enable/default/user-specified
 * @param thr_knee_param            : Soft Threshold knee parameter enable/default/user-specified
 * @param suppression_param         : Subband suppression parameter enable/default/user-specified
 * @param edge_param                : Edge enhancement parameter enable/default/user-specified
 * @param threshold_val             : User-specified noise threshold parameters
 * @param thr_knee_val              : User-specified soft thresholding knee parameters
 * @param suppression_val           : User-specified subband suppression parameters
 * @param edge_val                  : User-specified edge enhancement parameters
 */
/* ======================================================================= */
    typedef struct {
        Nsf3BayerFormatType  DataFormat;
        TIMM_OSAL_U16        LastStripWidthBytes;       /*This parameter is for last strip of NSF3 processing; will inform DMA valid strip-width to be output*/
        TIMM_OSAL_U16        BlockWidthInPixels;
        TIMM_OSAL_U16        BlockHeight;
        IP_RUN_BOOL          threshold_param;
        IP_RUN_BOOL          thr_knee_param;
        IP_RUN_BOOL          suppression_param;
        IP_RUN_BOOL          edge_param;
        TIMM_OSAL_S16       *threshold_val;
        TIMM_OSAL_U8        *thr_knee_val;
        TIMM_OSAL_U8        *suppression_val;
        TIMM_OSAL_U16       *edge_val;
    }Nsf3Params;


#endif


   /* ======================================================================= */
   /*! \struct  IP_RUN_APP_HWOBJ 
    * Structure to store the CSL handles. 
    *
    * @param pSimcopObj                : Handle to the SIMCOP registers
    * @param pSimcopDmaObj         : Handle to the SIMCOP DMA registers
    * @param pImxAObj                  : Handle to the IMX-A registers
    * @param pImxBObj                  : Handle to the IMX-B registers
    * @param pNsfObj                     : Handle to the NSF registers
    * @param pLdcObj                     : Handle to the LDC registers
    */
   /* ======================================================================= */
typedef struct {
    CSL_SimcopObj    *pSimcopObj;
    CSL_SimcopDmaObj *pSimcopDmaObj;
    CSL_ImxObj       *pImxAObj;
    CSL_ImxObj       *pImxBObj;
    CSL_Nf2Obj       *pNsfObj;
    CSL_LdcObj       *pLdcObj;
#ifdef BUILD_FOR_OMAP5
    CSL_Nsf3Obj      *pNsf3Obj;
#endif
}IP_RUN_APP_HWOBJ;


   /* ======================================================================= */
   /*! \struct  IP_RUN_Enabled_Modules 
    * Structure to store the information about modules which are enabled in given computation. 
    *
    * @param pSimcopObj                : Enabled/disabled the SIMCOP 
    * @param pSimcopDmaObj         : Enabled/disabled the SIMCOP DMA
    * @param pImxAObj                  : Enabled/disabled the IMX-A 
    * @param pImxBObj                  : Enabled/disabled the IMX-B 
    * @param pNsfObj                     : Enabled/disabled the NSF
    * @param pLdcObj                     : Enabled/disabled the LDC
    */
   /* ======================================================================= */
typedef struct {
    IP_RUN_BOOL Simcop_Enable;
    IP_RUN_BOOL SimcopDma_Enable;
    IP_RUN_BOOL Imx_A_Enable;
    IP_RUN_BOOL Imx_B_Enable;
    IP_RUN_BOOL Nsf_Enable;
    IP_RUN_BOOL Ldc_Enable;
    IP_RUN_BOOL Dct_Enable;
#ifdef BUILD_FOR_OMAP5
    IP_RUN_BOOL Nsf3_Enable;
#endif
}IP_RUN_Enabled_Modules;



   /* ======================================================================= */
   /*! \struct  IP_RUN_Enabled_Modules 
    * Structure to store the internal handles of IP_RUN. 
    *
    * @param ptModuleList                           : Pointer to enabled modules 
    * @param ptCslHandles                          : Pointer to the CSL handles
    * @param num_steps_pipeup                  : No. of steps in pipe-up stage
    * @param num_steps_pipefull                 : No. of steps in pipe-full stage
    * @param num_steps_pipedown              : No. of steps in pipe-down stage
    * @param num_stages                            : No. of stages in Pipeline
    */
   /* ======================================================================= */
typedef struct {
    IP_RUN_Enabled_Modules *ptModuleList;
    IP_RUN_APP_HWOBJ       *ptCslHandles;
    TIMM_OSAL_U16           num_steps_pipeup;
    TIMM_OSAL_U16           num_steps_pipefull;
    TIMM_OSAL_U16           num_steps_pipedown;
    TIMM_OSAL_U16           dma_raster_different;
    TIMM_OSAL_U16           dma_raster_stage;
    TIMM_OSAL_U8            num_stages;
    TIMM_OSAL_U32           pipeupconfig[16];
    TIMM_OSAL_U32           pipefullconfig[16];
    TIMM_OSAL_U32           pipedownconfig[16];
    TIMM_OSAL_U32           pipeuphwseqcfg;
    TIMM_OSAL_U32           pipefullhwseqcfg;
    TIMM_OSAL_U32           pipedownhwseqcfg;
    CSL_SimcopHwSetupCtrl   stSimcopSetupIntPipeUp;
}IP_RUN_INTERNAL_HANDLES;

typedef enum skipBlocksMode {
    ESKIP_VNF_MODE,
    ESKIP_LDCNSF_MODE,
    ESKIP_FASTNSF_MODE,
    ESKIP_FASTNSF_MODE_PASS2,
    ESKIP_VNF_VTC_MODE,
    ESKIP_VNF_DMA_MODE,
    ESKIP_DEFAULT_MODE

}EskipBlocksMode;


#ifndef _IP_RUN_DEBUG_DEF
#define _IP_RUN_DEBUG_DEF
   /**
    * Structure used as input argument of the debug callback function
    */
   typedef struct {

       Int16 blkX; // blkX is the X-axis coordinate of the block just being processed by the iMX
                   // the result of the processing sits in image buffer
       Int16 blkY; // blkY is the Y-axis coordinate of the block just being processed
       Int16 numBlocksX; // Total number of blocks in one row
       Int16 numBlocksY; // Total number of rows of blocks.
       Int8 *imBuf0Ptr;   // Pointer to the image buffer where the block of data has just been processed
       Int8 *coefBuf0Ptr; // Pointer to the coefficient memory
       Int8 *imBuf1Ptr;     /* Pointer to the image buffer where the block of data has just been processed by iMX1 */
       Int8 *coefBuf1Ptr;   /* Pointer to the coefficient memory of iMX1 */

   } IP_RUN_DebugStruct;

   /**
    * Definition of the debug callback function
    * set by IP_RUN_setDebugCB()
    */
   typedef void (*IP_RUN_DebugFunc) (IP_RUN_DebugStruct *debug, void*arg);
#endif

   /* ======================================================================= */
   /*! \struct  IP_run 
    * Top-structure for IP_RUN handle. 
    *
    * @param dmaIn                           : Array of input-DMA channel parameters
    * @param numDmaIn                    : Number of DMA channels which perform transfer from DDR to SIMCOP buffers
    * @param dmaIn_HWCh                 : Hardware-synchronization channel for input channels
    * @param dmaOut                         : Array of output-DMA channel parameters
    * @param numDmaOut                  : Number of DMA channels which perform transfer from SIMCOP buffers to DDR
    * @param dmaOut_HWCh              : Hardware-synchronization channel for output channels
    * @param ldcRuntimeParams         : LDC parameters
    * @param NsfTestParams               : NSF parameters
    * @param numVertBlocks               : Number of vertical blocks to be processed
    * @param numHorzBlocks              : Number of horizontal blocks to be processed
    * @param chunksize                      : Chunk size
    * @param raster_order                  : If raster_order =1, perform vertical raster scan
    * @param pipe                              : pipeline information
    * @param tPrivateHandles              : private handles for IP_RUN
    * @param callback                         : Call-back to be called when computation is complete
    */
   /* ======================================================================= */
   typedef struct {
       DmaTferStruct dmaIn[8];
       TIMM_OSAL_U16 numDmaIn;
       TIMM_OSAL_U16 dmaIn_HWCh;
       DmaTferStruct dmaOut[8];
       TIMM_OSAL_U16 numDmaOut;
       TIMM_OSAL_U16 dmaOut_HWCh;
       LDCParams    *ldcRuntimeParams;
       NsfParams    *NsfTestParams;
#ifdef BUILD_FOR_OMAP5
       Nsf3Params             *Nsf3RuntimeParams;
#endif
       TIMM_OSAL_U16 numVertBlocks;
       TIMM_OSAL_U16 numHorzBlocks;
       IP_RUN_BOOL   disablePipeDown;

       TIMM_OSAL_U16           numBlocksToSkip;
       EskipBlocksMode         skipBlocksMode; // Set to TRUE if DMA channels needs to be configured seperately.
       TIMM_OSAL_U32 chunksize ;
       TIMM_OSAL_U16 raster_order;      	
       pipeline pipe;
       IP_RUN_INTERNAL_HANDLES tPrivateHandles;
       void (*callbackvtc)(TIMM_OSAL_U32);
       void (*callback)(void);
       IP_RUN_DebugFunc debug_callback;
       void *debug_callback_arg;
       TIMM_OSAL_U16 edmaFlag;
       TIMM_OSAL_U16 preScatterGather;
       IP_RUN_Wait_Struct waitUpStruct;
       IP_RUN_Wait_Struct waitFullStruct;
       IP_RUN_Wait_Struct waitDownStruct;
   } IP_run;

   /*dma & imx sync */
#define DMA_SYNCRO_CH0 0x1
#define DMA_SYNCRO_CH1 0x2
#define DMA_SYNCRO_CH2 0x3
#define DMA_SYNCRO_CH3 0x4

#define IMX_A_SYNCRO_EN 0x1
#define IMX_B_SYNCRO_EN 0x2

   /*DMA ofst */
#define DMA_OFST_A_H 0x1
#define DMA_OFST_E_D 0x2

   /*Imx ofst */
#define IMX_DATA_OFST_ABCD 0x1
#define IMX_DATA_OFST_CDEF 0x2
#define IMX_DATA_OFST_EFGH 0x3
#define IMX_DATA_OFST_GHAB 0x4


   /*img_buff - int */
#define IMG_BUFF_A 0x1
#define IMG_BUFF_B 0x2
#define IMG_BUFF_C 0x4
#define IMG_BUFF_D 0x8
#define IMG_BUFF_E 0x10
#define IMG_BUFF_F 0x20
#define IMG_BUFF_G 0x40
#define IMG_BUFF_H 0x80

   /*IMX buffers */
#define IMX_A_CMDMEM 0x100
#define IMX_B_CMDMEM 0x200
#define IMX_COEFFI_A 0x400
#define IMX_COEFFI_B 0x800


   /* ports */
#define IMX_A_IO 0x1
#define IMX_B_IO 0x2 
#define NSF_IO 0x4
#define LDC_O 0x8
#define DCT_S 0x10
#define DCT_F 0x20
#ifdef BUILD_FOR_OMAP5
    #define NSF3_IO 0x40
#endif

/*DMA HW-sync channels*/
#define DMA_0 0x100
#define DMA_1 0x200
#define DMA_2 0x400
#define DMA_3 0x800


   /*Input memory for the LDC module*/
#define LDC_NO_INPUT_MEMORY   0x10
#define LDC_PRIVATE_INPUT_MEMORY  0x20
#define NSF_NO_INPUT_MEMORY 0x20
#ifdef BUILD_FOR_OMAP5
    #define NSF3_NO_INPUT_MEMORY 0x20
#endif

/*permanent connections */
#define IMX_A_COEFF_W 0x1000
#define IMX_B_COEFF_W 0x2000


   /*DMA channel sync channels*/
#define NO_DMA_SYNC_IP_RUN 0
#define DMA_SYNC_IPRUN_CH0 0x1
#define DMA_SYNC_IPRUN_CH1 0x2
#define DMA_SYNC_IPRUN_CH2 0x4
#define DMA_SYNC_IPRUN_CH3 0x8
#define DMA_SYNC_IPRUN_CH_0_1 	0x3
#define DMA_SYNC_IPRUN_CH_0_1_2 0x7
#define DMA_SYNC_IPRUN_CH_0_1_2_3 0xF




   /*IP_RUN specific macros*/
#ifdef CCS_PROJECT

//#define IP_RUN_Malloc(SIZE, BYTEALIGNMENT)      (((malloc(SIZE+BYTEALIGNMENT)+BYTEALIGNMENT-1)/BYTEALIGNMENT)*BYTEALIGNMENT)
#define IP_RUN_Malloc(SIZE, BYTEALIGNMENT)      (malloc(SIZE+BYTEALIGNMENT))
#define IP_RUN_Free(PTR)        if(PTR != NULL) free(PTR)
#define IP_RUN_Memset(PTR, SIZE, VAL)   memset(PTR, SIZE, VAL)
#define IP_RUN_Memcpy(PTR_Dst, PTR_Src, USIZE)   memcpy(PTR_Dst, PTR_Src, USIZE)

#else

#define IP_RUN_Malloc(SIZE, BYTEALIGNMENT)      TIMM_OSAL_MallocExtn(SIZE, TIMM_OSAL_TRUE, BYTEALIGNMENT, TIMMOSAL_MEM_SEGMENT_EXT, NULL)
#define IP_RUN_Free(PTR)        if(PTR != NULL) TIMM_OSAL_Free(PTR)
#define IP_RUN_Memset(PTR, SIZE, VAL)   TIMM_OSAL_Memset(PTR, SIZE, VAL)
#define IP_RUN_Memcpy(PTR_Dst, PTR_Src, USIZE)   TIMM_OSAL_Memcpy(PTR_Dst, PTR_Src, USIZE)

#endif

#define IP_RUN_ASSERT   IP_RUN_PARAMCHECK
#define IP_RUN_REQUIRE IP_RUN_PARAMCHECK
#define IP_RUN_ENSURE   IP_RUN_PARAMCHECK

   /*! \def IP_RUN_PARAMCHECK 
    * Macro to check whether given condition fails or satisfies;
    * If fails, exit with file-name, function-name and line number.
    */
#ifdef IP_RUN_TASK_BASED

#define IP_RUN_PARAMCHECK(C,V)  if (!(C)) {status = V;TIMM_OSAL_TraceFunction("ERROR: in %s [%s] :: %d \n", __FILE__,__FUNCTION__, __LINE__);goto EXIT;}
#define IP_RUN_EXIT_IF(_Cond,_ErrorCode) if ((_Cond)) {TIMM_OSAL_TraceFunction("Error :: %s : %s : %d :: Exiting because : %s\n",__FILE__, __FUNCTION__, __LINE__, #_Cond);goto EXIT;}

#else

#define IP_RUN_PARAMCHECK(C,V)  if (!(C)) {status = V;printf("ERROR: in %s [%s] :: %d \n", __FILE__,__FUNCTION__, __LINE__);goto EXIT;}
#define IP_RUN_EXIT_IF(_Cond,_ErrorCode) if ((_Cond)) {printf("Error :: %s : %s : %d :: Exiting because : %s\n",__FILE__, __FUNCTION__, __LINE__, #_Cond);goto EXIT;}

#endif


/*Function declarations*/


/* ===========================================================================*/
/**
 * IP_RUN_Get_Enabled_Modules()  Fills up the IP_RUN structure to provide information of enabled modules, which is extracted 
 * from the pipeline provided.  
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/
IP_RUN_RET_TYPE IP_RUN_Get_Enabled_Modules (IP_run* handle);



IP_RUN_RET_TYPE CENTAURUS_IP_RUN_Change_OCPTags(Uint16 SIMCOPDmaCtrl_TagCnt, IP_run *handle);

IP_RUN_RET_TYPE CENTAURUS_IP_RUN_fastRegisterAlgo (IP_run *handle);

/* ===========================================================================*/
/**
 * IP_RUN_RegisterAlgo()  Configures actual hardware registers of LDC, NSF and DMA with the configuration parameters
 * passed in the IP_run handle. These parameters are algorithm-specific. 
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/
IP_RUN_RET_TYPE CENTAURUS_IP_RUN_RegisterAlgo (IP_run *handle);



/* ===========================================================================*/
/**
 * IP_RUN_UnRegisterAlgo()  Releases the handles created during the RegisterAlgo call. 
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/
IP_RUN_RET_TYPE  CENTAURUS_IP_RUN_UnRegisterAlgo (IP_run *handle);



/* ===========================================================================*/
/**
 * IP_Run_Init()  This function creates the semaphores for use and IP_RUN task thread. It should be called only once before 
 * calling corresponding IP_Run_Deinit. If called more than once, it will return error. 
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/
IP_RUN_RET_TYPE CENTAURUS_IP_Run_Init();




/* ===========================================================================*/
/**
 * IP_Run_Deinit() This function deletes the semaphores created and also the IP_RUN task thread. It should be called only once after 
 * calling corresponding IP_Run_init. If called more than once, it will return error. 
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/
IP_RUN_RET_TYPE CENTAURUS_IP_Run_Deinit();



/* ===========================================================================*/
/**
 * IP_Run_Start()  This function queues up the IP_RUN handle and triggers the IP_RUN task to work with this passed handle.
 *
 * @param   handle          : Handle to the IP_RUN top-level structure.
 *
 * @return   status           : IP_RUN_SOK --> success, otherwise failure
 *
 */
/* ===========================================================================*/

IP_RUN_RET_TYPE CENTAURUS_IP_Run_Start( IP_run *handle);

void CENTAURUS_IP_Run_Wait(Uint8 edmaFlag, IP_RUN_Wait_Struct *waitStruct);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
