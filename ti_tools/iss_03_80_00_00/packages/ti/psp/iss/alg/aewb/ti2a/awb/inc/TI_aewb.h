/** ==================================================================
 *  @file   TI_aewb.h                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/alg/aewb/ti2a/awb/inc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef AEWB_TI_
#define AEWB_TI_

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_STEPS             8
#define NUM_RGB2RGB_MATRIXES  4

#define FD_FRAME_STEPS 				(5) 				//number of frames to wait between FD stages
#define FD_BRIGHTNESS_THRESHHOLD 	(8333.0*1000*1024) 	// threshold value to be crossed to trigger FD detection process

struct rgb2rgb_index {
    int color_temp;
    RGB2RGB_PARAM rgb2rgbparam;
    RGB2RGB_PARAM rgb2rgb2param;
};

extern unsigned char TI_WEIGHTING_SPOT[];
extern unsigned char TI_WEIGHTING_CENTER[];
extern unsigned char TI_WEIGHTING_MATRIX[];
extern Uint32 TI_YEE_TABLE[];

extern struct rgb2rgb_index rgb_maxtrixes[NUM_RGB2RGB_MATRIXES];

/* ===================================================================
 *  @func     TI_2A_config                                               
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
int TI_2A_config(int flicker_detection, int saldre);
/* ===================================================================
 *  @func     TI_2A_init_tables                                               
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
void TI_2A_init_tables(int width, int height);
/* ===================================================================
 *  @func     TI2A_applySettings                                               
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
static void TI2A_applySettings(IAEWB_Ae *curAe, IAEWB_Ae *nextAe, int numSmoothSteps, int step);
/* ===================================================================
 *  @func     TI_2A_SetEEValues                                               
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
short TI_2A_SetEEValues(int shift_val);
/* ===================================================================
 *  @func     RGB2RGB_stab                                               
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
int RGB2RGB_stab(int curr_RGB2RGBIndex);

#ifdef __cplusplus
}
#endif

#endif
