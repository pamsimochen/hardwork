/* ==============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file msp_vnf.h
 *
 * This header file defines the public interface of MSP for VNF module.
 * 
 *
 * @path  $(DUCATIVOB)\alg\vnf
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 28-07-2009 Venkat Peddigari : Updated LDC Params, CreateTime Params
 *! 23-07-2009 Venkat Peddigari : Modified to use Hungarian Notation, MSP Data Types
 *! 16-07-2009 Venkat Peddigari : Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *!that is, newest first.  The date format is dd-mm-yyyy.  
 * =========================================================================== */
#ifndef _MSP_VNF_H_
#define _MSP_VNF_H_

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
#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h> /* TODO remove MSP */ 
#include <ti/psp/iss/hal/iss/simcop/nsf/csl_nsf.h>

/*--------------------------- END -------------------------------*/
/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes --------------------------------*/
/*--------------------------- END -------------------------------*/
/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations ----------------------------------*/
typedef CSL_Nf2ShdConfig     MSP_VNF_ShdConfig;
typedef CSL_Nf2EeLxConfig    MSP_VNF_EdgeConfig;
typedef CSL_Nf2DsConfig      MSP_VNF_DesatConfig;
typedef CSL_Nf2ThrConfig     MSP_VNF_FilterConfig;


/*--------macros -----------------------------------------------*/


/* ======================================================================= */
/**
 * @def Byte alignment related macros used during memory allocation 
 */
/* ======================================================================= */
#define DEFAULT_BYTE_ALIGN     4
#define SIXTEEN_BYTE_ALIGN    16
#define THIRTYTWO_BYTE_ALIGN  32
#define ALPHA_LUT_SIZE        32


/*--------------------------- END ------------------------------*/


/* ======================================================================= */
/**
 * @enum MSP_VNF_PORT_TYPE
 * Port definition for input and output ports
 *
 * @param MSP_VNF_INPUT_PORT      : Input port
 * @param MSP_VNF_OUTPUT_PORT     : Output port
 */
/* ======================================================================= */
typedef enum 
{
   MSP_VNF_INPUT_PORT  = 0,
   MSP_VNF_OUTPUT_PORT = 1
}MSP_VNF_PORT_TYPE;


/* ======================================================================= */
/**
 * @enum MSP_VNF_PARAM_SET
 * Enumeration to check whether given feature is disabled or has to use default 
 * parameters or has to use user-supplied parameters
 *
 * @param MSP_VNF_PARAM_DISABLE   : Disable the feature
 * @param MSP_VNF_PARAM_DEFAULT   : Use default parameters 
 * @param MSP_VNF_PARAM_USER      : Use user-specified parameters
 */
/* ======================================================================= */
typedef enum
{
   MSP_VNF_PARAM_DISABLE = 0,
   MSP_VNF_PARAM_DEFAULT = 1,
   MSP_VNF_PARAM_USER    = 2
}MSP_VNF_PARAM_SET;


/* ======================================================================= */
/**
 * @enum MSP_VNF_SMOOTH_PARAM
 * Smooth parameter for NSF
 *
 * @param MSP_VNF_SFT_DISABLE     : Disable smooth parameter
 * @param MSP_VNF_SMOOTH_0        : Smooth parameter =0
 * @param MSP_VNF_SMOOTH_1        : Smooth parameter =1
 * @param MSP_VNF_SMOOTH_2        : Smooth parameter =2
 * @param MSP_VNF_SMOOTH_3        : Smooth parameter =3
 */
/* ======================================================================= */
typedef enum 
{
   MSP_VNF_SMOOTH_0 = 0,
   MSP_VNF_SMOOTH_1 = 1,
   MSP_VNF_SMOOTH_2 = 2,
   MSP_VNF_SMOOTH_3 = 3
}MSP_VNF_SMOOTH_PARAM;


/* ======================================================================= */
/**
 * @enum MSP_VNF_NSF_SET
 * Enumeration to check whether NSF2 is disabled
 * or enabled for chroma only
 * or enabled for both luma and chroma
 *
 * @param MSP_VNF_NSF_DISABLE   : disable the feature
 * @param MSP_VNF_NSF_CHROMA_ONLY   : chroma only NSF2
 * @param MSP_VNF_NSF_LUMA_CHROMA   : luma + chroma only NSF2
 */
