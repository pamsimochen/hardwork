
MODULE_NAME = hdvpss_platform

SRCDIR = src ti816x/src ti814x/src ti8107/src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

MODULE_SRC_BASE_PATH := $(hdvpss_PATH)/packages/ti/psp/platforms

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
CFLAGS_LOCAL_COMMON = $(HDVPSS_CFLAGS)

include $(MODULE_SRC_BASE_PATH)/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/ti816x/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/ti814x/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/ti8107/SRC_FILES.MK

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
