
MODULE_NAME = iss_drivers

SRCDIR = . common/src

INCDIR = . common/src $(CSL_CRED_DIR) $(omx_PATH) $(OMX_INTERFACES_PATH) $(ISS_PATH) $(CSL_CRAM_DIR) $(iss_PATH) \
         $(xdais_PATH)/packages $(timmosal_PATH) $(timmosal_PATH)/../.. common/driver_utils drv_ccp2tx/ drv_ccp2tx/inc \
         drv_sdma drv_sdma/inc $(hdvpss_PATH)/packages $(xdc_PATH)/packages \
         $(iss_PATH)/packages/ti/psp/iss/alg/aewb/ti2a/ae/inc $(iss_PATH)/packages/ti/psp/iss/alg/aewb/ti2a/awb/inc \
         $(iss_PATH)/packages/ti/psp/iss/alg/aewb/ti2a/fd/inc $(iss_PATH)/packages/ti/psp/iss/alg/fd/inc   \
         $(iss_PATH)/packages/ti/psp/iss/alg/imx/inc \
	 $(mctnf_PATH)/Inc
         
            

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss iss xdais

MODULE_SRC_BASE_PATH := $(iss_PATH)/packages/ti/psp/iss

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = 
SRCS_ti816x = 
SRCS_ti814x =
CFLAGS_LOCAL_COMMON = $(ISS_CFLAGS)

include $(MODULE_SRC_BASE_PATH)/core/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/common/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/drivers/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/hal/SRC_FILES.MK

# ISS ALGORITHMS
include $(MODULE_SRC_BASE_PATH)/alg/rm/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/fd/SRC_FILES.MK
include $(MODULE_SRC_BASE_PATH)/alg/aewb/ti2a/ae/SRC_FILES.MK


# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
