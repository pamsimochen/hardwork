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
 *     CREATE ARGS
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

#ifndef ti_sysbios_family_arp32_Hwi__include
#define ti_sysbios_family_arp32_Hwi__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arp32_Hwi__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_family_arp32_Hwi___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IInstance.h>
#include <ti/sysbios/family/arp32/Hwi__prologue.h>
#include <ti/sysbios/family/arp32/package/package.defs.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/interfaces/IHwi.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* FuncPtr */
typedef ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_family_arp32_Hwi_FuncPtr;

/* Irp */
typedef ti_sysbios_interfaces_IHwi_Irp ti_sysbios_family_arp32_Hwi_Irp;

/* HookSet */
typedef ti_sysbios_interfaces_IHwi_HookSet ti_sysbios_family_arp32_Hwi_HookSet;

/* MaskingOption */
typedef ti_sysbios_interfaces_IHwi_MaskingOption ti_sysbios_family_arp32_Hwi_MaskingOption;

/* StackInfo */
typedef ti_sysbios_interfaces_IHwi_StackInfo ti_sysbios_family_arp32_Hwi_StackInfo;

/* PlugFuncPtr */
typedef xdc_Void (*ti_sysbios_family_arp32_Hwi_PlugFuncPtr)(xdc_Void);

/* MaskingOption_NONE */
#define ti_sysbios_family_arp32_Hwi_MaskingOption_NONE ti_sysbios_interfaces_IHwi_MaskingOption_NONE

/* MaskingOption_ALL */
#define ti_sysbios_family_arp32_Hwi_MaskingOption_ALL ti_sysbios_interfaces_IHwi_MaskingOption_ALL

/* MaskingOption_SELF */
#define ti_sysbios_family_arp32_Hwi_MaskingOption_SELF ti_sysbios_interfaces_IHwi_MaskingOption_SELF

/* MaskingOption_BITMASK */
#define ti_sysbios_family_arp32_Hwi_MaskingOption_BITMASK ti_sysbios_interfaces_IHwi_MaskingOption_BITMASK

/* MaskingOption_LOWER */
#define ti_sysbios_family_arp32_Hwi_MaskingOption_LOWER ti_sysbios_interfaces_IHwi_MaskingOption_LOWER


/*
 * ======== CREATE ARGS ========
 */

/* Args__create */
typedef struct ti_sysbios_family_arp32_Hwi_Args__create {
    xdc_Int intNum;
    ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn;
} ti_sysbios_family_arp32_Hwi_Args__create;


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* IntcRegs */
struct ti_sysbios_family_arp32_Hwi_IntcRegs {
    volatile xdc_Bits32 rawStatus;
    volatile xdc_Bits32 status;
    volatile xdc_Bits32 set;
    volatile xdc_Bits32 clr;
};

/* Instance_State */
typedef xdc_Ptr __T1_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv;
typedef xdc_Ptr *__ARRAY1_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv __TA_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv;

/* Module_State */
typedef xdc_Bits32 __T1_ti_sysbios_family_arp32_Hwi_Module_State__interruptMask;
typedef xdc_Bits32 *__ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__interruptMask;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__interruptMask __TA_ti_sysbios_family_arp32_Hwi_Module_State__interruptMask;
typedef xdc_Bits16 __T1_ti_sysbios_family_arp32_Hwi_Module_State__disableIerMask;
typedef xdc_Bits16 *__ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__disableIerMask;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__disableIerMask __TA_ti_sysbios_family_arp32_Hwi_Module_State__disableIerMask;
typedef xdc_Bits16 __T1_ti_sysbios_family_arp32_Hwi_Module_State__restoreIerMask;
typedef xdc_Bits16 *__ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__restoreIerMask;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__restoreIerMask __TA_ti_sysbios_family_arp32_Hwi_Module_State__restoreIerMask;
typedef ti_sysbios_family_arp32_Hwi_Irp __T1_ti_sysbios_family_arp32_Hwi_Module_State__irp;
typedef ti_sysbios_family_arp32_Hwi_Irp *__ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__irp;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__irp __TA_ti_sysbios_family_arp32_Hwi_Module_State__irp;
typedef ti_sysbios_family_arp32_Hwi_Handle __T1_ti_sysbios_family_arp32_Hwi_Module_State__dispatchTable;
typedef ti_sysbios_family_arp32_Hwi_Handle *__ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__dispatchTable;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_Module_State__dispatchTable __TA_ti_sysbios_family_arp32_Hwi_Module_State__dispatchTable;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arp32_Hwi_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__diagsEnabled ti_sysbios_family_arp32_Hwi_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arp32_Hwi_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__diagsIncluded ti_sysbios_family_arp32_Hwi_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_sysbios_family_arp32_Hwi_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__diagsMask ti_sysbios_family_arp32_Hwi_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arp32_Hwi_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__gateObj ti_sysbios_family_arp32_Hwi_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arp32_Hwi_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__gatePrms ti_sysbios_family_arp32_Hwi_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arp32_Hwi_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__id ti_sysbios_family_arp32_Hwi_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arp32_Hwi_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerDefined ti_sysbios_family_arp32_Hwi_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arp32_Hwi_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerObj ti_sysbios_family_arp32_Hwi_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn0 ti_sysbios_family_arp32_Hwi_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn1 ti_sysbios_family_arp32_Hwi_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn2 ti_sysbios_family_arp32_Hwi_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn4 ti_sysbios_family_arp32_Hwi_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__loggerFxn8 ti_sysbios_family_arp32_Hwi_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_family_arp32_Hwi_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Module__startupDoneFxn ti_sysbios_family_arp32_Hwi_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arp32_Hwi_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Object__count ti_sysbios_family_arp32_Hwi_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arp32_Hwi_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Object__heap ti_sysbios_family_arp32_Hwi_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arp32_Hwi_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Object__sizeof ti_sysbios_family_arp32_Hwi_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arp32_Hwi_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_Object__table ti_sysbios_family_arp32_Hwi_Object__table__C;

/* dispatcherAutoNestingSupport */
#ifdef ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport__D
#define ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport (ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport__D)
#else
#define ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport (ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport__C)
typedef xdc_Bool CT__ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport__C;
#endif

/* dispatcherSwiSupport */
#ifdef ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport__D
#define ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport (ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport__D)
#else
#define ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport (ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport__C)
typedef xdc_Bool CT__ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport__C;
#endif

/* dispatcherTaskSupport */
#ifdef ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport__D
#define ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport (ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport__D)
#else
#define ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport (ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport__C)
typedef xdc_Bool CT__ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport__C;
#endif

/* dispatcherIrpTrackingSupport */
#ifdef ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport__D
#define ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport (ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport__D)
#else
#define ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport (ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport__C)
typedef xdc_Bool CT__ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport__C;
#endif

/* NUM_INTERRUPTS */
#ifdef ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS__D
#define ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS (ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS__D)
#else
#define ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS (ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS__C)
typedef xdc_Int CT__ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS__C;
#endif

/* NUM_VECTORS */
#ifdef ti_sysbios_family_arp32_Hwi_NUM_VECTORS__D
#define ti_sysbios_family_arp32_Hwi_NUM_VECTORS (ti_sysbios_family_arp32_Hwi_NUM_VECTORS__D)
#else
#define ti_sysbios_family_arp32_Hwi_NUM_VECTORS (ti_sysbios_family_arp32_Hwi_NUM_VECTORS__C)
typedef xdc_Int CT__ti_sysbios_family_arp32_Hwi_NUM_VECTORS;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_NUM_VECTORS ti_sysbios_family_arp32_Hwi_NUM_VECTORS__C;
#endif

/* A_badIntNum */
#define ti_sysbios_family_arp32_Hwi_A_badIntNum (ti_sysbios_family_arp32_Hwi_A_badIntNum__C)
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arp32_Hwi_A_badIntNum;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_A_badIntNum ti_sysbios_family_arp32_Hwi_A_badIntNum__C;

/* A_badVectNum */
#define ti_sysbios_family_arp32_Hwi_A_badVectNum (ti_sysbios_family_arp32_Hwi_A_badVectNum__C)
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arp32_Hwi_A_badVectNum;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_A_badVectNum ti_sysbios_family_arp32_Hwi_A_badVectNum__C;

/* E_alreadyDefined */
#define ti_sysbios_family_arp32_Hwi_E_alreadyDefined (ti_sysbios_family_arp32_Hwi_E_alreadyDefined__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_family_arp32_Hwi_E_alreadyDefined;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_E_alreadyDefined ti_sysbios_family_arp32_Hwi_E_alreadyDefined__C;

/* E_notImplemented */
#define ti_sysbios_family_arp32_Hwi_E_notImplemented (ti_sysbios_family_arp32_Hwi_E_notImplemented__C)
typedef xdc_runtime_Error_Id CT__ti_sysbios_family_arp32_Hwi_E_notImplemented;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_E_notImplemented ti_sysbios_family_arp32_Hwi_E_notImplemented__C;

/* LM_begin */
#define ti_sysbios_family_arp32_Hwi_LM_begin (ti_sysbios_family_arp32_Hwi_LM_begin__C)
typedef xdc_runtime_Log_Event CT__ti_sysbios_family_arp32_Hwi_LM_begin;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_LM_begin ti_sysbios_family_arp32_Hwi_LM_begin__C;

/* LD_end */
#define ti_sysbios_family_arp32_Hwi_LD_end (ti_sysbios_family_arp32_Hwi_LD_end__C)
typedef xdc_runtime_Log_Event CT__ti_sysbios_family_arp32_Hwi_LD_end;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_LD_end ti_sysbios_family_arp32_Hwi_LD_end__C;

/* initialIerMask */
#ifdef ti_sysbios_family_arp32_Hwi_initialIerMask__D
#define ti_sysbios_family_arp32_Hwi_initialIerMask (ti_sysbios_family_arp32_Hwi_initialIerMask__D)
#else
#define ti_sysbios_family_arp32_Hwi_initialIerMask (ti_sysbios_family_arp32_Hwi_initialIerMask__C)
typedef xdc_Bits32 CT__ti_sysbios_family_arp32_Hwi_initialIerMask;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_initialIerMask ti_sysbios_family_arp32_Hwi_initialIerMask__C;
#endif

/* INTCREGSBASEADDRS */
#define ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS (ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS__C)
typedef ti_sysbios_family_arp32_Hwi_IntcRegs* __T1_ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS;
typedef ti_sysbios_family_arp32_Hwi_IntcRegs* __ARRAY1_ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS[4];
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS __TA_ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS;
typedef __TA_ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS CT__ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS__C;

/* swiDisable */
#define ti_sysbios_family_arp32_Hwi_swiDisable (ti_sysbios_family_arp32_Hwi_swiDisable__C)
typedef xdc_UInt (*CT__ti_sysbios_family_arp32_Hwi_swiDisable)(void);
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_swiDisable ti_sysbios_family_arp32_Hwi_swiDisable__C;

/* swiRestoreHwi */
#define ti_sysbios_family_arp32_Hwi_swiRestoreHwi (ti_sysbios_family_arp32_Hwi_swiRestoreHwi__C)
typedef xdc_Void (*CT__ti_sysbios_family_arp32_Hwi_swiRestoreHwi)(xdc_UInt);
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_swiRestoreHwi ti_sysbios_family_arp32_Hwi_swiRestoreHwi__C;

/* taskDisable */
#define ti_sysbios_family_arp32_Hwi_taskDisable (ti_sysbios_family_arp32_Hwi_taskDisable__C)
typedef xdc_UInt (*CT__ti_sysbios_family_arp32_Hwi_taskDisable)(void);
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_taskDisable ti_sysbios_family_arp32_Hwi_taskDisable__C;

/* taskRestoreHwi */
#define ti_sysbios_family_arp32_Hwi_taskRestoreHwi (ti_sysbios_family_arp32_Hwi_taskRestoreHwi__C)
typedef xdc_Void (*CT__ti_sysbios_family_arp32_Hwi_taskRestoreHwi)(xdc_UInt);
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_taskRestoreHwi ti_sysbios_family_arp32_Hwi_taskRestoreHwi__C;

/* hooks */
#define ti_sysbios_family_arp32_Hwi_hooks (ti_sysbios_family_arp32_Hwi_hooks__C)
typedef ti_sysbios_family_arp32_Hwi_HookSet __T1_ti_sysbios_family_arp32_Hwi_hooks;
typedef struct { int length; ti_sysbios_family_arp32_Hwi_HookSet *elem; } __ARRAY1_ti_sysbios_family_arp32_Hwi_hooks;
typedef __ARRAY1_ti_sysbios_family_arp32_Hwi_hooks __TA_ti_sysbios_family_arp32_Hwi_hooks;
typedef __TA_ti_sysbios_family_arp32_Hwi_hooks CT__ti_sysbios_family_arp32_Hwi_hooks;
__extern __FAR__ const CT__ti_sysbios_family_arp32_Hwi_hooks ti_sysbios_family_arp32_Hwi_hooks__C;


/*
 * ======== PER-INSTANCE TYPES ========
 */

/* Params */
struct ti_sysbios_family_arp32_Hwi_Params {
    size_t __size;
    const void* __self;
    void* __fxns;
    xdc_runtime_IInstance_Params* instance;
    ti_sysbios_interfaces_IHwi_MaskingOption maskSetting;
    xdc_UArg arg;
    xdc_Bool enableInt;
    xdc_Int eventId;
    xdc_Int priority;
    xdc_Bits16 disableIerMask;
    xdc_Bits16 restoreIerMask;
    xdc_Int vectorNum;
    xdc_runtime_IInstance_Params __iprms;
};

/* Struct */
struct ti_sysbios_family_arp32_Hwi_Struct {
    const ti_sysbios_family_arp32_Hwi_Fxns__* __fxns;
    xdc_Bits16 __f0;
    xdc_Bits16 __f1;
    xdc_Int16 __f2;
    xdc_Int16 __f3;
    xdc_UInt __f4;
    xdc_UArg __f5;
    ti_sysbios_family_arp32_Hwi_FuncPtr __f6;
    ti_sysbios_family_arp32_Hwi_Irp __f7;
    __TA_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv __f8;
    xdc_runtime_Types_CordAddr __name;
};


/*
 * ======== VIRTUAL FUNCTIONS ========
 */

