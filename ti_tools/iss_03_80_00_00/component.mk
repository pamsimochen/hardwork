# File: component.mk
#       This file is component include make file of ISS.

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

iss_PKG_LIST = iss_drivers iss_platform iss_devices
#iss_alg iss_proxyServer iss_vstab   iss_i2c

#
# ISS Modules
#

# ISS Drivers
iss_drivers_RELPATH = ti/psp/iss
iss_drivers_PATH = $(iss_PATH)/packages/$(iss_drivers_RELPATH)
iss_drivers_INCLUDE = $(iss_PATH)/packages
iss_drivers_PKG_LIST = iss_drivers
iss_drivers_PLATFORM_DEPENDENCY = yes

# Platform Drivers
iss_platform_RELPATH = ti/psp/platforms
iss_platform_PATH = $(iss_PATH)/packages/$(iss_platform_RELPATH)
iss_platform_INCLUDE = $(iss_PATH)/packages
iss_platform_PKG_LIST = iss_platform
iss_platform_PLATFORM_DEPENDENCY = yes

# I2C Driver
iss_i2c_RELPATH = ti/psp/i2c
iss_i2c_PATH = $(iss_PATH)/packages/$(iss_i2c_RELPATH)
iss_i2c_INCLUDE = $(iss_PATH)/packages
iss_i2c_PKG_LIST = iss_i2c
iss_i2c_PLATFORM_DEPENDENCY = yes

# External Device Drivers
iss_devices_RELPATH = ti/psp/devices
iss_devices_PATH = $(iss_PATH)/packages/$(iss_devices_RELPATH)
iss_devices_INCLUDE = $(iss_PATH)/packages
iss_devices_PKG_LIST = iss_devices
iss_devices_PLATFORM_DEPENDENCY = yes

# Proxy Server
iss_proxyServer_RELPATH = ti/psp/proxyServer
iss_proxyServer_PATH = $(iss_PATH)/packages/$(iss_proxyServer_RELPATH)
iss_proxyServer_INCLUDE = $(iss_PATH)/packages
iss_proxyServer_PKG_LIST = iss_proxyServer
iss_proxyServer_PLATFORM_DEPENDENCY = yes

# Proxy Server
iss_alg_RELPATH = ti/psp/iss/alg
iss_alg_PATH = $(iss_PATH)/packages/$(iss_alg_RELPATH)
iss_alg_INCLUDE = $(iss_PATH)/packages
iss_alg_PKG_LIST = iss_alg
iss_alg_PLATFORM_DEPENDENCY = yes





iss_vstab_RELPATH = ti/psp/iss/alg/vstab
iss_vstab_PATH = $(iss_PATH)/packages/$(iss_vstab_RELPATH)
iss_vstab_INCLUDE = $(iss_PATH)/packages
iss_vstab_PKG_LIST = iss_vstab
iss_vstab_PLATFORM_DEPENDENCY = yes


iss_simcop_rm_RELPATH = ti/psp/iss/alg/rm
iss_simcop_rm_PATH = $(iss_PATH)/packages/$(iss_simcop_rm_RELPATH)
iss_simcop_rm_INCLUDE = $(iss_PATH)/packages
iss_simcop_rm_PKG_LIST = iss_simcop_rm
iss_simcop_rm_PLATFORM_DEPENDENCY = yes

# Links
iss_examples_links_RELPATH = ti/psp/examples/common/iss/chains/links
iss_examples_links_PATH = $(iss_PATH)/packages/$(iss_examples_links_RELPATH)
iss_examples_links_INCLUDE = $(iss_PATH)/packages
iss_examples_links_PKG_LIST = iss_examples_links
iss_examples_links_PLATFORM_DEPENDENCY = yes

# Component specific CFLAGS
CFLAGS_COMP_COMMON +=

# CFLAGS based on platform selected
ifeq ($(PLATFORM),ti814x-evm)
 ISS_CFLAGS = -DTI_814X_BUILD -DPLATFORM_EVM_SI
endif
ifeq ($(PLATFORM),ti814x-sim)
 ISS_CFLAGS = -DTI_814X_BUILD -DPLATFORM_SIM
endif

#Enable for Zebu build
#ISS_CFLAGS += -DPLATFORM_ZEBU

#Enable trace only for non-Klockwork build and for all profiles
ifeq ($(KW_BUILD),no)
 ISS_CFLAGS += -DVPS_TRACE_ENABLE
endif

# Enable standard asserts for klockwork build
ifeq ($(KW_BUILD),yes)
 ISS_CFLAGS += -DVPS_USE_STD_ASSERT -DVPS_ASSERT_ENABLE
endif

# Enable asserts for debug build only
ifeq ($(PROFILE_$(CORE)),debug)
 ISS_CFLAGS += -DVPS_ASSERT_ENABLE
endif

# Enable asserts for whole program debug as well, could comment out when not required
ifeq ($(PROFILE_$(CORE)),whole_program_debug)
 ISS_CFLAGS += -DVPS_ASSERT_ENABLE
endif

#Enable I2c For Low Power Mode since we are not initialising using HDVPSS
ifeq ($(POWER_OPT_DSS_OFF), YES)
 ISS_CFLAGS += -DPOWER_OPT_DSS_OFF
endif

ifeq ($(CAPTURE_MODE), MIPI_CSI2)
    ISS_CFLAGS += -DUSE_MIPI_MODE
endif

ifneq ($(IPNC_DEVICE), DM8127)
ISS_CFLAGS += -DIPNC_DM385
endif

ifeq ($(CAMERA_TILER_ENABLE), YES)
ISS_CFLAGS += -DENABLE_TILER
endif

ifeq ($(BTE_ENABLE), YES)
ISS_CFLAGS += -DENABLE_BTE
endif

ifeq ($(SYSTEM_SIMCOP_ENABLE), YES)
ISS_CFLAGS += -DSYSTEM_SIMCOP_ENABLE
endif

ifeq ($(SENSOR_BOARD_VENDOR), LIMG)
ISS_CFLAGS += -DLIMG_SENSOR_VENDOR
else
ISS_CFLAGS += -DAPPRO_SENSOR_VENDOR
endif

ISS_CFLAGS += -D$(IMGS_ID)
ISS_CFLAGS += -D$(PLATFORM_NAME)

export ISS_LIBS
export ISS_EXAMPLE_LIBS
export ISS_CFLAGS

# Nothing beyond this point
