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

#ifndef ti_sysbios_hal_unicache_Cache__include
#define ti_sysbios_hal_unicache_Cache__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_hal_unicache_Cache__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_hal_unicache_Cache___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/hal/unicache/package/package.defs.h>

#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/interfaces/ICache.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/hal/unicache/package/Cache_Module_GateProxy.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* Type */
typedef ti_sysbios_interfaces_ICache_Type ti_sysbios_hal_unicache_Cache_Type;

/* CACHE */
typedef xdc_UInt32 __T1_ti_sysbios_hal_unicache_Cache_CACHE__Reserved;
typedef xdc_UInt32 __ARRAY1_ti_sysbios_hal_unicache_Cache_CACHE__Reserved[0x77];
typedef __ARRAY1_ti_sysbios_hal_unicache_Cache_CACHE__Reserved __TA_ti_sysbios_hal_unicache_Cache_CACHE__Reserved;
struct ti_sysbios_hal_unicache_Cache_CACHE {
    xdc_UInt32 L1_INFO;
    xdc_UInt32 L1_CONFIG;
    xdc_UInt32 L1_INT;
    xdc_UInt32 L1_OCP;
    xdc_UInt32 L1_MAINT;
    xdc_Ptr L1_MTSTART;
    xdc_Ptr L1_MTEND;
    xdc_Ptr L1_CTADDR;
    xdc_UInt32 L1_CTDATA;
    __TA_ti_sysbios_hal_unicache_Cache_CACHE__Reserved Reserved;
    xdc_UInt32 L2_INFO;
    xdc_UInt32 L2_CONFIG;
    xdc_UInt32 L2_INT;
    xdc_UInt32 L2_OCP;
    xdc_UInt32 L2_MAINT;
    xdc_Ptr L2_MTSTART;
    xdc_Ptr L2_MTEND;
    xdc_Ptr L2_CTADDR;
    xdc_UInt32 L2_CTDATA;
};

/* cache */
#define ti_sysbios_hal_unicache_Cache_cache ti_sysbios_hal_unicache_Cache_cache
__extern volatile ti_sysbios_hal_unicache_Cache_CACHE ti_sysbios_hal_unicache_Cache_cache;

/* OCPConfig */
struct ti_sysbios_hal_unicache_Cache_OCPConfig {
    xdc_UInt8 wrap;
    xdc_UInt8 wrbuffer;
    xdc_UInt8 prefetch;
    xdc_UInt8 cleanbuf;
};

/* SecurityConfig */
struct ti_sysbios_hal_unicache_Cache_SecurityConfig {
    xdc_UInt8 secure;
    xdc_UInt8 nbypass;
    xdc_UInt8 secint;
    xdc_UInt8 secport;
    xdc_UInt8 secmain;
};

/* Type_L1P */
#define ti_sysbios_hal_unicache_Cache_Type_L1P ti_sysbios_interfaces_ICache_Type_L1P

/* Type_L1D */
#define ti_sysbios_hal_unicache_Cache_Type_L1D ti_sysbios_interfaces_ICache_Type_L1D

/* Type_L1 */
#define ti_sysbios_hal_unicache_Cache_Type_L1 ti_sysbios_interfaces_ICache_Type_L1

/* Type_L2P */
#define ti_sysbios_hal_unicache_Cache_Type_L2P ti_sysbios_interfaces_ICache_Type_L2P

/* Type_L2D */
#define ti_sysbios_hal_unicache_Cache_Type_L2D ti_sysbios_interfaces_ICache_Type_L2D

/* Type_L2 */
#define ti_sysbios_hal_unicache_Cache_Type_L2 ti_sysbios_interfaces_ICache_Type_L2

/* Type_ALLP */
#define ti_sysbios_hal_unicache_Cache_Type_ALLP ti_sysbios_interfaces_ICache_Type_ALLP

/* Type_ALLD */
#define ti_sysbios_hal_unicache_Cache_Type_ALLD ti_sysbios_interfaces_ICache_Type_ALLD

/* Type_ALL */
#define ti_sysbios_hal_unicache_Cache_Type_ALL ti_sysbios_interfaces_ICache_Type_ALL


