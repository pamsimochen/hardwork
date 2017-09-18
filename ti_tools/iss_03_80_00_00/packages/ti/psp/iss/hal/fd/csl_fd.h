/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_fd_.h
*
* This File contains declarations for csl_fd_.c
* This entire description will appear as one
* paragraph in the generated documentation.
*
 * @path  $(DUCATIVOB)\drivers\csl\fd\
*
* @rev  00.01
*/
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 29-Nov -2009 
 * Vijay Badiger: Created the file. *! *! *! Revisions appear in reverse
 * chronological order; *! that is, newest first.  The date format is
 * dd-Mon-yyyy. *
 * =========================================================================== */

#ifndef CSL_FD_H
#define CSL_FD_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/*-------program files ----------------------------------------*/
#include "csl_fd.h"
#include "cslr__fd.h"
#include "../iss/hal/iss/simcop/common/simcop.h"
    /****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

    /* ========================================================================== 
     */
/**
* CSL_FdObj - Fd Object structure
* @param  openMode  This is the mode which the CSL instance is opened
* @param  uid  This is a unique identifier to the instance of FD being referred to by this object
* @param  xio  This is the variable that contains the current state of a resource being shared by current instance of
                       FD with other peripherals
* @param  regs  This is a pointer to the registers of the instance of FD referred to by this object
* @param  perNum  This is the instance of FD being referred to by this object
*/
    /* ========================================================================== 
     */
    typedef volatile CSL_FdRegs *CSL_FdRegsOvly;

    typedef enum {
        CSL_FD_ANY = -1,
                      /**< <b>: Any instance of FD module</b> */
        CSL_FD_0 = 0  /**< <b>: FDInstance 0</b> */
    } CSL_FdNum;

    typedef struct {
        CSL_OpenMode openMode;
        CSL_Uid uid;
        CSL_Xio xio;
        CSL_FdRegsOvly regs;
        CSL_FdNum perNum;
    } CSL_FdObj;

#define CSL_FD_0_XIO 0                                     // hardcoded
#define CSL_FD_0_UID 0                                     // hardcoded

#define CSL_FD_BASE_ADDRESS (0x481D6000)

#define CSL_FD_REGS             (CSL_FdRegsOvly) (CSL_FD_BASE_ADDRESS)
    // extern BOOL fdDoneIrqOccurred;
    /* ========================================================================== 
     */
/**
* CSL_FdHandle - Pointer to CSL_FdObj
*/
    /* ========================================================================== 
     */
    typedef CSL_FdObj *CSL_FdHandle;

    /* 
     * Enums for Field value Tokens
     */

    /* ========================================================================== 
     */
/**
*CSL_FdModeType enumeration for selecting Face Direction
*
* @param CSL_FD_MODE_DIR_UP          Direction Up
* @param CSL_FD_MODE_DIR_RIGHT       Direction Right
* @param CSL_FD_MODE_DIR_LEFT        Direction Left
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_FD_MODE_DIR_UP = CSL_FD_DIR_MODE_UP,
        CSL_FD_MODE_DIR_RIGHT = CSL_FD_DIR_MODE_RIGHT,
        CSL_FD_MODE_DIR_LEFT = CSL_FD_DIR_MODE_LEFT
    } CSL_FdDirectionType;

/********************************************************************************
*
* CSL_HwEnableType enumeration for setting Autogating on or off
*
* @param CSL_FD_HW_DISABLE    Disable Hardware
* @param CSL_FD_HW_ENABLE     Enable Hardware
*
*********************************************************************************/
    typedef enum {
        CSL_FD_HW_INT_DISABLE = 0,
        CSL_FD_HW_INT_ENABLE = 1
    } CSL_FDHwIntEnableType;

/********************************************************************************
*
* CSL_HwEnableType enumeration for setting Autogating on or off
*
* @param CSL_FD_HW_DISABLE    Disable Hardware
* @param CSL_FD_HW_ENABLE     Enable Hardware
*
*********************************************************************************/
    typedef enum {
        CSL_FD_HW_FD_RUN_DISABLE = 0,
        CSL_FD_HW_FD_RUN_ENABLE = 1
    } CSL_FDHwFDEnableType;

/********************************************************************************
*
* CSL_HwEnableType enumeration for setting Autogating on or off
*
* @param CSL_FD_HW_DISABLE    Disable Hardware
* @param CSL_FD_HW_ENABLE     Enable Hardware
*
*********************************************************************************/
    typedef enum {
        CSL_FD_HW_SRST_DISABLE = 0,
        CSL_FD_HW_SRST_ENABLE = 1
    } CSL_FDHwSRSTEnableType;

    /* ========================================================================== 
     */
/** CSL_FdMinFaceSizeType enumeration for selecting Minimun face size
*
* @param CSL_FD_MODE_MIN_SIZE_20         Min face Size 20 x 20
* @param CSL_FD_MODE_MIN_SIZE_25       Min face size 25 x 25
* @param CSL_FD_MODE_MIN_SIZE_32     Min face size 32 x32
* @param CSL_FD_MODE_MIN_SIZE_40      Min face size 40 x40
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_FD_MODE_MIN_SIZE_20 = CSL_FD_MIN_SIZE_20,
        CSL_FD_MODE_MIN_SIZE_25 = CSL_FD_MIN_SIZE_25,
        CSL_FD_MODE_MIN_SIZE_32 = CSL_FD_MIN_SIZE_32,
        CSL_FD_MODE_MIN_SIZE_40 = CSL_FD_MIN_SIZE_40
    } CSL_FdMinFaceSizeType;

    typedef CSL_HwEnableType CSL_FdHwEnableType;
    typedef CSL_HwIntEnableType CSL_FdIntEnableType;

    /* ========================================================================== 
     */
