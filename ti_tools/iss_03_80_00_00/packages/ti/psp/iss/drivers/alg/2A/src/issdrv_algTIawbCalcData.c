/** ==================================================================
 *  @file   issdrv_algTIawbCalcData.c                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/drivers/alg/2A/src/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include <alg_ti_aewb_priv.h>

#ifdef IMGS_MICRON_AR0331
#include "issdrv_algTIawbCalcData_AR0331.c"
#endif
#ifdef IMGS_PANASONIC_MN34041
#include "issdrv_algTIawbCalcData_MN34041.c"
#endif
#ifdef IMGS_SONY_IMX035
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_OMNIVISION_OV2715
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_SONY_IMX036
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_OMNIVISION_OV9712
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_OMNIVISION_OV10630
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_MT9P031
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_MT9D131
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_MT9M034
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_TAXAS_TVP514X
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_MT9J003
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_AR0330
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
#include "AWB_ConfHardware_9m034_dual.txt"
#endif
#ifdef IMGS_SONY_IMX136
#include "issdrv_algTIawbCalcData_IMX136.c"
#endif
#ifdef IMGS_SONY_IMX140
#include "issdrv_algTIawbCalcData_IMX140.c"
#endif
#ifdef IMGS_SONY_IMX104
#include "issdrv_algTIawbCalcData_IMX104.c"
#endif
#ifdef IMGS_OMNIVISION_OV7740
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_OMNIVISION_OV2710
#include "issdrv_algTIawbCalcData_default.c"
#endif
#ifdef IMGS_SONY_IMX122
#include "issdrv_algTIawbCalcData_default.c"
#endif
