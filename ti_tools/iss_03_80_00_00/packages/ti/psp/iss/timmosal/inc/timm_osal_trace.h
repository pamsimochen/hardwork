/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information:
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *
 */

/*
*  @file timm_osal_trace.h
*  The timm_osal_types header file defines the primative osal type definitions.
*  @path
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 0.2: Ported to BIOS 6, gaurav.a@ti.com
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

#ifndef _TIMM_OSAL_TRACES_H_
#define _TIMM_OSAL_TRACES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdio.h"
#include "timm_osal_types.h"

/*******************************************************************************
* Enum and Defines
*******************************************************************************/
typedef enum TIMM_OSAL_TRACEGRP_TYPE
{
  TIMM_OSAL_TRACEGRP_SYSTEM=1,
  TIMM_OSAL_TRACEGRP_OMXBASE=(1<<1),
  TIMM_OSAL_TRACEGRP_DOMX=(1<<2),
  TIMM_OSAL_TRACEGRP_OMXVIDEOENC=(1<<3),
  TIMM_OSAL_TRACEGRP_OMXVIDEODEC=(1<<4),
  TIMM_OSAL_TRACEGRP_OMXCAM=(1<<5),
  TIMM_OSAL_TRACEGRP_OMXIMGDEC=(1<<6),
  TIMM_OSAL_TRACEGRP_DRIVERS=(1<<7),
  TIMM_OSAL_TRACEGRP_SIMCOPALGOS=(1<<8)
} TIMM_OSAL_TRACEGRP;
#if 0 /* HACK */
/*******************************************************************************
* Function prototypes
* *******************************************************************************/

/* 
 * Main trace API, but apps should NOT use this API directly. Based on the severity
 * should use one of the error, info, warning, debug, entering/exitig APIs. 
 * See below for API details
 */
void TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_CHAR *pcFormat, ...);

/*
 * API to trace dump an array of data. To be rarely used
 */
void TIMM_OSAL_TracePutBuf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_PTR* pDataBuf, TIMM_OSAL_S8 sElementSize, TIMM_OSAL_S32 lElementCount);

/*
 * API to flush the contents of trace buffer to trace output
 */
void TIMM_OSAL_TraceFlush();

/*
 * Set new set of trace groups. Application should first get the current set of
 * trace groups using TIMM_OSAL_GetTraceGrp API, then OR with new trace groups 
 * to be enabled and call this APIs
 */
void TIMM_OSAL_SetTraceGrp(TIMM_OSAL_U32 ulTraceGroups);

/* 
 * Get the current set of trace groups enabled
 * */
TIMM_OSAL_U32 TIMM_OSAL_GetTraceGrp();

    
/*
 * Depreciated API. 
 */
void TIMM_OSAL_TraceFunction (char *format, ...);
#define TIMM_OSAL_Debug    TIMM_OSAL_TraceFunction

/******************************************************************************
* Debug Trace defines
******************************************************************************/
/**
* The OSAL debug trace detail can be set at compile time by defining the flag
* TIMM_OSAL_DEBUG_TRACE_DETAIL=<Details>
* detail - 0 - no detail
*          1 - function name
*          2 - function name, line number
* Prefix is added to every debug trace message
*/
#ifndef TIMM_OSAL_DEBUG_TRACE_DETAIL
#define TIMM_OSAL_DEBUG_TRACE_DETAIL 0
#endif

#if ( TIMM_OSAL_DEBUG_TRACE_DETAIL > 0 )
#define DL1 "%s:"
#define DR1 ,__FILE__
#else
#define DL1
#define DR1
#endif
#if ( TIMM_OSAL_DEBUG_TRACE_DETAIL > 1 )
#define DL2 "[%d]:"
#define DR2 ,__LINE__
#else
#define DL2
#define DR2
#endif

/**
* The OSAL debug trace level can be set at compile time by defining the flag
* TIMM_OSAL_DEBUG_TRACE_LEVEL=<Level>
* The debug levels are:
* Level 0 - No trace
* Level 1 - Error   [Errors]
* Level 2 - Warning [Warnings that are useful to know about]
* Level 3 - Info    [General information]
* Level 4 - Debug   [most-commonly used statement for us developers]
* Level 5 - Trace   ["ENTERING <function>" and "EXITING <function>" statements]
*
* Example: if TIMM_OSAL_DEBUG_TRACE_LEVEL=3, then level 1,2 and 3 traces messages
* are enabled.
*/
#ifndef TIMM_OSAL_DEBUG_TRACE_LEVEL
#define TIMM_OSAL_DEBUG_TRACE_LEVEL 4
#endif

/*******************************************************************************
** New Trace to be used by Applications
*******************************************************************************/

