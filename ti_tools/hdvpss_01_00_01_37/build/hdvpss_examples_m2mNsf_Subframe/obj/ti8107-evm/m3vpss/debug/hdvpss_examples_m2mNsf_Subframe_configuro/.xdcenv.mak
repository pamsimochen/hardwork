#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/packages;/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages;/data/datalocal_videoapps01/ipnc_tools/basic_tools/ipc_1_24_03_32/packages;/proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/packages
override XDCROOT = /data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/packages;/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages;/data/datalocal_videoapps01/ipnc_tools/basic_tools/ipc_1_24_03_32/packages;/proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/packages;/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages;..
HOSTOS = Linux
endif