/*
 * ======== INTERNAL DEFINITIONS ========
 */


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsEnabled ti_sysbios_hal_unicache_Cache_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsIncluded ti_sysbios_hal_unicache_Cache_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_sysbios_hal_unicache_Cache_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsMask ti_sysbios_hal_unicache_Cache_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_hal_unicache_Cache_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__gateObj ti_sysbios_hal_unicache_Cache_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_hal_unicache_Cache_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__gatePrms ti_sysbios_hal_unicache_Cache_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_hal_unicache_Cache_Module__id;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__id ti_sysbios_hal_unicache_Cache_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_hal_unicache_Cache_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerDefined ti_sysbios_hal_unicache_Cache_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_hal_unicache_Cache_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerObj ti_sysbios_hal_unicache_Cache_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn0 ti_sysbios_hal_unicache_Cache_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn1 ti_sysbios_hal_unicache_Cache_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn2 ti_sysbios_hal_unicache_Cache_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn4 ti_sysbios_hal_unicache_Cache_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn8 ti_sysbios_hal_unicache_Cache_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_hal_unicache_Cache_Object__count;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__count ti_sysbios_hal_unicache_Cache_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_hal_unicache_Cache_Object__heap;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__heap ti_sysbios_hal_unicache_Cache_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_hal_unicache_Cache_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__sizeof ti_sysbios_hal_unicache_Cache_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_hal_unicache_Cache_Object__table;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__table ti_sysbios_hal_unicache_Cache_Object__table__C;

/* E_exception */
#define ti_sysbios_hal_unicache_Cache_E_exception (ti_sysbios_hal_unicache_Cache_E_exception__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_hal_unicache_Cache_E_exception;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_E_exception ti_sysbios_hal_unicache_Cache_E_exception__C;

/* configureCache */
#ifdef ti_sysbios_hal_unicache_Cache_configureCache__D
#define ti_sysbios_hal_unicache_Cache_configureCache (ti_sysbios_hal_unicache_Cache_configureCache__D)
#else
#define ti_sysbios_hal_unicache_Cache_configureCache (ti_sysbios_hal_unicache_Cache_configureCache__C)
typedef xdc_Bool CT__ti_sysbios_hal_unicache_Cache_configureCache;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_configureCache ti_sysbios_hal_unicache_Cache_configureCache__C;
#endif

/* enableCache */
#ifdef ti_sysbios_hal_unicache_Cache_enableCache__D
#define ti_sysbios_hal_unicache_Cache_enableCache (ti_sysbios_hal_unicache_Cache_enableCache__D)
#else
#define ti_sysbios_hal_unicache_Cache_enableCache (ti_sysbios_hal_unicache_Cache_enableCache__C)
typedef xdc_Bool CT__ti_sysbios_hal_unicache_Cache_enableCache;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_enableCache ti_sysbios_hal_unicache_Cache_enableCache__C;
#endif

/* maxLineModeBufSize */
#ifdef ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__D
#define ti_sysbios_hal_unicache_Cache_maxLineModeBufSize (ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__D)
#else
#define ti_sysbios_hal_unicache_Cache_maxLineModeBufSize (ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__C)
typedef xdc_SizeT CT__ti_sysbios_hal_unicache_Cache_maxLineModeBufSize;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_maxLineModeBufSize ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__C;
#endif

/* l1ocpConfig */
#ifdef ti_sysbios_hal_unicache_Cache_l1ocpConfig__D
#define ti_sysbios_hal_unicache_Cache_l1ocpConfig (ti_sysbios_hal_unicache_Cache_l1ocpConfig__D)
#else
#define ti_sysbios_hal_unicache_Cache_l1ocpConfig (ti_sysbios_hal_unicache_Cache_l1ocpConfig__C)
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_l1ocpConfig;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_l1ocpConfig ti_sysbios_hal_unicache_Cache_l1ocpConfig__C;
#endif

/* l1secConfig */
#ifdef ti_sysbios_hal_unicache_Cache_l1secConfig__D
#define ti_sysbios_hal_unicache_Cache_l1secConfig (ti_sysbios_hal_unicache_Cache_l1secConfig__D)
#else
#define ti_sysbios_hal_unicache_Cache_l1secConfig (ti_sysbios_hal_unicache_Cache_l1secConfig__C)
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_l1secConfig;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_l1secConfig ti_sysbios_hal_unicache_Cache_l1secConfig__C;
#endif

