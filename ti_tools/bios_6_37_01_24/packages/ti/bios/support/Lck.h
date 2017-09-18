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
 *     FUNCTION DECLARATIONS
 *     FUNCTION SELECTORS
 *     SYSTEM FUNCTIONS
 *     
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_bios_support_Lck__include
#define ti_bios_support_Lck__include

#ifndef __nested__
#define __nested__
#define ti_bios_support_Lck__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_bios_support_Lck___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/bios/support/package/package.defs.h>

#include <xdc/runtime/IModule.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>


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
typedef xdc_Bits32 CT__ti_bios_support_Lck_Module__diagsEnabled;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__diagsEnabled ti_bios_support_Lck_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_bios_support_Lck_Module__diagsIncluded;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__diagsIncluded ti_bios_support_Lck_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_bios_support_Lck_Module__diagsMask;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__diagsMask ti_bios_support_Lck_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_bios_support_Lck_Module__gateObj;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__gateObj ti_bios_support_Lck_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_bios_support_Lck_Module__gatePrms;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__gatePrms ti_bios_support_Lck_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_bios_support_Lck_Module__id;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__id ti_bios_support_Lck_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_bios_support_Lck_Module__loggerDefined;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerDefined ti_bios_support_Lck_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_bios_support_Lck_Module__loggerObj;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerObj ti_bios_support_Lck_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_bios_support_Lck_Module__loggerFxn0;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerFxn0 ti_bios_support_Lck_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_bios_support_Lck_Module__loggerFxn1;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerFxn1 ti_bios_support_Lck_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_bios_support_Lck_Module__loggerFxn2;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerFxn2 ti_bios_support_Lck_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_bios_support_Lck_Module__loggerFxn4;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerFxn4 ti_bios_support_Lck_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_bios_support_Lck_Module__loggerFxn8;
__extern __FAR__ const CT__ti_bios_support_Lck_Module__loggerFxn8 ti_bios_support_Lck_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_bios_support_Lck_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_bios_support_Lck_Module__startupDoneFxn ti_bios_support_Lck_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_bios_support_Lck_Object__count;
__extern __FAR__ const CT__ti_bios_support_Lck_Object__count ti_bios_support_Lck_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_bios_support_Lck_Object__heap;
__extern __FAR__ const CT__ti_bios_support_Lck_Object__heap ti_bios_support_Lck_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_bios_support_Lck_Object__sizeof;
__extern __FAR__ const CT__ti_bios_support_Lck_Object__sizeof ti_bios_support_Lck_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_bios_support_Lck_Object__table;
__extern __FAR__ const CT__ti_bios_support_Lck_Object__table ti_bios_support_Lck_Object__table__C;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_bios_support_Lck_Params {
    size_t __size;
    const void* __self;
    void* __fxns;
    xdc_runtime_IInstance_Params* instance;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_bios_support_Lck_Struct {
    xdc_Int __f0;
    ti_sysbios_knl_Task_Handle __f1;
    ti_sysbios_knl_Semaphore_Struct __f2;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_bios_support_Lck_Module_startup( state ) (-1)

/* Instance_init__F */
xdc__CODESECT(ti_bios_support_Lck_Instance_init__F, "ti_bios_support_Lck_Instance_init")
__extern void ti_bios_support_Lck_Instance_init__F( ti_bios_support_Lck_Object*, const ti_bios_support_Lck_Params* );

/* Instance_finalize__F */
xdc__CODESECT(ti_bios_support_Lck_Instance_finalize__F, "ti_bios_support_Lck_Instance_finalize")
__extern void ti_bios_support_Lck_Instance_finalize__F( ti_bios_support_Lck_Object* );

/* Instance_init__R */
xdc__CODESECT(ti_bios_support_Lck_Instance_init__R, "ti_bios_support_Lck_Instance_init")
__extern void ti_bios_support_Lck_Instance_init__R( ti_bios_support_Lck_Object*, const ti_bios_support_Lck_Params* );

/* Instance_finalize__R */
xdc__CODESECT(ti_bios_support_Lck_Instance_finalize__R, "ti_bios_support_Lck_Instance_finalize")
__extern void ti_bios_support_Lck_Instance_finalize__R( ti_bios_support_Lck_Object* );

/* Handle__label__S */
xdc__CODESECT(ti_bios_support_Lck_Handle__label__S, "ti_bios_support_Lck_Handle__label")
__extern xdc_runtime_Types_Label* ti_bios_support_Lck_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_bios_support_Lck_Module__startupDone__S, "ti_bios_support_Lck_Module__startupDone")
__extern xdc_Bool ti_bios_support_Lck_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_bios_support_Lck_Object__create__S, "ti_bios_support_Lck_Object__create")
__extern xdc_Ptr ti_bios_support_Lck_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar* __pa, xdc_SizeT __psz, xdc_runtime_Error_Block* __eb );

