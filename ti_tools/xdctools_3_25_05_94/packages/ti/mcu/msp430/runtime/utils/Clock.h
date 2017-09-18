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

#ifndef ti_mcu_msp430_runtime_utils_Clock__include
#define ti_mcu_msp430_runtime_utils_Clock__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_utils_Clock__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_utils_Clock___VERS 160


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

/* TimeValue */
typedef xdc_UInt16 ti_mcu_msp430_runtime_utils_Clock_TimeValue;

/* TimerRegs */
typedef xdc_Bits16 __T1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCTL;
typedef xdc_Bits16 __ARRAY1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCTL[7];
typedef __ARRAY1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCTL __TA_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCTL;
typedef xdc_Bits16 __T1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCR;
typedef xdc_Bits16 __ARRAY1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCR[7];
typedef __ARRAY1_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCR __TA_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCR;
struct ti_mcu_msp430_runtime_utils_Clock_TimerRegs {
    xdc_Bits16 CTL;
    __TA_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCTL CCTL;
    xdc_Bits16 R;
    __TA_ti_mcu_msp430_runtime_utils_Clock_TimerRegs__CCR CCR;
    xdc_Bits16 EX0;
    xdc_Bits16 IV;
};

/* TA2 */
#define ti_mcu_msp430_runtime_utils_Clock_TA2 (0x160)

/* TA3 */
#define ti_mcu_msp430_runtime_utils_Clock_TA3 (0x160)

/* TA5 */
#define ti_mcu_msp430_runtime_utils_Clock_TA5 (0x160)

/* TB3 */
#define ti_mcu_msp430_runtime_utils_Clock_TB3 (0x180)

/* TB7 */
#define ti_mcu_msp430_runtime_utils_Clock_TB7 (0x180)

/* T1A2 */
#define ti_mcu_msp430_runtime_utils_Clock_T1A2 (0x180)

/* T0A3 */
#define ti_mcu_msp430_runtime_utils_Clock_T0A3 (0x340)

/* T0A5 */
#define ti_mcu_msp430_runtime_utils_Clock_T0A5 (0x340)

/* T1A3 */
#define ti_mcu_msp430_runtime_utils_Clock_T1A3 (0x380)

/* T1A5 */
#define ti_mcu_msp430_runtime_utils_Clock_T1A5 (0x380)

/* T2A3 */
#define ti_mcu_msp430_runtime_utils_Clock_T2A3 (0x400)

/* T0B7 */
#define ti_mcu_msp430_runtime_utils_Clock_T0B7 (0x3d0)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsEnabled ti_mcu_msp430_runtime_utils_Clock_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsIncluded ti_mcu_msp430_runtime_utils_Clock_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Clock_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__gateObj ti_mcu_msp430_runtime_utils_Clock_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Clock_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__gatePrms ti_mcu_msp430_runtime_utils_Clock_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_utils_Clock_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__id ti_mcu_msp430_runtime_utils_Clock_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerDefined ti_mcu_msp430_runtime_utils_Clock_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerObj ti_mcu_msp430_runtime_utils_Clock_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn0 ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn1 ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn2 ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn4 ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn8 ti_mcu_msp430_runtime_utils_Clock_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_utils_Clock_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Module__startupDoneFxn ti_mcu_msp430_runtime_utils_Clock_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Clock_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Object__count ti_mcu_msp430_runtime_utils_Clock_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_utils_Clock_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Object__heap ti_mcu_msp430_runtime_utils_Clock_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_utils_Clock_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Object__sizeof ti_mcu_msp430_runtime_utils_Clock_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Clock_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_Object__table ti_mcu_msp430_runtime_utils_Clock_Object__table__C;

/* TIMER */
#define ti_mcu_msp430_runtime_utils_Clock_TIMER (ti_mcu_msp430_runtime_utils_Clock_TIMER__C)
typedef ti_mcu_msp430_runtime_utils_Clock_TimerRegs* CT__ti_mcu_msp430_runtime_utils_Clock_TIMER;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_TIMER ti_mcu_msp430_runtime_utils_Clock_TIMER__C;

/* fastClockKHz */
#ifdef ti_mcu_msp430_runtime_utils_Clock_fastClockKHz__D
#define ti_mcu_msp430_runtime_utils_Clock_fastClockKHz (ti_mcu_msp430_runtime_utils_Clock_fastClockKHz__D)
#else
#define ti_mcu_msp430_runtime_utils_Clock_fastClockKHz (ti_mcu_msp430_runtime_utils_Clock_fastClockKHz__C)
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Clock_fastClockKHz;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_fastClockKHz ti_mcu_msp430_runtime_utils_Clock_fastClockKHz__C;
#endif

/* slowClockHz */
#ifdef ti_mcu_msp430_runtime_utils_Clock_slowClockHz__D
#define ti_mcu_msp430_runtime_utils_Clock_slowClockHz (ti_mcu_msp430_runtime_utils_Clock_slowClockHz__D)
#else
#define ti_mcu_msp430_runtime_utils_Clock_slowClockHz (ti_mcu_msp430_runtime_utils_Clock_slowClockHz__C)
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Clock_slowClockHz;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Clock_slowClockHz ti_mcu_msp430_runtime_utils_Clock_slowClockHz__C;
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_utils_Clock_Module_startup ti_mcu_msp430_runtime_utils_Clock_Module_startup__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_Module_startup__E, "ti_mcu_msp430_runtime_utils_Clock_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Clock_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_Module_startup__F, "ti_mcu_msp430_runtime_utils_Clock_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Clock_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_Module_startup__R, "ti_mcu_msp430_runtime_utils_Clock_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Clock_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_Module__startupDone__S, "ti_mcu_msp430_runtime_utils_Clock_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Clock_Module__startupDone__S( void );

