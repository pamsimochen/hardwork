/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_dct.h
*
* This file contains Level 1 CSL function prototypes
*used for setting the registers of H/W  DCT module in SIMCOP
*
* @path DCT\inc
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS 
*!
*! 9-Sep     2008 Sowmya Priya : Initial Release
*========================================================================= */

#ifndef _CSL_DCT_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_DCT_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "cslr__dct_001.h"
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

/* ======================================================================= */
/* CSL_DctModeType enumeration for selecting mode of operation * * @param
 * CSL_SIMCOP_MODE_DCT DCT mode * @param CSL_SIMCOP_MODE_IDCT IDCT Mode */
/* ========================================================================== 
 */

typedef enum {
    CSL_SIMCOP_MODE_DCT = CSL_DCT_CFG_MODE_DCT,
    CSL_SIMCOP_MODE_IDCT = CSL_DCT_CFG_MODE_IDCT
} CSL_DctModeType;

/* ======================================================================= */
/* CSL_DctSpDataType enumeration for selecting mode of operation * * @param
 * CSL_SIMCOP_DCT_SPDATA_YUV420 YUV420 spatial data * @param
 * CSL_SIMCOP_DCT_SPDATA_YUV422 YUV422 spatial data * @param
 * CSL_SIMCOP_DCT_SPDATA_SEQBLK Sequential Blocks of spatial data */
/* ========================================================================== 
 */

typedef enum {
    CSL_SIMCOP_DCT_SPDATA_YUV420 = CSL_DCT_CFG_FMT_YUV420,
    CSL_SIMCOP_DCT_SPDATA_YUV422 = CSL_DCT_CFG_FMT_YUV422,
    CSL_SIMCOP_DCT_SPDATA_SEQBLK = CSL_DCT_CFG_FMT_SEQBLOCKS
} CSL_DctSpDataType;

/* ======================================================================= */
/* 
 *All  H/w signals generic to modules in SIMCOP
 * CSL_HwAutoGatingType enumeration for setting Autogating on or off
 * CSL_HwEnableType enumeration for setting the H/W on or off
 * CSL_HwTrgSrcType enumeration for checking trigger source (Sequencer/MMR)
 * CSL_HwStatus enumeration for checking busy status of H/W
 * CSL_HwIntEnableType enumeration for setting Interrupts
 */
/* ========================================================================== 
 */

typedef CSL_HwAutoGatingType CSL_DctAutoGatingType;

typedef CSL_HwEnableType CSL_DctHwEnableType;

typedef CSL_HwTrgSrcType CSL_DctTrgSrcType;

typedef CSL_HwIntEnableType CSL_DctIntEnableType;

typedef CSL_HwStatusType CSL_DctHwStatusType;

/* ========================================================================== 
 */
/**
* This is a structure description for DCT Object.
*
* ========================================================================== */

typedef struct {

    /** This is the mode which the CSL instance is opened     */
    CSL_OpenMode openMode;
    /** This is a unique identifier to the instance of DCT being
    *  referred to by this object
     */
    CSL_Uid uid;
    /** This is the variable that contains the current state of a
    *  resource being shared by current instance of DCT with
    *  other peripherals
    */
    CSL_Xio xio;
    /** This is a pointer to the registers of the instance of DCT
    *  referred to by this object
    */
    CSL_DctRegsOvly regs;
    /** This is the instance of DCT being referred to by this object  */
    CSL_DctNum perNum;

} CSL_DctObj;

#if 0
/* ======================================================================= */
/* CSL_DctHwCtrlCmd enumeration for selecting type of operation * * @param
 * CSL_DCT_CMD_SETMODE Set DCT or IDCT mode * @param CSL_DCT_CMD_SETSPDATAFMT 
 * Set Spatial Data format * @param CSL_DCT_CMD_SETTRIGGERSOURCE Set Trigger
 * Source Type * @param CSL_DCT_CMD_SETAUTOGATING Set Autogating on or off *
 * @param CSL_DCT_CMD_SETSPDATAADDR Set Spatial Data address * @param
 * CSL_DCT_CMD_SETFRDATAADDR Set Frequency Data address * @param
 * CSL_DCT_CMD_SETNMCUS Set Number of MCUs * @param CSL_DCT_CMD_ENABLEDCT
 * Enable DCT module * @param CSL_DCT_CMD_ENABLEINT Enable DCT interrupt *
 * *========================================================================== 
 */