/* Fxns__ */
struct ti_sysbios_family_arp32_Hwi_Fxns__ {
    xdc_runtime_Types_Base* __base;
    const xdc_runtime_Types_SysFxns2* __sysp;
    xdc_Bool (*getStackInfo)(ti_sysbios_interfaces_IHwi_StackInfo*, xdc_Bool);
    xdc_Void (*startup)(void);
    xdc_UInt (*disable)(void);
    xdc_UInt (*enable)(void);
    xdc_Void (*restore)(xdc_UInt);
    xdc_Void (*switchFromBootStack)(void);
    xdc_Void (*post)(xdc_UInt);
    xdc_Char* (*getTaskSP)(void);
    xdc_UInt (*disableInterrupt)(xdc_UInt);
    xdc_UInt (*enableInterrupt)(xdc_UInt);
    xdc_Void (*restoreInterrupt)(xdc_UInt, xdc_UInt);
    xdc_Void (*clearInterrupt)(xdc_UInt);
    ti_sysbios_interfaces_IHwi_FuncPtr (*getFunc)(ti_sysbios_family_arp32_Hwi_Handle, xdc_UArg*);
    xdc_Void (*setFunc)(ti_sysbios_family_arp32_Hwi_Handle, ti_sysbios_interfaces_IHwi_FuncPtr, xdc_UArg);
    xdc_Ptr (*getHookContext)(ti_sysbios_family_arp32_Hwi_Handle, xdc_Int);
    xdc_Void (*setHookContext)(ti_sysbios_family_arp32_Hwi_Handle, xdc_Int, xdc_Ptr);
    ti_sysbios_interfaces_IHwi_Irp (*getIrp)(ti_sysbios_family_arp32_Hwi_Handle);
    xdc_runtime_Types_SysFxns2 __sfxns;
};

/* Module__FXNS__C */
__extern const ti_sysbios_family_arp32_Hwi_Fxns__ ti_sysbios_family_arp32_Hwi_Module__FXNS__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arp32_Hwi_Module_startup ti_sysbios_family_arp32_Hwi_Module_startup__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Module_startup__E, "ti_sysbios_family_arp32_Hwi_Module_startup")
__extern xdc_Int ti_sysbios_family_arp32_Hwi_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Module_startup__F, "ti_sysbios_family_arp32_Hwi_Module_startup")
__extern xdc_Int ti_sysbios_family_arp32_Hwi_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Module_startup__R, "ti_sysbios_family_arp32_Hwi_Module_startup")
__extern xdc_Int ti_sysbios_family_arp32_Hwi_Module_startup__R( xdc_Int state );

/* Instance_init__F */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Instance_init__F, "ti_sysbios_family_arp32_Hwi_Instance_init")
__extern int ti_sysbios_family_arp32_Hwi_Instance_init__F( ti_sysbios_family_arp32_Hwi_Object*, xdc_Int intNum, ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn, const ti_sysbios_family_arp32_Hwi_Params*, xdc_runtime_Error_Block* );

/* Instance_finalize__F */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Instance_finalize__F, "ti_sysbios_family_arp32_Hwi_Instance_finalize")
__extern void ti_sysbios_family_arp32_Hwi_Instance_finalize__F( ti_sysbios_family_arp32_Hwi_Object* , int );

/* Instance_init__R */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Instance_init__R, "ti_sysbios_family_arp32_Hwi_Instance_init")
__extern int ti_sysbios_family_arp32_Hwi_Instance_init__R( ti_sysbios_family_arp32_Hwi_Object*, xdc_Int intNum, ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn, const ti_sysbios_family_arp32_Hwi_Params*, xdc_runtime_Error_Block* );

/* Instance_finalize__R */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Instance_finalize__R, "ti_sysbios_family_arp32_Hwi_Instance_finalize")
__extern void ti_sysbios_family_arp32_Hwi_Instance_finalize__R( ti_sysbios_family_arp32_Hwi_Object* , int );

/* Handle__label__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Handle__label__S, "ti_sysbios_family_arp32_Hwi_Handle__label")
__extern xdc_runtime_Types_Label* ti_sysbios_family_arp32_Hwi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab );

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Module__startupDone__S, "ti_sysbios_family_arp32_Hwi_Module__startupDone")
__extern xdc_Bool ti_sysbios_family_arp32_Hwi_Module__startupDone__S( void );

/* Object__create__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__create__S, "ti_sysbios_family_arp32_Hwi_Object__create")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_Object__create__S( xdc_Ptr __oa, xdc_SizeT __osz, xdc_Ptr __aa, const xdc_UChar* __pa, xdc_SizeT __psz, xdc_runtime_Error_Block* __eb );

/* Object__delete__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__delete__S, "ti_sysbios_family_arp32_Hwi_Object__delete")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_Object__delete__S( xdc_Ptr instp );

/* Object__destruct__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__destruct__S, "ti_sysbios_family_arp32_Hwi_Object__destruct")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_Object__destruct__S( xdc_Ptr objp );

/* Object__get__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__get__S, "ti_sysbios_family_arp32_Hwi_Object__get")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_Object__get__S( xdc_Ptr oarr, xdc_Int i );

/* Object__first__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__first__S, "ti_sysbios_family_arp32_Hwi_Object__first")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_Object__first__S( void );

/* Object__next__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Object__next__S, "ti_sysbios_family_arp32_Hwi_Object__next")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_Object__next__S( xdc_Ptr obj );

/* Params__init__S */
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_Params__init__S, "ti_sysbios_family_arp32_Hwi_Params__init")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_Params__init__S( xdc_Ptr dst, const xdc_Void* src, xdc_SizeT psz, xdc_SizeT isz );

/* getStackInfo__E */
#define ti_sysbios_family_arp32_Hwi_getStackInfo ti_sysbios_family_arp32_Hwi_getStackInfo__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getStackInfo__E, "ti_sysbios_family_arp32_Hwi_getStackInfo")
__extern xdc_Bool ti_sysbios_family_arp32_Hwi_getStackInfo__E( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getStackInfo__F, "ti_sysbios_family_arp32_Hwi_getStackInfo")
__extern xdc_Bool ti_sysbios_family_arp32_Hwi_getStackInfo__F( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth );
__extern xdc_Bool ti_sysbios_family_arp32_Hwi_getStackInfo__R( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth );

/* startup__E */
#define ti_sysbios_family_arp32_Hwi_startup ti_sysbios_family_arp32_Hwi_startup__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_startup__E, "ti_sysbios_family_arp32_Hwi_startup")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_startup__E( void );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_startup__F, "ti_sysbios_family_arp32_Hwi_startup")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_startup__F( void );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_startup__R( void );

/* switchFromBootStack__E */
#define ti_sysbios_family_arp32_Hwi_switchFromBootStack ti_sysbios_family_arp32_Hwi_switchFromBootStack__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_switchFromBootStack__E, "ti_sysbios_family_arp32_Hwi_switchFromBootStack")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_switchFromBootStack__E( void );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_switchFromBootStack__F, "ti_sysbios_family_arp32_Hwi_switchFromBootStack")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_switchFromBootStack__F( void );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_switchFromBootStack__R( void );

/* post__E */
#define ti_sysbios_family_arp32_Hwi_post ti_sysbios_family_arp32_Hwi_post__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_post__E, "ti_sysbios_family_arp32_Hwi_post")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_post__E( xdc_UInt intNum );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_post__F, "ti_sysbios_family_arp32_Hwi_post")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_post__F( xdc_UInt intNum );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_post__R( xdc_UInt intNum );

/* getTaskSP__E */
#define ti_sysbios_family_arp32_Hwi_getTaskSP ti_sysbios_family_arp32_Hwi_getTaskSP__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getTaskSP__E, "ti_sysbios_family_arp32_Hwi_getTaskSP")
__extern xdc_Char* ti_sysbios_family_arp32_Hwi_getTaskSP__E( void );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getTaskSP__F, "ti_sysbios_family_arp32_Hwi_getTaskSP")
__extern xdc_Char* ti_sysbios_family_arp32_Hwi_getTaskSP__F( void );
__extern xdc_Char* ti_sysbios_family_arp32_Hwi_getTaskSP__R( void );