/* l2ocpConfig */
#ifdef ti_sysbios_hal_unicache_Cache_l2ocpConfig__D
#define ti_sysbios_hal_unicache_Cache_l2ocpConfig (ti_sysbios_hal_unicache_Cache_l2ocpConfig__D)
#else
#define ti_sysbios_hal_unicache_Cache_l2ocpConfig (ti_sysbios_hal_unicache_Cache_l2ocpConfig__C)
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_l2ocpConfig;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2ocpConfig ti_sysbios_hal_unicache_Cache_l2ocpConfig__C;
#endif

/* l2secConfig */
#ifdef ti_sysbios_hal_unicache_Cache_l2secConfig__D
#define ti_sysbios_hal_unicache_Cache_l2secConfig (ti_sysbios_hal_unicache_Cache_l2secConfig__D)
#else
#define ti_sysbios_hal_unicache_Cache_l2secConfig (ti_sysbios_hal_unicache_Cache_l2secConfig__C)
typedef xdc_Bits32 CT__ti_sysbios_hal_unicache_Cache_l2secConfig;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2secConfig ti_sysbios_hal_unicache_Cache_l2secConfig__C;
#endif

/* l2CacheSupported */
#ifdef ti_sysbios_hal_unicache_Cache_l2CacheSupported__D
#define ti_sysbios_hal_unicache_Cache_l2CacheSupported (ti_sysbios_hal_unicache_Cache_l2CacheSupported__D)
#else
#define ti_sysbios_hal_unicache_Cache_l2CacheSupported (ti_sysbios_hal_unicache_Cache_l2CacheSupported__C)
typedef xdc_Bool CT__ti_sysbios_hal_unicache_Cache_l2CacheSupported;
__extern __FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2CacheSupported ti_sysbios_hal_unicache_Cache_l2CacheSupported__C;
#endif


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_hal_unicache_Cache_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Void (*enable)(xdc_Bits16);
    xdc_Void (*disable)(xdc_Bits16);
    xdc_Void (*inv)(xdc_Ptr, xdc_SizeT, xdc_Bits16, xdc_Bool);
    xdc_Void (*wb)(xdc_Ptr, xdc_SizeT, xdc_Bits16, xdc_Bool);
    xdc_Void (*wbInv)(xdc_Ptr, xdc_SizeT, xdc_Bits16, xdc_Bool);
    xdc_Void (*wbAll)(void);
    xdc_Void (*wbInvAll)(void);
    xdc_Void (*wait)(void);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_sysbios_hal_unicache_Cache_Fxns__ ti_sysbios_hal_unicache_Cache_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_hal_unicache_Cache_Module_startup ti_sysbios_hal_unicache_Cache_Module_startup__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_Module_startup__E, "ti_sysbios_hal_unicache_Cache_Module_startup")
__extern xdc_Int ti_sysbios_hal_unicache_Cache_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_Module_startup__F, "ti_sysbios_hal_unicache_Cache_Module_startup")
__extern xdc_Int ti_sysbios_hal_unicache_Cache_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_Module_startup__R, "ti_sysbios_hal_unicache_Cache_Module_startup")
__extern xdc_Int ti_sysbios_hal_unicache_Cache_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_Module__startupDone__S, "ti_sysbios_hal_unicache_Cache_Module__startupDone")
__extern xdc_Bool ti_sysbios_hal_unicache_Cache_Module__startupDone__S( void );

/* enable__E */
#define ti_sysbios_hal_unicache_Cache_enable ti_sysbios_hal_unicache_Cache_enable__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_enable__E, "ti_sysbios_hal_unicache_Cache_enable")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_enable__E( xdc_Bits16 type );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_enable__F, "ti_sysbios_hal_unicache_Cache_enable")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_enable__F( xdc_Bits16 type );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_enable__R( xdc_Bits16 type );

/* disable__E */
#define ti_sysbios_hal_unicache_Cache_disable ti_sysbios_hal_unicache_Cache_disable__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_disable__E, "ti_sysbios_hal_unicache_Cache_disable")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_disable__E( xdc_Bits16 type );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_disable__F, "ti_sysbios_hal_unicache_Cache_disable")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_disable__F( xdc_Bits16 type );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_disable__R( xdc_Bits16 type );

