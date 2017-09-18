/* ===========================================================================
* @file avi_mng.c
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
#include <avi_mng.h>
#include <file_msg_drv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys_env_type.h>
#include <pthread.h>
#include <unistd.h>
#include <file_dispatcher.h>
#include <sem_util.h>
#include <system_default.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <strings.h>
#include <dirent.h>
#include <stream_env_ctrl.h>
#include <Frame_rate_ctrl.h>

#ifdef AVI_DEBUG
#define AVI_DBG(fmt, args...)	fprintf(stderr, fmt, ##args)
#else
#define AVI_DBG(fmt, args...)
#endif
#define AVI_ERR(fmt, args...)	fprintf(stderr, fmt, ##args)

extern volatile SemHandl_t hAlarmStatusSem;
extern int gFileDispCount;
extern int GetfreememInfo( void );
extern int GetDmvaEvtType();

/* extern from av_server_ctrl.c */
extern const int audio_bitrate_aac[2][3];
extern const int audio_bitrate_g711[2][1];

static pthread_mutex_t gAviMutex, gAviConMutex;
static pthread_cond_t gAviRunCond = PTHREAD_COND_INITIALIZER;
static pthread_t gAviMngThr;
static volatile int gAviState = 0;
static SysInfo* pSysinfo;
static volatile char pAviRun = 0;
char gSdFileName[30] = "";
SemHandl_t hSdFileSem = NULL;

int UpdateSdFileName(char *pFileName)
{
#ifdef AVI_DEBUG
	int ret =
#endif
	SemWait(hSdFileSem);
#ifdef AVI_DEBUG
	if(ret){
		AVI_DBG("Error at %s, line %d\n", __func__, __LINE__);
		return -1;
	}
#endif
	strcpy(gSdFileName, pFileName);
#ifdef AVI_DEBUG
	ret =
#endif
	SemRelease(hSdFileSem);
#ifdef AVI_DEBUG
	if(ret){
		AVI_DBG("Error at %s, line %d\n", __func__, __LINE__);
		return -1;
	}
#endif
	return 0;
}

int ClearSdFileName()
{
#ifdef AVI_DEBUG
	int ret =
#endif
	SemWait(hSdFileSem);
#ifdef AVI_DEBUG
	if(ret){
		AVI_DBG("Error at %s, line %d\n", __func__, __LINE__);
		return -1;
	}
#endif
	gSdFileName[0] = '\0';
#ifdef AVI_DEBUG
	ret =
#endif
	SemRelease(hSdFileSem);
#ifdef AVI_DEBUG
	if(ret){
		AVI_DBG("Error at %s, line %d\n", __func__, __LINE__);
		return -1;
	}
#endif
	return 0;
}

