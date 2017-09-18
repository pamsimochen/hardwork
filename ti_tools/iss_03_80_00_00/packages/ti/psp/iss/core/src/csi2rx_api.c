/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csi2rx_api.c
*
* This file contains routines for CCP transmitter driver
*
* @path iss/drivers/drv_csi2rx/src
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *!
 * 13-Feb-2009 Petar Sivenov - Created *! *! 27-Mar-2009 Petar Sivenov - MSP
 * interface added *! *
 * =========================================================================== */

/* User code goes here */
/* ------compilation control switches --------------------------------------- 
 */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ---------------------------- */
#include <xdc/runtime/System.h>
#include <stdlib.h>
#include <ti/sysbios/hal/Hwi.h>
#include "ti/psp/iss/hal/cred_cram/cram/cram.h"
#include <ti/psp/iss/hal/cred_cram/cred/inc/cortexm3_sys_ape/baseaddress.h>

/*-------program files ----------------------------------------*/
// #include "../../../iss/framework/msp/msp_utils.h"
#include "ti/psp/iss/core/msp_types.h"
#include "ti/psp/iss/hal/iss/csi2/csi2.h"
#include "ti/psp/iss/core/inc/csi2rx_api.h"
#include "ti/psp/iss/hal/iss/common/csl_utils/csl_types.h"
// #include <drv_csi2rx/inc/csi2rx_private.h"
// #include "../../../framework/msp/inc/msp.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
#define OMAP4430_CONTROL_CORE   DEVICE_CORE_CONTROL_MODULE

/*--------data declarations -----------------------------------*/
CSI2_DEVICE_T CSI_DEV_LUT[2] = { CSI2_DEVICE_A, CSI2_DEVICE_B };

static csi2_ctx_cfg_t csi2ImageCtxDefaultParams = {
    CSI2_DISABLED_BIT,                                     // byteswap
    CSI2_DISABLED_BIT,                                     // generic
    1,                                                     // fec_number 
    0,                                                     // count 
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT, 
                                                           // //EOF_EN
    CSI2_DISABLED_BIT,                                     // EOL_EN
    CSI2_DISABLED_BIT,                                     // CS_EN
    CSI2_DISABLED_BIT,                                     // COUNT_UNLOCK
    CSI2_DISABLED_BIT,                                     // VP_FORCE
    CSI2_DISABLED_BIT,                                     // LINE_MODULO

    CSI2_USER_RAW6,                                        /* pixel format of 
                                                            * USER_DEFINED
                                                            * 0:RAW6; 1:RAW7; 
                                                            * 2:RAW8 */
    CSI2_CHANNEL_0,                                        /* Channel
                                                            * Id:0,1,2,3 */
    CSI2_SIMPLE_DPCM,                                      /* 0:Advanced;
                                                            * 1:Simple */
    // CSI2_RAW10_EXP16, /*Data format selection.*/ 
    CSI2_RAW10,                                            /* Data format
                                                            * selection. */

    0x0,                                                   /* alpha */
    200,                                                   /* line number */

    0x00,                                                  /* 8-bit
                                                            * interrupt-bit
                                                            * mask */

};

static csi2_ctx_cfg_t csi2EbdCtxDefaultParams = {
    CSI2_DISABLED_BIT,                                     // byteswap
    CSI2_DISABLED_BIT,                                     // generic
    1,                                                     // fec_number 
    0,                                                     // count 
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT, 
                                                           // //EOF_EN
    CSI2_DISABLED_BIT,                                     // EOL_EN
    CSI2_DISABLED_BIT,                                     // CS_EN
    CSI2_DISABLED_BIT,                                     // COUNT_UNLOCK
    CSI2_DISABLED_BIT,                                     // VP_FORCE
    CSI2_DISABLED_BIT,                                     // LINE_MODULO

    CSI2_USER_RAW6,                                        /* pixel format of 
                                                            * USER_DEFINED
                                                            * 0:RAW6; 1:RAW7; 
                                                            * 2:RAW8 */
    CSI2_CHANNEL_0,                                        /* Channel
                                                            * Id:0,1,2,3 */
    CSI2_SIMPLE_DPCM,                                      /* 0:Advanced;
                                                            * 1:Simple */
    // CSI2_RAW10_EXP16, /*Data format selection.*/ 
    CSI2_EMBEDDED_8BIT_NON_IMAGE_DATA,                     /* Data format
                                                            * selection. */

    0x0,                                                   /* alpha */
    200,                                                   /* line number */

    0x00,                                                  /* 8-bit
                                                            * interrupt-bit
                                                            * mask */

};

