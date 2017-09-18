/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Msg_Def.h
* @brief Definition of message command, message key, and message type.
*/
#ifndef __MSG_H__
#define __MSG_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys_env_type.h>
/* Codec server */
/**
* @defgroup UTILITY Utility API

* This contains kinds of API  for communicating.
* @{
*/

/** @} */
enum {
	MSG_CMD_START = 0,

	MSG_CMD_GET_MEM,
	MSG_CMD_QUIT,
	MSG_CMD_SET_MEM,
	MSG_CMD_PRINT_MEM,
	MSG_CMD_GET_CUR_FRAME,
	MSG_CMD_LOCK_FRAME,
	MSG_CMD_UNLOCK_FRAME,
	MSG_CMD_GET_MEDIA_INFO,
	MSG_CMD_GET_VOL,
	MSG_CMD_GET_NEW_FRAME,
	MSG_CMD_GET_AND_LOCK_IFRAME,
	MSG_CMD_SET_DAY_NIGHT,
	MSG_CMD_SET_WHITE_BALANCE,
	MSG_CMD_SET_BACKLIGHT,
	MSG_CMD_SET_BRIGHTNESS,
	MSG_CMD_SET_RESOLUTION,
	MSG_CMD_SET_FRAMERATE,
	MSG_CMD_SET_CONTRAST,
	MSG_CMD_SET_SATURATION,
	MSG_CMD_SET_SHARPNESS,
	MSG_CMD_SET_AAC_BITRATE,
	MSG_CMD_SET_BITRATE1,
	MSG_CMD_SET_BITRATE2,
	MSG_CMD_SET_JPEG_QUALITY,
	MSG_CMD_SET_FDETECT,
	MSG_CMD_SET_MIRROR,
	MSG_CMD_SET_OSDWINDOW,
	MSG_CMD_SET_FRAMERATE1,
	MSG_CMD_SET_FRAMERATE2,
	MSG_CMD_SET_FRAMERATE3,
	MSG_CMD_SWITCH_2A,
	MSG_CMD_VENDOR_2A,
	MSG_CMD_TYPE_2A,
	MSG_CMD_2A_PRIORITY,
	MSG_CMD_SET_IRIS,
	MSG_CMD_SET_FRAMECTRL,
	MSG_CMD_SET_TV_SYSTEM,
	MSG_CMD_SET_BINNING_SKIP,
	MSG_CMD_SET_TSTAMP,
	MSG_CMD_SET_DATETIMEPRM,
	MSG_CMD_PTZ,
	MSG_CMD_INIT_PTZ,
	MSG_CMD_SET_MOTION_BLOCK,
	MSG_CMD_SET_MOTION,
	MSG_CMD_SET_MOTION_CENABLE,
	MSG_CMD_SET_MOTION_LEVEL,
	MSG_CMD_SET_MOTION_CVALUE,
	MSG_CMD_SET_ROICFG,
	MSG_CMD_SET_OSDTEXT_EN,
	MSG_CMD_SET_HIST_EN,
	MSG_CMD_SET_OSDLOGO_EN,
	MSG_CMD_SET_OSDPRM1,
	MSG_CMD_SET_OSDPRM2,
	MSG_CMD_SET_OSDPRM3,
	MSG_CMD_SET_LOCALDISPLAY,
	MSG_CMD_SET_CLICKNAME,
	MSG_CMD_SET_CLICKSTORAGE,
	MSG_CMD_SET_ROIPRM1,
	MSG_CMD_SET_ROIPRM2,
	MSG_CMD_SET_ROIPRM3,
	MSG_CMD_SET_ADVCODECPRM1,
	MSG_CMD_SET_ADVCODECPRM2,
	MSG_CMD_SET_ADVCODECPRM3,
	MSG_CMD_MCFW_CREATE_PARAM,
	MSG_CMD_SET_OSDENABLE,
	MSG_CMD_SET_IFRAME,
	MSG_CMD_SET_LDC_ENABLE,
	MSG_CMD_SET_VNF_PARAM,
	MSG_CMD_SET_AUDIO_ALARM_LEVEL,
	MSG_CMD_SET_AUDIO_ALARM_FLAG,
	MSG_CMD_SET_DYNRANGE,
	MSG_CMD_SET_STREAMSTACK,
	MSG_CMD_STOPDEMO,
	MSG_CMD_POLLING,
	MSG_CMD_RATE_CONTROL1,
	MSG_CMD_RATE_CONTROL2,

	/* DMVA Specific */
	MSG_CMD_SET_DMVAENABLE,
    MSG_CMD_SET_DMVAALGOFRAMERATE,
    MSG_CMD_SET_DMVAALGODETECTFREQ,
    MSG_CMD_SET_DMVAEVTRECORDENABLE,
    MSG_CMD_SET_DMVASMETATRACKERENABLE,
	/* DMVA TZ Specific */
	MSG_CMD_SET_DMVA_TZ,
	/* DMVA MAIN Specific */
	MSG_CMD_SET_DMVA_MAIN,
	/* DMVA CTD Specific */
	MSG_CMD_SET_DMVA_CTD,
	/* DMVA OC Specific */
	MSG_CMD_SET_DMVA_OC,
	/* DMVA SMETA Specific */
	MSG_CMD_SET_DMVA_SMETA,
	/* DMVA IMD Specific */
	MSG_CMD_SET_DMVA_IMD,

	/* DMVA Enc Roi Enable */
	MSG_CMD_SET_DMVA_ENCROI_ENABLE,

    MSG_CMD_SET_VIDEO_CODEC_RES,
	
	MSG_CMD_SET_VIDEO_CODEC_CFG,
	
	MSG_CMD_GET_MCFW_INIT_STATUS,

	MSG_CMD_END
};

