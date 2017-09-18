/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_simcop_dma.h
*
* This File contains declarations for csl_simcop_dma.h
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\src\DMA\
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *!
 *! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
 *!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS
 *!
 *! 23-Sep-2008 Geetha Ravindran:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#ifndef _CSL_SIMCOP_DMA_H_
#define _CSL_SIMCOP_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif
    // #include<stdio.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
    // #include <ti/psp/iss/hal/iss/simcop/simcop_global/_csl_simcop.h>
    // #include "../cslr_simcop_dma_1.h"

#define CHAN_0 0
#define CHAN_1 1
#define CHAN_2 2
#define CHAN_3 3
#define CHAN_4 4
#define CHAN_5 5
#define CHAN_6 6
#define CHAN_7 7
#define MAX_NUM_OF_DMACHANNELS  8
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaObj - Simcop Dma Object structure
     * @param  openMode  This is the mode which the CSL instance is opened
     * @param  uid  This is a unique identifier to the instance of SIMCOP DMA being referred to by this object
     * @param  xio  This is the variable that contains the current state of a resource being shared by current instance of 
     SIMCOP DMA with other peripherals
     * @param  regs  This is a pointer to the registers of the instance of SIMCOP DMA referred to by this object
     * @param  perNum  This is the instance of SIMCOP DMA being referred to by this object
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_OpenMode openMode;
        CSL_Uid uid;
        CSL_Xio xio;
        CSL_SimcopDmaRegsOvly regs;
        CSL_SimcopDmaNum perNum;
    } CSL_SimcopDmaObj;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaHandle - Pointer to CSL_SimcopDmaObj
     */
    /* ========================================================================== 
     */

    typedef CSL_SimcopDmaObj *CSL_SimcopDmaHandle;
    /* 
     * Enums grouping Field value Tokens
     */

    /* ========================================================================== 
     */
