/* ===========================================================================
* @file file_mng.c
*
* @path $(IPNCPATH)\sys_server\src\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <file_mng.h>
#include <sys_env_type.h>
#include <system_default.h>
#include <ipnc_ver.h>

// Global variables
char *gLogBuffer = NULL;
char *gDmvaLogBuffer = NULL;


//#define SEARCH_DBG

#define SYS_ENV_DMVA_SIZE sizeof(Dmva_Config_Data)
#define DMVA_LOG_ENTRY_SIZE	sizeof(DmvaLogEntry_t)

#ifdef __FILE_DEBUG
#define DBG(fmt, args...) fprintf(stdout, "FileMng: Debug " fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERROR(fmt,args...)	printf("FileMng: Error " fmt, ##args)

typedef struct LogData_t{
	LogEntry_t tLogData;
	struct LogData_t* pNext;
}LogData_t;

static LogData_t* gLogHead = NULL;

typedef struct DmvaLogData_t{
	DmvaLogEntry_t tDmvaLogData;
	struct DmvaLogData_t* pNext;
}DmvaLogData_t;

static DmvaLogData_t* gDmvaLogHead = NULL;

static SysInfo SysInfoDefault =
{
	REL_UNIQUE_ID, /* relUniqueId */
	1, /* nprofile */
	1, /* nrelays */
	ACOUNT_DEFAULT, /* acounts[ACOUNT_NUM] */
	DEVICE_TYPE_DEFAULT, /* DeviceType */
    /* motion_config */
  	{
		MOTIONENABLE_DEFAULT,
		MOTIONCENABLE_DEFAULT,
		MOTIONLEVEL_DEFAULT,
		MOTIONCVALUE_DEFAULT,
		MOTIONBLOCK_DEFAULT
  	},
	/* face_config */
	{
		FDETECT_DEF,
		0,
		0,
		1920,
		1080,
		1920,
		1080,
		75,
		0,
		0,
		75,
		1,
		0,
		0
	},
	/* audio_config */
	{
		0,
		0,
		2,
		80,
		0,
		0,
		0,
		64000,
		50,
		0,
		80,
		0,
		"192.168.1.1"
	},
	/* osd_config */
	{
		{
			0,
			0,
			0,
			0,
			0,
			1,
			"text1",
			0
		},
		{
			0,
			0,
			0,
			0,
			0,
			1,
			"text2",
			0
		},
		{
			0,
			0,
			0,
			0,
			0,
			1,
			"text3",
			0
		}
	},
	/* codec_advconfig */
	{
		{
			30,
			0,
			28,
			10,
			40,
			0,
			100
		},
		{
			30,
			0,
			28,
			10,
			40,
			0,
			100
		},
		{
			30,
			0,
			28,
			10,
			40,
			0,
			100
		}
	},
	/* codec_roiconfig */
	{
		{
			0,
			1920,
			1080,
			{
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
			},
		},
		{
			0,
			720,
			480,
			{
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
			},
		},
		{
			0,
			1920,
			1080,
			{
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
				{
					0,
					0,
					0,
					0,
				},
			},
		},
	},
    /* stream_config */
	{
		{
			1920,
			1080,
			8557,
			"H264",
			"h264"
		},
		{
			720,
			480,
			8556,
			"H264",
			"h264"
		},
		{
			1920,
			1080,
			8555,
			"JPEG",
			"mjpeg"
		},
	},
    /* advanceMode */
	{
		VS_DEFAULT,
		LDC_DEFAULT,
		VNF_DEFAULT,
		VNF_MODE_DEFAULT,
		VNF_STRENGTH_DEFAULT,
		DYNRANGE_DEF,
		DYNRANGE_STRENGTH
	},
    /* ftp_config */
	{
		FTP_SERVER_IP_DEFAULT,
		FTP_USERNAME_DEFAULT,
		FTP_PASSWORD_DEFAULT,
		FTP_FOLDERNAME_DEFAULT,
		FTP_IMAGEAMOUNT_DEFAULT,
		FTP_PID_DEFAULT,
		FTP_PORT_DEFAULT,
        RFTPENABLE_DEFAULT,
	    FTPFILEFORMAT_DEFAULT
	},
    /* smtp_config */
	{
		SMTP_SERVER_IP_DEFAULT,
		SMTP_SERVER_PORT_DEFAULT,
		SMTP_USERNAME_DEFAULT,
		SMTP_PASSWORD_DEFAULT,
		SMTP_AUTHENTICATION_DEFAULT,
		SMTP_SNEDER_EMAIL_DEFAULT,
		SMTP_RECEIVER_EMAIL_DEFAULT,
		SMTP_CC_DEFAULT,
		SMTP_SUBJECT_DEFAULT,
		SMTP_TEXT_DEFAULT,
		SMTP_ATTACHMENTS_DEFAULT,
		SMTP_VIEW_DEFAULT,
		ASMTPATTACH_DEFAULT,
		ATTFILEFORMAT_DEFAULT
	},
    /* sdcard_config */
	{
		SDFILEFORMAT_DEFAULT,
		SDRENABLE_DEFAULT,
		SD_INSERT_DEFAULT
    },
    /* lan_config */
	{
		TITLE_DEFAULT,
		HOST_DEFAULT,
		WHITEBALANCE_DEFAULT,
		DAY_NIGHT_DEFAULT,
		BINNING_DEFAULT,
		BACKLIGHT_CONTROL_DEFAULT,
		BACKLIGHT_VALUE_DEFAULT,
		BRIGHTNESS_DEFAULT,
		CONTRAST_DEFAULT,
		SATURATION_DEFAULT,
		SHARPNESS_DEFAULT,
		LOSTALARM_DEFAULT,
		SDAENABLE_DEFAULT,
		AFTPENABLE_DEFAULT,
		ASMTPENABLE_DEFAULT,
#ifdef RVM_DEMO_MODE
		0,	// SINGLE_STREAM
		0,	// SINGLE_H264
#else		
		STREAMTYPE_DEFAULT,
		VCODECMODE_DEFAULT,
#endif		
		VCODECCOMBO_DEFAULT,
		VCODECRES_DEFAULT,
		FRAMERATE1_DEFAULT,
		FRAMERATE2_DEFAULT,
		FRAMERATE3_DEFAULT,
		JPEGQUALITY_DEF,
		MPEG41BITRATE_DEFAULT,
		MPEG42BITRATE_DEFAULT,
		MPEG41XSIZE_DEFAULT,
		MPEG41YSIZE_DEFAULT,
		MPEG42XSIZE_DEFAULT,
		MPEG42YSIZE_DEFAULT,
		JPEGXSIZE_DEFAULT,
		JPEGYSIZE_DEFAULT,
		MPEG41XSIZE_DEFAULT,
		MPEG41YSIZE_DEFAULT,
		MPEG42XSIZE_DEFAULT,
		MPEG42YSIZE_DEFAULT,
		SUPPORTSTREAM1_DEFAULT,
		SUPPORTSTREAM2_DEFAULT,
		SUPPORTSTREAM3_DEFAULT,
		SUPPORTSTREAM4_DEFAULT,
		SUPPORTSTREAM5_DEFAULT,
		SUPPORTSTREAM6_DEFAULT,
		ALARMDURATION_DEFAULT,
		AEW_SWITCH_DEFAULT,
		GIOINENABLE_DEFAULT,
		GIOINTYPE_DEFAULT,
		GIOOUTENABLE_DEFAULT,
		GIOOUTTYPE_DEFAULT,
		AVIDURATION_DEFAULT,
		AVIFORMAT_DEFAULT,
		ALARMSTATUS_DEFAULT,
      	MIRROR_DEF,
      	ENCRYPT_DEF,
      	DEMOCFG_DEF,
		AEW_TYPE_DEFAULT,
		HISTOGRAM_DEF,
		FRAMECTRL_DEFAULT,
		DATEFORMAT_DEFAULT,
		TSTAMPFORMAT_DEFAULT,
		1,
		1,
		0,
		"snap",
		0,
		RATECONTROL_DEFAULT,
		RATECONTROL_DEFAULT,
		LOCALDISPLAY_DEFAULT,
		1,
 		0,
		0,
		0,
		0,
		52,
		1, /* Enable infinite schedule */
		0,
		0,
		0,
		MJPEG_CODEC,//H264_CODEC, /* codectype1 */
		NO_CODEC, //H264_CODEC, /* codectype2 */
		NO_CODEC, //H264_CODEC, /* codectype3 */
     	0,
     	0,
     	POWER_MODE_DEF,
     	MODELNAME_DEFAULT,
		SCHEDULE_DEFAULT,
     	0,
     	0,
     	MAX_FRAMERATE_DEFAULT, //original
        //5,
     	MAX_FRAMERATE_DEFAULT,
     	MAX_FRAMERATE_DEFAULT,
     	DUMMY_DEF,
        /* net */
		{
			{IP_DEFAULT},
			{NETMASK_DEFAULT},
			{GATEWAY_DEAFULT},
			{DNS_DEFAULT},
			HTTP_PORT_DEFAULT,
			HTTPS_PORT_DEFAULT,
			DHCP_ENABLE_DEFAULT,
			DHCP_CONFIG_DEFAULT,
            DISCOVERY_MODE,
			NTP_SERVER_DEFAULT,
			NTP_TIMEZONE_DEFAULT,
			DAYLIGHT_DEFAULT,
			MAC_DEFAULT,
			IMAGESOURCE_DEFAULT,
			MULTICAST_ENABLE_DEFAULT
      	},
      	PWM1_DUTY_DEFAULT
	},
	{
		PTZZOOMIN_DEFAULT,
		PTZZOOMOUT_DEFAULT,
		PTZPANUP_DEFAULT,
		PTZPANDOWN_DEFAULT,
		PTZPANLEFT_DEFAULT,
		PTZPANRIGHT_DEFAULT
  	},
	CUR_LOG_DEFAULT,
	ONVIF_PROFILE,  /* onvif profiles */
	ONVIF_SCOPES,   /* onvif scopes */
	RELAYS_CONFIG,   /* onvif relays*/
	IMAGING_CONF,     /*imaging configuration */
	CUR_DMVA_LOG_DEFAULT,
	{
		1,       	// DMVA enable
		10,       	// dmvaexptalgoframerate_sys frame rate in fps.  (frame rate of 10fps implies that (30/10) = 3 frames will be skipped)
		2,      	// dmvaexptalgodetectfreq_sys detection rate in fps. (det rate of 2fps means that every 5th frame will be passed to detection module (10/2 = 5))
		1,       	// dmvaexptevtrecordenable_sys enable event recording
		0,       	// dmvaexptSmetaTrackerEnable_sys - On/Off SMETA tracker

  /* DMVA MAIN ANALYTICS PAGE */
		1,   		// cam id
		0,   		// schedule (0: off)
		0,   		// fd
		8,   		// detect mode, currently set to Trip Zone
		15,  		// displayoptions
		11,  		// event record
		0,  		// action
		1, 			// select event
		0,  		// archive flag 0: video
		1,  		// Event Type 1: TZ
		1,
		1,
		1,
		2,
		2,
		2,
		"TIMESTAMP",
		"EVT DETAILS",
  /* DMVA ADVANCED SETTINGS PAGE */
		83,
		2,
		5,
		3,
		6,
		5,
		3,
		2,
		6,
		1,
		2,
		3,
		1,
		"DESC1;DESC2",
  /* DMVA CTD PAGE */
		10, 		// not used
		11, 		// not used
		3,  		// Moderate sensitivity
		5,  		// reset time

	/* DMVA TZ Variables */
		3,    		// sensitivity
		10,   		// person min width
		10,   		// person min height
		10,   		// vehicle min width
		10,   		// vehicle min height
		20,   		// person max width
		20,   		// person max height
		20,   		// vehicle max width
		20,   		// vehicle max height
		0,    		// direction
		1,    		// enable

		4,			// zone 1 sides
		4,			// zone 2 sides

		0,			// zone 1, x, not used
		50,			// 32,//zone 1, x
		50,			// 144,
		11,			// 144,
		11,			// 32,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 1, y, not used
		17,			// 32,//zone 1, y
		83,			// 32,
		83,			// 160,
		17,			// 160,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 2, x, not used
		50,			// 144,//zone 2, x
		50,			// 256,
		89,			// 256,
		89,			// 144,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 2, y, not used
		17,			// 32,//zone 2, y
		83,			// 32,
		83,			// 160,
		17,			// 160,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"", 		// save
		0, 			// load
		12, 		// PresentAdjust

  /* DMVA OC PAGE */
		3,  		// sensitivity
		10,			// obj width
		10,			// obj height
		0,			// direction [0: bidirectional, 1: direction_1, 2: directin_2]
		1,			// enable
		1,			// left-right
		0,			// top-bottom

		"",  		// save
		0,   		// load

  /* DMVA SMETA PAGE */
		3,	  		// sensitivity
		23,			// enable
		0,
		0,
		0,
		0,
		12,	  		// Present Adjust 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle
		10,   		// person min width
		10,   		// person min height
		50,   		// person max width
		50,   		// person max height
		10,   		// vehicle min width
		10,   		// vehicle min height
		50,   		// vehicle max width
		50,   		// vehicle max height

		1,    		// numZones
		4,    		// numSides

		0,    		// zone 1, x, not used
		0,    		// zone 1, x
		100,
		100,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 1, y, not used
		0, 			// zone 1, y
		0,
		100,
		100,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		// SMETA Zone Labels
		"MYSMETAZONE1",

		"",  		// save
		0,   		// load

		0,   		// Enable velocity and obj id
  /* DMVA IMD PAGE */
		3,
		12,  		// Present Adjust
		10,  		// person min width
		10,  		// person min height
		10,  		// vehicle min width
		10,  		// vehicle min height
		50,  		// person max width
		50,  		// person max height
		50,  		// vehicle max width
		50,  		// vehicle max height
		2,   		// Num Zones
		15,  		// Zone Enable

		4,			// zone 1 sides
		4,			// zone 2 sides
		4,			// zone 3 sides
		4,			// zone 4 sides


		0,			// zone 1, x, not used
		13, 		// 5,//zone 1, x
		49, 		// 45,//719,
		49, 		// 45,//719,
		13, 		// 5,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 1, y, not used
		43, 		// 5,//zone 1, y
		43, 		// 5,
		96, 		// 45,//239,
		96, 		// 45,//239,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 2, x, not used
		3, 			// 5,//zone 2, x
		59, 		// 45,//719,
		59, 		// 45,//719,
		3, 			// 5,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 2, y, not used
		4, 			// 55,//239,//zone 2, y
		4, 			// 55,//239,
		34, 		// 95,//479,
		34, 		// 95,//479,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 3, x, not used
		0, 			// 55,//zone 3, x
		0, 			// 95,//719,
		0, 			// 95,//719,
		0, 			// 55,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 3, x, not used
		0, 			// 5,//239,//zone 2, y
		0, 			// 5,//239,
		0, 			// 45,//479,
		0, 			// 45,//479,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 4, x, not used
		0, 			// 55,//zone 4, x
		0, 			// 95,//719,
		0, 			// 95,//719,
		0, 			// 55,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		0,			// zone 4, y, not used
		0, 			// 55,//239,//zone 2, y
		0, 			// 55,//239,
		0, 			// 95,//479,
		0, 			// 95,//479,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,

		// IMD Zone Labels
		"MYIMDZONE1",
		"MYIMDZONE2",
		"MYIMDZONE3",
		"MYIMDZONE4",

		"",  		// save
		0,   		// load

		0,  		// dmvaEventDeleteIndex
		0,  		// dmvaEventDeleteStartIndex
		0,  		// dmvaEventDeleteEndIndex
		0,  		// dmvaEventDeleteAll

		0,  		// dmvaAppEncRoiEnable

		6,  		// TZ/OC min width limit
		9,  		// TZ/OC min height limit
		25, 		// TZ/OC max width limit
		25, 		// TZ/OC min height limit

		2,  		// IMD/SMETA people min width limit
		7,  		// IMD/SMETA people min height limit
		50, 		// IMD/SMETA people max width limit
		50, 		// IMD/SMETA people max height limit

		4,  		// IMD/SMETA vehicle min width limit
		4,  		// IMD/SMETA vehicle min height limit
		50, 		// IMD/SMETA vehicle max width limit
		50, 		// IMD/SMETA vehicle max height limit

		// TZ save/load
		0,  		// curRuleNo
		0,  		// numRules
		{"","","","",""},

		// IMD save/load
		0,  		// curRuleNo
		0,  		// numRules
		{"","","","",""},

		// OC save/load
		0,  		// curRuleNo
		0,  		// numRules
		{"","","","",""},

		// SMETA save/load
		0,  		// curRuleNo
		0,  		// numRules
		{"","","","",""},

		0, 			// serch start index
		0  			// serch end index

  /* DMVA ADV SETTINGS PAGE */

	},
	RTSPSERVERPORT
};

