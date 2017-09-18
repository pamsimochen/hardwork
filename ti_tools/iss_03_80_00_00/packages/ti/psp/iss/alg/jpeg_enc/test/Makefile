
APP_NAME = iss_jpeg_enc_test

SRCDIR = . src
INCDIR = . ../ ../inc  $(hdvpss_PATH)/packages $(iss_PATH)/packages $(iss_algo_PATH)/ $(iss_algo_PATH)/imx/inc/ $(iss_algo_PATH)/evf/inc/  $(iss_algo_PATH)/glbce/inc/ $(iss_algo_PATH)/evf/src/inc/ $(iss_algo_PATH)/evf/src/src_hw4macs/inc_m3/
INCDIR +=  $(iss_PATH)/packages/ti/psp/iss/hal/iss/simcop/common/   $(edma3lld_PATH)/packages
# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss

# List all the components required by the application
COMP_LIST_m3vpss = iss hdvpss

# XDC CFG File
XDC_CFG_FILE_m3vpss = mjpegenc_test.cfg 

# Common source files and CFLAGS across all platforms and cores
SRCS_m3vpss = main.c   platform_supportM3bios.c  vicp_edma3_M3_cfg.c  edma3OsFunctions.c mjpegLink_enc.c
#SRCS_m3vpss += evf_test.c
#SRCS_m3vpss += boxcar.c boxcarTestParams.c  
#SRCS_m3vpss += glbce.c  
#SRCS_m3vpss += glbce_noBoxcar.c  
CFLAGS_LOCAL_m3vpss = $(ISS_CFLAGS)  -D_4MACS  -D_DUAL_CORE  -D_BIOS -D_DEBUG   -D_SIMCOP
#-D_SIMCOP -D_DM365

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
