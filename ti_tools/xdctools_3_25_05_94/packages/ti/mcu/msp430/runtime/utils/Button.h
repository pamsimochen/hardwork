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

#ifndef ti_mcu_msp430_runtime_utils_Button__include
#define ti_mcu_msp430_runtime_utils_Button__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_utils_Button__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_utils_Button___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/mcu/msp430/runtime/utils/package/package.defs.h>

#include <xdc/runtime/IModule.h>


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
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsEnabled ti_mcu_msp430_runtime_utils_Button_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsIncluded ti_mcu_msp430_runtime_utils_Button_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__diagsMask ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__gateObj ti_mcu_msp430_runtime_utils_Button_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__gatePrms ti_mcu_msp430_runtime_utils_Button_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_utils_Button_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__id ti_mcu_msp430_runtime_utils_Button_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerDefined ti_mcu_msp430_runtime_utils_Button_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerObj ti_mcu_msp430_runtime_utils_Button_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn0 ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn1 ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn2 ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn4 ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn8 ti_mcu_msp430_runtime_utils_Button_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_utils_Button_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Module__startupDoneFxn ti_mcu_msp430_runtime_utils_Button_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Button_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Object__count ti_mcu_msp430_runtime_utils_Button_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_utils_Button_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Object__heap ti_mcu_msp430_runtime_utils_Button_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_utils_Button_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Object__sizeof ti_mcu_msp430_runtime_utils_Button_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_Object__table ti_mcu_msp430_runtime_utils_Button_Object__table__C;

/* PORT */
#define ti_mcu_msp430_runtime_utils_Button_PORT (ti_mcu_msp430_runtime_utils_Button_PORT__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_PORT;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_PORT ti_mcu_msp430_runtime_utils_Button_PORT__C;

/* PORT_IE */
#define ti_mcu_msp430_runtime_utils_Button_PORT_IE (ti_mcu_msp430_runtime_utils_Button_PORT_IE__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_PORT_IE;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_PORT_IE ti_mcu_msp430_runtime_utils_Button_PORT_IE__C;

/* PORT_OUT */
#define ti_mcu_msp430_runtime_utils_Button_PORT_OUT (ti_mcu_msp430_runtime_utils_Button_PORT_OUT__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_PORT_OUT;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_PORT_OUT ti_mcu_msp430_runtime_utils_Button_PORT_OUT__C;

/* PORT_REN */
#define ti_mcu_msp430_runtime_utils_Button_PORT_REN (ti_mcu_msp430_runtime_utils_Button_PORT_REN__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_PORT_REN;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_PORT_REN ti_mcu_msp430_runtime_utils_Button_PORT_REN__C;

/* PORT_IFG */
#define ti_mcu_msp430_runtime_utils_Button_PORT_IFG (ti_mcu_msp430_runtime_utils_Button_PORT_IFG__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Button_PORT_IFG;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_PORT_IFG ti_mcu_msp430_runtime_utils_Button_PORT_IFG__C;

/* SWITCH1 */
#ifdef ti_mcu_msp430_runtime_utils_Button_SWITCH1__D
#define ti_mcu_msp430_runtime_utils_Button_SWITCH1 (ti_mcu_msp430_runtime_utils_Button_SWITCH1__D)
#else
#define ti_mcu_msp430_runtime_utils_Button_SWITCH1 (ti_mcu_msp430_runtime_utils_Button_SWITCH1__C)
typedef xdc_Bits8 CT__ti_mcu_msp430_runtime_utils_Button_SWITCH1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_SWITCH1 ti_mcu_msp430_runtime_utils_Button_SWITCH1__C;
#endif

/* SWITCH2 */
#ifdef ti_mcu_msp430_runtime_utils_Button_SWITCH2__D
#define ti_mcu_msp430_runtime_utils_Button_SWITCH2 (ti_mcu_msp430_runtime_utils_Button_SWITCH2__D)
#else
#define ti_mcu_msp430_runtime_utils_Button_SWITCH2 (ti_mcu_msp430_runtime_utils_Button_SWITCH2__C)
typedef xdc_Bits8 CT__ti_mcu_msp430_runtime_utils_Button_SWITCH2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Button_SWITCH2 ti_mcu_msp430_runtime_utils_Button_SWITCH2__C;
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_utils_Button_Module_startup ti_mcu_msp430_runtime_utils_Button_Module_startup__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_Module_startup__E, "ti_mcu_msp430_runtime_utils_Button_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Button_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_Module_startup__F, "ti_mcu_msp430_runtime_utils_Button_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Button_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_Module_startup__R, "ti_mcu_msp430_runtime_utils_Button_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Button_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_Module__startupDone__S, "ti_mcu_msp430_runtime_utils_Button_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Button_Module__startupDone__S( void );

