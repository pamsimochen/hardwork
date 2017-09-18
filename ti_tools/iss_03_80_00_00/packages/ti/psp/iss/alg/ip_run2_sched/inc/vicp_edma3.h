/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  Prototype of Wrapper functions that interfaces with                     */
/*      EDMA3 low-level driver                                              */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*     NAME                                                                 */
/*        vicp_edma3.h -- Prototype of wrapper functions with EDMA3 LLD     */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*      This file contains prototype wrapper functions that interface with  */
/*      EDMA3 LLD. The implementation of these functions are OS/platform    */
/*      dependent and can be modified by the user. By default this file     */
/*      is built into the library dmcsl648_bios.lib or dmcsl644x_bios.lib.  */
/*      A new implementation can override the default one by adding the     */
/*      file containing the new implementation to the application's project.*/            
/*                                                                          */
/*     REV                                                                  */ 
/*        version 0.0.1:  24 Nov                                            */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _VICP_EDMA3_H

#define _VICP_EDMA3_H

/* Include EDMA3 Driver */
#include <xdc/std.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>


#ifdef __cplusplus
extern "C" {
#endif

extern EDMA3_DRV_Handle VICP_EDMA3_hEdma;
extern EDMA3_OS_Sem_Handle VICP_EDMA3_semHandle;
extern EDMA3_RM_EventQueue VICP_EDMA3_FROM_DDR_queue;
extern EDMA3_RM_EventQueue VICP_EDMA3_TO_DDR_queue;

/**
 * \brief SoC specific TC related information. Specified in the sample
 * configuration file (vicp_edma3_dm64xx_cfg.c).
 */
extern Uint32 VICP_EDMA3_numEdma3Tc;
extern Uint32 VICP_EDMA3_ccXferCompInt;
extern Uint32 VICP_EDMA3_ccErrorInt;
extern Uint32 VICP_EDMA3_tcErrorInt[8];
extern Uint32 VICP_EDMA3_hwInt;

EDMA3_DRV_Result VICP_EDMA3_init(void);
EDMA3_DRV_Result VICP_EDMA3_deinit (void);
void VICP_initInternal();

Int16 VICP_EDMA3_allocChannel(Uint16 chan);
void VICP_EDMA3_clearChanInt(Uint16 chan);
Int16 VICP_EDMA3_allocParamEntry(Uint16 entry);
Int16 VICP_EDMA3_freeParamEntry(Uint16 entry);
Int16 VICP_EDMA3_freeChannel(Uint16 chan);
void VICP_EDMA3_initParamEntry(Uint16 paramEntry,
        Int8            * srcAddress,
        Int8            * dstAddress,
        Uint16            arraySize,
        Uint16            arrayCount,
        Uint16            frameCount,
        Int16            inputArrayUpdate,
        Int16            outputArrayUpdate,
        Int16            inputFrameUpdate,
        Int16            outputFrameUpdate,
        Int16            staticEntry,
        Int16            syncType,
        Uint8              txfrCompleteCode,
        Int16              tccCompletionMode,
        Int16              txfrCompleteChain,
        Int16              txfrCompleteIntr,
        Int16              intTxfrCompleteChain,
        Int16              intTxfrCompleteIntr);

void VICP_EDMA3_link(Uint16 param1, Uint16 param2);

Int16 VICP_EDMA3_copyParamEntry(Uint16 dstEntry, Uint16 srcEntry);

Int16 VICP_EDMA3_mapChanToQ(Uint16 chan, Uint16 queue);

Int16 VICP_EDMA3_mapChanToInputQ(Uint16 chan);

Int16 VICP_EDMA3_mapChanToOutputQ(Uint16 chan);

Int16 VICP_EDMA3_startChan(Uint16 chan);

Int16 VICP_EDMA3_waitChan(Uint16 chan);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif




