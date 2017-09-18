/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file msp_jpeg_dec.h
*
* This File contains declarations for msp_jpeg_dec.c
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

#ifndef _MSP_JPEG_DEC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _MSP_JPEG_DEC_H_

#include <ti/iss/framework/msp/msp.h>

/*--------macros ------------------------------------------------*/

/* ======================================================================= */
/**
 * @def MSP_JPEG_DEC_TRACE  - Trace macros
 */
/* ======================================================================= */
#define MSP_JPEG_DEC_PRINT(ARGS,...)  TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,ARGS,##__VA_ARGS__)

#define JPEG_EXIT_IF(_Cond,_ErrorCode) { \
    if ((_Cond)) { \
        status = _ErrorCode; \
        MSP_JPEG_DEC_PRINT("Error :: %s : %s : %d :: Exiting because : %s\n", \
                __FILE__, __FUNCTION__, __LINE__, #_Cond); \
        goto EXIT; \
    } \
}

/* ======================================================================= */
/* MSP_JPEG_DEC_PORT_TYPE - Port definition for input and output ports
 * @param MSP_JPEG_DEC_INPUT_PORT : Input port @param
 * MSP_JPEG_DEC_OUTPUT_PORT : Output port */
/* ======================================================================= */

typedef enum {
    MSP_JPEG_DEC_INPUT_PORT = 0,
    MSP_JPEG_DEC_OUTPUT_PORT = 1
} MSP_JPEG_DEC_PORT_TYPE;

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/

// extern Bool bMtcr2OcpErrIrqOccured, bOcpErrIrqOccured,
// bVlcdjDecodeErrIrqOccured, bDoneIrqOccured;

/* ======================================================================= */
/* MSP_JPEG_DEC_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE - Operation mode of the 
 * encoder input buffer @param MSP_JPEG_DEC_SLICE_MODE : Slice based
 * operation (Memory src alone) @param MSP_JPEG_DEC_FRAME_MODE : Frame based
 * operation @param MSP_JPEG_DEC_STITCH_MODE : For stitching the encoder
 * outputs (Memory) */
/* ======================================================================= */

typedef enum {
    MSP_JPEG_DEC_SLICE_MODE = 0,
    MSP_JPEG_DEC_FRAME_MODE = 1
} MSP_JPEG_DEC_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE;

#if 0
/* ======================================================================= */
/* MSP_JPEG_DEC_SOURCE_TYPE - Source type selection for JPEG encoding @param 
 * MSP_JPEGE_FROMMEMORY : Encode from memory */
/* ======================================================================= */
typedef enum {
    MSP_JPEG_DEC_FROMMEMORY
} MSP_JPEG_DEC_SOURCE_TYPE;

#endif

/* ======================================================================= */
/* MSP_JPEG_DEC_ROTATION_TYPE - Rotation selection for JPEG Decoding while
 * operating in frame mode and Decoding from memory @param
 * MSP_JPEG_DEC_ROTATION_UNUSED : No rotation @param MSP_JPEG_DEC_ROTATION_90 
 * : Rotate by 90 @param MSP_JPEG_DEC_ROTATION_180 : Rotate by 180 @param
 * MSP_JPEG_DEC_ROTATION_270 : Rotate by 270 */
/* ======================================================================= */
typedef enum {
    MSP_JPEG_DEC_ROTATION_UNUSED = 0,
    MSP_JPEG_DEC_ROTATION_90,
    MSP_JPEG_DEC_ROTATION_180,
    MSP_JPEG_DEC_ROTATION_270
} MSP_JPEG_DEC_ROTATION_TYPE;

/* ======================================================================= */
/* MSP_JPEG_DEC_COLORFORMAT_TYPE - Color format type enumeration @param
 * MSP_JPEG_DEC_COLORFORMAT_UNUSED : Default @param
 * MSP_JPEG_DEC_COLORFORMAT_YUV420PLANAR : 420 Planar format @param
 * MSP_JPEG_DEC_COLORFORMAT_YUV422PLANAR : 422 planar @param
 * MSP_JPEG_DEC_COLORFORMAT_YUV420NV12 : 420 NV12 @param
 * MSP_JPEG_DEC_COLORFORMAT_YUV422INTERLEAVED : 422 interleaved */
/* ======================================================================= */
typedef enum {
    JPEG_COLORFORMAT_UNUSED = 0,
    JPEG_COLORFORMAT_YUV420 = 420,
    JPEG_COLORFORMAT_YUV422 = 422,
    JPEG_COLORFORMAT_YUV444 = 444
} JPEG_COLORFORMAT_TYPE;

