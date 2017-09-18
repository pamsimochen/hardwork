#
# EDMA3 LLD
#

ifeq ($(ROOTDIR),)
  $(error Error: Environment variable ROOTDIR not set)
endif

# Begin block - Build options
#  By the end of this block, we'd have either xdc or make or both in BUILD_OPTIONS
#  using which, we'd build either XDC packages (+run configuro)
#  or make based components (+link) or both.
BUILD_OPTIONS = xdc make
VALID_BUILD_OPTIONS = full $(BUILD_OPTIONS)
ifeq ($(filter $(BUILD),$(VALID_BUILD_OPTIONS)),)
 ifeq ($(BUILD),)
  BUILD = full
 else
  $(error Error: Invalid value specified for BUILD. Valid values are - $(VALID_BUILD_OPTIONS))
 endif
endif
ifneq ($(BUILD),full)
 BUILD_OPTIONS = $(BUILD)
endif
# End block - Build options

include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include component.mk

# Listing common targets
edma3_lld_COMP_LIST_ALL = $(edma3_lld_COMP_LIST_c6xdsp)
edma3_lld_PKG_LIST_ALL = $(foreach PKG,$(edma3_lld_COMP_LIST_ALL),$($(PKG)_PKG_LIST))
CLEANALL_TARGETS = $(addsuffix _clean, $(edma3_lld_LIBS_ALL))
CLEANALL_EXAMPLES = $(addsuffix _clean, $(edma3_lld_EXAMPLES_LIST))

.PHONY : $(edma3_lld_LIBS_ALL) $(edma3_lld_EXAMPLES_LIST) $(CLEANALL_TARGETS) $(CLEANALL_EXAMPLES)

all: libs examples

libs: $(edma3_lld_LIBS_ALL)

examples: $(edma3_lld_EXAMPLES_LIST)

clean: cleanlibs cleanexamples

cleanlibs: $(CLEANALL_TARGETS)

cleanexamples: $(CLEANALL_EXAMPLES)



#=======================================================================================================================================
#
# Rules to build and clean libs for all PLATFORM and TARGET
#
#=======================================================================================================================================


#=======================================================================================================================================
#To Build libs For Platform tda2xx-evm Target 66
edma3_lld_tda2xx-evm_66_libs: edma3_lld_tda2xx-evm_66_libs_drv edma3_lld_tda2xx-evm_66_libs_rm edma3_lld_tda2xx-evm_66_libs_drvsample edma3_lld_tda2xx-evm_66_libs_rmsample
edma3_lld_tda2xx-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making 66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making 66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif
edma3_lld_tda2xx-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif
edma3_lld_tda2xx-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif
edma3_lld_tda2xx-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif

#To Clean libs For Platform tda2xx-evm Target 66
edma3_lld_tda2xx-evm_66_libs_clean: edma3_lld_tda2xx-evm_66_libs_drv_clean edma3_lld_tda2xx-evm_66_libs_rm_clean edma3_lld_tda2xx-evm_66_libs_drvsample_clean edma3_lld_tda2xx-evm_66_libs_rmsample_clean
edma3_lld_tda2xx-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning 66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning 66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_tda2xx-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_tda2xx-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_tda2xx-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release

#=======================================================================================================================================
#To Build libs For Platform tda2xx-evm Target m4
edma3_lld_tda2xx-evm_m4_libs: edma3_lld_tda2xx-evm_m4_libs_drv edma3_lld_tda2xx-evm_m4_libs_rm edma3_lld_tda2xx-evm_m4_libs_drvsample edma3_lld_tda2xx-evm_m4_libs_rmsample
edma3_lld_tda2xx-evm_m4_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making m4:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Making m4:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
endif
edma3_lld_tda2xx-evm_m4_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
endif
edma3_lld_tda2xx-evm_m4_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
endif
edma3_lld_tda2xx-evm_m4_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
endif

#To Clean libs For Platform tda2xx-evm Target m4
edma3_lld_tda2xx-evm_m4_libs_clean: edma3_lld_tda2xx-evm_m4_libs_drv_clean edma3_lld_tda2xx-evm_m4_libs_rm_clean edma3_lld_tda2xx-evm_m4_libs_drvsample_clean edma3_lld_tda2xx-evm_m4_libs_rmsample_clean
edma3_lld_tda2xx-evm_m4_libs_drv_clean:
	$(ECHO) \# Cleaning m4:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Cleaning m4:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
edma3_lld_tda2xx-evm_m4_libs_rm_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
edma3_lld_tda2xx-evm_m4_libs_drvsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
edma3_lld_tda2xx-evm_m4_libs_rmsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release

#=======================================================================================================================================
#To Build libs For Platform tda2xx-evm Target a15
edma3_lld_tda2xx-evm_a15_libs: edma3_lld_tda2xx-evm_a15_libs_drv edma3_lld_tda2xx-evm_a15_libs_rm edma3_lld_tda2xx-evm_a15_libs_drvsample edma3_lld_tda2xx-evm_a15_libs_rmsample
edma3_lld_tda2xx-evm_a15_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making a15:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Making a15:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
endif
edma3_lld_tda2xx-evm_a15_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
endif
edma3_lld_tda2xx-evm_a15_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
endif
edma3_lld_tda2xx-evm_a15_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Making tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
endif

#To Clean libs For Platform tda2xx-evm Target a15
edma3_lld_tda2xx-evm_a15_libs_clean: edma3_lld_tda2xx-evm_a15_libs_drv_clean edma3_lld_tda2xx-evm_a15_libs_rm_clean edma3_lld_tda2xx-evm_a15_libs_drvsample_clean edma3_lld_tda2xx-evm_a15_libs_rmsample_clean
edma3_lld_tda2xx-evm_a15_libs_drv_clean:
	$(ECHO) \# Cleaning a15:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Cleaning a15:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
edma3_lld_tda2xx-evm_a15_libs_rm_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
edma3_lld_tda2xx-evm_a15_libs_drvsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
edma3_lld_tda2xx-evm_a15_libs_rmsample_clean:
	$(ECHO) \# Cleaning tda2xx-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Cleaning tda2xx-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release

#=======================================================================================================================================
#To Build libs For Platform ti816x-evm Target 674
edma3_lld_ti816x-evm_674_libs: edma3_lld_ti816x-evm_674_libs_drv edma3_lld_ti816x-evm_674_libs_rm edma3_lld_ti816x-evm_674_libs_drvsample edma3_lld_ti816x-evm_674_libs_rmsample
edma3_lld_ti816x-evm_674_libs_drv:
	$(ECHO) \# Making c674:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c674:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_rm:
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti816x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_drvsample:
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti816x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_rmsample:
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti816x-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#To Clean libs For Platform ti816x-evm Target 674
edma3_lld_ti816x-evm_674_libs_clean: edma3_lld_ti816x-evm_674_libs_drv_clean edma3_lld_ti816x-evm_674_libs_rm_clean edma3_lld_ti816x-evm_674_libs_drvsample_clean edma3_lld_ti816x-evm_674_libs_rmsample_clean
edma3_lld_ti816x-evm_674_libs_drv_clean:
	$(ECHO) \# Cleaning c674:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c674:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti816x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti816x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_ti816x-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti816x-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#=======================================================================================================================================
#To Build libs For Platform ti814x-evm Target 674
edma3_lld_ti814x-evm_674_libs: edma3_lld_ti814x-evm_674_libs_drv edma3_lld_ti814x-evm_674_libs_rm edma3_lld_ti814x-evm_674_libs_drvsample edma3_lld_ti814x-evm_674_libs_rmsample
edma3_lld_ti814x-evm_674_libs_drv:
	$(ECHO) \# Making c674:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c674:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_rm:
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti814x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_drvsample:
	$(ECHO) \# Making ti814x-evm :debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti814x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_rmsample:
	$(ECHO) \# Making ti814x-evm :debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making ti814x-evm :release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#To Clean libs For Platform ti814x-evm Target 674
edma3_lld_ti814x-evm_674_libs_clean: edma3_lld_ti814x-evm_674_libs_drv_clean edma3_lld_ti814x-evm_674_libs_rm_clean edma3_lld_ti814x-evm_674_libs_drvsample_clean edma3_lld_ti814x-evm_674_libs_rmsample_clean
edma3_lld_ti814x-evm_674_libs_drv_clean:
	$(ECHO) \# Cleaning c674:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c674:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti814x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti814x-evm :debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti814x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_ti814x-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti814x-evm :debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning ti814x-evm :release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#=======================================================================================================================================
#To Build libs For Platform ti816x-evm Target a8
edma3_lld_ti816x-evm_a8_libs: edma3_lld_ti816x-evm_a8_libs_drv edma3_lld_ti816x-evm_a8_libs_rm edma3_lld_ti816x-evm_a8_libs_drvsample edma3_lld_ti816x-evm_a8_libs_rmsample
edma3_lld_ti816x-evm_a8_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making a8:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making a8:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti816x-evm_a8_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti816x-evm_a8_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti816x-evm_a8_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif

#To Clean libs For Platform ti816x-evm Target a8
edma3_lld_ti816x-evm_a8_libs_clean: edma3_lld_ti816x-evm_a8_libs_drv_clean edma3_lld_ti816x-evm_a8_libs_rm_clean edma3_lld_ti816x-evm_a8_libs_drvsample_clean edma3_lld_ti816x-evm_a8_libs_rmsample_clean
edma3_lld_ti816x-evm_a8_libs_drv_clean:
	$(ECHO) \# Cleaning a8:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning a8:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti816x-evm_a8_libs_rm_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti816x-evm_a8_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti816x-evm_a8_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release

#=======================================================================================================================================
#To Build libs For Platform ti816x-evm Target m3
edma3_lld_ti816x-evm_m3_libs: edma3_lld_ti816x-evm_m3_libs_drv edma3_lld_ti816x-evm_m3_libs_rm edma3_lld_ti816x-evm_m3_libs_drvsample edma3_lld_ti816x-evm_m3_libs_rmsample
edma3_lld_ti816x-evm_m3_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making m3:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making m3:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making m3:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making m3:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
endif
edma3_lld_ti816x-evm_m3_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
endif
edma3_lld_ti816x-evm_m3_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
endif
edma3_lld_ti816x-evm_m3_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
endif

#To Clean libs For Platform ti816x-evm Target m3
edma3_lld_ti816x-evm_m3_libs_clean: edma3_lld_ti816x-evm_m3_libs_drv_clean edma3_lld_ti816x-evm_m3_libs_rm_clean edma3_lld_ti816x-evm_m3_libs_drvsample_clean edma3_lld_ti816x-evm_m3_libs_rmsample_clean
edma3_lld_ti816x-evm_m3_libs_drv_clean:
	$(ECHO) \# Cleaning m3:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning m3:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning m3:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning m3:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
edma3_lld_ti816x-evm_m3_libs_rm_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
edma3_lld_ti816x-evm_m3_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
edma3_lld_ti816x-evm_m3_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Cleaning ti816x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti816x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release

#=======================================================================================================================================
#To Build libs For Platform ti814x-evm Target a8
edma3_lld_ti814x-evm_a8_libs: edma3_lld_ti814x-evm_a8_libs_drv edma3_lld_ti814x-evm_a8_libs_rm edma3_lld_ti814x-evm_a8_libs_drvsample edma3_lld_ti814x-evm_a8_libs_rmsample
edma3_lld_ti814x-evm_a8_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making a8:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making a8:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti814x-evm_a8_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti814x-evm_a8_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_ti814x-evm_a8_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif

#To Clean libs For Platform ti814x-evm Target a8
edma3_lld_ti814x-evm_a8_libs_clean: edma3_lld_ti814x-evm_a8_libs_drv_clean edma3_lld_ti814x-evm_a8_libs_rm_clean edma3_lld_ti814x-evm_a8_libs_drvsample_clean edma3_lld_ti814x-evm_a8_libs_rmsample_clean
edma3_lld_ti814x-evm_a8_libs_drv_clean:
	$(ECHO) \# Cleaning a8:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning a8:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti814x-evm_a8_libs_rm_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti814x-evm_a8_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_ti814x-evm_a8_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release

#=======================================================================================================================================
#To Build libs For Platform ti814x-evm Target m3
edma3_lld_ti814x-evm_m3_libs: edma3_lld_ti814x-evm_m3_libs_rm edma3_lld_ti814x-evm_m3_libs_drvsample edma3_lld_ti814x-evm_m3_libs_rmsample
edma3_lld_ti814x-evm_m3_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making m3:debug:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making m3:release:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making m3:debug:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making m3:release:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
endif
edma3_lld_ti814x-evm_m3_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
endif
edma3_lld_ti814x-evm_m3_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_drv_sample 	
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
endif
edma3_lld_ti814x-evm_m3_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release	
	$(ECHO) \# Making ti814x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making ti814x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
