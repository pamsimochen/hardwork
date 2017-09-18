# File: env.mk. This file contains all the paths and other ENV variables

#
# Module paths
#

# Directory where all internal software packages are located; typically 
#  those that are checked into version controlled repository. In this case all
#  the OMX components and SDK/OMX demo.
INTERNAL_SW_ROOT = D:/PSP/EDMA3_BIOS6/edma_drv/edma3_lld_02_11_10_09

# Directory where all external (imported) software packages are located; typically 
#  those that are NOT checked into version controlled repository. In this case,
#  compiler tool chains, BIOS, XDC, Syslink, IPC, FC, CE, drivers, codecs, etc.
EXTERNAL_SW_ROOT = C:/PROGRA~1/TEXASI~1

# Destination root directory.
#   - specify the directory where you want to place the object, archive/library,
#     binary and other generated files in a different location than source tree
#   - or leave it blank to place then in the same tree as the source
#DEST_ROOT = E:/Temp/edma3_lld_02_11_02_03

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
UTILS_INSTALL_DIR = $(EXTERNAL_SW_ROOT)/xdctools_3_25_02_70

# Set path separator, etc based on the OS
ifeq ($(OS),Windows_NT)
  PATH_SEPARATOR = ;
  UTILSPATH = $(UTILS_INSTALL_DIR)/bin/
else 
  # else, assume it is linux
  PATH_SEPARATOR = :
endif

# BIOS
bios_PATH = $(EXTERNAL_SW_ROOT)/bios_6_35_03_47
bios_INCLUDE = $(bios_PATH)/packages

# XDC
xdc_PATH = $(EXTERNAL_SW_ROOT)/xdctools_3_25_02_70
xdc_INCLUDE = $(xdc_PATH)/packages

# EDMA3 LLD
edma3_lld_PATH = $(INTERNAL_SW_ROOT)
include $(edma3_lld_PATH)/packages/component.mk

#
# Tools paths
#
# Cortex-M3
CODEGEN_PATH_M3 = $(EXTERNAL_SW_ROOT)/TIARMC~1.4

# Cortex-M4
CODEGEN_PATH_M4 = $(EXTERNAL_SW_ROOT)/TIARMC~1.4

# Cortex-A8
CODEGEN_PATH_A8 = $(EXTERNAL_SW_ROOT)/TIARMC~1.4
# Cortex-A8 GCC
CODEGEN_PATH_A8_GCC = C:/PROGRA~1/GNUTOO~1/4F700~1.720

# Cortex-A15
CODEGEN_PATH_A15 = C:/PROGRA~1/GNUTOO~1/4F700~1.720

# ARM-9
CODEGEN_PATH_ARM9 = $(EXTERNAL_SW_ROOT)/TIARMC~1.4

# DSP - Since same toolchain does not support COFF and ELF, there are two entries
#        This would go away when one version supports both formats
CODEGEN_PATH_DSP = $(EXTERNAL_SW_ROOT)/C6000C~1.2
CODEGEN_PATH_DSPELF = $(EXTERNAL_SW_ROOT)/C6000C~1.2


# Commands commonly used within the make files

RM = $(UTILSPATH)rm
RMDIR = $(UTILSPATH)rm -rf
MKDIR = $(UTILSPATH)mkdir
ECHO = @$(UTILSPATH)echo
# MAKE = $(UTILSPATH)make
EGREP = $(UTILSPATH)egrep
CP = $(UTILSPATH)cp

# This is to avoid using full blown cygwin - chmod is really needed only linux
ifeq ($(OS),Windows_NT)
CHMOD = @echo
else
CHMOD = $(UTILSPATH)chmod
endif

#
# XDC specific ENV variables
#

# XDC Config.bld file (required for configuro) ; Derives from top-level omx_PATH
CONFIG_BLD_XDC_674 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_64p = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_64t = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_a8 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_arm9 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_66 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_m3 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_m4 = $(edma3_lld_PATH)/packages/_config.bld
CONFIG_BLD_XDC_a15 = $(edma3_lld_PATH)/packages/_config.bld

XDCPATH = $(bios_PATH)/packages;$(xdc_PATH)/packages;$(edma3_lld_PATH)/packages;
export XDCPATH

XDCROOT = $(xdc_PATH)
XDCTOOLS = $(xdc_PATH)
export XDCROOT
export XDCTOOLS

TMS470_CGTOOLS = $(CODEGEN_PATH_M3)
CGTOOLS = $(CODEGEN_PATH_DSP)
CGTOOLS_ELF = $(CODEGEN_PATH_DSPELF)
export TMS470_CGTOOLS

CODESOURCERYCGTOOLS = $(CODEGEN_PATH_A8)
export CODESOURCERYCGTOOLS

PATH += $(PATH_SEPARATOR)$(xdc_PATH)$(PATH_SEPARATOR)$(CODEGEN_PATH_M3)/bin
export PATH

# Nothing beyond this point
