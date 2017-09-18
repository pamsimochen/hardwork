/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  CPIS layer prototypes                                                   */
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
/*        gppCore.h --                                                      */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces supported   */
/*        by the general purpose C core functions belonging to the          */
/*        CoProcessor InfraStructure (CPIS)'s gpp core library              */
/*                                                                          */
/*     REV                                                                  */
/*        version 0.1  6 June , 2011                                        */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2011 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _GPPCORE_H

#define _GPPCORE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <tistdtypes.h>
#include "cpisCore.h"

typedef Int32 (*GPP_CPIS_Func)();

extern Uint32 GPP_CPIS_errno;

Int32 GPP_CPIS_getMemSize(Uint16 maxNumProc);
Int32 GPP_CPIS_init(CPIS_Init *init);
Int32 GPP_CPIS_deInit();

Int32 GPP_CPIS_start(CPIS_Handle handle, GPP_CPIS_Func func);

Int32 GPP_CPIS_wait(CPIS_Handle handle);

Int32 GPP_CPIS_delete(CPIS_Handle handle);

Int32 GPP_CPIS_updateSrcDstPtr(CPIS_Handle handle, CPIS_BaseParms *base);

Int32 GPP_CPIS_setWaitCB(Int32 (*waitCB)(void*arg));

Int32 GPP_CPIS_setWaitCBArg(void* waitCBarg);

Int32 GPP_CPIS_scatterGather(
        CPIS_Handle *handle,
        CPIS_BaseParms *base,
        CPIS_ScatterGatherParms *params,
        CPIS_ExecType execType
);

#ifdef __cplusplus
 }
#endif

#endif/* #define _GPPCORE_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