endif

#To Clean libs For Platform ti814x-evm Target m3
edma3_lld_ti814x-evm_m3_libs_clean: edma3_lld_ti814x-evm_m3_libs_drv_clean edma3_lld_ti814x-evm_m3_libs_rm_clean edma3_lld_ti814x-evm_m3_libs_drvsample_clean edma3_lld_ti814x-evm_m3_libs_rmsample_clean
edma3_lld_ti814x-evm_m3_libs_drv_clean:
	$(ECHO) \# Cleaning m3:debug:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning m3:release:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning m3:debug:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning m3:release:edma3_lld_drv 
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
edma3_lld_ti814x-evm_m3_libs_rm_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
edma3_lld_ti814x-evm_m3_libs_drvsample_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_drv_sample 	
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
edma3_lld_ti814x-evm_m3_libs_rmsample_clean:
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release	
	$(ECHO) \# Cleaning ti814x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning ti814x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release

#=======================================================================================================================================
#To Build libs For Platform c6a811x-evm Target 674
edma3_lld_c6a811x-evm_674_libs: edma3_lld_ti814x-evm_674_libs_drv edma3_lld_c6a811x-evm_674_libs_rm edma3_lld_c6a811x-evm_674_libs_drvsample edma3_lld_c6a811x-evm_674_libs_rmsample
edma3_lld_c6a811x-evm_674_libs_drvsample:
	$(ECHO) \# Making c6a811x-evm :debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6a811x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_c6a811x-evm_674_libs_rm:
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6a811x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm PROFILE_c6xdsp=release
edma3_lld_c6a811x-evm_674_libs_rmsample:
	$(ECHO) \# Making c6a811x-evm :debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6a811x-evm :release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#To Clean libs For Platform c6a811x-evm Target 674
edma3_lld_c6a811x-evm_674_libs_clean: edma3_lld_ti814x-evm_674_libs_drv_clean edma3_lld_c6a811x-evm_674_libs_rm_clean edma3_lld_c6a811x-evm_674_libs_drvsample_clean edma3_lld_c6a811x-evm_674_libs_rmsample_clean
edma3_lld_c6a811x-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm :debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6a811x-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_c6a811x-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6a811x-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm PROFILE_c6xdsp=release
edma3_lld_c6a811x-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6a811x-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release

#=======================================================================================================================================
#To Build libs For Platform c6a811x-evm Target a8
edma3_lld_c6a811x-evm_a8_libs: edma3_lld_ti814x-evm_a8_libs_drv edma3_lld_c6a811x-evm_a8_libs_rm edma3_lld_c6a811x-evm_a8_libs_drvsample edma3_lld_c6a811x-evm_a8_libs_rmsample
edma3_lld_c6a811x-evm_a8_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_c6a811x-evm_a8_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
endif
edma3_lld_c6a811x-evm_a8_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
endif

#To Clean libs For Platform c6a811x-evm Target a8
edma3_lld_c6a811x-evm_a8_libs_clean: edma3_lld_ti814x-evm_a8_libs_drv_clean edma3_lld_c6a811x-evm_a8_libs_rm_clean edma3_lld_c6a811x-evm_a8_libs_drvsample_clean edma3_lld_c6a811x-evm_a8_libs_rmsample_clean
edma3_lld_c6a811x-evm_a8_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_c6a811x-evm_a8_libs_rm_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
edma3_lld_c6a811x-evm_a8_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release

#=======================================================================================================================================
#To Build libs For Platform c6a811x-evm Target m3
edma3_lld_c6a811x-evm_m3_libs: edma3_lld_ti814x-evm_m3_libs_drv edma3_lld_c6a811x-evm_m3_libs_rm edma3_lld_c6a811x-evm_m3_libs_drvsample edma3_lld_c6a811x-evm_m3_libs_rmsample
edma3_lld_c6a811x-evm_m3_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_drv_sample 	
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
endif
edma3_lld_c6a811x-evm_m3_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
endif
edma3_lld_c6a811x-evm_m3_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release	
	$(ECHO) \# Making c6a811x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making c6a811x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
endif

#To Clean libs For Platform c6a811x-evm Target m3
edma3_lld_c6a811x-evm_m3_libs_clean: edma3_lld_ti814x-evm_m3_libs_drv_clean edma3_lld_c6a811x-evm_m3_libs_rm_clean edma3_lld_c6a811x-evm_m3_libs_drvsample_clean edma3_lld_c6a811x-evm_m3_libs_rmsample_clean
edma3_lld_c6a811x-evm_m3_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_drv_sample 	
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_drv_sample 
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
edma3_lld_c6a811x-evm_m3_libs_rm_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm 
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
edma3_lld_c6a811x-evm_m3_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release	
	$(ECHO) \# Cleaning c6a811x-evm:debug:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning c6a811x-evm:rel:edma3_lld_rm_sample 
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release

