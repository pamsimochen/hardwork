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

#ifndef ti_mcu_msp430_runtime_HeapNull__include
#define ti_mcu_msp430_runtime_HeapNull__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_HeapNull__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_HeapNull___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/mcu/msp430/runtime/package/package.defs.h>

#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Error.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsEnabled ti_mcu_msp430_runtime_HeapNull_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsIncluded ti_mcu_msp430_runtime_HeapNull_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__diagsMask ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_HeapNull_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__gateObj ti_mcu_msp430_runtime_HeapNull_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_HeapNull_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__gatePrms ti_mcu_msp430_runtime_HeapNull_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_HeapNull_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__id ti_mcu_msp430_runtime_HeapNull_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerDefined ti_mcu_msp430_runtime_HeapNull_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerObj ti_mcu_msp430_runtime_HeapNull_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn0 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn1 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn2 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn4 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn8 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_HeapNull_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Module__startupDoneFxn ti_mcu_msp430_runtime_HeapNull_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_HeapNull_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Object__count ti_mcu_msp430_runtime_HeapNull_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_HeapNull_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Object__heap ti_mcu_msp430_runtime_HeapNull_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_HeapNull_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Object__sizeof ti_mcu_msp430_runtime_HeapNull_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_HeapNull_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_HeapNull_Object__table ti_mcu_msp430_runtime_HeapNull_Object__table__C;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_mcu_msp430_runtime_HeapNull_Params {
    size_t __size;
    const void* __self;
    void* __fxns;
    xdc_runtime_IInstance_Params* instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_mcu_msp430_runtime_HeapNull_Struct {
    const ti_mcu_msp430_runtime_HeapNull_Fxns__* __fxns;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_mcu_msp430_runtime_HeapNull_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Ptr (*alloc)(ti_mcu_msp430_runtime_HeapNull_Handle, xdc_SizeT, xdc_SizeT, xdc_runtime_Error_Block*);
    xdc_Void (*free)(ti_mcu_msp430_runtime_HeapNull_Handle, xdc_Ptr, xdc_SizeT);
    xdc_Bool (*isBlocking)(ti_mcu_msp430_runtime_HeapNull_Handle);
    xdc_Void (*getStats)(ti_mcu_msp430_runtime_HeapNull_Handle, xdc_runtime_Memory_Stats*);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_mcu_msp430_runtime_HeapNull_Fxns__ ti_mcu_msp430_runtime_HeapNull_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_HeapNull_Module_startup( state ) (-1)

/* Instance_init__F */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Instance_init__F, "ti_mcu_msp430_runtime_HeapNull_Instance_init")
__extern void ti_mcu_msp430_runtime_HeapNull_Instance_init__F( ti_mcu_msp430_runtime_HeapNull_Object*, const ti_mcu_msp430_runtime_HeapNull_Params* );

/* Instance_init__R */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Instance_init__R, "ti_mcu_msp430_runtime_HeapNull_Instance_init")
__extern void ti_mcu_msp430_runtime_HeapNull_Instance_init__R( ti_mcu_msp430_runtime_HeapNull_Object*, const ti_mcu_msp430_runtime_HeapNull_Params* );

/* Handle__label__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Handle__label__S, "ti_mcu_msp430_runtime_HeapNull_Handle__label")
__extern xdc_runtime_Types_Label* ti_mcu_msp430_runtime_HeapNull_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Module__startupDone__S, "ti_mcu_msp430_runtime_HeapNull_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_HeapNull_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__create__S, "ti_mcu_msp430_runtime_HeapNull_Object__create")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar* __pa, xdc_SizeT __psz, xdc_runtime_Error_Block* __eb );

/* Object__delete__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__delete__S, "ti_mcu_msp430_runtime_HeapNull_Object__delete")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_Object__delete__S( xdc_Ptr instp );

/* Object__destruct__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__destruct__S, "ti_mcu_msp430_runtime_HeapNull_Object__destruct")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_Object__destruct__S( xdc_Ptr objp );

/* Object__get__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__get__S, "ti_mcu_msp430_runtime_HeapNull_Object__get")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__first__S, "ti_mcu_msp430_runtime_HeapNull_Object__first")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Object__next__S, "ti_mcu_msp430_runtime_HeapNull_Object__next")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_Params__init__S, "ti_mcu_msp430_runtime_HeapNull_Params__init")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_Params__init__S( xdc_Ptr dst, const xdc_Void* src, xdc_SizeT psz, xdc_SizeT isz );

/* getStats__E */
#define ti_mcu_msp430_runtime_HeapNull_getStats ti_mcu_msp430_runtime_HeapNull_getStats__E
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_getStats__E, "ti_mcu_msp430_runtime_HeapNull_getStats")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_getStats__E( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_runtime_Memory_Stats* stats );
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_getStats__F, "ti_mcu_msp430_runtime_HeapNull_getStats")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_getStats__F( ti_mcu_msp430_runtime_HeapNull_Object* __inst, xdc_runtime_Memory_Stats* stats );
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_getStats__R( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_runtime_Memory_Stats* stats );

