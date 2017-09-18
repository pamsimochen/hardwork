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
 *     PER-INSTANCE TYPES
 *     VIRTUAL FUNCTIONS
 *     FUNCTION DECLARATIONS
 *     FUNCTION SELECTORS
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

#ifndef ti_mcu_msp430_runtime_LoggerCallback__include
#define ti_mcu_msp430_runtime_LoggerCallback__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_LoggerCallback__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_LoggerCallback___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/mcu/msp430/runtime/package/package.defs.h>

#include <xdc/runtime/ILogger.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/ITimestampClient.h>
#include <ti/mcu/msp430/runtime/package/LoggerCallback_TimestampProxy.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* CallbackFxn */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_CallbackFxn)(xdc_runtime_Log_EventRec*);


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsEnabled ti_mcu_msp430_runtime_LoggerCallback_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsIncluded ti_mcu_msp430_runtime_LoggerCallback_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_LoggerCallback_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__gateObj ti_mcu_msp430_runtime_LoggerCallback_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_LoggerCallback_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__gatePrms ti_mcu_msp430_runtime_LoggerCallback_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_LoggerCallback_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__id ti_mcu_msp430_runtime_LoggerCallback_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerDefined ti_mcu_msp430_runtime_LoggerCallback_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerObj ti_mcu_msp430_runtime_LoggerCallback_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn0 ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn1 ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn2 ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn4 ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn8 ti_mcu_msp430_runtime_LoggerCallback_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_LoggerCallback_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Module__startupDoneFxn ti_mcu_msp430_runtime_LoggerCallback_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_LoggerCallback_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Object__count ti_mcu_msp430_runtime_LoggerCallback_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_LoggerCallback_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Object__heap ti_mcu_msp430_runtime_LoggerCallback_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_LoggerCallback_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Object__sizeof ti_mcu_msp430_runtime_LoggerCallback_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_LoggerCallback_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_Object__table ti_mcu_msp430_runtime_LoggerCallback_Object__table__C;

/* callbackFxn */
#define ti_mcu_msp430_runtime_LoggerCallback_callbackFxn (ti_mcu_msp430_runtime_LoggerCallback_callbackFxn__C)
typedef ti_mcu_msp430_runtime_LoggerCallback_CallbackFxn CT__ti_mcu_msp430_runtime_LoggerCallback_callbackFxn;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_LoggerCallback_callbackFxn ti_mcu_msp430_runtime_LoggerCallback_callbackFxn__C;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_mcu_msp430_runtime_LoggerCallback_Params {
    size_t __size;
    const void* __self;
    void* __fxns;
    xdc_runtime_IInstance_Params* instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_mcu_msp430_runtime_LoggerCallback_Struct {
    const ti_mcu_msp430_runtime_LoggerCallback_Fxns__* __fxns;
    xdc_Bool __f0;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_mcu_msp430_runtime_LoggerCallback_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*enable)(ti_mcu_msp430_runtime_LoggerCallback_Handle);
    xdc_Bool (*disable)(ti_mcu_msp430_runtime_LoggerCallback_Handle);
    xdc_Void (*write0)(ti_mcu_msp430_runtime_LoggerCallback_Handle, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId);
    xdc_Void (*write1)(ti_mcu_msp430_runtime_LoggerCallback_Handle, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg);
    xdc_Void (*write2)(ti_mcu_msp430_runtime_LoggerCallback_Handle, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg);
    xdc_Void (*write4)(ti_mcu_msp430_runtime_LoggerCallback_Handle, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg);
    xdc_Void (*write8)(ti_mcu_msp430_runtime_LoggerCallback_Handle, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_mcu_msp430_runtime_LoggerCallback_Fxns__ ti_mcu_msp430_runtime_LoggerCallback_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_LoggerCallback_Module_startup( state ) (-1)

/* Instance_init__F */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Instance_init__F, "ti_mcu_msp430_runtime_LoggerCallback_Instance_init")
__extern void ti_mcu_msp430_runtime_LoggerCallback_Instance_init__F( ti_mcu_msp430_runtime_LoggerCallback_Object*, const ti_mcu_msp430_runtime_LoggerCallback_Params* );

/* Instance_init__R */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Instance_init__R, "ti_mcu_msp430_runtime_LoggerCallback_Instance_init")
__extern void ti_mcu_msp430_runtime_LoggerCallback_Instance_init__R( ti_mcu_msp430_runtime_LoggerCallback_Object*, const ti_mcu_msp430_runtime_LoggerCallback_Params* );