static csi2_complexio_cfg_t csi2ComplexioCfgDefaultParams = {

    CSI2_ENABLED_BIT,                                      /* Controls the
                                                            * reset of the
                                                            * complex IO--
                                                            * 0:Reset active; 
                                                            * 1:reset
                                                            * de-asserted */
    CSI2_ON,                                               /* 0:CSI2_OFF,
                                                            * 1:CSI2_ON,
                                                            * 2:CSI2_ULP */
    CSI2_AUTO,                                             /* Automatic
                                                            * switch between
                                                            * CSI2_ULP and
                                                            * CSI2_ON
                                                            * states--0:disable; 
                                                            * 1:enable */

    CSI2_PLUS_MINUS,                                       /* data lane and
                                                            * clock lane
                                                            * polarity 0:+/-
                                                            * pin order;
                                                            * 1:-/+ pin order 
                                                            */
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,
    CSI2_PLUS_MINUS,

    CSI2_NOT_USED,                                         /* data lane and
                                                            * clock lane
                                                            * positions:
                                                            * 1,2,3,4,5 */
    CSI2_NOT_USED,
    CSI2_NOT_USED,
    CSI2_POSITION_2,
    CSI2_POSITION_1
};

static csi2_phy_cfg_t csi2PhysicalDefaultParams = {
    0x00000000,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x0000000A,
    0x0000000B,
    0x0000000C,
    0x0000000D,
    0x0000000E,
    0x0000000F
};

static csi2_ctrl_cfg_t csi2CtrlDefaultParams = {
    CSI2_FIFO_6_BY_8,                                      // mflag_level
                                                           // level_h;
    CSI2_FIFO_4_BY_8,                                      // mflag_level
                                                           // level_l;
    CSI2_SIZE_16x64,                                       // burst_size
    CSI2_OCPCLK,                                           // video_clock_mode;
    CSI2_DISABLE_STREAMING,                                // streaming_mode;
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT,// 
                                                           // non_posted_write;
    CSI2_DISABLED_BIT,                                     // vp_only_enable;
    CSI2_DISABLED_BIT,                                     // dbg_enable;
    CSI2_NATIVE_MIPI,                                      // endianness
                                                           // little_big_endian;

    CSI2_DISABLED_BIT,                                     // ecc_enable;
    CSI2_DISABLED_BIT                                      // secure_mode;
};

static csi2_ctrl_cfg_t csi2CtrlDefaultParamsVpOnly = {
    CSI2_FIFO_6_BY_8,                                      // mflag_level
                                                           // level_h;
    CSI2_FIFO_4_BY_8,                                      // mflag_level
                                                           // level_l;
    CSI2_SIZE_16x64,                                       // burst_size
    CSI2_OCPCLK,                                           // video_clock_mode;
    CSI2_DISABLE_STREAMING,                                // streaming_mode;
    CSI2_ENABLED_BIT,                                      // CSI2_DISABLED_BIT,// 
                                                           // non_posted_write;
    CSI2_ENABLED_BIT,                                      // vp_only_enable;
    CSI2_DISABLED_BIT,                                     // dbg_enable;
    CSI2_NATIVE_MIPI,                                      // endianness
                                                           // little_big_endian;

    CSI2_DISABLED_BIT,                                     // ecc_enable;
    CSI2_DISABLED_BIT                                      // secure_mode;
};

static csi2_timing_cfg_t csi2TimingDefaultParams = {
    CSI2_DISABLED_BIT,                                     // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ; 
    0x1FFFu,                                               // stop_state_counter;

    CSI2_DISABLED_BIT,                                     // force_rx_mode;
    CSI2_COUNTER_x1,                                       // multiplier ; 
    0x1FFFu                                                // stop_state_counter;
};

/*--------function prototypes ---------------------------------*/