/* disableInterrupt__E */
#define ti_sysbios_family_arp32_Hwi_disableInterrupt ti_sysbios_family_arp32_Hwi_disableInterrupt__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_disableInterrupt__E, "ti_sysbios_family_arp32_Hwi_disableInterrupt")
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_disableInterrupt__E( xdc_UInt intNum );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_disableInterrupt__F, "ti_sysbios_family_arp32_Hwi_disableInterrupt")
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_disableInterrupt__F( xdc_UInt intNum );
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_disableInterrupt__R( xdc_UInt intNum );

/* enableInterrupt__E */
#define ti_sysbios_family_arp32_Hwi_enableInterrupt ti_sysbios_family_arp32_Hwi_enableInterrupt__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_enableInterrupt__E, "ti_sysbios_family_arp32_Hwi_enableInterrupt")
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_enableInterrupt__E( xdc_UInt intNum );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_enableInterrupt__F, "ti_sysbios_family_arp32_Hwi_enableInterrupt")
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_enableInterrupt__F( xdc_UInt intNum );
__extern xdc_UInt ti_sysbios_family_arp32_Hwi_enableInterrupt__R( xdc_UInt intNum );

/* restoreInterrupt__E */
#define ti_sysbios_family_arp32_Hwi_restoreInterrupt ti_sysbios_family_arp32_Hwi_restoreInterrupt__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_restoreInterrupt__E, "ti_sysbios_family_arp32_Hwi_restoreInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_restoreInterrupt__E( xdc_UInt intNum, xdc_UInt key );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_restoreInterrupt__F, "ti_sysbios_family_arp32_Hwi_restoreInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_restoreInterrupt__F( xdc_UInt intNum, xdc_UInt key );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_restoreInterrupt__R( xdc_UInt intNum, xdc_UInt key );

/* clearInterrupt__E */
#define ti_sysbios_family_arp32_Hwi_clearInterrupt ti_sysbios_family_arp32_Hwi_clearInterrupt__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_clearInterrupt__E, "ti_sysbios_family_arp32_Hwi_clearInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_clearInterrupt__E( xdc_UInt intNum );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_clearInterrupt__F, "ti_sysbios_family_arp32_Hwi_clearInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_clearInterrupt__F( xdc_UInt intNum );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_clearInterrupt__R( xdc_UInt intNum );

/* getFunc__E */
#define ti_sysbios_family_arp32_Hwi_getFunc ti_sysbios_family_arp32_Hwi_getFunc__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getFunc__E, "ti_sysbios_family_arp32_Hwi_getFunc")
__extern ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_family_arp32_Hwi_getFunc__E( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_UArg* arg );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getFunc__F, "ti_sysbios_family_arp32_Hwi_getFunc")
__extern ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_family_arp32_Hwi_getFunc__F( ti_sysbios_family_arp32_Hwi_Object* __inst, xdc_UArg* arg );
__extern ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_family_arp32_Hwi_getFunc__R( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_UArg* arg );

/* setFunc__E */
#define ti_sysbios_family_arp32_Hwi_setFunc ti_sysbios_family_arp32_Hwi_setFunc__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_setFunc__E, "ti_sysbios_family_arp32_Hwi_setFunc")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setFunc__E( ti_sysbios_family_arp32_Hwi_Handle __inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_setFunc__F, "ti_sysbios_family_arp32_Hwi_setFunc")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setFunc__F( ti_sysbios_family_arp32_Hwi_Object* __inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setFunc__R( ti_sysbios_family_arp32_Hwi_Handle __inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg );

/* getHookContext__E */
#define ti_sysbios_family_arp32_Hwi_getHookContext ti_sysbios_family_arp32_Hwi_getHookContext__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getHookContext__E, "ti_sysbios_family_arp32_Hwi_getHookContext")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_getHookContext__E( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_Int id );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getHookContext__F, "ti_sysbios_family_arp32_Hwi_getHookContext")
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_getHookContext__F( ti_sysbios_family_arp32_Hwi_Object* __inst, xdc_Int id );
__extern xdc_Ptr ti_sysbios_family_arp32_Hwi_getHookContext__R( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_Int id );

/* setHookContext__E */
#define ti_sysbios_family_arp32_Hwi_setHookContext ti_sysbios_family_arp32_Hwi_setHookContext__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_setHookContext__E, "ti_sysbios_family_arp32_Hwi_setHookContext")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setHookContext__E( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_Int id, xdc_Ptr hookContext );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_setHookContext__F, "ti_sysbios_family_arp32_Hwi_setHookContext")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setHookContext__F( ti_sysbios_family_arp32_Hwi_Object* __inst, xdc_Int id, xdc_Ptr hookContext );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_setHookContext__R( ti_sysbios_family_arp32_Hwi_Handle __inst, xdc_Int id, xdc_Ptr hookContext );

/* getIrp__E */
#define ti_sysbios_family_arp32_Hwi_getIrp ti_sysbios_family_arp32_Hwi_getIrp__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getIrp__E, "ti_sysbios_family_arp32_Hwi_getIrp")
__extern ti_sysbios_interfaces_IHwi_Irp ti_sysbios_family_arp32_Hwi_getIrp__E( ti_sysbios_family_arp32_Hwi_Handle __inst );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getIrp__F, "ti_sysbios_family_arp32_Hwi_getIrp")
__extern ti_sysbios_interfaces_IHwi_Irp ti_sysbios_family_arp32_Hwi_getIrp__F( ti_sysbios_family_arp32_Hwi_Object* __inst );
__extern ti_sysbios_interfaces_IHwi_Irp ti_sysbios_family_arp32_Hwi_getIrp__R( ti_sysbios_family_arp32_Hwi_Handle __inst );

/* selfLoop__E */
#define ti_sysbios_family_arp32_Hwi_selfLoop ti_sysbios_family_arp32_Hwi_selfLoop__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_selfLoop__E, "ti_sysbios_family_arp32_Hwi_selfLoop")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_selfLoop__E( xdc_UArg arg );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_selfLoop__F, "ti_sysbios_family_arp32_Hwi_selfLoop")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_selfLoop__F( xdc_UArg arg );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_selfLoop__R( xdc_UArg arg );

/* getHandle__E */
#define ti_sysbios_family_arp32_Hwi_getHandle ti_sysbios_family_arp32_Hwi_getHandle__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getHandle__E, "ti_sysbios_family_arp32_Hwi_getHandle")
__extern ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_getHandle__E( xdc_UInt intNum );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getHandle__F, "ti_sysbios_family_arp32_Hwi_getHandle")
__extern ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_getHandle__F( xdc_UInt intNum );
__extern ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_getHandle__R( xdc_UInt intNum );

/* disableIER__E */
#define ti_sysbios_family_arp32_Hwi_disableIER ti_sysbios_family_arp32_Hwi_disableIER__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_disableIER__E, "ti_sysbios_family_arp32_Hwi_disableIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_disableIER__E( xdc_Bits16 mask );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_disableIER__F, "ti_sysbios_family_arp32_Hwi_disableIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_disableIER__F( xdc_Bits16 mask );
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_disableIER__R( xdc_Bits16 mask );

/* enableIER__E */
#define ti_sysbios_family_arp32_Hwi_enableIER ti_sysbios_family_arp32_Hwi_enableIER__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_enableIER__E, "ti_sysbios_family_arp32_Hwi_enableIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_enableIER__E( xdc_Bits16 mask );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_enableIER__F, "ti_sysbios_family_arp32_Hwi_enableIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_enableIER__F( xdc_Bits16 mask );
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_enableIER__R( xdc_Bits16 mask );

