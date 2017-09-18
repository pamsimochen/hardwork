# Makefile for EDMA3 LLD DRV

MODULE_NAME = edma3_lld_drv

SRCDIR = src
INCDIR = . ../../../.. src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = 

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = edma3_drv_init.c edma3_drv_basic.c edma3_drv_adv.c
ifeq ($(CORE),c6xdsp)
CFLAGS_LOCAL_COMMON = -mi10
endif

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