/* inv__E */
#define ti_sysbios_hal_unicache_Cache_inv ti_sysbios_hal_unicache_Cache_inv__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_inv__E, "ti_sysbios_hal_unicache_Cache_inv")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_inv__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_inv__F, "ti_sysbios_hal_unicache_Cache_inv")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_inv__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_inv__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* wb__E */
#define ti_sysbios_hal_unicache_Cache_wb ti_sysbios_hal_unicache_Cache_wb__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wb__E, "ti_sysbios_hal_unicache_Cache_wb")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wb__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wb__F, "ti_sysbios_hal_unicache_Cache_wb")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wb__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wb__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* wbInv__E */
#define ti_sysbios_hal_unicache_Cache_wbInv ti_sysbios_hal_unicache_Cache_wbInv__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbInv__E, "ti_sysbios_hal_unicache_Cache_wbInv")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInv__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbInv__F, "ti_sysbios_hal_unicache_Cache_wbInv")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInv__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInv__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* wait__E */
#define ti_sysbios_hal_unicache_Cache_wait ti_sysbios_hal_unicache_Cache_wait__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wait__E, "ti_sysbios_hal_unicache_Cache_wait")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wait__E( void );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wait__F, "ti_sysbios_hal_unicache_Cache_wait")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wait__F( void );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wait__R( void );

/* lock__E */
#define ti_sysbios_hal_unicache_Cache_lock ti_sysbios_hal_unicache_Cache_lock__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_lock__E, "ti_sysbios_hal_unicache_Cache_lock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_lock__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_lock__F, "ti_sysbios_hal_unicache_Cache_lock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_lock__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_lock__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* unlock__E */
#define ti_sysbios_hal_unicache_Cache_unlock ti_sysbios_hal_unicache_Cache_unlock__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_unlock__E, "ti_sysbios_hal_unicache_Cache_unlock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_unlock__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_unlock__F, "ti_sysbios_hal_unicache_Cache_unlock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_unlock__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_unlock__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* preload__E */
#define ti_sysbios_hal_unicache_Cache_preload ti_sysbios_hal_unicache_Cache_preload__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_preload__E, "ti_sysbios_hal_unicache_Cache_preload")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preload__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_preload__F, "ti_sysbios_hal_unicache_Cache_preload")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preload__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preload__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* preloadLock__E */
#define ti_sysbios_hal_unicache_Cache_preloadLock ti_sysbios_hal_unicache_Cache_preloadLock__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_preloadLock__E, "ti_sysbios_hal_unicache_Cache_preloadLock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preloadLock__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_preloadLock__F, "ti_sysbios_hal_unicache_Cache_preloadLock")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preloadLock__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_preloadLock__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait );

/* wbAll__E */
#define ti_sysbios_hal_unicache_Cache_wbAll ti_sysbios_hal_unicache_Cache_wbAll__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbAll__E, "ti_sysbios_hal_unicache_Cache_wbAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbAll__E( void );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbAll__F, "ti_sysbios_hal_unicache_Cache_wbAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbAll__F( void );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbAll__R( void );

/* invAll__E */
#define ti_sysbios_hal_unicache_Cache_invAll ti_sysbios_hal_unicache_Cache_invAll__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_invAll__E, "ti_sysbios_hal_unicache_Cache_invAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_invAll__E( void );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_invAll__F, "ti_sysbios_hal_unicache_Cache_invAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_invAll__F( void );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_invAll__R( void );

/* wbInvAll__E */
#define ti_sysbios_hal_unicache_Cache_wbInvAll ti_sysbios_hal_unicache_Cache_wbInvAll__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbInvAll__E, "ti_sysbios_hal_unicache_Cache_wbInvAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInvAll__E( void );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_wbInvAll__F, "ti_sysbios_hal_unicache_Cache_wbInvAll")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInvAll__F( void );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_wbInvAll__R( void );

/* read__E */
#define ti_sysbios_hal_unicache_Cache_read ti_sysbios_hal_unicache_Cache_read__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_read__E, "ti_sysbios_hal_unicache_Cache_read")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_read__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Ptr destBuf );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_read__F, "ti_sysbios_hal_unicache_Cache_read")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_read__F( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Ptr destBuf );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_read__R( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Ptr destBuf );

/* dumpRegs__E */
#define ti_sysbios_hal_unicache_Cache_dumpRegs ti_sysbios_hal_unicache_Cache_dumpRegs__E
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_dumpRegs__E, "ti_sysbios_hal_unicache_Cache_dumpRegs")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_dumpRegs__E( void );
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_dumpRegs__F, "ti_sysbios_hal_unicache_Cache_dumpRegs")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_dumpRegs__F( void );
__extern xdc_Void ti_sysbios_hal_unicache_Cache_dumpRegs__R( void );

