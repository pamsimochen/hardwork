/** ==================================================================
 *  @file   csl_sysdata.h                                                  
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
/*****************************************************\
 *  Copyright 2003, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
 *                                                   *
 * "@(#) PSP/CSL  3.0.0.0  (2003-09-30)              *
\*****************************************************/

#ifndef _CSL_SYSDATA_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_SYSDATA_H_

#include "../csl.h"
#include "../csl_types.h"

#include "../csl_resid.h"
/* #include "csl_intc.h" */

typedef struct CSL_SysDataObj {

    CSL_AltRouteHandler altRouteHandler;                   /* alternate base
                                                            * address router */
    CSL_Phy2VirtHandler p2vHandler;                        /* physical to
                                                            * virtual address 
                                                            * mapping routine 
                                                            */
    CSL_Virt2PhyHandler v2pHandler;                        /* virtual to
                                                            * physical
                                                            * address mapping 
                                                            * routine */
    Uint32 versionId;                                      /* CSL s/w version 
                                                            * ID */
    Uint32 chipId;                                         /* CSL chip ID */
    void *chipRegs;                                        /* CSL chip
                                                            * Overlay
                                                            * structure, to
                                                            * be initialized
                                                            * in
                                                            * CSL_chipInit() */
    CSL_Xio xioInUse;                                      /* Shared-pin
                                                            * allocation
                                                            * bit-mask */

    CSL_BitMask32 resInUse[(CSL_CHIP_NUM_RESOURCES + 31) / 32]; /* in-use
                                                                 * allocMask */
    CSL_BitMask32 initDone[(CSL_CHIP_NUM_MODULES + 31) / 32];   /* init-done
                                                                 * flags */

    /* Include one data pointer for each CSL module that needs additional
     * context memory here under. The actual memory is claimed during CSL
     * Initialization of that particular CSL module. */

    CSL_BitMask32 *intcAllocMask;
    /* CSL_IntcEventHandlerRecord * intcEventHandlerRecord; */

} CSL_SysDataObj;

typedef CSL_SysDataObj *CSL_SysDataHandle;

extern CSL_SysDataHandle CSL_sysDataHandle;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSL_SYSDATA_H_ 
                                                            */
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 4 Aug 10:09:08 9 2272 xkeshavm */
/* */
/* To upload API CSL 0.51 Release */
/********************************************************************/
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 1 21 Feb 2005 13:08:42 7270 xkeshavm */
/* */
/* the file name is renamed to csl_sysdata.h */
/********************************************************************/
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 1 13 Nov 2005 12:48:07 401 x0029935 */
/* */
/* Modified from davicni CSL files to Dm350 */
/* First CSL files build up fro Dm350 */
/********************************************************************/
