/** ==================================================================
 *  @file   flicker_corr_priv.h                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/alg/aewb/ti2a/fd/inc/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef __FLICKER_CORR__PRIV_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __FLICKER_CORR_PRIV_H__

#define MAX_FFT_SIZE 256


typedef struct
{
    int32 flicker_block_w;
    int32 flicker_block_h;
    int32 m;
    int32 f50;
    int32 f60;
    int32 resolution;
    uint32 correction;
    uint32 thr;

    //[FLICKER_BLOCK_H][FLICKER_BLOCK_W]
    int32 *rgb_dly_3;
    int32 *rgb_dly_2;
    int32 *rgb_dly_1;
    int32 *rgb_dly_0;

    //[FLICKER_BLOCK_H]
    int32 *first_line_buffer_diff;
    int32 *first_line_buffer_diff_left;
    int32 *first_line_buffer_diff_center;
    int32 *first_line_buffer_diff_right;
    int32 *second_line_buffer_diff;
    int32 *second_line_buffer_diff_left;
    int32 *second_line_buffer_diff_center;
    int32 *second_line_buffer_diff_right;

    //[MAX_FFT_SIZE]
    int32 *x_fft_fix;
    int32 *y_fft_fix;
    uint32 *fft_buffer_diff;
    uint32 *fft_buffer_left;
    uint32 *fft_buffer_center;
    uint32 *fft_buffer_right;

} flicker_detect_st;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