/** CSL_SimcopDmaStandByModeType  describes the possible values for STANDBYMODE field in SYSCONFIG register.  
*     The following tokens are directly mapped 
* 
* 	Force-standby mode	: 	Local initiator is unconditionally placed in standby state.Backup mode, for debug only 
*     No-standby mode		: 	Local initiator is unconditionally placed out of standby state.Backup mode, for debug only. 
*     Smart standby mode0 	: 	Check functional specification for details. 
*     Smart standby mode1 	: 	Check functional specification for details. 
*     
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_SYSCONFIG_STANDBYMODE_FORCED =
            CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_FORCE_STANDBY,
        CSL_DMA_SYSCONFIG_STANDBYMODE_NOSTANDBY =
            CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_NO_STANDBY,
        CSL_DMA_SYSCONFIG_STANDBYMODE_SMART0 =
            CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_SMART_IDLE0,
        CSL_DMA_SYSCONFIG_STANDBYMODE_SMART1 =
            CSL_SIMCOP_DMA_DMASYSCONFIG_STANDBYMODE_SMART_IDLE1
    } CSL_SimcopDmaStandByModeType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaSoftResetWriteType describes the possible values written in SOFTRESET field in SYSCONFIG register.  
*     The following tokens are directly mapped 
* 
* 	Initiate_SWReset	: 	Initiate software reset 
*    	NoAction			: 	No action 
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_SYSCONFIG_SOFTRESET_W_NOACTION =
            CSL_SIMCOP_DMA_DMASYSCONFIG_SOFTRESET_W_NO_ACTION,
        CSL_DMA_SYSCONFIG_SOFTRESET_W_INITIATE_SWRESET =
            CSL_SIMCOP_DMA_DMASYSCONFIG_SOFTRESET_W_INITIATE_SOFTWARE_RESET
    } CSL_SimcopDmaSoftResetWriteType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaSoftResetReadType describes the possible values read  from SOFTRESET field in SYSCONFIG register.  
*     The following tokens are directly mapped 
* 
* 	ResetOngoing			: 	Reset (software or other) ongoing 
*    	ResetDone			: 	Reset done, no pending action  
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_SYSCONFIG_SOFTRESET_R_RESETDONE =
            CSL_SIMCOP_DMA_DMASYSCONFIG_SOFTRESET_R_RESET_DONE,
        CSL_DMA_SYSCONFIG_SOFTRESET_R_RESETONGOING =
            CSL_SIMCOP_DMA_DMASYSCONFIG_SOFTRESET_R_RESET_ONGOING
    } CSL_SimcopDmaSoftResetReadType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaIRQEOIType describes the possible values written in LINE_NUMBER field in IRQEOI register.  
*     The following tokens are directly mapped 
* 
* 	Initiate_SWReset	: 	EOI for interrupt output line #0 
*    	NoAction			: 	No action 
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_IRQEOI_LINE_NUMBER_LINE0 =
            CSL_SIMCOP_DMA_DMAIRQEOI_LINE_NUMBER_W_EOI_LINE0,
        CSL_DMA_IRQEOI_LINE_NUMBER_LINE1 =
            CSL_SIMCOP_DMA_DMAIRQEOI_LINE_NUMBER_W_EOI_LINE1
    } CSL_SimcopDmaIRQEOIType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaPostedWritesType describes the possible values written in POSTED_WRITES field in DMA_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	NonPosted		: 	Only non posted writes are generated 
*     Posted			: 	Only posted writes are generated  
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CTRL_POSTED_WRITES_NONPOSTED =
            CSL_SIMCOP_DMA_DMACTRL_POSTED_WRITES_NON_POSTED,
        CSL_DMA_CTRL_POSTED_WRITES_POSTED =
            CSL_SIMCOP_DMA_DMACTRL_POSTED_WRITES_POSTED
    } CSL_SimcopDmaPostedWritesType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaMaxBurstSizeType describes the possible values written in MAX_BURST_SIZE field in DMA_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	1Cycle			: 	Single requests only 
*     2Cycles			: 	less or equal to 2
* 	4Cycle			: 	less or equal to 4  
*     8Cycles			: 	less or equal to 8    
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CTRL_MAX_BURST_SIZE_1CYCLE =
            CSL_SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE_1,
        CSL_DMA_CTRL_MAX_BURST_SIZE_2CYCLES =
            CSL_SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE_2,
        CSL_DMA_CTRL_MAX_BURST_SIZE_4CYCLES =
            CSL_SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE_4,
        CSL_DMA_CTRL_MAX_BURST_SIZE_8CYCLES =
            CSL_SIMCOP_DMA_DMACTRL_MAX_BURST_SIZE_8
    } CSL_SimcopDmaMaxBurstSizeType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanHWStopType describes the possible values written in HWSTOP field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	Chan0			: 	Use Hw synchronization channel 0 
*     Chna1			: 	Use Hw synchronization channel 1
* 	Chan2			: 	Use Hw synchronization channel 2  
*     Chan3			: 	Use Hw synchronization channel 3   
*     Disabled			: 	Disabled   
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CHAN_CTRL_HWSTOP_DISABLED =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTOP_DISABLED,
        CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN0 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTOP_HW_SEQ_CH0,
        CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN1 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTOP_HW_SEQ_CH1,
        CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN2 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTOP_HW_SEQ_CH2,
        CSL_DMA_CHAN_CTRL_HWSTOP_HWCHAN3 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTOP_HW_SEQ_CH3
    } CSL_SimcopDmaChanHWStopType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanHWStartType describes the possible values written in HWSTART field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	Chan0			: 	Use Hw synchronization channel 0 
*     Chna1			: 	Use Hw synchronization channel 1
* 	Chan2			: 	Use Hw synchronization channel 2  
*     Chan3			: 	Use Hw synchronization channel 3  
*     Disabled			: 	Disabled 
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CHAN_CTRL_HWSTART_DISABLED =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTART_DISABLED,
        CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN0 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTART_HW_SEQ_CH0,
        CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN1 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTART_HW_SEQ_CH1,
        CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN2 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTART_HW_SEQ_CH2,
        CSL_DMA_CHAN_CTRL_HWSTART_HWCHAN3 =
            CSL_SIMCOP_DMA_CHANCTRL_HWSTART_HW_SEQ_CH3
    } CSL_SimcopDmaChanHWStartType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanLinkedType describes the possible values written in LINKED field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	Chan0			: 	Start channel 0 when this channel has completed transfer of one 2D block  
*     Chna1			: 	Start channel 1 when this channel has completed transfer of one 2D block 
* 	Chan2			: 	Start channel 2 when this channel has completed transfer of one 2D block   
*     Chan3			: 	Start channel 3 when this channel has completed transfer of one 2D block  
*     Chan4			: 	Start channel 4 when this channel has completed transfer of one 2D block  
*     Chan5			: 	Start channel 5 when this channel has completed transfer of one 2D block 
*     Chan6			: 	Start channel 6 when this channel has completed transfer of one 2D block 
*     Chan7			: 	Start channel 7 when this channel has completed transfer of one 2D block 
*     Disabled			: 	Disabled 
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CHAN_CTRL_LINKED_DISABLED =
            CSL_SIMCOP_DMA_CHANCTRL_LINKED_DISABLED,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN0 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN0,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN1 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN1,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN2 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN2,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN3 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN3,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN4 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN4,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN5 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN5,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN6 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN6,
        CSL_DMA_CHAN_CTRL_LINKED_CHAN7 = CSL_SIMCOP_DMA_CHANCTRL_LINKED_CHAN7
    } CSL_SimcopDmaChanLinkedType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanGridType describes the possible values written in GRID field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	4BY4_X_ADDR_13TO0_Y_ADDR_26TO14			: 	Sub-tile : 4x4 bytes;Tile: 32x32 bytes;X=ADDR[13:0], Y=ADDR[26:14]  
*     4BY4_X_ADDR_12TO0_Y_ADDR_26TO13			: 	Sub-tile : 4x4 bytes;Tile: 32x32 bytes;X=ADDR[12:0], Y=ADDR[26:13] 
* 	8BY2_X_ADDR_14TO0_Y_ADDR_26TO15			: 	Sub-tile : 8x2 bytes;Tile: 64x16 bytes;X=ADDR[14:0], Y=ADDR[26:15]   
*     8BY2_X_ADDR_13TO0_Y_ADDR_26TO14			: 	Sub-tile : 8x2 bytes;Tile: 64x16 bytes;X=ADDR[13:0], Y=ADDR[26:14]   
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_GRID_4BY4_X_ADDR_13TO0_Y_ADDR_26TO14 =
            CSL_SIMCOP_DMA_CHANCTRL_GRID_4BY4_X_ADDR_13TO0_Y_ADDR_26TO14,
        CSL_DMA_CHAN_CTRL_GRID_4BY4_X_ADDR_12TO0_Y_ADDR_26TO13 =
            CSL_SIMCOP_DMA_CHANCTRL_GRID_4BY4_X_ADDR_12TO0_Y_ADDR_26TO13,
        CSL_DMA_CHAN_CTRL_GRID_8BY2_X_ADDR_14TO0_Y_ADDR_26TO15 =
            CSL_SIMCOP_DMA_CHANCTRL_GRID_8BY2_X_ADDR_14TO0_Y_ADDR_26TO15,
        CSL_DMA_CHAN_CTRL_GRID_8BY2_X_ADDR_13TO0_Y_ADDR_26TO14 =
            CSL_SIMCOP_DMA_CHANCTRL_GRID_8BY2_X_ADDR_13TO0_Y_ADDR_26TO14
    } CSL_SimcopDmaChanGridType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanTilerModeType describes the possible values written in TILERMODE field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	Regular			: 	Regular mode.INCR burst are used.ADDR[32]=0 for OCP transactions   
*     TILER			: 	Tiler mode.BLCK burst are used.ADDR[32]=1 for OCP transactions  
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_TILERMODE_REGULAR =
            CSL_SIMCOP_DMA_CHANCTRL_TILERMODE_REGULAR_MODE,
        CSL_DMA_CHAN_CTRL_TILERMODE_TILER =
            CSL_SIMCOP_DMA_CHANCTRL_TILERMODE_TILER_MODE
    } CSL_SimcopDmaChanTilerModeType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanDirType describes the possible values written in DIR field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	SysToIBuf		: 	System memory -> SIMCOP buffers   
*     IBufToSys		: 	SIMCOP buffers -> system memory   
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CHAN_CTRL_DIR_SYSTOIBUF =
            CSL_SIMCOP_DMA_CHANCTRL_DIR_SYSMEM_TO_SIMCOPBUFS,
        CSL_DMA_CHAN_CTRL_DIR_IBUFTOSYS =
            CSL_SIMCOP_DMA_CHANCTRL_DIR_SIMCOPBUFS_TO_SYSMEM
    } CSL_SimcopDmaChanDirType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanStatusType describes the possible values read from STATUS field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	Idle				: 	Idle		  
*     Active			: 	Active	 
* 	Pending			: 	Pending   
*     	Running			: 	Running   
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_STATUS_IDLE = CSL_SIMCOP_DMA_CHANCTRL_STATUS_IDLE,
        CSL_DMA_CHAN_CTRL_STATUS_ACTIVE = CSL_SIMCOP_DMA_CHANCTRL_STATUS_ACTIVE,
        CSL_DMA_CHAN_CTRL_STATUS_PENDING =
            CSL_SIMCOP_DMA_CHANCTRL_STATUS_PENDING,
        CSL_DMA_CHAN_CTRL_STATUS_RUNNING =
            CSL_SIMCOP_DMA_CHANCTRL_STATUS_RUNNING
    } CSL_SimcopDmaChanStatusType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanSWTriggerType describes the possible values written in SWTRIGGER field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	NoEffect					: 	System memory -> SIMCOP buffers  
*     CHGETOPNDINGIFACT		: 	Change the logical channel state to PENDING if it is in ACTIVE state.No effect if the channel is in RUNNING, 
*								PENDING or IDLE state    
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_SWTRIGGER_NOEFFECT =
            CSL_SIMCOP_DMA_CHANCTRL_SWTRIGGER_NO_EFFECT,
        CSL_DMA_CHAN_CTRL_SWTRIGGER_CHGETOPNDINGIFACT =
            CSL_SIMCOP_DMA_CHANCTRL_SWTRIGGER_CHANGE_TO_PENDING
    } CSL_SimcopDmaChanSWTriggerType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanDisableType describes the possible values written in DISABLE field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	NoEffect					: 	No Effect   
*     Disable					: 	Disable the channel.Changes the logical channel state to IDLE when it is in ACTIVE state.
*								Memorize a disable request when the channel is in RUNNING or PENDING state.  
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_DISABLE_NOEFFECT =
            CSL_SIMCOP_DMA_CHANCTRL_DISABLE_NOEFFECT,
        CSL_DMA_CHAN_CTRL_DISABLE_DISABLE =
            CSL_SIMCOP_DMA_CHANCTRL_DISABLE_DISABLE_CHANNEL
    } CSL_SimcopDmaChanDisableType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaChanEnableType describes the possible values written in ENABLE field in DMA_CHAN_CTRL register.  
*     The following tokens are directly mapped 
* 
* 	NoEffect					: 	No Effect   
*     Enable					: 	Enable the channel.Changes the state of the logical channel from IDLE to ACTIVE.    
*          
* This enumeration type is used by CSL_SimcopDmaHwSetup structure.
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_DMA_CHAN_CTRL_ENABLE_NOEFFECT =
            CSL_SIMCOP_DMA_CHANCTRL_ENABLE_NOEFFECT,
        CSL_DMA_CHAN_CTRL_ENABLE_ENABLE =
            CSL_SIMCOP_DMA_CHANCTRL_ENABLE_ENABLE_CHANNEL
    } CSL_SimcopDmaChanEnableType;

    /* 
     * Structures for SimcopDmaHwStatus
     */
    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaSysConfig - Clock management configuration structure.
     *
     *
     * @param  StandByMode  :  Configuration of the local initiator state management mode.
     *                          By definition, initiator may generate read/write transaction as long as it is out of STANDBY state.
     * @param  SoftResetWrite   :  Software reset. 
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        CSL_SimcopDmaStandByModeType StandByMode;
        CSL_SimcopDmaSoftResetWriteType SoftResetWrite;
    } CSL_SimcopDmaSysConfig;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaCtrl - DMA control .
     *
     *
     * @param  BWLimiter        :  SIMCOP DMA guarantees that there are at least BW_LIMITER functional clock cycles between two OCP requests.
     No IDLE cycles are inserted during an OCP transaction.This parameter could be used to reduce traffic generated 
     by the SIMCOP DMA for non timing critical applications.Doing so leaves more BW for other system initiators.
     Default value corresponds to maximum performance.
     * @param  TagCnt       :  Limits the outstanding transactions count.Only tags 0 - TAG_CNT will be used by SIMCOP DMA
     The maximum allowed value is 2^(CONTEXT+2)-1
     * @param  PostedWrites     :  Select write type. Setting depend on the used interconnect 
     * @param  MaxBurstSize     :  Defines the maximum burst length for INCR bursts.
     In case of 2D bursts, length x height is less or equal to this value.
     
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Uint16 BWLimiter;
        Uint8 TagCnt;
        CSL_SimcopDmaPostedWritesType PostedWrites;
        CSL_SimcopDmaMaxBurstSizeType MaxBurstSize;
    } CSL_SimcopDmaCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaHwSetup - Simcop Dma HW Setup structure.
     *
     *
     * @param  IRQStatusRaw :  Per-event raw interrupt status vector.Raw status is set even if event is not enabled.
     *                         Write 1 to set the (raw) status, mostly for debug.
     * @param  IRQStatus        :  Per-event "enabled" interrupt status vector.Enabled status isn't set unless event is enabled.
     *                         Write 1 to clear the status after interrupt has been serviced (raw status gets cleared, i.e. even if not enabled).
     * @param  IRQEnableSet     :  Per-event interrupt enable bit vector.Write 1 to set (enable interrupt).
     *                         Readout equal to corresponding _CLR register.
     * @param  IRQEnableClr     :  Per-event interrupt enable bit vector.Write 1 to clear (disable interrupt).
     *                         Readout equal to corresponding SET register.
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Bool IRQStatusRaw;
        Bool IRQStatus;
        Bool IRQEnableSet;
        Bool IRQEnableClr;
    } CSL_SimcopDmaIRQRegType;
    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaIRQConfigType - Interrupt configuration structure.
     *
     *
     * @param  FrameDone    :  Channel has completed transfer of the full frame
     * @param  BlockDone    :  Channel has completed transfer of one 2D block 
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        CSL_SimcopDmaIRQRegType FrameDone;
        CSL_SimcopDmaIRQRegType BlockDone;
    } CSL_SimcopDmaIRQConfigType;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanBlkSize - 2D block size structure.
     *
     *
     * @param  YNUM :  Height, in lines, per 2D blockValid values are 1- 8191.
     * @param  XNUM     :  Width, in 128-bit words, per 2D block.Valid values are 1-1023, that corresponds to 16 bytes to 16kBytes.
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Uint16 YNUM;
        Uint16 XNUM;
    } CSL_SimcopDmaChanBlkSize;
    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanFrame - Defines a frame. A frame is composed of 2D blocks structure.
     *
     *
     * @param  YCNT :  Vertical count of 2D blocks per frame.Valid values are 1-1023
     * @param  XCNT     :  Horizontal count of 2D blocks per frame.Valid values are 1-1023 
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Uint16 YCNT;
        Uint16 XCNT;
    } CSL_SimcopDmaChanFrame;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanCurrBlk - SW could read the coordinates of the last transferred block structure.
     *
     *
     * @param  BY       :  Vertical position of the last transferred 2D block in the frame.
     * @param  BX   :  Horizontal position of the last transferred 2D block in the frame.
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Uint16 BY;
        Uint16 BX;
    } CSL_SimcopDmaChanCurrBlk;
    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanBlkStep - Offset between 2D blocks structure.
     *
     *
     * @param  YSTEP    :  Vertical offset, in lines, between rows of 2D blocks.For contiguous 2D blocks YSTEP=YNUM
     *                 Valid values are -8192 to +8191.
     * @param  XSTEP    :  Horizontal offset, in 128-bit words, between 2D block columns.For contiguous 2D blocks XSTEP=XNUM
     *                 Valid values are -1024 to +1023, that corresponds to 16 bytes to 16kBytes
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Int16 YSTEP;
        Int16 XSTEP;
    } CSL_SimcopDmaChanBlkStep;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanCtrlStructType - DMA Logical channel control structure.
     *
     *
     * @param  HWStop       :  DMA logical channel HW synchronization.Controls generation of the DONE pulse for the logical channel
     * @param  HWStart      :  DMA logical channel HW synchronization.Controls sensitivity of the logical channel on a START pulse
     * @param  Linked       :  DMA logical channel linking.
     * @param  Grid         :  Selects the grid to be used when TILERMODE=1
     * @param  TilerMode    :  Selects OCP transaction breakdown algorithm
     * @param  Dir          :  Transfer direction
     * @param  Status       :  SW could poll this bit to know the state of the channel
     * @param  SWTrigger    :  Software trigger of the DMA channel.Read of this register always returns 0.
     * @param  Disable      :  Disable control of the logical channel.Read of this register always returns 0. 
     * @param  Enable   :  Enable control of the logical channel.Read of this register always returns 0.
     * 
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_SimcopDmaChanHWStopType HWStop;
        CSL_SimcopDmaChanHWStartType HWStart;
        CSL_SimcopDmaChanLinkedType Linked;
        CSL_SimcopDmaChanGridType Grid;
        CSL_SimcopDmaChanTilerModeType TilerMode;
        CSL_SimcopDmaChanDirType Dir;
        CSL_SimcopDmaChanStatusType Status;
        CSL_SimcopDmaChanSWTriggerType SWTrigger;
        CSL_SimcopDmaChanDisableType Disable;
        CSL_SimcopDmaChanEnableType Enable;
    } CSL_SimcopDmaChanCtrlStructType;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaChanCtrl - DMA Logical channel configuration.
     *
     *
     * @param  SetupChan        :  Flag to enable a DMA channel
     * @param  IRQConfig        :  Interrupt configuration
     * @param  ChanCtrl         :  DMA channel control 
     * @param  SMemAddr         :  System memory address
     * @param  SMemOfst         :  System memory line offset in 128-bit words.Maximum stride = 1Mbyte
     * @param  BufAddr          :  SIMCOP memory address
     * @param  BufOfst          :  SIMCOP memory line offset
     * @param  ChanBlkSize      :  2D block size
     * @param  Frame            :  Defines a frame. A frame is composed of 2D blocks
     * @param  ChanCurrBlk      :  SW could read the coordinates of the last transferred block.
     *                         The status is reset when the channel is enabled (change the state of CTRL.ENABLE from 0 to 1).
     * @param  ChanBlkStep      :  Offset between 2D blocks.
     * 
     */
    /* ========================================================================== 
     */

    typedef struct {
        Bool SetupChan;
        CSL_SimcopDmaIRQConfigType IRQConfig;
        CSL_SimcopDmaChanCtrlStructType ChanCtrl;
        Uint32 SMemAddr;
        Uint32 SMemOfst;
        Uint32 BufAddr;
        Uint32 BufOfst;
        CSL_SimcopDmaChanBlkSize ChanBlkSize;
        CSL_SimcopDmaChanFrame Frame;
        CSL_SimcopDmaChanCurrBlk ChanCurrBlk;
        CSL_SimcopDmaChanBlkStep ChanBlkStep;
    } CSL_SimcopDmaChanCtrl;

    /* ========================================================================== 
     */
    /* 
     * CSL_SimcopDmaHwSetup - Simcop Dma HW Setup structure.
     *
     *
     * @param  DmaSysConfig :  Clock management configuration
     * @param  EOILineNum   :  End Of Interrupt number specification
     * @param  DmaCtrl          :  DMA control 
     * @param  DmaChannelCtrl  :  DMA Logical channel configuration 
     * @param  OCP_ERROR    :  OCP error
     * 
     */
    /* ========================================================================== 
     */
    typedef struct {
        CSL_SimcopDmaSysConfig DmaSysConfig;
        CSL_SimcopDmaIRQEOIType EOILineNum;
        CSL_SimcopDmaCtrl DmaCtrl;
        CSL_SimcopDmaChanCtrl DmaChannelCtrl[8];
        Uint8 OCP_ERROR;
    } CSL_SimcopDmaHwSetup;
