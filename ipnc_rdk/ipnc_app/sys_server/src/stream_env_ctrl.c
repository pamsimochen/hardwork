/** ===========================================================================
* @file av_server_ctrl.c** @path $(IPNCPATH)\sys_server\src\** @desc* .
* Copyright (c) Appro Photoelectron Inc.  2009
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <asm/types.h>
#include <file_msg_drv.h>
#include <stream_env_ctrl.h>
#include <ApproDrvMsg.h>
#include <Appro_interface.h>
#include <system_default.h>
#include <system_control.h>
#include <rtsp_ctrl.h>
#include <Frame_rate_ctrl.h>
#include <ipnc_ver.h>

#define FLAG_STREAM1	(0x01)
#define FLAG_STREAM2	(0x02)
#define FLAG_STREAM3	(0x04)

#define SYS_DEBUG_PRINT
#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)

const int audio_samplerate[2] = {8000,16000};
const int audio_bitrate_aac[2][3] = {{24000,36000,48000},{32000,48000,64000}};
const int audio_bitrate_g711[2][1] = {{64000},{128000}};

int SetDmvaParams();

int CheckValueBound(int value, int min, int max)
{
	value = (value<min) ? min:value;
	value = (value>max) ? max:value;

	return value;
}
static void attach_opt(char *dst, char *src)
{
	if (strlen(dst) > 0)
		strcat(dst, " ");

	strcat(dst, src);
}

/**
* @brief Start stream
*
* @param *pConfig Pointer to stream settings
* @retval 0 Success
* @retval -1 Fail.
*/
int StartStream(StreamEnv_t* pConfig)
{
	char cmd[1024] = {0};
	char cmdopt[1024] = " ";
	char *stream_type[3] = {"SINGLESTREAM", "DUALSTREAM", "TRISTREAM"};
	char *video_codec_mode[CODEC_COMB_NUM] =
		{"SINGLE_H264", "SINGLE_MPEG4","SINGLE_JPEG",
		 "H264_JPEG", "MPEG4_JPEG", "DUAL_H264", "DUAL_MPEG4", "H264_MPEG4",
		 "TRIPLE_H264", "TRIPLE_MPEG4"};
	char *audio_SR[2] = {"AUDIO 8000","AUDIO 16000"};
	char *codec_tpye[3] = {"H264", "MPEG4","JPEG"};
	char *mecfg_config1[4] = {"HIGH_SPEED1", "HIGH_SPEED1", "HIGH_QUALITY1", "SVC1"};
	char *mecfg_config2[4] = {"HIGH_SPEED2", "HIGH_SPEED2", "HIGH_QUALITY2", "SVC2"};
	char *aewb_con[4] = {"", "AE", "AWB","AEWB"};
	char *aewb_ven[3] = {"NONE2A", "APPRO2A", "TI2A"};
	char demoFlg[] 	= "DEMO";
	char vaDemo[] 	= "VA";
	char adv_VS[] 	= "VS";
	char adv_LDC[] 	= "LDC";
	char adv_SNF[] 	= "SNF";
	char adv_TNF[] 	= "TNF";
	char *adv_VNFMode[2] = {"VNF_SPEED", "VNF_QUALITY"};
	char *glbceMode[3] = {"", "GLBCE HS", "GLBCE HQ"};
	char *glbceStr[3] = {"LOW", "MED", "HIGH"};
	char *sw_mecfg1, *sw_mecfg2;
	int chipConfig = CheckCpuSpeed();
	static int loaddone=0;

	int H264_PORTNUM_1 = pConfig -> H264_PORTNUM_1;
        int H264_PORTNUM_2 = pConfig -> H264_PORTNUM_2;
        int MPEG4_PORTNUM_1 = pConfig -> MPEG4_PORTNUM_1;
        int MPEG4_PORTNUM_2 = pConfig -> MPEG4_PORTNUM_2;
        int MJPEG_PORTNUM_1 = pConfig -> MJPEG_PORTNUM_1;


    printf("~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("pConfig->nDemoCfg = %d\n", pConfig->nDemoCfg); 
    printf("~~~~~~~~~~~~~~~~~~~~~~\n");
	if(pConfig->nDemoCfg == VA_DEMO)
	{
	    fSetDmvaEnable(1);
		attach_opt(cmdopt, vaDemo);
	}
	else
	{
	    fSetDmvaEnable(0);
	}

	if(pConfig->nDemoCfg != VA_DEMO)
	{
		if(pConfig->nDemoCfg)
			attach_opt(cmdopt, demoFlg);
	}
    printf("~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("cmdopt = %s\n", cmdopt); 
    printf("~~~~~~~~~~~~~~~~~~~~~~\n");

	printf("\n*****************************************************************\n");
	printf("\n    IPNC BUILD VERSION: %s	\n", IPNC_APP_VERSION);
	printf("\n*****************************************************************\n\n");

	fSetChipConfig(chipConfig);

	switch(pConfig->nDemoCfg)
	{
		case VNF_DEMO:
			pConfig -> vsEnable 	= 0;
			pConfig -> ldcEnable 	= 0;
			pConfig -> vnfEnable 	= 1;
			break;

		case VS_DEMO:
			pConfig -> vsEnable 	= 1;
			pConfig -> ldcEnable 	= 0;
			pConfig -> vnfEnable 	= 0;
			break;

		case LDC_DEMO:
			pConfig -> vsEnable 	= 0;
			pConfig -> ldcEnable 	= 1;
			pConfig -> vnfEnable 	= 0;
			break;

		case ROI_CENTER_DEMO:
			pConfig -> vsEnable 	= 0;
			pConfig -> ldcEnable 	= 0;
			pConfig -> vnfEnable 	= 0;
			break;
    }

    printf("pConfig->vsEnable = %d, pConfig->ldcEnable = %d, pConfig->vnfEnable = %d\n", pConfig->vsEnable, pConfig->ldcEnable, pConfig->vnfEnable);

	if((pConfig->nDemoCfg == VNF_DEMO)||(pConfig->nDemoCfg == VS_DEMO)||(pConfig->nDemoCfg == LDC_DEMO)||(pConfig->nDemoCfg == ROI_CENTER_DEMO))
	{
		if(pConfig->nDemoCfg == VNF_DEMO)
		{
			pConfig -> vnfMode = 5; pConfig -> vnfStrength = 1;
			fSetVnfMode(pConfig -> vnfMode);fSetVnfStrength(pConfig -> vnfStrength);
		}
		if(pConfig->nDemoCfg == ROI_CENTER_DEMO)
		{
			pConfig -> nBitrate1 = 512000; pConfig -> nBitrate2 = 512000;			
		}
		else
		{
			pConfig -> nBitrate1 = 8000000; pConfig -> nBitrate2 = 8000000;
		}
		fSetMP41bitrate(pConfig -> nBitrate1); fSetMP42bitrate(pConfig -> nBitrate2);
		pConfig -> nStreamType = DUAL_STREAM;
		pConfig->nVideocodecmode = CODEC_COMB_DUAL_H264;
		pConfig->nVideocodecres  = 1;
		fSetVideoMode(pConfig -> nStreamType);
		fSetVideoCodecCombo(2);
		fSetVideoCodecRes(pConfig -> nVideocodecres);
		fSetVideoCodecMode(pConfig -> nVideocodecmode);
	}

	if(pConfig -> powerMode == 0)
	{
		pConfig -> ldcEnable = 0;
		pConfig -> vnfEnable = 0;
	}

    printf("\n~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("pConfig->powerMode = %d\n", pConfig->powerMode); 
    printf("~~~~~~~~~~~~~~~~~~~~~~\n\n");

	if (pConfig -> vsEnable) {
		attach_opt(cmdopt, adv_VS);
	}
	fSetVstabValue(pConfig -> vsEnable);

	if (pConfig -> ldcEnable) {
		attach_opt(cmdopt, adv_LDC);
	}
	fSetLdcValue(pConfig -> ldcEnable);

	if (pConfig -> vnfEnable) {
		if((pConfig -> vnfMode!=0)&&(pConfig -> vnfMode!=3))
			attach_opt(cmdopt, adv_SNF);
		if((pConfig -> vnfMode!=1)&&(pConfig -> vnfMode!=4))
			attach_opt(cmdopt, adv_TNF);
	}
	fSetVnfValue(pConfig -> vnfEnable);

    printf("pConfig->powerMode= %d\n", pConfig->powerMode);

	if(pConfig -> powerMode == 1)
	{
		if(pConfig -> vnfMode>2)
			attach_opt(cmdopt, adv_VNFMode[1]);
		else
			attach_opt(cmdopt, adv_VNFMode[0]);
	}

    printf("pConfig->nStreamType = %d\n", pConfig->nStreamType);
    printf("pConfig -> nVideocodecmode = %d\n",pConfig -> nVideocodecmode );

	attach_opt(cmdopt, stream_type[pConfig -> nStreamType]);
	attach_opt(cmdopt, video_codec_mode[pConfig -> nVideocodecmode]);

    printf("\n~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("cmdopt = %s\n", cmdopt); 
    printf("~~~~~~~~~~~~~~~~~~~~~~\n\n");


	if(pConfig -> nCodectype1!=H264_CODEC) {
		pConfig -> nMEConfig1 = ENCPRESET_AUTO;
		fSetMEConfig1(pConfig -> nMEConfig1);
	}
	if(pConfig -> nCodectype2!=H264_CODEC) {
		pConfig -> nMEConfig2 = ENCPRESET_AUTO;
		fSetMEConfig2(pConfig -> nMEConfig2);
	}

	sw_mecfg1 = mecfg_config1[pConfig -> nMEConfig1];
	sw_mecfg2 = mecfg_config2[pConfig -> nMEConfig2];

	if(pConfig -> audioenable) {
		if(pConfig->audioSampleRate >= 0 && pConfig->audioSampleRate < 2) {
			attach_opt(cmdopt, audio_SR[pConfig->audioSampleRate]);
		}
	}
    printf("\n~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("pConfig -> nAEWswitch = %d\n", pConfig -> nAEWswitch); 
    printf("~~~~~~~~~~~~~~~~~~~~~~\n\n");


#ifdef _ONLY_TI2A
	if(pConfig -> nAEWswitch==1) {
		pConfig -> nAEWswitch = 2;
		fSetImageAEW(pConfig -> nAEWswitch);
	}
#endif

#ifdef _ONLY_APPRO2A
	if(pConfig -> nAEWswitch==2) {
		pConfig -> nAEWswitch = 1;
		fSetImageAEW(pConfig -> nAEWswitch);
	}
#endif

#ifdef WDR_ON_MODE /* Switch OFF AE */
	if(pConfig -> nAEWtype&1) {
		pConfig -> nAEWtype &= 2;
		fSetImageAEWType(pConfig -> nAEWtype);
	}
#endif

	if(pConfig->nAEWswitch >= 0 && pConfig->nAEWswitch < 3)
		attach_opt(cmdopt, aewb_ven[pConfig->nAEWswitch]);

	if(pConfig->nAEWtype >= 0 && pConfig->nAEWtype < 4)
		attach_opt(cmdopt, aewb_con[pConfig->nAEWtype]);

	if(pConfig -> dynRange) {
		attach_opt(cmdopt, glbceMode[pConfig -> dynRange]);
		attach_opt(cmdopt, glbceStr[pConfig -> dynRangeStrength]);
	}

#ifdef SYS_DEBUG_PRINT
	printf("DEMOCFG Value	: %d\n", pConfig -> nDemoCfg);
	printf("Videocodecmode	: %d\n", pConfig -> nVideocodecmode);
	printf("Videocodecres 	: %d\n", pConfig -> nVideocodecres);
	printf("streamtype 	: %d\n", pConfig -> nStreamType);
#endif
    printf("DEMOCFG Value	: %d\n", pConfig -> nDemoCfg);
	printf("Videocodecmode	: %d\n", pConfig -> nVideocodecmode);
	printf("Videocodecres 	: %d\n", pConfig -> nVideocodecres);
	printf("streamtype 	: %d\n", pConfig -> nStreamType);

	switch (pConfig -> nVideocodecmode) {
		case CODEC_COMB_TRIPLE_MPEG4:
		{	// Dual MPEG4 + MJPEG
			fSetStreamActive(1, 1, 1, 0, 0, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode9\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MPEG4 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1,sw_mecfg2, pConfig -> nJpegQuality);

		    if(pConfig -> vsEnable)
            		{
				fSetVideoSize(1, VS_W, VS_H);
				fSetVideoSize(2, VS_W, VS_H);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);
				fSetStreamConfig(2, VS_W, VS_H, codec_tpye[2], MJPEG_PORTNUM_1);
			}
            		else
            		{
				fSetVideoSize(1, 1920, 1080);
				fSetVideoSize(2, 1920, 1080);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);
				fSetStreamConfig(2, 1920, 1080, codec_tpye[2], MJPEG_PORTNUM_1);
			}

			break;
		}
		case CODEC_COMB_TRIPLE_H264:
		{	// Dual H.264 + MJPEG
			fSetStreamActive(1, 0, 0, 0, 1, 1);
			if (pConfig -> nVideocodecres == 3)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 10MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);

				fSetVideoSize(1, 3648, 2736);
				fSetVideoSize(5, 3648, 2736);
				fSetVideoSize(6, 720, 480);
				fSetStreamConfig(0, 3648, 2736, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
				fSetStreamConfig(2, 3648, 2736, codec_tpye[2], MJPEG_PORTNUM_1);

			}
			else if (pConfig -> nVideocodecres == 2)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 8MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);

				fSetVideoSize(1, 3264, 2448);
				fSetVideoSize(5, 3264, 2448);
				fSetVideoSize(6, 720, 480);
				fSetStreamConfig(0, 3264, 2448, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
				fSetStreamConfig(2, 3264, 2448, codec_tpye[2], MJPEG_PORTNUM_1);

			}
			else if (pConfig -> nVideocodecres == 1)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 5MP_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);

				fSetVideoSize(1, 2560, 1920);
				fSetVideoSize(5, 2560, 1920);
				fSetVideoSize(6, 720, 480);
				fSetStreamConfig(0, 2560, 1920, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
				fSetStreamConfig(2, 2560, 1920, codec_tpye[2], MJPEG_PORTNUM_1);

			}
			else {
				if (pConfig -> nVideocodecres != 0)
				{
					pConfig -> nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode8\n");
				}
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s H264 %s MJPEG %d &\n", cmdopt,sw_mecfg1,sw_mecfg2,pConfig -> nJpegQuality);
				fprintf(stderr, "\n vsenable : %d\n", pConfig -> vsEnable);

				if(pConfig -> vsEnable)
				{
					fSetVideoSize(1, VS_W, VS_H);
					fSetVideoSize(5, VS_W, VS_H);
					fSetVideoSize(6, 720, 480);
					fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
					fSetStreamConfig(2, VS_W, VS_H, codec_tpye[2], MJPEG_PORTNUM_1);
				}
				else
				{
					fSetVideoSize(1, 1920, 1080);
					fSetVideoSize(5, 1920, 1080);
					fSetVideoSize(6, 720, 480);
					fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
					fSetStreamConfig(2, 1920, 1080, codec_tpye[2], MJPEG_PORTNUM_1);
				}
			}

			break;
		}
		case CODEC_COMB_H264_MPEG4:
		{	// H.264 + MPEG4
			fSetStreamActive(0, 0, 1, 0, 1, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode7\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s MPEG4 %s &\n",
				cmdopt,sw_mecfg1,sw_mecfg2);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(5, VS_W, VS_H);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);
			}
			else
			{
				fSetVideoSize(5, 1920, 1080);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);

			}

			break;
		}
		case CODEC_COMB_DUAL_MPEG4:
		{	// DUAL MPEG4
			fSetStreamActive(0, 1, 1, 0, 0, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode6\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MPEG4 %s &\n",
				cmdopt,sw_mecfg1,sw_mecfg2);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(2, VS_W, VS_H);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);
			}
			else
			{
				fSetVideoSize(2, 1920, 1080);
				fSetVideoSize(3, 720, 480);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[1], MPEG4_PORTNUM_2);
			}

			break;
		}
		case CODEC_COMB_DUAL_H264:
		{	// DUAL H.264
			fSetStreamActive(0, 0, 0, 0, 1, 1);

		    if(pConfig -> nVideocodecres == 1)
			{
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_1080P H264 %s H264 %s &\n",
							cmdopt,sw_mecfg1,sw_mecfg2);

			    if(pConfig -> vsEnable)
				{
					fSetVideoSize(5, VS_W, VS_H);
					fSetVideoSize(6, VS_W, VS_H);
					fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_2);
				}
				else
				{
#ifdef USE_MCTNF
					if((pConfig->nDemoCfg == VNF_DEMO) ||
					   (pConfig->nDemoCfg == ROI_CENTER_DEMO))
					{
						fSetVideoSize(5, 1280, 720);
						fSetVideoSize(6, 1280, 720);
						fSetStreamConfig(0, 1280, 720, codec_tpye[0], H264_PORTNUM_1);
						fSetStreamConfig(1, 1280, 720, codec_tpye[0], H264_PORTNUM_2);					
					}
					else
					{
						fSetVideoSize(5, 1920, 1080);
						fSetVideoSize(6, 1920, 1080);
						fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
						fSetStreamConfig(1, 1920, 1080, codec_tpye[0], H264_PORTNUM_2);					
					}
#else				
					fSetVideoSize(5, 1920, 1080);
					fSetVideoSize(6, 1920, 1080);
					fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, 1920, 1080, codec_tpye[0], H264_PORTNUM_2);
