
APP_NAME := ti816x_hdvpss

SRCDIR = ../src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss ipc

# List all the components required by the application
COMP_LIST_m3vpss = hdvpss hdvpss_examples_utility

# Override with custom config.bld path
CONFIG_BLD_XDC_m3     := $(hdvpss_PATH)/packages/config_ti816x.bld
XDC_CFG_FILE_m3vpss = proxyServerHost_ti816x.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_m3vpss = ProxyServerHost_main.c
CFLAGS_LOCAL_m3vpss = $(HDVPSS_CFLAGS)

# Choose an application defined executable name
LOCAL_APP_NAME= $(APP_NAME)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