/* seizeRegs__I */
#define ti_sysbios_hal_unicache_Cache_seizeRegs ti_sysbios_hal_unicache_Cache_seizeRegs__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_seizeRegs__I, "ti_sysbios_hal_unicache_Cache_seizeRegs")
__extern xdc_UInt ti_sysbios_hal_unicache_Cache_seizeRegs__I( void );

/* releaseRegs__I */
#define ti_sysbios_hal_unicache_Cache_releaseRegs ti_sysbios_hal_unicache_Cache_releaseRegs__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_releaseRegs__I, "ti_sysbios_hal_unicache_Cache_releaseRegs")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_releaseRegs__I( xdc_UInt key );

/* ISR__I */
#define ti_sysbios_hal_unicache_Cache_ISR ti_sysbios_hal_unicache_Cache_ISR__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_ISR__I, "ti_sysbios_hal_unicache_Cache_ISR")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_ISR__I( xdc_UArg level );

/* printInfo__I */
#define ti_sysbios_hal_unicache_Cache_printInfo ti_sysbios_hal_unicache_Cache_printInfo__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_printInfo__I, "ti_sysbios_hal_unicache_Cache_printInfo")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_printInfo__I( xdc_UInt info, xdc_UInt level );

/* printConfig__I */
#define ti_sysbios_hal_unicache_Cache_printConfig ti_sysbios_hal_unicache_Cache_printConfig__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_printConfig__I, "ti_sysbios_hal_unicache_Cache_printConfig")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_printConfig__I( xdc_UInt cfg, xdc_UInt level );

/* printOCP__I */
#define ti_sysbios_hal_unicache_Cache_printOCP ti_sysbios_hal_unicache_Cache_printOCP__I
xdc__CODESECT(ti_sysbios_hal_unicache_Cache_printOCP__I, "ti_sysbios_hal_unicache_Cache_printOCP")
__extern xdc_Void ti_sysbios_hal_unicache_Cache_printOCP__I( xdc_UInt cfg, xdc_UInt level );


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_sysbios_interfaces_ICache_Module ti_sysbios_hal_unicache_Cache_Module_upCast( void )
{
    return (ti_sysbios_interfaces_ICache_Module)&ti_sysbios_hal_unicache_Cache_Module__FXNS__C;
}

/* Module_to_ti_sysbios_interfaces_ICache */
#define ti_sysbios_hal_unicache_Cache_Module_to_ti_sysbios_interfaces_ICache ti_sysbios_hal_unicache_Cache_Module_upCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_hal_unicache_Cache_Module_startupDone() ti_sysbios_hal_unicache_Cache_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_hal_unicache_Cache_Object_heap() ti_sysbios_hal_unicache_Cache_Object__heap__C

