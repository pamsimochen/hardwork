SRCDIR += drivers/src drivers/capture/src drivers/alg/vstab/src drivers/alg/2A/src
INCDIR += drivers/capture/ drivers/alg/vstab/ drivers/alg/2A/inc
SRCS_COMMON += iss_init.c issdrv_captureApi.c issdrv_algVstabApi.c issdrv_alg2AApi.c issdrv_algTIaewb.c issdrv_captureHal.c \
               issdrv_algTIaewbTables.c issdrv_algTIawbCalcData.c issdrv_ispApi.c issdrv_algAPPROaewbCalcData.c