/** Fd Hardware Control and Query Structures
* @param data_format enumeration of CSL_FdModeType
   @param bayer_format  enumeration of  CSL_FdBayerModeType
   @param frame_sizeh window width, in pixels, multiple of obh
   @param frame_sizew  compute window height, in pixels, multiple of obw
   @param ld_obh output block height, in pixels, for block processing
   @param ld_obw output block height, in pixels, for block processing
** ========================================================================== */
    typedef struct {

        Uint32 ResultX;
        Uint32 ResultY;
        Uint32 ResultConfidenceLevel;
        Uint32 ResultSize;
        Uint32 ResultAngle;

    } CSL_FdPosition;

    /* 
     * Fd Hardware Control and Query Enums
     */
    /* ========================================================================== 
     */
/** CSL_SimcopHwCtrlCmdType  describes the possible commands issued to write values to FD registers.
*
* This enumeration type is used by CSL_FdHwControl API
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_FD_CMD_INTENABLE,
        CSL_FD_CMD_SETPICADDR,
        CSL_FD_CMD_SETWKADDR,
        CSL_FD_CMD_SETCTRL,
        CSL_FD_CMD_SETDCOND_DIR,
        CSL_FD_CMD_SETDCOND_MINFACESIZE,
        CSL_FD_CMD_SETSTARTX,
        CSL_FD_CMD_SETSTARTY,
        CSL_FD_CMD_SETSIZEX,
        CSL_FD_CMD_SETSIZEY,
        CSL_FD_CMD_SETLHIT
    } CSL_FdHwCtrlCmdType;

    /* ========================================================================== 
     */
/** CSL_FdHwQueryType  describes the possible commands issued to read values from FD registers.
* This enumeration type is used by CSL_FdGetHwStatus API
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_FD_QUERY_PERIPHERAL_ID,
        CSL_FD_QUERY_INTENABLE,
        CSL_FD_QUERY_PICADDR,
        CSL_FD_QUERY_WKADDR,
        CSL_FD_QUERY_CTRL,
        CSL_FD_QUERY_DNUM,
        CSL_FD_QUERY_DCOND_DIR,
        CSL_FD_QUERY_DCOND_MINFACESIZE,
        CSL_FD_QUERY_STARTX,
        CSL_FD_QUERY_STARTY,
        CSL_FD_QUERY_SIZEX,
        CSL_FD_QUERY_SIZEY,
        CSL_FD_QUERY_LHIT,
        CSL_FD_QUERY_FACERESULT,
        CSL_FD_QUERY_IRQ_STATUS
    } CSL_FdHwQueryType;

/*========================================================================== */
/**
* CSL_FdHwSetupCtrl - FD Hardware Setup structure
*/
/*========================================================================== */
    typedef struct {
        Uint32 InterruptEnable;                            /* Interrupt
                                                            * enable */
        Uint32 Soft_Reset_bit;                             /* Soft reset bit */
        Uint32 Fd_Run;                                     /* Face detection
                                                            * run bit */
        Uint32 PictureAddress;                             /* Picture area
                                                            * address */
        Uint32 WorkAreaAddress;                            /* Work area
                                                            * address */
        Uint32 ControlReg;                                 /* control reg */
        Uint32 NumberOfFaceDetected;                       /* Number of faces 
                                                            * detected */
        Uint32 Face_Direction;                             /* Direction of
                                                            * face */
        Uint32 ImageStartX;                                /* ROI
                                                            * configuration
                                                            * parameter */
        Uint32 ImageStartY;                                /* ROI
                                                            * configuration
                                                            * parameter */
        Uint32 SizeX;                                      /* ROI
                                                            * configuration
                                                            * parameter */
        Uint32 SizeY;                                      /* ROI
                                                            * configuration
                                                            * parameter */
        Uint32 ThresholdValue;                             /* Threshold value 
                                                            * for face
                                                            * detection */
        Uint32 MinfaceSize;                                /* Min face size
                                                            * parameter */
        Uint32 IrqStatusClrEvent;                          /* Status clear
                                                            * after FD
                                                            * completion */
        Uint32 EOIEnable;                                  /* End of
                                                            * Interrupt for
                                                            * FDIF_IRQ1 */

    } CSL_FdHwSetupCtrl;

/*****************************************************/
/*--------function prototypes ---------------------------------*/

    CSL_Status CSL_fdOpen(CSL_FdObj * hFdObj, CSL_FdNum fd_Num,
                          CSL_OpenMode openMode);
    CSL_Status CSL_fdHwSetup(CSL_FdHandle hndl, CSL_FdHwSetupCtrl * setup);
    CSL_Status CSL_fdHwControl(CSL_FdHandle hndl, CSL_FdHwCtrlCmdType * cmd,
                               void *data);
    CSL_Status CSL_fdGetHwStatus(CSL_FdHandle hndl, CSL_FdHwQueryType * query,
                                 void *data);
    CSL_Status CSL_fdClose(CSL_FdHandle hndl);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* CSL_FD_H */
