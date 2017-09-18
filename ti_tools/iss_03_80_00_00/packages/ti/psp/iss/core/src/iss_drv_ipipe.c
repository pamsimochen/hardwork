/** ==================================================================
 *  @file   iss_drv_ipipe.c                                                  
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
// #include <ti/iss/drivers/hal/iss/isp/ipipe.h>
#include "../inc/iss_drv.h"

/*********************DEFULT IPIPE PARAMS***************************************/
ipipe_src_cfg_t preview_ipipe_src_cfg;

const IPIPE_RAW_PIXEL_COLOR_FMT_T color_pattern[4][4] = {
    IPIPE_PIXEL_COLOUR_R, IPIPE_PIXEL_COLOUR_GR, IPIPE_PIXEL_COLOUR_GB,
        IPIPE_PIXEL_COLOUR_B,
    IPIPE_PIXEL_COLOUR_GR, IPIPE_PIXEL_COLOUR_R, IPIPE_PIXEL_COLOUR_B,
        IPIPE_PIXEL_COLOUR_GB,
    IPIPE_PIXEL_COLOUR_GB, IPIPE_PIXEL_COLOUR_B, IPIPE_PIXEL_COLOUR_R,
        IPIPE_PIXEL_COLOUR_GR,
    IPIPE_PIXEL_COLOUR_B, IPIPE_PIXEL_COLOUR_GB, IPIPE_PIXEL_COLOUR_GR,
        IPIPE_PIXEL_COLOUR_R
};