/* ========================================================================= */
/**
* MSP_JPEGD_OUTPUT_TYPE enumeration for maintianing the possible output formats
* i.e., RGB or YUV type for the decoded information.
* @param MSP_JPEGD_TO_YUV444P_FORMAT   Decode to YUV444P format
* @param MSP_JPEGD_TO_YUV420P_FORMAT   Decode to YUV420P format.
*/
/* ========================================================================= */
typedef enum MSP_JPEGD_OUTPUT_TYPE {
    MSP_JPEGD_TO_YUV444P_FORMAT,
    MSP_JPEGD_TO_YUV420P_FORMAT,
    MSP_JPEGD_TO_YUV422P_FORMAT,
    MSP_JPEGD_TO_YUV444I_FORMAT,
    MSP_JPEGD_TO_YUV420NV12_FORMAT,
    MSP_JPEGD_TO_YUV422I_FORMAT
} MSP_JPEGD_OUTPUT_TYPE;

/* ======================================================================= */
/* MSP_JPEG_DEC_CREATE_PARAM - Create time parameter structure @param
 * ulImageWidth : Input frame width allowed @param ulImageHeight : Input
 * frame height allowed @param Format : Color format used @param
 * eUnComprsdOpMode : Input operating mode (Slice, frame, etc) @param
 * eRotationParam : Rotation param to be specified if encode from memory is
 * used and input is operating in frame mode */
/* ======================================================================= */
typedef struct {
    MSP_U32 ulImageWidth;
    MSP_U32 ulImageHeight;
    JPEG_COLORFORMAT_TYPE Format;
    MSP_BOOL Isplanar;
    MSP_U16 ulNumMCUY;
    MSP_U16 ulNumMCUCb;
    MSP_U16 ulNumMCUCr;
    MSP_JPEG_DEC_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE eUnComprsdOpMode;
} MSP_JPEG_DEC_CREATE_PARAM;

typedef struct {
    MSP_JPEG_DEC_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE eUnComprsdOpMode;
    MSP_U32 ulSliceHeight;
} MSP_JPEG_DEC_SLICE_PARAM;

typedef struct {
    MSP_U32 nXOrg;
    MSP_U32 nYOrg;
    MSP_U32 nXLength;
    MSP_U32 nYLength;
} MSP_JPEG_DEC_SUBREGION_PARAM;

/* ======================================================================= */
/* MSP_JPEG_DEC_INDEX_TYPE - Image operation specific configuration @param
 * MSP_JPEG_DEC_INDEXTYPE_CREATE_PARAMS: Index for JPEG Decoder create time
 * params @param MSP_JPEGE_INDEX_QFACTOR : Index for Quality factor */
/* ======================================================================= */
typedef enum {
    MSP_JPEG_DEC_INDEXTYPE_CREATE_PARAMS = 0,
    MSP_JPEG_DEC_INDEXTYPE_SET_OUTPUT_FMT,
    MSP_JPEG_DEC_INDEXTYPE_ROT_PARAMS,
    MSP_JPEG_DEC_INDEXTYPE_SLICE_PARAMS,
    MSP_JPEG_DEC_INDEXTYPE_SUBREGION_PARAMS
} MSP_JPEG_DEC_INDEX_TYPE;

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes --------------------------------*/
/* =========================================================================== */
/**
 * @fn MSP_JPEG_DEC_init()  Initialize the MSP component for JPEG Decoder.
 *
 * @param [out] handle    : Handle to the instance of MSP JPEG DEC.
 * @param [in] eProfile   : Enumeration for profile
 *
 * @return Return         : Error type
 *
 */
/* =========================================================================== */
/* ===================================================================
 *  @func     MSP_JPEG_DEC_init                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_init(MSP_HANDLE handle, MSP_PROFILE_TYPE eProfile);

/* ===================================================================
 *  @func     MSP_JPEG_DEC_deInit                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_deInit(MSP_HANDLE handle);

/* ===================================================================
 *  @func     MSP_JPEG_DEC_open                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_open(MSP_HANDLE handle, MSP_PTR pCreateParam);

/* ===================================================================
 *  @func     MSP_JPEG_DEC_close                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_close(MSP_HANDLE handle);

/* ===================================================================
 *  @func     MSP_JPEG_DEC_process                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_process(MSP_HANDLE handle, MSP_PTR pArg,
                                    MSP_BUFHEADER_TYPE * ptBufHdr);
/* ===================================================================
 *  @func     MSP_JPEG_DEC_control                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_control(MSP_HANDLE handle, MSP_CTRLCMD_TYPE tCmd,
                                    MSP_PTR pCmdParam);
/* ===================================================================
 *  @func     MSP_JPEG_DEC_config                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_config(MSP_HANDLE handle,
                                   MSP_INDEXTYPE tConfigIndex,
                                   MSP_PTR pConfigParam);
/* ===================================================================
 *  @func     MSP_JPEG_DEC_query                                               
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
MSP_ERROR_TYPE MSP_JPEG_DEC_query(MSP_HANDLE handle, MSP_INDEXTYPE tQueryIndex,
                                  MSP_PTR pQueryParam);

/*--------------------------- END -------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
