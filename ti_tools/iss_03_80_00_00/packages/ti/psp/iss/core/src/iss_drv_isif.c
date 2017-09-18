/** ==================================================================
 *  @file   iss_drv_isif.c                                                  
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

const isif_ip_src_cfg_t default_preview_isif_src_cfg = {
    ISIF_HD_VD_DISABLE,                                    // ISIF_SYEN_START_T 
                                                           // sync_enable;
                                                           // //SYEN of
                                                           // SYNCEN
    ISIF_DATA_INPUT_MODE_CCD_RAW,                          // ISIF_DATA_INPUT_MODE_T 
                                                           // ip_pix_fmt;
                                                           // //INPMOD of
                                                           // MODESET
    ISIF_DATA_NORMAL,                                      // ISIF_DATA_POLARITY_T 
                                                           // data_polarity;
                                                           // //DPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // field_pol;
                                                           // //FIPOL of
                                                           // MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // hd_pol; //HDPOL 
                                                           // of MODESET
    ISIF_NEGATIVE_POLARITY,                                // ISIF_POLARITY_T 
                                                           // vd_pol; //VDPOL 
                                                           // of MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // field_dir;
                                                           // //FIDD of
                                                           // MODESET
    ISIF_SIGNAL_DIRECTION_OUTPUT,                          // ISIF_SIGNAL_DIR_T 
                                                           // hd_vd_dir;
                                                           // //HDVDD of
                                                           // MODESET
    0,                                                     // uint16
                                                           // hd_width; //HDW 
                                                           // OF HDW
    0,                                                     // uint16
                                                           // vd_width; //VDW 
                                                           // OF VDW
    1,                                                     // 639, // uint16
                                                           // ppln_hs_interval; 
                                                           // //PPLN
    1,                                                     // 479, // uint16
                                                           // lpfr_vs_interval; 
                                                           // //LPFR
    ISIF_YCINSWAP_DISABLE,                                 // ISIF_YCINSWAP_T 
                                                           // yc_in_swap;
                                                           // //CCDCFG
                                                           // YCINSWP
    ISIF_MSB_INVERSE_CIN_DISABLE,                          // ISIF_MSB_INVERSE_CIN_T 
                                                           // msb_inverse_cin; 
                                                           // //CCDCFG
#ifdef APPRO_SENSOR_VENDOR
#ifdef IMGS_SONY_IMX136
	ISIF_GAIN_MSB_BIT13,
#else
    ISIF_GAIN_MSB_BIT9,                                    // ISIF_GAIN_MSB_BIT11, 
#endif	// modify by jem
#else                                                           // //
    ISIF_GAIN_MSB_BIT9,                                    // ISIF_GAIN_MSB_BIT11, 
#endif
                                                           // ISIF_GAIN_MSB_POS_T 
                                                           // ip_data_msb_pos; 
                                                           // // MSB position
    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field1_pixel_pos[4]; 
                                                           // //CPx_F1 CCOLP
    ISIF_COLOR_POSITION_GR_CY,
    ISIF_COLOR_POSITION_GR_G,
    ISIF_COLOR_POSITION_B_MG,

    ISIF_COLOR_POSITION_R_YE,                              // ISIF_COLOR_PATTERN_POS_T 
                                                           // field0_pixel_pos[4]; 
                                                           // //CPx_F0 CCOLP
    ISIF_COLOR_POSITION_GR_CY,
    ISIF_COLOR_POSITION_GR_G,
    ISIF_COLOR_POSITION_B_MG,

    ISIF_CFA_PATTERN_MOSAIC,                               // ISIF_CFA_PATTERN_T 
                                                           // cfa_pattern;
                                                           // //CGAMMAWD CFAP

    ISIF_REC656IF_MODE_DISABLE,                            // ISIF_REC656IF_MODE_T 
                                                           // r656_inferface_mode; 
                                                           // //REC656IF
                                                           // R656ON
    ISIF_ERR_CORRECTION_FVC_DISABLE,                       // ISIF_ERR_CORRECTION_FVC_T 
                                                           // err_correction_mode; 
                                                           // //REC656IF
                                                           // ECCFVH
    ISIF_BW656_DISABLE,                                    // ISIF_BW656_T
                                                           // ccir656_bit_width; 
                                                           // //CCDCFG BW656
};

const isif_ipfmt_pgm_addr_ptr_select_t default_preview_pgm_addr_ptr_sel[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15
};

const isif_ipfmt_line_init_t default_preview_line_init[] = {
    0x4, 0x111,
    0x3, 0x222,
    0x2, 0x333,
    0x1, 0x444,

    0x8, 0x555,
    0x7, 0x666,
    0x6, 0x777,
    0x5, 0x888,

    0x5, 0x888,
    0x6, 0x777,
    0x7, 0x666,
    0x8, 0x555,

    0x1, 0x444,
    0x2, 0x333,
    0x3, 0x222,
    0x4, 0x111,
};

/*****************/
const isif_ip_format_cfg_t default_preview_ip_data_format_params = {
    ISIF_CCD_FORMAT_DISABLE,                               // ISIF_CCD_IPFMT_T 
                                                           // enable; //FMTEN 
                                                           // FMTCFG
    ISIF_INPUT_SPLIT,                                      // ISIF_IPFMT_COMBINE_SPLIT_T 
                                                           // combine_enable; 
                                                           // //FMTCBL FMTCFG
    ISIF_NORMAL_SCAN_MODE,                                 // ISIF_SCAN_MODE_T 
                                                           // line_alternate; 
                                                           // //LNALT FMTCFG
    0,                                                     // uint8
                                                           // split_combine_line; 
                                                           // //LNUM FMTCFG
    0,                                                     // uint8
                                                           // address_increment_value; 
                                                           // //FMTAINC
                                                           // FMTCFG
    // 
    0,                                                     // uint8
                                                           // pgm_entry_length_set0; 
                                                           // //FMTPLEN0
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set1; 
                                                           // //FMTPLEN1
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set2; 
                                                           // //FMTPLEN2
                                                           // FMTPLEN
    0,                                                     // uint8
                                                           // pgm_entry_length_set3; 
                                                           // //FMTPLEN3
                                                           // FMTPLEN
    // 
    0,                                                     // uint16
                                                           // h_pos;//ip_first_pixel; 
                                                           // //FMTSPH
    0,                                                     // uint16
                                                           // h_size;//ip_pixel_length; 
                                                           // //FMTLNH
    0,                                                     // uint16
                                                           // v_pos;//ip_first_line; 
                                                           // //FMTSLV
    0,                                                     // uint16
                                                           // v_size;//ip_line_length; 
                                                           // //FMTLNV
    0,                                                     // uint16
                                                           // op_pixel_length; 
                                                           // //FMTRLEN
    0,                                                     // uint16
                                                           // output_hd_interval_split; 
                                                           // //FMTHCNT
    // 
    0,                                                     // uint8
                                                           // fmt_aptrs; //
                                                           // FMTAPTR used
                                                           // from FMTAPTR 0
                                                           // to fmt_aptrs,
                                                           // max 16 
    (isif_ipfmt_line_init_t *) & default_preview_line_init[0],  // isif_ipfmt_line_init_t* 
                                                                // line_init; 
                                                                // //FMTAPTR0 
                                                                // -
                                                                // FMTAPTR15
    // 
    0,                                                     // uint16
                                                           // pgm_addr_enable_bit_map0; 
                                                           // //PGM15EN-PGM0EN 
                                                           // FMTPGMVF0
    0,                                                     // uint16
                                                           // pgm_addr_enable_bit_map1; 
                                                           // //PGM31EN-PGM16EN 
                                                           // FMTPGMVF1
    // 
    0,                                                     // uint16
                                                           // pgm_addr_update_bit_map0; 
                                                           // //PGM15UPDT-PGM0UPDT 
                                                           // FMTPGMAPU0
    0,                                                     // uint16
                                                           // pgm_addr_update_bit_map1; 
                                                           // //PGM31UPDT-PGM0UPDT 
                                                           // FMTPGMAPU1
    // 12, // uint8 pgm_aptrs; //no of pgm address ptrs used 0 to 32
    (isif_ipfmt_pgm_addr_ptr_select_t *) & default_preview_pgm_addr_ptr_sel[0]  // isif_ipfmt_pgm_addr_ptr_select_t* 
                                                                                // pgm_addr_ptr_sel; 
                                                                                // //PGMXXAPTR 
                                                                                // - 
                                                                                // FMTPGMAPS7 
                                                                                // FMTPGMAPS0 
                                                                                // must 
                                                                                // be 
                                                                                // array 
                                                                                // of 
                                                                                // 32
};

