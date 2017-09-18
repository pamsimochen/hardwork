/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Csl_imx4.h
*
* This file contains register level structure definition for IMX module in SIMCOP
*
* @path ???
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 25-Sep 2008 Phanish: Initial Version! 
*========================================================================= */
/* ========================================================================== 
 */
/**
* This is a structure description for iMX Object.
*
* ========================================================================== */
#ifndef _CSL_IMX4_H_
#define _CSL_IMX4_H_

#ifdef __cplusplus
extern "C" {
#endif

    // #include <cslr.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>

/*****************************************************************************/
    /* DBC macros for IMX */
 /*****************************************************************************/
#define CSL_IMX_ASSERT CSL_EXIT_IF

#if 0
#define CSL_IMX_REQUIRE(C,V) { \
    if (!(C)) { \
        status = V; \
#if 0
    SIMCOP_PRINT
        ("Error ::%s:: %s : %s : %d :: Exiting because : %s ::requirement is not met\n",
         \
#V,__FILE__, __FUNCTION__, __LINE__, #C); \
#endif
          goto EXIT;}}
/* Mainly check for handle corruption */
#define CSL_IMX_ENSURE(C,V) { \
    if (!(C)) { \
        status = V; \
        #if 0
         SIMCOP_PRINT
         ("Error ::%s:: %s : %s : %d :: Exiting because : %s :: is not ensured\n",
          \
#V,__FILE__, __FUNCTION__, __LINE__, #C); \
#endif
          goto EXIT;}
          }

#endif

          typedef struct {

    /** This is the mode which the CSL IMX instance is opened     */
          CSL_OpenMode eOpenMode;
    /** This is a unique identifier to the instance of IMX being
    *  referred to by this object
     */
          CSL_Uid uid;
    /** This is the variable that contains the current state of a
    *  resource being shared by current instance of IMX with
    *  other peripherals
    */
          CSL_Xio xio;
    /** This is a pointer to the registers of the instance of IMX
    *  referred to by this object
    */
          CSL_ImxRegsOvly pRegs;
    /** This is the instance of IMX being referred to by this object  */
          CSL_ImxInstNum eInstanceNum;} CSL_ImxObj; typedef enum {

          CSL_IMX_MULTIPLY = 0x0,
          CSL_IMX_ABSOLUTEDIFF = 0x1,
          CSL_IMX_ADD = 0x2,
          CSL_IMX_SUBTRACT = 0x3,
          CSL_IMX_TLU = 0x4,
          CSL_IMX_MIN = 0x5,
          CSL_IMX_MAX = 0x6,
          CSL_IMX_AND = 0x8005,
          CSL_IMX_OR = 0x8006,                             /* These are basic 
                                                            * operations.
                                                            * Other
                                                            * Operations
                                                            * could be
                                                            * included */
          CSL_IMX_NOT = 0x8007
          } CSL_ImxOperation; typedef struct {

          Uint16 * pInput;                                 /* starting
                                                            * address of
                                                            * input */
          Uint16 * pCoeff;                                 /* starting
                                                            * address of
                                                            * coefficients */
          Uint16 * pOutput;                                /* starting
                                                            * address of
                                                            * output */
          Int16 usInputWidth;                              /* width/columns
                                                            * of input */
          Int16 usInputHeight;                             /* height/rows of
                                                            * input */
          Int16 usCoeffWidth;                              /* width/columns
                                                            * of coefficients 
                                                            */
          Int16 usCoeffHeight;                             /* height/rows of
                                                            * coefficients */
          Int16 usOutputWidth;                             /* width/columns
                                                            * of output */
          Int16 usOutputHeight;                            /* height/rows of
                                                            * output */
          Int16 usComputeWidth;                            /* computation
                                                            * dimension/number 
                                                            * of columns */
          Int16 usComputeHeight;                           /* computation
                                                            * dimension/number 
                                                            * of rows */
          // Int16 dnsmpl_horz; /* horizontal downsampling factor */
          // Int16 dnsmpl_vert; /* verical downsampling factor */
          Int16 ssInputType;                               /* short/byte,
                                                            * signed/unsigned 
                                                            */
          Int16 ssCoeffType;                               /* short/byte,
                                                            * signed/unsigned 
                                                            */
          Int16 ssOutputType;                              /* short/byte */
          Int16 ssRoundShift;                              /* shifting
                                                            * parameter */
          CSL_ImxOperation eImxOp;                         /* Operation to be 
                                                            * performed */
          Uint16 * pCmdMem;} CSL_ImxSetup;

          /* Handle to IMX */
          typedef CSL_ImxObj * CSL_ImxHandle; typedef struct {
          Uint16 * pCmdMemAddr; Uint16 * pCmdBuf; Uint16 usLength;  /* Length 
                                                                     * in
                                                                     * bytes */
          } CSL_ImxCmdMemType; typedef CSL_HwStatusType CSL_ImxHwStatusType;

          /* ======================================================================= 
           */
          /* CSL_ImxHwCtrlCmd enumeration for selecting type of operation * * 
           * @param CSL_IMX_CMD_START Start the IMX used * @param
           * CSL_IMX_CMD_STADDR Starting address in CMD MEM * @param
           * CSL_IMX_CMD_BKADDR Command to Break at a given Address * @param
           * CSL_IMX_CMD_BKENABLE Enable/Disable Break functionality * @param 
           * CSL_IMX_CMD_SETAUTOGATING Set Autogating on or off *
           * *========================================================================== 
           */
          typedef enum {
          CSL_IMX_CMD_START,
          CSL_IMX_CMD_STADDR,
          CSL_IMX_CMD_BKADDR,
          CSL_IMX_CMD_BKENABLE,
          CSL_IMX_CMD_AUTOGATING,
          CSL_IMX_CMD_WRITECMDMEM
          } CSL_ImxHwCtrlCmd;

          /* ======================================================================= 
           */
          /* CSL_ImxHwCtrlCmd enumeration for selecting type of operation * * 
           * @param CSL_IMX_CMD_START Start the IMX used * @param
           * CSL_IMX_CMD_STADDR Starting address in CMD MEM * @param
           * CSL_IMX_CMD_BKADDR Command to Break at a given Address * @param
           * CSL_IMX_CMD_BKENABLE Enable/Disable Break functionality * @param 
           * CSL_IMX_CMD_SETAUTOGATING Set Autogating on or off *
           * *========================================================================== 
           */
          typedef enum {
          CSL_IMX_QUERY_START,
          CSL_IMX_QUERY_STADDR,
          CSL_IMX_QUERY_CURRADDR,
          CSL_IMX_QUERY_MINMAXID,
          CSL_IMX_QUERY_MINMAXVAL,
          CSL_IMX_QUERY_BKSTAT,
          CSL_IMX_QUERY_BKADDR,
          CSL_IMX_QUERY_BKENABLE,
          CSL_IMX_QUERY_AUTOGATING
          } CSL_ImxHwQuery;

          /* ======================================================================= 
           */
          /* CSL_ImxBreakPointMode enumeration for selecting Break point type 
           * of operation * * @param CSL_IMX_BKPT_DISABLE Disable the Break
           * point functionality * @param CSL_IMX_BKPT_ENABLE Enable the
           * Break point functionality *
           * *========================================================================== 
           */
          typedef enum {
          CSL_IMX_BKPT_DISABLE = 0,
          CSL_IMX_BKPT_ENABLE = 1
          } CSL_ImxBkPtMode;

          /* ======================================================================= 
           */
          /* CSL_ImxBkPtHitType enumeration for knowing if Break point has
           * been hit * * @param CSL_IMX_BKPT_UNHIT Break point not reached
           * (if enabled, or IMX is busy or idle) * @param CSL_IMX_BKPT_HIT
           * Break point reached/hit *
           * *========================================================================== 
           */
          typedef enum {
          CSL_IMX_BKPT_UNHIT = 0,
          CSL_IMX_BKPT_HIT = 1
          } CSL_ImxBkPtHitType;

          /* ======================================================================= 
           */
          /* CSL_ImxClkCtrlType enumeration for selecting Break point type of 
           * operation * * @param CSL_IMX_CLK_AUTOGATE clock is on when busy, 
           * clock is off when idle * @param CSL_IMX_CLK_ALWAYSON clock is
           * on, all the time *
           * *========================================================================== 
           */
          typedef enum {
          CSL_IMX_CLK_AUTOGATE = 0,
          CSL_IMX_CLK_ALWAYSON = 1
          } CSL_ImxClkCtrlType;

#ifdef __cplusplus
          }
#endif

#endif /*_CSL_IMX4_H_*/