/**
 * @brief	check magic number
 * @param	fp [I ]file pointer
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int check_magic_num(FILE *fp)
{
	int ret;
	unsigned long MagicNum;
	if(fread(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
		ret = FAIL;
	} else {
		if(MagicNum == MAGIC_NUM){
			ret = SUCCESS;
		} else {
			ret = FAIL;
		}
	}
	return ret;
}
/**
 * @brief	Add new log event to tail
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddLogToTail(LogEntry_t *pLog)
{
	LogData_t *pLogData, *pTail = NULL;
	DBG("pLog->tLogData.event=%s\n", pLog->event);
	DBG("pLog->tLogData.time=%s\n", asctime(& pLog->time));
	pLogData = (LogData_t*) malloc(sizeof(LogData_t));
	if(pLogData == NULL){
		DBG("No enough memory\n");
		if(gLogHead == NULL)
			return FAIL;
		/* Do Nothing if no more memory */
	} else {
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		pLogData->pNext = NULL;
		if(gLogHead == NULL){
			gLogHead = pLogData;
		} else {
			for(pTail = gLogHead;pTail->pNext != NULL;)
				pTail = pTail->pNext;
			pTail->pNext = pLogData;
		}
	}
	return SUCCESS;
}

/**
 * @brief	Add new log event to tail
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddDmvaLogToTail(DmvaLogEntry_t *pDmvaLog)
{
	DmvaLogData_t *pDmvaLogData, *pTail = NULL;
	DBG("pDmvaLog->tDmvaLogData.event=%s\n", pDmvaLog->eventListEventDetails_sys);
	DBG("pDmvaLog->tDmvaLogData.time=%s\n", asctime(& pDmvaLog->time));
	pDmvaLogData = (DmvaLogData_t*) malloc(sizeof(DmvaLogData_t));
	if(pDmvaLogData == NULL){
		DBG("No enough memory\n");
		if(gDmvaLogHead == NULL)
			return FAIL;
		/* Do Nothing if no more memory */
	} else {
		memcpy(&pDmvaLogData->tDmvaLogData, pDmvaLog, sizeof(DmvaLogEntry_t));
		pDmvaLogData->pNext = NULL;
		if(gDmvaLogHead == NULL){
			gDmvaLogHead = pDmvaLogData;
		} else {
			for(pTail = gDmvaLogHead;pTail->pNext != NULL;)
				pTail = pTail->pNext;
			pTail->pNext = pDmvaLogData;
		}
	}
	return SUCCESS;
}
/**
 * @brief	Add new log event
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddLog(LogEntry_t *pLog)
{
	LogData_t *pLogData, *pFrontData = NULL;
	DBG("pLog->tLogData.event=%s\n", pLog->event);
	DBG("pLog->tLogData.time=%s\n", asctime(& pLog->time));
	pLogData = (LogData_t*) malloc(sizeof(LogData_t));
	if(pLogData == NULL){
		DBG("No enough memory\n");
		if(gLogHead == NULL)
			return FAIL;
		/* If no more memory, replace the oldest one with current. */
		pLogData = gLogHead;
		while(pLogData->pNext != NULL){
			pFrontData = pLogData;
			pLogData = pLogData->pNext;
		}
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		if(pFrontData != NULL){
			pFrontData->pNext = NULL;
			pLogData->pNext = gLogHead;
			gLogHead = pLogData;
		}
	} else {
		memcpy(&pLogData->tLogData, pLog, sizeof(LogEntry_t));
		pLogData->pNext = gLogHead;
		gLogHead = pLogData;
	}
	DBG("gLogHead->tLogData.event=%s\n", gLogHead->tLogData.event);
	DBG("gLogHead->tLogData.time=%s\n", asctime(& gLogHead->tLogData.time));
	return SUCCESS;
}
/**
 * @brief	Add new log event
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int AddDmvaLog(DmvaLogEntry_t *pDmvaLog)
{
	DmvaLogData_t *pDmvaLogData, *pFrontData = NULL;
	DBG("pDmvaLog->tDmvaLogData.eventListEventDetails_sys=%s\n", pDmvaLog->eventListEventDetails_sys);
	DBG("pDmvaLog->tDmvaLogData.time=%s\n", asctime(& pDmvaLog->time));
	pDmvaLogData = (DmvaLogData_t*) malloc(sizeof(DmvaLogData_t));
	if(pDmvaLogData == NULL){
		DBG("No enough memory\n");
		if(gDmvaLogHead == NULL)
			return FAIL;
		/* If no more memory, replace the oldest one with current. */
		pDmvaLogData = gDmvaLogHead;
		while(pDmvaLogData->pNext != NULL){
			pFrontData = pDmvaLogData;
			pDmvaLogData = pDmvaLogData->pNext;
		}
		memcpy(&pDmvaLogData->tDmvaLogData, pDmvaLog, sizeof(DmvaLogEntry_t));
		if(pFrontData != NULL){
			pFrontData->pNext = NULL;
			pDmvaLogData->pNext = gDmvaLogHead;
			gDmvaLogHead = pDmvaLogData;
		}
	} else {
		memcpy(&pDmvaLogData->tDmvaLogData, pDmvaLog, sizeof(DmvaLogEntry_t));
		pDmvaLogData->pNext = gDmvaLogHead;
		gDmvaLogHead = pDmvaLogData;
	}
	DBG("gDmvaLogHead->tDmvaLogData.eventListEventDetails_sys=%s\n", gDmvaLogHead->tDmvaLogData.eventListEventDetails_sys);
	DBG("gDmvaLogHead->tDmvaLogData.time=%s\n", asctime(& gDmvaLogHead->tDmvaLogData.time));
	return SUCCESS;
}

