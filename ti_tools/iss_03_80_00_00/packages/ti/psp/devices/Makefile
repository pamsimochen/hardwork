
MODULE_NAME = iss_devices
ifeq ($(IMGS_ID),IMGS_MICRON_MT9J003)
	SRCDIR = src mt9j003/src	
endif
ifeq ($(IMGS_ID), IMGS_MICRON_AR0331)	
	SRCDIR = src ar0331/src
endif
ifeq ($(IMGS_ID), IMGS_MICRON_AR0330)	
	SRCDIR = src ar0330/src
endif
ifeq ($(IMGS_ID), IMGS_PANASONIC_MN34041)	
	SRCDIR = src mn34041/src
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX035)	
	SRCDIR = src imx035/src
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV2715)	
	SRCDIR = src ov2715/src
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX036)
	SRCDIR = src imx036/src	
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV9712)
	SRCDIR = src ov9712/src	
endif
ifeq ($(IMGS_ID),IMGS_OMNIVISION_OV10630)
	SRCDIR = src ov10630/src	
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9P031)
	SRCDIR = src mt9p031/src		
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9D131)
	SRCDIR = src mt9d131/src	
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9M034)
	SRCDIR = src mt9m034/src	
endif
ifeq ($(IMGS_ID),IMGS_TAXAS_TVP514X)
	SRCDIR = src tvp514x/src	
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV7740)	
	SRCDIR = src ov7740/src
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD)
	SRCDIR = src mt9m034_dual/src	
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX136)	
	SRCDIR = src imx136_$(SENSOR_BOARD_VENDOR)/src
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX140)	
	SRCDIR = src imx140/src
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX104)	
	SRCDIR = src imx104/src
endif
ifeq ($(IMGS_ID),IMGS_ALTASENS_AL30210)	
	SRCDIR = src al30210/src
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV2710)	
	SRCDIR = src ov2710/src
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX122)
	SRCDIR = src imx122/src
endif

INCDIR = . $(hdvpss_PATH)/packages 

DEFINE+=-D$(IMGS_ID)

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc iss

MODULE_SRC_BASE_PATH := $(iss_PATH)/packages/ti/psp/devices

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
CFLAGS_LOCAL_COMMON = $(ISS_CFLAGS)

include $(MODULE_SRC_BASE_PATH)/SRC_FILES.MK

ifeq ($(IMGS_ID),IMGS_MICRON_MT9J003)
	include $(MODULE_SRC_BASE_PATH)/mt9j003/SRC_FILES.MK	
endif
ifeq ($(IMGS_ID), IMGS_MICRON_AR0330)
	include $(MODULE_SRC_BASE_PATH)/ar0330/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_MICRON_AR0331)
	include $(MODULE_SRC_BASE_PATH)/ar0331/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_PANASONIC_MN34041)
	include $(MODULE_SRC_BASE_PATH)/mn34041/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX035)
	include $(MODULE_SRC_BASE_PATH)/imx035/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV2715)
	include $(MODULE_SRC_BASE_PATH)/ov2715/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_SONY_IMX036)
	include $(MODULE_SRC_BASE_PATH)/imx036/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV9712)
	include $(MODULE_SRC_BASE_PATH)/ov9712/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_OMNIVISION_OV10630)
	include $(MODULE_SRC_BASE_PATH)/ov10630/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9P031)
	include $(MODULE_SRC_BASE_PATH)/mt9p031/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9D131)
	include $(MODULE_SRC_BASE_PATH)/mt9d131/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9M034)
	include $(MODULE_SRC_BASE_PATH)/mt9m034/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_TAXAS_TVP514X)
	include $(MODULE_SRC_BASE_PATH)/tvp514x/SRC_FILES.MK
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV7740)	
	include $(MODULE_SRC_BASE_PATH)/ov7740/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD)
	include $(MODULE_SRC_BASE_PATH)/mt9m034_dual/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX136)	
	include $(MODULE_SRC_BASE_PATH)/imx136_$(SENSOR_BOARD_VENDOR)/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX140)	
	include $(MODULE_SRC_BASE_PATH)/imx140/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX104)
	include $(MODULE_SRC_BASE_PATH)/imx104/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_ALTASENS_AL30210)	
	include $(MODULE_SRC_BASE_PATH)/al30210/SRC_FILES.MK	
endif
ifeq ($(IMGS_ID), IMGS_OMNIVISION_OV2710)
	include $(MODULE_SRC_BASE_PATH)/ov2710/SRC_FILES.MK
endif
ifeq ($(IMGS_ID),IMGS_SONY_IMX122)
	include $(MODULE_SRC_BASE_PATH)/imx122/SRC_FILES.MK
endif

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
