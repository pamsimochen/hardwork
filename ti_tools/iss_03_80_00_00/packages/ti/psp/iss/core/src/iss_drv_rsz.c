/** ==================================================================
 *  @file   iss_drv_rsz.c                                                  
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

extern rsz_A_regs_ovly rszA_reg;

extern rsz_B_regs_ovly rszB_reg;

extern rsz_regs_ovly rsz_reg;

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

rsz_ip_op_cfg_t default_preview_rszA_ip_op_params;

rsz_ip_op_cfg_t default_preview_rszB_ip_op_params;

rsz_resize_mode_cfg_t default_preview_rsz_resizer_mode_cfg_params;

rsz_passthru_mode_cfg_t iss_resizer_passthrough_mode;

rsz_output_dims_t iss_rszB_op_dims;

rsz_resize_ratio_t iss_rszB_op_ratio;

rsz_output_dims_t iss_rszA_op_dims;

rsz_resize_ratio_t iss_rszA_op_ratio;

rsz_mode_cfg_t default_preview_rsz_mode_cfg_params;

/** public functions **/

/** private functions **/

/***********************************RESIZER  PARAMS START***********************************************************/
/**********************************************************************************************************/
rsz_rgb_op_cfg_t rsz_A_rgb_preview_params;

rsz_rgb_op_cfg_t rsz_B_rgb_preview_params;

rsz_resize_ratio_t default_preview_rszA_resize_ratio = {

    256,                                                   // uint32
                                                           // RZB_V_DIF;/*The 
                                                           // parameter for
                                                           // vertical
                                                           // resize. */
    256                                                    // uint32
                                                           // RZB_H_DIF;/*The 
                                                           // parameter for
                                                           // horizontal
                                                           // resizing
                                                           // process. */
};

rsz_input_crop_dims_t default_preview_rszA_ip_params = {
    0,
    0                                                      // uint32 hps;
};

rsz_input_crop_dims_t default_preview_rszB_ip_params = {
    0,
    0                                                      // uint32 hps;
};

rsz_global_crop_dims_t default_preview_rsz_glb_crop = {
    0,                                                     // uint32
                                                           // v_pos;/*Global
                                                           // frame start*/
    480,                                                   // uint32 v_size;
                                                           // /*Global frame
                                                           // start*/
    0,                                                     // uint32
                                                           // h_pos;/*Crop
                                                           // Horizontal
                                                           // start */
    640,                                                   // uint32 h_size;
                                                           // /*Crop vertical 
                                                           // start */

};

rsz_h_phs_cfg_t default_preview_rsz_h_phs = { 0, 0 };

rsz_v_phs_cfg_t default_preview_rsz_v_phs = { 0, 0 };

rsz_downscale_cfg_t rszA_downscale_params = {
    0, 0
};

rsz_downscale_cfg_t rszB_downscale_params = {
    0, 0
};

rsz_common_cfg_t default_preview_rsz_common_params = {
    RSZ_IP_IPIPE,                                          // RSZ_IP_PORT_SEL_T 
                                                           // ip_port;
    RSZ_WRT_DISABLE,                                       // RSZ_WRT_ENABLE_T 
                                                           // wrt_enable;
    0,                                                     // uint16
                                                           // rszA_dma_size;//not 
                                                           // needed in
                                                           // Bypass and
                                                           // passthru mode
    0,                                                     // uint16
                                                           // rszB_dma_size;// 
                                                           // 
    0,                                                     // uint16
                                                           // fifo_thr_low;
                                                           // //IN_FIFO_CTRL;
    0,                                                     // uint16
                                                           // fifo_thr_high;
    YUV422_IP_YUV422_OP,
    RSZ_YC_COSITED,
    0,                                                     // rszA_int_lines
    0,                                                     // rszB_int_lines
    65535
};

const RSZ_FLIP_CFG_T ISS_DRV_RSZ_FLIP[2][2] = {
    RSZ_V_NFLIP_H_NFLIP, RSZ_V_NFLIP_H_FLIP,
    RSZ_V_FLIP_H_NFLIP, RSZ_V_FLIP_H_FLIP
};