/* delay__E */
#define ti_mcu_msp430_runtime_utils_Clock_delay ti_mcu_msp430_runtime_utils_Clock_delay__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_delay__E, "ti_mcu_msp430_runtime_utils_Clock_delay")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_delay__E( xdc_UInt16 usec );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_delay__F, "ti_mcu_msp430_runtime_utils_Clock_delay")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_delay__F( xdc_UInt16 usec );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_delay__R( xdc_UInt16 usec );

/* sleep__E */
#define ti_mcu_msp430_runtime_utils_Clock_sleep ti_mcu_msp430_runtime_utils_Clock_sleep__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_sleep__E, "ti_mcu_msp430_runtime_utils_Clock_sleep")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_sleep__E( xdc_UInt16 usec, xdc_UInt lpm );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_sleep__F, "ti_mcu_msp430_runtime_utils_Clock_sleep")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_sleep__F( xdc_UInt16 usec, xdc_UInt lpm );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Clock_sleep__R( xdc_UInt16 usec, xdc_UInt lpm );

/* getTime__E */
#define ti_mcu_msp430_runtime_utils_Clock_getTime ti_mcu_msp430_runtime_utils_Clock_getTime__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_getTime__E, "ti_mcu_msp430_runtime_utils_Clock_getTime")
__extern ti_mcu_msp430_runtime_utils_Clock_TimeValue ti_mcu_msp430_runtime_utils_Clock_getTime__E( void );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Clock_getTime__F, "ti_mcu_msp430_runtime_utils_Clock_getTime")
__extern ti_mcu_msp430_runtime_utils_Clock_TimeValue ti_mcu_msp430_runtime_utils_Clock_getTime__F( void );
__extern ti_mcu_msp430_runtime_utils_Clock_TimeValue ti_mcu_msp430_runtime_utils_Clock_getTime__R( void );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_utils_Clock_Module_startupDone() ti_mcu_msp430_runtime_utils_Clock_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_utils_Clock_Object_heap() ti_mcu_msp430_runtime_utils_Clock_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_utils_Clock_Module_heap() ti_mcu_msp430_runtime_utils_Clock_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_utils_Clock_Module__id ti_mcu_msp430_runtime_utils_Clock_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_utils_Clock_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_utils_Clock_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_utils_Clock_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_utils_Clock_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_utils_Clock_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_utils_Clock__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_utils_Clock__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_utils_Clock__internalaccess))

#ifndef ti_mcu_msp430_runtime_utils_Clock__include_state
#define ti_mcu_msp430_runtime_utils_Clock__include_state


#endif /* ti_mcu_msp430_runtime_utils_Clock__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_utils_Clock__nolocalnames)

#ifndef ti_mcu_msp430_runtime_utils_Clock__localnames__done
#define ti_mcu_msp430_runtime_utils_Clock__localnames__done

/* module prefix */
#define Clock_TimeValue ti_mcu_msp430_runtime_utils_Clock_TimeValue
#define Clock_TimerRegs ti_mcu_msp430_runtime_utils_Clock_TimerRegs
#define Clock_TA2 ti_mcu_msp430_runtime_utils_Clock_TA2
#define Clock_TA3 ti_mcu_msp430_runtime_utils_Clock_TA3
#define Clock_TA5 ti_mcu_msp430_runtime_utils_Clock_TA5
#define Clock_TB3 ti_mcu_msp430_runtime_utils_Clock_TB3
#define Clock_TB7 ti_mcu_msp430_runtime_utils_Clock_TB7
#define Clock_T1A2 ti_mcu_msp430_runtime_utils_Clock_T1A2
#define Clock_T0A3 ti_mcu_msp430_runtime_utils_Clock_T0A3
#define Clock_T0A5 ti_mcu_msp430_runtime_utils_Clock_T0A5
#define Clock_T1A3 ti_mcu_msp430_runtime_utils_Clock_T1A3
#define Clock_T1A5 ti_mcu_msp430_runtime_utils_Clock_T1A5
#define Clock_T2A3 ti_mcu_msp430_runtime_utils_Clock_T2A3
#define Clock_T0B7 ti_mcu_msp430_runtime_utils_Clock_T0B7
#define Clock_TIMER ti_mcu_msp430_runtime_utils_Clock_TIMER
#define Clock_fastClockKHz ti_mcu_msp430_runtime_utils_Clock_fastClockKHz
#define Clock_slowClockHz ti_mcu_msp430_runtime_utils_Clock_slowClockHz
#define Clock_delay ti_mcu_msp430_runtime_utils_Clock_delay
#define Clock_sleep ti_mcu_msp430_runtime_utils_Clock_sleep
#define Clock_getTime ti_mcu_msp430_runtime_utils_Clock_getTime
#define Clock_Module_name ti_mcu_msp430_runtime_utils_Clock_Module_name
#define Clock_Module_id ti_mcu_msp430_runtime_utils_Clock_Module_id
#define Clock_Module_startup ti_mcu_msp430_runtime_utils_Clock_Module_startup
#define Clock_Module_startupDone ti_mcu_msp430_runtime_utils_Clock_Module_startupDone
#define Clock_Module_hasMask ti_mcu_msp430_runtime_utils_Clock_Module_hasMask
#define Clock_Module_getMask ti_mcu_msp430_runtime_utils_Clock_Module_getMask
#define Clock_Module_setMask ti_mcu_msp430_runtime_utils_Clock_Module_setMask
#define Clock_Object_heap ti_mcu_msp430_runtime_utils_Clock_Object_heap
#define Clock_Module_heap ti_mcu_msp430_runtime_utils_Clock_Module_heap

#endif /* ti_mcu_msp430_runtime_utils_Clock__localnames__done */
#endif