/* system control */
/**
* @brief Commands used to control system server.
*/
enum {
	SYS_MSG_GET_DHCP_EN, ///< Get DHCP status.
	SYS_MSG_GET_HTTP_PORT, ///< Get http port.
	SYS_MSG_SET_NETMASK, ///< Set new netmask of IP netcam.
	SYS_MSG_GET_SNTP_FQDN, ///< Get SNTP FQDN.
	SYS_MSG_GET_TITLE, ///< Get IP netcam title.
	SYS_MSG_GET_MAC, ///< Get ethernet MAC address.
	SYS_MSG_GET_IP, ///< Get IP netcam IP address.
	SYS_MSG_GET_GATEWAY, ///< Get IP netcam gateway.
	SYS_MSG_GET_NETMASK, ///< Get IP netcam netmask.
	SYS_MSG_GET_FTP_SERVER_IP, ///< Get FTP FQDN.
	SYS_MSG_GET_FTP_USERNAME, ///< Get FTP user name.
	SYS_MSG_GET_FTP_PASSWORD, ///< Get FTP user password.
	SYS_MSG_GET_FTP_FOLDNAME, ///< Get FTP foleder.
	SYS_MSG_GET_FTP_IMAGEAMOUNT, ///< Get JPEG count at each FTP connection.
	SYS_MSG_GET_FTP_PID, ///< Get FTP process ID.
	SYS_MSG_GET_FTP_PORT, ///< Get FTP connection port.
	SYS_MSG_GET_SMTP_SERVER_IP, ///< Get SMTP FQDN.
	SYS_MSG_GET_SMTP_SERVER_PORT, ///< Get SMTP PORT.
	SYS_MSG_GET_SMTP_AUTHENTICATION, ///< See if use authentication to send e-mail.
	SYS_MSG_GET_SMTP_USERNAME, ///< Get SMTP user name.
	SYS_MSG_GET_SMTP_PASSWORD, ///< Get SMTP password.
	SYS_MSG_GET_SMTP_SENDER_EMAIL_ADDRESS, ///< Get sender e-mail address.
	SYS_MSG_GET_SMTP_RECEIVER_EMAIL_ADDRESS, ///< Get receiver e-mail address.
	SYS_MSG_GET_SMTP_CC, ///< Get e-mail CC.
	SYS_MSG_GET_SMTP_SUBJECT, ///< Get e-mail subject.
	SYS_MSG_GET_SMTP_TEXT, ///< Get e-mail text.
	SYS_MSG_GET_SMTP_ATTACHMENTS, ///< Get e-mail JPEG attachments.
	SYS_MSG_GET_SMTP_VIEW, ///< Get e-mail style.
	SYS_MSG_GET_DHCP_CONFIG, ///< Get DHCP status.
	SYS_MSG_SET_IP, ///< Set static IP address.
	SYS_MSG_SET_GATEWAY, ///< Set gateway in static IP mode.
	SYS_MSG_SET_DHCPC_ENABLE, ///< Set DHCP enable.
	SYS_MSG_SET_FTP_FQDN, ///< Set FTP FQDN.
	SYS_MSG_SET_FTP_USERNAME, ///< Set FTP username.
	SYS_MSG_SET_FTP_PASSWORD, ///< Set FTP password.
	SYS_MSG_SET_FTP_FOLDNAME, ///< Set FTP folder.
	SYS_MSG_SET_FTP_IMAGEAMOUNT, ///< Set JPEG count at each FTP connection.
	SYS_MSG_SET_FTP_PID, ///< Save FTP process ID.
	SYS_MSG_SET_FTP_PORT, ///< Set FTP port.
	SYS_MSG_SET_SMTP_SERVER_IP, ///< Set SMTP FQDN.
	SYS_MSG_SET_SMTP_SERVER_PORT, ///< Set SMTP PORT.
	SYS_MSG_SET_SMTP_AUTHENTICATION, ///< Let IP netcam use authentication to send e-mail or not.
	SYS_MSG_SET_SMTP_USERNAME, ///< Set SMTP user name.
	SYS_MSG_SET_SMTP_PASSWORD, ///< Set SMTP password.
	SYS_MSG_SET_SMTP_SENDER_EMAIL_ADDRESS, ///< Set sender e-mail address.
	SYS_MSG_SET_SMTP_RECEIVER_EMAIL_ADDRESS, ///< Set receiver e-mail address.
	SYS_MSG_SET_SMTP_CC, ///< Set e-mail CC.
	SYS_MSG_SET_SMTP_SUBJECT, ///< Set e-mail subject.
	SYS_MSG_SET_SMTP_TEXT, ///< Set e-mail text.
	SYS_MSG_SET_SMTP_ATTACHMENTS, ///< Set e-mail JPEG attachments.
	SYS_MSG_SET_SMTP_VIEW, ///< Set e-mail style.
	SYS_MSG_GET_DNS, ///< Get DNS address.
	SYS_MSG_SET_DNS, ///< Set DNS address.
	SYS_MSG_SET_HOSTNAME,    ///<set hostname.
	SYS_MSG_SET_SNTP_SERVER_IP, ///< Set SNTP FQDN.
	SYS_MSG_SET_DEMOCFG,
	SYS_MSG_SET_SCHEDULE, ///< Set user define schedule.
	SYS_MSG_DEL_SCHEDULE, ///< Delete all exist schedule.
    SYS_MSG_SET_LOSTALARM, ///< Enable ethernet lost alarm or not.
	SYS_MSG_SET_SDAENABLE, ///< Enable save file to SD card when alarm happen or not.
	SYS_MSG_SET_FTPAENABLE, ///< Enable send file to FTP server when alarm happen or not.
	SYS_MSG_SET_SMTPAENABLE, ///< Enable send file to SMTP server when alarm happen or not.
	SYS_MSG_SET_ALARMDURATION, ///< Set how long will one alarm last.
	SYS_MSG_SET_AVIDURATION, ///< Set length of one AVI file(in seconds).
	SYS_MSG_SET_AVIFORMAT, ///< Set format of AVI file.
	SYS_MSG_SET_FTPFILEFORMAT, ///< Sent which kind of files to FTP.
	SYS_MSG_SET_SDFILEFORMAT, ///< Save which kind of files to SD card.
	SYS_MSG_SET_ATTFILEFORMAT, ///< Sent which kind of files to SMTP.
	SYS_MSG_SET_ASMTPATTACH,
	SYS_MSG_SET_FTP_RFTPENABLE, ///< Enable send file to FTP server at schedule.
	SYS_MSG_SET_SD_REENABLE, ///< Enable save file to SD card at schedule.
	SYS_MSG_SET_IMAGE_SOURCE, ///< Select NTSC/PAL mode.
	/* Live Video Page */
	SYS_MSG_SET_CLICK_SNAP_FILENAME,
	SYS_MSG_SET_CLICK_SNAP_STORAGE,
	/* Video & Image */
	SYS_MSG_SET_TITLE, ///< Set IP netcam title.
	SYS_MSG_SET_VIDEO_MODE,
	SYS_MSG_SET_VIDEOCODEC_COMBO,
	SYS_MSG_SET_VIDEOCODEC_RES, ///< Set video resolution. This operation will take several time.
	SYS_MSG_SET_MPEG41_BITRATE, ///< Set MPEG1 bitrate.
	SYS_MSG_SET_MPEG42_BITRATE, ///< Set MPEG2 bitrate.
	SYS_MSG_SET_JPEG_QUALITY, ///< Set JPEG encode quality.
	SYS_MSG_SET_FRAMERATE1, ///< Set MPEG1 frame rate.
	SYS_MSG_SET_FRAMERATE2, ///< Set MPEG2 frame rate.
	SYS_MSG_SET_FRAMERATE3, ///< Set JPEG frame rate.
	SYS_MSG_SET_RATE_CONTROL1,
	SYS_MSG_SET_RATE_CONTROL2,
	SYS_MSG_SET_DATE_STAMP_ENABLE1,
	SYS_MSG_SET_DATE_STAMP_ENABLE2,
	SYS_MSG_SET_DATE_STAMP_ENABLE3,
	SYS_MSG_SET_TIME_STAMP_ENABLE1,
	SYS_MSG_SET_TIME_STAMP_ENABLE2,
	SYS_MSG_SET_TIME_STAMP_ENABLE3,
	SYS_MSG_SET_LOGO_ENABLE1,
	SYS_MSG_SET_LOGO_ENABLE2,
	SYS_MSG_SET_LOGO_ENABLE3,
	SYS_MSG_SET_LOGO_POSITION1,
	SYS_MSG_SET_LOGO_POSITION2,
	SYS_MSG_SET_LOGO_POSITION3,
	SYS_MSG_SET_TEXT_ENABLE1,
	SYS_MSG_SET_TEXT_ENABLE2,
	SYS_MSG_SET_TEXT_ENABLE3,
	SYS_MSG_SET_TEXT_POSITION1,
	SYS_MSG_SET_TEXT_POSITION2,
	SYS_MSG_SET_TEXT_POSITION3,
	SYS_MSG_SET_OVERLAY_TEXT1,
	SYS_MSG_SET_OVERLAY_TEXT2,
	SYS_MSG_SET_OVERLAY_TEXT3,
    SYS_MSG_SET_DETAIL_INFO1,
    SYS_MSG_SET_DETAIL_INFO2,
    SYS_MSG_SET_DETAIL_INFO3,
	SYS_MSG_SET_ENCRYPTION,
	SYS_MSG_SET_LOCAL_DISPLAY,
	SYS_MSG_SET_MIRROR, ///< Set Rotation.