#=======================================================================================================================================
#To Build libs For Platform c6472-evm Target 64p
edma3_lld_c6472-evm_64p_libs: edma3_lld_c6472-evm_64p_libs_drv edma3_lld_c6472-evm_64p_libs_rm edma3_lld_c6472-evm_64p_libs_drvsample edma3_lld_c6472-evm_64p_libs_rmsample
edma3_lld_c6472-evm_64p_libs_drv:
	$(ECHO) \# Making c64p:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c64p:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c64p:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c64p:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_rm:
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6472-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c6472-evm:release:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6472-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_drvsample:
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6472-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6472-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_rmsample:
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6472-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6472-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6472-evm:release:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#To Clean libs For Platform c6472-evm Target 64p
edma3_lld_c6472-evm_64p_libs_clean: edma3_lld_c6472-evm_64p_libs_drv_clean edma3_lld_c6472-evm_64p_libs_rm_clean edma3_lld_c6472-evm_64p_libs_drvsample_clean edma3_lld_c6472-evm_64p_libs_rmsample_clean
edma3_lld_c6472-evm_64p_libs_drv_clean:
	$(ECHO) \# Cleaning c64p:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c64p:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c64p:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c64p:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_rm_clean:
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6472-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6472-evm_64p_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6472-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6472-evm:release:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform c6670-evm Target 66
edma3_lld_c6670-evm_66_libs: edma3_lld_c6670-evm_66_libs_drv edma3_lld_c6670-evm_66_libs_rm edma3_lld_c6670-evm_66_libs_drvsample edma3_lld_c6670-evm_66_libs_rmsample
edma3_lld_c6670-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6670-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6670-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c6670-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6670-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6670-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6670-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6670-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6670-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform c6670-evm Target 66
edma3_lld_c6670-evm_66_libs_clean: edma3_lld_c6670-evm_66_libs_drv_clean edma3_lld_c6670-evm_66_libs_rm_clean edma3_lld_c6670-evm_66_libs_drvsample_clean edma3_lld_c6670-evm_66_libs_rmsample_clean
edma3_lld_c6670-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6670-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6670-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6670-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6670-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6670-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6670-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6670-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6670-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform c6678-evm Target 66
edma3_lld_c6678-evm_66_libs:edma3_lld_c6678-evm_66_libs_drv edma3_lld_c6678-evm_66_libs_rm edma3_lld_c6678-evm_66_libs_drvsample edma3_lld_c6678-evm_66_libs_rmsample
edma3_lld_c6678-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6678-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6678-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c6678-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6678-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6678-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6678-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6678-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6678-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform c6678-evm Target 66
edma3_lld_c6678-evm_66_libs_clean: edma3_lld_c6678-evm_66_libs_drv_clean edma3_lld_c6678-evm_66_libs_rm_clean edma3_lld_c6678-evm_66_libs_drvsample_clean edma3_lld_c6678-evm_66_libs_rmsample_clean
edma3_lld_c6678-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6678-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6678-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6678-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6678-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6678-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6678-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6678-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6678-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6614-evm Target 66
edma3_lld_tci6614-evm_66_libs:edma3_lld_tci6614-evm_66_libs_drv edma3_lld_tci6614-evm_66_libs_rm edma3_lld_tci6614-evm_66_libs_drvsample edma3_lld_tci6614-evm_66_libs_rmsample
edma3_lld_tci6614-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6614-evm Target 66
edma3_lld_tci6614-evm_66_libs_clean: edma3_lld_tci6614-evm_66_libs_drv_clean edma3_lld_tci6614-evm_66_libs_rm_clean edma3_lld_tci6614-evm_66_libs_drvsample_clean edma3_lld_tci6614-evm_66_libs_rmsample_clean
edma3_lld_tci6614-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform c6657-evm Target 66
edma3_lld_c6657-evm_66_libs:edma3_lld_c6657-evm_66_libs_drv edma3_lld_c6657-evm_66_libs_rm edma3_lld_c6657-evm_66_libs_drvsample edma3_lld_c6657-evm_66_libs_rmsample
edma3_lld_c6657-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform c6657-evm Target 66
edma3_lld_c6657-evm_66_libs_clean: edma3_lld_c6657-evm_66_libs_drv_clean edma3_lld_c6657-evm_66_libs_rm_clean edma3_lld_c6657-evm_66_libs_drvsample_clean edma3_lld_c6657-evm_66_libs_rmsample_clean
edma3_lld_c6657-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6638k2k-evm Target 66
edma3_lld_tci6638k2k-evm_66_libs:edma3_lld_tci6638k2k-evm_66_libs_drv edma3_lld_tci6638k2k-evm_66_libs_rm edma3_lld_tci6638k2k-evm_66_libs_drvsample edma3_lld_tci6638k2k-evm_66_libs_rmsample
edma3_lld_tci6638k2k-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6638k2k-evm Target 66
edma3_lld_tci6638k2k-evm_66_libs_clean: edma3_lld_tci6638k2k-evm_66_libs_drv_clean edma3_lld_tci6638k2k-evm_66_libs_rm_clean edma3_lld_tci6638k2k-evm_66_libs_drvsample_clean edma3_lld_tci6638k2k-evm_66_libs_rmsample_clean
edma3_lld_tci6638k2k-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6636k2h-evm Target 66
edma3_lld_tci6636k2h-evm_66_libs:edma3_lld_tci6636k2h-evm_66_libs_drv edma3_lld_tci6636k2h-evm_66_libs_rm edma3_lld_tci6636k2h-evm_66_libs_drvsample edma3_lld_tci6636k2h-evm_66_libs_rmsample
edma3_lld_tci6636k2h-evm_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6636k2h-evm_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6636k2h-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6636k2h-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6636k2h-evm_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6636k2h-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6636k2h-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6636k2h-evm_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6636k2h-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6636k2h-evm Target 66
edma3_lld_tci6636k2h-evm_66_libs_clean: edma3_lld_tci6636k2h-evm_66_libs_drv_clean edma3_lld_tci6636k2h-evm_66_libs_rm_clean edma3_lld_tci6636k2h-evm_66_libs_drvsample_clean edma3_lld_tci6636k2h-evm_66_libs_rmsample_clean
edma3_lld_tci6636k2h-evm_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6636k2h-evm_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6636k2h-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6636k2h-evm:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6636k2h-evm_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6636k2h-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6636k2h-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6636k2h-evm_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6636k2h-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform c6748-evm Target 674
edma3_lld_c6748-evm_674_libs: edma3_lld_ti814x-evm_674_libs_drv edma3_lld_c6748-evm_674_libs_rm edma3_lld_c6748-evm_674_libs_drvsample edma3_lld_c6748-evm_674_libs_rmsample
edma3_lld_c6748-evm_674_libs_rm:
	$(ECHO) \# Making c6748-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6748-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6748-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6748-evm PROFILE_c6xdsp=release
edma3_lld_c6748-evm_674_libs_rmsample:
	$(ECHO) \# Making c6748-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6748-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_c6748-evm_674_libs_drvsample:
	$(ECHO) \# Making c6748-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6748-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#To Clean libs For Platform c6748-evm Target 674
