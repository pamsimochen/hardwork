
MODULE_NAME = hdvpss_i2c

SRCDIR = src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = psp_i2cdrv.c
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