	/* Video advanced */
	SYS_MSG_SET_IPRATIO1,
	SYS_MSG_SET_IPRATIO2,
	SYS_MSG_SET_IPRATIO3,
	SYS_MSG_SET_FORCE_IFRAME1,
	SYS_MSG_SET_FORCE_IFRAME2,
	SYS_MSG_SET_FORCE_IFRAME3,
	SYS_MSG_SET_QPINIT1,
	SYS_MSG_SET_QPINIT2,
	SYS_MSG_SET_QPINIT3,
	SYS_MSG_SET_QPMIN1,
	SYS_MSG_SET_QPMIN2,
	SYS_MSG_SET_QPMIN3,
	SYS_MSG_SET_QPMAX1,
	SYS_MSG_SET_QPMAX2,
	SYS_MSG_SET_QPMAX3,
	SYS_MSG_SET_MECONFIG1,
	SYS_MSG_SET_MECONFIG2,
	SYS_MSG_SET_MECONFIG3,
	SYS_MSG_SET_PACKET_SIZE1,
	SYS_MSG_SET_PACKET_SIZE2,
	SYS_MSG_SET_PACKET_SIZE3,
	SYS_MSG_SET_ROI_ENABLE1,
	SYS_MSG_SET_ROI_ENABLE2,
	SYS_MSG_SET_ROI_ENABLE3,
	SYS_MSG_SET_STR1X1,
	SYS_MSG_SET_STR1Y1,
	SYS_MSG_SET_STR1W1,
	SYS_MSG_SET_STR1H1,
	SYS_MSG_SET_STR1X2,
	SYS_MSG_SET_STR1Y2,
	SYS_MSG_SET_STR1W2,
	SYS_MSG_SET_STR1H2,
	SYS_MSG_SET_STR1X3,
	SYS_MSG_SET_STR1Y3,
	SYS_MSG_SET_STR1W3,
	SYS_MSG_SET_STR1H3,
	SYS_MSG_SET_STR2X1,
	SYS_MSG_SET_STR2Y1,
	SYS_MSG_SET_STR2W1,
	SYS_MSG_SET_STR2H1,
	SYS_MSG_SET_STR2X2,
	SYS_MSG_SET_STR2Y2,
	SYS_MSG_SET_STR2W2,
	SYS_MSG_SET_STR2H2,
	SYS_MSG_SET_STR2X3,
	SYS_MSG_SET_STR2Y3,
	SYS_MSG_SET_STR2W3,
	SYS_MSG_SET_STR2H3,
	SYS_MSG_SET_STR3X1,
	SYS_MSG_SET_STR3Y1,
	SYS_MSG_SET_STR3W1,
	SYS_MSG_SET_STR3H1,
	SYS_MSG_SET_STR3X2,
	SYS_MSG_SET_STR3Y2,
	SYS_MSG_SET_STR3W2,
	SYS_MSG_SET_STR3H2,
	SYS_MSG_SET_STR3X3,
	SYS_MSG_SET_STR3Y3,
	SYS_MSG_SET_STR3W3,
	SYS_MSG_SET_STR3H3,
	/* Video Analytics */
	SYS_MSG_SET_MOTIONENABLE, ///< Enable motion detection or not.
	SYS_MSG_SET_MOTIONCENABLE, ///< Use customer define or normal mode.
	SYS_MSG_SET_MOTIONLEVEL, ///< Set motion level in normal mode.
	SYS_MSG_SET_MOTIONCVALUE, ///< Set customer value at customer mode.
	SYS_MSG_SET_MOTIONBLOCK, ///< Define motion blocks.
	SYS_MSG_SET_FDETECT,
	SYS_MSG_SET_FDX,
	SYS_MSG_SET_FDY,
	SYS_MSG_SET_FDW,
	SYS_MSG_SET_FDH,
	SYS_MSG_SET_FDCONF_LEVEL,
	SYS_MSG_SET_FD_DIRECTION,
	SYS_MSG_SET_FRECOGNITION,
	SYS_MSG_SET_FRCONF_LEVEL,
	SYS_MSG_SET_FR_DATABASE,
	SYS_MSG_SET_PRIVACY_MASK,
	SYS_MSG_SET_MASK_OPTIONS,
	/* Camera Page */
	SYS_MSG_SET_BRIGHTNESS, ///< Set brightness.
	SYS_MSG_SET_CONTRAST, ///< Set contrast.
	SYS_MSG_SET_SATURATION, ///< Set saturation.
	SYS_MSG_SET_SHARPNESS, ///< Set sharpness.
	SYS_MSG_SET_CLEARSYSLOG, ///< Clear System Log
	SYS_MSG_SET_CLEARACCESSLOG, ///< Clear Access Log
	SYS_MSG_SET_BLC,
	SYS_MSG_SET_BACKLIGHT, ///< Set backlight mode of IP netcam.
	SYS_MSG_SET_DYNRANGE,
	SYS_MSG_SET_DYNRANGESTRENGTH,
	SYS_MSG_SET_WHITE_BALANCE, ///< Set whitebalance moe of IP netcam.
	SYS_MSG_SET_DAY_NIGHT, ///< Set day/night mode of IP netcam.
	SYS_MSG_SET_HISTOGRAM,
	SYS_MSG_SET_BINNING, ///< Set binning mode.
	SYS_MSG_SET_2A_SWITCH,
	SYS_MSG_SET_2A_TYPE,
	SYS_MSG_SET_EXPPRIORITY,
	SYS_MSG_SET_VSTAB,
	SYS_MSG_SET_LDC,
	SYS_MSG_SET_VNF,
	SYS_MSG_SET_VNFMODE,
	SYS_MSG_SET_VNFSTRENGTH,
	SYS_MSG_SET_FRAMECTRL,
	SYS_MSG_SET_AUTOIRIS_VOL,
    /* Audio Page */
	SYS_MSG_SET_AUDIOENABLE, ///< Enable audio send to web site or not.
	SYS_MSG_SET_AUDIOMODE,
	SYS_MSG_SET_AUDIOINVOLUME, ///< Set alarm audio volume
	SYS_MSG_SET_AUDIO_ENCODE,
	SYS_MSG_SET_AUDIO_SAMPLERATE,
	SYS_MSG_SET_AUDIO_BITRATE,
	SYS_MSG_SET_AUDIO_ALARMLEVEL,
	SYS_MSG_SET_AUDIOOUTVOLUME,
	SYS_MSG_SET_AUDIORECEIVER_ENABLE,
	SYS_MSG_SET_AUDIOSERVER_IP,
    /* Date Time Page */
	SYS_MSG_SET_DAYLIGHT, ///< Set system time to use daylight or not.
	SYS_MSG_SET_TIMEZONE, ///< Set system time zone.
	SYS_MSG_SET_DATEFORMAT,
	SYS_MSG_SET_TSTAMPFORMAT, ///< Set timestamp format.
	SYS_MSG_SET_DATEPPOS,
	SYS_MSG_SET_TIMEPOS,
    /* Network & Port Page */
	SYS_MSG_SET_MULTICAST,///< Set Multicast fuction enable / disable.
	SYS_MSG_SET_HTTPPORT, ///< set web site port. This operation may take serveral time.
	SYS_MSG_SET_HTTPSPORT,
	SYS_MSG_SET_GIOINTYPE, ///< Set GIO input alarm triger type(high triger or low triger).
	SYS_MSG_SET_GIOOUTTYPE, ///< Set GIO output high or low.
	SYS_MSG_SET_RS485,
	/* Alarm & Storage */
	SYS_MSG_SET_ALARM_ENABLE,
	SYS_MSG_SET_AUDIOALARM,
	SYS_MSG_SET_EXT_ALARM,
	SYS_MSG_SET_ALARM_AUDIOPLAY,
	SYS_MSG_SET_ALARM_AUDIOFILE,
	SYS_MSG_SET_ALARMLOCAL,
	SYS_MSG_SET_RECORDLOCAL,
    SYS_MSG_SET_SCHEDULE_REPEAT_ENABLE,
    SYS_MSG_SET_SCHEDULE_NUM_WEEKS,
    SYS_MSG_SET_SCHEDULE_INFINITE_ENABLE,
	/* Maintainence */
	SYS_MSG_SET_DEFAULT,
	/**********************/
	SYS_MSG_SET_IMAGE_DEFAULT, ///< Set all image setting to default.
	SYS_MSG_DO_LOGIN, ///< Do user login.
	SYS_MSG_GET_USER_AUTHORITY, ///< Get user authority by name.
	SYS_MSG_ADD_USER, ///< Add/change user acount.
	SYS_MSG_DEL_USER, ///< Delete user acount.
	SYS_MSG_SET_GIOINENABLE, ///< Enable GIO input alarm or not.
	SYS_MSG_SET_GIOOUTENABLE, ///< Enable GIO ouput at alarm or not.
	SYS_MSG_QUIT, ///< Make system server quit processing.
	SYS_MSG_SD_FORMAT, ///< Format SD card.
	SYS_MSG_SD_UNMOUNT, ///< Unmount SD card.
	SYS_MSG_SCHEDULE_STOP, ///< Stop schedule.
	SYS_MSG_SCHEDULE_RESUME, ///< Resume schedule.
	SYS_MSG_GET_SD_FILENAME,///< Get current recording file in SD card.
	SYS_MSG_DO_BOOT_PROC,///< Tell system server to do something about boot control.
        SYS_MSG_ADD_PROFILE,  ///create onvif profile.....
        SYS_MSG_ADD_VIDEOSOURCE_CONF,        //add video source confiuration .
	SYS_MSG_SET_AUDIOSOURCE_CONF,
        SYS_MSG_SET_VIDEOSOURCE_CONF,
        SYS_MSG_SET_IMAGING_CONF,
        SYS_MSG_ADD_VIDEOENCODER_CONF,
        SYS_MSG_SET_VIDEOENCODER_CONF,
        SYS_MSG_DEL_VIDEOENCODER_CONF,
	SYS_MSG_ADD_AUDIO_SOURCE_CONF,
	SYS_MSG_ADD_AUDIO_ENCODER_CONF,
	SYS_MSG_SET_AUDIO_SOURCE_CONF,
	SYS_MSG_SET_AUDIO_ENCODER_CONF,
	SYS_MSG_DEL_AUDIO_SOURCE_CONF,
	SYS_MSG_DEL_AUDIO_ENCODER_CONF,
        SYS_MSG_DEL_PROFILE,
        SYS_MSG_SET_DIS_MODE,            //set discovery mode
        SYS_MSG_ADD_SCOPE,                //add scopes
        SYS_MSG_DEL_SCOPE,                //remove scopes
	SYS_MSG_SET_SCOPE,                //Set Scopes
        SYS_MSG_SET_ANALYTICS,             //set video analytics
        SYS_MSG_SET_OUTPUT,             //set video output
	SYS_MSG_SET_RELAY,		//set relays
	SYS_MSG_SET_RELAY_LOGICALSTATE,
	SYS_MSG_SET_METADATA,
	SYS_MSG_ADD_METADATA,
	SYS_MSG_DEL_METADATA,