/* Handle__label__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Handle__label__S, "ti_mcu_msp430_runtime_LoggerCallback_Handle__label")
__extern xdc_runtime_Types_Label* ti_mcu_msp430_runtime_LoggerCallback_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Module__startupDone__S, "ti_mcu_msp430_runtime_LoggerCallback_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__create__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__create")
__extern xdc_Ptr ti_mcu_msp430_runtime_LoggerCallback_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar* __pa, xdc_SizeT __psz, xdc_runtime_Error_Block* __eb );

/* Object__delete__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__delete__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__delete")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_Object__delete__S( xdc_Ptr instp );

/* Object__destruct__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__destruct__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__destruct")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_Object__destruct__S( xdc_Ptr objp );

/* Object__get__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__get__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__get")
__extern xdc_Ptr ti_mcu_msp430_runtime_LoggerCallback_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__first__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__first")
__extern xdc_Ptr ti_mcu_msp430_runtime_LoggerCallback_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Object__next__S, "ti_mcu_msp430_runtime_LoggerCallback_Object__next")
__extern xdc_Ptr ti_mcu_msp430_runtime_LoggerCallback_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_Params__init__S, "ti_mcu_msp430_runtime_LoggerCallback_Params__init")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_Params__init__S( xdc_Ptr dst, const xdc_Void* src, xdc_SizeT psz, xdc_SizeT isz );

/* enable__E */
#define ti_mcu_msp430_runtime_LoggerCallback_enable ti_mcu_msp430_runtime_LoggerCallback_enable__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_enable__E, "ti_mcu_msp430_runtime_LoggerCallback_enable")
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_enable__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_enable__F, "ti_mcu_msp430_runtime_LoggerCallback_enable")
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_enable__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst );
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_enable__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst );

/* disable__E */
#define ti_mcu_msp430_runtime_LoggerCallback_disable ti_mcu_msp430_runtime_LoggerCallback_disable__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_disable__E, "ti_mcu_msp430_runtime_LoggerCallback_disable")
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_disable__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_disable__F, "ti_mcu_msp430_runtime_LoggerCallback_disable")
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_disable__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst );
__extern xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_disable__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst );

/* write0__E */
#define ti_mcu_msp430_runtime_LoggerCallback_write0 ti_mcu_msp430_runtime_LoggerCallback_write0__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write0__E, "ti_mcu_msp430_runtime_LoggerCallback_write0")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write0__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write0__F, "ti_mcu_msp430_runtime_LoggerCallback_write0")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write0__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid );
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write0__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid );

/* write1__E */
#define ti_mcu_msp430_runtime_LoggerCallback_write1 ti_mcu_msp430_runtime_LoggerCallback_write1__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write1__E, "ti_mcu_msp430_runtime_LoggerCallback_write1")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write1__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1 );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write1__F, "ti_mcu_msp430_runtime_LoggerCallback_write1")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write1__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1 );
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write1__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1 );

/* write2__E */
#define ti_mcu_msp430_runtime_LoggerCallback_write2 ti_mcu_msp430_runtime_LoggerCallback_write2__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write2__E, "ti_mcu_msp430_runtime_LoggerCallback_write2")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write2__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2 );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write2__F, "ti_mcu_msp430_runtime_LoggerCallback_write2")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write2__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2 );
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write2__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2 );

/* write4__E */
#define ti_mcu_msp430_runtime_LoggerCallback_write4 ti_mcu_msp430_runtime_LoggerCallback_write4__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write4__E, "ti_mcu_msp430_runtime_LoggerCallback_write4")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write4__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4 );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write4__F, "ti_mcu_msp430_runtime_LoggerCallback_write4")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write4__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4 );
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write4__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4 );