/* ===================================================================
 *  @func     issCsi2rxTranscodeConfig                                               
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
MSP_ERROR_TYPE issCsi2rxTranscodeConfig(IssCSI2DevT deviceNum,
                                        IssCsi2rxTranscodeParams_t *
                                        pTransParams, MSP_U32 nCtxNumber)
{
    IssCsi2rxTranscodeSizes_t *sizes = &pTransParams->sizes;

    MSP_ERROR_TYPE err = MSP_ERROR_NONE;

    csi2_ctx_transcode_cfg_t transcode;

    /* TRANSCODE MODE */
    switch (pTransParams->mode)
    {
        case ISS_CSI_TRANSCODE_DISABLE:
            transcode.transcode_mode = CSI2_DISABLE_TRANSCODE;
            break;
        case ISS_CSI_TRANSCODE_RAW10_EXP16:
            transcode.transcode_mode = CSI2_UNCOMPRESSED_RAW10_EXP16;
            break;
        case ISS_CSI_TRANSCODE_ALAW_RAW10:
            transcode.transcode_mode = CSI2_ALAW_RAW10;
            break;
        case ISS_CSI_TRANSCODE_DPCM_RAW10:
            transcode.transcode_mode = CSI2_DPCM_RAW10;
            break;
        case ISS_CSI_TRANSCODE_DPCM_RAW12:
            transcode.transcode_mode = CSI2_DPCM_RAW12;
            break;
    }
    transcode.hcount = sizes->h_size;
    transcode.hskip = sizes->h_start;
    transcode.vcount = sizes->v_size;
    transcode.vskip = sizes->v_start;

    err =
        csi2_config_context_transcode(CSI_DEV_LUT[deviceNum], nCtxNumber,
                                      &transcode);

    return (err);

}

/* ================================================================ */
/* 
 *  Description:- Configures CSI2 for the path sensor->CSI2->IPIPEIF
 *  
 *
 *  @param   csi2_cfg_t*
 
 *  @return         CSI2_RETURN
 *================================================================== */

/* ===================================================================
 *  @func     issCsi2rxParamsConfig                                               
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
static CSI2_RETURN issCsi2rxParamsConfig(csi2_cfg_t * cfg, void *imgBuff,
                                         void *imgBuff1,
                                         csi2_context_number number,
                                         MSP_U8 numLanes,
                                         MSP_U32 csiDDRspeedMhz)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    MSP_S8 tmpcnt = numLanes;

    cfg->master_standby_mode = CSI2_NO_STANDBY;            // CSI2_SMART_STANDBY;
    cfg->auto_idle_ocp_clock = CSI2_FREE_RUNNING;          // CSI2_AUTO_IDLE;

    cfg->context_set_flag |= (0x01u << number);

    // for(i=0;i<8;i++)
    // {
    // set the CSI2 context params accordingly
    switch (number)
    {
            // image data context
        case CSI2_CONTEXT_0:
            cfg->ctx_cfg_ptr_array[number] = &csi2ImageCtxDefaultParams;
            break;

            // embedded data context
        case CSI2_CONTEXT_1:
            cfg->ctx_cfg_ptr_array[number] = &csi2EbdCtxDefaultParams;
            break;

        default:
            ;                                              // throw an
                                                           // excpetion here
    }
    // cfg->ctx_cfg_ptr_array[i] = &csi2CtxDefaultParams;
    // }
    if (NULL != imgBuff1)
    {
        cfg->control = &csi2CtrlDefaultParams;
        (cfg->ctx_cfg_ptr_array[number])->vp_force = CSI2_ENABLED_BIT;  // CSI2_DISABLED_BIT;
    }
    else
    {
        cfg->control = &csi2CtrlDefaultParamsVpOnly;
        (cfg->ctx_cfg_ptr_array[number])->vp_force = CSI2_ENABLED_BIT;
        imgBuff1 = imgBuff;
    }
    // enable unexpected error interrupts
    cfg->csi2_irq_enable = 0x7F00;
    cfg->complexio_cfg_1 = &csi2ComplexioCfgDefaultParams;
    cfg->complexio_cfg_2 = &csi2ComplexioCfgDefaultParams;

    csi2ComplexioCfgDefaultParams.data1_position =
        (CSI2_LANE_POSITION_T) (tmpcnt-- >
                                0) ? (CSI2_POSITION_2) : (CSI2_NOT_USED);
    csi2ComplexioCfgDefaultParams.data2_position =
        (CSI2_LANE_POSITION_T) (tmpcnt-- >
                                0) ? (CSI2_POSITION_3) : (CSI2_NOT_USED);
    csi2ComplexioCfgDefaultParams.data3_position =
        (CSI2_LANE_POSITION_T) (tmpcnt-- >
                                0) ? (CSI2_POSITION_4) : (CSI2_NOT_USED);
    csi2ComplexioCfgDefaultParams.data4_position =
        (CSI2_LANE_POSITION_T) (tmpcnt-- >
                                0) ? (CSI2_POSITION_5) : (CSI2_NOT_USED);

    cfg->complexio_irq_enable_1 = 0x00000000u;
    cfg->complexio_irq_enable_2 = 0x00000000u;

    cfg->timing_io = &csi2TimingDefaultParams;

    // set proper timing on CSI D-PHY
    cfg->physical = &csi2PhysicalDefaultParams;
    cfg->physical->phy_00 =
        ((((12500 * csiDDRspeedMhz + 1000000 / 2) / 1000000) - 1) & 0xFF) << 8;
    cfg->physical->phy_00 |=
        ((((90000 * csiDDRspeedMhz + 1000000 / 2) / 1000000) + 3) & 0xFF);

    return retval;

}

/* ===================================================================
 *  @func     issCsi2rxInit                                               
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
MSP_ERROR_TYPE issCsi2rxInit(void)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    /* CSI2 configuration */

    retval = csi2_init();
    if (retval != CSI2_SUCCESS)
    {
        return retval;
    }
    retval = csi2_open();
    iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_ENABLE);
  
    retval |= csi2_reset(CSI2_DEVICE_A, 0);
    
    return retval;

}

