/** ==================================================================
 *  @file   csl.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/                                                  
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
 \*****************************************************/

#ifndef _CSL_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_H_

#include "csl_types.h"
#include "csl_resid.h"
#include "csl_error.h"
#include "cslr.h"

typedef void *CSL_ResHandle;

typedef struct {
    CSL_OpenMode openMode;
    CSL_Uid uid;
    CSL_Xio xio;
} CSL_ResAttrs;

typedef void (*CSL_Phy2VirtHandler) (CSL_ResHandle);

typedef void (*CSL_Virt2PhyHandler) (CSL_ResHandle);

typedef void (*CSL_AltRouteHandler) (CSL_ResHandle);

/* ===================================================================
 *  @func     CSL_sysInit                                               
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
void CSL_sysInit(void
    );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSL_H_ */

/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 4 Aug 10:08:55 9 2272 xkeshavm */
/* */
/* To upload API CSL 0.51 Release */
/********************************************************************/
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 1 13 Nov 2005 12:47:38 401 x0029935 */
/* */
/* Modified from davicni CSL files to Dm350 */
/* First CSL files build up fro Dm350 */
/********************************************************************/
