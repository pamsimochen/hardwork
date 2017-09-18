/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Jpeg_test.h
*
* This file contains test wrapper api's for Jpeg Decode on SIMCOP in OMAP4/Monica
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

#include <ti/timmosal/timm_osal_types.h>
#include <ti/iss/framework/msp/msp.h>

typedef struct MSP_TEST_CASE_ENTRY {

    TIMM_OSAL_CHAR *pInputImage;                           /* Test image file 
                                                            * name */

} MSP_TEST_CASE_ENTRY;

MSP_TEST_CASE_ENTRY MSP_JPEGDTestCaseTable[] = {

    /* 01 */ {"JPEGD_0001_320x240_420.jpg"},
    /* 02 */ {"JPEGD_0002_80x60_420.jpg"},
    /* 03 */ {"JPEGD_0003_eagle_1600x1200_420.jpg"},
    /* 04 */ {"JPEGD_0004_market_5120x3200_16MP_420.jpg"},
    /* 05 */ {"JPEGD_0005_5120x3200_16MP_420.jpg"},
    /* 06 */ {"JPEGD_0006_chile_3072x2048_420.jpg"},
    /* 07 */ {"JPEGD_0007_ametlles_1280x1024_420.jpg"},
    /* 08 */ {"JPEGD_0008_flowers_5120x3200_16MP_420.jpg"},
    /* 09 */ {"JPEGD_0009_3072x2304_7MP_420.jpg"},
    /* 10 */ {"JPEGD_0010_5088x3392_16MP_422.jpg"},
    /* 11 */ {"JPEGD_0011_5120x3200_color_16MP_420.jpg"},
    /* 12 */ {"JPEGD_0012_5088x3392_16MP_422.jpg"},
    /* 13 */ {"JPEGD_0013_villa_4096x3072_12MP_420.jpg"},
    /* 14 */ {"JPEGD_0014_shredder_3072x3072_9MP_420.jpg"},
    /* 15 */ {"JPEGD_0015_3072x2048_6MP_420.jpg"},
    /* 16 */ {"JPEGD_0016_fall_3072x2304_7MP_420.jpg"},
    /* 17 */ {"JPEGD_0017_lily_2048x1536_422.jpg"},
    /* 18 */ {"JPEGD_0018_352x288_420.jpg"},
    /* 19 */ {"JPEGD_0019_market_640x480_420.jpg"},
    /* 20 */ {"JPEGD_0021_eagle_800x480_420.jpg"},

    /* 21 */ {"1_128X48_nv12.jpg"},
    /* 22 */ {"2_128X128_nv12.jpg"},
    /* 23 */ {"3_176X144_qf-50_ROT-0_nv12.jpg"},
    /* 24 */ {"4_640x480_nv12.jpg"},
    /* 25 */ {"5_640x480_Market_vga_nv12.jpg"},
    /* 26 */ {"6_640X640_panda_nv12.jpg"},
    /* 27 */ {"7_1024x768_flower_nv12.jpg"},
    /* 28 */ {"8_960x1280_foreman_qf-75_nv12.jpg"},
    /* 29 */ {"9_1280x1024_Ametlles_sxga_nv12.jpg"},
    /* 30 */ {"10_5088x3392_qf-75_rot-90_nv12.jpg"},

    /* 31 */ {"11_128X128_uyvy.jpg"},
    /* 32 */ {"12_512X512_uyvy.jpg"},
    /* 33 */ {"13_2560x2560_lena_qf-75_rot-0_uyvy.jpg"},
    /* 34 */ {"14_5088x3392_qf-75_rot-180_uyvy.jpg"},

    /* 35 */ {"15_176x144_444_ni.jpg"},
    /* 36 */ {"16_176x144_422_ni.jpg"},
    /* 37 */ {"17_176x144_420_ni.jpg"},
    /* 38 */ {"18_16x16_nv12.jpg"},
    /* 39 */ {"19_357x292_nv12.jpg"},
    /* 40 */ {"20_177x141_nv12.jpg"},
    /* 41 */ {"21_177x144_uyvy.jpg"},
    /* 42 */ {"lenna_512x512_nv12.jpg"},
    /* 43 */ {"mandril_256x256_nv12.jpg"},
    /* 44 */ {"lenka_6144x4096_24MP_nv12.jpg"},
    /* 45 */ {"old_5000x6694_32MP_420.jpg"}

};

#define INPUT_PATH "..\\..\\..\\..\\..\\alg\\jpeg_dec\\test\\test_vectors\\input\\"
#define OUTPUT_PATH "..\\..\\..\\..\\..\\alg\\jpeg_dec\\test\\test_vectors\\output\\"

/* ===================================================================
 *  @func     msp_jpegd_dummy_callback                                               
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
MSP_ERROR_TYPE msp_jpegd_dummy_callback(MSP_PTR hMSP,
                                        MSP_PTR pAppData,
                                        MSP_EVENT_TYPE tEvent,
                                        MSP_OPAQUE nEventData1,
                                        MSP_OPAQUE nEventData2);

/* ===================================================================
 *  @func     rearrange                                               
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
void rearrange(Uint16 nMcus, unsigned char *data_in, unsigned char *dataout,
               Uint16 * iwt, Uint16 * iht);

/* ===================================================================
 *  @func     Jpeg_test_dec                                               
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
void Jpeg_test_dec(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
