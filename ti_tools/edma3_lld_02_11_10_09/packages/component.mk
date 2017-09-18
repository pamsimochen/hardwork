# File: component.mk
#       This file is component include make file of OMX.

# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_PLATFORM_DEPENDENCY - "yes": means the code for this module depends on 
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on 
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#

# List of modules under EDMA3 LLD
edma3_lld_COMP_LIST_c6xdsp = edma3_lld_rm edma3_lld_drv

# List of examples under EDMA3 LLD
edma3_lld_EXAMPLES_LIST = edma3_drv_ti816x-evm_m3_example edma3_drv_c6472-evm_64p_example edma3_drv_c6472-evm_64p_be_example edma3_drv_c6748-evm_674_example \
                          edma3_drv_da830-evm_674_example edma3_drv_omapl138-evm_674_example \
                          edma3_drv_tci6486-evm_64p_example edma3_drv_tci6486-evm_64p_be_example edma3_drv_ti816x-evm_674_example \
                          edma3_drv_ti816x_sim_sample  \
                          edma3_drv_ti814x-evm_674_example edma3_drv_ti814x-evm_a8_example \
                          edma3_drv_tci6608-sim_66_example edma3_drv_tci6608-sim_66_be_example  \
                          edma3_drv_tci6616-sim_66_example edma3_drv_tci6616-sim_66_be_example  \
                          edma3_drv_tci6614-sim_66_example edma3_drv_tci6614-sim_66_be_example  \
                          edma3_drv_c6657-sim_66_example edma3_drv_c6657-sim_66_be_example  \
                          edma3_drv_tci6638k2k-sim_66_example edma3_drv_tci6638k2k-sim_66_be_example  \
                          edma3_drv_c6670-evm_66_example edma3_drv_c6670-evm_66_be_example  \
                          edma3_drv_c6678-evm_66_example edma3_drv_c6678-evm_66_be_example  \
                          edma3_drv_tci6614-evm_66_example edma3_drv_tci6614-evm_66_be_example  \
                          edma3_drv_c6657-evm_66_example edma3_drv_c6657-evm_66_be_example  \
                          edma3_drv_tci6638k2k-evm_66_example edma3_drv_tci6638k2k-evm_66_be_example  \
                          edma3_drv_tci6636k2h-evm_66_example edma3_drv_tci6636k2h-evm_66_be_example  \
                          edma3_drv_omapl138-evm_arm9_example edma3_drv_omap4-evm_64t_example

# List of libraries



ifeq ($(PLATFORM),)
PLATFORM = tda2xx-evm ti816x-evm ti814x-evm c6a811x-evm c6472-evm c6670-evm c6678-evm c6748-evm da830-evm omapl138-evm tci6486-evm tci6608-sim tci6616-sim tci6614-evm tci6614-sim c6657-evm c6657-sim tci6638k2k-evm tci6638k2k-sim tci6636k2h-evm
endif

ifeq ($(TARGET),)
TARGET = 674 m3 a8 64p 66 m4 a15
edma3_lld_LIBS_ALL = edma3_lld_rm_generic
endif

#Prepare library list to build from the PLATFORM and TARGET
ifeq ($(PLATFORM),generic)
edma3_lld_LIBS_ALL = edma3_lld_rm_generic
else
edma3_lld_LIBS_ALL += $(foreach plat, $(PLATFORM),$(foreach targ,$(TARGET),edma3_lld_$(plat)_$(targ)_libs))
endif

#Prepare Example list from PLATFORM and TARGET
ifeq ($(ENDIAN),big)
edma3_lld_EXAMPLES_LIST = $(foreach plat, $(PLATFORM),$(foreach targ,$(TARGET),edma3_drv_$(plat)_$(targ)_be_example))
else
edma3_lld_EXAMPLES_LIST = $(foreach plat, $(PLATFORM),$(foreach targ,$(TARGET),edma3_drv_$(plat)_$(targ)_example))
endif


# EDMA3 LLD RM - Resource manager
edma3_lld_rm_RELPATH = ti/sdo/edma3/rm
edma3_lld_rm_PATH = $(edma3_lld_PATH)/packages/$(edma3_lld_rm_RELPATH)
edma3_lld_rm_INCLUDE = $(edma3_lld_rm_PATH) $(edma3_lld_rm_PATH)/src
edma3_lld_rm_PKG_LIST = edma3_lld_rm edma3_lld_rm_sample
edma3_lld_rm_PLATFORM_DEPENDENCY = yes