/* enable__E */
#define ti_mcu_msp430_runtime_utils_Button_enable ti_mcu_msp430_runtime_utils_Button_enable__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_enable__E, "ti_mcu_msp430_runtime_utils_Button_enable")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_enable__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_enable__F, "ti_mcu_msp430_runtime_utils_Button_enable")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_enable__F( xdc_Bits8 mask );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_enable__R( xdc_Bits8 mask );

/* disable__E */
#define ti_mcu_msp430_runtime_utils_Button_disable ti_mcu_msp430_runtime_utils_Button_disable__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_disable__E, "ti_mcu_msp430_runtime_utils_Button_disable")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_disable__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_disable__F, "ti_mcu_msp430_runtime_utils_Button_disable")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_disable__F( xdc_Bits8 mask );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Button_disable__R( xdc_Bits8 mask );

/* read__E */
#define ti_mcu_msp430_runtime_utils_Button_read ti_mcu_msp430_runtime_utils_Button_read__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_read__E, "ti_mcu_msp430_runtime_utils_Button_read")
__extern xdc_Bits8 ti_mcu_msp430_runtime_utils_Button_read__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Button_read__F, "ti_mcu_msp430_runtime_utils_Button_read")
__extern xdc_Bits8 ti_mcu_msp430_runtime_utils_Button_read__F( xdc_Bits8 mask );
__extern xdc_Bits8 ti_mcu_msp430_runtime_utils_Button_read__R( xdc_Bits8 mask );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_utils_Button_Module_startupDone() ti_mcu_msp430_runtime_utils_Button_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_utils_Button_Object_heap() ti_mcu_msp430_runtime_utils_Button_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_utils_Button_Module_heap() ti_mcu_msp430_runtime_utils_Button_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_utils_Button_Module__id ti_mcu_msp430_runtime_utils_Button_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_utils_Button_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_utils_Button_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_utils_Button_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_utils_Button_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_utils_Button_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_utils_Button__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_utils_Button__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_utils_Button__internalaccess))

#ifndef ti_mcu_msp430_runtime_utils_Button__include_state
#define ti_mcu_msp430_runtime_utils_Button__include_state


#endif /* ti_mcu_msp430_runtime_utils_Button__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_utils_Button__nolocalnames)

#ifndef ti_mcu_msp430_runtime_utils_Button__localnames__done
#define ti_mcu_msp430_runtime_utils_Button__localnames__done

/* module prefix */
#define Button_PORT ti_mcu_msp430_runtime_utils_Button_PORT
#define Button_PORT_IE ti_mcu_msp430_runtime_utils_Button_PORT_IE
#define Button_PORT_OUT ti_mcu_msp430_runtime_utils_Button_PORT_OUT
#define Button_PORT_REN ti_mcu_msp430_runtime_utils_Button_PORT_REN
#define Button_PORT_IFG ti_mcu_msp430_runtime_utils_Button_PORT_IFG
#define Button_SWITCH1 ti_mcu_msp430_runtime_utils_Button_SWITCH1
#define Button_SWITCH2 ti_mcu_msp430_runtime_utils_Button_SWITCH2
#define Button_enable ti_mcu_msp430_runtime_utils_Button_enable
#define Button_disable ti_mcu_msp430_runtime_utils_Button_disable
#define Button_read ti_mcu_msp430_runtime_utils_Button_read
#define Button_Module_name ti_mcu_msp430_runtime_utils_Button_Module_name
#define Button_Module_id ti_mcu_msp430_runtime_utils_Button_Module_id
#define Button_Module_startup ti_mcu_msp430_runtime_utils_Button_Module_startup
#define Button_Module_startupDone ti_mcu_msp430_runtime_utils_Button_Module_startupDone
#define Button_Module_hasMask ti_mcu_msp430_runtime_utils_Button_Module_hasMask
#define Button_Module_getMask ti_mcu_msp430_runtime_utils_Button_Module_getMask
#define Button_Module_setMask ti_mcu_msp430_runtime_utils_Button_Module_setMask
#define Button_Object_heap ti_mcu_msp430_runtime_utils_Button_Object_heap
#define Button_Module_heap ti_mcu_msp430_runtime_utils_Button_Module_heap

#endif /* ti_mcu_msp430_runtime_utils_Button__localnames__done */
#endif
