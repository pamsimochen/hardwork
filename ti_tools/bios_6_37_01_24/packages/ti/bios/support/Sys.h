/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z63
 */

/*
 * ======== GENERATED SECTIONS ========
 *     
 *     PROLOGUE
 *     INCLUDES
 *     
 *     INTERNAL DEFINITIONS
 *     MODULE-WIDE CONFIGS
 *     FUNCTION DECLARATIONS
 *     SYSTEM FUNCTIONS
 *     
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_bios_support_Sys__include
#define ti_bios_support_Sys__include

#ifndef __nested__
#define __nested__
#define ti_bios_support_Sys__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_bios_support_Sys___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/bios/support/package/package.defs.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/IModule.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* ErrorFxn */
typedef xdc_Void (*ti_bios_support_Sys_ErrorFxn)(xdc_String, xdc_Int, xdc_VaList);

/* AbortFxn */
typedef xdc_Void (*ti_bios_support_Sys_AbortFxn)(xdc_String, xdc_VaList);

/* ExitFxn */
typedef xdc_Void (*ti_bios_support_Sys_ExitFxn)(xdc_Int);


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_bios_support_Sys_Module__diagsEnabled;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__diagsEnabled ti_bios_support_Sys_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_bios_support_Sys_Module__diagsIncluded;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__diagsIncluded ti_bios_support_Sys_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_bios_support_Sys_Module__diagsMask;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__diagsMask ti_bios_support_Sys_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_bios_support_Sys_Module__gateObj;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__gateObj ti_bios_support_Sys_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_bios_support_Sys_Module__gatePrms;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__gatePrms ti_bios_support_Sys_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_bios_support_Sys_Module__id;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__id ti_bios_support_Sys_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_bios_support_Sys_Module__loggerDefined;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerDefined ti_bios_support_Sys_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_bios_support_Sys_Module__loggerObj;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerObj ti_bios_support_Sys_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_bios_support_Sys_Module__loggerFxn0;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerFxn0 ti_bios_support_Sys_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_bios_support_Sys_Module__loggerFxn1;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerFxn1 ti_bios_support_Sys_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_bios_support_Sys_Module__loggerFxn2;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerFxn2 ti_bios_support_Sys_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_bios_support_Sys_Module__loggerFxn4;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerFxn4 ti_bios_support_Sys_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_bios_support_Sys_Module__loggerFxn8;
__extern __FAR__ const CT__ti_bios_support_Sys_Module__loggerFxn8 ti_bios_support_Sys_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_bios_support_Sys_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_bios_support_Sys_Module__startupDoneFxn ti_bios_support_Sys_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_bios_support_Sys_Object__count;
__extern __FAR__ const CT__ti_bios_support_Sys_Object__count ti_bios_support_Sys_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_bios_support_Sys_Object__heap;
__extern __FAR__ const CT__ti_bios_support_Sys_Object__heap ti_bios_support_Sys_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_bios_support_Sys_Object__sizeof;
__extern __FAR__ const CT__ti_bios_support_Sys_Object__sizeof ti_bios_support_Sys_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_bios_support_Sys_Object__table;
__extern __FAR__ const CT__ti_bios_support_Sys_Object__table ti_bios_support_Sys_Object__table__C;

/* E_error */
#define ti_bios_support_Sys_E_error (ti_bios_support_Sys_E_error__C)
typedef xdc_runtime_Error_Id CT__ti_bios_support_Sys_E_error;
__extern __FAR__ const CT__ti_bios_support_Sys_E_error ti_bios_support_Sys_E_error__C;

/* errorFxn */
#define ti_bios_support_Sys_errorFxn (ti_bios_support_Sys_errorFxn__C)
typedef ti_bios_support_Sys_ErrorFxn CT__ti_bios_support_Sys_errorFxn;
__extern __FAR__ const CT__ti_bios_support_Sys_errorFxn ti_bios_support_Sys_errorFxn__C;

/* abortFxn */
#define ti_bios_support_Sys_abortFxn (ti_bios_support_Sys_abortFxn__C)
typedef ti_bios_support_Sys_AbortFxn CT__ti_bios_support_Sys_abortFxn;
__extern __FAR__ const CT__ti_bios_support_Sys_abortFxn ti_bios_support_Sys_abortFxn__C;

/* exitFxn */
#define ti_bios_support_Sys_exitFxn (ti_bios_support_Sys_exitFxn__C)
typedef ti_bios_support_Sys_ExitFxn CT__ti_bios_support_Sys_exitFxn;
__extern __FAR__ const CT__ti_bios_support_Sys_exitFxn ti_bios_support_Sys_exitFxn__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_bios_support_Sys_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_bios_support_Sys_Module__startupDone__S, "ti_bios_support_Sys_Module__startupDone")
__extern xdc_Bool ti_bios_support_Sys_Module__startupDone__S( void );