#if 0
    /* ========================================================================== 
     */
/** CSL_SimcopDmaHwCtrlCmdType  describes the possible commands issued to write values to Simcop DMA registers.
* This enumeration type is used by CSL_SimcopDmaHwControl API
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_CMD_SET_CHAN3_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQSTATUS_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQENABLESET_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN3_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN3_HWSTOP,
        CSL_DMA_CMD_SET_CHAN3_HWSTART,
        CSL_DMA_CMD_SET_CHAN3_LINKED,
        CSL_DMA_CMD_SET_CHAN3_GRID,
        CSL_DMA_CMD_SET_CHAN3_TILERMODE,
        CSL_DMA_CMD_SET_CHAN3_DIR,
        CSL_DMA_CMD_SET_CHAN3_SWTRIGGER,
        CSL_DMA_CMD_SET_CHAN3_DISABLE,
        CSL_DMA_CMD_SET_CHAN3_ENABLE,
        CSL_DMA_CMD_SET_CHAN3_SMEM_ADDR,
        CSL_DMA_CMD_SET_CHAN3_SMEM_OFST,
        CSL_DMA_CMD_SET_CHAN3_BUF_ADDR,
        CSL_DMA_CMD_SET_CHAN3_BUF_OFST,
        CSL_DMA_CMD_SET_CHAN3_CHANBLKSIZE_YNUM,
        CSL_DMA_CMD_SET_CHAN3_CHANBLKSIZE_XNUM,
        CSL_DMA_CMD_SET_CHAN3_CHANFRAME_YCNT,
        CSL_DMA_CMD_SET_CHAN3_CHANFRAME_XCNT,
        CSL_DMA_CMD_SET_CHAN3_CHANBLKSTEP_YSTEP,
        CSL_DMA_CMD_SET_CHAN3_CHANBLKSTEP_XSTEP,

        CSL_DMA_CMD_SET_CHAN2_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQSTATUS_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQENABLESET_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN2_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN2_HWSTOP,
        CSL_DMA_CMD_SET_CHAN2_HWSTART,
        CSL_DMA_CMD_SET_CHAN2_LINKED,
        CSL_DMA_CMD_SET_CHAN2_GRID,
        CSL_DMA_CMD_SET_CHAN2_TILERMODE,
        CSL_DMA_CMD_SET_CHAN2_DIR,
        CSL_DMA_CMD_SET_CHAN2_SWTRIGGER,
        CSL_DMA_CMD_SET_CHAN2_DISABLE,
        CSL_DMA_CMD_SET_CHAN2_ENABLE,
        CSL_DMA_CMD_SET_CHAN2_SMEM_ADDR,
        CSL_DMA_CMD_SET_CHAN2_SMEM_OFST,
        CSL_DMA_CMD_SET_CHAN2_BUF_ADDR,
        CSL_DMA_CMD_SET_CHAN2_BUF_OFST,
        CSL_DMA_CMD_SET_CHAN2_CHANBLKSIZE_YNUM,
        CSL_DMA_CMD_SET_CHAN2_CHANBLKSIZE_XNUM,
        CSL_DMA_CMD_SET_CHAN2_CHANFRAME_YCNT,
        CSL_DMA_CMD_SET_CHAN2_CHANFRAME_XCNT,
        CSL_DMA_CMD_SET_CHAN2_CHANCURRBLK_BY,
        CSL_DMA_CMD_SET_CHAN2_CHANCURRBLK_BX,
        CSL_DMA_CMD_SET_CHAN2_CHANBLKSTEP_YSTEP,
        CSL_DMA_CMD_SET_CHAN2_CHANBLKSTEP_XSTEP,

        CSL_DMA_CMD_SET_CHAN1_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQSTATUS_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQENABLESET_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN1_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN1_HWSTOP,
        CSL_DMA_CMD_SET_CHAN1_HWSTART,
        CSL_DMA_CMD_SET_CHAN1_LINKED,
        CSL_DMA_CMD_SET_CHAN1_GRID,
        CSL_DMA_CMD_SET_CHAN1_TILERMODE,
        CSL_DMA_CMD_SET_CHAN1_DIR,
        CSL_DMA_CMD_SET_CHAN1_SWTRIGGER,
        CSL_DMA_CMD_SET_CHAN1_DISABLE,
        CSL_DMA_CMD_SET_CHAN1_ENABLE,
        CSL_DMA_CMD_SET_CHAN1_SMEM_ADDR,
        CSL_DMA_CMD_SET_CHAN1_SMEM_OFST,
        CSL_DMA_CMD_SET_CHAN1_BUF_ADDR,
        CSL_DMA_CMD_SET_CHAN1_BUF_OFST,
        CSL_DMA_CMD_SET_CHAN1_CHANBLKSIZE_YNUM,
        CSL_DMA_CMD_SET_CHAN1_CHANBLKSIZE_XNUM,
        CSL_DMA_CMD_SET_CHAN1_CHANFRAME_YCNT,
        CSL_DMA_CMD_SET_CHAN1_CHANFRAME_XCNT,
        CSL_DMA_CMD_SET_CHAN1_CHANCURRBLK_BY,
        CSL_DMA_CMD_SET_CHAN1_CHANCURRBLK_BX,
        CSL_DMA_CMD_SET_CHAN1_CHANBLKSTEP_YSTEP,
        CSL_DMA_CMD_SET_CHAN1_CHANBLKSTEP_XSTEP,

        CSL_DMA_CMD_SET_CHAN0_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQSTATUS_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQENABLESET_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_CMD_SET_CHAN0_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_CMD_SET_CHAN0_HWSTOP,
        CSL_DMA_CMD_SET_CHAN0_HWSTART,
        CSL_DMA_CMD_SET_CHAN0_LINKED,
        CSL_DMA_CMD_SET_CHAN0_GRID,
        CSL_DMA_CMD_SET_CHAN0_TILERMODE,
        CSL_DMA_CMD_SET_CHAN0_DIR,
        CSL_DMA_CMD_SET_CHAN0_SWTRIGGER,
        CSL_DMA_CMD_SET_CHAN0_DISABLE,
        CSL_DMA_CMD_SET_CHAN0_ENABLE,
        CSL_DMA_CMD_SET_CHAN0_SMEM_ADDR,
        CSL_DMA_CMD_SET_CHAN0_SMEM_OFST,
        CSL_DMA_CMD_SET_CHAN0_BUF_ADDR,
        CSL_DMA_CMD_SET_CHAN0_BUF_OFST,
        CSL_DMA_CMD_SET_CHAN0_CHANBLKSIZE_YNUM,
        CSL_DMA_CMD_SET_CHAN0_CHANBLKSIZE_XNUM,
        CSL_DMA_CMD_SET_CHAN0_CHANFRAME_YCNT,
        CSL_DMA_CMD_SET_CHAN0_CHANFRAME_XCNT,
        CSL_DMA_CMD_SET_CHAN0_CHANCURRBLK_BY,
        CSL_DMA_CMD_SET_CHAN0_CHANCURRBLK_BX,
        CSL_DMA_CMD_SET_CHAN0_CHANBLKSTEP_YSTEP,
        CSL_DMA_CMD_SET_CHAN0_CHANBLKSTEP_XSTEP,

        CSL_DMA_CMD_SET_OCP_IRQSTATUSRAW_ERR,
        CSL_DMA_CMD_SET_OCP_IRQSTATUS_ERR,
        CSL_DMA_CMD_SET_OCP_IRQENABLESET_ERR,
        CSL_DMA_CMD_SET_OCP_IRQENABLECLR_ERR,

        CSL_DMA_CMD_SET_SYSCONFIG_STANDBYMODE,
        CSL_DMA_CMD_SET_SYSCONFIG_SOFTRESET,

        CSL_DMA_CMD_SET_IRQEOI_LINE_NUMBER,

        CSL_DMA_CMD_SET_BW_LIMITER,
        CSL_DMA_CMD_SET_TAG_CNT,
        CSL_DMA_CMD_SET_POSTED_WRITES,
        CSL_DMA_CMD_SET_MAX_BURST_SIZE
    } CSL_SimcopDmaHwCtrlCmdType;

    /* ========================================================================== 
     */