#endif					
				}
			}
			else
			{
				if (pConfig -> nVideocodecres != 0)
				{
					pConfig -> nVideocodecres = 0;
					fSetVideoCodecRes(0);
					__E("\nCODEC Resolution Error mode5\n");
				}
				sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s H264 %s &\n",
							cmdopt,sw_mecfg1,sw_mecfg2);

			    if(pConfig -> vsEnable)
				{
					fSetVideoSize(5, VS_W, VS_H);
					fSetVideoSize(6, 720, 480);
					fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
				}
				else
				{
					fSetVideoSize(5, 1920, 1080);
					fSetVideoSize(6, 720, 480);
					fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
					fSetStreamConfig(1, 720, 480, codec_tpye[0], H264_PORTNUM_2);
				}
			}

			break;
		}
		case CODEC_COMB_MPEG4_JPEG:
		{	// MPEG4 + JPEG
			fSetStreamActive(1, 1, 0, 0, 0, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode4\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1, pConfig -> nJpegQuality);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(1, 720, 480);
				fSetVideoSize(2, VS_W, VS_H);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[2], MJPEG_PORTNUM_1);
			}
			else
			{
				fSetVideoSize(1, 720, 480);
				fSetVideoSize(2, 1920, 1080);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[1], MPEG4_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[2], MJPEG_PORTNUM_1);
			}

			break;
		}
		case CODEC_COMB_H264_JPEG:
		{	// H.264 + JPEG
			fSetStreamActive(1, 0, 0, 0, 1, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode3\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s MJPEG %d &\n",
				cmdopt,sw_mecfg1, pConfig -> nJpegQuality);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(1, 720, 480);
				fSetVideoSize(5, VS_W, VS_H);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[2], MJPEG_PORTNUM_1);
			}
			else
			{
				fSetVideoSize(1, 720, 480);
				fSetVideoSize(5, 1920, 1080);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
				fSetStreamConfig(1, 720, 480, codec_tpye[2], MJPEG_PORTNUM_1);
			}

			break;
		}
		case CODEC_COMB_SINGLE_JPEG:
		{	//Single JPEG
			fSetStreamActive(1, 0, 0, 0, 0, 0);

			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode2\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MJPEG %d &\n",
				cmdopt, pConfig -> nJpegQuality);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(1, VS_W, VS_H);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[2], MJPEG_PORTNUM_1);
			}
			else
			{
				fSetVideoSize(1, 1920, 1080);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[2], MJPEG_PORTNUM_1);
			}

			break;
		}
		case CODEC_COMB_SINGLE_MPEG4:
		{	// SINGLE MPEG4
			fSetStreamActive(0, 1, 0, 0, 0, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode1\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 MPEG4 %s &\n",
				cmdopt,sw_mecfg1);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(2, VS_W, VS_H);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[1], MPEG4_PORTNUM_1);
			}
			else
			{
				fSetVideoSize(2, 1920, 1080);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[1], MPEG4_PORTNUM_1);
			}

			break;
		}
		case CODEC_COMB_SINGLE_H264:
		default :
		{	// SINGLE H.264
			if (pConfig -> nVideocodecmode != CODEC_COMB_SINGLE_H264)
			{
				__E("\nCODEC Mode Error\n");
				pConfig -> nVideocodecmode = CODEC_COMB_SINGLE_H264;
				fSetVideoCodecMode(CODEC_COMB_SINGLE_H264);
			}
			fSetStreamActive(0, 0, 0, 0, 1, 0);
			if (pConfig -> nVideocodecres != 0)
			{
				pConfig -> nVideocodecres = 0;
				fSetVideoCodecRes(0);
				__E("\nCODEC Resolution Error mode0\n");
			}
			sprintf(cmd, "./bin/ipnc_rdk_mcfw.out %s 1080P_D1 H264 %s &\n",
				cmdopt,sw_mecfg1);

		    if(pConfig -> vsEnable)
			{
				fSetVideoSize(5, VS_W, VS_H);
				fSetStreamConfig(0, VS_W, VS_H, codec_tpye[0], H264_PORTNUM_1);
			}
			else
			{
				fSetVideoSize(5, 1920, 1080);
				fSetStreamConfig(0, 1920, 1080, codec_tpye[0], H264_PORTNUM_1);
			}

			break;
		}
	}

	/*
	 *	'SetCodecROIDemoParam()' is called at the end because 'fSetStreamConfig()'
     *  overwrites the ROI params by calling 'fResetCodecROIParam()' fn.	 
	 */
	if(pConfig->nDemoCfg == ROI_CENTER_DEMO)
	{		
#ifdef USE_MCTNF						
		SetCodecROIDemoParam(1280,720);
#else
		SetCodecROIDemoParam(1920,1080);
#endif			
	}	
	
	if(loaddone==0)
	{
		printf(cmd);
		system(cmd);
	}

	return 0;
}

/**
* @brief Wait AV server ready
*
* @param stream_flag Stream flag to tell which stream is available.
* @retval 0 Success
* @retval -1 Fail.
*/
int WaitStreamReady(__u8 stream_flag)
{
#define BOOT_PROC_TIMEOUT_CNT	(2000)
	AV_DATA vol_data;
	int count=0, ret;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	vol_data.serial = -1;

	if (stream_flag == 0) {
		while(count++ < BOOT_PROC_TIMEOUT_CNT){
			if(pSysInfo->lan_config.codectype1 == MJPEG_CODEC)
				ret = GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &vol_data);
			else
				ret = GetAVData(AV_OP_GET_MPEG4_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);
		}
	}
	if (stream_flag == 1){
		while(count++ < BOOT_PROC_TIMEOUT_CNT){
			if(pSysInfo->lan_config.codectype2 == MJPEG_CODEC)
				ret = GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &vol_data);
			else
				ret = GetAVData(AV_OP_GET_MPEG4_CIF_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);
		}
	}
	if (stream_flag == 2){
		while(count++ < BOOT_PROC_TIMEOUT_CNT){
			ret = GetAVData(AV_OP_GET_MPEG4_CIF_SERIAL, -1, &vol_data);
			if(ret == RET_SUCCESS && vol_data.serial > 10){
				return 0;
			}
			usleep(33333);
		}
	}
	return -1;
}

