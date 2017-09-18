/** ===========================================================================
* @file main.c
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
#include <unistd.h>
#include <Msg_Def.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <system_control.h>
#include "share_mem.h"
#include <pthread.h>
#include <file_msg_drv.h>
#include <alarm_msg_drv.h>
#include <schedule_mng.h>
#include <stream_env_ctrl.h>
#include <sd_config.h>


#define SYSTEM_SERVER_BUFFER_LENGTH	256
#define FILE_THR_CREATED	0x1
#define ALARM_THR_CREATED	0x2
#ifdef DEBUG
#define DBG(fmt, args...)	printf("Debug " fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...)	printf("Error " fmt, ##args)
static int IsSysQuit = 0;
extern void *FileMngThread(void *arg);
extern void *AlramThread(void *arg);

/**
 * @brief	receive and process system message
 * @param	"SYS_MSG_BUF* pMsg"	[IN]buffer of message
 * @retval	1: Message is proccessed
 * @retval	0: Message has some error
 */
int ProcSysMsg(SYS_MSG_BUF* pMsg)
{
	int ret = 0;
	void *pData = NULL;
	char buffer[SYSTEM_SERVER_BUFFER_LENGTH];
	switch(pMsg->cmd){
		case SYS_MSG_GET_DHCP_EN:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetDhcpEn()) == NULL)
				break;
			pMsg->length = sizeof(int);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_HTTP_PORT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetHttpPort()) == NULL)
				break;
			pMsg->length = sizeof(unsigned short);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SNTP_FQDN:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFqdn()) == NULL)
				break;
			pMsg->length = strlen(pData) + 1;
			ret = 2;
			break;
		}
		case SYS_MSG_SET_NETMASK:
		{
			in_addr_t net_mask;
			ShareMemRead(pMsg->offset, &net_mask, pMsg->length);
			if(SetNetMask(net_mask) != 0){
				printf("Fail at SYS_MSG_SET_NETMASK\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DAYLIGHT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DAYLIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDaylightTime(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_DAYLIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TIMEZONE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TIMEZONE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTimeZone(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_TIMEZONE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_GET_TITLE:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetTitle()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_MAC:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetMac()) == NULL)
				break;
			pMsg->length = MAC_LENGTH;
			ret = 2;
			break;
		}
		case SYS_MSG_GET_IP:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetIp()) == NULL)
				break;
			pMsg->length = sizeof(in_addr_t);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_GATEWAY:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetGateWay()) == NULL)
				break;
			pMsg->length = sizeof(in_addr_t);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_NETMASK:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetNetmask()) == NULL)
				break;
			pMsg->length = sizeof(in_addr_t);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_SERVER_IP:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpServerIp()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_USERNAME:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpUsername()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_PASSWORD:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpPassword()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_FOLDNAME:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpFoldername()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_IMAGEAMOUNT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpImageAmount()) == NULL)
				break;
			pMsg->length = sizeof(int);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_PORT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpPort()) == NULL)
				break;
			pMsg->length = sizeof(__u16);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_FTP_PID:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetFtpPid()) == NULL)
				break;
			pMsg->length = sizeof(int);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_SERVER_IP:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpServerIp()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_SERVER_PORT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpServerPort()) == NULL)
				break;
			pMsg->length = sizeof(__u16);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_AUTHENTICATION:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpAuthentication()) == NULL)
				break;
			pMsg->length = sizeof(char);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_USERNAME:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetsmtpUsername()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_PASSWORD:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpPassword()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_SENDER_EMAIL_ADDRESS:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpSenderEmail()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_RECEIVER_EMAIL_ADDRESS:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpReceiverEmail()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_CC:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpCC()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SD_FILENAME:
		{
			/* Get current recording file name in SD card */
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSdFileName()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_SUBJECT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpSubject()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_TEXT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpText()) == NULL)
				break;
			pMsg->length = strlen(pData);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_ATTACHMENTS:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpAttachments()) == NULL)
				break;
			pMsg->length = sizeof(char);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_SMTP_VIEW:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetSmtpView()) == NULL)
				break;
			pMsg->length = sizeof(char);
			ret = 2;
			break;
		}
		case SYS_MSG_GET_DHCP_CONFIG:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetDhcpCinfig()) == NULL)
				break;
			pMsg->length = sizeof(int);
			ret = 2;
			break;
		}
	/**	case SYS_MSG_GET_UPNP_PORT:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetUpnpPort()) == NULL)
				break;
			pMsg->length = sizeof(unsigned short);
			ret = 2;
			break;
		}*/
		case SYS_MSG_SET_IP:
		{
			in_addr_t ip;
			if(pMsg->length != sizeof(ip))
				break;
			ShareMemRead(pMsg->offset, &ip, pMsg->length);
			if(SetIp(ip) != 0){
				printf("Fail at SYS_MSG_SET_IP\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_HTTPPORT:
		{
			unsigned short port;
			if(pMsg->length != sizeof(port))
				break;
			ShareMemRead(pMsg->offset, &port, pMsg->length);
			if(SetHttpPort(port) != 0){
				printf("Fail at SYS_MSG_SET_HTTPPORT\n");
				break;
			}
			/* No result to boa */
			if(pMsg->src == SYS_BOA_MSG)
				pMsg->src = 0;
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TITLE:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetTitle(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_TITLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_GATEWAY:
		{
			in_addr_t gateway;
			if(pMsg->length != sizeof(gateway))
				break;
			ShareMemRead(pMsg->offset, &gateway, pMsg->length);
			if(SetGateway(gateway) != 0){
				printf("Fail at SYS_MSG_SET_GATEWAY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DHCPC_ENABLE:
		{
			int enable;
			if(pMsg->length != sizeof(enable))
				break;
			ShareMemRead(pMsg->offset, &enable, pMsg->length);
			if(SetDhcpEnable(enable) != 0){
				printf("Fail at SYS_MSG_SET_DHCPC_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_FQDN:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetFtpFqdn(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_FTP_FQDN\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_USERNAME:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetFtpUsername(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_FTP_USERNAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_PASSWORD:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetFtpPassword(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_FTP_PASSWORD\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_FOLDNAME:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetFtpFoldname(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_FTP_FOLDNAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_IMAGEAMOUNT:
		{
			int imageacount;
			if(pMsg->length != sizeof(imageacount))
				break;
			ShareMemRead(pMsg->offset, &imageacount, pMsg->length);
			if(SetFtpImageacount(imageacount) != 0){
				printf("Fail at SYS_MSG_SET_FTP_IMAGEAMOUNT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_PORT:
		{
			unsigned short pid;
			if(pMsg->length != sizeof(pid))
				break;
			ShareMemRead(pMsg->offset, &pid, pMsg->length);
			if(SetFtpPort(pid) != 0){
				printf("Fail at SYS_MSG_SET_FTP_PORT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_PID:
		{
			int pid;
			if(pMsg->length != sizeof(pid))
				break;
			ShareMemRead(pMsg->offset, &pid, pMsg->length);
			if(SetFtpPid(pid) != 0){
				printf("Fail at SYS_MSG_SET_FTP_PID\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_SERVER_IP:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpServerIp(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_USERNAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_SERVER_PORT:
		{
			unsigned short pid;
			if(pMsg->length != sizeof(pid))
				break;
			ShareMemRead(pMsg->offset, &pid, pMsg->length);
			if(SetSmtpServerPort(pid) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_SERVER_PORT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_AUTHENTICATION:
		{
			unsigned char authentication;
			if(pMsg->length != sizeof(authentication))
				break;
			ShareMemRead(pMsg->offset, &authentication, pMsg->length);
			if(SetSmtpAuthentication(authentication) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_AUTHENTICATION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_USERNAME:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpUsername(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_USERNAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_PASSWORD:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpPassword(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_PASSWORD\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_SENDER_EMAIL_ADDRESS:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpSenderEmail(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_SENDER_EMAIL_ADDRESS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_RECEIVER_EMAIL_ADDRESS:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpReceiverEmail(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_RECEIVER_EMAIL_ADDRESS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_CC:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpCC(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_CC\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_SUBJECT:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpSubject(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_SUBJECT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_TEXT:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSmtpText(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_TEXT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_ATTACHMENTS:
		{
			unsigned char attachments;
			if(pMsg->length != sizeof(attachments))
				break;
			ShareMemRead(pMsg->offset, &attachments, pMsg->length);
			if(SetSmtpAttachments(attachments) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_ATTACHMENTS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTP_VIEW:
		{
			unsigned char view;
			if(pMsg->length != sizeof(view))
				break;
			ShareMemRead(pMsg->offset, &view, pMsg->length);
			if(SetSmtpView(view) != 0){
				printf("Fail at SYS_MSG_SET_SMTP_VIEW\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_GET_DNS:
		{
			if(pMsg->src <= MSG_TYPE_MSG1)
				break;
			if((pData = GetDns()) == NULL)
				break;
			pMsg->length = sizeof(in_addr_t);
			ret = 2;
			break;
		}
		case SYS_MSG_SET_DNS:
		{
			in_addr_t ip;
			if(pMsg->length != sizeof(ip))
				break;
			ShareMemRead(pMsg->offset, &ip, pMsg->length);
			if(SetDns(ip) != 0){
				printf("Fail at SYS_MSG_SET_DNS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_HOSTNAME:
		{
		  	hostname_tmp hostname;
                        if(pMsg->length != sizeof(hostname))
                                break;
                        ShareMemRead(pMsg->offset, &hostname, pMsg->length);
                        if(set_hostname(&hostname) != 0){
                                printf("Fail at SYS_MSG_SET_HSTNAME\n");
                                break;
                        }
                        ret = 1;
                        break;
		}
		case SYS_MSG_SET_SNTP_SERVER_IP:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSntpServer(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SNTP_SERVER_IP\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DAY_NIGHT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DAY_NIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamDayNight(value) != 0){
				printf("Fail at SYS_MSG_SET_DAY_NIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_WHITE_BALANCE:
		{
			unsigned char value;
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			DBG("SYS_MSG_SET_WHITE_BALANCE :%d\n", value);
			if(SetCamWhiteBalance(value) != 0){
				printf("Fail at SYS_MSG_SET_WHITE_BALANCE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_BACKLIGHT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_BACKLIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamBacklight(value) != 0){
				printf("Fail at SYS_MSG_SET_BACKLIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_BRIGHTNESS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_BRIGHTNESS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamBrightness(value) != 0){
				printf("Fail at SYS_MSG_SET_BRIGHTNESS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_CONTRAST:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_CONTRAST\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamContrast(value) != 0){
				printf("Fail at SYS_MSG_SET_CONTRAST\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SATURATION:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SATURATION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamSaturation(value) != 0){
				printf("Fail at SYS_MSG_SET_SATURATION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SHARPNESS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SHARPNESS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetCamSharpness(value) != 0){
				printf("Fail at SYS_MSG_SET_SHARPNESS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_IMAGING_CONF:
		{
			Imaging_Conf_tmp imaging_conf_tmp;
			DBG("SYS_MSG_SET_IMAGING_CONF\n");
			if(pMsg->length != sizeof(imaging_conf_tmp))
				break;
			ShareMemRead(pMsg->offset, &imaging_conf_tmp, pMsg->length);
			if(SetImagingSettings(&imaging_conf_tmp) != 0){
				printf("Fail at SYS_MSG_SET_IMAGING_CONF\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_CLEARSYSLOG:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_CLEARSYSLOG\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetClearSysLog(value) != 0){
				printf("Fail at SYS_MSG_SET_CLEARSYSLOG\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_CLEARACCESSLOG:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_CLEARACCESSLOG\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetClearAccessLog(value) != 0){
				printf("Fail at SYS_MSG_SET_CLEARACCESSLOG\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_VSTAB:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VSTAB\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetVstabValue(value) != 0){
				printf("Fail at SYS_MSG_SET_VSTAB\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LDC:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LDC\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLdcValue(value) != 0){
				printf("Fail at SYS_MSG_SET_LDC\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_VNF:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VNF\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetVnfValue(value) != 0){
				printf("Fail at SYS_MSG_SET_VNF\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_VNFMODE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VNFMODE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetVnfMode(value) != 0){
				printf("Fail at SYS_MSG_SET_VNFMODE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_VNFSTRENGTH:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VNFSTRENGTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetVnfStrength(value) != 0){
				printf("Fail at SYS_MSG_SET_VNFSTRENGTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DYNRANGE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DYNRANGE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDynRange(value) != 0){
				printf("Fail at SYS_MSG_SET_DYNRANGE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DYNRANGESTRENGTH:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DYNRANGESTRENGTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDynRangeStrength(value) != 0){
				printf("Fail at SYS_MSG_SET_DYNRANGESTRENGTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FRAMECTRL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRAMECTRL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFramectrlValue(value) != 0){
				printf("Fail at SYS_MSG_SET_FRAMECTRL\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOMODE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIOMODE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioMode(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOMODE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIO_ENCODE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIO_ENCODE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioEncode(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIO_ENCODE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIO_SAMPLERATE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIO_SAMPLERATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioSampleRate(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIO_SAMPLERATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIO_BITRATE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIO_BITRATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioBitrate(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIO_BITRATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIO_ALARMLEVEL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIO_ALARMLEVEL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioAlarmLevel(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIO_ALARMLEVEL\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOOUTVOLUME:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIOOUTVOLUME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioOutVolume(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOOUTVOLUME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DEFAULT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DEFAULT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetSystemDefault(value) != 0){
				printf("Fail at SYS_MSG_SET_DEFAULT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_JPEG_QUALITY:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_JPEG_QUALITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMJPEGQuality(value) != 0){
				printf("Fail at SYS_MSG_SET_JPEG_QUALITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MIRROR:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MIRROR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMirror(value) != 0){
				printf("Fail at SYS_MSG_SET_MIRROR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DEMOCFG:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DEMOCFG\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDemoCfg(value) != 0){
				printf("Fail at SYS_MSG_SET_DEMOCFG\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_EXPPRIORITY:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_EXPPRIORITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetExpPriority(value) != 0){
				printf("Fail at SYS_MSG_SET_EXPPRIORITY\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_ALARMLOCAL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ALARMLOCAL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAlarmStorage(value) != 0){
				printf("Fail at SYS_MSG_SET_ALARMLOCAL\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_RECORDLOCAL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_RECORDLOCAL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetRecordStorage(value) != 0){
				printf("Fail at SYS_MSG_SET_RECORDLOCAL\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_CLICK_SNAP_STORAGE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_CLICK_SNAP_STORAGE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetClickSnapStorage(value) != 0){
				printf("Fail at SYS_MSG_SET_CLICK_SNAP_STORAGE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_CLICK_SNAP_FILENAME:
		{
			DBG("SYS_MSG_SET_CLICK_SNAP_FILENAME\n");
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetClickSnapFilename(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_CLICK_SNAP_FILENAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_RATE_CONTROL1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_RATE_CONTROL1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetRateControl1(value) != 0){
				printf("Fail at SYS_MSG_SET_RATE_CONTROL1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_RATE_CONTROL2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_RATE_CONTROL2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetRateControl2(value) != 0){
				printf("Fail at SYS_MSG_SET_RATE_CONTROL2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DATE_STAMP_ENABLE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DATE_STAMP_ENABLE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDateStampEnable1(value) != 0){
				printf("Fail at SYS_MSG_SET_DATE_STAMP_ENABLE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DATE_STAMP_ENABLE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DATE_STAMP_ENABLE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDateStampEnable2(value) != 0){
				printf("Fail at SYS_MSG_SET_DATE_STAMP_ENABLE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DATE_STAMP_ENABLE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DATE_STAMP_ENABLE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDateStampEnable3(value) != 0){
				printf("Fail at SYS_MSG_SET_DATE_STAMP_ENABLE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TIME_STAMP_ENABLE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TIME_STAMP_ENABLE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTimeStampEnable1(value) != 0){
				printf("Fail at SYS_MSG_SET_TIME_STAMP_ENABLE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TIME_STAMP_ENABLE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TIME_STAMP_ENABLE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTimeStampEnable2(value) != 0){
				printf("Fail at SYS_MSG_SET_TIME_STAMP_ENABLE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TIME_STAMP_ENABLE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TIME_STAMP_ENABLE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTimeStampEnable3(value) != 0){
				printf("Fail at SYS_MSG_SET_TIME_STAMP_ENABLE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOGO_ENABLE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_ENABLE1\n");
//			printf("\r\nDEBUG: Inside main.c   SYS_MSG_SET_LOGO_ENABLE1--------------------------------\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoEnable1(value) != 0){
				printf("Fail at SYS_MSG_SET_LOGO_ENABLE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOGO_ENABLE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_ENABLE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoEnable2(value) != 0){
				printf("Fail at SYS_MSG_SET_LOGO_ENABLE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOGO_ENABLE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_ENABLE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoEnable3(value) != 0){
				printf("Fail at SYS_MSG_SET_LOGO_ENABLE3\n");
				break;
			}
			ret = 1;
			break;
		}
              case SYS_MSG_SET_LOGO_POSITION1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_POSITION1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoPosition1(value) != 0){
				printf("Fail at SYS_MSG_SET_LOGO_POSITION1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOGO_POSITION2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_POSITION2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoPosition2(value) != 0){
				printf("Fail at Fail at  SYS_MSG_SET_LOGO_POSITION2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOGO_POSITION3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOGO_POSITION3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLogoPosition3(value) != 0){
				printf("Fail at  SYS_MSG_SET_LOGO_POSITION3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_ENABLE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_ENABLE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextEnable1(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_ENABLE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_ENABLE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_ENABLE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextEnable2(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_ENABLE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_ENABLE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_ENABLE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextEnable3(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_ENABLE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_OVERLAY_TEXT1:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("SYS_MSG_SET_OVERLAY_TEXT1 :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetOverlayText1(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_OVERLAY_TEXT1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_OVERLAY_TEXT2:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("SYS_MSG_SET_OVERLAY_TEXT2 :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetOverlayText2(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_OVERLAY_TEXT2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_OVERLAY_TEXT3:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("SYS_MSG_SET_OVERLAY_TEXT3 :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetOverlayText3(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_OVERLAY_TEXT3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_POSITION1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_POSITION1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextPosition1(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_POSITION1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_POSITION2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_POSITION2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextPosition2(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_POSITION2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TEXT_POSITION3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TEXT_POSITION3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTextPosition3(value) != 0){
				printf("Fail at SYS_MSG_SET_TEXT_POSITION3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ENCRYPTION:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ENCRYPTION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetEncryption(value) != 0){
				printf("Fail at SYS_MSG_SET_ENCRYPTION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOCAL_DISPLAY:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOCAL_DISPLAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLocalDisplay(value) != 0){
				printf("Fail at SYS_MSG_SET_LOCAL_DISPLAY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_IPRATIO1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_IPRATIO1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetIpratio1(value) != 0){
				printf("Fail at SYS_MSG_SET_IPRATIO1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_IPRATIO2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_IPRATIO2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetIpratio2(value) != 0){
				printf("Fail at SYS_MSG_SET_IPRATIO2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_IPRATIO3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_IPRATIO3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetIpratio3(value) != 0){
				printf("Fail at SYS_MSG_SET_IPRATIO3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FORCE_IFRAME1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FORCE_IFRAME1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetForceIframe1(value) != 0){
				printf("Fail at SYS_MSG_SET_FORCE_IFRAME1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FORCE_IFRAME2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FORCE_IFRAME2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetForceIframe2(value) != 0){
				printf("Fail at SYS_MSG_SET_FORCE_IFRAME2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FORCE_IFRAME3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FORCE_IFRAME3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetForceIframe3(value) != 0){
				printf("Fail at SYS_MSG_SET_FORCE_IFRAME3\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_QPINIT1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPINIT1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPInit1(value) != 0){
				printf("Fail at SYS_MSG_SET_QPINIT1\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_QPINIT2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPINIT2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPInit2(value) != 0){
				printf("Fail at SYS_MSG_SET_QPINIT2\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_QPINIT3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPINIT3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPInit3(value) != 0){
				printf("Fail at SYS_MSG_SET_QPINIT3\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_QPMIN1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMIN1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMin1(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMIN1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_QPMIN2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMIN2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMin2(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMIN2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_QPMIN3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMIN3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMin3(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMIN3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_QPMAX1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMAX1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMax1(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMAX1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_QPMAX2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMAX2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMax2(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMAX2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_QPMAX3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_QPMAX3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetQPMax3(value) != 0){
				printf("Fail at SYS_MSG_SET_QPMAX3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MECONFIG1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MECONFIG1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMEConfig1(value) != 0){
				printf("Fail at SYS_MSG_SET_MECONFIG1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MECONFIG2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MECONFIG2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMEConfig2(value) != 0){
				printf("Fail at SYS_MSG_SET_MECONFIG2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MECONFIG3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MECONFIG3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMEConfig3(value) != 0){
				printf("Fail at SYS_MSG_SET_MECONFIG3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_PACKET_SIZE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_PACKET_SIZE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetPacketSize1(value) != 0){
				printf("Fail at SYS_MSG_SET_PACKET_SIZE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_PACKET_SIZE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_PACKET_SIZE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetPacketSize2(value) != 0){
				printf("Fail at SYS_MSG_SET_PACKET_SIZE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_PACKET_SIZE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_PACKET_SIZE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetPacketSize3(value) != 0){
				printf("Fail at SYS_MSG_SET_PACKET_SIZE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ROI_ENABLE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ROI_ENABLE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetROIEnable1(value) != 0){
				printf("Fail at SYS_MSG_SET_ROI_ENABLE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ROI_ENABLE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ROI_ENABLE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetROIEnable2(value) != 0){
				printf("Fail at SYS_MSG_SET_ROI_ENABLE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ROI_ENABLE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ROI_ENABLE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetROIEnable3(value) != 0){
				printf("Fail at SYS_MSG_SET_ROI_ENABLE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1X1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1X1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1X1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1X1\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR1Y1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1Y1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1Y1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1Y1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1W1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1W1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1W1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1W1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1H1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1H1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1H1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1H1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1X2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1X2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1X2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1X2\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR1Y2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1Y2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1Y2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1Y2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1W2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1W2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1W2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1W2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1H2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1H2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1H2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1H2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1X3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1X3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1X3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1X3\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR1Y3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1Y3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1Y3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1Y3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1W3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1W3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1W3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1W3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR1H3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR1H3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr1H3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR1H3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2X1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2X1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2X1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2X1\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR2Y1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2Y1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2Y1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2Y1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2W1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2W1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2W1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2W1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2H1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2H1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2H1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2H1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2X2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2X2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2X2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2X2\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR2Y2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2Y2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2Y2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2Y2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2W2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2W2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2W2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2W2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2H2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2H2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2H2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2H2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2X3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2X3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2X3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2X3\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR2Y3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2Y3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2Y3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2Y3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2W3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2W3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2W3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2W3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR2H3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR2H3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr2H3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR2H3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3X1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3X1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3X1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3X1\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR3Y1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3Y1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3Y1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3Y1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3W1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3W1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3W1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3W1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3H1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3H1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3H1(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3H1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3X2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3X2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3X2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3X2\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR3Y2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3Y2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3Y2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3Y2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3W2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3W2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3W2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3W2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3H2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3H2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3H2(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3H2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3X3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3X3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3X3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3X3\n");
				break;
			}
			ret = 1;
			break;
		}
        case SYS_MSG_SET_STR3Y3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3Y3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3Y3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3Y3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3W3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3W3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3W3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3W3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_STR3H3:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_STR3H3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetStr3H3(value) != 0){
				printf("Fail at SYS_MSG_SET_STR3H3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DETAIL_INFO1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DETAIL_INFO1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDetailInfo1(value) != 0){
				printf("Fail at SYS_MSG_SET_DETAIL_INFO1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DETAIL_INFO2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DETAIL_INFO2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDetailInfo2(value) != 0){
				printf("Fail at SYS_MSG_SET_DETAIL_INFO2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DETAIL_INFO3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DETAIL_INFO3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDetailInfo3(value) != 0){
				printf("Fail at SYS_MSG_SET_DETAIL_INFO3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_HISTOGRAM:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_HISTOGRAM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetHistogram(value) != 0){
				printf("Fail at SYS_MSG_SET_HISTOGRAM\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_VIDEO_MODE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VIDEO_MODE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);

			int server_status = SetVideoMode(value);

			if(server_status < 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_VIDEO_MODE\n");
				break;
			}else if(server_status == 1){
				pMsg->length = 0xFF;
				ret = 1;
			}else{
				ret = 1;
			}
			break;
		}
		case SYS_MSG_SET_VIDEOCODEC_COMBO:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VIDEOCODEC_COMBO\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);

			int server_status = SetVideoCodecCombo(value);

			if(server_status < 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_VIDEOCODEC_COMBO\n");
				break;
			}else if(server_status == 1){
				pMsg->length = 0xFF;
				ret = 1;
			}else{
				ret = 1;
			}
			break;
		}
		case SYS_MSG_SET_VIDEOCODEC_RES:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_VIDEOCODEC_RES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			int server_status = SetVideoCodecRes(value);
			if( server_status < 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_VIDEOCODEC_RES\n");
				break;
			}else if(server_status == 1){
				pMsg->length = 0xFF;
				ret = 1;
			}else{
				ret = 1;
			}
			break;
		}
		case SYS_MSG_SET_IMAGE_SOURCE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_IMAGE_SOURCE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetImagesource(value) != 0){
				printf("Fail at SYS_MSG_SET_IMAGE_SOURCE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SCHEDULE:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetSchedule(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_SCHEDULE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_DEL_SCHEDULE:
		{
			int bEnable;
			DBG("SYS_MSG_DEL_SCHEDULE\n");
			if(pMsg->length != sizeof(bEnable))
				break;
			ShareMemRead(pMsg->offset, &bEnable, pMsg->length);
			if(DelSchedule(bEnable) != 0){
				printf("Fail at SYS_MSG_DEL_SCHEDULE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AVIDURATION:
		{
			__u8 value;
			DBG("SYS_MSG_SET_AVIDURATION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAVIDuration(value) != 0){
				printf("Fail at SYS_MSG_SET_AVIDURATION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AVIFORMAT:
		{
			__u8 value;
			DBG("SYS_MSG_SET_AVIFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAVIFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_AVIFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTPFILEFORMAT:
		{
			__u8 value;
			DBG("SYS_MSG_SET_FTPFILEFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFTPFileFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_FTPFILEFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SDFILEFORMAT:
		{
			__u8 value;
			DBG("SYS_MSG_SET_SDFILEFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetSDFileFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_SDFILEFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ATTFILEFORMAT:
		{
			__u8 value;
			DBG("SYS_MSG_SET_ATTFILEFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAttFileFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_ATTFILEFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOENABLE:
		{
			__u8 value;
			DBG("SYS_MSG_SET_AUDIOENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioON(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ASMTPATTACH:
		{
			__u8 value;
			DBG("SYS_MSG_SET_ASMTPATTACH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetASmtpAttach(value) != 0){
				printf("Fail at SYS_MSG_SET_ASMTPATTACH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTP_RFTPENABLE:
		{
			__u8 value;
			DBG("SYS_MSG_SET_FTP_RFTPENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetRftpenable(value) != 0){
				printf("Fail at SYS_MSG_SET_FTP_RFTPENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SD_REENABLE:
		{
			__u8 value;
			DBG("SYS_MSG_SET_SD_REENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetSdReEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_SD_REENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_LOSTALARM:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_LOSTALARM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetLostAlarm(value) != 0){
				printf("Fail at SYS_MSG_SET_LOSTALARM\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SDAENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SDAENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetSDAlarmEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_SDAENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MOTIONENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MOTIONENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMotionDetect(value) != 0){
				printf("Fail at SYS_MSG_SET_MOTIONENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MOTIONCENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MOTIONCENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMotionDetCEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_MOTIONCENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MOTIONLEVEL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MOTIONLEVEL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMotionDetLevel(value) != 0){
				printf("Fail at SYS_MSG_SET_MOTIONLEVEL\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MOTIONCVALUE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MOTIONCVALUE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMotionDetCValue(value) != 0){
				printf("Fail at SYS_MSG_SET_MOTIONCVALUE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_MOTIONBLOCK:
		{
			__u8 tempBuff[MOTION_BLK_LEN];
			int i;
			DBG("SYS_MSG_SET_MOTIONBLOCK\n");
			if(pMsg->length >  MOTION_BLK_LEN) {
				printf("Fail at SYS_MSG_SET_MOTIONBLOCK: %d %d\n",pMsg->length, MOTION_BLK_LEN);
				break;
			}
			ShareMemRead(pMsg->offset, tempBuff, pMsg->length);
			if(SetMotionDetectBlock((char *)tempBuff,pMsg->length) != 0){
				for(i =0;i<pMsg->length;i++){
					printf("%x,", tempBuff[i]);
				}
				printf("Fail at SYS_MSG_SET_MOTIONBLOCK\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDETECT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FDETECT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFaceDetect(value) != 0){
				printf("Fail at SYS_MSG_SET_FDETECT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDX:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_FDX\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDX(value) != 0){
				printf("Fail at SYS_MSG_SET_FDX\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_FDY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDY(value) != 0){
				printf("Fail at SYS_MSG_SET_FDY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDW:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_FDW\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDW(value) != 0){
				printf("Fail at SYS_MSG_SET_FDW\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_FDH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDH(value) != 0){
				printf("Fail at SYS_MSG_SET_FDH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FDCONF_LEVEL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FDCONF_LEVEL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDConfLevel(value) != 0){
				printf("Fail at SYS_MSG_SET_FDCONF_LEVEL\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FD_DIRECTION:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FD_DIRECTION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFDDirection(value) != 0){
				printf("Fail at SYS_MSG_SET_FD_DIRECTION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FRECOGNITION:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRECOGNITION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFRecognition(value) != 0){
				printf("Fail at SYS_MSG_SET_FRECOGNITION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FRCONF_LEVEL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRCONF_LEVEL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFRConfLevel(value) != 0){
				printf("Fail at SYS_MSG_SET_FRCONF_LEVEL\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FR_DATABASE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FR_DATABASE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFRDatabase(value) != 0){
				printf("Fail at SYS_MSG_SET_FR_DATABASE\n");
				break;
			}
			ret = 1;
			break;
		}
	    case SYS_MSG_SET_PRIVACY_MASK:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_PRIVACY_MASK\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetPrivacyMask(value) != 0){
				printf("Fail at SYS_MSG_SET_PRIVACY_MASK\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MASK_OPTIONS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MASK_OPTIONS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMaskOptions(value) != 0){
				printf("Fail at SYS_MSG_SET_MASK_OPTIONS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FTPAENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FTPAENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFTPAlarmEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_FTPAENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SMTPAENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SMTPAENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetSMTPAlarmEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_SMTPAENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_2A_SWITCH:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_2A_SWITCH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			int server_status = SetImageAEW(value);
			if(server_status < 0){
				printf("Fail at SYS_MSG_SET_2A_SWITCH\n");
				break;
			}else if(server_status == 1){
				pMsg->length = 0xFF;
				ret = 1;
			}else{
				ret = 1;
			}
			break;
		}
		case SYS_MSG_SET_2A_TYPE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_2A_TYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetImageAEWType(value) != 0){
				printf("Fail at SYS_MSG_SET_2A_TYPE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_BINNING:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_BINNING\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			int server_status = SetBinning(value);
			if(server_status < 0){
				printf("Fail at SYS_MSG_SET_BINNING\n");
				break;
			}else if(server_status == 1){
				pMsg->length = 0xFF;
				ret = 1;
			}else{
				ret = 1;
			}
			break;
		}
		case SYS_MSG_SET_BLC:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_BLC\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetBlc(value) != 0){
				printf("Fail at SYS_MSG_SET_BLC\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ALARMDURATION:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ALARMDURATION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAlarmDuration(value) != 0){
				printf("Fail at SYS_MSG_SET_ALARMDURATION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MPEG41_BITRATE:
		{
			int value;
			DBG("SYS_MSG_SET_MPEG41_BITRATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMP41bitrate(value) != 0){
				printf("Fail at SYS_MSG_SET_MPEG41_BITRATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MPEG42_BITRATE:
		{
			int value;
			DBG("SYS_MSG_SET_MPEG42_BITRATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetMP42bitrate(value) != 0){
				printf("Fail at SYS_MSG_SET_MPEG42_BITRATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_DEL_USER:
		{
			DBG("SYS_MSG_DEL_USER\n");
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			buffer[pMsg->length - 1] = '\0';
			if(strlen(buffer) > USER_LEN){
				ERR("Invalid user name\n");
				break;
			}
			if(DelUser(buffer) != 0){
				ERR("Fail at SYS_MSG_DEL_USER\n");
				break;
			}
			pMsg->length = 0;
			ret = 1;
			break;
		}
		case SYS_MSG_ADD_USER:
		{
			Acount_t value;
			DBG("SYS_MSG_ADD_USER\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			DBG("User:%s\n", value.user);
			DBG("Password:%s\n", value.password);
			DBG("authority:%d\n", value.authority);
			if(AddUser(&value) != 0){
				printf("Fail at SYS_MSG_ADD_USER\n");
				break;
			}
			pMsg->length = 0;
			ret = 1;
			break;
		}
		case SYS_MSG_SET_GIOINENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_GIOINENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetGIOInEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_GIOINENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_GIOINTYPE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_GIOINTYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetGIOInType(value) != 0){
				printf("Fail at SYS_MSG_SET_GIOINTYPE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_GIOOUTENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_GIOOUTENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetGIOOutEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_GIOOUTENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_GIOOUTTYPE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_GIOOUTTYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetGIOOutType(value) != 0){
				printf("Fail at SYS_MSG_SET_GIOOUTTYPE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DATEFORMAT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DATEFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDateFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_DATEFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TSTAMPFORMAT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TSTAMPFORMAT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTStampFormat(value) != 0){
				printf("Fail at SYS_MSG_SET_TSTAMPFORMAT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DATEPPOS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_DATEPPOS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDatePosition(value) != 0){
				printf("Fail at SYS_MSG_SET_DATEPPOS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_TIMEPOS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_TIMEPOS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetTimePosition(value) != 0){
				printf("Fail at SYS_MSG_SET_TIMEPOS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOINVOLUME:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIOINVOLUME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioInVolume(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOINVOLUME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_IMAGE_DEFAULT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_IMAGE_DEFAULT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetImageParaDefault(value) != 0){
				printf("Fail at SYS_MSG_SET_IMAGE_DEFAULT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_DO_LOGIN:
		{
			login_data_t	prm;
			DBG("SYS_MSG_DO_LOGIN\n");
			if(pMsg->length != sizeof(prm))
				break;
			ShareMemRead(pMsg->offset, &prm, pMsg->length);
			if(DoLogin(&prm))
				break;
			ret = 1;
			break;
		}
		case SYS_MSG_SD_FORMAT:
		{
			int prm;
			DBG("SYS_MSG_SD_FORMAT\n");
			if(pMsg->length != sizeof(prm))
				break;
			ShareMemRead(pMsg->offset, &prm, pMsg->length);
			if(prm != 1)
				break;
			if(SdFormat())
				break;
			ret = 1;
			break;
		}
		case SYS_MSG_SD_UNMOUNT:
		{
			unsigned char value;
			DBG("SYS_MSG_SD_UNMOUNT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SdUnmount(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SD_UNMOUNT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_GET_USER_AUTHORITY:
		{
			get_user_authority_t	prm;
			DBG("SYS_MSG_GET_USER_AUTHORITY\n");
			if(pMsg->length != sizeof(prm))
				break;
			ShareMemRead(pMsg->offset, &prm, pMsg->length);
			prm.authority= GetUserAuthority(prm.user_id);
			ShareMemWrite(pMsg->offset, &prm, pMsg->length);
			ret = 1;
			break;
		}

		case SYS_MSG_SCHEDULE_STOP:
		{
			ScheduleStop();
			DBG("SYS_MSG_SCHEDULE_STOP\n");
			ret = 1;
			break;
		}

		case SYS_MSG_SCHEDULE_RESUME:
		{
			ResumeSchedule();
			DBG("SYS_MSG_SCHEDULE_RESUME\n");
			ret = 1;
			break;
		}

		case SYS_MSG_QUIT:
		{
			IsSysQuit = 1;
			pMsg->src = 0;
			break;
		}
		case SYS_MSG_SET_FRAMERATE1:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRAMERATE1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFramerate1(value) != 0){
				printf("Fail at SYS_MSG_SET_FRAMERATE1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FRAMERATE2:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRAMERATE2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFramerate2(value) != 0){
				printf("Fail at SYS_MSG_SET_FRAMERATE2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_FRAMERATE3:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_FRAMERATE3\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetFramerate3(value) != 0){
				printf("Fail at SYS_MSG_SET_FRAMERATE3\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_MULTICAST:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_MULTICAST\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetNetMulticast(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_MULTICAST\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_DO_BOOT_PROC:
		{
			int value;
			DBG("SYS_MSG_DO_BOOT_PROC\n");
			//if(pMsg->length != sizeof(value))
			//	break;
			//ShareMemRead(pMsg->offset, &value, pMsg->length);
			value = pMsg->length;
			if(DoBootProc(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_DO_BOOT_PROC\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_HTTPSPORT:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_HTTPSPORT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetHttpsPort(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_HTTPSPORT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_RS485:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_RS485\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetRS485Port(value) != 0){
				printf("\nSystemServer:Fail at SYS_MSG_SET_RS485\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ALARM_ENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ALARM_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAlarmEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_ALARM_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_EXT_ALARM:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_EXT_ALARM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetExtAlarm(value) != 0){
				printf("Fail at SYS_MSG_SET_EXT_ALARM\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOALARM:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIOALARM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetAudioAlarm(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOALARM\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ALARM_AUDIOPLAY:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ALARM_AUDIOPLAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetAlarmAudioPlay(value) != 0){
				printf("Fail at SYS_MSG_SET_ALARM_AUDIOPLAY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_ALARM_AUDIOFILE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_ALARM_AUDIOFILEY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetAlarmAudioFile(value) != 0){
				printf("Fail at SYS_MSG_SET_ALARM_AUDIOFILE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SCHEDULE_REPEAT_ENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SCHEDULE_REPEAT_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetScheduleRepeatEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_SCHEDULE_REPEAT_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SCHEDULE_NUM_WEEKS:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SCHEDULE_NUM_WEEKS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetScheduleNumWeeks(value) != 0){
				printf("Fail at SYS_MSG_SET_SCHEDULE_NUM_WEEKS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_SCHEDULE_INFINITE_ENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_SCHEDULE_INFINITE_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(Avs_SetScheduleInfiniteEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_SCHEDULE_INFINITE_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIORECEIVER_ENABLE:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUDIORECEIVER_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAudioReceiverEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_AUDIORECEIVER_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUDIOSERVER_IP:
		{
			if(pMsg->length > SYSTEM_SERVER_BUFFER_LENGTH){
				printf("SYS_MSG_SET_AUDIOSERVER_IP :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetAudioSeverIp(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_AUDIOSERVER_IP\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_AUTOIRIS_VOL:
		{
			unsigned char value;
			DBG("SYS_MSG_SET_AUTOIRIS_VOL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetAutoIrisVol(value) != 0){
				printf("Fail at SYS_MSG_SET_AUTOIRIS_VOL\n");
				break;
			}
			ret = 1;
			break;
		}

                /* ONVIF START */
		case SYS_MSG_ADD_PROFILE:
                {
			media_profile profileacount;
                      DBG("SYS_MSG_ADD_PROFILETOKEN\n");
                      if(pMsg->length != sizeof(profileacount))
                            break;
                      ShareMemRead(pMsg->offset,&profileacount, pMsg->length);
                        if(AddMediaProfile(&profileacount) != 0){
                                printf("Fail at SYS_MSG_ADD_PROFILE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_SET_VIDEOSOURCE_CONF:
                {
                       video_source_conf video_source_conf_t;
                        DBG("SYS_MSG_ADD_VIDEOSOURCE_CONF\n");
                        if(pMsg->length != sizeof(video_source_conf_t))
                            break;
                         ShareMemRead(pMsg->offset,&video_source_conf_t, pMsg->length);
                        if(Addvideosourceconfiguration(&video_source_conf_t) != 0){
                                printf("Fail at SYS_MSG_ADD_VIDEOSOURCE_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_ADD_VIDEOSOURCE_CONF:
                {
                        video_source_conf set_video_source_conf_t;
                        DBG("SYS_MSG_ADD_AUDIOSOURCE_CONF\n");
                        if(pMsg->length != sizeof(set_video_source_conf_t))
                            break;
                         ShareMemRead(pMsg->offset,&set_video_source_conf_t, pMsg->length);
                        if(Setvideosourceconfiguration(&set_video_source_conf_t) != 0){
                                printf("Fail at SYS_MSG_ADD_PROFILE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_ADD_METADATA:
                {
                       Metadata_configuration_tmp metadata;
                        DBG("SYS_MSG_ADD_METADATA\n");
                        if(pMsg->length != sizeof(metadata))
                            break;
                         ShareMemRead(pMsg->offset,&metadata, pMsg->length);
                        if(Addmetadataconfiguration(&metadata) != 0){
                                printf("Fail at SYS_MSG_ADD_METADATA\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_SET_METADATA:
                {
                        Metadata_configuration_tmp metadata;
                        DBG("SYS_MSG_SET_METADATA\n");
                        if(pMsg->length != sizeof(metadata))
                            break;
                         ShareMemRead(pMsg->offset,&metadata, pMsg->length);
                        if(SetMetadataConf(&metadata) != 0){
                                printf("Fail at SYS_MSG_METADATA\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_DEL_METADATA:
                {
                       Metadata_configuration_tmp metadata;
                        DBG("SYS_MSG_DEL_METADATA\n");
                        if(pMsg->length != sizeof(metadata))
                            break;
                         ShareMemRead(pMsg->offset,&metadata, pMsg->length);
                        if(Delmetadataconfiguration(&metadata) != 0){
                                printf("Fail at SYS_MSG_DEL_METADATA\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_ADD_AUDIO_SOURCE_CONF:
                {
                       Add_AudioSource_Configuration_tmp Add_Audio_Conf;
                        DBG("SYS_MSG_ADD_AUDIO_SOURCE_CONF\n");
                        if(pMsg->length != sizeof(Add_Audio_Conf))
                            break;
                         ShareMemRead(pMsg->offset,&Add_Audio_Conf, pMsg->length);
                        if(addaudiosourceconfiguration(&Add_Audio_Conf) != 0){
                                printf("Fail at SYS_MSG_ADD_AUDIO_SOURCE_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_SET_AUDIO_SOURCE_CONF:
                {
			Add_AudioSource_Configuration_tmp Set_Audio_Conf;
                        DBG("SYS_MSG_SET_AUDIOSOURCE_CONF\n");
                        if(pMsg->length != sizeof(Set_Audio_Conf))
                            break;
                         ShareMemRead(pMsg->offset,&Set_Audio_Conf, pMsg->length);
                        if(Setaudiosourceconfiguration(&Set_Audio_Conf) != 0){
                                printf("Fail at SYS_MSG_SET_AUDIOSOURCE_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_ADD_AUDIO_ENCODER_CONF:
                {
                       Add_AudioEncoder_Configuration_tmp Add_Audio_Conf;
                        DBG("SYS_MSG_ADD_AUDIO_ENCODER_CONF\n");
                        if(pMsg->length != sizeof(Add_Audio_Conf))
                            break;
                         ShareMemRead(pMsg->offset,&Add_Audio_Conf, pMsg->length);
                        if(addaudioencoderconfiguration(&Add_Audio_Conf) != 0){
                                printf("Fail at SYS_MSG_ADD_AUDIO_ENCODER_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_SET_AUDIO_ENCODER_CONF:
                {
			Add_AudioEncoder_Configuration_tmp Set_Audio_Conf;
                        DBG("SYS_MSG_SET_AUDIOENCODER_CONF\n");
                        if(pMsg->length != sizeof(Set_Audio_Conf))
                            break;
                         ShareMemRead(pMsg->offset,&Set_Audio_Conf, pMsg->length);
                        if(Setaudioencoderconfiguration(&Set_Audio_Conf) != 0){
                                printf("Fail at SYS_MSG_SET_AUDIOENCODER_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_ADD_VIDEOENCODER_CONF:
                {
                       video_encoder_conf video_encoder_conf_t;
                        DBG("SYS_MSG_ADD_VIDEOENCODER_CONF\n");
                        if(pMsg->length != sizeof(video_encoder_conf_t))
                            break;
                         ShareMemRead(pMsg->offset,&video_encoder_conf_t, pMsg->length);
                        if(Addvideoencoderconfiguration(&video_encoder_conf_t) != 0){
                                printf("Fail at SYS_MSG_ADD_VIDEOENCODER_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
                case SYS_MSG_SET_VIDEOENCODER_CONF:
                {
                        video_encoder_conf set_video_encoder_conf_t;
                        DBG("SYS_MSG_SET_VIDEOENCODER_CONF\n");
                        if(pMsg->length != sizeof(set_video_encoder_conf_t))
                            break;
                         ShareMemRead(pMsg->offset,&set_video_encoder_conf_t, pMsg->length);
                        if(Setvideoencoderconfiguration(&set_video_encoder_conf_t) != 0){
                                printf("Fail at SYS_MSG_SET_VIDEOENCODER_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_DEL_VIDEOENCODER_CONF:
                {
                        video_encoder_conf del_video_encoder_conf_t;
                        DBG("SYS_MSG_DEL_VIDEOENCODER_CONF\n");
                        if(pMsg->length != sizeof(del_video_encoder_conf_t))
                            break;
                         ShareMemRead(pMsg->offset,&del_video_encoder_conf_t, pMsg->length);
                        if(Delvideoencoderconfiguration(&del_video_encoder_conf_t) != 0){
                                printf("Fail at SYS_MSG_DEL_VIDEOENCODER_CONF\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
                }
		case SYS_MSG_DEL_PROFILE:
		{
			int index;
			DBG("SYS_MSG_DEL_PROFILE\n");
			if(pMsg->length != sizeof(index))
				break;
			ShareMemRead(pMsg->offset, &index, pMsg->length);
			printf("Index value %d\n",index);
			if(DeleteProfile(index) != 0){
				printf("Fail at SYS_MSG_DEL_PROFILE\n");
				break;
			}
                        pMsg->length = 0;
			ret = 1;
			break;

		}
		case SYS_MSG_DEL_AUDIO_SOURCE_CONF:
		{
			Add_AudioSource_Configuration_tmp audio;
			DBG("SYS_MSG_DEL_AUDIO_SOURCE\n");
			if(pMsg->length != sizeof(audio))
				break;
			ShareMemRead(pMsg->offset, &audio, pMsg->length);
			printf("Index value %d\n",audio.position);
			if(RemoveAudioSource(audio.position) != 0){
				printf("Fail at SYS_MSG_DEL_AUDIO_SOURCE\n");
				break;
			}
                        pMsg->length = 0;
			ret = 1;
			break;

		}
		case SYS_MSG_DEL_AUDIO_ENCODER_CONF:
		{
			Add_AudioEncoder_Configuration_tmp audio;
			DBG("SYS_MSG_DEL_AUDIO_ENCODER\n");
			if(pMsg->length != sizeof(audio))
				break;
			ShareMemRead(pMsg->offset, &audio, pMsg->length);
			printf("Index value %d\n",audio.position);
			if(RemoveAudioEncoder(audio.position) != 0){
				printf("Fail at SYS_MSG_DEL_AUDIO_ENCODER\n");
				break;
			}
                        pMsg->length = 0;
			ret = 1;
			break;

		}
               case SYS_MSG_SET_DIS_MODE:
               {
                         unsigned char mode;
                         DBG("SYS_MSG_SET_DIS_MODE\n");
                            if(pMsg->length != sizeof(mode))
                                break;
                        ShareMemRead(pMsg->offset, &mode, pMsg->length);
                        if(SetDiscovery(mode) != 0){
                                printf("Fail at SYS_MSG_SET_DIS_MODE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;

               }
               case SYS_MSG_ADD_SCOPE:
               {
                        Onvif_Scopes_t onvif_scopes_tmp;
                        DBG("SYS_MSG_ADD_SCOPE\n");
                        if(pMsg->length != sizeof(onvif_scopes_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &onvif_scopes_tmp, pMsg->length);
                        if(AddScope(onvif_scopes_tmp) != 0){
                                printf("Fail at SYS_MSG_ADD_SCOPE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }
               case SYS_MSG_DEL_SCOPE:
               {
                        Onvif_Scopes_t onvif_scopes_tmp;
                        DBG("SYS_MSG_DEL_SCOPE\n");
                        if(pMsg->length != sizeof(onvif_scopes_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &onvif_scopes_tmp, pMsg->length);
                        if(DelScope(onvif_scopes_tmp) != 0){
                                printf("Fail at SYS_MSG_DEL_SCOPE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }

		case SYS_MSG_SET_SCOPE:
               {
                       Onvif_Scopes_t onvif_scopes_tmp;
                       DBG("SYS_MSG_DEL_SCOPE\n");
                       if(pMsg->length != sizeof(onvif_scopes_tmp))
                               break;
                       ShareMemRead(pMsg->offset, &onvif_scopes_tmp, pMsg->length);
                       if(SetScope(onvif_scopes_tmp) != 0){
                               printf("Fail at SYS_MSG_DEL_SCOPE\n");
                               break;
                       }
                       pMsg->length = 0;
                       ret = 1;
                       break;
               }

               case SYS_MSG_SET_ANALYTICS:
               {
                        video_analytics_conf video_analytics_tmp;
                        DBG("SYS_MSG_SET_ANALYTICS\n");
                        if(pMsg->length != sizeof(video_analytics_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &video_analytics_tmp, pMsg->length);
                        if(SetAnalytics(video_analytics_tmp) != 0){
                                printf("Fail at SYS_MSG_SET_ANALYTICS\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }
               case SYS_MSG_SET_OUTPUT:
               {
                        video_output_conf video_output_tmp;
                        DBG("SYS_MSG_SET_OUTPUT\n");
                        if(pMsg->length != sizeof(video_output_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &video_output_tmp, pMsg->length);
                        if(SetVideoOutput(video_output_tmp) != 0){
                                printf("Fail at SYS_MSG_SET_OUTPUT\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }
	       case SYS_MSG_SET_RELAY:
               {
                        relay_conf relay_conf_tmp;
                        DBG("SYS_MSG_SET_RELAY\n");
                        if(pMsg->length != sizeof(relay_conf_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &relay_conf_tmp, pMsg->length);
                        if(SetRelay(relay_conf_tmp) != 0){
                                printf("Fail at SYS_MSG_SET_RELAY\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }
	       case SYS_MSG_SET_RELAY_LOGICALSTATE:
               {
                        relay_conf relay_conf_tmp;
                        DBG("SYS_MSG_SET_RELAY_LOGICALSTATE\n");
                        if(pMsg->length != sizeof(relay_conf_tmp))
                                break;
                        ShareMemRead(pMsg->offset, &relay_conf_tmp, pMsg->length);
                        if(SetRelaylogicalstate(relay_conf_tmp) != 0){
                                printf("Fail at SYS_MSG_SET_RELAYLOGICALSTATE\n");
                                break;
                        }
                        pMsg->length = 0;
                        ret = 1;
                        break;
               }
                /* End-Onvif */

		/* Set DMVA Specific Parameters */
		case SYS_MSG_SET_DMVAENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVAENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVAENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVAALGOFRAMERATE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVAALGOFRAMERATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaExptAlgoFrameRate(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVAALGOFRAMERATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVAALGODETECTFREQ:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVAALGODETECTFREQ\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaExptAlgoDetectFreq(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVAALGODETECTFREQ\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVAEVTRECORDENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVAEVTRECORDENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaExptEvtRecordEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVAEVTRECORDENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVASMETATRACKERENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVASMETATRACKERENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaExptSmetaTrackerEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVASMETATRACKERENABLE\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA TZ Specific Parameters */
		case SYS_MSG_SET_DMVA_TZ_SENSITIVITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_SENSITIVITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZSensitivity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_SENSITIVITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_PERSONMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_PERSONMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZPersonMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_PERSONMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_PERSONMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_PERSONMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZPersonMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_PERSONMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_VEHICLEMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_VEHICLEMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZVehicleMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_VEHICLEMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_VEHICLEMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_VEHICLEMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZVehicleMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_VEHICLEMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_PERSONMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_PERSONMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZPersonMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_PERSONMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_PERSONMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_PERSONMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZPersonMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_PERSONMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_VEHICLEMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_VEHICLEMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZVehicleMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_VEHICLEMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_VEHICLEMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_VEHICLEMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZVehicleMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_VEHICLEMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_DIRECTION:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_DIRECTION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZDirection(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_DIRECTION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_PRESENTADJUST:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_PRESENTADJUST\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZPresentAdjust(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_PRESENTADJUST\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_X16\n");
				break;
			}
			ret = 1;
			break;
		}



		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE1_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE1_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone1_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE1_Y16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_TZ_ZONE2_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_X16\n");
				break;
			}
			ret = 1;
			break;
		}



		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_TZ_ZONE2_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_ZONE2_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZZone2_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_ZONE2_Y16\n");
				break;
			}
			ret = 1;
			break;
		}

        /* DMVA TZ save/load params */
		case SYS_MSG_SET_DMVA_TZ_SAVE:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_TZ_SAVE :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaTZSave(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_SAVE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_TZ_LOAD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_TZ_LOAD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaTZLoad(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_TZ_LOAD\n");
				break;
			}
			ret = 1;
			break;
		}


		/* Set DMVA Main Page specific Parameters */
		case SYS_MSG_SET_DMVA_MAIN_CAMID:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_CAMID\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCamID(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_CAMID\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_GETSCHEDULE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_GETSCHEDULE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaGetSchedule(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_GETSCHEDULE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_FD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_FD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgFD(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_FD\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_CTDIMDSMETATZOC:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_CTDIMDSMETATZOC\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgCTDImdSmetaTzOc(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_CTDIMDSMETATZOC\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_DISPLAYOPTIONS:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_DISPLAYOPTIONS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaDisplayOptions(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_DISPLAYOPTIONS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_RECORDINGVAME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_RECORDINGVAME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventRecordingVAME(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_RECORDINGVAME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListActionPlaySendSearchTrash(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_SELECTEVENT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_SELECTEVENT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListSelectEvent(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_SELECTEVENT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_ARCHIVEFLAG:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_ARCHIVEFLAG\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListArchiveFlag(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_ARCHIVEFLAG\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EVENTTYPE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EVENTTYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListEventType(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EVENTTYPE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_MAIN_EL_START_MONTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_START_MONTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListStartMonth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_START_MONTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EL_START_DAY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_START_DAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListStartDay(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_START_DAY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EL_START_TIME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_START_TIME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListStartTime(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_START_TIME\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EL_END_MONTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_END_MONTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListEndMonth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_END_MONTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EL_END_DAY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_END_DAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListEndDay(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_END_DAY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_MAIN_EL_END_TIME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_MAIN_EL_END_TIME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventListEndTime(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_END_TIME\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaEventListTimeStamp(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaEventListEventDetails(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA Advanced Settings Page specific Parameters */
		case SYS_MSG_SET_DMVA_ADVANCED_OBJECTMETADATA:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OBJECTMETADATA\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaObjectMetaData(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OBJECTMETADATA\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTTYPE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTTYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesFontType(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTTYPE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTSIZE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTSIZE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesFontSize(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTSIZE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesFontColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesBoxColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesCentroidColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesActiveZoneColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesInactiveZoneColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOverlayPropertiesVelocityColor(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILELOAD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILELOAD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEnvironmentProfileLoad(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILELOAD\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTORE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTORE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEnvironmentProfileStore(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTORE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILECLEAR:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILECLEAR\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEnvironmentProfileClear(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILECLEAR\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTATE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTATE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEnvironmentProfileState(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILESTATE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILEDESC:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILEDESC :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaEnvironmentProfileDesc(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ADVANCED_ENVPROFILEDESC\n");
				break;
			}
			ret = 1;
			break;
		}

        /* Set DMVA event delete specific parameters */
		case SYS_MSG_SET_DMVA_EVENT_DELETE_INDEX:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_EVENT_DELETE_INDEX\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventDeleteIndex(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_EVENT_DELETE_INDEX\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_EVENT_DELETE_START_INDEX:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_EVENT_DELETE_START_INDEX\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventDeleteStartIndex(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_EVENT_DELETE_START_INDEX\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_EVENT_DELETE_END_INDEX:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_EVENT_DELETE_END_INDEX\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventDeleteEndIndex(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_EVENT_DELETE_END_INDEX\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_EVENT_DELETE_ALL:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_EVENT_DELETE_ALL\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEventDeleteAll(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_EVENT_DELETE_ALL\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA CTD Page specific Parameters */
		case SYS_MSG_SET_DMVA_CTD_ENV1:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_CTD_ENV1\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgTDEnv1(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_CTD_ENV1\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_CTD_ENV2:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_CTD_ENV2\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgTDEnv2(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_CTD_ENV2\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_CTD_SENSITIVITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_CTD_SENSITIVITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgTDSensitivity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_CTD_SENSITIVITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_CTD_RESETTIME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_CTD_RESETTIME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaCfgTDResetTime(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_CTD_RESETTIME\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA OC Page specific Parameters */
		case SYS_MSG_SET_DMVA_OC_SENSITIVITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_SENSITIVITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCSensitivity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_SENSITIVITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_OBJECTWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_OBJECTWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCObjectWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_OBJECTWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_OBJECTHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_OBJECTHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCObjectHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_OBJECTHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_DIRECTION:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_DIRECTION\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCDirection(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_DIRECTION\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_ENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_LEFTRIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_LEFTRIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCLeftRight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_LEFTRIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_OC_TOPBOTTOM:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_TOPBOTTOM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCTopBottom(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_TOPBOTTOM\n");
				break;
			}
			ret = 1;
			break;
		}

        /* DMVA OC save/load params */
		case SYS_MSG_SET_DMVA_OC_SAVE:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_OC_SAVE :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaOCSave(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_SAVE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_OC_LOAD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_OC_LOAD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaOCLoad(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_OC_LOAD\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA SMETA Page specific Parameters */
		case SYS_MSG_SET_DMVA_SMETA_SENSITIVITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_SENSITIVITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETASensitivity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_SENSITIVITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ENABLESTREAMINGDATA:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ENABLESTREAMINGDATA\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAEnableStreamingData(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ENABLESTREAMINGDATA\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_STREAMBB:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_STREAMBB\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAStreamBB(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_STREAMBB\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_STREAMVELOCITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_STREAMVELOCITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAStreamVelocity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_STREAMVELOCITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_STREAMHISTOGRAM:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_STREAMHISTOGRAM\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAStreamHistogram(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_STREAMHISTOGRAM\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_STREAMMOMENTS:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_STREAMMOMENTS\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAStreamMoments(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_STREAMMOMENTS\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_PRESENTADJUST:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_PRESENTADJUST\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAPresentAdjust(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_PRESENTADJUST\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_PERSONMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_PERSONMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAPersonMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_PERSONMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_PERSONMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_PERSONMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAPersonMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_PERSONMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_VEHICLEMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_VEHICLEMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAVehicleMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_VEHICLEMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_VEHICLEMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_VEHICLEMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAVehicleMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_VEHICLEMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_PERSONMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_PERSONMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAPersonMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_PERSONMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_PERSONMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_PERSONMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAPersonMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_PERSONMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAVehicleMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAVehicleMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_VEHICLEMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_NUMZONES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_NUMZONES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETANumZones(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_NUMZONES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_X16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_SMETA_ZONE1_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_ZONE1_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETAZone1_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_Y16\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SMETA_ZONE1_LABEL:
		{
			if(pMsg->length > MAX_DMVA_LABEL_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_SMETA_ZONE1_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaSMETAZone1_Label(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_ZONE1_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		/* DMVA SMETA save/load params */
		case SYS_MSG_SET_DMVA_SMETA_SAVE:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_SMETA_SAVE :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaSMETASave(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_SAVE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SMETA_LOAD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SMETA_LOAD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSMETALoad(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SMETA_LOAD\n");
				break;
			}
			ret = 1;
			break;
		}

		/* Set DMVA IMD Page specific Parameters */
		case SYS_MSG_SET_DMVA_IMD_SENSITIVITY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_SENSITIVITY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDSensitivity(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_SENSITIVITY\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_PRESENTADJUST:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_PRESENTADJUST\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDPresentAdjust(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_PRESENTADJUST\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_PERSONMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_PERSONMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDPersonMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_PERSONMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_PERSONMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_PERSONMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDPersonMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_PERSONMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_VEHICLEMINWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_VEHICLEMINWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDVehicleMinWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_VEHICLEMINWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_VEHICLEMINHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_VEHICLEMINHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDVehicleMinHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_VEHICLEMINHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_PERSONMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_PERSONMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDPersonMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_PERSONMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_PERSONMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_PERSONMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDPersonMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_PERSONMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_VEHICLEMAXWIDTH:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_VEHICLEMAXWIDTH\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDVehicleMaxWidth(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_VEHICLEMAXWIDTH\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_VEHICLEMAXHEIGHT:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_VEHICLEMAXHEIGHT\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDVehicleMaxHeight(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_VEHICLEMAXHEIGHT\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_NUMZONES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_NUMZONES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDNumZones(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_NUMZONES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONEENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONEENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZoneEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONEENABLE\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4Numsides(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_ZONE1_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_X16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE1_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE1_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone1_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_Y16\n");
				break;
			}
			ret = 1;
			break;
		}




		case SYS_MSG_SET_DMVA_IMD_ZONE2_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_X16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE2_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE2_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone2_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_Y16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE3_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_X16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE3_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE3_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone3_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_Y16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE4_X01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_X16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_X16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_x16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_X16\n");
				break;
			}
			ret = 1;
			break;
		}


		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y01:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y01\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y01(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y01\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y02:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y02\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y02(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y02\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y03:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y03\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y03(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y03\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y04:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y04\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y04(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y04\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y05:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y05\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y05(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y05\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y06:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y06\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y06(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y06\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y07:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y07\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y07(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y07\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y08:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y08\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y08(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y08\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y09:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y09\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y09(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y09\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y10:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y10\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y10(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y10\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y11:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y11\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y11(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y11\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y12:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y12\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y12(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y12\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y13:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y13\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y13(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y13\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y14:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y14\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y14(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y14\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y15:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y15\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y15(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y15\n");
				break;
			}
			ret = 1;
			break;
		}
		case SYS_MSG_SET_DMVA_IMD_ZONE4_Y16:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_ZONE4_Y16\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDZone4_y16(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_Y16\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_ZONE1_LABEL:
		{
			if(pMsg->length > MAX_DMVA_LABEL_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_IMD_ZONE1_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaIMDZone1_Label(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE1_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_ZONE2_LABEL:
		{
			if(pMsg->length > MAX_DMVA_LABEL_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_IMD_ZONE2_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaIMDZone2_Label(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE2_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_ZONE3_LABEL:
		{
			if(pMsg->length > MAX_DMVA_LABEL_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_IMD_ZONE3_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaIMDZone3_Label(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE3_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_ZONE4_LABEL:
		{
			if(pMsg->length > MAX_DMVA_LABEL_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_IMD_ZONE4_LABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaIMDZone4_Label(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_ZONE4_LABEL\n");
				break;
			}
			ret = 1;
			break;
		}

        /* DMVA IMD save/load params */
		case SYS_MSG_SET_DMVA_IMD_SAVE:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_IMD_SAVE :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaIMDSave(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_SAVE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_IMD_LOAD:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_IMD_LOAD\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaIMDLoad(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_IMD_LOAD\n");
				break;
			}
			ret = 1;
			break;
		}

        /* DMVA Scheduler */
		case SYS_MSG_SET_DMVA_SCH_ENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_ENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_ENABLE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_RULETYPE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_RULETYPE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchRuleType(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_RULETYPE\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_LOADSETTING:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_LOADSETTING\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchLoadSetting(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_LOADSETTING\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_RULELABEL:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_SCH_RULELABEL :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaSchRuleLabel(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_RULELABEL\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_STARTDAY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_STARTDAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchStartDay(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_STARTDAY\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_STARTTIME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_STARTTIME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchStartTime(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_STARTTIME\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_ENDDAY:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_ENDDAY\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchEndDay(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_ENDDAY\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_ENDTIME:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_ENDTIME\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchEndTime(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_ENDTIME\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_START:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_SCH_START\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaSchStart(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_START\n");
				break;
			}
			ret = 1;
			break;
		}

		case SYS_MSG_SET_DMVA_SCH_DELRULE:
		{
			if(pMsg->length > MAX_DMVA_STRING_LENGTH){
				printf("SYS_MSG_SET_DMVA_SCH_DELRULE :String length bigger then System Server Buffer\n");
				break;
			}
			ShareMemRead(pMsg->offset, buffer, pMsg->length);
			if(SetDmvaSchDelRule(buffer, pMsg->length) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_SCH_DELRULE\n");
				break;
			}
			ret = 1;
			break;
		}

		/* DMVA Enc Roi Enable */
		case SYS_MSG_SET_DMVA_ENC_ROIENABLE:
		{
			unsigned int value;
			DBG("SYS_MSG_SET_DMVA_ENC_ROIENABLE\n");
			if(pMsg->length != sizeof(value))
				break;
			ShareMemRead(pMsg->offset, &value, pMsg->length);
			if(SetDmvaEncRoiEnable(value) != 0){
				printf("Fail at SYS_MSG_SET_DMVA_ENC_ROIENABLE\n");
				break;
			}
			ret = 1;
			break;
		}


		default:
			printf("Unknown message. Quit\n");
			IsSysQuit = 1;
			pMsg->src = 0;
			break;
	}
	if(ret == 2){
		pMsg->offset = (pMsg->src - MSG_TYPE_MSG1 - 1) * PROC_MEM_SIZE;
		ShareMemWrite(pMsg->offset, pData, pMsg->length);
		ret = 1;
	}
	return ret;
}

static int gSysMsgId;

/**
 * @brief	Set system message id
 * @param	"int qid"	[IN]queue id
 * @return	void
 */
void SetSysMsgId(int qid)
{
	gSysMsgId = qid;
}

/**
 * @brief	Get system message id
 * @param	none
 * @return	gSysMsgId
 */
int GetSysMsgId()
{
	return gSysMsgId;
}

/**
 * @brief	Clean system message queue
 * @param	none
 * @return	void
 */
void CleanSysMsg()
{
	SYS_MSG_BUF msgbuf;
	int i;
	for(i = MSG_TYPE_START; i < MSG_TYPE_END;i++)
		while(msgrcv( GetSysMsgId(), &msgbuf, sizeof(msgbuf) - sizeof(long),
					i, IPC_NOWAIT)>=0);
}

/**
 * @brief	System message thread
 * @param	"int qid" : [IN]QID
 * @param	"int mid" : [IN]MID
 * @return	0
 */
int SysMsgThread(int qid,int mid)
{
	SYS_MSG_BUF msgbuf;
	int msg_size, e_count = 0;
	while(IsSysQuit == 0){
		msg_size = msgrcv( qid, &msgbuf, sizeof(msgbuf) - sizeof(long),
				MSG_TYPE_MSG1, 0);
		if( msg_size < 0 ){
			if(e_count++ >= 3){
				ERR("System server receive msg fail \n");
				perror("ERROR:");
#if 0
				sleep(20);
				Msg_Kill(qid);
				qid = Msg_Init(SYS_MSG_KEY);
#endif
				break;
			}
		}else if(msgbuf.src == MSG_TYPE_MSG1 || msgbuf.src < 0){
			ERR("Got Error message\n");
			break;
		} else {
			e_count = 0;
			if(ProcSysMsg(&msgbuf) == 1)
				msgbuf.shmid = mid;
			else
				msgbuf.length = -1;
			if(msgbuf.src != 0){
				msgbuf.Des = msgbuf.src;
				msgbuf.src = MSG_TYPE_MSG1;
				msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
			}
		}
	}
	IsSysQuit = 1;
	return 0;
}

/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */
int main(int argc,char *argv[]) {
	int qid, mid,bThreadStatus = 0;
	struct shmid_ds buf;
	pthread_t tFileMngThread,tAlarmThread;

	/* mutex for fork */
	//gFORKSem = MakeSem();
	//if(gFORKSem == NULL){
	//    ERR("fork sem create fail\n");
	//    return -1;
	//}

	/* System message queue initial */
	qid = Msg_Init(SYS_MSG_KEY);
	SetSysMsgId(qid);
	if(pthread_create(&tFileMngThread, NULL, FileMngThread, NULL)){
		IsSysQuit = 1;
		ERR("FileMngThread create fail\n");
	}
	printf("FileMngThread created\n");
	bThreadStatus |= FILE_THR_CREATED;
	/* Create communication between file manager and system server */
	if(InitFileMsgDrv(FILE_MSG_KEY, FILE_SYS_MSG) != 0){
		if(bThreadStatus & FILE_THR_CREATED){
			SendFileQuitCmd();
			pthread_join (tFileMngThread, NULL);
		}
		ERR("File message driver init fail\n");
		return -1;
	}
	if(pthread_create(&tAlarmThread, NULL, AlramThread, NULL)){
		IsSysQuit = 1;
		ERR("AlramThread create fail\n");
	}
	printf("AlramThread created\n");
	bThreadStatus |= ALARM_THR_CREATED;
	/* Share memory initial (for system server) */
	if((mid = ShareMemInit(SYS_MSG_KEY)) < 0){
		IsSysQuit = 1;
		ERR("Share memory init fail\n");
	}
	printf("Share memory init success\n");
	while(IsSysQuit == 0){
		if(SystemInit() != 0){
			IsSysQuit = 1;
			break;
		}
		printf("SystemInit success\n");
		if(ScheduleMngInit(GetSysInfo()))
		{
			IsSysQuit = 1;
			break;
			ERR("Schedule manager init fail\n");
		}
		printf("Schedule manager init success\n");
		if(DmvaScheduleMngInit(GetSysInfo()))
		{
			IsSysQuit = 1;
			break;
			ERR("Dmva Schedule manager init fail\n");
		}
		printf("Dmva Schedule manager init success\n");
		/* Process all messages which was sent to system server */
		SysMsgThread(qid,mid);
	}
	if(bThreadStatus & ALARM_THR_CREATED){
		SendAlarmQuitCmd();
		pthread_join (tAlarmThread, NULL);
	}
	if(bThreadStatus & FILE_THR_CREATED){
		SendFileQuitCmd();
		pthread_join (tFileMngThread, NULL);
	}
	SystemCleanup();
	if(mid >= 0)
		shmctl(mid, IPC_RMID, &buf);
	Msg_Kill(qid);
	//DestroySem(gFORKSem);
	return 0;
}


