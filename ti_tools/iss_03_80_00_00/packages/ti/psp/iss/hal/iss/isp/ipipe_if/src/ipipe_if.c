/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file ipipe_if.c
 * this file implements the routines required to configure ipipe_if.c

 *
 *
 * @path   Centaurus\drivers\drv_isp\src\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *!
 *========================================================================= */

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/
#include "../inc/ipipe_if.h"
#include "../inc/ipipeif_reg.h"
#include "../../isp5_utils/isp5_sys_types.h"
#include "../../isp5_utils/isp5_csl_utils.h"
#include "../../common/inc/isp_common.h"

/* TO DO
 *
 * 1)Set clk input and div ratio based on INPSRC, 2) check if OCLIP is after
 * dfs gain or after gain in only ipipe input stage
 *
 *
 *
 *
 */

ipipeif_regs_ovly ipipeif_reg;

ipipeif_dev_data_t *ipipeif_devp;

ipipeif_dev_data_t ipipeif_dev;

/* ================================================================ */
/**
 * ipipeif_init()
 *  Description:- Init should be called only once before calling any other function
 *
 *
 *  @param   none

 *  @return    ISP_RETURN
 */
/*================================================================== */

/* ===================================================================
 *  @func     ipipeif_init
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
ISP_RETURN ipipeif_init()
{
    /* This memory can be malloc'ed */
    ipipeif_devp = &ipipeif_dev;

    ipipeif_devp->opened = FALSE;
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipeif_open()
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable ipipe clk
 *
 *
 *  @param   :- none

 *  @return  :- ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_open
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
ISP_RETURN ipipeif_open()
{

    ISP_RETURN retval = ISP_SUCCESS;

    if (ipipeif_devp->opened == TRUE)
    {

        return ISP_FAILURE;

    }
    ipipeif_devp->opened = TRUE;

    ipipeif_reg = (ipipeif_regs_ovly) IPIPEIF_BASE_ADDRESS;

    retval = isp_common_enable_clk(ISP_IPIPEIF_CLK);

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
 *  ipipeif_close()
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler
 *
 *
 *  @param   none

 *  @return   ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_close
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
ISP_RETURN ipipeif_close()
{
    ISP_RETURN retval = ISP_SUCCESS;

    if (ipipeif_devp->opened != TRUE)
    {

        return ISP_FAILURE;

    }

    ipipeif_reg = NULL;

    retval = isp_common_disable_clk(ISP_IPIPEIF_CLK);

    if (retval == ISP_SUCCESS)
    {
        // retval=isp_common_unset_interrupt_handler(ISP_IPIPE_INT_REG );
    }

    ipipeif_devp->opened = FALSE;
    return retval;

}

/* ================================================================ */
/**
 *  ipipeif_validate_params()
 *  Description:-
 *
 *
 *  @param  cfg     ipipeif_path_cfg_t * cfg pointer to the configuration structure
 *
 *  @return
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_validate_params
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
ISP_RETURN ipipeif_validate_params(ipipeif_path_cfg_t * cfg)
{

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  ipipeif_start()
 *  Description:- This routine will start /stop the ipipe module,
 *                 start is of one shot type or continuous typt
 *
 *
 *  @param enable IPIPE_START_T enbale
 *
 *  @return         ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_start
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
ISP_RETURN ipipeif_start(IPIPEIF_START_T enable)
{

    ISP_RETURN status = ISP_SUCCESS;

    if (enable > IPIPEIF_SINGLE)
        return ISP_FAILURE;

    switch (enable)
    {
        case IPIPEIF_STOP:
            ISP_SET32(ipipeif_reg->CFG1, CSL_IPIPEIF_CFG1_ONESHOT_CONTINUOUS,
                      CSL_IPIPEIF_CFG1_ONESHOT_SHIFT,
                      CSL_IPIPEIF_CFG1_ONESHOT_MASK);
            ISP_SET32(ipipeif_reg->IPIPEIF_ENABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_DISABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_SHIFT,
                      CSL_IPIPEIF_ENABLE_ENABLE_MASK);
            break;
        case IPIPEIF_RUN:
            ISP_SET32(ipipeif_reg->CFG1, CSL_IPIPEIF_CFG1_ONESHOT_CONTINUOUS,
                      CSL_IPIPEIF_CFG1_ONESHOT_SHIFT,
                      CSL_IPIPEIF_CFG1_ONESHOT_MASK);
            ISP_SET32(ipipeif_reg->IPIPEIF_ENABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_ENABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_SHIFT,
                      CSL_IPIPEIF_ENABLE_ENABLE_MASK);
            break;
        case IPIPEIF_SINGLE:
            ISP_SET32(ipipeif_reg->CFG1, CSL_IPIPEIF_CFG1_ONESHOT_ONESHOT,
                      CSL_IPIPEIF_ENABLE_ENABLE_SHIFT,
                      CSL_IPIPEIF_ENABLE_ENABLE_MASK);
            ISP_SET32(ipipeif_reg->IPIPEIF_ENABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_ENABLE,
                      CSL_IPIPEIF_ENABLE_ENABLE_SHIFT,
                      CSL_IPIPEIF_ENABLE_ENABLE_MASK);
            break;

    }

    return (status);

}

/* ================================================================ */
/**
 *  ipipeif_clk_config()
 *  Description:-this routine configures the m and n values for the ipipe-if module when pp is not driving the input
 *  NOTE!!!!!!!!!this routine is incomplete as TRM doesnt give enough info about the same.
 *
 *  @param  ipipeif_cfg ipipeif_path_cfg_t * ipipeif_cfg

 *  @return
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ipipeif_clk_config
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
ISP_RETURN ipipeif_clk_config(ipipeif_path_cfg_t * ipipeif_cfg)
{
    ipipeif_path_cfg_t *cfg = ipipeif_cfg;

    uint8 clk_sel = 0;

    uint8 m = 0;

    uint8 n = 0;

    if ((cfg->isif_inpsrc == IPIPEIF_ISIF_INPSRC_SDRAM_RAW) ||
        (cfg->isif_inpsrc == IPIPEIF_ISIF_INPSRC_SDRAM_YUV) ||
        (cfg->ipipe_inpsrc == IPIPEIF_IPIPE_INPSRC_SDRAM_RAW) ||
        (cfg->ipipe_inpsrc == IPIPEIF_IPIPE_INPSRC_SDRAM_YUV))
    {
        clk_sel = 1;

        uint8 m_ipipe = 0;

        uint8 m_isif = 0;

        uint8 n_ipipe = 0;

        uint8 n_isif = 0;

        uint16 clk_div = 0;

        /* select mffrom ipipe conf */
        if (cfg->ipipe_inpsrc == IPIPEIF_IPIPE_INPSRC_SDRAM_RAW)
        {
            m_ipipe = (cfg->ipipe_ip_cfg.sdram_ip.clkdiv_m);
            n_ipipe = (cfg->ipipe_ip_cfg.sdram_ip.clkdiv_n);
        }
        else if (cfg->ipipe_inpsrc == IPIPEIF_IPIPE_INPSRC_SDRAM_YUV)
        {
            m_ipipe = cfg->ipipe_ip_cfg.yuv_ip.clkdiv_m;
            n_ipipe = cfg->ipipe_ip_cfg.yuv_ip.clkdiv_n;
        }
        else
        {                                                  /* Should never
                                                            * happen:-) */
            return ISP_FAILURE;
        }
        /* select m from isif conf */
        if ((cfg->isif_inpsrc == IPIPEIF_ISIF_INPSRC_SDRAM_RAW))
        {
            m_isif = cfg->isif_ip_cfg.sdram_ip.clkdiv_m;
            n_isif = cfg->isif_ip_cfg.sdram_ip.clkdiv_n;
        }
        else if ((cfg->isif_inpsrc == IPIPEIF_ISIF_INPSRC_SDRAM_YUV))
        {
            m_isif = cfg->isif_ip_cfg.yuv_ip.clkdiv_m;
            n_isif = cfg->isif_ip_cfg.yuv_ip.clkdiv_n;
        }
        else
            return ISP_FAILURE;                            /* Should never
                                                            * happen:-) */

        /* Maximum clk speed requested max of m , least of n */

        m = (m_ipipe > m_isif) ? (m_ipipe) : (m_isif);

        n = (n_ipipe < n_isif) ? (n_ipipe) : (n_isif);

        /* m/n should not be greater than 1/2 when VPSS_clk is greter than
         * 121MHZ in dm360 but ISP-c1.doc doesnt talk about tghuis, so
         * ignoring it for the momment */
        /* if(m>(2*n)) { m=2*n; } */

        clk_div = (uint16) (((uint16) (m - 1) << 8) & (0xFF00));
        clk_div = ((clk_div) | (uint16) (n - 1));

        ISP_SET32(ipipeif_reg->CLKDIV, clk_div, CSL_IPIPEIF_CLKDIV_CLKDIV_SHIFT,
                  CSL_IPIPEIF_CLKDIV_CLKDIV_MASK);

    }

    ISP_SET32(ipipeif_reg->CFG1, clk_sel, CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
              CSL_IPIPEIF_CFG1_CLKSEL_SHIFT);

    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     ipipeif_set_sdram_in_addr
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
ISP_RETURN ipipeif_set_sdram_in_addr(uint32 address)
{
    ISP_SET32(ipipeif_reg->ADDRU, (((address) >> 21) & (0x07FF)),
              CSL_IPIPEIF_ADDRU_ADDRU_SHIFT, CSL_IPIPEIF_ADDRU_ADDRU_MASK);
    ISP_SET32(ipipeif_reg->ADDRL, ((address) >> 5 & (0xFFFF)),
              CSL_IPIPEIF_ADDRL_ADDRL_SHIFT, CSL_IPIPEIF_ADDRL_ADDRL_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
 *  ipipeif_config()
 *  Description:- This is the main cfg structure of ipipe-if. It configures both INPSRC1 and INPSRC2
 *
 *
 *  @param ipipeif_cfg   ipipeif_path_cfg_t * ipipeif_cfg
 *
 *  @return    :- ISP_RETURN
 */
/*================================================================== */

/* ===================================================================
 *  @func     ipipeif_config
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
ISP_RETURN ipipeif_config(ipipeif_path_cfg_t * ipipeif_cfg)
{

    uint16 clk_div;

    IPIPEIF_IPIPE_INPUT_SRC_SEL_T ipipe_ip_src = IPIPEIF_IPIPE_INPSRC_SDRAM_YUV;

    IPIPEIF_ISIF_INPUT_SRC_SEL_T isif_ip_src = IPIPEIF_ISIF_INPSRC_SDRAM_YUV;

    ipipeif_isif_input_cfg_t *isif_cfg = NULL;

    ipipeif_ipipe_input_cfg_t *ipipe_cfg = NULL;

    ISP_ASSERT(ipipeif_cfg, NULL);

    ipipe_ip_src = ipipeif_cfg->ipipe_inpsrc;
    isif_ip_src = ipipeif_cfg->isif_inpsrc;

    isif_cfg = &(ipipeif_cfg->isif_ip_cfg);

    ipipe_cfg = &(ipipeif_cfg->ipipe_ip_cfg);

    if (ISP_FAILURE == ipipeif_validate_params(ipipeif_cfg))
    {
        return ISP_FAILURE;
    }

    // if(ipipe_ip_src>IPIPEIF_IPIPE_INPSRC_SDRAM_YUV)
    // return ISP_FAILURE;
    switch (ipipe_ip_src)
    {

        case IPIPEIF_IPIPE_INPSRC_ISIF:
        {
            uint32 regval = 0;

            ipipeif_ipipe_ip_isif_t *cfg = &(ipipe_cfg->isif_ip);

            ISP_ASSERT(cfg, NULL);
            /* Main configuration CFG1 */

            ISP_SET32(ipipeif_reg->CFG1, ipipe_ip_src,
                      CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);
            ISP_SET32(ipipeif_reg->CFG1, cfg->avg_filter_enable,
                      CSL_IPIPEIF_CFG1_AVGFILT_SHIFT,
                      CSL_IPIPEIF_CFG1_AVGFILT_MASK);
            ISP_WRITE32(ipipeif_reg->GAIN, cfg->gain);

            ISP_SET32(ipipeif_reg->CFG2, cfg->YUV16,
                      CSL_IPIPEIF_CFG2_INPTYP_SHIFT,
                      CSL_IPIPEIF_CFG2_INPTYP_MASK);

            ISP_SET32(ipipeif_reg->CFG2, cfg->YUV8, CSL_IPIPEIF_CFG2_YUV8_SHIFT,
                      CSL_IPIPEIF_CFG2_YUV8_MASK);

            ISP_SET32(ipipeif_reg->CFG2, cfg->YUV8P,
                      CSL_IPIPEIF_CFG2_YUV8P_SHIFT,
                      CSL_IPIPEIF_CFG2_YUV8P_MASK);

            /* DPC configuration */
            if (NULL != cfg->dpc1_cfg)
            {
                regval = ipipeif_reg->DPC1;
                ISP_SET32(regval, cfg->dpc1_cfg->dpc_en,
                          CSL_IPIPEIF_DPC1_DPC1ENA_SHIFT,
                          CSL_IPIPEIF_DPC1_DPC1ENA_MASK);
                ISP_SET32(regval, cfg->dpc1_cfg->dpc_thr,
                          CSL_IPIPEIF_DPC1_DPC1TH_SHIFT,
                          CSL_IPIPEIF_DPC1_DPC1TH_MASK);
                ipipeif_reg->DPC2 = regval;
            }

            /* Decimation and Resizer */
            ISP_SET32(ipipeif_reg->CFG1, cfg->decimation_enable,
                      CSL_IPIPEIF_CFG1_DECM_SHIFT, CSL_IPIPEIF_CFG1_DECM_MASK);

            ISP_WRITE32(ipipeif_reg->RSZ, cfg->resize);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

        }
            break;

        case IPIPEIF_IPIPE_INPSRC_SDRAM_RAW:
        {

            uint32 regval = 0;

            ipipeif_ipipe_ip_sdram_t *cfg = &(ipipe_cfg->sdram_ip);

            ISP_ASSERT(cfg, NULL);

            /* Main configuration */
            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, ipipe_ip_src, CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);
            ISP_SET32(regval, cfg->data_shift, CSL_IPIPEIF_CFG1_DATASFT_SHIFT,
                      CSL_IPIPEIF_CFG1_DATASFT_MASK);
            ISP_SET32(regval, cfg->avg_filter_enable,
                      CSL_IPIPEIF_CFG1_AVGFILT_SHIFT,
                      CSL_IPIPEIF_CFG1_AVGFILT_MASK);
            ISP_SET32(regval, cfg->decimation_enable,
                      CSL_IPIPEIF_CFG1_DECM_SHIFT, CSL_IPIPEIF_CFG1_DECM_MASK);
            ISP_SET32(regval, cfg->unpack, CSL_IPIPEIF_CFG1_UNPACK_SHIFT,
                      CSL_IPIPEIF_CFG1_UNPACK_MASK);
            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs);
            ISP_WRITE32(ipipeif_reg->RSZ, cfg->resize);

            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);

            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            ISP_SET32(ipipeif_reg->CFG2, cfg->YUV16,
                      CSL_IPIPEIF_CFG2_INPTYP_SHIFT,
                      CSL_IPIPEIF_CFG2_INPTYP_MASK);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;

            /* Gain on ipipe ip path */
            ISP_WRITE32(ipipeif_reg->GAIN, cfg->gain);

            /* DPCM configuration */
            if (NULL != cfg->dpcm_cfg)
            {
                regval = ipipeif_reg->DPCM;

                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_en,
                          CSL_IPIPEIF_DPCM_ENA_SHIFT,
                          CSL_IPIPEIF_DPCM_ENA_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_predictor,
                          CSL_IPIPEIF_DPCM_PRED_SHIFT,
                          CSL_IPIPEIF_DPCM_PRED_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_bit_size,
                          CSL_IPIPEIF_DPCM_BITS_SHIFT,
                          CSL_IPIPEIF_DPCM_BITS_MASK);

                ipipeif_reg->DPCM = regval;
            }

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

            /* DPC cfg */
            if (NULL != cfg->dpc2_cfg)
            {

                regval = ipipeif_reg->DPC2;
                ISP_SET32(regval, cfg->dpc2_cfg->dpc_en,
                          CSL_IPIPEIF_DPC2_DPC2ENA_SHIFT,
                          CSL_IPIPEIF_DPC2_DPC2ENA_MASK);
                if (cfg->dpc2_cfg->dpc_en == IPIPEIF_FEATURE_ENABLE)
                {
                    ISP_SET32(regval, cfg->dpc2_cfg->dpc_thr,
                              CSL_IPIPEIF_DPC2_DPC2TH_SHIFT,
                              CSL_IPIPEIF_DPC2_DPC2TH_MASK);

                }
                ipipeif_reg->DPC2 = regval;
            }

            /* clk related settings */
            ISP_SET32(ipipeif_reg->CFG1, cfg->ip_clk_sel,
                      CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
                      CSL_IPIPEIF_CFG1_CLKSEL_MASK);

            clk_div = ((uint16) (cfg->clkdiv_m - 1) << 8) & (uint16) (0xFF00);
            clk_div = (clk_div) | (uint16) (cfg->clkdiv_n - 1);

            ISP_SET32(ipipeif_reg->CLKDIV, clk_div,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_SHIFT,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_MASK);

            // ISP_SET32(ipipeif_reg->CLKDIV,cfg->clkdiv_m,8,0x0000FF00);
            // ISP_SET32(ipipeif_reg->CLKDIV,cfg->clkdiv_n,8,0x000000FF);

        }
            return ISP_SUCCESS;

        case IPIPEIF_IPIPE_INPSRC_ISIF_DARKFM:
        {
            uint32 regval = 0;

            ipipeif_ipipe_ip_sdram_dfs_t *cfg = &(ipipe_cfg->sdram_dfs_ip);

            ISP_ASSERT(cfg, NULL);
            /* Main configuration */
            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, ipipe_ip_src, CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);
            ISP_SET32(regval, cfg->data_shift, CSL_IPIPEIF_CFG1_DATASFT_SHIFT,
                      CSL_IPIPEIF_CFG1_DATASFT_MASK);
            ISP_SET32(regval, cfg->avg_filter_enable,
                      CSL_IPIPEIF_CFG1_AVGFILT_SHIFT,
                      CSL_IPIPEIF_CFG1_AVGFILT_MASK);
            ISP_SET32(regval, cfg->decimation_enable,
                      CSL_IPIPEIF_CFG1_DECM_SHIFT, CSL_IPIPEIF_CFG1_DECM_MASK);
            ISP_SET32(regval, cfg->unpack, CSL_IPIPEIF_CFG1_UNPACK_SHIFT,
                      CSL_IPIPEIF_CFG1_UNPACK_MASK);

            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs);
            ISP_WRITE32(ipipeif_reg->RSZ, cfg->resize);

            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);

            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;

            /* Gain on ipipe ip path */
            ISP_WRITE32(ipipeif_reg->GAIN, cfg->gain);

            /* DPCM configuration */
            if (NULL != cfg->dpcm_cfg)
            {

                regval = ipipeif_reg->DPCM;

                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_en,
                          CSL_IPIPEIF_DPCM_ENA_SHIFT,
                          CSL_IPIPEIF_DPCM_ENA_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_predictor,
                          CSL_IPIPEIF_DPCM_PRED_SHIFT,
                          CSL_IPIPEIF_DPCM_PRED_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_bit_size,
                          CSL_IPIPEIF_DPCM_BITS_SHIFT,
                          CSL_IPIPEIF_DPCM_BITS_MASK);

                ipipeif_reg->DPCM = regval;
            }

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

            /* DPC cfg */
            if (NULL != cfg->dpc2_cfg)
            {

                regval = ipipeif_reg->DPC2;
                ISP_SET32(regval, cfg->dpc2_cfg->dpc_en,
                          CSL_IPIPEIF_DPC2_DPC2ENA_SHIFT,
                          CSL_IPIPEIF_DPC2_DPC2ENA_MASK);
                if (cfg->dpc2_cfg->dpc_en == IPIPEIF_FEATURE_ENABLE)
                {
                    ISP_SET32(regval, cfg->dpc2_cfg->dpc_thr,
                              CSL_IPIPEIF_DPC2_DPC2TH_SHIFT,
                              CSL_IPIPEIF_DPC2_DPC2TH_MASK);

                }
                ipipeif_reg->DPC2 = regval;
            }

        }
            break;
        case IPIPEIF_IPIPE_INPSRC_SDRAM_YUV:
        {
            uint32 regval = 0;

            ipipeif_ipipe_ip_yuv_t *cfg = &(ipipe_cfg->yuv_ip);

            ISP_ASSERT(cfg, NULL);

            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, ipipe_ip_src, CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);

            ISP_SET32(regval, cfg->avg_filter_enable,
                      CSL_IPIPEIF_CFG1_AVGFILT_SHIFT,
                      CSL_IPIPEIF_CFG1_AVGFILT_MASK);
            ISP_SET32(regval, cfg->decimation_enable,
                      CSL_IPIPEIF_CFG1_DECM_SHIFT, CSL_IPIPEIF_CFG1_DECM_MASK);

            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs);
            ISP_WRITE32(ipipeif_reg->RSZ, cfg->resize);
            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);

            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;

            /* clk related settings */
            ISP_SET32(ipipeif_reg->CFG1, cfg->ip_clk_sel,
                      CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
                      CSL_IPIPEIF_CFG1_CLKSEL_MASK);

            clk_div = ((uint16) (cfg->clkdiv_m - 1) << 8) & (uint16) (0xFF00);
            clk_div = (clk_div) | (uint16) (cfg->clkdiv_n - 1);
            ISP_SET32(ipipeif_reg->CLKDIV, clk_div,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_SHIFT,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_MASK);

            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_m-1),8,0x0000FF00);
            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_n-1),0,0x000000FF);
            //

        }
            return ISP_SUCCESS;

    }

    // if(isif_ip_src>IPIPEIF_IPIPE_INPSRC_SDRAM_YUV)
    // return ISP_FAILURE;
    switch (isif_ip_src)
    {

        case IPIPEIF_ISIF_INPSRC_PP_RAW:
        {

            uint32 regval = 0;

            ipipeif_isif_ip_pp_t *cfg = &(isif_cfg->pp_ip);

            ISP_ASSERT(cfg, NULL);

            /* Main configuration CFG1 */

            ISP_SET32(ipipeif_reg->CFG1, isif_ip_src,
                      CSL_IPIPEIF_CFG1_INPSRC1_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC1_MASK);

            /* DPC configuration */
            if (NULL != cfg->dpc1_cfg)
            {
                regval = ipipeif_reg->DPC1;
                ISP_SET32(regval, cfg->dpc1_cfg->dpc_en,
                          CSL_IPIPEIF_DPC1_DPC1ENA_SHIFT,
                          CSL_IPIPEIF_DPC1_DPC1ENA_MASK);
                ISP_SET32(regval, cfg->dpc1_cfg->dpc_thr,
                          CSL_IPIPEIF_DPC1_DPC1TH_SHIFT,
                          CSL_IPIPEIF_DPC1_DPC1TH_MASK);
                ipipeif_reg->DPC2 = regval;
            }

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            // ~~ ISP_SET32(
            // ipipeif_reg->CFG2,cfg->wen,CSL_IPIPEIF_CFG2_EXWEN_SHIFT,CSL_IPIPEIF_CFG2_EXWEN_MASK);
            ipipeif_reg->CFG2 = regval;

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

            /* clk related settings */
            ISP_SET32(ipipeif_reg->CFG1, cfg->ip_clk_sel,
                      CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
                      CSL_IPIPEIF_CFG1_CLKSEL_MASK);

        }
            break;

        case IPIPEIF_ISIF_INPSRC_SDRAM_RAW:
        {
            uint32 regval = 0;

            ipipeif_isif_ip_sdram_t *cfg = &(isif_cfg->sdram_ip);   /* made
                                                                     * _isif_
                                                                     */
            ISP_ASSERT(cfg, NULL);

            /* Main configuration */
            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, isif_ip_src, CSL_IPIPEIF_CFG1_INPSRC1_SHIFT, CSL_IPIPEIF_CFG1_INPSRC1_MASK);  /* made
                                                                                                             * _inpsrc2_
                                                                                                             */
            ISP_SET32(regval, cfg->data_shift, CSL_IPIPEIF_CFG1_DATASFT_SHIFT,
                      CSL_IPIPEIF_CFG1_DATASFT_MASK);
            ISP_SET32(regval, cfg->unpack, CSL_IPIPEIF_CFG1_UNPACK_SHIFT,
                      CSL_IPIPEIF_CFG1_UNPACK_MASK);

            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs);

            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);
            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;

            /* DPCM configuration */
            if (NULL != cfg->dpcm_cfg)
            {

                regval = ipipeif_reg->DPCM;

                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_en,
                          CSL_IPIPEIF_DPCM_ENA_SHIFT,
                          CSL_IPIPEIF_DPCM_ENA_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_predictor,
                          CSL_IPIPEIF_DPCM_PRED_SHIFT,
                          CSL_IPIPEIF_DPCM_PRED_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_bit_size,
                          CSL_IPIPEIF_DPCM_BITS_SHIFT,
                          CSL_IPIPEIF_DPCM_BITS_MASK);

                ipipeif_reg->DPCM = regval;
            }

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

            /* DPC cfg */
            if (NULL != cfg->dpc2_cfg)
            {

                regval = ipipeif_reg->DPC2;
                ISP_SET32(regval, cfg->dpc2_cfg->dpc_en,
                          CSL_IPIPEIF_DPC2_DPC2ENA_SHIFT,
                          CSL_IPIPEIF_DPC2_DPC2ENA_MASK);
                if (cfg->dpc2_cfg->dpc_en == IPIPEIF_FEATURE_ENABLE)
                {
                    ISP_SET32(regval, cfg->dpc2_cfg->dpc_thr,
                              CSL_IPIPEIF_DPC2_DPC2TH_SHIFT,
                              CSL_IPIPEIF_DPC2_DPC2TH_MASK);

                }
                ipipeif_reg->DPC2 = regval;
            }

            /* clk related settings */
            ISP_SET32(ipipeif_reg->CFG1, cfg->ip_clk_sel,
                      CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
                      CSL_IPIPEIF_CFG1_CLKSEL_MASK);

            clk_div = ((uint16) (cfg->clkdiv_m - 1) << 8) & (uint16) (0xFF00);
            clk_div = (clk_div) | (uint16) (cfg->clkdiv_n - 1);
            ISP_SET32(ipipeif_reg->CLKDIV, clk_div,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_SHIFT,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_MASK);
            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_m-1),8,0x0000FF00);
            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_n-1),0,0x000000FF);
            //

        }
            break;

        case IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM:
        {
            uint32 regval = 0;

            ipipeif_isif_ip_sdram_dfs_t *cfg = &(isif_cfg->sdram_dfs_ip);

            ISP_ASSERT(cfg, NULL);

            /* Main configuration */
            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, isif_ip_src, CSL_IPIPEIF_CFG1_INPSRC1_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC1_MASK);
            ISP_SET32(regval, cfg->data_shift, CSL_IPIPEIF_CFG1_DATASFT_SHIFT,
                      CSL_IPIPEIF_CFG1_DATASFT_MASK);
            ISP_SET32(regval, cfg->unpack, CSL_IPIPEIF_CFG1_UNPACK_SHIFT,
                      CSL_IPIPEIF_CFG1_UNPACK_MASK);
            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs >> 5);

            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);
            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            /* CFG2 */
            ISP_SET32(ipipeif_reg->CFG2, cfg->vd_polarity,
                      CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(ipipeif_reg->CFG2, cfg->hd_polarity,
                      CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(ipipeif_reg->CFG2, cfg->int_sw,
                      CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ISP_SET32(ipipeif_reg->CFG2, cfg->dfs_dir,
                      CSL_IPIPEIF_CFG2_DFSDIR_SHIFT,
                      CSL_IPIPEIF_CFG2_DFSDIR_MASK);

            /* DPCM configuration */
            if (NULL != cfg->dpcm_cfg)
            {

                regval = ipipeif_reg->DPCM;

                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_en,
                          CSL_IPIPEIF_DPCM_ENA_SHIFT,
                          CSL_IPIPEIF_DPCM_ENA_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_predictor,
                          CSL_IPIPEIF_DPCM_PRED_SHIFT,
                          CSL_IPIPEIF_DPCM_PRED_MASK);
                ISP_SET32(regval, cfg->dpcm_cfg->dpcm_bit_size,
                          CSL_IPIPEIF_DPCM_BITS_SHIFT,
                          CSL_IPIPEIF_DPCM_BITS_MASK);

                ipipeif_reg->DPCM = regval;
            }

            /* Resizer params */
            regval = ipipeif_reg->INIRSZ;

            ISP_SET32(regval, cfg->align_sync, CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
                      CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
            ISP_SET32(regval, cfg->ini_rsz, CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT,
                      CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);
            ipipeif_reg->INIRSZ = regval;

            /* DPC cfg */
            if (NULL != cfg->dpc2_cfg)
            {

                regval = ipipeif_reg->DPC2;
                ISP_SET32(regval, cfg->dpc2_cfg->dpc_en,
                          CSL_IPIPEIF_DPC2_DPC2ENA_SHIFT,
                          CSL_IPIPEIF_DPC2_DPC2ENA_MASK);
                if (cfg->dpc2_cfg->dpc_en == IPIPEIF_FEATURE_ENABLE)
                {
                    ISP_SET32(regval, cfg->dpc2_cfg->dpc_thr,
                              CSL_IPIPEIF_DPC2_DPC2TH_SHIFT,
                              CSL_IPIPEIF_DPC2_DPC2TH_MASK);

                }
                ipipeif_reg->DPC2 = regval;
            }

        }
            break;
        case IPIPEIF_ISIF_INPSRC_SDRAM_YUV:
        {
            uint32 regval = 0;

            ipipeif_isif_ip_yuv_t *cfg = &(isif_cfg->yuv_ip);

            ISP_ASSERT(cfg, NULL);

            regval = ipipeif_reg->CFG1;
            ISP_SET32(regval, isif_ip_src, CSL_IPIPEIF_CFG1_INPSRC1_SHIFT, CSL_IPIPEIF_CFG1_INPSRC1_MASK);  /* made
                                                                                                             * _inpsrc2_
                                                                                                             */

            ipipeif_reg->CFG1 = regval;

            /* Address and size configurations */
            ISP_WRITE32(ipipeif_reg->PPLN, cfg->crop_dims.ppln_hs_interval);
            ISP_WRITE32(ipipeif_reg->LPFR, cfg->crop_dims.lpfr_vs_interval);
            ISP_WRITE32(ipipeif_reg->HNUM, cfg->crop_dims.hnum);
            ISP_WRITE32(ipipeif_reg->VNUM, cfg->crop_dims.vnum);
            ISP_WRITE32(ipipeif_reg->ADOFS, cfg->adofs);

            ISP_SET32(ipipeif_reg->ADDRU, (((cfg->address) >> 21) & (0x07FF)),
                      CSL_IPIPEIF_ADDRU_ADDRU_SHIFT,
                      CSL_IPIPEIF_ADDRU_ADDRU_MASK);
            ISP_SET32(ipipeif_reg->ADDRL, ((cfg->address) >> 5 & (0xFFFF)),
                      CSL_IPIPEIF_ADDRL_ADDRL_SHIFT,
                      CSL_IPIPEIF_ADDRL_ADDRL_MASK);

            /* CFG2 */
            regval = ipipeif_reg->CFG2;
            ISP_SET32(regval, cfg->vd_polarity, CSL_IPIPEIF_CFG2_VDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_VDPOL_MASK);
            ISP_SET32(regval, cfg->hd_polarity, CSL_IPIPEIF_CFG2_HDPOL_SHIFT,
                      CSL_IPIPEIF_CFG2_HDPOL_MASK);
            ISP_SET32(regval, cfg->int_sw, CSL_IPIPEIF_CFG2_INTSRC_SHIFT,
                      CSL_IPIPEIF_CFG2_INTSRC_MASK);
            ipipeif_reg->CFG2 = regval;
            /* clk related settings */
            ISP_SET32(ipipeif_reg->CFG1, cfg->ip_clk_sel,
                      CSL_IPIPEIF_CFG1_CLKSEL_SHIFT,
                      CSL_IPIPEIF_CFG1_CLKSEL_MASK);

            clk_div = ((uint16) (cfg->clkdiv_m - 1) << 8) & (uint16) (0xFF00);
            clk_div = (clk_div) | (uint16) (cfg->clkdiv_n - 1);
            ISP_SET32(ipipeif_reg->CLKDIV, clk_div,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_SHIFT,
                      CSL_IPIPEIF_CLKDIV_CLKDIV_MASK);
            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_m-1),8,0x0000FF00);
            // ISP_SET32(ipipeif_reg->CLKDIV,(cfg->clkdiv_n-1),0,0x000000FF);
            //

        }
            break;

    }
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     ipipeif_h3a_input_config
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
ISP_RETURN ipipeif_h3a_input_config(ipipeif_h3a_input_cfg_t * h3a_cfg)
{

    // configure H3A input
    ISP_SET32(ipipeif_reg->RSZ2, h3a_cfg->resize, CSL_IPIPEIF_RSZ2_RSZ2_SHIFT,
              CSL_IPIPEIF_RSZ2_RSZ2_MASK);
    ISP_SET32(ipipeif_reg->RSZ2, h3a_cfg->decimation_enable,
              CSL_IPIPEIF_RSZ2_DECM2_SHIFT, CSL_IPIPEIF_RSZ2_DECM2_MASK);
    ISP_SET32(ipipeif_reg->RSZ2, h3a_cfg->avg_filter_enable,
              CSL_IPIPEIF_RSZ2_AVGFILT2_SHIFT, CSL_IPIPEIF_RSZ2_AVGFILT2_MASK);
    ISP_SET32(ipipeif_reg->INIRSZ2, h3a_cfg->align_sync,
              CSL_IPIPEIF_INIRSZ_ALNSYNC_SHIFT,
              CSL_IPIPEIF_INIRSZ_ALNSYNC_MASK);
    ISP_SET32(ipipeif_reg->INIRSZ2, h3a_cfg->ini_rsz,
              CSL_IPIPEIF_INIRSZ_INIRSZ_SHIFT, CSL_IPIPEIF_INIRSZ_INIRSZ_MASK);

    return ISP_SUCCESS;
}


