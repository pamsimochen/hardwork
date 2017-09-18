
MODULE_NAME = hdvpss_drivers

SRCDIR = common/src core/src drivers/capture/src drivers/display/src  \
         drivers/m2m/src drivers/m2mNsf/src drivers/src hal/src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

MODULE_SRC_BASE_PATH := $(hdvpss_PATH)/packages/ti/psp/vps

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
SRCS_ti816x =
SRCS_ti814x =
CFLAGS_LOCAL_COMMON = $(HDVPSS_CFLAGS)
ifeq ($(HDVENC_OUTPUT_SWAP),1)
CFLAGS_LOCAL_COMMON += -DVPSHAL_VENC_SWAP_COLORS_FOR_VGA_OUT_ON_TI813X
endif

include $(MODULE_SRC_BASE_PATH)/common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/core/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/capture/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/display/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/m2m/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/m2mNsf/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/hal/SRC_FILES.MK

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