isif_sdram_op_cfg_t default_preview_sdram_op_params = {
    0,                                                     // uint16
                                                           // sdram_hpos;
                                                           // //first pixel
                                                           // in line SPH
    0,                                                     // uint16
                                                           // sdram_hsize;
                                                           // //no of pixels
                                                           // in line = LNH+1
    0,                                                     // uint16
                                                           // sdram_vpos0;
                                                           // //wrt to VD,
                                                           // output to
                                                           // SDRAM, SLV0
                                                           // field 0
    0,                                                     // uint16
                                                           // sdram_vpos1;
                                                           // //wrt to VD,
                                                           // SLV1 field 1
    0,                                                     // uint16
                                                           // sdram_vsize;
                                                           // //no of line =
                                                           // LNV+1

    ISIF_HLINE_ADDR_INCREMENT,                             // ISIF_HLINE_ADDR_T 
                                                           // memory_addr_decrement; 
                                                           // //ADCR, HSIZE 
    0,                                                     // uint16
                                                           // memory_addr_offset; 
                                                           // //memory
                                                           // address offset
                                                           // . line, HSIZE

    ISIF_FID_NO_CHANGE,                                    // SIF_FID_POLARITY 
                                                           // fid_polarity;
                                                           // //FIINV OF
                                                           // SDOFST
    ISIF_FIELD_LINE_OFFSET_PLUS_1,                         // ISIF_FIELD_LINE_OFFSET_VAL_T 
                                                           // odd_field_line_offset; 
                                                           // //FOFST OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_even_field_offset; 
                                                           // //LOFSTEE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_even_field_offset; 
                                                           // //LOFSTOE OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // even_line_odd_field_offset; 
                                                           // //LOFSTEO OF
                                                           // SDOFST
    ISIF_LINE_OFFSET_PLUS_1,                               // ISIF_LINE_OFFSET_VAL_T 
                                                           // odd_line_odd_field_offset; 
                                                           // //LOFTOO OF
                                                           // SDOFST
    0x00000000,                                            // uint32
                                                           // memory_addr;
                                                           // //(CADU+CADL)<<5 
                                                           // 32byte aligned

    ISIF_LPF_ANTI_ALIAS_DISABLE,                           // ISIF_HPLF_CTRL_T 
                                                           // alias_filter_enable; 
                                                           // //HLPF of
                                                           // MODESET
    ISIF_DPCM_ENCODE_PREDICTOR2,                           // ISIF_DPCM_ENCODE_PREDICTOR_TYP_T 
                                                           // dpcm_predictor; 
                                                           // //DPCMPRE MISC
    ISIF_DPCM_ENCODER_ENABLE_OFF,                          // ISIF_DPCM_ENCODER_ENABLE_TYPE 
                                                           // dpcm_enable;
    ISIF_CCD_DATA_NO_SHIFT,                           	   // ISIF_CCD_DATA_SHIFT_SDRAM_T 
                                                           // ccd_raw_shift_value; 
                                                           // //CCDW of
                                                           // MODESET
    ISIF_CCD_MODE_NON_INTERLACED,                          // ISIF_CCD_MODE_T 
                                                           // ccd_field_mode; 
                                                           // //CCDMD of
                                                           // MODESET
    ISIF_NO_WEN,                                           // ISIF_SWEN_MODE_T 
                                                           // wen_selection_mode; 
                                                           // //SWEN of
                                                           // MODESET
    ISIF_DATA_WRITE_DISABLE,                               // ISIF_DATA_WRITE_START_T 
                                                           // data_write_enable; 
                                                           // //DWEN of
                                                           // SYNCEN
    ISIF_MSB_INVERSE_COUT_DISABLE,                         // ISIF_MSB_INVERSE_COUT_T 
                                                           // msb_inverse_cout; 
                                                           // //CCDCFG
                                                           // MSBINVO
    ISIF_BYTE_SWAP_DISABLE,                                // ISIF_BYTE_SWAP_T 
                                                           // sdram_byte_swap; 
                                                           // //CCDCFG BSWD
    ISIF_SELECT_Y_POS_EVEN_PX,                             // ISIF_SELECT_Y_POS_T 
                                                           // y_position;
                                                           // //CCDCFG Y8POS
    ISIF_NO_EXTRG,                                         // ISIF_EXTRG_T
                                                           // sdram_ext_trig; 
                                                           // //CCDCFG EXTRG
    ISIF_SDRAM_TRGSEL_DWEN_REG,                            // ISIF_SDRAM_TRGSEL_T 
                                                           // sdram_trig_select; 
                                                           // //CCDCFG TRGSEL
    ISIF_WENLOG_SIGNAL_LOGICAL_AND_INTERNAL,               // ISIF_WENLOG_SIGNAL_T 
                                                           // ccd_valid_log;
                                                           // //CCDCFG WENLOG
    ISIF_FID_LATCH_VSYNC,                                  // ISIF_FID_DETECTION_MODE_T 
                                                           // fid_detection_mode; 
                                                           // //CCDCFG FIDMD
    ISIF_YCOUTSWAP_DISABLE,                                // ISIF_YCOUTSWAP_T 
                                                           // yc_out_swap;
                                                           // //CCDCFG
                                                           // YCOUTSWP
    ISIF_SDRAM_PACK_16BITS,                                // ISIF_SDRAM_PACK_T 
                                                           // sdram_pack_fmt; 
                                                           // //CCDCFG
                                                           // SDRPACK
    ISIF_VLDC_RESYNC_VSYNC_ENABLE,                         // ISIF_VLDC_RESYNC_VSYNC_T 
                                                           // vsync_enable;
                                                           // //CCDCFG VLDC
                                                           // shadow reg or
                                                           // busy update??

    0xFF,                                                  // isif_culling_cfg_t 
                                                           // culling_params; 
                                                           // //Programmable
                                                           // H/V culling
                                                           // pattern
    0xFF,
    0xFF
};