edma3_lld_rm_sample_RELPATH = ti/sdo/edma3/rm/sample
edma3_lld_rm_sample_PATH = $(edma3_lld_PATH)/packages/$(edma3_lld_rm_sample_RELPATH)
edma3_lld_rm_sample_INCLUDE = $(edma3_lld_rm_sample_PATH) $(edma3_lld_rm_sample_PATH)/src
edma3_lld_rm_sample_PLATFORM_DEPENDENCY = yes

# EDMA3 LLD DRV - Driver
edma3_lld_drv_RELPATH = ti/sdo/edma3/drv
edma3_lld_drv_PATH = $(edma3_lld_PATH)/packages/$(edma3_lld_drv_RELPATH)
edma3_lld_drv_INCLUDE = $(edma3_lld_drv_PATH) $(edma3_lld_drv_PATH)/src
edma3_lld_drv_PKG_LIST = edma3_lld_drv edma3_lld_drv_sample
edma3_lld_drv_PLATFORM_DEPENDENCY = no

edma3_lld_drv_sample_RELPATH = ti/sdo/edma3/drv/sample
edma3_lld_drv_sample_PATH = $(edma3_lld_PATH)/packages/$(edma3_lld_drv_sample_RELPATH)
edma3_lld_drv_sample_INCLUDE = $(edma3_lld_drv_sample_PATH) $(edma3_lld_drv_sample_PATH)/src
edma3_lld_drv_sample_PLATFORM_DEPENDENCY = yes

# EDMA3 LLD top level
edma3_lld_INCLUDE = $(edma3_lld_PATH)/packages $(edma3_lld_rm_INCLUDE) $(edma3_lld_drv_INCLUDE) \
                    $(edma3_lld_rm_sample_INCLUDE) $(edma3_lld_drv_sample_INCLUDE)

# EDMA3 LLD examples
edma3_drv_c6472-evm_64p_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6472
edma3_drv_c6472-evm_64p_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6472-evm_64p_example_EXAMPLES_RELPATH)

edma3_drv_c6472-evm_64p_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6472BE
edma3_drv_c6472-evm_64p_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6472-evm_64p_be_example_EXAMPLES_RELPATH)

edma3_drv_c6748-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6748
edma3_drv_c6748-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6748-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_da830-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evmDA830
edma3_drv_da830-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_da830-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_omapl138-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evmOMAPL138
edma3_drv_omapl138-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_omapl138-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_omap4-evm_64t_example_EXAMPLES_RELPATH = examples/edma3_driver/evmOMAP4
edma3_drv_omap4-evm_64t_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_omap4-evm_64t_example_EXAMPLES_RELPATH)

edma3_drv_omapl138-evm_arm9_example_EXAMPLES_RELPATH = examples/edma3_driver/evmOMAPL138_ARM
edma3_drv_omapl138-evm_arm9_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_omapl138-evm_arm9_example_EXAMPLES_RELPATH)

edma3_drv_tci6486-evm_64p_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6486
edma3_drv_tci6486-evm_64p_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6486-evm_64p_example_EXAMPLES_RELPATH)

edma3_drv_tci6486-evm_64p_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6486BE
edma3_drv_tci6486-evm_64p_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6486-evm_64p_be_example_EXAMPLES_RELPATH)

edma3_drv_ti814x-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI814x
edma3_drv_ti814x-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti814x-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_ti814x-evm_a8_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI814x_A8
edma3_drv_ti814x-evm_a8_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti814x-evm_a8_example_EXAMPLES_RELPATH)

edma3_drv_ti814x-evm_m3_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI814x_M3
edma3_drv_ti814x-evm_m3_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti814x-evm_m3_example_EXAMPLES_RELPATH)

edma3_drv_ti816x-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI816x
edma3_drv_ti816x-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti816x-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_ti816x-evm_a8_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI816x_A8
edma3_drv_ti816x-evm_a8_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti816x-evm_a8_example_EXAMPLES_RELPATH)

