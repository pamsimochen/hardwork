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

#ifndef ti_mcu_msp430_runtime_GateGIE__include
#define ti_mcu_msp430_runtime_GateGIE__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_GateGIE__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_GateGIE___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/mcu/msp430/runtime/package/package.defs.h>

#include <xdc/runtime/IGateProvider.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Q_BLOCKING */
#define ti_mcu_msp430_runtime_GateGIE_Q_BLOCKING (1)

/* Q_PREEMPTING */
#define ti_mcu_msp430_runtime_GateGIE_Q_PREEMPTING (2)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsEnabled ti_mcu_msp430_runtime_GateGIE_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsIncluded ti_mcu_msp430_runtime_GateGIE_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__diagsMask ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_GateGIE_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__gateObj ti_mcu_msp430_runtime_GateGIE_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_GateGIE_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__gatePrms ti_mcu_msp430_runtime_GateGIE_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_GateGIE_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__id ti_mcu_msp430_runtime_GateGIE_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerDefined ti_mcu_msp430_runtime_GateGIE_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerObj ti_mcu_msp430_runtime_GateGIE_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn0 ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn1 ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn2 ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn4 ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn8 ti_mcu_msp430_runtime_GateGIE_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_GateGIE_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Module__startupDoneFxn ti_mcu_msp430_runtime_GateGIE_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_GateGIE_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Object__count ti_mcu_msp430_runtime_GateGIE_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_GateGIE_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Object__heap ti_mcu_msp430_runtime_GateGIE_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_GateGIE_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Object__sizeof ti_mcu_msp430_runtime_GateGIE_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_GateGIE_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_GateGIE_Object__table ti_mcu_msp430_runtime_GateGIE_Object__table__C;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_mcu_msp430_runtime_GateGIE_Params {
    size_t __size;
    const void* __self;
    void* __fxns;
    xdc_runtime_IInstance_Params* instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_mcu_msp430_runtime_GateGIE_Struct {
    const ti_mcu_msp430_runtime_GateGIE_Fxns__* __fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_mcu_msp430_runtime_GateGIE_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*query)(xdc_Int);
    xdc_IArg (*enter)(ti_mcu_msp430_runtime_GateGIE_Handle);
    xdc_Void (*leave)(ti_mcu_msp430_runtime_GateGIE_Handle, xdc_IArg);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_mcu_msp430_runtime_GateGIE_Fxns__ ti_mcu_msp430_runtime_GateGIE_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_GateGIE_Module_startup( state ) (-1)

/* Instance_init__F */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Instance_init__F, "ti_mcu_msp430_runtime_GateGIE_Instance_init")
__extern void ti_mcu_msp430_runtime_GateGIE_Instance_init__F( ti_mcu_msp430_runtime_GateGIE_Object*, const ti_mcu_msp430_runtime_GateGIE_Params* );

/* Instance_init__R */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Instance_init__R, "ti_mcu_msp430_runtime_GateGIE_Instance_init")
__extern void ti_mcu_msp430_runtime_GateGIE_Instance_init__R( ti_mcu_msp430_runtime_GateGIE_Object*, const ti_mcu_msp430_runtime_GateGIE_Params* );

/* Handle__label__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Handle__label__S, "ti_mcu_msp430_runtime_GateGIE_Handle__label")
__extern xdc_runtime_Types_Label* ti_mcu_msp430_runtime_GateGIE_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Module__startupDone__S, "ti_mcu_msp430_runtime_GateGIE_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_GateGIE_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__create__S, "ti_mcu_msp430_runtime_GateGIE_Object__create")
__extern xdc_Ptr ti_mcu_msp430_runtime_GateGIE_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar* __pa, xdc_SizeT __psz, xdc_runtime_Error_Block* __eb );

/* Object__delete__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__delete__S, "ti_mcu_msp430_runtime_GateGIE_Object__delete")
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_Object__delete__S( xdc_Ptr instp );

/* Object__destruct__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__destruct__S, "ti_mcu_msp430_runtime_GateGIE_Object__destruct")
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_Object__destruct__S( xdc_Ptr objp );

/* Object__get__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__get__S, "ti_mcu_msp430_runtime_GateGIE_Object__get")
__extern xdc_Ptr ti_mcu_msp430_runtime_GateGIE_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__first__S, "ti_mcu_msp430_runtime_GateGIE_Object__first")
__extern xdc_Ptr ti_mcu_msp430_runtime_GateGIE_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Object__next__S, "ti_mcu_msp430_runtime_GateGIE_Object__next")
__extern xdc_Ptr ti_mcu_msp430_runtime_GateGIE_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_Params__init__S, "ti_mcu_msp430_runtime_GateGIE_Params__init")
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_Params__init__S( xdc_Ptr dst, const xdc_Void* src, xdc_SizeT psz, xdc_SizeT isz );

/* query__E */
#define ti_mcu_msp430_runtime_GateGIE_query ti_mcu_msp430_runtime_GateGIE_query__E
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_query__E, "ti_mcu_msp430_runtime_GateGIE_query")
__extern xdc_Bool ti_mcu_msp430_runtime_GateGIE_query__E( xdc_Int qual );
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_query__F, "ti_mcu_msp430_runtime_GateGIE_query")
__extern xdc_Bool ti_mcu_msp430_runtime_GateGIE_query__F( xdc_Int qual );
__extern xdc_Bool ti_mcu_msp430_runtime_GateGIE_query__R( xdc_Int qual );

