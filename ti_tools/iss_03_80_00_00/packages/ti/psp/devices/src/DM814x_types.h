/** ==================================================================
 *  @file   DM814x_types.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
// << PUT CORRECT COPYRIGHT NOTICE HERE >>

/**
 * \file    DM814x_types.h
 *
 * \brief   Defines the statndard types for CCS sample code
 *
 * This header file contains the standard typedefs and macros
 * for use with CCS test code for Centaurur EVM.
 *
 * \author  0043
 *
 * \version 01a,09aug,2010 Created.
 */

#ifndef _DM814x_types_H_
#define _DM814x_types_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* 
     *====================
     * Includes
     *====================
     */

/**
@addtogroup DM814x Types
@{
*/

    /* 
     *====================
     * Defines
     *====================
     */
#define SUCCESS		0
#define FAILED		-1

#define SW_BREAKPOINT       asm( " .long 0xe1200070" );

#ifndef NULL
#define NULL 0
#endif

    /* 
     *====================
     * Typedefs
     *====================
     */
    typedef unsigned char UINT8;
    typedef unsigned short UINT16;
    typedef unsigned int UINT32;
    typedef char INT8;
    typedef short INT16;
    typedef int INT32;
    typedef int STATUS;

    /* @} *//* End of DM814x Types */

#ifdef __cplusplus
}                                                          /* End of extern C 
                                                            */
#endif                                                     /* #ifdef
                                                            * __cplusplus */
#endif                                                     /* End of
                                                            * _DM814x_types_H_ 
                                                            */