/**
* @brief Initial Stream server
*
* @param *pConfig Stream server comunicate setting
* @retval 0 Success
* @retval -1 Fail.
*/
int Init_StreamEnv(StreamEnv_t* pConfig)
{
	__u8 stream_flag = 0;
	McFWInitStatus initStatus;

	if(ApproDrvInit(SYS_MSG_TYPE)){
		__E("ApproDrvInit fail\n");
		return -1;
	}

	/*	This code makes sure McFW message handler is up and running */
	while(ApproPollingMsg() < 0){
		usleep(240000);
	}

	/* Wait for McFW init Done */
	initStatus.initDone = 0;
	do
	{
		GetMcFWInitStatus(&initStatus);
		if(initStatus.initDone == 0)
		{
			usleep(500 * 1000);
		}
	}while(initStatus.initDone == 0);

	/* Get the stream memory address from McFW */
	if(func_get_mem(NULL)){
		ApproDrvExit();
		__E("CMEM map fail\n");
		return -1;
	}

	SetVideoCodecType(pConfig -> nVideocodecmode, pConfig->nVideocodecres);

	if(pConfig -> nVideocodecmode >= CODEC_COMB_TRIPLE_H264) {
		stream_flag = (FLAG_STREAM1|FLAG_STREAM2|FLAG_STREAM3);
	}
	else if(pConfig -> nVideocodecmode >= CODEC_COMB_H264_JPEG) {
		stream_flag = (FLAG_STREAM1|FLAG_STREAM2);
	}
	else {
		stream_flag = FLAG_STREAM1;
	}
	SetVideoCodecCfg(0);

	if(WaitStreamReady(pConfig -> nStreamType) != 0) {
		ApproDrvExit();
		__E("WaitStreamReady Fail.\n");
		return -1;
	}

	SetDisplayValue(pConfig -> nDisplay);
	SetImage2AVendor(pConfig -> nAEWswitch);
	SetImage2AType(pConfig -> nAEWtype);
	Set2APriority(pConfig -> expPriority);
	SetTvSystem(pConfig -> imagesource);
	SetCamDayNight(pConfig -> nDayNight);
	SetWhiteBalance(pConfig -> nWhiteBalance);
	SetBacklight(pConfig -> nBackLight);
	SetBrightness(pConfig -> nBrightness);
	SetContrast(pConfig -> nContrast);
	SetSaturation(pConfig -> nSaturation);
	SetSharpness(pConfig -> nSharpness);
	SetHistEnable(pConfig -> histogram);
	SetFramectrl(pConfig->framectrl);
	SetMirr(pConfig -> nMirror);
	SetBinningSkip(pConfig -> nBinning);

	SetAdvMode();
	SetFaceDetectPrm();
	SetDynRangeParam();
	/* Set Motion Parameters */
	SetMotionDetectParam();
	SetTimeDateDetail();
	SetOSDEnable(stream_flag);

	if((stream_flag & FLAG_STREAM1) > 0) {
		SetMP41bitrate(pConfig -> nBitrate1);
		SetEncRateControl(0, pConfig -> nRateControl1);
		SetFramerate1(pConfig -> nFrameRate1);
		SetOSDDetail(0);
		SetCodecAdvParam(0);
		SetCodecROIParam(0);
	}
	if((stream_flag & FLAG_STREAM2) > 0) {
		SetMP42bitrate(pConfig -> nBitrate2);
		SetEncRateControl(1, pConfig -> nRateControl2);
		SetFramerate2(pConfig -> nFrameRate2);
		SetOSDDetail(1);
		SetCodecAdvParam(1);
		SetCodecROIParam(1);
	}
	if((stream_flag & FLAG_STREAM3) > 0) {
		SetJpgQuality(pConfig -> nJpegQuality);
		SetFramerate3(pConfig -> nFrameRate3);
		SetOSDDetail(2);
		SetCodecAdvParam(2);
		SetCodecROIParam(2);
	}

	SetAudioAlarmValue(pConfig->audioalarmlevel);
    SetAudioAlarmFlag(pConfig->audioalarm);
	SetAudioInVolume(pConfig->audioinvolume);
	SetAudioOutVolume(pConfig->audiooutvolume);
	CheckAudioParam();
	/* Don't start wis-streamer here, we'll do it later. */

	if(pConfig->nDemoCfg == VA_DEMO)
	{
		/* DMVA specific set config */
		SetDmvaParams();
	}

	return 0;
}

/**
* @brief Set Binning
*
* @param value 0:binning ; 1:skipping
* @return function to set Binning
*/
int SetBinning(unsigned char value) // 0:binning / 1:skipping
{
	int ret = 0;
	unsigned char prev;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.nBinning!=value) {
		prev = pSysInfo->lan_config.nBinning;
		if((ret = fSetBinning(value)) == 0)
		{
			SetBinningSkip(value);
		}
	}

	return ret;
}
/**
* @brief Set day or night
*
* @param value value of day or night
* @return function to set day or night
*/
int SetCamDayNight(unsigned char value)
{
	int i, j;
	unsigned int frameRateVal1, frameRateVal2, frameRateVal3;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	if (pSysInfo->lan_config.nDayNight == value)
		return 0;

	i = pSysInfo->lan_config.nVideocodecmode;
	j = pSysInfo->lan_config.nVideocodecres;

	if(fSetCamDayNight(value)==0) {
		SetDayNight(value);
		frameRateVal1 = frame_rate_control(value, i, j, 1, 0);
		frameRateVal2 = frame_rate_control(value, i, j, 2, 0);
		frameRateVal3 = frame_rate_control(value, i, j, 3, 0);
		fSetFramerate1(0);fSetFramerate2(0);fSetFramerate3(0);
		fSetFramerateVal1(frameRateVal1);fSetFramerateVal2(frameRateVal2);fSetFramerateVal3(frameRateVal3);
		SetStreamFramerate(0, frameRateVal1*1000);
		SetStreamFramerate(1, frameRateVal2*1000);
		SetStreamFramerate(2, frameRateVal3*1000);
	}
	else
		return -1;

	return 0;
}

/**
* @brief Set MPEG4-1 frame rate
*
* @param "unsigned char value": selected frame rate
* @return function to set MPEG4-1 frame rate
* @retval 0: success
* @retval -1: failed to set MPEG4-1 frame rate
*/
int SetFramerate1(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->lan_config.nVideocodecmode;
	j = pSysInfo->lan_config.nVideocodecres;

	ret = fSetFramerate1(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 1, value);
		ret = fSetFramerateVal1(frameRateVal);
		if(!IsRestart())
			SetStreamFramerate(0, frameRateVal*1000);
	}

	return ret;
}

int SetFramerate2(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->lan_config.nVideocodecmode;
	j = pSysInfo->lan_config.nVideocodecres;

	ret = fSetFramerate2(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 2, value);
		ret = fSetFramerateVal2(frameRateVal);
		if(!IsRestart())
			SetStreamFramerate(1, frameRateVal*1000);
	}

	return ret;
}
int SetFramerate3(unsigned char value)
{
	int i = 0, j = 0, ret = 0;
	unsigned int frameRateVal;
	SysInfo* pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	i = pSysInfo->lan_config.nVideocodecmode;
	j = pSysInfo->lan_config.nVideocodecres;

	ret = fSetFramerate3(value);

	if(ret == 0) {
		frameRateVal = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 3, value);
		ret = fSetFramerateVal3(frameRateVal);
		if(!IsRestart())
			SetStreamFramerate(2, frameRateVal*1000);
	}

	return ret;
}

/**
* @brief Set white balance
*
* @param value value of white balance
* @return function to set white balance
*/
int SetCamWhiteBalance(unsigned char value)
{
	SetWhiteBalance(value);
	return fSetCamWhiteBalance(value);
}

/**
* @brief Set backlight value
*
* @param value backlight value
* @return function to set backlight value
*/
int SetCamBacklight(unsigned char value)
{
	SetBacklight(value);
	return fSetCamBacklight(value);
}

/**
* @brief Set brightness
*
* @param value brightness value
* @return function to set brightness value
*/
int SetCamBrightness(unsigned char value)
{
	SetBrightness(value);
	return fSetCamBrightness(value);
}

/**
* @brief Set contrast
*
* @param value contrast value
* @return function to set contrast value
*/
int SetCamContrast(unsigned char value)
{
	SetContrast(value);
	return fSetCamContrast(value);
}

/**
* @brief Set saturation
*
* @param value saturation value
* @return function to set saturation value
*/
int SetCamSaturation(unsigned char value)
{
	SetSaturation(value);
	return fSetCamSaturation(value);
}

/**
* @brief Set sharpness
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetCamSharpness(unsigned char value)
{
	SetSharpness(value);
	return fSetCamSharpness(value);
}

/**
* @brief Clear System Log
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetClearSysLog(unsigned char value)
{
	return fSetClearSysLog(value);
}

/**
* @brief Clear Access Log
*
* @param value sharpness value
* @return function to set sharpness value
*/
int SetClearAccessLog(unsigned char value)
{
	return fSetClearAccessLog(value);
}

/**
* @brief Set audio status
*
* @param value audio status.
*
* @return function to set audio status
*/

int audiocount=0, audiochange=0;
#define AUDIO_MAX_VARIABLE (2)

void CheckAudioParam(void)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	if(pSysInfo->audio_config.audioON) {
		if (pSysInfo->audio_config.audiomode==1) {
			fSetAudioEnable(0);
			fSetAudioAlarm(0);
		}
		else {
			fSetAudioEnable(1);
		}
		if(pSysInfo->audio_config.audiomode==0) {
			fSetAlarmAudioPlay(0);
		}
	}
	else {
		fSetAudioEnable(0);
		fSetAlarmAudioPlay(0);
		fSetAudioAlarm(0);
	}
}

void SetAudioParams(void)
{
	int audioMode, prevAudioMode;
	int nExtFunc = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	audiocount = 0;
	if(audiochange==0)
		return;
	else
		audiochange = 0;

	audioMode = pSysInfo->audio_config.audiomode;
	prevAudioMode = pSysInfo->audio_config.audioenable;

	CheckAudioParam();

	if(prevAudioMode != pSysInfo->audio_config.audioenable) {
		SetRestart();
	}
	else {
		SetRTSPStreamingOFF();
		nExtFunc = GetRTSPStreamOption();
		SetRTSPStreamingON(nExtFunc);
	}

	return;
}

int SetAudioON(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	int ret = 0;

	audiocount++;
	if(value != pSysInfo->audio_config.audioON) {
		ret = fSetAudioON(value);
		audiochange = 1;
	}

	if(audiocount == AUDIO_MAX_VARIABLE)
		SetAudioParams();

	return ret;
}

int SetAudioMode(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	int ret = 0;

	audiocount++;
	if(value != pSysInfo->audio_config.audiomode) {
		ret = fSetAudioMode(value);
		audiochange = 1;
	}

	if(audiocount == AUDIO_MAX_VARIABLE)
		SetAudioParams();

	return ret;
}

int SetAudioInVolume(unsigned char value)
{
	char cmd[64];
	int setval;

	if(Check_device_version() == 0) {
		/* AIC 26 control */
		setval = (value * 127) / 100;
		sprintf(cmd, "amixer cset numid=15 %d", setval);
		system(cmd);
	} else {
		/* AIC 3104 control */
		setval = (value * 119) / 100;
		sprintf(cmd, "amixer sset 'PGA' %d", setval);
		system(cmd);
	}
	return fSetAudioInVolume(value);
}

int SetAudioEncode(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->audio_config.codectype) {
		ret = fSetAudioEncode(value);
		if(ret == 0)
			SetRestart();
	}

	return ret;
}

int SetAudioSampleRate(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->audio_config.samplerate) {
		ret = fSetAudioSampleRate(value);
		if(ret == 0)
			SetRestart();
	}

	return ret;
}

int SetAudioBitrate(unsigned char value)
{
	int ret = 0;
	int bitrate;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->audio_config.codectype == 1) { //if AAC LC
		bitrate = audio_bitrate_aac[pSysInfo->audio_config.samplerate][value];
		SetAACBitrate(bitrate);
	}
	else {
		bitrate = audio_bitrate_g711[pSysInfo->audio_config.samplerate][value];
	}

	fSetAudioBitrateValue(bitrate);

	ret = fSetAudioBitrate(value);

	return ret;
}

int SetAudioAlarmLevel(unsigned char value)
{
	value = CheckValueBound(value, 1, 100);
 	SetAudioAlarmValue(value);
	return fSetAudioAlarmLevel(value);
}

int SetAudioOutVolume(unsigned char value)
{
	char cmd[64];
	int setval;

	if(Check_device_version() == 0) {
		/* AIC 26 control */
		setval = (value * 127) / 100;
		sprintf(cmd, "amixer cset numid=1 %d", setval);
		system(cmd);
	} else {
		/* AIC 3104 control */
		setval = (value * 127) / 100;
		sprintf(cmd, "amixer sset 'PCM' %d", setval);
		system(cmd);
	}

	return fSetAudioOutVolume(value);
}

int fdetectcount=0;
#define FD_MAX_VARIABLE (12)

void SetFaceDetectPrm(void)
{
	FaceDetectParam faceParam;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return;

	fdetectcount = 0;

	faceParam.fdetect 		= pSysInfo->face_config.fdetect;
	faceParam.startX 		= pSysInfo->face_config.startX;
	faceParam.startY 		= pSysInfo->face_config.startY;
	faceParam.width 		= pSysInfo->face_config.width;
	faceParam.height 		= pSysInfo->face_config.height;
	faceParam.fdconflevel 	= pSysInfo->face_config.fdconflevel;
	faceParam.fddirection 	= pSysInfo->face_config.fddirection;
	faceParam.frecog 		= pSysInfo->face_config.frecog;
	faceParam.frconflevel 	= pSysInfo->face_config.frconflevel;
	faceParam.frdatabase 	= pSysInfo->face_config.frdatabase;
	faceParam.pmask 		= pSysInfo->face_config.pmask;
	faceParam.maskoption 	= pSysInfo->face_config.maskoption;

	//printf("FD DEBUG VALUE: %d %d %d %d %d\n" , faceParam.fdetect,
	//	faceParam.startX, faceParam.startY, faceParam.width, faceParam.height);

	SendFaceDetectMsg(&faceParam);
}