/* restoreIER__E */
#define ti_sysbios_family_arp32_Hwi_restoreIER ti_sysbios_family_arp32_Hwi_restoreIER__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_restoreIER__E, "ti_sysbios_family_arp32_Hwi_restoreIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_restoreIER__E( xdc_Bits16 mask );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_restoreIER__F, "ti_sysbios_family_arp32_Hwi_restoreIER")
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_restoreIER__F( xdc_Bits16 mask );
__extern xdc_Bits16 ti_sysbios_family_arp32_Hwi_restoreIER__R( xdc_Bits16 mask );

/* reconfig__E */
#define ti_sysbios_family_arp32_Hwi_reconfig ti_sysbios_family_arp32_Hwi_reconfig__E
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_reconfig__E, "ti_sysbios_family_arp32_Hwi_reconfig")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_reconfig__E( ti_sysbios_family_arp32_Hwi_Handle __inst, ti_sysbios_family_arp32_Hwi_FuncPtr fxn, const ti_sysbios_family_arp32_Hwi_Params* params );
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_reconfig__F, "ti_sysbios_family_arp32_Hwi_reconfig")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_reconfig__F( ti_sysbios_family_arp32_Hwi_Object* __inst, ti_sysbios_family_arp32_Hwi_FuncPtr fxn, const ti_sysbios_family_arp32_Hwi_Params* params );
__extern xdc_Void ti_sysbios_family_arp32_Hwi_reconfig__R( ti_sysbios_family_arp32_Hwi_Handle __inst, ti_sysbios_family_arp32_Hwi_FuncPtr fxn, const ti_sysbios_family_arp32_Hwi_Params* params );

/* getIsrStackAddress__I */
#define ti_sysbios_family_arp32_Hwi_getIsrStackAddress ti_sysbios_family_arp32_Hwi_getIsrStackAddress__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_getIsrStackAddress__I, "ti_sysbios_family_arp32_Hwi_getIsrStackAddress")
__extern xdc_Char* ti_sysbios_family_arp32_Hwi_getIsrStackAddress__I( void );

/* dispatchC__I */
#define ti_sysbios_family_arp32_Hwi_dispatchC ti_sysbios_family_arp32_Hwi_dispatchC__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispatchC__I, "ti_sysbios_family_arp32_Hwi_dispatchC")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispatchC__I( xdc_Int vectorNum );

/* dispatchCore__I */
#define ti_sysbios_family_arp32_Hwi_dispatchCore ti_sysbios_family_arp32_Hwi_dispatchCore__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispatchCore__I, "ti_sysbios_family_arp32_Hwi_dispatchCore")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispatchCore__I( xdc_Int vectorNum );

/* switchAndDispatch__I */
#define ti_sysbios_family_arp32_Hwi_switchAndDispatch ti_sysbios_family_arp32_Hwi_switchAndDispatch__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_switchAndDispatch__I, "ti_sysbios_family_arp32_Hwi_switchAndDispatch")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_switchAndDispatch__I( xdc_Int vectNum );

/* unPluggedInterrupt__I */
#define ti_sysbios_family_arp32_Hwi_unPluggedInterrupt ti_sysbios_family_arp32_Hwi_unPluggedInterrupt__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_unPluggedInterrupt__I, "ti_sysbios_family_arp32_Hwi_unPluggedInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_unPluggedInterrupt__I( void );

/* nmiInterrupt__I */
#define ti_sysbios_family_arp32_Hwi_nmiInterrupt ti_sysbios_family_arp32_Hwi_nmiInterrupt__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_nmiInterrupt__I, "ti_sysbios_family_arp32_Hwi_nmiInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_nmiInterrupt__I( void );

/* swiInterrupt__I */
#define ti_sysbios_family_arp32_Hwi_swiInterrupt ti_sysbios_family_arp32_Hwi_swiInterrupt__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_swiInterrupt__I, "ti_sysbios_family_arp32_Hwi_swiInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_swiInterrupt__I( void );

/* undefInterrupt__I */
#define ti_sysbios_family_arp32_Hwi_undefInterrupt ti_sysbios_family_arp32_Hwi_undefInterrupt__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_undefInterrupt__I, "ti_sysbios_family_arp32_Hwi_undefInterrupt")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_undefInterrupt__I( void );

/* dispIsr4__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr4 ti_sysbios_family_arp32_Hwi_dispIsr4__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr4__I, "ti_sysbios_family_arp32_Hwi_dispIsr4")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr4__I( void );

/* dispIsr5__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr5 ti_sysbios_family_arp32_Hwi_dispIsr5__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr5__I, "ti_sysbios_family_arp32_Hwi_dispIsr5")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr5__I( void );

/* dispIsr6__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr6 ti_sysbios_family_arp32_Hwi_dispIsr6__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr6__I, "ti_sysbios_family_arp32_Hwi_dispIsr6")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr6__I( void );

/* dispIsr7__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr7 ti_sysbios_family_arp32_Hwi_dispIsr7__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr7__I, "ti_sysbios_family_arp32_Hwi_dispIsr7")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr7__I( void );

/* dispIsr8__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr8 ti_sysbios_family_arp32_Hwi_dispIsr8__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr8__I, "ti_sysbios_family_arp32_Hwi_dispIsr8")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr8__I( void );

/* dispIsr9__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr9 ti_sysbios_family_arp32_Hwi_dispIsr9__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr9__I, "ti_sysbios_family_arp32_Hwi_dispIsr9")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr9__I( void );

/* dispIsr10__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr10 ti_sysbios_family_arp32_Hwi_dispIsr10__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr10__I, "ti_sysbios_family_arp32_Hwi_dispIsr10")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr10__I( void );

/* dispIsr11__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr11 ti_sysbios_family_arp32_Hwi_dispIsr11__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr11__I, "ti_sysbios_family_arp32_Hwi_dispIsr11")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr11__I( void );

/* dispIsr12__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr12 ti_sysbios_family_arp32_Hwi_dispIsr12__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr12__I, "ti_sysbios_family_arp32_Hwi_dispIsr12")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr12__I( void );

/* dispIsr13__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr13 ti_sysbios_family_arp32_Hwi_dispIsr13__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr13__I, "ti_sysbios_family_arp32_Hwi_dispIsr13")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr13__I( void );

/* dispIsr14__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr14 ti_sysbios_family_arp32_Hwi_dispIsr14__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr14__I, "ti_sysbios_family_arp32_Hwi_dispIsr14")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr14__I( void );

/* dispIsr15__I */
#define ti_sysbios_family_arp32_Hwi_dispIsr15 ti_sysbios_family_arp32_Hwi_dispIsr15__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_dispIsr15__I, "ti_sysbios_family_arp32_Hwi_dispIsr15")
__extern xdc_Void ti_sysbios_family_arp32_Hwi_dispIsr15__I( void );

/* postInit__I */
#define ti_sysbios_family_arp32_Hwi_postInit ti_sysbios_family_arp32_Hwi_postInit__I
xdc__CODESECT(ti_sysbios_family_arp32_Hwi_postInit__I, "ti_sysbios_family_arp32_Hwi_postInit")
__extern xdc_Int ti_sysbios_family_arp32_Hwi_postInit__I( ti_sysbios_family_arp32_Hwi_Object* hwi, xdc_runtime_Error_Block* eb );


/*
 * ======== FUNCTION SELECTORS ========
 */

