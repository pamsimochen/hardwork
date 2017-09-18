
include Rules.make

ISS_EXAMPLES = $(iss_PATH)/packages/ti/psp/examples

iss:
	$(MAKE) -C $(iss_PATH)/packages/ti/psp/iss $(TARGET)
	$(MAKE) -C $(iss_PATH)/packages/ti/psp/devices $(TARGET)
	$(MAKE) -C $(iss_PATH)/packages/ti/psp/platforms $(TARGET)
	$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/rm $(TARGET)
#	cp $(iss_PATH)/build/ti/psp/iss/alg/rm/lib/ti814x-evm/m3/$(PROFILE_m3vpss)/iss_simcop_rm.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/rm/lib/release/iss_simcop_rm.aem3
#	$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/jpeg_enc $(TARGET)
#	cp $(iss_PATH)/build/ti/psp/iss/alg/jpeg_enc/lib/ti814x-evm/m3/$(PROFILE_m3vpss)/iss_jpeg_enc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/jpeg_enc/lib/release/iss_jpeg_enc.aem3
#	$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/vnf $(TARGET)
#	cp $(iss_PATH)/build/ti/psp/iss/alg/vnf/lib/ti814x-evm/m3/$(PROFILE_m3vpss)/iss_vnf.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/vnf/lib/release/iss_vnf.aem3
#	$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/vstab $(TARGET)
#	cp $(iss_PATH)/build/ti/psp/iss/alg/vstab/lib/ti814x-evm/m3/$(PROFILE_m3vpss)/iss_vstab.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/vstab/lib/release/iss_vstab.aem3

jpege:
	$(MAKE) -C $(iss_algo_PATH)/jpeg_enc $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_jpeg_enc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/jpeg_enc/lib/release/iss_jpeg_enc.aem3

jpegeclean:
	$(MAKE) jpege TARGET=clean

glbce:
	$(MAKE) -C $(iss_algo_PATH)/glbce $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_glbce.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/glbce/lib/release/iss_glbce.aem3
	
glbceclean:
	$(MAKE) glbce TARGET=clean

wdr:
	$(MAKE) -C $(iss_algo_PATH)/wdr $(TARGET)
	-cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_wdr.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/wdr/lib/release/iss_wdr.aem3
	
wdrclean:
	$(MAKE) wdr TARGET=clean

vrun:
	$(MAKE) -C $(iss_algo_PATH)/vrun $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_vrun.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/vrun/lib/release/iss_vrun.aem3

vrunclean:
	$(MAKE) vrun TARGET=clean

##### TIMMOSAL BUILD #####
timmosal:
	$(MAKE) -C $(iss_algo_PATH)/timmosal $(TARGET) CORE=m3vpss
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_timmosal.aem3 $(iss_PATH)/packages/ti/psp/iss/timmosal/lib/release/iss_timmosal.aem3

timmosalclean:
	$(MAKE) timmosal TARGET=clean


ip_run:
	$(MAKE) -C $(iss_algo_PATH)/ip_run $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_ip_run.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/ip_run/lib/release/iss_ip_run.aem3

ip_run_clean:
	$(MAKE) ip_run TARGET=clean

imx:
	$(MAKE) -C $(iss_algo_PATH)/imx $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_imx.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/imx/lib/release/iss_imx.aem3

imx_clean:
	$(MAKE) imx TARGET=clean

swosd:
	$(MAKE) -C $(iss_algo_PATH)/swosd $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_swosd.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/swosd/lib/release/iss_swosd.aem3

swosd_clean:
	$(MAKE) swosd TARGET=clean

##### IP_RUN2 BUILD #####

ip_run2:
	$(MAKE) -C $(iss_algo_PATH)/ip_run2 $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_ip_run2_bios.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/ip_run2/lib/release/iss_ip_run2_bios.aem3

ip_run2_clean:
	$(MAKE) ip_run2 TARGET=clean

##### IP_RUN2_SCHED BUILD #####
ip_run2_sched:
	$(MAKE) -C $(iss_algo_PATH)/ip_run2_sched $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_ip_run2_sched.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/ip_run2_sched/lib/release/iss_ip_run2_sched.aem3

ip_run2_sched_clean:
	$(MAKE) ip_run2_sched TARGET=clean
	
##### EVF BUILD #####
imx_core:
	$(MAKE) -C $(iss_algo_PATH)/evf -f Makefile_cpis_core $(TARGET) 
	$(MAKE) -C $(iss_algo_PATH)/evf -f Makefile_cpis_imgproc $(TARGET) 
	$(MAKE) -C $(iss_algo_PATH)/evf -f Makefile_cpis_vision $(TARGET) 
	$(MAKE) -C $(iss_algo_PATH)/evf -f Makefile_gpp_core $(TARGET) 
	$(MAKE) -C $(iss_algo_PATH)/evf -f Makefile_gpp_imgproc $(TARGET) 
	$(MAKE) -C $(iss_algo_PATH)/evf -f makefile_gpp_vision $(TARGET) 

	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/cpis_core.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/cpis_imgproc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/cpis_vision.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/gpp_core.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/gpp_imgproc.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/gpp_vision.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/evf/lib/release/