/* write8__E */
#define ti_mcu_msp430_runtime_LoggerCallback_write8 ti_mcu_msp430_runtime_LoggerCallback_write8__E
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write8__E, "ti_mcu_msp430_runtime_LoggerCallback_write8")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write8__E( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8 );
xdc__CODESECT(ti_mcu_msp430_runtime_LoggerCallback_write8__F, "ti_mcu_msp430_runtime_LoggerCallback_write8")
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write8__F( ti_mcu_msp430_runtime_LoggerCallback_Object* __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8 );
__extern xdc_Void ti_mcu_msp430_runtime_LoggerCallback_write8__R( ti_mcu_msp430_runtime_LoggerCallback_Handle __inst, xdc_runtime_Log_Event evt, xdc_runtime_Types_ModuleId mid, xdc_IArg a1, xdc_IArg a2, xdc_IArg a3, xdc_IArg a4, xdc_IArg a5, xdc_IArg a6, xdc_IArg a7, xdc_IArg a8 );


/*
 * ======== FUNCTION SELECTORS ========
 */

/* enable_{FxnT,fxnP} */
typedef xdc_Bool (*ti_mcu_msp430_runtime_LoggerCallback_enable_FxnT)(void*);
static inline ti_mcu_msp430_runtime_LoggerCallback_enable_FxnT ti_mcu_msp430_runtime_LoggerCallback_enable_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_enable_FxnT)ti_mcu_msp430_runtime_LoggerCallback_enable; 
}

/* disable_{FxnT,fxnP} */
typedef xdc_Bool (*ti_mcu_msp430_runtime_LoggerCallback_disable_FxnT)(void*);
static inline ti_mcu_msp430_runtime_LoggerCallback_disable_FxnT ti_mcu_msp430_runtime_LoggerCallback_disable_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_disable_FxnT)ti_mcu_msp430_runtime_LoggerCallback_disable; 
}

/* write0_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_write0_FxnT)(void*, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId);
static inline ti_mcu_msp430_runtime_LoggerCallback_write0_FxnT ti_mcu_msp430_runtime_LoggerCallback_write0_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_write0_FxnT)ti_mcu_msp430_runtime_LoggerCallback_write0; 
}

/* write1_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_write1_FxnT)(void*, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg);
static inline ti_mcu_msp430_runtime_LoggerCallback_write1_FxnT ti_mcu_msp430_runtime_LoggerCallback_write1_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_write1_FxnT)ti_mcu_msp430_runtime_LoggerCallback_write1; 
}

/* write2_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_write2_FxnT)(void*, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg);
static inline ti_mcu_msp430_runtime_LoggerCallback_write2_FxnT ti_mcu_msp430_runtime_LoggerCallback_write2_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_write2_FxnT)ti_mcu_msp430_runtime_LoggerCallback_write2; 
}

/* write4_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_write4_FxnT)(void*, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg);
static inline ti_mcu_msp430_runtime_LoggerCallback_write4_FxnT ti_mcu_msp430_runtime_LoggerCallback_write4_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_write4_FxnT)ti_mcu_msp430_runtime_LoggerCallback_write4; 
}

/* write8_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_LoggerCallback_write8_FxnT)(void*, xdc_runtime_Log_Event, xdc_runtime_Types_ModuleId, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg, xdc_IArg);
static inline ti_mcu_msp430_runtime_LoggerCallback_write8_FxnT ti_mcu_msp430_runtime_LoggerCallback_write8_fxnP( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_write8_FxnT)ti_mcu_msp430_runtime_LoggerCallback_write8; 
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_ILogger_Module ti_mcu_msp430_runtime_LoggerCallback_Module_upCast( void )
{
    return (xdc_runtime_ILogger_Module)&ti_mcu_msp430_runtime_LoggerCallback_Module__FXNS__C;
}

/* Module_to_xdc_runtime_ILogger */
#define ti_mcu_msp430_runtime_LoggerCallback_Module_to_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_ILogger_Handle ti_mcu_msp430_runtime_LoggerCallback_Handle_upCast( ti_mcu_msp430_runtime_LoggerCallback_Handle i )
{
    return (xdc_runtime_ILogger_Handle)i;
}

/* Handle_to_xdc_runtime_ILogger */
#define ti_mcu_msp430_runtime_LoggerCallback_Handle_to_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Handle_upCast

