
MODULE_NAME = hdvpss_examples_links

SRCDIR = capture dei display displayHwMs grpx nsf null scalar scalarSwMs system videoSrc dup swMs grpx_link
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss ipc syslink

MODULE_SRC_BASE_PATH := $(hdvpss_PATH)/packages/ti/psp/examples/common/vps/chains/links

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = captureLink_tsk.c captureLink_drv.c nsfLink_tsk.c \
        nsfLink_drv.c scalarSwMsLink_tsk.c scalarSwMsLink_drv.c \
        displayLink_tsk.c displayLink_drv.c displayHwMsLink_tsk.c \
        displayHwMsLink_drv.c nullLink_tsk.c system_main.c system_linkApi.c \
        system_config.c system_dctrl_common.c grpx_drv.c scLink_drv.c scLink_tsk.c \
        deiLink_drv.c deiLink_tsk.c videoSrcLink_drv.c dupLink_tsk.c \
        swMsLink_tsk.c swMsLink_drv.c swMsLink_layouts.c grpxLink_drv.c \
        grpxLink_tsk.c

SRCS_ti816x = system_dctrl_ti816x.c
SRCS_ti814x = system_dctrl_ti814x.c
SRCS_ti8107 = system_dctrl_ti8107.c

CFLAGS_LOCAL_COMMON = $(HDVPSS_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