/* ===================================================================
 *  @func     issSetRszOutAddress                                               
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
ISP_RETURN issSetRszOutAddress(RESIZER_T resizer, RSZ_OP_ADDRESS_T addr_mode,
                               void *address, uint32 ppln)
{
    rsz_output_addr_t rsz_op_addr;

    rsz_op_addr.base_addr = (uint32) address;
    rsz_op_addr.start_addr = (uint32) address;
    rsz_op_addr.end_pointer = 0xFFF;                       // 0x0964;
    rsz_op_addr.start_pointer = 0;
    rsz_op_addr.start_offset = ppln;                       // must be in
                                                           // Bytes

    return rsz_cfg_op_addr(resizer, addr_mode, &rsz_op_addr);
}

/* ===================================================================
 *  @func     issSetRszInputPort                                               
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
ISP_RETURN issSetRszInputPort(RSZ_IP_PORT_SEL_T ip_port)
{
    ISP_RETURN retVal = ISP_FAILURE;

    if ((RSZ_IP_IPIPEIF == ip_port) || (RSZ_IP_IPIPE == ip_port))
    {
        rsz_cfg_ip_port(ip_port);
        retVal = ISP_SUCCESS;
    }
    return (retVal);
}
const uint16 rszAverageOut[] = { 2, 4, 8, 16, 32, 64, 128, 256 };

/* RESIZER A is used to down scale the image by a factor of 2 and resizer B
 * is configured to upscale by a factor of 2 */