/* getFunc_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_IHwi_FuncPtr (*ti_sysbios_family_arp32_Hwi_getFunc_FxnT)(void*, xdc_UArg*);
static inline ti_sysbios_family_arp32_Hwi_getFunc_FxnT ti_sysbios_family_arp32_Hwi_getFunc_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_getFunc_FxnT)ti_sysbios_family_arp32_Hwi_getFunc; 
}

/* setFunc_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_family_arp32_Hwi_setFunc_FxnT)(void*, ti_sysbios_interfaces_IHwi_FuncPtr, xdc_UArg);
static inline ti_sysbios_family_arp32_Hwi_setFunc_FxnT ti_sysbios_family_arp32_Hwi_setFunc_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_setFunc_FxnT)ti_sysbios_family_arp32_Hwi_setFunc; 
}

/* getHookContext_{FxnT,fxnP} */
typedef xdc_Ptr (*ti_sysbios_family_arp32_Hwi_getHookContext_FxnT)(void*, xdc_Int);
static inline ti_sysbios_family_arp32_Hwi_getHookContext_FxnT ti_sysbios_family_arp32_Hwi_getHookContext_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_getHookContext_FxnT)ti_sysbios_family_arp32_Hwi_getHookContext; 
}

/* setHookContext_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_family_arp32_Hwi_setHookContext_FxnT)(void*, xdc_Int, xdc_Ptr);
static inline ti_sysbios_family_arp32_Hwi_setHookContext_FxnT ti_sysbios_family_arp32_Hwi_setHookContext_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_setHookContext_FxnT)ti_sysbios_family_arp32_Hwi_setHookContext; 
}

/* getIrp_{FxnT,fxnP} */
typedef ti_sysbios_interfaces_IHwi_Irp (*ti_sysbios_family_arp32_Hwi_getIrp_FxnT)(void*);
static inline ti_sysbios_family_arp32_Hwi_getIrp_FxnT ti_sysbios_family_arp32_Hwi_getIrp_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_getIrp_FxnT)ti_sysbios_family_arp32_Hwi_getIrp; 
}

/* reconfig_{FxnT,fxnP} */
typedef xdc_Void (*ti_sysbios_family_arp32_Hwi_reconfig_FxnT)(void*, ti_sysbios_family_arp32_Hwi_FuncPtr, const ti_sysbios_family_arp32_Hwi_Params*);
static inline ti_sysbios_family_arp32_Hwi_reconfig_FxnT ti_sysbios_family_arp32_Hwi_reconfig_fxnP( void )
{
    return (ti_sysbios_family_arp32_Hwi_reconfig_FxnT)ti_sysbios_family_arp32_Hwi_reconfig; 
}


/*
 * ======== CONVERTORS ========
 */

/* Module_upCast */
static inline ti_sysbios_interfaces_IHwi_Module ti_sysbios_family_arp32_Hwi_Module_upCast( void )
{
    return (ti_sysbios_interfaces_IHwi_Module)&ti_sysbios_family_arp32_Hwi_Module__FXNS__C;
}

/* Module_to_ti_sysbios_interfaces_IHwi */
#define ti_sysbios_family_arp32_Hwi_Module_to_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Module_upCast

/* Handle_upCast */
static inline ti_sysbios_interfaces_IHwi_Handle ti_sysbios_family_arp32_Hwi_Handle_upCast( ti_sysbios_family_arp32_Hwi_Handle i )
{
    return (ti_sysbios_interfaces_IHwi_Handle)i;
}

/* Handle_to_ti_sysbios_interfaces_IHwi */
#define ti_sysbios_family_arp32_Hwi_Handle_to_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Handle_upCast

/* Handle_downCast */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_Handle_downCast( ti_sysbios_interfaces_IHwi_Handle i )
{
    ti_sysbios_interfaces_IHwi_Handle i2 = (ti_sysbios_interfaces_IHwi_Handle)i;
    return (void*)i2->__fxns == (void*)&ti_sysbios_family_arp32_Hwi_Module__FXNS__C ? (ti_sysbios_family_arp32_Hwi_Handle)i : 0;
}