int SetFaceDetect(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFaceDetect(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();

	return ret;
}

int SetFDX(unsigned int value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDX(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFDY(unsigned int value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDY(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFDW(unsigned int value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDW(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFDH(unsigned int value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDH(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFDConfLevel(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDConfLevel(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFDDirection(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFDDirection(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;

}

int SetFRecognition(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFRecognition(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFRConfLevel(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFRConfLevel(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetFRDatabase(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetFRDatabase(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}
int SetPrivacyMask(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetPrivacyMask(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetMaskOptions(unsigned char value)
{
	int ret = 0;
	fdetectcount++;
	ret = fSetMaskOptions(value);
	if(fdetectcount == FD_MAX_VARIABLE)
		SetFaceDetectPrm();
	return ret;
}

int SetMcfwCreatePrm(void)
{
	//int ret = 0;
	McfwCreatePrm mcfwCreatePrm;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    memset(&mcfwCreatePrm, 0, sizeof(McfwCreatePrm));

	mcfwCreatePrm.demoId = pSysInfo->lan_config.democfg;
	mcfwCreatePrm.usecase_id = pSysInfo->lan_config.nStreamType;
	mcfwCreatePrm.vendor2A = pSysInfo->lan_config.nAEWswitch;
	mcfwCreatePrm.mode2A = pSysInfo->lan_config.nAEWtype;
	mcfwCreatePrm.StreamPreset[0] = pSysInfo->codec_advconfig[0].meConfig;
	mcfwCreatePrm.StreamPreset[1] = pSysInfo->codec_advconfig[1].meConfig;
	if(pSysInfo->audio_config.audiomode!=1) {
		mcfwCreatePrm.audioCreatePrm.enable 	= pSysInfo->audio_config.audioON;
	}
	mcfwCreatePrm.audioCreatePrm.samplingRate 	= audio_samplerate[pSysInfo->audio_config.samplerate];
	mcfwCreatePrm.audioCreatePrm.codecType 		= pSysInfo->audio_config.codectype;
	if(pSysInfo->audio_config.codectype == 1) { //if AAC LC
		mcfwCreatePrm.audioCreatePrm.bitRate 	= audio_bitrate_aac[pSysInfo->audio_config.samplerate][pSysInfo->audio_config.bitrate];
	}
	else {
		mcfwCreatePrm.audioCreatePrm.bitRate 	= audio_bitrate_g711[pSysInfo->audio_config.samplerate][pSysInfo->audio_config.bitrate];
	}

	if (pSysInfo->advanceMode.vsEnable) {
		mcfwCreatePrm.advFtrCreatePrm.vsEnable = 1;
	}
	if (pSysInfo->advanceMode.ldcEnable) {
		mcfwCreatePrm.advFtrCreatePrm.ldcEnable = 1;
	}
	if (pSysInfo->advanceMode.vnfEnable) {
		mcfwCreatePrm.advFtrCreatePrm.snfEnable = 1;
		mcfwCreatePrm.advFtrCreatePrm.tnfEnable = 1;
	}

	SetMcfwCreatePrmMsg(&mcfwCreatePrm);

	return 0;
}


#define ADVANCED_ALL_VALUES	(5)
unsigned int advFeatCount=0;

int SetAdvMode(void)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount=0;

	if(pSysInfo->advanceMode.vnfMode>2)
	{
		SetLdcEnableMsg(pSysInfo->advanceMode.ldcEnable);
	}

	if(pSysInfo->lan_config.powerMode)
	{
		VnfParam vnfParam;

		vnfParam.enable 	= pSysInfo->advanceMode.vnfEnable;
		vnfParam.mode 		= pSysInfo->advanceMode.vnfMode;
		vnfParam.strength 	= pSysInfo->advanceMode.vnfStrength;

		SetVNFParamMsg(&vnfParam);
	}

	return ret;
}

int SetVstabValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->advanceMode.vsEnable){
		if ((ret = fSetVstabValue(value)) == 0) {
            SetRestart();
		}
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}
int SetLdcValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->advanceMode.ldcEnable){
		if ((ret = fSetLdcValue(value)) == 0) {
            SetRestart();
		}
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}
int SetVnfValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	if(value != pSysInfo->advanceMode.vnfEnable){
		ret = fSetVnfValue(value);
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

int SetVnfMode(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	int prev_mode 	= (pSysInfo->advanceMode.vnfMode>2)?1:0;
	int new_mode 	= (value>2)?1:0;

	advFeatCount ++;

	if(value != pSysInfo->advanceMode.vnfMode){
		ret = fSetVnfMode(value);
#ifdef MEMORY_256MB
		if(ret == 0)
		{
			/*
			 * In 256 MB memory scheme dynamic switching between NF is NOT supported.
			 * So there will be McFW restart.
			 */
			if(prev_mode != new_mode)
			{
				SetRestart();
			}
		}
#endif
	}

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}
int SetVnfStrength(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	advFeatCount ++;

	ret = fSetVnfStrength(value);

	if(advFeatCount==ADVANCED_ALL_VALUES)
		SetAdvMode();

	return ret;
}

#define DYNRANGE_ALL_VALUES	(2)
unsigned int dynRangeCount=0;

int SetDynRangeParam(void)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	dynRangeCount=0;

	DynRangePrm dynRangePrm;

	dynRangePrm.enable 		= (pSysInfo->advanceMode.dynRange>0)?1:0;
    dynRangePrm.mode        = pSysInfo->advanceMode.dynRange;
    dynRangePrm.level       = pSysInfo->advanceMode.dynRangeStrength;

	SetDynRangePrmMsg(&dynRangePrm);

	return 0;
}

int SetDynRange(unsigned char value)
{
	int ret=0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	dynRangeCount++;

	int prev_val = pSysInfo->advanceMode.dynRange;

	if(value != prev_val) {
		if ((ret = fSetDynRange(value)) == 0) {
			if((prev_val==0) || (value==0)) {
                SetRestart();
		    }
		}
	}

	if(dynRangeCount==DYNRANGE_ALL_VALUES)
		SetDynRangeParam();

	return ret;
}

int SetDynRangeStrength(unsigned char value)
{
	int ret=0;

	dynRangeCount++;

	ret = fSetDynRangeStrength(value);

	if(dynRangeCount==DYNRANGE_ALL_VALUES)
		SetDynRangeParam();

	return ret;
}

int SetFramectrlValue(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	if (value != pSysInfo->lan_config.framectrl)
		SetFramectrl(value);
	ret = fSetFrameCtrl(value);
	return ret;
}
void ResetBasicDefault(void)
{
	fSetVstabValue(VS_DEFAULT);fSetLdcValue(LDC_DEFAULT);
	fSetVnfValue(VNF_DEFAULT);fSetVnfMode(VNF_MODE_DEFAULT);fSetVnfStrength(VNF_STRENGTH_DEFAULT);
	fSetDynRange(DYNRANGE_DEF);fSetDynRangeStrength(DYNRANGE_STRENGTH);
	fSetMP41bitrate(MPEG41BITRATE_DEFAULT); fSetMP42bitrate(MPEG42BITRATE_DEFAULT);
	fSetVideoCodecMode(VCODECMODE_DEFAULT); fSetVideoCodecRes(VCODECRES_DEFAULT);
	fSetVideoCodecCombo(VCODECCOMBO_DEFAULT); fSetVideoMode(STREAMTYPE_DEFAULT);
}

int SetDemoCfg(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

#ifdef IPNC_LOW_POWER
	if(value >= VA_DEMO){
		value = 0;
	}
#endif

	if(value != pSysInfo->lan_config.democfg) {
		if ((ret = fSetDemoCfg(value)) == 0) {
			if((value == DEMO_NONE) || (value == VA_DEMO)) { /* reset to basic default modes */
				ResetBasicDefault();
			}
			SetRestart();
		}
	}

	return ret;
}

int SetClickSnapFilename(void * buf, int length)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

    if((ret = fSetClickSnapFilename(buf, length))==0) {
		if(pSysInfo->sdcard_config.sdinsert==3) {
    		SetClipSnapName((char*)buf, length);
		}
	}

	return ret;
}

int SetClickSnapStorage(unsigned char value)
{
	int ret =  fSetClickSnapStorage(value);

    if(ret==0)
    	SetClipSnapLocation(value);

	return ret;
}

int OSDChangeVal = 0;

void SendOSDChangeMsg(void)
{
	OSDChangeVal++;
	if(OSDChangeVal==OSD_PARAMS_UPDATED) {
		SetOSDWindow(OSD_MSG_TRUE);
		OSDChangeVal = 0;
	}

	return;
}

int SetHistogram(unsigned char value)
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.histogram) {
		ret = fSetHistogram(value);
		SetHistEnable(value);
	}

	return 0;
}


int SetExpPriority(unsigned char value)
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.expPriority) {
		ret = fSetExpPriority(value);
		Set2APriority(value);
	}

	return 0;
}
int SetIRIS(unsigned char value)
{
	SetDCIRIS(value);
	return 0;
}
/**
* @brief Set MPEG4-1 bitrate
*
* @param value MPEG4-1 bitrate value
* @return function to set MPEG4-1 bitrate
* @retval -1 failed to set MPEG4-1 bitrate
*/

int SetMP41bitrate(unsigned int value)
{
	SysInfo *pSysInfo = GetSysInfo();
	unsigned char nVideocodecmode, nVideocodecres;
	if(pSysInfo == NULL)
		return -1;

	nVideocodecmode= pSysInfo->lan_config.nVideocodecmode;
	nVideocodecres = pSysInfo->lan_config.nVideocodecres;

#if 0
	if((nVideocodecres == 3 && (nVideocodecmode == 6 || nVideocodecmode == 5))||(nVideocodecmode==2)){
		value = CheckValueBound(value, 64000, 8000000);
	}
	else {
		value = CheckValueBound(value, 64000, 12000000);
	}
#else
	value = CheckValueBound(value, 64000, 20000000);
#endif

	SetEncBitrate(0, value);
	return fSetMP41bitrate(value);
}

/**
* @brief Set MPEG4-2 bitrate
*
* @param value MPEG4-2 bitrate value
* @return function to set MPEG4-2 bitrate
* @retval -1 failed to set MPEG4-2 bitrate
*/
int SetMP42bitrate(unsigned int value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = CheckValueBound(value, 64000, 8000000);

	SetEncBitrate(1, value);
	return fSetMP42bitrate(value);
}

/**
* @brief Set motion JPEG quality
*
* @param value : QP value
* @return function to set motion JPEG quality
*/
int SetMJPEGQuality(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	value = (unsigned char)CheckValueBound((int)value, 2, 97);

	if(value != pSysInfo->lan_config.njpegquality) {
		SetJpgQuality(value);
		return fSetJpegQuality(value);
	}
	else
		return 0;
}

int SetMirror(unsigned char value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->lan_config.mirror) {
#if 0
       int ret = 0;
		if ((ret = fSetMirror(value)) == 0)
			SetRestart();
		return ret;
#else
		SetMirr(value);
		return fSetMirror(value);
#endif

	}
	return 0;

}

/**
* @brief Set image AEW
*
* @param "unsigned char value": 0: OFF, 1: APPRO
* @return SUCCESS: 0, FAIL: -1
*/
int SetImageAEW(unsigned char value)/*img2a*/
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

#ifdef _ONLY_TI2A
	if(value==1) {
		value = 2;
	}
#endif

#ifdef _ONLY_APPRO2A
	if(value==2) {
		value = 1;
	}
#endif

	if (pSysInfo->lan_config.nAEWswitch != value) {
		if ((ret = fSetImageAEW(value)) == 0) {
				SetImage2AVendor(value);
		}
	}

	return ret;
}

/**
* @brief Set image AEW
*
* @param "unsigned char value": 0: OFF, 1: APPRO
* @return SUCCESS: 0, FAIL: -1
*/
int SetImageAEWType(unsigned char value)/*img2a*/
{
	int ret = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

#ifdef WDR_ON_MODE /* Switch OFF AE */
	if(value&1) {
		value &= 2;
	}
#endif

	if (pSysInfo->lan_config.nAEWtype != value) {
		if ((ret = fSetImageAEWType(value)) == 0) {
			SetImage2AType(value);
		}
	}

	return ret;
}

/**
* @brief Set image source
*
* @param value value of image source( NTSC/PAL Select/)
* @return function to set image source
*/
/* NTSC/PAL Select */
int SetImagesource(unsigned char value)
{
	SetTvSystem(value);
	return fSetImageSource(value);
}

int SetRateControl1(unsigned char value)
{
    int ret =0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if (value != pSysInfo->lan_config.nRateControl1) {
		if ((ret = fSetRateControl1(value)) == 0) {
	        SetEncRateControl(0, value);
	        SetCodecAdvParam(0);
		}
	}

	return ret;
}
int SetRateControl2(unsigned char value)
{
    int ret =0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if (value != pSysInfo->lan_config.nRateControl2) {
		if ((ret = fSetRateControl2(value)) == 0) {
            SetEncRateControl(1, value);
	        SetCodecAdvParam(1);
		}
	}

	return ret;
}

/**
* @brief Set time stamp format
* @param value format
*/
int timedatecount=0;
#define TIMEDATE_MAX_VARIABLE (4)

int SetTimeDateDetail(void)
{
	DateTimePrm datetimeParam;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	timedatecount = 0;

	datetimeParam.dateFormat = pSysInfo->lan_config.dateformat;
	datetimeParam.datePos = pSysInfo->lan_config.dateposition;
	datetimeParam.timeFormat = pSysInfo->lan_config.tstampformat;
	datetimeParam.timePos = pSysInfo->lan_config.timeposition;

	return SetDateTimeFormat(&datetimeParam);
}

int SetDateFormat(unsigned char value)
{
	int ret = 0;
	timedatecount++;
	ret = fSetDateFormat(value);
	if(timedatecount == TIMEDATE_MAX_VARIABLE)
		SetTimeDateDetail();
	return ret;
}
int SetTStampFormat(unsigned char value)
{
	int ret = 0;
	timedatecount++;
	ret = fSetTStampFormat(value);
	if(timedatecount == TIMEDATE_MAX_VARIABLE)
		SetTimeDateDetail();
	return ret;
}
int SetDatePosition(unsigned char value)
{
	int ret = 0;
	timedatecount++;
	ret = fSetDatePosition(value);
	if(timedatecount == TIMEDATE_MAX_VARIABLE)
		SetTimeDateDetail();
	return ret;
}
int SetTimePosition(unsigned char value)
{
	int ret = 0;
	timedatecount++;
	ret = fSetTimePosition(value);
	if(timedatecount == TIMEDATE_MAX_VARIABLE)
		SetTimeDateDetail();
	return ret;
}

int osdcount[3]={0,0,0};

#define OSD_MAX_VARIABLE (8)

int SetOSDDetail(int id)
{
	OSDPrm osdPrm;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	osdcount[id] = 0;

	osdPrm.dateEnable = pSysInfo->osd_config[id].dstampenable;
	osdPrm.timeEnable = pSysInfo->osd_config[id].tstampenable;
	osdPrm.logoEnable = pSysInfo->osd_config[id].nLogoEnable;
	osdPrm.logoPos = pSysInfo->osd_config[id].nLogoPosition;
	osdPrm.textEnable = pSysInfo->osd_config[id].nTextEnable;
	osdPrm.textPos = pSysInfo->osd_config[id].nTextPosition;
	strcpy(osdPrm.text, (char *)pSysInfo->osd_config[id].overlaytext);
	osdPrm.detailedInfo = pSysInfo->osd_config[id].nDetailInfo;

	return SetOSDPrmMsg(id, &osdPrm);
}

int SetDateStampEnable1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetDateStampEnable1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetDateStampEnable2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetDateStampEnable2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetDateStampEnable3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetDateStampEnable3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetTimeStampEnable1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetTimeStampEnable1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetTimeStampEnable2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetTimeStampEnable2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetTimeStampEnable3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetTimeStampEnable3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetLogoEnable1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetLogoEnable1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetLogoEnable2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetLogoEnable2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetLogoEnable3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetLogoEnable3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}
int SetLogoPosition1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetLogoPosition1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetLogoPosition2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetLogoPosition2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetLogoPosition3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetLogoPosition3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetTextPosition1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetTextPosition1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetTextPosition2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetTextPosition2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetTextPosition3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetTextPosition3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetTextEnable1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetTextEnable1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetTextEnable2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetTextEnable2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetTextEnable3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetTextEnable3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetOverlayText1(void * buf, int length)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetOverlayText1(buf, length);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetOverlayText2(void * buf, int length)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetOverlayText2(buf, length);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetOverlayText3(void * buf, int length)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetOverlayText3(buf, length);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetDetailInfo1(unsigned char value)
{
	int ret = 0;
	osdcount[0]++;
	ret = fSetDetailInfo1(value);
	if(osdcount[0] == OSD_MAX_VARIABLE)
		SetOSDDetail(0);
	return ret;
}

int SetDetailInfo2(unsigned char value)
{
	int ret = 0;
	osdcount[1]++;
	ret = fSetDetailInfo2(value);
	if(osdcount[1] == OSD_MAX_VARIABLE)
		SetOSDDetail(1);
	return ret;
}

int SetDetailInfo3(unsigned char value)
{
	int ret = 0;
	osdcount[2]++;
	ret = fSetDetailInfo3(value);
	if(osdcount[2] == OSD_MAX_VARIABLE)
		SetOSDDetail(2);
	return ret;
}

int SetEncryption(unsigned char value)
{
    int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

#ifndef ENCRYPT_ON
	value = 0;
	printf("ENCRYPTION is not supported\n");
#endif

	if(value != pSysInfo->lan_config.encryptEnable) {
		if ((ret = fSetEncryption(value)) == 0)
			SetRestart();
	}

	return ret;
}

int SetLocalDisplay(unsigned char value)
{
      int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	SetDisplayValue(value);
    ret = fSetLocalDisplay(value);

	return ret;
}

int codeccount[3]={0,0,0};

#define CODEC_MAX_VARIABLE (7)

void SetCodecAdvParam(int id)
{
	CodecAdvPrm codecPrm;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return ;

	codeccount[id] = 0;

	codecPrm.ipRatio 		= pSysInfo->codec_advconfig[id].ipRatio;
	codecPrm.fIframe 		= pSysInfo->codec_advconfig[id].fIframe;
	codecPrm.qpInit 		= pSysInfo->codec_advconfig[id].qpInit;
	codecPrm.qpMin 			= pSysInfo->codec_advconfig[id].qpMin;
	codecPrm.qpMax 			= pSysInfo->codec_advconfig[id].qpMax;
	codecPrm.meConfig 		= pSysInfo->codec_advconfig[id].meConfig;
	codecPrm.packetSize 	= pSysInfo->codec_advconfig[id].packetSize;

	SetCodecAdvPrmMsg(id, &codecPrm);
}

int SetIpratio1(unsigned int value)
{
	int ret = 0, i = 0;

	value = CheckValueBound(value, 1, 60);

	codeccount[i]++;
    ret = fSetIpratio1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetIpratio2(unsigned int value)
{
	int ret = 0, i = 1;

	value = CheckValueBound(value, 1, 30);

	codeccount[i]++;
    ret = fSetIpratio2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetIpratio3(unsigned int value)
{
	int ret = 0, i = 2;

	value = CheckValueBound(value, 1, 30);

	codeccount[i]++;
	ret = fSetIpratio3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetForceIframe1(unsigned char value)
{
	int ret = 0, i = 0;

	codeccount[i]++;
	ret = fSetForceIframe1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetForceIframe2(unsigned char value)
{
	int ret = 0, i = 1;

	codeccount[i]++;
	ret = fSetForceIframe2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetForceIframe3(unsigned char value)
{
	int ret = 0, i = 2;

	codeccount[i]++;
	ret = fSetForceIframe3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPInit1(unsigned char value)
{
	int ret = 0, i = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
	ret = fSetQPInit1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPInit2(unsigned char value)
{
	int ret = 0, i = 1;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype2==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype2==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
	ret = fSetQPInit2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPInit3(unsigned char value)
{
	int ret = 0, i = 2;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype3==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype3==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
	ret = fSetQPInit3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMin1(unsigned char value)
{
	int ret = 0, i = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
	ret = fSetQPMin1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMin2(unsigned char value)
{
	int ret = 0, i = 1;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype2==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype2==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
    ret = fSetQPMin2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMin3(unsigned char value)
{
	int ret = 0, i = 2;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype3==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype3==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
	ret = fSetQPMin3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMax1(unsigned char value)
{
	int ret = 0, i = 0;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype1==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype1==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
    ret = fSetQPMax1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMax2(unsigned char value)
{
	int ret = 0, i = 1;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype2==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype2==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
    ret = fSetQPMax2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetQPMax3(unsigned char value)
{
	int ret = 0, i = 2;

	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(pSysInfo->lan_config.codectype3==H264_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 51);
	else if(pSysInfo->lan_config.codectype3==MPEG4_CODEC)
		value = (unsigned char)CheckValueBound((int)value, 4, 31);

	codeccount[i]++;
    ret = fSetQPMax3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetMEConfig1(unsigned char value)
{
	int ret = 0, i = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	codeccount[i]++;

	if(pSysInfo->lan_config.codectype1!=H264_CODEC)
		value = ENCPRESET_AUTO;

	if (value != pSysInfo->codec_advconfig[i].meConfig) {
		if ((ret = fSetMEConfig1(value)) == 0) {
			SetRestart();
		}
	}

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetMEConfig2(unsigned char value)
{
	int ret = 0, i = 1;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	codeccount[i]++;

	if(pSysInfo->lan_config.codectype2!=H264_CODEC)
		value = ENCPRESET_AUTO;

	if(value != ENCPRESET_SVC) {
		if (value != pSysInfo->codec_advconfig[i].meConfig) {
			if ((ret = fSetMEConfig2(value)) == 0) {
				SetRestart();
			}
		}
	}

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetMEConfig3(unsigned char value)
{
	int ret = 0, i = 2;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	codeccount[i]++;

	if(pSysInfo->lan_config.codectype3!=H264_CODEC)
		value = ENCPRESET_AUTO;

	if(value != ENCPRESET_SVC) {
		if (value != pSysInfo->codec_advconfig[i].meConfig) {
			if ((ret = fSetMEConfig3(value)) == 0) {
				SetRestart();
			}
		}
	}

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetPacketSize1(unsigned char value)
{
	int ret = 0, i = 0;

	codeccount[i]++;
    ret = fSetPacketSize1(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetPacketSize2(unsigned char value)
{
	int ret = 0, i = 1;

	codeccount[i]++;
    ret = fSetPacketSize2(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

int SetPacketSize3(unsigned char value)
{
	int ret = 0, i = 2;

	codeccount[i]++;
    ret = fSetPacketSize3(value);

	if(codeccount[i] == CODEC_MAX_VARIABLE)
		SetCodecAdvParam(i);

	return ret;
}

void SetCodecROIDemoParam(int width, int height)
{
	CodecROIPrm codecROIPrm;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return ;

	codecROIPrm.numROI = 1;

	codecROIPrm.roi[0].startx 	= width/4;
	codecROIPrm.roi[0].starty 	= height/4;
	codecROIPrm.roi[0].width	= width/2;
	codecROIPrm.roi[0].height 	= height/2;

	fSetROIEnable1(codecROIPrm.numROI);
	fSetStr1X1(codecROIPrm.roi[0].startx);
	fSetStr1Y1(codecROIPrm.roi[0].starty);
	fSetStr1W1(codecROIPrm.roi[0].width);
	fSetStr1H1(codecROIPrm.roi[0].height);

	fSetROIEnable2(0);
	fSetROIEnable3(0);
}

int roicount[MAX_GUI_ROI]={0,0,0};

#define ROI_MAX_VARIABLE (13)

void SetCodecROIParam(int id)
{
	int i=0;
	CodecROIPrm codecROIPrm;
    SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return ;

	roicount[id] = 0;

	codecROIPrm.numROI 			= pSysInfo->codec_roiconfig[id].numROI;

	for(i=0;i<MAX_GUI_ROI;i++) {
		codecROIPrm.roi[i].startx 	= pSysInfo->codec_roiconfig[id].roi[i].startx;
		codecROIPrm.roi[i].starty 	= pSysInfo->codec_roiconfig[id].roi[i].starty;
		codecROIPrm.roi[i].width	= pSysInfo->codec_roiconfig[id].roi[i].width;
		codecROIPrm.roi[i].height 	= pSysInfo->codec_roiconfig[id].roi[i].height;
		/* IH264_FACE_OBJECT 0 , IH264_FOREGROUND_OBJECT 2*/
		codecROIPrm.roi[i].type 	= ROI_FOREGROUND_TYPE;
	}

	SetCodecROIMsg(id, &codecROIPrm);
}

int SetROIEnable1(unsigned char value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetROIEnable1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetROIEnable2(unsigned char value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetROIEnable2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetROIEnable3(unsigned char value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetROIEnable3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1X1(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1X1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1Y1(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1Y1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1W1(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1W1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1H1(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1H1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1X2(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1X2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1Y2(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1Y2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1W2(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1W2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1H2(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1H2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1X3(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1X3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1Y3(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1Y3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1W3(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1W3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr1H3(unsigned int value)
{
	int ret = 0, i = 0;

	roicount[i]++;
    ret = fSetStr1H3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2X1(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2X1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2Y1(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2Y1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2W1(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2W1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2H1(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2H1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2X2(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2X2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2Y2(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2Y2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2W2(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2W2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2H2(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2H2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2X3(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2X3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2Y3(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2Y3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2W3(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2W3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr2H3(unsigned int value)
{
	int ret = 0, i = 1;

	roicount[i]++;
    ret = fSetStr2H3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3X1(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3X1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3Y1(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3Y1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3W1(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3W1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3H1(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3H1(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3X2(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3X2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3Y2(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3Y2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3W2(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3W2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3H2(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3H2(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3X3(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3X3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3Y3(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3Y3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3W3(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3W3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetStr3H3(unsigned int value)
{
	int ret = 0, i = 2;

	roicount[i]++;
    ret = fSetStr3H3(value);

	if(roicount[i] == ROI_MAX_VARIABLE)
		SetCodecROIParam(i);

	return ret;
}

int SetAudioReceiverEnable(unsigned char value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(value != pSysInfo->audio_config.audiorecvenable)
	{
    	ret = fSetAudioReceiverEnable(value);
 		system("killall -9 audio-receiver\n");
 		sleep(2);
		RTSP_AudioRecvON();
	}

	return ret;
}

int SetAudioSeverIp(void * buf, int length)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;

	if(strncmp((char *)buf, (char *)pSysInfo->audio_config.audioServerIp, length) != 0)
	{
	    ret = fSetAudioSeverIp(buf, length);

	    if(pSysInfo->audio_config.audiorecvenable)
	    {
 	  	    system("killall -9 audio-receiver\n");
 		    sleep(2);
		    RTSP_AudioRecvON();
	    }
	}

	return ret;
}

/* DMVA Variables */
/* DMVA TZ Variables */
int dmvaTZParamCount     = 0;
int dmvaTZDiffParamCount = 0;
#define DMVA_TZ_MAX_VARIABLE 				(77)

/* Set DMVA Main Page specific Parameters */
int dmvaMainParamCount     = 0;
int dmvaMainDiffParamCount = 0;
#define DMVA_MAIN_MAX_VARIABLE 				(1)

int dmvaMainSearchParamCount     = 0;
int dmvaMainSearchDiffParamCount = 0;
#define DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE 	(6)

/* Set DMVA Advanced Settings Page specific Parameters */
int dmvaAdvancedParamCount     = 0;
int dmvaAdvancedDiffParamCount = 0;
#define DMVA_ADVANCED_MAX_VARIABLE 			(14)

/* Set DMVA CTD Page specific Parameters */
int dmvaCTDParamCount     = 0;
int dmvaCTDDiffParamCount = 0;
#define DMVA_CTD_MAX_VARIABLE 				(4)

/* Set DMVA OC Page specific Parameters */
int dmvaOCParamCount     = 0;
int dmvaOCDiffParamCount = 0;
#define DMVA_OC_MAX_VARIABLE 				(6)

/* Set DMVA SMETA Page specific Parameters */
int dmvaSMETAParamCount     = 0;
int dmvaSMETADiffParamCount = 0;
#define DMVA_SMETA_MAX_VARIABLE 			(44)

/* Set DMVA IMD Page specific Parameters */
int dmvaIMDParamCount     = 0;
int dmvaIMDDiffParamCount = 0;
#define DMVA_IMD_MAX_VARIABLE 				(144)

void SetDmvaPrm(int ret)
{
	DmvaParam dmvaParam;
    SysInfo *pSysInfo = GetSysInfo();

    if(ret != -2)
    {
		dmvaParam.dmvaenable = pSysInfo->dmva_config.dmvaenable_sys;
		SendDmvaEnableMsg(&dmvaParam);
    }
}

void SetExptAlgoFrameRatePrm(int ret)
{
	DmvaParam dmvaParam;
    SysInfo *pSysInfo = GetSysInfo();

    if(ret != -2)
    {
		dmvaParam.dmvaexptalgoframerate = pSysInfo->dmva_config.dmvaexptalgoframerate_sys;
		SendDmvaExptAlgoFrameRateMsg(&dmvaParam);
    }
}

void SetExptAlgoDetectFreqPrm(int ret)
{
	DmvaParam dmvaParam;
    SysInfo *pSysInfo = GetSysInfo();

    if(ret != -2)
    {
		dmvaParam.dmvaexptalgodetectfreq = pSysInfo->dmva_config.dmvaexptalgodetectfreq_sys;
		SendDmvaExptAlgoDetectFreqMsg(&dmvaParam);
    }
}

void SetExptEvtRecordEnablePrm(int ret)
{
	DmvaParam dmvaParam;
    SysInfo *pSysInfo = GetSysInfo();

    if(ret != -2)
    {
		dmvaParam.dmvaexptevtrecordenable = pSysInfo->dmva_config.dmvaexptevtrecordenable_sys;
		SendDmvaExptEvtRecordEnableMsg(&dmvaParam);
    }
}

void SetExptSmetaTrackerEnablePrm(int ret)
{
	DmvaParam dmvaParam;
    SysInfo *pSysInfo = GetSysInfo();

    if(ret != -2)
    {
		dmvaParam.dmvaexptsmetatrackerenable = pSysInfo->dmva_config.dmvaexptSmetaTrackerEnable_sys;
		SendDmvaExptSmetaTrackerEnableMsg(&dmvaParam);
    }
}

int SetDmvaEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEnable(value);
	if(ret == -2) ret = 0;
	SetDmvaPrm(ret);

	return ret;
}
int SetDmvaExptAlgoFrameRate(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaExptAlgoFrameRate(value);
	if(ret == -2) ret = 0;
	SetExptAlgoFrameRatePrm(ret);

	return ret;
}
int SetDmvaExptAlgoDetectFreq(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaExptAlgoDetectFreq(value);
	if(ret == -2) ret = 0;
	SetExptAlgoDetectFreqPrm(ret);

	return ret;
}
int SetDmvaExptEvtRecordEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaExptEvtRecordEnable(value);
	if(ret == -2) ret = 0;
	SetExptEvtRecordEnablePrm(ret);

	return ret;
}

int SetDmvaExptSmetaTrackerEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaExptSmetaTrackerEnable(value);
	if(ret == -2) ret = 0;
	SetExptSmetaTrackerEnablePrm(ret);

	return ret;
}


void SetDmvaTZPrm(void)
{
	int i;

	DmvaTZParam dmvaTZParam;
    SysInfo *pSysInfo;

    if(dmvaTZDiffParamCount == 0)
    {
        dmvaTZParamCount = 0;
	}
	else
	{
		dmvaTZParamCount     = 0;
		dmvaTZDiffParamCount = 0;

        pSysInfo = GetSysInfo();

		/* DMVA TZ parameters */
		dmvaTZParam.dmvaTZSensitivity      = pSysInfo->dmva_config.dmvaAppTZSensitivity_sys;
		dmvaTZParam.dmvaTZPersonMinWidth   = pSysInfo->dmva_config.dmvaAppTZPersonMinWidth_sys;
		dmvaTZParam.dmvaTZPersonMinHeight  = pSysInfo->dmva_config.dmvaAppTZPersonMinHeight_sys;
		dmvaTZParam.dmvaTZVehicleMinWidth  = pSysInfo->dmva_config.dmvaAppTZVehicleMinWidth_sys;
		dmvaTZParam.dmvaTZVehicleMinHeight = pSysInfo->dmva_config.dmvaAppTZVehicleMinHeight_sys;
		dmvaTZParam.dmvaTZPersonMaxWidth   = pSysInfo->dmva_config.dmvaAppTZPersonMaxWidth_sys;
		dmvaTZParam.dmvaTZPersonMaxHeight  = pSysInfo->dmva_config.dmvaAppTZPersonMaxHeight_sys;
		dmvaTZParam.dmvaTZVehicleMaxWidth  = pSysInfo->dmva_config.dmvaAppTZVehicleMaxWidth_sys;
		dmvaTZParam.dmvaTZVehicleMaxHeight = pSysInfo->dmva_config.dmvaAppTZVehicleMaxHeight_sys;
		dmvaTZParam.dmvaTZDirection        = pSysInfo->dmva_config.dmvaAppTZDirection_sys;
		dmvaTZParam.dmvaTZPresentAdjust    = pSysInfo->dmva_config.dmvaAppTZPresentAdjust_sys;
		dmvaTZParam.dmvaTZZone1_ROI_numSides           = pSysInfo->dmva_config.dmvaAppTZZone1_ROI_numSides_sys;
		dmvaTZParam.dmvaTZZone2_ROI_numSides           = pSysInfo->dmva_config.dmvaAppTZZone2_ROI_numSides_sys;

		/* dmvaTZEnable is not transmitted as GUI keyword. So this is just a dummy copying */
		dmvaTZParam.dmvaTZEnable           = pSysInfo->dmva_config.dmvaAppTZEnable_sys;

		for(i=0;i<16;i++) {
			dmvaTZParam.dmvaTZZone1_x[i]           = pSysInfo->dmva_config.dmvaAppTZZone1_x[i+1];
			dmvaTZParam.dmvaTZZone1_y[i]           = pSysInfo->dmva_config.dmvaAppTZZone1_y[i+1];
			dmvaTZParam.dmvaTZZone2_x[i]           = pSysInfo->dmva_config.dmvaAppTZZone2_x[i+1];
			dmvaTZParam.dmvaTZZone2_y[i]           = pSysInfo->dmva_config.dmvaAppTZZone2_y[i+1];
		}

		SendDmvaTZSetupMsg(&dmvaTZParam);
    }
}

int SetDmvaTZSensitivity(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZSensitivity(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

int SetDmvaTZPersonMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZPersonMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZPersonMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZPersonMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZVehicleMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZVehicleMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZVehicleMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZVehicleMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZPersonMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZPersonMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZPersonMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZPersonMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZVehicleMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZVehicleMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZVehicleMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZVehicleMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZDirection(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZDirection(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}
int SetDmvaTZPresentAdjust(unsigned int value)
{
	int ret = 0;
	/* Note: This function need to be updated to connect the parameters appropriately */
	ret = fSetDmvaTZPresentAdjust(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

int SetDmvaTZEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZEnable(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

int SetDmvaTZZone1Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

int SetDmvaTZZone2Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

int SetDmvaTZZone1_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x01(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x02(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x03(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x04(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x05(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x06(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x07(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x08(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x09(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x10(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x11(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x12(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x13(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x14(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x15(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_x16(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}



int SetDmvaTZZone1_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y01(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y02(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y03(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y04(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y05(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y06(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y07(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y08(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y09(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y10(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y11(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y12(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y13(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y14(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y15(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone1_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone1_y16(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x01(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x02(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x03(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x04(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x05(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x06(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x07(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x08(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x09(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x10(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x11(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x12(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x13(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x14(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x15(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_x16(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}



int SetDmvaTZZone2_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y01(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y02(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y03(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y04(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y05(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y06(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y07(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y08(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y09(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y10(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y11(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y12(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y13(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y14(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y15(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}


int SetDmvaTZZone2_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaTZZone2_y16(value);
	if(ret == -2) ret = 0;
	else dmvaTZDiffParamCount ++;
	dmvaTZParamCount++;
	if(dmvaTZParamCount == DMVA_TZ_MAX_VARIABLE)
		SetDmvaTZPrm();

	return ret;
}

/* DMVA TZ save/load params */
int SetDmvaTZSave(void * buf, int length)
{
    return fSetDmvaTZSave(buf,length);
}

int SetDmvaTZLoad(unsigned int value)
{
    int ret = 0;
    ret = fSetDmvaTZLoad(value);

    if(ret == 0)
    {
        // Send the loaded TZ params to AV server
        dmvaTZDiffParamCount = 1;
        SetDmvaTZPrm();
	}

    return ret;
}

/* Set DMVA Main Page specific Parameters */

void SetDmvaMainListSearchPrm(void)
{
	DmvaMainParam dmvaMainParam;
    SysInfo *pSysInfo;

    if(dmvaMainSearchDiffParamCount == 0)
    {
        dmvaMainSearchParamCount = 0;
	}
    else
    {
		dmvaMainSearchParamCount     = 0;
        dmvaMainSearchDiffParamCount = 0;

        pSysInfo = GetSysInfo();

		/* DMVA MAIN PAGE parameters */
		dmvaMainParam.dmvaEventListStartMonth             = pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys;
		dmvaMainParam.dmvaEventListStartDay               = pSysInfo->dmva_config.dmvaAppEventListStartDay_sys;
		dmvaMainParam.dmvaEventListStartTime              = pSysInfo->dmva_config.dmvaAppEventListStartTime_sys;
		dmvaMainParam.dmvaEventListEndMonth               = pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys;
		dmvaMainParam.dmvaEventListEndDay                 = pSysInfo->dmva_config.dmvaAppEventListEndDay_sys;
		dmvaMainParam.dmvaEventListEndTime                = pSysInfo->dmva_config.dmvaAppEventListEndTime_sys;

		SendDmvaMainSetupMsg(&dmvaMainParam);
    }
}
void SetDmvaMainPrm(void)
{
	DmvaMainParam dmvaMainParam;
    SysInfo *pSysInfo;

    pSysInfo = GetSysInfo();

	if(pSysInfo->dmva_config.dmvaAppCfgCTDImdSmetaTzOc_sys <= 0x1F)
	{
		dmvaMainParam.dmvaCfgCTDImdSmetaTzOc = pSysInfo->dmva_config.dmvaAppCfgCTDImdSmetaTzOc_sys;
		dmvaMainParam.dmvaDisplayOptions     = pSysInfo->dmva_config.dmvaAppDisplayOptions_sys;
		dmvaMainParam.dmvaEventRecordingVAME = pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys;

		SendDmvaMainSetupMsg(&dmvaMainParam);
	}
}
int SetDmvaCamID(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCamID(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}
int SetDmvaGetSchedule(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaGetSchedule(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}
int SetDmvaCfgFD(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgFD(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}
int SetDmvaCfgCTDImdSmetaTzOc(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgCTDImdSmetaTzOc(value);
	if(ret == -2) ret = 0;
	else SetDmvaMainPrm();
	return ret;
}
int SetDmvaDisplayOptions(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaDisplayOptions(value);
	if(ret == -2) ret = 0;
	else SetDmvaMainPrm();
	return ret;
}
int SetDmvaEventRecordingVAME(unsigned int value)
{
	int ret = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if((pSysInfo->audio_config.audioenable == 0) && (value & 0x4))
	{
		value = (value & 0xB);
	}
	ret = fSetDmvaEventRecordingVAME(value);
	if(ret == -2) ret = 0;
	else SetDmvaMainPrm();
	return ret;
}
int SetDmvaEventListActionPlaySendSearchTrash(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListActionPlaySendSearchTrash(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();
#endif

	return ret;
}
int SetDmvaEventListSelectEvent(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListSelectEvent(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}
int SetDmvaEventListArchiveFlag(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListArchiveFlag(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}
int SetDmvaEventListEventType(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListEventType(value);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();

	return ret;
}


int SetDmvaEventListStartMonth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListStartMonth(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListStartDay(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListStartDay(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListStartTime(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListStartTime(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListEndMonth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListEndMonth(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListEndDay(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListEndDay(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListEndTime(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventListEndTime(value);
	if(ret == -2) ret = 0;

#if 0
	else dmvaMainSearchDiffParamCount ++;
	dmvaMainSearchParamCount++;
	if(dmvaMainSearchParamCount == DMVA_MAIN_LIST_SEARCH_MAX_VARIABLE)
		SetDmvaMainListSearchPrm();
#endif

	return ret;
}

int SetDmvaEventListTimeStamp(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaEventListTimeStamp(buf, length);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();
	return ret;
}

int SetDmvaEventListEventDetails(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaEventListEventDetails(buf, length);
	if(ret == -2) ret = 0;
	else dmvaMainDiffParamCount ++;
	dmvaMainParamCount++;
	if(dmvaMainParamCount == DMVA_MAIN_MAX_VARIABLE)
		SetDmvaMainPrm();
	return ret;
}

/* Set DMVA Advanced Settings Page specific Parameters */

void SetDmvaAdvancedPrm(void)
{
	//DmvaAdvancedParam dmvaAdvancedParam;
    SysInfo *pSysInfo;

	/* TO BE IMPLEMENTED TO SEND PARAMETERS TO AVSERVER */
    if(dmvaAdvancedDiffParamCount == 0)
    {
	    dmvaAdvancedParamCount = 0;
	}
	else
	{
	    dmvaAdvancedParamCount     = 0;
	    dmvaAdvancedDiffParamCount = 0;

	    pSysInfo = GetSysInfo();
    }
}
int SetDmvaObjectMetaData(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaObjectMetaData(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesFontType(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesFontType(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesFontSize(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesFontSize(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesFontColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesFontColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesBoxColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesBoxColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesCentroidColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesCentroidColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesActiveZoneColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesActiveZoneColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesInactiveZoneColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesInactiveZoneColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaOverlayPropertiesVelocityColor(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOverlayPropertiesVelocityColor(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}

int SetDmvaEnvironmentProfileLoad(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEnvironmentProfileLoad(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}
int SetDmvaEnvironmentProfileStore(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEnvironmentProfileStore(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}
int SetDmvaEnvironmentProfileClear(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEnvironmentProfileClear(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}
int SetDmvaEnvironmentProfileState(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEnvironmentProfileState(value);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}

	return ret;
}
int SetDmvaEnvironmentProfileDesc(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaEnvironmentProfileDesc(buf, length);
	if(ret == -2) ret = 0;
	else dmvaAdvancedDiffParamCount ++;
	dmvaAdvancedParamCount++;
	if(dmvaAdvancedParamCount == DMVA_ADVANCED_MAX_VARIABLE){
		SetDmvaAdvancedPrm();
	}
	return ret;
}

/* Set DMVA event delete specific functions */
int SetDmvaEventDeleteIndex(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventDeleteIndex(value);
	return ret;
}

int SetDmvaEventDeleteStartIndex(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventDeleteStartIndex(value);
	return ret;
}

int SetDmvaEventDeleteEndIndex(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventDeleteEndIndex(value);
	return ret;
}

int SetDmvaEventDeleteAll(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaEventDeleteAll(value);
	return ret;
}

/* Set DMVA CTD Page specific Parameters */
void SetDmvaCTDPrm(void)
{
	DmvaCTDParam dmvaCTDParam;
    SysInfo *pSysInfo;

    if(dmvaCTDDiffParamCount == 0)
    {
	    dmvaCTDParamCount = 0;
	}
	else
	{
	    dmvaCTDParamCount     = 0;
	    dmvaCTDDiffParamCount = 0;

	    pSysInfo = GetSysInfo();

	    /* DMVA CTD PAGE parameters */
	    dmvaCTDParam.dmvaCfgTDEnv1        = pSysInfo->dmva_config.dmvaAppCfgTDEnv1_sys;
	    dmvaCTDParam.dmvaCfgTDEnv2        = pSysInfo->dmva_config.dmvaAppCfgTDEnv2_sys;
	    dmvaCTDParam.dmvaCfgTDSensitivity = pSysInfo->dmva_config.dmvaAppCfgTDSensitivity_sys;
	    dmvaCTDParam.dmvaCfgTDResetTime   = pSysInfo->dmva_config.dmvaAppCfgTDResetTime_sys;

	    SendDmvaCTDSetupMsg(&dmvaCTDParam);
    }
}
int SetDmvaCfgTDEnv1(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgTDEnv1(value);
	if(ret == -2) ret = 0;
	else dmvaCTDDiffParamCount ++;
	dmvaCTDParamCount++;
	if(dmvaCTDParamCount == DMVA_CTD_MAX_VARIABLE)
		SetDmvaCTDPrm();

	return ret;
}
int SetDmvaCfgTDEnv2(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgTDEnv2(value);
	if(ret == -2) ret = 0;
	else dmvaCTDDiffParamCount ++;
	dmvaCTDParamCount++;
	if(dmvaCTDParamCount == DMVA_CTD_MAX_VARIABLE)
		SetDmvaCTDPrm();

	return ret;
}
int SetDmvaCfgTDSensitivity(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgTDSensitivity(value);
	if(ret == -2) ret = 0;
	else dmvaCTDDiffParamCount ++;
	dmvaCTDParamCount++;
	if(dmvaCTDParamCount == DMVA_CTD_MAX_VARIABLE)
		SetDmvaCTDPrm();

	return ret;
}
int SetDmvaCfgTDResetTime(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaCfgTDResetTime(value);
	if(ret == -2) ret = 0;
	else dmvaCTDDiffParamCount ++;
	dmvaCTDParamCount++;
	if(dmvaCTDParamCount == DMVA_CTD_MAX_VARIABLE)
		SetDmvaCTDPrm();

	return ret;
}

/* Set DMVA OC Page specific Parameters */
void SetDmvaOCPrm(void)
{
	DmvaOCParam dmvaOCParam;
    SysInfo *pSysInfo;

    if(dmvaOCDiffParamCount == 0)
    {
	    dmvaOCParamCount = 0;
	}
	else
	{
		dmvaOCParamCount     = 0;
		dmvaOCDiffParamCount = 0;

		pSysInfo = GetSysInfo();

		/* DMVA OC PAGE parameters */
		dmvaOCParam.dmvaOCSensitivity    = pSysInfo->dmva_config.dmvaAppOCSensitivity_sys;
		dmvaOCParam.dmvaOCObjectWidth    = pSysInfo->dmva_config.dmvaAppOCObjectWidth_sys;
		dmvaOCParam.dmvaOCObjectHeight   = pSysInfo->dmva_config.dmvaAppOCObjectHeight_sys;
		dmvaOCParam.dmvaOCDirection      = pSysInfo->dmva_config.dmvaAppOCDirection_sys;
		dmvaOCParam.dmvaOCEnable         = pSysInfo->dmva_config.dmvaAppOCEnable_sys;
		dmvaOCParam.dmvaOCLeftRight      = pSysInfo->dmva_config.dmvaAppOCLeftRight_sys;
		dmvaOCParam.dmvaOCTopBottom      = pSysInfo->dmva_config.dmvaAppOCTopBottom_sys;

		SendDmvaOCSetupMsg(&dmvaOCParam);
    }
}
int SetDmvaOCSensitivity(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCSensitivity(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCObjectWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCObjectWidth(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCObjectHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCObjectHeight(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCDirection(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCDirection(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCEnable(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCLeftRight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCLeftRight(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}
int SetDmvaOCTopBottom(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaOCTopBottom(value);
	if(ret == -2) ret = 0;
	else dmvaOCDiffParamCount ++;
	dmvaOCParamCount++;
	if(dmvaOCParamCount == DMVA_OC_MAX_VARIABLE)
		SetDmvaOCPrm();

	return ret;
}

/* DMVA OC save/load params */
int SetDmvaOCSave(void * buf, int length)
{
    return fSetDmvaOCSave(buf,length);
}

int SetDmvaOCLoad(unsigned int value)
{
    int ret = 0;
    ret = fSetDmvaOCLoad(value);

    if(ret == 0)
    {
        // Send the loaded OC params to AV server
        dmvaOCDiffParamCount = 1;
        SetDmvaOCPrm();
	}

    return ret;
}


/* Set DMVA SMETA Page specific Parameters */

void SetDmvaSMETAPrm(void)
{
	int i;

	DmvaSMETAParam dmvaSMETAParam;
    SysInfo *pSysInfo;

	//printf("\n Current Count is %d\n",dmvaSMETAParamCount);
    if(dmvaSMETADiffParamCount == 0)
    {
	    dmvaSMETAParamCount = 0;
		//printf("\n Making Count to %d\n",dmvaSMETAParamCount);
	}
	else
    {
		dmvaSMETAParamCount     = 0;
		dmvaSMETADiffParamCount = 0;

		pSysInfo = GetSysInfo();

		/* DMVA SMETA PAGE parameters */
		dmvaSMETAParam.dmvaSMETASensitivity          = pSysInfo->dmva_config.dmvaAppSMETASensitivity_sys;
		dmvaSMETAParam.dmvaSMETAEnableStreamingData  = pSysInfo->dmva_config.dmvaAppSMETAEnableStreamingData_sys;
		dmvaSMETAParam.dmvaSMETAStreamBB             = pSysInfo->dmva_config.dmvaAppSMETAStreamBB_sys;
		dmvaSMETAParam.dmvaSMETAStreamVelocity       = pSysInfo->dmva_config.dmvaAppSMETAStreamVelocity_sys;
		dmvaSMETAParam.dmvaSMETAStreamHistogram      = pSysInfo->dmva_config.dmvaAppSMETAStreamHistogram_sys;
		dmvaSMETAParam.dmvaSMETAStreamMoments        = pSysInfo->dmva_config.dmvaAppSMETAStreamMoments_sys;

		dmvaSMETAParam.dmvaSMETAPresentAdjust    = pSysInfo->dmva_config.dmvaAppSMETAPresentAdjust_sys;
		dmvaSMETAParam.dmvaSMETAPersonMinWidth   = pSysInfo->dmva_config.dmvaAppSMETAPersonMinWidth_sys;
		dmvaSMETAParam.dmvaSMETAPersonMinHeight  = pSysInfo->dmva_config.dmvaAppSMETAPersonMinHeight_sys;
		dmvaSMETAParam.dmvaSMETAVehicleMinWidth  = pSysInfo->dmva_config.dmvaAppSMETAVehicleMinWidth_sys;
		dmvaSMETAParam.dmvaSMETAVehicleMinHeight = pSysInfo->dmva_config.dmvaAppSMETAVehicleMinHeight_sys;
		dmvaSMETAParam.dmvaSMETAPersonMaxWidth   = pSysInfo->dmva_config.dmvaAppSMETAPersonMaxWidth_sys;
		dmvaSMETAParam.dmvaSMETAPersonMaxHeight  = pSysInfo->dmva_config.dmvaAppSMETAPersonMaxHeight_sys;
		dmvaSMETAParam.dmvaSMETAVehicleMaxWidth  = pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxWidth_sys;
		dmvaSMETAParam.dmvaSMETAVehicleMaxHeight = pSysInfo->dmva_config.dmvaAppSMETAVehicleMaxHeight_sys;
		dmvaSMETAParam.dmvaSMETANumZones         = pSysInfo->dmva_config.dmvaAppSMETANumZones_sys;

		dmvaSMETAParam.dmvaSMETAZone1_ROI_numSides           = pSysInfo->dmva_config.dmvaAppSMETAZone1_ROI_numSides_sys;

		for(i=0;i<16;i++) {
			dmvaSMETAParam.dmvaSMETAZone1_x[i]           = pSysInfo->dmva_config.dmvaAppSMETAZone1_x[i+1];
			dmvaSMETAParam.dmvaSMETAZone1_y[i]           = pSysInfo->dmva_config.dmvaAppSMETAZone1_y[i+1];
		}

		SendDmvaSMETASetupMsg(&dmvaSMETAParam);
    }
}
/* This variable not used */
int SetDmvaSMETAStreamBB(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAStreamBB(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
/* This variable not used */
int SetDmvaSMETAStreamVelocity(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAStreamVelocity(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

/* This variable not used */
int SetDmvaSMETAStreamHistogram(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAStreamHistogram(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

/* This variable not used */
int SetDmvaSMETAStreamMoments(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAStreamMoments(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETANumZones(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETANumZones(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETASensitivity(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETASensitivity(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAEnableStreamingData(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAEnableStreamingData(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAPresentAdjust(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAPresentAdjust(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAPersonMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAPersonMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAPersonMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAPersonMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAVehicleMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAVehicleMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAVehicleMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAVehicleMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAPersonMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAPersonMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAPersonMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAPersonMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAVehicleMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAVehicleMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}
int SetDmvaSMETAVehicleMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAVehicleMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x01(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x02(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x03(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x04(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x05(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x06(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x07(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x08(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x09(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x10(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x11(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x12(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x13(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x14(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x15(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_x16(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}


int SetDmvaSMETAZone1_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y01(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y02(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y03(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y04(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y05(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y06(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y07(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y08(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y09(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y10(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y11(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y12(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y13(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y14(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y15(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_y16(value);
	if(ret == -2) ret = 0;
	else dmvaSMETADiffParamCount ++;
	dmvaSMETAParamCount++;
	if(dmvaSMETAParamCount == DMVA_SMETA_MAX_VARIABLE)
		SetDmvaSMETAPrm();

	return ret;
}

int SetDmvaSMETAZone1_Label(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaSMETAZone1_Label(buf, length);
	if(ret == -2) ret = 0;
	//else dmvaIMDDiffParamCount ++;
	return ret;
}

/* DMVA SMETA save/load params */
int SetDmvaSMETASave(void * buf, int length)
{
    return fSetDmvaSMETASave(buf,length);
}

int SetDmvaSMETALoad(unsigned int value)
{
    int ret = 0;
    ret = fSetDmvaSMETALoad(value);

    if(ret == 0)
    {
        // Send the loaded SM params to AV server
        dmvaSMETADiffParamCount = 1;
        SetDmvaSMETAPrm();
	}

    return ret;
}


/* Set DMVA IMD Page specific Parameters */

void SetDmvaIMDPrm(void)
{
	DmvaIMDParam dmvaIMDParam;
	int i;
    SysInfo *pSysInfo;

    if(dmvaIMDDiffParamCount == 0)
    {
        dmvaIMDParamCount = 0;
	}
	else
	{
		dmvaIMDParamCount     = 0;
		dmvaIMDDiffParamCount = 0;

		pSysInfo = GetSysInfo();

		/* DMVA IMD PAGE parameters */
		dmvaIMDParam.dmvaIMDSensitivity      = pSysInfo->dmva_config.dmvaAppIMDSensitivity_sys;
		dmvaIMDParam.dmvaIMDPresentAdjust    = pSysInfo->dmva_config.dmvaAppIMDPresentAdjust_sys;
		dmvaIMDParam.dmvaIMDPersonMinWidth   = pSysInfo->dmva_config.dmvaAppIMDPersonMinWidth_sys;
		dmvaIMDParam.dmvaIMDPersonMinHeight  = pSysInfo->dmva_config.dmvaAppIMDPersonMinHeight_sys;
		dmvaIMDParam.dmvaIMDVehicleMinWidth  = pSysInfo->dmva_config.dmvaAppIMDVehicleMinWidth_sys;
		dmvaIMDParam.dmvaIMDVehicleMinHeight = pSysInfo->dmva_config.dmvaAppIMDVehicleMinHeight_sys;
		dmvaIMDParam.dmvaIMDPersonMaxWidth   = pSysInfo->dmva_config.dmvaAppIMDPersonMaxWidth_sys;
		dmvaIMDParam.dmvaIMDPersonMaxHeight  = pSysInfo->dmva_config.dmvaAppIMDPersonMaxHeight_sys;
		dmvaIMDParam.dmvaIMDVehicleMaxWidth  = pSysInfo->dmva_config.dmvaAppIMDVehicleMaxWidth_sys;
		dmvaIMDParam.dmvaIMDVehicleMaxHeight = pSysInfo->dmva_config.dmvaAppIMDVehicleMaxHeight_sys;
		dmvaIMDParam.dmvaIMDNumZones         = pSysInfo->dmva_config.dmvaAppIMDNumZones_sys;
		dmvaIMDParam.dmvaIMDZoneEnable       = pSysInfo->dmva_config.dmvaAppIMDZoneEnable_sys;

		dmvaIMDParam.dmvaIMDZone1_ROI_numSides           = pSysInfo->dmva_config.dmvaAppIMDZone1_ROI_numSides_sys;
		dmvaIMDParam.dmvaIMDZone2_ROI_numSides           = pSysInfo->dmva_config.dmvaAppIMDZone2_ROI_numSides_sys;
		dmvaIMDParam.dmvaIMDZone3_ROI_numSides           = pSysInfo->dmva_config.dmvaAppIMDZone3_ROI_numSides_sys;
		dmvaIMDParam.dmvaIMDZone4_ROI_numSides           = pSysInfo->dmva_config.dmvaAppIMDZone4_ROI_numSides_sys;

		for(i=0;i<16;i++) {
			dmvaIMDParam.dmvaIMDZone1_x[i]           = pSysInfo->dmva_config.dmvaAppIMDZone1_x[i+1];
			dmvaIMDParam.dmvaIMDZone1_y[i]           = pSysInfo->dmva_config.dmvaAppIMDZone1_y[i+1];
			dmvaIMDParam.dmvaIMDZone2_x[i]           = pSysInfo->dmva_config.dmvaAppIMDZone2_x[i+1];
			dmvaIMDParam.dmvaIMDZone2_y[i]           = pSysInfo->dmva_config.dmvaAppIMDZone2_y[i+1];
			dmvaIMDParam.dmvaIMDZone3_x[i]           = pSysInfo->dmva_config.dmvaAppIMDZone3_x[i+1];
			dmvaIMDParam.dmvaIMDZone3_y[i]           = pSysInfo->dmva_config.dmvaAppIMDZone3_y[i+1];
			dmvaIMDParam.dmvaIMDZone4_x[i]           = pSysInfo->dmva_config.dmvaAppIMDZone4_x[i+1];
			dmvaIMDParam.dmvaIMDZone4_y[i]           = pSysInfo->dmva_config.dmvaAppIMDZone4_y[i+1];
		}

		SendDmvaIMDSetupMsg(&dmvaIMDParam);
    }
}
int SetDmvaIMDSensitivity(unsigned int value)
{
	int ret = 0;

	ret = fSetDmvaIMDSensitivity(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDPresentAdjust(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDPresentAdjust(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDPersonMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDPersonMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDPersonMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDPersonMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDVehicleMinWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDVehicleMinWidth(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDVehicleMinHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDVehicleMinHeight(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDPersonMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDPersonMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDPersonMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDPersonMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDVehicleMaxWidth(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDVehicleMaxWidth(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDVehicleMaxHeight(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDVehicleMaxHeight(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDNumZones(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDNumZones(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}
int SetDmvaIMDZoneEnable(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZoneEnable(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone1Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4Numsides(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4Numsides(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone1_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_x16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone1_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_y16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_x16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone2_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone2_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_y16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_x16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone3_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone3_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_y16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_x16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_x16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}


int SetDmvaIMDZone4_y01(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y01(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y02(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y02(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y03(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y03(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y04(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y04(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y05(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y05(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y06(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y06(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y07(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y07(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y08(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y08(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y09(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y09(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y10(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y10(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y11(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y11(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y12(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y12(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y13(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y13(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y14(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y14(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y15(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y15(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone4_y16(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_y16(value);
	if(ret == -2) ret = 0;
	else dmvaIMDDiffParamCount ++;
	dmvaIMDParamCount++;
	if(dmvaIMDParamCount == DMVA_IMD_MAX_VARIABLE)
		SetDmvaIMDPrm();

	return ret;
}

int SetDmvaIMDZone1_Label(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaIMDZone1_Label(buf, length);
	if(ret == -2) ret = 0;
	//else dmvaIMDDiffParamCount ++;
	return ret;
}

int SetDmvaIMDZone2_Label(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaIMDZone2_Label(buf, length);
	if(ret == -2) ret = 0;
	//else dmvaIMDDiffParamCount ++;
	return ret;
}
int SetDmvaIMDZone3_Label(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaIMDZone3_Label(buf, length);
	if(ret == -2) ret = 0;
	//else dmvaIMDDiffParamCount ++;
	return ret;
}
int SetDmvaIMDZone4_Label(void * buf, int length)
{
	int ret = 0;
	ret = fSetDmvaIMDZone4_Label(buf, length);
	if(ret == -2) ret = 0;
	//else dmvaIMDDiffParamCount ++;
	return ret;
}

/* DMVA IMD save/load params */
int SetDmvaIMDSave(void * buf, int length)
{
	return fSetDmvaIMDSave(buf,length);
}

int SetDmvaIMDLoad(unsigned int value)
{
	int ret = 0;
	ret = fSetDmvaIMDLoad(value);

	if(ret == 0)
	{
        // Send the loaded IMD params to AV server
        dmvaIMDDiffParamCount = 1;
        SetDmvaIMDPrm();
	}

	return ret;
}


/* DMVA Scheduler */
#define DMVA_SCH_PARAM_COUNT 	7

int dmvaSchParamCount = 0;
DMVA_schedule_t tempDmvaSch;

int SetDmvaSchEnable(unsigned int value)
{
	SysInfo *pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
		return -1;

	// Whenever DMVA Scheduler is turned ON/OFF change the detect mode to NONE
	SetDmvaCfgCTDImdSmetaTzOc(0);

	#ifdef DMVA_SCH_DBG
	fprintf(stderr," DMVA SCH > setting detect mode to NONE\n");
	#endif

    if(value == 0)
    {
	    pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule = -1;
	}

    return fSetDmvaSchEnable(value);
}

int SetDmvaSchRuleType(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.ruleType = value;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchLoadSetting(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.loadSetting = value;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchRuleLabel(void * buf, int length)
{
    int ret = 0;

    memcpy(tempDmvaSch.ruleLabel,buf,length);
    tempDmvaSch.ruleLabel[length] = '\0';

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchStartDay(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.startDay = value + 1;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchStartTime(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.startTimeInMin = value;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchEndDay(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.endDay = value + 1;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchEndTime(unsigned int value)
{
    int ret = 0;

    tempDmvaSch.endTimeInMin = value;

    dmvaSchParamCount ++;
    if(dmvaSchParamCount == DMVA_SCH_PARAM_COUNT)
    {
		SemWait(gSemDmvaSchedule);
        ret = fSetDmvaSchCheckAddRule(&tempDmvaSch);
        SemRelease(gSemDmvaSchedule);
        dmvaSchParamCount = 0;
    }

    return ret;
}

int SetDmvaSchStart(unsigned int value)
{
    return fSetDmvaSchStart(value);
}

int SetDmvaSchDelRule(void * buf, int length)
{
	int ret = 0;

	SemWait(gSemDmvaSchedule);

	ret = fSetDmvaSchDelRule(buf,length);

	if(ret == -2)
	{
	    // the rule to be deleted is currently running
		// set detect mode to NONE
		SetDmvaCfgCTDImdSmetaTzOc(0);

		#ifdef DMVA_SCH_DBG
		fprintf(stderr," DMVA SCH > stopping the schedule rule\n");
		#endif

		ret = 0;
	}

    SemRelease(gSemDmvaSchedule);

    return ret;
}

/* DMVA Enc Roi Enable */
int SetDmvaEncRoiEnable(unsigned int value)
{
	int ret = 0;
    ret = fSetDmvaEncRoiEnable(value);
    if(ret == -2) ret = 0;
    else SendDmvaEncRoiEnableMsg(&value);
    return ret;
}

/* DMVA Set params */
int SetDmvaParams()
{
	dmvaTZDiffParamCount = 1;
	SetDmvaTZPrm();

	dmvaAdvancedDiffParamCount = 1;
	SetDmvaAdvancedPrm();

	dmvaCTDDiffParamCount = 1;
	SetDmvaCTDPrm();

	dmvaOCDiffParamCount = 1;
	SetDmvaOCPrm();

	dmvaSMETADiffParamCount = 1;
	SetDmvaSMETAPrm();

	dmvaIMDDiffParamCount = 1;
	SetDmvaIMDPrm();

	SetDmvaMainPrm();

	SetExptAlgoFrameRatePrm(0);
	SetExptAlgoDetectFreqPrm(0);
	SetExptEvtRecordEnablePrm(0);
	SetExptSmetaTrackerEnablePrm(0);
	SetDmvaPrm(0);

	return 0;
}

