/** ==================================================================
 *  @file   drv_intr.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/common/driver_utils/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef 	CSL_INTR_HEADER
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define 	CSL_INTR_HEADER

/* Interrupt numbers taken from Attila Interrupt Mapping table */
typedef enum {
    CSL_INTR_SOURCE_ID_SDMA0 = 34,   /**< SDMA module 1 Interrupt  */
    CSL_INTR_SOURCE_ID_SDMA1 = 35,   /**< SDMA module 2 Interrupt  */
    CSL_INTR_SOURCE_ID_SDMA2 = 36,   /**< SDMA module 3 Interrupt  */
    CSL_INTR_SOURCE_ID_SDMA3 = 37,   /**< SDMA module 4 Interrupt  */
    CSL_INTR_SOURCE_ID_I2CHS1 = 41,                        // I2C module 1
    CSL_INTR_SOURCE_ID_I2CHS2 = 42,                        // I2C module 2
    CSL_INTR_SOURCE_ID_I2CHS3 = 43,                        // I2C module 3
    CSL_INTR_SOURCE_ID_I2CHS4 = 44,                        // I2C module 4
    CSL_INTR_SOURCE_ID_MPU_GPIO0 = 51,   /**< GPIO module 1 Interrupt  */
    CSL_INTR_SOURCE_ID_MPU_GPIO1 = 52,   /**< GPIO module 2 Interrupt  */
    CSL_INTR_SOURCE_ID_GPT3 = 53,                          // General Purpose 
                                                           // Timer 3 //DH
    CSL_INTR_SOURCE_ID_GPT4 = 54,                          // General Purpose 
                                                           // Timer 4
    CSL_INTR_SOURCE_ID_GPT9 = 55,                          // General Purpose 
                                                           // Timer 9
    CSL_INTR_SOURCE_ID_GPT11 = 56,                         // General Purpose 
                                                           // Timer 11

    CSL_INTR_SOURCE_ID_MAXIMUM = 0xFF
} Intr_SourceId;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* CSL_INTR_HEADER 
                                                            */
