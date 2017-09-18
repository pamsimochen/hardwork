/** ==================================================================
 *  @file   iss_drv_common.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*******************************************************************
 *
 *    DESCRIPTION:
 *
 *    AUTHOR:
 *
 *    HISTORY:
 *
 *******************************************************************/
#ifndef _ISS_DRV_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _ISS_DRV_COMMON_H_

/** include files **/

#include <ti/psp/iss/hal/iss/isp/isp.h>

#ifndef ISS_SIMULATOR
#define TARGET_ZEBU
#endif

/** public data **/
/* AEWB SCM Range Constants */
#define MAX_SATURATION_LIM  1023
#define MIN_WIN_H           2
#define MAX_WIN_H           256
#define MIN_WIN_W           6
#define MAX_WIN_W           256
#define MAX_WINVC           32                             // 128
#define MAX_WINHC           16                             // 36
#define MAX_WINSTART        4095
#define MIN_SUB_INC         2
#define MAX_SUB_INC         32

#define MAX_FRAME_COUNT         0x0FFF
#define MAX_FUTURE_FRAMES   10

// One paxel is 32 Bytes + on every 8 paxel 8*2 Bytes for number of
// unsaturated pixels in previouse 8 paxels
#define AWWB_H3A_PAXEL_SIZE_BYTES   (32 + 2)
#define AEWB_H3A_MAX_BUFF_SIZE      (MAX_WINHC * MAX_WINVC * AWWB_H3A_PAXEL_SIZE_BYTES)

/* AF SCM Range Constants */
#define AF_IIRSH_MIN					0
#define AF_IIRSH_MAX					4094
#define AF_PAXEL_HORIZONTAL_COUNT_MIN	0
#define AF_PAXEL_HORIZONTAL_COUNT_MAX	35
#define AF_PAXEL_VERTICAL_COUNT_MIN		0
#define AF_PAXEL_VERTICAL_COUNT_MAX		127
#define AF_PAXEL_INCREMENT_MIN			0
#define AF_PAXEL_INCREMENT_MAX			14
#define AF_PAXEL_HEIGHT_MIN				0
#define AF_PAXEL_HEIGHT_MAX				127
#define AF_PAXEL_WIDTH_MIN				0
#define AF_PAXEL_WIDTH_MAX				127
#define AF_PAXEL_HZSTART_MIN			2
#define AF_PAXEL_HZSTART_MAX		4094

#define AF_PAXEL_VTSTART_MIN		0
#define AF_PAXEL_VTSTART_MAX		4095
#define AF_THRESHOLD_MAX		255
#define AF_COEF_MAX			4095
#define AF_PAXEL_SIZE			48
#define AF_NUMBER_OF_COEF       11

// Size of one paxel is 48 Bytes
#define AF_H3A_MAX_BUFF_SIZE    (AF_PAXEL_VERTICAL_COUNT_MAX * AF_PAXEL_HORIZONTAL_COUNT_MAX * 48)

typedef enum {
    ISP_H3A_STOPPED = 0,
    ISP_H3A_AEWB_ENABLED = (1 << 0),
    ISP_H3A_AEWB_BUSY = (1 << 1),
    ISP_H3A_AF_ENABLED = (1 << 2),
    ISP_H3A_AF_BUSY = (1 << 3)
} ISP_H3A_STATES_T;

typedef enum {
    IPIPE_DMA_DPC_1,                                       /* ISP5_DMA_REQ[2] */
    IPIPE_DMA_LAST_PIX,                                    /* ISP5_DMA_REQ[3] */
    IPIPE_DMA_DPC_0,                                       /* ISP5_DMA_REQ[2] */
    IPIPE_DMA_HIST,                                        /* ISP5_DMA_REQ[1] */
    IPIPE_DMA_BSC                                          /* ISP5_DMA_REQ[0] */
} IPIPE_DMA_REQ_T;

