
APP_NAME = hdvpss_examples_m2mDeiScale

SRCDIR = src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss ipc syslink

# List all the components required by the application
COMP_LIST_m3vpss = hdvpss hdvpss_examples_utility

# XDC CFG File
XDC_CFG_FILE_m3vpss = m2mDeiScale.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_m3vpss = M2mDeiScale_main.c M2mDeiScale_test.c M2mDeiScale_utils.c
CFLAGS_LOCAL_m3vpss = $(HDVPSS_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