isif_sdram_op_cfg_t capture_sdram_op_params;

isif_sensor_linear_cfg_t default_preview_linearization_params = {
    0,                                                     // uint8 enable;
                                                           // //LINEN LINCFG0
    ISIF_LINEAR_5_LEFT_SHIFT_VALUE,                        // ISIF_LINEAR_SHIFT_VAL_T 
                                                           // correction_shift; 
                                                           // //CORRSFT
                                                           // LINCFG0
    ISIF_LINEAR_NON_UNIFORM_SAMPLING,                      // ISIF_LINEAR_SAMPLING_T 
                                                           // non_uniform_mode; 
                                                           // //LINMD LINCFG0
    0xff,                                                  // uint16
                                                           // scale_value;
                                                           // //LUTSCL
                                                           // LINCFG1 u11q10
};

static const ISIF_COLOR_PATTERN_POSITION isif_drv_color_pattern[4][4] = {   // ISIF_COLOR_PATTERN_POS_T 
                                                                            // field0_pixel_pos[4]; 
                                                                            // //CPx_F0 
                                                                            // CCOLP
    ISIF_COLOR_POSITION_R_YE, ISIF_COLOR_POSITION_GR_CY,
        ISIF_COLOR_POSITION_GR_G, ISIF_COLOR_POSITION_B_MG,
    ISIF_COLOR_POSITION_GR_CY, ISIF_COLOR_POSITION_R_YE,
        ISIF_COLOR_POSITION_B_MG, ISIF_COLOR_POSITION_GR_G,
    ISIF_COLOR_POSITION_GR_G, ISIF_COLOR_POSITION_B_MG,
        ISIF_COLOR_POSITION_R_YE, ISIF_COLOR_POSITION_GR_CY,
    ISIF_COLOR_POSITION_B_MG, ISIF_COLOR_POSITION_GR_G,
        ISIF_COLOR_POSITION_GR_CY, ISIF_COLOR_POSITION_R_YE,
};

