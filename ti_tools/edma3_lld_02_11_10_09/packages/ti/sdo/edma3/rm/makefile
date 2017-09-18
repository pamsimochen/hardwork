# Makefile for EDMA3 LLD RM

MODULE_NAME = edma3_lld_rm

SRCDIR = src src/configs
INCDIR = . ../../../.. src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = 

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = edma3resmgr.c edma3_rm_gbl_data.c
ifeq ($(CORE),c6xdsp)
CFLAGS_LOCAL_COMMON = -mi10
endif

# Defines for picking proper configuration based on the cores
ifeq ($(CORE),c6xdsp)
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_DSP
endif
ifeq ($(CORE),a8host)
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_A8
endif
ifeq ($(CORE),m3video)
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_M3VIDEO
endif
ifeq ($(CORE),m3vpss)
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_M3VPSS
endif
ifeq ($(CORE),a15host)
CFLAGS_LOCAL_tda2xx-evm = -DBUILD_TDA2XX_MPU
endif

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
SRCS_c6472-evm = edma3_c6472_cfg.c
SRCS_tci6486-evm = edma3_tci6486_cfg.c
SRCS_tci6608-sim = edma3_tci6608_cfg.c
SRCS_tci6616-sim = edma3_tci6616_cfg.c
SRCS_tci6614-sim = edma3_tci6614_cfg.c
SRCS_c6657-sim = edma3_c6657_cfg.c
SRCS_tci6638k2k-sim = edma3_tci6638k2k_cfg.c
SRCS_c6670-evm = edma3_c6670_cfg.c
SRCS_c6678-evm = edma3_c6678_cfg.c
SRCS_tci6614-evm = edma3_tci6614_cfg.c
SRCS_c6657-evm = edma3_c6657_cfg.c
SRCS_tci6638k2k-evm = edma3_tci6638k2k_cfg.c
SRCS_tci6636k2h-evm = edma3_tci6636k2h_cfg.c
SRCS_omapl138-evm = edma3_omapl138_cfg.c
SRCS_c6748-evm = edma3_c6748_cfg.c
SRCS_da830-evm = edma3_da830_cfg.c
SRCS_omap4-evm = edma3_omap4_cfg.c
SRCS_ti814x-evm = edma3_ti814x_cfg.c
SRCS_tda2xx-evm = edma3_tda2xx_cfg.c
SRCS_ti816x-evm = edma3_ti816x_cfg.c
SRCS_c6a811x-evm = edma3_c6a811x_cfg.c
SRCS_ti816x-sim = edma3_ti816x_cfg.c
CFLAGS_LOCAL_generic = -DGENERIC

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
