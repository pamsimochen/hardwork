
MODULE_NAME = iss_examples_links

SRCDIR = camera display displayHwMs null scalar scalarSwMs system dup swMs
INCDIR = . $(hdvpss_PATH)/packages

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss iss ipc syslink

MODULE_SRC_BASE_PATH := $(iss_PATH)/packages/ti/psp/examples/common/iss/chains/links

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = cameraLink_tsk.c cameraLink_drv.c \
        scalarSwMsLink_tsk.c scalarSwMsLink_drv.c \
        displayLink_tsk.c displayLink_drv.c  displayHwMsLink_tsk.c \
        displayHwMsLink_drv.c nullLink_tsk.c system_main.c system_linkApi.c \
        system_config.c system_dctrl_common.c scLink_drv.c scLink_tsk.c \
        dupLink_tsk.c swMsLink_tsk.c swMsLink_drv.c swMsLink_layouts.c

SRCS_ti814x = system_dctrl_ti814x.c

CFLAGS_LOCAL_COMMON = $(ISS_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
