/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_rot.h
*
* This file contains Level 1 CSL interface types and function prototypes
*used for setting the registers of H/W  ROT module in SIMCOP in OMAP4/Monica
*
* @path ROT\inc
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*!
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
*! 08-Aug-2008 Anandhi: Initial version/Created this file
*========================================================================= */

#ifndef _CSL_ROT_H_
#define _CSL_ROT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
    // define s
    /* OPERATION TYPES */

/****************************************************************************
* CSL_RotOpType enumeration for selecting type of operation
*
* @param CSL_SIMCOP_ROT_ZERO_ROTATION            Data Copy
* @param CSL_SIMCOP_ROT_90DEG_ROTATION          90deg Rotation
* @param CSL_SIMCOP_ROT_180DEG_ROTATION        180deg Rotation
* @param CSL_SIMCOP_ROT_270DEG_ROTATION       270deg Rotation
* @param CSL_SIMCOP_ROT_DATA_SHIFT                  Data Shift operation
* @param CSL_SIMCOP_ROT_HOR_CIRCULAR_SHIFT  Horizontal Circular Shift Operation
*
***********************************************************************************/
    typedef enum {
        CSL_SIMCOP_ROT_ZERO_ROTATION = CSL_ROT_CFG_OP_ROTATE_0DEG,
        CSL_SIMCOP_ROT_90DEG_ROTATION = CSL_ROT_CFG_OP_ROTATE_90DEG,
        CSL_SIMCOP_ROT_180DEG_ROTATION = CSL_ROT_CFG_OP_ROTATE_180DEG,
        CSL_SIMCOP_ROT_270DEG_ROTATION = CSL_ROT_CFG_OP_ROTATE_270DEG,
        CSL_SIMCOP_ROT_DATA_SHIFT = CSL_ROT_CFG_OP_DATA_SHIFT,
        CSL_SIMCOP_ROT_HOR_CIRCULAR_SHIFT = CSL_ROT_CFG_OP_HOR_CIRC_SHIFT
    } CSL_RotOpType;

    /* DATA TYPES */

/****************************************************************************
* CSL_RotFmtType enumeration for selecting type of data input
*
* @param CSL_SIMCOP_ROT_8BIT_DATA    8bit data
* @param CSL_SIMCOP_ROT_16BIT_DATA   16bit data
* @param CSL_SIMCOP_ROT_32BIT_DATA   32bit data
* @param CSL_SIMCOP_ROT_YUV422_DATA YUV422 data
* @param CSL_SIMCOP_ROT_YUV420_DATA YUV420 data
*
****************************************************************************/
    typedef enum {
        CSL_SIMCOP_ROT_8BIT_DATA = CSL_ROT_CFG_FMT_8_BIT,
        CSL_SIMCOP_ROT_16BIT_DATA = CSL_ROT_CFG_FMT_16_BIT,
        CSL_SIMCOP_ROT_32BIT_DATA = CSL_ROT_CFG_FMT_32_BIT,
        CSL_SIMCOP_ROT_YUV422_DATA = CSL_ROT_CFG_FMT_YUV_422,
        CSL_SIMCOP_ROT_YUV420_DATA = CSL_ROT_CFG_FMT_YUV_420
    } CSL_RotFmtType;

#if 0
    /* TRIGGER SOURCE TYPES */
/********************************************************************************
*
* CSL_RotTrgSrcType enumeration for checking trigger source
*
* @param CSL_SIMCOP_ROT_TRIG_MMR_WRITE               Source is MMR Write
* @param CSL_SIMCOP_ROT_TRIG_HWSTART_SIGNAL    Source is Hardware Start Signal
*
*********************************************************************************/
    typedef enum {
        CSL_SIMCOP_ROT_TRIG_MMR_WRITE = CSL_ROT_CFG_TRIG_SRC_MMR_WRITE,
        CSL_SIMCOP_ROT_TRIG_HWSTART_SIGNAL =
            CSL_ROT_CFG_TRIG_SRC_HW_START_SIGNAL
    } CSL_RotTrgSrcType;

    /* HARDWARE STATUS TYPES */
/********************************************************************************
*
* CSL_RotHwStatus enumeration for checking busy status of ROT
*
* @param CSL_SIMCOP_ROT_HWSTATUS_IDLE    ROT is Idle
* @param CSL_SIMCOP_ROT_HWSTATUS_BUSY    ROT is Busy
*
*********************************************************************************/
    typedef enum {
        CSL_SIMCOP_ROT_HWSTATUS_IDLE = CSL_ROT_CTRL_BUSY_IDLE,
        CSL_SIMCOP_ROT_HWSTATUS_BUSY = CSL_ROT_CTRL_BUSY_BUSY
    } CSL_RotHwStatusType;

    /* HARDWARE ENABLE TYPES */