/* ======================================================================= */
typedef enum
{
   MSP_VNF_NSF_DISABLE = 0,
   MSP_VNF_NSF_CHROMA_ONLY = 1,
   MSP_VNF_NSF_LUMA_CHROMA = 2
}MSP_VNF_NSF_SET;


/* ======================================================================= */
/**
 * @struct MSP_VNF_NSF_PARAMS 
 * Create time parameter structure for NSF; used in the MSP_open() call
 *
 * @param bLumaEnable             : Enable/disable processing of luma component
 * @param bChromaEnable           : Enable/disable processing of chroma component
 * @param bSmoothLuma             : Enable/disable smooth parameter for luma component
 * @param bSmoothChroma           : Enable/disable smooth parameter for chroma component
 * @param eSmoothVal              : Smooth parameter for luma and chroma components
 * @param eFilterParam            : Filter parameter enable/default/user-specified 
 * @param eShdParam               : Shading correction parameter enable/default/user-specified 
 * @param eEdgeParam              : Edge enhancement parameter enable/default/user-specified 
 * @param eDesatParam             : Desaturation parameter enable/default/user-specified 
 * @param rgnFilterVal            : User-specified filter parameters
 * @param rgnShdVal               : User-specified shading correction parameters
 * @param rgnEdgeVal              : User-specified edge enhancement parameters
 * @param rgnDesatVal             : User-specified desaturation parameters
 * @param rgnHalfFilterVal        : User-specified filter parameters
 * @param rgnQuadFilterVal        : User-specified filter parameters
 * @param MSP_VNF_NSF_SET : flag to control enabling/disabling of 
 *                                        spatial filter in stage A of 3DNF algorithm.
 * @param eHalfFilterParam        : Half stage Filter parameter default/user-specified incase of TNF2/3DNF
 * @param eQuadFilterParam        : Filter parameter default/user-specified incase of TNF2/3DNF
 * @param tnf3TS                  :  TS used by NFS2 operating on Diff Downsampled image in TNF3
 */
/* ======================================================================= */
typedef struct 
{
   MSP_BOOL                       bLumaEnable;
   MSP_BOOL                       bChromaEnable;
   MSP_BOOL                       bSmoothLuma;     
   MSP_BOOL                       bSmoothChroma;
   MSP_VNF_SMOOTH_PARAM           eSmoothVal;
   MSP_VNF_PARAM_SET              eFilterParam;
   MSP_VNF_PARAM_SET              eShdParam;
   MSP_VNF_PARAM_SET              eEdgeParam;
   MSP_VNF_PARAM_SET              eDesatParam;    
   MSP_S16                       *rgnFilterVal;    
   MSP_S16                       *rgnShdVal;    
   MSP_S16                       *rgnEdgeVal;    
   MSP_S16                       *rgnDesatVal;
   MSP_S16                       *rgnHalfFilterVal; 
   MSP_S16                       *rgnQuadFilterVal; 
   MSP_VNF_NSF_SET                eNsfSet;
   MSP_VNF_PARAM_SET              eHalfFilterParam;
   MSP_VNF_PARAM_SET              eQuadFilterParam;
   MSP_S16                        tnf3TS;

}MSP_VNF_NSF_PARAMS;


/* ======================================================================= */
/**
 * @enum MSP_VNF_Y_INTERPOLATE_METHOD
 * Enumeration constant for interpolation method to be used for Y data in LDC
 *
 * @param MSP_VNF_YINTERPOLATION_BICUBIC    : Bicubic Interpolation
 * @param MSP_VNF_YINTERPOLATION_BILINEAR   : Bilinear Interpolation
 */
/* ========================================================================== */
typedef enum
{
   MSP_VNF_YINTERPOLATION_BICUBIC  = 0,
   MSP_VNF_YINTERPOLATION_BILINEAR = 1
}MSP_VNF_Y_INTERPOLATE_METHOD;