typedef enum {
    IPIPE_IRQ_RSZ,
    IPIPE_IRQ_ISIF,
    IPIPE_IRQ_H3A,
    IPIPE_IRQ_BSC,
    IPIPE_IRQ_VD,
    IPIPE_IRQ_2DLSC,
    IPIPE_IRQ_HIST
} IPIPE_IRQ_ID;

typedef enum {
    IPIPE_YUV_RANGE_FULL,
    IPIPE_YUV_RANGE_RESTRICTED
} IPIPE_YUV_RANGE;

typedef enum {
    IPIPE_OUT_FORMAT_UNUSED,
    IPIPE_OUT_FORMAT_YUV422,
    IPIPE_OUT_FORMAT_YUV420,
    IPIPE_OUT_FORMAT_RGB888,
    IPIPE_OUT_FORMAT_RGB565,
    IPIPE_OUT_FORMAT_BAYER
} IPIPE_OUT_FORMAT;

typedef enum {
    IPIPE_IN_COMP_Y,
    IPIPE_IN_COMP_C
} IPIPE_IN_YUV420_COMP_YC;

typedef enum {
    IPIPE_IN_FORMAT_BAYER,
    IPIPE_IN_FORMAT_YUV422,
    IPIPE_IN_FORMAT_YUV420
} IPIPE_IN_FORMAT;

typedef enum {
    IPIE_BAYER_PATTERN_RGGB,
    IPIE_BAYER_PATTERN_GRBG,
    IPIE_BAYER_PATTERN_GBRG,
    IPIE_BAYER_PATTERN_BGGR
} IPIPE_BAYER_PATTERN;

typedef enum {
    IPIPE_VP_DEV_CSIA,
    IPIPE_VP_DEV_CSIB,
    IPIPE_VP_DEV_CCP,
    IPIPE_VP_DEV_PI
} IPIPE_VP_DEVICE;

typedef enum {
    IPIPE_BAYER_MSB_BIT15,
    IPIPE_BAYER_MSB_BIT14,
    IPIPE_BAYER_MSB_BIT13,
    IPIPE_BAYER_MSB_BIT12,
    IPIPE_BAYER_MSB_BIT11,
    IPIPE_BAYER_MSB_BIT10,
    IPIPE_BAYER_MSB_BIT9,
    IPIPE_BAYER_MSB_BIT8,
    IPIPE_BAYER_MSB_BIT7
} IPIPE_BAYER_MSB_POS;

typedef enum {
    PROC_ISIF_VALID_NO = 0 << 0,                           // No valid
                                                           // parameter
    PROC_ISIF_VALID_2DLSC = 1 << 0,                        // Lens shadding
    PROC_ISIF_VALID_DC = 1 << 1,                           // Digital clamp
    PROC_ISIF_VALID_GOFFSET = 1 << 2,                      // Gain offset
    PROC_ISIF_VALID_FLASHT = 1 << 3,                       // Flash timming
    PROC_ISIF_VALID_VLCD = 1 << 4,                         // VLCD
    PROC_ISIF_VALID_ALL = 0xFFFF                           // All
} PROC_ISIF_VALID_ID;

typedef enum {
    PROC_IPIPE_VALID_NO = 0 << 0,                          // No valid
                                                           // parameter
    PROC_IPIPE_VALID_GIC = 1 << 0,
    PROC_IPIPE_VALID_EE = 1 << 1,
    PROC_IPIPE_VALID_NF1 = 1 << 2,
    PROC_IPIPE_VALID_NF2 = 1 << 3,
    PROC_IPIPE_VALID_CAR = 1 << 4,
    PROC_IPIPE_VALID_GAMMA = 1 << 5,
    PROC_IPIPE_VALID_LSC = 1 << 6,
    PROC_IPIPE_VALID_CFA = 1 << 7,
    PROC_IPIPE_VALID_RGB2YUV = 1 << 8,
    PROC_IPIPE_VALID_RGB2RGB1 = 1 << 9,
    PROC_IPIPE_VALID_RGB2RGB2 = 1 << 10,
    PROC_IPIPE_VALID_HIST = 1 << 11,
    PROC_IPIPE_VALID_GBCE = 1 << 12,
    PROC_IPIPE_VALID_WB = 1 << 13,
    PROC_IPIPE_VALID_YUV2YU = 1 << 14,
    PROC_IPIPE_VALID_BSC = 1 << 15,
    PROC_IPIPE_VALID_BOXCAR = 1 << 16,
    PROC_IPIPE_VALID_3DCC = 1 << 17,
    PROC_IPIPE_VALID_DPC_OTF = 1 << 18,
    PROC_IPIPE_VALID_DPC_LUT = 1 << 19,
    PROC_IPIPE_VALID_CGS = 1 << 20,
    PROC_IPIPE_VALID_ALL = 0xFFFF
} PROC_IPIPE_VALID_ID;