/* error__E */
#define ti_bios_support_Sys_error ti_bios_support_Sys_error__E
xdc__CODESECT(ti_bios_support_Sys_error__E, "ti_bios_support_Sys_error")
__extern xdc_Void ti_bios_support_Sys_error__E( xdc_String error, xdc_Int errno, xdc_VaList va );
xdc__CODESECT(ti_bios_support_Sys_error__F, "ti_bios_support_Sys_error")
__extern xdc_Void ti_bios_support_Sys_error__F( xdc_String error, xdc_Int errno, xdc_VaList va );
__extern xdc_Void ti_bios_support_Sys_error__R( xdc_String error, xdc_Int errno, xdc_VaList va );

/* abort__E */
#define ti_bios_support_Sys_abort ti_bios_support_Sys_abort__E
xdc__CODESECT(ti_bios_support_Sys_abort__E, "ti_bios_support_Sys_abort")
__extern xdc_Void ti_bios_support_Sys_abort__E( xdc_String str, xdc_VaList va );
xdc__CODESECT(ti_bios_support_Sys_abort__F, "ti_bios_support_Sys_abort")
__extern xdc_Void ti_bios_support_Sys_abort__F( xdc_String str, xdc_VaList va );
__extern xdc_Void ti_bios_support_Sys_abort__R( xdc_String str, xdc_VaList va );

/* exit__E */
#define ti_bios_support_Sys_exit ti_bios_support_Sys_exit__E
xdc__CODESECT(ti_bios_support_Sys_exit__E, "ti_bios_support_Sys_exit")
__extern xdc_Void ti_bios_support_Sys_exit__E( xdc_Int stat );
xdc__CODESECT(ti_bios_support_Sys_exit__F, "ti_bios_support_Sys_exit")
__extern xdc_Void ti_bios_support_Sys_exit__F( xdc_Int stat );
__extern xdc_Void ti_bios_support_Sys_exit__R( xdc_Int stat );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_bios_support_Sys_Module_startupDone() ti_bios_support_Sys_Module__startupDone__S()

/* Object_heap */
#define ti_bios_support_Sys_Object_heap() ti_bios_support_Sys_Object__heap__C

/* Module_heap */
#define ti_bios_support_Sys_Module_heap() ti_bios_support_Sys_Object__heap__C

/* Module_id */
static inline CT__ti_bios_support_Sys_Module__id ti_bios_support_Sys_Module_id( void ) 
{
    return ti_bios_support_Sys_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_bios_support_Sys_Module_hasMask( void ) 
{
    return ti_bios_support_Sys_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_bios_support_Sys_Module_getMask( void ) 
{
    return ti_bios_support_Sys_Module__diagsMask__C != NULL ? *ti_bios_support_Sys_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_bios_support_Sys_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_bios_support_Sys_Module__diagsMask__C != NULL) *ti_bios_support_Sys_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_bios_support_Sys__top__
#undef __nested__
#endif

#endif /* ti_bios_support_Sys__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_bios_support_Sys__internalaccess))

#ifndef ti_bios_support_Sys__include_state
#define ti_bios_support_Sys__include_state


#endif /* ti_bios_support_Sys__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_bios_support_Sys__nolocalnames)

#ifndef ti_bios_support_Sys__localnames__done
#define ti_bios_support_Sys__localnames__done

/* module prefix */
#define Sys_ErrorFxn ti_bios_support_Sys_ErrorFxn
#define Sys_AbortFxn ti_bios_support_Sys_AbortFxn
#define Sys_ExitFxn ti_bios_support_Sys_ExitFxn
#define Sys_E_error ti_bios_support_Sys_E_error
#define Sys_errorFxn ti_bios_support_Sys_errorFxn
#define Sys_abortFxn ti_bios_support_Sys_abortFxn
#define Sys_exitFxn ti_bios_support_Sys_exitFxn
#define Sys_error ti_bios_support_Sys_error
#define Sys_abort ti_bios_support_Sys_abort
#define Sys_exit ti_bios_support_Sys_exit
#define Sys_Module_name ti_bios_support_Sys_Module_name
#define Sys_Module_id ti_bios_support_Sys_Module_id
#define Sys_Module_startup ti_bios_support_Sys_Module_startup
#define Sys_Module_startupDone ti_bios_support_Sys_Module_startupDone
#define Sys_Module_hasMask ti_bios_support_Sys_Module_hasMask
#define Sys_Module_getMask ti_bios_support_Sys_Module_getMask
#define Sys_Module_setMask ti_bios_support_Sys_Module_setMask
#define Sys_Object_heap ti_bios_support_Sys_Object_heap
#define Sys_Module_heap ti_bios_support_Sys_Module_heap

#endif /* ti_bios_support_Sys__localnames__done */
#endif
