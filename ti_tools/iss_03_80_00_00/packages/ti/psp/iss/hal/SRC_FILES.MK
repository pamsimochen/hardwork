
SRCDIR +=	hal/iss/iss_common/src \
			hal/iss/csi2/src \
			hal/iss/ccp2/src	\
			hal/iss/isp/ipipe/src \
			hal/iss/isp/ipipe_if/src \
			hal/iss/isp/isif/src \
			hal/iss/isp/rsz/src \
			hal/iss/simcop/common/src \
			hal/iss/isp/h3a/src \
			hal/iss/isp/common/src \
			hal/iss/common/csl_utils \
			hal/iss/cbuff/src	\
			hal/iss/iss_common/src \
			hal/iss/bte/src \
			hal/iss/simcop/common \
			hal/iss/simcop/vlcdj/src \
			hal/iss/simcop/dct/src \
			hal/iss/simcop/rot/src \
			hal/iss/simcop/simcop_global/src \
			hal/iss/simcop/dma/src \
			hal/iss/simcop/imx/src \
			hal/iss/simcop/ldc/src \
			hal/iss/simcop/nsf/src \
			hal/fd/src							
	

INCDIR += . hal/iss/isp \
			hal/iss/bte/inc   \
			hal/iss/cbuff/inc \
			hal/iss/cbuff \
			hal/iss/ccp2 \
			hal/iss/ccp2/inc \
			hal/iss/common/csl_utils \
			hal/iss/csi2/inc \
			hal/iss/csi2 \
			hal/iss/isp/common/inc \
			hal/iss/isp/h3a/inc \
			hal/iss/isp/ipipe/inc \
			hal/iss/isp/ipipe_if/inc \
			hal/iss/isp/isif/inc \
			hal/iss/isp/rsz/inc \
			hal/iss/isp \
			hal/iss/iss_common \
			hal/iss/iss_common/inc \
			hal/iss/simcop/common/inc \
			hal/iss/simcop/dct/inc \
			hal/iss/simcop/dma/inc \
			hal/iss/simcop/ldc/inc \
			hal/iss/simcop/ldc \
			hal/iss/simcop/nsf/inc \
			hal/iss/simcop/rot/inc \
			hal/iss/simcop/simcop_global/inc \
			hal/iss/simcop/vlcdj/inc \
			hal/iss/simcop/imx/inc \
			hal/iss/simcop/ldc/inc \
			hal/iss/simcop/nsf/inc \
			hal/iss/test/simcop/test_algo/jpeg_dec/jpeg_decode/inc \
			hal/iss/test/simcop/test_algo/jpeg_dec/jpeg_test/inc \
			hal/fd/inc \
			hal/fd
						

SRCS_COMMON += bte.c cbuff.c cbuff_interrupts.c ccp2.c csl_utils.c csi2.c csi2_irq.c isp_common.c iss_common.c h3a.c ipipe.c ipipe_if.c isif.c rsz.c simcop_irq_mgr.c simcop_dma_irq_mgr.c csl_sysdata.c csl_resource.c csl_fd.c _csl_fd.c jpeg_tables.c csl_vlcd.c csl_dct.c csl_rot.c csl_simcop.c csl_simcop_dma.c _csl_vlcd.c _csl_dct.c _csl_rot.c _csl_simcop_dma.c _csl_simcop.c csl_imx4.c _csl_imx4.c csl_ldc.c _csl_ldc.c csl_nsf.c _csl_nsf.c