typedef enum {
    PROC_IPIPEIF_VALID_NO = 0 << 0,                        // No valid
                                                           // parameter
    PROC_IPIPEIF_VALID_DFS = 1 << 0,
    PROC_IPIPEIF_VALID_DPC1 = 1 << 1,
    PROC_IPIPEIF_VALID_DPC2 = 1 << 2,
    PROC_IPIPEIF_VALID_DPCM = 1 << 3,
    PROC_IPIPEIF_VALID_ALL = 0xFFFF
} PROC_IPIPEIF_VALID_ID;

typedef enum {
    PROC_IPIPE_RSZ_VALID_NO = 0 << 0,                      // No valid
                                                           // parameter
    PROC_IPIPE_RSZ_VALID_H_LPF = 1 << 0,
    PROC_IPIPE_RSZ_VALID_V_LPF = 1 << 1,
    PROC_IPIPE_RSZ_VALID_ALL = 0xFFFF
} PROC_IPIPE_RSZ_VALID_ID;

typedef enum {
    PROC_H3A_VALID_NO = 0 << 0,                            // No valid
                                                           // parameter
    PROC_H3A_VALID_AEWB = 1 << 0,
    PROC_H3A_VALID_AF = 1 << 1,
    PROC_H3A_VALID_H3A = 1 << 2,
    PROC_H3A_VALID_HIST = 1 << 3,
    PROC_H3A_VALID_ALL = 0xFFFF
} PROC_H3A_VALID_ID;

typedef enum {
    IPIPE_PROC_COMPR_NO,
    IPIPE_PROC_COMPR_DPCM,
    IPIPE_PROC_COMPR_ALAW
} IPIPE_PROC_COMPRESSION;

typedef enum {
    CCP2_OUTPUT_VP,
    CCP2_OUTPUT_OCP
} CCP2_OUTPUT_SELECT;

typedef struct {
    H3A_FEATURE_ENABLE_T median_en;
    uint8 median_threshold;
    uint16 saturation_limit;
    uint8 shift_value;

} h3a_aewb_params_t;