/* ===================================================================
 *  @func     issCsi2rxStop                                               
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
MSP_ERROR_TYPE issCsi2rxStop(IssCSI2DevT deviceNum)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    retval = csi2_disable(CSI_DEV_LUT[deviceNum]);
    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxReset                                               
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
MSP_ERROR_TYPE issCsi2rxReset(IssCSI2DevT deviceNum)
{
    return csi2_reset(CSI_DEV_LUT[deviceNum], 0);
}

/* ===================================================================
 *  @func     issCsi2rxDeinit                                               
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
MSP_ERROR_TYPE issCsi2rxDeinit(void)
{

    CSI2_RETURN retval = CSI2_SUCCESS;

    retval = csi2_close();
    iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);

    {
        // MSP_U32 reg = 0;
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI22_LANEENABLE, 
        // 0x0);
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI21_LANEENABLE, 
        // 0x0);
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI22_CTRLCLKEN, 
        // 0x0);
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI22_CAMMODE,
        // 0x3);
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI21_CTRLCLKEN, 
        // 0x0);
        // MASKSET_FIELD(reg,
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX__CAMERARX_CSI21_CAMMODE,
        // 0x3);
        // WR_REG_32(OMAP4430_CONTROL_CORE, 
        // OMAP4430_CONTROL_CORE__CONTROL_CAMERA_RX,
        // reg);
        // * ( (unsigned int *) (0xAA100000 + 0x608) ) = 0x001B0000;
        // //replaced by macros above
    }

    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxConfigPingPong                                               
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
MSP_ERROR_TYPE issCsi2rxConfigPingPong(IssCSI_BufferIdx idx, void *imgBuff,
                                       IssCSI2DevT deviceNum,
                                       IssCSI2CtxT number)
{
    CSI2_RETURN retval = CSI2_FAILURE;

    if (NULL != imgBuff)
    {
        retval =
            csi2_config_context_ping_pong_buffer(CSI_DEV_LUT[deviceNum],
                                                 (csi2_context_number) number,
                                                 (CSI2_BUFFER_SELECTION_T) idx,
                                                 (uint32) (imgBuff));
    }
    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxConfigCtx                                               
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
MSP_ERROR_TYPE issCsi2rxConfigCtx(IssCSI2DevT deviceNum, IssCSI2CtxT number,
                                  csi2_cfg_t * pIssCsi2Config, void *imgBuff,
                                  void *imgBuff1, MSP_U32 lineByteOffst,
                                  MSP_U8 numLanes, MSP_U32 csiDDRspeedMhz)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    CSI2_DEVICE_T eDevice = CSI_DEV_LUT[deviceNum];

    retval =
        issCsi2rxParamsConfig(pIssCsi2Config, imgBuff, imgBuff1,
                              (csi2_context_number) number, numLanes,
                              csiDDRspeedMhz);

    if (NULL != imgBuff)
    {
        retval =
            csi2_config_context_buffer(eDevice, (csi2_context_number) number,
                                       CSI2_PING, (uint32) (imgBuff),
                                       lineByteOffst);
        retval =
            csi2_config_context_buffer(eDevice, (csi2_context_number) number,
                                       CSI2_PONG, (uint32) (imgBuff1),
                                       lineByteOffst);
    }

    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxConfig                                               
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
MSP_ERROR_TYPE issCsi2rxConfig(IssCSI2DevT deviceNum,
                               csi2_cfg_t * pIssCsi2Config,
                               IssCsi2rxTranscodeParams_t * pTransParams)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    MSP_U32 nContextIdx = 0;

    retval |= csi2_config(CSI_DEV_LUT[deviceNum], pIssCsi2Config);
    do
    {
        if (pTransParams->valid)
        {
            retval |=
                issCsi2rxTranscodeConfig(deviceNum, pTransParams, nContextIdx);
        }
        pTransParams++;
    } while (++nContextIdx < CAM_CSI_NUM_CTX);
    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxStart                                               
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
MSP_ERROR_TYPE issCsi2rxStart(IssCSI2DevT deviceNum, IssCSI2CtxT number)
{
    CSI2_RETURN retval = CSI2_SUCCESS;

    retval |=
        csi2_enable_context(CSI_DEV_LUT[deviceNum],
                            (csi2_context_number) number);
    retval |= csi2_start(CSI_DEV_LUT[deviceNum]);

    return retval;
}

/* ===================================================================
 *  @func     issCsi2rxGetLatestBuffer                                               
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
IssCSI_BufferIdx issCsi2rxGetLatestBuffer(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number)
{
    return ((IssCSI_BufferIdx)
            (csi2_get_latest_buffer
             (CSI_DEV_LUT[deviceNum], (csi2_context_number) number)));
}

/* ===================================================================
 *  @func     issCsi2rxFrameEndIsr                                               
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
void issCsi2rxFrameEndIsr(ISS_RETURN status, uint32 arg1, void *arg2)
{
    IssCsi2rxIsrHandleT *issCsiHandle = (IssCsi2rxIsrHandleT *) arg2;

    if (NULL != issCsiHandle->callback)
    {
        issCsiHandle->callback(MSP_ERROR_NONE, issCsiHandle->arg1,
                               issCsiHandle->arg2);
    }
}

/* ===================================================================
 *  @func     issCsi2rxUnhookFrameStartIsr                                               
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
MSP_ERROR_TYPE issCsi2rxUnhookFrameStartIsr(IssCSI2DevT deviceNum,
                                            IssCSI2CtxT number,
                                            IssCsi2rxIsrHandleT * handler)
{
    CSI2_RETURN status;

    Interrupt_Handle_T *csi_handle = (Interrupt_Handle_T *) handler->private;

    status =
        csi2_context_irq_unhook_int_handler(CSI_DEV_LUT[deviceNum],
                                            (CSI2_CONTEXT_T) number,
                                            CSI2_FS_IRQ, csi_handle);
    free(handler->private);
    handler->callback = NULL;
    return status;
}

/* ===================================================================
 *  @func     issCsi2rxHookFrameStartIsr                                               
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
MSP_ERROR_TYPE issCsi2rxHookFrameStartIsr(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number,
                                          IssCsi2rxIsrHandleT * handler)
{
    Interrupt_Handle_T *csi_handle;

    csi_handle = (Interrupt_Handle_T *) malloc(sizeof(Interrupt_Handle_T));
    if (NULL == csi_handle)
    {
        return MSP_ERROR_NULLPTR;
    }
    csi_handle->arg1 = 1;
    csi_handle->arg2 = handler;
    csi_handle->callback = issCsi2rxFrameEndIsr;
    csi_handle->context = ISS_CALLBACK_CONTEXT_HWI;
    csi_handle->priority = 3;
    csi_handle->list_next = NULL;
    handler->private = csi_handle;
    return csi2_context_irq_hook_int_handler(CSI_DEV_LUT[deviceNum],
                                             (CSI2_CONTEXT_T) number,
                                             CSI2_FS_IRQ, csi_handle);
}

/* ===================================================================
 *  @func     issCsi2rxUnhookFrameEndIsr                                               
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
MSP_ERROR_TYPE issCsi2rxUnhookFrameEndIsr(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number,
                                          IssCsi2rxIsrHandleT * handler)
{
    CSI2_RETURN status;

    Interrupt_Handle_T *csi_handle = (Interrupt_Handle_T *) handler->private;

    status =
        csi2_context_irq_unhook_int_handler(CSI_DEV_LUT[deviceNum],
                                            (CSI2_CONTEXT_T) number,
                                            CSI2_FE_IRQ, csi_handle);
    free(handler->private);
    handler->callback = NULL;
    return status;
}

/* ===================================================================
 *  @func     issCsi2rxHookFrameEndIsr                                               
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
MSP_ERROR_TYPE issCsi2rxHookFrameEndIsr(IssCSI2DevT deviceNum,
                                        IssCSI2CtxT number,
                                        IssCsi2rxIsrHandleT * handler)
{
    Interrupt_Handle_T *csi_handle;

    csi_handle = (Interrupt_Handle_T *) malloc(sizeof(Interrupt_Handle_T));
    if (NULL == csi_handle)
    {
        return MSP_ERROR_NULLPTR;
    }
    csi_handle->arg1 = 1;
    csi_handle->arg2 = handler;
    csi_handle->callback = issCsi2rxFrameEndIsr;
    csi_handle->context = ISS_CALLBACK_CONTEXT_HWI;
    csi_handle->priority = 3;
    csi_handle->list_next = NULL;
    handler->private = csi_handle;
    return csi2_context_irq_hook_int_handler(CSI_DEV_LUT[deviceNum],
                                             (CSI2_CONTEXT_T) number,
                                             CSI2_FE_IRQ, csi_handle);
}

/* ===================================================================
 *  @func     issCsi2rxSetLineNum                                               
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
MSP_ERROR_TYPE issCsi2rxSetLineNum(IssCSI2DevT deviceNum, IssCSI2CtxT number,
                                   MSP_U32 lineNum)
{
    return csi2_setIntr_lineNum(CSI_DEV_LUT[deviceNum], (CSI2_CONTEXT_T) number,
                                (uint32) lineNum);
}

/* ===================================================================
 *  @func     issCsi2rxUnhookLineNumIsr                                               
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
MSP_ERROR_TYPE issCsi2rxUnhookLineNumIsr(IssCSI2DevT deviceNum,
                                         IssCSI2CtxT number,
                                         IssCsi2rxIsrHandleT * handler)
{
    CSI2_RETURN status;

    Interrupt_Handle_T *csi_handle = (Interrupt_Handle_T *) handler->private;

    status =
        csi2_context_irq_unhook_int_handler(CSI_DEV_LUT[deviceNum],
                                            (CSI2_CONTEXT_T) number,
                                            CSI2_LINE_NUMBER_IRQ, csi_handle);
    free(handler->private);
    handler->callback = NULL;
    return status;
}

/* ===================================================================
 *  @func     issCsi2rxHookLineNumIsr                                               
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
MSP_ERROR_TYPE issCsi2rxHookLineNumIsr(IssCSI2DevT deviceNum,
                                       IssCSI2CtxT number,
                                       IssCsi2rxIsrHandleT * handler,
                                       MSP_U32 lineNum)
{
    Interrupt_Handle_T *csi_handle;

    csi_handle = (Interrupt_Handle_T *) malloc(sizeof(Interrupt_Handle_T));
    if (NULL == csi_handle)
    {
        return MSP_ERROR_NULLPTR;
    }
    csi_handle->arg1 = 1;
    csi_handle->arg2 = handler;
    csi_handle->callback = issCsi2rxFrameEndIsr;
    csi_handle->context = ISS_CALLBACK_CONTEXT_HWI;
    csi_handle->priority = 3;
    csi_handle->list_next = NULL;
    handler->private = csi_handle;
    csi2_setIntr_lineNum(CSI_DEV_LUT[deviceNum], (CSI2_CONTEXT_T) number,
                         (uint32) lineNum);
    return csi2_context_irq_hook_int_handler(CSI_DEV_LUT[deviceNum],
                                             (CSI2_CONTEXT_T) number,
                                             CSI2_LINE_NUMBER_IRQ, csi_handle);
}

/* FIFO OVF Start
 * ------------------------------------------------------------------------------------------------- 
 */