/**
 * @brief	Show all system log
 * @return
 */
void ShowAllLog()
{
	LogData_t *ptr;
	for(ptr = gLogHead;ptr != NULL;ptr = ptr->pNext){
		fprintf(stderr, "Event:%s\n", ptr->tLogData.event);
		fprintf(stderr, "Time:%s\n", asctime(&ptr->tLogData.time));
	}
}
/**
 * @brief	Clean system log
 * @return
 */
void CleanLog()
{
	LogData_t *ptr;
	while(gLogHead != NULL){
		ptr = gLogHead;
		gLogHead = gLogHead->pNext;
		free(ptr);
	}
}
/**
 * @brief	Clean system log
 * @return
 */
void CleanDmvaLog()
{
	DmvaLogData_t *ptr;
	while(gDmvaLogHead != NULL){
		ptr = gDmvaLogHead;
		gDmvaLogHead = gDmvaLogHead->pNext;
		free(ptr);
	}
}
/**
 * @brief	read log from log file
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int ReadLog()
{
	FILE *fp;
	char Buffer[LOG_ENTRY_SIZE];
	int ret = SUCCESS, count = 0;
	if((fp = fopen(LOG_FILE, "rb")) == NULL){
		/* log file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			while(count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fread(Buffer, 1, LOG_ENTRY_SIZE,fp) != LOG_ENTRY_SIZE){
					break;
				} else {
					if(AddLogToTail((LogEntry_t *)Buffer) != SUCCESS){
						ret = FAIL;
						break;
					}
					count++;
				}
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;
}
/**
 * @brief	read log from log file
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int ReadDmvaLog()
{
	FILE *fp;
	char Buffer[DMVA_LOG_ENTRY_SIZE];
	int ret = SUCCESS, count = 0;
	if((fp = fopen(DMVA_LOG_FILE, "rb")) == NULL){
		/* log file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			while(count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fread(Buffer, 1, DMVA_LOG_ENTRY_SIZE,fp) != DMVA_LOG_ENTRY_SIZE){
					break;
				} else {
					if(AddDmvaLogToTail((DmvaLogEntry_t *)Buffer) != SUCCESS){
						ret = FAIL;
						break;
					}
					count++;
				}
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;
}
/**
 * @brief	create log file
 * @param	"int nPageNum" : [IN]log page number
 * @param	"int nItemIndex" : [IN]log index in that page
 * @return	LogEntry_t*: log data
 */
LogEntry_t* GetLog(int nPageNum, int nItemIndex)
{
	LogData_t* pLog;
	int count, index = nPageNum * NUM_LOG_PER_PAGE + nItemIndex;
	if(nPageNum < 0 || nItemIndex < 0)
		return NULL;
	for(count = 0, pLog = gLogHead;(count < index) && (pLog != NULL);count ++)
		pLog = pLog->pNext;
	return &pLog->tLogData;
}
/**
 * @brief	create log file
 * @param	"int nPageNum" : [IN]log page number
 * @param	"int nItemIndex" : [IN]log index in that page
 * @return	LogEntry_t*: log data
 */
