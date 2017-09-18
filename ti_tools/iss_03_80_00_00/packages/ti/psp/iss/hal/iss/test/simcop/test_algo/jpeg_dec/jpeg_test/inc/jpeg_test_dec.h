/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Jpeg_test.h
*
* This file contains test wrapper api's for Jpeg Encode on SIMCOP in OMAP4/Monica
*
* @path code\test
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 23-Sep 2008 Sowmya Priya: Initial Release
*!
*========================================================================= */

#ifndef _JPEG_TEST_DEC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _JPEG_TEST_DEC_H

#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// #include "jpeg_dec.h"
#include "jpeg_tables.h"
#include <tistdtypes.h>

#define TESTCASE 8

#define HMEM_WIDTH 8
#define HMEM_HEIGHT 256
#define QMEM_WIDTH 8
#define QMEM_HEIGHT 32
#define QMEM_SIZE      256                                 /* 512 bytes = 256 
                                                            * short */
#define HMEM_SIZE 4096
#if(0==TESTCASE)
#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 48
#define INPUT_IMAGE  "128X48.nv12_out70.jpg"
#elif(1==TESTCASE)
#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128
#define INPUT_IMAGE  "128X128_nv12.jpg"
#elif(2==TESTCASE)
#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 640
#define INPUT_IMAGE  "640X640.nv12_out90.jpg"
#elif(3==TESTCASE)
#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128
#define INPUT_IMAGE  "128X128_uyvy.jpg"
#elif(4==TESTCASE)
#define IMAGE_WIDTH  512
#define IMAGE_HEIGHT 512
#define INPUT_IMAGE  "512X512.uyvy_qf-70_out.jpg"
#elif(5==TESTCASE)
#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define INPUT_IMAGE  "vga.nv12_qf-70_out.jpg"
#elif(6==TESTCASE)
#define IMAGE_WIDTH  144
#define IMAGE_HEIGHT 176
#define INPUT_IMAGE  "176X144.nv12_qf-70_ROT-90_HSS_out.jpg"
#elif(7==TESTCASE)
#define IMAGE_WIDTH  2592
#define IMAGE_HEIGHT 1760
#define INPUT_IMAGE  "2592X1760.jpg"
#elif(8==TESTCASE)
#define IMAGE_WIDTH  176
#define IMAGE_HEIGHT 144
#define INPUT_IMAGE  "qcif_420_ni.jpg"
#elif(9==TESTCASE)
#define IMAGE_WIDTH  3392
#define IMAGE_HEIGHT 5088
#define INPUT_IMAGE  "3392X5088.jpg"
#endif

#define DCDTBL0OFFST 60
#define DCDTBL1OFFST 74
#define DCDTBL2OFFST 256
#define DCDTBL3OFFST 270

#define INPUT_PATH "..\\..\\test_vectors\\input\\"
#define OUTPUT_PATH "..\\..\\test_vectors\\output\\"

#define JPEG_BUFFER_SIZE MAX_OUTPUT_SIZE
#define MAX_OUTPUT_SIZE (BITSTRBUF_BANK_SIZE * SIMCOP_DMA_MAX_XCNT *9)
#define MAX_INPUT_SIZE 44000000

#define INPUT_SIZE_YUV422 (IMAGE_WIDTH *IMAGE_HEIGHT*2)
#define INPUT_SIZE_YUV420 (IMAGE_WIDTH *IMAGE_HEIGHT*3/2)

typedef struct {

    unsigned char qmat[128];
    unsigned short *ctl_tbl;
    unsigned short dc_y_dcd_size;
    unsigned short ac_y_dcd_size;
    unsigned short dc_uv_dcd_size;
    unsigned short ac_uv_dcd_size;
    unsigned short *dc_y_dcd_tbl;
    unsigned short *ac_y_dcd_tbl;
    unsigned short *dc_uv_dcd_tbl;
    unsigned short *ac_uv_dcd_tbl;
    unsigned char scan_header[20];

} DecTblParams;

/* ===================================================================
 *  @func     parse_header                                               
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
void parse_header(FILE * fp, short *iw, short *ih, short *yuv_mode,
                  DecTblParams * DecTblParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