/* ===================================================================
 *  @func     issRszPreviewParamsInit                                               
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
ISP_RETURN issRszPreviewParamsInit(rsz_config_t * rsz_cfg,
                                   iss_config_params_t * iss_config_params,
                                   ipipe_size_cfg_t * ipipe_size_cfg)
{
    iss_config_processing_t *iss_config = iss_config_params->iss_config;

    // uint32 frac_div, rsz_ratioA, rsz_ratioB;
    // ISP_RETURN retval = ISP_FAILURE;
    resizer_sizes_cfg_t *sizes_rszA = &ipipe_size_cfg->rszA;

    resizer_sizes_cfg_t *sizes_rszB = &ipipe_size_cfg->rszB;

    if (IPIPE_OUT_FORMAT_UNUSED != iss_config_params->out_format)
    {
        iss_rszA_op_dims.v_size = (sizes_rszA->out_sizes.v_size);
        iss_rszA_op_dims.h_size = (sizes_rszA->out_sizes.h_size);
        iss_rszA_op_ratio.h_rsz_ratio =
            (256 * sizes_rszA->in_sizes.h_size_crop) /
            sizes_rszA->out_sizes.h_size;
        iss_rszA_op_ratio.v_rsz_ratio =
            (256 * sizes_rszA->in_sizes.v_size_crop) /
            sizes_rszA->out_sizes.v_size;

        rszA_downscale_params.h_avg_size = 0;
        rszA_downscale_params.v_avg_size = 0;
        if ((iss_rszA_op_ratio.h_rsz_ratio > 4096) ||
            (iss_rszA_op_ratio.v_rsz_ratio > 4096))
        {
            default_preview_rszA_ip_op_params.downscale_sel =
                RSZ_DOWNSCALE_ENABLE;
            do
            {
                iss_rszA_op_ratio.h_rsz_ratio =
                    (256 * sizes_rszA->in_sizes.h_size_crop) /
                    sizes_rszA->out_sizes.h_size /
                    rszAverageOut[rszA_downscale_params.h_avg_size];
            } while ((iss_rszA_op_ratio.h_rsz_ratio > 4096) &&
                     (rszA_downscale_params.h_avg_size++ < 7));
            do
            {
                iss_rszA_op_ratio.v_rsz_ratio =
                    (256 * sizes_rszA->in_sizes.v_size_crop) /
                    sizes_rszA->out_sizes.v_size /
                    rszAverageOut[rszA_downscale_params.v_avg_size];
            } while ((iss_rszA_op_ratio.v_rsz_ratio > 4096) &&
                     (rszA_downscale_params.v_avg_size++ < 7));
        }
        else
        {
            default_preview_rszA_ip_op_params.downscale_sel =
                RSZ_DOWNSCALE_DISABLE;
        }
        if (iss_rszA_op_ratio.h_rsz_ratio > 4096)
        {
            iss_rszA_op_ratio.h_rsz_ratio = 4096;
        }
        if (iss_rszA_op_ratio.v_rsz_ratio > 4096)
        {
            iss_rszA_op_ratio.v_rsz_ratio = 4096;
        }
        if (iss_rszA_op_ratio.h_rsz_ratio < 13)
        {
            iss_rszA_op_ratio.h_rsz_ratio = 13;
        }
        if (iss_rszA_op_ratio.v_rsz_ratio < 13)
        {
            iss_rszA_op_ratio.v_rsz_ratio = 13;
        }
        default_preview_rszA_ip_op_params.downscale_p = &rszA_downscale_params;

        // rsz_ratioA = iss_rszA_op_ratio.h_rsz_ratio *
        // iss_rszA_op_ratio.v_rsz_ratio;
        // rsz_ratioA /= 256;

        default_preview_rszA_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;

        default_preview_rszA_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

        default_preview_rszA_ip_op_params.flip_param =
            ISS_DRV_RSZ_FLIP[iss_config_params->f_flip][iss_config_params->
                                                        f_mirror];

        if (PROC_IPIPE_RSZ_VALID_H_LPF & iss_config->f_proc_rsz_validity)
        {
            default_preview_rszA_ip_op_params.h_lpf_p = iss_config->h_lpf;
        }
        else
        {
            default_preview_rszA_ip_op_params.h_lpf_p = NULL;
        }
        if (PROC_IPIPE_RSZ_VALID_V_LPF & iss_config->f_proc_rsz_validity)
        {
            default_preview_rszA_ip_op_params.v_lpf_p = iss_config->v_lpf;
        }
        else
        {
            default_preview_rszA_ip_op_params.v_lpf_p = NULL;
        }

        default_preview_rszA_ip_op_params.h_phs_p = &default_preview_rsz_h_phs;
        default_preview_rszA_ip_op_params.v_phs_p = &default_preview_rsz_v_phs;
        default_preview_rszA_ip_op_params.ip_dims_p =
            &default_preview_rszA_ip_params;
        default_preview_rszA_ip_op_params.op_dims_p = &iss_rszA_op_dims;

        default_preview_rszA_ip_op_params.ratio_p = &iss_rszA_op_ratio; // default_preview_rszA_resize_ratio;

        default_preview_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
        default_preview_rszA_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

        // default_preview_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
        // default_preview_rszA_ip_op_params.yuv420_yc_op =
        // sizes_rszA->format;
        default_preview_rsz_glb_crop.h_size = sizes_rszA->in_sizes.h_size_crop;
        default_preview_rsz_glb_crop.v_size = sizes_rszA->in_sizes.v_size_crop;
        // default_preview_rsz_glb_crop.h_pos = ((sizes_rszA->in_sizes.h_size 
        // - sizes_rszA->in_sizes.h_size_crop) / 2) & ~3;
        // default_preview_rsz_glb_crop.v_pos = ((sizes_rszA->in_sizes.v_size 
        // - sizes_rszA->in_sizes.v_size_crop) / 2) & ~1;
        default_preview_rsz_glb_crop.h_pos = sizes_rszA->in_sizes.h_start;
        default_preview_rsz_glb_crop.v_pos = sizes_rszA->in_sizes.v_start;
    }

    if (IPIPE_OUT_FORMAT_UNUSED != iss_config_params->out_formatB)
    {
        // resizer b config
        iss_rszB_op_dims.v_size = (sizes_rszB->out_sizes.v_size);
        iss_rszB_op_dims.h_size = (sizes_rszB->out_sizes.h_size);
        iss_rszB_op_ratio.h_rsz_ratio =
            (256 * sizes_rszB->in_sizes.h_size_crop) /
            sizes_rszB->out_sizes.h_size;
        iss_rszB_op_ratio.v_rsz_ratio =
            (256 * sizes_rszB->in_sizes.v_size_crop) /
            sizes_rszB->out_sizes.v_size;

        rszB_downscale_params.h_avg_size = 0;
        rszB_downscale_params.v_avg_size = 0;
        if ((iss_rszB_op_ratio.h_rsz_ratio > 4096) ||
            (iss_rszB_op_ratio.v_rsz_ratio > 4096))
        {
            default_preview_rszB_ip_op_params.downscale_sel =
                RSZ_DOWNSCALE_ENABLE;
            while (sizes_rszB->in_sizes.h_size_crop /
                   rszAverageOut[rszB_downscale_params.h_avg_size] >
                   (RSZB_MAX_LINE_SIZE / 2))
            {
                rszB_downscale_params.h_avg_size++;
            }
            do
            {
                iss_rszB_op_ratio.h_rsz_ratio =
                    (256 * sizes_rszB->in_sizes.h_size_crop) /
                    sizes_rszB->out_sizes.h_size /
                    rszAverageOut[rszB_downscale_params.h_avg_size];
            } while ((iss_rszB_op_ratio.h_rsz_ratio > 4096) &&
                     (rszB_downscale_params.h_avg_size++ < 7));
            do
            {
                iss_rszB_op_ratio.v_rsz_ratio =
                    (256 * sizes_rszB->in_sizes.v_size_crop) /
                    sizes_rszB->out_sizes.v_size /
                    rszAverageOut[rszB_downscale_params.v_avg_size];
            } while ((iss_rszB_op_ratio.v_rsz_ratio > 4096) &&
                     (rszB_downscale_params.v_avg_size++ < 7));
        }
        else
        {
            default_preview_rszB_ip_op_params.downscale_sel =
                RSZ_DOWNSCALE_DISABLE;
        }

        // rsz_ratioB = iss_rszB_op_ratio.h_rsz_ratio *
        // iss_rszB_op_ratio.v_rsz_ratio;
        // rsz_ratioB /= 256;

        default_preview_rszB_ip_op_params.downscale_p = &rszB_downscale_params;

        default_preview_rszB_ip_op_params.v_resize_method = C_CUBIC_Y_CUBIC;
        default_preview_rszB_ip_op_params.h_resize_method = C_CUBIC_Y_CUBIC;

        default_preview_rszB_ip_op_params.flip_param =
            ISS_DRV_RSZ_FLIP[iss_config_params->f_flip_B][iss_config_params->
                                                          f_mirror_B];

        default_preview_rszB_ip_op_params.h_lpf_p = iss_config->h_lpf;
        default_preview_rszB_ip_op_params.v_lpf_p = iss_config->v_lpf;

        default_preview_rszB_ip_op_params.h_phs_p = &default_preview_rsz_h_phs;

        default_preview_rszB_ip_op_params.v_phs_p = &default_preview_rsz_v_phs;

        default_preview_rszB_ip_op_params.ip_dims_p = &default_preview_rszB_ip_params;  // crop 
                                                                                        // is 
                                                                                        // 0

        default_preview_rszB_ip_op_params.op_dims_p = &iss_rszB_op_dims;

        default_preview_rszB_ip_op_params.ratio_p = &iss_rszB_op_ratio;

        default_preview_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_DISABLE;
        default_preview_rszB_ip_op_params.rgb_cfg_p = (rsz_rgb_op_cfg_t *) NULL;

        // default_preview_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
        // default_preview_rszB_ip_op_params.yuv420_yc_op =
        // sizes_rszB->format;
        if (IPIPE_OUT_FORMAT_UNUSED == iss_config_params->out_format)
        {
            default_preview_rsz_glb_crop.h_size =
                sizes_rszB->in_sizes.h_size_crop;
            default_preview_rsz_glb_crop.v_size =
                sizes_rszB->in_sizes.v_size_crop;
            // default_preview_rsz_glb_crop.h_pos =
            // ((sizes_rszB->in_sizes.h_size -
            // sizes_rszB->in_sizes.h_size_crop) / 2) & ~3;
            // default_preview_rsz_glb_crop.v_pos =
            // ((sizes_rszB->in_sizes.v_size -
            // sizes_rszB->in_sizes.v_size_crop) / 2) & ~1;
            default_preview_rsz_glb_crop.h_pos = sizes_rszB->in_sizes.h_start;
            default_preview_rsz_glb_crop.v_pos = sizes_rszB->in_sizes.v_start;
        }
    }

    // resizer config
    rsz_cfg->mode = RESIZER_RESIZE;
    rsz_cfg->mode_cfg_p = &default_preview_rsz_mode_cfg_params;

    default_preview_rsz_mode_cfg_params.resize_mode_cfgp =
        &default_preview_rsz_resizer_mode_cfg_params;

    default_preview_rsz_resizer_mode_cfg_params.rsz_A_cfg =
        &default_preview_rszA_ip_op_params;
    default_preview_rsz_resizer_mode_cfg_params.rsz_B_cfg =
        &default_preview_rszB_ip_op_params;
    default_preview_rsz_resizer_mode_cfg_params.crop_dims =
        &default_preview_rsz_glb_crop;

    // frac_div = 65535;
    /* 
     * if(256 > iss_rszA_op_ratio.h_rsz_ratio) { frac_div =
     * (ipipe_size_cfg->clock_div * 128) / iss_rszA_op_ratio.h_rsz_ratio; //
     * frac_div = frac_div * iss_rszA_op_ratio.h_rsz_ratio/ 256;
     * 
     * } if(frac_div > 65535) { frac_div = 65535; } */
    // TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_DRIVERS, "resizer clock divider:
    // %d", frac_div);

    // if(ipipe_size_cfg->clock_div > 100) {
    // frac_div = frac_div * 100 / ipipe_size_cfg->clock_div;
    // }
    /* 
     * if(rsz_ratioA < (256)) { frac_div = frac_div * rsz_ratioA / 256; }
     * if(sizes_rszB->format != IPIPE_OUT_FORMAT_UNUSED) { if((rsz_ratioB <
     * 256) && (rsz_ratioB < rsz_ratioA)) { frac_div = 65535 * rsz_ratioB /
     * 256; } } */

    default_preview_rsz_resizer_mode_cfg_params.rsz_common_params =
        &default_preview_rsz_common_params;
    default_preview_rsz_common_params.ip_port = RSZ_IP_IPIPE;
    default_preview_rsz_common_params.fracdiv = 65535;

    switch (sizes_rszA->format)
    {
        case IPIPE_OUT_FORMAT_YUV420:
            if (IPIPE_IN_FORMAT_YUV420 == iss_config_params->in_format)
            {
                default_preview_rsz_common_params.rsz_pix_fmt_mode =
                    YUV420_IP_YUV420_OP;
                default_preview_rszA_ip_op_params.yuv420_yc_op =
                    RSZ_YUV420_Y_OP_ONLY;
            }
            else
            {
                default_preview_rsz_common_params.rsz_pix_fmt_mode =
                    YUV422_IP_YUV420_OP;
                default_preview_rszA_ip_op_params.yuv420_yc_op =
                    RSZ_YUV420_YC_OP;
            }
            break;
        case IPIPE_OUT_FORMAT_YUV422:
            default_preview_rsz_common_params.rsz_pix_fmt_mode =
                YUV422_IP_YUV422_OP;
            default_preview_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_RGB565:
            default_preview_rsz_common_params.rsz_pix_fmt_mode =
                YUV422_IP_RGB_OP;

            rsz_A_rgb_preview_params.mask_ctrl =
                RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE;
            rsz_A_rgb_preview_params.rgb_pix_size = RSZ_RGB_PIX_SIZE_16;

            default_preview_rszA_ip_op_params.rgb_cfg_p =
                &rsz_A_rgb_preview_params;
            default_preview_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
            default_preview_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_RGB888:
            default_preview_rsz_common_params.rsz_pix_fmt_mode =
                YUV422_IP_RGB_OP;

            rsz_A_rgb_preview_params.mask_ctrl =
                RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE;
            rsz_A_rgb_preview_params.rgb_pix_size = RSZ_RGB_PIX_SIZE_32;
            rsz_A_rgb_preview_params.rgb_alpha_blend = 128; // TODO - now it
                                                            // is only for
                                                            // testing

            default_preview_rszA_ip_op_params.rgb_cfg_p =
                &rsz_A_rgb_preview_params;
            default_preview_rszA_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
            default_preview_rszA_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_BAYER:
            rsz_cfg->mode = RESIZER_PASSTHRU;
            // rsz_cfg->mode = RESIZER_BYPASS;
            default_preview_rsz_common_params.ip_port = RSZ_IP_IPIPEIF;

            default_preview_rsz_common_params.rsz_pix_fmt_mode = RAW_IP_RAW_OP;
            iss_resizer_passthrough_mode.crop_dims =
                &default_preview_rsz_glb_crop;
            iss_resizer_passthrough_mode.flip_params = RSZ_V_NFLIP_H_NFLIP;
            iss_resizer_passthrough_mode.pix_fmt = RAW_IP_RAW_OP;
            iss_resizer_passthrough_mode.rsz_common_params =
                &default_preview_rsz_common_params;
            default_preview_rsz_mode_cfg_params.passthru_mode_cfgp =
                &iss_resizer_passthrough_mode;
            if ((IPIPE_OUT_FORMAT_BAYER != sizes_rszB->format) &&
                (IPIPE_OUT_FORMAT_UNUSED != sizes_rszB->format))
            {
                return ISP_FAILURE;
            }
            break;

        case IPIPE_OUT_FORMAT_UNUSED:
            break;
        default:
            return ISP_FAILURE;
    }

    switch (sizes_rszB->format)
    {
        case IPIPE_OUT_FORMAT_YUV420:
            default_preview_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV420_YC_OP;
            break;
        case IPIPE_OUT_FORMAT_YUV422:
            default_preview_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_RGB565:
            rsz_B_rgb_preview_params.mask_ctrl =
                RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE;
            rsz_B_rgb_preview_params.rgb_pix_size = RSZ_RGB_PIX_SIZE_16;

            default_preview_rszB_ip_op_params.rgb_cfg_p =
                &rsz_B_rgb_preview_params;
            default_preview_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
            default_preview_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_RGB888:

            rsz_B_rgb_preview_params.mask_ctrl =
                RSZ_RGB_MSK1_DISABLE_MSK0_DISABLE;
            rsz_B_rgb_preview_params.rgb_pix_size = RSZ_RGB_PIX_SIZE_32;
            rsz_B_rgb_preview_params.rgb_alpha_blend = 128; // TODO - now it
                                                            // is only for
                                                            // testing

            default_preview_rszB_ip_op_params.rgb_cfg_p =
                &rsz_B_rgb_preview_params;
            default_preview_rszB_ip_op_params.rgb_op_sel = RSZ_RGB_OP_ENABLE;
            default_preview_rszB_ip_op_params.yuv420_yc_op = RSZ_YUV422_OP;
            break;
        case IPIPE_OUT_FORMAT_BAYER:
            if (IPIPE_OUT_FORMAT_BAYER != sizes_rszA->format)
            {
                return ISP_FAILURE;
            }
            break;

        case IPIPE_OUT_FORMAT_UNUSED:
            break;

        default:
            return ISP_FAILURE;
    }

    iss_config->f_proc_rsz_validity = PROC_IPIPE_RSZ_VALID_NO;

    // return retval;
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     rszASetOutConfig                                               
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
ISP_RETURN rszASetOutConfig(int inWidth, int inHeight, int outWidth,
                            int outHeight)
{
    int vertReszRatio = 0;

    int horzReszRatio = 0;

    rsz_reg->SRC_VSZ = inHeight - 1;
    rsz_reg->SRC_HSZ = inWidth - 1;

    rszA_reg->RZA_I_VPS = 0;
    rszA_reg->RZA_I_HPS = 0;
    rszA_reg->RZA_O_VSZ = outHeight - 1;
    rszA_reg->RZA_O_HSZ = outWidth - 1;

    vertReszRatio = (inHeight * 256) / (outHeight);
    if (vertReszRatio < 16)
        vertReszRatio = 16;
    if (vertReszRatio > 4096)
        vertReszRatio = 4096;

    rszA_reg->RZA_V_DIF = vertReszRatio;
    rszA_reg->RZA_V_TYP = 0;

    horzReszRatio = (inWidth * 256) / (outWidth);
    if (horzReszRatio < 16)
        horzReszRatio = 16;
    if (horzReszRatio > 4096)
        horzReszRatio = 4096;

    rszA_reg->RZA_H_DIF = horzReszRatio;
    rszA_reg->RZA_H_TYP = 0;
    rszA_reg->RZA_DWN_EN = 0;
    rszA_reg->RZA_DWN_AV = 0;
    return 0;
}