	/* Advanced DMVA System Message Parameters */
	SYS_MSG_SET_DMVAENABLE,
    SYS_MSG_SET_DMVAALGOFRAMERATE,
    SYS_MSG_SET_DMVAALGODETECTFREQ,
    SYS_MSG_SET_DMVAEVTRECORDENABLE,
    SYS_MSG_SET_DMVASMETATRACKERENABLE,
	/* DMVA TZ System Message Parameters */
    SYS_MSG_SET_DMVA_TZ_SENSITIVITY,
    SYS_MSG_SET_DMVA_TZ_PERSONMINWIDTH,
    SYS_MSG_SET_DMVA_TZ_PERSONMINHEIGHT,
    SYS_MSG_SET_DMVA_TZ_VEHICLEMINWIDTH,
    SYS_MSG_SET_DMVA_TZ_VEHICLEMINHEIGHT,
    SYS_MSG_SET_DMVA_TZ_PERSONMAXWIDTH,
    SYS_MSG_SET_DMVA_TZ_PERSONMAXHEIGHT,
    SYS_MSG_SET_DMVA_TZ_VEHICLEMAXWIDTH,
    SYS_MSG_SET_DMVA_TZ_VEHICLEMAXHEIGHT,
    SYS_MSG_SET_DMVA_TZ_DIRECTION,
    SYS_MSG_SET_DMVA_TZ_PRESENTADJUST,
    SYS_MSG_SET_DMVA_TZ_ENABLE,

  SYS_MSG_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES,
  SYS_MSG_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES,

  SYS_MSG_SET_DMVA_TZ_ZONE1_X01,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X02,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X03,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X04,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X05,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X06,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X07,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X08,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X09,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X10,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X11,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X12,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X13,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X14,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X15,
  SYS_MSG_SET_DMVA_TZ_ZONE1_X16,

  SYS_MSG_SET_DMVA_TZ_ZONE1_Y01,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y02,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y03,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y04,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y05,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y06,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y07,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y08,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y09,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y10,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y11,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y12,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y13,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y14,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y15,
  SYS_MSG_SET_DMVA_TZ_ZONE1_Y16,

  SYS_MSG_SET_DMVA_TZ_ZONE2_X01,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X02,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X03,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X04,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X05,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X06,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X07,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X08,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X09,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X10,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X11,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X12,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X13,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X14,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X15,
  SYS_MSG_SET_DMVA_TZ_ZONE2_X16,

  SYS_MSG_SET_DMVA_TZ_ZONE2_Y01,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y02,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y03,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y04,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y05,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y06,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y07,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y08,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y09,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y10,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y11,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y12,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y13,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y14,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y15,
  SYS_MSG_SET_DMVA_TZ_ZONE2_Y16,
  SYS_MSG_SET_DMVA_TZ_SAVE,
  SYS_MSG_SET_DMVA_TZ_LOAD,

