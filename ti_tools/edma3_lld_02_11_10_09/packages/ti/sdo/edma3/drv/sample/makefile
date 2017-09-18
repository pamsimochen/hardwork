# Makefile for EDMA3 LLD DRV Sample

MODULE_NAME = edma3_lld_drv_sample

SRCDIR = src src/platforms
INCDIR = . ../../../../.. src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc

# Common source files and CFLAGS across all platforms and cores
ifeq ($(CORE),c6xdsp)
SRCS_COMMON = sample_cs.c sample_init.c
else
SRCS_COMMON = sample_arm_cs.c sample_arm_init.c
endif
ifeq ($(CORE),c6xdsp)
CFLAGS_LOCAL_COMMON = -mi10
endif

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
ifeq ($(CORE),c6xdsp)
SRCS_c6472-evm = sample_c6472_cfg.c sample_c6472_int_reg.c
SRCS_tci6486-evm = sample_tci6486_cfg.c sample_tci6486_int_reg.c
SRCS_tci6608-sim = sample_tci6608_cfg.c sample_tci6608_int_reg.c
SRCS_tci6616-sim = sample_tci6616_cfg.c sample_tci6616_int_reg.c
SRCS_tci6614-sim = sample_tci6614_cfg.c sample_tci6614_int_reg.c
SRCS_c6657-sim   = sample_c6657_cfg.c sample_c6657_int_reg.c
SRCS_tci6638k2k-sim = sample_tci6638k2k_cfg.c sample_tci6638k2k_int_reg.c
SRCS_c6670-evm   = sample_c6670_cfg.c sample_c6670_int_reg.c
SRCS_c6678-evm   = sample_c6678_cfg.c sample_c6678_int_reg.c
SRCS_tci6614-evm = sample_tci6614_cfg.c sample_tci6614_int_reg.c
SRCS_c6657-evm   = sample_c6657_cfg.c sample_c6657_int_reg.c
SRCS_tci6638k2k-evm = sample_tci6638k2k_cfg.c sample_tci6638k2k_int_reg.c
SRCS_tci6636k2h-evm = sample_tci6636k2h_cfg.c sample_tci6636k2h_int_reg.c
SRCS_omapl138-evm = sample_omapl138_cfg.c sample_omapl138_int_reg.c
SRCS_ti814x-evm = sample_ti814x_cfg.c sample_ti814x_int_reg.c
SRCS_omap4-evm   = sample_omap4_cfg.c sample_omap4_int_reg.c
CFLAGS_LOCAL_ti816x-evm = -DBUILD_NETRA_DSP
CFLAGS_LOCAL_ti814x-evm = -DBUILD_CENTAURUS_DSP
SRCS_ti816x-evm = sample_ti816x_cfg.c sample_ti816x_int_reg.c
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_DSP
SRCS_c6a811x-evm = sample_c6a811x_cfg.c sample_c6a811x_int_reg.c
SRCS_tda2xx-evm = sample_tda2xx_cfg.c sample_tda2xx_int_reg.c
CFLAGS_LOCAL_tda2xx-evm = -DBUILD_TDA2XX_DSP
else
SRCS_omapl138-evm = sample_omapl138_arm_cfg.c sample_omapl138_arm_int_reg.c
SRCS_tda2xx-evm = sample_tda2xx_cfg.c sample_tda2xx_arm_int_reg.c
endif

ifeq ($(CORE),a8host)
CFLAGS_LOCAL_ti816x-evm = -DBUILD_NETRA_A8
CFLAGS_LOCAL_ti814x-evm = -DBUILD_CENTAURUS_A8
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_A8
SRCS_ti814x-evm = sample_ti814x_cfg.c sample_ti814x_arm_int_reg.c
SRCS_ti816x-evm = sample_ti816x_cfg.c sample_ti816x_arm_int_reg.c
SRCS_c6a811x-evm = sample_c6a811x_cfg.c sample_c6a811x_arm_int_reg.c
endif
ifeq ($(CORE),m3video)
CFLAGS_LOCAL_ti816x-evm = -DBUILD_NETRA_M3VIDEO
CFLAGS_LOCAL_ti814x-evm = -DBUILD_CENTAURUS_M3VIDEO
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_M3VIDEO
SRCS_ti816x-evm = sample_ti816x_cfg.c sample_ti816x_arm_int_reg.c
SRCS_ti814x-evm = sample_ti814x_cfg.c sample_ti814x_arm_int_reg.c
SRCS_c6a811x-evm = sample_c6a811x_cfg.c sample_c6a811x_arm_int_reg.c
endif
ifeq ($(CORE),m3vpss)
CFLAGS_LOCAL_ti816x-evm = -DBUILD_NETRA_M3VPSS
CFLAGS_LOCAL_ti814x-evm = -DBUILD_CENTAURUS_M3VPSS
CFLAGS_LOCAL_c6a811x-evm = -DBUILD_C6A811X_M3VPSS
SRCS_ti816x-evm = sample_ti816x_cfg.c sample_ti816x_arm_int_reg.c
SRCS_ti814x-evm = sample_ti814x_cfg.c sample_ti814x_arm_int_reg.c
SRCS_c6a811x-evm = sample_c6a811x_cfg.c sample_c6a811x_arm_int_reg.c
endif
ifeq ($(CORE),m4)
CFLAGS_LOCAL_tda2xx-evm = -DBUILD_TDA2XX_IPU
endif
ifeq ($(CORE),a15host)
CFLAGS_LOCAL_tda2xx-evm = -DBUILD_TDA2XX_MPU
endif
SRCS_c6748-evm = sample_c6748_cfg.c sample_c6748_int_reg.c
SRCS_da830-evm = sample_da830_cfg.c sample_da830_int_reg.c

CFLAGS_LOCAL_ti814x-evm += -DCHIP_TI814X
CFLAGS_LOCAL_c6a811x-evm += -DCHIP_C6A811X

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
