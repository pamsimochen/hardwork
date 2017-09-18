/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file isif.c
 *
 * This file contains structures and functions for ISIF component of Ducati subsystem in OMAP4/Monica
 *
 * @path Centaurus\drivers\drv_isp\src\csl
 *
 * @rev 1.0
 *
 *
 */
/*========================================================================*/

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/

#include "../inc/isif.h"
#include "../../isp5_utils/isp5_sys_types.h"

#include "../../isp5_utils/isp5_csl_utils.h"
#include "../../common/inc/isp_common.h"

isif_regs_ovly isif_reg;

isif_dev_data_t isif_dev;

isif_dev_data_t *isif_devp;

/* ================================================================ */
/**
 *  isif_config_timing_generator() configures the timing generator of the ISIF module.
 *
 *  @param  cfg      isif_flash_cfg_t * cfg  is a pointer to the data-structure of isif_flash_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_timing_generator
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
ISP_RETURN isif_config_timing_generator(isif_flash_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);
    ISP_WRITE32(isif_reg->FLSHCFG0, cfg->enable);
    ISP_WRITE32(isif_reg->FLSHCFG1, cfg->flash_timing_start_line);
    ISP_WRITE32(isif_reg->FLSHCFG2, cfg->flash_timing_width);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_vd_line_numbers() configures the VD0, VD1, VD2 line number for the ISIF module.
 *
 *  @param  cfg      isif_vd_interrupt_t * cfg  is a pointer to the data-structure of isif_vd_interrupt_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_vd_line_numbers
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
ISP_RETURN isif_config_vd_line_numbers(isif_vd_interrupt_t * cfg)
{
    ISP_WRITE32(isif_reg->VDINT0, cfg->vd0_intr_timing_lineno);
    ISP_WRITE32(isif_reg->VDINT1, cfg->vd1_intr_timing_lineno);
    ISP_WRITE32(isif_reg->VDINT2, cfg->vd2_intr_timing_lineno);
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isif_config_vd0_line_number
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
ISP_RETURN isif_config_vd0_line_number(uint32 line_number)
{
    ISP_WRITE32(isif_reg->VDINT0, line_number);
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isif_config_vd1_line_number
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
ISP_RETURN isif_config_vd1_line_number(uint32 line_number)
{
    ISP_WRITE32(isif_reg->VDINT1, line_number);
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isif_config_vd2_line_number
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
ISP_RETURN isif_config_vd2_line_number(uint32 line_number)
{
    ISP_WRITE32(isif_reg->VDINT2, line_number);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_sensor_linearization() configures the sensor linearization component of  the ISIF module.
 *
 *  @param  cfg  isif_sensor_linear_cfg_t * cfg       is a pointer to the data-structure of isif_sensor_linear_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_sensor_linearization
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
ISP_RETURN isif_config_sensor_linearization(isif_sensor_linear_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);

    ISP_SET32(isif_reg->LINCFG0, cfg->correction_shift,
              CSL_ISIF_LINCFG0_CORRSFT_SHIFT, CSL_ISIF_LINCFG0_CORRSFT_MASK);
    ISP_SET32(isif_reg->LINCFG0, cfg->non_uniform_mode,
              CSL_ISIF_LINCFG0_LINMD_SHIFT, CSL_ISIF_LINCFG0_LINMD_MASK);
    ISP_WRITE32(isif_reg->LINCFG1, cfg->scale_value);
    ISP_SET32(isif_reg->LINCFG0, cfg->enable, CSL_ISIF_LINCFG0_LINEN_SHIFT,
              CSL_ISIF_LINCFG0_LINEN_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_gain_offset() configures the gain-offset component of  the ISIF module.
 *
 *  @param  cfg  isif_gain_offset_cfg_t * cfg:-  is a pointer to the data-structure of isif_gain_offset_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_gain_offset
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
ISP_RETURN isif_config_gain_offset(isif_gain_offset_cfg_t * cfg)
{
    uint8 gain_offset_enable;

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(isif_reg->CRGAIN, cfg->gain_r);
    ISP_WRITE32(isif_reg->CGRGAIN, cfg->gain_gr);
    ISP_WRITE32(isif_reg->CGBGAIN, cfg->gain_gb);
    ISP_WRITE32(isif_reg->CBGAIN, cfg->gain_bg);
    ISP_WRITE32(isif_reg->COFSTA, cfg->offset);

    /* white_balance for h3a input from isif */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_H3A_WHITE_BALANCE_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_WBEN2_SHIFT, CSL_ISIF_CGAMMAWD_WBEN2_MASK);

    /* ipipe_white_balance */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_IPIPE_WHITE_BALANCE_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_WBEN1_SHIFT, CSL_ISIF_CGAMMAWD_WBEN1_MASK);

    /* sdram_white_balance */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_SDRAM_WHITE_BALANCE_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_WBEN0_SHIFT, CSL_ISIF_CGAMMAWD_WBEN0_MASK);

    /* offset for h3a input from isif */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_H3A_OFFSET_CTRL_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_OFSTEN2_SHIFT, CSL_ISIF_CGAMMAWD_OFSTEN2_MASK);

    /* ipipe_offset */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_IPIPE_OFFSET_CTRL_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_OFSTEN1_SHIFT, CSL_ISIF_CGAMMAWD_OFSTEN1_MASK);

    /* sdram_offset */
    gain_offset_enable =
        ((cfg->gain_offset_featureflag & ISIF_SDRAM_OFFSET_CTRL_FLAG) && 1);
    ISP_SET32(isif_reg->CGAMMAWD, gain_offset_enable,
              CSL_ISIF_CGAMMAWD_OFSTEN0_SHIFT, CSL_ISIF_CGAMMAWD_OFSTEN0_MASK);

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isif_config_in_sizes
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
ISP_RETURN isif_config_in_sizes(isif_input_sizes_t * sizes)
{
    ISP_WRITE32(isif_reg->HDW, sizes->hd_width - 1);
    ISP_WRITE32(isif_reg->VDW, sizes->vd_width - 1);
    ISP_WRITE32(isif_reg->PPLN, sizes->ppln_hs_interval - 1);
    ISP_WRITE32(isif_reg->LPFR, (sizes->lpfr_vs_interval * 2) - 1);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_input_src() configures the input source formatting settings of the ISIF module.
 *
 *  @param  cfg  isif_ip_src_cfg_t * cfg     is a pointer to the data-structure of isif_ip_src_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_input_src
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
ISP_RETURN isif_config_input_src(isif_ip_src_cfg_t * cfg)
{
    uint8 i;

    ISP_ASSERT(cfg, NULL);
    ISP_WRITE32(isif_reg->HDW, cfg->hd_width - 1);
    ISP_WRITE32(isif_reg->VDW, cfg->vd_width - 1);
    ISP_WRITE32(isif_reg->PPLN, cfg->ppln_hs_interval - 1);
    ISP_WRITE32(isif_reg->LPFR, cfg->lpfr_vs_interval - 1);

    for (i = 0; i < 4; i++)
    {
        ISP_SET32(isif_reg->CCOLP, cfg->field1_pixel_pos[i],
                  (CSL_ISIF_CCOLP_CP0_F1_SHIFT - 2 * i),
                  CSL_ISIF_CCOLP_CP0_F1_MASK >> i);
        ISP_SET32(isif_reg->CCOLP, cfg->field0_pixel_pos[i],
                  (CSL_ISIF_CCOLP_CP0_F0_SHIFT - 2 * i),
                  CSL_ISIF_CCOLP_CP0_F0_MASK >> i);
    }
    ISP_SET32(isif_reg->SYNCEN, cfg->sync_enable, CSL_ISIF_SYNCEN_SYEN_SHIFT,
              CSL_ISIF_SYNCEN_SYEN_MASK);

    ISP_SET32(isif_reg->MODESET, cfg->ip_pix_fmt, CSL_ISIF_MODESET_INPMOD_SHIFT,
              CSL_ISIF_MODESET_INPMOD_MASK);
    ISP_SET32(isif_reg->CGAMMAWD, cfg->ip_data_msb_pos,
              CSL_ISIF_CGAMMAWD_GWDI_SHIFT, CSL_ISIF_CGAMMAWD_GWDI_MASK);
    ISP_SET32(isif_reg->CGAMMAWD, cfg->cfa_pattern,
              CSL_ISIF_CGAMMAWD_CFAP_SHIFT, CSL_ISIF_CGAMMAWD_CFAP_MASK);

    ISP_SET32(isif_reg->CCDCFG, cfg->yc_in_swap, CSL_ISIF_CCDCFG_YCINSWP_SHIFT,
              CSL_ISIF_CCDCFG_YCINSWP_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->msb_inverse_cin,
              CSL_ISIF_CCDCFG_MSBINVI_SHIFT, CSL_ISIF_CCDCFG_MSBINVI_MASK);

    ISP_SET32(isif_reg->CCDCFG, cfg->ccir656_bit_width,
              CSL_ISIF_CCDCFG_BW656_SHIFT, CSL_ISIF_CCDCFG_BW656_MASK);
    ISP_SET32(isif_reg->REC656IF, cfg->r656_inferface_mode,
              CSL_ISIF_REC656IF_R656ON_SHIFT, CSL_ISIF_REC656IF_R656ON_MASK);
    ISP_SET32(isif_reg->REC656IF, cfg->err_correction_mode,
              CSL_ISIF_REC656IF_ECCFVH_SHIFT, CSL_ISIF_REC656IF_ECCFVH_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_sdram_format() configures the sdram output formatting of  the ISIF module.
 *
 *  @param  cfg isif_sdram_op_cfg_t * cfg       is a pointer to the data-structure of isif_sdram_op_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_sdram_format
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
ISP_RETURN isif_config_sdram_format(isif_sdram_op_cfg_t * cfg)
{
    isif_fid_line_offset_ctg_t *cfg_offset = &(cfg->field_offset_params);

    ISP_RETURN retval = ISP_FAILURE;

    ISP_ASSERT(cfg, NULL);
    ISP_SET32(isif_reg->CCDCFG, cfg->vsync_enable, CSL_ISIF_CCDCFG_VDLC_SHIFT,
              CSL_ISIF_CCDCFG_VDLC_MASK);

    ISP_SET32(isif_reg->MODESET, cfg->ccd_raw_shift_value,
              CSL_ISIF_MODESET_CCDW_SHIFT, CSL_ISIF_MODESET_CCDW_MASK);
    ISP_SET32(isif_reg->MODESET, cfg->wen_selection_mode,
              CSL_ISIF_MODESET_SWEN_SHIFT, CSL_ISIF_MODESET_SWEN_MASK);
    ISP_SET32(isif_reg->MODESET, cfg->ccd_field_mode,
              CSL_ISIF_MODESET_CCDMD_SHIFT, CSL_ISIF_MODESET_CCDMD_MASK);

    ISP_SET32(isif_reg->CCDCFG, cfg->msb_inverse_cout,
              CSL_ISIF_CCDCFG_MSBINVO_SHIFT, CSL_ISIF_CCDCFG_MSBINVO_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->sdram_byte_swap,
              CSL_ISIF_CCDCFG_BSWD_SHIFT, CSL_ISIF_CCDCFG_BSWD_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->y_position, CSL_ISIF_CCDCFG_Y8POS_SHIFT,
              CSL_ISIF_CCDCFG_Y8POS_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->sdram_ext_trig,
              CSL_ISIF_CCDCFG_EXTRG_SHIFT, CSL_ISIF_CCDCFG_EXTRG_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->sdram_trig_select,
              CSL_ISIF_CCDCFG_TRGSEL_SHIFT, CSL_ISIF_CCDCFG_TRGSEL_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->ccd_valid_log,
              CSL_ISIF_CCDCFG_WENLOG_SHIFT, CSL_ISIF_CCDCFG_WENLOG_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->fid_detection_mode,
              CSL_ISIF_CCDCFG_FIDMD_SHIFT, CSL_ISIF_CCDCFG_FIDMD_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->yc_out_swap,
              CSL_ISIF_CCDCFG_YCOUTSWP_SHIFT, CSL_ISIF_CCDCFG_YCOUTSWP_MASK);
    ISP_SET32(isif_reg->CCDCFG, cfg->sdram_pack_fmt,
              CSL_ISIF_CCDCFG_SDRPACK_SHIFT, CSL_ISIF_CCDCFG_SDRPACK_MASK);

    ISP_WRITE32(isif_reg->SPH, cfg->sdram_hpos);
    ISP_WRITE32(isif_reg->LNH, cfg->sdram_hsize - 1);
    ISP_WRITE32(isif_reg->SLV0, cfg->sdram_vpos0);
    ISP_WRITE32(isif_reg->SLV1, cfg->sdram_vpos1);
    ISP_WRITE32(isif_reg->LNV, cfg->sdram_vsize - 1);

    ISP_SET32(isif_reg->HSIZE, cfg->memory_addr_decrement,
              CSL_ISIF_HSIZE_ADCR_SHIFT, CSL_ISIF_HSIZE_ADCR_MASK);
    ISP_SET32(isif_reg->HSIZE, cfg->memory_addr_offset >> 5,
              CSL_ISIF_HSIZE_HSIZE_SHIFT, CSL_ISIF_HSIZE_HSIZE_MASK);

    ISP_SET32(isif_reg->SDOFST, cfg_offset->fid_polarity,
              CSL_ISIF_SDOFST_FIINV_SHIFT, CSL_ISIF_SDOFST_FIINV_MASK);
    ISP_SET32(isif_reg->SDOFST, cfg_offset->odd_field_line_offset,
              CSL_ISIF_SDOFST_FOFST_SHIFT, CSL_ISIF_SDOFST_FOFST_MASK);
    ISP_SET32(isif_reg->SDOFST, cfg_offset->even_line_even_field_offset,
              CSL_ISIF_SDOFST_LOFTS0_SHIFT, CSL_ISIF_SDOFST_LOFTS0_MASK);
    ISP_SET32(isif_reg->SDOFST, cfg_offset->odd_line_even_field_offset,
              CSL_ISIF_SDOFST_LOFTS1_SHIFT, CSL_ISIF_SDOFST_LOFTS1_MASK);
    ISP_SET32(isif_reg->SDOFST, cfg_offset->even_line_odd_field_offset,
              CSL_ISIF_SDOFST_LOFTS2_SHIFT, CSL_ISIF_SDOFST_LOFTS2_MASK);
    ISP_SET32(isif_reg->SDOFST, cfg_offset->odd_line_odd_field_offset,
              CSL_ISIF_SDOFST_LOFTS3_SHIFT, CSL_ISIF_SDOFST_LOFTS3_MASK);

    retval = isif_config_sdram_address(cfg->memory_addr);
    if (retval != ISP_FAILURE)
    {
        ISP_SET32(isif_reg->SYNCEN, cfg->data_write_enable,
                  CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
    }
    return retval;
}

/* ===================================================================
 *  @func     isif_config_sdram_address
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
ISP_RETURN isif_config_sdram_address(uint32 address)
{
    if (address & 0x1F)
    {
        return ISP_FAILURE;
    }
    else
    {
        ISP_WRITE32(isif_reg->CADU, ((address >> 21) & 0x7FF));
        ISP_WRITE32(isif_reg->CADL, ((address >> 5) & 0xFFFF));
    }
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_lpf() configures the LPF component of  the ISIF module to be either ON or OFF.
 *
 *  @param   lpf_onoff     uint8 lpf_onoff      is an unsigned integer which indicates the status of LPF to be configured
 *  @return  status        Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_lpf
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
ISP_RETURN isif_config_lpf(uint8 lpf_onoff)
{
    ISP_SET32(isif_reg->MODESET, lpf_onoff, CSL_ISIF_MODESET_HLPF_SHIFT,
              CSL_ISIF_MODESET_HLPF_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_alaw_compression() configures the ALAW  component of the ISIF module to be either ON or OFF.
 *
 *  @param  alaw_onoff  uint8 alaw_onoff :-    is an unsigned integer which indicates the status of ALAW compression for sdram
 *  @return  status              Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_alaw_compression
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
ISP_RETURN isif_config_alaw_compression(uint8 alaw_onoff)
{
    ISP_SET32(isif_reg->CGAMMAWD, alaw_onoff, CSL_ISIF_CGAMMAWD_CCDTBL_SHIFT,
              CSL_ISIF_CGAMMAWD_CCDTBL_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_dpcm_encoder() configures the DPCM component of the ISIF module to be using either type1 or type-2.
 *
 *  @param  dpcm_predictor  uint8 dpcm_predictor is an unsigned integer which indicates the type of DPCM compression to be used
 *  @return  status                   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_dpcm_encoder
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
ISP_RETURN isif_config_dpcm_encoder(uint8 dpcm_predictor, uint8 dpcm_enable)
{
    ISP_SET32(isif_reg->MISC, dpcm_enable, CSL_ISIF_MISC_DPCMEN_SHIFT,
              CSL_ISIF_MISC_DPCMEN_MASK);
    ISP_SET32(isif_reg->MISC, dpcm_predictor, CSL_ISIF_MISC_DPCMPRE_SHIFT,
              CSL_ISIF_MISC_DPCMPRE_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_hv_culling() configures the horizontal-vertical culling of the input in  the ISIF module.
 *
 *  @param  cfg   isif_culling_cfg_t * cfg is a pointer to the data-structure of isif_culling_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_hv_culling
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
ISP_RETURN isif_config_hv_culling(isif_culling_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);
    ISP_WRITE32(isif_reg->CULV, cfg->vertical_culling);

    ISP_SET32(isif_reg->CULH, cfg->horizontal_odd_culling,
              CSL_ISIF_CULH_CLHO_SHIFT, CSL_ISIF_CULH_CLHO_MASK);
    ISP_SET32(isif_reg->CULH, cfg->horizontal_even_culling,
              CSL_ISIF_CULH_CLHE_SHIFT, CSL_ISIF_CULH_CLHE_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_hv_culling_disable() disables the culling mechanism in the ISIF module.
 *
 *  @param   none
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_hv_culling_disable
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
ISP_RETURN isif_config_hv_culling_disable()
{
    ISP_WRITE32(isif_reg->CULV, 0xFFFF);
    ISP_WRITE32(isif_reg->CULH, 0xFFFFFFFF);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_vertical_line_defect_correction() configures the VLDC component  of  the ISIF module.
 *
 *  @param  cfg     isif_vldc_cfg_t * cfg   is a pointer to the data-structure of isif_vldc_cfg_t type which encapsulates the configuration settings
 *  @return  status    Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_vertical_line_defect_correction
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
ISP_RETURN isif_config_vertical_line_defect_correction(isif_vldc_cfg_t * cfg)
{

    isif_vldc_defect_line_params_t *line_params = NULL;

    uint8 i;

    ISP_ASSERT(cfg, NULL);
    line_params = cfg->vldc_defect_line_params;

    /* vertical line defect correction disabled for table to be updated for 8
     * pgmable defect v-lines */
    ISP_SET32(isif_reg->DFCCTL, CSL_ISIF_DFCCTL_VDFCEN_OFF,
              CSL_ISIF_DFCCTL_VDFCEN_SHIFT, CSL_ISIF_DFCCTL_VDFCEN_MASK);

    ISP_SET32(isif_reg->DFCCTL, cfg->vldc_shift_value,
              CSL_ISIF_DFCCTL_VDFLSFT_SHIFT, CSL_ISIF_DFCCTL_VDFLSFT_MASK);
    ISP_SET32(isif_reg->DFCCTL, cfg->disable_vldc_upper_pixels,
              CSL_ISIF_DFCCTL_VDFCUDA_SHIFT, CSL_ISIF_DFCCTL_VDFCUDA_MASK);
    ISP_SET32(isif_reg->DFCCTL, cfg->vldc_mode_select,
              CSL_ISIF_DFCCTL_VDFCSL_SHIFT, CSL_ISIF_DFCCTL_VDFCSL_MASK);

    ISP_WRITE32(isif_reg->VDFSATLV, cfg->vldc_saturation_level);
    ISP_SET32(isif_reg->DFCMEMCTL, CSL_ISIF_DFCMEMCTL_DFCMCLR_CLEAR,
              CSL_ISIF_DFCMEMCTL_DFCMCLR_SHIFT,
              CSL_ISIF_DFCMEMCTL_DFCMCLR_MASK);
    ISP_SET32(isif_reg->DFCMEMCTL, CSL_ISIF_DFCMEMCTL_DFCMARST_CLR_ADDR,
              CSL_ISIF_DFCMEMCTL_DFCMARST_SHIFT,
              CSL_ISIF_DFCMEMCTL_DFCMARST_MASK);

    for (i = 0; i < cfg->defect_lines; i++)
    {
        ISP_WRITE32(isif_reg->DFCMEM0, line_params->vertical_defect_position);
        ISP_WRITE32(isif_reg->DFCMEM1, line_params->horizontal_defect_position);
        ISP_WRITE32(isif_reg->DFCMEM2, line_params->sub1_value_vldc);
        ISP_WRITE32(isif_reg->DFCMEM3, line_params->sub2_less_than_vldc);
        ISP_WRITE32(isif_reg->DFCMEM4, line_params->sub3_greater_than_vldc);

        /* update table for 1 v-line defect correction */
        line_params++;

        ISP_SET32(isif_reg->DFCMEMCTL, CSL_ISIF_DFCMEMCTL_DFCMWR_WRITE,
                  CSL_ISIF_DFCMEMCTL_DFCMWR_SHIFT,
                  CSL_ISIF_DFCMEMCTL_DFCMWR_MASK);

        /* wait for acknowledgement */

#if 0                                                      // debug debug
                                                           // simulation only
        while ((ISP_FEXT
                (isif_reg->DFCMEMCTL, CSL_ISIF_DFCMEMCTL_DFCMWR_SHIFT,
                 CSL_ISIF_DFCMEMCTL_DFCMWR_MASK)) !=
               CSL_ISIF_DFCMEMCTL_DFCMWR_WRITECOMPLETE)
        {;
        }
#endif
    }

    if (i < 8)
    {
        ISP_WRITE32(isif_reg->DFCMEM0, 0);
        ISP_WRITE32(isif_reg->DFCMEM1, 0xFFFF);
        ISP_SET32(isif_reg->DFCMEMCTL, CSL_ISIF_DFCMEMCTL_DFCMWR_WRITE,
                  CSL_ISIF_DFCMEMCTL_DFCMWR_SHIFT,
                  CSL_ISIF_DFCMEMCTL_DFCMWR_MASK);
    }

    ISP_SET32(isif_reg->DFCCTL, cfg->enable, CSL_ISIF_DFCCTL_VDFCEN_SHIFT,
              CSL_ISIF_DFCCTL_VDFCEN_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_2d_lsc() configures the 2d-Lens Shading Correction component  of  the ISIF module.
 *
 *  @param  cfg   isif_2dlsc_cfg_t * cfg  is a pointer to the data-structure of isif_2dlsc_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_2d_lsc
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
ISP_RETURN isif_config_2d_lsc(isif_2dlsc_cfg_t * cfg)
{

    ISP_ASSERT(cfg, NULL);
    if (cfg->enable)
    {
        ISP_WRITE32(isif_reg->DATAHOFST, cfg->hdirection_data_offset);
        ISP_WRITE32(isif_reg->DATAVOFST, cfg->vdirection_data_offset);

        ISP_WRITE32(isif_reg->LSCHVAL, cfg->lsc_hsize - 1);
        ISP_WRITE32(isif_reg->LSCVVAL, cfg->lsc_vsize - 1);

        ISP_SET32(isif_reg->DLSCINI, cfg->hposin_paxel,
                  CSL_ISIF_DLSCINI_X_SHIFT, CSL_ISIF_DLSCINI_X_MASK);
        ISP_SET32(isif_reg->DLSCINI, cfg->vposin_paxel,
                  CSL_ISIF_DLSCINI_Y_SHIFT, CSL_ISIF_DLSCINI_Y_MASK);

        ISP_SET32(isif_reg->DLSCCFG, cfg->pax_length,
                  CSL_ISIF_DLSCCFG_GAIN_MODE_M_SHIFT,
                  CSL_ISIF_DLSCCFG_GAIN_MODE_M_MASK);
        ISP_SET32(isif_reg->DLSCCFG, cfg->pax_height,
                  CSL_ISIF_DLSCCFG_GAIN_MODE_N_SHIFT,
                  CSL_ISIF_DLSCCFG_GAIN_MODE_N_MASK);
        ISP_SET32(isif_reg->DLSCCFG, cfg->gain_format,
                  CSL_ISIF_DLSCCFG_GAIN_FORMAT_SHIFT,
                  CSL_ISIF_DLSCCFG_GAIN_FORMAT_MASK);

        if ((cfg->gain_table_address) & 0x3)
        {
            return ISP_FAILURE;
        }

        else
        {
            ISP_WRITE32(isif_reg->DLSCGRBU,
                        (((cfg->gain_table_address) >> 16) & 0xFFFF));
            ISP_WRITE32(isif_reg->DLSCGRBL,
                        ((cfg->gain_table_address) & 0xFFFF));
        }

        ISP_WRITE32(isif_reg->DLSCGROF, cfg->gain_table_length);
        ISP_SET32(isif_reg->DLSCOFST, cfg->offset_scaling_factor,
                  CSL_ISIF_DLSCOFST_OFSTSF_SHIFT,
                  CSL_ISIF_DLSCOFST_OFSTSF_MASK);
        ISP_SET32(isif_reg->DLSCOFST, cfg->offset_shift_value,
                  CSL_ISIF_DLSCOFST_OFSTSFT_SHIFT,
                  CSL_ISIF_DLSCOFST_OFSTSFT_MASK);
        ISP_SET32(isif_reg->DLSCOFST, cfg->offset_enable,
                  CSL_ISIF_DLSCOFST_OFSTEN_SHIFT,
                  CSL_ISIF_DLSCOFST_OFSTEN_MASK);

        if (cfg->offset_table_address & 0x3)
        {
            return ISP_FAILURE;
        }

        else
        {
            ISP_WRITE32(isif_reg->DLSCORBU,
                        ((cfg->offset_table_address >> 16) & 0xFFFF));
            ISP_WRITE32(isif_reg->DLSCORBL,
                        (cfg->offset_table_address & 0xFFFF));
        }

        ISP_WRITE32(isif_reg->DLSCOROF, cfg->offset_table_length);
    }

    ISP_SET32(isif_reg->DLSCCFG, cfg->enable, CSL_ISIF_DLSCCFG_ENABLE_SHIFT,
              CSL_ISIF_DLSCCFG_ENABLE_MASK);
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isif_2dlsc_enable
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
ISP_RETURN isif_2dlsc_enable(uint8 enable)
{
    ISP_SET32(isif_reg->DLSCCFG, enable, CSL_ISIF_DLSCCFG_ENABLE_SHIFT,
              CSL_ISIF_DLSCCFG_ENABLE_MASK);
    return (ISP_SUCCESS);
}

/* ===================================================================
 *  @func     isif_2dlsc_sof_int_enable
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
ISP_RETURN isif_2dlsc_sof_int_enable(uint8 enable)
{
    ISP_SET32(isif_reg->DLSCIRQEN, enable, CSL_ISIF_DLSCIRQEN_SOF_SHIFT,
              CSL_ISIF_DLSCIRQEN_SOF_MASK);
    return (ISP_SUCCESS);
}

/* ===================================================================
 *  @func     isif_2dlsc_is_prefetched
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
uint8 isif_2dlsc_is_prefetched(void)
{
    uint8 res;

    res =
        ISP_FEXT(isif_reg->DLSCIRQST,
                 CSL_ISIF_DLSCIRQST_PREFETCH_COMPLETED_SHIFT,
                 CSL_ISIF_DLSCIRQST_PREFETCH_COMPLETED_MASK);
    ISP_SET32(isif_reg->DLSCIRQST, res,
              CSL_ISIF_DLSCIRQST_PREFETCH_COMPLETED_SHIFT,
              CSL_ISIF_DLSCIRQST_PREFETCH_COMPLETED_MASK);
    return (res);
}

/* ===================================================================
 *  @func     isif_config_pol
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
ISP_RETURN isif_config_pol(isif_ip_src_cfg_t * src_cfg)
{
    ISP_SET32(isif_reg->MODESET, src_cfg->hd_pol, CSL_ISIF_HDPOL_SHIFT,
              CSL_ISIF_HDPOL_MASK);
    ISP_SET32(isif_reg->MODESET, src_cfg->vd_pol, CSL_ISIF_VDPOL_SHIFT,
              CSL_ISIF_VDPOL_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_ip_data_formatter() configures the input data formatting settings of  the ISIF module.
 *
 *  @param  cfg  isif_ip_format_cfg_t * cfg      is a pointer to the data-structure of isif_ip_format_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_ip_data_formatter
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
ISP_RETURN isif_config_ip_data_formatter(isif_ip_format_cfg_t * cfg)
{
    volatile uint32 *reg_ptr;

    isif_ipfmt_line_init_t *cfg_ptr;

    isif_ipfmt_pgm_addr_ptr_select_t *cfg_ptr2;

    uint8 i;

    ISP_ASSERT(cfg, NULL);

    ISP_SET32(isif_reg->FMTCFG, cfg->combine_enable,
              CSL_ISIF_FMTCFG_FMTCBL_SHIFT, CSL_ISIF_FMTCFG_FMTCBL_MASK);
    ISP_SET32(isif_reg->FMTCFG, cfg->line_alternate,
              CSL_ISIF_FMTCFG_LNALT_SHIFT, CSL_ISIF_FMTCFG_LNALT_MASK);
    ISP_SET32(isif_reg->FMTCFG, cfg->split_combine_line,
              CSL_ISIF_FMTCFG_LNUM_SHIFT, CSL_ISIF_FMTCFG_LNUM_MASK);
    ISP_SET32(isif_reg->FMTCFG, cfg->address_increment_value,
              CSL_ISIF_FMTCFG_FMTAINC_SHIFT, CSL_ISIF_FMTCFG_FMTAINC_MASK);

    ISP_SET32(isif_reg->FMTPLEN, cfg->pgm_entry_length_set0,
              CSL_ISIF_FMTPLEN_FMTPLEN0_SHIFT, CSL_ISIF_FMTPLEN_FMTPLEN0_MASK);
    ISP_SET32(isif_reg->FMTPLEN, cfg->pgm_entry_length_set1,
              CSL_ISIF_FMTPLEN_FMTPLEN1_SHIFT, CSL_ISIF_FMTPLEN_FMTPLEN1_MASK);
    ISP_SET32(isif_reg->FMTPLEN, cfg->pgm_entry_length_set2,
              CSL_ISIF_FMTPLEN_FMTPLEN2_SHIFT, CSL_ISIF_FMTPLEN_FMTPLEN2_MASK);
    ISP_SET32(isif_reg->FMTPLEN, cfg->pgm_entry_length_set3,
              CSL_ISIF_FMTPLEN_FMTPLEN3_SHIFT, CSL_ISIF_FMTPLEN_FMTPLEN3_MASK);

    ISP_WRITE32(isif_reg->FMTSPH, cfg->h_pos);
    ISP_WRITE32(isif_reg->FMTLNH, cfg->h_size);
    ISP_WRITE32(isif_reg->FMTSLV, cfg->v_pos);
    ISP_WRITE32(isif_reg->FMTLNV, cfg->v_size);
    ISP_WRITE32(isif_reg->FMTRLEN, cfg->op_pixel_length);
    ISP_WRITE32(isif_reg->FMTHCNT, cfg->output_hd_interval_split);

    /* program output line and init value */
    reg_ptr = &(isif_reg->FMTAPTR0);
    cfg_ptr = cfg->line_init;

    for (i = 0; i < cfg->fmt_aptrs; i++)
    {
        ISP_SET32(*reg_ptr, cfg_ptr->output_line_addr,
                  CSL_ISIF_FMTAPTR0_LINE_SHIFT, CSL_ISIF_FMTAPTR0_LINE_MASK);
        ISP_SET32(*reg_ptr, cfg_ptr->init_addr, CSL_ISIF_FMTAPTR0_INIT_SHIFT,
                  CSL_ISIF_FMTAPTR0_INIT_MASK);
        reg_ptr++;
        cfg_ptr++;
    }

    /* program address pointer valid/skip */
    ISP_WRITE32(isif_reg->FMTPGMVF0, cfg->pgm_addr_enable_bit_map0);
    ISP_WRITE32(isif_reg->FMTPGMVF1, cfg->pgm_addr_enable_bit_map1);

    /* program address pointer auto increment/decrement config */
    ISP_WRITE32(isif_reg->FMTPGMAPU0, cfg->pgm_addr_update_bit_map0);
    ISP_WRITE32(isif_reg->FMTPGMAPU1, cfg->pgm_addr_update_bit_map1);

    /* program address pointer select from APTR0-16 */
    reg_ptr = &(isif_reg->FMTPGMAPS0);
    cfg_ptr2 = cfg->pgm_addr_ptr_sel;

    for (i = 0; i < 8; i++)
    {
        ISP_SET32(*reg_ptr, *cfg_ptr2, CSL_ISIF_FMTPGMAPS0_PGM0APTR_SHIFT,
                  CSL_ISIF_FMTPGMAPS0_PGM0APTR_MASK);
        cfg_ptr2++;
        ISP_SET32(*reg_ptr, *cfg_ptr2, CSL_ISIF_FMTPGMAPS0_PGM1APTR_SHIFT,
                  CSL_ISIF_FMTPGMAPS0_PGM1APTR_MASK);
        cfg_ptr2++;
        ISP_SET32(*reg_ptr, *cfg_ptr2, CSL_ISIF_FMTPGMAPS0_PGM2APTR_SHIFT,
                  CSL_ISIF_FMTPGMAPS0_PGM2APTR_MASK);
        cfg_ptr2++;
        ISP_SET32(*reg_ptr, *cfg_ptr2, CSL_ISIF_FMTPGMAPS0_PGM3APTR_SHIFT,
                  CSL_ISIF_FMTPGMAPS0_PGM3APTR_MASK);
        cfg_ptr2++;
        reg_ptr++;
    }
    ISP_SET32(isif_reg->FMTCFG, cfg->enable, CSL_ISIF_FMTCFG_FMTEN_SHIFT,
              CSL_ISIF_FMTCFG_FMTEN_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_color_space_converter() configures the Color-Space-Converter component of  the ISIF module.
 *
 *  @param  cfg   isif_csc_cfg_t * cfg     is a pointer to the data-structure of isif_csc_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_color_space_converter
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
ISP_RETURN isif_config_color_space_converter(isif_csc_cfg_t * cfg)
{
    uint8 i;

    volatile uint32 *reg_ptr;

    ISP_ASSERT(cfg, NULL);

    ISP_WRITE32(isif_reg->CSCCTL, cfg->enable);

    if (cfg->enable)
    {
        uint8 *csc_melement_ptr;

        reg_ptr = &(isif_reg->CSCM0);
        csc_melement_ptr = (uint8 *) cfg->csc_coef;

        for (i = 0; i < 8; i++)
        {
            ISP_SET32(*reg_ptr, *csc_melement_ptr, CSL_ISIF_CSCM0_CSCM00_SHIFT,
                      CSL_ISIF_CSCM0_CSCM00_MASK);
            csc_melement_ptr++;
            ISP_SET32(*reg_ptr, *csc_melement_ptr, CSL_ISIF_CSCM0_CSCM01_SHIFT,
                      CSL_ISIF_CSCM0_CSCM01_MASK);
            reg_ptr++;
            csc_melement_ptr++;
        }
    }

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_config_black_clamp() configures the black-clamp  component settings  of  the ISIF module.
 *
 *  @param  cfg  isif_clamp_cfg_t * cfg      is a pointer to the data-structure of isif_clamp_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config_black_clamp
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
ISP_RETURN isif_config_black_clamp(isif_clamp_cfg_t * cfg)
{
    ISP_ASSERT(cfg, NULL);

    ISP_SET32(isif_reg->CLAMPCFG, cfg->hclamp_mode,
              CSL_ISIF_CLAMPCFG_CLHMD_SHIFT, CSL_ISIF_CLAMPCFG_CLHMD_MASK);
    ISP_SET32(isif_reg->CLAMPCFG, cfg->black_clamp_mode,
              CSL_ISIF_CLAMPCFG_CLMD_SHIFT, CSL_ISIF_CLAMPCFG_CLMD_MASK);

#if defined(WDR_ON) & (defined(IMGS_MICRON_MT9M034) | defined(IMGS_SONY_IMX104) | defined(IMGS_SONY_IMX136)| defined(IMGS_SONY_IMX140) | defined(IMGS_MICRON_AR0331))
	ISP_WRITE32(isif_reg->CLDCOFST, 0);
#else
    ISP_WRITE32(isif_reg->CLDCOFST, cfg->dcoffset_clamp_value);
#endif
    ISP_WRITE32(isif_reg->CLSV, cfg->black_clamp_v_start_pos);

    ISP_WRITE32(isif_reg->CLVRV, cfg->vertical_black.reset_value);
    ISP_WRITE32(isif_reg->CLVWIN1, cfg->vertical_black.h_pos);
    ISP_WRITE32(isif_reg->CLVWIN2, cfg->vertical_black.v_pos);
    ISP_WRITE32(isif_reg->CLVWIN3, cfg->vertical_black.v_size);
    ISP_SET32(isif_reg->CLVWIN0, cfg->vertical_black.h_size,
              CSL_ISIF_CLVWIN0_CLVOBH_SHIFT, CSL_ISIF_CLVWIN0_CLVOBH_MASK);
    ISP_SET32(isif_reg->CLVWIN0, cfg->vertical_black.line_avg_coef,
              CSL_ISIF_CLVWIN0_CLVCOEF_SHIFT, CSL_ISIF_CLVWIN0_CLVCOEF_MASK);
    ISP_SET32(isif_reg->CLVWIN0, cfg->vertical_black.reset_mode,
              CSL_ISIF_CLVWIN0_CLVRVSL_SHIFT, CSL_ISIF_CLVWIN0_CLVRVSL_MASK);

    ISP_SET32(isif_reg->CLHWIN0, cfg->horizontal_black.pixel_value_limit,
              CSL_ISIF_CLHWIN0_CLHLMT_SHIFT, CSL_ISIF_CLHWIN0_CLHLMT_MASK);
    ISP_SET32(isif_reg->CLHWIN0, cfg->horizontal_black.right_window,
              CSL_ISIF_CLHWIN0_CLHWBS_SHIFT, CSL_ISIF_CLHWIN0_CLHWBS_MASK);
    ISP_SET32(isif_reg->CLHWIN0, cfg->horizontal_black.window_count_per_color,
              CSL_ISIF_CLHWIN0_CLHWC_SHIFT, CSL_ISIF_CLHWIN0_CLHWC_MASK);
    ISP_SET32(isif_reg->CLHWIN0, cfg->horizontal_black.v_size,
              CSL_ISIF_CLHWIN0_CLHWN_SHIFT, CSL_ISIF_CLHWIN0_CLHWN_MASK);
    ISP_SET32(isif_reg->CLHWIN0, cfg->horizontal_black.h_size,
              CSL_ISIF_CLHWIN0_CLHWM_SHIFT, CSL_ISIF_CLHWIN0_CLHWM_MASK);

    ISP_WRITE32(isif_reg->CLHWIN1, cfg->horizontal_black.h_pos);
    ISP_WRITE32(isif_reg->CLHWIN2, cfg->horizontal_black.v_pos);

    ISP_SET32(isif_reg->CLAMPCFG, cfg->enable, CSL_ISIF_CLAMPCFG_CLEN_SHIFT,
              CSL_ISIF_CLAMPCFG_CLEN_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_init() initializes the ISIF module. It also sets  the CSL structure to the ISIF base address.
 *
 *  @param
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_init
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
ISP_RETURN isif_init()
{
    /* This memory can be malloc'ed */
    isif_devp = &isif_dev;

    isif_devp->opened = FALSE;
    isif_reg = (isif_regs_ovly) ISIF_BASE_ADDRESS;
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  isif_start() configures the running mode of  the ISIF module.
 *
 *  @param  enable  is the start mode of ISIF, specified through data-structure isif_start_t
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_start
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
ISP_RETURN isif_start(isif_start_t enable)
{

    int status = ISP_SUCCESS;

    switch (enable)
    {
        case ISIF_STOP:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_DISABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_SYEN_DISABLE,
                      CSL_ISIF_SYNCEN_SYEN_SHIFT, CSL_ISIF_SYNCEN_SYEN_MASK);
            break;

        case ISIF_STOP_WRITE:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_DISABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            break;

            // break;
        case ISIF_START_SYNC:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_DISABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_SYEN_ENABLE,
                      CSL_ISIF_SYNCEN_SYEN_SHIFT, CSL_ISIF_SYNCEN_SYEN_MASK);
            break;

        case ISIF_START_WRITE:
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_DWEN_ENABLE,
                      CSL_ISIF_SYNCEN_DWEN_SHIFT, CSL_ISIF_SYNCEN_DWEN_MASK);
            ISP_SET32(isif_reg->SYNCEN, CSL_ISIF_SYNCEN_SYEN_ENABLE,
                      CSL_ISIF_SYNCEN_SYEN_SHIFT, CSL_ISIF_SYNCEN_SYEN_MASK);
            break;

        default:
            status = ISP_INVALID_INPUT;
    }

    return (status);

}

/* ================================================================ */
/**
 *  isif_close() stops the operation of ISIF module by gating the clock.
 *
 *  @param
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */
/* ===================================================================
 *  @func     isif_close
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
ISP_RETURN isif_close()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (isif_devp->opened == FALSE)
    {

        return ISP_FAILURE;

    }

    isif_reg = NULL;

    retval = isp_common_disable_clk(ISP_ISIF_CLK);

    isif_devp->opened = FALSE;
    return retval;
}

/* ================================================================ */
/**
 *  isif_open() enables the clock for the ISIF module.
 *
 *  @param  cfg        is a pointer to the data-structure of isif_gain_offset_cfg_t type which encapsulates the configuration settings
 *  @return  status   Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_open
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
ISP_RETURN isif_open()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (isif_devp->opened == TRUE)
    {
        return ISP_FAILURE;
    }

    isif_devp->opened = TRUE;
    isif_reg = (isif_regs_ovly) ISIF_BASE_ADDRESS;

    retval = isp_common_enable_clk(ISP_ISIF_CLK);

    if (retval == ISP_SUCCESS)
    {
        // retval=isp_common_set_interrupt_handler(ISP_IPIPE_INT_REG ,
        // ipipe_reg_update_callback,
        // 0,0 );
    }

    return retval;

}

/* ================================================================ */
/**
 *  isif_config() is the top-level configuration function to configure the ISIF module.
 *
 *  @param  isif_config  isif_cfg_t * isif_config      is a pointer to the data-structure of isif_cfg_t type which encapsulates the configuration settings
 *  @return  status              Success/failure of the operation
 */
/*================================================================== */

/* ===================================================================
 *  @func     isif_config
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
ISP_RETURN isif_config(isif_cfg_t * isif_config)
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (ISIF_INPUT_FLAG & isif_config->feature_flag)
    {
        retval = isif_config_input_src(isif_config->src_cfg);
    }

    if (((isif_config->feature_flag) & ISIF_LINEN_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            isif_config_sensor_linearization(isif_config->linearization_params);
    }

    if (((isif_config->feature_flag) & ISIF_IP_DATA_FORMATTER_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            isif_config_ip_data_formatter(isif_config->ip_data_format_params);
    }

    if (((isif_config->feature_flag) & ISIF_CSC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_color_space_converter(isif_config->csc_params);
    }

    if (((isif_config->feature_flag) & ISIF_BLACK_CLAMP_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_black_clamp(isif_config->digital_clamp_params);
    }

    if (((isif_config->feature_flag) & ISIF_VLDC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval =
            isif_config_vertical_line_defect_correction(isif_config->
                                                        vldc_params);
    }

    if (((isif_config->feature_flag) & ISIF_LSC_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_2d_lsc(isif_config->lsc_params);
    }

    if (((isif_config->feature_flag) & ISIF_GAIN_OFFSET_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_gain_offset(isif_config->gain_offset_params);
    }

    if (((isif_config->feature_flag) & ISIF_SDRAM_OP_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_sdram_format(isif_config->sdram_op_params);

        if (((isif_config->feature_flag) & ISIF_HLPF_FLAG) &&
            (retval != ISP_FAILURE))
        {
            retval = isif_config_lpf(1);
        }
        else
        {
            retval = isif_config_lpf(0);
        }

        if (((isif_config->feature_flag) & ISIF_ALAW_FLAG) &&
            (retval != ISP_FAILURE))
        {
            retval = isif_config_alaw_compression(1);
        }
        else
        {
            retval = isif_config_alaw_compression(0);
        }

        if (((isif_config->feature_flag) & ISIF_HVCULLING_FLAG) &&
            (retval != ISP_FAILURE))
        {
            retval =
                isif_config_hv_culling(&isif_config->sdram_op_params->
                                       culling_params);
        }
        else
        {
            retval = isif_config_hv_culling_disable();
        }

        if (((isif_config->feature_flag) & ISIF_DPCM_FLAG) &&
            (retval != ISP_FAILURE))
        {
            retval =
                isif_config_dpcm_encoder(isif_config->sdram_op_params->
                                         dpcm_predictor,
                                         isif_config->sdram_op_params->
                                         dpcm_enable);
        }
    }

    if (((isif_config->feature_flag) & ISIF_FLASH_TIMING_FLAG) &&
        (retval != ISP_FAILURE))
    {
        retval = isif_config_timing_generator(isif_config->flash_timing_params);
    }

    if (ISIF_VD_FLAG & isif_config->feature_flag)
    {
        isif_config_vd_line_numbers(&isif_config->vd);
    }
    return ISP_SUCCESS;

}
