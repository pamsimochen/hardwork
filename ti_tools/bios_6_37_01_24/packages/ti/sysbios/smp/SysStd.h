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
 *     VIRTUAL FUNCTIONS
 *     FUNCTION DECLARATIONS
 *     CONVERTORS
 *     SYSTEM FUNCTIONS
 *     
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_smp_SysStd__include
#define ti_sysbios_smp_SysStd__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_smp_SysStd__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_smp_SysStd___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/smp/SysStd__prologue.h>
#include <ti/sysbios/smp/package/package.defs.h>

#include <xdc/runtime/ISystemSupport.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* LineBuffer */
typedef xdc_Char __T1_ti_sysbios_smp_SysStd_LineBuffer__outbuf;
typedef xdc_Char __ARRAY1_ti_sysbios_smp_SysStd_LineBuffer__outbuf[256];
typedef __ARRAY1_ti_sysbios_smp_SysStd_LineBuffer__outbuf __TA_ti_sysbios_smp_SysStd_LineBuffer__outbuf;
struct ti_sysbios_smp_SysStd_LineBuffer {
    xdc_UInt outidx;
    __TA_ti_sysbios_smp_SysStd_LineBuffer__outbuf outbuf;
};

/* Module_State */
typedef ti_sysbios_smp_SysStd_LineBuffer __T1_ti_sysbios_smp_SysStd_Module_State__lineBuffers;
typedef ti_sysbios_smp_SysStd_LineBuffer *__ARRAY1_ti_sysbios_smp_SysStd_Module_State__lineBuffers;
typedef __ARRAY1_ti_sysbios_smp_SysStd_Module_State__lineBuffers __TA_ti_sysbios_smp_SysStd_Module_State__lineBuffers;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_smp_SysStd_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__diagsEnabled ti_sysbios_smp_SysStd_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_smp_SysStd_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__diagsIncluded ti_sysbios_smp_SysStd_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_sysbios_smp_SysStd_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__diagsMask ti_sysbios_smp_SysStd_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_smp_SysStd_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__gateObj ti_sysbios_smp_SysStd_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_smp_SysStd_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__gatePrms ti_sysbios_smp_SysStd_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_smp_SysStd_Module__id;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__id ti_sysbios_smp_SysStd_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_smp_SysStd_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerDefined ti_sysbios_smp_SysStd_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_smp_SysStd_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerObj ti_sysbios_smp_SysStd_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_smp_SysStd_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerFxn0 ti_sysbios_smp_SysStd_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_smp_SysStd_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerFxn1 ti_sysbios_smp_SysStd_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_smp_SysStd_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerFxn2 ti_sysbios_smp_SysStd_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_smp_SysStd_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerFxn4 ti_sysbios_smp_SysStd_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_smp_SysStd_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__loggerFxn8 ti_sysbios_smp_SysStd_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_smp_SysStd_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Module__startupDoneFxn ti_sysbios_smp_SysStd_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_smp_SysStd_Object__count;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Object__count ti_sysbios_smp_SysStd_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_smp_SysStd_Object__heap;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Object__heap ti_sysbios_smp_SysStd_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_smp_SysStd_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Object__sizeof ti_sysbios_smp_SysStd_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_smp_SysStd_Object__table;
__extern __FAR__ const CT__ti_sysbios_smp_SysStd_Object__table ti_sysbios_smp_SysStd_Object__table__C;


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_smp_SysStd_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*abort)(xdc_CString);
    xdc_Void (*exit)(xdc_Int);
    xdc_Void (*flush)(void);
    xdc_Void (*putch)(xdc_Char);
    xdc_Bool (*ready)(void);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_sysbios_smp_SysStd_Fxns__ ti_sysbios_smp_SysStd_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_smp_SysStd_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_smp_SysStd_Module__startupDone__S, "ti_sysbios_smp_SysStd_Module__startupDone")
__extern xdc_Bool ti_sysbios_smp_SysStd_Module__startupDone__S( void );

/* abort__E */
#define ti_sysbios_smp_SysStd_abort ti_sysbios_smp_SysStd_abort__E
xdc__CODESECT(ti_sysbios_smp_SysStd_abort__E, "ti_sysbios_smp_SysStd_abort")
__extern xdc_Void ti_sysbios_smp_SysStd_abort__E( xdc_CString str );
xdc__CODESECT(ti_sysbios_smp_SysStd_abort__F, "ti_sysbios_smp_SysStd_abort")
__extern xdc_Void ti_sysbios_smp_SysStd_abort__F( xdc_CString str );
__extern xdc_Void ti_sysbios_smp_SysStd_abort__R( xdc_CString str );

/* exit__E */
#define ti_sysbios_smp_SysStd_exit ti_sysbios_smp_SysStd_exit__E
xdc__CODESECT(ti_sysbios_smp_SysStd_exit__E, "ti_sysbios_smp_SysStd_exit")
__extern xdc_Void ti_sysbios_smp_SysStd_exit__E( xdc_Int stat );
xdc__CODESECT(ti_sysbios_smp_SysStd_exit__F, "ti_sysbios_smp_SysStd_exit")
__extern xdc_Void ti_sysbios_smp_SysStd_exit__F( xdc_Int stat );
__extern xdc_Void ti_sysbios_smp_SysStd_exit__R( xdc_Int stat );

