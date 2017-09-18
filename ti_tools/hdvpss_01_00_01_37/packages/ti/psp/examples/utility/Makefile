
MODULE_NAME = hdvpss_examples_utility

SRCDIR = src
INCDIR = .

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

MODULE_SRC_BASE_PATH := $(hdvpss_PATH)/packages/ti/psp/examples/utility

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = vpsutils_tiler.c vpsutils_mem.c vpsutils_mbx.c vpsutils_tsk.c \
                vpsutils_buf.c vpsutils_prf.c vpsutils_grpx.c  vpsutils.c   \
                vpsutils_mem_default.c vpsutils_mem_largeHeap.c vpsutils_app.c

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