ISP_RETURN ipipeif_set_dfs_config(ipipeif_ip_dfs_t *dfsCfg)
{
    UInt32 regval;
    Int32 status = ISP_SUCCESS;

    regval = ipipeif_reg->CFG1;
    if (dfsCfg->enable)
    {
        /* Check of errors */
        if (dfsCfg->address == NULL ||
            dfsCfg->address & 0x1F ||
            dfsCfg->adofs == 0 ||
            dfsCfg->adofs & 0x1F ||
            dfsCfg->crop_dims.lpfr_vs_interval == 0)
        {
            return (ISP_INVALID_INPUT);
        }

        /* Set the DFS Address */
        ipipeif_set_sdram_in_addr(dfsCfg->address);

        /* Set the Offset */
        ISP_WRITE32(ipipeif_reg->ADOFS, dfsCfg->adofs >> 5);

        ISP_SET32(ipipeif_reg->CFG2, dfsCfg->dfs_dir,
                  CSL_IPIPEIF_CFG2_DFSDIR_SHIFT,
                  CSL_IPIPEIF_CFG2_DFSDIR_MASK);

        ISP_SET32(regval, dfsCfg->unpack, CSL_IPIPEIF_CFG1_UNPACK_SHIFT,
            CSL_IPIPEIF_CFG1_UNPACK_MASK);

        ISP_SET32(regval, dfsCfg->data_shift, CSL_IPIPEIF_CFG1_DATASFT_SHIFT,
            CSL_IPIPEIF_CFG1_DATASFT_MASK);

        ISP_WRITE32(ipipeif_reg->PPLN, dfsCfg->crop_dims.ppln_hs_interval);
        ISP_WRITE32(ipipeif_reg->LPFR, dfsCfg->crop_dims.lpfr_vs_interval);
        ISP_WRITE32(ipipeif_reg->HNUM, dfsCfg->crop_dims.hnum);
        ISP_WRITE32(ipipeif_reg->VNUM, dfsCfg->crop_dims.vnum);
        ISP_WRITE32(ipipeif_reg->ADOFS, dfsCfg->adofs);

        if (dfsCfg->muxSel == IPIPEIF_ISIF_MUX)
        {
            ISP_SET32(regval, IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM, CSL_IPIPEIF_CFG1_INPSRC1_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC1_MASK);
        }
        else
        {
            ISP_SET32(regval, IPIPEIF_ISIF_INPSRC_PP_RAW_DARKFM, CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);
        }
    }
    else
    {
        if (dfsCfg->muxSel == IPIPEIF_ISIF_MUX)
        {
            ISP_SET32(regval, IPIPEIF_ISIF_INPSRC_PP_RAW, CSL_IPIPEIF_CFG1_INPSRC1_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC1_MASK);
        }
        else
        {
            ISP_SET32(regval, IPIPEIF_IPIPE_INPSRC_ISIF, CSL_IPIPEIF_CFG1_INPSRC2_SHIFT,
                      CSL_IPIPEIF_CFG1_INPSRC2_MASK);
        }
    }
    ipipeif_reg->CFG1 = regval;

    return (status);
}