imx_core_clean:
	$(MAKE) imx_core TARGET=clean	

swosd_test:
	$(MAKE) -C $(iss_algo_PATH)/swosd/test xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/swosd/test

swosd_test_clean:
	$(MAKE) -C $(iss_algo_PATH)/swosd/test clean

swosd_test_pp:
	$(MAKE) -C $(iss_algo_PATH)/swosd/test_PP xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/swosd/test_PP

swosd_test_pp_clean:
	$(MAKE) -C $(iss_algo_PATH)/swosd/test_PP clean

vnf:
	$(MAKE) -C $(iss_algo_PATH)/vnf $(TARGET)
	cp $(iss_PATH)/build/lib/m3/$(PROFILE_m3vpss)/iss_vnf.aem3 $(iss_PATH)/packages/ti/psp/iss/alg/vnf/lib/release/iss_vnf.aem3

vnf_clean:
	$(MAKE) vnf TARGET=clean

vnf_test:
	$(MAKE) -C $(iss_algo_PATH)/vnf/test xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/vnf/test

vnf_test_clean:
	$(MAKE) -C $(iss_algo_PATH)/vnf/test clean
	
	
glbce_test: iss
	$(MAKE) -C $(iss_algo_PATH)/glbce/test xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/glbce/test 

glbce_test_clean:
	$(MAKE) -C $(iss_algo_PATH)/glbce/test clean

wdr_test: iss
	$(MAKE) -C $(iss_algo_PATH)/wdr/test xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/wdr/test 

wdr_test_clean:
	$(MAKE) -C $(iss_algo_PATH)/wdr/test clean
	
mjpege_test: iss
	$(MAKE) -C $(iss_algo_PATH)/jpeg_enc/test xdc_configuro
	$(MAKE) -C $(iss_algo_PATH)/jpeg_enc/test 

mjpege_test_clean:
	$(MAKE) -C $(iss_algo_PATH)/jpeg_enc/test clean



appro:	
	#$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/aewb/appro2a clean $(TARGET)
	$(MAKE) -f Makefile -C $(iss_PATH)/packages/ti/psp/iss/alg/aewb/appro2a $(TARGET)
	#cp -f $(iss_PATH)/build/lib/m3/debug/iss_appro2A.aem3  $(iss_PATH)/packages/ti/psp/iss/alg/aewb/appro2a

clean:
	$(MAKE) iss TARGET=clean
	$(MAKE) examplesclean

examplesclean:
	$(MAKE) chains_clean

examples : iss chains

all: clean examples

platforms:
	$(MAKE) examples PLATFORM=ti814x-evm
	$(MAKE) examples PLATFORM=ti814x-sim

platformsclean:
	$(MAKE) clean PLATFORM=ti814x-evm
	$(MAKE) clean PLATFORM=ti814x-sim

profiles:
	$(MAKE) examples PROFILE_$(CORE)=debug
	$(MAKE) examples PROFILE_$(CORE)=whole_program_debug

profilesclean:
	$(MAKE) clean PROFILE_$(CORE)=debug
	$(MAKE) clean PROFILE_$(CORE)=whole_program_debug

allall:
	$(MAKE) platforms PROFILE_$(CORE)=debug
	$(MAKE) platforms PROFILE_$(CORE)=whole_program_debug

allclean:
#	$(MAKE) platformsclean PROFILE_$(CORE)=debug
#	$(MAKE) platformsclean PROFILE_$(CORE)=whole_program_debug
	$(RM) -rf $(iss_PATH)/build
mosaicDisplay: 
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/display/mosaicDisplay xdc_configuro
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/display/mosaicDisplay
	
mosaicDisplay_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/display/mosaicDisplay clean
i2c: iss
	$(MAKE) -C $(ISS_EXAMPLES)/common/i2c xdc_configuro
	$(MAKE) -C $(ISS_EXAMPLES)/common/i2c

i2c_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/i2c clean

captureVp: iss
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/capture/captureVp xdc_configuro
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/capture/captureVp

captureVp_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/capture/captureVp clean

links: iss
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/chains/links

links_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/chains/links clean

chains: links
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/chains xdc_configuro CONFIG_BLD_XDC_m3=$(iss_PATH)/packages/config_ti814x_largeHeap.bld
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/chains

chains_clean: links_clean
	$(MAKE) -C $(ISS_EXAMPLES)/common/iss/chains clean

proxy: iss
	$(MAKE) -C $(ISS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x xdc_configuro CONFIG_BLD_XDC_m3=$(iss_PATH)/packages/config_c6a814x.bld
	$(MAKE) -C $(ISS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x

proxy_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/proxyServer/hostProxyServerVpsM3/ti814x clean

utils: iss
	$(MAKE) -C $(ISS_EXAMPLES)/common/utils/printDesc xdc_configuro
	$(MAKE) -C $(ISS_EXAMPLES)/common/utils/printDesc

utils_clean:
	$(MAKE) -C $(ISS_EXAMPLES)/common/utils/printDesc clean