edma3_lld_c6748-evm_674_libs_clean: edma3_lld_ti814x-evm_674_libs_drv_clean edma3_lld_c6748-evm_674_libs_rm_clean edma3_lld_c6748-evm_674_libs_drvsample_clean edma3_lld_c6748-evm_674_libs_rmsample_clean
edma3_lld_c6748-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning c6748-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6748-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6748-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6748-evm PROFILE_c6xdsp=release
edma3_lld_c6748-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6748-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6748-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_c6748-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6748-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6748-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#=======================================================================================================================================
#To Build libs For Platform da830-evm Target 674
edma3_lld_da830-evm_674_libs: edma3_lld_ti814x-evm_674_libs_drv edma3_lld_da830-evm_674_libs_rm edma3_lld_da830-evm_674_libs_drvsample edma3_lld_da830-evm_674_libs_rmsample
edma3_lld_da830-evm_674_libs_rm:
	$(ECHO) \# Making da830-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=da830-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making da830-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=da830-evm PROFILE_c6xdsp=release
edma3_lld_da830-evm_674_libs_rmsample:
	$(ECHO) \# Making da830-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making da830-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_da830-evm_674_libs_drvsample:
	$(ECHO) \# Making da830-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making da830-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#To Clean libs For Platform da830-evm Target 674
edma3_lld_da830-evm_674_libs_clean: edma3_lld_ti814x-evm_674_libs_drv_clean edma3_lld_da830-evm_674_libs_rm_clean edma3_lld_da830-evm_674_libs_drvsample_clean edma3_lld_da830-evm_674_libs_rmsample_clean
edma3_lld_da830-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning da830-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=da830-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning da830-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=da830-evm PROFILE_c6xdsp=release
edma3_lld_da830-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning da830-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning da830-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_da830-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning da830-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning da830-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#=======================================================================================================================================
#To Build libs For Platform omap4-evm Target 64t
edma3_lld_omap4-evm_64t_libs: edma3_lld_omap4-evm_64t_libs_drv edma3_lld_omap4-evm_64t_libs_rm edma3_lld_omap4-evm_64t_libs_drvsample edma3_lld_omap4-evm_64t_libs_rmsample
edma3_lld_omap4-evm_64t_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c64t:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=omap4-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making c64t:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=omap4-evm PROFILE_c6xdsp=release
endif
edma3_lld_omap4-evm_64t_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omap4-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omap4-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making omap4-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omap4-evm PROFILE_c6xdsp=release
endif
edma3_lld_omap4-evm_64t_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omap4-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making omap4-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif
edma3_lld_omap4-evm_64t_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omap4-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making omap4-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release
endif

#To Clean libs For Platform omap4-evm Target 64t
edma3_lld_omap4-evm_64t_libs_clean: edma3_lld_omap4-evm_64t_libs_drv_clean edma3_lld_omap4-evm_64t_libs_rm_clean edma3_lld_omap4-evm_64t_libs_drvsample_clean edma3_lld_omap4-evm_64t_libs_rmsample_clean
edma3_lld_omap4-evm_64t_libs_drv_clean:
	$(ECHO) \# Cleaning c64t:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=omap4-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c64t:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=omap4-evm PROFILE_c6xdsp=release
edma3_lld_omap4-evm_64t_libs_rm_clean:
	$(ECHO) \# Cleaning omap4-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omap4-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning omap4-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omap4-evm PROFILE_c6xdsp=release
edma3_lld_omap4-evm_64t_libs_rmsample_clean:
	$(ECHO) \# Cleaning omap4-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning omap4-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_omap4-evm_64t_libs_drvsample_clean:
	$(ECHO) \# Cleaning omap4-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning omap4-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release

#=======================================================================================================================================
#To Build libs For Platform omapl138-evm Target 674
edma3_lld_omapl138-evm_674_libs: edma3_lld_ti814x-evm_674_libs_drv edma3_lld_omapl138-evm_674_libs_rm edma3_lld_omapl138-evm_674_libs_drvsample edma3_lld_omapl138-evm_674_libs_rmsample
edma3_lld_omapl138-evm_674_libs_rm:
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omapl138-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making omapl138-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omapl138-evm PROFILE_c6xdsp=release
edma3_lld_omapl138-evm_674_libs_rmsample:
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making omapl138-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_omapl138-evm_674_libs_drvsample:
	$(ECHO) \# Making omapl138-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#To Clean libs For Platform omapl138-evm Target 674
edma3_lld_omapl138-evm_674_libs_clean: edma3_lld_ti814x-evm_674_libs_drv_clean edma3_lld_omapl138-evm_674_libs_rm_clean edma3_lld_omapl138-evm_674_libs_drvsample_clean edma3_lld_omapl138-evm_674_libs_rmsample_clean
edma3_lld_omapl138-evm_674_libs_rm_clean:
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omapl138-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning omapl138-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omapl138-evm PROFILE_c6xdsp=release
edma3_lld_omapl138-evm_674_libs_rmsample_clean:
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning omapl138-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release
edma3_lld_omapl138-evm_674_libs_drvsample_clean:
	$(ECHO) \# Cleaning omapl138-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug

#=======================================================================================================================================
#To Build libs For Platform omapl138-evm Target arm9
edma3_lld_omapl138-evm_arm9_libs: edma3_lld_omapl138-evm_arm9_libs_drv edma3_lld_omapl138-evm_arm9_libs_rm edma3_lld_omapl138-evm_arm9_libs_drvsample edma3_lld_omapl138-evm_arm9_libs_rmsample
edma3_lld_omapl138-evm_arm9_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making arm9:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making arm9:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif
edma3_lld_omapl138-evm_arm9_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making omapl138-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif
edma3_lld_omapl138-evm_arm9_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making omapl138-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif
edma3_lld_omapl138-evm_arm9_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making omapl138-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making omapl138-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif

#To Clean libs For Platform omapl138-evm Target arm9
edma3_lld_omapl138-evm_arm9_libs_clean: edma3_lld_omapl138-evm_arm9_libs_drv_clean edma3_lld_omapl138-evm_arm9_libs_rm_clean edma3_lld_omapl138-evm_arm9_libs_drvsample_clean edma3_lld_omapl138-evm_arm9_libs_rmsample_clean
edma3_lld_omapl138-evm_arm9_libs_drv_clean:
	$(ECHO) \# Cleaning arm9:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning arm9:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
edma3_lld_omapl138-evm_arm9_libs_rm_clean:
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning omapl138-evm:rel:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
edma3_lld_omapl138-evm_arm9_libs_rmsample_clean:
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning omapl138-evm:rel:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
edma3_lld_omapl138-evm_arm9_libs_drvsample_clean:
	$(ECHO) \# Cleaning omapl138-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning omapl138-evm:rel:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release