/**
* @brief Number of calling AviRun().

* This number only be used in no stop mode.
*/
static int gAviRunNum = 0;
/**
* @brief Get current AVI manager status.
* This function will get AVI status and clear flag if needed.
* @param fClrFlg [I ] Bits to clear.
* @return AVI status.
*/
int GetAviState(int fClrFlg)
{
	int ret;
	pthread_mutex_lock(&gAviMutex);
	ret = gAviState;
	if(fClrFlg)
		gAviState &= (~fClrFlg);
	pthread_mutex_unlock(&gAviMutex);
	return ret;
}
/**
* @brief Save an AVI file.
* @param sAviName [I ] AVI file name(with path).
* @param nInterval [I ] How long a file is.
* @param nFrameRate [I ] AVI frame rate.
* @param nAudioCodec [I ] Select audio codec(0: no audio. 1: u-law. 2. AAC.).
* @param nMsgId [I ] Message ID to communicate with encode stream.
* @param bIsChkCard[I ] Tell Appro_avi_save to check card left space or not.
* @param nStreamId[I ] Which stream to save. 0: stream2. 1: stream3.
* @param nAudioBitrate[I] Audio bitrate which AV server used.
* @param pActualRun[O] How long does Appro_avi_save actually run.
* @return 0 on success. AVI_RUN_AGAIN means File size is reached 1GB.
			Otherwise meets fail.
*/
static int Appro_avi_save(char *sAviName, int nInterval, int nFrameRate,
						int nAudioCodec, int nMsgId, int bIsChkCard,
						int nStreamId, int nAudioBitrate, int* pActualRun)
{
	char cmd[80];
	int ret;
	struct timeval first,  second,  lapsed;
	sprintf(cmd, "./Appro_avi_save %s %d %d %d %d %d %d %d\n",
			sAviName, nInterval, nMsgId, nFrameRate, nAudioCodec, bIsChkCard,
			nStreamId, nAudioBitrate);
	AVI_DBG("%s\n",cmd);
	gettimeofday (&first, NULL);
	ret = system(cmd);
	gettimeofday (&second, NULL);
	if (first.tv_usec > second.tv_usec) {
		second.tv_usec += 1000000;
		second.tv_sec--;
	}
	lapsed.tv_usec = second.tv_usec - first.tv_usec;
	lapsed.tv_sec = second.tv_sec - first.tv_sec;
	AVI_DBG("lapsed time %d.%d (second)\n", (int)lapsed.tv_sec, (int)lapsed.tv_usec);
	if(ret != AVI_RUN_AGAIN){
		if(ret){
			/* remove error file*/
			sprintf(cmd, "rm -f %s\n", sAviName);
			AVI_DBG("%s\n",cmd);
			system(cmd);
		} else if(lapsed.tv_sec < 3){
			/* remove small file */
			sprintf(cmd, "rm -f %s\n", sAviName);
			AVI_DBG("%s\n",cmd);
			system(cmd);
			ret = -1;
		}
	}
	*pActualRun = lapsed.tv_sec;
	AVI_DBG("Return %d\n", ret);
	return ret;
}
/**
* @brief Generate a MPEG4 file name
* @param strFileName [O ] MPEG4 file name.
* @return Length of file name(without path).
*/
int GenerateMpeg4FileName(int nCfgFlg,char *strFileName)
{
	time_t tCurrentTime;
	struct tm *tmnow;
	time(&tCurrentTime);
	tmnow   = localtime(&tCurrentTime);
	if(nCfgFlg & AVI_FOR_DMVA)
	{
		return sprintf(strFileName, "/DMVA_%04d%02d%02d%02d%02d%02d.avi",
			tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, tmnow->tm_hour,
			tmnow->tm_min, tmnow->tm_sec);	
	}
	else
	{
		return sprintf(strFileName, "/%04d%02d%02d%02d%02d%02d.avi",
			tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, tmnow->tm_hour,
			tmnow->tm_min, tmnow->tm_sec);
	}
}
/**
* @brief Generate AVI config file for file dispatch.
*
* @param pAviFile [I ] AVI file name.
* @param pConfigFileName [O ] Config file name.
* @return 0 on success.
*/
int GenerateAviConfigFile(char *pAviFile, char *pConfigFileName)
{
	int ret, len;
	FILE *fp;
	len = strlen(pAviFile);
	strcpy(pConfigFileName, pAviFile);
	strcpy(pConfigFileName + len - 3, "cfg");
	if((fp = fopen(pConfigFileName, "wt")) == NULL)
		ret = -1;
	else {
		fprintf(fp, "%d\n", 1); // no fo files to attach
		fprintf(fp, "%s\n", pAviFile);
		fclose(fp);
		ret = 0;
	}
	strcpy(pConfigFileName, pConfigFileName);
	return ret;
}

long long GetDiskfreeSpace(char *pDisk)
{
	long long freespace = 0;
	struct statfs disk_statfs;

	if( statfs(pDisk, &disk_statfs) >= 0 )
	{
		freespace = (((long long)disk_statfs.f_bsize  * (long long)disk_statfs.f_bfree)/(long long)1024);
	}
	/**
	fprintf(stderr,"GetDiskfreeSpace %lli \n",freespace);
	fprintf(stderr,"f_type: 0x%X\n", disk_statfs.f_type);
	fprintf(stderr,"f_bsize: %d\n", disk_statfs.f_bsize);
	fprintf(stderr,"f_blocks: %li\n", disk_statfs.f_blocks);
	fprintf(stderr,"f_bfree: %li\n", disk_statfs.f_bfree);
	fprintf(stderr,"f_bavail: %li\n", disk_statfs.f_bavail);
	fprintf(stderr,"f_files: %li\n", disk_statfs.f_files);
	fprintf(stderr,"f_ffree: %li\n", disk_statfs.f_ffree);
	*/
	return freespace;
}