/********************************************************************************
*
* CSL_RotHwEnableType enumeration for setting ENABLE bit
*
* @param CSL_SIMCOP_ROT_DISABLE    Disable ROT
* @param CSL_SIMCOP_ROT_ENABLE     Enable ROT
*
*********************************************************************************/
    typedef enum {
        CSL_SIMCOP_ROT_DISABLE = CSL_ROT_CTRL_EN_DISABLE,
        CSL_SIMCOP_ROT_ENABLE = CSL_ROT_CTRL_EN_ENABLE
    } CSL_RotHwEnableType;

    /* AUTOGATING ENABLE TYPES */
/********************************************************************************
*
* CSL_RotAutoGatingType enumeration for setting Autogating on or off
*
* @param CSL_SIMCOP_AUTOGATING_OFF    Disable Autogating
* @param CSL_SIMCOP_AUTOGATING_ON     Enable Autogating
*
*********************************************************************************/
    typedef enum {
        CSL_SIMCOP_AUTOGATING_OFF = CSL_ROT_CFG_AUTOGATING_DISABLE,
        CSL_SIMCOP_AUTOGATING_ON = CSL_ROT_CFG_AUTOGATING_ENABLE
    } CSL_RotAutoGatingType;
#else
    typedef CSL_HwAutoGatingType CSL_RotAutoGatingType;
    typedef CSL_HwEnableType CSL_RotHwEnableType;
    typedef CSL_HwTrgSrcType CSL_RotTrgSrcType;
    typedef CSL_HwIntEnableType CSL_RotIntEnableType;
    typedef CSL_HwStatusType CSL_RotHwStatusType;
#endif
    // data structures

/*******************************************************************
* CSL_RotBlkSizeConfig - Rotation Block size structure
* Type RW
*******************************************************************/
    typedef struct {
        /* Number of blocks minus 1 */
        Uint8 nBlocks;
        /* Block width, in pixels (YUV420/422) or data units (8-/16-/32-bit).
         * Should be a multiple of 8, at least 8, for rotation. Should be at
         * least 4 for data shifting */
        Uint16 nBlockWidth;
        /* Block height, in pixels (YUV420/422) or number of rows
         * (8-/16-/32-bit). Should be a multiple of 8, at least 8, for
         * rotation. Should be at least 1 for data shifting */
        Uint16 nBlockHeight;

    } CSL_RotBlkSizeConfig;

/*******************************************************************
* CSL_RotStartAddrConfig - Rotation Start Block Address Config structure
* Type RW
*******************************************************************/
    typedef struct {
        /* Source starting address. Should be a multiple of 8 for rotation.
         * No constraint for data shifting. */
        Uint16 nSourceStart;
        /* Source Line offset in bytes Should be a multiple of 8 for rotation 
         * and normal data shifting. Should be a power of 2 and at least 32
         * for horizontal circular shifting. */
        Uint16 nSourceLineOffset;
        /* Source starting address for UV data in case of YUV420. Should be a 
         * multiple of 8 for rotation. No constraint for data shifting. */
        Uint16 nSourceStartUV;
    } CSL_RotStartAddrConfig;

/*******************************************************************
* CSL_RotDestAddrConfig - Rotation Destination Block Address Config structure
* Type RW
*******************************************************************/
    typedef struct {
        /* Destination starting address. Should be a multiple of 8 for
         * rotation. No constraint for data shifting. */
        Uint16 nDestStart;
        /* Destination Line offset in bytes Should be a multiple of 8 for
         * rotation and normal data shifting. Should be a power of 2 and at
         * least 32 for horizontal circular shifting. */
        Uint16 nDestLineOffset;
        /* Destination starting address for UV data in case of YUV420. Should 
         * be a multiple of 8 for rotation. No constraint for data shifting. */
        Uint16 nDestStartUV;
    } CSL_RotDestAddrConfig;

/*******************************************************************
* CSL_RotObj - Rotation Object structure
* Type RW
*******************************************************************/
    typedef struct {

        /** This is the mode which the CSL instance is opened     */
        CSL_OpenMode openMode;
        /** This is a unique identifier to the instance of ROT being
         *  referred to by this object
         */
        CSL_Uid uid;
        /** This is the variable that contains the current state of a
         *  resource being shared by current instance of ROT with
         *  other peripherals
         */
        CSL_Xio xio;
        /** This is a pointer to the registers of the instance of ROT
         *  referred to by this object
         */
        CSL_RotRegsOvly regs;
        /** This is the instance of ROT being referred to by this object  */
        CSL_RotNum perNum;

    } CSL_RotObj;