#if 0
DmvaLogEntry_t* GetDmvaLog(int nPageNum, int nItemIndex)
{
	DmvaLogData_t* pDmvaLog;
	int count, index = nPageNum * NUM_LOG_PER_PAGE + nItemIndex;
	if(nPageNum < 0 || nItemIndex < 0)
		return NULL;
	for(count = 0, pDmvaLog = gDmvaLogHead;(count < index) && (pDmvaLog != NULL);count ++)
		pDmvaLog = pDmvaLog->pNext;
	return &pDmvaLog->tDmvaLogData;
}
#else
DmvaLogEntry_t* GetDmvaLog(SysInfo *pSysInfo,int nPageNum, int nItemIndex)
{
	DmvaLogData_t* pDmvaLog;
	int count, index = nPageNum * NUM_LOG_PER_PAGE + nItemIndex;

	if(nPageNum < 0 || nItemIndex < 0)
	{
		return NULL;
	}

    if(pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys & 0x2)
    {
	    // Search button is ON
	    if(pSysInfo->dmva_config.dmvaEventSearchListStartIndex != 0)
	    {
	        index += (pSysInfo->dmva_config.dmvaEventSearchListStartIndex - 1);
		}

#ifdef SEARCH_DBG
        printf("search is ON and the index for search is %d,start index = %d\n",index,pSysInfo->dmva_config.dmvaEventSearchListStartIndex);
#endif
	}

	for(count = 0, pDmvaLog = gDmvaLogHead;(count < index) && (pDmvaLog != NULL);count ++)
	{
		pDmvaLog = pDmvaLog->pNext;

		if(pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys & 0x2)
		{
            // Search button is ON
            // check for end index
			if(count >= (pSysInfo->dmva_config.dmvaEventSearchListEndIndex - 1))
			{
#ifdef SEARCH_DBG
				printf("search is ON and the index(%d) is beyond end index(%d)\n",count,pSysInfo->dmva_config.dmvaEventSearchListEndIndex - 1);
#endif

				if(pSysInfo->dmva_config.dmvaEventSearchListEndIndex != 0)
				{
				    return NULL;
				}
			}
	    }
	}

	return &pDmvaLog->tDmvaLogData;
}
#endif
/**
 * @brief	create log file
 * @param	name [I ]File name to create in nand.
 * @param	pLogData  [I ]pointer to log list
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int create_log_file(char *name, LogData_t*pLogData){
	FILE *fp;
	int ret, count = 0;
	unsigned long MagicNum = MAGIC_NUM;

	char *logBufAddr = gLogBuffer;
	int writeSize;

#if 1
    // write the magic number into log buffer
	memcpy(logBufAddr,&MagicNum,sizeof(MagicNum));
	logBufAddr += sizeof(MagicNum);

    // write the log details into the log buffer
	while((pLogData != NULL) && (count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM))
	{
	    memcpy(logBufAddr,&pLogData->tLogData,LOG_ENTRY_SIZE);
	    logBufAddr += LOG_ENTRY_SIZE;
	    count ++;
	    pLogData = pLogData->pNext; // next log
	}

    writeSize = (count * LOG_ENTRY_SIZE) + sizeof(MagicNum);

    // write the contents of the log buffer to the file
	if((fp = fopen(name, "wb")) == NULL)
	{
	    ERROR("Can't create log file\n");
		ret = FAIL;
	}
	else
	{
        if(fwrite(gLogBuffer,1,writeSize,fp) != writeSize)
        {
		    ERROR("Writing log fail\n");
		    ret = FAIL;
		}
        else
        {
		    ret = SUCCESS;
		}

        fclose(fp);
	}

    return ret;
#else
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create log file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} else {
			ret = SUCCESS;
			while(pLogData != NULL &&
					count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fwrite(&pLogData->tLogData, 1, LOG_ENTRY_SIZE, fp) !=
						LOG_ENTRY_SIZE){
					ERROR("Writing log fail\n");
					ret = FAIL;
					break;
				} else {
					count++;
					pLogData = pLogData->pNext;
				}
		}
		fclose(fp);
	}
	return ret;
#endif
}
/**
 * @brief	create log file
 * @param	name [I ]File name to create in nand.
 * @param	pLogData  [I ]pointer to log list
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int create_dmva_log_file(char *name, DmvaLogData_t*pDmvaLogData){
	FILE *fp;
	int ret, count = 0;
	unsigned long MagicNum = MAGIC_NUM;

	char *logBufAddr = gDmvaLogBuffer;
	int writeSize;

#if 1
    // write the magic number into log buffer
	memcpy(logBufAddr,&MagicNum,sizeof(MagicNum));
	logBufAddr += sizeof(MagicNum);

    // write the log details into the log buffer
	while((pDmvaLogData != NULL) && (count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM))
	{
	    memcpy(logBufAddr,&pDmvaLogData->tDmvaLogData,DMVA_LOG_ENTRY_SIZE);
	    logBufAddr += DMVA_LOG_ENTRY_SIZE;
	    count ++;
	    pDmvaLogData = pDmvaLogData->pNext; // next log
	}

    writeSize = (count * DMVA_LOG_ENTRY_SIZE) + sizeof(MagicNum);

    // write the contents of the log buffer to the file
	if((fp = fopen(name, "wb")) == NULL)
	{
	    ERROR("Can't create log file\n");
		ret = FAIL;
	}
	else
	{
        if(fwrite(gDmvaLogBuffer,1,writeSize,fp) != writeSize)
        {
		    ERROR("Writing log fail\n");
		    ret = FAIL;
		}
		else
		{
		    ret = SUCCESS;
		}

        fclose(fp);
	}

    return ret;
#else
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create log file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} else {
			ret = SUCCESS;
			while(pDmvaLogData != NULL &&
					count < NUM_LOG_PER_PAGE * MAX_LOG_PAGE_NUM)
				if(fwrite(&pDmvaLogData->tDmvaLogData, 1, DMVA_LOG_ENTRY_SIZE, fp) !=
						DMVA_LOG_ENTRY_SIZE){
					ERROR("Writing log fail\n");
					ret = FAIL;
					break;
				} else {
					count++;
					pDmvaLogData = pDmvaLogData->pNext;
				}
		}
		fclose(fp);
	}
	return ret;
#endif
}
/**
 * @brief	create system file
 * @param	name [I ]File name to create in nand.
 * @param	Global [I ]Pointer to System information
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int create_sys_file(char *name, void *Global){
	FILE *fp;
	int ret;
	unsigned long MagicNum = MAGIC_NUM;
	if((fp = fopen(name, "wb")) == NULL){
		ERROR("Can't create system file\n");
		ret = FAIL;
	} else {
		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum)){
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		} else {
			if(fwrite(Global, 1, SYS_ENV_SIZE, fp) != SYS_ENV_SIZE){
				ERROR("Writing global fail\n");
				ret = FAIL;
			} else {
				ret = SUCCESS;
			}
		}
		fclose(fp);

		/* must sync twice !? */
		system("sync");
		system("sync");
	}
	return ret;
}


// AYK - 0310
int create_dmva_sys_file(char *name, void *Global,int setNo)
{
	FILE *fp;
	int ret;
	unsigned long MagicNum = MAGIC_NUM;
	if((fp = fopen(name, "r+b")) == NULL)
	{
		if((fp = fopen(name,"wb")) == NULL)
		{
			ERROR("Can't create %s file\n",name);
			ret = FAIL;
		}
		else
		{
		    fclose(fp);
		    if((fp = fopen(name, "r+b")) == NULL)
		    {
				ERROR("Can't create %s file\n",name);
				ret = FAIL;
			}
			else
			{
			   goto write;
			}
		}
	}
	else
	{
write:
		if(setNo < 0) setNo = 0;
		if(setNo >= MAX_DMVA_SAVELOAD_SETS) setNo = MAX_DMVA_SAVELOAD_SETS - 1;

		if(fseek(fp,setNo * (SYS_ENV_DMVA_SIZE + sizeof(unsigned long)),SEEK_SET) < 0)
		{
			ERROR("fseek fail\n");
			ret = FAIL;
		}

		if(fwrite(&MagicNum, 1, sizeof(MagicNum), fp) != sizeof(MagicNum))
		{
			ERROR("Writing Magic Number fail\n");
			ret = FAIL;
		}
		else
		{
			if(fwrite(Global, 1, SYS_ENV_DMVA_SIZE, fp) != SYS_ENV_DMVA_SIZE)
			{
				ERROR("Writing global fail\n");
				ret = FAIL;
			}
			else
			{
				ret = SUCCESS;
			}
		}

		fclose(fp);
	}

	return ret;
}

/**
 * @brief	read SysInfo from system file
 * @param	"void *Buffer" : [OUT]buffer to store SysInfo
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int ReadGlobal(void *Buffer)
{
	FILE *fp;
	int ret;
	if((fp = fopen(SYS_FILE, "rb")) == NULL){
		/* System file not exist */
		ret = FAIL;
	} else {
		if(check_magic_num(fp) == SUCCESS){
			if(fread(Buffer, 1, SYS_ENV_SIZE,fp) != SYS_ENV_SIZE){
				ret = FAIL;
			} else {
				ret = SUCCESS;
			}
		} else {
			ret = FAIL;
		}
		fclose(fp);
	}
	return ret;
}

int GetReadIndexDmvaSaveLoad(Dmva_save_load_rules_t *pSaveLoad,char *ruleName)
{
    int i;

    for(i = 0;i < MAX_DMVA_SAVELOAD_SETS;i ++)
    {
	    if(strcmp(pSaveLoad->ruleName[i],ruleName) == 0)
	    {
			printf("rule number = %d,rule name = %s\n",i + 1,pSaveLoad->ruleName[i]);
		    return i;
		}
	}

    printf("couldn't find the rule with the rule name %s\n",ruleName);

	return -1;
}


