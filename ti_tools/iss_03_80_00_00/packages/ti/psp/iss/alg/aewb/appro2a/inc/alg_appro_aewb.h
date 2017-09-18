/** ==================================================================
 *  @file   alg_appro_aewb.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/aewb/appro2a/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _ALG_APPRO_AEWB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_APPRO_AEWB_H_

#define UPDATE_SENSOR_GAIN		(1<<0)
#define UPDATE_SENSOR_SHUTTER	(1<<1)

#define UPDATE_MISC_PWM_VIDEO	(1<<0)
#define UPDATE_MISC_PWM_DC 		(1<<1)


typedef struct {
    int Gain;
    int Shutter;
    int update;
} SensorParm;

typedef struct {
    SensorParm sensor;
    void *ipipe;
    void *isif;
    unsigned int updateMisc;
    unsigned int PwmVideoDuty;
    unsigned int PwmVideoPeriod;
    unsigned int PwmDCDuty;
    unsigned int PwmDCPeriod;
} appro2a_output;

typedef struct _AWB_EngConfig{
	/* For get (Calibration) */
	int	IsDbgAwbCal;	//0:disable debug, 1:enable debug.
	int	CurrR;
	int	CurrG;
	int	CurrB;
	int	CurrGOR;
	int	CurrGOB;
	/* For set */
	int 	IsUpdateCal;	//0:no, 1:update
	int	LDegree;
	int	LRgain;
	int	LBgain;	
	int	HDegree;
	int	HRgain;
	int	HBgain;
	int 	User_AutoWide_Hdegree;
	int 	User_AutoWide_Ldegree;
	int 	User_AutoNormal_Hdegree;
	int 	User_AutoNormal_Ldegree;
	int   User_ModeRgain[6];
	int   User_ModeBgain[6];
	int 	Global_Rgain;	//base 100;
	int 	Global_Ggain;	//base 100;
	int 	Global_Bgain;	//base 100;	
}AWB_EngConfig;

typedef struct _RGB_EngConfig{
	int	BlackOfset_R;
	int	BlackOfset_G;
	int	BlackOfset_B;
	int 	IsRunUserR2R;	//1:run user RGB to RGB, 0:use system default. 
	float RGBtoRGB[3][3];
}RGB_EngConfig;

typedef struct _Engineer_Config{
	AWB_EngConfig AwbCfg;
	RGB_EngConfig RGBCfg;
	int 	NDShutterToNight;	/* Work only on ND_Auto */
	int 	NDShutterToDay;	/* (NDShutterToDay - NDShutterToNight) better >= 10 */
	int 	SharpnessOfset;
	int 	DenoiseOfset;
	int		AutoIrisAdjustLevel;
	int		dcsub;
	int		QualityIndex;
	int		CameraType;
}Engineer_Config;


enum {
    APPRO_CMD_START = 0,
    APPRO_CMD_FRAME_TIME,
    APPRO_CMD_AUTO_IRIS,
    APPRO_CMD_SHARPNESS,
    APPRO_CMD_SATURATION,
    APPRO_CMD_BRIGHTNESS,
    APPRO_CMD_CONTRAST,
    APPRO_CMD_BLC,
    APPRO_CMD_AEWB_ENABLE,
    APPRO_CMD_AEWB_TYPE,
    APPRO_CMD_AE_FRAME_PRIO,
    APPRO_CMD_AE_QUALITY_PRIO,
    APPRO_CMD_AE_FLICKER,
    APPRO_CMD_AWB_MODE,
    APPRO_CMD_GET_OUTPUT,
    APPRO_CMD_SET_ENGMODE,
    APPRO_CMD_END
};

/* ===================================================================
 *  @func     Appro2AInit                                               
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
void Appro2AInit(void);

/* ===================================================================
 *  @func     Appro2ACtrl                                               
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
void Appro2ACtrl(unsigned cmd, void *data);

/* ===================================================================
 *  @func     Appro2ACalc                                               
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
void Appro2ACalc(int winCtVert, int winCtHorz, int pixCtWin, void *pH3aMem);

/* ===================================================================
 *  @func     Appro2ARelease                                               
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
void Appro2ARelease(void);

/* ===================================================================
 *  @func     APPRO_2AEngModeParm                                               
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
void *APPRO_2AEngModeParm(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