#=======================================================================================================================================
#To Build libs For Platform tci6486-evm Target 64p
edma3_lld_tci6486-evm_64p_libs: edma3_lld_c6472-evm_64p_libs_drv edma3_lld_tci6486-evm_64p_libs_rm edma3_lld_tci6486-evm_64p_libs_drvsample edma3_lld_tci6486-evm_64p_libs_rmsample
edma3_lld_tci6486-evm_64p_libs_rm:
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6486-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6486-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6486-evm PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6486-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6486-evm_64p_libs_rmsample:
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6486-evm_64p_libs_drvsample:
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6486-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6486-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#To Clean libs For Platform tci6486-evm Target 64p
edma3_lld_tci6486-evm_64p_libs_clean: edma3_lld_c6472-evm_64p_libs_drv_clean edma3_lld_tci6486-evm_64p_libs_rm_clean edma3_lld_tci6486-evm_64p_libs_drvsample_clean edma3_lld_tci6486-evm_64p_libs_rmsample_clean
edma3_lld_tci6486-evm_64p_libs_rm_clean:
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6486-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6486-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6486-evm PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6486-evm PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6486-evm_64p_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6486-evm_64p_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6486-evm:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6486-evm:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6608-sim Target 66
edma3_lld_tci6608-sim_66_libs: edma3_lld_tci6608-sim_66_libs_drv edma3_lld_tci6608-sim_66_libs_rm edma3_lld_tci6608-sim_66_libs_drvsample edma3_lld_tci6608-sim_66_libs_rmsample
edma3_lld_tci6608-sim_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6608-sim_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6608-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6608-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6608-sim_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6608-sim_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6608-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6608-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6608-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6608-sim Target 66
edma3_lld_tci6608-sim_66_libs_clean: edma3_lld_tci6608-sim_66_libs_drv_clean edma3_lld_tci6608-sim_66_libs_rm_clean edma3_lld_tci6608-sim_66_libs_drvsample_clean edma3_lld_tci6608-sim_66_libs_rmsample_clean
edma3_lld_tci6608-sim_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6608-sim_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6608-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6608-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6608-sim_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6608-sim_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6608-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6608-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6608-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6616-sim Target 66
edma3_lld_tci6616-sim_66_libs: edma3_lld_tci6616-sim_66_libs_drv edma3_lld_tci6616-sim_66_libs_rm edma3_lld_tci6616-sim_66_libs_drvsample edma3_lld_tci6616-sim_66_libs_rmsample
edma3_lld_tci6616-sim_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6616-sim_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6616-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6616-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6616-sim_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6616-sim_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6616-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6616-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6616-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6616-sim Target 66
edma3_lld_tci6616-sim_66_libs_clean: edma3_lld_tci6616-sim_66_libs_drv_clean edma3_lld_tci6616-sim_66_libs_rm_clean edma3_lld_tci6616-sim_66_libs_drvsample_clean edma3_lld_tci6616-sim_66_libs_rmsample_clean
edma3_lld_tci6616-sim_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6616-sim_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6616-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6616-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6616-sim_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6616-sim_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6616-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6616-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6616-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6614-sim Target 66
edma3_lld_tci6614-sim_66_libs: edma3_lld_tci6614-sim_66_libs_drv edma3_lld_tci6614-sim_66_libs_rm edma3_lld_tci6614-sim_66_libs_drvsample edma3_lld_tci6614-sim_66_libs_rmsample
edma3_lld_tci6614-sim_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-sim_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-sim_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6614-sim_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6614-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6614-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6614-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6614-sim Target 66
edma3_lld_tci6614-sim_66_libs_clean: edma3_lld_tci6614-sim_66_libs_drv_clean edma3_lld_tci6614-sim_66_libs_rm_clean edma3_lld_tci6614-sim_66_libs_drvsample_clean edma3_lld_tci6614-sim_66_libs_rmsample_clean
edma3_lld_tci6614-sim_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-sim_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-sim_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6614-sim_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6614-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6614-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6614-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform c6657-sim Target 66
edma3_lld_c6657-sim_66_libs: edma3_lld_c6657-sim_66_libs_drv edma3_lld_c6657-sim_66_libs_rm edma3_lld_c6657-sim_66_libs_drvsample edma3_lld_c6657-sim_66_libs_rmsample
edma3_lld_c6657-sim_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-sim_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-sim_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_c6657-sim_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making c6657-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c6657-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making c6657-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform c6657-sim Target 66
edma3_lld_c6657-sim_66_libs_clean: edma3_lld_c6657-sim_66_libs_drv_clean edma3_lld_c6657-sim_66_libs_rm_clean edma3_lld_c6657-sim_66_libs_drvsample_clean edma3_lld_c6657-sim_66_libs_rmsample_clean
edma3_lld_c6657-sim_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-sim_66_libs_rm_clean:
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-sim_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_c6657-sim_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c6657-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c6657-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c6657-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform tci6638k2k-sim Target 66
edma3_lld_tci6638k2k-sim_66_libs: edma3_lld_tci6638k2k-sim_66_libs_drv edma3_lld_tci6638k2k-sim_66_libs_rm edma3_lld_tci6638k2k-sim_66_libs_drvsample edma3_lld_tci6638k2k-sim_66_libs_rmsample
edma3_lld_tci6638k2k-sim_66_libs_drv:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-sim_66_libs_rm:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-sim_66_libs_rmsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif
edma3_lld_tci6638k2k-sim_66_libs_drvsample:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making tci6638k2k-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making tci6638k2k-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making tci6638k2k-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
endif

