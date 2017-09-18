/** ==================================================================
 *  @file   TI_WDR_Engine.h                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _TI_WDR_ENGINE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _TI_WDR_ENGINE_H_

#define MAX_KNEE_PTS 5

typedef enum
{
	WDR_BAYER_GRBG = 0,
	WDR_BAYER_RGGB = 1,
	WDR_BAYER_GBRG = 2,
	WDR_BAYER_BGGR = 3
} WDR_ENGINE_BAYER_PATTERN;

typedef struct
{
	WDR_ENGINE_BAYER_PATTERN type;
	int isDataCompressed;
	int blackLevel;
	/* below section only valid when isDataCompressed = 1 */
    int numKneePts; /* number of knee points, max supported is MAX_KNEE_PTS, origin (0,0) is included */ 
    int thr[MAX_KNEE_PTS];   /* Array of values for the knee points P, before compression, origin (0,0) is thr[0]= 0 */
    int slope[MAX_KNEE_PTS]; /* slope for each piece-wise sections, usually first section's slope is 1 */
} WDR_ENGINE_SENSOR_FORMAT;

typedef struct
{
	int width;
	int height;
	int inputStride;
	int outputStride;
	WDR_ENGINE_SENSOR_FORMAT wdrSensor;
}	WDR_ENGINE_STATIC_PARAMS;

typedef struct
{
	int wdrMode;
	int update;
}	WDR_ENGINE_DYNAMIC_PARAMS;

/* ===================================================================
 *  @func     MSP_WDR_ProcessFrame                                               
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
Int32 WDR_ProcessFrame(WDR_ENGINE_DYNAMIC_PARAMS *dWdrParams,
	unsigned short* INPUT,
	unsigned short* OUTPUT);

/* ===================================================================
 *  @func     MSP_WDR_Engine_Init                                               
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
Int32 WDR_Engine_Init(WDR_ENGINE_STATIC_PARAMS *sWdrParams);

/* ===================================================================
 *  @func     MSP_WDR_Engine_DeInit                                               
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
Int32 WDR_Engine_DeInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _TI_WDR_ENGINE_H_