/* ===================================================================
 *  @func     issCsi2rxUnhookFifoOvfIsr                                               
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
MSP_ERROR_TYPE issCsi2rxUnhookFifoOvfIsr(IssCSI2DevT deviceNum,
                                         IssCsi2rxIsrHandleT * handler)
{
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxHookFifoOvfIsr                                               
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
MSP_ERROR_TYPE issCsi2rxHookFifoOvfIsr(IssCSI2DevT deviceNum,
                                       IssCsi2rxIsrHandleT * handler)
{
    csi2_register_interrupt_handler((CSI2_DEVICE_T) deviceNum,
                                    CSI2_FIFO_OVF_IRQ, handler->callback,
                                    handler->arg1, handler->arg2);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxFifoOvfEnable                                               
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
MSP_ERROR_TYPE issCsi2rxFifoOvfEnable(IssCSI2DevT deviceNum)
{
    csi2_enable_interrupt((CSI2_DEVICE_T) deviceNum, CSI2_FIFO_OVF_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxFifoOvfDisable                                               
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
MSP_ERROR_TYPE issCsi2rxFifoOvfDisable(IssCSI2DevT deviceNum)
{
    csi2_disable_interrupt((CSI2_DEVICE_T) deviceNum, CSI2_FIFO_OVF_IRQ);
    return MSP_ERROR_NONE;
}

/* FIFO OVF end
 * ------------------------------------------------------------------------------------------------------- 
 */