#if 0
/****************************************************************************
* CSL_RotHwCtrlCmd enumeration for selecting type of operation
*
* @param CSL_ROT_CMD_SETOPERATION       Set Rotation or Shift type
* @param CSL_ROT_CMD_SETFMT             Set Format of input data
* @param CSL_ROT_CMD_SETTRIGGERSOURCE   Set Trigger Source Type
* @param CSL_ROT_CMD_SETAUTOGATING      Set Autogating on or off
* @param CSL_ROT_CMD_SETSTARTADDR       Set Start Address and offset
* @param CSL_ROT_CMD_SETDESTADDR        Set Destination Address and offset
* @param CSL_ROT_CMD_SETBLOCKPARAM      Set Block size and number
* @param CSL_ROT_CMD_ENABLE             Enable ROT module
*
***********************************************************************************/
    typedef enum {

        CSL_ROT_CMD_SETOPERATION,
        CSL_ROT_CMD_SETFMT,
        CSL_ROT_CMD_SETTRIGGERSOURCE,
        CSL_ROT_CMD_SETAUTOGATING,
        CSL_ROT_CMD_SETSTARTADDR,
        CSL_ROT_CMD_SETDESTADDR,
        CSL_ROT_CMD_SETBLOCKPARAM,
        CSL_ROT_CMD_ENABLE
    } CSL_RotHwCtrlCmd;

/****************************************************************************
* CSL_RotHwQuery enumeration to list query types
*
* @param CSL_ROT_QUERY_REV_ID          Get Revision ID
* @param CSL_ROT_QUERY_HWSTATUS        Get ROT Idle/Busy
* @param CSL_ROT_QUERY_OPERATION       Get Rotation or Shift type
* @param CSL_ROT_QUERY_FMT             Get Format of input data
* @param CSL_ROT_QUERY_TRIGGERSOURCE   Get Trigger Source Type
* @param CSL_ROT_QUERY_AUTOGATING      Get Autogating on or off
* @param CSL_ROT_QUERY_STARTADDR       Get Start Address and offset
* @param CSL_ROT_QUERY_DESTADDR        Get Destination Address and offset
* @param CSL_ROT_QUERY_BLOCKPARAM      Get Block size and number
*
***********************************************************************************/

    typedef enum {

        CSL_ROT_QUERY_REV_ID,
        CSL_ROT_QUERY_HWSTATUS,
        CSL_ROT_QUERY_OPERATION,
        CSL_ROT_QUERY_FMT,
        CSL_ROT_QUERY_TRIGGERSOURCE,
        CSL_ROT_QUERY_AUTOGATING,
        CSL_ROT_QUERY_STARTADDR,
        CSL_ROT_QUERY_DESTADDR,
        CSL_ROT_QUERY_BLOCKPARAM
    } CSL_RotHwQuery;
#endif
/*******************************************************************
* CSL_RotHwSetup - Rotation Hardware Setup structure
* Type RW
*******************************************************************/
    typedef struct CSL_RotHwSetup_ {

        /* ROT Enable Parameter */
        CSL_RotHwEnableType HWEnable;
        /* Autogating Parameter */
        CSL_RotAutoGatingType Autogating;
        /* Trigger Source Type MMR_WRITE or HWSTART_SIGNAL */
        CSL_RotTrgSrcType TrgSrcType;
        /* Operation type, Rotate 90/180/270 or data shift */
        CSL_RotOpType OpType;
        /* Data type, YUV420/422, 8/16/32 bit data */
        CSL_RotFmtType FmtType;
        /* Block Height, Width and Number of blocks */
        CSL_RotBlkSizeConfig BlkParam;
        /* Start address for Y, UV, offset */
        CSL_RotStartAddrConfig StartAddrConfig;
        /* Destination address for Y, UV, offset */
        CSL_RotDestAddrConfig DestAddrConfig;

    } CSL_RotHwSetup;

    typedef CSL_RotObj *CSL_RotHandle;

    /* functions */
    CSL_Status CSL_rotInit(CSL_RotHandle hndl);
    CSL_Status CSL_rotOpen(CSL_RotObj * hRotObj, CSL_RotNum rotNum,
                           CSL_OpenMode openMode);
    CSL_Status CSL_rotClose(CSL_RotHandle hndl);
    CSL_Status CSL_rotHwSetup(CSL_RotHandle hndl, CSL_RotHwSetup * data);

#ifdef __cplusplus
}
#endif
#endif