#To Clean libs For Platform tci6638k2k-sim Target 66
edma3_lld_tci6638k2k-sim_66_libs_clean: edma3_lld_tci6638k2k-sim_66_libs_drv_clean edma3_lld_tci6638k2k-sim_66_libs_rm_clean edma3_lld_tci6638k2k-sim_66_libs_drvsample_clean edma3_lld_tci6638k2k-sim_66_libs_rmsample_clean
edma3_lld_tci6638k2k-sim_66_libs_drv_clean:
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning c66:debug:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning c66:release:edma3_lld_drv -for big_endian
	$(MAKE) -C $(edma3_lld_drv_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-sim_66_libs_rm_clean:
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm -for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-sim:release:edma3_lld_rm
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-sim:release:edma3_lld_rm for big_endian
	$(MAKE) -C $(edma3_lld_rm_PATH) clean PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-sim_66_libs_rmsample_clean:
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm_sample
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_rm_sample -for big_endian
	$(MAKE) -C $(edma3_lld_rm_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
edma3_lld_tci6638k2k-sim_66_libs_drvsample_clean:
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning tci6638k2k-sim:debug:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning tci6638k2k-sim:release:edma3_lld_drv_sample
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Cleaning tci6638k2k-sim:release:edma3_lld_drv_sample -for big_endian
	$(MAKE) -C $(edma3_lld_drv_sample_PATH) clean PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

#=======================================================================================================================================
#To Build libs For Platform Generic
edma3_lld_rm_generic:
	$(ECHO) \# Making generic/c674:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=674 PROFILE_674=debug
	$(ECHO) \# Making generic/c674:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=674 PROFILE_674=release
	$(ECHO) \# Making generic/c64p:debug:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=64p PROFILE_64p=debug ENDIAN=big
	$(ECHO) \# Making generic/c64p:release:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=64p PROFILE_64p=release ENDIAN=big
	$(ECHO) \# Making generic/c64p:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=64p PROFILE_64p=debug ENDIAN=little
	$(ECHO) \# Making generic/c64p:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=64p PROFILE_64p=release ENDIAN=little
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making generic/c66:debug:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=66 PROFILE_66=debug ENDIAN=big
	$(ECHO) \# Making generic/c66:release:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=66 PROFILE_66=release ENDIAN=big
	$(ECHO) \# Making generic/c66:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=66 PROFILE_66=debug ENDIAN=little
	$(ECHO) \# Making generic/c66:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=66 PROFILE_66=release ENDIAN=little
endif
	$(ECHO) \# Making generic/m3:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=m3 PROFILE_m3=debug
	$(ECHO) \# Making generic/m3:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=m3 PROFILE_m3=release
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Making generic/a8:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=a8 PROFILE_a8=debug
	$(ECHO) \# Making generic/a8:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=a8 PROFILE_a8=release
	$(ECHO) \# Making generic/arm9:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making generic/arm9:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic,,$@)_PATH) PLATFORM=generic CORE=arm9 PROFILE_arm9=release
endif

#To Clean libs For Platform Generic
edma3_lld_rm_generic_clean:
	$(ECHO) \# Cleaning generic/c674:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=674 PROFILE_674=debug
	$(ECHO) \# Cleaning generic/c674:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=674 PROFILE_674=release
	$(ECHO) \# Cleaning generic/c64p:debug:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=64p PROFILE_64p=debug ENDIAN=big
	$(ECHO) \# Cleaning generic/c64p:release:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=64p PROFILE_64p=release ENDIAN=big
	$(ECHO) \# Cleaning generic/c64p:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=64p PROFILE_64p=debug ENDIAN=little
	$(ECHO) \# Cleaning generic/c64p:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=64p PROFILE_64p=release ENDIAN=little
	$(ECHO) \# Cleaning generic/c66:debug:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=66 PROFILE_66=debug ENDIAN=big
	$(ECHO) \# Cleaning generic/c66:release:edma3_lld_rm: -for big_endian
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=66 PROFILE_66=release ENDIAN=big
	$(ECHO) \# Cleaning generic/c66:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=66 PROFILE_66=debug ENDIAN=little
	$(ECHO) \# Cleaning generic/c66:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=66 PROFILE_66=release ENDIAN=little
	$(ECHO) \# Cleaning generic/m3:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=m3 PROFILE_m3=debug
	$(ECHO) \# Cleaning generic/m3:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=m3 PROFILE_m3=release
	$(ECHO) \# Cleaning generic/a8:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=a8 PROFILE_a8=debug
	$(ECHO) \# Cleaning generic/a8:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=a8 PROFILE_a8=release
	$(ECHO) \# Cleaning generic/arm9:debug:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning generic/arm9:release:edma3_lld_rm
	$(MAKE) -C $($(subst _generic_clean,,$@)_PATH) clean PLATFORM=generic CORE=arm9 PROFILE_arm9=release

#=======================================================================================================================================
#
# Rule to build all examples
#
#=======================================================================================================================================
edma3_drv_c6472-evm_64p_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_c6472-evm_64p_be_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6472-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6748-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6748-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_da830-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=da830-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_omapl138-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omapl138-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_omap4-evm_64t_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omap4-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_tci6486-evm_64p_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_tci6486-evm_64p_be_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6486-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_ti816x-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_ti814x-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_c6a811x-evm_674_example:
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_tci6608-sim_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release
endif

edma3_drv_tci6608-sim_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6608-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big
endif

edma3_drv_tci6616-sim_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release
endif

edma3_drv_tci6616-sim_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6616-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big
endif

edma3_drv_tci6614-sim_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release
endif

edma3_drv_tci6614-sim_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big
endif

edma3_drv_c6657-sim_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release
endif

edma3_drv_c6657-sim_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big
endif

edma3_drv_tci6638k2k-sim_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release
endif

edma3_drv_tci6638k2k-sim_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-sim CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big
endif

edma3_drv_c6670-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release
endif

edma3_drv_c6670-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6670-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_c6678-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release
endif

edma3_drv_c6678-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6678-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_tci6614-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release
endif

edma3_drv_tci6614-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6614-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_c6657-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release
endif

edma3_drv_c6657-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6657-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_tci6638k2k-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release
endif

edma3_drv_tci6638k2k-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6638k2k-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_tci6636k2h-evm_66_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release
endif

edma3_drv_tci6636k2h-evm_66_be_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Configuring XDC packages for $@:c6xdsp:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tci6636k2h-evm CORE=c6xdsp PROFILE_c6xdsp=release ENDIAN=big
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big
endif


edma3_drv_omapl138-evm_arm9_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:arm9:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Configuring XDC packages for $@:arm9:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif

edma3_drv_ti814x-evm_a8_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:a8:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Configuring XDC packages for $@:a8:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_ti816x-evm_a8_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:a8:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Configuring XDC packages for $@:a8:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_c6a811x-evm_a8_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:a8:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Configuring XDC packages for $@:a8:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_ti816x-evm_m3_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:m3:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug

	$(ECHO) \# Configuring XDC packages for $@:m3:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release

	$(ECHO) \# Configuring XDC packages for $@:m3:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug

	$(ECHO) \# Configuring XDC packages for $@:m3:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
endif

edma3_drv_ti814x-evm_m3_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:m3:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug	

	$(ECHO) \# Configuring XDC packages for $@:m3:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release

	$(ECHO) \# Configuring XDC packages for $@:m3:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug

	$(ECHO) \# Configuring XDC packages for $@:m3:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release	
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
endif

edma3_drv_c6a811x-evm_m3_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:m3:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug	

	$(ECHO) \# Configuring XDC packages for $@:m3:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release

	$(ECHO) \# Configuring XDC packages for $@:m3:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug

	$(ECHO) \# Configuring XDC packages for $@:m3:release
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release	
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
endif
edma3_drv_tda2xx-evm_66_example:
	$(ECHO) \# Configuring XDC packages for $@:m4:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=debug	

	$(ECHO) \# Configuring XDC packages for $@:m4:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=c6xdsp PROFILE_c6xdsp=release

edma3_drv_tda2xx-evm_m4_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:m4:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug IPUCORE=0
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug	IPUCORE=0

	$(ECHO) \# Configuring XDC packages for $@:m4:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release IPUCORE=0
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release IPUCORE=0

	$(ECHO) \# Configuring XDC packages for $@:m4:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug IPUCORE=1
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug IPUCORE=1

	$(ECHO) \# Configuring XDC packages for $@:m4:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release IPUCORE=1
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release IPUCORE=1
endif

edma3_drv_tda2xx-evm_a15_example:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Configuring XDC packages for $@:a15host:debug 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug
	$(ECHO) \# Making example $@:debug
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=debug	

	$(ECHO) \# Configuring XDC packages for $@:a15host:release 
	$(MAKE) -C $($@_EXAMPLES_PATH) xdc_configuro PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
	$(ECHO) \# Making example $@:release
	$(MAKE) -C $($@_EXAMPLES_PATH) PLATFORM=tda2xx-evm CORE=a15host PROFILE_a15host=release
endif

#=======================================================================================================================================
#
# Rule to clean all examples
#
#=======================================================================================================================================

edma3_drv_c6472-evm_64p_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6472-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6472-evm PROFILE_c6xdsp=release

edma3_drv_c6472-evm_64p_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6472-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6472-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6748-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6748-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6748-evm PROFILE_c6xdsp=release

edma3_drv_da830-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=da830-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=da830-evm PROFILE_c6xdsp=release

edma3_drv_omapl138-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omapl138-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omapl138-evm PROFILE_c6xdsp=release

edma3_drv_omapl138-evm_arm9_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omapl138-evm CORE=arm9 PROFILE_arm9=release
endif

edma3_drv_omap4-evm_64t_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omap4-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=omap4-evm PROFILE_c6xdsp=release

edma3_drv_tci6486-evm_64p_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6486-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6486-evm PROFILE_c6xdsp=release

edma3_drv_tci6486-evm_64p_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6486-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6486-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_ti816x-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm PROFILE_c6xdsp=release

edma3_drv_ti814x-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm PROFILE_c6xdsp=release

edma3_drv_c6a811x-evm_674_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm PROFILE_c6xdsp=release

edma3_drv_ti814x-evm_a8_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_ti816x-evm_a8_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_c6a811x-evm_a8_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=a8host PROFILE_a8host=release
endif

edma3_drv_ti814x-evm_m3_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti814x-evm CORE=m3video PROFILE_m3video=release
endif

edma3_drv_tda2xx-evm_m4_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tda2xx-evm CORE=m4 PROFILE_m4=release
endif

edma3_drv_ti816x-evm_m3_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=ti816x-evm CORE=m3video PROFILE_m3video=release
endif

edma3_drv_c6a811x-evm_m3_example_clean:
ifeq ($(FORMAT),ELF)
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=m3vpss PROFILE_m3vpss=release
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6a811x-evm CORE=m3video PROFILE_m3video=release
endif

edma3_drv_tci6608-sim_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6608-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6608-sim PROFILE_c6xdsp=release

edma3_drv_tci6608-sim_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6608-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6608-sim PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6616-sim_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6616-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6616-sim PROFILE_c6xdsp=release

edma3_drv_tci6616-sim_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6616-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6616-sim PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6614-sim_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-sim PROFILE_c6xdsp=release

edma3_drv_tci6614-sim_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-sim PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6657-sim_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-sim PROFILE_c6xdsp=release

edma3_drv_c6657-sim_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-sim PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6638k2k-sim_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release

edma3_drv_tci6638k2k-sim_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-sim PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6670-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6670-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6670-evm PROFILE_c6xdsp=release

edma3_drv_c6670-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6670-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6670-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6678-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6678-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6678-evm PROFILE_c6xdsp=release

edma3_drv_c6678-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6678-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6678-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6614-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-evm PROFILE_c6xdsp=release

edma3_drv_tci6614-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6614-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_c6657-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-evm PROFILE_c6xdsp=release

edma3_drv_c6657-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=c6657-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6638k2k-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release

edma3_drv_tci6638k2k-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6638k2k-evm PROFILE_c6xdsp=release ENDIAN=big

edma3_drv_tci6636k2h-evm_66_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release

edma3_drv_tci6636k2h-evm_66_be_example_clean:
	$(ECHO) \# Cleaning example $@:debug
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=debug ENDIAN=big
	$(ECHO) \# Cleaning example $@:release
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean  PLATFORM=tci6636k2h-evm PROFILE_c6xdsp=release ENDIAN=big


# Help
help:
	$(ECHO) EDMA3 LLD Driver Help page
	$(ECHO) To build libraries and examples:
	$(ECHO) "Command line: make [all\|libs\|examples] [FORMAT=\{ELF\|COFF\}] [PLATFORM=\{ti816x-evm\|da830-3vm\|etc\}] [TARGET=\{674\|a8\|etc\}]"
	$(ECHO) "	Default target is "all", which builds both libraries and examples"
	$(ECHO) "	Default FORMAT is specified in makerules/build_config.mk. This can be"
	$(ECHO) "	 overridden by specifying a different FORMAT here."
	$(ECHO) "   To build for specific platform specify the PLATFORM here
	$(ECHO) To clean the build files:
	$(ECHO) "Command line: make \{clean\|cleanall\}"


# Nothing beyond this point