/* Object__delete__S */
xdc__CODESECT(ti_bios_support_Lck_Object__delete__S, "ti_bios_support_Lck_Object__delete")
__extern xdc_Void ti_bios_support_Lck_Object__delete__S( xdc_Ptr instp );

/* Object__destruct__S */
xdc__CODESECT(ti_bios_support_Lck_Object__destruct__S, "ti_bios_support_Lck_Object__destruct")
__extern xdc_Void ti_bios_support_Lck_Object__destruct__S( xdc_Ptr objp );

/* Object__get__S */
xdc__CODESECT(ti_bios_support_Lck_Object__get__S, "ti_bios_support_Lck_Object__get")
__extern xdc_Ptr ti_bios_support_Lck_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_bios_support_Lck_Object__first__S, "ti_bios_support_Lck_Object__first")
__extern xdc_Ptr ti_bios_support_Lck_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_bios_support_Lck_Object__next__S, "ti_bios_support_Lck_Object__next")
__extern xdc_Ptr ti_bios_support_Lck_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_bios_support_Lck_Params__init__S, "ti_bios_support_Lck_Params__init")
__extern xdc_Void ti_bios_support_Lck_Params__init__S( xdc_Ptr dst, const xdc_Void* src, xdc_SizeT psz, xdc_SizeT isz );

/* pend__E */
#define ti_bios_support_Lck_pend ti_bios_support_Lck_pend__E
xdc__CODESECT(ti_bios_support_Lck_pend__E, "ti_bios_support_Lck_pend")
__extern xdc_Bool ti_bios_support_Lck_pend__E( ti_bios_support_Lck_Handle __inst, xdc_UInt timeout );
xdc__CODESECT(ti_bios_support_Lck_pend__F, "ti_bios_support_Lck_pend")
__extern xdc_Bool ti_bios_support_Lck_pend__F( ti_bios_support_Lck_Object* __inst, xdc_UInt timeout );
__extern xdc_Bool ti_bios_support_Lck_pend__R( ti_bios_support_Lck_Handle __inst, xdc_UInt timeout );

/* post__E */
#define ti_bios_support_Lck_post ti_bios_support_Lck_post__E
xdc__CODESECT(ti_bios_support_Lck_post__E, "ti_bios_support_Lck_post")
__extern xdc_Void ti_bios_support_Lck_post__E( ti_bios_support_Lck_Handle __inst );
xdc__CODESECT(ti_bios_support_Lck_post__F, "ti_bios_support_Lck_post")
__extern xdc_Void ti_bios_support_Lck_post__F( ti_bios_support_Lck_Object* __inst );
__extern xdc_Void ti_bios_support_Lck_post__R( ti_bios_support_Lck_Handle __inst );


/*
 * ======== FUNCTION SELECTORS ========
 */

/* pend_{FxnT,fxnP} */
typedef xdc_Bool (*ti_bios_support_Lck_pend_FxnT)(void*, xdc_UInt);
static inline ti_bios_support_Lck_pend_FxnT ti_bios_support_Lck_pend_fxnP( void )
{
    return (ti_bios_support_Lck_pend_FxnT)ti_bios_support_Lck_pend; 
}

/* post_{FxnT,fxnP} */
typedef xdc_Void (*ti_bios_support_Lck_post_FxnT)(void*);
static inline ti_bios_support_Lck_post_FxnT ti_bios_support_Lck_post_fxnP( void )
{
    return (ti_bios_support_Lck_post_FxnT)ti_bios_support_Lck_post; 
}


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_bios_support_Lck_Module_startupDone() ti_bios_support_Lck_Module__startupDone__S()