/* ===================================================================
 *  @func     IssConvertLscScaleToInt                                               
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
uint32 IssConvertLscScaleToInt(ISIF_LSC_GAIN_MODE_MN scale)
{
    register uint32 nResult;

    switch (scale)
    {
        case ISIF_LSC_8_PIXEL:
            nResult = 8;
            break;
        case ISIF_LSC_16_PIXEL:
            nResult = 16;
            break;
        case ISIF_LSC_32_PIXEL:
            nResult = 32;
            break;
        case ISIF_LSC_64_PIXEL:
            nResult = 64;
            break;
        case ISIF_LSC_128_PIXEL:
            nResult = 128;
            break;
        default:
            // TODO: Report error here!!!
            nResult = 64;
            break;
    }
    return (nResult);
}

/* ===================================================================
 *  @func     IssIsifCopyAndSetLscTable                                               
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
void IssIsifCopyAndSetLscTable(uint32 new_table_ptr, isif_2dlsc_cfg_t * lsc_cfg,
                               iss_config_processing_t * iss_config,
                               ipipe_size_cfg_t * ipipe_size_cfg)
{
    uint32 lsc_decim_x;

    uint32 lsc_decim_y;

    uint32 new_x_oft;

    uint32 new_y_oft;

    uint32 new_table_rows;

    uint32 old_table_rows;

    // iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_2DLSC;
    memcpy(lsc_cfg, iss_config->lsc_2d, sizeof(isif_2dlsc_cfg_t));

    lsc_decim_x = IssConvertLscScaleToInt(lsc_cfg->pax_length);
    lsc_decim_y = IssConvertLscScaleToInt(lsc_cfg->pax_height);

    new_x_oft = (ipipe_size_cfg->ipipe_if.sizes.h_start / lsc_decim_x);
    new_y_oft = (ipipe_size_cfg->ipipe_if.sizes.v_start / lsc_decim_y);

    (lsc_cfg)->hposin_paxel =
        ipipe_size_cfg->ipipe_if.sizes.h_start % lsc_decim_x;
    (lsc_cfg)->vposin_paxel =
        ipipe_size_cfg->ipipe_if.sizes.v_start % lsc_decim_y;

    old_table_rows = (((lsc_cfg)->lsc_vsize + lsc_decim_y - 1) / lsc_decim_y);
    new_table_rows =
        ((ipipe_size_cfg->ipipe_if.sizes.v_size_crop + (lsc_cfg)->vposin_paxel +
          lsc_decim_y - 1) / lsc_decim_y);

    if (new_table_rows < old_table_rows)
    {
        memcpy(((void *) new_table_ptr),
               (((uint8 *) (lsc_cfg)->gain_table_address) +
                new_y_oft * (lsc_cfg)->gain_table_length + 4 * new_x_oft),
               (lsc_cfg)->gain_table_length * (new_table_rows + 1));
    }
    else
    {
        memcpy(((void *) new_table_ptr),
               (((uint8 *) (lsc_cfg)->gain_table_address) +
                new_y_oft * (lsc_cfg)->gain_table_length + 4 * new_x_oft),
               (lsc_cfg)->gain_table_length * old_table_rows);
        // Workaround for cases when table height is matching image height
        memcpy((void *) ((uint8 *) new_table_ptr +
                         (lsc_cfg)->gain_table_length * old_table_rows),
               (void *) ((uint8 *) new_table_ptr +
                         (lsc_cfg)->gain_table_length * (old_table_rows - 1)),
               256);
    }

    (lsc_cfg)->gain_table_address = new_table_ptr;

    if ((lsc_cfg)->lsc_hsize > ipipe_size_cfg->ipipe_if.sizes.h_size_crop)
    {
        (lsc_cfg)->lsc_hsize = ipipe_size_cfg->ipipe_if.sizes.h_size_crop;
    }
    if ((lsc_cfg)->lsc_vsize > ipipe_size_cfg->ipipe_if.sizes.v_size_crop)
    {
        (lsc_cfg)->lsc_vsize = ipipe_size_cfg->ipipe_if.sizes.v_size_crop;
    }
}

/* ===================================================================
 *  @func     IssIsifConfig2dLsc                                               
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
void IssIsifConfig2dLsc(isif_2dlsc_cfg_t ** lsc_cfg,
                        iss_config_processing_t * iss_config)
{
    iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_2DLSC;
    *lsc_cfg = (isif_2dlsc_cfg_t *) iss_config->lsc_2d;
}

/* ===================================================================
 *  @func     IssIsifConfigFilters                                               
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
void IssIsifConfigFilters(isif_cfg_t * cfg,
                          iss_config_processing_t * iss_config)
{
    cfg->feature_flag = 0;
    // cfg->feature_flag|= ISIF_CSC_FLAG ;
    // cfg->csc_params.enable = 1;
    // cfg->csc_params.csc_coef = &default_preview_csc_coef;
    /* 
     * if(PROC_ISIF_VALID_2DLSC & iss_config->f_proc_isif_validity) {
     * iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_2DLSC;
     * cfg->feature_flag |= ISIF_LSC_FLAG; cfg->lsc_params =
     * (isif_2dlsc_cfg_t*)iss_config->lsc_2d; } */
    if (PROC_ISIF_VALID_DC & iss_config->f_proc_isif_validity)
    {
        iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_DC;
        cfg->feature_flag |= ISIF_BLACK_CLAMP_FLAG;
        cfg->digital_clamp_params = (isif_clamp_cfg_t *) iss_config->clamp;
    }
    if (PROC_ISIF_VALID_FLASHT & iss_config->f_proc_isif_validity)
    {
        iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_FLASHT;
        cfg->feature_flag |= ISIF_FLASH_TIMING_FLAG;
        cfg->flash_timing_params = (isif_flash_cfg_t *) iss_config->flash;
    }
    if (PROC_ISIF_VALID_GOFFSET & iss_config->f_proc_isif_validity)
    {
        iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_GOFFSET;
        cfg->feature_flag |= ISIF_GAIN_OFFSET_FLAG;
        cfg->gain_offset_params =
            (isif_gain_offset_cfg_t *) iss_config->gain_offset;
    }
    if (PROC_ISIF_VALID_VLCD & iss_config->f_proc_isif_validity)
    {
        iss_config->f_proc_isif_validity &= ~PROC_ISIF_VALID_VLCD;
        cfg->feature_flag |= ISIF_VLDC_FLAG;
        cfg->vldc_params = (isif_vldc_cfg_t *) iss_config->vlcd;
    }
    // iss_config->f_proc_isif_validity = PROC_ISIF_VALID_NO;

}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures ISIF to read from ipipeif and put data in RAM at the address "img_buff"
 *  contained in the structure field "sdram_op_params", the flags in "feature_flag" enable different features in isif.
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     IssIsifParamsInitPreview                                               
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
ISP_RETURN IssIsifParamsInitPreview(isif_cfg_t * cfg, in_frame_sizes_t * sizes,
                                    iss_config_params_t * iss_params)
{

    IssIsifConfigFilters(cfg, iss_params->iss_config);

    // cfg->feature_flag |= ISIF_IP_DATA_FORMATTER_FLAG;
    cfg->ip_data_format_params =
        (isif_ip_format_cfg_t *) & default_preview_ip_data_format_params;

    cfg->feature_flag |= ISIF_SDRAM_OP_FLAG;
    // cfg->feature_flag |= ISIF_HLPF_FLAG;
    // cfg->feature_flag |= ISIF_ALAW_FLAG;
    // cfg->feature_flag |= ISIF_DPCM_FLAG;
    // cfg->feature_flag |= ISIF_HVCULLING_FLAG;
    cfg->sdram_op_params = &default_preview_sdram_op_params;
    default_preview_sdram_op_params.vsync_enable =
        ISIF_VLDC_RESYNC_VSYNC_ENABLE;
    default_preview_sdram_op_params.sdram_hpos = sizes->h_start + 14;
    default_preview_sdram_op_params.sdram_hsize = (sizes->h_size - 14);
    default_preview_sdram_op_params.sdram_vpos0 = sizes->v_start;
    default_preview_sdram_op_params.sdram_vpos1 = sizes->v_start;
    default_preview_sdram_op_params.sdram_vsize = (sizes->v_size - 2);
    // cfg->sdram_op_params->sdram_hsize = sizes->h_size;
    // cfg->sdram_op_params->sdram_vsize = sizes->v_size;

    default_preview_sdram_op_params.dpcm_enable = ISIF_DPCM_ENCODER_ENABLE_OFF;
#ifdef SENSOR_12BIT
    default_preview_sdram_op_params.sdram_pack_fmt = ISIF_SDRAM_PACK_12BITS;
#else
    default_preview_sdram_op_params.sdram_pack_fmt = ISIF_SDRAM_PACK_16BITS;
#endif

#ifdef APPRO_SENSOR_VENDOR
#ifdef IMGS_SONY_IMX136
	default_preview_sdram_op_params.sdram_pack_fmt = ISIF_SDRAM_PACK_16BITS;	//ISIF_SDRAM_PACK_12BITS;
#endif
#endif
    default_preview_sdram_op_params.memory_addr_offset = sizes->ppln * 2;

#ifdef USE_PARALLEL_VIDEO_PORT
    default_preview_sdram_op_params.ccd_raw_shift_value = 0;

#endif

    cfg->feature_flag |= ISIF_DPCM_FLAG;

    // cfg->feature_flag |= ISIF_LINEN_FLAG;
    cfg->linearization_params = &default_preview_linearization_params;

    // cfg->feature_flag |= ISIF_VLDC_FLAG;
    cfg->src_cfg = (isif_ip_src_cfg_t *) & default_preview_isif_src_cfg;
    cfg->src_cfg->hd_width = sizes->h_size;
    cfg->src_cfg->vd_width = sizes->v_size;
    cfg->src_cfg->lpfr_vs_interval = sizes->v_size;
    cfg->src_cfg->ppln_hs_interval = sizes->h_size;

    if (IPIPE_VP_DEV_PI == iss_params->iss_config->vp_device)
    {
        cfg->src_cfg->vd_pol = ISIF_NEGATIVE_POLARITY;
        cfg->src_cfg->hd_pol = ISIF_NEGATIVE_POLARITY;
    }
    else
    {
        cfg->src_cfg->vd_pol = ISIF_POSITIVE_POLARITY;
        cfg->src_cfg->hd_pol = ISIF_POSITIVE_POLARITY;
    }

    // isif_ip_src_cfg_t* src_cfg
    isif_config_pol(cfg->src_cfg);

    cfg->src_cfg->ip_data_msb_pos =
        (ISIF_GAIN_MSB_POS) iss_params->iss_config->msb_pos;

    memcpy(cfg->src_cfg->field0_pixel_pos,
           &isif_drv_color_pattern[iss_params->iss_config->color_pattern][0],
           4);
    memcpy(cfg->src_cfg->field1_pixel_pos,
           &isif_drv_color_pattern[iss_params->iss_config->color_pattern][0],
           4);

    if (IPIPE_IN_FORMAT_YUV422 == iss_params->in_format)
    {
        ((isif_ip_src_cfg_t *) & default_preview_isif_src_cfg)->ip_pix_fmt =
            ISIF_DATA_INPUT_MODE_YUV8;
    }
    else
    {
        ((isif_ip_src_cfg_t *) & default_preview_isif_src_cfg)->ip_pix_fmt =
            ISIF_DATA_INPUT_MODE_CCD_RAW;
    }
    cfg->feature_flag |= ISIF_INPUT_FLAG;
    return ISP_SUCCESS;

}

