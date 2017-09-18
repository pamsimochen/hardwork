#ifndef _GPP_GLBCECPISLIB_H

#define _GPP_GLBCECPISLIB_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <tistdtypes.h>

//#include "vicplib.h"
#include "vicpGlbce.h"

Int32 GPP_CPIS_glbce(
    CPIS_Handle *handle,
    CPIS_BaseParms *base,
    CPIS_GlbceParms *params,
    CPIS_ExecType execType
);



#ifdef __cplusplus
 }
#endif

#endif/* #define _GPP_CPISLIB_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