/* Object_heap */
#define ti_bios_support_Lck_Object_heap() ti_bios_support_Lck_Object__heap__C

/* Module_heap */
#define ti_bios_support_Lck_Module_heap() ti_bios_support_Lck_Object__heap__C

/* Module_id */
static inline CT__ti_bios_support_Lck_Module__id ti_bios_support_Lck_Module_id( void ) 
{
    return ti_bios_support_Lck_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_bios_support_Lck_Module_hasMask( void ) 
{
    return ti_bios_support_Lck_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_bios_support_Lck_Module_getMask( void ) 
{
    return ti_bios_support_Lck_Module__diagsMask__C != NULL ? *ti_bios_support_Lck_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_bios_support_Lck_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_bios_support_Lck_Module__diagsMask__C != NULL) *ti_bios_support_Lck_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_bios_support_Lck_Params_init( ti_bios_support_Lck_Params* prms ) 
{
    if (prms) {
        ti_bios_support_Lck_Params__init__S(prms, 0, sizeof(ti_bios_support_Lck_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_bios_support_Lck_Params_copy( ti_bios_support_Lck_Params* dst, const ti_bios_support_Lck_Params* src ) 
{
    if (dst) {
        ti_bios_support_Lck_Params__init__S(dst, (const void *)src, sizeof(ti_bios_support_Lck_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_bios_support_Lck_Object_count() ti_bios_support_Lck_Object__count__C

/* Object_sizeof */
#define ti_bios_support_Lck_Object_sizeof() ti_bios_support_Lck_Object__sizeof__C

/* Object_get */
static inline ti_bios_support_Lck_Handle ti_bios_support_Lck_Object_get( ti_bios_support_Lck_Instance_State* oarr, int i ) 
{
    return (ti_bios_support_Lck_Handle)ti_bios_support_Lck_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_bios_support_Lck_Handle ti_bios_support_Lck_Object_first( void )
{
    return (ti_bios_support_Lck_Handle)ti_bios_support_Lck_Object__first__S();
}

/* Object_next */
static inline ti_bios_support_Lck_Handle ti_bios_support_Lck_Object_next( ti_bios_support_Lck_Object* obj )
{
    return (ti_bios_support_Lck_Handle)ti_bios_support_Lck_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label* ti_bios_support_Lck_Handle_label( ti_bios_support_Lck_Handle inst, xdc_runtime_Types_Label* lab )
{
    return ti_bios_support_Lck_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_bios_support_Lck_Handle_name( ti_bios_support_Lck_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_bios_support_Lck_Handle__label__S(inst, &lab)->iname;
}

/* create */
static inline ti_bios_support_Lck_Handle ti_bios_support_Lck_create( const ti_bios_support_Lck_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    return (ti_bios_support_Lck_Handle)ti_bios_support_Lck_Object__create__S(0, 0, 0, (const xdc_UChar*)__prms, sizeof(ti_bios_support_Lck_Params), __eb);
}

/* construct */
static inline void ti_bios_support_Lck_construct( ti_bios_support_Lck_Struct* __obj, const ti_bios_support_Lck_Params* __prms )
{
    ti_bios_support_Lck_Object__create__S(__obj, sizeof (ti_bios_support_Lck_Struct), 0, (const xdc_UChar*)__prms, sizeof(ti_bios_support_Lck_Params), NULL);
}

/* delete */
static inline void ti_bios_support_Lck_delete( ti_bios_support_Lck_Handle* instp )
{
    ti_bios_support_Lck_Object__delete__S(instp);
}

/* destruct */
static inline void ti_bios_support_Lck_destruct( ti_bios_support_Lck_Struct* obj )
{
    ti_bios_support_Lck_Object__destruct__S(obj);
}

/* handle */
static inline ti_bios_support_Lck_Handle ti_bios_support_Lck_handle( ti_bios_support_Lck_Struct* str )
{
    return (ti_bios_support_Lck_Handle)str;
}

/* struct */
static inline ti_bios_support_Lck_Struct* ti_bios_support_Lck_struct( ti_bios_support_Lck_Handle inst )
{
    return (ti_bios_support_Lck_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_bios_support_Lck__top__
#undef __nested__
#endif

#endif /* ti_bios_support_Lck__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_bios_support_Lck__internalaccess))

#ifndef ti_bios_support_Lck__include_state
#define ti_bios_support_Lck__include_state

/* Object */
struct ti_bios_support_Lck_Object {
    xdc_Int value;
    ti_sysbios_knl_Task_Handle owner;
    char __dummy;
};

/* Instance_State_sem */
__extern __FAR__ const xdc_SizeT ti_bios_support_Lck_Instance_State_sem__O;
static inline ti_sysbios_knl_Semaphore_Handle ti_bios_support_Lck_Instance_State_sem(ti_bios_support_Lck_Object* obj)
{
    return (ti_sysbios_knl_Semaphore_Handle)(((char*)obj) + ti_bios_support_Lck_Instance_State_sem__O);
}

#endif /* ti_bios_support_Lck__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_bios_support_Lck__nolocalnames)

#ifndef ti_bios_support_Lck__localnames__done
#define ti_bios_support_Lck__localnames__done

/* module prefix */
#define Lck_Instance ti_bios_support_Lck_Instance
#define Lck_Handle ti_bios_support_Lck_Handle
#define Lck_Module ti_bios_support_Lck_Module
#define Lck_Object ti_bios_support_Lck_Object
#define Lck_Struct ti_bios_support_Lck_Struct
#define Lck_Instance_State ti_bios_support_Lck_Instance_State
#define Lck_Instance_State_sem ti_bios_support_Lck_Instance_State_sem
#define Lck_Params ti_bios_support_Lck_Params
#define Lck_pend ti_bios_support_Lck_pend
#define Lck_pend_fxnP ti_bios_support_Lck_pend_fxnP
#define Lck_pend_FxnT ti_bios_support_Lck_pend_FxnT
#define Lck_post ti_bios_support_Lck_post
#define Lck_post_fxnP ti_bios_support_Lck_post_fxnP
#define Lck_post_FxnT ti_bios_support_Lck_post_FxnT
#define Lck_Module_name ti_bios_support_Lck_Module_name
#define Lck_Module_id ti_bios_support_Lck_Module_id
#define Lck_Module_startup ti_bios_support_Lck_Module_startup
#define Lck_Module_startupDone ti_bios_support_Lck_Module_startupDone
#define Lck_Module_hasMask ti_bios_support_Lck_Module_hasMask
#define Lck_Module_getMask ti_bios_support_Lck_Module_getMask
#define Lck_Module_setMask ti_bios_support_Lck_Module_setMask
#define Lck_Object_heap ti_bios_support_Lck_Object_heap
#define Lck_Module_heap ti_bios_support_Lck_Module_heap
#define Lck_construct ti_bios_support_Lck_construct
#define Lck_create ti_bios_support_Lck_create
#define Lck_handle ti_bios_support_Lck_handle
#define Lck_struct ti_bios_support_Lck_struct
#define Lck_Handle_label ti_bios_support_Lck_Handle_label
#define Lck_Handle_name ti_bios_support_Lck_Handle_name
#define Lck_Instance_init ti_bios_support_Lck_Instance_init
#define Lck_Object_count ti_bios_support_Lck_Object_count
#define Lck_Object_get ti_bios_support_Lck_Object_get
#define Lck_Object_first ti_bios_support_Lck_Object_first
#define Lck_Object_next ti_bios_support_Lck_Object_next
#define Lck_Object_sizeof ti_bios_support_Lck_Object_sizeof
#define Lck_Params_copy ti_bios_support_Lck_Params_copy
#define Lck_Params_init ti_bios_support_Lck_Params_init
#define Lck_Instance_finalize ti_bios_support_Lck_Instance_finalize
#define Lck_delete ti_bios_support_Lck_delete
#define Lck_destruct ti_bios_support_Lck_destruct

#endif /* ti_bios_support_Lck__localnames__done */
#endif