/* ======================================================================= */
/**
 * @struct MSP_VNF_LUT_PARAMS
 * Look-up table related parameters for LDC
 *
 * @param unLdcKvl           : Vertical left magnification factor
 * @param unLdcKhl           : Horizontal left magnification factor
 * @param unLdcKvu           : Vertical upper magnification factor
 * @param unLdcKhr           : Horizontal right magnification factor
 * @param punLdcLutTable     : Pointer to the look up table address
 * @param unLdcRth           : Radial threshold
 * @param ucRightShiftBits   : Denotes the number of right shift bits
 */
/* ======================================================================= */
typedef struct{
   MSP_U8     unLdcKvl;
   MSP_U8     unLdcKhl;
   MSP_U8     unLdcKvu;
   MSP_U8     unLdcKhr;
   MSP_U16   *punLdcLutTable;
   MSP_U16    unLdcRth;
   MSP_U8     ucRightShiftBits; 
}MSP_VNF_LUT_PARAMS;


/* ======================================================================= */
/**
 * @struct MSP_VNF_AFFINE_PARAMS
 * Affine transform parameters for LDC
 *
 * @param unAffineA : Affine transwarp A 
 * @param unAffineB : Affine transwarp B
 * @param unAffineC : Affine transwarp C
 * @param unAffineD : Affine transwarp D
 * @param unAffineE : Affine transwarp E
 * @param unAffineF : Affine transwarp F
 *  --       --     --                   --     --     --       --       --
 * | h_affine  | = | unAffineA   unAffineB | * | h_input |  +  | unAffineC |
 * | v_affine  |   | unAffineD   unAffineE |   | v_input |     | unAffineF |
 *  --       --     --                   --     --     --       --       --
 * where (h_affine, v_affine) denotes the tranformed coordinates after applying affine transformation
 *       (h_input, v_input) denotes the coordinates of the input pixels (can be distorted or distortion corrected pixels)
 * 
 * If s & r denote the desired scaling factor and rotation angle that needs to be applied via affine transform to the input,
 * 
 * unAffineA = s * cos(r); unAffineB = s * sin(r); unAffineD = -s * sin(r); unAffineE = s * cos(r); (Uses S14Q12 Format)
 * unAffineC = h_0 - h_0 * A - v_0 * B; unAffineF = v_0 - h_0 * D - v_0 * E (Uses S16Q3 Format)
 * 
 * where (h_0, v_0) denotes the coordinates of the lens center
*/
/* ======================================================================= */
typedef struct{
   MSP_S16    unAffineA;
   MSP_S16    unAffineB;
   MSP_S16    unAffineC;
   MSP_S16    unAffineD;
   MSP_S16    unAffineE;
   MSP_S16    unAffineF;
}MSP_VNF_AFFINE_PARAMS;


/* ======================================================================= */
/**
 * @struct MSP_VNF_LENS_DISTORTION_PARAMS
 * Lens distortion parameters for LDC
 * 
 * @param unLensCentreY      : Lens-centre Y-coordinate 
 * @param unLensCentreX      : Lens-centre X-coordinate 
 * @param ptLutParams        : Look-up table related parameters
 */
/* ======================================================================= */
typedef struct 
{
   MSP_U16                   unLensCentreY;
   MSP_U16                   unLensCentreX;
   MSP_VNF_LUT_PARAMS       *ptLutParams;  
}MSP_VNF_LENS_DISTORTION_PARAMS;


/* ======================================================================= */
/**
 * @struct MSP_VNF_START_LDC
 * Start-coordinate for the LDC from where to begin the output
 *
 * @param unStartX      : Output start X-coordinate 
 * @param unStartY      : Output start Y-coordinate 
 */
/* ======================================================================= */
typedef struct
{
   MSP_U16    unStartX;
   MSP_U16    unStartY;
}MSP_VNF_START_LDC;


/* ======================================================================= */
/**
 * @struct MSP_VNF_LDC_PARAMS 
 * Create time parameter structure for LDC; used in the MSP_open() call
 *
 * @param unPixelPad                   : Pixel pad added 
 * @param eYInterpolationMethod        : Y-interpolation method
 * @param ptLensDistortionParams       : Lens distortion parameters
 * @param ptAffineParams               : Affine-transform related parameters
 * @param tStartXY                     : Start coordinate value for LDC to output
 */
