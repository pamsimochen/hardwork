#ifndef BUFFMAP_H_
#define BUFFMAP_H_
/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/**
********************************************************************************
 * @file <buffer_map.h>
 *
 * @brief This File contains all structure definitions and function prototypes
 *        for handling buffer management
 *
 *******************************************************************************
*/

/**
 * Select Raw/Tiled mode here
*/
//#define TILED_MODE
#define RAW_MODE

/**
 * Macro for selecting record/playback
*/
#define VIDEO_RECORD
//#define VIDEO_PLAYBACK
//#define HS_CAPTURE

/******************************************************************************/
// Video Record Usecase
/******************************************************************************/

#ifdef VIDEO_RECORD
#ifdef TILED_MODE
// Tiled mode
#define LDC_VNF_PING_LUMA   0x61C00000         //1920 x 1088
#define LDC_VNF_PONG_LUMA   0x61C00800         //1920 x 1088
#define LDC_VNF_PING_CROMA  0x62E00000         //1920 x 544
#define LDC_VNF_PONG_CROMA  0x63700000         //1920 x 544
#define IVAHD_INPUT_1_LUMA  0x61C01000         //1920 x 1088
#define IVAHD_INPUT_2_LUMA  0x61C01800         //1920 x 1088
#define IVAHD_INPUT_3_LUMA  0x61C02000         //2048 x 1152
#define IVAHD_INPUT_1_CROMA 0x62E00800         //1920 x 544
#define IVAHD_INPUT_2_CROMA 0x63700800         //1920 x 544
#define IVAHD_INPUT_3_CROMA 0x62E01000         //1920 x 544
#define RECON_1_LUMA        0x61C02800         //2048 x 1152
#define RECON_2_LUMA        0x61C03000         //2048 x 1152
#define RECON_3_LUMA        0x61C03800         //2048 x 1152
#define RECON_1_CROMA       0x63701000         //2048 x 576
#define RECON_2_CROMA       0x62E01800         //2048 x 576
#define RECON_3_CROMA       0x63701800         //2048 x 576
#define OVERLAY_BUF         0x72E04000         //3840 x 540
#define PREVIEW_PING        0x72E05000         //3440 x 480
#define PREVIEW_PONG        0x72E06000         //3440 x 480 


/**
 * Use index to address table given by DMM Init function
*/
#define LDC_VNF_PING_LUMA_IDX     0
#define LDC_VNF_PING_CROMA_IDX    1

#define LDC_VNF_PONG_LUMA_IDX     2
#define LDC_VNF_PONG_CROMA_IDX    3

#define IVAHD_INPUT_1_LUMA_IDX    4
#define IVAHD_INPUT_1_CROMA_IDX   5

#define IVAHD_INPUT_2_LUMA_IDX    6
#define IVAHD_INPUT_2_CROMA_IDX   7

#define IVAHD_INPUT_3_LUMA_IDX    8
#define IVAHD_INPUT_3_CROMA_IDX   9


/**
 * Raw or paged mode
*/
#define CSIR_BUF               0x80000000         //4368 x 800  (1bpp)
#define RESIZER_FULL_PING      0x80355200         //2304 x 1306 (2bpp)
#define RESIZER_FULL_PONG      0x80912600         //2304 x 1306 (2bpp)
#define RESIZER_FD_LUMA_PING   0x80ECFA00         //384  x 288  (1bpp)
#define RESIZER_FD_CROMA_PING  0x80EEAA00         //384  x 144  (1bpp)
#define RESIZER_FD_LUMA_PONG   0x80EF8200         //384  x 288  (1bpp)
#define RESIZER_FD_CROMA_PONG  0x80F13200         //384  x 144  (1bpp)
#define BITSTREAM_BUF_PING     0x80F20A00         //1MB
#define BITSTREAM_BUF_PONG     0x81020A00         //1MB 

#else

/**
 * Raw page mode
*/
#define LDC_VNF_PING_LUMA     0x81C00000         //1920 x 1088
#define LDC_VNF_PONG_LUMA     0x81DFE000         //1920 x 1088
#define LDC_VNF_PING_CROMA    0x81FFC000         //1920 x 544
#define LDC_VNF_PONG_CROMA    0x820FB000         //1920 x 544
#define IVAHD_INPUT_1_LUMA    0x821FA000         //1920 x 1088
#define IVAHD_INPUT_2_LUMA    0x823F8000         //1920 x 1088
#define IVAHD_INPUT_3_LUMA    0x825F6000         //2048 x 1152
#define IVAHD_INPUT_1_CROMA   0x82836000         //1920 x 544
#define IVAHD_INPUT_2_CROMA   0x82935000         //1920 x 544
#define IVAHD_INPUT_3_CROMA   0x82A34000         //1920 x 544
#define RECON_1_LUMA          0x82B33000         //2048 x 1152
#define RECON_2_LUMA          0x82D73000         //2048 x 1152
#define RECON_3_LUMA          0x82FB3000         //2048 x 1152
#define RECON_1_CROMA         0x831F3000         //2048 x 576
#define RECON_2_CROMA         0x83313000         //2048 x 576
#define RECON_3_CROMA         0x83433000         //2048 x 576
#define OVERLAY_BUF           0x83553000         //3840 x 540
#define PREVIEW_PING          0x8374D400         //3440 x 480
#define PREVIEW_PONG          0x838E0600         //3440 x 480 

/**
 * Raw or page mode
*/
#define CSIR_BUF              0x80000000         //4368 x 800  (1bpp)
#define RESIZER_FULL_PING     0x80355200         //2304 x 1306 (2bpp)
#define RESIZER_FULL_PONG     0x80912600         //2304 x 1306 (2bpp)
#define RESIZER_FD_LUMA_PING  0x80ECFA00         //384  x 288  (1bpp)
#define RESIZER_FD_CROMA_PING 0x80EEAA00         //384  x 144  (1bpp)
#define RESIZER_FD_LUMA_PONG  0x80EF8200         //384  x 288  (1bpp)
#define RESIZER_FD_CROMA_PONG 0x80F13200         //384  x 144  (1bpp)
#define BITSTREAM_BUF_PING    0x80F20A00         //1MB
#define BITSTREAM_BUF_PONG    0x81020A00         //1MB 
#endif
#endif

#endif