/**
* @brief DMVA event log.
* @param pFileName [I ] Dmva Event file name.
*/
static void DmvaEventLog(char *pFileName)
{
    DmvaLogEntry_t dmvaLogEntry;
	char eventRecType[10],*charPtr;
	char timeStr[5];
	SysInfo *pSysInfo = GetSysInfo();

    strcpy(dmvaLogEntry.eventListFileName,pFileName);    
	
    // Get Year
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 5,4);
    timeStr[4] = '\0';
    dmvaLogEntry.time.tm_year = atoi(timeStr);

    // Get Month
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 9,2);
    timeStr[2] = '\0';
    dmvaLogEntry.time.tm_mon = atoi(timeStr);

    // Get Day
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 11,2);
    timeStr[2] = '\0';
    dmvaLogEntry.time.tm_mday = atoi(timeStr);

    // Get Hour
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 13,2);
    timeStr[2] = '\0';
    dmvaLogEntry.time.tm_hour = atoi(timeStr);

    // Get Minute
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 15,2);
    timeStr[2] = '\0';
    dmvaLogEntry.time.tm_min = atoi(timeStr);

    // Get Second
    strncpy(timeStr,dmvaLogEntry.eventListFileName + 17,2);
    timeStr[2] = '\0';
    dmvaLogEntry.time.tm_sec = atoi(timeStr);	
	
	sprintf(dmvaLogEntry.eventListTimeStamp_sys, "%d-%02d-%02d %02d:%02d:%02d",
			dmvaLogEntry.time.tm_year, dmvaLogEntry.time.tm_mon, dmvaLogEntry.time.tm_mday,
			dmvaLogEntry.time.tm_hour, dmvaLogEntry.time.tm_min, dmvaLogEntry.time.tm_sec);	
			
	dmvaLogEntry.eventListArchiveFlag_sys = 0x1;	
    dmvaLogEntry.eventListEventType_sys	  = GetDmvaEvtType();
	
	switch(dmvaLogEntry.eventListEventType_sys) 
	{
	    case 0:
		    sprintf(dmvaLogEntry.eventListEventDetails_sys, "IMD EVENT ");
			break;
		case 1:
			sprintf(dmvaLogEntry.eventListEventDetails_sys, "TRIP ZONE EVENT ");
			break;
		case 2:
			sprintf(dmvaLogEntry.eventListEventDetails_sys, "OBJECT COUNT EVENT ");
			break;
		case 3:
			sprintf(dmvaLogEntry.eventListEventDetails_sys, "TAMPER EVENT ");
			break;
		default:
			sprintf(dmvaLogEntry.eventListEventDetails_sys, "DMVA EVENT");
			break;
	};
	
    eventRecType[0] = '\0';
    charPtr         = eventRecType;

    if(pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys & 0x4)
    {
	    // audio is recorded
	    sprintf(charPtr,"AUD ");
		charPtr += 4;
	}

    if(pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys & 0x2)
    {
	    // meta data is recorded
	    sprintf(charPtr,"MET ");
	}

    if(strlen(eventRecType) > 0)
    {
	    strcat(dmvaLogEntry.eventListEventDetails_sys,eventRecType);
	}

	fSetDmvaSysLog(&dmvaLogEntry);	
}