/**
* TIMM_OSAL_ErrorExt() -- Fatal errors
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 1)
#define TIMM_OSAL_ErrorExt(TRACE_GRP, ARGS, ...)   \
                  TIMM_OSAL_TracePrintf(TRACE_GRP, "ERROR:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_ErrorExt(TRACE_GRP, ARGS)
#endif

/**
* TIMM_OSAL_WarningExt() -- Warnings that are useful to know about
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 2)
#define TIMM_OSAL_WarningExt(TRACE_GRP, ARGS, ...)  \
                 TIMM_OSAL_TracePrintf(TRACE_GRP, "WARNING:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_WarningExt(TRACE_GRP, ARGS,...)
#endif

/**
* TIMM_OSAL_InfoExt() -- general information
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 3)
#define TIMM_OSAL_InfoExt(TRACE_GRP,ARGS, ...)  \
                 TIMM_OSAL_TracePrintf(TRACE_GRP, "INFO:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_InfoExt(TRACE_GRP, ARGS, ...)
#endif

/**
* TIMM_OSAL_DebugExt() -- most-commonly used statement for us developers
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 4)
#define TIMM_OSAL_TraceExt(TRACE_GRP, ARGS, ...)  \
                TIMM_OSAL_TracePrintf(TRACE_GRP, "TRACE:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_TraceExt(TRACE_GRP, ARGS, ...)
#endif

/**
* TIMM_OSAL_EnteringExt() -- "ENTERING <function>" statements
* TIMM_OSAL_ExitingExt()  -- "EXITING <function>" statements
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 5)
#define TIMM_OSAL_EnteringExt(TRACE_GRP)   TIMM_OSAL_TracePrintf(TRACE_GRP, "ENTERING: %s",__FUNCTION__)
#define TIMM_OSAL_ExitingExt(TRACE_GRP,ARG)   \
                TIMM_OSAL_TracePrintf(TRACE_GRP, "EXITING: %s:Returned(%d)",__FUNCTION__,ARG)
#else
#define TIMM_OSAL_EnteringExt(TRACE_GRP)
#define TIMM_OSAL_ExitingExt(TRACE_GRP, ARG)
#endif

/*******************************************************************************
** Deprecated Trace APIs. These APIs will not be maintained in future. Developers
are urged to move to new Trace APIs
*******************************************************************************/

/**
* TIMM_OSAL_Error() -- Fatal errors
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 1)
#define TIMM_OSAL_Error(ARGS, ...)   \
                  TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: ERROR:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_Error(ARGS, ...)
#endif

/**
* TIMM_OSAL_Warning() -- Warnings that are useful to know about
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 2)
#define TIMM_OSAL_Warning(ARGS, ...)  \
                 TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: WARNING:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_Warning(ARGS,...)
#endif

/**
* TIMM_OSAL_Info() -- general information
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 3)
#define TIMM_OSAL_Info(ARGS, ...)  \
                 TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: INFO:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_Info(ARGS,...)
#endif

/**
* TIMM_OSAL_Debug() -- most-commonly used statement for us developers
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 4)
#define TIMM_OSAL_Trace(ARGS, ...)  \
                TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: TRACE:"DL1 DL2 ARGS "\n" DR1 DR2, ##__VA_ARGS__)
#else
#define TIMM_OSAL_Trace(ARGS, ##__VA_ARGS__)
#endif

/**
* TIMM_OSAL_Entering() -- "ENTERING <function>" statements
* TIMM_OSAL_Exiting()  -- "EXITING <function>" statements
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 5)
#define TIMM_OSAL_Entering()   TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: ENTERING: %s",__FUNCTION__)
#define TIMM_OSAL_Exiting(ARG)   \
                TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP_SYSTEM, "Depreciated API: EXITING: %s:Returned(%d)",__FUNCTION__,ARG)
#else
#define TIMM_OSAL_Entering()
#define TIMM_OSAL_Exiting(ARG)
#endif


#else //#if 0

/*******************************************************************************
* Function prototypes
* *******************************************************************************/

/* 
 * Main trace API, but apps should NOT use this API directly. Based on the severity
 * should use one of the error, info, warning, debug, entering/exitig APIs. 
 * See below for API details
 */
void TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_CHAR *pcFormat, ...);

/*
 * API to trace dump an array of data. To be rarely used
 */
void TIMM_OSAL_TracePutBuf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_PTR* pDataBuf, TIMM_OSAL_S8 sElementSize, TIMM_OSAL_S32 lElementCount);

/*
 * API to flush the contents of trace buffer to trace output
 */
void TIMM_OSAL_TraceFlush();

/*
 * Set new set of trace groups. Application should first get the current set of
 * trace groups using TIMM_OSAL_GetTraceGrp API, then OR with new trace groups 
 * to be enabled and call this APIs
 */
void TIMM_OSAL_SetTraceGrp(TIMM_OSAL_U32 ulTraceGroups);

/* 
 * Get the current set of trace groups enabled
 * */
TIMM_OSAL_U32 TIMM_OSAL_GetTraceGrp();

    
/*
 * Depreciated API. 
 */
void TIMM_OSAL_TraceFunction (char *format, ...);
#define TIMM_OSAL_Debug    TIMM_OSAL_TraceFunction