typedef struct {
    PROC_ISIF_VALID_ID f_proc_isif_validity;
    PROC_IPIPE_VALID_ID f_proc_ipipe_validity;
    PROC_IPIPEIF_VALID_ID f_proc_ipipeif_validity;
    PROC_IPIPE_RSZ_VALID_ID f_proc_rsz_validity;
    PROC_H3A_VALID_ID f_h3a_validity;
    IPIPE_BAYER_PATTERN color_pattern;
    IPIPE_BAYER_MSB_POS msb_pos;
    RSZ_IP_PORT_SEL_T rsz_ip_port_sel;
    IPIPE_VP_DEVICE vp_device;

    isif_2dlsc_cfg_t *lsc_2d;
    isif_clamp_cfg_t *clamp;
    isif_flash_cfg_t *flash;
    isif_gain_offset_cfg_t *gain_offset;
    isif_vldc_cfg_t *vlcd;

    ipipe_noise_filter_cfg_t *nf1;
    ipipe_noise_filter_cfg_t *nf2;
    ipipe_gic_cfg_t *gic;
    ipipe_wb_cfg_t *wb;
    ipipe_cfa_cfg_t *cfa;
    ipipe_gamma_cfg_t *gamma;
    ipipe_rgb_rgb_cfg_t *rgb2rgb1;
    ipipe_rgb_rgb_cfg_t *rgb2rgb2;
    ipipe_rgb_yuv_cfg_t *rgb2yuv;
    ipipe_gbce_cfg_t *gbce;
    ipipe_yuv444_yuv422_cfg_t *yuv2yuv;
    ipipe_ee_cfg_t *ee;
    ipipe_car_cfg_t *car;
    ipipe_lsc_cfg_t *lsc;
    ipipe_hist_cfg_t *histogram;
    ipipe_boxcar_cfg_t *boxcar;
    ipipe_bsc_cfg_t *bsc;
    ipipe_3d_lut_cfg_t *cc3dlut;
    ipipe_dpc_otf_cfg_t *dpc_otf;
    ipipe_dpc_lut_cfg_t *dpc_lut;
    ipipe_cgs_cfg_t *cgs;

    ipipeif_dfs_cfg_t *dfs;
    ipipeif_dpc_cfg_t *dpc1;
    ipipeif_dpc_cfg_t *dpc2;
    ipipeif_dpcm_cfg_t *dpcm;

    rsz_lpf_cfg_t *h_lpf;
    rsz_lpf_cfg_t *v_lpf;

    h3a_aewb_param_t *h3a_aewb_params;
    h3a_af_param_t *h3a_af_params;
    h3a_common_cfg_t *h3a_common_params;

    uint8 *pfIsValid;

} iss_config_processing_t;

typedef struct {
    uint32 in_size_ppln;
    uint32 in_size_x;
    uint32 in_size_y;
    int in_start_x;
    int in_start_y;
    uint32 out_size_bpln;
    uint32 out_size_x;
    uint32 out_size_y;
    uint32 out_sizeB_bpln;
    uint32 out_sizeB_x;
    uint32 out_sizeB_y;
    uint32 crop_x;
    uint32 crop_y;
    uint32 out_size_bplnC;
    uint32 out_sizeB_bplnC;
    uint32 clockPerscents;

} config_sizes_t;

typedef struct {
    uint32 VidstabEnb;
    config_sizes_t config_sizes;
    IPIPE_IN_FORMAT in_format;
    IPIPE_OUT_FORMAT out_format;
    IPIPE_OUT_FORMAT out_formatB;
    IPIPE_PROC_COMPRESSION f_compres_in;
    IPIPE_PROC_COMPRESSION f_compres_out;
    uint32 f_flip;
    uint32 f_mirror;
    uint32 f_flip_B;
    uint32 f_mirror_B;
    iss_config_processing_t *iss_config;
    void *lsc_2d_table_buffer;
    IPIPE_IN_YUV420_COMP_YC in_comp;
} iss_config_params_t;

typedef struct {
    uint32 h_start;
    uint32 h_size;
    uint32 v_start;
    uint32 v_size;
    uint32 ppln;
    uint32 h_size_crop;
    uint32 v_size_crop;

} in_frame_sizes_t;

typedef struct {
    uint32 h_start;
    uint32 h_size;
    uint32 v_start;
    uint32 v_size;
    uint32 bpln;

} out_frame_sizes_t;

typedef struct {
    in_frame_sizes_t in_sizes;
    out_frame_sizes_t out_sizes;
    IPIPE_OUT_FORMAT format;
    uint32 outC_ppln;

} resizer_sizes_cfg_t;

typedef struct {
    in_frame_sizes_t sizes;

} module_sizes_cfg_t;

typedef struct {
    resizer_sizes_cfg_t rszA;
    resizer_sizes_cfg_t rszB;
    module_sizes_cfg_t isif;
    module_sizes_cfg_t ipipe;
    module_sizes_cfg_t ipipe_if;
    uint32 f_flip_A;
    uint32 f_mirror_A;
    uint32 f_flip_B;
    uint32 f_mirror_B;
    uint32 clock_div;
    uint32 ipipeif_rsz;
    uint32 bytes_per_pixel;
    RSZ_IP_PORT_SEL_T rsz_ip_port;

} ipipe_size_cfg_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     // _ISS_DRV_COMMON_H_