	  /* DMVA MAIN ANALYTICS PAGE */
	  SYS_MSG_SET_DMVA_MAIN_CAMID,
	  SYS_MSG_SET_DMVA_MAIN_GETSCHEDULE,
	  SYS_MSG_SET_DMVA_MAIN_FD,
	  SYS_MSG_SET_DMVA_MAIN_CTDIMDSMETATZOC,
	  SYS_MSG_SET_DMVA_MAIN_DISPLAYOPTIONS,
	  SYS_MSG_SET_DMVA_MAIN_RECORDINGVAME,
	  SYS_MSG_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH,
	  SYS_MSG_SET_DMVA_MAIN_SELECTEVENT,
	  SYS_MSG_SET_DMVA_MAIN_ARCHIVEFLAG,
	  SYS_MSG_SET_DMVA_MAIN_EVENTTYPE,

	  SYS_MSG_SET_DMVA_MAIN_EL_START_MONTH,
	  SYS_MSG_SET_DMVA_MAIN_EL_START_DAY,
	  SYS_MSG_SET_DMVA_MAIN_EL_START_TIME,
	  SYS_MSG_SET_DMVA_MAIN_EL_END_MONTH,
	  SYS_MSG_SET_DMVA_MAIN_EL_END_DAY,
	  SYS_MSG_SET_DMVA_MAIN_EL_END_TIME,

	  SYS_MSG_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL,
	  SYS_MSG_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL,

	  /* DMVA ADVANCED SETTINGS PAGE */
	  SYS_MSG_SET_DMVA_ADVANCED_OBJECTMETADATA,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTTYPE,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTSIZE,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR,
	  SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILELOAD,
	  SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTORE,
	  SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILECLEAR,
	  SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTATE,
	  SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILEDESC,

	  /* DMVA event delete */
	  SYS_MSG_SET_DMVA_EVENT_DELETE_INDEX,
	  SYS_MSG_SET_DMVA_EVENT_DELETE_START_INDEX,
	  SYS_MSG_SET_DMVA_EVENT_DELETE_END_INDEX,
	  SYS_MSG_SET_DMVA_EVENT_DELETE_ALL,

	  /* DMVA CTD PAGE */
	  SYS_MSG_SET_DMVA_CTD_ENV1,
	  SYS_MSG_SET_DMVA_CTD_ENV2,
	  SYS_MSG_SET_DMVA_CTD_SENSITIVITY,
	  SYS_MSG_SET_DMVA_CTD_RESETTIME,

	  /* DMVA OC PAGE */
	  SYS_MSG_SET_DMVA_OC_SENSITIVITY,
	  SYS_MSG_SET_DMVA_OC_OBJECTWIDTH,
	  SYS_MSG_SET_DMVA_OC_OBJECTHEIGHT,
	  SYS_MSG_SET_DMVA_OC_DIRECTION,
	  SYS_MSG_SET_DMVA_OC_ENABLE,
	  SYS_MSG_SET_DMVA_OC_LEFTRIGHT,
	  SYS_MSG_SET_DMVA_OC_TOPBOTTOM,
      SYS_MSG_SET_DMVA_OC_SAVE,
      SYS_MSG_SET_DMVA_OC_LOAD,


	  /* DMVA SMETA PAGE */
	  SYS_MSG_SET_DMVA_SMETA_SENSITIVITY,
	  SYS_MSG_SET_DMVA_SMETA_ENABLESTREAMINGDATA,
	  SYS_MSG_SET_DMVA_SMETA_STREAMBB,
	  SYS_MSG_SET_DMVA_SMETA_STREAMVELOCITY,
	  SYS_MSG_SET_DMVA_SMETA_STREAMHISTOGRAM,
	  SYS_MSG_SET_DMVA_SMETA_STREAMMOMENTS,
	  SYS_MSG_SET_DMVA_SMETA_PRESENTADJUST,
	  SYS_MSG_SET_DMVA_SMETA_PERSONMINWIDTH,
	  SYS_MSG_SET_DMVA_SMETA_PERSONMINHEIGHT,
	  SYS_MSG_SET_DMVA_SMETA_VEHICLEMINWIDTH,
	  SYS_MSG_SET_DMVA_SMETA_VEHICLEMINHEIGHT,
	  SYS_MSG_SET_DMVA_SMETA_PERSONMAXWIDTH,
	  SYS_MSG_SET_DMVA_SMETA_PERSONMAXHEIGHT,
	  SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXWIDTH,
	  SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXHEIGHT,
	  SYS_MSG_SET_DMVA_SMETA_NUMZONES,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES,

  SYS_MSG_SET_DMVA_SMETA_ZONE1_X01,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X02,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X03,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X04,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X05,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X06,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X07,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X08,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X09,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X10,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X11,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X12,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X13,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X14,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X15,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_X16,

  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y01,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y02,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y03,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y04,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y05,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y06,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y07,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y08,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y09,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y10,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y11,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y12,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y13,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y14,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y15,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_Y16,
  SYS_MSG_SET_DMVA_SMETA_ZONE1_LABEL,
  SYS_MSG_SET_DMVA_SMETA_SAVE,
  SYS_MSG_SET_DMVA_SMETA_LOAD,

	  /* DMVA IMD PAGE */
	  SYS_MSG_SET_DMVA_IMD_SENSITIVITY,
	  SYS_MSG_SET_DMVA_IMD_PRESENTADJUST,
	  SYS_MSG_SET_DMVA_IMD_PERSONMINWIDTH,
	  SYS_MSG_SET_DMVA_IMD_PERSONMINHEIGHT,
	  SYS_MSG_SET_DMVA_IMD_VEHICLEMINWIDTH,
	  SYS_MSG_SET_DMVA_IMD_VEHICLEMINHEIGHT,
	  SYS_MSG_SET_DMVA_IMD_PERSONMAXWIDTH,
	  SYS_MSG_SET_DMVA_IMD_PERSONMAXHEIGHT,
	  SYS_MSG_SET_DMVA_IMD_VEHICLEMAXWIDTH,
	  SYS_MSG_SET_DMVA_IMD_VEHICLEMAXHEIGHT,
	  SYS_MSG_SET_DMVA_IMD_NUMZONES,
	  SYS_MSG_SET_DMVA_IMD_ZONEENABLE,

  SYS_MSG_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES,
  SYS_MSG_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES,
  SYS_MSG_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES,
  SYS_MSG_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES,

  SYS_MSG_SET_DMVA_IMD_ZONE1_X01,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X02,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X03,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X04,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X05,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X06,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X07,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X08,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X09,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X10,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X11,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X12,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X13,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X14,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X15,
  SYS_MSG_SET_DMVA_IMD_ZONE1_X16,

  SYS_MSG_SET_DMVA_IMD_ZONE1_Y01,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y02,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y03,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y04,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y05,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y06,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y07,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y08,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y09,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y10,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y11,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y12,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y13,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y14,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y15,
  SYS_MSG_SET_DMVA_IMD_ZONE1_Y16,

  SYS_MSG_SET_DMVA_IMD_ZONE2_X01,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X02,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X03,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X04,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X05,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X06,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X07,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X08,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X09,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X10,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X11,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X12,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X13,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X14,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X15,
  SYS_MSG_SET_DMVA_IMD_ZONE2_X16,

