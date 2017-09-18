
ifeq ($(hdvpss_PATH), )

#Default build environment, windows or linux
ifeq ($(OS), )
  OS := linux
endif

hdvpss_RELPATH = hdvpss_01_00_01_37_patched

ifeq ($(OS),Windows_NT)
  CODEGEN_PATH_M3 := C:/PROGRA~1/TEXASI~1/TMS470~1.2
  hdvpss_PATH := C:/PROGRA~1/TEXASI~1/$(hdvpss_RELPATH)
  bios_PATH := C:/PROGRA~1/TEXASI~1/bios_6_33_02_27_eng
  xdc_PATH := C:/PROGRA~1/TEXASI~1/xdctools_3_23_01_37_eng
  ipc_PATH := C:/PROGRA~1/TEXASI~1/ipc_1_24_01_24
else
  CODEGEN_PATH_M3 := /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5/
  hdvpss_PATH := /proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/$(hdvpss_RELPATH)
  bios_PATH :=  /data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/
  xdc_PATH := /data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/
  ipc_PATH := /data/datalocal_videoapps01/ipnc_tools/basic_tools/ipc_1_24_03_32/
endif

ROOTDIR := $(hdvpss_PATH)

ifeq ($(CORE), )
  CORE := m3vpss
endif

# Default platform
ifeq ($(PLATFORM), )
  PLATFORM := ti8107-evm
  #PLATFORM := ti814x-evm
endif

# Default profile
ifeq ($(PROFILE_$(CORE)), )
  PROFILE_$(CORE) := debug
endif

XDCPATH = $(bios_PATH)/packages;$(xdc_PATH)/packages;$(ipc_PATH)/packages;$(hdvpss_PATH)/packages;

# Default klockwork build flag
ifeq ($(KW_BUILD), )
  KW_BUILD := no
endif
ifeq ($(VERSION_TS), )
  VERSION_TS := no
endif

export OS
export PLATFORM
export CORE
export PROFILE_$(CORE)
export CODEGEN_PATH_M3
export bios_PATH
export xdc_PATH
export hdvpss_PATH
export ipc_PATH
export ROOTDIR
export XDCPATH
export KW_BUILD
export VERSION_TS

endif

include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(ROOTDIR)/makerules/platform.mk
include $(hdvpss_PATH)/component.mk
