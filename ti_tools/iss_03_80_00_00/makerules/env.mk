# File: env.mk. This file contains all the paths and other ENV variables

#
# Module paths
#

# Destination root directory.
#   - specify the directory where you want to place the object, archive/library,
#     binary and other generated files in a different location than source tree
#   - or leave it blank to place then in the same tree as the source
DEST_ROOT = $(iss_PATH)/build

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
UTILS_INSTALL_DIR = $(xdc_PATH)

# Set path separator, etc based on the OS
ifeq ($(OS),Windows_NT)
  PATH_SEPARATOR = ;
  UTILSPATH = $(UTILS_INSTALL_DIR)/bin/
else
  # else, assume it is linux
  PATH_SEPARATOR = :
  UTILSPATH = /bin/
endif

# BIOS
bios_INCLUDE = $(bios_PATH)/packages

# IPC
ipc_INCLUDE = $(ipc_PATH)/packages

# SYSLINK
syslink_INCLUDE =

# XDC
xdc_INCLUDE = $(xdc_PATH)/packages

# ISS drivers
iss_INCLUDE = $(iss_PATH)/packages
include $(iss_PATH)/component.mk
export iss_PATH

#
# Tools paths
#

# Cortex-A8
CODEGEN_PATH_A8 =

# DSP
CODEGEN_PATH_DSP =


# Commands commonly used within the make files
RM = $(UTILSPATH)rm
RMDIR = $(UTILSPATH)rm -rf
MKDIR = $(UTILSPATH)mkdir
ECHO = @$(UTILSPATH)echo
# MAKE = $(UTILSPATH)make
EGREP = $(UTILSPATH)egrep
CP = $(UTILSPATH)cp
ifeq ($(OS),Windows_NT)
  CHMOD = $(UTILSPATH)echo
else
  CHMOD = $(UTILSPATH)chmod
endif

#
# XDC specific ENV variables
#
# XDC Config.bld file (required for configuro); Derives from top-level iss_PATH
ifeq ($(PLATFORM),ti816x-evm)
  CONFIG_BLD_XDC_m3     = $(iss_PATH)/packages/config_ti816x.bld
endif
ifeq ($(PLATFORM),ti816x-sim)
  CONFIG_BLD_XDC_m3     = $(iss_PATH)/packages/config_ti816x.bld
endif
ifeq ($(PLATFORM),ti814x-evm)
  CONFIG_BLD_XDC_m3     = $(iss_PATH)/packages/config_ti814x.bld
endif
ifeq ($(PLATFORM),ti814x-sim)
  CONFIG_BLD_XDC_m3     = $(iss_PATH)/packages/config_ti814x.bld
endif

XDCROOT = $(xdc_PATH)
XDCTOOLS = $(xdc_PATH)
export XDCROOT
export XDCTOOLS

CGTOOLS = $(CODEGEN_PATH_M3)
export CGTOOLS

CODESOURCERYCGTOOLS = $(CODEGEN_PATH_A8)
export CODESOURCERYCGTOOLS

PATH += $(PATH_SEPARATOR)$(xdc_PATH)$(PATH_SEPARATOR)$(CODEGEN_PATH_M3)/bin
export PATH

# Nothing beyond this point