edma3_drv_ti816x-evm_m3_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTI816x_M3
edma3_drv_ti816x-evm_m3_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_ti816x-evm_m3_example_EXAMPLES_RELPATH)

edma3_drv_c6a811x-evm_674_example_EXAMPLES_RELPATH = examples/edma3_driver/evmC6A811x
edma3_drv_c6a811x-evm_674_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6a811x-evm_674_example_EXAMPLES_RELPATH)

edma3_drv_c6a811x-evm_a8_example_EXAMPLES_RELPATH = examples/edma3_driver/evmC6A811x_A8
edma3_drv_c6a811x-evm_a8_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6a811x-evm_a8_example_EXAMPLES_RELPATH)

edma3_drv_c6a811x-evm_m3_example_EXAMPLES_RELPATH = examples/edma3_driver/evmC6A811x_M3
edma3_drv_c6a811x-evm_m3_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6a811x-evm_m3_example_EXAMPLES_RELPATH)

edma3_drv_tci6608-sim_66_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6608
edma3_drv_tci6608-sim_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6608-sim_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6608-sim_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6608BE
edma3_drv_tci6608-sim_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6608-sim_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6616-sim_66_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6616
edma3_drv_tci6616-sim_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6616-sim_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6616-sim_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6616BE
edma3_drv_tci6616-sim_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6616-sim_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6614-sim_66_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6614
edma3_drv_tci6614-sim_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6614-sim_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6614-sim_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6614BE
edma3_drv_tci6614-sim_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6614-sim_66_be_example_EXAMPLES_RELPATH)

edma3_drv_c6657-sim_66_example_EXAMPLES_RELPATH = examples/edma3_driver/simC6657
edma3_drv_c6657-sim_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6657-sim_66_example_EXAMPLES_RELPATH)

edma3_drv_c6657-sim_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/simC6657BE
edma3_drv_c6657-sim_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6657-sim_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6638k2k-sim_66_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6638K2K
edma3_drv_tci6638k2k-sim_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6638k2k-sim_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6638k2k-sim_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/simTCI6638K2KBE
edma3_drv_tci6638k2k-sim_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6638k2k-sim_66_be_example_EXAMPLES_RELPATH)

edma3_drv_c6670-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6670
edma3_drv_c6670-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6670-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_c6670-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6670BE
edma3_drv_c6670-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6670-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_c6678-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6678
edma3_drv_c6678-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6678-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_c6678-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evm6678BE
edma3_drv_c6678-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6678-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6614-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6614
edma3_drv_tci6614-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6614-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6614-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6614BE
edma3_drv_tci6614-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6614-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_c6657-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evmC6657
edma3_drv_c6657-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6657-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_c6657-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evmC6657BE
edma3_drv_c6657-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_c6657-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6638k2k-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6638K2K
edma3_drv_tci6638k2k-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6638k2k-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6638k2k-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6638K2KBE
edma3_drv_tci6638k2k-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6638k2k-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tci6636k2h-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6636K2H
edma3_drv_tci6636k2h-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6636k2h-evm_66_example_EXAMPLES_RELPATH)

edma3_drv_tci6636k2h-evm_66_be_example_EXAMPLES_RELPATH = examples/edma3_driver/evmTCI6636K2HBE
edma3_drv_tci6636k2h-evm_66_be_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tci6636k2h-evm_66_be_example_EXAMPLES_RELPATH)

edma3_drv_tda2xx-evm_m4_example_EXAMPLES_RELPATH = examples/edma3_driver/evmtda2xx_M4
edma3_drv_tda2xx-evm_m4_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tda2xx-evm_m4_example_EXAMPLES_RELPATH)

edma3_drv_tda2xx-evm_a15_example_EXAMPLES_RELPATH = examples/edma3_driver/evmtda2xx_A15
edma3_drv_tda2xx-evm_a15_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tda2xx-evm_a15_example_EXAMPLES_RELPATH)

edma3_drv_tda2xx-evm_66_example_EXAMPLES_RELPATH = examples/edma3_driver/evmtda2xx
edma3_drv_tda2xx-evm_66_example_EXAMPLES_PATH = $(edma3_lld_PATH)/$(edma3_drv_tda2xx-evm_66_example_EXAMPLES_RELPATH)

# Nothing beyond this point
