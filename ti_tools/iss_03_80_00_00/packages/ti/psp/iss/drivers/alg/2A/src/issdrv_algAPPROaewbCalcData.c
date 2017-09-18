
#include <ti/psp/iss/alg/aewb/appro2a/inc/alg_appro_aewb.h>


void APPRO_2AEngModeAR0331(Engineer_Config *EngineerCfg)
{
	int i;
	float UserRGBtoRGB[3][3] = {
		{1.496185,  -0.204725,  -0.291551},
		{-0.395150,  1.518724,  -0.123577},  
		{-0.032753,  -0.452567,  1.485316}
	};
	/* Engineer mode config start. */
	/* WB */
	EngineerCfg->AwbCfg.IsDbgAwbCal = 0; 

	EngineerCfg->AwbCfg.IsUpdateCal = 1;
	EngineerCfg->AwbCfg.LDegree = 2800;
	EngineerCfg->AwbCfg.LRgain = 139;
	EngineerCfg->AwbCfg.LBgain = 344;
	EngineerCfg->AwbCfg.HDegree = 6000;
	EngineerCfg->AwbCfg.HRgain = 215;
	EngineerCfg->AwbCfg.HBgain = 188;

	EngineerCfg->AwbCfg.User_AutoWide_Hdegree = 8000;
	EngineerCfg->AwbCfg.User_AutoWide_Ldegree = 2500;
	EngineerCfg->AwbCfg.User_AutoNormal_Hdegree = 5600;
	EngineerCfg->AwbCfg.User_AutoNormal_Ldegree = 4000;
	for(i=0;i<sizeof(EngineerCfg->AwbCfg.User_ModeRgain)/sizeof(EngineerCfg->AwbCfg.User_ModeRgain[0]);i++)
	{
		EngineerCfg->AwbCfg.User_ModeRgain[i]= 200;
		EngineerCfg->AwbCfg.User_ModeBgain[i]= 200;
	}
	EngineerCfg->AwbCfg.Global_Rgain = 100;
	EngineerCfg->AwbCfg.Global_Ggain = 100;	
	EngineerCfg->AwbCfg.Global_Bgain = 100;	
	/* RGB */
	EngineerCfg->RGBCfg.BlackOfset_R = 0;
	EngineerCfg->RGBCfg.BlackOfset_G = 0;
	EngineerCfg->RGBCfg.BlackOfset_B = 0;	
	EngineerCfg->RGBCfg.RGBtoRGB[0][0] = UserRGBtoRGB[0][0];
	EngineerCfg->RGBCfg.RGBtoRGB[0][1] = UserRGBtoRGB[0][1];
	EngineerCfg->RGBCfg.RGBtoRGB[0][2] = UserRGBtoRGB[0][2];
	EngineerCfg->RGBCfg.RGBtoRGB[1][0] = UserRGBtoRGB[1][0];
	EngineerCfg->RGBCfg.RGBtoRGB[1][1] = UserRGBtoRGB[1][1];
	EngineerCfg->RGBCfg.RGBtoRGB[1][2] = UserRGBtoRGB[1][2];
	EngineerCfg->RGBCfg.RGBtoRGB[2][0] = UserRGBtoRGB[2][0];
	EngineerCfg->RGBCfg.RGBtoRGB[2][1] = UserRGBtoRGB[2][1];
	EngineerCfg->RGBCfg.RGBtoRGB[2][2] = UserRGBtoRGB[2][2];
	EngineerCfg->RGBCfg.IsRunUserR2R = 1;
	/* others */
	EngineerCfg->NDShutterToNight = 12;
	EngineerCfg->NDShutterToDay= 25;
	EngineerCfg->SharpnessOfset = 0;
	EngineerCfg->DenoiseOfset = 0;
	EngineerCfg->AutoIrisAdjustLevel = 0;
	EngineerCfg->QualityIndex	= 0;
	EngineerCfg->CameraType = 0;
  	EngineerCfg->dcsub = -168;

	return;
}