int ReadGlobalDmva(char *fileName,void *Buffer,int setNo)
{
	FILE *fp;
	int ret;

	if((fp = fopen(fileName, "rb")) == NULL)
	{
		ret = FAIL;
	}
	else
	{
		if(setNo < 0) return FAIL;
		if(setNo >= MAX_DMVA_SAVELOAD_SETS) setNo = MAX_DMVA_SAVELOAD_SETS - 1;

		if(fseek(fp,setNo * (SYS_ENV_DMVA_SIZE + sizeof(unsigned long)),SEEK_SET) < 0)
		{
			ERROR("fseek fail\n");
			ret = FAIL;
		}

		if(check_magic_num(fp) == SUCCESS)
		{
			if(fread(Buffer, 1, SYS_ENV_DMVA_SIZE,fp) != SYS_ENV_DMVA_SIZE)
			{
				ret = FAIL;
			}
			else
			{
				ret = SUCCESS;
			}
		}
		else
		{
			ret = FAIL;
		}

		fclose(fp);
	}

	return ret;
}

int ReadGlobalDmvaTZ(void *Buffer,int setNo)
{
	printf("TZ load rule no = %d\n",setNo);
    return (ReadGlobalDmva(SYS_DMVATZ_FILE,Buffer,setNo));
}

int ReadGlobalDmvaIMD(void *Buffer,int setNo)
{
	printf("IMD load rule no = %d\n",setNo);
    return (ReadGlobalDmva(SYS_DMVAIMD_FILE,Buffer,setNo));
}

int ReadGlobalDmvaOC(void *Buffer,int setNo)
{
	printf("OC load rule no = %d\n",setNo);
    return (ReadGlobalDmva(SYS_DMVAOC_FILE,Buffer,setNo));
}

int ReadGlobalDmvaSMETA(void *Buffer,int setNo)
{
	printf("SMETA load rule no = %d\n",setNo);
    return (ReadGlobalDmva(SYS_DMVASMETA_FILE,Buffer,setNo));
}

int CopyDmvaTzParams(SysInfo *pDst,Dmva_Config_Data *pSrc)
{
	int i;

    pDst->dmva_config.dmvaAppTZSensitivity_sys        = pSrc->dmvaAppTZSensitivity_sys;
    pDst->dmva_config.dmvaAppTZPersonMinWidth_sys     = pSrc->dmvaAppTZPersonMinWidth_sys;
    pDst->dmva_config.dmvaAppTZPersonMinHeight_sys    = pSrc->dmvaAppTZPersonMinHeight_sys;
    pDst->dmva_config.dmvaAppTZVehicleMinWidth_sys    = pSrc->dmvaAppTZVehicleMinWidth_sys;
    pDst->dmva_config.dmvaAppTZVehicleMinHeight_sys   = pSrc->dmvaAppTZVehicleMinHeight_sys;
    pDst->dmva_config.dmvaAppTZPersonMaxWidth_sys     = pSrc->dmvaAppTZPersonMaxWidth_sys;
    pDst->dmva_config.dmvaAppTZPersonMaxHeight_sys    = pSrc->dmvaAppTZPersonMaxHeight_sys;
    pDst->dmva_config.dmvaAppTZVehicleMaxWidth_sys    = pSrc->dmvaAppTZVehicleMaxWidth_sys;
    pDst->dmva_config.dmvaAppTZVehicleMaxHeight_sys   = pSrc->dmvaAppTZVehicleMaxHeight_sys;
    pDst->dmva_config.dmvaAppTZDirection_sys          = pSrc->dmvaAppTZDirection_sys;
    pDst->dmva_config.dmvaAppTZEnable_sys             = pSrc->dmvaAppTZEnable_sys;
    pDst->dmva_config.dmvaAppTZZone1_ROI_numSides_sys = pSrc->dmvaAppTZZone1_ROI_numSides_sys;
    pDst->dmva_config.dmvaAppTZZone2_ROI_numSides_sys = pSrc->dmvaAppTZZone2_ROI_numSides_sys;

    for(i = 0;i < 17;i ++)
    {
	    pDst->dmva_config.dmvaAppTZZone1_x[i] = pSrc->dmvaAppTZZone1_x[i];
	    pDst->dmva_config.dmvaAppTZZone1_y[i] = pSrc->dmvaAppTZZone1_y[i];
	    pDst->dmva_config.dmvaAppTZZone2_x[i] = pSrc->dmvaAppTZZone2_x[i];
	    pDst->dmva_config.dmvaAppTZZone2_y[i] = pSrc->dmvaAppTZZone2_y[i];
	}

	return 0;
}

int CopyDmvaImdParams(SysInfo *pDst,Dmva_Config_Data *pSrc)
{
    int i;

    pDst->dmva_config.dmvaAppIMDSensitivity_sys      = pSrc->dmvaAppIMDSensitivity_sys;
    pDst->dmva_config.dmvaAppIMDPresentAdjust_sys    = pSrc->dmvaAppIMDPresentAdjust_sys;
    pDst->dmva_config.dmvaAppIMDPersonMinWidth_sys   = pSrc->dmvaAppIMDPersonMinWidth_sys;
    pDst->dmva_config.dmvaAppIMDPersonMinHeight_sys  = pSrc->dmvaAppIMDPersonMinHeight_sys;
    pDst->dmva_config.dmvaAppIMDVehicleMinWidth_sys  = pSrc->dmvaAppIMDVehicleMinWidth_sys;
    pDst->dmva_config.dmvaAppIMDVehicleMinHeight_sys = pSrc->dmvaAppIMDVehicleMinHeight_sys;
    pDst->dmva_config.dmvaAppIMDPersonMaxWidth_sys   = pSrc->dmvaAppIMDPersonMaxWidth_sys;
    pDst->dmva_config.dmvaAppIMDPersonMaxHeight_sys  = pSrc->dmvaAppIMDPersonMaxHeight_sys;
    pDst->dmva_config.dmvaAppIMDVehicleMaxWidth_sys  = pSrc->dmvaAppIMDVehicleMaxWidth_sys;
    pDst->dmva_config.dmvaAppIMDVehicleMaxHeight_sys = pSrc->dmvaAppIMDVehicleMaxHeight_sys;
    pDst->dmva_config.dmvaAppIMDNumZones_sys         = pSrc->dmvaAppIMDNumZones_sys;
    pDst->dmva_config.dmvaAppIMDZoneEnable_sys       = pSrc->dmvaAppIMDZoneEnable_sys;

    pDst->dmva_config.dmvaAppIMDZone1_ROI_numSides_sys = pSrc->dmvaAppIMDZone1_ROI_numSides_sys;
    pDst->dmva_config.dmvaAppIMDZone2_ROI_numSides_sys = pSrc->dmvaAppIMDZone2_ROI_numSides_sys;
    pDst->dmva_config.dmvaAppIMDZone3_ROI_numSides_sys = pSrc->dmvaAppIMDZone3_ROI_numSides_sys;
    pDst->dmva_config.dmvaAppIMDZone4_ROI_numSides_sys = pSrc->dmvaAppIMDZone4_ROI_numSides_sys;

    for(i = 0;i < 17;i ++)
    {
        pDst->dmva_config.dmvaAppIMDZone1_x[i] = pSrc->dmvaAppIMDZone1_x[i];
        pDst->dmva_config.dmvaAppIMDZone1_y[i] = pSrc->dmvaAppIMDZone1_y[i];
        pDst->dmva_config.dmvaAppIMDZone2_x[i] = pSrc->dmvaAppIMDZone2_x[i];
        pDst->dmva_config.dmvaAppIMDZone2_y[i] = pSrc->dmvaAppIMDZone2_y[i];
        pDst->dmva_config.dmvaAppIMDZone3_x[i] = pSrc->dmvaAppIMDZone3_x[i];
        pDst->dmva_config.dmvaAppIMDZone3_y[i] = pSrc->dmvaAppIMDZone3_y[i];
        pDst->dmva_config.dmvaAppIMDZone4_x[i] = pSrc->dmvaAppIMDZone4_x[i];
        pDst->dmva_config.dmvaAppIMDZone4_y[i] = pSrc->dmvaAppIMDZone4_y[i];
	}

	memcpy(pDst->dmva_config.dmvaIMDZone_1_Label_sys,pSrc->dmvaIMDZone_1_Label_sys,MAX_DMVA_LABEL_STRING_LENGTH);
	memcpy(pDst->dmva_config.dmvaIMDZone_2_Label_sys,pSrc->dmvaIMDZone_2_Label_sys,MAX_DMVA_LABEL_STRING_LENGTH);
	memcpy(pDst->dmva_config.dmvaIMDZone_3_Label_sys,pSrc->dmvaIMDZone_3_Label_sys,MAX_DMVA_LABEL_STRING_LENGTH);
	memcpy(pDst->dmva_config.dmvaIMDZone_4_Label_sys,pSrc->dmvaIMDZone_4_Label_sys,MAX_DMVA_LABEL_STRING_LENGTH);

	return 0;
}