/* Handle_downCast */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_Handle_downCast( xdc_runtime_ILogger_Handle i )
{
    xdc_runtime_ILogger_Handle i2 = (xdc_runtime_ILogger_Handle)i;
    return (void*)i2->__fxns == (void*)&ti_mcu_msp430_runtime_LoggerCallback_Module__FXNS__C ? (ti_mcu_msp430_runtime_LoggerCallback_Handle)i : 0;
}

/* Handle_from_xdc_runtime_ILogger */
#define ti_mcu_msp430_runtime_LoggerCallback_Handle_from_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_LoggerCallback_Module_startupDone() ti_mcu_msp430_runtime_LoggerCallback_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_LoggerCallback_Object_heap() ti_mcu_msp430_runtime_LoggerCallback_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_LoggerCallback_Module_heap() ti_mcu_msp430_runtime_LoggerCallback_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_LoggerCallback_Module__id ti_mcu_msp430_runtime_LoggerCallback_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_LoggerCallback_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_LoggerCallback_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_LoggerCallback_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_LoggerCallback_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_LoggerCallback_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_mcu_msp430_runtime_LoggerCallback_Params_init( ti_mcu_msp430_runtime_LoggerCallback_Params* prms ) 
{
    if (prms) {
        ti_mcu_msp430_runtime_LoggerCallback_Params__init__S(prms, 0, sizeof(ti_mcu_msp430_runtime_LoggerCallback_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_mcu_msp430_runtime_LoggerCallback_Params_copy( ti_mcu_msp430_runtime_LoggerCallback_Params* dst, const ti_mcu_msp430_runtime_LoggerCallback_Params* src ) 
{
    if (dst) {
        ti_mcu_msp430_runtime_LoggerCallback_Params__init__S(dst, (const void *)src, sizeof(ti_mcu_msp430_runtime_LoggerCallback_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_mcu_msp430_runtime_LoggerCallback_Object_count() ti_mcu_msp430_runtime_LoggerCallback_Object__count__C

/* Object_sizeof */
#define ti_mcu_msp430_runtime_LoggerCallback_Object_sizeof() ti_mcu_msp430_runtime_LoggerCallback_Object__sizeof__C

/* Object_get */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_Object_get( ti_mcu_msp430_runtime_LoggerCallback_Instance_State* oarr, int i ) 
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Handle)ti_mcu_msp430_runtime_LoggerCallback_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_Object_first( void )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Handle)ti_mcu_msp430_runtime_LoggerCallback_Object__first__S();
}

/* Object_next */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_Object_next( ti_mcu_msp430_runtime_LoggerCallback_Object* obj )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Handle)ti_mcu_msp430_runtime_LoggerCallback_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label* ti_mcu_msp430_runtime_LoggerCallback_Handle_label( ti_mcu_msp430_runtime_LoggerCallback_Handle inst, xdc_runtime_Types_Label* lab )
{
    return ti_mcu_msp430_runtime_LoggerCallback_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_mcu_msp430_runtime_LoggerCallback_Handle_name( ti_mcu_msp430_runtime_LoggerCallback_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_mcu_msp430_runtime_LoggerCallback_Handle__label__S(inst, &lab)->iname;
}

/* create */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_create( const ti_mcu_msp430_runtime_LoggerCallback_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Handle)ti_mcu_msp430_runtime_LoggerCallback_Object__create__S(0, 0, 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_LoggerCallback_Params), __eb);
}

/* construct */
static inline void ti_mcu_msp430_runtime_LoggerCallback_construct( ti_mcu_msp430_runtime_LoggerCallback_Struct* __obj, const ti_mcu_msp430_runtime_LoggerCallback_Params* __prms )
{
    ti_mcu_msp430_runtime_LoggerCallback_Object__create__S(__obj, sizeof (ti_mcu_msp430_runtime_LoggerCallback_Struct), 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_LoggerCallback_Params), NULL);
}

/* delete */
static inline void ti_mcu_msp430_runtime_LoggerCallback_delete( ti_mcu_msp430_runtime_LoggerCallback_Handle* instp )
{
    ti_mcu_msp430_runtime_LoggerCallback_Object__delete__S(instp);
}

/* destruct */
static inline void ti_mcu_msp430_runtime_LoggerCallback_destruct( ti_mcu_msp430_runtime_LoggerCallback_Struct* obj )
{
    ti_mcu_msp430_runtime_LoggerCallback_Object__destruct__S(obj);
}

/* handle */
static inline ti_mcu_msp430_runtime_LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_handle( ti_mcu_msp430_runtime_LoggerCallback_Struct* str )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Handle)str;
}

