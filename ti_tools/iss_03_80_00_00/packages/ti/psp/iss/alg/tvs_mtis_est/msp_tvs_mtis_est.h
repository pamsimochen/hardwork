/** ==================================================================
 *  @file   msp_tvs_mtis_est.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/tvs_mtis_est/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ==================================================================== Texas 
 * Instruments OMAP(TM) Platform Software (c) Copyright Texas Instruments,
 * Incorporated. All Rights Reserved. Use of this software is controlled by
 * the terms and conditions found in the license agreement under which this
 * software has been supplied.
 * ==================================================================== */
/* -------------------------------------------------------------------------- 
 */
/* 
 * msp_tvs_mtis_est.h
 * This header file defines the public interface of MSP VSTAB/ MTIS module for 
 * translational motion estimation for video stabilization and NOT compensation
 *
 * @path alg\tvs_mtis_est\
 *
 *@rev 1.0
 */
/* -------------------------------------------------------------------------- 
 */
/* ==========================================================================
 * ! ! Revision History ! =================================== ! ! 17-Aug-2009
 * Phanish HS [phanish.hs@ti.com]: Initial version !
 * ========================================================================== */

#ifndef _MSP_TVS_MTIS_H_
#define _MSP_TVS_MTIS_H_

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
#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h>

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
/*--------macros -----------------------------------------------*/
/*--------------------------- END ------------------------------*/

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_CREATE_PARAM - Create time parameter structure @param
     * sVertImgSizeBeforeBSC : Vertical size of frame that is input to BSC
     * @param sHorzImgSizeBeforeBSC : Horizontal size of frame that is input
     * to BSC @param sVertDownsampleRatioInImgSensor : Vertical downsampling
     * ratio in image sensor in video mode @param
     * sHorzDownSampleRatioInImgSensor : Horizontal downsampling ratio in
     * image sensor in video mode @param sVertImgSizeAfterIPIPE : Vertical
     * image size at the output after IPIPE resizer @param
     * sHorzImgSizeAfterIPIPE : Horizontal image size at the output after
     * IPIPE resizer @param sBounLines : Number of BSC data lines @param
     * sBounPels : Number of BSC data pels @param sMaxSearchAmpV : Max Search 
     * area along vertical direction @param sMaxSearchAmpH : Max Search area
     * along horizontal direction @param usKvMin : Min Panning coefficients
     * in vertical direction @param usKvMax : Max Panning coefficients in
     * vertical direction @param usKhMin : Min Panning coefficients in
     * horizontal direction @param usKhMax : Max Panning coefficients in
     * horizontal direction */
    /* ======================================================================= 
     */
    typedef struct {
        /* Input frame size settings */
        MSP_S16 sVertImgSizeBeforeBSC;                     /* Vertical size
                                                            * of frame that
                                                            * is input to BSC 
                                                            */
        MSP_S16 sHorzImgSizeBeforeBSC;                     /* Horizontal size 
                                                            * of frame that
                                                            * is input to BSC 
                                                            */
        MSP_S16 sVertDownsampleRatioInImgSensor;           /* Vertical
                                                            * downsampling
                                                            * ratio in image
                                                            * sensor in video 
                                                            * mode */
        MSP_S16 sHorzDownSampleRatioInImgSensor;           /* Horizontal
                                                            * downsampling
                                                            * ratio in image
                                                            * sensor in video 
                                                            * mode */

        // Output frame size settings (VS results are scaled according to
        // this frame size)
        MSP_S16 sVertImgSizeAfterIPIPE;                    // Vertical image
                                                           // size at the
                                                           // output after
                                                           // IPIPE resizer
        MSP_S16 sHorzImgSizeAfterIPIPE;                    // Horizontal
                                                           // image size at
                                                           // the output
                                                           // after IPIPE
                                                           // resizer

        /* 
         * Number of motion compensation pixels in the vertical and horizontal directions
         * These values should be relative to sVertImgSizeBeforeBSC and sHorzImgSizeBeforeBSC from above
         * It is best to compute these values as a percentage of sVertImgSizeBeforeBSC and sHorzImgSizeBeforeBSC
         * Recommended value is 10% of sVertImgSizeBeforeBSC and sHorzImgSizeBeforeBSC
         */
        MSP_S16 sBounLines;
        MSP_S16 sBounPels;

        /* 
         * Maximum MV search amplitudes in the vertical and horizontal directions
         * These values should be computed relative to sVertImgSizeBeforeBSC and sHorzImgSizeBeforeBSC from above
         * Recommended value for these inputs is -1, which lets VS select default values automatically
         */
        MSP_S16 sMaxSearchAmpV;
        MSP_S16 sMaxSearchAmpH;

        /* Panning coefficients */
        MSP_U16 usKvMin;
        MSP_U16 usKvMax;
        MSP_U16 usKhMin;
        MSP_U16 usKhMax;

        // Smart ISO Input Parameters
        MSP_S32 slMax_Blur;
        MSP_S32 slMax_AG;
        MSP_S32 slMax_DG;
        MSP_S32 slMin_Exp;
    } MSP_TVS_MTIS_CREATE_PARAM;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_MTISMOTIONPARAMS - Output motion related parameter
     * structure @param maxMVv Maximum vertical motion vector @param maxMVh
     * Maximum horizontal motion vector @param allMVv : Vertical component of 
     * all 9 MVs from the frame (3x3 grid) @param allMVh : Vertical component 
     * of all 9 MVs from the frame (3x3 grid) @param frame_period : Period
     * between frames @param Result_Motion : Resultant Motion */
    /* ======================================================================= 
     */
    typedef struct {
        // SISO Frame Output Parameters
        MSP_S16 maxMVv;                                    // Maximum
                                                           // vertical motion 
                                                           // vector
        MSP_S16 maxMVh;                                    // Maximum
                                                           // horizontal
                                                           // motion vector
        MSP_S16 allMVv[9];                                 // Vertical
                                                           // component of
                                                           // all 9 MVs from
                                                           // the frame (3x3
                                                           // grid)
        MSP_S16 allMVh[9];                                 // allMVh
        // Added on NOv 4,2008
        MSP_S16 frame_period;                              // Period between
                                                           // frames 
        // SISO Frame Output Parameters
        MSP_S32 Result_Motion;                             // Resultant
                                                           // Motion
    } MSP_TVS_MTIS_MTISMOTIONPARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_MTISPREVIEWPARAMS - Output parmeters from preview mode;
     * reused for capture instance @param sFastestObjH Maximum horizontal
     * motion vector @param sFastestObjV Maximum vertical motion vector
     * @param lMax_Blur : @param lMax_AG : @param lMax_DG : @param
     * lMin_Exp : */
    /* ======================================================================= 
     */

    typedef struct {
        MSP_S16 sFastestObjH;
        MSP_S16 sFastestObjV;

        /* Expressed in terms of # of pixels in VGA resolution. Found 3
         * pixels for a VGA frame to be satistfactory These quantities are
         * sensor dependent. */
        MSP_S32 lMax_Blur;
        MSP_S32 lMax_AG;                                   // Expressed as a
                                                           // ratio For e.g.
                                                           // 1x gain - 8x
                                                           // gain or 16x
                                                           // gain 
        MSP_S32 lMax_DG;                                   // Expressed as a
                                                           // ratio For e.g.
                                                           // 1x gain - 8x
                                                           // gain or 16x
                                                           // gain 
        MSP_S32 lMin_Exp;                                  // Expressed in
                                                           // milliseconds

    } MSP_TVS_MTIS_MTISPREVIEWPARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_FRAMEPARAMS - Input parmeters from capture mode; output
     * from preview is used for getting modified Gain and exposure parameters 
     * for capture. @param sFramePeriod @param sCurrentAG @param sCurrentDG 
     * : @param sCurrentExp : @param stPrvOutParams : */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_S16 sFramePeriod;                              // Frmae period 
        MSP_S32 sCurrentAG;                                // Current Analog
                                                           // Gain for the
                                                           // frame
        MSP_S32 sCurrentDG;                                // Current Digital 
                                                           // Gain for the
                                                           // frame
        MSP_S32 sCurrentExp;                               // Current
                                                           // Exposure time
                                                           // for the frame
        MSP_TVS_MTIS_MTISPREVIEWPARAMS stPrvOutParams;     // This is the
                                                           // output
                                                           // parameters
                                                           // computed from
                                                           // preview fed as
                                                           // input for the
                                                           // current capture 
                                                           // frame
    } MSP_TVS_MTIS_FRAMEPARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_MTISOUTPARAMS - Output MTIS/ Smart ISO parameter
     * structure @param SISO_Status Status after running the MTIS to compute 
     * the modified gain/exposure values @param SISO_AG Modified SISO analog
     * gain @param SISO_DG : Modified SISO digital gain @param SISO_Exp :
     * Modified SISO exposure param @param SISO_Motion : Motion info */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_S32 SISO_Status;                               // MTIS status
        MSP_S32 SISO_AG;                                   // MTIS Analog
                                                           // Gain
        MSP_S32 SISO_DG;                                   // MTIS Digital
                                                           // Gain
        MSP_S32 SISO_Exp;                                  // MTIS Exposure
                                                           // Time
        MSP_S32 SISO_Motion;                               // MTIS returned
                                                           // motion value
    } MSP_TVS_MTIS_MTISOUTPARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_INPUT_TYPE - Input type definition to distinguish Current 
     * frame and Previous frame @param MSP_TVS_MTIS_INPUT_CURR : Current
     * frame @param MSP_TVS_MTIS_INPUT_PREV : Previous frame */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_TVS_MTIS_INPUT_CURR = 0,
        MSP_TVS_MTIS_INPUT_PREV = 1
    } MSP_TVS_MTIS_INPUT_TYPE;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_INDEX_TYPE - Vtsab config parameters for changing between 
     * frames @param MSP_TVS_MTIS_INDEX_MAXMOTIONCOMPH: Index for
     * configuring max amplitude of motion compensation along H direction.
     * Default is -1 (also recommended), if not specified through MSP_Config
     * @param MSP_TVS_MTIS_INDEX_MAXMOTIONCOMPV: Index for configuring max
     * amplitude of motion compensation along Y direction Default is -1 (also
     * recommended), if not specified through MSP_Config @param
     * MSP_TVS_MTIS_INDEX_DISABLE : Index for disabling VSTAB for a frame */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_TVS_MTIS_INDEX_MAXMOTIONCOMPH = 0,
        MSP_TVS_MTIS_INDEX_MAXMOTIONCOMPV = 1,
        MSP_TVS_MTIS_INDEX_DISABLE = 2,
        MSP_TVS_MTIS_INDEX_CURRENTFRAMEPARAMS = 3,
        MSP_TVS_MTIS_INDEX_NEWPARAMS = 4
    } MSP_TVS_MTIS_INDEX_TYPE;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_QUERY_TYPE - Vtsab config parameters for changing between 
     * frames @param MSP_TVS_MTIS_QUERY_BSCPARAMS: BSC configurations can be 
     * queried using this command @param MSP_TVS_MTIS_QUERY_MEMREQS: Memory
     * requiements could as well be queried */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_TVS_MTIS_QUERY_BSCPARAMS = 0,
        MSP_TVS_MTIS_QUERY_MTISPARAMS = 1,
        MSP_TVS_MTIS_QUERY_MTISMOTIONPARAMS = 2,
        MSP_TVS_MTIS_QUERY_SCRATCHMEMREQS = 3,
        MSP_TVS_MTIS_QUERY_PERSISTENTMEMREQS = 4,
        MSP_TVS_MTIS_QUERY_EXTERNALMEMREQS = 5,
        MSP_TVS_MTIS_QUERY_MTISPRVOUTPARAMS = 6
    } MSP_TVS_MTIS_QUERY_TYPE;

    // VIDEO STABILIZATION CONTEXT
    /* typedef struct { //Amount of memory used MSP_U32
     * ulExternalDataMemoryNeeds; MSP_U32 ulInternalPersistentMemoryNeeds;
     * MSP_U32 ulInternalScratchMemoryNeeds; } MSP_TVS_MTIS_MEMREQS; */

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_BSC_PARAMS - OUTPUT BSC PARAMETERS DURING CREATION
     * @param lBsc_row_vct : @param lBsc_row_vpos : @param lBsc_row_vnum :
     * @param lBsc_row_vskip : @param lBsc_row_hpos : @param lBsc_row_hnum
     * : @param lBsc_row_hskip : @param lBsc_row_shf : @param lBsc_col_vct
     * : @param lBsc_col_vpos : @param lBsc_col_vnum : @param
     * lBsc_col_vskip : @param lBsc_col_hpos : @param lBsc_col_hnum :
     * @param lBsc_col_hskip : @param lBsc_col_shf : */
    /* ======================================================================= 
     */
    /* OUTPUT BSC PARAMETERS DURING CREATION */
    typedef struct {
        /* Settings for the BSC calculator. Use these settings to initialize
         * the BSC calculator. */
        MSP_S32 lBsc_row_vct;
        MSP_S32 lBsc_row_vpos;
        MSP_S32 lBsc_row_vnum;
        MSP_S32 lBsc_row_vskip;
        MSP_S32 lBsc_row_hpos;
        MSP_S32 lBsc_row_hnum;
        MSP_S32 lBsc_row_hskip;
        MSP_S32 lBsc_row_shf;
        MSP_S32 lBsc_col_vct;
        MSP_S32 lBsc_col_vpos;
        MSP_S32 lBsc_col_vnum;
        MSP_S32 lBsc_col_vskip;
        MSP_S32 lBsc_col_hpos;
        MSP_S32 lBsc_col_hnum;
        MSP_S32 lBsc_col_hskip;
        MSP_S32 lBsc_col_shf;
    } MSP_TVS_MTIS_BSC_PARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_BSC_FRAME_PARAMS - Input/ Output parameter structure for
     * new resolution + corresponding BSC register values @param
     * sVertImgSizeBeforeBSC Vertical size of frame that is input to BSC
     * @param sHorzImgSizeBeforeBSC Horizontal size of frame that is input to 
     * BSC @param sVertDownsampleRatioInImgSensor : Vertical downsampling
     * ratio in image sensor in video mode @param
     * sHorzDownSampleRatioInImgSensor : Horizontal downsampling ratio in
     * image sensor in video mode @param bCurrentSettings : TRUE for getting
     * the configurations for the current configurations of resolution FALSE
     * for getting initial configuration or previously set configurations
     * @param stBSCParams : BSC Params (output of a query call) */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_S16 sVertImgSizeBeforeBSC;                     /* Vertical size
                                                            * of frame that
                                                            * is input to BSC 
                                                            */
        MSP_S16 sHorzImgSizeBeforeBSC;                     /* Horizontal size 
                                                            * of frame that
                                                            * is input to BSC 
                                                            */
        MSP_S16 sVertDownsampleRatioInImgSensor;           /* Vertical
                                                            * downsampling
                                                            * ratio in image
                                                            * sensor in video 
                                                            * mode */
        MSP_S16 sHorzDownSampleRatioInImgSensor;           /* Horizontal
                                                            * downsampling
                                                            * ratio in image
                                                            * sensor in video 
                                                            * mode */
        MSP_BOOL bCurrentSettings;                         /* Should be 1 for 
                                                            * the new values
                                                            * of frame
                                                            * settings or if
                                                            * 0 returns the
                                                            * already applied 
                                                            * BSC settings */
        MSP_TVS_MTIS_BSC_PARAMS stBSCParams;
    } MSP_TVS_MTIS_BSC_FRAME_PARAMS;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_MEMORY_USED - OUTPUT MEM USED BY THE ALGORITHM @param
     * lExternalDataMemoryUsed @param lInternalPersistentMemoryUsed @param
     * lInternalScratchMemoryUsed : */
    /* ======================================================================= 
     */
    /* OUTPUT MEM USED BY THE ALGORITHM */
    typedef struct {
        /* Amount of memory used */
        MSP_S32 lExternalDataMemoryUsed;
        MSP_S32 lInternalPersistentMemoryUsed;
        MSP_S32 lInternalScratchMemoryUsed;
    } MSP_TVS_MTIS_MEMORY_USED;

    /* ======================================================================= 
     */
    /* MSP_TVS_MTIS_FRAME_OUT - FRAME OUTPUT - gives stabilized coordinates
     * and the confidence values @param sROh : X coord @param sROv : Y coord
     * @param usVertStatus : Status in the vertical direction @param
     * usHorzStatus : Status in the horizontal direction */
    /* ======================================================================= 
     */
    /* FRAME OUTPUT */
    typedef struct {
        /* Horizontal and vertical coordinates of the stabilized window These 
         * values should be interpreted relative to vertImgSizeBeforeBSC and
         * horzImgSizeBeforeBSC in VS_CreationParamsStruct If IPIPE resizer
         * is used to change frame size, these coordinates should be scaled
         * accordingly. */
        MSP_S16 sROh;
        MSP_S16 sROv;

        /* Status in the horizontal and vertical directions */
        MSP_U16 usVertStatus;
        MSP_U16 usHorzStatus;
    } MSP_TVS_MTIS_FRAME_OUT;

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes --------------------------------*/
/*--------macros ------------------------------------------------*/
/*--------------------------- END -------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_TVS_MTIS_H_ 
                                                            */