/* enter__E */
#define ti_mcu_msp430_runtime_GateGIE_enter ti_mcu_msp430_runtime_GateGIE_enter__E
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_enter__E, "ti_mcu_msp430_runtime_GateGIE_enter")
__extern xdc_IArg ti_mcu_msp430_runtime_GateGIE_enter__E( ti_mcu_msp430_runtime_GateGIE_Handle __inst );
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_enter__F, "ti_mcu_msp430_runtime_GateGIE_enter")
__extern xdc_IArg ti_mcu_msp430_runtime_GateGIE_enter__F( ti_mcu_msp430_runtime_GateGIE_Object* __inst );
__extern xdc_IArg ti_mcu_msp430_runtime_GateGIE_enter__R( ti_mcu_msp430_runtime_GateGIE_Handle __inst );

/* leave__E */
#define ti_mcu_msp430_runtime_GateGIE_leave ti_mcu_msp430_runtime_GateGIE_leave__E
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_leave__E, "ti_mcu_msp430_runtime_GateGIE_leave")
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_leave__E( ti_mcu_msp430_runtime_GateGIE_Handle __inst, xdc_IArg key );
xdc__CODESECT(ti_mcu_msp430_runtime_GateGIE_leave__F, "ti_mcu_msp430_runtime_GateGIE_leave")
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_leave__F( ti_mcu_msp430_runtime_GateGIE_Object* __inst, xdc_IArg key );
__extern xdc_Void ti_mcu_msp430_runtime_GateGIE_leave__R( ti_mcu_msp430_runtime_GateGIE_Handle __inst, xdc_IArg key );


/*
 * ======== FUNCTION SELECTORS ========
 */

/* enter_{FxnT,fxnP} */
typedef xdc_IArg (*ti_mcu_msp430_runtime_GateGIE_enter_FxnT)(void*);
static inline ti_mcu_msp430_runtime_GateGIE_enter_FxnT ti_mcu_msp430_runtime_GateGIE_enter_fxnP( void )
{
    return (ti_mcu_msp430_runtime_GateGIE_enter_FxnT)ti_mcu_msp430_runtime_GateGIE_enter; 
}

/* leave_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_GateGIE_leave_FxnT)(void*, xdc_IArg);
static inline ti_mcu_msp430_runtime_GateGIE_leave_FxnT ti_mcu_msp430_runtime_GateGIE_leave_fxnP( void )
{
    return (ti_mcu_msp430_runtime_GateGIE_leave_FxnT)ti_mcu_msp430_runtime_GateGIE_leave; 
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IGateProvider_Module ti_mcu_msp430_runtime_GateGIE_Module_upCast( void )
{
    return (xdc_runtime_IGateProvider_Module)&ti_mcu_msp430_runtime_GateGIE_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IGateProvider */
#define ti_mcu_msp430_runtime_GateGIE_Module_to_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IGateProvider_Handle ti_mcu_msp430_runtime_GateGIE_Handle_upCast( ti_mcu_msp430_runtime_GateGIE_Handle i )
{
    return (xdc_runtime_IGateProvider_Handle)i;
}

/* Handle_to_xdc_runtime_IGateProvider */
#define ti_mcu_msp430_runtime_GateGIE_Handle_to_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Handle_upCast

/* Handle_downCast */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_Handle_downCast( xdc_runtime_IGateProvider_Handle i )
{
    xdc_runtime_IGateProvider_Handle i2 = (xdc_runtime_IGateProvider_Handle)i;
    return (void*)i2->__fxns == (void*)&ti_mcu_msp430_runtime_GateGIE_Module__FXNS__C ? (ti_mcu_msp430_runtime_GateGIE_Handle)i : 0;
}

