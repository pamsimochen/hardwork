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

#ifndef ti_mcu_msp430_runtime_utils_Led__include
#define ti_mcu_msp430_runtime_utils_Led__include

#ifndef __nested__
#define __nested__
#define ti_mcu_msp430_runtime_utils_Led__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_mcu_msp430_runtime_utils_Led___VERS 160


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

/* PORT0 */
#define ti_mcu_msp430_runtime_utils_Led_PORT0 (0x11)

/* PORT1 */
#define ti_mcu_msp430_runtime_utils_Led_PORT1 (0x21)

/* PORT2 */
#define ti_mcu_msp430_runtime_utils_Led_PORT2 (0x29)

/* PORT3 */
#define ti_mcu_msp430_runtime_utils_Led_PORT3 (0x19)

/* PORT4 */
#define ti_mcu_msp430_runtime_utils_Led_PORT4 (0x1D)

/* PORT5 */
#define ti_mcu_msp430_runtime_utils_Led_PORT5 (0x31)

/* PORT6 */
#define ti_mcu_msp430_runtime_utils_Led_PORT6 (0x35)

/* PORT7 */
#define ti_mcu_msp430_runtime_utils_Led_PORT7 (0x3A)

/* PORT8 */
#define ti_mcu_msp430_runtime_utils_Led_PORT8 (0x3B)

/* PORTA_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTA_L (0x202)

/* PORTA_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTA_H (0x203)

/* PORTB_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTB_L (0x222)

/* PORTB_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTB_H (0x223)

/* PORTC_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTC_L (0x242)

/* PORTC_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTC_H (0x243)

/* PORTD_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTD_L (0x262)

/* PORTD_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTD_H (0x263)

/* PORTE_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTE_L (0x282)

/* PORTE_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTE_H (0x283)

/* PORTF_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTF_L (0x2A2)

/* PORT0DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT0DIR (0x12)

/* PORT1DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT1DIR (0x22)

/* PORT2DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT2DIR (0x2A)

/* PORT3DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT3DIR (0x1A)

/* PORT4DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT4DIR (0x1E)

/* PORT5DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT5DIR (0x32)

/* PORT6DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT6DIR (0x36)

/* PORT7DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT7DIR (0x3C)

/* PORT8DIR */
#define ti_mcu_msp430_runtime_utils_Led_PORT8DIR (0x3D)

/* PORTADIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTADIR_L (0x204)

/* PORTADIR_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTADIR_H (0x205)

/* PORTBDIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTBDIR_L (0x224)

/* PORTBDIR_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTBDIR_H (0x225)

/* PORTCDIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTCDIR_L (0x244)

/* PORTCDIR_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTCDIR_H (0x245)

/* PORTDDIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTDDIR_L (0x264)

/* PORTDDIR_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTDDIR_H (0x265)

/* PORTEDIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTEDIR_L (0x284)

/* PORTEDIR_H */
#define ti_mcu_msp430_runtime_utils_Led_PORTEDIR_H (0x285)

/* PORTFDIR_L */
#define ti_mcu_msp430_runtime_utils_Led_PORTFDIR_L (0x2A4)


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsEnabled;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsEnabled ti_mcu_msp430_runtime_utils_Led_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsIncluded;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsIncluded ti_mcu_msp430_runtime_utils_Led_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsMask;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__diagsMask ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_Module__gateObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__gateObj ti_mcu_msp430_runtime_utils_Led_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_Module__gatePrms;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__gatePrms ti_mcu_msp430_runtime_utils_Led_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_mcu_msp430_runtime_utils_Led_Module__id;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__id ti_mcu_msp430_runtime_utils_Led_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerDefined;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerDefined ti_mcu_msp430_runtime_utils_Led_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerObj;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerObj ti_mcu_msp430_runtime_utils_Led_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn0;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn0 ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn1;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn1 ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn2;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn2 ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn4;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn4 ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn8;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn8 ti_mcu_msp430_runtime_utils_Led_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_mcu_msp430_runtime_utils_Led_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Module__startupDoneFxn ti_mcu_msp430_runtime_utils_Led_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_mcu_msp430_runtime_utils_Led_Object__count;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Object__count ti_mcu_msp430_runtime_utils_Led_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_mcu_msp430_runtime_utils_Led_Object__heap;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Object__heap ti_mcu_msp430_runtime_utils_Led_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_mcu_msp430_runtime_utils_Led_Object__sizeof;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Object__sizeof ti_mcu_msp430_runtime_utils_Led_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_Object__table;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_Object__table ti_mcu_msp430_runtime_utils_Led_Object__table__C;

