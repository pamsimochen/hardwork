/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file msp_ldcnsf.h
 *
 * This header file defines the public interface of MSP LDC+NSF module.
 * 
 *
 * @path  $(DUCATIVOB)\alg\ldcnsf\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 03-08-2009 Sanish Mahadik: Changed to use Hungarian Notation, Interface changes 
 *! 25-05-2009 Sanish Mahadik: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef _MSP_LDCNSF_H_
#define _MSP_LDCNSF_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
   /****************************************************************
    * INCLUDE FILES
    ****************************************************************/
    /* ----- system and platform files -------------------------- */
   /*-------program files ----------------------------------------*/
#include "../../framework/msp/msp.h"
#include  "../../drivers/csl/iss/simcop/nsf/csl_nsf.h"
#include "ti/iss/framework/tools_library/inc/tools_tiler_view.h"

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
    typedef CSL_Nf2ShdConfig MSP_LDCNSF_ShdConfig;
    typedef CSL_Nf2EeLxConfig MSP_LDCNSF_EdgeConfig;
    typedef CSL_Nf2DsConfig MSP_LDCNSF_DesatConfig;
    typedef CSL_Nf2ThrConfig MSP_LDCNSF_FilterConfig;

   /*--------macros -----------------------------------------------*/

   /*--------------------------- END ------------------------------*/

    /* ======================================================================= 
     */
    /* ! \enum MSP_LDCNSF_PORT_TYPE Port definition for input and output
     * ports of MSP component (used in MSP_process call) @param
     * MSP_LDCNSF_INPUT_PORT : Input port @param MSP_LDCNSF_OUTPUT_PORT :
     * Output port */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_LDCNSF_INPUT_PORT = 0,
        MSP_LDCNSF_OUTPUT_PORT = 1
    } MSP_LDCNSF_PORT_TYPE;

    /* ======================================================================= 
     */
    /* ! \enum MSP_LDCNSF_SMOOTH_PARAM Smoothening parameter enumeration for
     * NSF @param MSP_LDCNSF_SMOOTH_0 :Soft-threshold slope =0 @param
     * MSP_LDCNSF_SMOOTH_1 :Soft-threshold slope =1/8 @param
     * MSP_LDCNSF_SMOOTH_2 :Soft-threshold slope =1/4 @param
     * MSP_LDCNSF_SMOOTH_3 :Soft-threshold slope =1/2 */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_LDCNSF_SMOOTH_0 = 0,
        MSP_LDCNSF_SMOOTH_1 = 1,
        MSP_LDCNSF_SMOOTH_2 = 2,
        MSP_LDCNSF_SMOOTH_3 = 3
    } MSP_LDCNSF_SMOOTH_PARAM;

    /* ======================================================================= 
     */
    /* ! \enum MSP_LDCNSF_PARAM_SET Enumeration to check whether given
     * feature is disabled or has to use default parameters or has to use
     * user-supplied parameters @param MSP_LDCNSF_PARAM_DISABLE : Disable
     * the feature @param MSP_LDCNSF_PARAM_DEFAULT : Use default parameters
     * @param MSP_LDCNSF_PARAM_USER : Use user-supplied parameters */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_LDCNSF_PARAM_DISABLE = 0,
        MSP_LDCNSF_PARAM_DEFAULT = 1,
        MSP_LDCNSF_PARAM_USER = 2
    } MSP_LDCNSF_PARAM_SET;

    /* ======================================================================= 
     */
    /* ! \struct MSP_LDCNSF_NSF_PARAM Create time parameter structure
     * @param bLumaEnable : Enable/disable processing of luma component
     * @param bChromaEnable : Enable/disable processing of chroma component
     * @param bSmoothLuma : Enable/disable smoothening for luma component
     * @param bSmoothChroma : Enable/disable smoothening for chroma component
     * @param eSmoothVal : Smoothening parameter for luma and chroma
     * components @param eFilterParam : Filtering parameter
     * enable/default/user-specified @param eShdParam : Shading correction
     * parameter enable/default/user-specified @param eEdgeParam : Edge
     * enhancement parameter enable/default/user-specified @param eDesatParam 
     * : Desaturation parameter enable/default/user-specified @param
     * rgnFilterVal : User-specified Filtering parameters @param rgnShdVal :
     * User-specified shading correction parameters @param rgnEdgeVal :
     * User-specified edge enhancement parameters @param rgnDesatVal :
     * User-specified desaturation parameters */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_BOOL bLumaEnable;
        MSP_BOOL bChromaEnable;
        MSP_BOOL bSmoothLuma;
        MSP_BOOL bSmoothChroma;
        MSP_LDCNSF_SMOOTH_PARAM eSmoothVal;
        MSP_LDCNSF_PARAM_SET eFilterParam;
        MSP_LDCNSF_PARAM_SET eShdParam;
        MSP_LDCNSF_PARAM_SET eEdgeParam;
        MSP_LDCNSF_PARAM_SET eDesatParam;
        MSP_S16 *rgnFilterVal;
        MSP_S16 *rgnShdVal;
        MSP_S16 *rgnEdgeVal;
        MSP_S16 *rgnDesatVal;
    } MSP_LDCNSF_NSF_PARAM;

    /* ========================================================================== 
     */
   /** MSP_LDCNSF_Y_INTERPOLATE_METHOD enumeration for interpolation method for Y data in LDC
    *
    * @param MSP_LDCNSF_YINTERPOLATION_BICUBIC    : Bicubic Interpolation
    * @param MSP_LDCNSF_YINTERPOLATION_BILINEAR   : Bilinear Interpolation
    */
    /* ========================================================================== 
     */
    typedef enum {
        MSP_LDCNSF_YINTERPOLATION_BICUBIC = 0,
        MSP_LDCNSF_YINTERPOLATION_BILINEAR = 1
    } MSP_LDCNSF_Y_INTERPOLATE_METHOD;

    /* ======================================================================= 
     */
    /* MSP_LDCNSF_DATAFORMAT - Data Format type for LDC+NSF MSP component
     * @param MSP_LDCNSF_YUV_FORMAT_YCBCR422 : YUV 422 data @param
     * MSP_LDCNSF_YUV_FORMAT_YCBCR420 : YUV420(NV12) data with interleaved UV 
     * planes @param MSP_LDCNSF_BAYER_FORMAT_UNPACK12 : BAYER data of type
     * unpacked 12-bit @param MSP_LDCNSF_BAYER_FORMAT_PACK12 : BAYER data of 
     * type packed 12-bit @param MSP_LDCNSF_BAYER_FORMAT_PACK8 : BAYER data
     * of type packed 8-bit @param MSP_LDCNSF_BAYER_FORMAT_ALAW8 : BAYER data 
     * of type ALAW 8-bit */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_LDCNSF_YUV_FORMAT_YCBCR422 = 0,
        MSP_LDCNSF_YUV_FORMAT_YCBCR420 = 1,
        MSP_LDCNSF_BAYER_FORMAT_UNPACK12 = 2,
        MSP_LDCNSF_BAYER_FORMAT_PACK12 = 3,
        MSP_LDCNSF_BAYER_FORMAT_PACK8 = 4,
        MSP_LDCNSF_BAYER_FORMAT_ALAW8 = 5
    } MSP_LDCNSF_DATAFORMAT;

    /* ========================================================================== 
     */
   /** MSP_LDCNSF_INIT_COLOR enumeration for Initial color for LD back mapping (used in Bayer data-format mode only)
    *
    * @param MSP_LDCNSF_INITCOLOR_RED         Red is the initial color pixel
    * @param MSP_LDCNSF_INITCOLOR_GREENRED    GreenRed Red is the initial color pixel
    * @param MSP_LDCNSF_INITCOLOR_GREENBLUE   GreenBlue is the initial color pixel
    * @param MSP_LDCNSF_INITCOLOR_BLUE        Blue is the initial color pixel
    */
    /* ========================================================================== 
     */
    typedef enum {
        MSP_LDCNSF_INITCOLOR_RED = 0,
        MSP_LDCNSF_INITCOLOR_GREENRED = 1,
        MSP_LDCNSF_INITCOLOR_GREENBLUE = 2,
        MSP_LDCNSF_INITCOLOR_BLUE = 3,
        MSP_LDCNSF_INITCOLOR_DISABLED = 4
    } MSP_LDCNSF_INIT_COLOR;

    /* ======================================================================= 
     */
    /* MSP_LDCNSF_LUT_PARAMS - Look-up Table related parameters @param
     * unLdcKvl : Vertical left magnification factor @param unLdcKhl :
     * Horizontal left magnification factor @param unLdcKvu : Vertical upper
     * magnification factor @param unLdcKhr : Horizontal right magnification
     * factor @param punLdcLutTable : Pointer to the look up table address
     * @param unLdcRth : Radial threshold @param ucRightShiftBits : Denotes
     * the number of right shift bits
     * 
     */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_U8 unLdcKvl;
        MSP_U8 unLdcKhl;
        MSP_U8 unLdcKvu;
        MSP_U8 unLdcKhr;
        MSP_U16 *punLdcLutTable;
        MSP_U16 unLdcRth;
        MSP_U8 ucRightShiftBits;
    } MSP_LDCNSF_LUT_PARAMS;

    /* ======================================================================= 
     */
    /* MSP_LDCNSF_AFFINE_PARAMS - Affine transform parameters for LDC @param
     * unAffineA : Affine transwarp A @param unAffineB : Affine transwarp B
     * @param unAffineC : Affine transwarp C @param unAffineD : Affine
     * transwarp D @param unAffineE : Affine transwarp E @param unAffineF :
     * Affine transwarp F -- -- -- -- -- -- -- -- | h_affine | = | unAffineA
     * unAffineB | * | h_input | + | unAffineC | | v_affine | | unAffineD
     * unAffineE | | v_input | | unAffineF | -- -- -- -- -- -- -- -- where
     * (h_affine, v_affine) denotes the tranformed coordinates after applying 
     * affine transformation (h_input, v_input) denotes the coordinates of
     * the input pixels (can be distorted or distortion corrected pixels) If 
     * s & r denote the desired scaling factor and rotation angle that needs
     * to be applied via affine transform to the input, unAffineA = s *
     * cos(r); unAffineB = s * sin(r); unAffineD = -s * sin(r); unAffineE = s 
     * * cos(r); unAffineC = h_0 - h_0 * A - v_0 * B; unAffineF = v_0 - h_0 * 
     * D - v_0 * E where (h_0, v_0) denotes the coordinates of the lens
     * center */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_S16 unAffineA;
        MSP_S16 unAffineB;
        MSP_S16 unAffineC;
        MSP_S16 unAffineD;
        MSP_S16 unAffineE;
        MSP_S16 unAffineF;
    } MSP_LDCNSF_AFFINE_PARAMS;

    /* ======================================================================= 
     */
    /* ! \struct MSP_LDCNSF_LENS_DISTORTION_PARAMS - Lens distortion
     * parameters for LDC @param unLensCentreY : Lens-centre Y-coordinate
     * @param unLensCentreX : Lens-centre X-coordinate @param ptLutParams :
     * Look-up table related parameters */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_U16 unLensCentreY;
        MSP_U16 unLensCentreX;
        MSP_LDCNSF_LUT_PARAMS *ptLutParams;
    } MSP_LDCNSF_LENS_DISTORTION_PARAMS;

    /* ======================================================================= 
     */
    /* ! \struct MSP_LDCNSF_START_LDC - Start-coodinate for the LDC from
     * where to begin the output @param unStartX : Output start X-coordinate 
     * * @param unStartY : Output start X-coordinate */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_U16 unStartX;
        MSP_U16 unStartY;
    } MSP_LDCNSF_START_LDC;

    /* ======================================================================= 
     */
    /* ! \struct MSP_VNF_LDC_PARAMS Create time parameter structure for LDC; 
     * used in the MSP_open() call @param unPixelPad : Pixel pad added
     * @param eInterpolationLuma : Y-interpolation method @param
     * ptLensDistortionParams : Lens distortion parameters @param
     * ptAffineParams : Affine-transform related parameters @param tStartXY : 
     * Start-coordinate value for LDC to output @param eBayerInitColor :
     * Init color for BAYER data in CAC mode */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_U16 unPixelPad;
        MSP_LDCNSF_Y_INTERPOLATE_METHOD eInterpolationLuma;
        MSP_LDCNSF_LENS_DISTORTION_PARAMS *ptLensDistortionParams;
        MSP_LDCNSF_AFFINE_PARAMS *ptAffineParams;
        MSP_LDCNSF_START_LDC tStartXY;
        MSP_LDCNSF_INIT_COLOR eBayerInitColor;
    } MSP_LDCNSF_LDC_PARAMS;

    /* ======================================================================= 
     */
    /* ! \enum MSP_LDCNSF_OPERATION_MODE Mode of operation of the LDC+NSF
     * module, according to the input and output formats. @param
     * MODE_LDC_AFFINE : Performs LDC + Affine Transform @param MODE_NSF_ONLY 
     * : Performs Spatial Filtering through NSF @param MODE_AFFINE_ONLY :
     * Performs only Affine transform @param MODE_AFFINE_NSF : Performs
     * Affine Transform + Spatial Filtering through NSF @param
     * MODE_LDC_AFFINE_NSF : Performs LDC + Affine Transform + Spatial
     * Filtering through NSF */
    /* ======================================================================= 
     */
    typedef enum {
        MODE_LDC_AFFINE = 0,
        MODE_CAC_AFFINE,
        MODE_NSF_ONLY,
        MODE_AFFINE_NSF,
        MODE_AFFINE_ONLY,
        MODE_LDC_AFFINE_NSF,
        MODE_FAST_NSF
    } MSP_LDCNSF_OPERATION_MODE;

    /* ======================================================================= 
     */
    /* ! \struct MSP_LDCNSF_CREATE_PARAMS Create time parameter structure
     * for LDCNSF; used in the MSP_open() call @param ulComputeWidth : Frame 
     * compute-width @param ulComputeHeight : Frame compute-height @param
     * ulInputStride : Input stride or line-offset in bytes @param
     * ulInputStrideChroma : Input stride or line-offset in bytes for chroma
     * component; used only when 420 NV12 format @param ulOutputStrideLuma :
     * Output stride or line-offset in bytes @param ulOutputStrideChroma :
     * Output stride or line-offset in bytes for chroma; used only when 420
     * NV12 format @param eOperateMode : Operation mode of the component
     * @param eInputFormat : Input format such as YUV 422 or YUV 420 @param
     * eOutputFormat : Output format such as YUV 422 or YUV 420 @param
     * ptLdcParams : Parameters for LDC configuration @param ptNsfParams :
     * Parameters for NSF configuration */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_U32 ulComputeWidth;
        MSP_U32 ulComputeHeight;
        MSP_U32 ulInputStride;
        MSP_U32 ulInputStrideChroma;
        MSP_U32 ulOutputStride;
        MSP_U32 ulOutputStrideChroma;
        MSP_LDCNSF_OPERATION_MODE eOperateMode;
        MSP_LDCNSF_DATAFORMAT eInputFormat;
        MSP_LDCNSF_DATAFORMAT eOutputFormat;
        MSP_LDCNSF_LDC_PARAMS tLdcParams;
        MSP_LDCNSF_NSF_PARAM tNsfParams;
        MSP_PTR pPrivate;

        MSP_U32 ulTilBuffWidth;
        MSP_U32 ulTilBuffHeight;
        MSP_CONFIG_ROT_TYPE eMspRotation;
    } MSP_LDCNSF_CREATE_PARAMS;

    /* ======================================================================= 
     */
    /* ! \enum MSP_LDCNSF_CONFIG_INDEX Configuaration index for the MSP
     * LDCNSF component which informs which configuration is to be performed
     * @param CONFIG_LDC_AFFINE : Configure affine parameters of LDC @param
     * CONFIG_LDC_START_POSITION : Configure output start co-ordinate of LDC
     * @param CONFIG_NSF_NOISE_FILTER : Configure NSF noise filter parameters
     * @param CONFIG_NSF_SHADING_CORRECTION : Configure NSF shading correction 
     * parameters @param CONFIG_NSF_EDGE : Configure NSF edge enhancement
     * parameters @param CONFIG_NSF_DESATURATION : Configure NSF desaturation
     * parameters */
    /* ======================================================================= 
     */

    typedef enum {
        CONFIG_LDC_AFFINE = 0,
        CONFIG_LDC_START_POSITION = 1,
        CONFIG_NSF_NOISE_FILTER = 2,
        CONFIG_NSF_SHADING_CORRECTION = 3,
        CONFIG_NSF_EDGE = 4,
        CONFIG_NSF_DESATURATION = 5
    } MSP_LDCNSF_CONFIG_INDEX;

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_LDCNSF_H_ 
                                                            */
