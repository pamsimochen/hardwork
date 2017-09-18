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

#ifndef ti_mcu_msp430_runtime_SysCallback__include
#define ti_mcu_msp430_runtime_SysCallback__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_SysCallback__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_SysCallback___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/mcu/msp430/runtime/package/package.defs.h>

#include <xdc/runtime/ISystemSupport.h>
#include <xdc/runtime/Types.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* PutCharFxn */
typedef xdc_Void (*ti_mcu_msp430_runtime_SysCallback_PutCharFxn)(xdc_Char);

/* AtExitFxn */
typedef xdc_Int (*ti_mcu_msp430_runtime_SysCallback_AtExitFxn)(xdc_Void(*)(xdc_Void));

/* AbortFxn */
typedef xdc_Void (*ti_mcu_msp430_runtime_SysCallback_AbortFxn)(xdc_Void);

/* InitFxn */
typedef xdc_Void (*ti_mcu_msp430_runtime_SysCallback_InitFxn)(xdc_Void);

/* ExitFxn */
typedef xdc_Void (*ti_mcu_msp430_runtime_SysCallback_ExitFxn)(xdc_Int);


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsEnabled ti_mcu_msp430_runtime_SysCallback_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsIncluded ti_mcu_msp430_runtime_SysCallback_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__diagsMask ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_SysCallback_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__gateObj ti_mcu_msp430_runtime_SysCallback_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_SysCallback_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__gatePrms ti_mcu_msp430_runtime_SysCallback_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_SysCallback_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__id ti_mcu_msp430_runtime_SysCallback_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerDefined ti_mcu_msp430_runtime_SysCallback_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerObj ti_mcu_msp430_runtime_SysCallback_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn0 ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn1 ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn2 ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn4 ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn8 ti_mcu_msp430_runtime_SysCallback_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_SysCallback_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Module__startupDoneFxn ti_mcu_msp430_runtime_SysCallback_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_SysCallback_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Object__count ti_mcu_msp430_runtime_SysCallback_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_SysCallback_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Object__heap ti_mcu_msp430_runtime_SysCallback_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_SysCallback_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Object__sizeof ti_mcu_msp430_runtime_SysCallback_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_SysCallback_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_Object__table ti_mcu_msp430_runtime_SysCallback_Object__table__C;

/* putCharFxn */
#define ti_mcu_msp430_runtime_SysCallback_putCharFxn (ti_mcu_msp430_runtime_SysCallback_putCharFxn__C)
typedef ti_mcu_msp430_runtime_SysCallback_PutCharFxn CT__ti_mcu_msp430_runtime_SysCallback_putCharFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_putCharFxn ti_mcu_msp430_runtime_SysCallback_putCharFxn__C;

/* atExitFxn */
#define ti_mcu_msp430_runtime_SysCallback_atExitFxn (ti_mcu_msp430_runtime_SysCallback_atExitFxn__C)
typedef ti_mcu_msp430_runtime_SysCallback_AtExitFxn CT__ti_mcu_msp430_runtime_SysCallback_atExitFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_atExitFxn ti_mcu_msp430_runtime_SysCallback_atExitFxn__C;

/* abortFxn */
#define ti_mcu_msp430_runtime_SysCallback_abortFxn (ti_mcu_msp430_runtime_SysCallback_abortFxn__C)
typedef ti_mcu_msp430_runtime_SysCallback_AbortFxn CT__ti_mcu_msp430_runtime_SysCallback_abortFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_abortFxn ti_mcu_msp430_runtime_SysCallback_abortFxn__C;

/* initFxn */
#define ti_mcu_msp430_runtime_SysCallback_initFxn (ti_mcu_msp430_runtime_SysCallback_initFxn__C)
typedef ti_mcu_msp430_runtime_SysCallback_InitFxn CT__ti_mcu_msp430_runtime_SysCallback_initFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_initFxn ti_mcu_msp430_runtime_SysCallback_initFxn__C;