/* ===================================================================
 *  @func     issIpipePreviewParamsInit                                               
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
ISP_RETURN issIpipePreviewParamsInit(ipipe_cfg_t * cfg,
                                     iss_config_params_t * iss_cfg_params,
                                     in_frame_sizes_t * sizes)
{

    cfg->src_cfg = &preview_ipipe_src_cfg;
    cfg->filter_flag = 0;

    // cfg->dpc_lut_params = &default_preview_dpc_lut_params;
    // cfg->dpc_otf_params = &default_preview_dpc_otf_params;

    issIpipeConfigProcessing(cfg, iss_cfg_params->iss_config);

#ifdef IMGS_MICRON_MT9J003
    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_AR0331
    cfg->src_cfg->dims.hps = 0;                            // 0
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 0;                            // 0
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_PANASONIC_MN34041
    cfg->src_cfg->dims.hps = 80-1;
    cfg->src_cfg->dims.vps = 8; //24;
#ifdef MN34041_DATA_OP_FPGA
   	cfg->src_cfg->dims.hps = 80;//230;
    cfg->src_cfg->dims.vps = 8; //24;
#endif
#ifdef MN34041_DATA_OP_LVDS324
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.vps = 8;
#endif
   	cfg->src_cfg->dims.hsz = sizes->h_size_crop;
   	cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_SONY_IMX035
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_OMNIVISION_OV2715
    cfg->src_cfg->dims.hps = 1;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 1;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_SONY_IMX036
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 20;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_OMNIVISION_OV9712
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_OMNIVISION_OV10630
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_MT9P031
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_MT9D131
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_MT9M034
    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_TAXAS_TVP514X
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_AR0330
    cfg->src_cfg->dims.hps = 10;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD	
    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_SONY_IMX136
    cfg->src_cfg->dims.hps = 1;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 13;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_SONY_IMX104
    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif
#ifdef IMGS_ALTASENS_AL30210
    cfg->src_cfg->dims.hps = 1;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 1;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif  
#ifdef IMGS_OMNIVISION_OV2710
    cfg->src_cfg->dims.hps = 80;
    cfg->src_cfg->dims.hsz = sizes->h_size_crop;
    cfg->src_cfg->dims.vps = 8;
    cfg->src_cfg->dims.v_size = sizes->v_size_crop;
#endif

    cfg->src_cfg->wrt = 0;                                 // 1;
    if (IPIPE_IN_FORMAT_BAYER == iss_cfg_params->in_format)
    {
        cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_YCRCB_OUTPUT;
    }
    else
    {
        cfg->src_cfg->io_pixel_fmt = IPIPE_YCRCB_INPUT_YCRCB_OUTPUT;
    }
    cfg->src_cfg->Even_line_Even_pixel = color_pattern[iss_cfg_params->iss_config->color_pattern][0];   // IPIPE_PIXEL_COLOUR_R;
    cfg->src_cfg->Even_line_Odd_pixel = color_pattern[iss_cfg_params->iss_config->color_pattern][1];    // IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Even_pixel = color_pattern[iss_cfg_params->iss_config->color_pattern][2];    // IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Odd_pixel = color_pattern[iss_cfg_params->iss_config->color_pattern][3]; // IPIPE_PIXEL_COLOUR_B;

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issIpipeConfigProcessing                                               
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
ISP_RETURN issIpipeConfigProcessing(ipipe_cfg_t * cfg,
                                    iss_config_processing_t * iss_config)
{
    cfg->filter_flag = 0;

    if (PROC_IPIPE_VALID_NF1 & iss_config->f_proc_ipipe_validity)
    {
        cfg->nf1_params = (ipipe_noise_filter_cfg_t *) iss_config->nf1;
        cfg->filter_flag |= IPIPE_NOISE_FILTER_1_FLAG;
    }
    if (PROC_IPIPE_VALID_NF2 & iss_config->f_proc_ipipe_validity)
    {
        cfg->nf2_params = (ipipe_noise_filter_cfg_t *) iss_config->nf2;
        cfg->filter_flag |= IPIPE_NOISE_FILTER_2_FLAG;
    }
    if (PROC_IPIPE_VALID_GIC & iss_config->f_proc_ipipe_validity)
    {
        cfg->gic_params = (ipipe_gic_cfg_t *) iss_config->gic;
        cfg->filter_flag |= IPIPE_GIC_FLAG;
    }
    if (PROC_IPIPE_VALID_WB & iss_config->f_proc_ipipe_validity)
    {
        cfg->wb_params = (ipipe_wb_cfg_t *) iss_config->wb;
        cfg->filter_flag |= IPIPE_WB_FLAG;
    }
    if (PROC_IPIPE_VALID_CFA & iss_config->f_proc_ipipe_validity)
    {
        cfg->cfa_params = (ipipe_cfa_cfg_t *) iss_config->cfa;
        cfg->filter_flag |= IPIPE_CFA_FLAG;
    }
    if (PROC_IPIPE_VALID_RGB2RGB1 & iss_config->f_proc_ipipe_validity)
    {
        cfg->rgb_rgb1_params = (ipipe_rgb_rgb_cfg_t *) iss_config->rgb2rgb1;
        cfg->filter_flag |= IPIPE_RGB_RGB_1_FLAG;
    }
    if (PROC_IPIPE_VALID_GAMMA & iss_config->f_proc_ipipe_validity)
    {
        cfg->gamma_params = (ipipe_gamma_cfg_t *) iss_config->gamma;
        cfg->filter_flag |= IPIPE_GAMMA_FLAG;
    }
    if (PROC_IPIPE_VALID_RGB2RGB2 & iss_config->f_proc_ipipe_validity)
    {
        cfg->rgb_rgb2_params = (ipipe_rgb_rgb_cfg_t *) iss_config->rgb2rgb2;
        cfg->filter_flag |= IPIPE_RGB_RGB_2_FLAG;
    }
    // cfg->ipipe_3d_lut_params = &default_preview_ipipe_3d_lut_params;

    if (PROC_IPIPE_VALID_RGB2YUV & iss_config->f_proc_ipipe_validity)
    {
        cfg->rgb_yuv_params = (ipipe_rgb_yuv_cfg_t *) iss_config->rgb2yuv;
        cfg->filter_flag |= IPIPE_RGB_TO_YUV_FLAG;
    }
    if (PROC_IPIPE_VALID_GBCE & iss_config->f_proc_ipipe_validity)
    {
        cfg->gbce_params = (ipipe_gbce_cfg_t *) iss_config->gbce;
        cfg->filter_flag |= IPIPE_GBCE_FLAG;
    }
    if (PROC_IPIPE_VALID_YUV2YU & iss_config->f_proc_ipipe_validity)
    {
        cfg->yuv444_yuv422_params =
            (ipipe_yuv444_yuv422_cfg_t *) iss_config->yuv2yuv;
        cfg->filter_flag |= IPIPE_YUV444_YUV422_FLAG;
    }
    if (PROC_IPIPE_VALID_EE & iss_config->f_proc_ipipe_validity)
    {
        cfg->ee_param = (ipipe_ee_cfg_t *) iss_config->ee;
        cfg->filter_flag |= IPIPE_EE_FLAG;
    }
    if (PROC_IPIPE_VALID_CAR & iss_config->f_proc_ipipe_validity)
    {
        cfg->car_params = (ipipe_car_cfg_t *) iss_config->car;
        cfg->filter_flag |= IPIPE_CAR_FLAG;
    }
    if (PROC_IPIPE_VALID_BOXCAR & iss_config->f_proc_ipipe_validity)
    {
        cfg->boxcar_params = (ipipe_boxcar_cfg_t *) iss_config->boxcar;
        cfg->filter_flag |= IPIPE_BOXCAR_FLAG;
    }
    if (PROC_IPIPE_VALID_LSC & iss_config->f_proc_ipipe_validity)
    {
        cfg->lsc_params = (ipipe_lsc_cfg_t *) iss_config->lsc;
        cfg->filter_flag |= IPIPE_LSC_FLAG;
    }
    if (PROC_IPIPE_VALID_CGS & iss_config->f_proc_ipipe_validity)
    {
        cfg->cgs_params = iss_config->cgs;
        cfg->filter_flag |= IPIPE_CGS_FLAG;
    }
    if (PROC_IPIPE_VALID_HIST & iss_config->f_proc_ipipe_validity)
    {
        cfg->histogram_params = iss_config->histogram;
        cfg->filter_flag |= IPIPE_HST_FLAG;
    }
    if (PROC_IPIPE_VALID_BSC & iss_config->f_proc_ipipe_validity)
    {
        cfg->bsc_params = iss_config->bsc;
        cfg->filter_flag |= IPIPE_BSC_FLAG;
    }

    if (PROC_IPIPE_VALID_3DCC & iss_config->f_proc_ipipe_validity)
    {
        cfg->ipipe_3d_lut_params = iss_config->cc3dlut;
        cfg->filter_flag |= IPIPE_3D_LUT_FLAG;
    }

    if (PROC_IPIPE_VALID_DPC_OTF & iss_config->f_proc_ipipe_validity)
    {
        cfg->dpc_otf_params = iss_config->dpc_otf;
        cfg->filter_flag |= IPIPE_OTF_DPC_FLAG;
    }

    if (PROC_IPIPE_VALID_DPC_LUT & iss_config->f_proc_ipipe_validity)
    {
        cfg->dpc_lut_params = iss_config->dpc_lut;
        cfg->filter_flag |= IPIPE_LUT_DPC_FLAG;
    }

    iss_config->f_proc_ipipe_validity = PROC_IPIPE_VALID_NO;

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     issIpipeCaptureParamsInitNoWrite                                               
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
ISP_RETURN issIpipeCaptureParamsInitNoWrite(ipipe_cfg_t * cfg,
                                            iss_config_processing_t *
                                            iss_config,
                                            in_frame_sizes_t * sizes)
{

    cfg->src_cfg = &preview_ipipe_src_cfg;
    cfg->filter_flag = 0;

    issIpipeConfigProcessing(cfg, iss_config);

    cfg->src_cfg->dims.hps = 0;
    cfg->src_cfg->dims.hsz = sizes->h_size;
    cfg->src_cfg->dims.vps = 0;
    cfg->src_cfg->dims.v_size = sizes->v_size;

    cfg->src_cfg->wrt = 1;
    // cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_DISABLE_OUTPUT;
    cfg->src_cfg->io_pixel_fmt = IPIPE_BAYER_INPUT_BAYER_OUTPUT;

    cfg->src_cfg->Even_line_Even_pixel = color_pattern[iss_config->color_pattern][0];   // IPIPE_PIXEL_COLOUR_R;
    cfg->src_cfg->Even_line_Odd_pixel = color_pattern[iss_config->color_pattern][1];    // IPIPE_PIXEL_COLOUR_GR;
    cfg->src_cfg->Odd_line_Even_pixel = color_pattern[iss_config->color_pattern][2];    // IPIPE_PIXEL_COLOUR_GB;
    cfg->src_cfg->Odd_line_Odd_pixel = color_pattern[iss_config->color_pattern][3]; // IPIPE_PIXEL_COLOUR_B;

    return ISP_SUCCESS;
}

/**************************IPIPE    PARAMS END *************************************************************/
// ***************************************************************************************************//