void APPRO_2AEngModeMT9J003(Engineer_Config *EngineerCfg)
{
	int i;
	float UserRGBtoRGB[3][3] = {
		{  1.946600,  -0.848282,  -0.098319,  },	  		
		{  -0.183576,  1.593072,  -0.409496,  },			
		{  0.025312,  -0.498257,  1.472945,  },
	};
	/* Engineer mode config start. */
	/* WB */
	EngineerCfg->AwbCfg.IsDbgAwbCal = 0; 

	EngineerCfg->AwbCfg.IsUpdateCal = 1;
	EngineerCfg->AwbCfg.LDegree = 2800;
	EngineerCfg->AwbCfg.LRgain = 126;
	EngineerCfg->AwbCfg.LBgain = 247;
	EngineerCfg->AwbCfg.HDegree = 6000;
	EngineerCfg->AwbCfg.HRgain = 212;
	EngineerCfg->AwbCfg.HBgain = 147;

	EngineerCfg->AwbCfg.User_AutoWide_Hdegree = 8000;
	EngineerCfg->AwbCfg.User_AutoWide_Ldegree = 2500;
	EngineerCfg->AwbCfg.User_AutoNormal_Hdegree = 5600;
	EngineerCfg->AwbCfg.User_AutoNormal_Ldegree = 4000;
	for(i=0;i<sizeof(EngineerCfg->AwbCfg.User_ModeRgain)/sizeof(EngineerCfg->AwbCfg.User_ModeRgain[0]);i++)
	{
		EngineerCfg->AwbCfg.User_ModeRgain[i]= 200;
		EngineerCfg->AwbCfg.User_ModeBgain[i]= 200;
	}
	EngineerCfg->AwbCfg.Global_Rgain = 100;
	EngineerCfg->AwbCfg.Global_Ggain = 100;	
	EngineerCfg->AwbCfg.Global_Bgain = 100;	
	/* RGB */
	EngineerCfg->RGBCfg.BlackOfset_R = 0;
	EngineerCfg->RGBCfg.BlackOfset_G = 0;
	EngineerCfg->RGBCfg.BlackOfset_B = 0;	
	EngineerCfg->RGBCfg.RGBtoRGB[0][0] = UserRGBtoRGB[0][0];
	EngineerCfg->RGBCfg.RGBtoRGB[0][1] = UserRGBtoRGB[0][1];
	EngineerCfg->RGBCfg.RGBtoRGB[0][2] = UserRGBtoRGB[0][2];
	EngineerCfg->RGBCfg.RGBtoRGB[1][0] = UserRGBtoRGB[1][0];
	EngineerCfg->RGBCfg.RGBtoRGB[1][1] = UserRGBtoRGB[1][1];
	EngineerCfg->RGBCfg.RGBtoRGB[1][2] = UserRGBtoRGB[1][2];
	EngineerCfg->RGBCfg.RGBtoRGB[2][0] = UserRGBtoRGB[2][0];
	EngineerCfg->RGBCfg.RGBtoRGB[2][1] = UserRGBtoRGB[2][1];
	EngineerCfg->RGBCfg.RGBtoRGB[2][2] = UserRGBtoRGB[2][2];
	EngineerCfg->RGBCfg.IsRunUserR2R = 1;
	/* others */
	EngineerCfg->NDShutterToNight = 12;
	EngineerCfg->NDShutterToDay= 25;
	EngineerCfg->SharpnessOfset = 0;
	EngineerCfg->DenoiseOfset = 0;
	EngineerCfg->AutoIrisAdjustLevel = 0;
	EngineerCfg->QualityIndex	= 1;
	EngineerCfg->CameraType = 0;
  	EngineerCfg->dcsub = -168;

	return;
}


void APPRO_2AEngModeMN34041(Engineer_Config *EngineerCfg)
{
	int i;
	float UserRGBtoRGB[3][3] = {
		
		{1.817050605,	-0.855461855,	0.038411249},
		{-0.349097644,	1.772234602,	-0.42313696},
		{0.166092577,	-1.008338586,	1.842246012},

	};
	/* Engineer mode config start. */
	/* WB */
	EngineerCfg->AwbCfg.IsDbgAwbCal = 0; 

	EngineerCfg->AwbCfg.IsUpdateCal = 1;
	EngineerCfg->AwbCfg.LDegree = 2800;
	EngineerCfg->AwbCfg.LRgain = 197;
	EngineerCfg->AwbCfg.LBgain = 311;
	EngineerCfg->AwbCfg.HDegree = 6000;
	EngineerCfg->AwbCfg.HRgain = 271;
	EngineerCfg->AwbCfg.HBgain = 235;

	EngineerCfg->AwbCfg.User_AutoWide_Hdegree = 8000;
	EngineerCfg->AwbCfg.User_AutoWide_Ldegree = 2500;
	EngineerCfg->AwbCfg.User_AutoNormal_Hdegree = 5600;
	EngineerCfg->AwbCfg.User_AutoNormal_Ldegree = 4000;
	for(i=0;i<sizeof(EngineerCfg->AwbCfg.User_ModeRgain)/sizeof(EngineerCfg->AwbCfg.User_ModeRgain[0]);i++)
	{
		EngineerCfg->AwbCfg.User_ModeRgain[i]= 200;
		EngineerCfg->AwbCfg.User_ModeBgain[i]= 200;
	}
	EngineerCfg->AwbCfg.Global_Rgain = 100;
	EngineerCfg->AwbCfg.Global_Ggain = 100;	
	EngineerCfg->AwbCfg.Global_Bgain = 100;	
	/* RGB */
	EngineerCfg->RGBCfg.BlackOfset_R = 0;
	EngineerCfg->RGBCfg.BlackOfset_G = 0;
	EngineerCfg->RGBCfg.BlackOfset_B = 0;	
	EngineerCfg->RGBCfg.RGBtoRGB[0][0] = UserRGBtoRGB[0][0];
	EngineerCfg->RGBCfg.RGBtoRGB[0][1] = UserRGBtoRGB[0][1];
	EngineerCfg->RGBCfg.RGBtoRGB[0][2] = UserRGBtoRGB[0][2];
	EngineerCfg->RGBCfg.RGBtoRGB[1][0] = UserRGBtoRGB[1][0];
	EngineerCfg->RGBCfg.RGBtoRGB[1][1] = UserRGBtoRGB[1][1];
	EngineerCfg->RGBCfg.RGBtoRGB[1][2] = UserRGBtoRGB[1][2];
	EngineerCfg->RGBCfg.RGBtoRGB[2][0] = UserRGBtoRGB[2][0];
	EngineerCfg->RGBCfg.RGBtoRGB[2][1] = UserRGBtoRGB[2][1];
	EngineerCfg->RGBCfg.RGBtoRGB[2][2] = UserRGBtoRGB[2][2];
	EngineerCfg->RGBCfg.IsRunUserR2R = 1;
	/* others */
	EngineerCfg->NDShutterToNight = 12;
	EngineerCfg->NDShutterToDay= 25;
	EngineerCfg->SharpnessOfset = 0;
	EngineerCfg->DenoiseOfset = 0;
	EngineerCfg->AutoIrisAdjustLevel = 0;
	EngineerCfg->QualityIndex	= 0;
	EngineerCfg->CameraType = 1;
  	EngineerCfg->dcsub = -256;

	return;
}

