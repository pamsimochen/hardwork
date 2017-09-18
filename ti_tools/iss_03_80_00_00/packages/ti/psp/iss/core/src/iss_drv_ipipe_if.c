/** ==================================================================
 *  @file   iss_drv_ipipe_if.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/src/                                                  
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

/** include files **/
#include <ti/psp/iss/hal/iss/isp/isp.h>
#include "../inc/iss_drv.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from SDRAM and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     issIpipeifPreviewParamsInit                                               
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
ISP_RETURN issIpipeifPreviewParamsInit(ipipeif_path_cfg_t * cfg,
                                       iss_config_processing_t * iss_config)
{
    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW;         // IPIPEIF_ISIF_INPSRC_SDRAM_YUV
    if (PROC_IPIPEIF_VALID_DPC1 & iss_config->f_proc_ipipeif_validity)
    {
        cfg->isif_ip_cfg.pp_ip.dpc1_cfg = iss_config->dpc1;
    }
    else
    {
        cfg->isif_ip_cfg.pp_ip.dpc1_cfg = NULL;
    }
    cfg->isif_ip_cfg.pp_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.pp_ip.int_sw = IPIPEIF_START_POS_PP_VD;
    cfg->isif_ip_cfg.pp_ip.align_sync = 0;
    cfg->isif_ip_cfg.pp_ip.ini_rsz = 0;
    cfg->isif_ip_cfg.pp_ip.ip_clk_sel = IPIPEIF_IP_CLK_VPORT;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;

    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    if (PROC_IPIPEIF_VALID_DPC1 & iss_config->f_proc_ipipeif_validity)
    {
        cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = iss_config->dpc1;
    }
    else
    {
        cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = NULL;
    }
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;   // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    iss_config->f_proc_ipipeif_validity = PROC_IPIPEIF_VALID_NO;

    return ISP_SUCCESS;
}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures Ipipe-if h3a resizer
 *  
 *
 *  @param   ipipeif_h3a_input_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     issIpipeifH3aInputParamsInit                                               
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
ISP_RETURN issIpipeifH3aInputParamsInit(ipipeif_h3a_input_cfg_t * h3a_cfg,
                                        uint32 start, uint32 decimation)
{
    if (decimation > IPIPEIF_DOWNSCALE_NOM)
    {
        h3a_cfg->decimation_enable = CSL_IPIPEIF_RSZ2_DECM2_DECIMATE;
    }
    else
    {
        h3a_cfg->decimation_enable = CSL_IPIPEIF_RSZ2_DECM2_NODECIMATION;
    }
    h3a_cfg->resize = decimation;
    h3a_cfg->avg_filter_enable =
        (IPIPEIF_AVGFILT_T) CSL_IPIPEIF_RSZ2_AVGFILT2_ON;
    h3a_cfg->align_sync = 0;
    h3a_cfg->ini_rsz = start;

    return ISP_SUCCESS;
}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures Ipipe-if test parameters,  ipipe i/p is from 
 SDRAM,     YUV-> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     issIpipeifYuvInputParamsInit                                               
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
ISP_RETURN issIpipeifYuvInputParamsInit(ipipeif_path_cfg_t * cfg,
                                        ipipe_size_cfg_t * ipipe_size_cfg)
{
    in_frame_sizes_t *in_sizes = &ipipe_size_cfg->ipipe_if.sizes;

    uint32 rsz_ratio;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_SDRAM_YUV;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_SDRAM_YUV;

    cfg->ipipe_ip_cfg.yuv_ip.address = (uint32) NULL;
    cfg->ipipe_ip_cfg.yuv_ip.adofs = (in_sizes->ppln * 2) >> 5;
    cfg->ipipe_ip_cfg.yuv_ip.align_sync = 1;
    cfg->ipipe_ip_cfg.yuv_ip.avg_filter_enable = AVGFILT_OFF;

    rsz_ratio =
        ipipe_size_cfg->rszA.out_sizes.h_size /
        ipipe_size_cfg->rszA.in_sizes.h_size_crop;

    cfg->ipipe_ip_cfg.yuv_ip.clkdiv_m = 1;
    cfg->ipipe_ip_cfg.yuv_ip.clkdiv_n = 2;
    if (rsz_ratio > 0)
    {
        cfg->ipipe_ip_cfg.yuv_ip.clkdiv_n = rsz_ratio + 2;
    }
#ifdef TARGET_ZEBU
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.ppln_hs_interval = in_sizes->h_size + 16;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.lpfr_vs_interval = in_sizes->v_size + 16;
#else
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.ppln_hs_interval = in_sizes->h_size;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.lpfr_vs_interval = in_sizes->v_size;
#endif
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.hnum = in_sizes->h_size;
    cfg->ipipe_ip_cfg.yuv_ip.crop_dims.vnum = in_sizes->v_size;

    cfg->ipipe_ip_cfg.yuv_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;
    cfg->ipipe_ip_cfg.yuv_ip.ini_rsz = 1;
    cfg->ipipe_ip_cfg.yuv_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;
    cfg->ipipe_ip_cfg.yuv_ip.ip_clk_sel = IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV;

    cfg->ipipe_ip_cfg.yuv_ip.resize = 16;

    cfg->ipipe_ip_cfg.yuv_ip.vd_polarity = 1;
    cfg->ipipe_ip_cfg.yuv_ip.hd_polarity = 1;

    return ISP_SUCCESS;
}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures Ipipe-if test parameters,  ipipe i/p is from 
 SDRAM,     YUV-> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     issIpipeifYuv420InputParamsInit                                               
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
ISP_RETURN issIpipeifYuv420InputParamsInit(ipipeif_path_cfg_t * cfg,
                                           ipipe_size_cfg_t * ipipe_size_cfg,
                                           uint8 YC_sel)
{
    in_frame_sizes_t *in_sizes = &ipipe_size_cfg->ipipe_if.sizes;

    uint32 rsz_ratio;

    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_SDRAM_RAW;

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_SDRAM_RAW;

    cfg->ipipe_ip_cfg.sdram_ip.address = NULL;
    cfg->ipipe_ip_cfg.sdram_ip.adofs = (in_sizes->ppln * 2) >> 5;

    cfg->ipipe_ip_cfg.sdram_ip.data_shift = DATASFT_BITS11_0;
    cfg->ipipe_ip_cfg.sdram_ip.unpack = UNPACK_PACK_8_BITS_PIXEL_LINEAR;
    cfg->ipipe_ip_cfg.sdram_ip.oneshot = 1;
    cfg->ipipe_ip_cfg.sdram_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.sdram_ip.gain = 512;
    cfg->ipipe_ip_cfg.sdram_ip.dpcm_cfg = NULL;
    cfg->ipipe_ip_cfg.sdram_ip.dpc2_cfg = NULL;
    cfg->ipipe_ip_cfg.sdram_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;
    cfg->ipipe_ip_cfg.sdram_ip.resize = 16;
    cfg->ipipe_ip_cfg.sdram_ip.vd_polarity = 1;
    cfg->ipipe_ip_cfg.sdram_ip.hd_polarity = 1;
    cfg->ipipe_ip_cfg.sdram_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;
    cfg->ipipe_ip_cfg.sdram_ip.align_sync = 1;
    cfg->ipipe_ip_cfg.sdram_ip.ini_rsz = 1;
    cfg->ipipe_ip_cfg.sdram_ip.ip_clk_sel = IPIPEIF_IP_CLK_IPIPEIF_CLK_DIV;
    cfg->ipipe_ip_cfg.sdram_ip.clkdiv_m = 1;
    cfg->ipipe_ip_cfg.sdram_ip.clkdiv_n = 2;
    if (YC_sel)
    {
        cfg->ipipe_ip_cfg.sdram_ip.YUV16 = YCBCR_16_BIT_DATA;
    }
    else
    {
        cfg->ipipe_ip_cfg.sdram_ip.YUV16 = RAW_DATA;
    }

    rsz_ratio =
        ipipe_size_cfg->rszA.out_sizes.h_size /
        ipipe_size_cfg->rszA.in_sizes.h_size_crop;

    if (rsz_ratio > 0)
    {
        cfg->ipipe_ip_cfg.sdram_ip.clkdiv_n = rsz_ratio + 2;
    }
    if (YC_sel)
    {
        cfg->ipipe_ip_cfg.sdram_ip.crop_dims.ppln_hs_interval =
            in_sizes->h_size + 16;
        cfg->ipipe_ip_cfg.sdram_ip.crop_dims.lpfr_vs_interval =
            in_sizes->v_size + 16;
    }
    else
    {
        cfg->ipipe_ip_cfg.sdram_ip.crop_dims.ppln_hs_interval =
            in_sizes->h_size + 16;
        cfg->ipipe_ip_cfg.sdram_ip.crop_dims.lpfr_vs_interval =
            in_sizes->v_size + 16;
    }
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.hnum = in_sizes->h_size;
    cfg->ipipe_ip_cfg.sdram_ip.crop_dims.vnum = in_sizes->v_size;

    return ISP_SUCCESS;
}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures Ipipe-if test parameters, ISIF i/p is from SDRAM and ipipe i/p is from 
 ISIF,     RAM-> ipipe-if-> ISIF -> ipipe-if-> ipipe.   
 *  
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     issIpipeifDarkFrameParamsInit                                               
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
ISP_RETURN issIpipeifDarkFrameParamsInit(ipipeif_path_cfg_t * cfg,
                                         iss_config_processing_t * iss_config,
                                         in_frame_sizes_t * sizes,
                                         IPIPE_PROC_COMPRESSION f_compres_in)
{
    cfg->isif_inpsrc = IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM;

    cfg->isif_ip_cfg.sdram_dfs_ip.address = (NULL);
    cfg->isif_ip_cfg.sdram_dfs_ip.crop_dims.hnum = sizes->h_size_crop;
    cfg->isif_ip_cfg.sdram_dfs_ip.crop_dims.vnum = sizes->v_size_crop;
    cfg->isif_ip_cfg.sdram_dfs_ip.crop_dims.lpfr_vs_interval =
        (sizes->v_size - sizes->v_size_crop) / 2;
    cfg->isif_ip_cfg.sdram_dfs_ip.crop_dims.ppln_hs_interval =
        (sizes->h_size - sizes->h_size_crop) / 2;

    cfg->isif_ip_cfg.sdram_dfs_ip.align_sync = 0;
    cfg->isif_ip_cfg.sdram_dfs_ip.data_shift = DATASFT_BITS11_0;    // MSB
                                                                    // position

    if (iss_config->f_proc_ipipeif_validity & PROC_IPIPEIF_VALID_DFS)
    {
        iss_config->f_proc_ipipeif_validity &= ~PROC_IPIPEIF_VALID_DFS;
        cfg->isif_ip_cfg.sdram_dfs_ip.dfs_cfg = iss_config->dfs;
    }
    else
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.dfs_cfg = NULL;
    }
    if (iss_config->f_proc_ipipeif_validity & PROC_IPIPEIF_VALID_DPC2)
    {
        iss_config->f_proc_ipipeif_validity &= ~PROC_IPIPEIF_VALID_DPC2;
        cfg->isif_ip_cfg.sdram_dfs_ip.dpc2_cfg = iss_config->dpc2;
    }
    else
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.dpc2_cfg = NULL;
    }
    if (iss_config->f_proc_ipipeif_validity & PROC_IPIPEIF_VALID_DPCM)
    {
        iss_config->f_proc_ipipeif_validity &= ~PROC_IPIPEIF_VALID_DPCM;
        cfg->isif_ip_cfg.sdram_dfs_ip.dpcm_cfg = iss_config->dpcm;
    }
    else
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.dpcm_cfg = NULL;
    }
    cfg->isif_ip_cfg.sdram_dfs_ip.hd_polarity = 1;
    cfg->isif_ip_cfg.sdram_dfs_ip.vd_polarity = 1;
    cfg->isif_ip_cfg.sdram_dfs_ip.ini_rsz = 0;
    cfg->isif_ip_cfg.sdram_dfs_ip.int_sw = 0;
    cfg->isif_ip_cfg.sdram_dfs_ip.dfs_dir = IPIPEIF_DFS_DIR_SENSOR;

    if (IPIPE_PROC_COMPR_NO == f_compres_in)
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.unpack = UNPACK_NORMAL_16_BITS_PIXEL;
        cfg->isif_ip_cfg.sdram_dfs_ip.adofs = sizes->ppln * 2;
    }
    else if (IPIPE_PROC_COMPR_ALAW == f_compres_in)
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.unpack = UNPACK_PACK_8_BITS_PIXEL_ALAW;
        cfg->isif_ip_cfg.sdram_dfs_ip.adofs = sizes->ppln;
    }
    else
    {
        cfg->isif_ip_cfg.sdram_dfs_ip.unpack = UNPACK_PACK_8_BITS_PIXEL_LINEAR;
        cfg->isif_ip_cfg.sdram_dfs_ip.adofs = sizes->ppln;
    }

    cfg->ipipe_inpsrc = IPIPEIF_IPIPE_INPSRC_ISIF;

    cfg->ipipe_ip_cfg.isif_ip.avg_filter_enable = AVGFILT_OFF;
    cfg->ipipe_ip_cfg.isif_ip.gain = 512;
    cfg->ipipe_ip_cfg.isif_ip.YUV16 = RAW_DATA;            // YCBCR_16_BIT_DATA 
                                                           // RAW_DATA
    cfg->ipipe_ip_cfg.isif_ip.YUV8 = YUV8_YUV16;           // YUV8_YUV16
                                                           // YUV8_YUV8to16
    cfg->ipipe_ip_cfg.isif_ip.YUV8P = YUV8P_C_EVEN_Y_ODD;  // YUV8P_C_EVEN_Y_ODD 
                                                           // YUV8P_Y_EVEN_C_ODD
    if (PROC_IPIPEIF_VALID_DPC1 & iss_config->f_proc_ipipeif_validity)
    {
        iss_config->f_proc_ipipeif_validity &= ~PROC_IPIPEIF_VALID_DPC1;
        cfg->ipipe_ip_cfg.isif_ip.dpc1_cfg = iss_config->dpc1;
    }
    cfg->ipipe_ip_cfg.isif_ip.decimation_enable = IPIPEIF_FEATURE_DISABLE;  // 
    cfg->ipipe_ip_cfg.isif_ip.resize = 16;                 // 
    cfg->ipipe_ip_cfg.isif_ip.vd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.hd_polarity = 1;             // 
    cfg->ipipe_ip_cfg.isif_ip.int_sw = IPIPEIF_START_POS_ISIF_VD;   // 
    cfg->ipipe_ip_cfg.isif_ip.align_sync = 0;              // 
    cfg->ipipe_ip_cfg.isif_ip.ini_rsz = 0;                 // 

    // iss_config->f_proc_ipipeif_validity = PROC_IPIPEIF_VALID_NO;

    return ISP_SUCCESS;
}
