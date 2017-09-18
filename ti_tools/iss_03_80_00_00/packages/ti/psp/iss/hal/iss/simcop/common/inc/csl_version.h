/** ==================================================================
 *  @file   csl_version.h                                                  
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
 *  Copyright 2004, Texas Instruments Incorporated.  *
 *  All rights reserved.                             *
 *  Restricted rights to use, duplicate or disclose  *
 *  this   code   are  granted   through  contract.  *
\*****************************************************/

/* @(#) PSP/CSL 3.00.02.01[DAVINCIPG1_0] (2004-04-25) */

#ifndef _CSL_VERSION_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_VERSION_H_

#define CSL_VERSION_ID          (0x03000201)               /* 0xAABBCCDD ->
                                                            * Arch (AA); API
                                                            * Changes (BB);
                                                            * Major (CC);
                                                            * Minor (DD) */
#define CSL_VERSION_STR         "@# CSL Revision: 3.00.02.01;"

extern void CSL_version3_00_02_01();

#define CSL_CHIP_ID             (0x300)
#define CSL_CHIP_STR            "Chip: DAVINCI, PG 1.0"

extern void CSL_chipDavinciPG1_0(void);

/* ===================================================================
 *  @func     CSL_versionCheck                                               
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
static void CSL_versionCheck(void)
{
    CSL_version3_00_02_01();
    CSL_chipDavinciPG1_0();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSL_VERSION_H_ 
                                                            */

/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 7 14 Jan 2005 13:32:25 5888 xkeshavm */
/* */
/* Uploaded the CSL0.57 JAN 2005 Release and built the library for ARM and
 * DSP */
/********************************************************************/
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 1 13 Nov 2005 12:48:08 401 x0029935 */
/* */
/* Modified from davicni CSL files to Dm350 */
/* First CSL files build up fro Dm350 */
/********************************************************************/