int CopyDmvaOCParams(SysInfo *pDst,Dmva_Config_Data *pSrc)
{
    pDst->dmva_config.dmvaAppOCSensitivity_sys  = pSrc->dmvaAppOCSensitivity_sys;
    pDst->dmva_config.dmvaAppOCObjectWidth_sys  = pSrc->dmvaAppOCObjectWidth_sys;
    pDst->dmva_config.dmvaAppOCObjectHeight_sys = pSrc->dmvaAppOCObjectHeight_sys;
    pDst->dmva_config.dmvaAppOCDirection_sys    = pSrc->dmvaAppOCDirection_sys;
    pDst->dmva_config.dmvaAppOCEnable_sys       = pSrc->dmvaAppOCEnable_sys;
    pDst->dmva_config.dmvaAppOCLeftRight_sys    = pSrc->dmvaAppOCLeftRight_sys;
    pDst->dmva_config.dmvaAppOCTopBottom_sys    = pSrc->dmvaAppOCTopBottom_sys;

    return 0;
}

int CopyDmvaSMETAParams(SysInfo *pDst,Dmva_Config_Data *pSrc)
{
    int i;

    pDst->dmva_config.dmvaAppSMETASensitivity_sys         = pSrc->dmvaAppSMETASensitivity_sys;
    pDst->dmva_config.dmvaAppSMETAEnableStreamingData_sys = pSrc->dmvaAppSMETAEnableStreamingData_sys;
    pDst->dmva_config.dmvaAppSMETAStreamBB_sys            = pSrc->dmvaAppSMETAStreamBB_sys;
    pDst->dmva_config.dmvaAppSMETAStreamVelocity_sys      = pSrc->dmvaAppSMETAStreamVelocity_sys;
    pDst->dmva_config.dmvaAppSMETAStreamHistogram_sys     = pSrc->dmvaAppSMETAStreamHistogram_sys;
    pDst->dmva_config.dmvaAppSMETAStreamMoments_sys       = pSrc->dmvaAppSMETAStreamMoments_sys;
    pDst->dmva_config.dmvaAppSMETAPresentAdjust_sys       = pSrc->dmvaAppSMETAPresentAdjust_sys;
    pDst->dmva_config.dmvaAppSMETAPersonMinWidth_sys      = pSrc->dmvaAppSMETAPersonMinWidth_sys;
    pDst->dmva_config.dmvaAppSMETAPersonMinHeight_sys     = pSrc->dmvaAppSMETAPersonMinHeight_sys;
    pDst->dmva_config.dmvaAppSMETAPersonMaxWidth_sys      = pSrc->dmvaAppSMETAPersonMaxWidth_sys;
    pDst->dmva_config.dmvaAppSMETAPersonMaxHeight_sys     = pSrc->dmvaAppSMETAPersonMaxHeight_sys;
    pDst->dmva_config.dmvaAppSMETAVehicleMinWidth_sys     = pSrc->dmvaAppSMETAVehicleMinWidth_sys;
    pDst->dmva_config.dmvaAppSMETAVehicleMinHeight_sys    = pSrc->dmvaAppSMETAVehicleMinHeight_sys;
    pDst->dmva_config.dmvaAppSMETAVehicleMaxWidth_sys     = pSrc->dmvaAppSMETAVehicleMaxWidth_sys;
    pDst->dmva_config.dmvaAppSMETAVehicleMaxHeight_sys    = pSrc->dmvaAppSMETAVehicleMaxHeight_sys;
    pDst->dmva_config.dmvaAppSMETANumZones_sys            = pSrc->dmvaAppSMETANumZones_sys;
    pDst->dmva_config.dmvaAppSMETAZone1_ROI_numSides_sys  = pSrc->dmvaAppSMETAZone1_ROI_numSides_sys;

    for(i = 0;i < 17;i ++)
    {
        pDst->dmva_config.dmvaAppSMETAZone1_x[i] = pSrc->dmvaAppSMETAZone1_x[i];
        pDst->dmva_config.dmvaAppSMETAZone1_y[i] = pSrc->dmvaAppSMETAZone1_y[i];
	}
	memcpy(pDst->dmva_config.dmvaSMETAZone_1_Label_sys,pSrc->dmvaSMETAZone_1_Label_sys,MAX_DMVA_LABEL_STRING_LENGTH);

    return 0;
}