/** CSL_SimcopDmaHWQueryType  describes the possible commands issued to read values to Simcop DMA registers.
* This enumeration type is used by CSL_SimcopDmaGetHWStatus API
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_DMA_QUERY_CHAN3_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_QUERY_CHAN3_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN3_IRQSTATUS_FRAME_DONE,
        CSL_DMA_QUERY_CHAN3_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN3_IRQENABLESET_FRAME_DONE,
        CSL_DMA_QUERY_CHAN3_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN3_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_QUERY_CHAN3_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN3_HWSTOP,
        CSL_DMA_QUERY_CHAN3_HWSTART,
        CSL_DMA_QUERY_CHAN3_LINKED,
        CSL_DMA_QUERY_CHAN3_GRID,
        CSL_DMA_QUERY_CHAN3_TILERMODE,
        CSL_DMA_QUERY_CHAN3_DIR,
        CSL_DMA_QUERY_CHAN3_STATUS,
        CSL_DMA_QUERY_CHAN3_SMEM_ADDR,
        CSL_DMA_QUERY_CHAN3_SMEM_OFST,
        CSL_DMA_QUERY_CHAN3_BUF_ADDR,
        CSL_DMA_QUERY_CHAN3_BUF_OFST,
        CSL_DMA_QUERY_CHAN3_CHANBLKSIZE_YNUM,
        CSL_DMA_QUERY_CHAN3_CHANBLKSIZE_XNUM,
        CSL_DMA_QUERY_CHAN3_CHANFRAME_YCNT,
        CSL_DMA_QUERY_CHAN3_CHANFRAME_XCNT,
        CSL_DMA_QUERY_CHAN3_CHANCURRBLK_BY,
        CSL_DMA_QUERY_CHAN3_CHANCURRBLK_BX,
        CSL_DMA_QUERY_CHAN3_CHANBLKSTEP_YSTEP,
        CSL_DMA_QUERY_CHAN3_CHANBLKSTEP_XSTEP,

        CSL_DMA_QUERY_CHAN2_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_QUERY_CHAN2_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN2_IRQSTATUS_FRAME_DONE,
        CSL_DMA_QUERY_CHAN2_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN2_IRQENABLESET_FRAME_DONE,
        CSL_DMA_QUERY_CHAN2_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN2_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_QUERY_CHAN2_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN2_HWSTOP,
        CSL_DMA_QUERY_CHAN2_HWSTART,
        CSL_DMA_QUERY_CHAN2_LINKED,
        CSL_DMA_QUERY_CHAN2_GRID,
        CSL_DMA_QUERY_CHAN2_TILERMODE,
        CSL_DMA_QUERY_CHAN2_DIR,
        CSL_DMA_QUERY_CHAN2_STATUS,
        CSL_DMA_QUERY_CHAN2_SMEM_ADDR,
        CSL_DMA_QUERY_CHAN2_SMEM_OFST,
        CSL_DMA_QUERY_CHAN2_BUF_ADDR,
        CSL_DMA_QUERY_CHAN2_BUF_OFST,
        CSL_DMA_QUERY_CHAN2_CHANBLKSIZE_YNUM,
        CSL_DMA_QUERY_CHAN2_CHANBLKSIZE_XNUM,
        CSL_DMA_QUERY_CHAN2_CHANFRAME_YCNT,
        CSL_DMA_QUERY_CHAN2_CHANFRAME_XCNT,
        CSL_DMA_QUERY_CHAN2_CHANCURRBLK_BY,
        CSL_DMA_QUERY_CHAN2_CHANCURRBLK_BX,
        CSL_DMA_QUERY_CHAN2_CHANBLKSTEP_YSTEP,
        CSL_DMA_QUERY_CHAN2_CHANBLKSTEP_XSTEP,

        CSL_DMA_QUERY_CHAN1_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_QUERY_CHAN1_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN1_IRQSTATUS_FRAME_DONE,
        CSL_DMA_QUERY_CHAN1_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN1_IRQENABLESET_FRAME_DONE,
        CSL_DMA_QUERY_CHAN1_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN1_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_QUERY_CHAN1_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN1_HWSTOP,
        CSL_DMA_QUERY_CHAN1_HWSTART,
        CSL_DMA_QUERY_CHAN1_LINKED,
        CSL_DMA_QUERY_CHAN1_GRID,
        CSL_DMA_QUERY_CHAN1_TILERMODE,
        CSL_DMA_QUERY_CHAN1_DIR,
        CSL_DMA_QUERY_CHAN1_STATUS,
        CSL_DMA_QUERY_CHAN1_SMEM_ADDR,
        CSL_DMA_QUERY_CHAN1_SMEM_OFST,
        CSL_DMA_QUERY_CHAN1_BUF_ADDR,
        CSL_DMA_QUERY_CHAN1_BUF_OFST,
        CSL_DMA_QUERY_CHAN1_CHANBLKSIZE_YNUM,
        CSL_DMA_QUERY_CHAN1_CHANBLKSIZE_XNUM,
        CSL_DMA_QUERY_CHAN1_CHANFRAME_YCNT,
        CSL_DMA_QUERY_CHAN1_CHANFRAME_XCNT,
        CSL_DMA_QUERY_CHAN1_CHANCURRBLK_BY,
        CSL_DMA_QUERY_CHAN1_CHANCURRBLK_BX,
        CSL_DMA_QUERY_CHAN1_CHANBLKSTEP_YSTEP,
        CSL_DMA_QUERY_CHAN1_CHANBLKSTEP_XSTEP,

        CSL_DMA_QUERY_CHAN0_IRQSTATUSRAW_FRAME_DONE,
        CSL_DMA_QUERY_CHAN0_IRQSTATUSRAW_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN0_IRQSTATUS_FRAME_DONE,
        CSL_DMA_QUERY_CHAN0_IRQSTATUS_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN0_IRQENABLESET_FRAME_DONE,
        CSL_DMA_QUERY_CHAN0_IRQENABLESET_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN0_IRQENABLECLR_FRAME_DONE,
        CSL_DMA_QUERY_CHAN0_IRQENABLECLR_BLOCK_DONE,
        CSL_DMA_QUERY_CHAN0_HWSTOP,
        CSL_DMA_QUERY_CHAN0_HWSTART,
        CSL_DMA_QUERY_CHAN0_LINKED,
        CSL_DMA_QUERY_CHAN0_GRID,
        CSL_DMA_QUERY_CHAN0_TILERMODE,
        CSL_DMA_QUERY_CHAN0_DIR,
        CSL_DMA_QUERY_CHAN0_STATUS,
        CSL_DMA_QUERY_CHAN0_SMEM_ADDR,
        CSL_DMA_QUERY_CHAN0_SMEM_OFST,
        CSL_DMA_QUERY_CHAN0_BUF_ADDR,
        CSL_DMA_QUERY_CHAN0_BUF_OFST,
        CSL_DMA_QUERY_CHAN0_CHANBLKSIZE_YNUM,
        CSL_DMA_QUERY_CHAN0_CHANBLKSIZE_XNUM,
        CSL_DMA_QUERY_CHAN0_CHANFRAME_YCNT,
        CSL_DMA_QUERY_CHAN0_CHANFRAME_XCNT,
        CSL_DMA_QUERY_CHAN0_CHANCURRBLK_BY,
        CSL_DMA_QUERY_CHAN0_CHANCURRBLK_BX,
        CSL_DMA_QUERY_CHAN0_CHANBLKSTEP_YSTEP,
        CSL_DMA_QUERY_CHAN0_CHANBLKSTEP_XSTEP,

        CSL_DMA_QUERY_OCP_IRQSTATUSRAW_ERR,
        CSL_DMA_QUERY_OCP_IRQSTATUS_ERR,
        CSL_DMA_QUERY_OCP_IRQENABLESET_ERR,
        CSL_DMA_QUERY_OCP_IRQENABLECLR_ERR,

        CSL_DMA_QUERY_REVISION_SCHEME,
        CSL_DMA_QUERY_REVISION_FUNC,
        CSL_DMA_QUERY_REVISION_R_RTL,
        CSL_DMA_QUERY_REVISION_X_MAJOR,
        CSL_DMA_QUERY_REVISION_CUSTOM,
        CSL_DMA_QUERY_REVISION_Y_MINOR,

        CSL_DMA_QUERY_HWINFO_CHAN,
        CSL_DMA_QUERY_HWINFO_CONTEXT,

        CSL_DMA_QUERY_SYSCONFIG_STANDBYMODE,
        CSL_DMA_QUERY_SYSCONFIG_SOFTRESET,

        CSL_DMA_QUERY_IRQEOI_LINE_NUMBER,

        CSL_DMA_QUERY_BW_LIMITER,
        CSL_DMA_QUERY_TAG_CNT,
        CSL_DMA_QUERY_POSTED_WRITES,
        CSL_DMA_QUERY_MAX_BURST_SIZE
    } CSL_SimcopDmaHWQueryType;
#endif
/*--------function prototypes ---------------------------------*/
    CSL_Status CSL_simcopDmaInit(CSL_SimcopDmaHandle hndl);
    CSL_Status CSL_simcopDmaOpen(CSL_SimcopDmaObj * hSimcopDmaObj,
                                 CSL_SimcopDmaNum simcopDmaNum,
                                 CSL_OpenMode openMode);
    CSL_Status CSL_simcopDmaClose(CSL_SimcopDmaHandle hndl);
    CSL_Status CSL_simcopDmaHwSetup(CSL_SimcopDmaHandle hndl,
                                    CSL_SimcopDmaHwSetup * setup);
#if 0
    CSL_Status CSL_SimcopDmaHwControl(CSL_SimcopDmaHandle hndl,
                                      CSL_SimcopDmaHwCtrlCmdType cmd,
                                      void *data);
    CSL_Status CSL_SimcopDmaGetHWStatus(CSL_SimcopDmaHandle hndl,
                                        CSL_SimcopDmaHWQueryType query,
                                        void *data);
#endif

#ifdef __cplusplus
}
#endif
#endif/*_CSL_SIMCOP_DMA_H_*/
