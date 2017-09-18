/** ==================================================================
 *  @file   csl_error.h                                                  
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
#ifndef _CSL_ERROR_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_ERROR_H_
/* Below Error codes are Global across all CSL Modules. */
#define CSL_SOK                 (0)                        /* Success */
#define CSL_ESYS_FAIL           (-1)                       /* Generic failure 
                                                            */
#define CSL_ESYS_INUSE          (-2)                       /* Peripheral
                                                            * resource is
                                                            * already in use */
#define CSL_ESYS_XIO            (-3)                       /* Encountered a
                                                            * shared I/O(XIO) 
                                                            * pin conflict */
#define CSL_ESYS_OVFL           (-4)                       /* Encoutered CSL
                                                            * system resource 
                                                            * overflow */
#define CSL_ESYS_BADHANDLE      (-5)                       /* Handle passed
                                                            * to CSL was
                                                            * invalid */
#define CSL_ESYS_INVPARAMS      (-6)                       /* invalid
                                                            * parameters */
#define CSL_ESYS_INVCMD         (-7)                       /* invalid command 
                                                            */
#define CSL_ESYS_INVQUERY       (-8)                       /* invalid query */
#define CSL_ESYS_NOTSUPPORTED   (-9)                       /* action not
                                                            * supported */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
