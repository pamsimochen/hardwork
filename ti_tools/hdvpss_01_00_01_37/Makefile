
include Rules.make

HDVPSS_EXAMPLES = $(hdvpss_PATH)/packages/ti/psp/examples

#This is only for getting commit id from git. This should not
#be used for any production system
print_git:
ifeq ($(VERSION_TS),yes)
	echo "char version_id[]=\"" > packages/ti/psp/vps/test1.h
	git log -n 1 --format=oneline > packages/ti/psp/vps/test2.h
	echo "\";" > packages/ti/psp/vps/test3.h
	cat packages/ti/psp/vps/test1.h packages/ti/psp/vps/test2.h packages/ti/psp/vps/test3.h > packages/ti/psp/vps/test.h
	rm  packages/ti/psp/vps/test1.h
	rm  packages/ti/psp/vps/test2.h
	rm  packages/ti/psp/vps/test3.h
endif

hdvpss:
#	$(MAKE) print_git
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/vps $(TARGET)
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/i2c $(TARGET)
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/devices $(TARGET)
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/platforms $(TARGET)
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/proxyServer $(TARGET)
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/examples/utility $(TARGET)

hdvpss_netcam:
	$(MAKE) print_git
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/vps $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/i2c $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/devices $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/platforms $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/proxyServer $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam
	$(MAKE) -C $(hdvpss_PATH)/packages/ti/psp/examples/utility $(TARGET) CUSTOM_MEM_FRAME_HEAP_SIZE=16 CUSTOM_BOARD=netcam

clean:
	$(MAKE) hdvpss TARGET=clean
	$(MAKE) examplesclean

examplesclean:
	$(MAKE) links_clean
	$(MAKE) chains_clean
	$(MAKE) captureVip_clean
	$(MAKE) display_clean
	$(MAKE) grpx_clean
	$(MAKE) proxy_clean
	$(MAKE) m2mSc_clean
	$(MAKE) m2mNsf_clean
	$(MAKE) m2mDei_clean
	$(MAKE) utils_clean
	$(MAKE) i2c_clean

examples : chains captureVip display grpx proxy m2mSc m2mDei m2mNsf utils i2c

examples_netcam : display_netcam m2mSc_netcam m2mDei_netcam grpx_netcam

all: clean examples

platforms:
	$(MAKE) print_git
	$(MAKE) examples PLATFORM=ti816x-evm
	$(MAKE) examples PLATFORM=ti814x-evm
	$(MAKE) examples PLATFORM=ti8107-evm
	$(MAKE) examples PLATFORM=ti816x-sim
	$(MAKE) examples PLATFORM=ti814x-sim

platformsclean:
	$(MAKE) clean PLATFORM=ti816x-evm
	$(MAKE) clean PLATFORM=ti814x-evm
	$(MAKE) clean PLATFORM=ti8107-evm
	$(MAKE) clean PLATFORM=ti816x-sim
	$(MAKE) clean PLATFORM=ti814x-sim

profiles:
	$(MAKE) print_git
	$(MAKE) examples PROFILE_$(CORE)=debug
	$(MAKE) examples PROFILE_$(CORE)=whole_program_debug

profilesclean:
	$(MAKE) clean PROFILE_$(CORE)=debug
	$(MAKE) clean PROFILE_$(CORE)=whole_program_debug

allall:
	$(MAKE) print_git
	$(MAKE) platforms PROFILE_$(CORE)=debug
	$(MAKE) platforms PROFILE_$(CORE)=whole_program_debug

allclean:
#	$(MAKE) platformsclean PROFILE_$(CORE)=debug
#	$(MAKE) platformsclean PROFILE_$(CORE)=whole_program_debug
	$(RM) -rf $(hdvpss_PATH)/build

links: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains/links

links_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains/links clean

chains: links
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti816x_largeHeap.bld
endif
ifeq ($(SOC),ti814x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti814x_largeHeap.bld
endif
ifeq ($(SOC),ti8107)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_largeHeap.bld
endif
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains

chains_clean: links_clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/chains clean

captureVip: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/capture/captureVip xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/capture/captureVip

captureVip_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/capture/captureVip clean

display_netcam: hdvpss_netcam
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_netcam.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay

display: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/sdDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/sdDisplay
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/triDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/triDisplay

display_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/sdDisplay clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/triDisplay clean

mosaicDisplay: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/mosaicDisplay

sdDisplay: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/sdDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/sdDisplay

triDisplay: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/triDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/display/triDisplay

grpx_netcam: hdvpss_netcam
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplay xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_netcam.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplay

grpx: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplay xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplay
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplayMultiReg xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplayMultiReg

grpx_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplay clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/graphics/grpxDisplayMultiReg clean

proxy_netcam: hdvpss_netcam
ifeq ($(SOC),ti8107)
	echo $(hdvpss_PATH)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti8107_netcam xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_netcam.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti8107_netcam
endif

proxy: hdvpss
ifeq ($(SOC),ti814x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_c6a814x.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x_1G xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti814x_1G.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x_1G
endif
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti816x xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti816x
endif
ifeq ($(SOC),ti8107)
	echo $(hdvpss_PATH)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti8107 xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti814x_1G.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti8107
endif

proxyDisplay: hdvpss
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyWithDisplayApp/ti816x xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyWithDisplayApp/ti816x
endif

proxy_clean:
ifeq ($(SOC),ti814x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x_1G clean
endif
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti816x clean
endif
ifeq ($(SOC),ti8107)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti8107 clean
endif

proxyDisplay_clean:
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/proxyWithDisplayApp/ti816x clean
endif

m2mSc_netcam: hdvpss_netcam
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mScMultiChan xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_netcam.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mScMultiChan

m2mSc: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mScMultiChan xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mScMultiChan

m2mSc_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mScMultiChan clean

m2mComp: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mComp xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mComp

m2mComp_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mComp clean

m2mNsf: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mNsf xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mNsf
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mNsf_Subframe xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mNsf_Subframe

m2mNsf_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mNsf clean

m2mDei_netcam: hdvpss_netcam
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mDeiScale xdc_configuro CONFIG_BLD_XDC_m3=$(hdvpss_PATH)/packages/config_ti8107_netcam.bld
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mDeiScale

m2mDei: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mDeiScale xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mDeiScale
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/ti816x/vps/m2m/m2mDeiHqMqMode1 xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/ti816x/vps/m2m/m2mDeiHqMqMode1
endif

m2mDei_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/vps/m2m/m2mDeiScale clean
ifeq ($(SOC),ti816x)
	$(MAKE) -C $(HDVPSS_EXAMPLES)/ti816x/vps/m2m/m2mDeiHqMqMode1 clean
endif

utils: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/printDesc xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/printDesc
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/vpdmaListDump xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/vpdmaListDump

utils_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/printDesc clean
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/utils/vpdmaListDump clean

i2c: hdvpss
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/i2c xdc_configuro
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/i2c

i2c_clean:
	$(MAKE) -C $(HDVPSS_EXAMPLES)/common/i2c clean