/**
 * @brief	write SysInfo to system file
 * @param	"void *Buffer" : [IN]buffer of SysInfo
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int WriteGlobal(void *Buffer)
{
	int ret;
	ret = create_log_file(LOG_FILE, gLogHead);
	ret |= create_dmva_log_file(DMVA_LOG_FILE, gDmvaLogHead);
	ret |= create_sys_file(SYS_FILE, Buffer);
    usleep(500*1000);
	return ret | system("sync");
}

int WriteGlobalLog(void *Buffer)
{
    return 	create_log_file(LOG_FILE, gLogHead);
}

int WriteGlobalDmvaLog(void *Buffer)
{
    return 	create_dmva_log_file(DMVA_LOG_FILE, gDmvaLogHead);
}

int WriteGlobalSys(void *Buffer)
{
    return 	create_sys_file(SYS_FILE, Buffer);
}

int GetWriteIndexDmvaSaveLoad(Dmva_save_load_rules_t *pSaveLoad,char *ruleName)
{
    int i;

    for(i = 0;i < pSaveLoad->numRules;i ++)
    {
	    if(strcmp(ruleName,pSaveLoad->ruleName[i]) == 0)
	    {
			printf("cur rule no = %d and cur rule name = %s\n",i + 1,pSaveLoad->ruleName[i]);
		    return i;
		}
	}

    // New rule is added
    pSaveLoad->curRuleNo ++;

    if(pSaveLoad->numRules < MAX_DMVA_SAVELOAD_SETS)
        pSaveLoad->numRules  ++;

    if(pSaveLoad->curRuleNo > MAX_DMVA_SAVELOAD_SETS)
	    pSaveLoad->curRuleNo = 1;


    strcpy(pSaveLoad->ruleName[pSaveLoad->curRuleNo - 1],ruleName);

    printf("cur rule no = %d and cur rule name = %s\n",pSaveLoad->curRuleNo,pSaveLoad->ruleName[pSaveLoad->curRuleNo - 1]);

	return (pSaveLoad->curRuleNo - 1);
}

// AYK - 0310
int WriteGlobalDmvaTZ(void *Buffer,int setNo)
{
    int ret;
    printf("saving TZ rule no = %d\n",setNo);
    ret = create_dmva_sys_file(SYS_DMVATZ_FILE,Buffer,setNo);
    return ret;
}

int WriteGlobalDmvaIMD(void *Buffer,int setNo)
{
    int ret;
    printf("saving IMD rule no = %d\n",setNo);
    ret = create_dmva_sys_file(SYS_DMVAIMD_FILE,Buffer,setNo);
    return ret;
}

int WriteGlobalDmvaOC(void *Buffer,int setNo)
{
    int ret;
    printf("saving OC rule no = %d\n",setNo);
    ret = create_dmva_sys_file(SYS_DMVAOC_FILE,Buffer,setNo);
    return ret;
}

int WriteGlobalDmvaSMETA(void *Buffer,int setNo)
{
    int ret;
    printf("saving SMETA rule no = %d\n",setNo);
    ret = create_dmva_sys_file(SYS_DMVASMETA_FILE,Buffer,setNo);
    return ret;
}


/**
 * @brief	file manager initialization
 * @param	ShareMem [O ]Pointer to share memory where system information will
 be stored.
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngInit(void *ShareMem)
{
	int ret;
	SysInfo *pSysInfo = ShareMem;
	DBG("Global value size:%d\n", SYS_ENV_SIZE);

	// Allocate log and Dmva log buffers
	gLogBuffer     = (int)malloc(MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * LOG_ENTRY_SIZE);
	if(gLogBuffer == NULL)
	{
	    ERROR("\n*** cannot allocate %d memory\n",MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * LOG_ENTRY_SIZE);
	}

	gDmvaLogBuffer = (int)malloc(MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * DMVA_LOG_ENTRY_SIZE);
	if(gDmvaLogBuffer == NULL)
	{
	    ERROR("\n*** cannot allocate %d memory\n",MAX_LOG_PAGE_NUM * (NUM_LOG_PER_PAGE + 1) * DMVA_LOG_ENTRY_SIZE);
	}

	ret = ReadGlobal(ShareMem);
	if(ret == FAIL){
		printf("Initialize IPNC Default Values\n");

		memset(&SysInfoDefault.dmva_config.dmvaScheduleMgr,0x00,sizeof(DMVA_scheduleMgr_t));
		ret = create_sys_file(SYS_FILE, &SysInfoDefault);
		if(ret == SUCCESS)
			memcpy(ShareMem, &SysInfoDefault, SYS_ENV_SIZE);
		else
			ERROR("Initialize fail (FileMgnInit)\n");
	}
	if(ret == SUCCESS){

		// Set alarmStatus to 0
		pSysInfo->lan_config.alarmstatus  = 0;
		//pSysInfo->sdcard_config.schAviRun = 0;

		ret = ReadLog();
		if(ret == FAIL)
		{
			ret = create_log_file(LOG_FILE, NULL);
			if(ret == FAIL)
			    ERROR("create_log_file failed\n");
		}

		ret = ReadDmvaLog();
		if(ret == FAIL)
		{
			ret = create_dmva_log_file(DMVA_LOG_FILE, NULL);
			if(ret == FAIL)
			    ERROR("create_dmva_log_file failed\n");
		}

#ifdef __FILE_DEBUG
		else
			ShowAllLog();
#endif
	}

	return ret;
}

/**
 * @brief	file manager exit
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngExit()
{
	CleanLog();
	CleanDmvaLog();

	// Free the log and Dmva log buffers
	free(gLogBuffer);
	free(gDmvaLogBuffer);

	return 0;
}

/**
 * @brief	file manager reset
 * @param	"void *ShareMem" : [IN]pointer to share memory
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int FileMngReset(void *ShareMem)
{
	int ret;
	memset(&SysInfoDefault.dmva_config.dmvaScheduleMgr,0x00,sizeof(DMVA_scheduleMgr_t));
	ret = create_sys_file(SYS_FILE, &SysInfoDefault);
	if(ret == SUCCESS)
		memcpy(ShareMem, &SysInfoDefault, SYS_ENV_SIZE);
	return ret;
}

// AYK - 0420
/**
 * @brief	Dmva event delete
 * @param	"int eventIndex" : [IN]Index for the Dmva evevnt to be deleted
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
int DmvaEventDelete(int eventIndex)
{
	int ret;
    int index = 1;
    DmvaLogData_t *pDmvaLog,*pPrevDmvaLog;
    char cmd[128];

    // Start from the head
    pDmvaLog = gDmvaLogHead;

    // Search the evevnt with the index evevntIndex
    while((index < eventIndex) && (pDmvaLog != NULL))
    {
		pPrevDmvaLog = pDmvaLog;
        pDmvaLog     = pDmvaLog->pNext;
        index ++;
	}

	if(pDmvaLog == NULL)
	{
	    ERROR("Dmva event with the index %d is not found\n",eventIndex);
	    return FAIL;
	}

	if(index == 1)
	{
	    // evevnt is at the head of the list
	    gDmvaLogHead = pDmvaLog->pNext;
	    fprintf(stderr,"deleting dmva event at the head of list ...\n");
	    fprintf(stderr,"changing the head to %X\n",gDmvaLogHead);
	}
	else
	{
        pPrevDmvaLog->pNext = pDmvaLog->pNext;
	}

    fprintf(stderr,"deleting %s file\n",pDmvaLog->tDmvaLogData.eventListFileName);

	// delete the file from the SD card
	sprintf(cmd,"rm -f /mnt/mmc/ipnc/%s\n",pDmvaLog->tDmvaLogData.eventListFileName);

	system(cmd);

    fprintf(stderr,"freeing memory at 0x%X\n",pDmvaLog);

	// free the dmva evevnt
	free(pDmvaLog);

	// write back in the nand
	return (create_dmva_log_file(DMVA_LOG_FILE, gDmvaLogHead));
}

/**
 * @brief	Dmva event delete from start index to end index
 * @param	"int startIndex" : [IN]start index for the Dmva evevnt to be deleted
 * @param	"int endIndex" : [IN]end index for the Dmva evevnt to be deleted
 * @return	error code : SUCCESS(0) or FAIL(-1)
 */
 int DmvaEventDeleteStartEnd(int startIndex,int endIndex)
 {
    int index = 1,tempIndex,numDeleteEvents,i;
    DmvaLogData_t *pDmvaLog,*pPrevDmvaLog,*pNextDmvaLog;
    char cmd[128];

    pPrevDmvaLog = pNextDmvaLog = NULL;

    // start from the head
    pDmvaLog = gDmvaLogHead;

    if(startIndex > endIndex)
    {
	    tempIndex  = startIndex;
	    startIndex = endIndex;
	    endIndex   = tempIndex;
	}

    // Find the start index
    while((index < startIndex) && (pDmvaLog != NULL))
    {
		pPrevDmvaLog = pDmvaLog;
        pDmvaLog     = pDmvaLog->pNext;
        index ++;
	}

	if(pDmvaLog == NULL)
	{
	    ERROR("Dmva event with the start index %d is not found\n",startIndex);
	    return FAIL;
	}

	numDeleteEvents = (endIndex - startIndex) + 1;

	for(i = 0;i < numDeleteEvents;i ++)
	{
		if(pDmvaLog == NULL)
		{
		    break;
		}

		pNextDmvaLog = pDmvaLog->pNext;

		fprintf(stderr,"%d:deleting %s file\n",startIndex + i,pDmvaLog->tDmvaLogData.eventListFileName);

		// delete the file from the SD card
		sprintf(cmd,"rm -f /mnt/mmc/ipnc/%s\n",pDmvaLog->tDmvaLogData.eventListFileName);

		system(cmd);

        fprintf(stderr,"freeing memory at 0x%X\n",pDmvaLog);

		// free the dmva evevnt
		free(pDmvaLog);

		pDmvaLog = pNextDmvaLog;
	}

	if(index == 1)
	{
	    // evevnt is at the head of the list
	    gDmvaLogHead = pNextDmvaLog;
	    fprintf(stderr,"deleting dmva event at the head of list ...\n");
	    fprintf(stderr,"changing the head to %X\n",gDmvaLogHead);
	}
    else
    {
	    pPrevDmvaLog->pNext = pNextDmvaLog;
	}

	// write back in the nand
	return (create_dmva_log_file(DMVA_LOG_FILE, gDmvaLogHead));
 }

 /**
  * @brief	Dmva event delete all
  * @return	error code : SUCCESS(0) or FAIL(-1)
  */
int DmvaEventDeleteAll()
{
	DmvaLogData_t *pDmvaLog,*pNextDmvaLog;
    char cmd[128];

    // start from the head
    pDmvaLog = gDmvaLogHead;

    if(pDmvaLog == NULL)
    {
        return SUCCESS;
	}

    while(pDmvaLog != NULL)
    {
		// delete the file from the SD card
		sprintf(cmd,"rm -f /mnt/mmc/ipnc/%s\n",pDmvaLog->tDmvaLogData.eventListFileName);

		system(cmd);

        pNextDmvaLog = pDmvaLog->pNext;

		// free the dmva event
		free(pDmvaLog);

        // goto next event
		pDmvaLog = pNextDmvaLog;
	}

	gDmvaLogHead = NULL;

	fprintf(stderr,"DMVA event deletion DONE ...\n");

	// write back in the nand
	return (create_dmva_log_file(DMVA_LOG_FILE, gDmvaLogHead));
}


/* DMVA event search functions */

#define SECONDS_IN_DAY        (24 * 60 * 60)
unsigned int gBaseYear;

/* check if the year is leap year or not */
int CheckLeapYear(unsigned int year)
{
	if(((year % 400) == 0) ||
	   (((year % 4) == 0) && ((year % 100) != 0)))
	{
	    // Leap Year
	    return 1;
	}
	else
	{
	    // Not leap year
	    return 0;
	}
}

/* Get the time stamp in seconds from the month,day and time */
unsigned int GetDmvaEventTimeStamp(unsigned int year,
                                   unsigned int month,
                                   unsigned int day,
                                   unsigned int time,
                                   int *pTimeFlag)
{
    int offsetYear;
    unsigned int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned int numDays = 0,i;
    unsigned int totalSeconds;

    // Check if the day is greater than the no of days in the month
    if(day > daysInMonth[month])
    {
	    day = daysInMonth[month];
	}

    for(i = 0;i < (month - 1);i ++)
    {
	    numDays += daysInMonth[i];

	    if(i == 1)
	    {
			// FEB
			if(CheckLeapYear(year))
			{
			   // Leap Year
			   numDays ++;
			}
		}
	}

    numDays += (day - 1);

    if(*pTimeFlag == 1)
    {
        totalSeconds = (numDays * SECONDS_IN_DAY) + (time * 60);
    }
    else
    {
        totalSeconds = (numDays * SECONDS_IN_DAY) + time;
	}

	offsetYear = year - gBaseYear;

	if((offsetYear != 0) && (offsetYear != 1))
	{
	    printf("cannot search across more than 2 year range\n");
	    *pTimeFlag = -1;
	    return 0;
	}

#ifdef SEARCH_DBG
	printf("offset year = %d\n",offsetYear);
#endif

	for(i = 0;i < offsetYear;i++)
	{
        if(CheckLeapYear(gBaseYear + i))
        {
            totalSeconds += (366 * SECONDS_IN_DAY);
	    }
	    else
	    {
            totalSeconds += (365 * SECONDS_IN_DAY);
		}
	}

    *pTimeFlag = 0;
    return totalSeconds;
}

