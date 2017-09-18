
APP_NAME = iss_examples_i2cRdWrUtil

SRCDIR = src
INCDIR = . $(hdvpss_PATH)/packages $(iss_PATH)/packages

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

# List all the components required by the application
COMP_LIST_m3vpss = iss hdvpss

# XDC CFG File
XDC_CFG_FILE_m3vpss = pspI2c.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_m3vpss = psp_i2cRdWrUtil.c
CFLAGS_LOCAL_m3vpss = $(ISS_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
