/** ==================================================================
 *  @file   globalDefs.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/tvs_mtis_est/test/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                           DSPS R&D Center
 *                     Video and Image Processing
 *         
 *         Copyright (c) 2004 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *      
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *                    TI INTERNAL - TI PROPRIETARY
 *
 *
 *  Contact: Aziz Umit Batur     <batur@ti.com>
 *
 *=======================================================================
 *
 *  File: globalDefs.h
 *  
 *=======================================================================
 *
 *  Revision 4.5 (2-April-2009)
 *
 =======================================================================*/

#ifndef _GLOBAL_DEFS_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _GLOBAL_DEFS_INCLUDED_

// DEFINE THE
// PLATFORM-------------------------------------------------------------------

// #define IS_WINDOWS //Windows platform 
// #define NO_PRINTF //disables all printf statements
// #define WRITE_DEBUG //enables saving debug files

/* 
 * #ifdef IS_WINDOWS #include <windows.h> #include <io.h> #endif */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// VARIOUS I/O
// OPTIONS-------------------------------------------------------------------

#define FILE_IO                 0x00000001
#define WIN_IO                  0x00000002

#define INTERLACED              0x00000008

#define GRAYSCALE               0x00000010
#define YUV420                  0x00000020

#define DISP_IN_ORG_FRAME       0x00000040

#define FINAL_DISP              0x00000080

// DEFAULTS FOR I/O
// OPTIONS--------------------------------------------------------------

#define DEF_INPUT_TYPE          (FILE_IO | YUV420)
// #define DEF_OUTPUT_TYPE (FILE_IO | GRAYSCALE)// | FINAL_DISP)// |
// DISP_IN_ORG_FRAME)
// #define DEF_INPUT_TYPE (FILE_IO | GRAYSCALE)// | INTERLACED)
// #define DEF_OUTPUT_TYPE (WIN_IO | GRAYSCALE | FINAL_DISP |
// DISP_IN_ORG_FRAME) 
#define DEF_OUTPUT_TYPE         (WIN_IO | FILE_IO | YUV420 | FINAL_DISP)    // | 
                                                                            // DISP_IN_ORG_FRAME)
// #define DEF_OUTPUT_TYPE (WIN_IO | YUV420 | FINAL_DISP)// |
// DISP_IN_ORG_FRAME) 
// #define DEF_OUTPUT_TYPE (WIN_IO | FILE_IO | YUV420 | FINAL_DISP)// |
// DISP_IN_ORG_FRAME) 
// #define DEF_OUTPUT_TYPE (YUV420 | FINAL_DISP)// | DISP_IN_ORG_FRAME) 
// #define DEF_OUTPUT_TYPE (WIN_IO | YUV420)// | FINAL_DISP)// |
// DISP_IN_ORG_FRAME)
// #define DEF_OUTPUT_TYPE (YUV420)// | FINAL_DISP)// | DISP_IN_ORG_FRAME)

#define DEF_FRAME_RATE          25                         // Frame rate for
                                                           // win display
#define DEF_HEADER_LENGTH       0                          // Header length
                                                           // for the input
                                                           // sequence file
#define MAX_NUM_OF_INPUT_ARGS   20                         // Maximum number
                                                           // of command line 
                                                           // input arguments

// DATA TYPE
// DEFINITIONS-----------------------------------------------------------------

typedef unsigned char Byte;

typedef short Word16;

typedef unsigned short uWord16;

typedef int Word32;

typedef unsigned int uWord32;

// typedef long Word32;

// DEFAULT VALUES FOR SOME VS INPUT
// PARAMETERS-------------------------------------------

// #define DEF_FRAME_VSIZE 576 
// #define DEF_FRAME_HSIZE 768 
// #define DEF_FRAME_VSIZE 480 
// #define DEF_FRAME_HSIZE 640 
// #define DEF_FRAME_VSIZE 240 
// #define DEF_FRAME_HSIZE 320 
// #define DEF_FRAME_VSIZE 144 
// #define DEF_FRAME_HSIZE 176 
// #define DEF_FRAME_VSIZE 288 
// #define DEF_FRAME_HSIZE 352 
// #define DEF_FRAME_VSIZE 120 
// #define DEF_FRAME_HSIZE 160 
// #define DEF_FRAME_VSIZE 960 
// #define DEF_FRAME_HSIZE 1280 
// #define DEF_FRAME_VSIZE 1200 
// #define DEF_FRAME_HSIZE 1600 
#define DEF_FRAME_VSIZE                 1200
#define DEF_FRAME_HSIZE                 2048

#define DEF_BOUN_LINES                  240
#define DEF_BOUN_PELS                   512
// #define DEF_BOUN_LINES 48 
// #define DEF_BOUN_PELS 64 
// #define DEF_BOUN_LINES 24 
// #define DEF_BOUN_PELS 32 
// #define DEF_BOUN_LINES 120 
// #define DEF_BOUN_PELS 160 
// #define DEF_BOUN_LINES 28 
// #define DEF_BOUN_PELS 36 

#define DEF_DISPLAY_RESOLUTION          0                  // 0->pel,
                                                           // 1->half-pel
#define DEF_INT_SCRATCH_MEMORY_SIZE		10000
#define DEF_INT_PERSISTENT_MEMORY_SIZE	300
#define DEF_EXT_DATA_MEMORY_SIZE    	15000

// #define DEF_KMIN_H (uWord16) 27852//(0.85*(1<<15))
// #define DEF_KMIN_V (uWord16) 27852//(0.85*(1<<15))
#define DEF_KMIN_H                      (uWord16) 29846    // (0.85*(1<<15))
#define DEF_KMIN_V                      (uWord16) 29846    // (0.85*(1<<15))
#define DEF_KMAX_H                      (uWord16) (1*(1<<15))
#define DEF_KMAX_V                      (uWord16) (1*(1<<15))

#define DEF_MAX_SEARCH_AMP_V            -1                 // Negative value
                                                           // is ignored
#define DEF_MAX_SEARCH_AMP_H            -1                 // Negative value
                                                           // is ignored
#define DEF_MAX_MOTION_COMP_AMP_V       -1                 // Negative value
                                                           // is ignored
#define DEF_MAX_MOTION_COMP_AMP_H       -1                 // Negative value
                                                           // is ignored
#define	DEF_SEARCH_COMPLEXITY           -1                 // Negative value
                                                           // is ignored
#define DEF_MINIMUM_RELIABILITY         -1                 // Negative value
                                                           // is ignored
#define DEF_ME_RESOLUTION               0                  // 0->pel,
                                                           // 1->half-pel

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