/**
* @brief Create an AVI file and call file dispatcher.
* @param nDstFlg [I ] Where AVI will save.
* @return 0 on success.
*/
int AviSave(int nCfgFlg)
{
	char strFilename[80], strConfigFile[80];
	int nFrameRate, AVI_INTERVAL, nSetFlg = 0;
	int AVIType, i, j, nAudioCodec, nAudioBitrate;
	int nLastRunTime = 0, nTotalTime = 0;
	SysInfo *pSysInfo = GetSysInfo();
	if(pSysInfo == NULL)
		return -1;
	AVI_DBG("%s\n", __func__);
	if (GetfreememInfo() < 0)
			return 0;

	GenerateMpeg4FileName(nCfgFlg,strFilename + sprintf(strFilename, "%s", AVI_PATH));

	/* Get settings from UI. */
	switch (pSysInfo->lan_config.aviduration){
		case 0:
			AVI_INTERVAL=5;
			break;
		case 1:
			AVI_INTERVAL=10;
			break;
		default:
			AVI_INTERVAL=5;
			break;
	}

	AVIType=0;
	i = pSysInfo->lan_config.nVideocodecmode;
	j = pSysInfo->lan_config.nVideocodecres;

	if (pSysInfo->lan_config.aviformat == 0) {
		if(pSysInfo->lan_config.Supportstream5) {
		/* supportH264 */
			AVIType=3;
		}else if (pSysInfo->lan_config.Supportstream2) {
		/* supportMpeg4 */
			AVIType=0;
		}
		nFrameRate = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 1, pSysInfo->lan_config.nFrameRate1);
	}else {
		if(pSysInfo->lan_config.Supportstream6) {
		/* supportH264cif */
			AVIType=4;
		}else if(pSysInfo->lan_config.Supportstream3) {
		/* supportMpeg4cif */
			AVIType=1;
		}
		nFrameRate = frame_rate_control(pSysInfo->lan_config.nDayNight, i, j, 2, pSysInfo->lan_config.nFrameRate2);
	}
	/* Audio config */
	if(pSysInfo->audio_config.audioenable == 0){
		nAudioCodec = 0;
		nAudioBitrate = 0;
	}else if(pSysInfo->audio_config.codectype == 0){
		nAudioCodec = 1;
		nAudioBitrate = audio_bitrate_g711[pSysInfo->audio_config.samplerate][pSysInfo->audio_config.bitrate];
	}else{
		nAudioCodec = 2;
		nAudioBitrate = audio_bitrate_aac[pSysInfo->audio_config.samplerate][pSysInfo->audio_config.bitrate];
	}

	/* Record method here. */
	if(nCfgFlg & AVI_NO_STOP) {
		/* No stop case */
		if(nCfgFlg & TMP_IN_SD) {
			/* AVI file was store in SD card in X seconds uint. */
			AVI_DBG("Save %d seconds long AVI in SD card.\n", 60);
			i = j = 0;
			UpdateSdFileName(strFilename + strlen(AVI_PATH) + 1);
			sprintf(strFilename + sprintf(strFilename, "%s", SD_PATH), "/%s", gSdFileName);
			do {
				if(i == AVI_RUN_AGAIN) {
					nTotalTime += nLastRunTime;
					AVI_ERR("%s : AVI_RUN_AGAIN case(Your file size is \
							bigger than file size limit in Appro_avi_save).\n",
							__FILE__);
				} else if(i == 0) {
					nTotalTime = 0;
				}
				i = Appro_avi_save(strFilename, 60 - nTotalTime, nFrameRate,
								nAudioCodec, AVI_MSG_ID, 1,
								AVIType, nAudioBitrate, &nLastRunTime);
				if(i == 0 || i == AVI_RUN_AGAIN) {
					if(nCfgFlg & AVI_TO_SMTP)
						nSetFlg |= FILE_DISPATCH_TO_SMTP;
					if(nCfgFlg & AVI_TO_FTP)
						nSetFlg |= FILE_DISPATCH_TO_FTP;
					if(nSetFlg) {
						GenerateAviConfigFile(strFilename, strConfigFile);
						DoFileDispatch(strConfigFile, nSetFlg, 0);
					}
				}
				j = GetAviState(0) & AVI_NO_STOP;
				j |= (i == AVI_RUN_AGAIN);
				if(j) {
					GenerateMpeg4FileName(nCfgFlg,strFilename + sprintf(strFilename, SD_PATH));
					UpdateSdFileName(strFilename + strlen(SD_PATH) + 1);
				}
			} while(j);
			ClearSdFileName();
		} else {
			/* Because we can't send a non stop AVI to anywhere besides
			SD card, we save the AVI file to SD card directly. */
			UpdateSdFileName(strFilename + strlen(AVI_PATH) + 1);
			sprintf(strFilename + sprintf(strFilename, "%s", SD_PATH), "/%s", gSdFileName);
			
			if(nCfgFlg & AVI_FOR_DMVA)
			{				
				if(((pSysInfo->dmva_config.dmvaAppEventRecordingVAME_sys & 0x4) == 0) &&
				   (pSysInfo->audio_config.audioenable == 1))
				{
					nAudioCodec   = 0;
					nAudioBitrate = 0;
				}								   
			}
			
			while(Appro_avi_save(strFilename, 999999999, nFrameRate,
								nAudioCodec, AVI_MSG_ID, 1,
								AVIType, nAudioBitrate, &nLastRunTime) == AVI_RUN_AGAIN) {
				/* AVI file is bigger then 1GB, and there is space left on SD card.
				We should create a new file name to save another AVI file.
				*/
				GenerateMpeg4FileName(nCfgFlg,strFilename + sprintf(strFilename, SD_PATH));
				UpdateSdFileName(strFilename + strlen(SD_PATH) + 1);
			}
			
			if(nCfgFlg & AVI_FOR_DMVA)
			{
			    DmvaEventLog(gSdFileName);    
			}
			
			ClearSdFileName();
		}
	} else {
		if(gFileDispCount < FILE_DISP_THR_LIMIT) {
			if(nCfgFlg & TMP_IN_SD) {
				AVI_ERR("error: %s : Not support feature!!!\n", __FILE__);
			} else {
				/* The AVI file shoud not bigger than 1GB in this case */
				if(0 == Appro_avi_save(strFilename, AVI_INTERVAL, nFrameRate,
									nAudioCodec, AVI_MSG_ID,
									0, AVIType, nAudioBitrate,
									&nLastRunTime)) {
					GenerateAviConfigFile(strFilename, strConfigFile);
					if(nCfgFlg & AVI_TO_SD)
						nSetFlg |= FILE_DISPATCH_TO_SD;
					if(nCfgFlg & AVI_TO_SMTP)
						nSetFlg |= FILE_DISPATCH_TO_SMTP;
					if(nCfgFlg & AVI_TO_FTP)
						nSetFlg |= FILE_DISPATCH_TO_FTP;
					DoFileDispatch(strConfigFile, nSetFlg, 1);
				}
			}
		} else {
			AVI_ERR("Avi File dispatcher threads over limit %d\n", FILE_DISP_THR_LIMIT);
		}
	}
	return 0;
}
/**
* @brief AVI manager thread
* @param arg [I ] Not used.
* @return 0 on normal end.
*/
void *AviMngThr(void *arg)
{
	unsigned short nFlg;
	int nState;
	while(!(GetAviState(0) & AVI_THR_EXIT)){
		AVI_DBG("Ready\n");
		while(!((nState = GetAviState(AVI_RUN | AVI_CFG_MASK)) & (AVI_RUN | AVI_THR_EXIT))){
			/* Update alarm status to AVI stop in UI */
			if(SemWait(hAlarmStatusSem) == 0){
				nFlg = pSysinfo->lan_config.alarmstatus & (~FLG_UI_AVI);
				if(!(nFlg & (FLG_UI_AVI | FLG_UI_JPG)))
					nFlg &= (~FLG_UI_RECORD);
				fSetAlarmStatus(nFlg);
				SemRelease(hAlarmStatusSem);
			}
			AVI_DBG("Go to sleep\n");
			pthread_cond_wait(&gAviRunCond, &gAviConMutex);
			AVI_DBG("I Wake up\n");
		}
		if(AVI_THR_EXIT & nState)
			break;
		/* Update alarm status to AVI start in UI */
		if(SemWait(hAlarmStatusSem) == 0){
			nFlg = pSysinfo->lan_config.alarmstatus | FLG_UI_AVI |	FLG_UI_RECORD;
			fSetAlarmStatus(nFlg);
			SemRelease(hAlarmStatusSem);
		}
		AVI_DBG("Run\n");
		AviSave(nState & (AVI_CFG_MASK | AVI_NO_STOP));
	}
	AVI_DBG("Thread exit\n");
	return (void *)0;
}
/**
* @brief Initialize AVI manager.
* @retval 0 Success
* @retval -1 Fail
*/
int AviMngInit()
{
	if((pSysinfo = GetSysInfo()) == NULL){
		AVI_ERR("Can't get system info!!\n");
		return -1;
	}
	if( pthread_mutex_init(&gAviMutex, NULL) != 0 ){
		pSysinfo = NULL;
		AVI_ERR("Mutex create faill!!\n");
		return -1;
	}
	if( pthread_mutex_init(&gAviConMutex, NULL) != 0 ){
		pSysinfo = NULL;
		pthread_mutex_destroy(&gAviMutex);
		AVI_ERR("Mutex create faill!!\n");
		return -1;
	}
	if(hSdFileSem == NULL)
		hSdFileSem = MakeSem();
	if(hSdFileSem == NULL){
		pSysinfo = NULL;
		pthread_mutex_destroy(&gAviMutex);
		pthread_mutex_destroy(&gAviConMutex);
		AVI_ERR("hSdFileSem create fail!!\n");
		return -1;
	}
	if(pthread_create(&gAviMngThr, NULL, AviMngThr, NULL)){
		pSysinfo = NULL;
		pthread_mutex_destroy(&gAviMutex);
		pthread_mutex_destroy(&gAviConMutex);
		DestroySem(hSdFileSem);
		hSdFileSem = NULL;
		AVI_ERR("AviMngThr create faill!!\n");
		return -1;
	}
	gAviState = AVI_INT_STATE;
	return 0;
}
/**
* @brief Make AVI manager leave.
* @retval 0 Success
* @retval -1 Fail
*/
int AviMngExit()
{
	int ret = 0;
	AVI_DBG("Enter %s\n", __func__);
	pSysinfo = NULL;
	pthread_mutex_lock(&gAviMutex);
	gAviState |= AVI_THR_EXIT;
	pthread_mutex_unlock(&gAviMutex);
	pthread_cond_signal(&gAviRunCond);
	AVI_DBG("Wait AVI manager done\n");
	pthread_join(gAviMngThr, (void **)&ret);
	pthread_detach(gAviMngThr);
	ret |= pthread_mutex_destroy(&gAviConMutex);
	ret |= pthread_mutex_destroy(&gAviMutex);
	if(DestroySem(hSdFileSem))
		ret |= -1;
	else
		hSdFileSem = NULL;
	AVI_DBG("Leave %s\n", __func__);
	return ret;
}
/**
* @brief Tell AVI manager to start record a AVI.

* This function will make AVI manager to start recording if there is no AVI
* recording. If there is one AVI recording, AVI manager will start another AVI
* recording when it have done current recording.
* @param nMode [I] Which mode to run.
* @param nDstFlg [I ] AVI file destination flag.
* @retval 0 Command sent successfully.
* @retval -1 Failed to send command.
*/
int AviRun(int nMode, int nDstFlg)
{
	int ret = 0, nRunNum;
	nDstFlg &= (AVI_DST_MASK | AVI_FOR_DMVA);
	AVI_DBG("Enter %s: mode %d, Flag : 0x%x\n", __func__, nMode, nDstFlg);
	pthread_mutex_lock(&gAviMutex);
	switch(nMode) {
		case AVI_MODE1:
			gAviState |= (AVI_RUN | nDstFlg);
			ret = pthread_cond_signal(&gAviRunCond);
			break;
		case AVI_MODE2:
			nRunNum = gAviRunNum++;
			if(nRunNum == 0) {
				gAviState |= (AVI_RUN | AVI_NO_STOP);
				if(nDstFlg & AVI_FOR_DMVA)
				    gAviState |= AVI_FOR_DMVA;    
				ret = pthread_cond_signal(&gAviRunCond);
			}
			break;
		case AVI_MODE3:
			nRunNum = gAviRunNum++;
			if(nRunNum == 0) {
				gAviState |= (AVI_RUN | TMP_IN_SD | AVI_NO_STOP | nDstFlg);
				ret = pthread_cond_signal(&gAviRunCond);
			}
			break;
		default:
			ret = -1;
			break;
	}
	pthread_mutex_unlock(&gAviMutex);
	return ret;
}
/**
* @brief Send a stop command to AVI manager

* AVI recording will stop immediately in normal mode. In no stop mode, AVI
* recording will stop when gAviRunNum is zero.
* @retval 0 Command sent without error.
* @retval -1 Error occurs in AviRun and AviStop.
*/
int AviStop()
{
	int ret = 0, nRunNum, nState;
	nState = GetAviState(0);
	AVI_DBG("Enter %s\n", __func__);
	if(nState & AVI_NO_STOP){
		/* AVI no stop case */
		pthread_mutex_lock(&gAviMutex);
		nRunNum = (--gAviRunNum);
		if(nRunNum < 0){
			AVI_DBG("Error usage in AviRun and AviStop\n");
			/* Restore to default */
			gAviRunNum = 0;
			ret = -1;
		} else if(nRunNum == 0){
			ret = system("killall -2 Appro_avi_save");
			gAviState &= (~AVI_NO_STOP);
			if(ret)
				AVI_ERR("Failed to stop AVI recording\n");
		}
		pthread_mutex_unlock(&gAviMutex);
	} else {
		if(pSysinfo->lan_config.alarmstatus & FLG_UI_AVI){
			ret = system("killall -2 Appro_avi_save");
			if(ret)
				AVI_ERR("Failed to stop AVI recording\n");
		} else {
			ret = -1;
			AVI_DBG("No AVI recording now\n");
		}
		GetAviState(AVI_RUN);
	}
	return ret;
}

/**
* @brief If DMVA event rec is going ON then stop it

* AVI recording will stop immediately in normal mode. In no stop mode, AVI
* recording will stop when gAviRunNum is zero.
* @retval 0 Command sent without error.
* @retval -1 Error occurs in AviRun and AviStop.
*/
int StopDmvaEvtRec()
{
	SendDMVAAlarmEventStop();	
	return 0;
}