  SYS_MSG_SET_DMVA_IMD_ZONE2_Y01,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y02,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y03,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y04,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y05,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y06,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y07,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y08,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y09,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y10,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y11,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y12,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y13,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y14,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y15,
  SYS_MSG_SET_DMVA_IMD_ZONE2_Y16,

  SYS_MSG_SET_DMVA_IMD_ZONE3_X01,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X02,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X03,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X04,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X05,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X06,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X07,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X08,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X09,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X10,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X11,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X12,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X13,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X14,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X15,
  SYS_MSG_SET_DMVA_IMD_ZONE3_X16,

  SYS_MSG_SET_DMVA_IMD_ZONE3_Y01,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y02,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y03,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y04,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y05,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y06,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y07,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y08,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y09,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y10,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y11,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y12,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y13,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y14,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y15,
  SYS_MSG_SET_DMVA_IMD_ZONE3_Y16,

  SYS_MSG_SET_DMVA_IMD_ZONE4_X01,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X02,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X03,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X04,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X05,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X06,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X07,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X08,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X09,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X10,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X11,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X12,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X13,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X14,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X15,
  SYS_MSG_SET_DMVA_IMD_ZONE4_X16,

  SYS_MSG_SET_DMVA_IMD_ZONE4_Y01,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y02,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y03,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y04,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y05,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y06,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y07,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y08,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y09,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y10,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y11,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y12,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y13,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y14,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y15,
  SYS_MSG_SET_DMVA_IMD_ZONE4_Y16,

  SYS_MSG_SET_DMVA_IMD_ZONE1_LABEL,
  SYS_MSG_SET_DMVA_IMD_ZONE2_LABEL,
  SYS_MSG_SET_DMVA_IMD_ZONE3_LABEL,
  SYS_MSG_SET_DMVA_IMD_ZONE4_LABEL,

  SYS_MSG_SET_DMVA_IMD_SAVE,
  SYS_MSG_SET_DMVA_IMD_LOAD,

  /* DMVA Scheduler */
  SYS_MSG_SET_DMVA_SCH_ENABLE,
  SYS_MSG_SET_DMVA_SCH_RULETYPE,
  SYS_MSG_SET_DMVA_SCH_LOADSETTING,
  SYS_MSG_SET_DMVA_SCH_RULELABEL,
  SYS_MSG_SET_DMVA_SCH_STARTDAY,
  SYS_MSG_SET_DMVA_SCH_STARTTIME,
  SYS_MSG_SET_DMVA_SCH_ENDDAY,
  SYS_MSG_SET_DMVA_SCH_ENDTIME,
  SYS_MSG_SET_DMVA_SCH_START,
  SYS_MSG_SET_DMVA_SCH_DELRULE,

  /* DMVA Enc Roi Enable */
  SYS_MSG_SET_DMVA_ENC_ROIENABLE,

	SYS_MSG_MAX
};

/**
* @brief Commands used to communicate with file manager.

*/
typedef enum {
	FILE_MSG_QUIT, ///< Let file manager shutdown.
	FILE_MSG_READ, ///< Read system settings from file manager.
	FILE_MSG_WRITE_I, ///< Tell file manager to write system config immediately.
	FILE_MSG_WRITE, ///< Tell file manager to write system config.
	FILE_MSG_FORMAT, ///< Ask file manager to format NAND flash.
	FILE_MSG_RESET, ///< Reset to default.
	FILE_GET_MID, ///< Get file manager's share memory ID.
	FILE_MSG_READ_DMVA_LOG,///< Read DMVA system settings from file manager.
	FILE_MSG_WRITE_DMVATZ,
	FILE_MSG_READ_DMVATZ,
	FILE_MSG_WRITE_DMVAIMD,
	FILE_MSG_READ_DMVAIMD,
    FILE_MSG_WRITE_DMVAOC,
    FILE_MSG_READ_DMVAOC,
    FILE_MSG_WRITE_DMVASMETA,
    FILE_MSG_READ_DMVASMETA,
	FILE_MSG_DMVA_DELETE_INDEX,
	FILE_MSG_DMVA_DELETE_STARTEND_INDEX,
	FILE_MSG_DMVA_DELETE_ALL,
	FILE_MSG_WRITE_DMVA_LOG,
	FILE_MSG_SEARCH_DMVA_EVT,
	FILE_MSG_CLEAR_SYSLOG,
	FILE_MSG_CLEAR_ACCESSLOG,
	FILE_MSG_MAX ///< File manager command number.
} FileCmd_t;
/**
* @ingroup MSG_UTIL
* @brief Message type

*/
enum {
	MSG_TYPE_START = 0,

	MSG_TYPE_MSG1, ///< Message type 1. Always reserved for server.
	MSG_TYPE_MSG2, ///< Message type 2.
	MSG_TYPE_MSG3, ///< Message type 3.
	MSG_TYPE_MSG4, ///< Message type 4.
	MSG_TYPE_MSG5, ///< Message type 5.
	MSG_TYPE_MSG6, ///< Message type 6.
	MSG_TYPE_MSG7, ///< Message type 7.
	MSG_TYPE_MSG8, ///< Message type 8.
	MSG_TYPE_MSG9, ///< Message type 9.
	MSG_TYPE_MSG10, ///< Message type 10.
	MSG_TYPE_MSG11, ///< Message type 11.
	MSG_TYPE_MSG12, ///< Message type 12.
	MSG_TYPE_MSG13, ///< Message type 13.
	MSG_TYPE_MSG14, ///< Message type 14.

	MSG_TYPE_END
};

typedef enum {
	FMT_MJPEG = 0,
	FMT_MPEG4,
	FMT_MPEG4_EXT,
	FMT_AUDIO,
	FMT_MAX_NUM
} FrameFormat_t;

typedef struct _FrameInfo{
	int offset;
	int serial_no;
	int size;
	int width;
	int height;
	int format;
	int frameType;
	unsigned int quality;
	unsigned int flags;
	unsigned int timestamp;
	unsigned int temporalId;
	int		ref_serial[FMT_MAX_NUM];
}FrameInfo_t;

typedef struct _MSG_MEM{
	unsigned long addr;
	unsigned long size;
}MSG_MEM;