typedef enum {
    CSL_DCT_CMD_SETMODE,
    CSL_DCT_CMD_SETSPDATAFMT,
    CSL_DCT_CMD_SETTRIGGERSOURCE,
    CSL_DCT_CMD_SETAUTOGATING,
    CSL_DCT_CMD_SETSPDATAADDR,
    CSL_DCT_CMD_SETFRDATAADDR,
    CSL_DCT_CMD_SETNMCUS,
    CSL_DCT_CMD_ENABLE,
    CSL_DCT_CMD_ENABLEINT
} CSL_DctHwCtrlCmd;

/* ======================================================================= */
/* CSL_DctHwQuery enumeration to list query types * * @param
 * CSL_DCT_QUERY_REV_ID Get Revision ID * @param CSL_DCT_QUERY_HWSTATUS Get
 * DCT Idle/Busy * @param CSL_DCT_QUERY_OPERATION Get Mode DCT or IDCT *
 * @param CSL_DCT_QUERY_FMT Get Format of input data * @param
 * CSL_DCT_QUERY_TRIGGERSOURCE Get Trigger Source Type * @param
 * CSL_DCT_QUERY_AUTOGATING Get Autogating on or off * @param
 * CSL_DCT_QUERY_STARTADDR Get Start Address and offset * @param
 * CSL_DCT_QUERY_DESTADDR Get Destination Address and offset * @param
 * CSL_DCT_QUERY_BLOCKPARAM Get Block size and number */
/* ========================================================================== 
 */

typedef enum {
    CSL_DCT_QUERY_REV_ID,
    CSL_DCT_QUERY_HWSTATUS,
    CSL_DCT_QUERY_INTEN,
    CSL_DCT_QUERY_MODE,
    CSL_DCT_QUERY_SPDATAFMT,
    CSL_DCT_QUERY_TRIGGERSOURCE,
    CSL_DCT_QUERY_AUTOGATING,
    CSL_DCT_QUERY_SPDATAADDR,
    CSL_DCT_QUERY_FRDATAADDR,
    CSL_DCT_QUERY_NMCUS
} CSL_DctHwQuery;

#endif

/* ======================================================================= */
/* CSL_DctHwSetup - DCT Hardware Setup structure * Type RW *
 * ========================================================================== 
 */

typedef struct CSL_DctHwSetup_ {
    CSL_DctHwEnableType HwEnable;
    /* Autogating Parameter */
    CSL_DctAutoGatingType Autogating;
    /* Trigger Source Type MMR_WRITE or HWSTART_SIGNAL */
    CSL_DctTrgSrcType TrgSrcType;
    /* Number of MCUs */
    Uint16 nMcus;
    /* Mode type, DCT or IDCT */
    CSL_DctModeType nModeType;
    /* Spatial Data type, YUV420/422, sequential blocks */
    CSL_DctSpDataType SpFmtType;
    /* Start address for Spatial data */
    Uint32 pSpDataAddr;
    /* Start address for Frequency data */
    Uint32 pFreqDataAddr;
    /* Interrupt enable for dct hw */
    CSL_DctIntEnableType IntEnable;

} CSL_DctHwSetup;

/* Handle to DCT */

typedef CSL_DctObj *CSL_DctHandle;

/*****************************************************************************/
/* CSL Level 1 functions to Intialize , setup and control and deinitialize
 * the DCT block */
 /*****************************************************************************/

/* ===================================================================
 *  @func     CSL_dctInit                                               
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
CSL_Status CSL_dctInit(CSL_DctHandle hndl);

/* ===================================================================
 *  @func     CSL_dctOpen                                               
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
CSL_Status CSL_dctOpen(CSL_DctObj * hDctObj,
                       CSL_DctNum dctNum, CSL_OpenMode openMode);
/* ===================================================================
 *  @func     CSL_dctClose                                               
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
CSL_Status CSL_dctClose(CSL_DctHandle hndl);

/* ===================================================================
 *  @func     CSL_dctHwSetup                                               
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
CSL_Status CSL_dctHwSetup(CSL_DctHandle hndl, CSL_DctHwSetup * setup);

#if 0
/* ===================================================================
 *  @func     CSL_dctHwControl                                               
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
CSL_Status CSL_dctHwControl(CSL_DctHandle hndl, CSL_DctHwCtrlCmd cmd,
                            void *data);
/* ===================================================================
 *  @func     CSL_dctGetHwStatus                                               
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
CSL_Status CSL_dctGetHwStatus(CSL_DctHandle hndl, CSL_DctHwQuery query,
                              void *data);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_CSL_DCT_H_*/