/*
 * time stamp string format is YYYY-MM-DD HH:MM:SS
 */

/* Get the year from the time stamp string */
unsigned int GetDmvaEventYear(char *timeStampString)
{
	char yearStr[5];
    strncpy(yearStr,timeStampString,4);
    yearStr[4] = '\0';
    return (atoi(yearStr));
}

/* Get the month from the time stamp string */
unsigned int GetDmvaEventMonth(char *timeStampString)
{
    char monthStr[3];
    strncpy(monthStr,timeStampString + 5,2);
    monthStr[2] = '\0';
    return (atoi(monthStr));
}

/* Get the day from the time stamp string */
unsigned int GetDmvaEventDay(char *timeStampString)
{
    char dayStr[3];
    strncpy(dayStr,timeStampString + 8,2);
    dayStr[2] = '\0';
    return (atoi(dayStr));
}

/* Get the time in sec from the time stamp string */
unsigned int GetDmvaEventTime(char *timeStampString)
{
    char hourStr[3],minStr[3],secStr[3];
    strncpy(hourStr,timeStampString + 11,2);
    strncpy(minStr,timeStampString + 14,2);
    strncpy(secStr,timeStampString + 17,2);
    hourStr[2] = '\0';
    minStr[2]  = '\0';
    secStr[2]  = '\0';
    return ((atoi(hourStr) * 60 * 60) + (atoi(minStr) * 60) + atoi(secStr));
}

/* Search for the start and end indices */
int SearchStartEndDmvaEvents(SysInfo *pSysInfo)
{
    // start from the head of the DMVA event list
    DmvaLogData_t *pDmvaLog;
    unsigned int eventIndex = 0;
    unsigned int startTimeStamp,endTimeStamp,eventTimeStamp,temp; // in seconds
    unsigned int curYear,startYear,endYear,eventYear;
    int timeFlag;

    pDmvaLog = gDmvaLogHead;

	pSysInfo->dmva_config.dmvaEventSearchListStartIndex = 0;
	pSysInfo->dmva_config.dmvaEventSearchListEndIndex   = 0;

    if((pDmvaLog == NULL) ||
       (pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys == 0) ||
       (pSysInfo->dmva_config.dmvaAppEventListStartDay_sys   == 0) ||
       (pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys   == 0) ||
       (pSysInfo->dmva_config.dmvaAppEventListEndDay_sys     == 0) ||
       ((pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys & 0x2) == 0))
    {
		// There are no DMVA events/start/end time is invalid
		printf("search parameters are incorrect\n");
		pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
        return SUCCESS;
	}

    curYear = GetDmvaEventYear(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys);
	startYear = endYear = curYear;
	if(pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys > pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys)
	{
        startYear --;
	}

	gBaseYear = startYear;

#ifdef SEARCH_DBG
    printf("start year = %u,end year = %u\n",startYear,endYear);
#endif

    timeFlag = 1;
    startTimeStamp = GetDmvaEventTimeStamp(startYear,
		                                   pSysInfo->dmva_config.dmvaAppEventListStartMonth_sys,
                                           pSysInfo->dmva_config.dmvaAppEventListStartDay_sys,
                                           pSysInfo->dmva_config.dmvaAppEventListStartTime_sys,&timeFlag);

    timeFlag = 1;
    endTimeStamp   = GetDmvaEventTimeStamp(endYear,
		                                   pSysInfo->dmva_config.dmvaAppEventListEndMonth_sys,
                                           pSysInfo->dmva_config.dmvaAppEventListEndDay_sys,
                                           pSysInfo->dmva_config.dmvaAppEventListEndTime_sys,&timeFlag);

	if(startTimeStamp > endTimeStamp)
	{
		// swap the start and end time
        temp = startTimeStamp;
		startTimeStamp = endTimeStamp;
		endTimeStamp = startTimeStamp;
	}

#ifdef SEARCH_DBG
    printf("startTS = %u sec,endTS = %u sec\n",startTimeStamp,endTimeStamp);
#endif

    // Find the start index
    while(pDmvaLog != NULL)
    {
		eventIndex ++;

        timeFlag = 0;
        eventYear = GetDmvaEventYear(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys);
        if(eventYear > curYear)
        {
		    printf("DMVA events are not in ascending order\n");
		    pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
		    return SUCCESS;
		}
		eventTimeStamp = GetDmvaEventTimeStamp(eventYear,
			                                   GetDmvaEventMonth(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),
		                                       GetDmvaEventDay(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),
		                                       GetDmvaEventTime(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),&timeFlag);

        if(timeFlag == -1)
        {
		    pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
		    return SUCCESS;
		}

#ifdef SEARCH_DBG
        printf("event TS = %u sec\n",eventTimeStamp);
#endif

        if((eventTimeStamp <= endTimeStamp) &&
		   (eventTimeStamp >= startTimeStamp))
        {
            pSysInfo->dmva_config.dmvaEventSearchListStartIndex = eventIndex;
			// goto next event
			pDmvaLog = pDmvaLog->pNext;
            printf("start index = %u\n",pSysInfo->dmva_config.dmvaEventSearchListStartIndex);
            break;
		}

		// goto next event
		pDmvaLog = pDmvaLog->pNext;
    }

    // Find the end index
    while(pDmvaLog != NULL)
    {
		// When we are here,we have already found the start index
		eventIndex ++;

        timeFlag = 0;
        eventYear = GetDmvaEventYear(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys);
        if(eventYear > curYear)
        {
		    printf("DMVA events are not in ascending order\n");
		    pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
		    return SUCCESS;
		}
		eventTimeStamp = GetDmvaEventTimeStamp(eventYear,
			                                   GetDmvaEventMonth(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),
		                                       GetDmvaEventDay(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),
		                                       GetDmvaEventTime(pDmvaLog->tDmvaLogData.eventListTimeStamp_sys),&timeFlag);

        if(timeFlag == -1)
        {
		    pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
		    return SUCCESS;
		}

#ifdef SEARCH_DBG
        printf("event TS = %u sec\n",eventTimeStamp);
#endif

        if(eventTimeStamp <= startTimeStamp)
        {
            pSysInfo->dmva_config.dmvaEventSearchListEndIndex = eventIndex;
            if(eventTimeStamp != startTimeStamp)
                pSysInfo->dmva_config.dmvaEventSearchListEndIndex = eventIndex - 1;
            printf("end index = %u\n",pSysInfo->dmva_config.dmvaEventSearchListEndIndex);
            return SUCCESS;
		}

		// goto next event
		pDmvaLog = pDmvaLog->pNext;
	}

    if(pSysInfo->dmva_config.dmvaEventSearchListStartIndex != 0)
    {
        pSysInfo->dmva_config.dmvaEventSearchListEndIndex = 0x7FFFFFFF;
        printf("end index = %u\n",pSysInfo->dmva_config.dmvaEventSearchListEndIndex);
	}
	else
	{
		// We couldn't search the dmva events between start and end TS
		// so lets turn OFF the search button - clear bit 1
	    pSysInfo->dmva_config.dmvaAppEventListActionPlaySendSearchTrash_sys &= ~0x2;
	}

    return SUCCESS;
}

/* clear system and access logs */
int ClearSystemLog()
{
    LogData_t *pLog = gLogHead;
    LogData_t *pNextLog,*pPrevLog = NULL;
    int logListCount = 0;

    while(pLog != NULL)
    {
		if(strstr(pLog->tLogData.event,"login") != NULL)
		{
			if(logListCount == 0)
			    gLogHead = pLog;

            pNextLog    = pLog->pNext;
            pLog->pNext = NULL;
            if(pPrevLog != NULL)
            {
			    pPrevLog->pNext = pLog;
			}
            pPrevLog = pLog;

			pLog = pNextLog; // goto next entry
			logListCount ++;
		    continue;
		}

	    pNextLog = pLog->pNext;
	    free(pLog);
	    pLog = pNextLog;
	}

	if(logListCount == 0)
	    gLogHead = NULL;

    return 0;
}

int ClearAccessLog()
{
    LogData_t *pLog = gLogHead;
    LogData_t *pNextLog,*pPrevLog = NULL;
    int logListCount = 0;

    while(pLog != NULL)
    {
		if(strstr(pLog->tLogData.event,"login") == NULL)
		{
			if(logListCount == 0)
			    gLogHead = pLog;

            pNextLog    = pLog->pNext;
            pLog->pNext = NULL;
            if(pPrevLog != NULL)
            {
			    pPrevLog->pNext = pLog;
			}
            pPrevLog = pLog;

			pLog = pNextLog; // goto next entry
			logListCount ++;
		    continue;
		}

	    pNextLog = pLog->pNext;
	    free(pLog);
	    pLog = pNextLog;
	}

	if(logListCount == 0)
	    gLogHead = NULL;

    return 0;
}
