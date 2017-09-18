#include <string.h>
#include <stdlib.h>
#include <asm/types.h>
#include <time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>
#include "boa.h"
#include "appro_api.h"
#include "net_config.h"
#include <sysctrl.h>
#include "file_list.h"
#include "para_list.h"
#include <sys_env_type.h>
#include <file_msg_drv.h>
#include <system_default.h>
#include "psia.h"
#include <sys/reboot.h>
static char *nicname = "eth0";

unsigned int event_location = 0;

unsigned int dmva_event_location = 0;

static char gf_Restarting = 0;
static char gf_CmdBlocking = 0;
//static char codecNumberValue=0;
//static char codecMenuValue=0;
static char alarm_status = 0;
unsigned char lower_ascii[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

extern int TranslateWebPara(AUTHORITY authority, char *target, char *para, char *subpara);

void strtolower(unsigned char *str)
{
	while (*str) {
		*str = lower_ascii[*str];
		str++;
	}
}

/*
typedef union __NET_IPV4 {
	__u32	int32;
	__u8	str[4];
} NET_IPV4;*/


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
static int netsplit( char *pAddress, void *ip )
{
	unsigned int ret;
	NET_IPV4 *ipaddr = (NET_IPV4 *)ip;

	if ((ret = atoi(pAddress + 9)) > 255)
		return FALSE;
	ipaddr->str[3] = ret;

	*( pAddress + 9 ) = '\x0';
	if ((ret = atoi(pAddress + 6)) > 255)
		return FALSE;
	ipaddr->str[2] = ret;

	*( pAddress + 6 ) = '\x0';
	if ((ret = atoi(pAddress + 3)) > 255)
		return FALSE;
	ipaddr->str[1] = ret;

	*( pAddress + 3 ) = '\x0';
	if ((ret = atoi(pAddress + 0)) > 255)
		return FALSE;
	ipaddr->str[0] = ret;

	return TRUE;
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
int ipv4_str_to_num(char *data, struct in_addr *ipaddr)
{
	if ( strchr(data, '.') == NULL )
		return netsplit(data, ipaddr);
	return inet_aton(data, ipaddr);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
char *req_bufptr(request * req)
{
	return (req->buffer + req->buffer_end);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_dhcpclient(request *req, COMMAND_ARGUMENT *argm)
{
	int value = 0;

	do {
		if (ControlSystemData(SFIELD_GET_DHCPC_ENABLE, (void *)&value, sizeof(value)) < 0)
			break;;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, value);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dhcpclient(request *req, COMMAND_ARGUMENT *argm)
{
	int sys_dhcp;
	int value = atoi(argm->value);

	do {
		if ( (value != TRUE) && (value != FALSE) )
			break;
		if(ControlSystemData(SFIELD_GET_DHCP_CONFIG, (void *)&sys_dhcp, sizeof(sys_dhcp)) < 0)
			break;
		if(sys_dhcp == 0)
			break;
		if (ControlSystemData(SFIELD_GET_DHCPC_ENABLE, (void *)&sys_dhcp, sizeof(sys_dhcp)) < 0)
			break;

		if (sys_dhcp != value) {
			if (value == TRUE)
				net_enable_dhcpcd();
			else
				net_disable_dhcpcd();

			ControlSystemData(SFIELD_SET_DHCPC_ENABLE, (void *)&value, sizeof(value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_netip(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ip;

	do {
//		if (ControlSystemData(SFIELD_GET_IP, (void *)&ip.s_addr, sizeof(ip.s_addr)) < 0)
		if ( (ip.s_addr = net_get_ifaddr(nicname)) == -1)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_netip(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ipaddr, sys_ip;

	do {
		if (ipv4_str_to_num(argm->value, &ipaddr) == 0)
			break;

//		if (ControlSystemData(SFIELD_GET_IP, (void *)&sys_ip.s_addr, sizeof(sys_ip.s_addr)) < 0)
		if ( (sys_ip.s_addr = net_get_ifaddr(nicname)) == -1)
			break;

		if (sys_ip.s_addr != ipaddr.s_addr) {
			if (net_set_ifaddr(nicname, ipaddr.s_addr) < 0)
				break;

			ControlSystemData(SFIELD_SET_IP, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));
//			argm->flags |= HTTP_OPTION_CHANGED;

		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_netmask(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ip;

	do {
//		if (ControlSystemData(SFIELD_GET_NETMASK, (void *)&ip.s_addr, sizeof(ip.s_addr)) < 0)
		if ( (ip.s_addr = net_get_netmask(nicname)) == -1)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_netmask(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ipaddr, sys_ip;

	do {
		if (ipv4_str_to_num(argm->value, &ipaddr) == 0)
			break;

//		if (ControlSystemData(SFIELD_GET_NETMASK, (void *)&sys_ip.s_addr, sizeof(sys_ip.s_addr)) < 0)
		if ( (sys_ip.s_addr = net_get_netmask(nicname)) == -1)
			break;

		if (sys_ip.s_addr != ipaddr.s_addr) {
			if (net_set_netmask(nicname, ipaddr.s_addr) < 0)
				break;

			ControlSystemData(SFIELD_SET_NETMASK, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));
//			argm->flags |= HTTP_OPTION_CHANGED;

		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_dnsip(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ip;

/*	do {
		if (ControlSystemData(SFIELD_GET_DNS, (void *)&ip.s_addr, sizeof(ip.s_addr)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
	*/
	ip.s_addr = net_get_dns();
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
}

void set_dnsip(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ipaddr, sys_ip;

	do {
		if (ipv4_str_to_num(argm->value, &ipaddr) == 0)
			break;

//		if (ControlSystemData(SFIELD_GET_DNS, (void *)&sys_ip.s_addr, sizeof(sys_ip.s_addr)) < 0)
//			break;
		sys_ip.s_addr = net_get_dns();

		if (sys_ip.s_addr != ipaddr.s_addr) {
			if (net_set_dns(inet_ntoa(ipaddr)) < 0)
				break;

			ControlSystemData(SFIELD_SET_DNS, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));
//			argm->flags |= HTTP_OPTION_CHANGED;

		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_gateway(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ip;

/*	do {
		if (ControlSystemData(SFIELD_GET_GATEWAY, (void *)&ip.s_addr, sizeof(ip.s_addr)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
	*/
	ip.s_addr = net_get_gateway();
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, inet_ntoa(ip));
}

void set_gateway(request *req, COMMAND_ARGUMENT *argm)
{
	struct in_addr ipaddr, sys_ip;

	do {
		if (ipv4_str_to_num(argm->value, &ipaddr) == 0)
			break;

//		if (ControlSystemData(SFIELD_GET_GATEWAY, (void *)&sys_ip.s_addr, sizeof(sys_ip.s_addr)) < 0)
//			break;
		sys_ip.s_addr = net_get_gateway();

		if (sys_ip.s_addr != ipaddr.s_addr) {
			if (net_set_gateway(ipaddr.s_addr) < 0)
				break;

			ControlSystemData(SFIELD_SET_GATEWAY, (void *)&ipaddr.s_addr, sizeof(ipaddr.s_addr));
//			argm->flags |= HTTP_OPTION_CHANGED;

		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_sntpip(request *req, COMMAND_ARGUMENT *argm)
{
	char value[256];
	int len;

	do {
		if ((len = ControlSystemData(SFIELD_GET_SNTP_FQDN, (void *)value, sizeof(value))) < 0)
			break;
		value[len] = '\0';
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, value);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_sntpfqdn(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SNTP_FQDN, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SNTP_SERVER, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

void paratest(request *req, COMMAND_ARGUMENT *argm)
{
	int ret;
	char buf[720];
	char *spridx = NULL;
	do {
		if ((spridx = index(argm->value, '.')) == NULL) {
			ret = TranslateWebPara(req->authority, buf, argm->value, "");
		} else {
			*spridx++ = '\0';
			ret = TranslateWebPara(req->authority, buf, argm->value, spridx);
			*--spridx = '.';
		}
		if (ret < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->value, buf);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->value);
}
/*-----------------------------------------------------------------*/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_ftppassword(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[33];

	do {
		if (ControlSystemData(SFIELD_GET_FTP_PASSWORD, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_FTP_PASSWORD, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_ftpuser(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[33];

	do {
		if (ControlSystemData(SFIELD_GET_FTP_USERNAME, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_FTP_USERNAME, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_ftppath(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_FTP_FOLDNAME, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_FTP_FOLDNAME, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtppwd(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_PASSWORD, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SMTP_PASSWORD, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpuser(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_USERNAME, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SMTP_USERNAME, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpto(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_RECEIVER_EMAIL_ADDRESS, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SMTP_RECEIVER_EMAIL_ADDRESS, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpsender(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_SENDER_EMAIL_ADDRESS, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SMTP_SENDER_EMAIL_ADDRESS, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpfqdn(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_SERVER_IP, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_SMTP_SERVER_IP, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpport(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned short sys_port, port = atoi(argm->value);

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_PORT, (void *)&sys_port, sizeof(sys_port)) < 0)
			break;

		if (sys_port != port) {
			ControlSystemData(SFIELD_SET_SMTP_PORT, (void *)&port, sizeof(port));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_ftpfqdn(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_value[256];

	do {
		if (ControlSystemData(SFIELD_GET_FTP_SERVER_IP, (void *)sys_value, sizeof(sys_value)) < 0)
			break;

		if (strcmp(sys_value, argm->value)) {
			ControlSystemData(SFIELD_SET_FTP_SERVER_IP, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_smtpauth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char sys_value, value = atoi(argm->value);

	do {
		if (ControlSystemData(SFIELD_GET_SMTP_AUTHENTICATION, (void *)&sys_value, sizeof(sys_value)) < 0)
			break;

		if (sys_value != value) {
			ControlSystemData(SFIELD_SET_SMTP_AUTHENTICATION, (void *)&value, sizeof(value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_ftpport(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned short sys_port, port = atoi(argm->value);

	do {
		if (ControlSystemData(SFIELD_GET_FTP_PORT, (void *)&sys_port, sizeof(sys_port)) < 0)
			break;

		if (sys_port != port) {
			ControlSystemData(SFIELD_SET_FTP_PORT, (void *)&port, sizeof(port));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void add_user(request *req, COMMAND_ARGUMENT *argm)
{
	add_user_t acount;
	char *uname, *upass, *uauth;

	do {
	    uname = argm->value;
	    upass = index(uname, ':');
	    *upass++ = '\0';
	    uauth = index(upass, ':');
	    *uauth++ = '\0';

	    if((strlen(uname) > USER_LEN) || (strlen(uname) < MIN_USER_LEN))
	        break;

	    if((strlen(upass) > PASSWORD_LEN) || (strlen(upass) < MIN_PASSWORD_LEN))
	    	break;

	    strcpy(acount.user_id, uname);
	    strcpy(acount.password, upass);
		//fprintf(stderr, "[BOA] ID:%s PASWD:%s\n", acount.user_id, acount.password);
		//fprintf(stderr, "[BOA] AUTH:%s\n", uauth);
	    acount.authority = atoi(uauth);
	    *--upass = ':';
	    *--uauth = ':';

		if (ControlSystemData(SFIELD_ADD_USER, (void *)&acount, sizeof(acount)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void del_user(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		if (req->authority > AUTHORITY_ADMIN) {
			dbg("Not enought permission!\n");
			break;
		}
		if (strcasecmp(req->user_id, argm->value) == 0) {
			dbg("Can not delete the user : %s\n", argm->value);
			break;
		}

		if (ControlSystemData(SFIELD_DEL_USER, (void *)argm->value, strlen(argm->value)+1) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void sd_format(request *req, COMMAND_ARGUMENT *argm)
{
	int value = atoi(argm->value);
	do {
		if (ControlSystemData(SFIELD_SD_FORMAT, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void set_language(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
	return;
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void get_alarmstatus(request *req, COMMAND_ARGUMENT *argm)
{
    char cmd[80];
	do{
		SysInfo* pSysInfo = GetSysInfo();
		if(pSysInfo == NULL)
			break;

		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%2x\n", argm->name, pSysInfo->lan_config.alarmstatus);

        if(pSysInfo->lan_config.alarmstatus&0xf) /* first 4 bits indicate alarm status */
        {
			alarm_status = 1;
        }
        else{
            if(alarm_status == 1)
            {
                sprintf(cmd, "killall -9 aplay");
                system(cmd);
            }
            alarm_status = 0;
        }
		return;
	} while(0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void sd_unmount(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);
	do {
		if (ControlSystemData(SFIELD_SD_UNMOUNT, (void *)&value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 * 090727 for old-password check                                           *
 ***************************************************************************/
void checkpassword(request *req, COMMAND_ARGUMENT *argm)
{
	int user_no;
	char *user_name, *user_passwd;

	do {
		if ((user_name = index(argm->value, ':')) == NULL)
			break;
		*user_name++ = '\0';
		user_no = atoi(argm->value);
		//*--user_name = '.';
		if ((user_passwd = index(user_name, ':')) == NULL)
			break;
		*user_passwd++ = '\0';
		SysInfo* pSysInfo = GetSysInfo();
		if(pSysInfo == NULL) break; //pSysInfo->acounts[user_no].password
		if (strcmp(user_name, pSysInfo->acounts[user_no].user) != 0) break;
		if (strcmp(user_passwd, pSysInfo->acounts[user_no].password) != 0) break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************
 *                                                                         *
 ***************************************************************************/

void get_motionxblock(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, 4);
	return;
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_ALARMDURATION, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void get_motionyblock(request *req, COMMAND_ARGUMENT *argm)
{
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, 3);
	return;
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_ALARMDURATION, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** LIVE VIDEO PAGE ***********************************/
/***************************************************************************/
void set_democfg(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DEMOCFG, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

 void set_clicksnapfilename(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		if(ControlSystemData(SFIELD_SET_CLICK_SNAP_FILENAME, (void *)argm->value, strlen(argm->value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

 void set_clicksnapstorage(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_CLICK_SNAP_STORAGE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** VIDEO & IMAGE PAGE ***********************************/
/***************************************************************************/
void set_cameratitle(request *req, COMMAND_ARGUMENT *argm)
{
	char sys_camtitle[256] = {0};

	do {
		if (ControlSystemData(SFIELD_GET_TITLE, (void *)sys_camtitle, sizeof(sys_camtitle)) < 0)
			break;

		if (strcmp(sys_camtitle, argm->value)) {
			ControlSystemData(SFIELD_SET_TITLE, (void *)argm->value, strlen(argm->value));
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_videomode(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VIDEO_MODE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_videocodeccombo(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VIDEOCODECCOMBO, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_videocodecres(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VIDEOCODECRES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_mpeg41bitrate(request *req, COMMAND_ARGUMENT *argm)
{
   int value = atoi(argm->value);
   value *= 1000;

	do {
		if (ControlSystemData(SFIELD_SET_MPEG41_BITRATE, (void *)&value, sizeof(value)) < 0) {
			dbg("######ControlSystemData Error!!!!\n");
			break;
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_mpeg42bitrate(request *req, COMMAND_ARGUMENT *argm)
{
   int value = atoi(argm->value);
   value *= 1000;

	do {
		if (ControlSystemData(SFIELD_SET_MPEG42_BITRATE, (void *)&value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_jpgquality(request *req, COMMAND_ARGUMENT *argm) /*here*/
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_JQUALITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_framerate1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FRAMERATE1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_framerate2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FRAMERATE2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_framerate3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FRAMERATE3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_ratecontrol1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		int ret = ControlSystemData(SFIELD_SET_RATE_CONTROL1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		dbg("\n++++ret = %d++++\n\n", ret);
		if (ret == 0xFF && gf_Restarting != 1) {
			gf_Restarting = 1;
		}
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_ratecontrol2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		int ret = ControlSystemData(SFIELD_SET_RATE_CONTROL2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		dbg("\n++++ret = %d++++\n\n", ret);
		if (ret == 0xFF && gf_Restarting != 1) {
			gf_Restarting = 1;
		}
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_datestampenable1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_DSTAMPENABLE1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_datestampenable2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_DSTAMPENABLE2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_datestampenable3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_DSTAMPENABLE3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_timestampenable1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TSTAMPENABLE1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_timestampenable2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TSTAMPENABLE2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_timestampenable3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TSTAMPENABLE3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoenable1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_LOGOENABLE1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoenable2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_LOGOENABLE2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoenable3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_LOGOENABLE3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoposition1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_LOGO_POSITION1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoposition2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_LOGO_POSITION2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_logoposition3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_LOGO_POSITION3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_textenable1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TEXTENABLE1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_textenable2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TEXTENABLE2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_textenable3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TEXTENABLE3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_textposition1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_TEXT_POSITION1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_textposition2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_TEXT_POSITION2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_textposition3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_TEXT_POSITION3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_overlaytext1(request *req, COMMAND_ARGUMENT *argm)
{
	do {

		ControlSystemData(SFIELD_SET_OVERLAY_TEXT1, (void *)argm->value, strlen(argm->value));

		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_overlaytext2(request *req, COMMAND_ARGUMENT *argm)
{
	do {

		ControlSystemData(SFIELD_SET_OVERLAY_TEXT2, (void *)argm->value, strlen(argm->value));

		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_overlaytext3(request *req, COMMAND_ARGUMENT *argm)
{
	do {

		ControlSystemData(SFIELD_SET_OVERLAY_TEXT3, (void *)argm->value, strlen(argm->value));

		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_detailinfo1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_DETAIL_INFO1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_detailinfo2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_DETAIL_INFO2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_detailinfo3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_DETAIL_INFO3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_encryption(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ENCRYPTION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_localdisplay(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_LOCAL_DISPLAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_mirctrl(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MIRROR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** VIDEO ADVANCED PAGE *************************************/
/***************************************************************************/
  void set_ipratio1(request *req, COMMAND_ARGUMENT *argm)
{
	__u32 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_IPRATIO1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_ipratio2(request *req, COMMAND_ARGUMENT *argm)
{
	__u32 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_IPRATIO2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_ipratio3(request *req, COMMAND_ARGUMENT *argm)
{
	__u32 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_IPRATIO3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_forceIframe1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FORCE_IFRAME1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_forceIframe2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FORCE_IFRAME2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_forceIframe3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FORCE_IFRAME3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_qpinit1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPINIT1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_qpinit2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPINIT2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_qpinit3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPINIT3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_qpmin1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMIN1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_qpmin2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMIN2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_qpmin3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMIN3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_qpmax1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMAX1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_qpmax2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMAX2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_qpmax3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_QPMAX3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_meconfig1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MECONFIG1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_meconfig2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MECONFIG2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_meconfig3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MECONFIG3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_packetsize1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_PACKETSIZE1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_packetsize2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_PACKETSIZE2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_packetsize3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_PACKETSIZE3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_regionofinterestenable1(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ROI_ENABLE1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_regionofinterestenable2(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ROI_ENABLE2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_regionofinterestenable3(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ROI_ENABLE3, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1x1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1X1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1y1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1Y1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1w1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1W1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1h1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1H1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1x2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1X2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1y2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1Y2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1w2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1W2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1h2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1H2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_str1x3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1X3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1y3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1Y3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1w3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1W3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str1h3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR1H3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2x1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2X1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2y1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2Y1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2w1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2W1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2h1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2H1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2x2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2X2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2y2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2Y2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2w2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2W2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2h2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2H2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_str2x3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2X3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2y3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2Y3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2w3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2W3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str2h3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR2H3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3x1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3X1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3y1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3Y1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3w1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3W1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3h1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3H1, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3x2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3X2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3y2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3Y2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3w2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3W2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3h2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3H2, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
  void set_str3x3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3X3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3y3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3Y3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3w3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3W3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_str3h3(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_STR3H3, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** VIDEO ANALYTICS PAGE ***********************************/
/***************************************************************************/

void set_motioncenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_MOTIONCENABLE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_motioncvalue(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_MOTIONCVALUE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_motionlevel(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		if(ControlSystemData(SFIELD_SET_MOTIONLEVEL, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_motionblock(request *req, COMMAND_ARGUMENT *argm)
{
	extern int para_motion_blk(char *data, char *arg);
	static char tempBuff[MOTION_BLK_LEN];

	if(strlen(argm->value) == 3)
	{
		if(ControlSystemData(SFIELD_SET_MOTIONBLOCK, argm->value, (strlen(argm->value)+1)) < 0)
		{
			req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
			return;
		}

		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, argm->value);
			return;
	}
	else{
		para_motion_blk(tempBuff, NULL);
		dbg(OPTION_OK "%s=%s\n",argm->name, tempBuff);
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%s\n", argm->name, tempBuff);
	}
}

void set_fdetect(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FDETECT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_fdx(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FDX, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_fdy(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FDY, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

 void set_fdw(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FDW, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_fdh(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FDH, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_fdconflevel(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FDCONF_LEVEL, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_fddirection(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FD_DIRECTION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_frecognition(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FRECOGNITION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_frconflevel(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_FRCONF_LEVEL, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_frdatabase(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FR_DATABASE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
 void set_privacymask(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_PRIVACY_MASK, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_maskoptions(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MASK_OPTIONS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** CAMERA PAGE ***********************************/
/***************************************************************************/

void set_brightness(request *req, COMMAND_ARGUMENT *argm)
{
	if (*argm->value == '\0') {
		SysInfo* pSysInfo = GetSysInfo();
		if(pSysInfo == NULL) {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		} else {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, pSysInfo->lan_config.nBrightness);
		}
		return;
	} else {
		__u8 value = atoi(argm->value);
		ControlSystemData(SFIELD_SET_BRIGHTNESS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	}
}

void set_contrast(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_CONTRAST, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_saturation(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_SATURATION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_sharpness(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_SHARPNESS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_blc(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_BLC, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_backlight(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_BACKLIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_dynrange(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DYNRANGE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dynrangestrength(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DYNRANGESTRENGTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_imagesource(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_IMAGESOURCE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_white_balance(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_WHITE_BALANCE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_day_night(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		if ( value > 1 )
			break;

		ControlSystemData(SFIELD_SET_DAY_NIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_histogram(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_HISTOGRAM, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_binning(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		int ret = ControlSystemData(SFIELD_SET_BINNING, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		dbg("\n++++ret = %d++++\n\n", ret);
		if (ret == 0xFF && gf_Restarting != 1) {
			gf_Restarting = 1;
		}
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_img2a(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		int ret = ControlSystemData(SFIELD_SET_IMAGE2A, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		dbg("\n++++ret = %d++++\n\n", ret);
		if (ret == 0xFF && gf_Restarting != 1) {
			gf_Restarting = 1;
		}
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_img2atype(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_IMAGE2ATYPE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_expPriority(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_EXPPRIORITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_vidstb(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VSTAB, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_ldc(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_LDC, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_vnf(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VNF, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_vnfmode(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VNFMODE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_vnfstrength(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_VNFSTRENGTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_framectrl(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_FRAMECTRL, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** AUDIO PAGE ***********************************/
/***************************************************************************/
void set_audioenable(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIOENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void get_audioenable(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		SysInfo* pSysInfo = GetSysInfo();
		if(pSysInfo == NULL)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, pSysInfo->audio_config.audioenable);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

int CheckAudioStatus(void)
{
	SysInfo* pSysInfo = GetSysInfo();
	return pSysInfo->audio_config.audioenable;
}

 void set_audiomode(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIOMODE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_audioinvolume(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AUDIOINVOLUME, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_encoding(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIO_ENCODE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_samplerate(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIO_SAMPLERATE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_audiobitrate(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIO_BITRATE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_alarmlevel(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_AUDIO_ALARMLEVEL, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_audiooutvolume(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AUDIOOUTVOLUME, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_audioreceiverenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AUDIORECEIVERENABLE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_autoirisvol(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AUTOIRIS_VOL, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_audioserverip(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_AUDIOSERVERIP, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** DATE TIME PAGE ***********************************/
/***************************************************************************/

void get_date(request *req, COMMAND_ARGUMENT *argm)
{
	static char wday_name[7][3] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	time_t tnow;
	struct tm *tmnow;
	int string_len;

	time(&tnow);
	tmnow = localtime(&tnow);

	string_len = sprintf(req_bufptr(req), OPTION_OK "%s=%04d-%02d-%02d %.3s\n", argm->name,
			tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday, wday_name[tmnow->tm_wday]);
	req->buffer_end += string_len;
}

void get_time(request *req, COMMAND_ARGUMENT *argm)
{
	time_t tnow;
	struct tm *tmnow;
	int string_len;

	time(&tnow);
	tmnow = localtime(&tnow);

	string_len = sprintf(req_bufptr(req), OPTION_OK "%s=%02d:%02d:%02d\n", argm->name,
			 		tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);
	req->buffer_end += string_len;
}

int sys_set_date(int year, int month, int day)
{
	struct tm *tm;
	time_t now;
	unsigned char v2;

	now = time(NULL);
	tm = localtime(&now);

	year = (year>1900) ? year-1900 : 0;
	tm->tm_year = year;
	month = (month>0) ? month-1 : 0;
	tm->tm_mon = month;
	tm->tm_mday = day;

	if ((now = mktime(tm)) < 0)
		return -1;
	ControlSystemData(SFIELD_SCHEDULE_STOP, (void *)&v2, sizeof(v2));
	stime(&now);
	system("hwclock -uw");
	ControlSystemData(SFIELD_SCHEDULE_RESUME, (void *)&v2, sizeof(v2));
	return 0;
}

int sys_set_time(int hour, int min, int sec)
{
	struct tm *tm;
	time_t now;
	unsigned char v2;

	now = time(NULL);
	tm = localtime(&now);

	tm->tm_hour = hour;
	tm->tm_min = min;
	tm->tm_sec = sec + 3;

	if ((now = mktime(tm)) < 0)
		return -1;
	ControlSystemData(SFIELD_SCHEDULE_STOP, (void *)&v2, sizeof(v2));
	stime(&now);
	system("hwclock -uw");
	ControlSystemData(SFIELD_SCHEDULE_RESUME, (void *)&v2, sizeof(v2));
	return 0;
}

void set_system_date(request *req, COMMAND_ARGUMENT *argm)
{
	int year, month, day;

	do {
		if (sscanf(argm->value, "%d/%d/%d", &year, &month, &day) != 3)
			break;

		if (sys_set_date(year, month, day) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_system_time(request *req, COMMAND_ARGUMENT *argm)
{
	int hour, min, sec;

	do {
		if (sscanf(argm->value, "%d:%d:%d", &hour, &min, &sec) != 3)
			break;

		if (sys_set_time(hour, min, sec) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

#define NTPCLIENT_EXE	"/opt/ipnc/ntpclient"
#define NTP_TIMEOUT_CNT	(10)
static int ntp_check = 0;
void *ntpThrFxn(void *arg)
{
	char buf[256];
	char *server_name = arg;

	sprintf(buf, NTPCLIENT_EXE " -s -t -i %d -h %s\n", NTP_TIMEOUT_CNT, server_name);
	dbg("run_ntpclient: %s\n", buf);
	system(buf);
	sleep(1);
	system("hwclock -uw");
	ntp_check = 1;

	return (void *) 0;
}

int run_ntpclient(char *server_name)
{
	char buf[256];
	pthread_t ntpThread;
	ntp_check = 0;
	int timout_cnt = NTP_TIMEOUT_CNT;
	int ret = 0;
	unsigned char v2;

	if (pthread_create(&ntpThread, NULL, ntpThrFxn, server_name))
	{
       		return -1;
	}

	while(timout_cnt--)
	{
		if( ntp_check == 0 )
		{
			sleep(1);
		}
		else
		{
			break;
		}
	}

	if( ntp_check == 0 )
	{
		sprintf(buf, "killall -9 ntpclient");
		system(buf);
		ret = -7;
		fprintf(stderr, "UNABLE TO SYNC TO SNTP SERVER in %d seconds\n", NTP_TIMEOUT_CNT);
	}

	pthread_join(ntpThread, NULL);
	ControlSystemData(SFIELD_SCHEDULE_RESUME, (void *)&v2, sizeof(v2));
	ntp_check = 0;

	return ret;
}

void set_time_frequency(request *req, COMMAND_ARGUMENT *argm)
{
	char value[256];
	int ret;
	unsigned char v2;

	do {
		if (ControlSystemData(SFIELD_GET_SNTP_FQDN, (void *)value, sizeof(value)) < 0)
			break;
		ControlSystemData(SFIELD_SCHEDULE_STOP, (void *)&v2, sizeof(v2));
		ret = run_ntpclient(value);
		if (ret < 0)
		{
			if (ret == -7)
			{
				req->buffer_end += sprintf(req_bufptr(req), "TIMEOUT %s\n", argm->name);
				return;
			}
			break;
		}
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_timezone(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		if (value > 24)
			break;
		ControlSystemData(SFIELD_SET_TIMEZONE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_daylight(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DAYLIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dateformat(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_DATEFORMAT, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_tstampformat(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TSTAMPFORMAT, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dateposition(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_DATEPPOS, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_timeposition(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_TIMEPOS, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** NETWORK & PORT PAGE ***********************************/
/***************************************************************************/
void get_httpport(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned short value;

	do {
		if (ControlSystemData(SFIELD_GET_HTTPPORT, (void *)&value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s=%d\n", argm->name, value);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_httpport(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned short sys_value;
	unsigned short value = atoi(argm->value);
	int ret;

	do {
		if ( value < MIN_HTTP_PORT )
			break;

		if (ControlSystemData(SFIELD_GET_HTTPPORT, (void *)&sys_value, sizeof(sys_value)) < 0)
			break;

		if (sys_value != value) {
			ret = ControlSystemData(SFIELD_SET_HTTPPORT, (void *)&value, sizeof(value));
			if (ret == -1)
				req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s:Port not acceptable\n", argm->name);
			else
				req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s:Port acceptable\n", argm->name);
			return;
		}
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
 void set_httpsport(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_HTTPSPORT, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_giointype(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_GIOINTYPE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_gioouttype(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_GIOOUTTYPE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

 void set_rs485(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_RS485, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}
void set_multicast(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_MULTICAST, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************/
/***************** ALARM PAGE ***********************************/
/***************************************************************************/
 void set_alarmenable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_ALARM_ENABLE, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_alarmduration(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_ALARMDURATION, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_aviduration(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);
    SysInfo* pSysInfo = GetSysInfo();
	if (pSysInfo->sdcard_config.sdinsert == 3) // inserted and mounted
		return;
	do {

		ControlSystemData(SFIELD_SET_AVIDURATION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_aviformat(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_AVIFORMAT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_ftpfileformat(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_FTPFILEFORMAT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_sdfileformat(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_SDFILEFORMAT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_attfileformat(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_ATTFILEFORMAT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_asmtpattach(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_SMTP_ATTACHMENTS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_rftpenable(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_FTP_RFTPENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_sdrenable(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_SD_SDRENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_schedule(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		if(ControlSystemData(SFIELD_SET_SCHEDULE, argm->value, strlen(argm->value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void del_schedule(request *req, COMMAND_ARGUMENT *argm)
{
	int bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_DEL_SCHEDULE, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_motionenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
        if(ControlSystemData(SFIELD_SET_MOTIONENABLE, &value, strlen(argm->value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_lostalarm(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_LOSTALARM, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_audioalarm(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AUDIOALARM, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_extalarm(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_EXT_ALARM, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_gioinenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_GIOINENABLE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_giooutenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_GIOOUTENABLE, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_sdaenable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_SDAENABLE, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_aftpenable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_AFTPENABLE, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_asmtpenable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned char bEnable = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_ASMTPENABLE, &bEnable, sizeof(bEnable)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_alarmlocalstorage(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_ALARMLOCAL, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_recordlocalstorage(request *req, COMMAND_ARGUMENT *argm)
{
    __u8 value = atoi(argm->value);

	do {

		ControlSystemData(SFIELD_SET_RECORDLOCAL, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
    return;
	} while (0);

  req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


 void set_alarmaudioplay(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ALARM_AUDIOPLAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_alarmaudiofile(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_ALARM_AUDIOFILE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_schedulerepeatenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_SCHEDULE_REPEAT_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_schedulenumweeks(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	do {
		if(ControlSystemData(SFIELD_SET_SCHEDULE_NUMWEEKS, &value, sizeof(value)) < 0)
			break;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_scheduleinfiniteenable(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_SCHEDULE_INFINITE_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
/***************************************************************************/
/***************** MAINTAINENCE PAGE ***********************************/
/***************************************************************************/
void restart_ipcam(request *req, COMMAND_ARGUMENT *argm)
{
#if 0
	char cmd[256];

	int dev_fp;
	void *pMem_map;
	unsigned long phyAddr= 0x01C41000;
	unsigned long length = 0xAE0;
	unsigned int *pPSCCtrl = NULL;
	unsigned int *pPTCMD = NULL;
#endif
	req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
#if 0
	if ((dev_fp=open("/dev/mem",O_RDWR|O_SYNC))==-1)
	{
		printf("dev_fp Fail!! \n");
		return ;
	}
	pMem_map=mmap(	(void	*)phyAddr,length/*0x10000000*/,
					PROT_READ|PROT_WRITE|PROT_EXEC,MAP_SHARED,
					dev_fp,phyAddr);

	if (pMem_map!=(void *)phyAddr)
	{
		printf("pMem_map Fail!! \n");
		return ;
	}


	sprintf(cmd, "killall -KILL av_server.out \n");
	system(cmd);

	pPTCMD 		= (unsigned int *)( (char *)pMem_map+0x120);
	pPSCCtrl 	= (unsigned int *)( (char *)pMem_map+0xA00);

	/*EDMA */
	*(pPSCCtrl+0) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*EDMA */
	*(pPSCCtrl+1) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*EDMA */
	*(pPSCCtrl+2) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*EDMA */
	*(pPSCCtrl+3) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*EDMA */
	*(pPSCCtrl+4) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*VPSS */
	*(pPSCCtrl+47) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*MJCP */
	*(pPSCCtrl+50) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);
	/*HDVICP */
	*(pPSCCtrl+51) 	= 2;
	*pPTCMD 		= 1;
	usleep(5000);

	if( pMem_map )
		munmap(pMem_map, length);

	if( dev_fp >= 0)
		close(dev_fp);

	sprintf(cmd, "shutdown -r now\n");
	system(cmd);
#endif
	sync();
	reboot(RB_AUTOBOOT );

	return;
}
void set_paradefaultcmd(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DEFAULT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_sysclearcmd(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	cleanSystemLog(); // clears the log entries in the file
	do {
		ControlSystemData(SFIELD_SET_CLEARSYSLOG, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_accessclearcmd(request *req, COMMAND_ARGUMENT *argm)
{
	__u8 value = atoi(argm->value);
	cleanAccessLog(); // clears the log entries in the log file
	do {
		ControlSystemData(SFIELD_SET_CLEARACCESSLOG, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA specific Parameters */

/***************************************************************************
 *                                                                         *
 ***************************************************************************/
void dmva_event_start(request *req, COMMAND_ARGUMENT *argm)
{
	int value = atoi(argm->value);
	do {
		if (value < 0)
			break;
		dmva_event_location = value;
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/***************************************************************************
 *                                                                         *
 ***************************************************************************/

void set_dmvaEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVAENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaExptAlgoFrameRate(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVAALGOFRAMERATE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaExptAlgoDetectFreq(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVAALGODETECTFREQ, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaExptEvtRecordEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVAEVTRECORDENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaExptSmetaTrackerEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVASMETATRACKERENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA TRIP Zone Page specific Parameters */

void set_dmvaTZSensitivity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_SENSITIVITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dmvaTZPersonMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 21;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_PERSONMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZPersonMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 22;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_PERSONMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dmvaTZVehicleMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 23;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_VEHICLEMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZVehicleMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 24;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_VEHICLEMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZPersonMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 25;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_PERSONMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dmvaTZPersonMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 26;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_PERSONMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}
void set_dmvaTZVehicleMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 27;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_VEHICLEMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZVehicleMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 28;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_VEHICLEMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZDirection(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 29;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_DIRECTION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZPresentAdjust(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 30;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_PRESENTADJUST, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaTZEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 31;
#endif
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZZone1_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZZone2_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaTZZone1_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaTZZone1_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone1_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE1_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}





void set_dmvaTZZone2_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaTZZone2_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaTZZone2_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_ZONE2_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


/* DMVA TZ save/load params */
void set_dmvaTZSave(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_SAVE, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaTZLoad(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_TZ_LOAD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



/* Set DMVA Main Page specific Parameters */
void set_dmvaCamID(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_CAMID, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaGetSchedule(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_GETSCHEDULE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaCfgFD(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_FD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaCfgCTDImdSmetaTzOc(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_CTDIMDSMETATZOC, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaDisplayOptions(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_DISPLAYOPTIONS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventRecordingVAME(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_RECORDINGVAME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListActionPlaySendSearchTrash(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_ACTIONPLAYSENDSEARCHTRASH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListSelectEvent(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_SELECTEVENT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListArchiveFlag(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_ARCHIVEFLAG, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListEventType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EVENTTYPE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA CTD Page specific Parameters */
void set_dmvaCfgTDEnv1(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_CTD_ENV1, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaCfgTDEnv2(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_CTD_ENV2, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaCfgTDSensitivity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_CTD_SENSITIVITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaCfgTDResetTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_CTD_RESETTIME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA OC Page specific Parameters */
void set_dmvaOCSensitivity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_SENSITIVITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCObjectWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_OBJECTWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCObjectHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_OBJECTHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCDirection(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_DIRECTION, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCLeftRight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_LEFTRIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCTopBottom(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_TOPBOTTOM, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Save/Load DMVA OC params */
void set_dmvaOCSave(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_SAVE, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOCLoad(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_OC_LOAD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA SMETA Page specific Parameters */
void set_dmvaSMETASensitivity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_SENSITIVITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAEnableStreamingData(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ENABLESTREAMINGDATA, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAStreamBB(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_STREAMBB, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAStreamVelocity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_STREAMVELOCITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAStreamHistogram(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_STREAMHISTOGRAM, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAStreamMoments(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_STREAMMOMENTS, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAPresentAdjust(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_PRESENTADJUST, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAPersonMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_PERSONMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAPersonMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_PERSONMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAVehicleMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_VEHICLEMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAVehicleMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_VEHICLEMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAPersonMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_PERSONMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAPersonMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_PERSONMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAVehicleMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_VEHICLEMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAVehicleMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_VEHICLEMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETANumZones(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_NUMZONES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAZone1_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaSMETAZone1_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaSMETAZone1_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETAZone_1_Label(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_ZONE1_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Save/Load DMVA SMETA params */
void set_dmvaSMETASave(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_SAVE, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaSMETALoad(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SMETA_LOAD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* Set DMVA IMD Page specific Parameters */
void set_dmvaIMDSensitivity(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_SENSITIVITY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDPresentAdjust(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_PRESENTADJUST, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDPersonMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_PERSONMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDPersonMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_PERSONMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDVehicleMinWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_VEHICLEMINWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDVehicleMinHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_VEHICLEMINHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDPersonMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_PERSONMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDPersonMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_PERSONMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDVehicleMaxWidth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_VEHICLEMAXWIDTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDVehicleMaxHeight(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_VEHICLEMAXHEIGHT, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDNumZones(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_NUMZONES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZoneEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
#ifdef TESTING_GUI_PARAMS
	value = 20;
#endif

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONEENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}








/* Set DMVA IMD Page Zone specific Parameters */


void set_dmvaIMDZone1_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone2_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone3_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone4_ROI_numSides(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_ROI_NUMSIDES, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone1_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaIMDZone1_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone1_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}





void set_dmvaIMDZone2_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaIMDZone2_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone2_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaIMDZone3_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone3_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}





void set_dmvaIMDZone4_x01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_x16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_X16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}




void set_dmvaIMDZone4_y01(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y01, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y02(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y02, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y03(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y03, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y04(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y04, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y05(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y05, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y06(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y06, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y07(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y07, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y08(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y08, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y09(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y09, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y10(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y10, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y11(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y11, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y12(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y12, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y13(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y13, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y14(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y14, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y15(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y15, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone4_y16(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_Y16, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}



void set_dmvaIMDZone_1_Label(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE1_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaIMDZone_2_Label(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE2_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone_3_Label(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE3_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDZone_4_Label(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_ZONE4_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* DMVA IMD save/load params */

void set_dmvaIMDSave(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_SAVE, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaIMDLoad(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_IMD_LOAD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListStartMonth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_START_MONTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListStartDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_START_DAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_dmvaEventListStartTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_START_TIME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_dmvaEventListEndMonth(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_END_MONTH, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListEndDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_END_DAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_dmvaEventListEndTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_END_TIME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

void set_dmvaEventListTimeStamp(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_TIME_STAMP_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventListEventDetails(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_MAIN_EL_EVENT_DETAILS_LABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}

/* DMVA ADVANCED SETTINGS  */

void set_dmvaObjectMetaData(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OBJECTMETADATA, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOverlayPropertiesFontType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTTYPE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaOverlayPropertiesFontSize(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTSIZE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOverlayPropertiesFontColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPFONTCOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaOverlayPropertiesBoxColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPBOXCOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOverlayPropertiesCentroidColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPCENTROIDCOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaOverlayPropertiesActiveZoneColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPACTIVEZONECOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOverlayPropertiesInactiveZoneColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPINACTIVEZONECOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaOverlayPropertiesVelocityColor(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_OVLPROPVELOCITYCOLOR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaEnvironmentProfileDesc(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_ENVPROFILEDESC, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);

}


void set_dmvaEnvironmentProfileLoad(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_ENVPROFILELOAD, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEnvironmentProfileStore(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_ENVPROFILESTORE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEnvironmentProfileClear(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_ENVPROFILECLEAR, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEnvironmentProfileState(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);
	do {
		ControlSystemData(SFIELD_SET_DMVA_ADVANCED_ENVPROFILESTATE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


/* DMVA event delete */
void set_dmvaEventDeleteIndex(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_EVENT_DELETE_INDEX, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventDeleteStartIndex(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_EVENT_DELETE_START_INDEX, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventDeleteEndIndex(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_EVENT_DELETE_END_INDEX, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaEventDeleteAll(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_EVENT_DELETE_ALL, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* DMVA Scheduler */
void set_dmvaScheduleEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleRuleType(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_RULETYPE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


void set_dmvaScheduleLoadSetting(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_LOADSETTING, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleRuleLabel(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_RULELABEL, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleStartDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_STARTDAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleStartTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_STARTTIME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleEndDay(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_ENDDAY, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleEndTime(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_ENDTIME, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleStart(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_START, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

void set_dmvaScheduleDelRule(request *req, COMMAND_ARGUMENT *argm)
{
	do {
		ControlSystemData(SFIELD_SET_DMVA_SCH_DELRULE, (void *)argm->value, strlen(argm->value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);
	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}

/* DMVA Enc ROI Enable */
void set_dmvaEncRoiEnable(request *req, COMMAND_ARGUMENT *argm)
{
	unsigned int value = atoi(argm->value);

	do {
		ControlSystemData(SFIELD_SET_DMVA_ENC_ROI_ENABLE, (void *)&value, sizeof(value));
		req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		return;
	} while (0);

	req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
}


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
#define HASH_TABLE_SIZE	(sizeof(HttpOptionTable)/sizeof(HTTP_OPTION))
HTTP_OPTION HttpOptionTable [] =
{

	{ "paratest"            , paratest                  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "language"		    , set_language	           	, AUTHORITY_VIEWER, FALSE,  TRUE, NULL },

	{ "adduser"		        , add_user		        	, AUTHORITY_ADMIN, FALSE,  TRUE, NULL },
	{ "deluser"		        , del_user	    	    	, AUTHORITY_ADMIN, FALSE,  TRUE, NULL },
	{ "checkpassword"       , checkpassword             , AUTHORITY_ADMIN, FALSE,  TRUE, NULL },

	{ "motionxblock"		, get_motionxblock			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "motionyblock"		, get_motionyblock			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      LIVE VIDEO SCREEN */

	{ "democfg"				, set_democfg				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "clicksnapfilename"	, set_clicksnapfilename	    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "clicksnapstorage"	, set_clicksnapstorage	    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getalarmstatus"		, get_alarmstatus			, AUTHORITY_VIEWER, FALSE,  TRUE, NULL },

	/*      VIDEO & IMAGE SCREEN 				*/

	{ "title"				, set_cameratitle			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "videocodec"			, set_videomode				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "videocodeccombo"		, set_videocodeccombo		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "videocodecres"		, set_videocodecres			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "bitrate1"			, set_mpeg41bitrate			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "bitrate2"			, set_mpeg42bitrate			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "livequality"			, set_jpgquality			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "framerate1"			, set_framerate1			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "framerate2"			, set_framerate2			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "framerate3"			, set_framerate3			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ratecontrol1"		, set_ratecontrol1	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ratecontrol2"		, set_ratecontrol2	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "datestampenable1"	, set_datestampenable1	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "datestampenable2"	, set_datestampenable2	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "datestampenable3"	, set_datestampenable3	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timestampenable1"	, set_timestampenable1	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timestampenable2"	, set_timestampenable2	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timestampenable3"	, set_timestampenable3	   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoenable1"	    , set_logoenable1	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoenable2" 	, set_logoenable2	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoenable3"	    , set_logoenable3	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoposition1"	, set_logoposition1	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoposition2"	, set_logoposition2	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "logoposition3"	, set_logoposition3	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textenable1"	    , set_textenable1	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textenable2"	    , set_textenable2	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textenable3"	    , set_textenable3	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textposition1"	, set_textposition1	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textposition2"	, set_textposition2	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "textposition3"	, set_textposition3	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "overlaytext1"	, set_overlaytext1			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "overlaytext2"	, set_overlaytext2			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "overlaytext3"	, set_overlaytext3			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "detailinfo1"	    , set_detailinfo1	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "detailinfo2"	    , set_detailinfo2	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "detailinfo3"	    , set_detailinfo3	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "encryptvideo"	, set_encryption			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "localdisplay"	, set_localdisplay          , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "mirctrl"			, set_mirctrl				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "aviduration"		, set_aviduration			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "aviformat"		, set_aviformat				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*       VIDEO ADVANCED PAGE    */

	{ "ipratio1"	    , set_ipratio1	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ipratio2"	    , set_ipratio2	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ipratio3"	    , set_ipratio3	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "forceiframe1"	, set_forceIframe1	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "forceiframe2"	, set_forceIframe2	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "forceiframe3"	, set_forceIframe3	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpinit1"	        , set_qpinit1	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpinit2"	        , set_qpinit2	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpinit3"         , set_qpinit3	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmin1"	        , set_qpmin1	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmin2"	        , set_qpmin2	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmin3"          , set_qpmin3	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmax1"	        , set_qpmax1	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmax2"	        , set_qpmax2	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "qpmax3"	        , set_qpmax3	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "meconfig1"	    , set_meconfig1	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "meconfig2"	    , set_meconfig2	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "meconfig3"	    , set_meconfig3	       , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "packetsize1"	    , set_packetsize1	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "packetsize2"	    , set_packetsize2	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "packetsize3"	    , set_packetsize3	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "regionofinterestenable1"	, set_regionofinterestenable1	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "regionofinterestenable2"	, set_regionofinterestenable2	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "regionofinterestenable3"	, set_regionofinterestenable3	   , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1x1"	            , set_str1x1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1y1"	            , set_str1y1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1w1"	            , set_str1w1	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1h1"	            , set_str1h1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1x2"	            , set_str1x2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1y2"	            , set_str1y2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1w2"	            , set_str1w2	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1h2"	            , set_str1h2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1x3"	            , set_str1x3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1y3"	            , set_str1y3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1w3"	            , set_str1w3	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str1h3"	            , set_str1h3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2x1"	            , set_str2x1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2y1"	            , set_str2y1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2w1"	            , set_str2w1	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2h1"	            , set_str2h1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2x2"	            , set_str2x2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2y2"	            , set_str2y2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2w2"	            , set_str2w2	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2h2"	            , set_str2h2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2x3"	            , set_str2x3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2y3"	            , set_str2y3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2w3"	            , set_str2w3	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str2h3"	            , set_str2h3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3x1"	            , set_str3x1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3y1"	            , set_str3y1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3w1"	            , set_str3w1	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3h1"	            , set_str3h1	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3x2"	            , set_str3x2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3y2"	            , set_str3y2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3w2"	            , set_str3w2	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3h2"	            , set_str3h2	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3x3"	            , set_str3x3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3y3"	            , set_str3y3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3w3"	            , set_str3w3	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "str3h3"	            , set_str3h3	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      DMVA SCREENS 				*/
	{ "dmvaeventstart"	   	, dmva_event_start		    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenable"				, set_dmvaEnable						, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaexptalgoframerate"	, set_dmvaExptAlgoFrameRate				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaexptalgodetectfreq"	, set_dmvaExptAlgoDetectFreq			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaexptevtrecordenable"	, set_dmvaExptEvtRecordEnable			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaexptsmetatrackerenable"	, set_dmvaExptSmetaTrackerEnable	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* DMVA MAIN  PAGE */
	{ "dmvacamid"      							, set_dmvaCamID								, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvagetschedule"      					, set_dmvaGetSchedule						, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvacfgfd"      							, set_dmvaCfgFD								, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvacfgctdimdsmostzoc"      				, set_dmvaCfgCTDImdSmetaTzOc					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvadisplayoptions"      					, set_dmvaDisplayOptions						, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventrecordingvame"      				, set_dmvaEventRecordingVAME					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistactionplaysendsearchtrash"   , set_dmvaEventListActionPlaySendSearchTrash	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistselectevent"      			, set_dmvaEventListSelectEvent				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistarchiveflag"      			, set_dmvaEventListArchiveFlag				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlisteventtype"      				, set_dmvaEventListEventType					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventliststartmonth"      	, set_dmvaEventListStartMonth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventliststartday"      	    , set_dmvaEventListStartDay				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventliststarttime"      	    , set_dmvaEventListStartTime				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistendmonth"      	    , set_dmvaEventListEndMonth				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistendday"      	    , set_dmvaEventListEndDay				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlistendtime"      	    , set_dmvaEventListEndTime				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlisttimestamp"      	    , set_dmvaEventListTimeStamp				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventlisteventdetails"      	, set_dmvaEventListEventDetails			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* DMVA ADVANCED SETTINGS  PAGE */
	{ "dmvaobjectmetadata"      				, set_dmvaObjectMetaData			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesfonttype"      		, set_dmvaOverlayPropertiesFontType	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesfontsize"			, set_dmvaOverlayPropertiesFontSize	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesfontcolor"			, set_dmvaOverlayPropertiesFontColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesboxcolor"  			, set_dmvaOverlayPropertiesBoxColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiescentriodcolor"  	, set_dmvaOverlayPropertiesCentroidColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesactivezonecolor"  	, set_dmvaOverlayPropertiesActiveZoneColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesinactivezonecolor" 	, set_dmvaOverlayPropertiesInactiveZoneColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoverlaypropertiesvelocitycolor"  	, set_dmvaOverlayPropertiesVelocityColor, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenvironmentprofileload"      			, set_dmvaEnvironmentProfileLoad		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenvironmentprofilestore"     			, set_dmvaEnvironmentProfileStore		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenvironmentprofileclear"     			, set_dmvaEnvironmentProfileClear		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenvironmentprofilestate"     			, set_dmvaEnvironmentProfileState		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaenvironmentprofiledesc"      			, set_dmvaEnvironmentProfileDesc		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },


	/* DMVA event delete */
	{ "dmvaeventdeleteindex"      	, set_dmvaEventDeleteIndex			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventdeletestartindex"      	, set_dmvaEventDeleteStartIndex			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventdeleteendindex"      	, set_dmvaEventDeleteEndIndex			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaeventdeleteall"      	, set_dmvaEventDeleteAll			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Camera Tamper Detect  PAGE */
	{ "dmvacfgtdenv1"      			, set_dmvaCfgTDEnv1					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvacfgtdenv2"      			, set_dmvaCfgTDEnv2					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvacfgtdsensitivity"         , set_dmvaCfgTDSensitivity			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvacfgtdresettime"      	    , set_dmvaCfgTDResetTime				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Trip Zone PAGE */
	{ "dmvatzsensitivity"      	    , set_dmvaTZSensitivity				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzpersonminwidth"      	, set_dmvaTZPersonMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzpersonminheight"      	, set_dmvaTZPersonMinHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzvehicleminwidth"      	, set_dmvaTZVehicleMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzvehicleminheight"      	, set_dmvaTZVehicleMinHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzpersonmaxwidth"      	, set_dmvaTZPersonMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzpersonmaxheight"      	, set_dmvaTZPersonMaxHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzvehiclemaxwidth"      	, set_dmvaTZVehicleMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzvehiclemaxheight"      	, set_dmvaTZVehicleMaxHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzdirection"      	    , set_dmvaTZDirection				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzpresentadjust"    	    , set_dmvaTZPresentAdjust			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzenable"      	        , set_dmvaTZEnable					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvatzzone1_roi_numsides"      	        , set_dmvaTZZone1_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_roi_numsides"      	        , set_dmvaTZZone2_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvatzzone1_x01"      	        , set_dmvaTZZone1_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x02"      	        , set_dmvaTZZone1_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x03"      	        , set_dmvaTZZone1_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x04"      	        , set_dmvaTZZone1_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x05"      	        , set_dmvaTZZone1_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x06"      	        , set_dmvaTZZone1_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x07"      	        , set_dmvaTZZone1_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x08"      	        , set_dmvaTZZone1_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x09"      	        , set_dmvaTZZone1_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x10"      	        , set_dmvaTZZone1_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x11"      	        , set_dmvaTZZone1_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x12"      	        , set_dmvaTZZone1_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x13"      	        , set_dmvaTZZone1_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x14"      	        , set_dmvaTZZone1_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x15"      	        , set_dmvaTZZone1_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_x16"      	        , set_dmvaTZZone1_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvatzzone1_y01"      	        , set_dmvaTZZone1_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y02"      	        , set_dmvaTZZone1_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y03"      	        , set_dmvaTZZone1_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y04"      	        , set_dmvaTZZone1_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y05"      	        , set_dmvaTZZone1_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y06"      	        , set_dmvaTZZone1_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y07"      	        , set_dmvaTZZone1_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y08"      	        , set_dmvaTZZone1_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y09"      	        , set_dmvaTZZone1_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y10"      	        , set_dmvaTZZone1_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y11"      	        , set_dmvaTZZone1_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y12"      	        , set_dmvaTZZone1_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y13"      	        , set_dmvaTZZone1_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y14"      	        , set_dmvaTZZone1_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y15"      	        , set_dmvaTZZone1_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone1_y16"      	        , set_dmvaTZZone1_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvatzzone2_x01"      	        , set_dmvaTZZone2_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x02"      	        , set_dmvaTZZone2_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x03"      	        , set_dmvaTZZone2_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x04"      	        , set_dmvaTZZone2_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x05"      	        , set_dmvaTZZone2_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x06"      	        , set_dmvaTZZone2_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x07"      	        , set_dmvaTZZone2_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x08"      	        , set_dmvaTZZone2_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x09"      	        , set_dmvaTZZone2_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x10"      	        , set_dmvaTZZone2_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x11"      	        , set_dmvaTZZone2_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x12"      	        , set_dmvaTZZone2_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x13"      	        , set_dmvaTZZone2_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x14"      	        , set_dmvaTZZone2_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x15"      	        , set_dmvaTZZone2_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_x16"      	        , set_dmvaTZZone2_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvatzzone2_y01"      	        , set_dmvaTZZone2_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y02"      	        , set_dmvaTZZone2_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y03"      	        , set_dmvaTZZone2_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y04"      	        , set_dmvaTZZone2_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y05"      	        , set_dmvaTZZone2_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y06"      	        , set_dmvaTZZone2_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y07"      	        , set_dmvaTZZone2_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y08"      	        , set_dmvaTZZone2_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y09"      	        , set_dmvaTZZone2_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y10"      	        , set_dmvaTZZone2_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y11"      	        , set_dmvaTZZone2_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y12"      	        , set_dmvaTZZone2_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y13"      	        , set_dmvaTZZone2_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y14"      	        , set_dmvaTZZone2_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y15"      	        , set_dmvaTZZone2_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzzone2_y16"      	        , set_dmvaTZZone2_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Save/Load DMVA TZ params */
	{ "dmvatzsave"      	        , set_dmvaTZSave					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvatzload"      	        , set_dmvaTZLoad					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Object Counting  PAGE */
	{ "dmvaocsensitivity"      		, set_dmvaOCSensitivity				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocobjectwidth"      		, set_dmvaOCObjectWidth				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocobjectheight"      		, set_dmvaOCObjectHeight				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocdirection"      		, set_dmvaOCDirection				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocenable"      			, set_dmvaOCEnable					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocleftright"      		, set_dmvaOCLeftRight				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaoctopbottom"      		, set_dmvaOCTopBottom				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

    /* Save/Load DMVA OC params */
	{ "dmvaocsave"      	        , set_dmvaOCSave					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaocload"      	        , set_dmvaOCLoad					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Streaming MOS  PAGE */
	{ "dmvasmetasensitivity"      	, set_dmvaSMETASensitivity			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetaenablestreamingdata"  , set_dmvaSMETAEnableStreamingData	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetastreambb"      		, set_dmvaSMETAStreamBB				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetastreamvelocity"      	, set_dmvaSMETAStreamVelocity			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetastreamhistogram"      , set_dmvaSMETAStreamHistogram		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetastreammoments"      	, set_dmvaSMETAStreamMoments			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetapresentadjust"      	, set_dmvaSMETAPresentAdjust			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetapersonminwidth"     	, set_dmvaSMETAPersonMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetapersonminheight"    	, set_dmvaSMETAPersonMinHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetavehicleminwidth"    	, set_dmvaSMETAVehicleMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetavehicleminheight"   	, set_dmvaSMETAVehicleMinHeight		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetapersonmaxwidth"		, set_dmvaSMETAPersonMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetapersonmaxheight"      	, set_dmvaSMETAPersonMaxHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetavehiclemaxwidth"      	, set_dmvaSMETAVehicleMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetavehiclemaxheight"      , set_dmvaSMETAVehicleMaxHeight		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetanumzones"      		, set_dmvaSMETANumZones				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_roi_numsides"      	        , set_dmvaSMETAZone1_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x01"      	        , set_dmvaSMETAZone1_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x02"      	        , set_dmvaSMETAZone1_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x03"      	        , set_dmvaSMETAZone1_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x04"      	        , set_dmvaSMETAZone1_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x05"      	        , set_dmvaSMETAZone1_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x06"      	        , set_dmvaSMETAZone1_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x07"      	        , set_dmvaSMETAZone1_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x08"      	        , set_dmvaSMETAZone1_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x09"      	        , set_dmvaSMETAZone1_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x10"      	        , set_dmvaSMETAZone1_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x11"      	        , set_dmvaSMETAZone1_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x12"      	        , set_dmvaSMETAZone1_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x13"      	        , set_dmvaSMETAZone1_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x14"      	        , set_dmvaSMETAZone1_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x15"      	        , set_dmvaSMETAZone1_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_x16"      	        , set_dmvaSMETAZone1_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvasmetazone1_y01"      	        , set_dmvaSMETAZone1_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y02"      	        , set_dmvaSMETAZone1_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y03"      	        , set_dmvaSMETAZone1_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y04"      	        , set_dmvaSMETAZone1_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y05"      	        , set_dmvaSMETAZone1_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y06"      	        , set_dmvaSMETAZone1_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y07"      	        , set_dmvaSMETAZone1_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y08"      	        , set_dmvaSMETAZone1_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y09"      	        , set_dmvaSMETAZone1_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y10"      	        , set_dmvaSMETAZone1_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y11"      	        , set_dmvaSMETAZone1_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y12"      	        , set_dmvaSMETAZone1_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y13"      	        , set_dmvaSMETAZone1_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y14"      	        , set_dmvaSMETAZone1_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y15"      	        , set_dmvaSMETAZone1_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetazone1_y16"      	        , set_dmvaSMETAZone1_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvasmetazone_1_label"      	        , set_dmvaSMETAZone_1_Label					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Save/Load DMVA SM params */
	{ "dmvasmetasave"      	        , set_dmvaSMETASave					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvasmetaload"      	        , set_dmvaSMETALoad					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* IMD  PAGE */
	{ "dmvaimdsensitivity"      		, set_dmvaIMDSensitivity				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdpresentadjust"      	, set_dmvaIMDPresentAdjust			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdpersonminwidth"     	, set_dmvaIMDPersonMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdpersonminheight"    	, set_dmvaIMDPersonMinHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdvehicleminwidth"    	, set_dmvaIMDVehicleMinWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdvehicleminheight"   	, set_dmvaIMDVehicleMinHeight		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdpersonmaxwidth"		, set_dmvaIMDPersonMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdpersonmaxheight"      	, set_dmvaIMDPersonMaxHeight			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdvehiclemaxwidth"      	, set_dmvaIMDVehicleMaxWidth			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdvehiclemaxheight"      , set_dmvaIMDVehicleMaxHeight		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdnumzones"      		, set_dmvaIMDNumZones				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzoneenable"      		, set_dmvaIMDZoneEnable				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone1_roi_numsides"      	        , set_dmvaIMDZone1_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_roi_numsides"      	        , set_dmvaIMDZone2_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_roi_numsides"      	        , set_dmvaIMDZone3_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_roi_numsides"      	        , set_dmvaIMDZone4_ROI_numSides					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone1_x01"      	        , set_dmvaIMDZone1_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x02"      	        , set_dmvaIMDZone1_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x03"      	        , set_dmvaIMDZone1_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x04"      	        , set_dmvaIMDZone1_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x05"      	        , set_dmvaIMDZone1_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x06"      	        , set_dmvaIMDZone1_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x07"      	        , set_dmvaIMDZone1_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x08"      	        , set_dmvaIMDZone1_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x09"      	        , set_dmvaIMDZone1_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x10"      	        , set_dmvaIMDZone1_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x11"      	        , set_dmvaIMDZone1_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x12"      	        , set_dmvaIMDZone1_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x13"      	        , set_dmvaIMDZone1_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x14"      	        , set_dmvaIMDZone1_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x15"      	        , set_dmvaIMDZone1_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_x16"      	        , set_dmvaIMDZone1_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone1_y01"      	        , set_dmvaIMDZone1_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y02"      	        , set_dmvaIMDZone1_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y03"      	        , set_dmvaIMDZone1_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y04"      	        , set_dmvaIMDZone1_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y05"      	        , set_dmvaIMDZone1_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y06"      	        , set_dmvaIMDZone1_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y07"      	        , set_dmvaIMDZone1_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y08"      	        , set_dmvaIMDZone1_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y09"      	        , set_dmvaIMDZone1_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y10"      	        , set_dmvaIMDZone1_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y11"      	        , set_dmvaIMDZone1_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y12"      	        , set_dmvaIMDZone1_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y13"      	        , set_dmvaIMDZone1_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y14"      	        , set_dmvaIMDZone1_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y15"      	        , set_dmvaIMDZone1_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone1_y16"      	        , set_dmvaIMDZone1_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone2_x01"      	        , set_dmvaIMDZone2_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x02"      	        , set_dmvaIMDZone2_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x03"      	        , set_dmvaIMDZone2_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x04"      	        , set_dmvaIMDZone2_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x05"      	        , set_dmvaIMDZone2_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x06"      	        , set_dmvaIMDZone2_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x07"      	        , set_dmvaIMDZone2_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x08"      	        , set_dmvaIMDZone2_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x09"      	        , set_dmvaIMDZone2_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x10"      	        , set_dmvaIMDZone2_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x11"      	        , set_dmvaIMDZone2_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x12"      	        , set_dmvaIMDZone2_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x13"      	        , set_dmvaIMDZone2_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x14"      	        , set_dmvaIMDZone2_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x15"      	        , set_dmvaIMDZone2_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_x16"      	        , set_dmvaIMDZone2_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone2_y01"      	        , set_dmvaIMDZone2_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y02"      	        , set_dmvaIMDZone2_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y03"      	        , set_dmvaIMDZone2_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y04"      	        , set_dmvaIMDZone2_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y05"      	        , set_dmvaIMDZone2_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y06"      	        , set_dmvaIMDZone2_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y07"      	        , set_dmvaIMDZone2_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y08"      	        , set_dmvaIMDZone2_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y09"      	        , set_dmvaIMDZone2_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y10"      	        , set_dmvaIMDZone2_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y11"      	        , set_dmvaIMDZone2_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y12"      	        , set_dmvaIMDZone2_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y13"      	        , set_dmvaIMDZone2_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y14"      	        , set_dmvaIMDZone2_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y15"      	        , set_dmvaIMDZone2_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone2_y16"      	        , set_dmvaIMDZone2_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone3_x01"      	        , set_dmvaIMDZone3_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x02"      	        , set_dmvaIMDZone3_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x03"      	        , set_dmvaIMDZone3_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x04"      	        , set_dmvaIMDZone3_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x05"      	        , set_dmvaIMDZone3_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x06"      	        , set_dmvaIMDZone3_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x07"      	        , set_dmvaIMDZone3_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x08"      	        , set_dmvaIMDZone3_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x09"      	        , set_dmvaIMDZone3_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x10"      	        , set_dmvaIMDZone3_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x11"      	        , set_dmvaIMDZone3_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x12"      	        , set_dmvaIMDZone3_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x13"      	        , set_dmvaIMDZone3_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x14"      	        , set_dmvaIMDZone3_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x15"      	        , set_dmvaIMDZone3_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_x16"      	        , set_dmvaIMDZone3_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone3_y01"      	        , set_dmvaIMDZone3_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y02"      	        , set_dmvaIMDZone3_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y03"      	        , set_dmvaIMDZone3_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y04"      	        , set_dmvaIMDZone3_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y05"      	        , set_dmvaIMDZone3_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y06"      	        , set_dmvaIMDZone3_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y07"      	        , set_dmvaIMDZone3_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y08"      	        , set_dmvaIMDZone3_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y09"      	        , set_dmvaIMDZone3_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y10"      	        , set_dmvaIMDZone3_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y11"      	        , set_dmvaIMDZone3_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y12"      	        , set_dmvaIMDZone3_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y13"      	        , set_dmvaIMDZone3_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y14"      	        , set_dmvaIMDZone3_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y15"      	        , set_dmvaIMDZone3_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone3_y16"      	        , set_dmvaIMDZone3_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone4_x01"      	        , set_dmvaIMDZone4_x01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x02"      	        , set_dmvaIMDZone4_x02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x03"      	        , set_dmvaIMDZone4_x03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x04"      	        , set_dmvaIMDZone4_x04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x05"      	        , set_dmvaIMDZone4_x05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x06"      	        , set_dmvaIMDZone4_x06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x07"      	        , set_dmvaIMDZone4_x07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x08"      	        , set_dmvaIMDZone4_x08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x09"      	        , set_dmvaIMDZone4_x09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x10"      	        , set_dmvaIMDZone4_x10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x11"      	        , set_dmvaIMDZone4_x11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x12"      	        , set_dmvaIMDZone4_x12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x13"      	        , set_dmvaIMDZone4_x13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x14"      	        , set_dmvaIMDZone4_x14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x15"      	        , set_dmvaIMDZone4_x15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_x16"      	        , set_dmvaIMDZone4_x16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "dmvaimdzone4_y01"      	        , set_dmvaIMDZone4_y01					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y02"      	        , set_dmvaIMDZone4_y02					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y03"      	        , set_dmvaIMDZone4_y03					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y04"      	        , set_dmvaIMDZone4_y04					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y05"      	        , set_dmvaIMDZone4_y05					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y06"      	        , set_dmvaIMDZone4_y06					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y07"      	        , set_dmvaIMDZone4_y07					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y08"      	        , set_dmvaIMDZone4_y08					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y09"      	        , set_dmvaIMDZone4_y09					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y10"      	        , set_dmvaIMDZone4_y10					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y11"      	        , set_dmvaIMDZone4_y11					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y12"      	        , set_dmvaIMDZone4_y12					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y13"      	        , set_dmvaIMDZone4_y13					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y14"      	        , set_dmvaIMDZone4_y14					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y15"      	        , set_dmvaIMDZone4_y15					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone4_y16"      	        , set_dmvaIMDZone4_y16					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* DMVA Strings */
	{ "dmvaimdzone_1_label"      	        , set_dmvaIMDZone_1_Label					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone_2_label"      	        , set_dmvaIMDZone_2_Label					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone_3_label"      	        , set_dmvaIMDZone_3_Label					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdzone_4_label"      	        , set_dmvaIMDZone_4_Label					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* Save/Load DMVA IMD params */
	{ "dmvaimdsave"      	        , set_dmvaIMDSave					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaimdload"      	        , set_dmvaIMDLoad					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* DMVA scheduler */
	{ "dmvascheduleenable"      	, set_dmvaScheduleEnable			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvascheduleruletype"      	, set_dmvaScheduleRuleType			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvascheduleloadsetting"     , set_dmvaScheduleLoadSetting		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaschedulerulelabel"      	, set_dmvaScheduleRuleLabel			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaschedulestartday"      	, set_dmvaScheduleStartDay			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaschedulestarttime"      	, set_dmvaScheduleStartTime			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvascheduleendday"      	, set_dmvaScheduleEndDay			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvascheduleendtime"      	, set_dmvaScheduleEndTime			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvaschedulestart"      	    , set_dmvaScheduleStart			    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dmvascheduledelrule"      	, set_dmvaScheduleDelRule			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/* DMVA Enc ROI */
	{ "dmvaencroienable"      	    , set_dmvaEncRoiEnable			    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      ADVANCED SETTING SCREEN 				*/

	{ "motionblock"         , set_motionblock           , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "motionsensitivity"   , set_motionlevel           , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "motioncenable"       , set_motioncenable         , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "motioncvalue"        , set_motioncvalue          , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdetect"				, set_fdetect				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdx"	            	, set_fdx	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdy"	            	, set_fdy	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdw"	            	, set_fdw	                , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdh"	            	, set_fdh	     	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "fdconflevel"     	, set_fdconflevel	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "fddirection"     	, set_fddirection	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "frecognition"		, set_frecognition	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "frconflevel"     	, set_frconflevel	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "frdatabase"      	, set_frdatabase	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "privacymask"     	, set_privacymask	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "maskoptions"     	, set_maskoptions	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      CAMERA SCREEN 				*/

	{ "brightness"				, set_brightness			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "contrast"				, set_contrast				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "saturation"				, set_saturation			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sharpness"				, set_sharpness				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "blc"						, set_blc					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "backlight"				, set_backlight				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dynrange"   				, set_dynrange         		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "maxgain"                 , set_dynrangestrength   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "exposurectrl"			, set_imagesource			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "awb"						, set_white_balance			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "colorkiller"				, set_day_night				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "histogram"     			, set_histogram          	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "binning"					, set_binning				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "img2a"					, set_img2a					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "img2atype"				, set_img2atype				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "priority"                , set_expPriority          	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "vidstb1"  				, set_vidstb				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "lensdistortcorrection"  	, set_ldc					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
  	{ "tnfltctrl"               , set_vnf            		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
 	{ "nfltctrl"                , set_vnfmode             	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "maxexposuretime"         , set_vnfstrength      		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "framectrl"               , set_framectrl            	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "autoirisvol"             , set_autoirisvol          	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      AUDIO SCREEN 				*/

	{ "audioenable"		     , set_audioenable			  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "getaudioenable"       , get_audioenable            , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audiomode"	         , set_audiomode	     	  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audioinvolume"	     , set_audioinvolume          , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "encoding"	         , set_encoding	     	      , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "samplerate"	         , set_samplerate	     	  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audiobitrate"	     , set_audiobitrate	          , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "alarmlevel"	         , set_alarmlevel	     	  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audiooutvolume"	 	 , set_audiooutvolume	      , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audioreceiverenable"	 , set_audioreceiverenable	  , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "audioserverip"	 	 , set_audioserverip	      , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      DATE TIME SCREEN			*/

	{ "newdate"				, set_system_date			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "newtime"				, set_system_time			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getdate"			    , get_date					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "gettime"			    , get_time					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getsntpip"		    , get_sntpip				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timefrequency"		, set_time_frequency		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timezone"			, set_timezone				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "daylight"			, set_daylight				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dateformat"			, set_dateformat			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "tstampformat"		, set_tstampformat	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dateposition"	    , set_dateposition 	      	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "timeposition"	    , set_timeposition 	      	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      NETWORK & PORT SCREEN			*/

	{ "dhcpenable"			, set_dhcpclient			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "netip"				, set_netip					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "netmask"				, set_netmask				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "gateway"				, set_gateway				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "dnsip"				, set_dnsip					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getdhcpenable"	    , get_dhcpclient			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getnetip"		    , get_netip					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getnetmask"		    , get_netmask				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getgateway"		    , get_gateway				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "getdnsip"		    , get_dnsip					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "ftpip"	    	    , set_ftpfqdn				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ftpipport"		    , set_ftpport				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ftpuser"		        , set_ftpuser				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ftppath"		        , set_ftppath   	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ftppassword"		    , set_ftppassword			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "smtpauth"		    , set_smtpauth				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "smtpuser"		    , set_smtpuser		    	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "smtppwd"		        , set_smtppwd			    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "smtpsender"		    , set_smtpsender			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "smtpip"	    		, set_smtpfqdn				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "smtpport"	    	, set_smtpport				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "emailuser"		    , set_smtpto				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "multicast"			, set_multicast				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "sntpip"				, set_sntpfqdn				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	{ "httpport"			, set_httpport				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "gethttpport"		    , get_httpport				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "httpsport"			, set_httpsport				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "portinput"			, set_giointype	        	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "portoutput"			, set_gioouttype	     	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "rs485"				, set_rs485					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      ALARM SCREEN			*/

	{ "alarmenable"         , set_alarmenable	        , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "alarmduration"		, set_alarmduration			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "motionenable"        , set_motionenable          , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "lostalarm"           , set_lostalarm 			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "darkblankalarm"      , set_audioalarm   			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "audioalarm"      	, set_audioalarm   			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "extalarm"            , set_extalarm	            , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "exttriggerinput"     , set_gioinenable	      	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "exttriggeroutput"    , set_giooutenable	      	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "aftpenable"          , set_aftpenable			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "ftpfileformat"		, set_ftpfileformat			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "asmtpenable"			, set_asmtpenable			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "attfileformat"	    , set_attfileformat			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "asmtpattach"			, set_asmtpattach			, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sdaenable"			, set_sdaenable				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sdfileformat"	    , set_sdfileformat		   	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "alarmlocalstorage"   , set_alarmlocalstorage		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "alarmaudioplay"      , set_alarmaudioplay	    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "alarmaudiofile"      , set_alarmaudiofile	    , AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      SCHEDULE SCREEN			*/

	{ "rftpenable"				, set_rftpenable				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sdrenable"				, set_sdrenable					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "recordlocalstorage"  	, set_recordlocalstorage		, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "schedulerepeatenable"    , set_schedulerepeatenable	  	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "schedulenumweeks"        , set_schedulenumweeks	      	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
    { "scheduleinfiniteenable"  , set_scheduleinfiniteenable 	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "schedule"				, set_schedule					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "delschedule"		    	, del_schedule					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      MAINTAINENCE SCREEN			*/

	{ "ipcamrestartcmd"		, restart_ipcam						, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "paradefaultcmd"		, set_paradefaultcmd				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sysclearcmd"			, set_sysclearcmd					, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "accessclearcmd"		, set_accessclearcmd				, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

	/*      SDCARD EXPLORER SCREEN			*/

	{ "sdformat"	    	, sd_format	            	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },
	{ "sdunmount"	    	, sd_unmount	          	, AUTHORITY_OPERATOR, FALSE,  TRUE, NULL },

};

unsigned int hash_cal_value(char *name)
{
	unsigned int value = 0;

	while (*name)
		value = value * 37 + (unsigned int)(*name++);
	return value;
}


void hash_insert_entry(CMD_HASH_TABLE *table, HTTP_OPTION *op)
{
	if (table->entry) {
		op->next = table->entry;
	}
	table->entry = op;
}

CMD_HASH_TABLE *cmd_hash;
int hash_table_init(void)
{
	int i;

	if ( (cmd_hash = (CMD_HASH_TABLE *)calloc(sizeof(CMD_HASH_TABLE), MAX_CMD_HASH_SIZE)) == NULL) {
		return -1;
	}
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		hash_insert_entry(cmd_hash+(hash_cal_value(HttpOptionTable[i].name)%MAX_CMD_HASH_SIZE), HttpOptionTable+i);
	}
	dbg("HASH_TABLE_SIZE = %d\n", HASH_TABLE_SIZE);
	return 0;
}

HTTP_OPTION *http_option_search(char *arg)
{
	HTTP_OPTION *opt;

	opt = cmd_hash[hash_cal_value(arg)%MAX_CMD_HASH_SIZE].entry;

	while (opt) {
		if ( strcmp(opt->name, arg) == 0 )
			return opt;
		opt = opt->next;
	}
	return NULL;
}

void http_run_command(request *req, COMMAND_ARGUMENT *arg, int num)
{
	AUTHORITY authority = req->authority;
	HTTP_OPTION *option;
	int i;

	send_request_ok_api(req);     /* All's well */
	dbg("argument count = %d\n", num);
	for (i=0; i<num; i++) {
		strtolower((unsigned char *)arg[i].name);  // convert the command argument to lowcase
#ifdef DEBUG
		dbg("arg[%d].name=%s", i, arg[i].name);
		if (arg[i].value[0])
			dbg(", value=%s\n", arg[i].value);
		else
			dbg("\n");
#endif
		option = http_option_search(arg[i].name);
		if (option) {
			if (authority <= option->authority && gf_CmdBlocking == 0) {
				arg[i].flags = 0;
				(*option->handler) (req, &arg[i]);
			}
			else {
				req_write(req, OPTION_UA);
				req_write(req, arg[i].name);
				dbg("http_run_command: Permission denied!!!\n");
			}
		}
		else {
			req_write(req, OPTION_UW);
			req_write(req, arg[i].name);
		}
	}
	if ((gf_CmdBlocking = gf_Restarting) > 0)
		dbg("+++ Command Block +++++++++++\n");
}

extern int ShowAllWebValue(char *data, int max_size, AUTHORITY authority);
extern int ShowAllPara(char *data, int max_size, AUTHORITY authority);
int html_ini_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret;

	ret = ShowAllWebValue(addr, max_size, authority);
	return ret;
}

int html_keyword_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret;

	ret = ShowAllPara(addr, max_size, authority);
	return ret;
}

int html_cmdlist_cmd(AUTHORITY authority, char *addr, int max_size)
{
	int ret = 0,i,count = 0,size;
	HTTP_OPTION *option;
	char buf[80];
	for (i=0; i<HASH_TABLE_SIZE; i++) {
		option = &HttpOptionTable[i];
		if (authority > option->authority)
			continue;
		size = sprintf(buf, "%03d.%-25s%d\n", ++count, option->name, option->authority);
		if(ret + size + 1 > max_size){
			ret = sprintf(addr, "Not enogh size to show");
			break;
		}
		ret += sprintf(addr + ret, "%s", buf);
	}
	return ret;
}
#define MMC_NODE "/dev/mmcblk0p1"
#define MMC_PATH "/mnt/mmc/ipnc/"

int http_sdget_cmd(request *req, COMMAND_ARGUMENT *argm, char *addr, int max_size, char *sdpath)
{
	int ret = 0;

	if (argm->value != '\0')
	{
		if( CheckFileExist(MMC_PATH,argm->value) == 0 )
		{
			strcpy(sdpath, MMC_PATH);
			return 0;
		}
	}

	MEM_List_files_To_html( MMC_PATH, MMC_PATH, addr, max_size);
	ret = strlen(addr);

	return ret;
}

extern char html_sdget_basic_hdr[];

void http_sddel_cmd(request *req, COMMAND_ARGUMENT *argm)
{
	char http_ip_addr[100];

	GetIP_Addr(http_ip_addr);

	send_request_ok_api(req);

	if (strcmp(argm->name, "FILE") == 0)
	{
		if ( DeleteFile(MMC_PATH,argm->value) == 0 ) {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_OK "%s\n", argm->name);
		}
		else {
			req->buffer_end += sprintf(req_bufptr(req), OPTION_NG "%s\n", argm->name);
		}
		req->buffer_end += sprintf(req_bufptr(req), "<HR>HTTP Server at ipnc<BR></PRE></BODY></HTML> %s ", html_sdget_basic_hdr);
	}
	else {
		req_write(req, OPTION_UW);
		req_write(req, argm->name);
	}
}

int http_sysget_cmd(char *addr, int max_size)
{
       int ret = 0;

       SysLog_List_To_html(addr, max_size);

	 ret = strlen(addr);

	return ret;
}

int http_accessget_cmd(char *addr, int max_size)
{
       int ret = 0;

       AccessLog_List_To_html(addr, max_size);

	 ret = strlen(addr);

	return ret;
}

int http_actvconlistget_cmd(char *addr, int max_size)
{
       int ret = 0;

       ActvCon_List_To_html(addr, max_size);

	 ret = strlen(addr);

	return ret;
}

int http_paraget_cmd(char *addr, int max_size)
{
       int ret = 0;

       Curr_Param_List_To_html(addr, max_size);

	 ret = strlen(addr);

	return ret;
}