/* flush__E */
#define ti_sysbios_smp_SysStd_flush ti_sysbios_smp_SysStd_flush__E
xdc__CODESECT(ti_sysbios_smp_SysStd_flush__E, "ti_sysbios_smp_SysStd_flush")
__extern xdc_Void ti_sysbios_smp_SysStd_flush__E( void );
xdc__CODESECT(ti_sysbios_smp_SysStd_flush__F, "ti_sysbios_smp_SysStd_flush")
__extern xdc_Void ti_sysbios_smp_SysStd_flush__F( void );
__extern xdc_Void ti_sysbios_smp_SysStd_flush__R( void );

/* putch__E */
#define ti_sysbios_smp_SysStd_putch ti_sysbios_smp_SysStd_putch__E
xdc__CODESECT(ti_sysbios_smp_SysStd_putch__E, "ti_sysbios_smp_SysStd_putch")
__extern xdc_Void ti_sysbios_smp_SysStd_putch__E( xdc_Char ch );
xdc__CODESECT(ti_sysbios_smp_SysStd_putch__F, "ti_sysbios_smp_SysStd_putch")
__extern xdc_Void ti_sysbios_smp_SysStd_putch__F( xdc_Char ch );
__extern xdc_Void ti_sysbios_smp_SysStd_putch__R( xdc_Char ch );

/* ready__E */
#define ti_sysbios_smp_SysStd_ready ti_sysbios_smp_SysStd_ready__E
xdc__CODESECT(ti_sysbios_smp_SysStd_ready__E, "ti_sysbios_smp_SysStd_ready")
__extern xdc_Bool ti_sysbios_smp_SysStd_ready__E( void );
xdc__CODESECT(ti_sysbios_smp_SysStd_ready__F, "ti_sysbios_smp_SysStd_ready")
__extern xdc_Bool ti_sysbios_smp_SysStd_ready__F( void );
__extern xdc_Bool ti_sysbios_smp_SysStd_ready__R( void );


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_ISystemSupport_Module ti_sysbios_smp_SysStd_Module_upCast( void )
{
    return (xdc_runtime_ISystemSupport_Module)&ti_sysbios_smp_SysStd_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ISystemSupport */
#define ti_sysbios_smp_SysStd_Module_to_xdc_runtime_ISystemSupport ti_sysbios_smp_SysStd_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_smp_SysStd_Module_startupDone() ti_sysbios_smp_SysStd_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_smp_SysStd_Object_heap() ti_sysbios_smp_SysStd_Object__heap__C

/* Module_heap */
#define ti_sysbios_smp_SysStd_Module_heap() ti_sysbios_smp_SysStd_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_smp_SysStd_Module__id ti_sysbios_smp_SysStd_Module_id( void ) 
{
    return ti_sysbios_smp_SysStd_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_smp_SysStd_Module_hasMask( void ) 
{
    return ti_sysbios_smp_SysStd_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_smp_SysStd_Module_getMask( void ) 
{
    return ti_sysbios_smp_SysStd_Module__diagsMask__C != NULL ? *ti_sysbios_smp_SysStd_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_smp_SysStd_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_smp_SysStd_Module__diagsMask__C != NULL) *ti_sysbios_smp_SysStd_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/sysbios/smp/SysStd__epilogue.h>

#ifdef ti_sysbios_smp_SysStd__top__
#undef __nested__
#endif

#endif /* ti_sysbios_smp_SysStd__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_smp_SysStd__internalaccess))

#ifndef ti_sysbios_smp_SysStd__include_state
#define ti_sysbios_smp_SysStd__include_state

/* Module_State */
struct ti_sysbios_smp_SysStd_Module_State {
    __TA_ti_sysbios_smp_SysStd_Module_State__lineBuffers lineBuffers;
};

/* Module__state__V */
extern struct ti_sysbios_smp_SysStd_Module_State__ ti_sysbios_smp_SysStd_Module__state__V;

#endif /* ti_sysbios_smp_SysStd__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_smp_SysStd__nolocalnames)

#ifndef ti_sysbios_smp_SysStd__localnames__done
#define ti_sysbios_smp_SysStd__localnames__done

/* module prefix */
#define SysStd_LineBuffer ti_sysbios_smp_SysStd_LineBuffer
#define SysStd_Module_State ti_sysbios_smp_SysStd_Module_State
#define SysStd_abort ti_sysbios_smp_SysStd_abort
#define SysStd_exit ti_sysbios_smp_SysStd_exit
#define SysStd_flush ti_sysbios_smp_SysStd_flush
#define SysStd_putch ti_sysbios_smp_SysStd_putch
#define SysStd_ready ti_sysbios_smp_SysStd_ready
#define SysStd_Module_name ti_sysbios_smp_SysStd_Module_name
#define SysStd_Module_id ti_sysbios_smp_SysStd_Module_id
#define SysStd_Module_startup ti_sysbios_smp_SysStd_Module_startup
#define SysStd_Module_startupDone ti_sysbios_smp_SysStd_Module_startupDone
#define SysStd_Module_hasMask ti_sysbios_smp_SysStd_Module_hasMask
#define SysStd_Module_getMask ti_sysbios_smp_SysStd_Module_getMask
#define SysStd_Module_setMask ti_sysbios_smp_SysStd_Module_setMask
#define SysStd_Object_heap ti_sysbios_smp_SysStd_Object_heap
#define SysStd_Module_heap ti_sysbios_smp_SysStd_Module_heap
#define SysStd_Module_upCast ti_sysbios_smp_SysStd_Module_upCast
#define SysStd_Module_to_xdc_runtime_ISystemSupport ti_sysbios_smp_SysStd_Module_to_xdc_runtime_ISystemSupport

#endif /* ti_sysbios_smp_SysStd__localnames__done */
#endif