#ifdef IMGS_SONY_IMX136		// modify by jem
void APPRO_2AEngModeIMX136(Engineer_Config *EngineerCfg)
{
	int i;

	float UserRGBtoRGB[3][3] = {				
		{1.806532439,	-0.779140248,	-0.027392191},
		{-0.201112856,	1.473768681,	-0.272655828},
		{0.04032953,	-0.539086093,	1.498756566}
	};

	/* Engineer mode config start. */
	/* WB */
	EngineerCfg->AwbCfg.IsDbgAwbCal = 0; 

	EngineerCfg->AwbCfg.IsUpdateCal = 1;

	EngineerCfg->AwbCfg.LDegree = 2800;
	EngineerCfg->AwbCfg.LRgain = 207;	//210;
	EngineerCfg->AwbCfg.LBgain = 348;	//362;
	EngineerCfg->AwbCfg.HDegree = 6500;
	EngineerCfg->AwbCfg.HRgain = 354;	//365;
	EngineerCfg->AwbCfg.HBgain = 185;	//192;

	EngineerCfg->AwbCfg.User_AutoWide_Hdegree = 8000;
	EngineerCfg->AwbCfg.User_AutoWide_Ldegree = 2500;
	EngineerCfg->AwbCfg.User_AutoNormal_Hdegree = 5600;
	EngineerCfg->AwbCfg.User_AutoNormal_Ldegree = 4000;
	for(i=0;i<sizeof(EngineerCfg->AwbCfg.User_ModeRgain)/sizeof(EngineerCfg->AwbCfg.User_ModeRgain[0]);i++)
	{
		EngineerCfg->AwbCfg.User_ModeRgain[i]= 200;
		EngineerCfg->AwbCfg.User_ModeBgain[i]= 200;
	}
	EngineerCfg->AwbCfg.Global_Rgain = 100;
	EngineerCfg->AwbCfg.Global_Ggain = 100;	
	EngineerCfg->AwbCfg.Global_Bgain = 100;	
	/* RGB */
	EngineerCfg->RGBCfg.BlackOfset_R = 0;
	EngineerCfg->RGBCfg.BlackOfset_G = 0;
	EngineerCfg->RGBCfg.BlackOfset_B = 0;	
	EngineerCfg->RGBCfg.RGBtoRGB[0][0] = UserRGBtoRGB[0][0];
	EngineerCfg->RGBCfg.RGBtoRGB[0][1] = UserRGBtoRGB[0][1];
	EngineerCfg->RGBCfg.RGBtoRGB[0][2] = UserRGBtoRGB[0][2];
	EngineerCfg->RGBCfg.RGBtoRGB[1][0] = UserRGBtoRGB[1][0];
	EngineerCfg->RGBCfg.RGBtoRGB[1][1] = UserRGBtoRGB[1][1];
	EngineerCfg->RGBCfg.RGBtoRGB[1][2] = UserRGBtoRGB[1][2];
	EngineerCfg->RGBCfg.RGBtoRGB[2][0] = UserRGBtoRGB[2][0];
	EngineerCfg->RGBCfg.RGBtoRGB[2][1] = UserRGBtoRGB[2][1];
	EngineerCfg->RGBCfg.RGBtoRGB[2][2] = UserRGBtoRGB[2][2];
	EngineerCfg->RGBCfg.IsRunUserR2R = 1;
	/* others */
	EngineerCfg->NDShutterToNight = 12;
	EngineerCfg->NDShutterToDay= 25;
	EngineerCfg->SharpnessOfset = 0;
	EngineerCfg->DenoiseOfset = 0;
	EngineerCfg->AutoIrisAdjustLevel = 0;
	EngineerCfg->QualityIndex	= 0;
	EngineerCfg->CameraType = 1;

	EngineerCfg->dcsub = 0;

	return;
}
#endif	/* IMGS_SONY_IMX136 */	

void *APPRO_2AEngModeParm(void)
{
	static Engineer_Config EngineerCfg;

#ifdef IMGS_MICRON_AR0331	 
	APPRO_2AEngModeAR0331(&EngineerCfg);
#elif IMGS_PANASONIC_MN34041
	APPRO_2AEngModeMN34041(&EngineerCfg);
#elif IMGS_SONY_IMX136	
	APPRO_2AEngModeIMX136(&EngineerCfg);
#else // the rest of all will use setting for IMGS_MICRON_MT9J003
	APPRO_2AEngModeMT9J003(&EngineerCfg);
#endif
	return &EngineerCfg;
}

