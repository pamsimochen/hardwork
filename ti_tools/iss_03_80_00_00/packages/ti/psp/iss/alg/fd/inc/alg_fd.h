/** ==================================================================
 *  @file   alg_fd.h                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/alg/fd/inc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _APP_FD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _APP_FD_H_

/*STATUS CODES*/
#define FD_SOK                  (1)     /* Success */
#define FD_FAILED               (-1)    /* Generic failure */
#define FD_NOTSUPPORTED         (-2)    /* Format (YUV, Height, Width or QF not Supported*/
#define FD_INVPARAMS            (-3)    /* invalid parameters*/
#define FD_INITFAILED           (-4)    /* initialization failed*/
#define FD_NOMEMORY             (-5)    /* no memory available*/
#define FD_INVALIDHANDLE        (-6)    /* invalid handle passed*/
#define FD_DEINITFAILED         (-7)    /* deinitialization failed*/

/* ======================================================================= */
/* MSP_FD_MIN_SIZE_PARAM - Minimum face size supported for FD
 *
 * @param MSP_FD_MIN_SIZE_20   : 20 x 20
 * @param MSP_FD_MIN_SIZE_25   : 25 x 25
  * @param MSP_FD_MIN_SIZE_32  : 32 x 32
 * @param MSP_FD_MIN_SIZE_40   : 40 x 40
 */
/* ======================================================================= */
typedef enum
{
    MSP_FD_MIN_SIZE_20 = 0,
    MSP_FD_MIN_SIZE_25 = 1,
    MSP_FD_MIN_SIZE_32 = 2,
    MSP_FD_MIN_SIZE_40 = 3

}MSP_FD_MIN_SIZE_PARAM;

/* ======================================================================= */
/*! \enum MSP_FD_DIR_PARAM
*  Direction parameter for FD
*
* @param MSP_FD_DIR_UP    : Direction UP
* @param MSP_FD_DIR_LEFT  : Direction LEFT
* @param MSP_FD_DIR_RIGHT : Direction RIGHT
*/
/* ======================================================================= */
typedef enum
{
    MSP_FD_DIR_UP = 0,
    MSP_FD_DIR_RIGHT = 1,
    MSP_FD_DIR_LEFT = 2

}MSP_FD_DIR_PARAM;

/* ======================================================================= */
/*! \struct  MSP_FD_ROI_PARAMS
 * ROI parameter structure for FD; used in the MSP_open() call
 *
 * @param ulInputImageStartX : Start X
 * @param ulInputImageStartY : Start Y
 * @param ulSizeX            : Stride Width X
 * @param ulSizeY            : Stride Height Y
 */
/* ======================================================================= */
typedef struct
{
    MSP_U32 ulInputImageStartX;
    MSP_U32 ulInputImageStartY;
    MSP_U32 ulSizeX;
    MSP_U32 ulSizeY;

}MSP_FD_ROI_PARAMS;

/* ======================================================================= */
/*! \struct  MSP_FD_CREATE_PARAMS
* Create time parameter structure for FD; used in the MSP_open() call
* @param ulThresholdValue  : Face detection threshold
* @param ulInputWidth      : Input width
* @param ulInputHeight     : Input height
* @param ulMinFaceSize     : Minimum face size
* @param ulDirection       : Direction of FD
* @param ptFdRoi           : Parameters for FD ROI configuration
 @param tFdResults         : Face Detect results structure
 */
/* ======================================================================= */
typedef struct
{
    MSP_U32                ulThresholdValue;
    MSP_U32                ulInputWidth;
    MSP_U32                ulInputHeight;
    MSP_FD_MIN_SIZE_PARAM  ulMinFaceSize;
    MSP_FD_DIR_PARAM       ulDirection;
    MSP_FD_ROI_PARAMS      *ptFdRoi;

} MSP_FD_CREATE_PARAMS;

/* ======================================================================= */
    /*! \struct  MSP_FD_POSITION
    * Face detection  position
    *
    * @param resultX                        :  Coodinate of the face in X position
    * @param resultX                        :  Coodinate of the face in Y position
    * @param resultConfidenceLevel          :  Confidence level in asserting an area as face
    * @param resultSize                     :  Size of the face
    * @param resultAngle                    :  Orientation of the face
    */
   /* ======================================================================= */
typedef struct
{
    MSP_U16 ulResultX;
    MSP_U16 ulResultY;
    MSP_U16 ulResultConfidenceLevel;
    MSP_U16 ulResultSize;
    MSP_U16 ulResultAngle;

}MSP_FD_POSITION;

/* ======================================================================= */
/*! \struct  MSP_FD_RESULT_PARAMS
   * Face detection sensor output parameters
    *
    * @param tFacePosition :  Structure containig face position
    * @param faceCount     :  No of faces detected
 */
/* ======================================================================= */
typedef struct
{
   MSP_U16         ulFaceCount;
   MSP_FD_POSITION tFacePosition[35];

}MSP_FD_RESULT_PARAMS;

/* Function prototypes */
/* ===================================================================
 *  @func     app_fd_init                                               
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
CSL_Status app_fd_init(MSP_FD_CREATE_PARAMS *pApp_params);
/* ===================================================================
 *  @func     app_fd_deinit                                               
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
CSL_Status app_fd_deinit();
/* ===================================================================
 *  @func     app_fd_Addr_init                                               
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
CSL_Status app_fd_Addr_init( MSP_BUFHEADER_TYPE *ptBufaddr);
/* ===================================================================
 *  @func     fd_execute                                               
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
CSL_Status fd_execute();
/* ===================================================================
 *  @func     FD_get_all_face_detect_positions                                               
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
CSL_Status FD_get_all_face_detect_positions( MSP_FD_RESULT_PARAMS *ptresultParam);
/* ===================================================================
 *  @func     FD_get_num_faces                                               
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
void FD_get_num_faces(MSP_FD_RESULT_PARAMS *ptresultParam);

/* ===================================================================
 *  @func     fd_config_direction_update                                               
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
CSL_Status fd_config_direction_update(MSP_FD_DIR_PARAM  fdDirection);
/* ===================================================================
 *  @func     fd_config_threshold_update                                               
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
CSL_Status fd_config_threshold_update(MSP_U32 fdThreshold);
/* ===================================================================
 *  @func     fd_config_minfacesize_update                                               
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
CSL_Status fd_config_minfacesize_update(MSP_FD_MIN_SIZE_PARAM  minFaceSize);
/* ===================================================================
 *  @func     fd_config_ROI_update                                               
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
CSL_Status fd_config_ROI_update(MSP_FD_ROI_PARAMS  *pFdRoiPrm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // _APP_FD_H_