/* struct */
static inline ti_mcu_msp430_runtime_LoggerCallback_Struct* ti_mcu_msp430_runtime_LoggerCallback_struct( ti_mcu_msp430_runtime_LoggerCallback_Handle inst )
{
    return (ti_mcu_msp430_runtime_LoggerCallback_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_LoggerCallback__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_LoggerCallback__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_LoggerCallback__internalaccess))

#ifndef ti_mcu_msp430_runtime_LoggerCallback__include_state
#define ti_mcu_msp430_runtime_LoggerCallback__include_state

/* Object */
struct ti_mcu_msp430_runtime_LoggerCallback_Object {
    const ti_mcu_msp430_runtime_LoggerCallback_Fxns__* __fxns;
    xdc_Bool enabled;
};

#endif /* ti_mcu_msp430_runtime_LoggerCallback__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_LoggerCallback__nolocalnames)

#ifndef ti_mcu_msp430_runtime_LoggerCallback__localnames__done
#define ti_mcu_msp430_runtime_LoggerCallback__localnames__done

/* module prefix */
#define LoggerCallback_Instance ti_mcu_msp430_runtime_LoggerCallback_Instance
#define LoggerCallback_Handle ti_mcu_msp430_runtime_LoggerCallback_Handle
#define LoggerCallback_Module ti_mcu_msp430_runtime_LoggerCallback_Module
#define LoggerCallback_Object ti_mcu_msp430_runtime_LoggerCallback_Object
#define LoggerCallback_Struct ti_mcu_msp430_runtime_LoggerCallback_Struct
#define LoggerCallback_CallbackFxn ti_mcu_msp430_runtime_LoggerCallback_CallbackFxn
#define LoggerCallback_Instance_State ti_mcu_msp430_runtime_LoggerCallback_Instance_State
#define LoggerCallback_callbackFxn ti_mcu_msp430_runtime_LoggerCallback_callbackFxn
#define LoggerCallback_Params ti_mcu_msp430_runtime_LoggerCallback_Params
#define LoggerCallback_enable ti_mcu_msp430_runtime_LoggerCallback_enable
#define LoggerCallback_enable_fxnP ti_mcu_msp430_runtime_LoggerCallback_enable_fxnP
#define LoggerCallback_enable_FxnT ti_mcu_msp430_runtime_LoggerCallback_enable_FxnT
#define LoggerCallback_disable ti_mcu_msp430_runtime_LoggerCallback_disable
#define LoggerCallback_disable_fxnP ti_mcu_msp430_runtime_LoggerCallback_disable_fxnP
#define LoggerCallback_disable_FxnT ti_mcu_msp430_runtime_LoggerCallback_disable_FxnT
#define LoggerCallback_write0 ti_mcu_msp430_runtime_LoggerCallback_write0
#define LoggerCallback_write0_fxnP ti_mcu_msp430_runtime_LoggerCallback_write0_fxnP
#define LoggerCallback_write0_FxnT ti_mcu_msp430_runtime_LoggerCallback_write0_FxnT
#define LoggerCallback_write1 ti_mcu_msp430_runtime_LoggerCallback_write1
#define LoggerCallback_write1_fxnP ti_mcu_msp430_runtime_LoggerCallback_write1_fxnP
#define LoggerCallback_write1_FxnT ti_mcu_msp430_runtime_LoggerCallback_write1_FxnT
#define LoggerCallback_write2 ti_mcu_msp430_runtime_LoggerCallback_write2
#define LoggerCallback_write2_fxnP ti_mcu_msp430_runtime_LoggerCallback_write2_fxnP
#define LoggerCallback_write2_FxnT ti_mcu_msp430_runtime_LoggerCallback_write2_FxnT
#define LoggerCallback_write4 ti_mcu_msp430_runtime_LoggerCallback_write4
#define LoggerCallback_write4_fxnP ti_mcu_msp430_runtime_LoggerCallback_write4_fxnP
#define LoggerCallback_write4_FxnT ti_mcu_msp430_runtime_LoggerCallback_write4_FxnT
#define LoggerCallback_write8 ti_mcu_msp430_runtime_LoggerCallback_write8
#define LoggerCallback_write8_fxnP ti_mcu_msp430_runtime_LoggerCallback_write8_fxnP
#define LoggerCallback_write8_FxnT ti_mcu_msp430_runtime_LoggerCallback_write8_FxnT
#define LoggerCallback_Module_name ti_mcu_msp430_runtime_LoggerCallback_Module_name
#define LoggerCallback_Module_id ti_mcu_msp430_runtime_LoggerCallback_Module_id
#define LoggerCallback_Module_startup ti_mcu_msp430_runtime_LoggerCallback_Module_startup
#define LoggerCallback_Module_startupDone ti_mcu_msp430_runtime_LoggerCallback_Module_startupDone
#define LoggerCallback_Module_hasMask ti_mcu_msp430_runtime_LoggerCallback_Module_hasMask
#define LoggerCallback_Module_getMask ti_mcu_msp430_runtime_LoggerCallback_Module_getMask
#define LoggerCallback_Module_setMask ti_mcu_msp430_runtime_LoggerCallback_Module_setMask
#define LoggerCallback_Object_heap ti_mcu_msp430_runtime_LoggerCallback_Object_heap
#define LoggerCallback_Module_heap ti_mcu_msp430_runtime_LoggerCallback_Module_heap
#define LoggerCallback_construct ti_mcu_msp430_runtime_LoggerCallback_construct
#define LoggerCallback_create ti_mcu_msp430_runtime_LoggerCallback_create
#define LoggerCallback_handle ti_mcu_msp430_runtime_LoggerCallback_handle
#define LoggerCallback_struct ti_mcu_msp430_runtime_LoggerCallback_struct
#define LoggerCallback_Handle_label ti_mcu_msp430_runtime_LoggerCallback_Handle_label
#define LoggerCallback_Handle_name ti_mcu_msp430_runtime_LoggerCallback_Handle_name
#define LoggerCallback_Instance_init ti_mcu_msp430_runtime_LoggerCallback_Instance_init
#define LoggerCallback_Object_count ti_mcu_msp430_runtime_LoggerCallback_Object_count
#define LoggerCallback_Object_get ti_mcu_msp430_runtime_LoggerCallback_Object_get
#define LoggerCallback_Object_first ti_mcu_msp430_runtime_LoggerCallback_Object_first
#define LoggerCallback_Object_next ti_mcu_msp430_runtime_LoggerCallback_Object_next
#define LoggerCallback_Object_sizeof ti_mcu_msp430_runtime_LoggerCallback_Object_sizeof
#define LoggerCallback_Params_copy ti_mcu_msp430_runtime_LoggerCallback_Params_copy
#define LoggerCallback_Params_init ti_mcu_msp430_runtime_LoggerCallback_Params_init
#define LoggerCallback_delete ti_mcu_msp430_runtime_LoggerCallback_delete
#define LoggerCallback_destruct ti_mcu_msp430_runtime_LoggerCallback_destruct
#define LoggerCallback_Module_upCast ti_mcu_msp430_runtime_LoggerCallback_Module_upCast
#define LoggerCallback_Module_to_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Module_to_xdc_runtime_ILogger
#define LoggerCallback_Handle_upCast ti_mcu_msp430_runtime_LoggerCallback_Handle_upCast
#define LoggerCallback_Handle_to_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Handle_to_xdc_runtime_ILogger
#define LoggerCallback_Handle_downCast ti_mcu_msp430_runtime_LoggerCallback_Handle_downCast
#define LoggerCallback_Handle_from_xdc_runtime_ILogger ti_mcu_msp430_runtime_LoggerCallback_Handle_from_xdc_runtime_ILogger

/* proxies */
#include <ti/mcu/msp430/runtime/package/LoggerCallback_TimestampProxy.h>

#endif /* ti_mcu_msp430_runtime_LoggerCallback__localnames__done */
#endif
