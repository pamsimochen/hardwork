/** ===========================================================================
* @file sd_config.c
*
* @path $(IPNCPATH)\sys_server\src\
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2009
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asm/types.h>
#include <ipnc_gio_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <system_default.h>
#include <file_msg_drv.h>
#include <unistd.h>
#include "schedule_mng.h"
#include "sd_config.h"

#define SD_INSERTED 		0x01
#define SD_MOUNTED  		0x02
#define SD_INSERT_AND_MOUNT 0x03

#define SD_UMOUNT_MAX_TRIES	5

#define SD_INSERT_PATH				"/sys/block/mmcblk0/size"
#define SD_WRITE_PROTECT_PATH		"/sys/block/mmcblk0/ro"

#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)

extern int gAS_Pause;

char gbSD_WriteProtect = 1;
static __u8 gSdStatus = 0x00;
/**
* @brief Check if SD card is inserted
*
* @retval 1 Inserted
* @retval 0 Not inserted
*/
int IsSDInserted()
{
	int ret = 0;
	FILE *fp;
	fp = fopen(SD_INSERT_PATH, "rb");
	if(fp){
		ret = 1;
		fclose(fp);
	}
	return ret;
}
/**
* @brief Check if SD card is write protected
*
* @retval 1 write protect
* @retval 0 Not write protect
*/
int IsSDReadOnly()
{
	int 	fd_proc = 0;
	char	data[5];
	int	result = 0;

	fd_proc = open(SD_WRITE_PROTECT_PATH, O_RDONLY);

	if( !fd_proc )
	{
		return 0;
	}
	result = read(fd_proc,  data, 1);
	close(fd_proc);
	if( data[0] == '1')
	{
		return 1;
	}else{	
		return 0;
	}
}

/**
* @brief Unmount SD card
* @retval 0 Success
*/
int Unmount()
{
    int i;
	int ret = 0;
	
	for(i = 0;i < SD_UMOUNT_MAX_TRIES;i++)
	{
	    ret = system("umount /mnt/mmc\n");
		if(ret == 0)
		{
		    return 0;
		}
		else
		{
		    fprintf(stderr,"SD Umount try count = %d\n",(i + 1));
		}
		sleep(1);
	}
	
	return -1;
}

/**
* @brief Check if SD card is inserted
*
* @param isInit [I] if now is system initial call
* @param bEnable [I] if really do check or not
* @return Value to determind if system environment update
* @retval 1 status update
* @retval 0 status not update
*/
int CheckSDCard(int isInit, int bEnable)
{
	__u8 bSdInsert;
	static int count = 0;
	char cmd[80];
	if(bEnable == 0)
		bSdInsert = 1;
	else
		bSdInsert = IsSDInserted();
	if (bSdInsert != (gSdStatus & SD_INSERTED) || isInit)
	{
		if(bSdInsert)
		{
			if(isInit){
				/* Because schedule manager is not ready,
				we don't nead to pause and resume it. */
				gAS_Pause = 1;
				system("umount /mnt/mmc\n");
				system("mount -t vfat /dev/mmcblk0 /mnt/mmc\n");
				system("mount -t vfat /dev/mmcblk0p1 /mnt/mmc\n");
				sprintf(cmd, "mkdir -p %s\n", SD_PATH);
				system(cmd);
				count = 0;
				gSdStatus |= SD_INSERT_AND_MOUNT;
				if(bEnable == 0)
					gbSD_WriteProtect = 0;
				else
					gbSD_WriteProtect = IsSDReadOnly();
				fSetSDInsert(gSdStatus);
				gAS_Pause = 0;
				return 1;
			} else if(count < 7){
				count ++;
				return 0;
			}else{
				gAS_Pause = 1;
				PauseSchedule();
				system("umount /mnt/mmc\n");
				system("mount -t vfat /dev/mmcblk0 /mnt/mmc\n");
				system("mount -t vfat /dev/mmcblk0p1 /mnt/mmc\n");
				sprintf(cmd, "mkdir -p %s\n", SD_PATH);
				system(cmd);
				count = 0;
				gSdStatus |= SD_INSERT_AND_MOUNT;
				if(bEnable == 0)
					gbSD_WriteProtect = 0;
				else
					gbSD_WriteProtect = IsSDReadOnly();
				fSetSDInsert(gSdStatus);
				gAS_Pause = 0;
				ResumeSchedule();
				return 1;
			}
		}
		else
		{
			if ((gSdStatus & SD_MOUNTED) || isInit)
				system("umount /mnt/mmc\n");
			gSdStatus &= ~(SD_INSERT_AND_MOUNT);
			fSetSDInsert(gSdStatus);
		}
		return 1;
	}
	return 0;
}

/**
* @brief Unmount SD card
* @retval 0 Success
*/
int SdUnmount(unsigned char value)
{
	/* Stop SD card recording */
	gAS_Pause = 1;
	PauseSchedule();
	StopDmvaEvtRec();
	int ret = 0;
	if (value == 1) {
	    if(Unmount() == 0)
		{
		    gSdStatus &= ~(SD_MOUNTED);
			fSetSDInsert(gSdStatus);		
		}
		else
		{
		    ret = -1;
		}
		sleep(1);
	} else if (value == 8) {
		char cmd[80];
		__u8 bSdInsert = IsSDInserted();
		if (bSdInsert == 1) {
			system("umount /mnt/mmc\n");
			system("mount -t vfat /dev/mmcblk0 /mnt/mmc\n");
			system("mount -t vfat /dev/mmcblk0p1 /mnt/mmc\n");
			sprintf(cmd, "mkdir -p %s\n", SD_PATH);
			ret = system(cmd);
			gSdStatus |= SD_INSERT_AND_MOUNT;
			gbSD_WriteProtect = IsSDReadOnly();
			fSetSDInsert(gSdStatus);
		} else {
			ret = -1;
		}
	}
	gAS_Pause = 0;
	ResumeSchedule();
	return ret;
}
/**
* @brief Format SD card
* @retval -1 Error
*/
int SdFormat()
{
	int ret = 0;
	if(gbSD_WriteProtect)
		return -1;		
	/* Stop SD card recording */
	gAS_Pause = 1;
	PauseSchedule();
	StopDmvaEvtRec();
    Unmount();		
	if(system("mkdosfs /dev/mmcblk0p1\n") && system("mkdosfs /dev/mmcblk0\n")){
		__E("SD format fail\n");
		ret = -1;
	}
	if(system("mount -t vfat /dev/mmcblk0p1 /mnt/mmc/\n") &&
			system("mount -t vfat /dev/mmcblk0 /mnt/mmc/\n")){
		__E("SD mount fail\n");
		ret = -1;
	}
	if(system("mkdir /mnt/mmc/ipnc/\n")){
		__E("SD make dir ipnc fail\n");
		return -1;
	}	
	gAS_Pause = 0;
	ResumeSchedule();		
	return ret;
}