/******************************************************************************
* Debug Trace defines
******************************************************************************/
/**
* The OSAL debug trace detail can be set at compile time by defining the flag
* TIMM_OSAL_DEBUG_TRACE_DETAIL=<Details>
* detail - 0 - no detail
*          1 - function name
*          2 - function name, line number
* Prefix is added to every debug trace message
*/
#ifndef TIMM_OSAL_DEBUG_TRACE_DETAIL
#define TIMM_OSAL_DEBUG_TRACE_DETAIL 0
#endif

#if ( TIMM_OSAL_DEBUG_TRACE_DETAIL > 0 )
#define DL1 "%s:"
#define DR1 ,__FILE__
#else
#define DL1
#define DR1
#endif
#if ( TIMM_OSAL_DEBUG_TRACE_DETAIL > 1 )
#define DL2 "[%d]:"
#define DR2 ,__LINE__
#else
#define DL2
#define DR2
#endif

/**
* The OSAL debug trace level can be set at compile time by defining the flag
* TIMM_OSAL_DEBUG_TRACE_LEVEL=<Level>
* The debug levels are:
* Level 0 - No trace
* Level 1 - Error   [Errors]
* Level 2 - Warning [Warnings that are useful to know about]
* Level 3 - Info    [General information]
* Level 4 - Debug   [most-commonly used statement for us developers]
* Level 5 - Trace   ["ENTERING <function>" and "EXITING <function>" statements]
*
* Example: if TIMM_OSAL_DEBUG_TRACE_LEVEL=3, then level 1,2 and 3 traces messages
* are enabled.
*/
#ifndef TIMM_OSAL_DEBUG_TRACE_LEVEL
#define TIMM_OSAL_DEBUG_TRACE_LEVEL 4
#endif

/*******************************************************************************
** New Trace to be used by Applications
*******************************************************************************/

/**
* TIMM_OSAL_ErrorExt() -- Fatal errors
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 1)
#define TIMM_OSAL_ErrorExt(TRACE_GRP, ARGS)
#else
#define TIMM_OSAL_ErrorExt(TRACE_GRP, ARGS)
#endif

/**
* TIMM_OSAL_WarningExt() -- Warnings that are useful to know about
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 2)
#define TIMM_OSAL_WarningExt(TRACE_GRP, ARGS)
#else
#define TIMM_OSAL_WarningExt(TRACE_GRP, ARGS,...)
#endif

/**
* TIMM_OSAL_InfoExt() -- general information
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 3)
#define TIMM_OSAL_InfoExt(TRACE_GRP,ARGS) 
#else
#define TIMM_OSAL_InfoExt(TRACE_GRP, ARGS, ...)
#endif

/**
* TIMM_OSAL_DebugExt() -- most-commonly used statement for us developers
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 4)
#define TIMM_OSAL_TraceExt(TRACE_GRP, ARGS) 
#else
#define TIMM_OSAL_TraceExt(TRACE_GRP, ARGS)
#endif

/**
* TIMM_OSAL_EnteringExt() -- "ENTERING <function>" statements
* TIMM_OSAL_ExitingExt()  -- "EXITING <function>" statements
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 5)
#define TIMM_OSAL_EnteringExt(TRACE_GRP) 
#define TIMM_OSAL_ExitingExt(TRACE_GRP,ARG) 
#else
#define TIMM_OSAL_EnteringExt(TRACE_GRP)
#define TIMM_OSAL_ExitingExt(TRACE_GRP, ARG)
#endif

/*******************************************************************************
** Deprecated Trace APIs. These APIs will not be maintained in future. Developers
are urged to move to new Trace APIs
*******************************************************************************/

/**
* TIMM_OSAL_Error() -- Fatal errors
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 1)
#define TIMM_OSAL_Error(ARGS)   
#else
#define TIMM_OSAL_Error(ARGS, ...)
#endif

/**
* TIMM_OSAL_Warning() -- Warnings that are useful to know about
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 2)
#define TIMM_OSAL_Warning(ARGS) 
#else
#define TIMM_OSAL_Warning(ARGS,...)
#endif

/**
* TIMM_OSAL_Info() -- general information
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 3)
#define TIMM_OSAL_Info(ARGS) 
#else
#define TIMM_OSAL_Info(ARGS,...)
#endif

/**
* TIMM_OSAL_Debug() -- most-commonly used statement for us developers
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 4)
#define TIMM_OSAL_Trace(ARGS) 
#else
#define TIMM_OSAL_Trace(ARGS, ##__VA_ARGS__)
#endif

/**
* TIMM_OSAL_Entering() -- "ENTERING <function>" statements
* TIMM_OSAL_Exiting()  -- "EXITING <function>" statements
*/
#if (TIMM_OSAL_DEBUG_TRACE_LEVEL >= 5)
#define TIMM_OSAL_Entering()  
#define TIMM_OSAL_Exiting(ARG)
#else
#define TIMM_OSAL_Entering()
#define TIMM_OSAL_Exiting(ARG)
#endif

#endif
 //#if 0

/******************************************************************************
** New Trace APIs
*******************************************************************************/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_TRACES_H_ */