/* ======================================================================= */
typedef struct 
{
   MSP_U16                             unPixelPad;
   MSP_VNF_Y_INTERPOLATE_METHOD        eYInterpolationMethod;
   MSP_VNF_LENS_DISTORTION_PARAMS     *ptLensDistortionParams;
   MSP_VNF_AFFINE_PARAMS              *ptAffineParams;
   MSP_VNF_START_LDC                   tStartXY;
}MSP_VNF_LDC_PARAMS;


/* ======================================================================= */
/**
 * @struct  MSP_VNF_TNF_PARAMS 
 * Create time parameter structure for TNF; used in the MSP_open() call
 *
 * @param unYComponentWt          : Assigned weight of luma component for motion computation (Q4 format is used)
 * @param unUComponentWt          : Assigned weight of chroma, cb or u component for motion computation (Q4 format is used)
 * @param unVComponentWt          : Assigned weight of chroma, cr or v component for motion computation (Q4 format is used)
 * @param unMaxBlendingFactor     : Maximum value of blending factor used for generating the temporal noise filter look-up table
 * @param unMotionThreshold       : Threshold for maximum allowed motion component, if motion exceeds this threshold then the 
 *                                  temporal filtered frame would be same as current frame
 * @param unDiffScaleFactor       : Scale factor used for boosting differential image in case of 3DNF/TNF2. Value should not be 0.
 *                                  Recommended value is 1.
 *                                  [TNF3] Same variable is used for the boosting the differential image precision in case of TNF3. 
 *                                   This vairiable is same as variable q used in the Ref implementation/slides
 *              
 * @param unDiffShiftFactor       : Shift factor used during derivation of alpha value. Should be expressed as pow(2,n) n >= 0.
 *                                  Recommended values is 0.
 * @param eTnfAlphaLUTParam       : enum to indicate whether to use default alpha LUT or user defined.
 * @param unStrengthOfTNF3        : values of the strength of Blending of TNF3
 * @param eTnf3Preset             : enum to indicate whether to use default values of unDiffScaleFactor, unMotionThreshold, 
 */
/* ======================================================================= */
typedef struct 
{
   MSP_U16    unYComponentWt;
   MSP_U16    unUComponentWt;
   MSP_U16    unVComponentWt;
   MSP_U16    unMaxBlendingFactor;
   MSP_U16    unMotionThreshold;
   MSP_U16    unDiffScaleFactor; 
   MSP_U16    unDiffShiftFactor;
   MSP_VNF_PARAM_SET  eTnfAlphaLUTParam; 
   MSP_U16    unStrengthOfTNF3;   //TNF strength: A   
   MSP_VNF_PARAM_SET eTnf3Preset;
   MSP_S16	  brightnessOffset;
   MSP_U16    unStrengthOfTNF3Chroma;   //TNF strength: A
}MSP_VNF_TNF_PARAMS;


/* ======================================================================= */
/**
 * @enum MSP_VNF_OPERATION_MODE
 * Mode of operation of the VNF module
 *
 * @param MODE_LDC_AFFINE_NSF_TNF      : Performs LDC, Affine Transform, Spatial and Temporal Filtering
 * @param MODE_AFFINE_NSF_TNF          : Performs only Affine Transform, Spatial and Temporal Filtering
 * @param MODE_TNF                     : Performs only Temporal filtering
 * @param MODE_3DNF                    : Performs improved spatial and temporal filtering (TNF2/3DNF)
 * @param MODE_TNF3                    : Performs improved spatial and temporal filtering (TNF3)
 */
/* ======================================================================= */
typedef enum
{
   MODE_LDC_AFFINE_NSF_TNF = 0,
   MODE_AFFINE_NSF_TNF     = 1,
   MODE_TNF                = 2,
   MODE_3DNF               = 3,
   MODE_LDC_TNF			   = 4,
   MODE_LDC				   = 5,
   MODE_TNF3               = 6,
   MODE_NSF2			   = 7
}MSP_VNF_OPERATION_MODE;


/* ======================================================================= */
/**
 * @enum MSP_VNF_DATA_FORMAT
 * Data format type for VNF
 *
 * @param MSP_VNF_DATA_FORMAT_YCBCR422      : YUV 422 data
 * @param MSP_VNF_DATA_FORMAT_YCBCR420      : YUV 420 data
 */