/* ===================================================================
 *  @func     rszBSetOutConfig                                               
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
ISP_RETURN rszBSetOutConfig(int inWidth, int inHeight, int outWidth,
                            int outHeight)
{
    int vertReszRatio = 0;

    int horzReszRatio = 0;

    rsz_reg->SRC_VSZ = inHeight - 1;
    rsz_reg->SRC_HSZ = inWidth - 1;

    rszB_reg->RZB_I_VPS = 0;
    rszB_reg->RZB_I_HPS = 0;
    rszB_reg->RZB_O_VSZ = outHeight - 1;
    rszB_reg->RZB_O_HSZ = outWidth - 1;

    vertReszRatio = (inHeight * 256) / (outHeight);
    if (vertReszRatio < 16)
        vertReszRatio = 16;
    if (vertReszRatio > 4096)
        vertReszRatio = 4096;

    rszB_reg->RZB_V_DIF = vertReszRatio;
    rszB_reg->RZB_V_TYP = 0;
    horzReszRatio = (inWidth * 256) / (outWidth);
    if (horzReszRatio < 16)
        horzReszRatio = 16;
    if (horzReszRatio > 4096)
        horzReszRatio = 4096;

    rszB_reg->RZB_H_DIF = horzReszRatio;
    rszB_reg->RZB_H_TYP = 0;

    rszB_reg->RZB_DWN_EN = 0;
    rszB_reg->RZB_DWN_AV = 0;
    return 0;
}