/* alloc__E */
#define ti_mcu_msp430_runtime_HeapNull_alloc ti_mcu_msp430_runtime_HeapNull_alloc__E
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_alloc__E, "ti_mcu_msp430_runtime_HeapNull_alloc")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_alloc__E( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb );
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_alloc__F, "ti_mcu_msp430_runtime_HeapNull_alloc")
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_alloc__F( ti_mcu_msp430_runtime_HeapNull_Object* __inst, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb );
__extern xdc_Ptr ti_mcu_msp430_runtime_HeapNull_alloc__R( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb );

/* free__E */
#define ti_mcu_msp430_runtime_HeapNull_free ti_mcu_msp430_runtime_HeapNull_free__E
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_free__E, "ti_mcu_msp430_runtime_HeapNull_free")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_free__E( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_Ptr block, xdc_SizeT size );
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_free__F, "ti_mcu_msp430_runtime_HeapNull_free")
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_free__F( ti_mcu_msp430_runtime_HeapNull_Object* __inst, xdc_Ptr block, xdc_SizeT size );
__extern xdc_Void ti_mcu_msp430_runtime_HeapNull_free__R( ti_mcu_msp430_runtime_HeapNull_Handle __inst, xdc_Ptr block, xdc_SizeT size );

/* isBlocking__E */
#define ti_mcu_msp430_runtime_HeapNull_isBlocking ti_mcu_msp430_runtime_HeapNull_isBlocking__E
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_isBlocking__E, "ti_mcu_msp430_runtime_HeapNull_isBlocking")
__extern xdc_Bool ti_mcu_msp430_runtime_HeapNull_isBlocking__E( ti_mcu_msp430_runtime_HeapNull_Handle __inst );
xdc__CODESECT(ti_mcu_msp430_runtime_HeapNull_isBlocking__F, "ti_mcu_msp430_runtime_HeapNull_isBlocking")
__extern xdc_Bool ti_mcu_msp430_runtime_HeapNull_isBlocking__F( ti_mcu_msp430_runtime_HeapNull_Object* __inst );
__extern xdc_Bool ti_mcu_msp430_runtime_HeapNull_isBlocking__R( ti_mcu_msp430_runtime_HeapNull_Handle __inst );


/*
 * ======== FUNCTION SELECTORS ========
 */

/* getStats_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_HeapNull_getStats_FxnT)(void*, xdc_runtime_Memory_Stats*);
static inline ti_mcu_msp430_runtime_HeapNull_getStats_FxnT ti_mcu_msp430_runtime_HeapNull_getStats_fxnP( void )
{
    return (ti_mcu_msp430_runtime_HeapNull_getStats_FxnT)ti_mcu_msp430_runtime_HeapNull_getStats; 
}

/* alloc_{FxnT,fxnP} */
typedef xdc_Ptr (*ti_mcu_msp430_runtime_HeapNull_alloc_FxnT)(void*, xdc_SizeT, xdc_SizeT, xdc_runtime_Error_Block*);
static inline ti_mcu_msp430_runtime_HeapNull_alloc_FxnT ti_mcu_msp430_runtime_HeapNull_alloc_fxnP( void )
{
    return (ti_mcu_msp430_runtime_HeapNull_alloc_FxnT)ti_mcu_msp430_runtime_HeapNull_alloc; 
}