/* Handle_from_ti_sysbios_interfaces_IHwi */
#define ti_sysbios_family_arp32_Hwi_Handle_from_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Handle_downCast


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arp32_Hwi_Module_startupDone() ti_sysbios_family_arp32_Hwi_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arp32_Hwi_Object_heap() ti_sysbios_family_arp32_Hwi_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arp32_Hwi_Module_heap() ti_sysbios_family_arp32_Hwi_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arp32_Hwi_Module__id ti_sysbios_family_arp32_Hwi_Module_id( void ) 
{
    return ti_sysbios_family_arp32_Hwi_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arp32_Hwi_Module_hasMask( void ) 
{
    return ti_sysbios_family_arp32_Hwi_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arp32_Hwi_Module_getMask( void ) 
{
    return ti_sysbios_family_arp32_Hwi_Module__diagsMask__C != NULL ? *ti_sysbios_family_arp32_Hwi_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arp32_Hwi_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_family_arp32_Hwi_Module__diagsMask__C != NULL) *ti_sysbios_family_arp32_Hwi_Module__diagsMask__C = mask;
}

/* Params_init */
static inline void ti_sysbios_family_arp32_Hwi_Params_init( ti_sysbios_family_arp32_Hwi_Params* prms ) 
{
    if (prms) {
        ti_sysbios_family_arp32_Hwi_Params__init__S(prms, 0, sizeof(ti_sysbios_family_arp32_Hwi_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Params_copy */
static inline void ti_sysbios_family_arp32_Hwi_Params_copy( ti_sysbios_family_arp32_Hwi_Params* dst, const ti_sysbios_family_arp32_Hwi_Params* src ) 
{
    if (dst) {
        ti_sysbios_family_arp32_Hwi_Params__init__S(dst, (const void *)src, sizeof(ti_sysbios_family_arp32_Hwi_Params), sizeof(xdc_runtime_IInstance_Params));
    }
}

/* Object_count */
#define ti_sysbios_family_arp32_Hwi_Object_count() ti_sysbios_family_arp32_Hwi_Object__count__C

/* Object_sizeof */
#define ti_sysbios_family_arp32_Hwi_Object_sizeof() ti_sysbios_family_arp32_Hwi_Object__sizeof__C

/* Object_get */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_Object_get( ti_sysbios_family_arp32_Hwi_Instance_State* oarr, int i ) 
{
    return (ti_sysbios_family_arp32_Hwi_Handle)ti_sysbios_family_arp32_Hwi_Object__get__S(oarr, i);
}

/* Object_first */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_Object_first( void )
{
    return (ti_sysbios_family_arp32_Hwi_Handle)ti_sysbios_family_arp32_Hwi_Object__first__S();
}

/* Object_next */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_Object_next( ti_sysbios_family_arp32_Hwi_Object* obj )
{
    return (ti_sysbios_family_arp32_Hwi_Handle)ti_sysbios_family_arp32_Hwi_Object__next__S(obj);
}

/* Handle_label */
static inline xdc_runtime_Types_Label* ti_sysbios_family_arp32_Hwi_Handle_label( ti_sysbios_family_arp32_Hwi_Handle inst, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_family_arp32_Hwi_Handle__label__S(inst, lab);
}

/* Handle_name */
static inline String ti_sysbios_family_arp32_Hwi_Handle_name( ti_sysbios_family_arp32_Hwi_Handle inst )
{
    xdc_runtime_Types_Label lab;
    return ti_sysbios_family_arp32_Hwi_Handle__label__S(inst, &lab)->iname;
}

/* create */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_create( xdc_Int intNum, ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn, const ti_sysbios_family_arp32_Hwi_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    ti_sysbios_family_arp32_Hwi_Args__create __args;
    __args.intNum = intNum;
    __args.hwiFxn = hwiFxn;
    return (ti_sysbios_family_arp32_Hwi_Handle)ti_sysbios_family_arp32_Hwi_Object__create__S(0, 0, &__args, (const xdc_UChar*)__prms, sizeof(ti_sysbios_family_arp32_Hwi_Params), __eb);
}

/* construct */
static inline void ti_sysbios_family_arp32_Hwi_construct( ti_sysbios_family_arp32_Hwi_Struct* __obj, xdc_Int intNum, ti_sysbios_interfaces_IHwi_FuncPtr hwiFxn, const ti_sysbios_family_arp32_Hwi_Params* __prms, xdc_runtime_Error_Block* __eb )
{
    ti_sysbios_family_arp32_Hwi_Args__create __args;
    __args.intNum = intNum;
    __args.hwiFxn = hwiFxn;
    ti_sysbios_family_arp32_Hwi_Object__create__S(__obj, sizeof (ti_sysbios_family_arp32_Hwi_Struct), &__args, (const xdc_UChar*)__prms, sizeof(ti_sysbios_family_arp32_Hwi_Params), __eb);
}

/* delete */
static inline void ti_sysbios_family_arp32_Hwi_delete( ti_sysbios_family_arp32_Hwi_Handle* instp )
{
    ti_sysbios_family_arp32_Hwi_Object__delete__S(instp);
}

/* destruct */
static inline void ti_sysbios_family_arp32_Hwi_destruct( ti_sysbios_family_arp32_Hwi_Struct* obj )
{
    ti_sysbios_family_arp32_Hwi_Object__destruct__S(obj);
}

/* handle */
static inline ti_sysbios_family_arp32_Hwi_Handle ti_sysbios_family_arp32_Hwi_handle( ti_sysbios_family_arp32_Hwi_Struct* str )
{
    return (ti_sysbios_family_arp32_Hwi_Handle)str;
}

/* struct */
static inline ti_sysbios_family_arp32_Hwi_Struct* ti_sysbios_family_arp32_Hwi_struct( ti_sysbios_family_arp32_Hwi_Handle inst )
{
    return (ti_sysbios_family_arp32_Hwi_Struct*)inst;
}


/*
 * ======== EPILOGUE ========
 */

#include <ti/sysbios/family/arp32/Hwi__epilogue.h>

#ifdef ti_sysbios_family_arp32_Hwi__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arp32_Hwi__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arp32_Hwi__internalaccess))

#ifndef ti_sysbios_family_arp32_Hwi__include_state
#define ti_sysbios_family_arp32_Hwi__include_state

/* Module_State */
struct ti_sysbios_family_arp32_Hwi_Module_State {
    __TA_ti_sysbios_family_arp32_Hwi_Module_State__interruptMask interruptMask;
    __TA_ti_sysbios_family_arp32_Hwi_Module_State__disableIerMask disableIerMask;
    __TA_ti_sysbios_family_arp32_Hwi_Module_State__restoreIerMask restoreIerMask;
    volatile xdc_Int vectNum;
    xdc_Char* taskSP;
    xdc_Char* isrStack;
    __TA_ti_sysbios_family_arp32_Hwi_Module_State__irp irp;
    __TA_ti_sysbios_family_arp32_Hwi_Module_State__dispatchTable dispatchTable;
};

/* Module__state__V */
extern struct ti_sysbios_family_arp32_Hwi_Module_State__ ti_sysbios_family_arp32_Hwi_Module__state__V;

/* Object */
struct ti_sysbios_family_arp32_Hwi_Object {
    const ti_sysbios_family_arp32_Hwi_Fxns__* __fxns;
    xdc_Bits16 disableIerMask;
    xdc_Bits16 restoreIerMask;
    xdc_Int16 vectorNum;
    xdc_Int16 index;
    xdc_UInt intNum;
    xdc_UArg arg;
    ti_sysbios_family_arp32_Hwi_FuncPtr fxn;
    ti_sysbios_family_arp32_Hwi_Irp irp;
    __TA_ti_sysbios_family_arp32_Hwi_Instance_State__hookEnv hookEnv;
};

#endif /* ti_sysbios_family_arp32_Hwi__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arp32_Hwi__nolocalnames)

#ifndef ti_sysbios_family_arp32_Hwi__localnames__done
#define ti_sysbios_family_arp32_Hwi__localnames__done

/* module prefix */
#define Hwi_Instance ti_sysbios_family_arp32_Hwi_Instance
#define Hwi_Handle ti_sysbios_family_arp32_Hwi_Handle
#define Hwi_Module ti_sysbios_family_arp32_Hwi_Module
#define Hwi_Object ti_sysbios_family_arp32_Hwi_Object
#define Hwi_Struct ti_sysbios_family_arp32_Hwi_Struct
#define Hwi_FuncPtr ti_sysbios_family_arp32_Hwi_FuncPtr
#define Hwi_Irp ti_sysbios_family_arp32_Hwi_Irp
#define Hwi_HookSet ti_sysbios_family_arp32_Hwi_HookSet
#define Hwi_MaskingOption ti_sysbios_family_arp32_Hwi_MaskingOption
#define Hwi_StackInfo ti_sysbios_family_arp32_Hwi_StackInfo
#define Hwi_PlugFuncPtr ti_sysbios_family_arp32_Hwi_PlugFuncPtr
#define Hwi_IntcRegs ti_sysbios_family_arp32_Hwi_IntcRegs
#define Hwi_Instance_State ti_sysbios_family_arp32_Hwi_Instance_State
#define Hwi_Module_State ti_sysbios_family_arp32_Hwi_Module_State
#define Hwi_MaskingOption_NONE ti_sysbios_family_arp32_Hwi_MaskingOption_NONE
#define Hwi_MaskingOption_ALL ti_sysbios_family_arp32_Hwi_MaskingOption_ALL
#define Hwi_MaskingOption_SELF ti_sysbios_family_arp32_Hwi_MaskingOption_SELF
#define Hwi_MaskingOption_BITMASK ti_sysbios_family_arp32_Hwi_MaskingOption_BITMASK
#define Hwi_MaskingOption_LOWER ti_sysbios_family_arp32_Hwi_MaskingOption_LOWER
#define Hwi_dispatcherAutoNestingSupport ti_sysbios_family_arp32_Hwi_dispatcherAutoNestingSupport
#define Hwi_dispatcherSwiSupport ti_sysbios_family_arp32_Hwi_dispatcherSwiSupport
#define Hwi_dispatcherTaskSupport ti_sysbios_family_arp32_Hwi_dispatcherTaskSupport
#define Hwi_dispatcherIrpTrackingSupport ti_sysbios_family_arp32_Hwi_dispatcherIrpTrackingSupport
#define Hwi_NUM_INTERRUPTS ti_sysbios_family_arp32_Hwi_NUM_INTERRUPTS
#define Hwi_NUM_VECTORS ti_sysbios_family_arp32_Hwi_NUM_VECTORS
#define Hwi_A_badIntNum ti_sysbios_family_arp32_Hwi_A_badIntNum
#define Hwi_A_badVectNum ti_sysbios_family_arp32_Hwi_A_badVectNum
#define Hwi_E_alreadyDefined ti_sysbios_family_arp32_Hwi_E_alreadyDefined
#define Hwi_E_notImplemented ti_sysbios_family_arp32_Hwi_E_notImplemented
#define Hwi_LM_begin ti_sysbios_family_arp32_Hwi_LM_begin
#define Hwi_LD_end ti_sysbios_family_arp32_Hwi_LD_end
#define Hwi_initialIerMask ti_sysbios_family_arp32_Hwi_initialIerMask
#define Hwi_INTCREGSBASEADDRS ti_sysbios_family_arp32_Hwi_INTCREGSBASEADDRS
#define Hwi_swiDisable ti_sysbios_family_arp32_Hwi_swiDisable
#define Hwi_swiRestoreHwi ti_sysbios_family_arp32_Hwi_swiRestoreHwi
#define Hwi_taskDisable ti_sysbios_family_arp32_Hwi_taskDisable
#define Hwi_taskRestoreHwi ti_sysbios_family_arp32_Hwi_taskRestoreHwi
#define Hwi_hooks ti_sysbios_family_arp32_Hwi_hooks
#define Hwi_Params ti_sysbios_family_arp32_Hwi_Params
#define Hwi_getStackInfo ti_sysbios_family_arp32_Hwi_getStackInfo
#define Hwi_startup ti_sysbios_family_arp32_Hwi_startup
#define Hwi_switchFromBootStack ti_sysbios_family_arp32_Hwi_switchFromBootStack
#define Hwi_post ti_sysbios_family_arp32_Hwi_post
#define Hwi_getTaskSP ti_sysbios_family_arp32_Hwi_getTaskSP
#define Hwi_disableInterrupt ti_sysbios_family_arp32_Hwi_disableInterrupt
#define Hwi_enableInterrupt ti_sysbios_family_arp32_Hwi_enableInterrupt
#define Hwi_restoreInterrupt ti_sysbios_family_arp32_Hwi_restoreInterrupt
#define Hwi_clearInterrupt ti_sysbios_family_arp32_Hwi_clearInterrupt
#define Hwi_getFunc ti_sysbios_family_arp32_Hwi_getFunc
#define Hwi_getFunc_fxnP ti_sysbios_family_arp32_Hwi_getFunc_fxnP
#define Hwi_getFunc_FxnT ti_sysbios_family_arp32_Hwi_getFunc_FxnT
#define Hwi_setFunc ti_sysbios_family_arp32_Hwi_setFunc
#define Hwi_setFunc_fxnP ti_sysbios_family_arp32_Hwi_setFunc_fxnP
#define Hwi_setFunc_FxnT ti_sysbios_family_arp32_Hwi_setFunc_FxnT
#define Hwi_getHookContext ti_sysbios_family_arp32_Hwi_getHookContext
#define Hwi_getHookContext_fxnP ti_sysbios_family_arp32_Hwi_getHookContext_fxnP
#define Hwi_getHookContext_FxnT ti_sysbios_family_arp32_Hwi_getHookContext_FxnT
#define Hwi_setHookContext ti_sysbios_family_arp32_Hwi_setHookContext
#define Hwi_setHookContext_fxnP ti_sysbios_family_arp32_Hwi_setHookContext_fxnP
#define Hwi_setHookContext_FxnT ti_sysbios_family_arp32_Hwi_setHookContext_FxnT
#define Hwi_getIrp ti_sysbios_family_arp32_Hwi_getIrp
#define Hwi_getIrp_fxnP ti_sysbios_family_arp32_Hwi_getIrp_fxnP
#define Hwi_getIrp_FxnT ti_sysbios_family_arp32_Hwi_getIrp_FxnT
#define Hwi_disable ti_sysbios_family_arp32_Hwi_disable
#define Hwi_enable ti_sysbios_family_arp32_Hwi_enable
#define Hwi_restore ti_sysbios_family_arp32_Hwi_restore
#define Hwi_selfLoop ti_sysbios_family_arp32_Hwi_selfLoop
#define Hwi_getHandle ti_sysbios_family_arp32_Hwi_getHandle
#define Hwi_disableIER ti_sysbios_family_arp32_Hwi_disableIER
#define Hwi_enableIER ti_sysbios_family_arp32_Hwi_enableIER
#define Hwi_restoreIER ti_sysbios_family_arp32_Hwi_restoreIER
#define Hwi_reconfig ti_sysbios_family_arp32_Hwi_reconfig
#define Hwi_reconfig_fxnP ti_sysbios_family_arp32_Hwi_reconfig_fxnP
#define Hwi_reconfig_FxnT ti_sysbios_family_arp32_Hwi_reconfig_FxnT
#define Hwi_Module_name ti_sysbios_family_arp32_Hwi_Module_name
#define Hwi_Module_id ti_sysbios_family_arp32_Hwi_Module_id
#define Hwi_Module_startup ti_sysbios_family_arp32_Hwi_Module_startup
#define Hwi_Module_startupDone ti_sysbios_family_arp32_Hwi_Module_startupDone
#define Hwi_Module_hasMask ti_sysbios_family_arp32_Hwi_Module_hasMask
#define Hwi_Module_getMask ti_sysbios_family_arp32_Hwi_Module_getMask
#define Hwi_Module_setMask ti_sysbios_family_arp32_Hwi_Module_setMask
#define Hwi_Object_heap ti_sysbios_family_arp32_Hwi_Object_heap
#define Hwi_Module_heap ti_sysbios_family_arp32_Hwi_Module_heap
#define Hwi_construct ti_sysbios_family_arp32_Hwi_construct
#define Hwi_create ti_sysbios_family_arp32_Hwi_create
#define Hwi_handle ti_sysbios_family_arp32_Hwi_handle
#define Hwi_struct ti_sysbios_family_arp32_Hwi_struct
#define Hwi_Handle_label ti_sysbios_family_arp32_Hwi_Handle_label
#define Hwi_Handle_name ti_sysbios_family_arp32_Hwi_Handle_name
#define Hwi_Instance_init ti_sysbios_family_arp32_Hwi_Instance_init
#define Hwi_Object_count ti_sysbios_family_arp32_Hwi_Object_count
#define Hwi_Object_get ti_sysbios_family_arp32_Hwi_Object_get
#define Hwi_Object_first ti_sysbios_family_arp32_Hwi_Object_first
#define Hwi_Object_next ti_sysbios_family_arp32_Hwi_Object_next
#define Hwi_Object_sizeof ti_sysbios_family_arp32_Hwi_Object_sizeof
#define Hwi_Params_copy ti_sysbios_family_arp32_Hwi_Params_copy
#define Hwi_Params_init ti_sysbios_family_arp32_Hwi_Params_init
#define Hwi_Instance_finalize ti_sysbios_family_arp32_Hwi_Instance_finalize
#define Hwi_delete ti_sysbios_family_arp32_Hwi_delete
#define Hwi_destruct ti_sysbios_family_arp32_Hwi_destruct
#define Hwi_Module_upCast ti_sysbios_family_arp32_Hwi_Module_upCast
#define Hwi_Module_to_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Module_to_ti_sysbios_interfaces_IHwi
#define Hwi_Handle_upCast ti_sysbios_family_arp32_Hwi_Handle_upCast
#define Hwi_Handle_to_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Handle_to_ti_sysbios_interfaces_IHwi
#define Hwi_Handle_downCast ti_sysbios_family_arp32_Hwi_Handle_downCast
#define Hwi_Handle_from_ti_sysbios_interfaces_IHwi ti_sysbios_family_arp32_Hwi_Handle_from_ti_sysbios_interfaces_IHwi

#endif /* ti_sysbios_family_arp32_Hwi__localnames__done */
#endif