/* PORT */
#define ti_mcu_msp430_runtime_utils_Led_PORT (ti_mcu_msp430_runtime_utils_Led_PORT__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_PORT;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_PORT ti_mcu_msp430_runtime_utils_Led_PORT__C;

/* DIRCFG */
#define ti_mcu_msp430_runtime_utils_Led_DIRCFG (ti_mcu_msp430_runtime_utils_Led_DIRCFG__C)
typedef xdc_Ptr CT__ti_mcu_msp430_runtime_utils_Led_DIRCFG;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_DIRCFG ti_mcu_msp430_runtime_utils_Led_DIRCFG__C;

/* RED */
#ifdef ti_mcu_msp430_runtime_utils_Led_RED__D
#define ti_mcu_msp430_runtime_utils_Led_RED (ti_mcu_msp430_runtime_utils_Led_RED__D)
#else
#define ti_mcu_msp430_runtime_utils_Led_RED (ti_mcu_msp430_runtime_utils_Led_RED__C)
typedef xdc_Bits8 CT__ti_mcu_msp430_runtime_utils_Led_RED;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_RED ti_mcu_msp430_runtime_utils_Led_RED__C;
#endif

/* GREEN */
#ifdef ti_mcu_msp430_runtime_utils_Led_GREEN__D
#define ti_mcu_msp430_runtime_utils_Led_GREEN (ti_mcu_msp430_runtime_utils_Led_GREEN__D)
#else
#define ti_mcu_msp430_runtime_utils_Led_GREEN (ti_mcu_msp430_runtime_utils_Led_GREEN__C)
typedef xdc_Bits8 CT__ti_mcu_msp430_runtime_utils_Led_GREEN;
__extern __FAR__ const CT__ti_mcu_msp430_runtime_utils_Led_GREEN ti_mcu_msp430_runtime_utils_Led_GREEN__C;
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_mcu_msp430_runtime_utils_Led_Module_startup ti_mcu_msp430_runtime_utils_Led_Module_startup__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_Module_startup__E, "ti_mcu_msp430_runtime_utils_Led_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Led_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_Module_startup__F, "ti_mcu_msp430_runtime_utils_Led_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Led_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_Module_startup__R, "ti_mcu_msp430_runtime_utils_Led_Module_startup")
__extern xdc_Int ti_mcu_msp430_runtime_utils_Led_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_Module__startupDone__S, "ti_mcu_msp430_runtime_utils_Led_Module__startupDone")
__extern xdc_Bool ti_mcu_msp430_runtime_utils_Led_Module__startupDone__S( void );

/* off__E */
#define ti_mcu_msp430_runtime_utils_Led_off ti_mcu_msp430_runtime_utils_Led_off__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_off__E, "ti_mcu_msp430_runtime_utils_Led_off")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_off__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_off__F, "ti_mcu_msp430_runtime_utils_Led_off")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_off__F( xdc_Bits8 mask );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_off__R( xdc_Bits8 mask );

/* on__E */
#define ti_mcu_msp430_runtime_utils_Led_on ti_mcu_msp430_runtime_utils_Led_on__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_on__E, "ti_mcu_msp430_runtime_utils_Led_on")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_on__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_on__F, "ti_mcu_msp430_runtime_utils_Led_on")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_on__F( xdc_Bits8 mask );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_on__R( xdc_Bits8 mask );