/* free_{FxnT,fxnP} */
typedef xdc_Void (*ti_mcu_msp430_runtime_HeapNull_free_FxnT)(void*, xdc_Ptr, xdc_SizeT);
static inline ti_mcu_msp430_runtime_HeapNull_free_FxnT ti_mcu_msp430_runtime_HeapNull_free_fxnP( void )
{
    return (ti_mcu_msp430_runtime_HeapNull_free_FxnT)ti_mcu_msp430_runtime_HeapNull_free; 
}

/* isBlocking_{FxnT,fxnP} */
typedef xdc_Bool (*ti_mcu_msp430_runtime_HeapNull_isBlocking_FxnT)(void*);
static inline ti_mcu_msp430_runtime_HeapNull_isBlocking_FxnT ti_mcu_msp430_runtime_HeapNull_isBlocking_fxnP( void )
{
    return (ti_mcu_msp430_runtime_HeapNull_isBlocking_FxnT)ti_mcu_msp430_runtime_HeapNull_isBlocking; 
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline xdc_runtime_IHeap_Module ti_mcu_msp430_runtime_HeapNull_Module_upCast( void )
{
    return (xdc_runtime_IHeap_Module)&ti_mcu_msp430_runtime_HeapNull_Module__FXNS__C;
}

/* Module_to_xdc_runtime_IHeap */
#define ti_mcu_msp430_runtime_HeapNull_Module_to_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Module_upCast

/* Handle_upCast */
static inline xdc_runtime_IHeap_Handle ti_mcu_msp430_runtime_HeapNull_Handle_upCast( ti_mcu_msp430_runtime_HeapNull_Handle i )
{
    return (xdc_runtime_IHeap_Handle)i;
}

/* Handle_to_xdc_runtime_IHeap */
#define ti_mcu_msp430_runtime_HeapNull_Handle_to_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Handle_upCast

/* Handle_downCast */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_Handle_downCast( xdc_runtime_IHeap_Handle i )
{
    xdc_runtime_IHeap_Handle i2 = (xdc_runtime_IHeap_Handle)i;
    return (void*)i2->__fxns == (void*)&ti_mcu_msp430_runtime_HeapNull_Module__FXNS__C ? (ti_mcu_msp430_runtime_HeapNull_Handle)i : 0;
}

/* Handle_from_xdc_runtime_IHeap */
#define ti_mcu_msp430_runtime_HeapNull_Handle_from_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_HeapNull_Module_startupDone() ti_mcu_msp430_runtime_HeapNull_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_HeapNull_Object_heap() ti_mcu_msp430_runtime_HeapNull_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_HeapNull_Module_heap() ti_mcu_msp430_runtime_HeapNull_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_HeapNull_Module__id ti_mcu_msp430_runtime_HeapNull_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_HeapNull_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_HeapNull_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_HeapNull_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_HeapNull_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_mcu_msp430_runtime_HeapNull_Params_init( ti_mcu_msp430_runtime_HeapNull_Params* prms ) 
{
    if (prms) {
        ti_mcu_msp430_runtime_HeapNull_Params__init__S(prms, 0, sizeof(ti_mcu_msp430_runtime_HeapNull_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_mcu_msp430_runtime_HeapNull_Params_copy( ti_mcu_msp430_runtime_HeapNull_Params* dst, const ti_mcu_msp430_runtime_HeapNull_Params* src ) 
{
    if (dst) {
        ti_mcu_msp430_runtime_HeapNull_Params__init__S(dst, (const void *)src, sizeof(ti_mcu_msp430_runtime_HeapNull_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_mcu_msp430_runtime_HeapNull_Object_count() ti_mcu_msp430_runtime_HeapNull_Object__count__C

/* Object_sizeof */
#define ti_mcu_msp430_runtime_HeapNull_Object_sizeof() ti_mcu_msp430_runtime_HeapNull_Object__sizeof__C

/* Object_get */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_Object_get( ti_mcu_msp430_runtime_HeapNull_Instance_State* oarr, int i ) 
{
    return (ti_mcu_msp430_runtime_HeapNull_Handle)ti_mcu_msp430_runtime_HeapNull_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_Object_first( void )
{
    return (ti_mcu_msp430_runtime_HeapNull_Handle)ti_mcu_msp430_runtime_HeapNull_Object__first__S();
}

/* Object_next */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_Object_next( ti_mcu_msp430_runtime_HeapNull_Object* obj )
{
    return (ti_mcu_msp430_runtime_HeapNull_Handle)ti_mcu_msp430_runtime_HeapNull_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label* ti_mcu_msp430_runtime_HeapNull_Handle_label( ti_mcu_msp430_runtime_HeapNull_Handle inst, xdc_runtime_Types_Label* lab )
{
    return ti_mcu_msp430_runtime_HeapNull_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_mcu_msp430_runtime_HeapNull_Handle_name( ti_mcu_msp430_runtime_HeapNull_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_mcu_msp430_runtime_HeapNull_Handle__label__S(inst, &lab)->iname;
}

/* create */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_create( const ti_mcu_msp430_runtime_HeapNull_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    return (ti_mcu_msp430_runtime_HeapNull_Handle)ti_mcu_msp430_runtime_HeapNull_Object__create__S(0, 0, 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_HeapNull_Params), __eb);
}

/* construct */
static inline void ti_mcu_msp430_runtime_HeapNull_construct( ti_mcu_msp430_runtime_HeapNull_Struct* __obj, const ti_mcu_msp430_runtime_HeapNull_Params* __prms )
{
    ti_mcu_msp430_runtime_HeapNull_Object__create__S(__obj, sizeof (ti_mcu_msp430_runtime_HeapNull_Struct), 0, (const xdc_UChar*)__prms, sizeof(ti_mcu_msp430_runtime_HeapNull_Params), NULL);
}

/* delete */
static inline void ti_mcu_msp430_runtime_HeapNull_delete( ti_mcu_msp430_runtime_HeapNull_Handle* instp )
{
    ti_mcu_msp430_runtime_HeapNull_Object__delete__S(instp);
}

/* destruct */
static inline void ti_mcu_msp430_runtime_HeapNull_destruct( ti_mcu_msp430_runtime_HeapNull_Struct* obj )
{
    ti_mcu_msp430_runtime_HeapNull_Object__destruct__S(obj);
}

/* handle */
static inline ti_mcu_msp430_runtime_HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_handle( ti_mcu_msp430_runtime_HeapNull_Struct* str )
{
    return (ti_mcu_msp430_runtime_HeapNull_Handle)str;
}

/* struct */
static inline ti_mcu_msp430_runtime_HeapNull_Struct* ti_mcu_msp430_runtime_HeapNull_struct( ti_mcu_msp430_runtime_HeapNull_Handle inst )
{
    return (ti_mcu_msp430_runtime_HeapNull_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_HeapNull__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_HeapNull__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_HeapNull__internalaccess))

#ifndef ti_mcu_msp430_runtime_HeapNull__include_state
#define ti_mcu_msp430_runtime_HeapNull__include_state

/* Object */
struct ti_mcu_msp430_runtime_HeapNull_Object {
    const ti_mcu_msp430_runtime_HeapNull_Fxns__* __fxns;
};

#endif /* ti_mcu_msp430_runtime_HeapNull__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_HeapNull__nolocalnames)

#ifndef ti_mcu_msp430_runtime_HeapNull__localnames__done
#define ti_mcu_msp430_runtime_HeapNull__localnames__done

/* module prefix */
#define HeapNull_Instance ti_mcu_msp430_runtime_HeapNull_Instance
#define HeapNull_Handle ti_mcu_msp430_runtime_HeapNull_Handle
#define HeapNull_Module ti_mcu_msp430_runtime_HeapNull_Module
#define HeapNull_Object ti_mcu_msp430_runtime_HeapNull_Object
#define HeapNull_Struct ti_mcu_msp430_runtime_HeapNull_Struct
#define HeapNull_Instance_State ti_mcu_msp430_runtime_HeapNull_Instance_State
#define HeapNull_Params ti_mcu_msp430_runtime_HeapNull_Params
#define HeapNull_getStats ti_mcu_msp430_runtime_HeapNull_getStats
#define HeapNull_getStats_fxnP ti_mcu_msp430_runtime_HeapNull_getStats_fxnP
#define HeapNull_getStats_FxnT ti_mcu_msp430_runtime_HeapNull_getStats_FxnT
#define HeapNull_alloc ti_mcu_msp430_runtime_HeapNull_alloc
#define HeapNull_alloc_fxnP ti_mcu_msp430_runtime_HeapNull_alloc_fxnP
#define HeapNull_alloc_FxnT ti_mcu_msp430_runtime_HeapNull_alloc_FxnT
#define HeapNull_free ti_mcu_msp430_runtime_HeapNull_free
#define HeapNull_free_fxnP ti_mcu_msp430_runtime_HeapNull_free_fxnP
#define HeapNull_free_FxnT ti_mcu_msp430_runtime_HeapNull_free_FxnT
#define HeapNull_isBlocking ti_mcu_msp430_runtime_HeapNull_isBlocking
#define HeapNull_isBlocking_fxnP ti_mcu_msp430_runtime_HeapNull_isBlocking_fxnP
#define HeapNull_isBlocking_FxnT ti_mcu_msp430_runtime_HeapNull_isBlocking_FxnT
#define HeapNull_Module_name ti_mcu_msp430_runtime_HeapNull_Module_name
#define HeapNull_Module_id ti_mcu_msp430_runtime_HeapNull_Module_id
#define HeapNull_Module_startup ti_mcu_msp430_runtime_HeapNull_Module_startup
#define HeapNull_Module_startupDone ti_mcu_msp430_runtime_HeapNull_Module_startupDone
#define HeapNull_Module_hasMask ti_mcu_msp430_runtime_HeapNull_Module_hasMask
#define HeapNull_Module_getMask ti_mcu_msp430_runtime_HeapNull_Module_getMask
#define HeapNull_Module_setMask ti_mcu_msp430_runtime_HeapNull_Module_setMask
#define HeapNull_Object_heap ti_mcu_msp430_runtime_HeapNull_Object_heap
#define HeapNull_Module_heap ti_mcu_msp430_runtime_HeapNull_Module_heap
#define HeapNull_construct ti_mcu_msp430_runtime_HeapNull_construct
#define HeapNull_create ti_mcu_msp430_runtime_HeapNull_create
#define HeapNull_handle ti_mcu_msp430_runtime_HeapNull_handle
#define HeapNull_struct ti_mcu_msp430_runtime_HeapNull_struct
#define HeapNull_Handle_label ti_mcu_msp430_runtime_HeapNull_Handle_label
#define HeapNull_Handle_name ti_mcu_msp430_runtime_HeapNull_Handle_name
#define HeapNull_Instance_init ti_mcu_msp430_runtime_HeapNull_Instance_init
#define HeapNull_Object_count ti_mcu_msp430_runtime_HeapNull_Object_count
#define HeapNull_Object_get ti_mcu_msp430_runtime_HeapNull_Object_get
#define HeapNull_Object_first ti_mcu_msp430_runtime_HeapNull_Object_first
#define HeapNull_Object_next ti_mcu_msp430_runtime_HeapNull_Object_next
#define HeapNull_Object_sizeof ti_mcu_msp430_runtime_HeapNull_Object_sizeof
#define HeapNull_Params_copy ti_mcu_msp430_runtime_HeapNull_Params_copy
#define HeapNull_Params_init ti_mcu_msp430_runtime_HeapNull_Params_init
#define HeapNull_delete ti_mcu_msp430_runtime_HeapNull_delete
#define HeapNull_destruct ti_mcu_msp430_runtime_HeapNull_destruct
#define HeapNull_Module_upCast ti_mcu_msp430_runtime_HeapNull_Module_upCast
#define HeapNull_Module_to_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Module_to_xdc_runtime_IHeap
#define HeapNull_Handle_upCast ti_mcu_msp430_runtime_HeapNull_Handle_upCast
#define HeapNull_Handle_to_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Handle_to_xdc_runtime_IHeap
#define HeapNull_Handle_downCast ti_mcu_msp430_runtime_HeapNull_Handle_downCast
#define HeapNull_Handle_from_xdc_runtime_IHeap ti_mcu_msp430_runtime_HeapNull_Handle_from_xdc_runtime_IHeap

#endif /* ti_mcu_msp430_runtime_HeapNull__localnames__done */
#endif
