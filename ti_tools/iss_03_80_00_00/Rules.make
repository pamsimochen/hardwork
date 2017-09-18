# Copyright Texas Instruments
# Default build environment, windows or linux
ifeq ($(OS), )
  OS := Linux
endif
#
ISS_REL_TAG		 := iss
TOOLS_DIR	 := /home/zdai/ENV/812x/RDK/ti_tools
#BASE_HOME 	 := /data/datalocal_videoapps01/user/DM812x/IPNetCam
BASE_HOME 	 := /home/zdai/ENV/812x/RDK


#IMGS_ID := IMGS_MICRON_MT9J003
#IMGS_ID := IMGS_MICRON_MT9P031
IMGS_ID := IMGS_MICRON_AR0331

BASIC_TOOL_DIR   := $(TOOLS_DIR)

TOOLS_INSTALL_DIR := $(BASIC_TOOL_DIR)
ISS_TOOL_DIR	  := $(BASE_HOME)/ti_tools
IPNC_INSTALL_DIR    := $(BASE_HOME)/ipnc_rdk
MCFW_ROOT_PATH    := $(IPNC_INSTALL_DIR)/ipnc_mcfw
BUILD_TOOL_DIR   := $(TOOLS_INSTALL_DIR)/linux_devkit
BUILD_TOOL_PREFIX := $(BUILD_TOOL_DIR)/bin/arm-arago-linux-gnueabi-

# Configure to build Iss Examples since conflict of Utils with RDK
ISS_EXAMPLES_DIR := YES

CODEGEN_PATH_M3   := $(TOOLS_INSTALL_DIR)/cgt470_5_0_6
iss_PATH       	  := $(ISS_TOOL_DIR)/$(ISS_REL_TAG)
iss_algo_PATH     := $(ISS_TOOL_DIR)/iss_alg
edma3lld_PATH     := $(TOOLS_INSTALL_DIR)/edma3_lld_02_11_07_04
fc_PATH           := $(TOOLS_INSTALL_DIR)/framework_components_3_24_00_09
bios_PATH         := $(TOOLS_INSTALL_DIR)/bios_6_35_04_50
xdc_PATH          := $(TOOLS_INSTALL_DIR)/xdctools_3_25_03_72
ipc_PATH 	  := $(TOOLS_INSTALL_DIR)/ipc_1_25_03_15
syslink_PATH      := $(ISS_TOOL_DIR)/syslink_2_21_01_05
hdvpss_PATH       := $(ISS_TOOL_DIR)/hdvpss_01_00_01_37
xdais_PATH        := $(TOOLS_INSTALL_DIR)/xdais_7_24_00_04
TIMMOSAL_PATH      := $(iss_PATH)/packages/ti/psp/iss/timmosal

ISS_INSTALL_DIR  := $(iss_PATH)

FILESYS_INSTALL_DIR	:= /data/datalocal_videoapps01/user/venu/DM812x/IPNetCam_rel_1_8/target
TARGET_FS 	    := ./
TARGET_FS_DIR	    := $(TARGET_FS)

ROOTDIR := $(iss_PATH)

ifeq ($(CORE), )
  CORE := m3vpss
endif

# Default platform
ifeq ($(PLATFORM), )
  PLATFORM := ti814x-evm
  PLATFORM_NAME := IPNC_RDK_PLATFORM
endif

# Default profile
ifeq ($(PROFILE_$(CORE)), )
#PROFILE_$(CORE) := debug
  #PROFILE_$(CORE)	:= whole_program_debug
PROFILE_$(CORE) := release
endif

XDCPATH = $(bios_PATH)/packages;$(xdc_PATH)/packages;$(ipc_PATH)/packages;$(iss_PATH)/packages;$(hdvpss_PATH)/packages;$(xdais_PATH)/packages;$(edma3lld_PATH)/packages;$(fc_PATH)/packages;

# Default klockwork build flag
ifeq ($(KW_BUILD), )
  KW_BUILD := no
endif

export OS
export PLATFORM
export PLATFORM_NAME
export CORE
export PROFILE_$(CORE)
export CODEGEN_PATH_M3
export bios_PATH
export edma3lld_PATH
export xdc_PATH
export iss_PATH
export iss_algo_PATH
export ipc_PATH
export hdvpss_PATH
export ROOTDIR
export XDCPATH
export xdais_PATH
export KW_BUILD
export IMGS_ID
export TIMMOSAL_PATH
export MCFW_ROOT_PATH
export TARGET_FS
export TARGET_FS_DIR


include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(ROOTDIR)/makerules/platform.mk
include $(iss_PATH)/component.mk
