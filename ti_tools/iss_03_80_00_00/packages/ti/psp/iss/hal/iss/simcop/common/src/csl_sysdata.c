/** ==================================================================
 *  @file   csl_sysdata.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ============================================================================ 
 * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005 Use of this
 * software is controlled by the terms and conditions found in the license
 * agreement under which this software has been supplied.
 * =========================================================================== */

/* ---- File: <csl_sysData.c> ---- */

#include "../inc/csl_sysdata.h"
#include "../inc/csl_version.h"
// #include <soc.h>

#pragma DATA_SECTION (CSL_sysDataObj, ".bss:csl_section:sys");

CSL_SysDataObj CSL_sysDataObj = {

    NULL,                                                  /* alternate base
                                                            * address router */
    NULL,                                                  /* physical to
                                                            * virtual address 
                                                            * mapping routine 
                                                            */
    NULL,                                                  /* virtual to
                                                            * physical
                                                            * address mapping 
                                                            * routine */
    CSL_VERSION_ID,                                        /* defined in
                                                            * csl_version.h */
    CSL_CHIP_ID,                                           /* defined in
                                                            * _csl_device.h */
    NULL,                                                  /* pointer to the
                                                            * chip-register
                                                            * overlay */
    0,                                                     /* XIO shared pins 
                                                            * in use mask */

    {0}
    ,                                                      /* CSL peripheral
                                                            * resources
                                                            * in-use
                                                            * mask-matrix */
    {0}
    ,                                                      /* CSL modules'
                                                            * init Done bit
                                                            * flag matrix */

    /* these lines have been intentionally left blank to align with the
     * declaration of the structure in csl_sysData.h */

    0                                                      // , /*
                                                           // intcAllocMask[
                                                           // ] */
        // 0 /* intcEventHandlerRecord[ ] */
};

CSL_SysDataHandle CSL_sysDataHandle = &CSL_sysDataObj;
