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

#ifndef ti_mcu_msp430_runtime_utils_Stack__include
#define ti_mcu_msp430_runtime_utils_Stack__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_utils_Stack__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_utils_Stack___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/mcu/msp430/runtime/utils/package/package.defs.h>

#include <xdc/runtime/IModule.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Status */
struct ti_mcu_msp430_runtime_utils_Stack_Status {
    xdc_Int unused;
    xdc_Int used;
    xdc_Int curDepth;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsEnabled ti_mcu_msp430_runtime_utils_Stack_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsIncluded ti_mcu_msp430_runtime_utils_Stack_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Stack_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__gateObj ti_mcu_msp430_runtime_utils_Stack_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Stack_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__gatePrms ti_mcu_msp430_runtime_utils_Stack_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_utils_Stack_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__id ti_mcu_msp430_runtime_utils_Stack_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerDefined ti_mcu_msp430_runtime_utils_Stack_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerObj ti_mcu_msp430_runtime_utils_Stack_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn0 ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn1 ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn2 ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn4 ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn8 ti_mcu_msp430_runtime_utils_Stack_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_utils_Stack_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Module__startupDoneFxn ti_mcu_msp430_runtime_utils_Stack_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Stack_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Object__count ti_mcu_msp430_runtime_utils_Stack_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_utils_Stack_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Object__heap ti_mcu_msp430_runtime_utils_Stack_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_utils_Stack_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Object__sizeof ti_mcu_msp430_runtime_utils_Stack_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Stack_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_Object__table ti_mcu_msp430_runtime_utils_Stack_Object__table__C;

/* UNUSED */
#define ti_mcu_msp430_runtime_utils_Stack_UNUSED (ti_mcu_msp430_runtime_utils_Stack_UNUSED__C)
typedef xdc_runtime_Log_Event CT__ti_mcu_msp430_runtime_utils_Stack_UNUSED;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Stack_UNUSED ti_mcu_msp430_runtime_utils_Stack_UNUSED__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_utils_Stack_Module_startup ti_mcu_msp430_runtime_utils_Stack_Module_startup__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_Module_startup__E, "ti_mcu_msp430_runtime_utils_Stack_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_Module_startup__F, "ti_mcu_msp430_runtime_utils_Stack_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_Module_startup__R, "ti_mcu_msp430_runtime_utils_Stack_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_Module__startupDone__S, "ti_mcu_msp430_runtime_utils_Stack_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Stack_Module__startupDone__S( void );

/* check__E */
#define ti_mcu_msp430_runtime_utils_Stack_check ti_mcu_msp430_runtime_utils_Stack_check__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_check__E, "ti_mcu_msp430_runtime_utils_Stack_check")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Stack_check__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_check__F, "ti_mcu_msp430_runtime_utils_Stack_check")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Stack_check__F( void );
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Stack_check__R( void );

/* fill__E */
#define ti_mcu_msp430_runtime_utils_Stack_fill ti_mcu_msp430_runtime_utils_Stack_fill__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_fill__E, "ti_mcu_msp430_runtime_utils_Stack_fill")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_fill__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_fill__F, "ti_mcu_msp430_runtime_utils_Stack_fill")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_fill__F( void );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_fill__R( void );

/* getUnused__E */
#define ti_mcu_msp430_runtime_utils_Stack_getUnused ti_mcu_msp430_runtime_utils_Stack_getUnused__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_getUnused__E, "ti_mcu_msp430_runtime_utils_Stack_getUnused")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_getUnused__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_getUnused__F, "ti_mcu_msp430_runtime_utils_Stack_getUnused")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_getUnused__F( void );
__extern xdc_Int ti_mcu_msp430_runtime_utils_Stack_getUnused__R( void );

/* getStatus__E */
#define ti_mcu_msp430_runtime_utils_Stack_getStatus ti_mcu_msp430_runtime_utils_Stack_getStatus__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_getStatus__E, "ti_mcu_msp430_runtime_utils_Stack_getStatus")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_getStatus__E( ti_mcu_msp430_runtime_utils_Stack_Status* stat );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Stack_getStatus__F, "ti_mcu_msp430_runtime_utils_Stack_getStatus")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_getStatus__F( ti_mcu_msp430_runtime_utils_Stack_Status* stat );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Stack_getStatus__R( ti_mcu_msp430_runtime_utils_Stack_Status* stat );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_utils_Stack_Module_startupDone() ti_mcu_msp430_runtime_utils_Stack_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_utils_Stack_Object_heap() ti_mcu_msp430_runtime_utils_Stack_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_utils_Stack_Module_heap() ti_mcu_msp430_runtime_utils_Stack_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_utils_Stack_Module__id ti_mcu_msp430_runtime_utils_Stack_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_utils_Stack_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_utils_Stack_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_utils_Stack_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_utils_Stack_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_utils_Stack_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_utils_Stack__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_utils_Stack__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_utils_Stack__internalaccess))

#ifndef ti_mcu_msp430_runtime_utils_Stack__include_state
#define ti_mcu_msp430_runtime_utils_Stack__include_state


#endif /* ti_mcu_msp430_runtime_utils_Stack__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_utils_Stack__nolocalnames)

#ifndef ti_mcu_msp430_runtime_utils_Stack__localnames__done
#define ti_mcu_msp430_runtime_utils_Stack__localnames__done

/* module prefix */
#define Stack_Status ti_mcu_msp430_runtime_utils_Stack_Status
#define Stack_UNUSED ti_mcu_msp430_runtime_utils_Stack_UNUSED
#define Stack_check ti_mcu_msp430_runtime_utils_Stack_check
#define Stack_fill ti_mcu_msp430_runtime_utils_Stack_fill
#define Stack_getUnused ti_mcu_msp430_runtime_utils_Stack_getUnused
#define Stack_getStatus ti_mcu_msp430_runtime_utils_Stack_getStatus
#define Stack_Module_name ti_mcu_msp430_runtime_utils_Stack_Module_name
#define Stack_Module_id ti_mcu_msp430_runtime_utils_Stack_Module_id
#define Stack_Module_startup ti_mcu_msp430_runtime_utils_Stack_Module_startup
#define Stack_Module_startupDone ti_mcu_msp430_runtime_utils_Stack_Module_startupDone
#define Stack_Module_hasMask ti_mcu_msp430_runtime_utils_Stack_Module_hasMask
#define Stack_Module_getMask ti_mcu_msp430_runtime_utils_Stack_Module_getMask
#define Stack_Module_setMask ti_mcu_msp430_runtime_utils_Stack_Module_setMask
#define Stack_Object_heap ti_mcu_msp430_runtime_utils_Stack_Object_heap
#define Stack_Module_heap ti_mcu_msp430_runtime_utils_Stack_Module_heap

#endif /* ti_mcu_msp430_runtime_utils_Stack__localnames__done */
#endif
