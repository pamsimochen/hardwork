# File: component.mk
#       This file is component include make file of HDVPSS.

# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_PLATFORM_DEPENDENCY - "yes": means the code for this module depends on
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#

hdvpss_PKG_LIST = hdvpss_drivers hdvpss_platform hdvpss_i2c hdvpss_devices hdvpss_proxyServer

#
# HDVPSS Modules
#

# HDVPSS Drivers
hdvpss_drivers_RELPATH = ti/psp/vps
hdvpss_drivers_PATH = $(hdvpss_PATH)/packages/$(hdvpss_drivers_RELPATH)
hdvpss_drivers_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_drivers_PKG_LIST = hdvpss_drivers
hdvpss_drivers_PLATFORM_DEPENDENCY = yes

# Platform Drivers
hdvpss_platform_RELPATH = ti/psp/platforms
hdvpss_platform_PATH = $(hdvpss_PATH)/packages/$(hdvpss_platform_RELPATH)
hdvpss_platform_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_platform_PKG_LIST = hdvpss_platform
hdvpss_platform_PLATFORM_DEPENDENCY = yes

# I2C Driver
hdvpss_i2c_RELPATH = ti/psp/i2c
hdvpss_i2c_PATH = $(hdvpss_PATH)/packages/$(hdvpss_i2c_RELPATH)
hdvpss_i2c_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_i2c_PKG_LIST = hdvpss_i2c
hdvpss_i2c_PLATFORM_DEPENDENCY = yes

# External Device Drivers
hdvpss_devices_RELPATH = ti/psp/devices
hdvpss_devices_PATH = $(hdvpss_PATH)/packages/$(hdvpss_devices_RELPATH)
hdvpss_devices_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_devices_PKG_LIST = hdvpss_devices
hdvpss_devices_PLATFORM_DEPENDENCY = yes

# Proxy Server
hdvpss_proxyServer_RELPATH = ti/psp/proxyServer
hdvpss_proxyServer_PATH = $(hdvpss_PATH)/packages/$(hdvpss_proxyServer_RELPATH)
hdvpss_proxyServer_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_proxyServer_PKG_LIST = hdvpss_proxyServer
hdvpss_proxyServer_PLATFORM_DEPENDENCY = yes

# Example Utility
hdvpss_examples_utility_RELPATH = ti/psp/examples/utility
hdvpss_examples_utility_PATH = $(hdvpss_PATH)/packages/$(hdvpss_examples_utility_RELPATH)
hdvpss_examples_utility_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_examples_utility_PKG_LIST = hdvpss_examples_utility
hdvpss_examples_utility_PLATFORM_DEPENDENCY = yes

# Links
hdvpss_examples_links_RELPATH = ti/psp/examples/common/vps/chains/links
hdvpss_examples_links_PATH = $(hdvpss_PATH)/packages/$(hdvpss_examples_links_RELPATH)
hdvpss_examples_links_INCLUDE = $(hdvpss_PATH)/packages
hdvpss_examples_links_PKG_LIST = hdvpss_examples_links
hdvpss_examples_links_PLATFORM_DEPENDENCY = yes

# Component specific CFLAGS
CFLAGS_COMP_COMMON +=

# CFLAGS based on platform selected
ifeq ($(PLATFORM),ti816x-evm)
 HDVPSS_CFLAGS = -DTI_816X_BUILD -DPLATFORM_EVM_SI
endif
ifeq ($(PLATFORM),ti816x-sim)
 HDVPSS_CFLAGS = -DTI_816X_BUILD -DPLATFORM_SIM
endif
ifeq ($(PLATFORM),ti814x-evm)
 HDVPSS_CFLAGS = -DTI_814X_BUILD -DPLATFORM_EVM_SI -D_IPNC_HW_PLATFORM_
endif
ifeq ($(PLATFORM),ti814x-sim)
 HDVPSS_CFLAGS = -DTI_814X_BUILD -DPLATFORM_SIM
endif
ifeq ($(PLATFORM),ti8107-evm)
 HDVPSS_CFLAGS = -DTI_8107_BUILD -DPLATFORM_EVM_SI -D_IPNC_HW_PLATFORM_
endif

#Enable for Zebu build
#HDVPSS_CFLAGS += -DPLATFORM_ZEBU

#Enable trace only for non-Klockwork build and for all profiles
ifeq ($(KW_BUILD),no)
 HDVPSS_CFLAGS += -DVPS_TRACE_ENABLE
endif

# Enable standard asserts for klockwork build
ifeq ($(KW_BUILD),yes)
 HDVPSS_CFLAGS += -DVPS_USE_STD_ASSERT -DVPS_ASSERT_ENABLE
endif

#Enable version time stamp if its enabled while building
ifeq ($(VERSION_TS),yes)
 HDVPSS_CFLAGS += -DVERSION_TS_ENABLE
endif

# Enable asserts for debug build only
ifeq ($(PROFILE_$(CORE)),debug)
 HDVPSS_CFLAGS += -DVPS_ASSERT_ENABLE
endif

# Enable custom frame buffer memory size
ifneq ($(CUSTOM_MEM_FRAME_HEAP_SIZE), )
 HDVPSS_CFLAGS += -DCUSTOM_MEM_FRAME_HEAP_SIZE=$(CUSTOM_MEM_FRAME_HEAP_SIZE)
endif

# Custom board - Netcam
ifeq ($(CUSTOM_BOARD),netcam)
 HDVPSS_CFLAGS += -DCUSTOM_BOARD_NETCAM
endif

# Enable asserts for whole program debug as well, could comment out when not required
ifeq ($(PROFILE_$(CORE)),whole_program_debug)
 HDVPSS_CFLAGS += -DVPS_ASSERT_ENABLE
endif

export HDVPSS_LIBS
export HDVPSS_EXAMPLE_LIBS
export HDVPSS_CFLAGS

# Nothing beyond this point