/* exitFxn */
#define ti_mcu_msp430_runtime_SysCallback_exitFxn (ti_mcu_msp430_runtime_SysCallback_exitFxn__C)
typedef ti_mcu_msp430_runtime_SysCallback_ExitFxn CT__ti_mcu_msp430_runtime_SysCallback_exitFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_SysCallback_exitFxn ti_mcu_msp430_runtime_SysCallback_exitFxn__C;


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_mcu_msp430_runtime_SysCallback_Fxns__ {
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
__extern const ti_mcu_msp430_runtime_SysCallback_Fxns__ ti_mcu_msp430_runtime_SysCallback_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_SysCallback_Module_startup ti_mcu_msp430_runtime_SysCallback_Module_startup__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_Module_startup__E, "ti_mcu_msp430_runtime_SysCallback_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_SysCallback_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_Module_startup__F, "ti_mcu_msp430_runtime_SysCallback_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_SysCallback_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_Module_startup__R, "ti_mcu_msp430_runtime_SysCallback_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_SysCallback_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_Module__startupDone__S, "ti_mcu_msp430_runtime_SysCallback_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_SysCallback_Module__startupDone__S( void );

/* abort__E */
#define ti_mcu_msp430_runtime_SysCallback_abort ti_mcu_msp430_runtime_SysCallback_abort__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_abort__E, "ti_mcu_msp430_runtime_SysCallback_abort")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_abort__E( xdc_CString str );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_abort__F, "ti_mcu_msp430_runtime_SysCallback_abort")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_abort__F( xdc_CString str );
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_abort__R( xdc_CString str );

/* exit__E */
#define ti_mcu_msp430_runtime_SysCallback_exit ti_mcu_msp430_runtime_SysCallback_exit__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_exit__E, "ti_mcu_msp430_runtime_SysCallback_exit")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_exit__E( xdc_Int stat );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_exit__F, "ti_mcu_msp430_runtime_SysCallback_exit")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_exit__F( xdc_Int stat );
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_exit__R( xdc_Int stat );

/* flush__E */
#define ti_mcu_msp430_runtime_SysCallback_flush ti_mcu_msp430_runtime_SysCallback_flush__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_flush__E, "ti_mcu_msp430_runtime_SysCallback_flush")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_flush__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_flush__F, "ti_mcu_msp430_runtime_SysCallback_flush")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_flush__F( void );
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_flush__R( void );

/* putch__E */
#define ti_mcu_msp430_runtime_SysCallback_putch ti_mcu_msp430_runtime_SysCallback_putch__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_putch__E, "ti_mcu_msp430_runtime_SysCallback_putch")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_putch__E( xdc_Char ch );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_putch__F, "ti_mcu_msp430_runtime_SysCallback_putch")
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_putch__F( xdc_Char ch );
__extern xdc_Void ti_mcu_msp430_runtime_SysCallback_putch__R( xdc_Char ch );

/* ready__E */
#define ti_mcu_msp430_runtime_SysCallback_ready ti_mcu_msp430_runtime_SysCallback_ready__E
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_ready__E, "ti_mcu_msp430_runtime_SysCallback_ready")
__extern xdc_Bool ti_mcu_msp430_runtime_SysCallback_ready__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_SysCallback_ready__F, "ti_mcu_msp430_runtime_SysCallback_ready")
__extern xdc_Bool ti_mcu_msp430_runtime_SysCallback_ready__F( void );
__extern xdc_Bool ti_mcu_msp430_runtime_SysCallback_ready__R( void );


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_ISystemSupport_Module ti_mcu_msp430_runtime_SysCallback_Module_upCast( void )
{
    return (xdc_runtime_ISystemSupport_Module)&ti_mcu_msp430_runtime_SysCallback_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ISystemSupport */
#define ti_mcu_msp430_runtime_SysCallback_Module_to_xdc_runtime_ISystemSupport ti_mcu_msp430_runtime_SysCallback_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_SysCallback_Module_startupDone() ti_mcu_msp430_runtime_SysCallback_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_SysCallback_Object_heap() ti_mcu_msp430_runtime_SysCallback_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_SysCallback_Module_heap() ti_mcu_msp430_runtime_SysCallback_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_SysCallback_Module__id ti_mcu_msp430_runtime_SysCallback_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_SysCallback_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_SysCallback_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_SysCallback_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_SysCallback_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_SysCallback_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_SysCallback__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_SysCallback__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_SysCallback__internalaccess))

#ifndef ti_mcu_msp430_runtime_SysCallback__include_state
#define ti_mcu_msp430_runtime_SysCallback__include_state


#endif /* ti_mcu_msp430_runtime_SysCallback__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_SysCallback__nolocalnames)

#ifndef ti_mcu_msp430_runtime_SysCallback__localnames__done
#define ti_mcu_msp430_runtime_SysCallback__localnames__done

/* module prefix */
#define SysCallback_PutCharFxn ti_mcu_msp430_runtime_SysCallback_PutCharFxn
#define SysCallback_AtExitFxn ti_mcu_msp430_runtime_SysCallback_AtExitFxn
#define SysCallback_AbortFxn ti_mcu_msp430_runtime_SysCallback_AbortFxn
#define SysCallback_InitFxn ti_mcu_msp430_runtime_SysCallback_InitFxn
#define SysCallback_ExitFxn ti_mcu_msp430_runtime_SysCallback_ExitFxn
#define SysCallback_putCharFxn ti_mcu_msp430_runtime_SysCallback_putCharFxn
#define SysCallback_atExitFxn ti_mcu_msp430_runtime_SysCallback_atExitFxn
#define SysCallback_abortFxn ti_mcu_msp430_runtime_SysCallback_abortFxn
#define SysCallback_initFxn ti_mcu_msp430_runtime_SysCallback_initFxn
#define SysCallback_exitFxn ti_mcu_msp430_runtime_SysCallback_exitFxn
#define SysCallback_abort ti_mcu_msp430_runtime_SysCallback_abort
#define SysCallback_exit ti_mcu_msp430_runtime_SysCallback_exit
#define SysCallback_flush ti_mcu_msp430_runtime_SysCallback_flush
#define SysCallback_putch ti_mcu_msp430_runtime_SysCallback_putch
#define SysCallback_ready ti_mcu_msp430_runtime_SysCallback_ready
#define SysCallback_Module_name ti_mcu_msp430_runtime_SysCallback_Module_name
#define SysCallback_Module_id ti_mcu_msp430_runtime_SysCallback_Module_id
#define SysCallback_Module_startup ti_mcu_msp430_runtime_SysCallback_Module_startup
#define SysCallback_Module_startupDone ti_mcu_msp430_runtime_SysCallback_Module_startupDone
#define SysCallback_Module_hasMask ti_mcu_msp430_runtime_SysCallback_Module_hasMask
#define SysCallback_Module_getMask ti_mcu_msp430_runtime_SysCallback_Module_getMask
#define SysCallback_Module_setMask ti_mcu_msp430_runtime_SysCallback_Module_setMask
#define SysCallback_Object_heap ti_mcu_msp430_runtime_SysCallback_Object_heap
#define SysCallback_Module_heap ti_mcu_msp430_runtime_SysCallback_Module_heap
#define SysCallback_Module_upCast ti_mcu_msp430_runtime_SysCallback_Module_upCast
#define SysCallback_Module_to_xdc_runtime_ISystemSupport ti_mcu_msp430_runtime_SysCallback_Module_to_xdc_runtime_ISystemSupport

#endif /* ti_mcu_msp430_runtime_SysCallback__localnames__done */
#endif