typedef struct _MSG_BUF {
	long 		Des; ///< message type, must be > 0 .
	int 		cmd; ///< data .
	int			Src;
	int			ret;
	MSG_MEM		mem_info;
	FrameInfo_t	frame_info;
} MSG_BUF;
/**
* @brief System message driver buffer type
*/
typedef struct SYS_MSG_BUF{
	long 		Des; ///< Where this message go. See /ref Sys_Msg_Def.h
	int			cmd; ///< command to control system server.
	int			src; ///< Where this message from. See /ref Sys_Msg_Def.h
	int			shmid; ///< Share memory which was used to communicate with system server.
	int			length; ///< Data length.
	int			offset; ///< Offset to begin of share memory.
} SYS_MSG_BUF;
/**
* @brief File message buffer type

* This structure is used to communicate with file manager.
*/
typedef struct _FILE_MSG_BUF{
	long 		Des; ///< Where this message go.
	FileCmd_t	cmd; ///< Message command ID.
	int			src; ///< Where this message from.
	int			shmid; ///< Share memory ID which is used to communicate.
	int			length; ///< Data length in share memory.
	int			shmidx; ///< Share memory index (0 or 1). 0 is used for system settings; and 1 is used for system log.
	int			nPageNum; ///< System log page number.
	int			nItemNum; ///< System log item number.
} FILE_MSG_BUF;
/**
* @ingroup ALARM_MSG_DRV
* @brief Commands used to communicate with alarm manager.

*/
typedef enum {
	ALARM_EVENT_MOTION_DETECT, ///< Motion detection alarm.
	ALARM_EVENT_AUDIO_TRIG,
	ALARM_EVENT_MOTION_AUDIO, ///< Motion detection audio alarm.
	ALARM_EVENT_ALARMRESET_TRIG, ///< Reset alarm.
	ALARM_EVENT_ALARMIN_TRIG, ///< External trigger alarm.
	ALARM_EVENT_ALARMETH_TRIG, ///< Ethernet lost alarm.
	ALARM_EVENT_SCHEDULE, ///< Schedule alarm.
	ALARM_EVENT_SCHEDULE_END, ///< Schedule end alarm.
	ALARM_EVENT_QUIT, ///<  Let alarm manager shutdown.
	ALARM_EVENT_DMVA_START, ///<  DMVA Alarm Start.
	ALARM_EVENT_DMVA_STOP, ///<  DMVA Alarm Stop.
	ALARM_EVENT_MAX ///<  Alarm manager command number.
} Event_t;
/**
* @ingroup ALARM_MSG_DRV
* @brief Alarm message buffer type

* This structure is used to communicate with alarm manager.
*/
typedef struct ALARM_MSG_BUF{
	long 		Des; ///< Where this message go.
	int			src; ///< Where this message from.
	Event_t		event; ///< Message command ID.
	int			index; ///< To tell witch schedule is.
	int			serial;///< Motion detect JPEG serial.
	int			alarmaudio_duration;///<Play alarm audio base on duration.
	int			alarmaudio_enable;///<Restart the alarm audio.
} ALARM_MSG_BUF;

#define MSG_KEY			0x12345
#define SYS_MSG_KEY		0x36f9 ///< Message key used by system server.
#define FILE_MSG_KEY	0xc54be5 ///< File message key.
#define ALARM_KEY		0x542c7ae6 ///< Alarm message key.
#define AUD_MSG_KEY     0x31       ///< Audio alarm message key
#define STREAMER_PATH "/opt/ipnc/wis-streamer" //path of wis-streamer Used for creating msg queue for wis-streamer
#define Message_Wis_Streamer 2   //Priority of Wis-streamer msg queue

/*
Pre define message usage by function
ex:
FuncName_Msg_Def.h
*/
#include <Stream_Msg_Def.h>
#include <File_Msg_Def.h>
#include <Sys_Msg_Def.h>
#include <Alarm_Msg_Def.h>
/**
* @ingroup UTILITY API
* @defgroup MSG_QUEUE Message queue.

* This is the place to store message data for communication.
* @{
*/
int Msg_Init( int msgKey );
int Msg_Kill( int qid );
int Msg_Send( int qid, void *pdata , int size );
int Msg_Rsv( int qid, int msg_type, void *pdata , int size );
int Msg_Send_Rsv( int qid, int msg_type, void *pdata , int size );
int msg_queue_wis_streamer(void);
/** @} */

#define NO_ADV_FTR	0x00
#define FFLAG_VS	0x01
#define FFLAG_LDC	0x02
#define FFLAG_SNF	0x04
#define FFLAG_TNF	0x08

#define VNF_MASK	0x0C
#define VNF_RESET	0x03
#define PREPR_MASK	0x03
#define PREPR_RESET	0x0C

#define VS_MASK			0x01
#define VS_RESET		0x0E
#define VNF_LDC_MASK	0x0E
#define VNF_LDC_RESET	0x01

#define MAX_GUI_ROI	(3)

typedef enum{
	FACE_NO_RECOG=0,
	FACE_RECOGIZE,
	FACE_REGUSER,
	FACE_DELUSER
}FACE_RECOG_TYPE;

typedef enum{
	DEMO_NONE=0,
    VA_DEMO,
	ROI_CENTER_DEMO,
	VS_DEMO,
	VNF_DEMO,
	LDC_DEMO,
	MAX_DEMO_MODE
}ADV_DEMO_ENUM;

typedef enum{
	ROI_FACE_TYPE=0,
	ROI_BACKGROUND_TYPE,
	ROI_FOREGROUND_TYPE,
	ROI_DEFAULT_TYPE,
	ROI_PRIVACY_MASK
}ROI_TYPE;

typedef struct _AudioCreatePrm{
	int enable;
	int samplingRate;
	int codecType;
	int bitRate;
}AudioCreatePrm;

typedef struct _AdvFtrCreatePrm{
	int vsEnable;
	int ldcEnable;
	int snfEnable;
	int tnfEnable;
    int glbceEnable;
}AdvFtrCreatePrm;

typedef struct _McfwCreatePrm{
	int demoId;
	int usecase_id;
	int vendor2A;
	int mode2A;
	int StreamPreset[2];
	AudioCreatePrm audioCreatePrm;
	AdvFtrCreatePrm advFtrCreatePrm;
}McfwCreatePrm;

typedef struct _FaceDetectParam
{
  int	   fdetect; 					    ///< facedetect (ON/OFF)
  int      startX;
  int      startY;
  int      width;
  int      height;
  int      fdconflevel;
  int      fddirection;
  int 	   frecog;
  int	   frconflevel;
  int	   frdatabase;
  int	   pmask;
  int      maskoption;
}FaceDetectParam;

typedef struct _DmvaParam
{
  int	   dmvaenable; 					    ///< dmvaenable (ON/OFF)
  int	   dmvaexptalgoframerate; 		    ///< video (fps/N): 3 means 30/3 = 10 fps
  int	   dmvaexptalgodetectfreq; 		    ///< 10 means once in 10 frames that go to DMVA
  int	   dmvaexptevtrecordenable; 	    ///< 0: Off, 1: On
  int      dmvaexptsmetatrackerenable;      ///< 0: Off, 1: On
}DmvaParam;

typedef struct _DmvaTZParam
{
  /* DMVA TZ parameters */
  int dmvaTZSensitivity; ///< TZ parameters /* 1 to 5: Low to High */
  int dmvaTZPersonMinWidth;///< TZ parameters
  int dmvaTZPersonMinHeight;///< TZ parameters
  int dmvaTZVehicleMinWidth;///< TZ parameters
  int dmvaTZVehicleMinHeight;///< TZ parameters
  int dmvaTZPersonMaxWidth;///< TZ parameters
  int dmvaTZPersonMaxHeight;///< TZ parameters
  int dmvaTZVehicleMaxWidth;///< TZ parameters
  int dmvaTZVehicleMaxHeight;///< TZ parameters
  int dmvaTZDirection; ///< TZ parameters/* 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */
  int dmvaTZEnable;///< TZ parameters
  int dmvaTZZone1_ROI_numSides;///< TZ parameters
  int dmvaTZZone2_ROI_numSides;///< TZ parameters
  int dmvaTZZone1_x[16];///< TZ parameters
  int dmvaTZZone1_y[16];///< TZ parameters
  int dmvaTZZone2_x[16];///< TZ parameters
  int dmvaTZZone2_y[16];///< TZ parameters
  int dmvaTZPresentAdjust; ///< TZ parameters
}DmvaTZParam;