/* ===================================================================
 *  @func     issCsi2rxFrameEndIrqEnable                                               
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
MSP_ERROR_TYPE issCsi2rxFrameEndIrqEnable(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number)
{
    csi2_context_irq_enable_interrupt(CSI_DEV_LUT[deviceNum],
                                      (CSI2_CONTEXT_T) number, CSI2_FE_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxSetIntLineNum                                               
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
MSP_ERROR_TYPE issCsi2rxSetIntLineNum(IssCSI2DevT deviceNum, IssCSI2CtxT number,
                                      MSP_U32 lineNum)
{
    if (csi2_setIntr_lineNum
        (CSI_DEV_LUT[deviceNum], (CSI2_CONTEXT_T) number, (uint32) lineNum))
        return MSP_ERROR_FAIL;

    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxFrameStartIrqEnable                                               
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
MSP_ERROR_TYPE issCsi2rxFrameStartIrqEnable(IssCSI2DevT deviceNum,
                                            IssCSI2CtxT number)
{
    csi2_context_irq_enable_interrupt(CSI_DEV_LUT[deviceNum],
                                      (CSI2_CONTEXT_T) number, CSI2_FS_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxLineNumIrqEnable                                               
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
MSP_ERROR_TYPE issCsi2rxLineNumIrqEnable(IssCSI2DevT deviceNum,
                                         IssCSI2CtxT number)
{
    csi2_context_irq_enable_interrupt(CSI_DEV_LUT[deviceNum],
                                      (CSI2_CONTEXT_T) number,
                                      CSI2_LINE_NUMBER_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxFrameEndIrqDisable                                               
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
MSP_ERROR_TYPE issCsi2rxFrameEndIrqDisable(IssCSI2DevT deviceNum,
                                           IssCSI2CtxT number)
{
    csi2_context_irq_disable_interrupt(CSI_DEV_LUT[deviceNum],
                                       (CSI2_CONTEXT_T) number, CSI2_FE_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxFrameStartIrqDisable                                               
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
MSP_ERROR_TYPE issCsi2rxFrameStartIrqDisable(IssCSI2DevT deviceNum,
                                             IssCSI2CtxT number)
{
    csi2_context_irq_disable_interrupt(CSI_DEV_LUT[deviceNum],
                                       (CSI2_CONTEXT_T) number, CSI2_FS_IRQ);
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     issCsi2rxLineNumIrqDisable                                               
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
MSP_ERROR_TYPE issCsi2rxLineNumIrqDisable(IssCSI2DevT deviceNum,
                                          IssCSI2CtxT number)
{
    csi2_context_irq_disable_interrupt(CSI_DEV_LUT[deviceNum],
                                       (CSI2_CONTEXT_T) number,
                                       CSI2_LINE_NUMBER_IRQ);
    return MSP_ERROR_NONE;
}