/* toggle__E */
#define ti_mcu_msp430_runtime_utils_Led_toggle ti_mcu_msp430_runtime_utils_Led_toggle__E
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_toggle__E, "ti_mcu_msp430_runtime_utils_Led_toggle")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_toggle__E( xdc_Bits8 mask );
xdc__CODESECT(ti_mcu_msp430_runtime_utils_Led_toggle__F, "ti_mcu_msp430_runtime_utils_Led_toggle")
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_toggle__F( xdc_Bits8 mask );
__extern xdc_Void ti_mcu_msp430_runtime_utils_Led_toggle__R( xdc_Bits8 mask );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_mcu_msp430_runtime_utils_Led_Module_startupDone() ti_mcu_msp430_runtime_utils_Led_Module__startupDone__S()

/* Object_heap */
#define ti_mcu_msp430_runtime_utils_Led_Object_heap() ti_mcu_msp430_runtime_utils_Led_Object__heap__C

/* Module_heap */
#define ti_mcu_msp430_runtime_utils_Led_Module_heap() ti_mcu_msp430_runtime_utils_Led_Object__heap__C

/* Module_id */
static inline CT__ti_mcu_msp430_runtime_utils_Led_Module__id ti_mcu_msp430_runtime_utils_Led_Module_id( void ) 
{
    return ti_mcu_msp430_runtime_utils_Led_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_mcu_msp430_runtime_utils_Led_Module_hasMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_mcu_msp430_runtime_utils_Led_Module_getMask( void ) 
{
    return ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C != NULL ? *ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_mcu_msp430_runtime_utils_Led_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C != NULL) *ti_mcu_msp430_runtime_utils_Led_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_mcu_msp430_runtime_utils_Led__top__
#undef __nested__
#endif

#endif /* ti_mcu_msp430_runtime_utils_Led__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_mcu_msp430_runtime_utils_Led__internalaccess))

#ifndef ti_mcu_msp430_runtime_utils_Led__include_state
#define ti_mcu_msp430_runtime_utils_Led__include_state


#endif /* ti_mcu_msp430_runtime_utils_Led__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_mcu_msp430_runtime_utils_Led__nolocalnames)

#ifndef ti_mcu_msp430_runtime_utils_Led__localnames__done
#define ti_mcu_msp430_runtime_utils_Led__localnames__done

/* module prefix */
#define Led_PORT0 ti_mcu_msp430_runtime_utils_Led_PORT0
#define Led_PORT1 ti_mcu_msp430_runtime_utils_Led_PORT1
#define Led_PORT2 ti_mcu_msp430_runtime_utils_Led_PORT2
#define Led_PORT3 ti_mcu_msp430_runtime_utils_Led_PORT3
#define Led_PORT4 ti_mcu_msp430_runtime_utils_Led_PORT4
#define Led_PORT5 ti_mcu_msp430_runtime_utils_Led_PORT5
#define Led_PORT6 ti_mcu_msp430_runtime_utils_Led_PORT6
#define Led_PORT7 ti_mcu_msp430_runtime_utils_Led_PORT7
#define Led_PORT8 ti_mcu_msp430_runtime_utils_Led_PORT8
#define Led_PORTA_L ti_mcu_msp430_runtime_utils_Led_PORTA_L
#define Led_PORTA_H ti_mcu_msp430_runtime_utils_Led_PORTA_H
#define Led_PORTB_L ti_mcu_msp430_runtime_utils_Led_PORTB_L
#define Led_PORTB_H ti_mcu_msp430_runtime_utils_Led_PORTB_H
#define Led_PORTC_L ti_mcu_msp430_runtime_utils_Led_PORTC_L
#define Led_PORTC_H ti_mcu_msp430_runtime_utils_Led_PORTC_H
#define Led_PORTD_L ti_mcu_msp430_runtime_utils_Led_PORTD_L
#define Led_PORTD_H ti_mcu_msp430_runtime_utils_Led_PORTD_H
#define Led_PORTE_L ti_mcu_msp430_runtime_utils_Led_PORTE_L
#define Led_PORTE_H ti_mcu_msp430_runtime_utils_Led_PORTE_H
#define Led_PORTF_L ti_mcu_msp430_runtime_utils_Led_PORTF_L
#define Led_PORT0DIR ti_mcu_msp430_runtime_utils_Led_PORT0DIR
#define Led_PORT1DIR ti_mcu_msp430_runtime_utils_Led_PORT1DIR
#define Led_PORT2DIR ti_mcu_msp430_runtime_utils_Led_PORT2DIR
#define Led_PORT3DIR ti_mcu_msp430_runtime_utils_Led_PORT3DIR
#define Led_PORT4DIR ti_mcu_msp430_runtime_utils_Led_PORT4DIR
#define Led_PORT5DIR ti_mcu_msp430_runtime_utils_Led_PORT5DIR
#define Led_PORT6DIR ti_mcu_msp430_runtime_utils_Led_PORT6DIR
#define Led_PORT7DIR ti_mcu_msp430_runtime_utils_Led_PORT7DIR
#define Led_PORT8DIR ti_mcu_msp430_runtime_utils_Led_PORT8DIR
#define Led_PORTADIR_L ti_mcu_msp430_runtime_utils_Led_PORTADIR_L
#define Led_PORTADIR_H ti_mcu_msp430_runtime_utils_Led_PORTADIR_H
#define Led_PORTBDIR_L ti_mcu_msp430_runtime_utils_Led_PORTBDIR_L
#define Led_PORTBDIR_H ti_mcu_msp430_runtime_utils_Led_PORTBDIR_H
#define Led_PORTCDIR_L ti_mcu_msp430_runtime_utils_Led_PORTCDIR_L
#define Led_PORTCDIR_H ti_mcu_msp430_runtime_utils_Led_PORTCDIR_H
#define Led_PORTDDIR_L ti_mcu_msp430_runtime_utils_Led_PORTDDIR_L
#define Led_PORTDDIR_H ti_mcu_msp430_runtime_utils_Led_PORTDDIR_H
#define Led_PORTEDIR_L ti_mcu_msp430_runtime_utils_Led_PORTEDIR_L
#define Led_PORTEDIR_H ti_mcu_msp430_runtime_utils_Led_PORTEDIR_H
#define Led_PORTFDIR_L ti_mcu_msp430_runtime_utils_Led_PORTFDIR_L
#define Led_PORT ti_mcu_msp430_runtime_utils_Led_PORT
#define Led_DIRCFG ti_mcu_msp430_runtime_utils_Led_DIRCFG
#define Led_RED ti_mcu_msp430_runtime_utils_Led_RED
#define Led_GREEN ti_mcu_msp430_runtime_utils_Led_GREEN
#define Led_off ti_mcu_msp430_runtime_utils_Led_off
#define Led_on ti_mcu_msp430_runtime_utils_Led_on
#define Led_toggle ti_mcu_msp430_runtime_utils_Led_toggle
#define Led_Module_name ti_mcu_msp430_runtime_utils_Led_Module_name
#define Led_Module_id ti_mcu_msp430_runtime_utils_Led_Module_id
#define Led_Module_startup ti_mcu_msp430_runtime_utils_Led_Module_startup
#define Led_Module_startupDone ti_mcu_msp430_runtime_utils_Led_Module_startupDone
#define Led_Module_hasMask ti_mcu_msp430_runtime_utils_Led_Module_hasMask
#define Led_Module_getMask ti_mcu_msp430_runtime_utils_Led_Module_getMask
#define Led_Module_setMask ti_mcu_msp430_runtime_utils_Led_Module_setMask
#define Led_Object_heap ti_mcu_msp430_runtime_utils_Led_Object_heap
#define Led_Module_heap ti_mcu_msp430_runtime_utils_Led_Module_heap

#endif /* ti_mcu_msp430_runtime_utils_Led__localnames__done */
#endif