/* = == == == == == == == == == == == == == == == == == == == == == == == ==
 * == == == == == == == = */
/* 
 *  Description:- Configures ISIF to read from ipipeif and put data in RAM at the address "img_buff"
 *  contained in the structure field "sdram_op_params", the flags in "feature_flag" enable different features in isif.
 *
 *  @param   ipipeif_path_cfg_t 
 
 *  @return         ISP_RETURN
 *= == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */

/* ===================================================================
 *  @func     IssIsifParamsInitCaptureInMemory                                               
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
ISP_RETURN IssIsifParamsInitCaptureInMemory(isif_cfg_t * cfg,
                                            iss_config_processing_t *
                                            iss_config,
                                            in_frame_sizes_t * sizes,
                                            IPIPE_PROC_COMPRESSION
                                            f_compres_out)
{

    IssIsifConfigFilters(cfg, iss_config);

    // cfg->feature_flag |= ISIF_IP_DATA_FORMATTER_FLAG;
    cfg->ip_data_format_params =
        (isif_ip_format_cfg_t *) & default_preview_ip_data_format_params;

    // cfg->feature_flag |= ISIF_HLPF_FLAG;
    // cfg->feature_flag |= ISIF_ALAW_FLAG;
    // cfg->feature_flag |= ISIF_HVCULLING_FLAG;
    memcpy(&capture_sdram_op_params, (void *) &default_preview_sdram_op_params,
           sizeof(default_preview_sdram_op_params));

    capture_sdram_op_params.sdram_hpos = sizes->h_start;
    capture_sdram_op_params.sdram_hsize = sizes->h_size;
    capture_sdram_op_params.sdram_vpos0 = sizes->v_start;
    capture_sdram_op_params.sdram_vpos1 = sizes->v_start;
    capture_sdram_op_params.sdram_vsize = sizes->v_size;
    // capture_sdram_op_params.memory_addr = address;
    capture_sdram_op_params.vsync_enable = ISIF_VLDC_RESYNC_VSYNC_DISABLE;

    capture_sdram_op_params.data_write_enable = ISIF_DATA_WRITE_DISABLE;
    if (IPIPE_PROC_COMPR_NO == f_compres_out)
    {
        capture_sdram_op_params.dpcm_enable = ISIF_DPCM_ENCODER_ENABLE_OFF;
        capture_sdram_op_params.sdram_pack_fmt = ISIF_SDRAM_PACK_16BITS;
        capture_sdram_op_params.memory_addr_offset = (sizes->ppln * 2);
    }
    else
    {
        capture_sdram_op_params.dpcm_enable = ISIF_DPCM_ENCODER_ENABLE_ON;
        capture_sdram_op_params.sdram_pack_fmt = ISIF_SDRAM_PACK_8BITS;
        capture_sdram_op_params.memory_addr_offset = sizes->ppln;
    }
    cfg->feature_flag |= ISIF_DPCM_FLAG;
    cfg->feature_flag |= ISIF_SDRAM_OP_FLAG;
    cfg->sdram_op_params = &capture_sdram_op_params;

    cfg->linearization_params = &default_preview_linearization_params;

    cfg->src_cfg = (isif_ip_src_cfg_t *) & default_preview_isif_src_cfg;

    cfg->src_cfg->hd_width = sizes->h_size;
    cfg->src_cfg->vd_width = sizes->v_size;
    cfg->src_cfg->lpfr_vs_interval = sizes->v_size;
    cfg->src_cfg->ppln_hs_interval = sizes->h_size;
    cfg->src_cfg->vd_pol = ISIF_NEGATIVE_POLARITY;
    cfg->src_cfg->hd_pol = ISIF_NEGATIVE_POLARITY;

    cfg->src_cfg->ip_data_msb_pos = (ISIF_GAIN_MSB_POS) iss_config->msb_pos;

    memcpy(cfg->src_cfg->field0_pixel_pos,
           &isif_drv_color_pattern[iss_config->color_pattern][0], 4);
    memcpy(cfg->src_cfg->field1_pixel_pos,
           &isif_drv_color_pattern[iss_config->color_pattern][0], 4);

    cfg->feature_flag |= ISIF_INPUT_FLAG;

    // iss_config->f_proc_isif_validity = PROC_ISIF_VALID_NO;

    return ISP_SUCCESS;

}
