
APP_NAME = hdvpss_examples_chains

SRCDIR = src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss ipc syslink

# List all the components required by the application
COMP_LIST_m3vpss = hdvpss hdvpss_examples_utility hdvpss_examples_links

# XDC CFG File
XDC_CFG_FILE_m3vpss = vpsChains.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = chains_main.c chains_multiChCaptureNsf.c chains_multiChCaptureNsfDei.c \
        chains_tvp5158NonMuxCapture.c \
        chains_singleChCaptureNsfDeiTvp7002.c chains_singleChCaptureSii9135.c \
        chains_singleChCaptureSii9135Tvp7002.c chains_singleChCaptureTvp7002DisSync.c \
        chains_singleChCaptureNsfDeiTvp5158.c chains_singleChCaptureNsfDeiSii9135_1080i.c chains_singleChCaptureNsfDeiSii9135_480i.c \
        chains_singleChCaptureNsfDeiSii9135_480i_fullscreen.c chains_singleChCaptureTvp7002.c \
        chains_swMs.c chains_singleChCaptureTvp7002FieldMerged.c

SRCS_ti816x = chains_multiChSystemUseCaseSwMs_TriDisplay.c chains_multiChSystemUseCaseHwMs_SingleDisplay.c \
    chains_multiChSystemUseCaseHwMs_TriDisplay.c chains_multiChSystemUseCaseSwMs2_TriDisplay.c

SRCS_ti814x =

SRCS_ti8107 =

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
