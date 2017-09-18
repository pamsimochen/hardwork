/** ==================================================================
 *  @file   iss_mem_map.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*******************************************
 * Memory mapping of ISS
 * Spec Version 1.11
 *******************************************/
#ifndef _ISS_MEM_MAP_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_MEM_MAP_H

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

#define ISS_BASE_ADDRESS ISS_REGS_BASE_ADDR                // 0x50000000

/******************* ISS Address Mapping **********************/

// ISS MEMORY
#define ISS_START_ADDRESS (ISS_BASE_ADDRESS + 0x00000000)
#define ISS_END_ADDRESS (ISS_BASE_ADDRESS + 0x000000FF)

/******************* SIMCOP Address Mapping  *****************/
// SIMCOP Module Area
#define SIMCOP_START_ADDRESS (ISS_BASE_ADDRESS + 0x00020000)
#define SIMCOP_END_ADDRESS   (ISS_BASE_ADDRESS + 0x0003FFFF)

// SIMCOPDMA Area
#define SIMCOPDMA_START_ADDRESS (SIMCOP_START_ADDRESS + 0x00000200)
#define SIMCOPDMA_END_ADDRESS   (SIMCOP_START_ADDRESS + 0x000003FF)

// DCT Area
#define DCT_START_ADDRESS (SIMCOP_START_ADDRESS + 0x00000800)
#define DCT_END_ADDRESS (SIMCOP_START_ADDRESS + 0x0000081F)

// VLCDJ Areaa
#define VLCDJ_START_ADDRESS (SIMCOP_START_ADDRESS + 0x00000600)
#define VLCDJ_END_ADDRESS (SIMCOP_START_ADDRESS + 0x0000067F)

// ROT Area
#define ROT_START_ADDRESS (SIMCOP_START_ADDRESS + 0x00000700)
#define ROT_END_ADDRESS (SIMCOP_START_ADDRESS+ 0x0000073F)

// iMX Area
#define iMXa_START_ADDRESS (SIMCOP_START_ADDRESS + 0x00000180)
#define iMXa_END_ADDRESS (SIMCOP_START_ADDRESS + 0x000001BF)
/******************* ISP-SYS1 Address Mapping  ***************/

// ISPmodule Sys1 Area 
#define ISP_SYS1_START_ADDRESS (ISS_BASE_ADDRESS + 0x00010000)
#define ISP_SYS1_END_ADDRESS (ISS_BASE_ADDRESS + 0x0001FFFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _ISS_MEM_MAP_H