typedef struct _DmvaMainParam
{
  /* DMVA MAIN PAGE parameters */
  int dmvaCamID;
  int dmvaGetSchedule;
  int dmvaCfgFD;
  int dmvaCfgCTDImdSmetaTzOc; /* Bit 4: OC, Bit 3: TZ, Bit 2: SMETA, Bit 1: IMD, Bit 0: CTD */
  int dmvaDisplayOptions; /* Bit 3: Object Bounding Box, Bit 2: Zones, Bit 1: TimeStamp, Bit 0: MetaData Overlay*/
  int dmvaEventRecordingVAME;/* Bit 3: Video Stream, Bit 2: Audio Stream, Bit 1: MetaData Stream, Bit 0: Event Log */
  int dmvaEventListActionPlaySendSearchTrash;/* Bit 3: Play, Bit 2: Send Email, Bit 1: Search, Bit 0: Trash*/
  int dmvaEventListSelectEvent;
  int dmvaEventListArchiveFlag;
  int dmvaEventListEventType;
  int dmvaEventListStartMonth;
  int dmvaEventListStartDay;
  int dmvaEventListStartTime;
  int dmvaEventListEndMonth;
  int dmvaEventListEndDay;
  int dmvaEventListEndTime;
}DmvaMainParam;

typedef struct _DmvaCTDParam
{
  /* DMVA CTD PAGE parameters */
  int dmvaCfgTDEnv1;/* Detect Lights On to Off */
  int dmvaCfgTDEnv2;/* Detect Lights On to Off */
  int dmvaCfgTDSensitivity;/* 1 to 5: Low to High */
  int dmvaCfgTDResetTime; /* Number of seconds */
}DmvaCTDParam;

typedef struct _DmvaOCParam
{
  /* DMVA OC PAGE parameters */
  int dmvaOCSensitivity; /* 1 to 5: Low to High */
  int dmvaOCObjectWidth;
  int dmvaOCObjectHeight;
  int dmvaOCDirection;/* 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */
  int dmvaOCEnable;
  int dmvaOCLeftRight;
  int dmvaOCTopBottom;
}DmvaOCParam;

typedef struct _DmvaSMETAParam
{
  /* DMVA SMETA PAGE parameters */
  int dmvaSMETASensitivity; /* 1 to 5: Low to High */
  int dmvaSMETAEnableStreamingData;
  int dmvaSMETAStreamBB;
  int dmvaSMETAStreamVelocity;
  int dmvaSMETAStreamHistogram;
  int dmvaSMETAStreamMoments;

  int dmvaSMETAPresentAdjust; /* 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  int dmvaSMETAPersonMinWidth;
  int dmvaSMETAPersonMinHeight;
  int dmvaSMETAVehicleMinWidth;
  int dmvaSMETAVehicleMinHeight;
  int dmvaSMETAPersonMaxWidth;
  int dmvaSMETAPersonMaxHeight;
  int dmvaSMETAVehicleMaxWidth;
  int dmvaSMETAVehicleMaxHeight;
  int dmvaSMETANumZones; /* 1 to 4 */
  int dmvaSMETAZone1_ROI_numSides;///< SMETA parameters
  int dmvaSMETAZone1_x[16];///< SMETA parameters
  int dmvaSMETAZone1_y[16];///< SMETA parameters
}DmvaSMETAParam;

typedef struct _DmvaIMDParam
{
  /* DMVA IMD PAGE parameters */
  int dmvaIMDSensitivity; /* 1 to 5: Low to High */
  int dmvaIMDPresentAdjust; /* 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  int dmvaIMDPersonMinWidth;
  int dmvaIMDPersonMinHeight;
  int dmvaIMDVehicleMinWidth;
  int dmvaIMDVehicleMinHeight;
  int dmvaIMDPersonMaxWidth;
  int dmvaIMDPersonMaxHeight;
  int dmvaIMDVehicleMaxWidth;
  int dmvaIMDVehicleMaxHeight;
  int dmvaIMDNumZones; /* 1 to 4 */
  int dmvaIMDZoneEnable; /* Bit 3: Zone 3 enable, Bit 2: Zone 2 enable, Bit 1: Zone 1 enable, Bit 0: Zone 0 enable*/
  int dmvaIMDZone1_ROI_numSides;///< IMD parameters
  int dmvaIMDZone2_ROI_numSides;///< IMD parameters
  int dmvaIMDZone3_ROI_numSides;///< IMD parameters
  int dmvaIMDZone4_ROI_numSides;///< IMD parameters
  int dmvaIMDZone1_x[16];///< IMD parameters
  int dmvaIMDZone1_y[16];///< IMD parameters
  int dmvaIMDZone2_x[16];///< IMD parameters
  int dmvaIMDZone2_y[16];///< IMD parameters
  int dmvaIMDZone3_x[16];///< IMD parameters
  int dmvaIMDZone3_y[16];///< IMD parameters
  int dmvaIMDZone4_x[16];///< IMD parameters
  int dmvaIMDZone4_y[16];///< IMD parameters
}DmvaIMDParam;

typedef struct _ApproMotionPrm{
	int bMotionEnable;
	int bMotionCEnale;
	int MotionLevel;
	int MotionCValue;
	int MotionBlock;
}ApproMotionPrm;

typedef struct _OSDPrm{
	int detailedInfo;
	int	dateEnable;
	int	timeEnable;
	int	logoEnable;
	int logoPos;
	int	textEnable;
	int textPos;
	char text[24];
}OSDPrm;

typedef struct _DateTimePrm{
	int dateFormat;
	int datePos;
	int timeFormat;
	int timePos;
}DateTimePrm;

typedef struct _CodecAdvPrm{
	int ipRatio;
	int fIframe;
	int qpInit;
	int qpMin;
	int qpMax;
	int meConfig;
	int packetSize;
}CodecAdvPrm;

typedef struct _ROI_Prm{
	int startx;
	int starty;
	int width;
	int height;
	int type;
}ROI_Prm;

typedef struct _CodecROIPrm{
	char 	numROI;
	ROI_Prm	roi[3];
}CodecROIPrm;

typedef struct _DynRangePrm{
	int enable;
	int mode;
	int level;
}DynRangePrm;

typedef struct _VnfParam{
	int enable;
	int mode;
	int strength;
}VnfParam;

typedef struct {
    unsigned int numStrm;
    struct {
        unsigned int width;
        unsigned int height;
    } resolution[3]; /*This is the maximum streams supported */
} StreamResolution;

typedef struct
{
	CODEC_TYPE codecType;
	ENCPRESET_MODE encPreset;
	unsigned int width;
	unsigned int height;
	
}CodecCfg;

typedef struct
{
	unsigned int numStream;
	unsigned int restartFlag;
	CodecCfg codecCfg[3];
	
}VideoCodecCfg;

typedef struct
{
	unsigned int initDone;
	
}McFWInitStatus;

#if defined (__cplusplus)
}
#endif

#endif
/**
* @page MSG_QUEUE_HOW How to use message queue API?
* 1. Call the API Msg_Init( int msgKey ) to initialize message queue.\n
* 2. Now you can use any message queue API functions as you want.\n
* 3. Call the API Msg_Kill( int qid )  to cleanup message queue.
* @section MSG_QUEUE_HOW_EX Example
* @code
#include <Msg_Def.h>
int main()
{
	int qid, key;
	if((qid=Msg_Init(key)) < 0){
		return -1;
	}
	// More message queue API
	Msg_Kill(qid );
	return 0;
}
* @endcode
*/
