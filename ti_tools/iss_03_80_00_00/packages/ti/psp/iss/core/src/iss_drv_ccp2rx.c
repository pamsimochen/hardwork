/** ==================================================================
 *  @file   iss_drv_ccp2rx.c                                                  
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
#include "ti/psp/iss/core/inc/iss_drv.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/*********************DEFULT CCP2 PARAMS***************************************/

const ccp2_global_ctrl ccp2_global_ctrl_default_preview_params = {
    RISING,
    1,
    NON_INV,
    0,
    CTRL_BURST_TWO_X_SIXTY_FOUR,
    NONMIPI_CCP2_MODE,
    FRAME_DIS_IMME,
    OUTPUT_SERIAL,
    DATA_CLOCK_PHY_SEL,
    INTERFACE_DISABLE_IMME,
    NON_POSTED,
    65536 / 1,
    VP_CLOCK_DISABLE,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL,
    SIXTEEN_CYCLES
};

const ccp2_lcm_ctrl ccp2_lcm_ctrl_default_preview_params = {
    DST_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    LCM_DST_COMPR_NO,
    LCM_RAW12,
    SRC_PACK_DISABLED,
    SIMPLE_PREDICTOR,
    LCM_SRC_DECOMPR_NO,
    LCM_RAW12,
    CTRL_BURST_SIXTEEN_X_SIXTY_FOUR,
    FULL_SPEED,
    SEND_TO_VIDEO_NO_COMPRESSION,
    0
};

/*********************DEFULT CCP2 PARAMS end***************************************/

/*************************CCP2_param_init**********************/

/* ===================================================================
 *  @func     issCcp2PreviewParamsInit                                               
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
ISP_RETURN issCcp2PreviewParamsInit(ccp2_path_enable_t * cfg, uint32 clock_div,
                                    CCP2_OUTPUT_SELECT out_sel)
{
    memcpy((void *) (&(cfg->ctrl_global)),
           (const void *) (&ccp2_global_ctrl_default_preview_params),
           sizeof(ccp2_global_ctrl));

    cfg->ctrl_global.FRACDIV = clock_div;
    if (CCP2_OUTPUT_OCP == out_sel)
    {
        cfg->ctrl_global.CCP2_CTRL_VP_ONLY_EN = 0;
    }
    return ISP_SUCCESS;
}

// This is to configure the memory channel of ccp2
/* ===================================================================
 *  @func     issCcp2PreviewLcmInit                                               
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
ISP_RETURN issCcp2PreviewLcmInit(ccp2_lcm_enable_t * lcm_cfg,
                                 in_frame_sizes_t * sizes, uint32 dst_bpln,
                                 IPIPE_PROC_COMPRESSION f_compres_in,
                                 CCP2_OUTPUT_SELECT out_sel)
{
    memcpy((void *) (&(lcm_cfg->mem_chan_ctrl)),
           (const void *) (&ccp2_lcm_ctrl_default_preview_params),
           sizeof(ccp2_lcm_ctrl));
    if (IPIPE_PROC_COMPR_DPCM == f_compres_in)
    {
        lcm_cfg->mem_chan_ctrl.LCM_SRC_FORMAT = LCM_RAW8;
        lcm_cfg->mem_chan_ctrl.LCM_SRC_DECOPRESS = LCM_SRC_DECOMPR_DPCM_RAW10;
        lcm_cfg->mem_chan_ctrl.LCM_SRC_PACK = SRC_PACK_ENABLED;
        lcm_cfg->mem_chan_config.LCM_SRC_OFST = (sizes->ppln) / 32;
        lcm_cfg->mem_chan_config.LCM_HSIZE_SKIP = (sizes->h_start % 32);
    }
    else if (IPIPE_PROC_COMPR_ALAW == f_compres_in)
    {
        lcm_cfg->mem_chan_ctrl.LCM_SRC_FORMAT = LCM_RAW8;
        lcm_cfg->mem_chan_ctrl.LCM_SRC_DECOPRESS = LCM_SRC_DECOMPR_ALAW;
        lcm_cfg->mem_chan_config.LCM_SRC_OFST = (sizes->ppln) / 32;
        lcm_cfg->mem_chan_config.LCM_HSIZE_SKIP = (sizes->h_start % 32);
    }
    else
    {
        lcm_cfg->mem_chan_config.LCM_SRC_OFST = (sizes->ppln * 2) / 32;
        lcm_cfg->mem_chan_config.LCM_HSIZE_SKIP = (sizes->h_start % 16);
    }
    lcm_cfg->mem_chan_config.LCM_VSIZE_COUNT = sizes->v_size_crop + 16; // +16 
                                                                        // is 
                                                                        // for 
                                                                        // ZEBU
    lcm_cfg->mem_chan_config.LCM_HSIZE_COUNT = sizes->h_size_crop;
    lcm_cfg->mem_chan_config.LCM_DST_OFST = dst_bpln;
    lcm_cfg->mem_chan_config.LCM_PREFETCH_SWORDS =
        (((sizes->h_size + sizes->h_start) * 16) / 64 + 15) & ~15;

    lcm_cfg->mem_chan_config.LCM_DST_ADDR = NULL;
    lcm_cfg->mem_chan_config.LCM_SRC_ADDR = NULL;

    if (CCP2_OUTPUT_OCP == out_sel)
    {
        lcm_cfg->mem_chan_ctrl.DST_PORT = SEND_TO_MEMORY;
        lcm_cfg->mem_chan_ctrl.LCM_DST_FORMAT =
            lcm_cfg->mem_chan_ctrl.LCM_SRC_FORMAT;
        lcm_cfg->mem_chan_config.LCM_VSIZE_COUNT = sizes->v_size_crop;
    }

    return ISP_SUCCESS;
}