/* ======================================================================= */
typedef enum{
   MSP_VNF_YUV_FORMAT_YCBCR422 = 0,
   MSP_VNF_YUV_FORMAT_YCBCR420 = 1  
}MSP_VNF_DATA_FORMAT;


/* ======================================================================= */
/**
 * @struct MSP_VNF_CREATE_PARAMS 
 * Create time parameter structure for VNF; used in the MSP_open() call
 *
 * @param ulComputeWidth                    : Frame compute-width 
 * @param ulComputeHeight                   : Frame compute-height
 * @param ulInputStride                         : Input stride or line-offset in bytes for YUV422 Interleaved or luma in case of YUV420 NV12 input
 * @param ulInputStrideChroma              : Input stride or line-offset in bytes for chroma in case of YUV420 NV12 input
 * @param ulOutputStrideLuma                : Output stride or line-offset in bytes for luma
 * @param ulOutputStrideChroma            : Output stride or line-offset in bytes for chroma
 * @param eOperateMode                       : Operation mode of the component
 * @param eInputFormat                         : Input format such as YUV 422 or YUV 420
 * @param eOutputFormat                       : Output format such as YUV 422 or YUV 420
 * @param ptLdcParams                          : Parameters for LDC configuration
 * @param ptNsfParams                          : Parameters for NSF configuration
 * @param ptTnfParams                          : Parameters for TNF configuration
 * @param bDisablePipeDown                  : Enable/Disable Pipe Down
 */
/* ======================================================================= */
typedef struct 
{
   MSP_U32                        ulComputeWidth;
   MSP_U32                        ulComputeHeight;
   MSP_U32                        ulInputStride;
   MSP_U32                        ulInputStrideChroma;
   MSP_U32                        ulOutputStrideLuma;
   MSP_U32                        ulOutputStrideChroma;
   MSP_VNF_OPERATION_MODE         eOperateMode;
   MSP_VNF_DATA_FORMAT            eInputFormat;
   MSP_VNF_DATA_FORMAT            eOutputFormat;
   MSP_VNF_LDC_PARAMS            *ptLdcParams;
   MSP_VNF_NSF_PARAMS            *ptNsfParams;
   MSP_VNF_TNF_PARAMS            *ptTnfParams;
   MSP_BOOL                      bDisablePipeDown;
   MSP_U32                       ulTilBuffWidth;
   MSP_U32                       ulTilBuffHeight;
   MSP_CONFIG_ROT_TYPE           eMspRotation;
}MSP_VNF_CREATE_PARAMS;


/* ======================================================================= */
/**
 * @enum MSP_VNF_CONFIG_INDEX
 * Configuaration index used for the MSP VNF component to indicate which configuration needs to be performed
 *
 * @param CONFIG_VNF_AFFINE                      : Configure affine parameters of LDC for VNF
 * @param CONFIG_VNF_START_LDC                   : Configure output start co-ordinate of LDC for VNF
 * @param CONFIG_VNF_NSF_FILTER                  : Configure NSF filter parameters to be used for VNF
 * @param CONFIG_VNF_NSF_SHADING_CORRECTION      : Configure NSF shading correction parameters
 * @param CONFIG_VNF_NSF_EDGE                    : Configure NSF edge enhancement parameters
 * @param CONFIG_VNF_NSF_DESATURATION            : Configure NSF desaturation parameters
 * @param CONFIG_VNF_TNF                         : Configure TNF parameters used for VNF LUT along with 
 *                                                 weights for chroma and luma
 */
/* ======================================================================= */
typedef enum
{
   CONFIG_VNF_AFFINE                   = 0,
   CONFIG_VNF_START_LDC                = 1,
   CONFIG_VNF_NSF_FILTER               = 2,
   CONFIG_VNF_NSF_SHADING_CORRECTION   = 3,
   CONFIG_VNF_NSF_EDGE                 = 4,
   CONFIG_VNF_NSF_DESATURATION         = 5,
   CONFIG_VNF_TNF                      = 6
}MSP_VNF_CONFIG_INDEX;

MSP_ERROR_TYPE MSP_VNF_acquireResources(MSP_HANDLE handle);

MSP_ERROR_TYPE MSP_VNF_releaseResources(MSP_HANDLE handle);

char * getVersion_VNF();


char * getVersion_LDC();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_MSP_VNF_H_ */