/* Handle_from_xdc_runtime_IGateProvider */
#define ti_mcu_msp430_runtime_GateGIE_Handle_from_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_GateGIE_Module_startupDone() ti_mcu_msp430_runtime_GateGIE_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_GateGIE_Object_heap() ti_mcu_msp430_runtime_GateGIE_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_GateGIE_Module_heap() ti_mcu_msp430_runtime_GateGIE_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_GateGIE_Module__id ti_mcu_msp430_runtime_GateGIE_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_GateGIE_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_GateGIE_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_GateGIE_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_GateGIE_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_GateGIE_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_mcu_msp430_runtime_GateGIE_Params_init( ti_mcu_msp430_runtime_GateGIE_Params* prms ) 
{
    if (prms) {
        ti_mcu_msp430_runtime_GateGIE_Params__init__S(prms, 0, sizeof(ti_mcu_msp430_runtime_GateGIE_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_mcu_msp430_runtime_GateGIE_Params_copy( ti_mcu_msp430_runtime_GateGIE_Params* dst, const ti_mcu_msp430_runtime_GateGIE_Params* src ) 
{
    if (dst) {
        ti_mcu_msp430_runtime_GateGIE_Params__init__S(dst, (const void *)src, sizeof(ti_mcu_msp430_runtime_GateGIE_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_mcu_msp430_runtime_GateGIE_Object_count() ti_mcu_msp430_runtime_GateGIE_Object__count__C

/* Object_sizeof */
#define ti_mcu_msp430_runtime_GateGIE_Object_sizeof() ti_mcu_msp430_runtime_GateGIE_Object__sizeof__C

/* Object_get */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_Object_get( ti_mcu_msp430_runtime_GateGIE_Instance_State* oarr, int i ) 
{
    return (ti_mcu_msp430_runtime_GateGIE_Handle)ti_mcu_msp430_runtime_GateGIE_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_Object_first( void )
{
    return (ti_mcu_msp430_runtime_GateGIE_Handle)ti_mcu_msp430_runtime_GateGIE_Object__first__S();
}

/* Object_next */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_Object_next( ti_mcu_msp430_runtime_GateGIE_Object* obj )
{
    return (ti_mcu_msp430_runtime_GateGIE_Handle)ti_mcu_msp430_runtime_GateGIE_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label* ti_mcu_msp430_runtime_GateGIE_Handle_label( ti_mcu_msp430_runtime_GateGIE_Handle inst, xdc_runtime_Types_Label* lab )
{
    return ti_mcu_msp430_runtime_GateGIE_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_mcu_msp430_runtime_GateGIE_Handle_name( ti_mcu_msp430_runtime_GateGIE_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_mcu_msp430_runtime_GateGIE_Handle__label__S(inst, &lab)->iname;
}

/* create */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_create( const ti_mcu_msp430_runtime_GateGIE_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    return (ti_mcu_msp430_runtime_GateGIE_Handle)ti_mcu_msp430_runtime_GateGIE_Object__create__S(0, 0, 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_GateGIE_Params), __eb);
}

/* construct */
static inline void ti_mcu_msp430_runtime_GateGIE_construct( ti_mcu_msp430_runtime_GateGIE_Struct* __obj, const ti_mcu_msp430_runtime_GateGIE_Params* __prms )
{
    ti_mcu_msp430_runtime_GateGIE_Object__create__S(__obj, sizeof (ti_mcu_msp430_runtime_GateGIE_Struct), 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_GateGIE_Params), NULL);
}

/* delete */
static inline void ti_mcu_msp430_runtime_GateGIE_delete( ti_mcu_msp430_runtime_GateGIE_Handle* instp )
{
    ti_mcu_msp430_runtime_GateGIE_Object__delete__S(instp);
}

/* destruct */
static inline void ti_mcu_msp430_runtime_GateGIE_destruct( ti_mcu_msp430_runtime_GateGIE_Struct* obj )
{
    ti_mcu_msp430_runtime_GateGIE_Object__destruct__S(obj);
}

/* handle */
static inline ti_mcu_msp430_runtime_GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_handle( ti_mcu_msp430_runtime_GateGIE_Struct* str )
{
    return (ti_mcu_msp430_runtime_GateGIE_Handle)str;
}

/* struct */
static inline ti_mcu_msp430_runtime_GateGIE_Struct* ti_mcu_msp430_runtime_GateGIE_struct( ti_mcu_msp430_runtime_GateGIE_Handle inst )
{
    return (ti_mcu_msp430_runtime_GateGIE_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_GateGIE__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_GateGIE__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_GateGIE__internalaccess))

#ifndef ti_mcu_msp430_runtime_GateGIE__include_state
#define ti_mcu_msp430_runtime_GateGIE__include_state

/* Object */
struct ti_mcu_msp430_runtime_GateGIE_Object {
    const ti_mcu_msp430_runtime_GateGIE_Fxns__* __fxns;
};

#endif /* ti_mcu_msp430_runtime_GateGIE__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_GateGIE__nolocalnames)

#ifndef ti_mcu_msp430_runtime_GateGIE__localnames__done
#define ti_mcu_msp430_runtime_GateGIE__localnames__done

/* module prefix */
#define GateGIE_Instance ti_mcu_msp430_runtime_GateGIE_Instance
#define GateGIE_Handle ti_mcu_msp430_runtime_GateGIE_Handle
#define GateGIE_Module ti_mcu_msp430_runtime_GateGIE_Module
#define GateGIE_Object ti_mcu_msp430_runtime_GateGIE_Object
#define GateGIE_Struct ti_mcu_msp430_runtime_GateGIE_Struct
#define GateGIE_Q_BLOCKING ti_mcu_msp430_runtime_GateGIE_Q_BLOCKING
#define GateGIE_Q_PREEMPTING ti_mcu_msp430_runtime_GateGIE_Q_PREEMPTING
#define GateGIE_Instance_State ti_mcu_msp430_runtime_GateGIE_Instance_State
#define GateGIE_Params ti_mcu_msp430_runtime_GateGIE_Params
#define GateGIE_query ti_mcu_msp430_runtime_GateGIE_query
#define GateGIE_enter ti_mcu_msp430_runtime_GateGIE_enter
#define GateGIE_enter_fxnP ti_mcu_msp430_runtime_GateGIE_enter_fxnP
#define GateGIE_enter_FxnT ti_mcu_msp430_runtime_GateGIE_enter_FxnT
#define GateGIE_leave ti_mcu_msp430_runtime_GateGIE_leave
#define GateGIE_leave_fxnP ti_mcu_msp430_runtime_GateGIE_leave_fxnP
#define GateGIE_leave_FxnT ti_mcu_msp430_runtime_GateGIE_leave_FxnT
#define GateGIE_Module_name ti_mcu_msp430_runtime_GateGIE_Module_name
#define GateGIE_Module_id ti_mcu_msp430_runtime_GateGIE_Module_id
#define GateGIE_Module_startup ti_mcu_msp430_runtime_GateGIE_Module_startup
#define GateGIE_Module_startupDone ti_mcu_msp430_runtime_GateGIE_Module_startupDone
#define GateGIE_Module_hasMask ti_mcu_msp430_runtime_GateGIE_Module_hasMask
#define GateGIE_Module_getMask ti_mcu_msp430_runtime_GateGIE_Module_getMask
#define GateGIE_Module_setMask ti_mcu_msp430_runtime_GateGIE_Module_setMask
#define GateGIE_Object_heap ti_mcu_msp430_runtime_GateGIE_Object_heap
#define GateGIE_Module_heap ti_mcu_msp430_runtime_GateGIE_Module_heap
#define GateGIE_construct ti_mcu_msp430_runtime_GateGIE_construct
#define GateGIE_create ti_mcu_msp430_runtime_GateGIE_create
#define GateGIE_handle ti_mcu_msp430_runtime_GateGIE_handle
#define GateGIE_struct ti_mcu_msp430_runtime_GateGIE_struct
#define GateGIE_Handle_label ti_mcu_msp430_runtime_GateGIE_Handle_label
#define GateGIE_Handle_name ti_mcu_msp430_runtime_GateGIE_Handle_name
#define GateGIE_Instance_init ti_mcu_msp430_runtime_GateGIE_Instance_init
#define GateGIE_Object_count ti_mcu_msp430_runtime_GateGIE_Object_count
#define GateGIE_Object_get ti_mcu_msp430_runtime_GateGIE_Object_get
#define GateGIE_Object_first ti_mcu_msp430_runtime_GateGIE_Object_first
#define GateGIE_Object_next ti_mcu_msp430_runtime_GateGIE_Object_next
#define GateGIE_Object_sizeof ti_mcu_msp430_runtime_GateGIE_Object_sizeof
#define GateGIE_Params_copy ti_mcu_msp430_runtime_GateGIE_Params_copy
#define GateGIE_Params_init ti_mcu_msp430_runtime_GateGIE_Params_init
#define GateGIE_delete ti_mcu_msp430_runtime_GateGIE_delete
#define GateGIE_destruct ti_mcu_msp430_runtime_GateGIE_destruct
#define GateGIE_Module_upCast ti_mcu_msp430_runtime_GateGIE_Module_upCast
#define GateGIE_Module_to_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Module_to_xdc_runtime_IGateProvider
#define GateGIE_Handle_upCast ti_mcu_msp430_runtime_GateGIE_Handle_upCast
#define GateGIE_Handle_to_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Handle_to_xdc_runtime_IGateProvider
#define GateGIE_Handle_downCast ti_mcu_msp430_runtime_GateGIE_Handle_downCast
#define GateGIE_Handle_from_xdc_runtime_IGateProvider ti_mcu_msp430_runtime_GateGIE_Handle_from_xdc_runtime_IGateProvider

#endif /* ti_mcu_msp430_runtime_GateGIE__localnames__done */
#endif
