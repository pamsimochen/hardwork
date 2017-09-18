/** ==================================================================
 *  @file   issutils_mem_default.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/iss/iss.h>
#include <ti/psp/platforms/utility/issutils_mem.h>

/** \brief Maximum frame buffer memory pool size */
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define ISSUTILS_MEM_FRAME_HEAP_SIZE    (123*MB)
#endif

/* Memory pool */
#pragma DATA_ALIGN(gIssUtils_heapMemFrame, ISS_BUFFER_ALIGNMENT)
#pragma DATA_SECTION(gIssUtils_heapMemFrame,".bss:frameBuffer");
UInt32 gIssUtils_heapMemFrame[ISSUTILS_MEM_FRAME_HEAP_SIZE / sizeof(UInt32)];

/* ===================================================================
 *  @func     IssUtils_memInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_memInit()
{
    IssUtils_memInit_internal(gIssUtils_heapMemFrame,
                              sizeof(gIssUtils_heapMemFrame));

    return 0;
}