/* Module_heap */
#define ti_sysbios_hal_unicache_Cache_Module_heap() ti_sysbios_hal_unicache_Cache_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_hal_unicache_Cache_Module__id ti_sysbios_hal_unicache_Cache_Module_id( void ) 
{
    return ti_sysbios_hal_unicache_Cache_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_hal_unicache_Cache_Module_hasMask( void ) 
{
    return ti_sysbios_hal_unicache_Cache_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_hal_unicache_Cache_Module_getMask( void ) 
{
    return ti_sysbios_hal_unicache_Cache_Module__diagsMask__C != NULL ? *ti_sysbios_hal_unicache_Cache_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_hal_unicache_Cache_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_hal_unicache_Cache_Module__diagsMask__C != NULL) *ti_sysbios_hal_unicache_Cache_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_hal_unicache_Cache__top__
#undef __nested__
#endif

#endif /* ti_sysbios_hal_unicache_Cache__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_hal_unicache_Cache__internalaccess))

#ifndef ti_sysbios_hal_unicache_Cache__include_state
#define ti_sysbios_hal_unicache_Cache__include_state


#endif /* ti_sysbios_hal_unicache_Cache__include_state */

#endif

/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_hal_unicache_Cache__nolocalnames)

#ifndef ti_sysbios_hal_unicache_Cache__localnames__done
#define ti_sysbios_hal_unicache_Cache__localnames__done

/* module prefix */
#define Cache_Type ti_sysbios_hal_unicache_Cache_Type
#define Cache_CACHE ti_sysbios_hal_unicache_Cache_CACHE
#define Cache_cache ti_sysbios_hal_unicache_Cache_cache
#define Cache_OCPConfig ti_sysbios_hal_unicache_Cache_OCPConfig
#define Cache_SecurityConfig ti_sysbios_hal_unicache_Cache_SecurityConfig
#define Cache_Type_L1P ti_sysbios_hal_unicache_Cache_Type_L1P
#define Cache_Type_L1D ti_sysbios_hal_unicache_Cache_Type_L1D
#define Cache_Type_L1 ti_sysbios_hal_unicache_Cache_Type_L1
#define Cache_Type_L2P ti_sysbios_hal_unicache_Cache_Type_L2P
#define Cache_Type_L2D ti_sysbios_hal_unicache_Cache_Type_L2D
#define Cache_Type_L2 ti_sysbios_hal_unicache_Cache_Type_L2
#define Cache_Type_ALLP ti_sysbios_hal_unicache_Cache_Type_ALLP
#define Cache_Type_ALLD ti_sysbios_hal_unicache_Cache_Type_ALLD
#define Cache_Type_ALL ti_sysbios_hal_unicache_Cache_Type_ALL
#define Cache_E_exception ti_sysbios_hal_unicache_Cache_E_exception
#define Cache_configureCache ti_sysbios_hal_unicache_Cache_configureCache
#define Cache_enableCache ti_sysbios_hal_unicache_Cache_enableCache
#define Cache_maxLineModeBufSize ti_sysbios_hal_unicache_Cache_maxLineModeBufSize
#define Cache_l1ocpConfig ti_sysbios_hal_unicache_Cache_l1ocpConfig
#define Cache_l1secConfig ti_sysbios_hal_unicache_Cache_l1secConfig
#define Cache_l2ocpConfig ti_sysbios_hal_unicache_Cache_l2ocpConfig
#define Cache_l2secConfig ti_sysbios_hal_unicache_Cache_l2secConfig
#define Cache_l2CacheSupported ti_sysbios_hal_unicache_Cache_l2CacheSupported
#define Cache_enable ti_sysbios_hal_unicache_Cache_enable
#define Cache_disable ti_sysbios_hal_unicache_Cache_disable
#define Cache_inv ti_sysbios_hal_unicache_Cache_inv
#define Cache_wb ti_sysbios_hal_unicache_Cache_wb
#define Cache_wbInv ti_sysbios_hal_unicache_Cache_wbInv
#define Cache_wait ti_sysbios_hal_unicache_Cache_wait
#define Cache_lock ti_sysbios_hal_unicache_Cache_lock
#define Cache_unlock ti_sysbios_hal_unicache_Cache_unlock
#define Cache_preload ti_sysbios_hal_unicache_Cache_preload
#define Cache_preloadLock ti_sysbios_hal_unicache_Cache_preloadLock
#define Cache_wbAll ti_sysbios_hal_unicache_Cache_wbAll
#define Cache_invAll ti_sysbios_hal_unicache_Cache_invAll
#define Cache_wbInvAll ti_sysbios_hal_unicache_Cache_wbInvAll
#define Cache_read ti_sysbios_hal_unicache_Cache_read
#define Cache_dumpRegs ti_sysbios_hal_unicache_Cache_dumpRegs
#define Cache_Module_name ti_sysbios_hal_unicache_Cache_Module_name
#define Cache_Module_id ti_sysbios_hal_unicache_Cache_Module_id
#define Cache_Module_startup ti_sysbios_hal_unicache_Cache_Module_startup
#define Cache_Module_startupDone ti_sysbios_hal_unicache_Cache_Module_startupDone
#define Cache_Module_hasMask ti_sysbios_hal_unicache_Cache_Module_hasMask
#define Cache_Module_getMask ti_sysbios_hal_unicache_Cache_Module_getMask
#define Cache_Module_setMask ti_sysbios_hal_unicache_Cache_Module_setMask
#define Cache_Object_heap ti_sysbios_hal_unicache_Cache_Object_heap
#define Cache_Module_heap ti_sysbios_hal_unicache_Cache_Module_heap
#define Cache_Module_upCast ti_sysbios_hal_unicache_Cache_Module_upCast
#define Cache_Module_to_ti_sysbios_interfaces_ICache ti_sysbios_hal_unicache_Cache_Module_to_ti_sysbios_interfaces_ICache

/* proxies */
#include <ti/sysbios/hal/unicache/package/Cache_Module_GateProxy.h>

#endif /* ti_sysbios_hal_unicache_Cache__localnames__done */
#endif
