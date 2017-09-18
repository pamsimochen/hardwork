/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-y25
 */

#define __nested__
#define __config__

#include <xdc/std.h>

/*
 * ======== GENERATED SECTIONS ========
 *     
 *     MODULE INCLUDES
 *     
 *     <module-name> INTERNALS
 *     <module-name> INHERITS
 *     <module-name> VTABLE
 *     <module-name> PATCH TABLE
 *     <module-name> DECLARATIONS
 *     <module-name> OBJECT OFFSETS
 *     <module-name> TEMPLATES
 *     <module-name> INITIALIZERS
 *     <module-name> FUNCTION STUBS
 *     <module-name> PROXY BODY
 *     <module-name> OBJECT DESCRIPTOR
 *     <module-name> SYSTEM FUNCTIONS
 *     <module-name> PRAGMAS
 *     
 *     INITIALIZATION ENTRY POINT
 *     PROGRAM GLOBALS
 *     CLINK DIRECTIVES
 */


/*
 * ======== MODULE INCLUDES ========
 */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/family/arm/ducati/CTM.h>
#include <ti/sysbios/family/arm/ducati/Core.h>
#include <ti/sysbios/family/arm/ducati/GateDualCore.h>
#include <ti/sysbios/family/arm/ducati/Timer.h>
#include <ti/sysbios/family/arm/ducati/TimerSupport.h>
#include <ti/sysbios/family/arm/ducati/TimestampProvider.h>
#include <ti/sysbios/family/arm/ducati/Wugen.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/family/arm/m3/IntrinsicsSupport.h>
#include <ti/sysbios/family/arm/m3/TaskSupport.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/hal/Cache.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/hal/ammu/AMMU.h>
#include <ti/sysbios/hal/unicache/Cache.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Idle.h>
#include <ti/sysbios/knl/Intrinsics.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/syncs/SyncSem.h>
#include <ti/sysbios/timers/dmtimer/Timer.h>
#include <ti/sysbios/utils/Load.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Core.h>
#include <xdc/runtime/Defaults.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Main.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Registry.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/SysStd.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Timestamp.h>


/*
 * ======== ti.sysbios.BIOS INTERNALS ========
 */


/*
 * ======== ti.sysbios.BIOS_RtsGateProxy INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_BIOS_RtsGateProxy_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_BIOS_RtsGateProxy_Module__;

/* Module__root__V */
extern ti_sysbios_BIOS_RtsGateProxy_Module__ ti_sysbios_BIOS_RtsGateProxy_Module__root__V;

/* @@@ ti_sysbios_knl_Queue_Object__ */
typedef struct ti_sysbios_knl_Queue_Object__ {
    ti_sysbios_knl_Queue_Elem elem;
} ti_sysbios_knl_Queue_Object__;

/* @@@ ti_sysbios_knl_Queue_Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_knl_Queue_Object__ obj;
} ti_sysbios_knl_Queue_Object2__;

/* @@@ ti_sysbios_knl_Semaphore_Object__ */
typedef struct ti_sysbios_knl_Semaphore_Object__ {
    ti_sysbios_knl_Event_Handle event;
    xdc_UInt eventId;
    ti_sysbios_knl_Semaphore_Mode mode;
    volatile xdc_UInt16 count;
    ti_sysbios_knl_Queue_Object__ Object_field_pendQ;
} ti_sysbios_knl_Semaphore_Object__;

/* @@@ ti_sysbios_knl_Semaphore_Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_knl_Semaphore_Object__ obj;
} ti_sysbios_knl_Semaphore_Object2__;

/* Object__ */
typedef struct ti_sysbios_gates_GateMutex_Object__ {
    const ti_sysbios_gates_GateMutex_Fxns__* __fxns;
    ti_sysbios_knl_Task_Handle owner;
    ti_sysbios_knl_Semaphore_Object__ Object_field_sem;
} ti_sysbios_gates_GateMutex_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_gates_GateMutex_Object__ obj;
} ti_sysbios_gates_GateMutex_Object2__;

/* Object */
typedef ti_sysbios_gates_GateMutex_Object__ ti_sysbios_BIOS_RtsGateProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_BIOS_RtsGateProxy_Object__ obj;
} ti_sysbios_BIOS_RtsGateProxy_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_BIOS_RtsGateProxy___VERS
    #define ti_sysbios_BIOS_RtsGateProxy___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_BIOS_RtsGateProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.family.arm.ducati.CTM INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.Core INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_family_arm_ducati_GateDualCore_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_family_arm_ducati_GateDualCore_Module__;

/* Module__root__V */
extern ti_sysbios_family_arm_ducati_GateDualCore_Module__ ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_family_arm_ducati_GateDualCore_Object__ {
    const ti_sysbios_family_arm_ducati_GateDualCore_Fxns__* __fxns;
    xdc_UInt index;
    volatile xdc_UInt32* gatePtr;
    volatile xdc_UInt8* gateBytePtr;
    xdc_UInt stalls;
    xdc_UInt noStalls;
    xdc_UInt totalStalls;
    xdc_UInt maxStall;
} ti_sysbios_family_arm_ducati_GateDualCore_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_family_arm_ducati_GateDualCore_Object__ obj;
} ti_sysbios_family_arm_ducati_GateDualCore_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_family_arm_ducati_GateDualCore___VERS
    #define ti_sysbios_family_arm_ducati_GateDualCore___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_family_arm_ducati_GateDualCore___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.family.arm.ducati.Timer INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_family_arm_ducati_Timer_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_family_arm_ducati_Timer_Module__;

/* Module__root__V */
extern ti_sysbios_family_arm_ducati_Timer_Module__ ti_sysbios_family_arm_ducati_Timer_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_family_arm_ducati_Timer_Object__ {
    const ti_sysbios_family_arm_ducati_Timer_Fxns__* __fxns;
    xdc_Bool staticInst;
    xdc_Int id;
    xdc_Int ctmid;
    ti_sysbios_family_arm_ducati_Timer_RunMode runMode;
    ti_sysbios_family_arm_ducati_Timer_StartMode startMode;
    xdc_UInt period;
    ti_sysbios_family_arm_ducati_Timer_PeriodType periodType;
    xdc_UInt intNum;
    xdc_UArg arg;
    ti_sysbios_family_arm_m3_Hwi_FuncPtr tickFxn;
    xdc_runtime_Types_FreqHz extFreq;
    ti_sysbios_family_arm_m3_Hwi_Handle hwi;
} ti_sysbios_family_arm_ducati_Timer_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_family_arm_ducati_Timer_Object__ obj;
} ti_sysbios_family_arm_ducati_Timer_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_family_arm_ducati_Timer___VERS
    #define ti_sysbios_family_arm_ducati_Timer___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_family_arm_ducati_Timer___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.family.arm.ducati.TimerSupport INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.TimestampProvider INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.Wugen INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.m3.Hwi INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_family_arm_m3_Hwi_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_family_arm_m3_Hwi_Module__;

/* Module__root__V */
extern ti_sysbios_family_arm_m3_Hwi_Module__ ti_sysbios_family_arm_m3_Hwi_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_family_arm_m3_Hwi_Object__ {
    const ti_sysbios_family_arm_m3_Hwi_Fxns__* __fxns;
    xdc_UArg arg;
    ti_sysbios_family_arm_m3_Hwi_FuncPtr fxn;
    xdc_Int intNum;
    ti_sysbios_family_arm_m3_Hwi_Irp irp;
    __TA_ti_sysbios_family_arm_m3_Hwi_Instance_State__hookEnv hookEnv;
} ti_sysbios_family_arm_m3_Hwi_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_family_arm_m3_Hwi_Object__ obj;
} ti_sysbios_family_arm_m3_Hwi_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_family_arm_m3_Hwi___VERS
    #define ti_sysbios_family_arm_m3_Hwi___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_family_arm_m3_Hwi___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.m3.TaskSupport INTERNALS ========
 */


/*
 * ======== ti.sysbios.gates.GateHwi INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_gates_GateHwi_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_gates_GateHwi_Module__;

/* Module__root__V */
extern ti_sysbios_gates_GateHwi_Module__ ti_sysbios_gates_GateHwi_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_gates_GateHwi_Object__ {
    const ti_sysbios_gates_GateHwi_Fxns__* __fxns;
} ti_sysbios_gates_GateHwi_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_gates_GateHwi_Object__ obj;
} ti_sysbios_gates_GateHwi_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_gates_GateHwi___VERS
    #define ti_sysbios_gates_GateHwi___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_gates_GateHwi___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.gates.GateMutex INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_gates_GateMutex_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_gates_GateMutex_Module__;

/* Module__root__V */
extern ti_sysbios_gates_GateMutex_Module__ ti_sysbios_gates_GateMutex_Module__root__V;

/* <-- ti_sysbios_gates_GateMutex_Object */

/* __ParamsPtr */
#ifdef ti_sysbios_gates_GateMutex___VERS
    #define ti_sysbios_gates_GateMutex___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_gates_GateMutex___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.hal.Cache INTERNALS ========
 */


/*
 * ======== ti.sysbios.hal.Cache_CacheProxy INTERNALS ========
 */


/*
 * ======== ti.sysbios.hal.Hwi INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_hal_Hwi_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_hal_Hwi_Module__;

/* Module__root__V */
extern ti_sysbios_hal_Hwi_Module__ ti_sysbios_hal_Hwi_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_hal_Hwi_Object__ {
    const ti_sysbios_hal_Hwi_Fxns__* __fxns;
    ti_sysbios_hal_Hwi_HwiProxy_Handle pi;
} ti_sysbios_hal_Hwi_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_hal_Hwi_Object__ obj;
} ti_sysbios_hal_Hwi_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_hal_Hwi___VERS
    #define ti_sysbios_hal_Hwi___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_hal_Hwi___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.hal.Hwi_HwiProxy INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_hal_Hwi_HwiProxy_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_hal_Hwi_HwiProxy_Module__;

/* Module__root__V */
extern ti_sysbios_hal_Hwi_HwiProxy_Module__ ti_sysbios_hal_Hwi_HwiProxy_Module__root__V;

/* <-- ti_sysbios_family_arm_m3_Hwi_Object */

/* Object */
typedef ti_sysbios_family_arm_m3_Hwi_Object__ ti_sysbios_hal_Hwi_HwiProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_hal_Hwi_HwiProxy_Object__ obj;
} ti_sysbios_hal_Hwi_HwiProxy_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_hal_Hwi_HwiProxy___VERS
    #define ti_sysbios_hal_Hwi_HwiProxy___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_hal_Hwi_HwiProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.hal.Timer INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_hal_Timer_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_hal_Timer_Module__;

/* Module__root__V */
extern ti_sysbios_hal_Timer_Module__ ti_sysbios_hal_Timer_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_hal_Timer_Object__ {
    const ti_sysbios_hal_Timer_Fxns__* __fxns;
    ti_sysbios_hal_Timer_TimerProxy_Handle pi;
} ti_sysbios_hal_Timer_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_hal_Timer_Object__ obj;
} ti_sysbios_hal_Timer_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_hal_Timer___VERS
    #define ti_sysbios_hal_Timer___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_hal_Timer___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.hal.Timer_TimerProxy INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_hal_Timer_TimerProxy_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_hal_Timer_TimerProxy_Module__;

/* Module__root__V */
extern ti_sysbios_hal_Timer_TimerProxy_Module__ ti_sysbios_hal_Timer_TimerProxy_Module__root__V;

/* <-- ti_sysbios_family_arm_ducati_Timer_Object */

/* Object */
typedef ti_sysbios_family_arm_ducati_Timer_Object__ ti_sysbios_hal_Timer_TimerProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_hal_Timer_TimerProxy_Object__ obj;
} ti_sysbios_hal_Timer_TimerProxy_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_hal_Timer_TimerProxy___VERS
    #define ti_sysbios_hal_Timer_TimerProxy___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_hal_Timer_TimerProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.hal.ammu.AMMU INTERNALS ========
 */


/*
 * ======== ti.sysbios.hal.unicache.Cache INTERNALS ========
 */


/*
 * ======== ti.sysbios.hal.unicache.Cache_Module_GateProxy INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__;

/* Module__root__V */
extern ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__ ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__root__V;

/* <-- ti_sysbios_family_arm_ducati_GateDualCore_Object */

/* Object */
typedef ti_sysbios_family_arm_ducati_GateDualCore_Object__ ti_sysbios_hal_unicache_Cache_Module_GateProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_hal_unicache_Cache_Module_GateProxy_Object__ obj;
} ti_sysbios_hal_unicache_Cache_Module_GateProxy_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_hal_unicache_Cache_Module_GateProxy___VERS
    #define ti_sysbios_hal_unicache_Cache_Module_GateProxy___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_hal_unicache_Cache_Module_GateProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.heaps.HeapBuf INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_heaps_HeapBuf_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_heaps_HeapBuf_Module__;

/* Module__root__V */
extern ti_sysbios_heaps_HeapBuf_Module__ ti_sysbios_heaps_HeapBuf_Module__root__V;

/* <-- ti_sysbios_knl_Queue_Object */

/* Object__ */
typedef struct ti_sysbios_heaps_HeapBuf_Object__ {
    const ti_sysbios_heaps_HeapBuf_Fxns__* __fxns;
    xdc_SizeT blockSize;
    xdc_SizeT align;
    xdc_UInt numBlocks;
    xdc_runtime_Memory_Size bufSize;
    __TA_ti_sysbios_heaps_HeapBuf_Instance_State__buf buf;
    xdc_UInt numFreeBlocks;
    xdc_UInt minFreeBlocks;
    ti_sysbios_knl_Queue_Object__ Object_field_freeList;
} ti_sysbios_heaps_HeapBuf_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_heaps_HeapBuf_Object__ obj;
} ti_sysbios_heaps_HeapBuf_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_heaps_HeapBuf___VERS
    #define ti_sysbios_heaps_HeapBuf___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_heaps_HeapBuf___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.heaps.HeapMem INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_heaps_HeapMem_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_heaps_HeapMem_Module__;

/* Module__root__V */
extern ti_sysbios_heaps_HeapMem_Module__ ti_sysbios_heaps_HeapMem_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_heaps_HeapMem_Object__ {
    const ti_sysbios_heaps_HeapMem_Fxns__* __fxns;
    xdc_runtime_Memory_Size align;
    __TA_ti_sysbios_heaps_HeapMem_Instance_State__buf buf;
    ti_sysbios_heaps_HeapMem_Header head;
} ti_sysbios_heaps_HeapMem_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_heaps_HeapMem_Object__ obj;
} ti_sysbios_heaps_HeapMem_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_heaps_HeapMem___VERS
    #define ti_sysbios_heaps_HeapMem___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_heaps_HeapMem___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.heaps.HeapMem_Module_GateProxy INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__;

/* Module__root__V */
extern ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__ ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__root__V;

/* <-- ti_sysbios_gates_GateMutex_Object */

/* Object */
typedef ti_sysbios_gates_GateMutex_Object__ ti_sysbios_heaps_HeapMem_Module_GateProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_heaps_HeapMem_Module_GateProxy_Object__ obj;
} ti_sysbios_heaps_HeapMem_Module_GateProxy_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_heaps_HeapMem_Module_GateProxy___VERS
    #define ti_sysbios_heaps_HeapMem_Module_GateProxy___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_heaps_HeapMem_Module_GateProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Clock INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_knl_Clock_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_knl_Clock_Module__;

/* Module__root__V */
extern ti_sysbios_knl_Clock_Module__ ti_sysbios_knl_Clock_Module__root__V;

/* <-- ti_sysbios_knl_Queue_Object */

/* Object__ */
typedef struct ti_sysbios_knl_Clock_Object__ {
    ti_sysbios_knl_Queue_Elem elem;
    xdc_UInt32 timeout;
    xdc_UInt32 currTimeout;
    xdc_UInt32 period;
    volatile xdc_Bool active;
    ti_sysbios_knl_Clock_FuncPtr fxn;
    xdc_UArg arg;
} ti_sysbios_knl_Clock_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_knl_Clock_Object__ obj;
} ti_sysbios_knl_Clock_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_knl_Clock___VERS
    #define ti_sysbios_knl_Clock___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_knl_Clock___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Idle INTERNALS ========
 */


/*
 * ======== ti.sysbios.knl.Intrinsics INTERNALS ========
 */


/*
 * ======== ti.sysbios.knl.Intrinsics_SupportProxy INTERNALS ========
 */


/*
 * ======== ti.sysbios.knl.Queue INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_knl_Queue_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_knl_Queue_Module__;

/* Module__root__V */
extern ti_sysbios_knl_Queue_Module__ ti_sysbios_knl_Queue_Module__root__V;

/* <-- ti_sysbios_knl_Queue_Object */

/* __ParamsPtr */
#ifdef ti_sysbios_knl_Queue___VERS
    #define ti_sysbios_knl_Queue___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_knl_Queue___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Semaphore INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_knl_Semaphore_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_knl_Semaphore_Module__;

/* Module__root__V */
extern ti_sysbios_knl_Semaphore_Module__ ti_sysbios_knl_Semaphore_Module__root__V;

/* <-- ti_sysbios_knl_Semaphore_Object */

/* __ParamsPtr */
#ifdef ti_sysbios_knl_Semaphore___VERS
    #define ti_sysbios_knl_Semaphore___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_knl_Semaphore___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Swi INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_knl_Swi_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_knl_Swi_Module__;

/* Module__root__V */
extern ti_sysbios_knl_Swi_Module__ ti_sysbios_knl_Swi_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_knl_Swi_Object__ {
    ti_sysbios_knl_Queue_Elem qElem;
    ti_sysbios_knl_Swi_FuncPtr fxn;
    xdc_UArg arg0;
    xdc_UArg arg1;
    xdc_UInt priority;
    xdc_UInt mask;
    xdc_Bool posted;
    xdc_UInt initTrigger;
    xdc_UInt trigger;
    ti_sysbios_knl_Queue_Handle readyQ;
    __TA_ti_sysbios_knl_Swi_Instance_State__hookEnv hookEnv;
} ti_sysbios_knl_Swi_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_knl_Swi_Object__ obj;
} ti_sysbios_knl_Swi_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_knl_Swi___VERS
    #define ti_sysbios_knl_Swi___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_knl_Swi___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Task INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_knl_Task_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_knl_Task_Module__;

/* Module__root__V */
extern ti_sysbios_knl_Task_Module__ ti_sysbios_knl_Task_Module__root__V;

/* <-- ti_sysbios_knl_Queue_Object */

/* Object__ */
typedef struct ti_sysbios_knl_Task_Object__ {
    ti_sysbios_knl_Queue_Elem qElem;
    volatile xdc_Int priority;
    xdc_UInt mask;
    xdc_Ptr context;
    ti_sysbios_knl_Task_Mode mode;
    ti_sysbios_knl_Task_PendElem* pendElem;
    xdc_SizeT stackSize;
    __TA_ti_sysbios_knl_Task_Instance_State__stack stack;
    xdc_runtime_IHeap_Handle stackHeap;
    ti_sysbios_knl_Task_FuncPtr fxn;
    xdc_UArg arg0;
    xdc_UArg arg1;
    xdc_Ptr env;
    __TA_ti_sysbios_knl_Task_Instance_State__hookEnv hookEnv;
    xdc_Bool vitalTaskFlag;
    ti_sysbios_knl_Queue_Handle readyQ;
} ti_sysbios_knl_Task_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_knl_Task_Object__ obj;
} ti_sysbios_knl_Task_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_knl_Task___VERS
    #define ti_sysbios_knl_Task___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_knl_Task___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.knl.Task_SupportProxy INTERNALS ========
 */


/*
 * ======== ti.sysbios.syncs.SyncSem INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_syncs_SyncSem_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_syncs_SyncSem_Module__;

/* Module__root__V */
extern ti_sysbios_syncs_SyncSem_Module__ ti_sysbios_syncs_SyncSem_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_syncs_SyncSem_Object__ {
    const ti_sysbios_syncs_SyncSem_Fxns__* __fxns;
    xdc_Bool userSem;
    ti_sysbios_knl_Semaphore_Handle sem;
} ti_sysbios_syncs_SyncSem_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_syncs_SyncSem_Object__ obj;
} ti_sysbios_syncs_SyncSem_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_syncs_SyncSem___VERS
    #define ti_sysbios_syncs_SyncSem___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_syncs_SyncSem___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.timers.dmtimer.Timer INTERNALS ========
 */

/* Module__ */
typedef struct ti_sysbios_timers_dmtimer_Timer_Module__ {
    xdc_runtime_Types_Link link;
} ti_sysbios_timers_dmtimer_Timer_Module__;

/* Module__root__V */
extern ti_sysbios_timers_dmtimer_Timer_Module__ ti_sysbios_timers_dmtimer_Timer_Module__root__V;

/* Object__ */
typedef struct ti_sysbios_timers_dmtimer_Timer_Object__ {
    const ti_sysbios_timers_dmtimer_Timer_Fxns__* __fxns;
    xdc_Bool staticInst;
    xdc_Int id;
    xdc_UInt tiocpCfg;
    xdc_UInt tmar;
    xdc_UInt tier;
    xdc_UInt twer;
    xdc_UInt tclr;
    xdc_UInt tsicr;
    ti_sysbios_interfaces_ITimer_RunMode runMode;
    ti_sysbios_interfaces_ITimer_StartMode startMode;
    xdc_UInt period;
    ti_sysbios_interfaces_ITimer_PeriodType periodType;
    xdc_UInt intNum;
    xdc_UArg arg;
    ti_sysbios_hal_Hwi_FuncPtr tickFxn;
    xdc_runtime_Types_FreqHz extFreq;
    ti_sysbios_hal_Hwi_Handle hwi;
    xdc_UInt prevThreshold;
    xdc_UInt rollovers;
} ti_sysbios_timers_dmtimer_Timer_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    ti_sysbios_timers_dmtimer_Timer_Object__ obj;
} ti_sysbios_timers_dmtimer_Timer_Object2__;

/* __ParamsPtr */
#ifdef ti_sysbios_timers_dmtimer_Timer___VERS
    #define ti_sysbios_timers_dmtimer_Timer___ParamsPtr xdc_UChar*
#else
    #define ti_sysbios_timers_dmtimer_Timer___ParamsPtr xdc_Ptr
#endif


/*
 * ======== ti.sysbios.timers.dmtimer.Timer_TimerSupportProxy INTERNALS ========
 */


/*
 * ======== ti.sysbios.utils.Load INTERNALS ========
 */


/*
 * ======== xdc.runtime.Assert INTERNALS ========
 */


/*
 * ======== xdc.runtime.Core INTERNALS ========
 */


/*
 * ======== xdc.runtime.Defaults INTERNALS ========
 */


/*
 * ======== xdc.runtime.Diags INTERNALS ========
 */


/*
 * ======== xdc.runtime.Error INTERNALS ========
 */


/*
 * ======== xdc.runtime.Gate INTERNALS ========
 */


/*
 * ======== xdc.runtime.Log INTERNALS ========
 */


/*
 * ======== xdc.runtime.Main INTERNALS ========
 */


/*
 * ======== xdc.runtime.Main_Module_GateProxy INTERNALS ========
 */

/* Module__ */
typedef struct xdc_runtime_Main_Module_GateProxy_Module__ {
    xdc_runtime_Types_Link link;
} xdc_runtime_Main_Module_GateProxy_Module__;

/* Module__root__V */
extern xdc_runtime_Main_Module_GateProxy_Module__ xdc_runtime_Main_Module_GateProxy_Module__root__V;

/* <-- ti_sysbios_gates_GateHwi_Object */

/* Object */
typedef ti_sysbios_gates_GateHwi_Object__ xdc_runtime_Main_Module_GateProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    xdc_runtime_Main_Module_GateProxy_Object__ obj;
} xdc_runtime_Main_Module_GateProxy_Object2__;

/* __ParamsPtr */
#ifdef xdc_runtime_Main_Module_GateProxy___VERS
    #define xdc_runtime_Main_Module_GateProxy___ParamsPtr xdc_UChar*
#else
    #define xdc_runtime_Main_Module_GateProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== xdc.runtime.Memory INTERNALS ========
 */


/*
 * ======== xdc.runtime.Memory_HeapProxy INTERNALS ========
 */

/* Module__ */
typedef struct xdc_runtime_Memory_HeapProxy_Module__ {
    xdc_runtime_Types_Link link;
} xdc_runtime_Memory_HeapProxy_Module__;

/* Module__root__V */
extern xdc_runtime_Memory_HeapProxy_Module__ xdc_runtime_Memory_HeapProxy_Module__root__V;

/* <-- ti_sysbios_heaps_HeapMem_Object */

/* Object */
typedef ti_sysbios_heaps_HeapMem_Object__ xdc_runtime_Memory_HeapProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    xdc_runtime_Memory_HeapProxy_Object__ obj;
} xdc_runtime_Memory_HeapProxy_Object2__;

/* __ParamsPtr */
#ifdef xdc_runtime_Memory_HeapProxy___VERS
    #define xdc_runtime_Memory_HeapProxy___ParamsPtr xdc_UChar*
#else
    #define xdc_runtime_Memory_HeapProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== xdc.runtime.Registry INTERNALS ========
 */


/*
 * ======== xdc.runtime.Startup INTERNALS ========
 */


/*
 * ======== xdc.runtime.SysStd INTERNALS ========
 */


/*
 * ======== xdc.runtime.System INTERNALS ========
 */


/*
 * ======== xdc.runtime.System_Module_GateProxy INTERNALS ========
 */

/* Module__ */
typedef struct xdc_runtime_System_Module_GateProxy_Module__ {
    xdc_runtime_Types_Link link;
} xdc_runtime_System_Module_GateProxy_Module__;

/* Module__root__V */
extern xdc_runtime_System_Module_GateProxy_Module__ xdc_runtime_System_Module_GateProxy_Module__root__V;

/* <-- ti_sysbios_gates_GateHwi_Object */

/* Object */
typedef ti_sysbios_gates_GateHwi_Object__ xdc_runtime_System_Module_GateProxy_Object__;

/* Object2__ */
typedef struct {
    xdc_runtime_Types_InstHdr hdr;
    xdc_runtime_System_Module_GateProxy_Object__ obj;
} xdc_runtime_System_Module_GateProxy_Object2__;

/* __ParamsPtr */
#ifdef xdc_runtime_System_Module_GateProxy___VERS
    #define xdc_runtime_System_Module_GateProxy___ParamsPtr xdc_UChar*
#else
    #define xdc_runtime_System_Module_GateProxy___ParamsPtr xdc_Ptr
#endif


/*
 * ======== xdc.runtime.System_SupportProxy INTERNALS ========
 */


/*
 * ======== xdc.runtime.Text INTERNALS ========
 */


/*
 * ======== xdc.runtime.Timestamp INTERNALS ========
 */


/*
 * ======== xdc.runtime.Timestamp_SupportProxy INTERNALS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base xdc_runtime_IModule_Interface__BASE__C = {0};
__FAR__ const xdc_runtime_Types_Base xdc_runtime_IGateProvider_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base ti_sysbios_interfaces_IIntrinsicsSupport_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== ti.sysbios.gates.GateHwi INHERITS ========
 */



/*
 * ======== ti.sysbios.gates.GateMutex INHERITS ========
 */



/*
 * ======== ti.sysbios.hal.unicache.Cache INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base ti_sysbios_interfaces_ICache_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== ti.sysbios.heaps.HeapBuf INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base xdc_runtime_IHeap_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== ti.sysbios.heaps.HeapMem INHERITS ========
 */



/*
 * ======== ti.sysbios.syncs.SyncSem INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base xdc_runtime_knl_ISync_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== xdc.runtime.SysStd INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base xdc_runtime_ISystemSupport_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== xdc.runtime.Timestamp INHERITS ========
 */

__FAR__ const xdc_runtime_Types_Base xdc_runtime_ITimestampClient_Interface__BASE__C = {(void*)&xdc_runtime_IModule_Interface__BASE__C};


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_family_arm_ducati_GateDualCore_Fxns__ ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C = {
    (void*)&xdc_runtime_IGateProvider_Interface__BASE__C, /* base__ */
    &ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_family_arm_ducati_GateDualCore_query__E,
    ti_sysbios_family_arm_ducati_GateDualCore_enter__E,
    ti_sysbios_family_arm_ducati_GateDualCore_leave__E,
    {
        ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S,
        ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S,
        ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S,
        0x8028, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_family_arm_m3_IntrinsicsSupport_Fxns__ ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__FXNS__C = {
    (void*)&ti_sysbios_interfaces_IIntrinsicsSupport_Interface__BASE__C, /* base__ */
    &ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_family_arm_m3_IntrinsicsSupport_maxbit__E,
    {
        NULL, /* __create */
        NULL, /* __delete */
        NULL, /* __label */
        0x8023, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.gates.GateHwi VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_gates_GateHwi_Fxns__ ti_sysbios_gates_GateHwi_Module__FXNS__C = {
    (void*)&xdc_runtime_IGateProvider_Interface__BASE__C, /* base__ */
    &ti_sysbios_gates_GateHwi_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_gates_GateHwi_query__E,
    ti_sysbios_gates_GateHwi_enter__E,
    ti_sysbios_gates_GateHwi_leave__E,
    {
        ti_sysbios_gates_GateHwi_Object__create__S,
        ti_sysbios_gates_GateHwi_Object__delete__S,
        ti_sysbios_gates_GateHwi_Handle__label__S,
        0x8034, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.gates.GateMutex VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_gates_GateMutex_Fxns__ ti_sysbios_gates_GateMutex_Module__FXNS__C = {
    (void*)&xdc_runtime_IGateProvider_Interface__BASE__C, /* base__ */
    &ti_sysbios_gates_GateMutex_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_gates_GateMutex_query__E,
    ti_sysbios_gates_GateMutex_enter__E,
    ti_sysbios_gates_GateMutex_leave__E,
    {
        ti_sysbios_gates_GateMutex_Object__create__S,
        ti_sysbios_gates_GateMutex_Object__delete__S,
        ti_sysbios_gates_GateMutex_Handle__label__S,
        0x8035, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.hal.unicache.Cache VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_hal_unicache_Cache_Fxns__ ti_sysbios_hal_unicache_Cache_Module__FXNS__C = {
    (void*)&ti_sysbios_interfaces_ICache_Interface__BASE__C, /* base__ */
    &ti_sysbios_hal_unicache_Cache_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_hal_unicache_Cache_enable__E,
    ti_sysbios_hal_unicache_Cache_disable__E,
    ti_sysbios_hal_unicache_Cache_inv__E,
    ti_sysbios_hal_unicache_Cache_wb__E,
    ti_sysbios_hal_unicache_Cache_wbInv__E,
    ti_sysbios_hal_unicache_Cache_wbAll__E,
    ti_sysbios_hal_unicache_Cache_wbInvAll__E,
    ti_sysbios_hal_unicache_Cache_wait__E,
    {
        NULL, /* __create */
        NULL, /* __delete */
        NULL, /* __label */
        0x803b, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.heaps.HeapBuf VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_heaps_HeapBuf_Fxns__ ti_sysbios_heaps_HeapBuf_Module__FXNS__C = {
    (void*)&xdc_runtime_IHeap_Interface__BASE__C, /* base__ */
    &ti_sysbios_heaps_HeapBuf_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_heaps_HeapBuf_alloc__E,
    ti_sysbios_heaps_HeapBuf_free__E,
    ti_sysbios_heaps_HeapBuf_isBlocking__E,
    ti_sysbios_heaps_HeapBuf_getStats__E,
    {
        ti_sysbios_heaps_HeapBuf_Object__create__S,
        ti_sysbios_heaps_HeapBuf_Object__delete__S,
        ti_sysbios_heaps_HeapBuf_Handle__label__S,
        0x802e, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.heaps.HeapMem VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_heaps_HeapMem_Fxns__ ti_sysbios_heaps_HeapMem_Module__FXNS__C = {
    (void*)&xdc_runtime_IHeap_Interface__BASE__C, /* base__ */
    &ti_sysbios_heaps_HeapMem_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_heaps_HeapMem_alloc__E,
    ti_sysbios_heaps_HeapMem_free__E,
    ti_sysbios_heaps_HeapMem_isBlocking__E,
    ti_sysbios_heaps_HeapMem_getStats__E,
    {
        ti_sysbios_heaps_HeapMem_Object__create__S,
        ti_sysbios_heaps_HeapMem_Object__delete__S,
        ti_sysbios_heaps_HeapMem_Handle__label__S,
        0x802f, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.syncs.SyncSem VTABLE ========
 */

/* Module__FXNS__C */
const ti_sysbios_syncs_SyncSem_Fxns__ ti_sysbios_syncs_SyncSem_Module__FXNS__C = {
    (void*)&xdc_runtime_knl_ISync_Interface__BASE__C, /* base__ */
    &ti_sysbios_syncs_SyncSem_Module__FXNS__C.__sfxns, /* __sysp */
    ti_sysbios_syncs_SyncSem_query__E,
    ti_sysbios_syncs_SyncSem_signal__E,
    ti_sysbios_syncs_SyncSem_wait__E,
    {
        ti_sysbios_syncs_SyncSem_Object__create__S,
        ti_sysbios_syncs_SyncSem_Object__delete__S,
        ti_sysbios_syncs_SyncSem_Handle__label__S,
        0x8039, /* __mid */
    } /* __sfxns */
};


/*
 * ======== xdc.runtime.SysStd VTABLE ========
 */

/* Module__FXNS__C */
const xdc_runtime_SysStd_Fxns__ xdc_runtime_SysStd_Module__FXNS__C = {
    (void*)&xdc_runtime_ISystemSupport_Interface__BASE__C, /* base__ */
    &xdc_runtime_SysStd_Module__FXNS__C.__sfxns, /* __sysp */
    xdc_runtime_SysStd_abort__E,
    xdc_runtime_SysStd_exit__E,
    xdc_runtime_SysStd_flush__E,
    xdc_runtime_SysStd_putch__E,
    xdc_runtime_SysStd_ready__E,
    {
        NULL, /* __create */
        NULL, /* __delete */
        NULL, /* __label */
        0x800e, /* __mid */
    } /* __sfxns */
};


/*
 * ======== xdc.runtime.Timestamp VTABLE ========
 */

/* Module__FXNS__C */
const xdc_runtime_Timestamp_Fxns__ xdc_runtime_Timestamp_Module__FXNS__C = {
    (void*)&xdc_runtime_ITimestampClient_Interface__BASE__C, /* base__ */
    &xdc_runtime_Timestamp_Module__FXNS__C.__sfxns, /* __sysp */
    xdc_runtime_Timestamp_get32__E,
    xdc_runtime_Timestamp_get64__E,
    xdc_runtime_Timestamp_getFreq__E,
    {
        NULL, /* __create */
        NULL, /* __delete */
        NULL, /* __label */
        0x8010, /* __mid */
    } /* __sfxns */
};


/*
 * ======== ti.sysbios.BIOS DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_BIOS_Module_State__ {
    xdc_runtime_Types_FreqHz cpuFreq;
    xdc_UInt rtsGateCount;
    xdc_IArg rtsGateKey;
    ti_sysbios_BIOS_RtsGateProxy_Handle rtsGate;
    ti_sysbios_BIOS_ThreadType threadType;
    ti_sysbios_BIOS_StartFuncPtr startFunc;
} ti_sysbios_BIOS_Module_State__;

/* --> ti_sysbios_BIOS_startFunc */
extern xdc_Void ti_sysbios_BIOS_startFunc(xdc_Void);

/* Module__state__V */
ti_sysbios_BIOS_Module_State__ ti_sysbios_BIOS_Module__state__V;


/*
 * ======== ti.sysbios.BIOS_RtsGateProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.CTM DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.Core DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_family_arm_ducati_GateDualCore_Object__ ti_sysbios_family_arm_ducati_GateDualCore_Object__table__V[1];

/* Module_State__ */
typedef struct ti_sysbios_family_arm_ducati_GateDualCore_Module_State__ {
    __TA_ti_sysbios_family_arm_ducati_GateDualCore_Module_State__usedGates usedGates;
} ti_sysbios_family_arm_ducati_GateDualCore_Module_State__;

/* Module__state__V */
ti_sysbios_family_arm_ducati_GateDualCore_Module_State__ ti_sysbios_family_arm_ducati_GateDualCore_Module__state__V;


/*
 * ======== ti.sysbios.family.arm.ducati.Timer DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_family_arm_ducati_Timer_Object__ ti_sysbios_family_arm_ducati_Timer_Object__table__V[1];

/* Module_State__ */
typedef struct ti_sysbios_family_arm_ducati_Timer_Module_State__ {
    xdc_UInt availMask;
    __TA_ti_sysbios_family_arm_ducati_Timer_Module_State__handles handles;
} ti_sysbios_family_arm_ducati_Timer_Module_State__;

/* Module__state__V */
ti_sysbios_family_arm_ducati_Timer_Module_State__ ti_sysbios_family_arm_ducati_Timer_Module__state__V;


/*
 * ======== ti.sysbios.family.arm.ducati.TimerSupport DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_family_arm_ducati_TimerSupport_Module_State__ {
    __TA_ti_sysbios_family_arm_ducati_TimerSupport_Module_State__intNums intNums;
} ti_sysbios_family_arm_ducati_TimerSupport_Module_State__;

/* --> ti_sysbios_family_arm_ducati_TimerSupport_Module_State_0_intNums__A */
__T1_ti_sysbios_family_arm_ducati_TimerSupport_Module_State__intNums ti_sysbios_family_arm_ducati_TimerSupport_Module_State_0_intNums__A[4];

/* Module__state__V */
ti_sysbios_family_arm_ducati_TimerSupport_Module_State__ ti_sysbios_family_arm_ducati_TimerSupport_Module__state__V;


/*
 * ======== ti.sysbios.family.arm.ducati.TimestampProvider DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.Wugen DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.m3.Hwi DECLARATIONS ========
 */

/* --> ti_sysbios_family_arm_m3_Hwi_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Instance_State__hookEnv ti_sysbios_family_arm_m3_Hwi_Instance_State_0_hookEnv__A[1];

/* --> ti_sysbios_family_arm_m3_Hwi_Instance_State_1_hookEnv__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Instance_State__hookEnv ti_sysbios_family_arm_m3_Hwi_Instance_State_1_hookEnv__A[1];

/* Object__table__V */
ti_sysbios_family_arm_m3_Hwi_Object__ ti_sysbios_family_arm_m3_Hwi_Object__table__V[2];

/* Module_State__ */
typedef struct ti_sysbios_family_arm_m3_Hwi_Module_State__ {
    xdc_Bits16 enables;
    __TA_ti_sysbios_family_arm_m3_Hwi_Module_State__iser iser;
    __TA_ti_sysbios_family_arm_m3_Hwi_Module_State__priorities priorities;
    xdc_Char* taskSP;
    xdc_Bool excActive;
    ti_sysbios_family_arm_m3_Hwi_ExcContext* excContext;
    xdc_Ptr excStack;
    xdc_Ptr isrStack;
    xdc_Ptr isrStackBase;
    xdc_SizeT isrStackSize;
    xdc_Ptr vectorTableBase;
    xdc_UInt swiTaskKeys;
    __TA_ti_sysbios_family_arm_m3_Hwi_Module_State__dispatchTable dispatchTable;
} ti_sysbios_family_arm_m3_Hwi_Module_State__;

/* --> ti_sysbios_family_arm_m3_Hwi_Module_State_0_priorities__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Module_State__priorities ti_sysbios_family_arm_m3_Hwi_Module_State_0_priorities__A[80];

/* --> __TI_STACK_BASE */
extern void* __TI_STACK_BASE;

/* --> ti_sysbios_family_arm_m3_Hwi_Module_State_0_dispatchTable__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Module_State__dispatchTable ti_sysbios_family_arm_m3_Hwi_Module_State_0_dispatchTable__A[80];

/* Module__state__V */
ti_sysbios_family_arm_m3_Hwi_Module_State__ ti_sysbios_family_arm_m3_Hwi_Module__state__V;

/* --> ti_sysbios_knl_Swi_disable__E */
extern xdc_UInt ti_sysbios_knl_Swi_disable__E(xdc_Void);

/* --> ti_sysbios_knl_Swi_restoreHwi__E */
extern xdc_Void ti_sysbios_knl_Swi_restoreHwi__E(xdc_UInt);

/* --> ti_sysbios_knl_Task_disable__E */
extern xdc_UInt ti_sysbios_knl_Task_disable__E(xdc_Void);

/* --> ti_sysbios_knl_Task_restoreHwi__E */
extern xdc_Void ti_sysbios_knl_Task_restoreHwi__E(xdc_UInt);

/* --> ti_sysbios_family_arm_m3_Hwi_hooks__A */
const __T1_ti_sysbios_family_arm_m3_Hwi_hooks ti_sysbios_family_arm_m3_Hwi_hooks__A[1];


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.family.arm.m3.TaskSupport DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.gates.GateHwi DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_gates_GateHwi_Object__ ti_sysbios_gates_GateHwi_Object__table__V[1];


/*
 * ======== ti.sysbios.gates.GateMutex DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_gates_GateMutex_Object__ ti_sysbios_gates_GateMutex_Object__table__V[2];


/*
 * ======== ti.sysbios.hal.Cache DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.Cache_CacheProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.Hwi DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_hal_Hwi_Object__ ti_sysbios_hal_Hwi_Object__table__V[1];


/*
 * ======== ti.sysbios.hal.Hwi_HwiProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.Timer DECLARATIONS ========
 */

/* Object__table__V */
ti_sysbios_hal_Timer_Object__ ti_sysbios_hal_Timer_Object__table__V[1];


/*
 * ======== ti.sysbios.hal.Timer_TimerProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.ammu.AMMU DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.unicache.Cache DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.hal.unicache.Cache_Module_GateProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.heaps.HeapBuf DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_heaps_HeapBuf_Module_State__ {
    __TA_ti_sysbios_heaps_HeapBuf_Module_State__constructedHeaps constructedHeaps;
} ti_sysbios_heaps_HeapBuf_Module_State__;

/* Module__state__V */
ti_sysbios_heaps_HeapBuf_Module_State__ ti_sysbios_heaps_HeapBuf_Module__state__V;


/*
 * ======== ti.sysbios.heaps.HeapMem DECLARATIONS ========
 */

/* --> ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A */
__T1_ti_sysbios_heaps_HeapMem_Instance_State__buf ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A[86016];
#ifdef __ti__align
    #pragma DATA_ALIGN(ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A, 8);
#endif
#ifdef __GNUC__
#if __GNUC__ >= 4
__T1_ti_sysbios_heaps_HeapMem_Instance_State__buf ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A[86016] __attribute__ ((aligned(8)));
#endif
#endif

/* Object__table__V */
ti_sysbios_heaps_HeapMem_Object__ ti_sysbios_heaps_HeapMem_Object__table__V[1];


/*
 * ======== ti.sysbios.heaps.HeapMem_Module_GateProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.knl.Clock DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_knl_Clock_Module_State__ {
    volatile xdc_UInt32 ticks;
    xdc_UInt swiCount;
    ti_sysbios_hal_Timer_Handle timer;
    xdc_Void (*doTickFunc)(xdc_UArg);
    ti_sysbios_knl_Swi_Handle swi;
    xdc_UInt32 periodCounts;
    volatile xdc_UInt numTickSkip;
    xdc_UInt skipsWorkFunc;
    xdc_Bool inWorkFunc;
    ti_sysbios_knl_Queue_Object__ Object_field_clockQ;
} ti_sysbios_knl_Clock_Module_State__;

/* Module__state__V */
ti_sysbios_knl_Clock_Module_State__ ti_sysbios_knl_Clock_Module__state__V;


/*
 * ======== ti.sysbios.knl.Idle DECLARATIONS ========
 */

/* --> ti_sysbios_hal_Hwi_checkStack */
extern xdc_Void ti_sysbios_hal_Hwi_checkStack(xdc_Void);

/* --> ti_sysbios_knl_Idle_funcList__A */
const __T1_ti_sysbios_knl_Idle_funcList ti_sysbios_knl_Idle_funcList__A[2];


/*
 * ======== ti.sysbios.knl.Intrinsics DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.knl.Intrinsics_SupportProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.knl.Queue DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.knl.Semaphore DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.knl.Swi DECLARATIONS ========
 */

/* --> ti_sysbios_knl_Swi_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_knl_Swi_Instance_State__hookEnv ti_sysbios_knl_Swi_Instance_State_0_hookEnv__A[1];

/* Object__table__V */
ti_sysbios_knl_Swi_Object__ ti_sysbios_knl_Swi_Object__table__V[1];

/* Module_State__ */
typedef struct ti_sysbios_knl_Swi_Module_State__ {
    xdc_Bool locked;
    xdc_UInt curSet;
    xdc_UInt curTrigger;
    ti_sysbios_knl_Swi_Handle curSwi;
    ti_sysbios_knl_Queue_Handle curQ;
    __TA_ti_sysbios_knl_Swi_Module_State__readyQ readyQ;
    __TA_ti_sysbios_knl_Swi_Module_State__constructedSwis constructedSwis;
} ti_sysbios_knl_Swi_Module_State__;

/* --> ti_sysbios_knl_Swi_Module_State_0_readyQ__A */
__T1_ti_sysbios_knl_Swi_Module_State__readyQ ti_sysbios_knl_Swi_Module_State_0_readyQ__A[16];

/* Module__state__V */
ti_sysbios_knl_Swi_Module_State__ ti_sysbios_knl_Swi_Module__state__V;

/* --> ti_sysbios_knl_Swi_hooks__A */
const __T1_ti_sysbios_knl_Swi_hooks ti_sysbios_knl_Swi_hooks__A[1];

/* --> ti_sysbios_knl_Task_disable__E */
extern xdc_UInt ti_sysbios_knl_Task_disable__E(xdc_Void);

/* --> ti_sysbios_knl_Task_restore__E */
extern xdc_Void ti_sysbios_knl_Task_restore__E(xdc_UInt);


/*
 * ======== ti.sysbios.knl.Task DECLARATIONS ========
 */

/* --> ti_sysbios_knl_Task_Instance_State_0_stack__A */
__T1_ti_sysbios_knl_Task_Instance_State__stack ti_sysbios_knl_Task_Instance_State_0_stack__A[16384];
#ifdef __ti__sect
    #pragma DATA_SECTION(ti_sysbios_knl_Task_Instance_State_0_stack__A, ".bss:taskStackSection");
#endif
#ifdef __GNUC__
#if __GNUC__ >= 4
__T1_ti_sysbios_knl_Task_Instance_State__stack ti_sysbios_knl_Task_Instance_State_0_stack__A[16384] __attribute__ ((section(".bss:taskStackSection")));
#endif
#endif
#ifdef __ti__align
    #pragma DATA_ALIGN(ti_sysbios_knl_Task_Instance_State_0_stack__A, 8);
#endif
#ifdef __GNUC__
#if __GNUC__ >= 4
__T1_ti_sysbios_knl_Task_Instance_State__stack ti_sysbios_knl_Task_Instance_State_0_stack__A[16384] __attribute__ ((aligned(8)));
#endif
#endif

/* --> ti_sysbios_knl_Task_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_knl_Task_Instance_State__hookEnv ti_sysbios_knl_Task_Instance_State_0_hookEnv__A[2];

/* Object__table__V */
ti_sysbios_knl_Task_Object__ ti_sysbios_knl_Task_Object__table__V[1];

/* Module_State__ */
typedef struct ti_sysbios_knl_Task_Module_State__ {
    xdc_Bool locked;
    volatile xdc_UInt curSet;
    xdc_Bool workFlag;
    xdc_UInt vitalTasks;
    ti_sysbios_knl_Task_Handle curTask;
    ti_sysbios_knl_Queue_Handle curQ;
    __TA_ti_sysbios_knl_Task_Module_State__readyQ readyQ;
    ti_sysbios_knl_Task_Handle idleTask;
    __TA_ti_sysbios_knl_Task_Module_State__constructedTasks constructedTasks;
    ti_sysbios_knl_Queue_Object__ Object_field_inactiveQ;
    ti_sysbios_knl_Queue_Object__ Object_field_terminatedQ;
} ti_sysbios_knl_Task_Module_State__;

/* --> ti_sysbios_knl_Task_Module_State_0_readyQ__A */
__T1_ti_sysbios_knl_Task_Module_State__readyQ ti_sysbios_knl_Task_Module_State_0_readyQ__A[16];

/* Module__state__V */
ti_sysbios_knl_Task_Module_State__ ti_sysbios_knl_Task_Module__state__V;

/* --> ti_sysbios_knl_Task_checkStacks */
extern xdc_Void ti_sysbios_knl_Task_checkStacks(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle);

/* --> ti_sysbios_utils_Load_taskRegHook__E */
extern xdc_Void ti_sysbios_utils_Load_taskRegHook__E(xdc_Int);

/* --> ti_sysbios_knl_Task_hooks__A */
const __T1_ti_sysbios_knl_Task_hooks ti_sysbios_knl_Task_hooks__A[2];


/*
 * ======== ti.sysbios.knl.Task_SupportProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.syncs.SyncSem DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.timers.dmtimer.Timer DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_timers_dmtimer_Timer_Module_State__ {
    xdc_Bits32 availMask;
    xdc_runtime_Types_FreqHz intFreq;
    __TA_ti_sysbios_timers_dmtimer_Timer_Module_State__device device;
    __TA_ti_sysbios_timers_dmtimer_Timer_Module_State__handles handles;
    xdc_Bool firstInit;
} ti_sysbios_timers_dmtimer_Timer_Module_State__;

/* --> ti_sysbios_timers_dmtimer_Timer_Module_State_0_device__A */
__T1_ti_sysbios_timers_dmtimer_Timer_Module_State__device ti_sysbios_timers_dmtimer_Timer_Module_State_0_device__A[4];

/* --> ti_sysbios_timers_dmtimer_Timer_Module_State_0_handles__A */
__T1_ti_sysbios_timers_dmtimer_Timer_Module_State__handles ti_sysbios_timers_dmtimer_Timer_Module_State_0_handles__A[4];

/* Module__state__V */
ti_sysbios_timers_dmtimer_Timer_Module_State__ ti_sysbios_timers_dmtimer_Timer_Module__state__V;


/*
 * ======== ti.sysbios.timers.dmtimer.Timer_TimerSupportProxy DECLARATIONS ========
 */


/*
 * ======== ti.sysbios.utils.Load DECLARATIONS ========
 */

/* Module_State__ */
typedef struct ti_sysbios_utils_Load_Module_State__ {
    xdc_Int taskHId;
    xdc_UInt32 taskStartTime;
    xdc_UInt32 timeElapsed;
    ti_sysbios_knl_Task_Handle runningTask;
    xdc_Bool firstSwitchDone;
    xdc_UInt32 swiStartTime;
    ti_sysbios_utils_Load_HookContext swiEnv;
    xdc_UInt32 swiCnt;
    xdc_UInt32 hwiStartTime;
    ti_sysbios_utils_Load_HookContext hwiEnv;
    xdc_UInt32 hwiCnt;
    xdc_UInt32 timeSlotCnt;
    xdc_UInt32 minLoop;
    xdc_UInt32 minIdle;
    xdc_UInt32 t0;
    xdc_UInt32 idleCnt;
    xdc_UInt32 cpuLoad;
    ti_sysbios_knl_Queue_Object__ Object_field_taskList;
    ti_sysbios_gates_GateMutex_Object__ Object_field_gate;
} ti_sysbios_utils_Load_Module_State__;

/* Module__state__V */
ti_sysbios_utils_Load_Module_State__ ti_sysbios_utils_Load_Module__state__V;

/* --> VpsUtils_prfLoadUpdate */
extern xdc_Void VpsUtils_prfLoadUpdate(xdc_Void);


/*
 * ======== xdc.runtime.Assert DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Core DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Defaults DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Diags DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Error DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_Error_Module_State__ {
    xdc_UInt16 count;
} xdc_runtime_Error_Module_State__;

/* Module__state__V */
xdc_runtime_Error_Module_State__ xdc_runtime_Error_Module__state__V;


/*
 * ======== xdc.runtime.Gate DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Log DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Main DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Main_Module_GateProxy DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Memory DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_Memory_Module_State__ {
    xdc_SizeT maxDefaultTypeAlign;
} xdc_runtime_Memory_Module_State__;

/* Module__state__V */
xdc_runtime_Memory_Module_State__ xdc_runtime_Memory_Module__state__V;


/*
 * ======== xdc.runtime.Memory_HeapProxy DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Registry DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_Registry_Module_State__ {
    xdc_runtime_Registry_Desc* listHead;
    xdc_runtime_Types_ModuleId curId;
} xdc_runtime_Registry_Module_State__;

/* Module__state__V */
xdc_runtime_Registry_Module_State__ xdc_runtime_Registry_Module__state__V;


/*
 * ======== xdc.runtime.Startup DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_Startup_Module_State__ {
    xdc_Int* stateTab;
    xdc_Bool execFlag;
    xdc_Bool rtsDoneFlag;
} xdc_runtime_Startup_Module_State__;

/* Module__state__V */
xdc_runtime_Startup_Module_State__ xdc_runtime_Startup_Module__state__V;

/* --> ti_sysbios_hal_Hwi_initStack */
extern xdc_Void ti_sysbios_hal_Hwi_initStack(xdc_Void);

/* --> xdc_runtime_Startup_firstFxns__A */
const __T1_xdc_runtime_Startup_firstFxns xdc_runtime_Startup_firstFxns__A[3];

/* --> xdc_runtime_Startup_lastFxns__A */
const __T1_xdc_runtime_Startup_lastFxns xdc_runtime_Startup_lastFxns__A[1];

/* --> xdc_runtime_System_Module_startup__E */
extern xdc_Int xdc_runtime_System_Module_startup__E(xdc_Int);

/* --> ti_sysbios_hal_Hwi_Module_startup__E */
extern xdc_Int ti_sysbios_hal_Hwi_Module_startup__E(xdc_Int);

/* --> ti_sysbios_hal_Timer_Module_startup__E */
extern xdc_Int ti_sysbios_hal_Timer_Module_startup__E(xdc_Int);

/* --> ti_sysbios_knl_Clock_Module_startup__E */
extern xdc_Int ti_sysbios_knl_Clock_Module_startup__E(xdc_Int);

/* --> ti_sysbios_knl_Swi_Module_startup__E */
extern xdc_Int ti_sysbios_knl_Swi_Module_startup__E(xdc_Int);

/* --> ti_sysbios_knl_Task_Module_startup__E */
extern xdc_Int ti_sysbios_knl_Task_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_m3_Hwi_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_m3_Hwi_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_Core_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_Core_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_Timer_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_Timer_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_TimerSupport_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_TimerSupport_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_TimestampProvider_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_TimestampProvider_Module_startup__E(xdc_Int);

/* --> ti_sysbios_family_arm_ducati_Wugen_Module_startup__E */
extern xdc_Int ti_sysbios_family_arm_ducati_Wugen_Module_startup__E(xdc_Int);

/* --> ti_sysbios_heaps_HeapBuf_Module_startup__E */
extern xdc_Int ti_sysbios_heaps_HeapBuf_Module_startup__E(xdc_Int);

/* --> ti_sysbios_timers_dmtimer_Timer_Module_startup__E */
extern xdc_Int ti_sysbios_timers_dmtimer_Timer_Module_startup__E(xdc_Int);

/* --> ti_sysbios_hal_unicache_Cache_Module_startup__E */
extern xdc_Int ti_sysbios_hal_unicache_Cache_Module_startup__E(xdc_Int);

/* --> xdc_runtime_Startup_sfxnTab__A */
const __T1_xdc_runtime_Startup_sfxnTab xdc_runtime_Startup_sfxnTab__A[16];

/* --> xdc_runtime_Startup_sfxnRts__A */
const __T1_xdc_runtime_Startup_sfxnRts xdc_runtime_Startup_sfxnRts__A[16];


/*
 * ======== xdc.runtime.SysStd DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.System DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_System_Module_State__ {
    __TA_xdc_runtime_System_Module_State__atexitHandlers atexitHandlers;
    xdc_Int numAtexitHandlers;
    xdc_Int exitStatus;
} xdc_runtime_System_Module_State__;

/* --> xdc_runtime_System_Module_State_0_atexitHandlers__A */
__T1_xdc_runtime_System_Module_State__atexitHandlers xdc_runtime_System_Module_State_0_atexitHandlers__A[8];

/* Module__state__V */
xdc_runtime_System_Module_State__ xdc_runtime_System_Module__state__V;

/* --> xdc_runtime_System_printfExtend__I */
extern xdc_Int xdc_runtime_System_printfExtend__I(xdc_Char**,xdc_Char**,xdc_VaList*,xdc_runtime_System_ParseData*);


/*
 * ======== xdc.runtime.System_Module_GateProxy DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.System_SupportProxy DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Text DECLARATIONS ========
 */

/* Module_State__ */
typedef struct xdc_runtime_Text_Module_State__ {
    xdc_Ptr charBase;
    xdc_Ptr nodeBase;
} xdc_runtime_Text_Module_State__;

/* Module__state__V */
xdc_runtime_Text_Module_State__ xdc_runtime_Text_Module__state__V;

/* --> xdc_runtime_Text_charTab__A */
const __T1_xdc_runtime_Text_charTab xdc_runtime_Text_charTab__A[5163];

/* --> xdc_runtime_Text_nodeTab__A */
const __T1_xdc_runtime_Text_nodeTab xdc_runtime_Text_nodeTab__A[62];


/*
 * ======== xdc.runtime.Timestamp DECLARATIONS ========
 */


/*
 * ======== xdc.runtime.Timestamp_SupportProxy DECLARATIONS ========
 */


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_gates_GateMutex_Instance_State_sem__O = offsetof(ti_sysbios_gates_GateMutex_Object__, Object_field_sem);


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_heaps_HeapBuf_Instance_State_freeList__O = offsetof(ti_sysbios_heaps_HeapBuf_Object__, Object_field_freeList);


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_knl_Clock_Module_State_clockQ__O = offsetof(ti_sysbios_knl_Clock_Module_State__, Object_field_clockQ);


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_knl_Semaphore_Instance_State_pendQ__O = offsetof(ti_sysbios_knl_Semaphore_Object__, Object_field_pendQ);


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_knl_Task_Module_State_inactiveQ__O = offsetof(ti_sysbios_knl_Task_Module_State__, Object_field_inactiveQ);
__FAR__ const xdc_SizeT ti_sysbios_knl_Task_Module_State_terminatedQ__O = offsetof(ti_sysbios_knl_Task_Module_State__, Object_field_terminatedQ);


/*
 * ======== OBJECT OFFSETS ========
 */

__FAR__ const xdc_SizeT ti_sysbios_utils_Load_Module_State_taskList__O = offsetof(ti_sysbios_utils_Load_Module_State__, Object_field_taskList);
__FAR__ const xdc_SizeT ti_sysbios_utils_Load_Module_State_gate__O = offsetof(ti_sysbios_utils_Load_Module_State__, Object_field_gate);


/*
 * ======== xdc.cfg.Program TEMPLATE ========
 */

/*
 *  ======== __ASM__ ========
 *  Define absolute path prefix for this executable's
 *  configuration generated files.
 */
xdc__META(__ASM__, "@(#)__ASM__ = /proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/build/hdvpss_examples_m2mNsf_Subframe/obj/ti8107-evm/m3vpss/debug/hdvpss_examples_m2mNsf_Subframe_configuro/package/cfg/m2mNsf_subframe_pem3");

/*
 *  ======== __ISA__ ========
 *  Define the ISA of this executable.  This symbol is used by platform
 *  specific "exec" commands that support more than one ISA; e.g., gdb
 */
xdc__META(__ISA__, "@(#)__ISA__ = v7M");

/*
 *  ======== __PLAT__ ========
 *  Define the name of the platform that can run this executable.  This
 *  symbol is used by platform independent "exec" commands
 */
xdc__META(__PLAT__, "@(#)__PLAT__ = ti.platforms.evmDM8148");

/*
 *  ======== __TARG__ ========
 *  Define the name of the target used to build this executable.
 */
xdc__META(__TARG__, "@(#)__TARG__ = ti.targets.arm.elf.M3");

/*
 *  ======== __TRDR__ ========
 *  Define the name of the class that can read/parse this executable.
 */
xdc__META(__TRDR__, "@(#)__TRDR__ = ti.targets.omf.elf.Elf32");


/*
 * ======== xdc.cfg.SourceDir TEMPLATE ========
 */



/*
 * ======== xdc.runtime.Diags TEMPLATE ========
 */



/*
 * ======== xdc.runtime.Startup TEMPLATE ========
 */

/*
 *  ======== MODULE STARTUP DONE FUNCTIONS ========
 */
xdc_Bool xdc_runtime_System_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[0] < 0;
}
xdc_Bool ti_sysbios_hal_Hwi_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[1] < 0;
}
xdc_Bool ti_sysbios_hal_Timer_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[2] < 0;
}
xdc_Bool ti_sysbios_knl_Clock_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[3] < 0;
}
xdc_Bool ti_sysbios_knl_Swi_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[4] < 0;
}
xdc_Bool ti_sysbios_knl_Task_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[5] < 0;
}
xdc_Bool ti_sysbios_family_arm_m3_Hwi_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[6] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_Core_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[7] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[8] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_Timer_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[9] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[10] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[11] < 0;
}
xdc_Bool ti_sysbios_family_arm_ducati_Wugen_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[12] < 0;
}
xdc_Bool ti_sysbios_heaps_HeapBuf_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[13] < 0;
}
xdc_Bool ti_sysbios_timers_dmtimer_Timer_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[14] < 0;
}
xdc_Bool ti_sysbios_hal_unicache_Cache_Module__startupDone__F(void) {
    return (&xdc_runtime_Startup_Module__state__V)->stateTab == 0 || (&xdc_runtime_Startup_Module__state__V)->stateTab[15] < 0;
}

/* 
 * Startup_exec__I is an internal entry point called by target/platform
 * boot code. Boot code is not brought into a partial-link assembly. So,
 * without this pragma, whole program optimizers would otherwise optimize-out
 * this function.
 */
#ifdef __ti__
#pragma FUNC_EXT_CALLED(xdc_runtime_Startup_exec__I);
#endif

#ifdef __GNUC__
#if __GNUC__ >= 4
xdc_Void xdc_runtime_Startup_exec__I(void) __attribute__ ((externally_visible));
#endif
#endif

/*
 *  ======== xdc_runtime_Startup_exec__I ========
 *  Initialize all used modules that have startup functions
 */
xdc_Void xdc_runtime_Startup_exec__I(void)
{
    xdc_Int state[16];
    xdc_runtime_Startup_startModsFxn__C(state, 16);
}

/*
 * ======== xdc.runtime.Reset TEMPLATE ========
 */

/* 
 * Startup_reset__I is an internal entry point called by target/platform
 * boot code. Boot code is not brought into a partial-link assembly. So,
 * without this pragma, whole program optimizers would otherwise optimize-out
 * this function.
 */
#ifdef __ti__
#pragma FUNC_EXT_CALLED(xdc_runtime_Startup_reset__I);
#endif

#ifdef __GNUC__
#if __GNUC__ >= 4
xdc_Void xdc_runtime_Startup_reset__I(void) __attribute__ ((externally_visible));
#endif
#endif

extern Void ti_sysbios_hal_ammu_AMMU_init__I(void);  /* user defined reset function */

/*
 *  ======== xdc_runtime_Startup_reset__I ========
 *  This function is called by bootstrap initialization code as early as
 *  possible in the startup process.  This function calls all functions in
 *  the Reset.fxns array _as well as_ Startup.resetFxn (if it's non-NULL)
 */
xdc_Void xdc_runtime_Startup_reset__I(void)
{
    ti_sysbios_hal_ammu_AMMU_init__I();
}

/*
 * ======== xdc.runtime.System TEMPLATE ========
 */


#include <xdc/std.h>
#include <limits.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Assert.h>

/*
 *  ======== System_printfExtend__I ========
 *  This function processes optional extended formats of printf.
 *  
 *  It returns the number of characters added to the result.
 *
 *  Precision (maximum string length) is not supported for %$S.
 *
 *  Right-justified (which is default) minimum width is not supported
 *  for %$L, %$S, or %$F.
 */
Int xdc_runtime_System_printfExtend__I(Char **pbuf, Char **pfmt, 
    VaList *pva, xdc_runtime_System_ParseData *parse)
{
    Char    *fmt = *pfmt;
    Int     res;
    Char    c;
    Bool    found = FALSE;
    
    /* 
     * Create a local variable 'va' to ensure that the dereference of
     * pva only occurs once.
     */
    va_list va = *pva;
    
    res = 0;

    c = *fmt++;
    *pfmt = *pfmt + 1;

    
    if (c == '$') {
        c = *fmt++;
        *pfmt = *pfmt + 1;
        
        if (c == 'L') {
            xdc_runtime_Types_Label *lab = parse->aFlag ? 
                (xdc_runtime_Types_Label *)iargToPtr(va_arg(va, IArg)) :
                (xdc_runtime_Types_Label *)va_arg(va, void *);
            
            /* 
             * Call Text_putLab to write out the label, taking the precision 
             * into account.
             */
            res = xdc_runtime_Text_putLab(lab, pbuf, parse->precis);
            
            /*
             * Set the length to 0 to indicate to 'doPrint' that nothing should
             * be copied from parse.ptr.
             */
            parse->len = 0;
            
            /* Update the minimum width field. */
            parse->width -= res;
            
            found = TRUE;
        }
        
        if (c == 'F') {
            xdc_runtime_Types_Site site;
            
            /* Retrieve the file name string from the argument list */
            site.file = parse->aFlag ? (Char *) iargToPtr(va_arg(va, IArg)) :
                                       (Char *) va_arg(va, Char *);
            
            /* Retrieve the line number from the argument list. */
            site.line = parse->aFlag ? (Int) va_arg(va, IArg) : 
                                       (Int) va_arg(va, Int);
            
            /* 
             * Omit the 'mod' field, set it to 0.
             * '0' is a safe sentinel value - the IDs for named modules are 
             * 0x8000 and higher, and the IDs for unnamed modules go from 0x1
             * to 0x7fff.
             */
            site.mod = 0;
            
            /* 
             * Call putSite to format the file and line number.
             * If a precision was specified, it will be used as the maximum
             * string lengrth.
             */
            res = xdc_runtime_Text_putSite(&site, pbuf, parse->precis);
            
            /*
             * Set the length to 0 to indicate to 'doPrint' that nothing should
             * be copied from parse.ptr.
             */
            parse->len = 0;
       
            /* Update the minimum width field */
            parse->width -= res;
            
            found = TRUE;
        }
        
        if (c == 'S') {
            /* Retrieve the format string from the argument list */
            parse->ptr = parse->aFlag ? 
                (Char *) iargToPtr(va_arg(va, IArg)) :
                (Char *) va_arg(va, Char *);
            
            /* Update pva before passing it to doPrint. */
            *pva = va;
            
            /* Perform the recursive format. System_doPrint does not advance
             * the buffer pointer, so it has to be done explicitly.
             */
            res = xdc_runtime_System_doPrint__I(*pbuf, parse->ptr, pva, parse->aFlag);
            if (*pbuf) {
                *pbuf += res;
            }
            
            /* Update the temporary variable with any changes to *pva */
            va = *pva;
            
            /*
             * Set the length to 0 to indicate to 'doPrint' that nothing should
             * be copied from parse.ptr
             */
            parse->len = 0;

            /* Update the minimum width field */
            parse->width -= res;
            
            /* Indicate that we were able to interpret the specifier */
            found = TRUE;
        }
        
    }

    if (c == 'f') {
        /* support arguments _after_ optional float support */
        if (parse->aFlag) {
            (void)va_arg(va, IArg);
        }
        else {
            (void)va_arg(va, double);
        }
    }    

    if (found == FALSE) {
        /* other character (like %) copy to output */
        *(parse->ptr) = c;
        parse->len = 1;
    }

    /*
     * Before returning, we must update the value of pva. We use a label here
     * so that all return points will go through this update.
     * The 'goto end' is here to ensure that there is always a reference to the
     * label (to avoid the compiler complaining). 
     */
    goto end; 
end:
    *pva = va;
    return (res);
}

/*
 * ======== xdc.runtime.Text TEMPLATE ========
 */


/*
 *  ======== xdc_runtime_Text_visitRope__I ========
 *  This function is indirectly called within Text.c through
 *  the visitRopeFxn configuration parameter of xdc.runtime.Text.
 */
Void xdc_runtime_Text_visitRope__I(xdc_runtime_Text_RopeId rope,
    xdc_Fxn visFxn, xdc_Ptr visState)
{
    xdc_String stack[6];
    xdc_runtime_Text_visitRope2__I(rope, visFxn, visState, stack);
}


/*
 * ======== ti.sysbios.BIOS TEMPLATE ========
 */

Void ti_sysbios_BIOS_exitFunc__I(Int);

extern Void ti_sysbios_BIOS_registerRTSLock();
extern Void ti_sysbios_timers_dmtimer_Timer_startup__E();

Void ti_sysbios_BIOS_startFunc__I()
{
    xdc_runtime_System_atexit(
	(xdc_runtime_System_AtexitHandler)ti_sysbios_BIOS_exitFunc__I);
    ti_sysbios_BIOS_registerRTSLock();
    ti_sysbios_timers_dmtimer_Timer_startup__E();
    ti_sysbios_hal_Hwi_startup();
    ti_sysbios_hal_Timer_startup();
    ti_sysbios_knl_Swi_startup();
    ti_sysbios_knl_Task_startup();
}

#if defined(__ti__)
#include <_lock.h>
#endif

Void ti_sysbios_BIOS_exitFunc__I(Int notused)
{
    ti_sysbios_knl_Swi_disable();
    ti_sysbios_knl_Task_disable();
    ti_sysbios_BIOS_setThreadType(ti_sysbios_BIOS_ThreadType_Main);

#if defined(__ti__)
    if ((&ti_sysbios_BIOS_Module__state__V)->rtsGate != NULL) {
	_register_lock(_nop);
	_register_unlock(_nop);
    }
#endif
}




#if defined(__ti__)

#pragma FUNC_EXT_CALLED(malloc);
#pragma FUNC_EXT_CALLED(memalign);
#pragma FUNC_EXT_CALLED(free);
#pragma FUNC_EXT_CALLED(calloc);
#pragma FUNC_EXT_CALLED(realloc);

#define ATTRIBUTE

#else

#define ATTRIBUTE __attribute__ ((used))

#endif

#include <xdc/std.h>

#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <stdlib.h>
#include <string.h>

/* malloc header */
typedef struct Header {
    Ptr   actualBuf;
    UArg  size;
} Header;

/*
 *  ======== malloc ========
 */
Void ATTRIBUTE *malloc(SizeT size)
{
    Header *packet;
    xdc_runtime_Error_Block eb;

    xdc_runtime_Error_init(&eb);

    if (size == 0) {
	return (NULL);
    }

    packet = (Header *)xdc_runtime_Memory_alloc(NULL,
	(SizeT)(size + sizeof(Header)), 0, &eb);

    if (packet == NULL) {
	return (NULL);
    }

    packet->actualBuf = (Ptr)packet;
    packet->size = size + sizeof(Header);

    return (packet + 1);
}

/*
 *  ======== memalign ========
 *  mirrors the memalign() function from the TI run-time library
 */
Void ATTRIBUTE *memalign(SizeT alignment, SizeT size)
{
    Header                      *packet;
    Void                        *tmp;
    xdc_runtime_Error_Block     eb;

    xdc_runtime_Error_init(&eb);

    if (size == 0) {
	return (NULL);
    }

    /*
     * If alignment is not a power of two or is less than the minimum
     * alignment of malloc(), then simply return what malloc() returns.
     * (same as behaviour as RTS library's memalign())
     * Note: HeapMem's header is the same as this header after it is raised
     * to a power of 2.
     */
    if (alignment <= sizeof(Header) || (alignment & (alignment-1))) {
	return malloc(size);
    }

    /*
     * Allocate 'align + size' so that we have room for the 'packet'
     * and can return an aligned buffer.
     */
    tmp = xdc_runtime_Memory_alloc(NULL, alignment + size,
	(SizeT)alignment, &eb);

    if (tmp == NULL) {
	return (NULL);
    }

    packet = (Header *)((char *)tmp + alignment - sizeof(Header));

    packet->actualBuf = tmp;
    packet->size = size + sizeof(Header);

    return (packet + 1);
}

/*
 *  ======== calloc ========
 */
Void ATTRIBUTE *calloc(SizeT nmemb, SizeT size)
{
    SizeT       nbytes;
    Ptr         retval;

    nbytes = nmemb * size;

    retval = malloc(nbytes);
    if (retval != NULL) {
	(Void)memset(retval, (Int)'\0', nbytes);
    }

    return (retval);
}

/*
 *  ======== free ========
 */
Void ATTRIBUTE free(Void *ptr)
{
    Header      *packet;

    if (ptr != NULL) {
	packet = ((Header *)ptr) - 1;

	xdc_runtime_Memory_free(NULL, (Ptr)packet->actualBuf,
	    (packet->size + ((char*)packet - (char*)packet->actualBuf)));
    }
}

/*
 *  ======== realloc ========
 */
Void ATTRIBUTE *realloc(Void *ptr, SizeT size)
{
    Ptr         retval;
    Header      *packet;
    SizeT       oldSize;

    if (ptr == NULL) {
	retval = malloc(size);
    }
    else if (size == 0) {
	free(ptr);
	retval = NULL;
    }
    else {
	packet = (Header *)ptr - 1;
	retval = malloc(size);
	if (retval != NULL) {
	    oldSize = packet->size - sizeof(Header);
	    (Void)memcpy(retval, ptr, (size < oldSize) ? size : oldSize);
	    free(ptr);
	}
    }

    return (retval);
}


/*
 * ======== ti.sysbios.hal.Hwi TEMPLATE ========
 */

#ifdef __ti__
    #pragma FUNC_EXT_CALLED(ti_sysbios_hal_Hwi_HwiProxy_disable__E);
    #pragma FUNC_EXT_CALLED(ti_sysbios_hal_Hwi_HwiProxy_enable__E);
    #pragma FUNC_EXT_CALLED(ti_sysbios_hal_Hwi_HwiProxy_restore__E);
#endif

/*
 * ======== ti.sysbios.knl.Clock TEMPLATE ========
 */

Void ti_sysbios_knl_Clock_doTick__I(UArg arg) 
{
    /* update system time */
    (&ti_sysbios_knl_Clock_Module__state__V)->ticks++;

    (&ti_sysbios_knl_Clock_Module__state__V)->swiCount++;

    ti_sysbios_knl_Clock_logTick__E();

    if (!ti_sysbios_knl_Queue_empty(ti_sysbios_knl_Clock_Module_State_clockQ())) {

        ti_sysbios_knl_Swi_post((&ti_sysbios_knl_Clock_Module__state__V)->swi);

    }
}

/*
 * ======== ti.sysbios.family.arm.m3.Hwi TEMPLATE ========
 */

extern Void _c_int00();

#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_resetVectors, ".resetVecs");

/* const because it is meant to be placed in ROM */
const UInt32 ti_sysbios_family_arm_m3_Hwi_resetVectors[] = {
    (UInt32)(&__TI_STACK_BASE),
    (UInt32)(&_c_int00),
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),            /* NMI */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),      /* Hard Fault */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* Mem Fault */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* Bus Fault */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),     /* Usage Fault */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* reserved */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* reserved */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* reserved */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* reserved */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),         /* SVCall */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* Debug Mon */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),       /* reserved */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_pendSV__I),  /* pendSV */
    (UInt32)(ti_sysbios_family_arm_m3_Hwi_dispatch__I),  /* intNum = 15 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 16 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 17 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 18 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 19 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 20 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 21 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 22 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 23 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 24 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 25 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 26 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 27 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 28 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 29 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 30 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 31 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 32 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 33 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 34 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 35 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 36 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 37 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 38 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 39 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 40 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 41 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 42 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 43 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 44 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 45 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 46 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 47 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 48 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 49 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 50 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 51 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 52 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 53 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 54 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 55 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 56 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 57 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 58 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 59 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 60 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 61 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 62 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 63 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 64 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 65 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 66 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 67 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 68 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 69 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 70 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 71 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 72 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 73 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 74 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 75 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 76 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 77 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 78 */
    (UInt32)(&ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I),  /* intNum = 79 */
};


#define Hwi_nvic ti_sysbios_family_arm_m3_Hwi_nvic

/*
 *  ======== Hwi_disableInterrupt ========
 */
UInt ti_sysbios_family_arm_m3_Hwi_disableInterrupt__E(UInt intNum)
{
    UInt oldEnableState, index, mask;

    if (intNum >= 16) {

	index = (intNum-16) >> 5;

	mask = 1 << ((intNum-16) & 0x1f);

	oldEnableState = Hwi_nvic.ISER[index] & mask;

	Hwi_nvic.ICER[index] = mask;
    }
    else if (intNum == 15) {
	oldEnableState = Hwi_nvic.STCSR & 0x00000002;
	Hwi_nvic.STCSR &= ~0x00000002;  /* disable SysTick Int */
    }

    return oldEnableState;
}

/*
 *  ======== Hwi_enableInterrupt ========
 */
UInt ti_sysbios_family_arm_m3_Hwi_enableInterrupt__E(UInt intNum)
{
    UInt oldEnableState, index, mask;

    if (intNum >= 16) {

	index = (intNum-16) >> 5;

	mask = 1 << ((intNum-16) & 0x1f);

	oldEnableState = Hwi_nvic.ISER[index] & mask;

	Hwi_nvic.ISER[index] = mask;
    }
    else if (intNum == 15) {
	oldEnableState = Hwi_nvic.STCSR & 0x00000002;
	Hwi_nvic.STCSR |= 0x00000002;   /* enable SysTick Int */
    }

    return oldEnableState;
}

/*
 *  ======== Hwi_restoreInterrupt ========
 */
Void ti_sysbios_family_arm_m3_Hwi_restoreInterrupt__E(UInt intNum, UInt key)
{
    UInt index, mask;

    if (intNum >= 16) {

	index = (intNum-16) >> 5;

	mask = 1 << ((intNum-16) & 0x1f);

	if (key) {
	    Hwi_nvic.ISER[index] = mask;
	}
	else {
	    Hwi_nvic.ICER[index] = mask;
	}
    }
    else if (intNum == 15) {
	if (key) {
	    Hwi_nvic.STCSR |= 0x00000002;       /* enable SysTick Int */
	}
	else {
	    Hwi_nvic.STCSR &= ~0x00000002;      /* disable SysTick Int */
	}
    }
}
#undef Hwi_nvic

/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore TEMPLATE ========
 */


#ifdef __ti__

UInt32 ti_sysbios_family_arm_ducati_GateDualCore_gateArray[4];

#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_gateArray, ".ducatiGates");

#else

UInt32 ti_sysbios_family_arm_ducati_GateDualCore_gateArray[4] __attribute__ ((section(".ducatiGates")));

#endif



/*
 * ======== ti.sysbios.BIOS INITIALIZERS ========
 */

/* Module__state__V */
ti_sysbios_BIOS_Module_State__ ti_sysbios_BIOS_Module__state__V = {
    {
        (xdc_Bits32)0x0,  /* hi */
        (xdc_Bits32)0xbebc200,  /* lo */
    },  /* cpuFreq */
    (xdc_UInt)0x0,  /* rtsGateCount */
    ((xdc_IArg)(0x0)),  /* rtsGateKey */
    (ti_sysbios_BIOS_RtsGateProxy_Handle)&ti_sysbios_gates_GateMutex_Object__table__V[1],  /* rtsGate */
    ti_sysbios_BIOS_ThreadType_Main,  /* threadType */
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_BIOS_startFunc)),  /* startFunc */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__diagsEnabled ti_sysbios_BIOS_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__diagsIncluded ti_sysbios_BIOS_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__diagsMask ti_sysbios_BIOS_Module__diagsMask__C = ((CT__ti_sysbios_BIOS_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__gateObj ti_sysbios_BIOS_Module__gateObj__C = ((CT__ti_sysbios_BIOS_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__gatePrms ti_sysbios_BIOS_Module__gatePrms__C = ((CT__ti_sysbios_BIOS_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__id ti_sysbios_BIOS_Module__id__C = (xdc_Bits16)0x8012;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerDefined ti_sysbios_BIOS_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerObj ti_sysbios_BIOS_Module__loggerObj__C = ((CT__ti_sysbios_BIOS_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerFxn0 ti_sysbios_BIOS_Module__loggerFxn0__C = ((CT__ti_sysbios_BIOS_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerFxn1 ti_sysbios_BIOS_Module__loggerFxn1__C = ((CT__ti_sysbios_BIOS_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerFxn2 ti_sysbios_BIOS_Module__loggerFxn2__C = ((CT__ti_sysbios_BIOS_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerFxn4 ti_sysbios_BIOS_Module__loggerFxn4__C = ((CT__ti_sysbios_BIOS_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__loggerFxn8 ti_sysbios_BIOS_Module__loggerFxn8__C = ((CT__ti_sysbios_BIOS_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_BIOS_Module__startupDoneFxn ti_sysbios_BIOS_Module__startupDoneFxn__C = ((CT__ti_sysbios_BIOS_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_BIOS_Object__count ti_sysbios_BIOS_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_BIOS_Object__heap ti_sysbios_BIOS_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_BIOS_Object__sizeof ti_sysbios_BIOS_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_BIOS_Object__table ti_sysbios_BIOS_Object__table__C = 0;

/* smpEnabled__C */
__FAR__ const CT__ti_sysbios_BIOS_smpEnabled ti_sysbios_BIOS_smpEnabled__C = 0;

/* cpuFreq__C */
__FAR__ const CT__ti_sysbios_BIOS_cpuFreq ti_sysbios_BIOS_cpuFreq__C = {
    (xdc_Bits32)0x0,  /* hi */
    (xdc_Bits32)0xbebc200,  /* lo */
};

/* taskEnabled__C */
__FAR__ const CT__ti_sysbios_BIOS_taskEnabled ti_sysbios_BIOS_taskEnabled__C = 1;

/* swiEnabled__C */
__FAR__ const CT__ti_sysbios_BIOS_swiEnabled ti_sysbios_BIOS_swiEnabled__C = 1;

/* clockEnabled__C */
__FAR__ const CT__ti_sysbios_BIOS_clockEnabled ti_sysbios_BIOS_clockEnabled__C = 1;

/* heapSize__C */
__FAR__ const CT__ti_sysbios_BIOS_heapSize ti_sysbios_BIOS_heapSize__C = (xdc_SizeT)0x15000;

/* heapSection__C */
__FAR__ const CT__ti_sysbios_BIOS_heapSection ti_sysbios_BIOS_heapSection__C = 0;

/* installedErrorHook__C */
__FAR__ const CT__ti_sysbios_BIOS_installedErrorHook ti_sysbios_BIOS_installedErrorHook__C = ((CT__ti_sysbios_BIOS_installedErrorHook)((xdc_Fxn)xdc_runtime_Error_print__E));


/*
 * ======== ti.sysbios.BIOS_RtsGateProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.family.arm.ducati.CTM INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsMask ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__gateObj ti_sysbios_family_arm_ducati_CTM_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__gatePrms ti_sysbios_family_arm_ducati_CTM_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__id ti_sysbios_family_arm_ducati_CTM_Module__id__C = (xdc_Bits16)0x8027;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerObj ti_sysbios_family_arm_ducati_CTM_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8 ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__count ti_sysbios_family_arm_ducati_CTM_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__heap ti_sysbios_family_arm_ducati_CTM_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__sizeof ti_sysbios_family_arm_ducati_CTM_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_CTM_Object__table ti_sysbios_family_arm_ducati_CTM_Object__table__C = 0;


/*
 * ======== ti.sysbios.family.arm.ducati.Core INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__diagsMask ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__gateObj ti_sysbios_family_arm_ducati_Core_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__gatePrms ti_sysbios_family_arm_ducati_Core_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__id ti_sysbios_family_arm_ducati_Core_Module__id__C = (xdc_Bits16)0x8026;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerDefined ti_sysbios_family_arm_ducati_Core_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerObj ti_sysbios_family_arm_ducati_Core_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8 ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__count ti_sysbios_family_arm_ducati_Core_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__heap ti_sysbios_family_arm_ducati_Core_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__sizeof ti_sysbios_family_arm_ducati_Core_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_Object__table ti_sysbios_family_arm_ducati_Core_Object__table__C = 0;

/* id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_id ti_sysbios_family_arm_ducati_Core_id__C = (xdc_UInt)0x1;

/* E_mismatchedIds__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Core_E_mismatchedIds ti_sysbios_family_arm_ducati_Core_E_mismatchedIds__C = (((xdc_runtime_Error_Id)3401) << 16 | 0);


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_family_arm_ducati_GateDualCore_Params ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C = {
    sizeof (ti_sysbios_family_arm_ducati_GateDualCore_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C.__iprms, /* instance */
    (xdc_UInt)0x0,  /* index */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_family_arm_ducati_GateDualCore_Module__ ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V = {
    {&ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link,  /* link.next */
    &ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_family_arm_ducati_GateDualCore_Object__ ti_sysbios_family_arm_ducati_GateDualCore_Object__table__V[1] = {
    {/* instance#0 */
        &ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C,
        (xdc_UInt)0x0,  /* index */
        ((xdc_UInt32*)0),  /* gatePtr */
        ((xdc_UInt8*)0),  /* gateBytePtr */
        (xdc_UInt)0x0,  /* stalls */
        (xdc_UInt)0x0,  /* noStalls */
        (xdc_UInt)0x0,  /* totalStalls */
        (xdc_UInt)0x0,  /* maxStall */
    },
};

/* Module__state__V */
ti_sysbios_family_arm_ducati_GateDualCore_Module_State__ ti_sysbios_family_arm_ducati_GateDualCore_Module__state__V = {
    {
        (xdc_UInt8)0x1,  /* [0] */
        (xdc_UInt8)0x0,  /* [1] */
        (xdc_UInt8)0x0,  /* [2] */
        (xdc_UInt8)0x0,  /* [3] */
    },  /* usedGates */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__id ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C = (xdc_Bits16)0x8028;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__count ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__heap ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof__C = sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_Object__table ti_sysbios_family_arm_ducati_GateDualCore_Object__table__C = ti_sysbios_family_arm_ducati_GateDualCore_Object__table__V;

/* A_nestedEnter__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter__C = (((xdc_runtime_Assert_Id)1360) << 16 | 16);

/* E_invalidIndex__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex__C = (((xdc_runtime_Error_Id)3466) << 16 | 0);

/* E_gateInUse__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse__C = (((xdc_runtime_Error_Id)3484) << 16 | 0);

/* gateArrayAddress__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress__C = ((CT__ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress)(0x7f0));

/* numGates__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_numGates ti_sysbios_family_arm_ducati_GateDualCore_numGates__C = (xdc_UInt)0x4;

/* initGates__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_initGates ti_sysbios_family_arm_ducati_GateDualCore_initGates__C = 0;

/* enableStats__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_GateDualCore_enableStats ti_sysbios_family_arm_ducati_GateDualCore_enableStats__C = 0;


/*
 * ======== ti.sysbios.family.arm.ducati.Timer INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_ducati_Timer_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_family_arm_ducati_Timer_Params ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C = {
    sizeof (ti_sysbios_family_arm_ducati_Timer_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C.__iprms, /* instance */
    ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS,  /* runMode */
    ti_sysbios_interfaces_ITimer_StartMode_AUTO,  /* startMode */
    ((xdc_UArg)0),  /* arg */
    (xdc_UInt32)0x0,  /* period */
    ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS,  /* periodType */
    {
        (xdc_Bits32)0x0,  /* hi */
        (xdc_Bits32)0x0,  /* lo */
    },  /* extFreq */
    ((ti_sysbios_family_arm_m3_Hwi_Params*)0),  /* hwiParams */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_family_arm_ducati_Timer_Module__ ti_sysbios_family_arm_ducati_Timer_Module__root__V = {
    {&ti_sysbios_family_arm_ducati_Timer_Module__root__V.link,  /* link.next */
    &ti_sysbios_family_arm_ducati_Timer_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_family_arm_ducati_Timer_Object__ ti_sysbios_family_arm_ducati_Timer_Object__table__V[1] = {
    {/* instance#0 */
        0,
        1,  /* staticInst */
        (xdc_Int)0x0,  /* id */
        (xdc_Int)0x0,  /* ctmid */
        ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS,  /* runMode */
        ti_sysbios_interfaces_ITimer_StartMode_AUTO,  /* startMode */
        (xdc_UInt)0x30d40,  /* period */
        ti_sysbios_interfaces_ITimer_PeriodType_COUNTS,  /* periodType */
        (xdc_UInt)0xf,  /* intNum */
        ((xdc_UArg)0),  /* arg */
        ((xdc_Void(*)(xdc_UArg))((xdc_Fxn)ti_sysbios_knl_Clock_doTick__I)),  /* tickFxn */
        {
            (xdc_Bits32)0x0,  /* hi */
            (xdc_Bits32)0x0,  /* lo */
        },  /* extFreq */
        (ti_sysbios_family_arm_m3_Hwi_Handle)&ti_sysbios_family_arm_m3_Hwi_Object__table__V[1],  /* hwi */
    },
};

/* Module__state__V */
ti_sysbios_family_arm_ducati_Timer_Module_State__ ti_sysbios_family_arm_ducati_Timer_Module__state__V = {
    (xdc_UInt)0x2,  /* availMask */
    {
        (ti_sysbios_family_arm_ducati_Timer_Handle)&ti_sysbios_family_arm_ducati_Timer_Object__table__V[0],  /* [0] */
        0,  /* [1] */
    },  /* handles */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__diagsEnabled ti_sysbios_family_arm_ducati_Timer_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__diagsIncluded ti_sysbios_family_arm_ducati_Timer_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__diagsMask ti_sysbios_family_arm_ducati_Timer_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__gateObj ti_sysbios_family_arm_ducati_Timer_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__gatePrms ti_sysbios_family_arm_ducati_Timer_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__id ti_sysbios_family_arm_ducati_Timer_Module__id__C = (xdc_Bits16)0x8029;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerDefined ti_sysbios_family_arm_ducati_Timer_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerObj ti_sysbios_family_arm_ducati_Timer_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Module__startupDoneFxn ti_sysbios_family_arm_ducati_Timer_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_Timer_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Object__count ti_sysbios_family_arm_ducati_Timer_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Object__heap ti_sysbios_family_arm_ducati_Timer_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Object__sizeof ti_sysbios_family_arm_ducati_Timer_Object__sizeof__C = sizeof(ti_sysbios_family_arm_ducati_Timer_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_Object__table ti_sysbios_family_arm_ducati_Timer_Object__table__C = ti_sysbios_family_arm_ducati_Timer_Object__table__V;

/* E_invalidTimer__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_E_invalidTimer ti_sysbios_family_arm_ducati_Timer_E_invalidTimer__C = (((xdc_runtime_Error_Id)3268) << 16 | 0);

/* E_notAvailable__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_E_notAvailable ti_sysbios_family_arm_ducati_Timer_E_notAvailable__C = (((xdc_runtime_Error_Id)3304) << 16 | 0);

/* E_cannotSupport__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_E_cannotSupport ti_sysbios_family_arm_ducati_Timer_E_cannotSupport__C = (((xdc_runtime_Error_Id)3343) << 16 | 0);

/* anyMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_anyMask ti_sysbios_family_arm_ducati_Timer_anyMask__C = (xdc_UInt)0x3;

/* startupNeeded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Timer_startupNeeded ti_sysbios_family_arm_ducati_Timer_startupNeeded__C = (xdc_UInt)0x1;


/*
 * ======== ti.sysbios.family.arm.ducati.TimerSupport INITIALIZERS ========
 */

/* --> ti_sysbios_family_arm_ducati_TimerSupport_Module_State_0_intNums__A */
__T1_ti_sysbios_family_arm_ducati_TimerSupport_Module_State__intNums ti_sysbios_family_arm_ducati_TimerSupport_Module_State_0_intNums__A[4] = {
    (xdc_UInt8)0x1c,  /* [0] */
    (xdc_UInt8)0x1d,  /* [1] */
    (xdc_UInt8)0x1e,  /* [2] */
    (xdc_UInt8)0x1f,  /* [3] */
};

/* Module__state__V */
ti_sysbios_family_arm_ducati_TimerSupport_Module_State__ ti_sysbios_family_arm_ducati_TimerSupport_Module__state__V = {
    ((void*)ti_sysbios_family_arm_ducati_TimerSupport_Module_State_0_intNums__A),  /* intNums */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsEnabled ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsIncluded ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsMask ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__gateObj ti_sysbios_family_arm_ducati_TimerSupport_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__gatePrms ti_sysbios_family_arm_ducati_TimerSupport_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__id ti_sysbios_family_arm_ducati_TimerSupport_Module__id__C = (xdc_Bits16)0x802a;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerDefined ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerObj ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn0 ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn1 ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn2 ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn4 ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn8 ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDoneFxn ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Object__count ti_sysbios_family_arm_ducati_TimerSupport_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Object__heap ti_sysbios_family_arm_ducati_TimerSupport_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Object__sizeof ti_sysbios_family_arm_ducati_TimerSupport_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimerSupport_Object__table ti_sysbios_family_arm_ducati_TimerSupport_Object__table__C = 0;


/*
 * ======== ti.sysbios.family.arm.ducati.TimestampProvider INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsEnabled ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsIncluded ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsMask ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__gateObj ti_sysbios_family_arm_ducati_TimestampProvider_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__gatePrms ti_sysbios_family_arm_ducati_TimestampProvider_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__id ti_sysbios_family_arm_ducati_TimestampProvider_Module__id__C = (xdc_Bits16)0x802b;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerDefined ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerObj ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn0 ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn1 ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn2 ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn4 ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn8 ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDoneFxn ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Object__count ti_sysbios_family_arm_ducati_TimestampProvider_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Object__heap ti_sysbios_family_arm_ducati_TimestampProvider_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Object__sizeof ti_sysbios_family_arm_ducati_TimestampProvider_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_Object__table ti_sysbios_family_arm_ducati_TimestampProvider_Object__table__C = 0;

/* inpsel__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_TimestampProvider_inpsel ti_sysbios_family_arm_ducati_TimestampProvider_inpsel__C = (xdc_UInt8)0x0;


/*
 * ======== ti.sysbios.family.arm.ducati.Wugen INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__diagsEnabled ti_sysbios_family_arm_ducati_Wugen_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__diagsIncluded ti_sysbios_family_arm_ducati_Wugen_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__diagsMask ti_sysbios_family_arm_ducati_Wugen_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__gateObj ti_sysbios_family_arm_ducati_Wugen_Module__gateObj__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__gatePrms ti_sysbios_family_arm_ducati_Wugen_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__id ti_sysbios_family_arm_ducati_Wugen_Module__id__C = (xdc_Bits16)0x802c;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerDefined ti_sysbios_family_arm_ducati_Wugen_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerObj ti_sysbios_family_arm_ducati_Wugen_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn0 ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn1 ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn2 ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn4 ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn8 ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Module__startupDoneFxn ti_sysbios_family_arm_ducati_Wugen_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_ducati_Wugen_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Object__count ti_sysbios_family_arm_ducati_Wugen_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Object__heap ti_sysbios_family_arm_ducati_Wugen_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Object__sizeof ti_sysbios_family_arm_ducati_Wugen_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_ducati_Wugen_Object__table ti_sysbios_family_arm_ducati_Wugen_Object__table__C = 0;


/*
 * ======== ti.sysbios.family.arm.m3.Hwi INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_m3_Hwi_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_family_arm_m3_Hwi_Params ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C = {
    sizeof (ti_sysbios_family_arm_m3_Hwi_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C.__iprms, /* instance */
    ti_sysbios_interfaces_IHwi_MaskingOption_LOWER,  /* maskSetting */
    ((xdc_UArg)(0x0)),  /* arg */
    1,  /* enableInt */
    (xdc_Int)(-0x0 - 1),  /* eventId */
    (xdc_Int)0xff,  /* priority */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* --> ti_sysbios_family_arm_m3_Hwi_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Instance_State__hookEnv ti_sysbios_family_arm_m3_Hwi_Instance_State_0_hookEnv__A[1];

/* --> ti_sysbios_family_arm_m3_Hwi_Instance_State_1_hookEnv__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Instance_State__hookEnv ti_sysbios_family_arm_m3_Hwi_Instance_State_1_hookEnv__A[1];

/* Module__root__V */
ti_sysbios_family_arm_m3_Hwi_Module__ ti_sysbios_family_arm_m3_Hwi_Module__root__V = {
    {&ti_sysbios_family_arm_m3_Hwi_Module__root__V.link,  /* link.next */
    &ti_sysbios_family_arm_m3_Hwi_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_family_arm_m3_Hwi_Object__ ti_sysbios_family_arm_m3_Hwi_Object__table__V[2] = {
    {/* instance#0 */
        0,
        ((xdc_UArg)(0x1)),  /* arg */
        ((xdc_Void(*)(xdc_UArg))((xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I)),  /* fxn */
        (xdc_Int)0x11,  /* intNum */
        ((xdc_UArg)0),  /* irp */
        ((void*)ti_sysbios_family_arm_m3_Hwi_Instance_State_0_hookEnv__A),  /* hookEnv */
    },
    {/* instance#1 */
        0,
        ((xdc_UArg)((void*)(ti_sysbios_family_arm_ducati_Timer_Handle)&ti_sysbios_family_arm_ducati_Timer_Object__table__V[0])),  /* arg */
        ((xdc_Void(*)(xdc_UArg))((xdc_Fxn)ti_sysbios_family_arm_ducati_Timer_periodicStub__E)),  /* fxn */
        (xdc_Int)0xf,  /* intNum */
        ((xdc_UArg)0),  /* irp */
        ((void*)ti_sysbios_family_arm_m3_Hwi_Instance_State_1_hookEnv__A),  /* hookEnv */
    },
};

/* --> ti_sysbios_family_arm_m3_Hwi_Module_State_0_priorities__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Module_State__priorities ti_sysbios_family_arm_m3_Hwi_Module_State_0_priorities__A[80] = {
    (xdc_UInt8)0x0,  /* [0] */
    (xdc_UInt8)0x0,  /* [1] */
    (xdc_UInt8)0x0,  /* [2] */
    (xdc_UInt8)0x0,  /* [3] */
    (xdc_UInt8)0x0,  /* [4] */
    (xdc_UInt8)0x0,  /* [5] */
    (xdc_UInt8)0x0,  /* [6] */
    (xdc_UInt8)0x0,  /* [7] */
    (xdc_UInt8)0x0,  /* [8] */
    (xdc_UInt8)0x0,  /* [9] */
    (xdc_UInt8)0x0,  /* [10] */
    (xdc_UInt8)0x0,  /* [11] */
    (xdc_UInt8)0x0,  /* [12] */
    (xdc_UInt8)0x0,  /* [13] */
    (xdc_UInt8)0x0,  /* [14] */
    (xdc_UInt8)0xff,  /* [15] */
    (xdc_UInt8)0xff,  /* [16] */
    (xdc_UInt8)0x0,  /* [17] */
    (xdc_UInt8)0xff,  /* [18] */
    (xdc_UInt8)0xff,  /* [19] */
    (xdc_UInt8)0xff,  /* [20] */
    (xdc_UInt8)0xff,  /* [21] */
    (xdc_UInt8)0xff,  /* [22] */
    (xdc_UInt8)0xff,  /* [23] */
    (xdc_UInt8)0xff,  /* [24] */
    (xdc_UInt8)0xff,  /* [25] */
    (xdc_UInt8)0xff,  /* [26] */
    (xdc_UInt8)0xff,  /* [27] */
    (xdc_UInt8)0xff,  /* [28] */
    (xdc_UInt8)0xff,  /* [29] */
    (xdc_UInt8)0xff,  /* [30] */
    (xdc_UInt8)0xff,  /* [31] */
    (xdc_UInt8)0xff,  /* [32] */
    (xdc_UInt8)0xff,  /* [33] */
    (xdc_UInt8)0xff,  /* [34] */
    (xdc_UInt8)0xff,  /* [35] */
    (xdc_UInt8)0xff,  /* [36] */
    (xdc_UInt8)0xff,  /* [37] */
    (xdc_UInt8)0xff,  /* [38] */
    (xdc_UInt8)0xff,  /* [39] */
    (xdc_UInt8)0xff,  /* [40] */
    (xdc_UInt8)0xff,  /* [41] */
    (xdc_UInt8)0xff,  /* [42] */
    (xdc_UInt8)0xff,  /* [43] */
    (xdc_UInt8)0xff,  /* [44] */
    (xdc_UInt8)0xff,  /* [45] */
    (xdc_UInt8)0xff,  /* [46] */
    (xdc_UInt8)0xff,  /* [47] */
    (xdc_UInt8)0xff,  /* [48] */
    (xdc_UInt8)0xff,  /* [49] */
    (xdc_UInt8)0xff,  /* [50] */
    (xdc_UInt8)0xff,  /* [51] */
    (xdc_UInt8)0xff,  /* [52] */
    (xdc_UInt8)0xff,  /* [53] */
    (xdc_UInt8)0xff,  /* [54] */
    (xdc_UInt8)0xff,  /* [55] */
    (xdc_UInt8)0xff,  /* [56] */
    (xdc_UInt8)0xff,  /* [57] */
    (xdc_UInt8)0xff,  /* [58] */
    (xdc_UInt8)0xff,  /* [59] */
    (xdc_UInt8)0xff,  /* [60] */
    (xdc_UInt8)0xff,  /* [61] */
    (xdc_UInt8)0xff,  /* [62] */
    (xdc_UInt8)0xff,  /* [63] */
    (xdc_UInt8)0xff,  /* [64] */
    (xdc_UInt8)0xff,  /* [65] */
    (xdc_UInt8)0xff,  /* [66] */
    (xdc_UInt8)0xff,  /* [67] */
    (xdc_UInt8)0xff,  /* [68] */
    (xdc_UInt8)0xff,  /* [69] */
    (xdc_UInt8)0xff,  /* [70] */
    (xdc_UInt8)0xff,  /* [71] */
    (xdc_UInt8)0xff,  /* [72] */
    (xdc_UInt8)0xff,  /* [73] */
    (xdc_UInt8)0xff,  /* [74] */
    (xdc_UInt8)0xff,  /* [75] */
    (xdc_UInt8)0xff,  /* [76] */
    (xdc_UInt8)0xff,  /* [77] */
    (xdc_UInt8)0xff,  /* [78] */
    (xdc_UInt8)0xff,  /* [79] */
};

/* --> ti_sysbios_family_arm_m3_Hwi_Module_State_0_dispatchTable__A */
__T1_ti_sysbios_family_arm_m3_Hwi_Module_State__dispatchTable ti_sysbios_family_arm_m3_Hwi_Module_State_0_dispatchTable__A[80] = {
    0,  /* [0] */
    0,  /* [1] */
    0,  /* [2] */
    0,  /* [3] */
    0,  /* [4] */
    0,  /* [5] */
    0,  /* [6] */
    0,  /* [7] */
    0,  /* [8] */
    0,  /* [9] */
    0,  /* [10] */
    0,  /* [11] */
    0,  /* [12] */
    0,  /* [13] */
    0,  /* [14] */
    (ti_sysbios_family_arm_m3_Hwi_Handle)&ti_sysbios_family_arm_m3_Hwi_Object__table__V[1],  /* [15] */
    0,  /* [16] */
    (ti_sysbios_family_arm_m3_Hwi_Handle)&ti_sysbios_family_arm_m3_Hwi_Object__table__V[0],  /* [17] */
    0,  /* [18] */
    0,  /* [19] */
    0,  /* [20] */
    0,  /* [21] */
    0,  /* [22] */
    0,  /* [23] */
    0,  /* [24] */
    0,  /* [25] */
    0,  /* [26] */
    0,  /* [27] */
    0,  /* [28] */
    0,  /* [29] */
    0,  /* [30] */
    0,  /* [31] */
    0,  /* [32] */
    0,  /* [33] */
    0,  /* [34] */
    0,  /* [35] */
    0,  /* [36] */
    0,  /* [37] */
    0,  /* [38] */
    0,  /* [39] */
    0,  /* [40] */
    0,  /* [41] */
    0,  /* [42] */
    0,  /* [43] */
    0,  /* [44] */
    0,  /* [45] */
    0,  /* [46] */
    0,  /* [47] */
    0,  /* [48] */
    0,  /* [49] */
    0,  /* [50] */
    0,  /* [51] */
    0,  /* [52] */
    0,  /* [53] */
    0,  /* [54] */
    0,  /* [55] */
    0,  /* [56] */
    0,  /* [57] */
    0,  /* [58] */
    0,  /* [59] */
    0,  /* [60] */
    0,  /* [61] */
    0,  /* [62] */
    0,  /* [63] */
    0,  /* [64] */
    0,  /* [65] */
    0,  /* [66] */
    0,  /* [67] */
    0,  /* [68] */
    0,  /* [69] */
    0,  /* [70] */
    0,  /* [71] */
    0,  /* [72] */
    0,  /* [73] */
    0,  /* [74] */
    0,  /* [75] */
    0,  /* [76] */
    0,  /* [77] */
    0,  /* [78] */
    0,  /* [79] */
};

/* Module__state__V */
ti_sysbios_family_arm_m3_Hwi_Module_State__ ti_sysbios_family_arm_m3_Hwi_Module__state__V = {
    (xdc_Bits16)0x8000,  /* enables */
    {
        (xdc_Bits32)0x2,  /* [0] */
        (xdc_Bits32)0x0,  /* [1] */
        (xdc_Bits32)0x0,  /* [2] */
        (xdc_Bits32)0x0,  /* [3] */
        (xdc_Bits32)0x0,  /* [4] */
        (xdc_Bits32)0x0,  /* [5] */
        (xdc_Bits32)0x0,  /* [6] */
        (xdc_Bits32)0x0,  /* [7] */
    },  /* iser */
    ((void*)ti_sysbios_family_arm_m3_Hwi_Module_State_0_priorities__A),  /* priorities */
    ((xdc_Char*)0),  /* taskSP */
    0,  /* excActive */
    ((ti_sysbios_family_arm_m3_Hwi_ExcContext*)0),  /* excContext */
    ((xdc_Ptr)0),  /* excStack */
    ((xdc_Ptr)0),  /* isrStack */
    ((xdc_Ptr)((void*)&__TI_STACK_BASE)),  /* isrStackBase */
    (xdc_SizeT)0x4000,  /* isrStackSize */
    ((xdc_Ptr)(0x800)),  /* vectorTableBase */
    (xdc_UInt)0x101,  /* swiTaskKeys */
    ((void*)ti_sysbios_family_arm_m3_Hwi_Module_State_0_dispatchTable__A),  /* dispatchTable */
};

/* --> ti_sysbios_family_arm_m3_Hwi_hooks__A */
const __T1_ti_sysbios_family_arm_m3_Hwi_hooks ti_sysbios_family_arm_m3_Hwi_hooks__A[1] = {
    {
        ((xdc_Void(*)(xdc_Int))0),  /* registerFxn */
        ((xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle,xdc_runtime_Error_Block*))0),  /* createFxn */
        ((xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle))((xdc_Fxn)ti_sysbios_utils_Load_hwiBeginHook__E)),  /* beginFxn */
        ((xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle))((xdc_Fxn)ti_sysbios_utils_Load_hwiEndHook__E)),  /* endFxn */
        ((xdc_Void(*)(ti_sysbios_interfaces_IHwi_Handle))0),  /* deleteFxn */
    },  /* [0] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__diagsEnabled ti_sysbios_family_arm_m3_Hwi_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__diagsIncluded ti_sysbios_family_arm_m3_Hwi_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__diagsMask ti_sysbios_family_arm_m3_Hwi_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__gateObj ti_sysbios_family_arm_m3_Hwi_Module__gateObj__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__gatePrms ti_sysbios_family_arm_m3_Hwi_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__id ti_sysbios_family_arm_m3_Hwi_Module__id__C = (xdc_Bits16)0x8022;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerDefined ti_sysbios_family_arm_m3_Hwi_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerObj ti_sysbios_family_arm_m3_Hwi_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Module__startupDoneFxn ti_sysbios_family_arm_m3_Hwi_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_m3_Hwi_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Object__count ti_sysbios_family_arm_m3_Hwi_Object__count__C = 2;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Object__heap ti_sysbios_family_arm_m3_Hwi_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Object__sizeof ti_sysbios_family_arm_m3_Hwi_Object__sizeof__C = sizeof(ti_sysbios_family_arm_m3_Hwi_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_Object__table ti_sysbios_family_arm_m3_Hwi_Object__table__C = ti_sysbios_family_arm_m3_Hwi_Object__table__V;

/* dispatcherAutoNestingSupport__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport ti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport__C = 1;

/* dispatcherSwiSupport__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport ti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__C = 1;

/* dispatcherTaskSupport__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport ti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__C = 1;

/* dispatcherIrpTrackingSupport__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport ti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport__C = 1;

/* NUM_INTERRUPTS__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_NUM_INTERRUPTS ti_sysbios_family_arm_m3_Hwi_NUM_INTERRUPTS__C = (xdc_Int)0x50;

/* NUM_PRIORITIES__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_NUM_PRIORITIES ti_sysbios_family_arm_m3_Hwi_NUM_PRIORITIES__C = (xdc_Int)0x10;

/* LM_begin__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_LM_begin ti_sysbios_family_arm_m3_Hwi_LM_begin__C = (((xdc_runtime_Log_Event)4537) << 16 | 768);

/* LD_end__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_LD_end ti_sysbios_family_arm_m3_Hwi_LD_end__C = (((xdc_runtime_Log_Event)4607) << 16 | 512);

/* A_unsupportedMaskingOption__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_A_unsupportedMaskingOption ti_sysbios_family_arm_m3_Hwi_A_unsupportedMaskingOption__C = (((xdc_runtime_Assert_Id)1307) << 16 | 16);

/* E_alreadyDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_alreadyDefined ti_sysbios_family_arm_m3_Hwi_E_alreadyDefined__C = (((xdc_runtime_Error_Id)2860) << 16 | 0);

/* E_exception__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_exception ti_sysbios_family_arm_m3_Hwi_E_exception__C = (((xdc_runtime_Error_Id)2908) << 16 | 0);

/* E_noIsr__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_noIsr ti_sysbios_family_arm_m3_Hwi_E_noIsr__C = (((xdc_runtime_Error_Id)3113) << 16 | 0);

/* E_NMI__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_NMI ti_sysbios_family_arm_m3_Hwi_E_NMI__C = (((xdc_runtime_Error_Id)3141) << 16 | 0);

/* E_hardFault__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_hardFault ti_sysbios_family_arm_m3_Hwi_E_hardFault__C = (((xdc_runtime_Error_Id)3151) << 16 | 0);

/* E_memFault__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_memFault ti_sysbios_family_arm_m3_Hwi_E_memFault__C = (((xdc_runtime_Error_Id)3167) << 16 | 0);

/* E_busFault__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_busFault ti_sysbios_family_arm_m3_Hwi_E_busFault__C = (((xdc_runtime_Error_Id)3182) << 16 | 0);

/* E_usageFault__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_usageFault ti_sysbios_family_arm_m3_Hwi_E_usageFault__C = (((xdc_runtime_Error_Id)3197) << 16 | 0);

/* E_svCall__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_svCall ti_sysbios_family_arm_m3_Hwi_E_svCall__C = (((xdc_runtime_Error_Id)3214) << 16 | 0);

/* E_debugMon__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_debugMon ti_sysbios_family_arm_m3_Hwi_E_debugMon__C = (((xdc_runtime_Error_Id)3235) << 16 | 0);

/* E_reserved__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_E_reserved ti_sysbios_family_arm_m3_Hwi_E_reserved__C = (((xdc_runtime_Error_Id)3250) << 16 | 0);

/* excHookFunc__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_excHookFunc ti_sysbios_family_arm_m3_Hwi_excHookFunc__C = ((CT__ti_sysbios_family_arm_m3_Hwi_excHookFunc)0);

/* disablePriority__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_disablePriority ti_sysbios_family_arm_m3_Hwi_disablePriority__C = (xdc_UInt)0x10;

/* priGroup__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_priGroup ti_sysbios_family_arm_m3_Hwi_priGroup__C = (xdc_UInt)0x0;

/* swiDisable__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_swiDisable ti_sysbios_family_arm_m3_Hwi_swiDisable__C = ((CT__ti_sysbios_family_arm_m3_Hwi_swiDisable)((xdc_Fxn)ti_sysbios_knl_Swi_disable__E));

/* swiRestoreHwi__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_swiRestoreHwi ti_sysbios_family_arm_m3_Hwi_swiRestoreHwi__C = ((CT__ti_sysbios_family_arm_m3_Hwi_swiRestoreHwi)((xdc_Fxn)ti_sysbios_knl_Swi_restoreHwi__E));

/* taskDisable__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_taskDisable ti_sysbios_family_arm_m3_Hwi_taskDisable__C = ((CT__ti_sysbios_family_arm_m3_Hwi_taskDisable)((xdc_Fxn)ti_sysbios_knl_Task_disable__E));

/* taskRestoreHwi__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_taskRestoreHwi ti_sysbios_family_arm_m3_Hwi_taskRestoreHwi__C = ((CT__ti_sysbios_family_arm_m3_Hwi_taskRestoreHwi)((xdc_Fxn)ti_sysbios_knl_Task_restoreHwi__E));

/* ccr__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_ccr ti_sysbios_family_arm_m3_Hwi_ccr__C = (xdc_UInt32)0x200;

/* hooks__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_hooks ti_sysbios_family_arm_m3_Hwi_hooks__C = {1, ((__T1_ti_sysbios_family_arm_m3_Hwi_hooks*)ti_sysbios_family_arm_m3_Hwi_hooks__A)};

/* excHandlerFunc__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_Hwi_excHandlerFunc ti_sysbios_family_arm_m3_Hwi_excHandlerFunc__C = ((CT__ti_sysbios_family_arm_m3_Hwi_excHandlerFunc)((xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_excHandlerMax__I));


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsEnabled ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsIncluded ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsMask ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gateObj ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gateObj__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gatePrms ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__id ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__id__C = (xdc_Bits16)0x8023;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerDefined ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerObj ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn0 ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn1 ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn2 ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn4 ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn8 ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDoneFxn ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__count ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__heap ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__sizeof ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__table ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__table__C = 0;


/*
 * ======== ti.sysbios.family.arm.m3.TaskSupport INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__id ti_sysbios_family_arm_m3_TaskSupport_Module__id__C = (xdc_Bits16)0x8024;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8 ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Module__startupDoneFxn ti_sysbios_family_arm_m3_TaskSupport_Module__startupDoneFxn__C = ((CT__ti_sysbios_family_arm_m3_TaskSupport_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__count ti_sysbios_family_arm_m3_TaskSupport_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__heap ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_Object__table ti_sysbios_family_arm_m3_TaskSupport_Object__table__C = 0;

/* defaultStackSize__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C = (xdc_SizeT)0x800;

/* stackAlignment__C */
__FAR__ const CT__ti_sysbios_family_arm_m3_TaskSupport_stackAlignment ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C = (xdc_UInt)0x8;


/*
 * ======== ti.sysbios.gates.GateHwi INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_gates_GateHwi_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_gates_GateHwi_Params ti_sysbios_gates_GateHwi_Object__PARAMS__C = {
    sizeof (ti_sysbios_gates_GateHwi_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_gates_GateHwi_Object__PARAMS__C.__iprms, /* instance */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_gates_GateHwi_Module__ ti_sysbios_gates_GateHwi_Module__root__V = {
    {&ti_sysbios_gates_GateHwi_Module__root__V.link,  /* link.next */
    &ti_sysbios_gates_GateHwi_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_gates_GateHwi_Object__ ti_sysbios_gates_GateHwi_Object__table__V[1] = {
    {/* instance#0 */
        &ti_sysbios_gates_GateHwi_Module__FXNS__C,
    },
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsEnabled ti_sysbios_gates_GateHwi_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsIncluded ti_sysbios_gates_GateHwi_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__diagsMask ti_sysbios_gates_GateHwi_Module__diagsMask__C = ((CT__ti_sysbios_gates_GateHwi_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__gateObj ti_sysbios_gates_GateHwi_Module__gateObj__C = ((CT__ti_sysbios_gates_GateHwi_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__gatePrms ti_sysbios_gates_GateHwi_Module__gatePrms__C = ((CT__ti_sysbios_gates_GateHwi_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__id ti_sysbios_gates_GateHwi_Module__id__C = (xdc_Bits16)0x8034;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerDefined ti_sysbios_gates_GateHwi_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerObj ti_sysbios_gates_GateHwi_Module__loggerObj__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn0 ti_sysbios_gates_GateHwi_Module__loggerFxn0__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn1 ti_sysbios_gates_GateHwi_Module__loggerFxn1__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn2 ti_sysbios_gates_GateHwi_Module__loggerFxn2__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn4 ti_sysbios_gates_GateHwi_Module__loggerFxn4__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__loggerFxn8 ti_sysbios_gates_GateHwi_Module__loggerFxn8__C = ((CT__ti_sysbios_gates_GateHwi_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Module__startupDoneFxn ti_sysbios_gates_GateHwi_Module__startupDoneFxn__C = ((CT__ti_sysbios_gates_GateHwi_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Object__count ti_sysbios_gates_GateHwi_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Object__heap ti_sysbios_gates_GateHwi_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Object__sizeof ti_sysbios_gates_GateHwi_Object__sizeof__C = sizeof(ti_sysbios_gates_GateHwi_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_gates_GateHwi_Object__table ti_sysbios_gates_GateHwi_Object__table__C = ti_sysbios_gates_GateHwi_Object__table__V;


/*
 * ======== ti.sysbios.gates.GateMutex INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_gates_GateMutex_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_gates_GateMutex_Params ti_sysbios_gates_GateMutex_Object__PARAMS__C = {
    sizeof (ti_sysbios_gates_GateMutex_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_gates_GateMutex_Object__PARAMS__C.__iprms, /* instance */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_gates_GateMutex_Module__ ti_sysbios_gates_GateMutex_Module__root__V = {
    {&ti_sysbios_gates_GateMutex_Module__root__V.link,  /* link.next */
    &ti_sysbios_gates_GateMutex_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_gates_GateMutex_Object__ ti_sysbios_gates_GateMutex_Object__table__V[2] = {
    {/* instance#0 */
        &ti_sysbios_gates_GateMutex_Module__FXNS__C,
        0,  /* owner */
        {
            0,  /* event */
            (xdc_UInt)0x1,  /* eventId */
            ti_sysbios_knl_Semaphore_Mode_COUNTING,  /* mode */
            (xdc_UInt16)0x1,  /* count */
            {
                {
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_gates_GateMutex_Object__table__V[0].Object_field_sem.Object_field_pendQ.elem)),  /* next */
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_gates_GateMutex_Object__table__V[0].Object_field_sem.Object_field_pendQ.elem)),  /* prev */
                },  /* elem */
            },  /* Object_field_pendQ */
        },  /* Object_field_sem */
    },
    {/* instance#1 */
        &ti_sysbios_gates_GateMutex_Module__FXNS__C,
        0,  /* owner */
        {
            0,  /* event */
            (xdc_UInt)0x1,  /* eventId */
            ti_sysbios_knl_Semaphore_Mode_COUNTING,  /* mode */
            (xdc_UInt16)0x1,  /* count */
            {
                {
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_gates_GateMutex_Object__table__V[1].Object_field_sem.Object_field_pendQ.elem)),  /* next */
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_gates_GateMutex_Object__table__V[1].Object_field_sem.Object_field_pendQ.elem)),  /* prev */
                },  /* elem */
            },  /* Object_field_pendQ */
        },  /* Object_field_sem */
    },
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__diagsEnabled ti_sysbios_gates_GateMutex_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__diagsIncluded ti_sysbios_gates_GateMutex_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__diagsMask ti_sysbios_gates_GateMutex_Module__diagsMask__C = ((CT__ti_sysbios_gates_GateMutex_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__gateObj ti_sysbios_gates_GateMutex_Module__gateObj__C = ((CT__ti_sysbios_gates_GateMutex_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__gatePrms ti_sysbios_gates_GateMutex_Module__gatePrms__C = ((CT__ti_sysbios_gates_GateMutex_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__id ti_sysbios_gates_GateMutex_Module__id__C = (xdc_Bits16)0x8035;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerDefined ti_sysbios_gates_GateMutex_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerObj ti_sysbios_gates_GateMutex_Module__loggerObj__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerFxn0 ti_sysbios_gates_GateMutex_Module__loggerFxn0__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerFxn1 ti_sysbios_gates_GateMutex_Module__loggerFxn1__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerFxn2 ti_sysbios_gates_GateMutex_Module__loggerFxn2__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerFxn4 ti_sysbios_gates_GateMutex_Module__loggerFxn4__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__loggerFxn8 ti_sysbios_gates_GateMutex_Module__loggerFxn8__C = ((CT__ti_sysbios_gates_GateMutex_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Module__startupDoneFxn ti_sysbios_gates_GateMutex_Module__startupDoneFxn__C = ((CT__ti_sysbios_gates_GateMutex_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Object__count ti_sysbios_gates_GateMutex_Object__count__C = 2;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Object__heap ti_sysbios_gates_GateMutex_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Object__sizeof ti_sysbios_gates_GateMutex_Object__sizeof__C = sizeof(ti_sysbios_gates_GateMutex_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_Object__table ti_sysbios_gates_GateMutex_Object__table__C = ti_sysbios_gates_GateMutex_Object__table__V;

/* A_badContext__C */
__FAR__ const CT__ti_sysbios_gates_GateMutex_A_badContext ti_sysbios_gates_GateMutex_A_badContext__C = (((xdc_runtime_Assert_Id)2276) << 16 | 16);


/*
 * ======== ti.sysbios.hal.Cache INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__diagsEnabled ti_sysbios_hal_Cache_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__diagsIncluded ti_sysbios_hal_Cache_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__diagsMask ti_sysbios_hal_Cache_Module__diagsMask__C = ((CT__ti_sysbios_hal_Cache_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__gateObj ti_sysbios_hal_Cache_Module__gateObj__C = ((CT__ti_sysbios_hal_Cache_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__gatePrms ti_sysbios_hal_Cache_Module__gatePrms__C = ((CT__ti_sysbios_hal_Cache_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__id ti_sysbios_hal_Cache_Module__id__C = (xdc_Bits16)0x8016;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerDefined ti_sysbios_hal_Cache_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerObj ti_sysbios_hal_Cache_Module__loggerObj__C = ((CT__ti_sysbios_hal_Cache_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerFxn0 ti_sysbios_hal_Cache_Module__loggerFxn0__C = ((CT__ti_sysbios_hal_Cache_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerFxn1 ti_sysbios_hal_Cache_Module__loggerFxn1__C = ((CT__ti_sysbios_hal_Cache_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerFxn2 ti_sysbios_hal_Cache_Module__loggerFxn2__C = ((CT__ti_sysbios_hal_Cache_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerFxn4 ti_sysbios_hal_Cache_Module__loggerFxn4__C = ((CT__ti_sysbios_hal_Cache_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__loggerFxn8 ti_sysbios_hal_Cache_Module__loggerFxn8__C = ((CT__ti_sysbios_hal_Cache_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Module__startupDoneFxn ti_sysbios_hal_Cache_Module__startupDoneFxn__C = ((CT__ti_sysbios_hal_Cache_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Object__count ti_sysbios_hal_Cache_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Object__heap ti_sysbios_hal_Cache_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Object__sizeof ti_sysbios_hal_Cache_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_hal_Cache_Object__table ti_sysbios_hal_Cache_Object__table__C = 0;


/*
 * ======== ti.sysbios.hal.Cache_CacheProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.hal.Hwi INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_hal_Hwi_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_hal_Hwi_Params ti_sysbios_hal_Hwi_Object__PARAMS__C = {
    sizeof (ti_sysbios_hal_Hwi_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_hal_Hwi_Object__PARAMS__C.__iprms, /* instance */
    ti_sysbios_interfaces_IHwi_MaskingOption_LOWER,  /* maskSetting */
    ((xdc_UArg)(0x0)),  /* arg */
    1,  /* enableInt */
    (xdc_Int)(-0x0 - 1),  /* eventId */
    (xdc_Int)(-0x0 - 1),  /* priority */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_hal_Hwi_Module__ ti_sysbios_hal_Hwi_Module__root__V = {
    {&ti_sysbios_hal_Hwi_Module__root__V.link,  /* link.next */
    &ti_sysbios_hal_Hwi_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_hal_Hwi_Object__ ti_sysbios_hal_Hwi_Object__table__V[1] = {
    {/* instance#0 */
        0,
        (ti_sysbios_hal_Hwi_HwiProxy_Handle)&ti_sysbios_family_arm_m3_Hwi_Object__table__V[0],  /* pi */
    },
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__diagsEnabled ti_sysbios_hal_Hwi_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__diagsIncluded ti_sysbios_hal_Hwi_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__diagsMask ti_sysbios_hal_Hwi_Module__diagsMask__C = ((CT__ti_sysbios_hal_Hwi_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__gateObj ti_sysbios_hal_Hwi_Module__gateObj__C = ((CT__ti_sysbios_hal_Hwi_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__gatePrms ti_sysbios_hal_Hwi_Module__gatePrms__C = ((CT__ti_sysbios_hal_Hwi_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__id ti_sysbios_hal_Hwi_Module__id__C = (xdc_Bits16)0x8014;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerDefined ti_sysbios_hal_Hwi_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerObj ti_sysbios_hal_Hwi_Module__loggerObj__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerFxn0 ti_sysbios_hal_Hwi_Module__loggerFxn0__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerFxn1 ti_sysbios_hal_Hwi_Module__loggerFxn1__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerFxn2 ti_sysbios_hal_Hwi_Module__loggerFxn2__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerFxn4 ti_sysbios_hal_Hwi_Module__loggerFxn4__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__loggerFxn8 ti_sysbios_hal_Hwi_Module__loggerFxn8__C = ((CT__ti_sysbios_hal_Hwi_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Module__startupDoneFxn ti_sysbios_hal_Hwi_Module__startupDoneFxn__C = ((CT__ti_sysbios_hal_Hwi_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Object__count ti_sysbios_hal_Hwi_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Object__heap ti_sysbios_hal_Hwi_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Object__sizeof ti_sysbios_hal_Hwi_Object__sizeof__C = sizeof(ti_sysbios_hal_Hwi_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_Object__table ti_sysbios_hal_Hwi_Object__table__C = ti_sysbios_hal_Hwi_Object__table__V;

/* dispatcherAutoNestingSupport__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_dispatcherAutoNestingSupport ti_sysbios_hal_Hwi_dispatcherAutoNestingSupport__C = 1;

/* dispatcherSwiSupport__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_dispatcherSwiSupport ti_sysbios_hal_Hwi_dispatcherSwiSupport__C = 1;

/* dispatcherTaskSupport__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_dispatcherTaskSupport ti_sysbios_hal_Hwi_dispatcherTaskSupport__C = 1;

/* dispatcherIrpTrackingSupport__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_dispatcherIrpTrackingSupport ti_sysbios_hal_Hwi_dispatcherIrpTrackingSupport__C = 1;

/* E_stackOverflow__C */
__FAR__ const CT__ti_sysbios_hal_Hwi_E_stackOverflow ti_sysbios_hal_Hwi_E_stackOverflow__C = (((xdc_runtime_Error_Id)2729) << 16 | 0);


/*
 * ======== ti.sysbios.hal.Hwi_HwiProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.hal.Timer INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_hal_Timer_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_hal_Timer_Params ti_sysbios_hal_Timer_Object__PARAMS__C = {
    sizeof (ti_sysbios_hal_Timer_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_hal_Timer_Object__PARAMS__C.__iprms, /* instance */
    ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS,  /* runMode */
    ti_sysbios_interfaces_ITimer_StartMode_AUTO,  /* startMode */
    ((xdc_UArg)0),  /* arg */
    (xdc_UInt32)0x0,  /* period */
    ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS,  /* periodType */
    {
        (xdc_Bits32)0x0,  /* hi */
        (xdc_Bits32)0x0,  /* lo */
    },  /* extFreq */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_hal_Timer_Module__ ti_sysbios_hal_Timer_Module__root__V = {
    {&ti_sysbios_hal_Timer_Module__root__V.link,  /* link.next */
    &ti_sysbios_hal_Timer_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_hal_Timer_Object__ ti_sysbios_hal_Timer_Object__table__V[1] = {
    {/* instance#0 */
        0,
        (ti_sysbios_hal_Timer_TimerProxy_Handle)&ti_sysbios_family_arm_ducati_Timer_Object__table__V[0],  /* pi */
    },
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__diagsEnabled ti_sysbios_hal_Timer_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__diagsIncluded ti_sysbios_hal_Timer_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__diagsMask ti_sysbios_hal_Timer_Module__diagsMask__C = ((CT__ti_sysbios_hal_Timer_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__gateObj ti_sysbios_hal_Timer_Module__gateObj__C = ((CT__ti_sysbios_hal_Timer_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__gatePrms ti_sysbios_hal_Timer_Module__gatePrms__C = ((CT__ti_sysbios_hal_Timer_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__id ti_sysbios_hal_Timer_Module__id__C = (xdc_Bits16)0x8015;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerDefined ti_sysbios_hal_Timer_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerObj ti_sysbios_hal_Timer_Module__loggerObj__C = ((CT__ti_sysbios_hal_Timer_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerFxn0 ti_sysbios_hal_Timer_Module__loggerFxn0__C = ((CT__ti_sysbios_hal_Timer_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerFxn1 ti_sysbios_hal_Timer_Module__loggerFxn1__C = ((CT__ti_sysbios_hal_Timer_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerFxn2 ti_sysbios_hal_Timer_Module__loggerFxn2__C = ((CT__ti_sysbios_hal_Timer_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerFxn4 ti_sysbios_hal_Timer_Module__loggerFxn4__C = ((CT__ti_sysbios_hal_Timer_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__loggerFxn8 ti_sysbios_hal_Timer_Module__loggerFxn8__C = ((CT__ti_sysbios_hal_Timer_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Module__startupDoneFxn ti_sysbios_hal_Timer_Module__startupDoneFxn__C = ((CT__ti_sysbios_hal_Timer_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Object__count ti_sysbios_hal_Timer_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Object__heap ti_sysbios_hal_Timer_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Object__sizeof ti_sysbios_hal_Timer_Object__sizeof__C = sizeof(ti_sysbios_hal_Timer_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_hal_Timer_Object__table ti_sysbios_hal_Timer_Object__table__C = ti_sysbios_hal_Timer_Object__table__V;


/*
 * ======== ti.sysbios.hal.Timer_TimerProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.hal.ammu.AMMU INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__diagsEnabled ti_sysbios_hal_ammu_AMMU_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__diagsIncluded ti_sysbios_hal_ammu_AMMU_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__diagsMask ti_sysbios_hal_ammu_AMMU_Module__diagsMask__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__gateObj ti_sysbios_hal_ammu_AMMU_Module__gateObj__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__gatePrms ti_sysbios_hal_ammu_AMMU_Module__gatePrms__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__id ti_sysbios_hal_ammu_AMMU_Module__id__C = (xdc_Bits16)0x803d;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerDefined ti_sysbios_hal_ammu_AMMU_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerObj ti_sysbios_hal_ammu_AMMU_Module__loggerObj__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn0 ti_sysbios_hal_ammu_AMMU_Module__loggerFxn0__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn1 ti_sysbios_hal_ammu_AMMU_Module__loggerFxn1__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn2 ti_sysbios_hal_ammu_AMMU_Module__loggerFxn2__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn4 ti_sysbios_hal_ammu_AMMU_Module__loggerFxn4__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn8 ti_sysbios_hal_ammu_AMMU_Module__loggerFxn8__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Module__startupDoneFxn ti_sysbios_hal_ammu_AMMU_Module__startupDoneFxn__C = ((CT__ti_sysbios_hal_ammu_AMMU_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Object__count ti_sysbios_hal_ammu_AMMU_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Object__heap ti_sysbios_hal_ammu_AMMU_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Object__sizeof ti_sysbios_hal_ammu_AMMU_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_Object__table ti_sysbios_hal_ammu_AMMU_Object__table__C = 0;

/* configureAmmu__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_configureAmmu ti_sysbios_hal_ammu_AMMU_configureAmmu__C = 1;

/* numLargePages__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_numLargePages ti_sysbios_hal_ammu_AMMU_numLargePages__C = (xdc_UInt)0x4;

/* numMediumPages__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_numMediumPages ti_sysbios_hal_ammu_AMMU_numMediumPages__C = (xdc_UInt)0x2;

/* numSmallPages__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_numSmallPages ti_sysbios_hal_ammu_AMMU_numSmallPages__C = (xdc_UInt)0xa;

/* numLinePages__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_numLinePages ti_sysbios_hal_ammu_AMMU_numLinePages__C = (xdc_UInt)0x0;

/* mmuInitConfig__C */
__FAR__ const CT__ti_sysbios_hal_ammu_AMMU_mmuInitConfig ti_sysbios_hal_ammu_AMMU_mmuInitConfig__C = {
    {
        ((xdc_Char*)0),  /* [0] */
        ((xdc_Char*)0),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
    },  /* LARGE_ADDR */
    {
        ((xdc_Char*)((-0x0 - 1))),  /* [0] */
        ((xdc_Char*)((-0x0 - 1))),  /* [1] */
        ((xdc_Char*)((-0x0 - 1))),  /* [2] */
        ((xdc_Char*)((-0x0 - 1))),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
    },  /* LARGE_XLTE */
    {
        (xdc_UInt32)0x0,  /* [0] */
        (xdc_UInt32)0x0,  /* [1] */
        (xdc_UInt32)0x0,  /* [2] */
        (xdc_UInt32)0x0,  /* [3] */
        (xdc_UInt32)0x0,  /* [4] */
        (xdc_UInt32)0x0,  /* [5] */
        (xdc_UInt32)0x0,  /* [6] */
        (xdc_UInt32)0x0,  /* [7] */
    },  /* LARGE_POLICY */
    {
        ((xdc_Char*)0),  /* [0] */
        ((xdc_Char*)0),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
        ((xdc_Char*)0),  /* [8] */
        ((xdc_Char*)0),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
    },  /* MEDIUM_ADDR */
    {
        ((xdc_Char*)((void*)0x1)),  /* [0] */
        ((xdc_Char*)((void*)0x1)),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
        ((xdc_Char*)0),  /* [8] */
        ((xdc_Char*)0),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
    },  /* MEDIUM_XLTE */
    {
        (xdc_UInt32)0x0,  /* [0] */
        (xdc_UInt32)0x0,  /* [1] */
        (xdc_UInt32)0x0,  /* [2] */
        (xdc_UInt32)0x0,  /* [3] */
        (xdc_UInt32)0x0,  /* [4] */
        (xdc_UInt32)0x0,  /* [5] */
        (xdc_UInt32)0x0,  /* [6] */
        (xdc_UInt32)0x0,  /* [7] */
        (xdc_UInt32)0x0,  /* [8] */
        (xdc_UInt32)0x0,  /* [9] */
        (xdc_UInt32)0x0,  /* [10] */
        (xdc_UInt32)0x0,  /* [11] */
        (xdc_UInt32)0x0,  /* [12] */
        (xdc_UInt32)0x0,  /* [13] */
        (xdc_UInt32)0x0,  /* [14] */
        (xdc_UInt32)0x0,  /* [15] */
    },  /* MEDIUM_POLICY */
    {
        ((xdc_Char*)0),  /* [0] */
        ((xdc_Char*)((void*)0x40000000)),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
        ((xdc_Char*)0),  /* [8] */
        ((xdc_Char*)0),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
        ((xdc_Char*)0),  /* [16] */
        ((xdc_Char*)0),  /* [17] */
        ((xdc_Char*)0),  /* [18] */
        ((xdc_Char*)0),  /* [19] */
        ((xdc_Char*)0),  /* [20] */
        ((xdc_Char*)0),  /* [21] */
        ((xdc_Char*)0),  /* [22] */
        ((xdc_Char*)0),  /* [23] */
        ((xdc_Char*)0),  /* [24] */
        ((xdc_Char*)0),  /* [25] */
        ((xdc_Char*)0),  /* [26] */
        ((xdc_Char*)0),  /* [27] */
        ((xdc_Char*)0),  /* [28] */
        ((xdc_Char*)0),  /* [29] */
        ((xdc_Char*)0),  /* [30] */
        ((xdc_Char*)0),  /* [31] */
    },  /* SMALL_ADDR */
    {
        ((xdc_Char*)((void*)0x1)),  /* [0] */
        ((xdc_Char*)((void*)0x55080000)),  /* [1] */
        ((xdc_Char*)((void*)0x1)),  /* [2] */
        ((xdc_Char*)((void*)0x1)),  /* [3] */
        ((xdc_Char*)((void*)0x1)),  /* [4] */
        ((xdc_Char*)((void*)0x1)),  /* [5] */
        ((xdc_Char*)((void*)0x1)),  /* [6] */
        ((xdc_Char*)((void*)0x1)),  /* [7] */
        ((xdc_Char*)((void*)0x1)),  /* [8] */
        ((xdc_Char*)((void*)0x1)),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
        ((xdc_Char*)0),  /* [16] */
        ((xdc_Char*)0),  /* [17] */
        ((xdc_Char*)0),  /* [18] */
        ((xdc_Char*)0),  /* [19] */
        ((xdc_Char*)0),  /* [20] */
        ((xdc_Char*)0),  /* [21] */
        ((xdc_Char*)0),  /* [22] */
        ((xdc_Char*)0),  /* [23] */
        ((xdc_Char*)0),  /* [24] */
        ((xdc_Char*)0),  /* [25] */
        ((xdc_Char*)0),  /* [26] */
        ((xdc_Char*)0),  /* [27] */
        ((xdc_Char*)0),  /* [28] */
        ((xdc_Char*)0),  /* [29] */
        ((xdc_Char*)0),  /* [30] */
        ((xdc_Char*)0),  /* [31] */
    },  /* SMALL_XLTE */
    {
        (xdc_UInt32)0x0,  /* [0] */
        (xdc_UInt32)0x3,  /* [1] */
        (xdc_UInt32)0x0,  /* [2] */
        (xdc_UInt32)0x0,  /* [3] */
        (xdc_UInt32)0x0,  /* [4] */
        (xdc_UInt32)0x0,  /* [5] */
        (xdc_UInt32)0x0,  /* [6] */
        (xdc_UInt32)0x0,  /* [7] */
        (xdc_UInt32)0x0,  /* [8] */
        (xdc_UInt32)0x0,  /* [9] */
        (xdc_UInt32)0x0,  /* [10] */
        (xdc_UInt32)0x0,  /* [11] */
        (xdc_UInt32)0x0,  /* [12] */
        (xdc_UInt32)0x0,  /* [13] */
        (xdc_UInt32)0x0,  /* [14] */
        (xdc_UInt32)0x0,  /* [15] */
        (xdc_UInt32)0x0,  /* [16] */
        (xdc_UInt32)0x0,  /* [17] */
        (xdc_UInt32)0x0,  /* [18] */
        (xdc_UInt32)0x0,  /* [19] */
        (xdc_UInt32)0x0,  /* [20] */
        (xdc_UInt32)0x0,  /* [21] */
        (xdc_UInt32)0x0,  /* [22] */
        (xdc_UInt32)0x0,  /* [23] */
        (xdc_UInt32)0x0,  /* [24] */
        (xdc_UInt32)0x0,  /* [25] */
        (xdc_UInt32)0x0,  /* [26] */
        (xdc_UInt32)0x0,  /* [27] */
        (xdc_UInt32)0x0,  /* [28] */
        (xdc_UInt32)0x0,  /* [29] */
        (xdc_UInt32)0x0,  /* [30] */
        (xdc_UInt32)0x0,  /* [31] */
    },  /* SMALL_POLICY */
    {
        (xdc_UInt32)0x0,  /* [0] */
        (xdc_UInt32)0x0,  /* [1] */
        (xdc_UInt32)0x0,  /* [2] */
        (xdc_UInt32)0x0,  /* [3] */
        (xdc_UInt32)0x0,  /* [4] */
        (xdc_UInt32)0x0,  /* [5] */
        (xdc_UInt32)0x0,  /* [6] */
        (xdc_UInt32)0x0,  /* [7] */
        (xdc_UInt32)0x0,  /* [8] */
        (xdc_UInt32)0x0,  /* [9] */
        (xdc_UInt32)0x0,  /* [10] */
        (xdc_UInt32)0x0,  /* [11] */
        (xdc_UInt32)0x0,  /* [12] */
        (xdc_UInt32)0x0,  /* [13] */
        (xdc_UInt32)0x0,  /* [14] */
        (xdc_UInt32)0x0,  /* [15] */
        (xdc_UInt32)0x0,  /* [16] */
        (xdc_UInt32)0x0,  /* [17] */
        (xdc_UInt32)0x0,  /* [18] */
        (xdc_UInt32)0x0,  /* [19] */
        (xdc_UInt32)0x0,  /* [20] */
        (xdc_UInt32)0x0,  /* [21] */
        (xdc_UInt32)0x0,  /* [22] */
        (xdc_UInt32)0x0,  /* [23] */
        (xdc_UInt32)0x0,  /* [24] */
        (xdc_UInt32)0x0,  /* [25] */
        (xdc_UInt32)0x0,  /* [26] */
        (xdc_UInt32)0x0,  /* [27] */
        (xdc_UInt32)0x0,  /* [28] */
        (xdc_UInt32)0x0,  /* [29] */
        (xdc_UInt32)0x0,  /* [30] */
        (xdc_UInt32)0x0,  /* [31] */
    },  /* SMALL_MAINT */
    {
        ((xdc_Char*)0),  /* [0] */
        ((xdc_Char*)0),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
        ((xdc_Char*)0),  /* [8] */
        ((xdc_Char*)0),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
        ((xdc_Char*)0),  /* [16] */
        ((xdc_Char*)0),  /* [17] */
        ((xdc_Char*)0),  /* [18] */
        ((xdc_Char*)0),  /* [19] */
        ((xdc_Char*)0),  /* [20] */
        ((xdc_Char*)0),  /* [21] */
        ((xdc_Char*)0),  /* [22] */
        ((xdc_Char*)0),  /* [23] */
        ((xdc_Char*)0),  /* [24] */
        ((xdc_Char*)0),  /* [25] */
        ((xdc_Char*)0),  /* [26] */
        ((xdc_Char*)0),  /* [27] */
        ((xdc_Char*)0),  /* [28] */
        ((xdc_Char*)0),  /* [29] */
        ((xdc_Char*)0),  /* [30] */
        ((xdc_Char*)0),  /* [31] */
    },  /* LINE_ADDR */
    {
        ((xdc_Char*)0),  /* [0] */
        ((xdc_Char*)0),  /* [1] */
        ((xdc_Char*)0),  /* [2] */
        ((xdc_Char*)0),  /* [3] */
        ((xdc_Char*)0),  /* [4] */
        ((xdc_Char*)0),  /* [5] */
        ((xdc_Char*)0),  /* [6] */
        ((xdc_Char*)0),  /* [7] */
        ((xdc_Char*)0),  /* [8] */
        ((xdc_Char*)0),  /* [9] */
        ((xdc_Char*)0),  /* [10] */
        ((xdc_Char*)0),  /* [11] */
        ((xdc_Char*)0),  /* [12] */
        ((xdc_Char*)0),  /* [13] */
        ((xdc_Char*)0),  /* [14] */
        ((xdc_Char*)0),  /* [15] */
        ((xdc_Char*)0),  /* [16] */
        ((xdc_Char*)0),  /* [17] */
        ((xdc_Char*)0),  /* [18] */
        ((xdc_Char*)0),  /* [19] */
        ((xdc_Char*)0),  /* [20] */
        ((xdc_Char*)0),  /* [21] */
        ((xdc_Char*)0),  /* [22] */
        ((xdc_Char*)0),  /* [23] */
        ((xdc_Char*)0),  /* [24] */
        ((xdc_Char*)0),  /* [25] */
        ((xdc_Char*)0),  /* [26] */
        ((xdc_Char*)0),  /* [27] */
        ((xdc_Char*)0),  /* [28] */
        ((xdc_Char*)0),  /* [29] */
        ((xdc_Char*)0),  /* [30] */
        ((xdc_Char*)0),  /* [31] */
    },  /* LINE_XLTE */
    {
        (xdc_UInt32)0x0,  /* [0] */
        (xdc_UInt32)0x0,  /* [1] */
        (xdc_UInt32)0x0,  /* [2] */
        (xdc_UInt32)0x0,  /* [3] */
        (xdc_UInt32)0x0,  /* [4] */
        (xdc_UInt32)0x0,  /* [5] */
        (xdc_UInt32)0x0,  /* [6] */
        (xdc_UInt32)0x0,  /* [7] */
        (xdc_UInt32)0x0,  /* [8] */
        (xdc_UInt32)0x0,  /* [9] */
        (xdc_UInt32)0x0,  /* [10] */
        (xdc_UInt32)0x0,  /* [11] */
        (xdc_UInt32)0x0,  /* [12] */
        (xdc_UInt32)0x0,  /* [13] */
        (xdc_UInt32)0x0,  /* [14] */
        (xdc_UInt32)0x0,  /* [15] */
        (xdc_UInt32)0x0,  /* [16] */
        (xdc_UInt32)0x0,  /* [17] */
        (xdc_UInt32)0x0,  /* [18] */
        (xdc_UInt32)0x0,  /* [19] */
        (xdc_UInt32)0x0,  /* [20] */
        (xdc_UInt32)0x0,  /* [21] */
        (xdc_UInt32)0x0,  /* [22] */
        (xdc_UInt32)0x0,  /* [23] */
        (xdc_UInt32)0x0,  /* [24] */
        (xdc_UInt32)0x0,  /* [25] */
        (xdc_UInt32)0x0,  /* [26] */
        (xdc_UInt32)0x0,  /* [27] */
        (xdc_UInt32)0x0,  /* [28] */
        (xdc_UInt32)0x0,  /* [29] */
        (xdc_UInt32)0x0,  /* [30] */
        (xdc_UInt32)0x0,  /* [31] */
    },  /* LINE_POLICY */
    ((xdc_Char*)((void*)0x1)),  /* DEBUG_XLTE */
    (xdc_UInt32)0x0,  /* DEBUG_POLICY */
    (xdc_UInt32)0x0,  /* MAINT */
    ((xdc_Char*)0),  /* MSTART */
    ((xdc_Char*)0),  /* MEND */
    (xdc_UInt32)0x0,  /* MSTAT */
};


/*
 * ======== ti.sysbios.hal.unicache.Cache INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsEnabled ti_sysbios_hal_unicache_Cache_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsIncluded ti_sysbios_hal_unicache_Cache_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__diagsMask ti_sysbios_hal_unicache_Cache_Module__diagsMask__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__gateObj ti_sysbios_hal_unicache_Cache_Module__gateObj__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__gateObj)((void*)(xdc_runtime_IGateProvider_Handle)&ti_sysbios_family_arm_ducati_GateDualCore_Object__table__V[0]));

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__gatePrms ti_sysbios_hal_unicache_Cache_Module__gatePrms__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__id ti_sysbios_hal_unicache_Cache_Module__id__C = (xdc_Bits16)0x803b;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerDefined ti_sysbios_hal_unicache_Cache_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerObj ti_sysbios_hal_unicache_Cache_Module__loggerObj__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn0 ti_sysbios_hal_unicache_Cache_Module__loggerFxn0__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn1 ti_sysbios_hal_unicache_Cache_Module__loggerFxn1__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn2 ti_sysbios_hal_unicache_Cache_Module__loggerFxn2__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn4 ti_sysbios_hal_unicache_Cache_Module__loggerFxn4__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn8 ti_sysbios_hal_unicache_Cache_Module__loggerFxn8__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn__C = ((CT__ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__count ti_sysbios_hal_unicache_Cache_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__heap ti_sysbios_hal_unicache_Cache_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__sizeof ti_sysbios_hal_unicache_Cache_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_Object__table ti_sysbios_hal_unicache_Cache_Object__table__C = 0;

/* E_exception__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_E_exception ti_sysbios_hal_unicache_Cache_E_exception__C = (((xdc_runtime_Error_Id)3760) << 16 | 0);

/* configureCache__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_configureCache ti_sysbios_hal_unicache_Cache_configureCache__C = 0;

/* enableCache__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_enableCache ti_sysbios_hal_unicache_Cache_enableCache__C = 0;

/* maxLineModeBufSize__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_maxLineModeBufSize ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__C = (xdc_SizeT)0x400;

/* l1ocpConfig__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_l1ocpConfig ti_sysbios_hal_unicache_Cache_l1ocpConfig__C = (xdc_Bits32)0x0;

/* l1secConfig__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_l1secConfig ti_sysbios_hal_unicache_Cache_l1secConfig__C = (xdc_Bits32)0x1c;

/* l2ocpConfig__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2ocpConfig ti_sysbios_hal_unicache_Cache_l2ocpConfig__C = (xdc_Bits32)0x0;

/* l2secConfig__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2secConfig ti_sysbios_hal_unicache_Cache_l2secConfig__C = (xdc_Bits32)0x1c;

/* l2CacheSupported__C */
__FAR__ const CT__ti_sysbios_hal_unicache_Cache_l2CacheSupported ti_sysbios_hal_unicache_Cache_l2CacheSupported__C = 0;


/*
 * ======== ti.sysbios.hal.unicache.Cache_Module_GateProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.heaps.HeapBuf INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_heaps_HeapBuf_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_heaps_HeapBuf_Params ti_sysbios_heaps_HeapBuf_Object__PARAMS__C = {
    sizeof (ti_sysbios_heaps_HeapBuf_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_heaps_HeapBuf_Object__PARAMS__C.__iprms, /* instance */
    (xdc_SizeT)0x0,  /* align */
    (xdc_UInt)0x0,  /* numBlocks */
    (xdc_SizeT)0x0,  /* blockSize */
    ((xdc_UArg)(0x0)),  /* bufSize */
    ((xdc_Ptr)(0x0)),  /* buf */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_heaps_HeapBuf_Module__ ti_sysbios_heaps_HeapBuf_Module__root__V = {
    {&ti_sysbios_heaps_HeapBuf_Module__root__V.link,  /* link.next */
    &ti_sysbios_heaps_HeapBuf_Module__root__V.link},  /* link.prev */
};

/* Module__state__V */
ti_sysbios_heaps_HeapBuf_Module_State__ ti_sysbios_heaps_HeapBuf_Module__state__V = {
    ((void*)0),  /* constructedHeaps */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsEnabled ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsIncluded ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask ti_sysbios_heaps_HeapBuf_Module__diagsMask__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__gateObj ti_sysbios_heaps_HeapBuf_Module__gateObj__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__gatePrms ti_sysbios_heaps_HeapBuf_Module__gatePrms__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__id ti_sysbios_heaps_HeapBuf_Module__id__C = (xdc_Bits16)0x802e;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerDefined ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerObj ti_sysbios_heaps_HeapBuf_Module__loggerObj__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn0 ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn1 ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn2 ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn4 ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn8 ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Module__startupDoneFxn ti_sysbios_heaps_HeapBuf_Module__startupDoneFxn__C = ((CT__ti_sysbios_heaps_HeapBuf_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__count ti_sysbios_heaps_HeapBuf_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__heap ti_sysbios_heaps_HeapBuf_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__sizeof ti_sysbios_heaps_HeapBuf_Object__sizeof__C = sizeof(ti_sysbios_heaps_HeapBuf_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_Object__table ti_sysbios_heaps_HeapBuf_Object__table__C = 0;

/* A_nullBuf__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_nullBuf ti_sysbios_heaps_HeapBuf_A_nullBuf__C = (((xdc_runtime_Assert_Id)1401) << 16 | 16);

/* A_bufAlign__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_bufAlign ti_sysbios_heaps_HeapBuf_A_bufAlign__C = (((xdc_runtime_Assert_Id)1430) << 16 | 16);

/* A_invalidAlign__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidAlign ti_sysbios_heaps_HeapBuf_A_invalidAlign__C = (((xdc_runtime_Assert_Id)1455) << 16 | 16);

/* A_zeroBlockSize__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_zeroBlockSize ti_sysbios_heaps_HeapBuf_A_zeroBlockSize__C = (((xdc_runtime_Assert_Id)1545) << 16 | 16);

/* A_zeroBlocks__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_zeroBlocks ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C = (((xdc_runtime_Assert_Id)1570) << 16 | 16);

/* A_zeroBufSize__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_zeroBufSize ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C = (((xdc_runtime_Assert_Id)1595) << 16 | 16);

/* A_invalidBufSize__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidBufSize ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C = (((xdc_runtime_Assert_Id)1618) << 16 | 16);

/* A_noBlocksToFree__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_noBlocksToFree ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C = (((xdc_runtime_Assert_Id)1676) << 16 | 16);

/* A_invalidFree__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_A_invalidFree ti_sysbios_heaps_HeapBuf_A_invalidFree__C = (((xdc_runtime_Assert_Id)1736) << 16 | 16);

/* E_size__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_E_size ti_sysbios_heaps_HeapBuf_E_size__C = (((xdc_runtime_Error_Id)3499) << 16 | 0);

/* trackMaxAllocs__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_trackMaxAllocs ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C = 0;

/* numConstructedHeaps__C */
__FAR__ const CT__ti_sysbios_heaps_HeapBuf_numConstructedHeaps ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C = (xdc_UInt)0x0;


/*
 * ======== ti.sysbios.heaps.HeapMem INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_heaps_HeapMem_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_heaps_HeapMem_Params ti_sysbios_heaps_HeapMem_Object__PARAMS__C = {
    sizeof (ti_sysbios_heaps_HeapMem_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_heaps_HeapMem_Object__PARAMS__C.__iprms, /* instance */
    ((xdc_Ptr)(0x0)),  /* buf */
    ((xdc_UArg)(0x0)),  /* size */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* --> ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A */
__T1_ti_sysbios_heaps_HeapMem_Instance_State__buf ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A[86016];

/* Module__root__V */
ti_sysbios_heaps_HeapMem_Module__ ti_sysbios_heaps_HeapMem_Module__root__V = {
    {&ti_sysbios_heaps_HeapMem_Module__root__V.link,  /* link.next */
    &ti_sysbios_heaps_HeapMem_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_heaps_HeapMem_Object__ ti_sysbios_heaps_HeapMem_Object__table__V[1] = {
    {/* instance#0 */
        &ti_sysbios_heaps_HeapMem_Module__FXNS__C,
        ((xdc_UArg)(0x8)),  /* align */
        ((void*)ti_sysbios_heaps_HeapMem_Instance_State_0_buf__A),  /* buf */
        {
            ((ti_sysbios_heaps_HeapMem_Header*)0),  /* next */
            ((xdc_UArg)(0x15000)),  /* size */
        },  /* head */
    },
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__diagsEnabled ti_sysbios_heaps_HeapMem_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__diagsIncluded ti_sysbios_heaps_HeapMem_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__diagsMask ti_sysbios_heaps_HeapMem_Module__diagsMask__C = ((CT__ti_sysbios_heaps_HeapMem_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__gateObj ti_sysbios_heaps_HeapMem_Module__gateObj__C = ((CT__ti_sysbios_heaps_HeapMem_Module__gateObj)((void*)(xdc_runtime_IGateProvider_Handle)&ti_sysbios_gates_GateMutex_Object__table__V[0]));

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__gatePrms ti_sysbios_heaps_HeapMem_Module__gatePrms__C = ((CT__ti_sysbios_heaps_HeapMem_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__id ti_sysbios_heaps_HeapMem_Module__id__C = (xdc_Bits16)0x802f;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerDefined ti_sysbios_heaps_HeapMem_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerObj ti_sysbios_heaps_HeapMem_Module__loggerObj__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn0 ti_sysbios_heaps_HeapMem_Module__loggerFxn0__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn1 ti_sysbios_heaps_HeapMem_Module__loggerFxn1__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn2 ti_sysbios_heaps_HeapMem_Module__loggerFxn2__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn4 ti_sysbios_heaps_HeapMem_Module__loggerFxn4__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn8 ti_sysbios_heaps_HeapMem_Module__loggerFxn8__C = ((CT__ti_sysbios_heaps_HeapMem_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Module__startupDoneFxn ti_sysbios_heaps_HeapMem_Module__startupDoneFxn__C = ((CT__ti_sysbios_heaps_HeapMem_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Object__count ti_sysbios_heaps_HeapMem_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Object__heap ti_sysbios_heaps_HeapMem_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Object__sizeof ti_sysbios_heaps_HeapMem_Object__sizeof__C = sizeof(ti_sysbios_heaps_HeapMem_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_Object__table ti_sysbios_heaps_HeapMem_Object__table__C = ti_sysbios_heaps_HeapMem_Object__table__V;

/* A_zeroBlock__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_A_zeroBlock ti_sysbios_heaps_HeapMem_A_zeroBlock__C = (((xdc_runtime_Assert_Id)1764) << 16 | 16);

/* A_heapSize__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_A_heapSize ti_sysbios_heaps_HeapMem_A_heapSize__C = (((xdc_runtime_Assert_Id)1800) << 16 | 16);

/* A_align__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_A_align ti_sysbios_heaps_HeapMem_A_align__C = (((xdc_runtime_Assert_Id)1845) << 16 | 16);

/* E_memory__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_E_memory ti_sysbios_heaps_HeapMem_E_memory__C = (((xdc_runtime_Error_Id)3547) << 16 | 0);

/* A_invalidFree__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_A_invalidFree ti_sysbios_heaps_HeapMem_A_invalidFree__C = (((xdc_runtime_Assert_Id)1736) << 16 | 16);

/* reqAlign__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_reqAlign ti_sysbios_heaps_HeapMem_reqAlign__C = (xdc_Int)0x8;

/* reqAlignMask__C */
__FAR__ const CT__ti_sysbios_heaps_HeapMem_reqAlignMask ti_sysbios_heaps_HeapMem_reqAlignMask__C = (xdc_Int)0x7;


/*
 * ======== ti.sysbios.heaps.HeapMem_Module_GateProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.knl.Clock INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Clock_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_knl_Clock_Params ti_sysbios_knl_Clock_Object__PARAMS__C = {
    sizeof (ti_sysbios_knl_Clock_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_knl_Clock_Object__PARAMS__C.__iprms, /* instance */
    0,  /* startFlag */
    (xdc_UInt)0x0,  /* period */
    ((xdc_UArg)0),  /* arg */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_knl_Clock_Module__ ti_sysbios_knl_Clock_Module__root__V = {
    {&ti_sysbios_knl_Clock_Module__root__V.link,  /* link.next */
    &ti_sysbios_knl_Clock_Module__root__V.link},  /* link.prev */
};

/* Module__state__V */
ti_sysbios_knl_Clock_Module_State__ ti_sysbios_knl_Clock_Module__state__V = {
    (xdc_UInt32)0x0,  /* ticks */
    (xdc_UInt)0x0,  /* swiCount */
    (ti_sysbios_hal_Timer_Handle)&ti_sysbios_hal_Timer_Object__table__V[0],  /* timer */
    ((xdc_Void(*)(xdc_UArg))((xdc_Fxn)ti_sysbios_knl_Clock_doTick__I)),  /* doTickFunc */
    (ti_sysbios_knl_Swi_Handle)&ti_sysbios_knl_Swi_Object__table__V[0],  /* swi */
    (xdc_UInt32)0x0,  /* periodCounts */
    (xdc_UInt)0x0,  /* numTickSkip */
    (xdc_UInt)0x0,  /* skipsWorkFunc */
    0,  /* inWorkFunc */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Clock_Module__state__V.Object_field_clockQ.elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Clock_Module__state__V.Object_field_clockQ.elem)),  /* prev */
        },  /* elem */
    },  /* Object_field_clockQ */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__diagsEnabled ti_sysbios_knl_Clock_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__diagsIncluded ti_sysbios_knl_Clock_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__diagsMask ti_sysbios_knl_Clock_Module__diagsMask__C = ((CT__ti_sysbios_knl_Clock_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__gateObj ti_sysbios_knl_Clock_Module__gateObj__C = ((CT__ti_sysbios_knl_Clock_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__gatePrms ti_sysbios_knl_Clock_Module__gatePrms__C = ((CT__ti_sysbios_knl_Clock_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__id ti_sysbios_knl_Clock_Module__id__C = (xdc_Bits16)0x8018;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerDefined ti_sysbios_knl_Clock_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerObj ti_sysbios_knl_Clock_Module__loggerObj__C = ((CT__ti_sysbios_knl_Clock_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerFxn0 ti_sysbios_knl_Clock_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Clock_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerFxn1 ti_sysbios_knl_Clock_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Clock_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerFxn2 ti_sysbios_knl_Clock_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Clock_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerFxn4 ti_sysbios_knl_Clock_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Clock_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__loggerFxn8 ti_sysbios_knl_Clock_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Clock_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Module__startupDoneFxn ti_sysbios_knl_Clock_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Clock_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Object__count ti_sysbios_knl_Clock_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Object__heap ti_sysbios_knl_Clock_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Object__sizeof ti_sysbios_knl_Clock_Object__sizeof__C = sizeof(ti_sysbios_knl_Clock_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Clock_Object__table ti_sysbios_knl_Clock_Object__table__C = 0;

/* LW_delayed__C */
__FAR__ const CT__ti_sysbios_knl_Clock_LW_delayed ti_sysbios_knl_Clock_LW_delayed__C = (((xdc_runtime_Log_Event)3966) << 16 | 1024);

/* LM_tick__C */
__FAR__ const CT__ti_sysbios_knl_Clock_LM_tick ti_sysbios_knl_Clock_LM_tick__C = (((xdc_runtime_Log_Event)3988) << 16 | 768);

/* LM_begin__C */
__FAR__ const CT__ti_sysbios_knl_Clock_LM_begin ti_sysbios_knl_Clock_LM_begin__C = (((xdc_runtime_Log_Event)4006) << 16 | 768);

/* A_clockDisabled__C */
__FAR__ const CT__ti_sysbios_knl_Clock_A_clockDisabled ti_sysbios_knl_Clock_A_clockDisabled__C = (((xdc_runtime_Assert_Id)355) << 16 | 16);

/* A_badThreadType__C */
__FAR__ const CT__ti_sysbios_knl_Clock_A_badThreadType ti_sysbios_knl_Clock_A_badThreadType__C = (((xdc_runtime_Assert_Id)436) << 16 | 16);

/* tickSource__C */
__FAR__ const CT__ti_sysbios_knl_Clock_tickSource ti_sysbios_knl_Clock_tickSource__C = ti_sysbios_knl_Clock_TickSource_TIMER;

/* tickMode__C */
__FAR__ const CT__ti_sysbios_knl_Clock_tickMode ti_sysbios_knl_Clock_tickMode__C = ti_sysbios_knl_Clock_TickMode_PERIODIC;

/* timerId__C */
__FAR__ const CT__ti_sysbios_knl_Clock_timerId ti_sysbios_knl_Clock_timerId__C = (xdc_UInt)(-0x0 - 1);

/* tickPeriod__C */
__FAR__ const CT__ti_sysbios_knl_Clock_tickPeriod ti_sysbios_knl_Clock_tickPeriod__C = (xdc_UInt32)0x3e8;


/*
 * ======== ti.sysbios.knl.Idle INITIALIZERS ========
 */

/* --> ti_sysbios_knl_Idle_funcList__A */
const __T1_ti_sysbios_knl_Idle_funcList ti_sysbios_knl_Idle_funcList__A[2] = {
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_hal_Hwi_checkStack)),  /* [0] */
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_utils_Load_idleFxn__E)),  /* [1] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsEnabled ti_sysbios_knl_Idle_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsIncluded ti_sysbios_knl_Idle_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__diagsMask ti_sysbios_knl_Idle_Module__diagsMask__C = ((CT__ti_sysbios_knl_Idle_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__gateObj ti_sysbios_knl_Idle_Module__gateObj__C = ((CT__ti_sysbios_knl_Idle_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__gatePrms ti_sysbios_knl_Idle_Module__gatePrms__C = ((CT__ti_sysbios_knl_Idle_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__id ti_sysbios_knl_Idle_Module__id__C = (xdc_Bits16)0x8019;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerDefined ti_sysbios_knl_Idle_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerObj ti_sysbios_knl_Idle_Module__loggerObj__C = ((CT__ti_sysbios_knl_Idle_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn0 ti_sysbios_knl_Idle_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Idle_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn1 ti_sysbios_knl_Idle_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Idle_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn2 ti_sysbios_knl_Idle_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Idle_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn4 ti_sysbios_knl_Idle_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Idle_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__loggerFxn8 ti_sysbios_knl_Idle_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Idle_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Module__startupDoneFxn ti_sysbios_knl_Idle_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Idle_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Object__count ti_sysbios_knl_Idle_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Object__heap ti_sysbios_knl_Idle_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Object__sizeof ti_sysbios_knl_Idle_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Idle_Object__table ti_sysbios_knl_Idle_Object__table__C = 0;

/* funcList__C */
__FAR__ const CT__ti_sysbios_knl_Idle_funcList ti_sysbios_knl_Idle_funcList__C = {2, ((__T1_ti_sysbios_knl_Idle_funcList*)ti_sysbios_knl_Idle_funcList__A)};


/*
 * ======== ti.sysbios.knl.Intrinsics INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__diagsEnabled ti_sysbios_knl_Intrinsics_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__diagsIncluded ti_sysbios_knl_Intrinsics_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__diagsMask ti_sysbios_knl_Intrinsics_Module__diagsMask__C = ((CT__ti_sysbios_knl_Intrinsics_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__gateObj ti_sysbios_knl_Intrinsics_Module__gateObj__C = ((CT__ti_sysbios_knl_Intrinsics_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__gatePrms ti_sysbios_knl_Intrinsics_Module__gatePrms__C = ((CT__ti_sysbios_knl_Intrinsics_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__id ti_sysbios_knl_Intrinsics_Module__id__C = (xdc_Bits16)0x801a;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerDefined ti_sysbios_knl_Intrinsics_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerObj ti_sysbios_knl_Intrinsics_Module__loggerObj__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn0 ti_sysbios_knl_Intrinsics_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn1 ti_sysbios_knl_Intrinsics_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn2 ti_sysbios_knl_Intrinsics_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn4 ti_sysbios_knl_Intrinsics_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn8 ti_sysbios_knl_Intrinsics_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Intrinsics_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Module__startupDoneFxn ti_sysbios_knl_Intrinsics_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Intrinsics_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Object__count ti_sysbios_knl_Intrinsics_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Object__heap ti_sysbios_knl_Intrinsics_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Object__sizeof ti_sysbios_knl_Intrinsics_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Intrinsics_Object__table ti_sysbios_knl_Intrinsics_Object__table__C = 0;


/*
 * ======== ti.sysbios.knl.Intrinsics_SupportProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.knl.Queue INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Queue_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_knl_Queue_Params ti_sysbios_knl_Queue_Object__PARAMS__C = {
    sizeof (ti_sysbios_knl_Queue_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_knl_Queue_Object__PARAMS__C.__iprms, /* instance */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_knl_Queue_Module__ ti_sysbios_knl_Queue_Module__root__V = {
    {&ti_sysbios_knl_Queue_Module__root__V.link,  /* link.next */
    &ti_sysbios_knl_Queue_Module__root__V.link},  /* link.prev */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__diagsEnabled ti_sysbios_knl_Queue_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__diagsIncluded ti_sysbios_knl_Queue_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__diagsMask ti_sysbios_knl_Queue_Module__diagsMask__C = ((CT__ti_sysbios_knl_Queue_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__gateObj ti_sysbios_knl_Queue_Module__gateObj__C = ((CT__ti_sysbios_knl_Queue_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__gatePrms ti_sysbios_knl_Queue_Module__gatePrms__C = ((CT__ti_sysbios_knl_Queue_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__id ti_sysbios_knl_Queue_Module__id__C = (xdc_Bits16)0x801b;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerDefined ti_sysbios_knl_Queue_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerObj ti_sysbios_knl_Queue_Module__loggerObj__C = ((CT__ti_sysbios_knl_Queue_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerFxn0 ti_sysbios_knl_Queue_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Queue_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerFxn1 ti_sysbios_knl_Queue_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Queue_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerFxn2 ti_sysbios_knl_Queue_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Queue_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerFxn4 ti_sysbios_knl_Queue_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Queue_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__loggerFxn8 ti_sysbios_knl_Queue_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Queue_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Module__startupDoneFxn ti_sysbios_knl_Queue_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Queue_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Object__count ti_sysbios_knl_Queue_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Object__heap ti_sysbios_knl_Queue_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Object__sizeof ti_sysbios_knl_Queue_Object__sizeof__C = sizeof(ti_sysbios_knl_Queue_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Queue_Object__table ti_sysbios_knl_Queue_Object__table__C = 0;


/*
 * ======== ti.sysbios.knl.Semaphore INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Semaphore_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_knl_Semaphore_Params ti_sysbios_knl_Semaphore_Object__PARAMS__C = {
    sizeof (ti_sysbios_knl_Semaphore_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_knl_Semaphore_Object__PARAMS__C.__iprms, /* instance */
    0,  /* event */
    (xdc_UInt)0x1,  /* eventId */
    ti_sysbios_knl_Semaphore_Mode_COUNTING,  /* mode */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_knl_Semaphore_Module__ ti_sysbios_knl_Semaphore_Module__root__V = {
    {&ti_sysbios_knl_Semaphore_Module__root__V.link,  /* link.next */
    &ti_sysbios_knl_Semaphore_Module__root__V.link},  /* link.prev */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__diagsEnabled ti_sysbios_knl_Semaphore_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__diagsIncluded ti_sysbios_knl_Semaphore_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__diagsMask ti_sysbios_knl_Semaphore_Module__diagsMask__C = ((CT__ti_sysbios_knl_Semaphore_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__gateObj ti_sysbios_knl_Semaphore_Module__gateObj__C = ((CT__ti_sysbios_knl_Semaphore_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__gatePrms ti_sysbios_knl_Semaphore_Module__gatePrms__C = ((CT__ti_sysbios_knl_Semaphore_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__id ti_sysbios_knl_Semaphore_Module__id__C = (xdc_Bits16)0x801c;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerDefined ti_sysbios_knl_Semaphore_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerObj ti_sysbios_knl_Semaphore_Module__loggerObj__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerFxn0 ti_sysbios_knl_Semaphore_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerFxn1 ti_sysbios_knl_Semaphore_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerFxn2 ti_sysbios_knl_Semaphore_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerFxn4 ti_sysbios_knl_Semaphore_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__loggerFxn8 ti_sysbios_knl_Semaphore_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Semaphore_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Module__startupDoneFxn ti_sysbios_knl_Semaphore_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Semaphore_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Object__count ti_sysbios_knl_Semaphore_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Object__heap ti_sysbios_knl_Semaphore_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Object__sizeof ti_sysbios_knl_Semaphore_Object__sizeof__C = sizeof(ti_sysbios_knl_Semaphore_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_Object__table ti_sysbios_knl_Semaphore_Object__table__C = 0;

/* LM_post__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_LM_post ti_sysbios_knl_Semaphore_LM_post__C = (((xdc_runtime_Log_Event)4038) << 16 | 768);

/* LM_pend__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_LM_pend ti_sysbios_knl_Semaphore_LM_pend__C = (((xdc_runtime_Log_Event)4068) << 16 | 768);

/* A_noEvents__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_A_noEvents ti_sysbios_knl_Semaphore_A_noEvents__C = (((xdc_runtime_Assert_Id)756) << 16 | 16);

/* A_invTimeout__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_A_invTimeout ti_sysbios_knl_Semaphore_A_invTimeout__C = (((xdc_runtime_Assert_Id)811) << 16 | 16);

/* A_badContext__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_A_badContext ti_sysbios_knl_Semaphore_A_badContext__C = (((xdc_runtime_Assert_Id)635) << 16 | 16);

/* supportsEvents__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_supportsEvents ti_sysbios_knl_Semaphore_supportsEvents__C = 0;

/* eventPost__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_eventPost ti_sysbios_knl_Semaphore_eventPost__C = ((CT__ti_sysbios_knl_Semaphore_eventPost)0);

/* eventSync__C */
__FAR__ const CT__ti_sysbios_knl_Semaphore_eventSync ti_sysbios_knl_Semaphore_eventSync__C = ((CT__ti_sysbios_knl_Semaphore_eventSync)0);


/*
 * ======== ti.sysbios.knl.Swi INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Swi_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_knl_Swi_Params ti_sysbios_knl_Swi_Object__PARAMS__C = {
    sizeof (ti_sysbios_knl_Swi_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_knl_Swi_Object__PARAMS__C.__iprms, /* instance */
    ((xdc_UArg)(0x0)),  /* arg0 */
    ((xdc_UArg)(0x0)),  /* arg1 */
    (xdc_UInt)(-0x0 - 1),  /* priority */
    (xdc_UInt)0x0,  /* trigger */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* --> ti_sysbios_knl_Swi_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_knl_Swi_Instance_State__hookEnv ti_sysbios_knl_Swi_Instance_State_0_hookEnv__A[1];

/* Module__root__V */
ti_sysbios_knl_Swi_Module__ ti_sysbios_knl_Swi_Module__root__V = {
    {&ti_sysbios_knl_Swi_Module__root__V.link,  /* link.next */
    &ti_sysbios_knl_Swi_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_knl_Swi_Object__ ti_sysbios_knl_Swi_Object__table__V[1] = {
    {/* instance#0 */
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Object__table__V[0].qElem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Object__table__V[0].qElem)),  /* prev */
        },  /* qElem */
        ((xdc_Void(*)(xdc_UArg,xdc_UArg))((xdc_Fxn)ti_sysbios_knl_Clock_workFunc__E)),  /* fxn */
        ((xdc_UArg)(0x0)),  /* arg0 */
        ((xdc_UArg)(0x0)),  /* arg1 */
        (xdc_UInt)0xf,  /* priority */
        (xdc_UInt)0x8000,  /* mask */
        0,  /* posted */
        (xdc_UInt)0x0,  /* initTrigger */
        (xdc_UInt)0x0,  /* trigger */
        (ti_sysbios_knl_Queue_Handle)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[15],  /* readyQ */
        ((void*)ti_sysbios_knl_Swi_Instance_State_0_hookEnv__A),  /* hookEnv */
    },
};

/* --> ti_sysbios_knl_Swi_Module_State_0_readyQ__A */
__T1_ti_sysbios_knl_Swi_Module_State__readyQ ti_sysbios_knl_Swi_Module_State_0_readyQ__A[16] = {
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[0].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[0].elem)),  /* prev */
        },  /* elem */
    },  /* [0] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[1].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[1].elem)),  /* prev */
        },  /* elem */
    },  /* [1] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[2].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[2].elem)),  /* prev */
        },  /* elem */
    },  /* [2] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[3].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[3].elem)),  /* prev */
        },  /* elem */
    },  /* [3] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[4].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[4].elem)),  /* prev */
        },  /* elem */
    },  /* [4] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[5].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[5].elem)),  /* prev */
        },  /* elem */
    },  /* [5] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[6].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[6].elem)),  /* prev */
        },  /* elem */
    },  /* [6] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[7].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[7].elem)),  /* prev */
        },  /* elem */
    },  /* [7] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[8].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[8].elem)),  /* prev */
        },  /* elem */
    },  /* [8] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[9].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[9].elem)),  /* prev */
        },  /* elem */
    },  /* [9] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[10].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[10].elem)),  /* prev */
        },  /* elem */
    },  /* [10] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[11].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[11].elem)),  /* prev */
        },  /* elem */
    },  /* [11] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[12].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[12].elem)),  /* prev */
        },  /* elem */
    },  /* [12] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[13].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[13].elem)),  /* prev */
        },  /* elem */
    },  /* [13] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[14].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[14].elem)),  /* prev */
        },  /* elem */
    },  /* [14] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[15].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Swi_Module_State_0_readyQ__A[15].elem)),  /* prev */
        },  /* elem */
    },  /* [15] */
};

/* Module__state__V */
ti_sysbios_knl_Swi_Module_State__ ti_sysbios_knl_Swi_Module__state__V = {
    1,  /* locked */
    (xdc_UInt)0x0,  /* curSet */
    (xdc_UInt)0x0,  /* curTrigger */
    0,  /* curSwi */
    0,  /* curQ */
    ((void*)ti_sysbios_knl_Swi_Module_State_0_readyQ__A),  /* readyQ */
    ((void*)0),  /* constructedSwis */
};

/* --> ti_sysbios_knl_Swi_hooks__A */
const __T1_ti_sysbios_knl_Swi_hooks ti_sysbios_knl_Swi_hooks__A[1] = {
    {
        ((xdc_Void(*)(xdc_Int))0),  /* registerFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Swi_Handle,xdc_runtime_Error_Block*))0),  /* createFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Swi_Handle))0),  /* readyFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Swi_Handle))((xdc_Fxn)ti_sysbios_utils_Load_swiBeginHook__E)),  /* beginFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Swi_Handle))((xdc_Fxn)ti_sysbios_utils_Load_swiEndHook__E)),  /* endFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Swi_Handle))0),  /* deleteFxn */
    },  /* [0] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__diagsEnabled ti_sysbios_knl_Swi_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__diagsIncluded ti_sysbios_knl_Swi_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__diagsMask ti_sysbios_knl_Swi_Module__diagsMask__C = ((CT__ti_sysbios_knl_Swi_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__gateObj ti_sysbios_knl_Swi_Module__gateObj__C = ((CT__ti_sysbios_knl_Swi_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__gatePrms ti_sysbios_knl_Swi_Module__gatePrms__C = ((CT__ti_sysbios_knl_Swi_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__id ti_sysbios_knl_Swi_Module__id__C = (xdc_Bits16)0x801d;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerDefined ti_sysbios_knl_Swi_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerObj ti_sysbios_knl_Swi_Module__loggerObj__C = ((CT__ti_sysbios_knl_Swi_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerFxn0 ti_sysbios_knl_Swi_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Swi_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerFxn1 ti_sysbios_knl_Swi_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Swi_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerFxn2 ti_sysbios_knl_Swi_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Swi_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerFxn4 ti_sysbios_knl_Swi_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Swi_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__loggerFxn8 ti_sysbios_knl_Swi_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Swi_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Module__startupDoneFxn ti_sysbios_knl_Swi_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Swi_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Object__count ti_sysbios_knl_Swi_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Object__heap ti_sysbios_knl_Swi_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Object__sizeof ti_sysbios_knl_Swi_Object__sizeof__C = sizeof(ti_sysbios_knl_Swi_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Swi_Object__table ti_sysbios_knl_Swi_Object__table__C = ti_sysbios_knl_Swi_Object__table__V;

/* LM_begin__C */
__FAR__ const CT__ti_sysbios_knl_Swi_LM_begin ti_sysbios_knl_Swi_LM_begin__C = (((xdc_runtime_Log_Event)4111) << 16 | 768);

/* LD_end__C */
__FAR__ const CT__ti_sysbios_knl_Swi_LD_end ti_sysbios_knl_Swi_LD_end__C = (((xdc_runtime_Log_Event)4158) << 16 | 512);

/* LM_post__C */
__FAR__ const CT__ti_sysbios_knl_Swi_LM_post ti_sysbios_knl_Swi_LM_post__C = (((xdc_runtime_Log_Event)4176) << 16 | 768);

/* A_swiDisabled__C */
__FAR__ const CT__ti_sysbios_knl_Swi_A_swiDisabled ti_sysbios_knl_Swi_A_swiDisabled__C = (((xdc_runtime_Assert_Id)876) << 16 | 16);

/* A_badPriority__C */
__FAR__ const CT__ti_sysbios_knl_Swi_A_badPriority ti_sysbios_knl_Swi_A_badPriority__C = (((xdc_runtime_Assert_Id)933) << 16 | 16);

/* numPriorities__C */
__FAR__ const CT__ti_sysbios_knl_Swi_numPriorities ti_sysbios_knl_Swi_numPriorities__C = (xdc_UInt)0x10;

/* hooks__C */
__FAR__ const CT__ti_sysbios_knl_Swi_hooks ti_sysbios_knl_Swi_hooks__C = {1, ((__T1_ti_sysbios_knl_Swi_hooks*)ti_sysbios_knl_Swi_hooks__A)};

/* taskDisable__C */
__FAR__ const CT__ti_sysbios_knl_Swi_taskDisable ti_sysbios_knl_Swi_taskDisable__C = ((CT__ti_sysbios_knl_Swi_taskDisable)((xdc_Fxn)ti_sysbios_knl_Task_disable__E));

/* taskRestore__C */
__FAR__ const CT__ti_sysbios_knl_Swi_taskRestore ti_sysbios_knl_Swi_taskRestore__C = ((CT__ti_sysbios_knl_Swi_taskRestore)((xdc_Fxn)ti_sysbios_knl_Task_restore__E));

/* numConstructedSwis__C */
__FAR__ const CT__ti_sysbios_knl_Swi_numConstructedSwis ti_sysbios_knl_Swi_numConstructedSwis__C = (xdc_UInt)0x0;


/*
 * ======== ti.sysbios.knl.Task INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Task_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_knl_Task_Params ti_sysbios_knl_Task_Object__PARAMS__C = {
    sizeof (ti_sysbios_knl_Task_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_knl_Task_Object__PARAMS__C.__iprms, /* instance */
    ((xdc_UArg)(0x0)),  /* arg0 */
    ((xdc_UArg)(0x0)),  /* arg1 */
    (xdc_Int)0x1,  /* priority */
    ((xdc_Ptr)0),  /* stack */
    (xdc_SizeT)0x0,  /* stackSize */
    0,  /* stackHeap */
    ((xdc_Ptr)0),  /* env */
    1,  /* vitalTaskFlag */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* --> ti_sysbios_knl_Task_Instance_State_0_stack__A */
__T1_ti_sysbios_knl_Task_Instance_State__stack ti_sysbios_knl_Task_Instance_State_0_stack__A[16384];

/* --> ti_sysbios_knl_Task_Instance_State_0_hookEnv__A */
__T1_ti_sysbios_knl_Task_Instance_State__hookEnv ti_sysbios_knl_Task_Instance_State_0_hookEnv__A[2];

/* Module__root__V */
ti_sysbios_knl_Task_Module__ ti_sysbios_knl_Task_Module__root__V = {
    {&ti_sysbios_knl_Task_Module__root__V.link,  /* link.next */
    &ti_sysbios_knl_Task_Module__root__V.link},  /* link.prev */
};

/* Object__table__V */
ti_sysbios_knl_Task_Object__ ti_sysbios_knl_Task_Object__table__V[1] = {
    {/* instance#0 */
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Object__table__V[0].qElem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Object__table__V[0].qElem)),  /* prev */
        },  /* qElem */
        (xdc_Int)0x0,  /* priority */
        (xdc_UInt)0x1,  /* mask */
        ((xdc_Ptr)0),  /* context */
        ti_sysbios_knl_Task_Mode_READY,  /* mode */
        ((ti_sysbios_knl_Task_PendElem*)0),  /* pendElem */
        (xdc_SizeT)0x4000,  /* stackSize */
        ((void*)ti_sysbios_knl_Task_Instance_State_0_stack__A),  /* stack */
        0,  /* stackHeap */
        ((xdc_Void(*)(xdc_UArg,xdc_UArg))((xdc_Fxn)ti_sysbios_knl_Idle_loop__E)),  /* fxn */
        ((xdc_UArg)(0x0)),  /* arg0 */
        ((xdc_UArg)(0x0)),  /* arg1 */
        ((xdc_Ptr)0),  /* env */
        ((void*)ti_sysbios_knl_Task_Instance_State_0_hookEnv__A),  /* hookEnv */
        1,  /* vitalTaskFlag */
        0,  /* readyQ */
    },
};

/* --> ti_sysbios_knl_Task_Module_State_0_readyQ__A */
__T1_ti_sysbios_knl_Task_Module_State__readyQ ti_sysbios_knl_Task_Module_State_0_readyQ__A[16] = {
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[0].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[0].elem)),  /* prev */
        },  /* elem */
    },  /* [0] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[1].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[1].elem)),  /* prev */
        },  /* elem */
    },  /* [1] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[2].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[2].elem)),  /* prev */
        },  /* elem */
    },  /* [2] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[3].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[3].elem)),  /* prev */
        },  /* elem */
    },  /* [3] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[4].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[4].elem)),  /* prev */
        },  /* elem */
    },  /* [4] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[5].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[5].elem)),  /* prev */
        },  /* elem */
    },  /* [5] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[6].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[6].elem)),  /* prev */
        },  /* elem */
    },  /* [6] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[7].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[7].elem)),  /* prev */
        },  /* elem */
    },  /* [7] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[8].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[8].elem)),  /* prev */
        },  /* elem */
    },  /* [8] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[9].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[9].elem)),  /* prev */
        },  /* elem */
    },  /* [9] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[10].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[10].elem)),  /* prev */
        },  /* elem */
    },  /* [10] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[11].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[11].elem)),  /* prev */
        },  /* elem */
    },  /* [11] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[12].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[12].elem)),  /* prev */
        },  /* elem */
    },  /* [12] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[13].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[13].elem)),  /* prev */
        },  /* elem */
    },  /* [13] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[14].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[14].elem)),  /* prev */
        },  /* elem */
    },  /* [14] */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[15].elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module_State_0_readyQ__A[15].elem)),  /* prev */
        },  /* elem */
    },  /* [15] */
};

/* Module__state__V */
ti_sysbios_knl_Task_Module_State__ ti_sysbios_knl_Task_Module__state__V = {
    1,  /* locked */
    (xdc_UInt)0x0,  /* curSet */
    0,  /* workFlag */
    (xdc_UInt)0x1,  /* vitalTasks */
    0,  /* curTask */
    0,  /* curQ */
    ((void*)ti_sysbios_knl_Task_Module_State_0_readyQ__A),  /* readyQ */
    (ti_sysbios_knl_Task_Handle)&ti_sysbios_knl_Task_Object__table__V[0],  /* idleTask */
    ((void*)0),  /* constructedTasks */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module__state__V.Object_field_inactiveQ.elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module__state__V.Object_field_inactiveQ.elem)),  /* prev */
        },  /* elem */
    },  /* Object_field_inactiveQ */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module__state__V.Object_field_terminatedQ.elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_knl_Task_Module__state__V.Object_field_terminatedQ.elem)),  /* prev */
        },  /* elem */
    },  /* Object_field_terminatedQ */
};

/* --> ti_sysbios_knl_Task_hooks__A */
const __T1_ti_sysbios_knl_Task_hooks ti_sysbios_knl_Task_hooks__A[2] = {
    {
        ((xdc_Void(*)(xdc_Int))0),  /* registerFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*))0),  /* createFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))0),  /* readyFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle))((xdc_Fxn)ti_sysbios_knl_Task_checkStacks)),  /* switchFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))0),  /* exitFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))0),  /* deleteFxn */
    },  /* [0] */
    {
        ((xdc_Void(*)(xdc_Int))((xdc_Fxn)ti_sysbios_utils_Load_taskRegHook__E)),  /* registerFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle,xdc_runtime_Error_Block*))((xdc_Fxn)ti_sysbios_utils_Load_taskCreateHook__E)),  /* createFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))0),  /* readyFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle,ti_sysbios_knl_Task_Handle))((xdc_Fxn)ti_sysbios_utils_Load_taskSwitchHook__E)),  /* switchFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))0),  /* exitFxn */
        ((xdc_Void(*)(ti_sysbios_knl_Task_Handle))((xdc_Fxn)ti_sysbios_utils_Load_taskDeleteHook__E)),  /* deleteFxn */
    },  /* [1] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__diagsEnabled ti_sysbios_knl_Task_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__diagsIncluded ti_sysbios_knl_Task_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__diagsMask ti_sysbios_knl_Task_Module__diagsMask__C = ((CT__ti_sysbios_knl_Task_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__gateObj ti_sysbios_knl_Task_Module__gateObj__C = ((CT__ti_sysbios_knl_Task_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__gatePrms ti_sysbios_knl_Task_Module__gatePrms__C = ((CT__ti_sysbios_knl_Task_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__id ti_sysbios_knl_Task_Module__id__C = (xdc_Bits16)0x801e;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerDefined ti_sysbios_knl_Task_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerObj ti_sysbios_knl_Task_Module__loggerObj__C = ((CT__ti_sysbios_knl_Task_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn0 ti_sysbios_knl_Task_Module__loggerFxn0__C = ((CT__ti_sysbios_knl_Task_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn1 ti_sysbios_knl_Task_Module__loggerFxn1__C = ((CT__ti_sysbios_knl_Task_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn2 ti_sysbios_knl_Task_Module__loggerFxn2__C = ((CT__ti_sysbios_knl_Task_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn4 ti_sysbios_knl_Task_Module__loggerFxn4__C = ((CT__ti_sysbios_knl_Task_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__loggerFxn8 ti_sysbios_knl_Task_Module__loggerFxn8__C = ((CT__ti_sysbios_knl_Task_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_knl_Task_Module__startupDoneFxn ti_sysbios_knl_Task_Module__startupDoneFxn__C = ((CT__ti_sysbios_knl_Task_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_knl_Task_Object__count ti_sysbios_knl_Task_Object__count__C = 1;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_knl_Task_Object__heap ti_sysbios_knl_Task_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_knl_Task_Object__sizeof ti_sysbios_knl_Task_Object__sizeof__C = sizeof(ti_sysbios_knl_Task_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_knl_Task_Object__table ti_sysbios_knl_Task_Object__table__C = ti_sysbios_knl_Task_Object__table__V;

/* LM_switch__C */
__FAR__ const CT__ti_sysbios_knl_Task_LM_switch ti_sysbios_knl_Task_LM_switch__C = (((xdc_runtime_Log_Event)4216) << 16 | 768);

/* LM_sleep__C */
__FAR__ const CT__ti_sysbios_knl_Task_LM_sleep ti_sysbios_knl_Task_LM_sleep__C = (((xdc_runtime_Log_Event)4284) << 16 | 768);

/* LD_ready__C */
__FAR__ const CT__ti_sysbios_knl_Task_LD_ready ti_sysbios_knl_Task_LD_ready__C = (((xdc_runtime_Log_Event)4329) << 16 | 512);

/* LD_block__C */
__FAR__ const CT__ti_sysbios_knl_Task_LD_block ti_sysbios_knl_Task_LD_block__C = (((xdc_runtime_Log_Event)4370) << 16 | 512);

/* LM_yield__C */
__FAR__ const CT__ti_sysbios_knl_Task_LM_yield ti_sysbios_knl_Task_LM_yield__C = (((xdc_runtime_Log_Event)4402) << 16 | 768);

/* LM_setPri__C */
__FAR__ const CT__ti_sysbios_knl_Task_LM_setPri ti_sysbios_knl_Task_LM_setPri__C = (((xdc_runtime_Log_Event)4450) << 16 | 768);

/* LD_exit__C */
__FAR__ const CT__ti_sysbios_knl_Task_LD_exit ti_sysbios_knl_Task_LD_exit__C = (((xdc_runtime_Log_Event)4506) << 16 | 512);

/* E_stackOverflow__C */
__FAR__ const CT__ti_sysbios_knl_Task_E_stackOverflow ti_sysbios_knl_Task_E_stackOverflow__C = (((xdc_runtime_Error_Id)2766) << 16 | 0);

/* E_spOutOfBounds__C */
__FAR__ const CT__ti_sysbios_knl_Task_E_spOutOfBounds ti_sysbios_knl_Task_E_spOutOfBounds__C = (((xdc_runtime_Error_Id)2809) << 16 | 0);

/* A_badThreadType__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_badThreadType ti_sysbios_knl_Task_A_badThreadType__C = (((xdc_runtime_Assert_Id)982) << 16 | 16);

/* A_badTaskState__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_badTaskState ti_sysbios_knl_Task_A_badTaskState__C = (((xdc_runtime_Assert_Id)1051) << 16 | 16);

/* A_noPendElem__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_noPendElem ti_sysbios_knl_Task_A_noPendElem__C = (((xdc_runtime_Assert_Id)1105) << 16 | 16);

/* A_taskDisabled__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_taskDisabled ti_sysbios_knl_Task_A_taskDisabled__C = (((xdc_runtime_Assert_Id)1159) << 16 | 16);

/* A_badPriority__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_badPriority ti_sysbios_knl_Task_A_badPriority__C = (((xdc_runtime_Assert_Id)1222) << 16 | 16);

/* A_badTimeout__C */
__FAR__ const CT__ti_sysbios_knl_Task_A_badTimeout ti_sysbios_knl_Task_A_badTimeout__C = (((xdc_runtime_Assert_Id)1272) << 16 | 16);

/* numPriorities__C */
__FAR__ const CT__ti_sysbios_knl_Task_numPriorities ti_sysbios_knl_Task_numPriorities__C = (xdc_UInt)0x10;

/* defaultStackSize__C */
__FAR__ const CT__ti_sysbios_knl_Task_defaultStackSize ti_sysbios_knl_Task_defaultStackSize__C = (xdc_SizeT)0x4000;

/* defaultStackHeap__C */
__FAR__ const CT__ti_sysbios_knl_Task_defaultStackHeap ti_sysbios_knl_Task_defaultStackHeap__C = 0;

/* allBlockedFunc__C */
__FAR__ const CT__ti_sysbios_knl_Task_allBlockedFunc ti_sysbios_knl_Task_allBlockedFunc__C = ((CT__ti_sysbios_knl_Task_allBlockedFunc)((xdc_Fxn)ti_sysbios_knl_Task_allBlockedFunction__I));

/* initStackFlag__C */
__FAR__ const CT__ti_sysbios_knl_Task_initStackFlag ti_sysbios_knl_Task_initStackFlag__C = 1;

/* deleteTerminatedTasks__C */
__FAR__ const CT__ti_sysbios_knl_Task_deleteTerminatedTasks ti_sysbios_knl_Task_deleteTerminatedTasks__C = 0;

/* hooks__C */
__FAR__ const CT__ti_sysbios_knl_Task_hooks ti_sysbios_knl_Task_hooks__C = {2, ((__T1_ti_sysbios_knl_Task_hooks*)ti_sysbios_knl_Task_hooks__A)};

/* numConstructedTasks__C */
__FAR__ const CT__ti_sysbios_knl_Task_numConstructedTasks ti_sysbios_knl_Task_numConstructedTasks__C = (xdc_UInt)0x0;


/*
 * ======== ti.sysbios.knl.Task_SupportProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.syncs.SyncSem INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_syncs_SyncSem_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_syncs_SyncSem_Params ti_sysbios_syncs_SyncSem_Object__PARAMS__C = {
    sizeof (ti_sysbios_syncs_SyncSem_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_syncs_SyncSem_Object__PARAMS__C.__iprms, /* instance */
    0,  /* sem */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_syncs_SyncSem_Module__ ti_sysbios_syncs_SyncSem_Module__root__V = {
    {&ti_sysbios_syncs_SyncSem_Module__root__V.link,  /* link.next */
    &ti_sysbios_syncs_SyncSem_Module__root__V.link},  /* link.prev */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__diagsEnabled ti_sysbios_syncs_SyncSem_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__diagsIncluded ti_sysbios_syncs_SyncSem_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__diagsMask ti_sysbios_syncs_SyncSem_Module__diagsMask__C = ((CT__ti_sysbios_syncs_SyncSem_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__gateObj ti_sysbios_syncs_SyncSem_Module__gateObj__C = ((CT__ti_sysbios_syncs_SyncSem_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__gatePrms ti_sysbios_syncs_SyncSem_Module__gatePrms__C = ((CT__ti_sysbios_syncs_SyncSem_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__id ti_sysbios_syncs_SyncSem_Module__id__C = (xdc_Bits16)0x8039;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerDefined ti_sysbios_syncs_SyncSem_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerObj ti_sysbios_syncs_SyncSem_Module__loggerObj__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn0 ti_sysbios_syncs_SyncSem_Module__loggerFxn0__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn1 ti_sysbios_syncs_SyncSem_Module__loggerFxn1__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn2 ti_sysbios_syncs_SyncSem_Module__loggerFxn2__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn4 ti_sysbios_syncs_SyncSem_Module__loggerFxn4__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn8 ti_sysbios_syncs_SyncSem_Module__loggerFxn8__C = ((CT__ti_sysbios_syncs_SyncSem_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Module__startupDoneFxn ti_sysbios_syncs_SyncSem_Module__startupDoneFxn__C = ((CT__ti_sysbios_syncs_SyncSem_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Object__count ti_sysbios_syncs_SyncSem_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Object__heap ti_sysbios_syncs_SyncSem_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Object__sizeof ti_sysbios_syncs_SyncSem_Object__sizeof__C = sizeof(ti_sysbios_syncs_SyncSem_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_syncs_SyncSem_Object__table ti_sysbios_syncs_SyncSem_Object__table__C = 0;


/*
 * ======== ti.sysbios.timers.dmtimer.Timer INITIALIZERS ========
 */

/* Object__DESC__C */
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_timers_dmtimer_Timer_Object__DESC__C;

/* Object__PARAMS__C */
__FAR__ const ti_sysbios_timers_dmtimer_Timer_Params ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C = {
    sizeof (ti_sysbios_timers_dmtimer_Timer_Params), /* __size */
    0, /* __self */
    0, /* __fxns */
    (xdc_runtime_IInstance_Params*)&ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C.__iprms, /* instance */
    ti_sysbios_interfaces_ITimer_RunMode_CONTINUOUS,  /* runMode */
    ti_sysbios_interfaces_ITimer_StartMode_AUTO,  /* startMode */
    ((xdc_UArg)0),  /* arg */
    (xdc_UInt32)0x0,  /* period */
    ti_sysbios_interfaces_ITimer_PeriodType_MICROSECS,  /* periodType */
    {
        (xdc_Bits32)0x0,  /* hi */
        (xdc_Bits32)0x0,  /* lo */
    },  /* extFreq */
    {
        (xdc_Bits8)0x0,  /* idlemode */
        (xdc_Bits8)0x0,  /* emufree */
        (xdc_Bits8)0x1,  /* softreset */
    },  /* tiocpCfg */
    {
        (xdc_Bits8)0x0,  /* mat_it_ena */
        (xdc_Bits8)0x1,  /* ovf_it_ena */
        (xdc_Bits8)0x0,  /* tcar_it_ena */
    },  /* tier */
    {
        (xdc_Bits8)0x0,  /* mat_wup_ena */
        (xdc_Bits8)0x0,  /* ovf_wup_ena */
        (xdc_Bits8)0x0,  /* tcar_wup_ena */
    },  /* twer */
    {
        (xdc_Bits32)0x0,  /* ptv */
        (xdc_Bits8)0x0,  /* pre */
        (xdc_Bits8)0x0,  /* ce */
        (xdc_Bits8)0x0,  /* scpwm */
        (xdc_Bits16)0x0,  /* tcm */
        (xdc_Bits16)0x0,  /* trg */
        (xdc_Bits8)0x0,  /* pt */
        (xdc_Bits8)0x0,  /* captmode */
        (xdc_Bits8)0x0,  /* gpocfg */
    },  /* tclr */
    {
        (xdc_Bits8)0x0,  /* sft */
        (xdc_Bits8)0x1,  /* posted */
    },  /* tsicr */
    (xdc_UInt32)0x0,  /* tmar */
    (xdc_Int)(-0x0 - 1),  /* intNum */
    ((ti_sysbios_hal_Hwi_Params*)0),  /* hwiParams */
    {
        sizeof (xdc_runtime_IInstance_Params), /* __size */
        0,  /* name */
    },  /* instance */
};

/* Module__root__V */
ti_sysbios_timers_dmtimer_Timer_Module__ ti_sysbios_timers_dmtimer_Timer_Module__root__V = {
    {&ti_sysbios_timers_dmtimer_Timer_Module__root__V.link,  /* link.next */
    &ti_sysbios_timers_dmtimer_Timer_Module__root__V.link},  /* link.prev */
};

/* --> ti_sysbios_timers_dmtimer_Timer_Module_State_0_device__A */
__T1_ti_sysbios_timers_dmtimer_Timer_Module_State__device ti_sysbios_timers_dmtimer_Timer_Module_State_0_device__A[4] = {
    {
        (xdc_UInt)0x1c,  /* intNum */
        (xdc_UInt)(-0x0 - 1),  /* eventId */
        ((xdc_Ptr)((void*)0x48044000)),  /* baseAddr */
    },  /* [0] */
    {
        (xdc_UInt)0x1d,  /* intNum */
        (xdc_UInt)(-0x0 - 1),  /* eventId */
        ((xdc_Ptr)((void*)0x48046000)),  /* baseAddr */
    },  /* [1] */
    {
        (xdc_UInt)0x1e,  /* intNum */
        (xdc_UInt)(-0x0 - 1),  /* eventId */
        ((xdc_Ptr)((void*)0x48048000)),  /* baseAddr */
    },  /* [2] */
    {
        (xdc_UInt)0x1f,  /* intNum */
        (xdc_UInt)(-0x0 - 1),  /* eventId */
        ((xdc_Ptr)((void*)0x4804a000)),  /* baseAddr */
    },  /* [3] */
};

/* --> ti_sysbios_timers_dmtimer_Timer_Module_State_0_handles__A */
__T1_ti_sysbios_timers_dmtimer_Timer_Module_State__handles ti_sysbios_timers_dmtimer_Timer_Module_State_0_handles__A[4] = {
    0,  /* [0] */
    0,  /* [1] */
    0,  /* [2] */
    0,  /* [3] */
};

/* Module__state__V */
ti_sysbios_timers_dmtimer_Timer_Module_State__ ti_sysbios_timers_dmtimer_Timer_Module__state__V = {
    (xdc_Bits32)0xf,  /* availMask */
    {
        (xdc_Bits32)0x0,  /* hi */
        (xdc_Bits32)0x8000,  /* lo */
    },  /* intFreq */
    ((void*)ti_sysbios_timers_dmtimer_Timer_Module_State_0_device__A),  /* device */
    ((void*)ti_sysbios_timers_dmtimer_Timer_Module_State_0_handles__A),  /* handles */
    1,  /* firstInit */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__diagsEnabled ti_sysbios_timers_dmtimer_Timer_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__diagsIncluded ti_sysbios_timers_dmtimer_Timer_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__diagsMask ti_sysbios_timers_dmtimer_Timer_Module__diagsMask__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__gateObj ti_sysbios_timers_dmtimer_Timer_Module__gateObj__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__gatePrms ti_sysbios_timers_dmtimer_Timer_Module__gatePrms__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__id ti_sysbios_timers_dmtimer_Timer_Module__id__C = (xdc_Bits16)0x8032;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerDefined ti_sysbios_timers_dmtimer_Timer_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerObj ti_sysbios_timers_dmtimer_Timer_Module__loggerObj__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Module__startupDoneFxn ti_sysbios_timers_dmtimer_Timer_Module__startupDoneFxn__C = ((CT__ti_sysbios_timers_dmtimer_Timer_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Object__count ti_sysbios_timers_dmtimer_Timer_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Object__heap ti_sysbios_timers_dmtimer_Timer_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Object__sizeof ti_sysbios_timers_dmtimer_Timer_Object__sizeof__C = sizeof(ti_sysbios_timers_dmtimer_Timer_Object__);

/* Object__table__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_Object__table ti_sysbios_timers_dmtimer_Timer_Object__table__C = 0;

/* A_notAvailable__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_A_notAvailable ti_sysbios_timers_dmtimer_Timer_A_notAvailable__C = (((xdc_runtime_Assert_Id)1960) << 16 | 16);

/* E_invalidTimer__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_E_invalidTimer ti_sysbios_timers_dmtimer_Timer_E_invalidTimer__C = (((xdc_runtime_Error_Id)3268) << 16 | 0);

/* E_notAvailable__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_E_notAvailable ti_sysbios_timers_dmtimer_Timer_E_notAvailable__C = (((xdc_runtime_Error_Id)3304) << 16 | 0);

/* E_cannotSupport__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_E_cannotSupport ti_sysbios_timers_dmtimer_Timer_E_cannotSupport__C = (((xdc_runtime_Error_Id)3343) << 16 | 0);

/* E_freqMismatch__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_E_freqMismatch ti_sysbios_timers_dmtimer_Timer_E_freqMismatch__C = (((xdc_runtime_Error_Id)3583) << 16 | 0);

/* anyMask__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_anyMask ti_sysbios_timers_dmtimer_Timer_anyMask__C = (xdc_Bits32)0xf;

/* checkFrequency__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_checkFrequency ti_sysbios_timers_dmtimer_Timer_checkFrequency__C = 1;

/* startupNeeded__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_startupNeeded ti_sysbios_timers_dmtimer_Timer_startupNeeded__C = 0;

/* numTimerDevices__C */
__FAR__ const CT__ti_sysbios_timers_dmtimer_Timer_numTimerDevices ti_sysbios_timers_dmtimer_Timer_numTimerDevices__C = (xdc_Int)0x4;


/*
 * ======== ti.sysbios.timers.dmtimer.Timer_TimerSupportProxy INITIALIZERS ========
 */


/*
 * ======== ti.sysbios.utils.Load INITIALIZERS ========
 */

/* Module__state__V */
ti_sysbios_utils_Load_Module_State__ ti_sysbios_utils_Load_Module__state__V = {
    (xdc_Int)0x0,  /* taskHId */
    (xdc_UInt32)0x0,  /* taskStartTime */
    (xdc_UInt32)0x0,  /* timeElapsed */
    0,  /* runningTask */
    0,  /* firstSwitchDone */
    (xdc_UInt32)0x0,  /* swiStartTime */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.swiEnv.qElem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.swiEnv.qElem)),  /* prev */
        },  /* qElem */
        (xdc_UInt32)0x0,  /* totalTimeElapsed */
        (xdc_UInt32)0x0,  /* totalTime */
        (xdc_UInt32)0x0,  /* nextTotalTime */
        (xdc_UInt32)0x0,  /* timeOfLastUpdate */
        ((xdc_Ptr)0),  /* threadHandle */
    },  /* swiEnv */
    (xdc_UInt32)0x0,  /* swiCnt */
    (xdc_UInt32)0x0,  /* hwiStartTime */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.hwiEnv.qElem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.hwiEnv.qElem)),  /* prev */
        },  /* qElem */
        (xdc_UInt32)0x0,  /* totalTimeElapsed */
        (xdc_UInt32)0x0,  /* totalTime */
        (xdc_UInt32)0x0,  /* nextTotalTime */
        (xdc_UInt32)0x0,  /* timeOfLastUpdate */
        ((xdc_Ptr)0),  /* threadHandle */
    },  /* hwiEnv */
    (xdc_UInt32)0x0,  /* hwiCnt */
    (xdc_UInt32)0x0,  /* timeSlotCnt */
    (xdc_UInt32)0xffffffff,  /* minLoop */
    (xdc_UInt32)0x0,  /* minIdle */
    (xdc_UInt32)0x0,  /* t0 */
    (xdc_UInt32)0x0,  /* idleCnt */
    (xdc_UInt32)0x0,  /* cpuLoad */
    {
        {
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.Object_field_taskList.elem)),  /* next */
            ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.Object_field_taskList.elem)),  /* prev */
        },  /* elem */
    },  /* Object_field_taskList */
    {
        &ti_sysbios_gates_GateMutex_Module__FXNS__C,
        0,  /* owner */
        {
            0,  /* event */
            (xdc_UInt)0x1,  /* eventId */
            ti_sysbios_knl_Semaphore_Mode_COUNTING,  /* mode */
            (xdc_UInt16)0x1,  /* count */
            {
                {
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.Object_field_gate.Object_field_sem.Object_field_pendQ.elem)),  /* next */
                    ((ti_sysbios_knl_Queue_Elem*)((void*)&ti_sysbios_utils_Load_Module__state__V.Object_field_gate.Object_field_sem.Object_field_pendQ.elem)),  /* prev */
                },  /* elem */
            },  /* Object_field_pendQ */
        },  /* Object_field_sem */
    },  /* Object_field_gate */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__diagsEnabled ti_sysbios_utils_Load_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__diagsIncluded ti_sysbios_utils_Load_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__diagsMask ti_sysbios_utils_Load_Module__diagsMask__C = ((CT__ti_sysbios_utils_Load_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__gateObj ti_sysbios_utils_Load_Module__gateObj__C = ((CT__ti_sysbios_utils_Load_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__gatePrms ti_sysbios_utils_Load_Module__gatePrms__C = ((CT__ti_sysbios_utils_Load_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__id ti_sysbios_utils_Load_Module__id__C = (xdc_Bits16)0x8037;

/* Module__loggerDefined__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerDefined ti_sysbios_utils_Load_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerObj ti_sysbios_utils_Load_Module__loggerObj__C = ((CT__ti_sysbios_utils_Load_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn0 ti_sysbios_utils_Load_Module__loggerFxn0__C = ((CT__ti_sysbios_utils_Load_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn1 ti_sysbios_utils_Load_Module__loggerFxn1__C = ((CT__ti_sysbios_utils_Load_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn2 ti_sysbios_utils_Load_Module__loggerFxn2__C = ((CT__ti_sysbios_utils_Load_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn4 ti_sysbios_utils_Load_Module__loggerFxn4__C = ((CT__ti_sysbios_utils_Load_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__loggerFxn8 ti_sysbios_utils_Load_Module__loggerFxn8__C = ((CT__ti_sysbios_utils_Load_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__ti_sysbios_utils_Load_Module__startupDoneFxn ti_sysbios_utils_Load_Module__startupDoneFxn__C = ((CT__ti_sysbios_utils_Load_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__ti_sysbios_utils_Load_Object__count ti_sysbios_utils_Load_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__ti_sysbios_utils_Load_Object__heap ti_sysbios_utils_Load_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__ti_sysbios_utils_Load_Object__sizeof ti_sysbios_utils_Load_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__ti_sysbios_utils_Load_Object__table ti_sysbios_utils_Load_Object__table__C = 0;

/* LS_cpuLoad__C */
__FAR__ const CT__ti_sysbios_utils_Load_LS_cpuLoad ti_sysbios_utils_Load_LS_cpuLoad__C = (((xdc_runtime_Log_Event)4625) << 16 | 2048);

/* LS_hwiLoad__C */
__FAR__ const CT__ti_sysbios_utils_Load_LS_hwiLoad ti_sysbios_utils_Load_LS_hwiLoad__C = (((xdc_runtime_Log_Event)4642) << 16 | 2048);

/* LS_swiLoad__C */
__FAR__ const CT__ti_sysbios_utils_Load_LS_swiLoad ti_sysbios_utils_Load_LS_swiLoad__C = (((xdc_runtime_Log_Event)4660) << 16 | 2048);

/* LS_taskLoad__C */
__FAR__ const CT__ti_sysbios_utils_Load_LS_taskLoad ti_sysbios_utils_Load_LS_taskLoad__C = (((xdc_runtime_Log_Event)4678) << 16 | 2048);

/* postUpdate__C */
__FAR__ const CT__ti_sysbios_utils_Load_postUpdate ti_sysbios_utils_Load_postUpdate__C = ((CT__ti_sysbios_utils_Load_postUpdate)((xdc_Fxn)VpsUtils_prfLoadUpdate));

/* updateInIdle__C */
__FAR__ const CT__ti_sysbios_utils_Load_updateInIdle ti_sysbios_utils_Load_updateInIdle__C = 1;

/* windowInMs__C */
__FAR__ const CT__ti_sysbios_utils_Load_windowInMs ti_sysbios_utils_Load_windowInMs__C = (xdc_UInt)0x1f4;

/* hwiEnabled__C */
__FAR__ const CT__ti_sysbios_utils_Load_hwiEnabled ti_sysbios_utils_Load_hwiEnabled__C = 1;

/* swiEnabled__C */
__FAR__ const CT__ti_sysbios_utils_Load_swiEnabled ti_sysbios_utils_Load_swiEnabled__C = 1;

/* taskEnabled__C */
__FAR__ const CT__ti_sysbios_utils_Load_taskEnabled ti_sysbios_utils_Load_taskEnabled__C = 1;

/* autoAddTasks__C */
__FAR__ const CT__ti_sysbios_utils_Load_autoAddTasks ti_sysbios_utils_Load_autoAddTasks__C = 1;


/*
 * ======== xdc.runtime.Assert INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Assert_Module__diagsEnabled xdc_runtime_Assert_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Assert_Module__diagsIncluded xdc_runtime_Assert_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Assert_Module__diagsMask xdc_runtime_Assert_Module__diagsMask__C = ((CT__xdc_runtime_Assert_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Assert_Module__gateObj xdc_runtime_Assert_Module__gateObj__C = ((CT__xdc_runtime_Assert_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Assert_Module__gatePrms xdc_runtime_Assert_Module__gatePrms__C = ((CT__xdc_runtime_Assert_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Assert_Module__id xdc_runtime_Assert_Module__id__C = (xdc_Bits16)0x8002;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerDefined xdc_runtime_Assert_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerObj xdc_runtime_Assert_Module__loggerObj__C = ((CT__xdc_runtime_Assert_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerFxn0 xdc_runtime_Assert_Module__loggerFxn0__C = ((CT__xdc_runtime_Assert_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerFxn1 xdc_runtime_Assert_Module__loggerFxn1__C = ((CT__xdc_runtime_Assert_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerFxn2 xdc_runtime_Assert_Module__loggerFxn2__C = ((CT__xdc_runtime_Assert_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerFxn4 xdc_runtime_Assert_Module__loggerFxn4__C = ((CT__xdc_runtime_Assert_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Assert_Module__loggerFxn8 xdc_runtime_Assert_Module__loggerFxn8__C = ((CT__xdc_runtime_Assert_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Assert_Module__startupDoneFxn xdc_runtime_Assert_Module__startupDoneFxn__C = ((CT__xdc_runtime_Assert_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Assert_Object__count xdc_runtime_Assert_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Assert_Object__heap xdc_runtime_Assert_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Assert_Object__sizeof xdc_runtime_Assert_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Assert_Object__table xdc_runtime_Assert_Object__table__C = 0;

/* E_assertFailed__C */
__FAR__ const CT__xdc_runtime_Assert_E_assertFailed xdc_runtime_Assert_E_assertFailed__C = (((xdc_runtime_Error_Id)2489) << 16 | 0);


/*
 * ======== xdc.runtime.Core INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Core_Module__diagsEnabled xdc_runtime_Core_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Core_Module__diagsIncluded xdc_runtime_Core_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Core_Module__diagsMask xdc_runtime_Core_Module__diagsMask__C = ((CT__xdc_runtime_Core_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Core_Module__gateObj xdc_runtime_Core_Module__gateObj__C = ((CT__xdc_runtime_Core_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Core_Module__gatePrms xdc_runtime_Core_Module__gatePrms__C = ((CT__xdc_runtime_Core_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Core_Module__id xdc_runtime_Core_Module__id__C = (xdc_Bits16)0x8003;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerDefined xdc_runtime_Core_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerObj xdc_runtime_Core_Module__loggerObj__C = ((CT__xdc_runtime_Core_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerFxn0 xdc_runtime_Core_Module__loggerFxn0__C = ((CT__xdc_runtime_Core_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerFxn1 xdc_runtime_Core_Module__loggerFxn1__C = ((CT__xdc_runtime_Core_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerFxn2 xdc_runtime_Core_Module__loggerFxn2__C = ((CT__xdc_runtime_Core_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerFxn4 xdc_runtime_Core_Module__loggerFxn4__C = ((CT__xdc_runtime_Core_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Core_Module__loggerFxn8 xdc_runtime_Core_Module__loggerFxn8__C = ((CT__xdc_runtime_Core_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Core_Module__startupDoneFxn xdc_runtime_Core_Module__startupDoneFxn__C = ((CT__xdc_runtime_Core_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Core_Object__count xdc_runtime_Core_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Core_Object__heap xdc_runtime_Core_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Core_Object__sizeof xdc_runtime_Core_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Core_Object__table xdc_runtime_Core_Object__table__C = 0;

/* A_initializedParams__C */
__FAR__ const CT__xdc_runtime_Core_A_initializedParams xdc_runtime_Core_A_initializedParams__C = (((xdc_runtime_Assert_Id)1) << 16 | 16);


/*
 * ======== xdc.runtime.Defaults INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__diagsEnabled xdc_runtime_Defaults_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__diagsIncluded xdc_runtime_Defaults_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__diagsMask xdc_runtime_Defaults_Module__diagsMask__C = ((CT__xdc_runtime_Defaults_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__gateObj xdc_runtime_Defaults_Module__gateObj__C = ((CT__xdc_runtime_Defaults_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__gatePrms xdc_runtime_Defaults_Module__gatePrms__C = ((CT__xdc_runtime_Defaults_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__id xdc_runtime_Defaults_Module__id__C = (xdc_Bits16)0x8004;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerDefined xdc_runtime_Defaults_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerObj xdc_runtime_Defaults_Module__loggerObj__C = ((CT__xdc_runtime_Defaults_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerFxn0 xdc_runtime_Defaults_Module__loggerFxn0__C = ((CT__xdc_runtime_Defaults_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerFxn1 xdc_runtime_Defaults_Module__loggerFxn1__C = ((CT__xdc_runtime_Defaults_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerFxn2 xdc_runtime_Defaults_Module__loggerFxn2__C = ((CT__xdc_runtime_Defaults_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerFxn4 xdc_runtime_Defaults_Module__loggerFxn4__C = ((CT__xdc_runtime_Defaults_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__loggerFxn8 xdc_runtime_Defaults_Module__loggerFxn8__C = ((CT__xdc_runtime_Defaults_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Defaults_Module__startupDoneFxn xdc_runtime_Defaults_Module__startupDoneFxn__C = ((CT__xdc_runtime_Defaults_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Defaults_Object__count xdc_runtime_Defaults_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Defaults_Object__heap xdc_runtime_Defaults_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Defaults_Object__sizeof xdc_runtime_Defaults_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Defaults_Object__table xdc_runtime_Defaults_Object__table__C = 0;


/*
 * ======== xdc.runtime.Diags INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Diags_Module__diagsEnabled xdc_runtime_Diags_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Diags_Module__diagsIncluded xdc_runtime_Diags_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Diags_Module__diagsMask xdc_runtime_Diags_Module__diagsMask__C = ((CT__xdc_runtime_Diags_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Diags_Module__gateObj xdc_runtime_Diags_Module__gateObj__C = ((CT__xdc_runtime_Diags_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Diags_Module__gatePrms xdc_runtime_Diags_Module__gatePrms__C = ((CT__xdc_runtime_Diags_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Diags_Module__id xdc_runtime_Diags_Module__id__C = (xdc_Bits16)0x8005;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerDefined xdc_runtime_Diags_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerObj xdc_runtime_Diags_Module__loggerObj__C = ((CT__xdc_runtime_Diags_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerFxn0 xdc_runtime_Diags_Module__loggerFxn0__C = ((CT__xdc_runtime_Diags_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerFxn1 xdc_runtime_Diags_Module__loggerFxn1__C = ((CT__xdc_runtime_Diags_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerFxn2 xdc_runtime_Diags_Module__loggerFxn2__C = ((CT__xdc_runtime_Diags_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerFxn4 xdc_runtime_Diags_Module__loggerFxn4__C = ((CT__xdc_runtime_Diags_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Diags_Module__loggerFxn8 xdc_runtime_Diags_Module__loggerFxn8__C = ((CT__xdc_runtime_Diags_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Diags_Module__startupDoneFxn xdc_runtime_Diags_Module__startupDoneFxn__C = ((CT__xdc_runtime_Diags_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Diags_Object__count xdc_runtime_Diags_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Diags_Object__heap xdc_runtime_Diags_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Diags_Object__sizeof xdc_runtime_Diags_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Diags_Object__table xdc_runtime_Diags_Object__table__C = 0;

/* setMaskEnabled__C */
__FAR__ const CT__xdc_runtime_Diags_setMaskEnabled xdc_runtime_Diags_setMaskEnabled__C = 0;

/* dictBase__C */
__FAR__ const CT__xdc_runtime_Diags_dictBase xdc_runtime_Diags_dictBase__C = ((CT__xdc_runtime_Diags_dictBase)0);


/*
 * ======== xdc.runtime.Error INITIALIZERS ========
 */

/* Module__state__V */
xdc_runtime_Error_Module_State__ xdc_runtime_Error_Module__state__V = {
    (xdc_UInt16)0x0,  /* count */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Error_Module__diagsEnabled xdc_runtime_Error_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Error_Module__diagsIncluded xdc_runtime_Error_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Error_Module__diagsMask xdc_runtime_Error_Module__diagsMask__C = ((CT__xdc_runtime_Error_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Error_Module__gateObj xdc_runtime_Error_Module__gateObj__C = ((CT__xdc_runtime_Error_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Error_Module__gatePrms xdc_runtime_Error_Module__gatePrms__C = ((CT__xdc_runtime_Error_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Error_Module__id xdc_runtime_Error_Module__id__C = (xdc_Bits16)0x8006;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerDefined xdc_runtime_Error_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerObj xdc_runtime_Error_Module__loggerObj__C = ((CT__xdc_runtime_Error_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerFxn0 xdc_runtime_Error_Module__loggerFxn0__C = ((CT__xdc_runtime_Error_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerFxn1 xdc_runtime_Error_Module__loggerFxn1__C = ((CT__xdc_runtime_Error_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerFxn2 xdc_runtime_Error_Module__loggerFxn2__C = ((CT__xdc_runtime_Error_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerFxn4 xdc_runtime_Error_Module__loggerFxn4__C = ((CT__xdc_runtime_Error_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Error_Module__loggerFxn8 xdc_runtime_Error_Module__loggerFxn8__C = ((CT__xdc_runtime_Error_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Error_Module__startupDoneFxn xdc_runtime_Error_Module__startupDoneFxn__C = ((CT__xdc_runtime_Error_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Error_Object__count xdc_runtime_Error_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Error_Object__heap xdc_runtime_Error_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Error_Object__sizeof xdc_runtime_Error_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Error_Object__table xdc_runtime_Error_Object__table__C = 0;

/* E_generic__C */
__FAR__ const CT__xdc_runtime_Error_E_generic xdc_runtime_Error_E_generic__C = (((xdc_runtime_Error_Id)2511) << 16 | 0);

/* E_memory__C */
__FAR__ const CT__xdc_runtime_Error_E_memory xdc_runtime_Error_E_memory__C = (((xdc_runtime_Error_Id)2515) << 16 | 0);

/* E_msgCode__C */
__FAR__ const CT__xdc_runtime_Error_E_msgCode xdc_runtime_Error_E_msgCode__C = (((xdc_runtime_Error_Id)2549) << 16 | 0);

/* policy__C */
__FAR__ const CT__xdc_runtime_Error_policy xdc_runtime_Error_policy__C = xdc_runtime_Error_UNWIND;

/* raiseHook__C */
__FAR__ const CT__xdc_runtime_Error_raiseHook xdc_runtime_Error_raiseHook__C = ((CT__xdc_runtime_Error_raiseHook)((xdc_Fxn)ti_sysbios_BIOS_errorRaiseHook__I));

/* maxDepth__C */
__FAR__ const CT__xdc_runtime_Error_maxDepth xdc_runtime_Error_maxDepth__C = (xdc_UInt16)0x10;


/*
 * ======== xdc.runtime.Gate INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Gate_Module__diagsEnabled xdc_runtime_Gate_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Gate_Module__diagsIncluded xdc_runtime_Gate_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Gate_Module__diagsMask xdc_runtime_Gate_Module__diagsMask__C = ((CT__xdc_runtime_Gate_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Gate_Module__gateObj xdc_runtime_Gate_Module__gateObj__C = ((CT__xdc_runtime_Gate_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Gate_Module__gatePrms xdc_runtime_Gate_Module__gatePrms__C = ((CT__xdc_runtime_Gate_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Gate_Module__id xdc_runtime_Gate_Module__id__C = (xdc_Bits16)0x8007;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerDefined xdc_runtime_Gate_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerObj xdc_runtime_Gate_Module__loggerObj__C = ((CT__xdc_runtime_Gate_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerFxn0 xdc_runtime_Gate_Module__loggerFxn0__C = ((CT__xdc_runtime_Gate_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerFxn1 xdc_runtime_Gate_Module__loggerFxn1__C = ((CT__xdc_runtime_Gate_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerFxn2 xdc_runtime_Gate_Module__loggerFxn2__C = ((CT__xdc_runtime_Gate_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerFxn4 xdc_runtime_Gate_Module__loggerFxn4__C = ((CT__xdc_runtime_Gate_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Gate_Module__loggerFxn8 xdc_runtime_Gate_Module__loggerFxn8__C = ((CT__xdc_runtime_Gate_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Gate_Module__startupDoneFxn xdc_runtime_Gate_Module__startupDoneFxn__C = ((CT__xdc_runtime_Gate_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Gate_Object__count xdc_runtime_Gate_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Gate_Object__heap xdc_runtime_Gate_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Gate_Object__sizeof xdc_runtime_Gate_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Gate_Object__table xdc_runtime_Gate_Object__table__C = 0;


/*
 * ======== xdc.runtime.Log INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Log_Module__diagsEnabled xdc_runtime_Log_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Log_Module__diagsIncluded xdc_runtime_Log_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Log_Module__diagsMask xdc_runtime_Log_Module__diagsMask__C = ((CT__xdc_runtime_Log_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Log_Module__gateObj xdc_runtime_Log_Module__gateObj__C = ((CT__xdc_runtime_Log_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Log_Module__gatePrms xdc_runtime_Log_Module__gatePrms__C = ((CT__xdc_runtime_Log_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Log_Module__id xdc_runtime_Log_Module__id__C = (xdc_Bits16)0x8008;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerDefined xdc_runtime_Log_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerObj xdc_runtime_Log_Module__loggerObj__C = ((CT__xdc_runtime_Log_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerFxn0 xdc_runtime_Log_Module__loggerFxn0__C = ((CT__xdc_runtime_Log_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerFxn1 xdc_runtime_Log_Module__loggerFxn1__C = ((CT__xdc_runtime_Log_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerFxn2 xdc_runtime_Log_Module__loggerFxn2__C = ((CT__xdc_runtime_Log_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerFxn4 xdc_runtime_Log_Module__loggerFxn4__C = ((CT__xdc_runtime_Log_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Log_Module__loggerFxn8 xdc_runtime_Log_Module__loggerFxn8__C = ((CT__xdc_runtime_Log_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Log_Module__startupDoneFxn xdc_runtime_Log_Module__startupDoneFxn__C = ((CT__xdc_runtime_Log_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Log_Object__count xdc_runtime_Log_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Log_Object__heap xdc_runtime_Log_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Log_Object__sizeof xdc_runtime_Log_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Log_Object__table xdc_runtime_Log_Object__table__C = 0;

/* L_construct__C */
__FAR__ const CT__xdc_runtime_Log_L_construct xdc_runtime_Log_L_construct__C = (((xdc_runtime_Log_Event)3790) << 16 | 4);

/* L_create__C */
__FAR__ const CT__xdc_runtime_Log_L_create xdc_runtime_Log_L_create__C = (((xdc_runtime_Log_Event)3814) << 16 | 4);

/* L_destruct__C */
__FAR__ const CT__xdc_runtime_Log_L_destruct xdc_runtime_Log_L_destruct__C = (((xdc_runtime_Log_Event)3835) << 16 | 4);

/* L_delete__C */
__FAR__ const CT__xdc_runtime_Log_L_delete xdc_runtime_Log_L_delete__C = (((xdc_runtime_Log_Event)3854) << 16 | 4);

/* L_error__C */
__FAR__ const CT__xdc_runtime_Log_L_error xdc_runtime_Log_L_error__C = (((xdc_runtime_Log_Event)3871) << 16 | 192);

/* L_warning__C */
__FAR__ const CT__xdc_runtime_Log_L_warning xdc_runtime_Log_L_warning__C = (((xdc_runtime_Log_Event)3885) << 16 | 224);

/* L_info__C */
__FAR__ const CT__xdc_runtime_Log_L_info xdc_runtime_Log_L_info__C = (((xdc_runtime_Log_Event)3901) << 16 | 16384);

/* L_start__C */
__FAR__ const CT__xdc_runtime_Log_L_start xdc_runtime_Log_L_start__C = (((xdc_runtime_Log_Event)3908) << 16 | 32768);

/* L_stop__C */
__FAR__ const CT__xdc_runtime_Log_L_stop xdc_runtime_Log_L_stop__C = (((xdc_runtime_Log_Event)3919) << 16 | 32768);

/* L_startInstance__C */
__FAR__ const CT__xdc_runtime_Log_L_startInstance xdc_runtime_Log_L_startInstance__C = (((xdc_runtime_Log_Event)3929) << 16 | 32768);

/* L_stopInstance__C */
__FAR__ const CT__xdc_runtime_Log_L_stopInstance xdc_runtime_Log_L_stopInstance__C = (((xdc_runtime_Log_Event)3948) << 16 | 32768);


/*
 * ======== xdc.runtime.Main INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Main_Module__diagsEnabled xdc_runtime_Main_Module__diagsEnabled__C = (xdc_Bits32)0x98;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Main_Module__diagsIncluded xdc_runtime_Main_Module__diagsIncluded__C = (xdc_Bits32)0x98;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Main_Module__diagsMask xdc_runtime_Main_Module__diagsMask__C = ((CT__xdc_runtime_Main_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Main_Module__gateObj xdc_runtime_Main_Module__gateObj__C = ((CT__xdc_runtime_Main_Module__gateObj)((void*)(xdc_runtime_IGateProvider_Handle)&ti_sysbios_gates_GateHwi_Object__table__V[0]));

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Main_Module__gatePrms xdc_runtime_Main_Module__gatePrms__C = ((CT__xdc_runtime_Main_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Main_Module__id xdc_runtime_Main_Module__id__C = (xdc_Bits16)0x8009;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerDefined xdc_runtime_Main_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerObj xdc_runtime_Main_Module__loggerObj__C = ((CT__xdc_runtime_Main_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerFxn0 xdc_runtime_Main_Module__loggerFxn0__C = ((CT__xdc_runtime_Main_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerFxn1 xdc_runtime_Main_Module__loggerFxn1__C = ((CT__xdc_runtime_Main_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerFxn2 xdc_runtime_Main_Module__loggerFxn2__C = ((CT__xdc_runtime_Main_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerFxn4 xdc_runtime_Main_Module__loggerFxn4__C = ((CT__xdc_runtime_Main_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Main_Module__loggerFxn8 xdc_runtime_Main_Module__loggerFxn8__C = ((CT__xdc_runtime_Main_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Main_Module__startupDoneFxn xdc_runtime_Main_Module__startupDoneFxn__C = ((CT__xdc_runtime_Main_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Main_Object__count xdc_runtime_Main_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Main_Object__heap xdc_runtime_Main_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Main_Object__sizeof xdc_runtime_Main_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Main_Object__table xdc_runtime_Main_Object__table__C = 0;


/*
 * ======== xdc.runtime.Main_Module_GateProxy INITIALIZERS ========
 */


/*
 * ======== xdc.runtime.Memory INITIALIZERS ========
 */

/* Module__state__V */
xdc_runtime_Memory_Module_State__ xdc_runtime_Memory_Module__state__V = {
    (xdc_SizeT)0x8,  /* maxDefaultTypeAlign */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Memory_Module__diagsEnabled xdc_runtime_Memory_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Memory_Module__diagsIncluded xdc_runtime_Memory_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Memory_Module__diagsMask xdc_runtime_Memory_Module__diagsMask__C = ((CT__xdc_runtime_Memory_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Memory_Module__gateObj xdc_runtime_Memory_Module__gateObj__C = ((CT__xdc_runtime_Memory_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Memory_Module__gatePrms xdc_runtime_Memory_Module__gatePrms__C = ((CT__xdc_runtime_Memory_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Memory_Module__id xdc_runtime_Memory_Module__id__C = (xdc_Bits16)0x800a;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerDefined xdc_runtime_Memory_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerObj xdc_runtime_Memory_Module__loggerObj__C = ((CT__xdc_runtime_Memory_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerFxn0 xdc_runtime_Memory_Module__loggerFxn0__C = ((CT__xdc_runtime_Memory_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerFxn1 xdc_runtime_Memory_Module__loggerFxn1__C = ((CT__xdc_runtime_Memory_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerFxn2 xdc_runtime_Memory_Module__loggerFxn2__C = ((CT__xdc_runtime_Memory_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerFxn4 xdc_runtime_Memory_Module__loggerFxn4__C = ((CT__xdc_runtime_Memory_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Memory_Module__loggerFxn8 xdc_runtime_Memory_Module__loggerFxn8__C = ((CT__xdc_runtime_Memory_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Memory_Module__startupDoneFxn xdc_runtime_Memory_Module__startupDoneFxn__C = ((CT__xdc_runtime_Memory_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Memory_Object__count xdc_runtime_Memory_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Memory_Object__heap xdc_runtime_Memory_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Memory_Object__sizeof xdc_runtime_Memory_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Memory_Object__table xdc_runtime_Memory_Object__table__C = 0;

/* defaultHeapInstance__C */
__FAR__ const CT__xdc_runtime_Memory_defaultHeapInstance xdc_runtime_Memory_defaultHeapInstance__C = (xdc_runtime_IHeap_Handle)&ti_sysbios_heaps_HeapMem_Object__table__V[0];


/*
 * ======== xdc.runtime.Memory_HeapProxy INITIALIZERS ========
 */


/*
 * ======== xdc.runtime.Registry INITIALIZERS ========
 */

/* Module__state__V */
xdc_runtime_Registry_Module_State__ xdc_runtime_Registry_Module__state__V = {
    ((xdc_runtime_Types_RegDesc*)0),  /* listHead */
    (xdc_Bits16)0x7fff,  /* curId */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Registry_Module__diagsEnabled xdc_runtime_Registry_Module__diagsEnabled__C = (xdc_Bits32)0x90;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Registry_Module__diagsIncluded xdc_runtime_Registry_Module__diagsIncluded__C = (xdc_Bits32)0x90;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Registry_Module__diagsMask xdc_runtime_Registry_Module__diagsMask__C = ((CT__xdc_runtime_Registry_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Registry_Module__gateObj xdc_runtime_Registry_Module__gateObj__C = ((CT__xdc_runtime_Registry_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Registry_Module__gatePrms xdc_runtime_Registry_Module__gatePrms__C = ((CT__xdc_runtime_Registry_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Registry_Module__id xdc_runtime_Registry_Module__id__C = (xdc_Bits16)0x800b;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerDefined xdc_runtime_Registry_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerObj xdc_runtime_Registry_Module__loggerObj__C = ((CT__xdc_runtime_Registry_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerFxn0 xdc_runtime_Registry_Module__loggerFxn0__C = ((CT__xdc_runtime_Registry_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerFxn1 xdc_runtime_Registry_Module__loggerFxn1__C = ((CT__xdc_runtime_Registry_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerFxn2 xdc_runtime_Registry_Module__loggerFxn2__C = ((CT__xdc_runtime_Registry_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerFxn4 xdc_runtime_Registry_Module__loggerFxn4__C = ((CT__xdc_runtime_Registry_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Registry_Module__loggerFxn8 xdc_runtime_Registry_Module__loggerFxn8__C = ((CT__xdc_runtime_Registry_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Registry_Module__startupDoneFxn xdc_runtime_Registry_Module__startupDoneFxn__C = ((CT__xdc_runtime_Registry_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Registry_Object__count xdc_runtime_Registry_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Registry_Object__heap xdc_runtime_Registry_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Registry_Object__sizeof xdc_runtime_Registry_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Registry_Object__table xdc_runtime_Registry_Object__table__C = 0;


/*
 * ======== xdc.runtime.Startup INITIALIZERS ========
 */

/* Module__state__V */
xdc_runtime_Startup_Module_State__ xdc_runtime_Startup_Module__state__V = {
    ((xdc_Int*)0),  /* stateTab */
    0,  /* execFlag */
    0,  /* rtsDoneFlag */
};

/* --> xdc_runtime_Startup_firstFxns__A */
const __T1_xdc_runtime_Startup_firstFxns xdc_runtime_Startup_firstFxns__A[3] = {
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_heaps_HeapMem_init__I)),  /* [0] */
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_hal_Hwi_initStack)),  /* [1] */
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_initVTOR__I)),  /* [2] */
};

/* --> xdc_runtime_Startup_lastFxns__A */
const __T1_xdc_runtime_Startup_lastFxns xdc_runtime_Startup_lastFxns__A[1] = {
    ((xdc_Void(*)(xdc_Void))((xdc_Fxn)xdc_runtime_System_lastFxn__I)),  /* [0] */
};

/* --> xdc_runtime_Startup_sfxnTab__A */
const __T1_xdc_runtime_Startup_sfxnTab xdc_runtime_Startup_sfxnTab__A[16] = {
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)xdc_runtime_System_Module_startup__E)),  /* [0] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_hal_Hwi_Module_startup__E)),  /* [1] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_hal_Timer_Module_startup__E)),  /* [2] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_knl_Clock_Module_startup__E)),  /* [3] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_knl_Swi_Module_startup__E)),  /* [4] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_knl_Task_Module_startup__E)),  /* [5] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_Module_startup__E)),  /* [6] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_Core_Module_startup__E)),  /* [7] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E)),  /* [8] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_Timer_Module_startup__E)),  /* [9] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_TimerSupport_Module_startup__E)),  /* [10] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_TimestampProvider_Module_startup__E)),  /* [11] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_family_arm_ducati_Wugen_Module_startup__E)),  /* [12] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_heaps_HeapBuf_Module_startup__E)),  /* [13] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_timers_dmtimer_Timer_Module_startup__E)),  /* [14] */
    ((xdc_Int(*)(xdc_Int))((xdc_Fxn)ti_sysbios_hal_unicache_Cache_Module_startup__E)),  /* [15] */
};

/* --> xdc_runtime_Startup_sfxnRts__A */
const __T1_xdc_runtime_Startup_sfxnRts xdc_runtime_Startup_sfxnRts__A[16] = {
    1,  /* [0] */
    0,  /* [1] */
    0,  /* [2] */
    0,  /* [3] */
    0,  /* [4] */
    0,  /* [5] */
    0,  /* [6] */
    0,  /* [7] */
    1,  /* [8] */
    0,  /* [9] */
    0,  /* [10] */
    1,  /* [11] */
    0,  /* [12] */
    1,  /* [13] */
    0,  /* [14] */
    0,  /* [15] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Startup_Module__diagsEnabled xdc_runtime_Startup_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Startup_Module__diagsIncluded xdc_runtime_Startup_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Startup_Module__diagsMask xdc_runtime_Startup_Module__diagsMask__C = ((CT__xdc_runtime_Startup_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Startup_Module__gateObj xdc_runtime_Startup_Module__gateObj__C = ((CT__xdc_runtime_Startup_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Startup_Module__gatePrms xdc_runtime_Startup_Module__gatePrms__C = ((CT__xdc_runtime_Startup_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Startup_Module__id xdc_runtime_Startup_Module__id__C = (xdc_Bits16)0x800c;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerDefined xdc_runtime_Startup_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerObj xdc_runtime_Startup_Module__loggerObj__C = ((CT__xdc_runtime_Startup_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerFxn0 xdc_runtime_Startup_Module__loggerFxn0__C = ((CT__xdc_runtime_Startup_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerFxn1 xdc_runtime_Startup_Module__loggerFxn1__C = ((CT__xdc_runtime_Startup_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerFxn2 xdc_runtime_Startup_Module__loggerFxn2__C = ((CT__xdc_runtime_Startup_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerFxn4 xdc_runtime_Startup_Module__loggerFxn4__C = ((CT__xdc_runtime_Startup_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Startup_Module__loggerFxn8 xdc_runtime_Startup_Module__loggerFxn8__C = ((CT__xdc_runtime_Startup_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Startup_Module__startupDoneFxn xdc_runtime_Startup_Module__startupDoneFxn__C = ((CT__xdc_runtime_Startup_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Startup_Object__count xdc_runtime_Startup_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Startup_Object__heap xdc_runtime_Startup_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Startup_Object__sizeof xdc_runtime_Startup_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Startup_Object__table xdc_runtime_Startup_Object__table__C = 0;

/* maxPasses__C */
__FAR__ const CT__xdc_runtime_Startup_maxPasses xdc_runtime_Startup_maxPasses__C = (xdc_Int)0x20;

/* firstFxns__C */
__FAR__ const CT__xdc_runtime_Startup_firstFxns xdc_runtime_Startup_firstFxns__C = {3, ((__T1_xdc_runtime_Startup_firstFxns*)xdc_runtime_Startup_firstFxns__A)};

/* lastFxns__C */
__FAR__ const CT__xdc_runtime_Startup_lastFxns xdc_runtime_Startup_lastFxns__C = {1, ((__T1_xdc_runtime_Startup_lastFxns*)xdc_runtime_Startup_lastFxns__A)};

/* startModsFxn__C */
__FAR__ const CT__xdc_runtime_Startup_startModsFxn xdc_runtime_Startup_startModsFxn__C = ((CT__xdc_runtime_Startup_startModsFxn)((xdc_Fxn)xdc_runtime_Startup_startMods__I));

/* execImpl__C */
__FAR__ const CT__xdc_runtime_Startup_execImpl xdc_runtime_Startup_execImpl__C = ((CT__xdc_runtime_Startup_execImpl)((xdc_Fxn)xdc_runtime_Startup_exec__I));

/* sfxnTab__C */
__FAR__ const CT__xdc_runtime_Startup_sfxnTab xdc_runtime_Startup_sfxnTab__C = ((CT__xdc_runtime_Startup_sfxnTab)xdc_runtime_Startup_sfxnTab__A);

/* sfxnRts__C */
__FAR__ const CT__xdc_runtime_Startup_sfxnRts xdc_runtime_Startup_sfxnRts__C = ((CT__xdc_runtime_Startup_sfxnRts)xdc_runtime_Startup_sfxnRts__A);


/*
 * ======== xdc.runtime.SysStd INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__diagsEnabled xdc_runtime_SysStd_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__diagsIncluded xdc_runtime_SysStd_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__diagsMask xdc_runtime_SysStd_Module__diagsMask__C = ((CT__xdc_runtime_SysStd_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__gateObj xdc_runtime_SysStd_Module__gateObj__C = ((CT__xdc_runtime_SysStd_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__gatePrms xdc_runtime_SysStd_Module__gatePrms__C = ((CT__xdc_runtime_SysStd_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__id xdc_runtime_SysStd_Module__id__C = (xdc_Bits16)0x800e;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerDefined xdc_runtime_SysStd_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerObj xdc_runtime_SysStd_Module__loggerObj__C = ((CT__xdc_runtime_SysStd_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerFxn0 xdc_runtime_SysStd_Module__loggerFxn0__C = ((CT__xdc_runtime_SysStd_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerFxn1 xdc_runtime_SysStd_Module__loggerFxn1__C = ((CT__xdc_runtime_SysStd_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerFxn2 xdc_runtime_SysStd_Module__loggerFxn2__C = ((CT__xdc_runtime_SysStd_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerFxn4 xdc_runtime_SysStd_Module__loggerFxn4__C = ((CT__xdc_runtime_SysStd_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__loggerFxn8 xdc_runtime_SysStd_Module__loggerFxn8__C = ((CT__xdc_runtime_SysStd_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_SysStd_Module__startupDoneFxn xdc_runtime_SysStd_Module__startupDoneFxn__C = ((CT__xdc_runtime_SysStd_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_SysStd_Object__count xdc_runtime_SysStd_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_SysStd_Object__heap xdc_runtime_SysStd_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_SysStd_Object__sizeof xdc_runtime_SysStd_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_SysStd_Object__table xdc_runtime_SysStd_Object__table__C = 0;


/*
 * ======== xdc.runtime.System INITIALIZERS ========
 */

/* --> xdc_runtime_System_Module_State_0_atexitHandlers__A */
__T1_xdc_runtime_System_Module_State__atexitHandlers xdc_runtime_System_Module_State_0_atexitHandlers__A[8] = {
    ((xdc_Void(*)(xdc_Int))0),  /* [0] */
    ((xdc_Void(*)(xdc_Int))0),  /* [1] */
    ((xdc_Void(*)(xdc_Int))0),  /* [2] */
    ((xdc_Void(*)(xdc_Int))0),  /* [3] */
    ((xdc_Void(*)(xdc_Int))0),  /* [4] */
    ((xdc_Void(*)(xdc_Int))0),  /* [5] */
    ((xdc_Void(*)(xdc_Int))0),  /* [6] */
    ((xdc_Void(*)(xdc_Int))0),  /* [7] */
};

/* Module__state__V */
xdc_runtime_System_Module_State__ xdc_runtime_System_Module__state__V = {
    ((void*)xdc_runtime_System_Module_State_0_atexitHandlers__A),  /* atexitHandlers */
    (xdc_Int)0x0,  /* numAtexitHandlers */
    (xdc_Int)0xcafe,  /* exitStatus */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_System_Module__diagsEnabled xdc_runtime_System_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_System_Module__diagsIncluded xdc_runtime_System_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_System_Module__diagsMask xdc_runtime_System_Module__diagsMask__C = ((CT__xdc_runtime_System_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_System_Module__gateObj xdc_runtime_System_Module__gateObj__C = ((CT__xdc_runtime_System_Module__gateObj)((void*)(xdc_runtime_IGateProvider_Handle)&ti_sysbios_gates_GateHwi_Object__table__V[0]));

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_System_Module__gatePrms xdc_runtime_System_Module__gatePrms__C = ((CT__xdc_runtime_System_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_System_Module__id xdc_runtime_System_Module__id__C = (xdc_Bits16)0x800d;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerDefined xdc_runtime_System_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerObj xdc_runtime_System_Module__loggerObj__C = ((CT__xdc_runtime_System_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerFxn0 xdc_runtime_System_Module__loggerFxn0__C = ((CT__xdc_runtime_System_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerFxn1 xdc_runtime_System_Module__loggerFxn1__C = ((CT__xdc_runtime_System_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerFxn2 xdc_runtime_System_Module__loggerFxn2__C = ((CT__xdc_runtime_System_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerFxn4 xdc_runtime_System_Module__loggerFxn4__C = ((CT__xdc_runtime_System_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_System_Module__loggerFxn8 xdc_runtime_System_Module__loggerFxn8__C = ((CT__xdc_runtime_System_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_System_Module__startupDoneFxn xdc_runtime_System_Module__startupDoneFxn__C = ((CT__xdc_runtime_System_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_System_Object__count xdc_runtime_System_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_System_Object__heap xdc_runtime_System_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_System_Object__sizeof xdc_runtime_System_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_System_Object__table xdc_runtime_System_Object__table__C = 0;

/* A_cannotFitIntoArg__C */
__FAR__ const CT__xdc_runtime_System_A_cannotFitIntoArg xdc_runtime_System_A_cannotFitIntoArg__C = (((xdc_runtime_Assert_Id)307) << 16 | 16);

/* maxAtexitHandlers__C */
__FAR__ const CT__xdc_runtime_System_maxAtexitHandlers xdc_runtime_System_maxAtexitHandlers__C = (xdc_Int)0x8;

/* extendFxn__C */
__FAR__ const CT__xdc_runtime_System_extendFxn xdc_runtime_System_extendFxn__C = ((CT__xdc_runtime_System_extendFxn)((xdc_Fxn)xdc_runtime_System_printfExtend__I));


/*
 * ======== xdc.runtime.System_Module_GateProxy INITIALIZERS ========
 */


/*
 * ======== xdc.runtime.System_SupportProxy INITIALIZERS ========
 */


/*
 * ======== xdc.runtime.Text INITIALIZERS ========
 */

/* Module__state__V */
xdc_runtime_Text_Module_State__ xdc_runtime_Text_Module__state__V = {
    ((xdc_Ptr)((void*)&xdc_runtime_Text_charTab__A[0])),  /* charBase */
    ((xdc_Ptr)((void*)&xdc_runtime_Text_nodeTab__A[0])),  /* nodeBase */
};

/* --> xdc_runtime_Text_charTab__A */
const __T1_xdc_runtime_Text_charTab xdc_runtime_Text_charTab__A[5163] = {
    (xdc_Char)0x0,  /* [0] */
    (xdc_Char)0x41,  /* [1] */
    (xdc_Char)0x5f,  /* [2] */
    (xdc_Char)0x69,  /* [3] */
    (xdc_Char)0x6e,  /* [4] */
    (xdc_Char)0x69,  /* [5] */
    (xdc_Char)0x74,  /* [6] */
    (xdc_Char)0x69,  /* [7] */
    (xdc_Char)0x61,  /* [8] */
    (xdc_Char)0x6c,  /* [9] */
    (xdc_Char)0x69,  /* [10] */
    (xdc_Char)0x7a,  /* [11] */
    (xdc_Char)0x65,  /* [12] */
    (xdc_Char)0x64,  /* [13] */
    (xdc_Char)0x50,  /* [14] */
    (xdc_Char)0x61,  /* [15] */
    (xdc_Char)0x72,  /* [16] */
    (xdc_Char)0x61,  /* [17] */
    (xdc_Char)0x6d,  /* [18] */
    (xdc_Char)0x73,  /* [19] */
    (xdc_Char)0x3a,  /* [20] */
    (xdc_Char)0x20,  /* [21] */
    (xdc_Char)0x75,  /* [22] */
    (xdc_Char)0x6e,  /* [23] */
    (xdc_Char)0x69,  /* [24] */
    (xdc_Char)0x6e,  /* [25] */
    (xdc_Char)0x69,  /* [26] */
    (xdc_Char)0x74,  /* [27] */
    (xdc_Char)0x69,  /* [28] */
    (xdc_Char)0x61,  /* [29] */
    (xdc_Char)0x6c,  /* [30] */
    (xdc_Char)0x69,  /* [31] */
    (xdc_Char)0x7a,  /* [32] */
    (xdc_Char)0x65,  /* [33] */
    (xdc_Char)0x64,  /* [34] */
    (xdc_Char)0x20,  /* [35] */
    (xdc_Char)0x50,  /* [36] */
    (xdc_Char)0x61,  /* [37] */
    (xdc_Char)0x72,  /* [38] */
    (xdc_Char)0x61,  /* [39] */
    (xdc_Char)0x6d,  /* [40] */
    (xdc_Char)0x73,  /* [41] */
    (xdc_Char)0x20,  /* [42] */
    (xdc_Char)0x73,  /* [43] */
    (xdc_Char)0x74,  /* [44] */
    (xdc_Char)0x72,  /* [45] */
    (xdc_Char)0x75,  /* [46] */
    (xdc_Char)0x63,  /* [47] */
    (xdc_Char)0x74,  /* [48] */
    (xdc_Char)0x0,  /* [49] */
    (xdc_Char)0x48,  /* [50] */
    (xdc_Char)0x65,  /* [51] */
    (xdc_Char)0x61,  /* [52] */
    (xdc_Char)0x70,  /* [53] */
    (xdc_Char)0x4d,  /* [54] */
    (xdc_Char)0x69,  /* [55] */
    (xdc_Char)0x6e,  /* [56] */
    (xdc_Char)0x5f,  /* [57] */
    (xdc_Char)0x63,  /* [58] */
    (xdc_Char)0x72,  /* [59] */
    (xdc_Char)0x65,  /* [60] */
    (xdc_Char)0x61,  /* [61] */
    (xdc_Char)0x74,  /* [62] */
    (xdc_Char)0x65,  /* [63] */
    (xdc_Char)0x20,  /* [64] */
    (xdc_Char)0x63,  /* [65] */
    (xdc_Char)0x61,  /* [66] */
    (xdc_Char)0x6e,  /* [67] */
    (xdc_Char)0x6e,  /* [68] */
    (xdc_Char)0x6f,  /* [69] */
    (xdc_Char)0x74,  /* [70] */
    (xdc_Char)0x20,  /* [71] */
    (xdc_Char)0x68,  /* [72] */
    (xdc_Char)0x61,  /* [73] */
    (xdc_Char)0x76,  /* [74] */
    (xdc_Char)0x65,  /* [75] */
    (xdc_Char)0x20,  /* [76] */
    (xdc_Char)0x61,  /* [77] */
    (xdc_Char)0x20,  /* [78] */
    (xdc_Char)0x7a,  /* [79] */
    (xdc_Char)0x65,  /* [80] */
    (xdc_Char)0x72,  /* [81] */
    (xdc_Char)0x6f,  /* [82] */
    (xdc_Char)0x20,  /* [83] */
    (xdc_Char)0x73,  /* [84] */
    (xdc_Char)0x69,  /* [85] */
    (xdc_Char)0x7a,  /* [86] */
    (xdc_Char)0x65,  /* [87] */
    (xdc_Char)0x20,  /* [88] */
    (xdc_Char)0x76,  /* [89] */
    (xdc_Char)0x61,  /* [90] */
    (xdc_Char)0x6c,  /* [91] */
    (xdc_Char)0x75,  /* [92] */
    (xdc_Char)0x65,  /* [93] */
    (xdc_Char)0x0,  /* [94] */
    (xdc_Char)0x48,  /* [95] */
    (xdc_Char)0x65,  /* [96] */
    (xdc_Char)0x61,  /* [97] */
    (xdc_Char)0x70,  /* [98] */
    (xdc_Char)0x53,  /* [99] */
    (xdc_Char)0x74,  /* [100] */
    (xdc_Char)0x64,  /* [101] */
    (xdc_Char)0x5f,  /* [102] */
    (xdc_Char)0x63,  /* [103] */
    (xdc_Char)0x72,  /* [104] */
    (xdc_Char)0x65,  /* [105] */
    (xdc_Char)0x61,  /* [106] */
    (xdc_Char)0x74,  /* [107] */
    (xdc_Char)0x65,  /* [108] */
    (xdc_Char)0x20,  /* [109] */
    (xdc_Char)0x63,  /* [110] */
    (xdc_Char)0x61,  /* [111] */
    (xdc_Char)0x6e,  /* [112] */
    (xdc_Char)0x6e,  /* [113] */
    (xdc_Char)0x6f,  /* [114] */
    (xdc_Char)0x74,  /* [115] */
    (xdc_Char)0x20,  /* [116] */
    (xdc_Char)0x68,  /* [117] */
    (xdc_Char)0x61,  /* [118] */
    (xdc_Char)0x76,  /* [119] */
    (xdc_Char)0x65,  /* [120] */
    (xdc_Char)0x20,  /* [121] */
    (xdc_Char)0x61,  /* [122] */
    (xdc_Char)0x20,  /* [123] */
    (xdc_Char)0x7a,  /* [124] */
    (xdc_Char)0x65,  /* [125] */
    (xdc_Char)0x72,  /* [126] */
    (xdc_Char)0x6f,  /* [127] */
    (xdc_Char)0x20,  /* [128] */
    (xdc_Char)0x73,  /* [129] */
    (xdc_Char)0x69,  /* [130] */
    (xdc_Char)0x7a,  /* [131] */
    (xdc_Char)0x65,  /* [132] */
    (xdc_Char)0x20,  /* [133] */
    (xdc_Char)0x76,  /* [134] */
    (xdc_Char)0x61,  /* [135] */
    (xdc_Char)0x6c,  /* [136] */
    (xdc_Char)0x75,  /* [137] */
    (xdc_Char)0x65,  /* [138] */
    (xdc_Char)0x0,  /* [139] */
    (xdc_Char)0x48,  /* [140] */
    (xdc_Char)0x65,  /* [141] */
    (xdc_Char)0x61,  /* [142] */
    (xdc_Char)0x70,  /* [143] */
    (xdc_Char)0x53,  /* [144] */
    (xdc_Char)0x74,  /* [145] */
    (xdc_Char)0x64,  /* [146] */
    (xdc_Char)0x20,  /* [147] */
    (xdc_Char)0x69,  /* [148] */
    (xdc_Char)0x6e,  /* [149] */
    (xdc_Char)0x73,  /* [150] */
    (xdc_Char)0x74,  /* [151] */
    (xdc_Char)0x61,  /* [152] */
    (xdc_Char)0x6e,  /* [153] */
    (xdc_Char)0x63,  /* [154] */
    (xdc_Char)0x65,  /* [155] */
    (xdc_Char)0x20,  /* [156] */
    (xdc_Char)0x74,  /* [157] */
    (xdc_Char)0x6f,  /* [158] */
    (xdc_Char)0x74,  /* [159] */
    (xdc_Char)0x61,  /* [160] */
    (xdc_Char)0x6c,  /* [161] */
    (xdc_Char)0x46,  /* [162] */
    (xdc_Char)0x72,  /* [163] */
    (xdc_Char)0x65,  /* [164] */
    (xdc_Char)0x65,  /* [165] */
    (xdc_Char)0x53,  /* [166] */
    (xdc_Char)0x69,  /* [167] */
    (xdc_Char)0x7a,  /* [168] */
    (xdc_Char)0x65,  /* [169] */
    (xdc_Char)0x20,  /* [170] */
    (xdc_Char)0x69,  /* [171] */
    (xdc_Char)0x73,  /* [172] */
    (xdc_Char)0x20,  /* [173] */
    (xdc_Char)0x67,  /* [174] */
    (xdc_Char)0x72,  /* [175] */
    (xdc_Char)0x65,  /* [176] */
    (xdc_Char)0x61,  /* [177] */
    (xdc_Char)0x74,  /* [178] */
    (xdc_Char)0x65,  /* [179] */
    (xdc_Char)0x72,  /* [180] */
    (xdc_Char)0x20,  /* [181] */
    (xdc_Char)0x74,  /* [182] */
    (xdc_Char)0x68,  /* [183] */
    (xdc_Char)0x61,  /* [184] */
    (xdc_Char)0x6e,  /* [185] */
    (xdc_Char)0x20,  /* [186] */
    (xdc_Char)0x73,  /* [187] */
    (xdc_Char)0x74,  /* [188] */
    (xdc_Char)0x61,  /* [189] */
    (xdc_Char)0x72,  /* [190] */
    (xdc_Char)0x74,  /* [191] */
    (xdc_Char)0x69,  /* [192] */
    (xdc_Char)0x6e,  /* [193] */
    (xdc_Char)0x67,  /* [194] */
    (xdc_Char)0x20,  /* [195] */
    (xdc_Char)0x73,  /* [196] */
    (xdc_Char)0x69,  /* [197] */
    (xdc_Char)0x7a,  /* [198] */
    (xdc_Char)0x65,  /* [199] */
    (xdc_Char)0x0,  /* [200] */
    (xdc_Char)0x48,  /* [201] */
    (xdc_Char)0x65,  /* [202] */
    (xdc_Char)0x61,  /* [203] */
    (xdc_Char)0x70,  /* [204] */
    (xdc_Char)0x53,  /* [205] */
    (xdc_Char)0x74,  /* [206] */
    (xdc_Char)0x64,  /* [207] */
    (xdc_Char)0x5f,  /* [208] */
    (xdc_Char)0x61,  /* [209] */
    (xdc_Char)0x6c,  /* [210] */
    (xdc_Char)0x6c,  /* [211] */
    (xdc_Char)0x6f,  /* [212] */
    (xdc_Char)0x63,  /* [213] */
    (xdc_Char)0x20,  /* [214] */
    (xdc_Char)0x2d,  /* [215] */
    (xdc_Char)0x20,  /* [216] */
    (xdc_Char)0x72,  /* [217] */
    (xdc_Char)0x65,  /* [218] */
    (xdc_Char)0x71,  /* [219] */
    (xdc_Char)0x75,  /* [220] */
    (xdc_Char)0x65,  /* [221] */
    (xdc_Char)0x73,  /* [222] */
    (xdc_Char)0x74,  /* [223] */
    (xdc_Char)0x65,  /* [224] */
    (xdc_Char)0x64,  /* [225] */
    (xdc_Char)0x20,  /* [226] */
    (xdc_Char)0x61,  /* [227] */
    (xdc_Char)0x6c,  /* [228] */
    (xdc_Char)0x69,  /* [229] */
    (xdc_Char)0x67,  /* [230] */
    (xdc_Char)0x6e,  /* [231] */
    (xdc_Char)0x6d,  /* [232] */
    (xdc_Char)0x65,  /* [233] */
    (xdc_Char)0x6e,  /* [234] */
    (xdc_Char)0x74,  /* [235] */
    (xdc_Char)0x20,  /* [236] */
    (xdc_Char)0x69,  /* [237] */
    (xdc_Char)0x73,  /* [238] */
    (xdc_Char)0x20,  /* [239] */
    (xdc_Char)0x67,  /* [240] */
    (xdc_Char)0x72,  /* [241] */
    (xdc_Char)0x65,  /* [242] */
    (xdc_Char)0x61,  /* [243] */
    (xdc_Char)0x74,  /* [244] */
    (xdc_Char)0x65,  /* [245] */
    (xdc_Char)0x72,  /* [246] */
    (xdc_Char)0x20,  /* [247] */
    (xdc_Char)0x74,  /* [248] */
    (xdc_Char)0x68,  /* [249] */
    (xdc_Char)0x61,  /* [250] */
    (xdc_Char)0x6e,  /* [251] */
    (xdc_Char)0x20,  /* [252] */
    (xdc_Char)0x61,  /* [253] */
    (xdc_Char)0x6c,  /* [254] */
    (xdc_Char)0x6c,  /* [255] */
    (xdc_Char)0x6f,  /* [256] */
    (xdc_Char)0x77,  /* [257] */
    (xdc_Char)0x65,  /* [258] */
    (xdc_Char)0x64,  /* [259] */
    (xdc_Char)0x0,  /* [260] */
    (xdc_Char)0x41,  /* [261] */
    (xdc_Char)0x5f,  /* [262] */
    (xdc_Char)0x69,  /* [263] */
    (xdc_Char)0x6e,  /* [264] */
    (xdc_Char)0x76,  /* [265] */
    (xdc_Char)0x61,  /* [266] */
    (xdc_Char)0x6c,  /* [267] */
    (xdc_Char)0x69,  /* [268] */
    (xdc_Char)0x64,  /* [269] */
    (xdc_Char)0x4c,  /* [270] */
    (xdc_Char)0x6f,  /* [271] */
    (xdc_Char)0x67,  /* [272] */
    (xdc_Char)0x67,  /* [273] */
    (xdc_Char)0x65,  /* [274] */
    (xdc_Char)0x72,  /* [275] */
    (xdc_Char)0x3a,  /* [276] */
    (xdc_Char)0x20,  /* [277] */
    (xdc_Char)0x54,  /* [278] */
    (xdc_Char)0x68,  /* [279] */
    (xdc_Char)0x65,  /* [280] */
    (xdc_Char)0x20,  /* [281] */
    (xdc_Char)0x6c,  /* [282] */
    (xdc_Char)0x6f,  /* [283] */
    (xdc_Char)0x67,  /* [284] */
    (xdc_Char)0x67,  /* [285] */
    (xdc_Char)0x65,  /* [286] */
    (xdc_Char)0x72,  /* [287] */
    (xdc_Char)0x20,  /* [288] */
    (xdc_Char)0x69,  /* [289] */
    (xdc_Char)0x64,  /* [290] */
    (xdc_Char)0x20,  /* [291] */
    (xdc_Char)0x25,  /* [292] */
    (xdc_Char)0x64,  /* [293] */
    (xdc_Char)0x20,  /* [294] */
    (xdc_Char)0x69,  /* [295] */
    (xdc_Char)0x73,  /* [296] */
    (xdc_Char)0x20,  /* [297] */
    (xdc_Char)0x69,  /* [298] */
    (xdc_Char)0x6e,  /* [299] */
    (xdc_Char)0x76,  /* [300] */
    (xdc_Char)0x61,  /* [301] */
    (xdc_Char)0x6c,  /* [302] */
    (xdc_Char)0x69,  /* [303] */
    (xdc_Char)0x64,  /* [304] */
    (xdc_Char)0x2e,  /* [305] */
    (xdc_Char)0x0,  /* [306] */
    (xdc_Char)0x41,  /* [307] */
    (xdc_Char)0x5f,  /* [308] */
    (xdc_Char)0x63,  /* [309] */
    (xdc_Char)0x61,  /* [310] */
    (xdc_Char)0x6e,  /* [311] */
    (xdc_Char)0x6e,  /* [312] */
    (xdc_Char)0x6f,  /* [313] */
    (xdc_Char)0x74,  /* [314] */
    (xdc_Char)0x46,  /* [315] */
    (xdc_Char)0x69,  /* [316] */
    (xdc_Char)0x74,  /* [317] */
    (xdc_Char)0x49,  /* [318] */
    (xdc_Char)0x6e,  /* [319] */
    (xdc_Char)0x74,  /* [320] */
    (xdc_Char)0x6f,  /* [321] */
    (xdc_Char)0x41,  /* [322] */
    (xdc_Char)0x72,  /* [323] */
    (xdc_Char)0x67,  /* [324] */
    (xdc_Char)0x3a,  /* [325] */
    (xdc_Char)0x20,  /* [326] */
    (xdc_Char)0x73,  /* [327] */
    (xdc_Char)0x69,  /* [328] */
    (xdc_Char)0x7a,  /* [329] */
    (xdc_Char)0x65,  /* [330] */
    (xdc_Char)0x6f,  /* [331] */
    (xdc_Char)0x66,  /* [332] */
    (xdc_Char)0x28,  /* [333] */
    (xdc_Char)0x46,  /* [334] */
    (xdc_Char)0x6c,  /* [335] */
    (xdc_Char)0x6f,  /* [336] */
    (xdc_Char)0x61,  /* [337] */
    (xdc_Char)0x74,  /* [338] */
    (xdc_Char)0x29,  /* [339] */
    (xdc_Char)0x20,  /* [340] */
    (xdc_Char)0x3e,  /* [341] */
    (xdc_Char)0x20,  /* [342] */
    (xdc_Char)0x73,  /* [343] */
    (xdc_Char)0x69,  /* [344] */
    (xdc_Char)0x7a,  /* [345] */
    (xdc_Char)0x65,  /* [346] */
    (xdc_Char)0x6f,  /* [347] */
    (xdc_Char)0x66,  /* [348] */
    (xdc_Char)0x28,  /* [349] */
    (xdc_Char)0x41,  /* [350] */
    (xdc_Char)0x72,  /* [351] */
    (xdc_Char)0x67,  /* [352] */
    (xdc_Char)0x29,  /* [353] */
    (xdc_Char)0x0,  /* [354] */
    (xdc_Char)0x41,  /* [355] */
    (xdc_Char)0x5f,  /* [356] */
    (xdc_Char)0x63,  /* [357] */
    (xdc_Char)0x6c,  /* [358] */
    (xdc_Char)0x6f,  /* [359] */
    (xdc_Char)0x63,  /* [360] */
    (xdc_Char)0x6b,  /* [361] */
    (xdc_Char)0x44,  /* [362] */
    (xdc_Char)0x69,  /* [363] */
    (xdc_Char)0x73,  /* [364] */
    (xdc_Char)0x61,  /* [365] */
    (xdc_Char)0x62,  /* [366] */
    (xdc_Char)0x6c,  /* [367] */
    (xdc_Char)0x65,  /* [368] */
    (xdc_Char)0x64,  /* [369] */
    (xdc_Char)0x3a,  /* [370] */
    (xdc_Char)0x20,  /* [371] */
    (xdc_Char)0x43,  /* [372] */
    (xdc_Char)0x61,  /* [373] */
    (xdc_Char)0x6e,  /* [374] */
    (xdc_Char)0x6e,  /* [375] */
    (xdc_Char)0x6f,  /* [376] */
    (xdc_Char)0x74,  /* [377] */
    (xdc_Char)0x20,  /* [378] */
    (xdc_Char)0x63,  /* [379] */
    (xdc_Char)0x72,  /* [380] */
    (xdc_Char)0x65,  /* [381] */
    (xdc_Char)0x61,  /* [382] */
    (xdc_Char)0x74,  /* [383] */
    (xdc_Char)0x65,  /* [384] */
    (xdc_Char)0x20,  /* [385] */
    (xdc_Char)0x61,  /* [386] */
    (xdc_Char)0x20,  /* [387] */
    (xdc_Char)0x63,  /* [388] */
    (xdc_Char)0x6c,  /* [389] */
    (xdc_Char)0x6f,  /* [390] */
    (xdc_Char)0x63,  /* [391] */
    (xdc_Char)0x6b,  /* [392] */
    (xdc_Char)0x20,  /* [393] */
    (xdc_Char)0x69,  /* [394] */
    (xdc_Char)0x6e,  /* [395] */
    (xdc_Char)0x73,  /* [396] */
    (xdc_Char)0x74,  /* [397] */
    (xdc_Char)0x61,  /* [398] */
    (xdc_Char)0x6e,  /* [399] */
    (xdc_Char)0x63,  /* [400] */
    (xdc_Char)0x65,  /* [401] */
    (xdc_Char)0x20,  /* [402] */
    (xdc_Char)0x77,  /* [403] */
    (xdc_Char)0x68,  /* [404] */
    (xdc_Char)0x65,  /* [405] */
    (xdc_Char)0x6e,  /* [406] */
    (xdc_Char)0x20,  /* [407] */
    (xdc_Char)0x42,  /* [408] */
    (xdc_Char)0x49,  /* [409] */
    (xdc_Char)0x4f,  /* [410] */
    (xdc_Char)0x53,  /* [411] */
    (xdc_Char)0x2e,  /* [412] */
    (xdc_Char)0x63,  /* [413] */
    (xdc_Char)0x6c,  /* [414] */
    (xdc_Char)0x6f,  /* [415] */
    (xdc_Char)0x63,  /* [416] */
    (xdc_Char)0x6b,  /* [417] */
    (xdc_Char)0x45,  /* [418] */
    (xdc_Char)0x6e,  /* [419] */
    (xdc_Char)0x61,  /* [420] */
    (xdc_Char)0x62,  /* [421] */
    (xdc_Char)0x6c,  /* [422] */
    (xdc_Char)0x65,  /* [423] */
    (xdc_Char)0x64,  /* [424] */
    (xdc_Char)0x20,  /* [425] */
    (xdc_Char)0x69,  /* [426] */
    (xdc_Char)0x73,  /* [427] */
    (xdc_Char)0x20,  /* [428] */
    (xdc_Char)0x66,  /* [429] */
    (xdc_Char)0x61,  /* [430] */
    (xdc_Char)0x6c,  /* [431] */
    (xdc_Char)0x73,  /* [432] */
    (xdc_Char)0x65,  /* [433] */
    (xdc_Char)0x2e,  /* [434] */
    (xdc_Char)0x0,  /* [435] */
    (xdc_Char)0x41,  /* [436] */
    (xdc_Char)0x5f,  /* [437] */
    (xdc_Char)0x62,  /* [438] */
    (xdc_Char)0x61,  /* [439] */
    (xdc_Char)0x64,  /* [440] */
    (xdc_Char)0x54,  /* [441] */
    (xdc_Char)0x68,  /* [442] */
    (xdc_Char)0x72,  /* [443] */
    (xdc_Char)0x65,  /* [444] */
    (xdc_Char)0x61,  /* [445] */
    (xdc_Char)0x64,  /* [446] */
    (xdc_Char)0x54,  /* [447] */
    (xdc_Char)0x79,  /* [448] */
    (xdc_Char)0x70,  /* [449] */
    (xdc_Char)0x65,  /* [450] */
    (xdc_Char)0x3a,  /* [451] */
    (xdc_Char)0x20,  /* [452] */
    (xdc_Char)0x43,  /* [453] */
    (xdc_Char)0x61,  /* [454] */
    (xdc_Char)0x6e,  /* [455] */
    (xdc_Char)0x6e,  /* [456] */
    (xdc_Char)0x6f,  /* [457] */
    (xdc_Char)0x74,  /* [458] */
    (xdc_Char)0x20,  /* [459] */
    (xdc_Char)0x63,  /* [460] */
    (xdc_Char)0x72,  /* [461] */
    (xdc_Char)0x65,  /* [462] */
    (xdc_Char)0x61,  /* [463] */
    (xdc_Char)0x74,  /* [464] */
    (xdc_Char)0x65,  /* [465] */
    (xdc_Char)0x2f,  /* [466] */
    (xdc_Char)0x64,  /* [467] */
    (xdc_Char)0x65,  /* [468] */
    (xdc_Char)0x6c,  /* [469] */
    (xdc_Char)0x65,  /* [470] */
    (xdc_Char)0x74,  /* [471] */
    (xdc_Char)0x65,  /* [472] */
    (xdc_Char)0x20,  /* [473] */
    (xdc_Char)0x61,  /* [474] */
    (xdc_Char)0x20,  /* [475] */
    (xdc_Char)0x43,  /* [476] */
    (xdc_Char)0x6c,  /* [477] */
    (xdc_Char)0x6f,  /* [478] */
    (xdc_Char)0x63,  /* [479] */
    (xdc_Char)0x6b,  /* [480] */
    (xdc_Char)0x20,  /* [481] */
    (xdc_Char)0x66,  /* [482] */
    (xdc_Char)0x72,  /* [483] */
    (xdc_Char)0x6f,  /* [484] */
    (xdc_Char)0x6d,  /* [485] */
    (xdc_Char)0x20,  /* [486] */
    (xdc_Char)0x48,  /* [487] */
    (xdc_Char)0x77,  /* [488] */
    (xdc_Char)0x69,  /* [489] */
    (xdc_Char)0x20,  /* [490] */
    (xdc_Char)0x6f,  /* [491] */
    (xdc_Char)0x72,  /* [492] */
    (xdc_Char)0x20,  /* [493] */
    (xdc_Char)0x53,  /* [494] */
    (xdc_Char)0x77,  /* [495] */
    (xdc_Char)0x69,  /* [496] */
    (xdc_Char)0x20,  /* [497] */
    (xdc_Char)0x74,  /* [498] */
    (xdc_Char)0x68,  /* [499] */
    (xdc_Char)0x72,  /* [500] */
    (xdc_Char)0x65,  /* [501] */
    (xdc_Char)0x61,  /* [502] */
    (xdc_Char)0x64,  /* [503] */
    (xdc_Char)0x2e,  /* [504] */
    (xdc_Char)0x0,  /* [505] */
    (xdc_Char)0x41,  /* [506] */
    (xdc_Char)0x5f,  /* [507] */
    (xdc_Char)0x6e,  /* [508] */
    (xdc_Char)0x75,  /* [509] */
    (xdc_Char)0x6c,  /* [510] */
    (xdc_Char)0x6c,  /* [511] */
    (xdc_Char)0x45,  /* [512] */
    (xdc_Char)0x76,  /* [513] */
    (xdc_Char)0x65,  /* [514] */
    (xdc_Char)0x6e,  /* [515] */
    (xdc_Char)0x74,  /* [516] */
    (xdc_Char)0x4d,  /* [517] */
    (xdc_Char)0x61,  /* [518] */
    (xdc_Char)0x73,  /* [519] */
    (xdc_Char)0x6b,  /* [520] */
    (xdc_Char)0x73,  /* [521] */
    (xdc_Char)0x3a,  /* [522] */
    (xdc_Char)0x20,  /* [523] */
    (xdc_Char)0x6f,  /* [524] */
    (xdc_Char)0x72,  /* [525] */
    (xdc_Char)0x4d,  /* [526] */
    (xdc_Char)0x61,  /* [527] */
    (xdc_Char)0x73,  /* [528] */
    (xdc_Char)0x6b,  /* [529] */
    (xdc_Char)0x20,  /* [530] */
    (xdc_Char)0x61,  /* [531] */
    (xdc_Char)0x6e,  /* [532] */
    (xdc_Char)0x64,  /* [533] */
    (xdc_Char)0x20,  /* [534] */
    (xdc_Char)0x61,  /* [535] */
    (xdc_Char)0x6e,  /* [536] */
    (xdc_Char)0x64,  /* [537] */
    (xdc_Char)0x4d,  /* [538] */
    (xdc_Char)0x61,  /* [539] */
    (xdc_Char)0x73,  /* [540] */
    (xdc_Char)0x6b,  /* [541] */
    (xdc_Char)0x20,  /* [542] */
    (xdc_Char)0x61,  /* [543] */
    (xdc_Char)0x72,  /* [544] */
    (xdc_Char)0x65,  /* [545] */
    (xdc_Char)0x20,  /* [546] */
    (xdc_Char)0x6e,  /* [547] */
    (xdc_Char)0x75,  /* [548] */
    (xdc_Char)0x6c,  /* [549] */
    (xdc_Char)0x6c,  /* [550] */
    (xdc_Char)0x2e,  /* [551] */
    (xdc_Char)0x0,  /* [552] */
    (xdc_Char)0x41,  /* [553] */
    (xdc_Char)0x5f,  /* [554] */
    (xdc_Char)0x6e,  /* [555] */
    (xdc_Char)0x75,  /* [556] */
    (xdc_Char)0x6c,  /* [557] */
    (xdc_Char)0x6c,  /* [558] */
    (xdc_Char)0x45,  /* [559] */
    (xdc_Char)0x76,  /* [560] */
    (xdc_Char)0x65,  /* [561] */
    (xdc_Char)0x6e,  /* [562] */
    (xdc_Char)0x74,  /* [563] */
    (xdc_Char)0x49,  /* [564] */
    (xdc_Char)0x64,  /* [565] */
    (xdc_Char)0x3a,  /* [566] */
    (xdc_Char)0x20,  /* [567] */
    (xdc_Char)0x70,  /* [568] */
    (xdc_Char)0x6f,  /* [569] */
    (xdc_Char)0x73,  /* [570] */
    (xdc_Char)0x74,  /* [571] */
    (xdc_Char)0x65,  /* [572] */
    (xdc_Char)0x64,  /* [573] */
    (xdc_Char)0x20,  /* [574] */
    (xdc_Char)0x65,  /* [575] */
    (xdc_Char)0x76,  /* [576] */
    (xdc_Char)0x65,  /* [577] */
    (xdc_Char)0x6e,  /* [578] */
    (xdc_Char)0x74,  /* [579] */
    (xdc_Char)0x49,  /* [580] */
    (xdc_Char)0x64,  /* [581] */
    (xdc_Char)0x20,  /* [582] */
    (xdc_Char)0x69,  /* [583] */
    (xdc_Char)0x73,  /* [584] */
    (xdc_Char)0x20,  /* [585] */
    (xdc_Char)0x6e,  /* [586] */
    (xdc_Char)0x75,  /* [587] */
    (xdc_Char)0x6c,  /* [588] */
    (xdc_Char)0x6c,  /* [589] */
    (xdc_Char)0x2e,  /* [590] */
    (xdc_Char)0x0,  /* [591] */
    (xdc_Char)0x41,  /* [592] */
    (xdc_Char)0x5f,  /* [593] */
    (xdc_Char)0x65,  /* [594] */
    (xdc_Char)0x76,  /* [595] */
    (xdc_Char)0x65,  /* [596] */
    (xdc_Char)0x6e,  /* [597] */
    (xdc_Char)0x74,  /* [598] */
    (xdc_Char)0x49,  /* [599] */
    (xdc_Char)0x6e,  /* [600] */
    (xdc_Char)0x55,  /* [601] */
    (xdc_Char)0x73,  /* [602] */
    (xdc_Char)0x65,  /* [603] */
    (xdc_Char)0x3a,  /* [604] */
    (xdc_Char)0x20,  /* [605] */
    (xdc_Char)0x45,  /* [606] */
    (xdc_Char)0x76,  /* [607] */
    (xdc_Char)0x65,  /* [608] */
    (xdc_Char)0x6e,  /* [609] */
    (xdc_Char)0x74,  /* [610] */
    (xdc_Char)0x20,  /* [611] */
    (xdc_Char)0x6f,  /* [612] */
    (xdc_Char)0x62,  /* [613] */
    (xdc_Char)0x6a,  /* [614] */
    (xdc_Char)0x65,  /* [615] */
    (xdc_Char)0x63,  /* [616] */
    (xdc_Char)0x74,  /* [617] */
    (xdc_Char)0x20,  /* [618] */
    (xdc_Char)0x61,  /* [619] */
    (xdc_Char)0x6c,  /* [620] */
    (xdc_Char)0x72,  /* [621] */
    (xdc_Char)0x65,  /* [622] */
    (xdc_Char)0x61,  /* [623] */
    (xdc_Char)0x64,  /* [624] */
    (xdc_Char)0x79,  /* [625] */
    (xdc_Char)0x20,  /* [626] */
    (xdc_Char)0x69,  /* [627] */
    (xdc_Char)0x6e,  /* [628] */
    (xdc_Char)0x20,  /* [629] */
    (xdc_Char)0x75,  /* [630] */
    (xdc_Char)0x73,  /* [631] */
    (xdc_Char)0x65,  /* [632] */
    (xdc_Char)0x2e,  /* [633] */
    (xdc_Char)0x0,  /* [634] */
    (xdc_Char)0x41,  /* [635] */
    (xdc_Char)0x5f,  /* [636] */
    (xdc_Char)0x62,  /* [637] */
    (xdc_Char)0x61,  /* [638] */
    (xdc_Char)0x64,  /* [639] */
    (xdc_Char)0x43,  /* [640] */
    (xdc_Char)0x6f,  /* [641] */
    (xdc_Char)0x6e,  /* [642] */
    (xdc_Char)0x74,  /* [643] */
    (xdc_Char)0x65,  /* [644] */
    (xdc_Char)0x78,  /* [645] */
    (xdc_Char)0x74,  /* [646] */
    (xdc_Char)0x3a,  /* [647] */
    (xdc_Char)0x20,  /* [648] */
    (xdc_Char)0x62,  /* [649] */
    (xdc_Char)0x61,  /* [650] */
    (xdc_Char)0x64,  /* [651] */
    (xdc_Char)0x20,  /* [652] */
    (xdc_Char)0x63,  /* [653] */
    (xdc_Char)0x61,  /* [654] */
    (xdc_Char)0x6c,  /* [655] */
    (xdc_Char)0x6c,  /* [656] */
    (xdc_Char)0x69,  /* [657] */
    (xdc_Char)0x6e,  /* [658] */
    (xdc_Char)0x67,  /* [659] */
    (xdc_Char)0x20,  /* [660] */
    (xdc_Char)0x63,  /* [661] */
    (xdc_Char)0x6f,  /* [662] */
    (xdc_Char)0x6e,  /* [663] */
    (xdc_Char)0x74,  /* [664] */
    (xdc_Char)0x65,  /* [665] */
    (xdc_Char)0x78,  /* [666] */
    (xdc_Char)0x74,  /* [667] */
    (xdc_Char)0x2e,  /* [668] */
    (xdc_Char)0x20,  /* [669] */
    (xdc_Char)0x4d,  /* [670] */
    (xdc_Char)0x75,  /* [671] */
    (xdc_Char)0x73,  /* [672] */
    (xdc_Char)0x74,  /* [673] */
    (xdc_Char)0x20,  /* [674] */
    (xdc_Char)0x62,  /* [675] */
    (xdc_Char)0x65,  /* [676] */
    (xdc_Char)0x20,  /* [677] */
    (xdc_Char)0x63,  /* [678] */
    (xdc_Char)0x61,  /* [679] */
    (xdc_Char)0x6c,  /* [680] */
    (xdc_Char)0x6c,  /* [681] */
    (xdc_Char)0x65,  /* [682] */
    (xdc_Char)0x64,  /* [683] */
    (xdc_Char)0x20,  /* [684] */
    (xdc_Char)0x66,  /* [685] */
    (xdc_Char)0x72,  /* [686] */
    (xdc_Char)0x6f,  /* [687] */
    (xdc_Char)0x6d,  /* [688] */
    (xdc_Char)0x20,  /* [689] */
    (xdc_Char)0x61,  /* [690] */
    (xdc_Char)0x20,  /* [691] */
    (xdc_Char)0x54,  /* [692] */
    (xdc_Char)0x61,  /* [693] */
    (xdc_Char)0x73,  /* [694] */
    (xdc_Char)0x6b,  /* [695] */
    (xdc_Char)0x2e,  /* [696] */
    (xdc_Char)0x0,  /* [697] */
    (xdc_Char)0x4d,  /* [698] */
    (xdc_Char)0x61,  /* [699] */
    (xdc_Char)0x69,  /* [700] */
    (xdc_Char)0x6c,  /* [701] */
    (xdc_Char)0x62,  /* [702] */
    (xdc_Char)0x6f,  /* [703] */
    (xdc_Char)0x78,  /* [704] */
    (xdc_Char)0x5f,  /* [705] */
    (xdc_Char)0x63,  /* [706] */
    (xdc_Char)0x72,  /* [707] */
    (xdc_Char)0x65,  /* [708] */
    (xdc_Char)0x61,  /* [709] */
    (xdc_Char)0x74,  /* [710] */
    (xdc_Char)0x65,  /* [711] */
    (xdc_Char)0x27,  /* [712] */
    (xdc_Char)0x73,  /* [713] */
    (xdc_Char)0x20,  /* [714] */
    (xdc_Char)0x62,  /* [715] */
    (xdc_Char)0x75,  /* [716] */
    (xdc_Char)0x66,  /* [717] */
    (xdc_Char)0x53,  /* [718] */
    (xdc_Char)0x69,  /* [719] */
    (xdc_Char)0x7a,  /* [720] */
    (xdc_Char)0x65,  /* [721] */
    (xdc_Char)0x20,  /* [722] */
    (xdc_Char)0x70,  /* [723] */
    (xdc_Char)0x61,  /* [724] */
    (xdc_Char)0x72,  /* [725] */
    (xdc_Char)0x61,  /* [726] */
    (xdc_Char)0x6d,  /* [727] */
    (xdc_Char)0x65,  /* [728] */
    (xdc_Char)0x74,  /* [729] */
    (xdc_Char)0x65,  /* [730] */
    (xdc_Char)0x72,  /* [731] */
    (xdc_Char)0x20,  /* [732] */
    (xdc_Char)0x69,  /* [733] */
    (xdc_Char)0x73,  /* [734] */
    (xdc_Char)0x20,  /* [735] */
    (xdc_Char)0x69,  /* [736] */
    (xdc_Char)0x6e,  /* [737] */
    (xdc_Char)0x76,  /* [738] */
    (xdc_Char)0x61,  /* [739] */
    (xdc_Char)0x6c,  /* [740] */
    (xdc_Char)0x69,  /* [741] */
    (xdc_Char)0x64,  /* [742] */
    (xdc_Char)0x20,  /* [743] */
    (xdc_Char)0x28,  /* [744] */
    (xdc_Char)0x74,  /* [745] */
    (xdc_Char)0x6f,  /* [746] */
    (xdc_Char)0x6f,  /* [747] */
    (xdc_Char)0x20,  /* [748] */
    (xdc_Char)0x73,  /* [749] */
    (xdc_Char)0x6d,  /* [750] */
    (xdc_Char)0x61,  /* [751] */
    (xdc_Char)0x6c,  /* [752] */
    (xdc_Char)0x6c,  /* [753] */
    (xdc_Char)0x29,  /* [754] */
    (xdc_Char)0x0,  /* [755] */
    (xdc_Char)0x41,  /* [756] */
    (xdc_Char)0x5f,  /* [757] */
    (xdc_Char)0x6e,  /* [758] */
    (xdc_Char)0x6f,  /* [759] */
    (xdc_Char)0x45,  /* [760] */
    (xdc_Char)0x76,  /* [761] */
    (xdc_Char)0x65,  /* [762] */
    (xdc_Char)0x6e,  /* [763] */
    (xdc_Char)0x74,  /* [764] */
    (xdc_Char)0x73,  /* [765] */
    (xdc_Char)0x3a,  /* [766] */
    (xdc_Char)0x20,  /* [767] */
    (xdc_Char)0x54,  /* [768] */
    (xdc_Char)0x68,  /* [769] */
    (xdc_Char)0x65,  /* [770] */
    (xdc_Char)0x20,  /* [771] */
    (xdc_Char)0x45,  /* [772] */
    (xdc_Char)0x76,  /* [773] */
    (xdc_Char)0x65,  /* [774] */
    (xdc_Char)0x6e,  /* [775] */
    (xdc_Char)0x74,  /* [776] */
    (xdc_Char)0x2e,  /* [777] */
    (xdc_Char)0x73,  /* [778] */
    (xdc_Char)0x75,  /* [779] */
    (xdc_Char)0x70,  /* [780] */
    (xdc_Char)0x70,  /* [781] */
    (xdc_Char)0x6f,  /* [782] */
    (xdc_Char)0x72,  /* [783] */
    (xdc_Char)0x74,  /* [784] */
    (xdc_Char)0x73,  /* [785] */
    (xdc_Char)0x45,  /* [786] */
    (xdc_Char)0x76,  /* [787] */
    (xdc_Char)0x65,  /* [788] */
    (xdc_Char)0x6e,  /* [789] */
    (xdc_Char)0x74,  /* [790] */
    (xdc_Char)0x73,  /* [791] */
    (xdc_Char)0x20,  /* [792] */
    (xdc_Char)0x66,  /* [793] */
    (xdc_Char)0x6c,  /* [794] */
    (xdc_Char)0x61,  /* [795] */
    (xdc_Char)0x67,  /* [796] */
    (xdc_Char)0x20,  /* [797] */
    (xdc_Char)0x69,  /* [798] */
    (xdc_Char)0x73,  /* [799] */
    (xdc_Char)0x20,  /* [800] */
    (xdc_Char)0x64,  /* [801] */
    (xdc_Char)0x69,  /* [802] */
    (xdc_Char)0x73,  /* [803] */
    (xdc_Char)0x61,  /* [804] */
    (xdc_Char)0x62,  /* [805] */
    (xdc_Char)0x6c,  /* [806] */
    (xdc_Char)0x65,  /* [807] */
    (xdc_Char)0x64,  /* [808] */
    (xdc_Char)0x2e,  /* [809] */
    (xdc_Char)0x0,  /* [810] */
    (xdc_Char)0x41,  /* [811] */
    (xdc_Char)0x5f,  /* [812] */
    (xdc_Char)0x69,  /* [813] */
    (xdc_Char)0x6e,  /* [814] */
    (xdc_Char)0x76,  /* [815] */
    (xdc_Char)0x54,  /* [816] */
    (xdc_Char)0x69,  /* [817] */
    (xdc_Char)0x6d,  /* [818] */
    (xdc_Char)0x65,  /* [819] */
    (xdc_Char)0x6f,  /* [820] */
    (xdc_Char)0x75,  /* [821] */
    (xdc_Char)0x74,  /* [822] */
    (xdc_Char)0x3a,  /* [823] */
    (xdc_Char)0x20,  /* [824] */
    (xdc_Char)0x43,  /* [825] */
    (xdc_Char)0x61,  /* [826] */
    (xdc_Char)0x6e,  /* [827] */
    (xdc_Char)0x27,  /* [828] */
    (xdc_Char)0x74,  /* [829] */
    (xdc_Char)0x20,  /* [830] */
    (xdc_Char)0x75,  /* [831] */
    (xdc_Char)0x73,  /* [832] */
    (xdc_Char)0x65,  /* [833] */
    (xdc_Char)0x20,  /* [834] */
    (xdc_Char)0x42,  /* [835] */
    (xdc_Char)0x49,  /* [836] */
    (xdc_Char)0x4f,  /* [837] */
    (xdc_Char)0x53,  /* [838] */
    (xdc_Char)0x5f,  /* [839] */
    (xdc_Char)0x45,  /* [840] */
    (xdc_Char)0x56,  /* [841] */
    (xdc_Char)0x45,  /* [842] */
    (xdc_Char)0x4e,  /* [843] */
    (xdc_Char)0x54,  /* [844] */
    (xdc_Char)0x5f,  /* [845] */
    (xdc_Char)0x41,  /* [846] */
    (xdc_Char)0x43,  /* [847] */
    (xdc_Char)0x51,  /* [848] */
    (xdc_Char)0x55,  /* [849] */
    (xdc_Char)0x49,  /* [850] */
    (xdc_Char)0x52,  /* [851] */
    (xdc_Char)0x45,  /* [852] */
    (xdc_Char)0x44,  /* [853] */
    (xdc_Char)0x20,  /* [854] */
    (xdc_Char)0x77,  /* [855] */
    (xdc_Char)0x69,  /* [856] */
    (xdc_Char)0x74,  /* [857] */
    (xdc_Char)0x68,  /* [858] */
    (xdc_Char)0x20,  /* [859] */
    (xdc_Char)0x74,  /* [860] */
    (xdc_Char)0x68,  /* [861] */
    (xdc_Char)0x69,  /* [862] */
    (xdc_Char)0x73,  /* [863] */
    (xdc_Char)0x20,  /* [864] */
    (xdc_Char)0x53,  /* [865] */
    (xdc_Char)0x65,  /* [866] */
    (xdc_Char)0x6d,  /* [867] */
    (xdc_Char)0x61,  /* [868] */
    (xdc_Char)0x70,  /* [869] */
    (xdc_Char)0x68,  /* [870] */
    (xdc_Char)0x6f,  /* [871] */
    (xdc_Char)0x72,  /* [872] */
    (xdc_Char)0x65,  /* [873] */
    (xdc_Char)0x2e,  /* [874] */
    (xdc_Char)0x0,  /* [875] */
    (xdc_Char)0x41,  /* [876] */
    (xdc_Char)0x5f,  /* [877] */
    (xdc_Char)0x73,  /* [878] */
    (xdc_Char)0x77,  /* [879] */
    (xdc_Char)0x69,  /* [880] */
    (xdc_Char)0x44,  /* [881] */
    (xdc_Char)0x69,  /* [882] */
    (xdc_Char)0x73,  /* [883] */
    (xdc_Char)0x61,  /* [884] */
    (xdc_Char)0x62,  /* [885] */
    (xdc_Char)0x6c,  /* [886] */
    (xdc_Char)0x65,  /* [887] */
    (xdc_Char)0x64,  /* [888] */
    (xdc_Char)0x3a,  /* [889] */
    (xdc_Char)0x20,  /* [890] */
    (xdc_Char)0x43,  /* [891] */
    (xdc_Char)0x61,  /* [892] */
    (xdc_Char)0x6e,  /* [893] */
    (xdc_Char)0x6e,  /* [894] */
    (xdc_Char)0x6f,  /* [895] */
    (xdc_Char)0x74,  /* [896] */
    (xdc_Char)0x20,  /* [897] */
    (xdc_Char)0x63,  /* [898] */
    (xdc_Char)0x72,  /* [899] */
    (xdc_Char)0x65,  /* [900] */
    (xdc_Char)0x61,  /* [901] */
    (xdc_Char)0x74,  /* [902] */
    (xdc_Char)0x65,  /* [903] */
    (xdc_Char)0x20,  /* [904] */
    (xdc_Char)0x61,  /* [905] */
    (xdc_Char)0x20,  /* [906] */
    (xdc_Char)0x53,  /* [907] */
    (xdc_Char)0x77,  /* [908] */
    (xdc_Char)0x69,  /* [909] */
    (xdc_Char)0x20,  /* [910] */
    (xdc_Char)0x77,  /* [911] */
    (xdc_Char)0x68,  /* [912] */
    (xdc_Char)0x65,  /* [913] */
    (xdc_Char)0x6e,  /* [914] */
    (xdc_Char)0x20,  /* [915] */
    (xdc_Char)0x53,  /* [916] */
    (xdc_Char)0x77,  /* [917] */
    (xdc_Char)0x69,  /* [918] */
    (xdc_Char)0x20,  /* [919] */
    (xdc_Char)0x69,  /* [920] */
    (xdc_Char)0x73,  /* [921] */
    (xdc_Char)0x20,  /* [922] */
    (xdc_Char)0x64,  /* [923] */
    (xdc_Char)0x69,  /* [924] */
    (xdc_Char)0x73,  /* [925] */
    (xdc_Char)0x61,  /* [926] */
    (xdc_Char)0x62,  /* [927] */
    (xdc_Char)0x6c,  /* [928] */
    (xdc_Char)0x65,  /* [929] */
    (xdc_Char)0x64,  /* [930] */
    (xdc_Char)0x2e,  /* [931] */
    (xdc_Char)0x0,  /* [932] */
    (xdc_Char)0x41,  /* [933] */
    (xdc_Char)0x5f,  /* [934] */
    (xdc_Char)0x62,  /* [935] */
    (xdc_Char)0x61,  /* [936] */
    (xdc_Char)0x64,  /* [937] */
    (xdc_Char)0x50,  /* [938] */
    (xdc_Char)0x72,  /* [939] */
    (xdc_Char)0x69,  /* [940] */
    (xdc_Char)0x6f,  /* [941] */
    (xdc_Char)0x72,  /* [942] */
    (xdc_Char)0x69,  /* [943] */
    (xdc_Char)0x74,  /* [944] */
    (xdc_Char)0x79,  /* [945] */
    (xdc_Char)0x3a,  /* [946] */
    (xdc_Char)0x20,  /* [947] */
    (xdc_Char)0x41,  /* [948] */
    (xdc_Char)0x6e,  /* [949] */
    (xdc_Char)0x20,  /* [950] */
    (xdc_Char)0x69,  /* [951] */
    (xdc_Char)0x6e,  /* [952] */
    (xdc_Char)0x76,  /* [953] */
    (xdc_Char)0x61,  /* [954] */
    (xdc_Char)0x6c,  /* [955] */
    (xdc_Char)0x69,  /* [956] */
    (xdc_Char)0x64,  /* [957] */
    (xdc_Char)0x20,  /* [958] */
    (xdc_Char)0x53,  /* [959] */
    (xdc_Char)0x77,  /* [960] */
    (xdc_Char)0x69,  /* [961] */
    (xdc_Char)0x20,  /* [962] */
    (xdc_Char)0x70,  /* [963] */
    (xdc_Char)0x72,  /* [964] */
    (xdc_Char)0x69,  /* [965] */
    (xdc_Char)0x6f,  /* [966] */
    (xdc_Char)0x72,  /* [967] */
    (xdc_Char)0x69,  /* [968] */
    (xdc_Char)0x74,  /* [969] */
    (xdc_Char)0x79,  /* [970] */
    (xdc_Char)0x20,  /* [971] */
    (xdc_Char)0x77,  /* [972] */
    (xdc_Char)0x61,  /* [973] */
    (xdc_Char)0x73,  /* [974] */
    (xdc_Char)0x20,  /* [975] */
    (xdc_Char)0x75,  /* [976] */
    (xdc_Char)0x73,  /* [977] */
    (xdc_Char)0x65,  /* [978] */
    (xdc_Char)0x64,  /* [979] */
    (xdc_Char)0x2e,  /* [980] */
    (xdc_Char)0x0,  /* [981] */
    (xdc_Char)0x41,  /* [982] */
    (xdc_Char)0x5f,  /* [983] */
    (xdc_Char)0x62,  /* [984] */
    (xdc_Char)0x61,  /* [985] */
    (xdc_Char)0x64,  /* [986] */
    (xdc_Char)0x54,  /* [987] */
    (xdc_Char)0x68,  /* [988] */
    (xdc_Char)0x72,  /* [989] */
    (xdc_Char)0x65,  /* [990] */
    (xdc_Char)0x61,  /* [991] */
    (xdc_Char)0x64,  /* [992] */
    (xdc_Char)0x54,  /* [993] */
    (xdc_Char)0x79,  /* [994] */
    (xdc_Char)0x70,  /* [995] */
    (xdc_Char)0x65,  /* [996] */
    (xdc_Char)0x3a,  /* [997] */
    (xdc_Char)0x20,  /* [998] */
    (xdc_Char)0x43,  /* [999] */
    (xdc_Char)0x61,  /* [1000] */
    (xdc_Char)0x6e,  /* [1001] */
    (xdc_Char)0x6e,  /* [1002] */
    (xdc_Char)0x6f,  /* [1003] */
    (xdc_Char)0x74,  /* [1004] */
    (xdc_Char)0x20,  /* [1005] */
    (xdc_Char)0x63,  /* [1006] */
    (xdc_Char)0x72,  /* [1007] */
    (xdc_Char)0x65,  /* [1008] */
    (xdc_Char)0x61,  /* [1009] */
    (xdc_Char)0x74,  /* [1010] */
    (xdc_Char)0x65,  /* [1011] */
    (xdc_Char)0x2f,  /* [1012] */
    (xdc_Char)0x64,  /* [1013] */
    (xdc_Char)0x65,  /* [1014] */
    (xdc_Char)0x6c,  /* [1015] */
    (xdc_Char)0x65,  /* [1016] */
    (xdc_Char)0x74,  /* [1017] */
    (xdc_Char)0x65,  /* [1018] */
    (xdc_Char)0x20,  /* [1019] */
    (xdc_Char)0x61,  /* [1020] */
    (xdc_Char)0x20,  /* [1021] */
    (xdc_Char)0x74,  /* [1022] */
    (xdc_Char)0x61,  /* [1023] */
    (xdc_Char)0x73,  /* [1024] */
    (xdc_Char)0x6b,  /* [1025] */
    (xdc_Char)0x20,  /* [1026] */
    (xdc_Char)0x66,  /* [1027] */
    (xdc_Char)0x72,  /* [1028] */
    (xdc_Char)0x6f,  /* [1029] */
    (xdc_Char)0x6d,  /* [1030] */
    (xdc_Char)0x20,  /* [1031] */
    (xdc_Char)0x48,  /* [1032] */
    (xdc_Char)0x77,  /* [1033] */
    (xdc_Char)0x69,  /* [1034] */
    (xdc_Char)0x20,  /* [1035] */
    (xdc_Char)0x6f,  /* [1036] */
    (xdc_Char)0x72,  /* [1037] */
    (xdc_Char)0x20,  /* [1038] */
    (xdc_Char)0x53,  /* [1039] */
    (xdc_Char)0x77,  /* [1040] */
    (xdc_Char)0x69,  /* [1041] */
    (xdc_Char)0x20,  /* [1042] */
    (xdc_Char)0x74,  /* [1043] */
    (xdc_Char)0x68,  /* [1044] */
    (xdc_Char)0x72,  /* [1045] */
    (xdc_Char)0x65,  /* [1046] */
    (xdc_Char)0x61,  /* [1047] */
    (xdc_Char)0x64,  /* [1048] */
    (xdc_Char)0x2e,  /* [1049] */
    (xdc_Char)0x0,  /* [1050] */
    (xdc_Char)0x41,  /* [1051] */
    (xdc_Char)0x5f,  /* [1052] */
    (xdc_Char)0x62,  /* [1053] */
    (xdc_Char)0x61,  /* [1054] */
    (xdc_Char)0x64,  /* [1055] */
    (xdc_Char)0x54,  /* [1056] */
    (xdc_Char)0x61,  /* [1057] */
    (xdc_Char)0x73,  /* [1058] */
    (xdc_Char)0x6b,  /* [1059] */
    (xdc_Char)0x53,  /* [1060] */
    (xdc_Char)0x74,  /* [1061] */
    (xdc_Char)0x61,  /* [1062] */
    (xdc_Char)0x74,  /* [1063] */
    (xdc_Char)0x65,  /* [1064] */
    (xdc_Char)0x3a,  /* [1065] */
    (xdc_Char)0x20,  /* [1066] */
    (xdc_Char)0x43,  /* [1067] */
    (xdc_Char)0x61,  /* [1068] */
    (xdc_Char)0x6e,  /* [1069] */
    (xdc_Char)0x27,  /* [1070] */
    (xdc_Char)0x74,  /* [1071] */
    (xdc_Char)0x20,  /* [1072] */
    (xdc_Char)0x64,  /* [1073] */
    (xdc_Char)0x65,  /* [1074] */
    (xdc_Char)0x6c,  /* [1075] */
    (xdc_Char)0x65,  /* [1076] */
    (xdc_Char)0x74,  /* [1077] */
    (xdc_Char)0x65,  /* [1078] */
    (xdc_Char)0x20,  /* [1079] */
    (xdc_Char)0x61,  /* [1080] */
    (xdc_Char)0x20,  /* [1081] */
    (xdc_Char)0x74,  /* [1082] */
    (xdc_Char)0x61,  /* [1083] */
    (xdc_Char)0x73,  /* [1084] */
    (xdc_Char)0x6b,  /* [1085] */
    (xdc_Char)0x20,  /* [1086] */
    (xdc_Char)0x69,  /* [1087] */
    (xdc_Char)0x6e,  /* [1088] */
    (xdc_Char)0x20,  /* [1089] */
    (xdc_Char)0x52,  /* [1090] */
    (xdc_Char)0x55,  /* [1091] */
    (xdc_Char)0x4e,  /* [1092] */
    (xdc_Char)0x4e,  /* [1093] */
    (xdc_Char)0x49,  /* [1094] */
    (xdc_Char)0x4e,  /* [1095] */
    (xdc_Char)0x47,  /* [1096] */
    (xdc_Char)0x20,  /* [1097] */
    (xdc_Char)0x73,  /* [1098] */
    (xdc_Char)0x74,  /* [1099] */
    (xdc_Char)0x61,  /* [1100] */
    (xdc_Char)0x74,  /* [1101] */
    (xdc_Char)0x65,  /* [1102] */
    (xdc_Char)0x2e,  /* [1103] */
    (xdc_Char)0x0,  /* [1104] */
    (xdc_Char)0x41,  /* [1105] */
    (xdc_Char)0x5f,  /* [1106] */
    (xdc_Char)0x6e,  /* [1107] */
    (xdc_Char)0x6f,  /* [1108] */
    (xdc_Char)0x50,  /* [1109] */
    (xdc_Char)0x65,  /* [1110] */
    (xdc_Char)0x6e,  /* [1111] */
    (xdc_Char)0x64,  /* [1112] */
    (xdc_Char)0x45,  /* [1113] */
    (xdc_Char)0x6c,  /* [1114] */
    (xdc_Char)0x65,  /* [1115] */
    (xdc_Char)0x6d,  /* [1116] */
    (xdc_Char)0x3a,  /* [1117] */
    (xdc_Char)0x20,  /* [1118] */
    (xdc_Char)0x4e,  /* [1119] */
    (xdc_Char)0x6f,  /* [1120] */
    (xdc_Char)0x74,  /* [1121] */
    (xdc_Char)0x20,  /* [1122] */
    (xdc_Char)0x65,  /* [1123] */
    (xdc_Char)0x6e,  /* [1124] */
    (xdc_Char)0x6f,  /* [1125] */
    (xdc_Char)0x75,  /* [1126] */
    (xdc_Char)0x67,  /* [1127] */
    (xdc_Char)0x68,  /* [1128] */
    (xdc_Char)0x20,  /* [1129] */
    (xdc_Char)0x69,  /* [1130] */
    (xdc_Char)0x6e,  /* [1131] */
    (xdc_Char)0x66,  /* [1132] */
    (xdc_Char)0x6f,  /* [1133] */
    (xdc_Char)0x20,  /* [1134] */
    (xdc_Char)0x74,  /* [1135] */
    (xdc_Char)0x6f,  /* [1136] */
    (xdc_Char)0x20,  /* [1137] */
    (xdc_Char)0x64,  /* [1138] */
    (xdc_Char)0x65,  /* [1139] */
    (xdc_Char)0x6c,  /* [1140] */
    (xdc_Char)0x65,  /* [1141] */
    (xdc_Char)0x74,  /* [1142] */
    (xdc_Char)0x65,  /* [1143] */
    (xdc_Char)0x20,  /* [1144] */
    (xdc_Char)0x42,  /* [1145] */
    (xdc_Char)0x4c,  /* [1146] */
    (xdc_Char)0x4f,  /* [1147] */
    (xdc_Char)0x43,  /* [1148] */
    (xdc_Char)0x4b,  /* [1149] */
    (xdc_Char)0x45,  /* [1150] */
    (xdc_Char)0x44,  /* [1151] */
    (xdc_Char)0x20,  /* [1152] */
    (xdc_Char)0x74,  /* [1153] */
    (xdc_Char)0x61,  /* [1154] */
    (xdc_Char)0x73,  /* [1155] */
    (xdc_Char)0x6b,  /* [1156] */
    (xdc_Char)0x2e,  /* [1157] */
    (xdc_Char)0x0,  /* [1158] */
    (xdc_Char)0x41,  /* [1159] */
    (xdc_Char)0x5f,  /* [1160] */
    (xdc_Char)0x74,  /* [1161] */
    (xdc_Char)0x61,  /* [1162] */
    (xdc_Char)0x73,  /* [1163] */
    (xdc_Char)0x6b,  /* [1164] */
    (xdc_Char)0x44,  /* [1165] */
    (xdc_Char)0x69,  /* [1166] */
    (xdc_Char)0x73,  /* [1167] */
    (xdc_Char)0x61,  /* [1168] */
    (xdc_Char)0x62,  /* [1169] */
    (xdc_Char)0x6c,  /* [1170] */
    (xdc_Char)0x65,  /* [1171] */
    (xdc_Char)0x64,  /* [1172] */
    (xdc_Char)0x3a,  /* [1173] */
    (xdc_Char)0x20,  /* [1174] */
    (xdc_Char)0x43,  /* [1175] */
    (xdc_Char)0x61,  /* [1176] */
    (xdc_Char)0x6e,  /* [1177] */
    (xdc_Char)0x6e,  /* [1178] */
    (xdc_Char)0x6f,  /* [1179] */
    (xdc_Char)0x74,  /* [1180] */
    (xdc_Char)0x20,  /* [1181] */
    (xdc_Char)0x63,  /* [1182] */
    (xdc_Char)0x72,  /* [1183] */
    (xdc_Char)0x65,  /* [1184] */
    (xdc_Char)0x61,  /* [1185] */
    (xdc_Char)0x74,  /* [1186] */
    (xdc_Char)0x65,  /* [1187] */
    (xdc_Char)0x20,  /* [1188] */
    (xdc_Char)0x61,  /* [1189] */
    (xdc_Char)0x20,  /* [1190] */
    (xdc_Char)0x74,  /* [1191] */
    (xdc_Char)0x61,  /* [1192] */
    (xdc_Char)0x73,  /* [1193] */
    (xdc_Char)0x6b,  /* [1194] */
    (xdc_Char)0x20,  /* [1195] */
    (xdc_Char)0x77,  /* [1196] */
    (xdc_Char)0x68,  /* [1197] */
    (xdc_Char)0x65,  /* [1198] */
    (xdc_Char)0x6e,  /* [1199] */
    (xdc_Char)0x20,  /* [1200] */
    (xdc_Char)0x74,  /* [1201] */
    (xdc_Char)0x61,  /* [1202] */
    (xdc_Char)0x73,  /* [1203] */
    (xdc_Char)0x6b,  /* [1204] */
    (xdc_Char)0x69,  /* [1205] */
    (xdc_Char)0x6e,  /* [1206] */
    (xdc_Char)0x67,  /* [1207] */
    (xdc_Char)0x20,  /* [1208] */
    (xdc_Char)0x69,  /* [1209] */
    (xdc_Char)0x73,  /* [1210] */
    (xdc_Char)0x20,  /* [1211] */
    (xdc_Char)0x64,  /* [1212] */
    (xdc_Char)0x69,  /* [1213] */
    (xdc_Char)0x73,  /* [1214] */
    (xdc_Char)0x61,  /* [1215] */
    (xdc_Char)0x62,  /* [1216] */
    (xdc_Char)0x6c,  /* [1217] */
    (xdc_Char)0x65,  /* [1218] */
    (xdc_Char)0x64,  /* [1219] */
    (xdc_Char)0x2e,  /* [1220] */
    (xdc_Char)0x0,  /* [1221] */
    (xdc_Char)0x41,  /* [1222] */
    (xdc_Char)0x5f,  /* [1223] */
    (xdc_Char)0x62,  /* [1224] */
    (xdc_Char)0x61,  /* [1225] */
    (xdc_Char)0x64,  /* [1226] */
    (xdc_Char)0x50,  /* [1227] */
    (xdc_Char)0x72,  /* [1228] */
    (xdc_Char)0x69,  /* [1229] */
    (xdc_Char)0x6f,  /* [1230] */
    (xdc_Char)0x72,  /* [1231] */
    (xdc_Char)0x69,  /* [1232] */
    (xdc_Char)0x74,  /* [1233] */
    (xdc_Char)0x79,  /* [1234] */
    (xdc_Char)0x3a,  /* [1235] */
    (xdc_Char)0x20,  /* [1236] */
    (xdc_Char)0x41,  /* [1237] */
    (xdc_Char)0x6e,  /* [1238] */
    (xdc_Char)0x20,  /* [1239] */
    (xdc_Char)0x69,  /* [1240] */
    (xdc_Char)0x6e,  /* [1241] */
    (xdc_Char)0x76,  /* [1242] */
    (xdc_Char)0x61,  /* [1243] */
    (xdc_Char)0x6c,  /* [1244] */
    (xdc_Char)0x69,  /* [1245] */
    (xdc_Char)0x64,  /* [1246] */
    (xdc_Char)0x20,  /* [1247] */
    (xdc_Char)0x74,  /* [1248] */
    (xdc_Char)0x61,  /* [1249] */
    (xdc_Char)0x73,  /* [1250] */
    (xdc_Char)0x6b,  /* [1251] */
    (xdc_Char)0x20,  /* [1252] */
    (xdc_Char)0x70,  /* [1253] */
    (xdc_Char)0x72,  /* [1254] */
    (xdc_Char)0x69,  /* [1255] */
    (xdc_Char)0x6f,  /* [1256] */
    (xdc_Char)0x72,  /* [1257] */
    (xdc_Char)0x69,  /* [1258] */
    (xdc_Char)0x74,  /* [1259] */
    (xdc_Char)0x79,  /* [1260] */
    (xdc_Char)0x20,  /* [1261] */
    (xdc_Char)0x77,  /* [1262] */
    (xdc_Char)0x61,  /* [1263] */
    (xdc_Char)0x73,  /* [1264] */
    (xdc_Char)0x20,  /* [1265] */
    (xdc_Char)0x75,  /* [1266] */
    (xdc_Char)0x73,  /* [1267] */
    (xdc_Char)0x65,  /* [1268] */
    (xdc_Char)0x64,  /* [1269] */
    (xdc_Char)0x2e,  /* [1270] */
    (xdc_Char)0x0,  /* [1271] */
    (xdc_Char)0x41,  /* [1272] */
    (xdc_Char)0x5f,  /* [1273] */
    (xdc_Char)0x62,  /* [1274] */
    (xdc_Char)0x61,  /* [1275] */
    (xdc_Char)0x64,  /* [1276] */
    (xdc_Char)0x54,  /* [1277] */
    (xdc_Char)0x69,  /* [1278] */
    (xdc_Char)0x6d,  /* [1279] */
    (xdc_Char)0x65,  /* [1280] */
    (xdc_Char)0x6f,  /* [1281] */
    (xdc_Char)0x75,  /* [1282] */
    (xdc_Char)0x74,  /* [1283] */
    (xdc_Char)0x3a,  /* [1284] */
    (xdc_Char)0x20,  /* [1285] */
    (xdc_Char)0x43,  /* [1286] */
    (xdc_Char)0x61,  /* [1287] */
    (xdc_Char)0x6e,  /* [1288] */
    (xdc_Char)0x27,  /* [1289] */
    (xdc_Char)0x74,  /* [1290] */
    (xdc_Char)0x20,  /* [1291] */
    (xdc_Char)0x73,  /* [1292] */
    (xdc_Char)0x6c,  /* [1293] */
    (xdc_Char)0x65,  /* [1294] */
    (xdc_Char)0x65,  /* [1295] */
    (xdc_Char)0x70,  /* [1296] */
    (xdc_Char)0x20,  /* [1297] */
    (xdc_Char)0x46,  /* [1298] */
    (xdc_Char)0x4f,  /* [1299] */
    (xdc_Char)0x52,  /* [1300] */
    (xdc_Char)0x45,  /* [1301] */
    (xdc_Char)0x56,  /* [1302] */
    (xdc_Char)0x45,  /* [1303] */
    (xdc_Char)0x52,  /* [1304] */
    (xdc_Char)0x2e,  /* [1305] */
    (xdc_Char)0x0,  /* [1306] */
    (xdc_Char)0x41,  /* [1307] */
    (xdc_Char)0x5f,  /* [1308] */
    (xdc_Char)0x75,  /* [1309] */
    (xdc_Char)0x6e,  /* [1310] */
    (xdc_Char)0x73,  /* [1311] */
    (xdc_Char)0x75,  /* [1312] */
    (xdc_Char)0x70,  /* [1313] */
    (xdc_Char)0x70,  /* [1314] */
    (xdc_Char)0x6f,  /* [1315] */
    (xdc_Char)0x72,  /* [1316] */
    (xdc_Char)0x74,  /* [1317] */
    (xdc_Char)0x65,  /* [1318] */
    (xdc_Char)0x64,  /* [1319] */
    (xdc_Char)0x4d,  /* [1320] */
    (xdc_Char)0x61,  /* [1321] */
    (xdc_Char)0x73,  /* [1322] */
    (xdc_Char)0x6b,  /* [1323] */
    (xdc_Char)0x69,  /* [1324] */
    (xdc_Char)0x6e,  /* [1325] */
    (xdc_Char)0x67,  /* [1326] */
    (xdc_Char)0x4f,  /* [1327] */
    (xdc_Char)0x70,  /* [1328] */
    (xdc_Char)0x74,  /* [1329] */
    (xdc_Char)0x69,  /* [1330] */
    (xdc_Char)0x6f,  /* [1331] */
    (xdc_Char)0x6e,  /* [1332] */
    (xdc_Char)0x3a,  /* [1333] */
    (xdc_Char)0x20,  /* [1334] */
    (xdc_Char)0x75,  /* [1335] */
    (xdc_Char)0x6e,  /* [1336] */
    (xdc_Char)0x73,  /* [1337] */
    (xdc_Char)0x75,  /* [1338] */
    (xdc_Char)0x70,  /* [1339] */
    (xdc_Char)0x70,  /* [1340] */
    (xdc_Char)0x6f,  /* [1341] */
    (xdc_Char)0x72,  /* [1342] */
    (xdc_Char)0x74,  /* [1343] */
    (xdc_Char)0x65,  /* [1344] */
    (xdc_Char)0x64,  /* [1345] */
    (xdc_Char)0x20,  /* [1346] */
    (xdc_Char)0x6d,  /* [1347] */
    (xdc_Char)0x61,  /* [1348] */
    (xdc_Char)0x73,  /* [1349] */
    (xdc_Char)0x6b,  /* [1350] */
    (xdc_Char)0x53,  /* [1351] */
    (xdc_Char)0x65,  /* [1352] */
    (xdc_Char)0x74,  /* [1353] */
    (xdc_Char)0x74,  /* [1354] */
    (xdc_Char)0x69,  /* [1355] */
    (xdc_Char)0x6e,  /* [1356] */
    (xdc_Char)0x67,  /* [1357] */
    (xdc_Char)0x2e,  /* [1358] */
    (xdc_Char)0x0,  /* [1359] */
    (xdc_Char)0x41,  /* [1360] */
    (xdc_Char)0x5f,  /* [1361] */
    (xdc_Char)0x6e,  /* [1362] */
    (xdc_Char)0x65,  /* [1363] */
    (xdc_Char)0x73,  /* [1364] */
    (xdc_Char)0x74,  /* [1365] */
    (xdc_Char)0x65,  /* [1366] */
    (xdc_Char)0x64,  /* [1367] */
    (xdc_Char)0x45,  /* [1368] */
    (xdc_Char)0x6e,  /* [1369] */
    (xdc_Char)0x74,  /* [1370] */
    (xdc_Char)0x65,  /* [1371] */
    (xdc_Char)0x72,  /* [1372] */
    (xdc_Char)0x3a,  /* [1373] */
    (xdc_Char)0x20,  /* [1374] */
    (xdc_Char)0x43,  /* [1375] */
    (xdc_Char)0x61,  /* [1376] */
    (xdc_Char)0x6e,  /* [1377] */
    (xdc_Char)0x27,  /* [1378] */
    (xdc_Char)0x74,  /* [1379] */
    (xdc_Char)0x20,  /* [1380] */
    (xdc_Char)0x6e,  /* [1381] */
    (xdc_Char)0x65,  /* [1382] */
    (xdc_Char)0x73,  /* [1383] */
    (xdc_Char)0x74,  /* [1384] */
    (xdc_Char)0x20,  /* [1385] */
    (xdc_Char)0x27,  /* [1386] */
    (xdc_Char)0x65,  /* [1387] */
    (xdc_Char)0x6e,  /* [1388] */
    (xdc_Char)0x74,  /* [1389] */
    (xdc_Char)0x65,  /* [1390] */
    (xdc_Char)0x72,  /* [1391] */
    (xdc_Char)0x27,  /* [1392] */
    (xdc_Char)0x20,  /* [1393] */
    (xdc_Char)0x63,  /* [1394] */
    (xdc_Char)0x61,  /* [1395] */
    (xdc_Char)0x6c,  /* [1396] */
    (xdc_Char)0x6c,  /* [1397] */
    (xdc_Char)0x73,  /* [1398] */
    (xdc_Char)0x2e,  /* [1399] */
    (xdc_Char)0x0,  /* [1400] */
    (xdc_Char)0x62,  /* [1401] */
    (xdc_Char)0x75,  /* [1402] */
    (xdc_Char)0x66,  /* [1403] */
    (xdc_Char)0x20,  /* [1404] */
    (xdc_Char)0x70,  /* [1405] */
    (xdc_Char)0x61,  /* [1406] */
    (xdc_Char)0x72,  /* [1407] */
    (xdc_Char)0x61,  /* [1408] */
    (xdc_Char)0x6d,  /* [1409] */
    (xdc_Char)0x65,  /* [1410] */
    (xdc_Char)0x74,  /* [1411] */
    (xdc_Char)0x65,  /* [1412] */
    (xdc_Char)0x72,  /* [1413] */
    (xdc_Char)0x20,  /* [1414] */
    (xdc_Char)0x63,  /* [1415] */
    (xdc_Char)0x61,  /* [1416] */
    (xdc_Char)0x6e,  /* [1417] */
    (xdc_Char)0x6e,  /* [1418] */
    (xdc_Char)0x6f,  /* [1419] */
    (xdc_Char)0x74,  /* [1420] */
    (xdc_Char)0x20,  /* [1421] */
    (xdc_Char)0x62,  /* [1422] */
    (xdc_Char)0x65,  /* [1423] */
    (xdc_Char)0x20,  /* [1424] */
    (xdc_Char)0x6e,  /* [1425] */
    (xdc_Char)0x75,  /* [1426] */
    (xdc_Char)0x6c,  /* [1427] */
    (xdc_Char)0x6c,  /* [1428] */
    (xdc_Char)0x0,  /* [1429] */
    (xdc_Char)0x62,  /* [1430] */
    (xdc_Char)0x75,  /* [1431] */
    (xdc_Char)0x66,  /* [1432] */
    (xdc_Char)0x20,  /* [1433] */
    (xdc_Char)0x6e,  /* [1434] */
    (xdc_Char)0x6f,  /* [1435] */
    (xdc_Char)0x74,  /* [1436] */
    (xdc_Char)0x20,  /* [1437] */
    (xdc_Char)0x70,  /* [1438] */
    (xdc_Char)0x72,  /* [1439] */
    (xdc_Char)0x6f,  /* [1440] */
    (xdc_Char)0x70,  /* [1441] */
    (xdc_Char)0x65,  /* [1442] */
    (xdc_Char)0x72,  /* [1443] */
    (xdc_Char)0x6c,  /* [1444] */
    (xdc_Char)0x79,  /* [1445] */
    (xdc_Char)0x20,  /* [1446] */
    (xdc_Char)0x61,  /* [1447] */
    (xdc_Char)0x6c,  /* [1448] */
    (xdc_Char)0x69,  /* [1449] */
    (xdc_Char)0x67,  /* [1450] */
    (xdc_Char)0x6e,  /* [1451] */
    (xdc_Char)0x65,  /* [1452] */
    (xdc_Char)0x64,  /* [1453] */
    (xdc_Char)0x0,  /* [1454] */
    (xdc_Char)0x61,  /* [1455] */
    (xdc_Char)0x6c,  /* [1456] */
    (xdc_Char)0x69,  /* [1457] */
    (xdc_Char)0x67,  /* [1458] */
    (xdc_Char)0x6e,  /* [1459] */
    (xdc_Char)0x20,  /* [1460] */
    (xdc_Char)0x70,  /* [1461] */
    (xdc_Char)0x61,  /* [1462] */
    (xdc_Char)0x72,  /* [1463] */
    (xdc_Char)0x61,  /* [1464] */
    (xdc_Char)0x6d,  /* [1465] */
    (xdc_Char)0x65,  /* [1466] */
    (xdc_Char)0x74,  /* [1467] */
    (xdc_Char)0x65,  /* [1468] */
    (xdc_Char)0x72,  /* [1469] */
    (xdc_Char)0x20,  /* [1470] */
    (xdc_Char)0x6d,  /* [1471] */
    (xdc_Char)0x75,  /* [1472] */
    (xdc_Char)0x73,  /* [1473] */
    (xdc_Char)0x74,  /* [1474] */
    (xdc_Char)0x20,  /* [1475] */
    (xdc_Char)0x62,  /* [1476] */
    (xdc_Char)0x65,  /* [1477] */
    (xdc_Char)0x20,  /* [1478] */
    (xdc_Char)0x30,  /* [1479] */
    (xdc_Char)0x20,  /* [1480] */
    (xdc_Char)0x6f,  /* [1481] */
    (xdc_Char)0x72,  /* [1482] */
    (xdc_Char)0x20,  /* [1483] */
    (xdc_Char)0x61,  /* [1484] */
    (xdc_Char)0x20,  /* [1485] */
    (xdc_Char)0x70,  /* [1486] */
    (xdc_Char)0x6f,  /* [1487] */
    (xdc_Char)0x77,  /* [1488] */
    (xdc_Char)0x65,  /* [1489] */
    (xdc_Char)0x72,  /* [1490] */
    (xdc_Char)0x20,  /* [1491] */
    (xdc_Char)0x6f,  /* [1492] */
    (xdc_Char)0x66,  /* [1493] */
    (xdc_Char)0x20,  /* [1494] */
    (xdc_Char)0x32,  /* [1495] */
    (xdc_Char)0x20,  /* [1496] */
    (xdc_Char)0x3e,  /* [1497] */
    (xdc_Char)0x3d,  /* [1498] */
    (xdc_Char)0x20,  /* [1499] */
    (xdc_Char)0x74,  /* [1500] */
    (xdc_Char)0x68,  /* [1501] */
    (xdc_Char)0x65,  /* [1502] */
    (xdc_Char)0x20,  /* [1503] */
    (xdc_Char)0x76,  /* [1504] */
    (xdc_Char)0x61,  /* [1505] */
    (xdc_Char)0x6c,  /* [1506] */
    (xdc_Char)0x75,  /* [1507] */
    (xdc_Char)0x65,  /* [1508] */
    (xdc_Char)0x20,  /* [1509] */
    (xdc_Char)0x6f,  /* [1510] */
    (xdc_Char)0x66,  /* [1511] */
    (xdc_Char)0x20,  /* [1512] */
    (xdc_Char)0x4d,  /* [1513] */
    (xdc_Char)0x65,  /* [1514] */
    (xdc_Char)0x6d,  /* [1515] */
    (xdc_Char)0x6f,  /* [1516] */
    (xdc_Char)0x72,  /* [1517] */
    (xdc_Char)0x79,  /* [1518] */
    (xdc_Char)0x5f,  /* [1519] */
    (xdc_Char)0x67,  /* [1520] */
    (xdc_Char)0x65,  /* [1521] */
    (xdc_Char)0x74,  /* [1522] */
    (xdc_Char)0x4d,  /* [1523] */
    (xdc_Char)0x61,  /* [1524] */
    (xdc_Char)0x78,  /* [1525] */
    (xdc_Char)0x44,  /* [1526] */
    (xdc_Char)0x65,  /* [1527] */
    (xdc_Char)0x66,  /* [1528] */
    (xdc_Char)0x61,  /* [1529] */
    (xdc_Char)0x75,  /* [1530] */
    (xdc_Char)0x6c,  /* [1531] */
    (xdc_Char)0x74,  /* [1532] */
    (xdc_Char)0x54,  /* [1533] */
    (xdc_Char)0x79,  /* [1534] */
    (xdc_Char)0x70,  /* [1535] */
    (xdc_Char)0x65,  /* [1536] */
    (xdc_Char)0x41,  /* [1537] */
    (xdc_Char)0x6c,  /* [1538] */
    (xdc_Char)0x69,  /* [1539] */
    (xdc_Char)0x67,  /* [1540] */
    (xdc_Char)0x6e,  /* [1541] */
    (xdc_Char)0x28,  /* [1542] */
    (xdc_Char)0x29,  /* [1543] */
    (xdc_Char)0x0,  /* [1544] */
    (xdc_Char)0x62,  /* [1545] */
    (xdc_Char)0x6c,  /* [1546] */
    (xdc_Char)0x6f,  /* [1547] */
    (xdc_Char)0x63,  /* [1548] */
    (xdc_Char)0x6b,  /* [1549] */
    (xdc_Char)0x53,  /* [1550] */
    (xdc_Char)0x69,  /* [1551] */
    (xdc_Char)0x7a,  /* [1552] */
    (xdc_Char)0x65,  /* [1553] */
    (xdc_Char)0x20,  /* [1554] */
    (xdc_Char)0x63,  /* [1555] */
    (xdc_Char)0x61,  /* [1556] */
    (xdc_Char)0x6e,  /* [1557] */
    (xdc_Char)0x6e,  /* [1558] */
    (xdc_Char)0x6f,  /* [1559] */
    (xdc_Char)0x74,  /* [1560] */
    (xdc_Char)0x20,  /* [1561] */
    (xdc_Char)0x62,  /* [1562] */
    (xdc_Char)0x65,  /* [1563] */
    (xdc_Char)0x20,  /* [1564] */
    (xdc_Char)0x7a,  /* [1565] */
    (xdc_Char)0x65,  /* [1566] */
    (xdc_Char)0x72,  /* [1567] */
    (xdc_Char)0x6f,  /* [1568] */
    (xdc_Char)0x0,  /* [1569] */
    (xdc_Char)0x6e,  /* [1570] */
    (xdc_Char)0x75,  /* [1571] */
    (xdc_Char)0x6d,  /* [1572] */
    (xdc_Char)0x42,  /* [1573] */
    (xdc_Char)0x6c,  /* [1574] */
    (xdc_Char)0x6f,  /* [1575] */
    (xdc_Char)0x63,  /* [1576] */
    (xdc_Char)0x6b,  /* [1577] */
    (xdc_Char)0x73,  /* [1578] */
    (xdc_Char)0x20,  /* [1579] */
    (xdc_Char)0x63,  /* [1580] */
    (xdc_Char)0x61,  /* [1581] */
    (xdc_Char)0x6e,  /* [1582] */
    (xdc_Char)0x6e,  /* [1583] */
    (xdc_Char)0x6f,  /* [1584] */
    (xdc_Char)0x74,  /* [1585] */
    (xdc_Char)0x20,  /* [1586] */
    (xdc_Char)0x62,  /* [1587] */
    (xdc_Char)0x65,  /* [1588] */
    (xdc_Char)0x20,  /* [1589] */
    (xdc_Char)0x7a,  /* [1590] */
    (xdc_Char)0x65,  /* [1591] */
    (xdc_Char)0x72,  /* [1592] */
    (xdc_Char)0x6f,  /* [1593] */
    (xdc_Char)0x0,  /* [1594] */
    (xdc_Char)0x62,  /* [1595] */
    (xdc_Char)0x75,  /* [1596] */
    (xdc_Char)0x66,  /* [1597] */
    (xdc_Char)0x53,  /* [1598] */
    (xdc_Char)0x69,  /* [1599] */
    (xdc_Char)0x7a,  /* [1600] */
    (xdc_Char)0x65,  /* [1601] */
    (xdc_Char)0x20,  /* [1602] */
    (xdc_Char)0x63,  /* [1603] */
    (xdc_Char)0x61,  /* [1604] */
    (xdc_Char)0x6e,  /* [1605] */
    (xdc_Char)0x6e,  /* [1606] */
    (xdc_Char)0x6f,  /* [1607] */
    (xdc_Char)0x74,  /* [1608] */
    (xdc_Char)0x20,  /* [1609] */
    (xdc_Char)0x62,  /* [1610] */
    (xdc_Char)0x65,  /* [1611] */
    (xdc_Char)0x20,  /* [1612] */
    (xdc_Char)0x7a,  /* [1613] */
    (xdc_Char)0x65,  /* [1614] */
    (xdc_Char)0x72,  /* [1615] */
    (xdc_Char)0x6f,  /* [1616] */
    (xdc_Char)0x0,  /* [1617] */
    (xdc_Char)0x48,  /* [1618] */
    (xdc_Char)0x65,  /* [1619] */
    (xdc_Char)0x61,  /* [1620] */
    (xdc_Char)0x70,  /* [1621] */
    (xdc_Char)0x42,  /* [1622] */
    (xdc_Char)0x75,  /* [1623] */
    (xdc_Char)0x66,  /* [1624] */
    (xdc_Char)0x5f,  /* [1625] */
    (xdc_Char)0x63,  /* [1626] */
    (xdc_Char)0x72,  /* [1627] */
    (xdc_Char)0x65,  /* [1628] */
    (xdc_Char)0x61,  /* [1629] */
    (xdc_Char)0x74,  /* [1630] */
    (xdc_Char)0x65,  /* [1631] */
    (xdc_Char)0x27,  /* [1632] */
    (xdc_Char)0x73,  /* [1633] */
    (xdc_Char)0x20,  /* [1634] */
    (xdc_Char)0x62,  /* [1635] */
    (xdc_Char)0x75,  /* [1636] */
    (xdc_Char)0x66,  /* [1637] */
    (xdc_Char)0x53,  /* [1638] */
    (xdc_Char)0x69,  /* [1639] */
    (xdc_Char)0x7a,  /* [1640] */
    (xdc_Char)0x65,  /* [1641] */
    (xdc_Char)0x20,  /* [1642] */
    (xdc_Char)0x70,  /* [1643] */
    (xdc_Char)0x61,  /* [1644] */
    (xdc_Char)0x72,  /* [1645] */
    (xdc_Char)0x61,  /* [1646] */
    (xdc_Char)0x6d,  /* [1647] */
    (xdc_Char)0x65,  /* [1648] */
    (xdc_Char)0x74,  /* [1649] */
    (xdc_Char)0x65,  /* [1650] */
    (xdc_Char)0x72,  /* [1651] */
    (xdc_Char)0x20,  /* [1652] */
    (xdc_Char)0x69,  /* [1653] */
    (xdc_Char)0x73,  /* [1654] */
    (xdc_Char)0x20,  /* [1655] */
    (xdc_Char)0x69,  /* [1656] */
    (xdc_Char)0x6e,  /* [1657] */
    (xdc_Char)0x76,  /* [1658] */
    (xdc_Char)0x61,  /* [1659] */
    (xdc_Char)0x6c,  /* [1660] */
    (xdc_Char)0x69,  /* [1661] */
    (xdc_Char)0x64,  /* [1662] */
    (xdc_Char)0x20,  /* [1663] */
    (xdc_Char)0x28,  /* [1664] */
    (xdc_Char)0x74,  /* [1665] */
    (xdc_Char)0x6f,  /* [1666] */
    (xdc_Char)0x6f,  /* [1667] */
    (xdc_Char)0x20,  /* [1668] */
    (xdc_Char)0x73,  /* [1669] */
    (xdc_Char)0x6d,  /* [1670] */
    (xdc_Char)0x61,  /* [1671] */
    (xdc_Char)0x6c,  /* [1672] */
    (xdc_Char)0x6c,  /* [1673] */
    (xdc_Char)0x29,  /* [1674] */
    (xdc_Char)0x0,  /* [1675] */
    (xdc_Char)0x43,  /* [1676] */
    (xdc_Char)0x61,  /* [1677] */
    (xdc_Char)0x6e,  /* [1678] */
    (xdc_Char)0x6e,  /* [1679] */
    (xdc_Char)0x6f,  /* [1680] */
    (xdc_Char)0x74,  /* [1681] */
    (xdc_Char)0x20,  /* [1682] */
    (xdc_Char)0x63,  /* [1683] */
    (xdc_Char)0x61,  /* [1684] */
    (xdc_Char)0x6c,  /* [1685] */
    (xdc_Char)0x6c,  /* [1686] */
    (xdc_Char)0x20,  /* [1687] */
    (xdc_Char)0x48,  /* [1688] */
    (xdc_Char)0x65,  /* [1689] */
    (xdc_Char)0x61,  /* [1690] */
    (xdc_Char)0x70,  /* [1691] */
    (xdc_Char)0x42,  /* [1692] */
    (xdc_Char)0x75,  /* [1693] */
    (xdc_Char)0x66,  /* [1694] */
    (xdc_Char)0x5f,  /* [1695] */
    (xdc_Char)0x66,  /* [1696] */
    (xdc_Char)0x72,  /* [1697] */
    (xdc_Char)0x65,  /* [1698] */
    (xdc_Char)0x65,  /* [1699] */
    (xdc_Char)0x20,  /* [1700] */
    (xdc_Char)0x77,  /* [1701] */
    (xdc_Char)0x68,  /* [1702] */
    (xdc_Char)0x65,  /* [1703] */
    (xdc_Char)0x6e,  /* [1704] */
    (xdc_Char)0x20,  /* [1705] */
    (xdc_Char)0x6e,  /* [1706] */
    (xdc_Char)0x6f,  /* [1707] */
    (xdc_Char)0x20,  /* [1708] */
    (xdc_Char)0x62,  /* [1709] */
    (xdc_Char)0x6c,  /* [1710] */
    (xdc_Char)0x6f,  /* [1711] */
    (xdc_Char)0x63,  /* [1712] */
    (xdc_Char)0x6b,  /* [1713] */
    (xdc_Char)0x73,  /* [1714] */
    (xdc_Char)0x20,  /* [1715] */
    (xdc_Char)0x68,  /* [1716] */
    (xdc_Char)0x61,  /* [1717] */
    (xdc_Char)0x76,  /* [1718] */
    (xdc_Char)0x65,  /* [1719] */
    (xdc_Char)0x20,  /* [1720] */
    (xdc_Char)0x62,  /* [1721] */
    (xdc_Char)0x65,  /* [1722] */
    (xdc_Char)0x65,  /* [1723] */
    (xdc_Char)0x6e,  /* [1724] */
    (xdc_Char)0x20,  /* [1725] */
    (xdc_Char)0x61,  /* [1726] */
    (xdc_Char)0x6c,  /* [1727] */
    (xdc_Char)0x6c,  /* [1728] */
    (xdc_Char)0x6f,  /* [1729] */
    (xdc_Char)0x63,  /* [1730] */
    (xdc_Char)0x61,  /* [1731] */
    (xdc_Char)0x74,  /* [1732] */
    (xdc_Char)0x65,  /* [1733] */
    (xdc_Char)0x64,  /* [1734] */
    (xdc_Char)0x0,  /* [1735] */
    (xdc_Char)0x41,  /* [1736] */
    (xdc_Char)0x5f,  /* [1737] */
    (xdc_Char)0x69,  /* [1738] */
    (xdc_Char)0x6e,  /* [1739] */
    (xdc_Char)0x76,  /* [1740] */
    (xdc_Char)0x61,  /* [1741] */
    (xdc_Char)0x6c,  /* [1742] */
    (xdc_Char)0x69,  /* [1743] */
    (xdc_Char)0x64,  /* [1744] */
    (xdc_Char)0x46,  /* [1745] */
    (xdc_Char)0x72,  /* [1746] */
    (xdc_Char)0x65,  /* [1747] */
    (xdc_Char)0x65,  /* [1748] */
    (xdc_Char)0x3a,  /* [1749] */
    (xdc_Char)0x20,  /* [1750] */
    (xdc_Char)0x49,  /* [1751] */
    (xdc_Char)0x6e,  /* [1752] */
    (xdc_Char)0x76,  /* [1753] */
    (xdc_Char)0x61,  /* [1754] */
    (xdc_Char)0x6c,  /* [1755] */
    (xdc_Char)0x69,  /* [1756] */
    (xdc_Char)0x64,  /* [1757] */
    (xdc_Char)0x20,  /* [1758] */
    (xdc_Char)0x66,  /* [1759] */
    (xdc_Char)0x72,  /* [1760] */
    (xdc_Char)0x65,  /* [1761] */
    (xdc_Char)0x65,  /* [1762] */
    (xdc_Char)0x0,  /* [1763] */
    (xdc_Char)0x41,  /* [1764] */
    (xdc_Char)0x5f,  /* [1765] */
    (xdc_Char)0x7a,  /* [1766] */
    (xdc_Char)0x65,  /* [1767] */
    (xdc_Char)0x72,  /* [1768] */
    (xdc_Char)0x6f,  /* [1769] */
    (xdc_Char)0x42,  /* [1770] */
    (xdc_Char)0x6c,  /* [1771] */
    (xdc_Char)0x6f,  /* [1772] */
    (xdc_Char)0x63,  /* [1773] */
    (xdc_Char)0x6b,  /* [1774] */
    (xdc_Char)0x3a,  /* [1775] */
    (xdc_Char)0x20,  /* [1776] */
    (xdc_Char)0x43,  /* [1777] */
    (xdc_Char)0x61,  /* [1778] */
    (xdc_Char)0x6e,  /* [1779] */
    (xdc_Char)0x6e,  /* [1780] */
    (xdc_Char)0x6f,  /* [1781] */
    (xdc_Char)0x74,  /* [1782] */
    (xdc_Char)0x20,  /* [1783] */
    (xdc_Char)0x61,  /* [1784] */
    (xdc_Char)0x6c,  /* [1785] */
    (xdc_Char)0x6c,  /* [1786] */
    (xdc_Char)0x6f,  /* [1787] */
    (xdc_Char)0x63,  /* [1788] */
    (xdc_Char)0x61,  /* [1789] */
    (xdc_Char)0x74,  /* [1790] */
    (xdc_Char)0x65,  /* [1791] */
    (xdc_Char)0x20,  /* [1792] */
    (xdc_Char)0x73,  /* [1793] */
    (xdc_Char)0x69,  /* [1794] */
    (xdc_Char)0x7a,  /* [1795] */
    (xdc_Char)0x65,  /* [1796] */
    (xdc_Char)0x20,  /* [1797] */
    (xdc_Char)0x30,  /* [1798] */
    (xdc_Char)0x0,  /* [1799] */
    (xdc_Char)0x41,  /* [1800] */
    (xdc_Char)0x5f,  /* [1801] */
    (xdc_Char)0x68,  /* [1802] */
    (xdc_Char)0x65,  /* [1803] */
    (xdc_Char)0x61,  /* [1804] */
    (xdc_Char)0x70,  /* [1805] */
    (xdc_Char)0x53,  /* [1806] */
    (xdc_Char)0x69,  /* [1807] */
    (xdc_Char)0x7a,  /* [1808] */
    (xdc_Char)0x65,  /* [1809] */
    (xdc_Char)0x3a,  /* [1810] */
    (xdc_Char)0x20,  /* [1811] */
    (xdc_Char)0x52,  /* [1812] */
    (xdc_Char)0x65,  /* [1813] */
    (xdc_Char)0x71,  /* [1814] */
    (xdc_Char)0x75,  /* [1815] */
    (xdc_Char)0x65,  /* [1816] */
    (xdc_Char)0x73,  /* [1817] */
    (xdc_Char)0x74,  /* [1818] */
    (xdc_Char)0x65,  /* [1819] */
    (xdc_Char)0x64,  /* [1820] */
    (xdc_Char)0x20,  /* [1821] */
    (xdc_Char)0x68,  /* [1822] */
    (xdc_Char)0x65,  /* [1823] */
    (xdc_Char)0x61,  /* [1824] */
    (xdc_Char)0x70,  /* [1825] */
    (xdc_Char)0x20,  /* [1826] */
    (xdc_Char)0x73,  /* [1827] */
    (xdc_Char)0x69,  /* [1828] */
    (xdc_Char)0x7a,  /* [1829] */
    (xdc_Char)0x65,  /* [1830] */
    (xdc_Char)0x20,  /* [1831] */
    (xdc_Char)0x69,  /* [1832] */
    (xdc_Char)0x73,  /* [1833] */
    (xdc_Char)0x20,  /* [1834] */
    (xdc_Char)0x74,  /* [1835] */
    (xdc_Char)0x6f,  /* [1836] */
    (xdc_Char)0x6f,  /* [1837] */
    (xdc_Char)0x20,  /* [1838] */
    (xdc_Char)0x73,  /* [1839] */
    (xdc_Char)0x6d,  /* [1840] */
    (xdc_Char)0x61,  /* [1841] */
    (xdc_Char)0x6c,  /* [1842] */
    (xdc_Char)0x6c,  /* [1843] */
    (xdc_Char)0x0,  /* [1844] */
    (xdc_Char)0x41,  /* [1845] */
    (xdc_Char)0x5f,  /* [1846] */
    (xdc_Char)0x61,  /* [1847] */
    (xdc_Char)0x6c,  /* [1848] */
    (xdc_Char)0x69,  /* [1849] */
    (xdc_Char)0x67,  /* [1850] */
    (xdc_Char)0x6e,  /* [1851] */
    (xdc_Char)0x3a,  /* [1852] */
    (xdc_Char)0x20,  /* [1853] */
    (xdc_Char)0x52,  /* [1854] */
    (xdc_Char)0x65,  /* [1855] */
    (xdc_Char)0x71,  /* [1856] */
    (xdc_Char)0x75,  /* [1857] */
    (xdc_Char)0x65,  /* [1858] */
    (xdc_Char)0x73,  /* [1859] */
    (xdc_Char)0x74,  /* [1860] */
    (xdc_Char)0x65,  /* [1861] */
    (xdc_Char)0x64,  /* [1862] */
    (xdc_Char)0x20,  /* [1863] */
    (xdc_Char)0x61,  /* [1864] */
    (xdc_Char)0x6c,  /* [1865] */
    (xdc_Char)0x69,  /* [1866] */
    (xdc_Char)0x67,  /* [1867] */
    (xdc_Char)0x6e,  /* [1868] */
    (xdc_Char)0x20,  /* [1869] */
    (xdc_Char)0x69,  /* [1870] */
    (xdc_Char)0x73,  /* [1871] */
    (xdc_Char)0x20,  /* [1872] */
    (xdc_Char)0x6e,  /* [1873] */
    (xdc_Char)0x6f,  /* [1874] */
    (xdc_Char)0x74,  /* [1875] */
    (xdc_Char)0x20,  /* [1876] */
    (xdc_Char)0x61,  /* [1877] */
    (xdc_Char)0x20,  /* [1878] */
    (xdc_Char)0x70,  /* [1879] */
    (xdc_Char)0x6f,  /* [1880] */
    (xdc_Char)0x77,  /* [1881] */
    (xdc_Char)0x65,  /* [1882] */
    (xdc_Char)0x72,  /* [1883] */
    (xdc_Char)0x20,  /* [1884] */
    (xdc_Char)0x6f,  /* [1885] */
    (xdc_Char)0x66,  /* [1886] */
    (xdc_Char)0x20,  /* [1887] */
    (xdc_Char)0x32,  /* [1888] */
    (xdc_Char)0x0,  /* [1889] */
    (xdc_Char)0x49,  /* [1890] */
    (xdc_Char)0x6e,  /* [1891] */
    (xdc_Char)0x76,  /* [1892] */
    (xdc_Char)0x61,  /* [1893] */
    (xdc_Char)0x6c,  /* [1894] */
    (xdc_Char)0x69,  /* [1895] */
    (xdc_Char)0x64,  /* [1896] */
    (xdc_Char)0x20,  /* [1897] */
    (xdc_Char)0x62,  /* [1898] */
    (xdc_Char)0x6c,  /* [1899] */
    (xdc_Char)0x6f,  /* [1900] */
    (xdc_Char)0x63,  /* [1901] */
    (xdc_Char)0x6b,  /* [1902] */
    (xdc_Char)0x20,  /* [1903] */
    (xdc_Char)0x61,  /* [1904] */
    (xdc_Char)0x64,  /* [1905] */
    (xdc_Char)0x64,  /* [1906] */
    (xdc_Char)0x72,  /* [1907] */
    (xdc_Char)0x65,  /* [1908] */
    (xdc_Char)0x73,  /* [1909] */
    (xdc_Char)0x73,  /* [1910] */
    (xdc_Char)0x20,  /* [1911] */
    (xdc_Char)0x6f,  /* [1912] */
    (xdc_Char)0x6e,  /* [1913] */
    (xdc_Char)0x20,  /* [1914] */
    (xdc_Char)0x74,  /* [1915] */
    (xdc_Char)0x68,  /* [1916] */
    (xdc_Char)0x65,  /* [1917] */
    (xdc_Char)0x20,  /* [1918] */
    (xdc_Char)0x66,  /* [1919] */
    (xdc_Char)0x72,  /* [1920] */
    (xdc_Char)0x65,  /* [1921] */
    (xdc_Char)0x65,  /* [1922] */
    (xdc_Char)0x2e,  /* [1923] */
    (xdc_Char)0x20,  /* [1924] */
    (xdc_Char)0x46,  /* [1925] */
    (xdc_Char)0x61,  /* [1926] */
    (xdc_Char)0x69,  /* [1927] */
    (xdc_Char)0x6c,  /* [1928] */
    (xdc_Char)0x65,  /* [1929] */
    (xdc_Char)0x64,  /* [1930] */
    (xdc_Char)0x20,  /* [1931] */
    (xdc_Char)0x74,  /* [1932] */
    (xdc_Char)0x6f,  /* [1933] */
    (xdc_Char)0x20,  /* [1934] */
    (xdc_Char)0x66,  /* [1935] */
    (xdc_Char)0x72,  /* [1936] */
    (xdc_Char)0x65,  /* [1937] */
    (xdc_Char)0x65,  /* [1938] */
    (xdc_Char)0x20,  /* [1939] */
    (xdc_Char)0x62,  /* [1940] */
    (xdc_Char)0x6c,  /* [1941] */
    (xdc_Char)0x6f,  /* [1942] */
    (xdc_Char)0x63,  /* [1943] */
    (xdc_Char)0x6b,  /* [1944] */
    (xdc_Char)0x20,  /* [1945] */
    (xdc_Char)0x62,  /* [1946] */
    (xdc_Char)0x61,  /* [1947] */
    (xdc_Char)0x63,  /* [1948] */
    (xdc_Char)0x6b,  /* [1949] */
    (xdc_Char)0x20,  /* [1950] */
    (xdc_Char)0x74,  /* [1951] */
    (xdc_Char)0x6f,  /* [1952] */
    (xdc_Char)0x20,  /* [1953] */
    (xdc_Char)0x68,  /* [1954] */
    (xdc_Char)0x65,  /* [1955] */
    (xdc_Char)0x61,  /* [1956] */
    (xdc_Char)0x70,  /* [1957] */
    (xdc_Char)0x2e,  /* [1958] */
    (xdc_Char)0x0,  /* [1959] */
    (xdc_Char)0x41,  /* [1960] */
    (xdc_Char)0x5f,  /* [1961] */
    (xdc_Char)0x6e,  /* [1962] */
    (xdc_Char)0x6f,  /* [1963] */
    (xdc_Char)0x74,  /* [1964] */
    (xdc_Char)0x41,  /* [1965] */
    (xdc_Char)0x76,  /* [1966] */
    (xdc_Char)0x61,  /* [1967] */
    (xdc_Char)0x69,  /* [1968] */
    (xdc_Char)0x6c,  /* [1969] */
    (xdc_Char)0x61,  /* [1970] */
    (xdc_Char)0x62,  /* [1971] */
    (xdc_Char)0x6c,  /* [1972] */
    (xdc_Char)0x65,  /* [1973] */
    (xdc_Char)0x3a,  /* [1974] */
    (xdc_Char)0x20,  /* [1975] */
    (xdc_Char)0x73,  /* [1976] */
    (xdc_Char)0x74,  /* [1977] */
    (xdc_Char)0x61,  /* [1978] */
    (xdc_Char)0x74,  /* [1979] */
    (xdc_Char)0x69,  /* [1980] */
    (xdc_Char)0x63,  /* [1981] */
    (xdc_Char)0x61,  /* [1982] */
    (xdc_Char)0x6c,  /* [1983] */
    (xdc_Char)0x6c,  /* [1984] */
    (xdc_Char)0x79,  /* [1985] */
    (xdc_Char)0x20,  /* [1986] */
    (xdc_Char)0x63,  /* [1987] */
    (xdc_Char)0x72,  /* [1988] */
    (xdc_Char)0x65,  /* [1989] */
    (xdc_Char)0x61,  /* [1990] */
    (xdc_Char)0x74,  /* [1991] */
    (xdc_Char)0x65,  /* [1992] */
    (xdc_Char)0x64,  /* [1993] */
    (xdc_Char)0x20,  /* [1994] */
    (xdc_Char)0x74,  /* [1995] */
    (xdc_Char)0x69,  /* [1996] */
    (xdc_Char)0x6d,  /* [1997] */
    (xdc_Char)0x65,  /* [1998] */
    (xdc_Char)0x72,  /* [1999] */
    (xdc_Char)0x20,  /* [2000] */
    (xdc_Char)0x6e,  /* [2001] */
    (xdc_Char)0x6f,  /* [2002] */
    (xdc_Char)0x74,  /* [2003] */
    (xdc_Char)0x20,  /* [2004] */
    (xdc_Char)0x61,  /* [2005] */
    (xdc_Char)0x76,  /* [2006] */
    (xdc_Char)0x61,  /* [2007] */
    (xdc_Char)0x69,  /* [2008] */
    (xdc_Char)0x6c,  /* [2009] */
    (xdc_Char)0x61,  /* [2010] */
    (xdc_Char)0x62,  /* [2011] */
    (xdc_Char)0x6c,  /* [2012] */
    (xdc_Char)0x65,  /* [2013] */
    (xdc_Char)0x0,  /* [2014] */
    (xdc_Char)0x41,  /* [2015] */
    (xdc_Char)0x5f,  /* [2016] */
    (xdc_Char)0x62,  /* [2017] */
    (xdc_Char)0x61,  /* [2018] */
    (xdc_Char)0x64,  /* [2019] */
    (xdc_Char)0x43,  /* [2020] */
    (xdc_Char)0x6f,  /* [2021] */
    (xdc_Char)0x6e,  /* [2022] */
    (xdc_Char)0x74,  /* [2023] */
    (xdc_Char)0x65,  /* [2024] */
    (xdc_Char)0x78,  /* [2025] */
    (xdc_Char)0x74,  /* [2026] */
    (xdc_Char)0x3a,  /* [2027] */
    (xdc_Char)0x20,  /* [2028] */
    (xdc_Char)0x62,  /* [2029] */
    (xdc_Char)0x61,  /* [2030] */
    (xdc_Char)0x64,  /* [2031] */
    (xdc_Char)0x20,  /* [2032] */
    (xdc_Char)0x63,  /* [2033] */
    (xdc_Char)0x61,  /* [2034] */
    (xdc_Char)0x6c,  /* [2035] */
    (xdc_Char)0x6c,  /* [2036] */
    (xdc_Char)0x69,  /* [2037] */
    (xdc_Char)0x6e,  /* [2038] */
    (xdc_Char)0x67,  /* [2039] */
    (xdc_Char)0x20,  /* [2040] */
    (xdc_Char)0x63,  /* [2041] */
    (xdc_Char)0x6f,  /* [2042] */
    (xdc_Char)0x6e,  /* [2043] */
    (xdc_Char)0x74,  /* [2044] */
    (xdc_Char)0x65,  /* [2045] */
    (xdc_Char)0x78,  /* [2046] */
    (xdc_Char)0x74,  /* [2047] */
    (xdc_Char)0x2e,  /* [2048] */
    (xdc_Char)0x20,  /* [2049] */
    (xdc_Char)0x4d,  /* [2050] */
    (xdc_Char)0x61,  /* [2051] */
    (xdc_Char)0x79,  /* [2052] */
    (xdc_Char)0x20,  /* [2053] */
    (xdc_Char)0x6e,  /* [2054] */
    (xdc_Char)0x6f,  /* [2055] */
    (xdc_Char)0x74,  /* [2056] */
    (xdc_Char)0x20,  /* [2057] */
    (xdc_Char)0x62,  /* [2058] */
    (xdc_Char)0x65,  /* [2059] */
    (xdc_Char)0x20,  /* [2060] */
    (xdc_Char)0x65,  /* [2061] */
    (xdc_Char)0x6e,  /* [2062] */
    (xdc_Char)0x74,  /* [2063] */
    (xdc_Char)0x65,  /* [2064] */
    (xdc_Char)0x72,  /* [2065] */
    (xdc_Char)0x65,  /* [2066] */
    (xdc_Char)0x64,  /* [2067] */
    (xdc_Char)0x20,  /* [2068] */
    (xdc_Char)0x66,  /* [2069] */
    (xdc_Char)0x72,  /* [2070] */
    (xdc_Char)0x6f,  /* [2071] */
    (xdc_Char)0x6d,  /* [2072] */
    (xdc_Char)0x20,  /* [2073] */
    (xdc_Char)0x61,  /* [2074] */
    (xdc_Char)0x20,  /* [2075] */
    (xdc_Char)0x68,  /* [2076] */
    (xdc_Char)0x61,  /* [2077] */
    (xdc_Char)0x72,  /* [2078] */
    (xdc_Char)0x64,  /* [2079] */
    (xdc_Char)0x77,  /* [2080] */
    (xdc_Char)0x61,  /* [2081] */
    (xdc_Char)0x72,  /* [2082] */
    (xdc_Char)0x65,  /* [2083] */
    (xdc_Char)0x20,  /* [2084] */
    (xdc_Char)0x69,  /* [2085] */
    (xdc_Char)0x6e,  /* [2086] */
    (xdc_Char)0x74,  /* [2087] */
    (xdc_Char)0x65,  /* [2088] */
    (xdc_Char)0x72,  /* [2089] */
    (xdc_Char)0x72,  /* [2090] */
    (xdc_Char)0x75,  /* [2091] */
    (xdc_Char)0x70,  /* [2092] */
    (xdc_Char)0x74,  /* [2093] */
    (xdc_Char)0x20,  /* [2094] */
    (xdc_Char)0x74,  /* [2095] */
    (xdc_Char)0x68,  /* [2096] */
    (xdc_Char)0x72,  /* [2097] */
    (xdc_Char)0x65,  /* [2098] */
    (xdc_Char)0x61,  /* [2099] */
    (xdc_Char)0x64,  /* [2100] */
    (xdc_Char)0x2e,  /* [2101] */
    (xdc_Char)0x0,  /* [2102] */
    (xdc_Char)0x41,  /* [2103] */
    (xdc_Char)0x5f,  /* [2104] */
    (xdc_Char)0x62,  /* [2105] */
    (xdc_Char)0x61,  /* [2106] */
    (xdc_Char)0x64,  /* [2107] */
    (xdc_Char)0x43,  /* [2108] */
    (xdc_Char)0x6f,  /* [2109] */
    (xdc_Char)0x6e,  /* [2110] */
    (xdc_Char)0x74,  /* [2111] */
    (xdc_Char)0x65,  /* [2112] */
    (xdc_Char)0x78,  /* [2113] */
    (xdc_Char)0x74,  /* [2114] */
    (xdc_Char)0x3a,  /* [2115] */
    (xdc_Char)0x20,  /* [2116] */
    (xdc_Char)0x62,  /* [2117] */
    (xdc_Char)0x61,  /* [2118] */
    (xdc_Char)0x64,  /* [2119] */
    (xdc_Char)0x20,  /* [2120] */
    (xdc_Char)0x63,  /* [2121] */
    (xdc_Char)0x61,  /* [2122] */
    (xdc_Char)0x6c,  /* [2123] */
    (xdc_Char)0x6c,  /* [2124] */
    (xdc_Char)0x69,  /* [2125] */
    (xdc_Char)0x6e,  /* [2126] */
    (xdc_Char)0x67,  /* [2127] */
    (xdc_Char)0x20,  /* [2128] */
    (xdc_Char)0x63,  /* [2129] */
    (xdc_Char)0x6f,  /* [2130] */
    (xdc_Char)0x6e,  /* [2131] */
    (xdc_Char)0x74,  /* [2132] */
    (xdc_Char)0x65,  /* [2133] */
    (xdc_Char)0x78,  /* [2134] */
    (xdc_Char)0x74,  /* [2135] */
    (xdc_Char)0x2e,  /* [2136] */
    (xdc_Char)0x20,  /* [2137] */
    (xdc_Char)0x4d,  /* [2138] */
    (xdc_Char)0x61,  /* [2139] */
    (xdc_Char)0x79,  /* [2140] */
    (xdc_Char)0x20,  /* [2141] */
    (xdc_Char)0x6e,  /* [2142] */
    (xdc_Char)0x6f,  /* [2143] */
    (xdc_Char)0x74,  /* [2144] */
    (xdc_Char)0x20,  /* [2145] */
    (xdc_Char)0x62,  /* [2146] */
    (xdc_Char)0x65,  /* [2147] */
    (xdc_Char)0x20,  /* [2148] */
    (xdc_Char)0x65,  /* [2149] */
    (xdc_Char)0x6e,  /* [2150] */
    (xdc_Char)0x74,  /* [2151] */
    (xdc_Char)0x65,  /* [2152] */
    (xdc_Char)0x72,  /* [2153] */
    (xdc_Char)0x65,  /* [2154] */
    (xdc_Char)0x64,  /* [2155] */
    (xdc_Char)0x20,  /* [2156] */
    (xdc_Char)0x66,  /* [2157] */
    (xdc_Char)0x72,  /* [2158] */
    (xdc_Char)0x6f,  /* [2159] */
    (xdc_Char)0x6d,  /* [2160] */
    (xdc_Char)0x20,  /* [2161] */
    (xdc_Char)0x61,  /* [2162] */
    (xdc_Char)0x20,  /* [2163] */
    (xdc_Char)0x73,  /* [2164] */
    (xdc_Char)0x6f,  /* [2165] */
    (xdc_Char)0x66,  /* [2166] */
    (xdc_Char)0x74,  /* [2167] */
    (xdc_Char)0x77,  /* [2168] */
    (xdc_Char)0x61,  /* [2169] */
    (xdc_Char)0x72,  /* [2170] */
    (xdc_Char)0x65,  /* [2171] */
    (xdc_Char)0x20,  /* [2172] */
    (xdc_Char)0x6f,  /* [2173] */
    (xdc_Char)0x72,  /* [2174] */
    (xdc_Char)0x20,  /* [2175] */
    (xdc_Char)0x68,  /* [2176] */
    (xdc_Char)0x61,  /* [2177] */
    (xdc_Char)0x72,  /* [2178] */
    (xdc_Char)0x64,  /* [2179] */
    (xdc_Char)0x77,  /* [2180] */
    (xdc_Char)0x61,  /* [2181] */
    (xdc_Char)0x72,  /* [2182] */
    (xdc_Char)0x65,  /* [2183] */
    (xdc_Char)0x20,  /* [2184] */
    (xdc_Char)0x69,  /* [2185] */
    (xdc_Char)0x6e,  /* [2186] */
    (xdc_Char)0x74,  /* [2187] */
    (xdc_Char)0x65,  /* [2188] */
    (xdc_Char)0x72,  /* [2189] */
    (xdc_Char)0x72,  /* [2190] */
    (xdc_Char)0x75,  /* [2191] */
    (xdc_Char)0x70,  /* [2192] */
    (xdc_Char)0x74,  /* [2193] */
    (xdc_Char)0x20,  /* [2194] */
    (xdc_Char)0x74,  /* [2195] */
    (xdc_Char)0x68,  /* [2196] */
    (xdc_Char)0x72,  /* [2197] */
    (xdc_Char)0x65,  /* [2198] */
    (xdc_Char)0x61,  /* [2199] */
    (xdc_Char)0x64,  /* [2200] */
    (xdc_Char)0x2e,  /* [2201] */
    (xdc_Char)0x0,  /* [2202] */
    (xdc_Char)0x41,  /* [2203] */
    (xdc_Char)0x5f,  /* [2204] */
    (xdc_Char)0x62,  /* [2205] */
    (xdc_Char)0x61,  /* [2206] */
    (xdc_Char)0x64,  /* [2207] */
    (xdc_Char)0x43,  /* [2208] */
    (xdc_Char)0x6f,  /* [2209] */
    (xdc_Char)0x6e,  /* [2210] */
    (xdc_Char)0x74,  /* [2211] */
    (xdc_Char)0x65,  /* [2212] */
    (xdc_Char)0x78,  /* [2213] */
    (xdc_Char)0x74,  /* [2214] */
    (xdc_Char)0x3a,  /* [2215] */
    (xdc_Char)0x20,  /* [2216] */
    (xdc_Char)0x62,  /* [2217] */
    (xdc_Char)0x61,  /* [2218] */
    (xdc_Char)0x64,  /* [2219] */
    (xdc_Char)0x20,  /* [2220] */
    (xdc_Char)0x63,  /* [2221] */
    (xdc_Char)0x61,  /* [2222] */
    (xdc_Char)0x6c,  /* [2223] */
    (xdc_Char)0x6c,  /* [2224] */
    (xdc_Char)0x69,  /* [2225] */
    (xdc_Char)0x6e,  /* [2226] */
    (xdc_Char)0x67,  /* [2227] */
    (xdc_Char)0x20,  /* [2228] */
    (xdc_Char)0x63,  /* [2229] */
    (xdc_Char)0x6f,  /* [2230] */
    (xdc_Char)0x6e,  /* [2231] */
    (xdc_Char)0x74,  /* [2232] */
    (xdc_Char)0x65,  /* [2233] */
    (xdc_Char)0x78,  /* [2234] */
    (xdc_Char)0x74,  /* [2235] */
    (xdc_Char)0x2e,  /* [2236] */
    (xdc_Char)0x20,  /* [2237] */
    (xdc_Char)0x53,  /* [2238] */
    (xdc_Char)0x65,  /* [2239] */
    (xdc_Char)0x65,  /* [2240] */
    (xdc_Char)0x20,  /* [2241] */
    (xdc_Char)0x47,  /* [2242] */
    (xdc_Char)0x61,  /* [2243] */
    (xdc_Char)0x74,  /* [2244] */
    (xdc_Char)0x65,  /* [2245] */
    (xdc_Char)0x4d,  /* [2246] */
    (xdc_Char)0x75,  /* [2247] */
    (xdc_Char)0x74,  /* [2248] */
    (xdc_Char)0x65,  /* [2249] */
    (xdc_Char)0x78,  /* [2250] */
    (xdc_Char)0x50,  /* [2251] */
    (xdc_Char)0x72,  /* [2252] */
    (xdc_Char)0x69,  /* [2253] */
    (xdc_Char)0x20,  /* [2254] */
    (xdc_Char)0x41,  /* [2255] */
    (xdc_Char)0x50,  /* [2256] */
    (xdc_Char)0x49,  /* [2257] */
    (xdc_Char)0x20,  /* [2258] */
    (xdc_Char)0x64,  /* [2259] */
    (xdc_Char)0x6f,  /* [2260] */
    (xdc_Char)0x63,  /* [2261] */
    (xdc_Char)0x20,  /* [2262] */
    (xdc_Char)0x66,  /* [2263] */
    (xdc_Char)0x6f,  /* [2264] */
    (xdc_Char)0x72,  /* [2265] */
    (xdc_Char)0x20,  /* [2266] */
    (xdc_Char)0x64,  /* [2267] */
    (xdc_Char)0x65,  /* [2268] */
    (xdc_Char)0x74,  /* [2269] */
    (xdc_Char)0x61,  /* [2270] */
    (xdc_Char)0x69,  /* [2271] */
    (xdc_Char)0x6c,  /* [2272] */
    (xdc_Char)0x73,  /* [2273] */
    (xdc_Char)0x2e,  /* [2274] */
    (xdc_Char)0x0,  /* [2275] */
    (xdc_Char)0x41,  /* [2276] */
    (xdc_Char)0x5f,  /* [2277] */
    (xdc_Char)0x62,  /* [2278] */
    (xdc_Char)0x61,  /* [2279] */
    (xdc_Char)0x64,  /* [2280] */
    (xdc_Char)0x43,  /* [2281] */
    (xdc_Char)0x6f,  /* [2282] */
    (xdc_Char)0x6e,  /* [2283] */
    (xdc_Char)0x74,  /* [2284] */
    (xdc_Char)0x65,  /* [2285] */
    (xdc_Char)0x78,  /* [2286] */
    (xdc_Char)0x74,  /* [2287] */
    (xdc_Char)0x3a,  /* [2288] */
    (xdc_Char)0x20,  /* [2289] */
    (xdc_Char)0x62,  /* [2290] */
    (xdc_Char)0x61,  /* [2291] */
    (xdc_Char)0x64,  /* [2292] */
    (xdc_Char)0x20,  /* [2293] */
    (xdc_Char)0x63,  /* [2294] */
    (xdc_Char)0x61,  /* [2295] */
    (xdc_Char)0x6c,  /* [2296] */
    (xdc_Char)0x6c,  /* [2297] */
    (xdc_Char)0x69,  /* [2298] */
    (xdc_Char)0x6e,  /* [2299] */
    (xdc_Char)0x67,  /* [2300] */
    (xdc_Char)0x20,  /* [2301] */
    (xdc_Char)0x63,  /* [2302] */
    (xdc_Char)0x6f,  /* [2303] */
    (xdc_Char)0x6e,  /* [2304] */
    (xdc_Char)0x74,  /* [2305] */
    (xdc_Char)0x65,  /* [2306] */
    (xdc_Char)0x78,  /* [2307] */
    (xdc_Char)0x74,  /* [2308] */
    (xdc_Char)0x2e,  /* [2309] */
    (xdc_Char)0x20,  /* [2310] */
    (xdc_Char)0x53,  /* [2311] */
    (xdc_Char)0x65,  /* [2312] */
    (xdc_Char)0x65,  /* [2313] */
    (xdc_Char)0x20,  /* [2314] */
    (xdc_Char)0x47,  /* [2315] */
    (xdc_Char)0x61,  /* [2316] */
    (xdc_Char)0x74,  /* [2317] */
    (xdc_Char)0x65,  /* [2318] */
    (xdc_Char)0x4d,  /* [2319] */
    (xdc_Char)0x75,  /* [2320] */
    (xdc_Char)0x74,  /* [2321] */
    (xdc_Char)0x65,  /* [2322] */
    (xdc_Char)0x78,  /* [2323] */
    (xdc_Char)0x20,  /* [2324] */
    (xdc_Char)0x41,  /* [2325] */
    (xdc_Char)0x50,  /* [2326] */
    (xdc_Char)0x49,  /* [2327] */
    (xdc_Char)0x20,  /* [2328] */
    (xdc_Char)0x64,  /* [2329] */
    (xdc_Char)0x6f,  /* [2330] */
    (xdc_Char)0x63,  /* [2331] */
    (xdc_Char)0x20,  /* [2332] */
    (xdc_Char)0x66,  /* [2333] */
    (xdc_Char)0x6f,  /* [2334] */
    (xdc_Char)0x72,  /* [2335] */
    (xdc_Char)0x20,  /* [2336] */
    (xdc_Char)0x64,  /* [2337] */
    (xdc_Char)0x65,  /* [2338] */
    (xdc_Char)0x74,  /* [2339] */
    (xdc_Char)0x61,  /* [2340] */
    (xdc_Char)0x69,  /* [2341] */
    (xdc_Char)0x6c,  /* [2342] */
    (xdc_Char)0x73,  /* [2343] */
    (xdc_Char)0x2e,  /* [2344] */
    (xdc_Char)0x0,  /* [2345] */
    (xdc_Char)0x41,  /* [2346] */
    (xdc_Char)0x5f,  /* [2347] */
    (xdc_Char)0x7a,  /* [2348] */
    (xdc_Char)0x65,  /* [2349] */
    (xdc_Char)0x72,  /* [2350] */
    (xdc_Char)0x6f,  /* [2351] */
    (xdc_Char)0x54,  /* [2352] */
    (xdc_Char)0x69,  /* [2353] */
    (xdc_Char)0x6d,  /* [2354] */
    (xdc_Char)0x65,  /* [2355] */
    (xdc_Char)0x6f,  /* [2356] */
    (xdc_Char)0x75,  /* [2357] */
    (xdc_Char)0x74,  /* [2358] */
    (xdc_Char)0x3a,  /* [2359] */
    (xdc_Char)0x20,  /* [2360] */
    (xdc_Char)0x54,  /* [2361] */
    (xdc_Char)0x69,  /* [2362] */
    (xdc_Char)0x6d,  /* [2363] */
    (xdc_Char)0x65,  /* [2364] */
    (xdc_Char)0x6f,  /* [2365] */
    (xdc_Char)0x75,  /* [2366] */
    (xdc_Char)0x74,  /* [2367] */
    (xdc_Char)0x20,  /* [2368] */
    (xdc_Char)0x76,  /* [2369] */
    (xdc_Char)0x61,  /* [2370] */
    (xdc_Char)0x6c,  /* [2371] */
    (xdc_Char)0x75,  /* [2372] */
    (xdc_Char)0x65,  /* [2373] */
    (xdc_Char)0x20,  /* [2374] */
    (xdc_Char)0x61,  /* [2375] */
    (xdc_Char)0x6e,  /* [2376] */
    (xdc_Char)0x6e,  /* [2377] */
    (xdc_Char)0x6f,  /* [2378] */
    (xdc_Char)0x74,  /* [2379] */
    (xdc_Char)0x20,  /* [2380] */
    (xdc_Char)0x62,  /* [2381] */
    (xdc_Char)0x65,  /* [2382] */
    (xdc_Char)0x20,  /* [2383] */
    (xdc_Char)0x7a,  /* [2384] */
    (xdc_Char)0x65,  /* [2385] */
    (xdc_Char)0x72,  /* [2386] */
    (xdc_Char)0x6f,  /* [2387] */
    (xdc_Char)0x0,  /* [2388] */
    (xdc_Char)0x41,  /* [2389] */
    (xdc_Char)0x5f,  /* [2390] */
    (xdc_Char)0x69,  /* [2391] */
    (xdc_Char)0x6e,  /* [2392] */
    (xdc_Char)0x76,  /* [2393] */
    (xdc_Char)0x61,  /* [2394] */
    (xdc_Char)0x6c,  /* [2395] */
    (xdc_Char)0x69,  /* [2396] */
    (xdc_Char)0x64,  /* [2397] */
    (xdc_Char)0x4b,  /* [2398] */
    (xdc_Char)0x65,  /* [2399] */
    (xdc_Char)0x79,  /* [2400] */
    (xdc_Char)0x3a,  /* [2401] */
    (xdc_Char)0x20,  /* [2402] */
    (xdc_Char)0x74,  /* [2403] */
    (xdc_Char)0x68,  /* [2404] */
    (xdc_Char)0x65,  /* [2405] */
    (xdc_Char)0x20,  /* [2406] */
    (xdc_Char)0x6b,  /* [2407] */
    (xdc_Char)0x65,  /* [2408] */
    (xdc_Char)0x79,  /* [2409] */
    (xdc_Char)0x20,  /* [2410] */
    (xdc_Char)0x6d,  /* [2411] */
    (xdc_Char)0x75,  /* [2412] */
    (xdc_Char)0x73,  /* [2413] */
    (xdc_Char)0x74,  /* [2414] */
    (xdc_Char)0x20,  /* [2415] */
    (xdc_Char)0x62,  /* [2416] */
    (xdc_Char)0x65,  /* [2417] */
    (xdc_Char)0x20,  /* [2418] */
    (xdc_Char)0x73,  /* [2419] */
    (xdc_Char)0x65,  /* [2420] */
    (xdc_Char)0x74,  /* [2421] */
    (xdc_Char)0x20,  /* [2422] */
    (xdc_Char)0x74,  /* [2423] */
    (xdc_Char)0x6f,  /* [2424] */
    (xdc_Char)0x20,  /* [2425] */
    (xdc_Char)0x61,  /* [2426] */
    (xdc_Char)0x20,  /* [2427] */
    (xdc_Char)0x6e,  /* [2428] */
    (xdc_Char)0x6f,  /* [2429] */
    (xdc_Char)0x6e,  /* [2430] */
    (xdc_Char)0x2d,  /* [2431] */
    (xdc_Char)0x64,  /* [2432] */
    (xdc_Char)0x65,  /* [2433] */
    (xdc_Char)0x66,  /* [2434] */
    (xdc_Char)0x61,  /* [2435] */
    (xdc_Char)0x75,  /* [2436] */
    (xdc_Char)0x6c,  /* [2437] */
    (xdc_Char)0x74,  /* [2438] */
    (xdc_Char)0x20,  /* [2439] */
    (xdc_Char)0x76,  /* [2440] */
    (xdc_Char)0x61,  /* [2441] */
    (xdc_Char)0x6c,  /* [2442] */
    (xdc_Char)0x75,  /* [2443] */
    (xdc_Char)0x65,  /* [2444] */
    (xdc_Char)0x0,  /* [2445] */
    (xdc_Char)0x41,  /* [2446] */
    (xdc_Char)0x5f,  /* [2447] */
    (xdc_Char)0x6e,  /* [2448] */
    (xdc_Char)0x75,  /* [2449] */
    (xdc_Char)0x6c,  /* [2450] */
    (xdc_Char)0x6c,  /* [2451] */
    (xdc_Char)0x48,  /* [2452] */
    (xdc_Char)0x61,  /* [2453] */
    (xdc_Char)0x6e,  /* [2454] */
    (xdc_Char)0x64,  /* [2455] */
    (xdc_Char)0x6c,  /* [2456] */
    (xdc_Char)0x65,  /* [2457] */
    (xdc_Char)0x3a,  /* [2458] */
    (xdc_Char)0x20,  /* [2459] */
    (xdc_Char)0x4e,  /* [2460] */
    (xdc_Char)0x75,  /* [2461] */
    (xdc_Char)0x6c,  /* [2462] */
    (xdc_Char)0x6c,  /* [2463] */
    (xdc_Char)0x20,  /* [2464] */
    (xdc_Char)0x68,  /* [2465] */
    (xdc_Char)0x61,  /* [2466] */
    (xdc_Char)0x6e,  /* [2467] */
    (xdc_Char)0x64,  /* [2468] */
    (xdc_Char)0x6c,  /* [2469] */
    (xdc_Char)0x65,  /* [2470] */
    (xdc_Char)0x20,  /* [2471] */
    (xdc_Char)0x70,  /* [2472] */
    (xdc_Char)0x61,  /* [2473] */
    (xdc_Char)0x73,  /* [2474] */
    (xdc_Char)0x73,  /* [2475] */
    (xdc_Char)0x65,  /* [2476] */
    (xdc_Char)0x64,  /* [2477] */
    (xdc_Char)0x20,  /* [2478] */
    (xdc_Char)0x74,  /* [2479] */
    (xdc_Char)0x6f,  /* [2480] */
    (xdc_Char)0x20,  /* [2481] */
    (xdc_Char)0x63,  /* [2482] */
    (xdc_Char)0x72,  /* [2483] */
    (xdc_Char)0x65,  /* [2484] */
    (xdc_Char)0x61,  /* [2485] */
    (xdc_Char)0x74,  /* [2486] */
    (xdc_Char)0x65,  /* [2487] */
    (xdc_Char)0x0,  /* [2488] */
    (xdc_Char)0x61,  /* [2489] */
    (xdc_Char)0x73,  /* [2490] */
    (xdc_Char)0x73,  /* [2491] */
    (xdc_Char)0x65,  /* [2492] */
    (xdc_Char)0x72,  /* [2493] */
    (xdc_Char)0x74,  /* [2494] */
    (xdc_Char)0x69,  /* [2495] */
    (xdc_Char)0x6f,  /* [2496] */
    (xdc_Char)0x6e,  /* [2497] */
    (xdc_Char)0x20,  /* [2498] */
    (xdc_Char)0x66,  /* [2499] */
    (xdc_Char)0x61,  /* [2500] */
    (xdc_Char)0x69,  /* [2501] */
    (xdc_Char)0x6c,  /* [2502] */
    (xdc_Char)0x75,  /* [2503] */
    (xdc_Char)0x72,  /* [2504] */
    (xdc_Char)0x65,  /* [2505] */
    (xdc_Char)0x25,  /* [2506] */
    (xdc_Char)0x73,  /* [2507] */
    (xdc_Char)0x25,  /* [2508] */
    (xdc_Char)0x73,  /* [2509] */
    (xdc_Char)0x0,  /* [2510] */
    (xdc_Char)0x25,  /* [2511] */
    (xdc_Char)0x24,  /* [2512] */
    (xdc_Char)0x53,  /* [2513] */
    (xdc_Char)0x0,  /* [2514] */
    (xdc_Char)0x6f,  /* [2515] */
    (xdc_Char)0x75,  /* [2516] */
    (xdc_Char)0x74,  /* [2517] */
    (xdc_Char)0x20,  /* [2518] */
    (xdc_Char)0x6f,  /* [2519] */
    (xdc_Char)0x66,  /* [2520] */
    (xdc_Char)0x20,  /* [2521] */
    (xdc_Char)0x6d,  /* [2522] */
    (xdc_Char)0x65,  /* [2523] */
    (xdc_Char)0x6d,  /* [2524] */
    (xdc_Char)0x6f,  /* [2525] */
    (xdc_Char)0x72,  /* [2526] */
    (xdc_Char)0x79,  /* [2527] */
    (xdc_Char)0x3a,  /* [2528] */
    (xdc_Char)0x20,  /* [2529] */
    (xdc_Char)0x68,  /* [2530] */
    (xdc_Char)0x65,  /* [2531] */
    (xdc_Char)0x61,  /* [2532] */
    (xdc_Char)0x70,  /* [2533] */
    (xdc_Char)0x3d,  /* [2534] */
    (xdc_Char)0x30,  /* [2535] */
    (xdc_Char)0x78,  /* [2536] */
    (xdc_Char)0x25,  /* [2537] */
    (xdc_Char)0x78,  /* [2538] */
    (xdc_Char)0x2c,  /* [2539] */
    (xdc_Char)0x20,  /* [2540] */
    (xdc_Char)0x73,  /* [2541] */
    (xdc_Char)0x69,  /* [2542] */
    (xdc_Char)0x7a,  /* [2543] */
    (xdc_Char)0x65,  /* [2544] */
    (xdc_Char)0x3d,  /* [2545] */
    (xdc_Char)0x25,  /* [2546] */
    (xdc_Char)0x75,  /* [2547] */
    (xdc_Char)0x0,  /* [2548] */
    (xdc_Char)0x25,  /* [2549] */
    (xdc_Char)0x73,  /* [2550] */
    (xdc_Char)0x20,  /* [2551] */
    (xdc_Char)0x30,  /* [2552] */
    (xdc_Char)0x78,  /* [2553] */
    (xdc_Char)0x25,  /* [2554] */
    (xdc_Char)0x78,  /* [2555] */
    (xdc_Char)0x0,  /* [2556] */
    (xdc_Char)0x45,  /* [2557] */
    (xdc_Char)0x5f,  /* [2558] */
    (xdc_Char)0x62,  /* [2559] */
    (xdc_Char)0x61,  /* [2560] */
    (xdc_Char)0x64,  /* [2561] */
    (xdc_Char)0x4c,  /* [2562] */
    (xdc_Char)0x65,  /* [2563] */
    (xdc_Char)0x76,  /* [2564] */
    (xdc_Char)0x65,  /* [2565] */
    (xdc_Char)0x6c,  /* [2566] */
    (xdc_Char)0x3a,  /* [2567] */
    (xdc_Char)0x20,  /* [2568] */
    (xdc_Char)0x42,  /* [2569] */
    (xdc_Char)0x61,  /* [2570] */
    (xdc_Char)0x64,  /* [2571] */
    (xdc_Char)0x20,  /* [2572] */
    (xdc_Char)0x66,  /* [2573] */
    (xdc_Char)0x69,  /* [2574] */
    (xdc_Char)0x6c,  /* [2575] */
    (xdc_Char)0x74,  /* [2576] */
    (xdc_Char)0x65,  /* [2577] */
    (xdc_Char)0x72,  /* [2578] */
    (xdc_Char)0x20,  /* [2579] */
    (xdc_Char)0x6c,  /* [2580] */
    (xdc_Char)0x65,  /* [2581] */
    (xdc_Char)0x76,  /* [2582] */
    (xdc_Char)0x65,  /* [2583] */
    (xdc_Char)0x6c,  /* [2584] */
    (xdc_Char)0x20,  /* [2585] */
    (xdc_Char)0x76,  /* [2586] */
    (xdc_Char)0x61,  /* [2587] */
    (xdc_Char)0x6c,  /* [2588] */
    (xdc_Char)0x75,  /* [2589] */
    (xdc_Char)0x65,  /* [2590] */
    (xdc_Char)0x3a,  /* [2591] */
    (xdc_Char)0x20,  /* [2592] */
    (xdc_Char)0x25,  /* [2593] */
    (xdc_Char)0x64,  /* [2594] */
    (xdc_Char)0x0,  /* [2595] */
    (xdc_Char)0x66,  /* [2596] */
    (xdc_Char)0x72,  /* [2597] */
    (xdc_Char)0x65,  /* [2598] */
    (xdc_Char)0x65,  /* [2599] */
    (xdc_Char)0x28,  /* [2600] */
    (xdc_Char)0x29,  /* [2601] */
    (xdc_Char)0x20,  /* [2602] */
    (xdc_Char)0x69,  /* [2603] */
    (xdc_Char)0x6e,  /* [2604] */
    (xdc_Char)0x76,  /* [2605] */
    (xdc_Char)0x61,  /* [2606] */
    (xdc_Char)0x6c,  /* [2607] */
    (xdc_Char)0x69,  /* [2608] */
    (xdc_Char)0x64,  /* [2609] */
    (xdc_Char)0x20,  /* [2610] */
    (xdc_Char)0x69,  /* [2611] */
    (xdc_Char)0x6e,  /* [2612] */
    (xdc_Char)0x20,  /* [2613] */
    (xdc_Char)0x67,  /* [2614] */
    (xdc_Char)0x72,  /* [2615] */
    (xdc_Char)0x6f,  /* [2616] */
    (xdc_Char)0x77,  /* [2617] */
    (xdc_Char)0x74,  /* [2618] */
    (xdc_Char)0x68,  /* [2619] */
    (xdc_Char)0x2d,  /* [2620] */
    (xdc_Char)0x6f,  /* [2621] */
    (xdc_Char)0x6e,  /* [2622] */
    (xdc_Char)0x6c,  /* [2623] */
    (xdc_Char)0x79,  /* [2624] */
    (xdc_Char)0x20,  /* [2625] */
    (xdc_Char)0x48,  /* [2626] */
    (xdc_Char)0x65,  /* [2627] */
    (xdc_Char)0x61,  /* [2628] */
    (xdc_Char)0x70,  /* [2629] */
    (xdc_Char)0x4d,  /* [2630] */
    (xdc_Char)0x69,  /* [2631] */
    (xdc_Char)0x6e,  /* [2632] */
    (xdc_Char)0x0,  /* [2633] */
    (xdc_Char)0x54,  /* [2634] */
    (xdc_Char)0x68,  /* [2635] */
    (xdc_Char)0x65,  /* [2636] */
    (xdc_Char)0x20,  /* [2637] */
    (xdc_Char)0x52,  /* [2638] */
    (xdc_Char)0x54,  /* [2639] */
    (xdc_Char)0x53,  /* [2640] */
    (xdc_Char)0x20,  /* [2641] */
    (xdc_Char)0x68,  /* [2642] */
    (xdc_Char)0x65,  /* [2643] */
    (xdc_Char)0x61,  /* [2644] */
    (xdc_Char)0x70,  /* [2645] */
    (xdc_Char)0x20,  /* [2646] */
    (xdc_Char)0x69,  /* [2647] */
    (xdc_Char)0x73,  /* [2648] */
    (xdc_Char)0x20,  /* [2649] */
    (xdc_Char)0x75,  /* [2650] */
    (xdc_Char)0x73,  /* [2651] */
    (xdc_Char)0x65,  /* [2652] */
    (xdc_Char)0x64,  /* [2653] */
    (xdc_Char)0x20,  /* [2654] */
    (xdc_Char)0x75,  /* [2655] */
    (xdc_Char)0x70,  /* [2656] */
    (xdc_Char)0x2e,  /* [2657] */
    (xdc_Char)0x20,  /* [2658] */
    (xdc_Char)0x45,  /* [2659] */
    (xdc_Char)0x78,  /* [2660] */
    (xdc_Char)0x61,  /* [2661] */
    (xdc_Char)0x6d,  /* [2662] */
    (xdc_Char)0x69,  /* [2663] */
    (xdc_Char)0x6e,  /* [2664] */
    (xdc_Char)0x65,  /* [2665] */
    (xdc_Char)0x20,  /* [2666] */
    (xdc_Char)0x50,  /* [2667] */
    (xdc_Char)0x72,  /* [2668] */
    (xdc_Char)0x6f,  /* [2669] */
    (xdc_Char)0x67,  /* [2670] */
    (xdc_Char)0x72,  /* [2671] */
    (xdc_Char)0x61,  /* [2672] */
    (xdc_Char)0x6d,  /* [2673] */
    (xdc_Char)0x2e,  /* [2674] */
    (xdc_Char)0x68,  /* [2675] */
    (xdc_Char)0x65,  /* [2676] */
    (xdc_Char)0x61,  /* [2677] */
    (xdc_Char)0x70,  /* [2678] */
    (xdc_Char)0x2e,  /* [2679] */
    (xdc_Char)0x0,  /* [2680] */
    (xdc_Char)0x45,  /* [2681] */
    (xdc_Char)0x5f,  /* [2682] */
    (xdc_Char)0x62,  /* [2683] */
    (xdc_Char)0x61,  /* [2684] */
    (xdc_Char)0x64,  /* [2685] */
    (xdc_Char)0x43,  /* [2686] */
    (xdc_Char)0x6f,  /* [2687] */
    (xdc_Char)0x6d,  /* [2688] */
    (xdc_Char)0x6d,  /* [2689] */
    (xdc_Char)0x61,  /* [2690] */
    (xdc_Char)0x6e,  /* [2691] */
    (xdc_Char)0x64,  /* [2692] */
    (xdc_Char)0x3a,  /* [2693] */
    (xdc_Char)0x20,  /* [2694] */
    (xdc_Char)0x52,  /* [2695] */
    (xdc_Char)0x65,  /* [2696] */
    (xdc_Char)0x63,  /* [2697] */
    (xdc_Char)0x65,  /* [2698] */
    (xdc_Char)0x69,  /* [2699] */
    (xdc_Char)0x76,  /* [2700] */
    (xdc_Char)0x65,  /* [2701] */
    (xdc_Char)0x64,  /* [2702] */
    (xdc_Char)0x20,  /* [2703] */
    (xdc_Char)0x69,  /* [2704] */
    (xdc_Char)0x6e,  /* [2705] */
    (xdc_Char)0x76,  /* [2706] */
    (xdc_Char)0x61,  /* [2707] */
    (xdc_Char)0x6c,  /* [2708] */
    (xdc_Char)0x69,  /* [2709] */
    (xdc_Char)0x64,  /* [2710] */
    (xdc_Char)0x20,  /* [2711] */
    (xdc_Char)0x63,  /* [2712] */
    (xdc_Char)0x6f,  /* [2713] */
    (xdc_Char)0x6d,  /* [2714] */
    (xdc_Char)0x6d,  /* [2715] */
    (xdc_Char)0x61,  /* [2716] */
    (xdc_Char)0x6e,  /* [2717] */
    (xdc_Char)0x64,  /* [2718] */
    (xdc_Char)0x2c,  /* [2719] */
    (xdc_Char)0x20,  /* [2720] */
    (xdc_Char)0x69,  /* [2721] */
    (xdc_Char)0x64,  /* [2722] */
    (xdc_Char)0x3a,  /* [2723] */
    (xdc_Char)0x20,  /* [2724] */
    (xdc_Char)0x25,  /* [2725] */
    (xdc_Char)0x64,  /* [2726] */
    (xdc_Char)0x2e,  /* [2727] */
    (xdc_Char)0x0,  /* [2728] */
    (xdc_Char)0x45,  /* [2729] */
    (xdc_Char)0x5f,  /* [2730] */
    (xdc_Char)0x73,  /* [2731] */
    (xdc_Char)0x74,  /* [2732] */
    (xdc_Char)0x61,  /* [2733] */
    (xdc_Char)0x63,  /* [2734] */
    (xdc_Char)0x6b,  /* [2735] */
    (xdc_Char)0x4f,  /* [2736] */
    (xdc_Char)0x76,  /* [2737] */
    (xdc_Char)0x65,  /* [2738] */
    (xdc_Char)0x72,  /* [2739] */
    (xdc_Char)0x66,  /* [2740] */
    (xdc_Char)0x6c,  /* [2741] */
    (xdc_Char)0x6f,  /* [2742] */
    (xdc_Char)0x77,  /* [2743] */
    (xdc_Char)0x3a,  /* [2744] */
    (xdc_Char)0x20,  /* [2745] */
    (xdc_Char)0x49,  /* [2746] */
    (xdc_Char)0x53,  /* [2747] */
    (xdc_Char)0x52,  /* [2748] */
    (xdc_Char)0x20,  /* [2749] */
    (xdc_Char)0x73,  /* [2750] */
    (xdc_Char)0x74,  /* [2751] */
    (xdc_Char)0x61,  /* [2752] */
    (xdc_Char)0x63,  /* [2753] */
    (xdc_Char)0x6b,  /* [2754] */
    (xdc_Char)0x20,  /* [2755] */
    (xdc_Char)0x6f,  /* [2756] */
    (xdc_Char)0x76,  /* [2757] */
    (xdc_Char)0x65,  /* [2758] */
    (xdc_Char)0x72,  /* [2759] */
    (xdc_Char)0x66,  /* [2760] */
    (xdc_Char)0x6c,  /* [2761] */
    (xdc_Char)0x6f,  /* [2762] */
    (xdc_Char)0x77,  /* [2763] */
    (xdc_Char)0x2e,  /* [2764] */
    (xdc_Char)0x0,  /* [2765] */
    (xdc_Char)0x45,  /* [2766] */
    (xdc_Char)0x5f,  /* [2767] */
    (xdc_Char)0x73,  /* [2768] */
    (xdc_Char)0x74,  /* [2769] */
    (xdc_Char)0x61,  /* [2770] */
    (xdc_Char)0x63,  /* [2771] */
    (xdc_Char)0x6b,  /* [2772] */
    (xdc_Char)0x4f,  /* [2773] */
    (xdc_Char)0x76,  /* [2774] */
    (xdc_Char)0x65,  /* [2775] */
    (xdc_Char)0x72,  /* [2776] */
    (xdc_Char)0x66,  /* [2777] */
    (xdc_Char)0x6c,  /* [2778] */
    (xdc_Char)0x6f,  /* [2779] */
    (xdc_Char)0x77,  /* [2780] */
    (xdc_Char)0x3a,  /* [2781] */
    (xdc_Char)0x20,  /* [2782] */
    (xdc_Char)0x54,  /* [2783] */
    (xdc_Char)0x61,  /* [2784] */
    (xdc_Char)0x73,  /* [2785] */
    (xdc_Char)0x6b,  /* [2786] */
    (xdc_Char)0x20,  /* [2787] */
    (xdc_Char)0x30,  /* [2788] */
    (xdc_Char)0x78,  /* [2789] */
    (xdc_Char)0x25,  /* [2790] */
    (xdc_Char)0x78,  /* [2791] */
    (xdc_Char)0x20,  /* [2792] */
    (xdc_Char)0x73,  /* [2793] */
    (xdc_Char)0x74,  /* [2794] */
    (xdc_Char)0x61,  /* [2795] */
    (xdc_Char)0x63,  /* [2796] */
    (xdc_Char)0x6b,  /* [2797] */
    (xdc_Char)0x20,  /* [2798] */
    (xdc_Char)0x6f,  /* [2799] */
    (xdc_Char)0x76,  /* [2800] */
    (xdc_Char)0x65,  /* [2801] */
    (xdc_Char)0x72,  /* [2802] */
    (xdc_Char)0x66,  /* [2803] */
    (xdc_Char)0x6c,  /* [2804] */
    (xdc_Char)0x6f,  /* [2805] */
    (xdc_Char)0x77,  /* [2806] */
    (xdc_Char)0x2e,  /* [2807] */
    (xdc_Char)0x0,  /* [2808] */
    (xdc_Char)0x45,  /* [2809] */
    (xdc_Char)0x5f,  /* [2810] */
    (xdc_Char)0x73,  /* [2811] */
    (xdc_Char)0x70,  /* [2812] */
    (xdc_Char)0x4f,  /* [2813] */
    (xdc_Char)0x75,  /* [2814] */
    (xdc_Char)0x74,  /* [2815] */
    (xdc_Char)0x4f,  /* [2816] */
    (xdc_Char)0x66,  /* [2817] */
    (xdc_Char)0x42,  /* [2818] */
    (xdc_Char)0x6f,  /* [2819] */
    (xdc_Char)0x75,  /* [2820] */
    (xdc_Char)0x6e,  /* [2821] */
    (xdc_Char)0x64,  /* [2822] */
    (xdc_Char)0x73,  /* [2823] */
    (xdc_Char)0x3a,  /* [2824] */
    (xdc_Char)0x20,  /* [2825] */
    (xdc_Char)0x54,  /* [2826] */
    (xdc_Char)0x61,  /* [2827] */
    (xdc_Char)0x73,  /* [2828] */
    (xdc_Char)0x6b,  /* [2829] */
    (xdc_Char)0x20,  /* [2830] */
    (xdc_Char)0x30,  /* [2831] */
    (xdc_Char)0x78,  /* [2832] */
    (xdc_Char)0x25,  /* [2833] */
    (xdc_Char)0x78,  /* [2834] */
    (xdc_Char)0x20,  /* [2835] */
    (xdc_Char)0x73,  /* [2836] */
    (xdc_Char)0x74,  /* [2837] */
    (xdc_Char)0x61,  /* [2838] */
    (xdc_Char)0x63,  /* [2839] */
    (xdc_Char)0x6b,  /* [2840] */
    (xdc_Char)0x20,  /* [2841] */
    (xdc_Char)0x65,  /* [2842] */
    (xdc_Char)0x72,  /* [2843] */
    (xdc_Char)0x72,  /* [2844] */
    (xdc_Char)0x6f,  /* [2845] */
    (xdc_Char)0x72,  /* [2846] */
    (xdc_Char)0x2c,  /* [2847] */
    (xdc_Char)0x20,  /* [2848] */
    (xdc_Char)0x53,  /* [2849] */
    (xdc_Char)0x50,  /* [2850] */
    (xdc_Char)0x20,  /* [2851] */
    (xdc_Char)0x3d,  /* [2852] */
    (xdc_Char)0x20,  /* [2853] */
    (xdc_Char)0x30,  /* [2854] */
    (xdc_Char)0x78,  /* [2855] */
    (xdc_Char)0x25,  /* [2856] */
    (xdc_Char)0x78,  /* [2857] */
    (xdc_Char)0x2e,  /* [2858] */
    (xdc_Char)0x0,  /* [2859] */
    (xdc_Char)0x45,  /* [2860] */
    (xdc_Char)0x5f,  /* [2861] */
    (xdc_Char)0x61,  /* [2862] */
    (xdc_Char)0x6c,  /* [2863] */
    (xdc_Char)0x72,  /* [2864] */
    (xdc_Char)0x65,  /* [2865] */
    (xdc_Char)0x61,  /* [2866] */
    (xdc_Char)0x64,  /* [2867] */
    (xdc_Char)0x79,  /* [2868] */
    (xdc_Char)0x44,  /* [2869] */
    (xdc_Char)0x65,  /* [2870] */
    (xdc_Char)0x66,  /* [2871] */
    (xdc_Char)0x69,  /* [2872] */
    (xdc_Char)0x6e,  /* [2873] */
    (xdc_Char)0x65,  /* [2874] */
    (xdc_Char)0x64,  /* [2875] */
    (xdc_Char)0x3a,  /* [2876] */
    (xdc_Char)0x20,  /* [2877] */
    (xdc_Char)0x48,  /* [2878] */
    (xdc_Char)0x77,  /* [2879] */
    (xdc_Char)0x69,  /* [2880] */
    (xdc_Char)0x20,  /* [2881] */
    (xdc_Char)0x61,  /* [2882] */
    (xdc_Char)0x6c,  /* [2883] */
    (xdc_Char)0x72,  /* [2884] */
    (xdc_Char)0x65,  /* [2885] */
    (xdc_Char)0x61,  /* [2886] */
    (xdc_Char)0x64,  /* [2887] */
    (xdc_Char)0x79,  /* [2888] */
    (xdc_Char)0x20,  /* [2889] */
    (xdc_Char)0x64,  /* [2890] */
    (xdc_Char)0x65,  /* [2891] */
    (xdc_Char)0x66,  /* [2892] */
    (xdc_Char)0x69,  /* [2893] */
    (xdc_Char)0x6e,  /* [2894] */
    (xdc_Char)0x65,  /* [2895] */
    (xdc_Char)0x64,  /* [2896] */
    (xdc_Char)0x3a,  /* [2897] */
    (xdc_Char)0x20,  /* [2898] */
    (xdc_Char)0x69,  /* [2899] */
    (xdc_Char)0x6e,  /* [2900] */
    (xdc_Char)0x74,  /* [2901] */
    (xdc_Char)0x72,  /* [2902] */
    (xdc_Char)0x23,  /* [2903] */
    (xdc_Char)0x20,  /* [2904] */
    (xdc_Char)0x25,  /* [2905] */
    (xdc_Char)0x64,  /* [2906] */
    (xdc_Char)0x0,  /* [2907] */
    (xdc_Char)0x45,  /* [2908] */
    (xdc_Char)0x5f,  /* [2909] */
    (xdc_Char)0x65,  /* [2910] */
    (xdc_Char)0x78,  /* [2911] */
    (xdc_Char)0x63,  /* [2912] */
    (xdc_Char)0x65,  /* [2913] */
    (xdc_Char)0x70,  /* [2914] */
    (xdc_Char)0x74,  /* [2915] */
    (xdc_Char)0x69,  /* [2916] */
    (xdc_Char)0x6f,  /* [2917] */
    (xdc_Char)0x6e,  /* [2918] */
    (xdc_Char)0x3a,  /* [2919] */
    (xdc_Char)0x20,  /* [2920] */
    (xdc_Char)0x69,  /* [2921] */
    (xdc_Char)0x64,  /* [2922] */
    (xdc_Char)0x20,  /* [2923] */
    (xdc_Char)0x3d,  /* [2924] */
    (xdc_Char)0x20,  /* [2925] */
    (xdc_Char)0x25,  /* [2926] */
    (xdc_Char)0x64,  /* [2927] */
    (xdc_Char)0x2c,  /* [2928] */
    (xdc_Char)0x20,  /* [2929] */
    (xdc_Char)0x70,  /* [2930] */
    (xdc_Char)0x63,  /* [2931] */
    (xdc_Char)0x20,  /* [2932] */
    (xdc_Char)0x3d,  /* [2933] */
    (xdc_Char)0x20,  /* [2934] */
    (xdc_Char)0x25,  /* [2935] */
    (xdc_Char)0x30,  /* [2936] */
    (xdc_Char)0x38,  /* [2937] */
    (xdc_Char)0x78,  /* [2938] */
    (xdc_Char)0x2e,  /* [2939] */
    (xdc_Char)0xa,  /* [2940] */
    (xdc_Char)0x54,  /* [2941] */
    (xdc_Char)0x6f,  /* [2942] */
    (xdc_Char)0x20,  /* [2943] */
    (xdc_Char)0x73,  /* [2944] */
    (xdc_Char)0x65,  /* [2945] */
    (xdc_Char)0x65,  /* [2946] */
    (xdc_Char)0x20,  /* [2947] */
    (xdc_Char)0x6d,  /* [2948] */
    (xdc_Char)0x6f,  /* [2949] */
    (xdc_Char)0x72,  /* [2950] */
    (xdc_Char)0x65,  /* [2951] */
    (xdc_Char)0x20,  /* [2952] */
    (xdc_Char)0x65,  /* [2953] */
    (xdc_Char)0x78,  /* [2954] */
    (xdc_Char)0x63,  /* [2955] */
    (xdc_Char)0x65,  /* [2956] */
    (xdc_Char)0x70,  /* [2957] */
    (xdc_Char)0x74,  /* [2958] */
    (xdc_Char)0x69,  /* [2959] */
    (xdc_Char)0x6f,  /* [2960] */
    (xdc_Char)0x6e,  /* [2961] */
    (xdc_Char)0x20,  /* [2962] */
    (xdc_Char)0x64,  /* [2963] */
    (xdc_Char)0x65,  /* [2964] */
    (xdc_Char)0x74,  /* [2965] */
    (xdc_Char)0x61,  /* [2966] */
    (xdc_Char)0x69,  /* [2967] */
    (xdc_Char)0x6c,  /* [2968] */
    (xdc_Char)0x2c,  /* [2969] */
    (xdc_Char)0x20,  /* [2970] */
    (xdc_Char)0x73,  /* [2971] */
    (xdc_Char)0x65,  /* [2972] */
    (xdc_Char)0x74,  /* [2973] */
    (xdc_Char)0x20,  /* [2974] */
    (xdc_Char)0x74,  /* [2975] */
    (xdc_Char)0x69,  /* [2976] */
    (xdc_Char)0x2e,  /* [2977] */
    (xdc_Char)0x73,  /* [2978] */
    (xdc_Char)0x79,  /* [2979] */
    (xdc_Char)0x73,  /* [2980] */
    (xdc_Char)0x62,  /* [2981] */
    (xdc_Char)0x69,  /* [2982] */
    (xdc_Char)0x6f,  /* [2983] */
    (xdc_Char)0x73,  /* [2984] */
    (xdc_Char)0x2e,  /* [2985] */
    (xdc_Char)0x66,  /* [2986] */
    (xdc_Char)0x61,  /* [2987] */
    (xdc_Char)0x6d,  /* [2988] */
    (xdc_Char)0x69,  /* [2989] */
    (xdc_Char)0x6c,  /* [2990] */
    (xdc_Char)0x79,  /* [2991] */
    (xdc_Char)0x2e,  /* [2992] */
    (xdc_Char)0x61,  /* [2993] */
    (xdc_Char)0x72,  /* [2994] */
    (xdc_Char)0x6d,  /* [2995] */
    (xdc_Char)0x2e,  /* [2996] */
    (xdc_Char)0x6d,  /* [2997] */
    (xdc_Char)0x33,  /* [2998] */
    (xdc_Char)0x2e,  /* [2999] */
    (xdc_Char)0x48,  /* [3000] */
    (xdc_Char)0x77,  /* [3001] */
    (xdc_Char)0x69,  /* [3002] */
    (xdc_Char)0x2e,  /* [3003] */
    (xdc_Char)0x65,  /* [3004] */
    (xdc_Char)0x6e,  /* [3005] */
    (xdc_Char)0x61,  /* [3006] */
    (xdc_Char)0x62,  /* [3007] */
    (xdc_Char)0x6c,  /* [3008] */
    (xdc_Char)0x65,  /* [3009] */
    (xdc_Char)0x45,  /* [3010] */
    (xdc_Char)0x78,  /* [3011] */
    (xdc_Char)0x63,  /* [3012] */
    (xdc_Char)0x65,  /* [3013] */
    (xdc_Char)0x70,  /* [3014] */
    (xdc_Char)0x74,  /* [3015] */
    (xdc_Char)0x69,  /* [3016] */
    (xdc_Char)0x6f,  /* [3017] */
    (xdc_Char)0x6e,  /* [3018] */
    (xdc_Char)0x20,  /* [3019] */
    (xdc_Char)0x3d,  /* [3020] */
    (xdc_Char)0x20,  /* [3021] */
    (xdc_Char)0x74,  /* [3022] */
    (xdc_Char)0x72,  /* [3023] */
    (xdc_Char)0x75,  /* [3024] */
    (xdc_Char)0x65,  /* [3025] */
    (xdc_Char)0x20,  /* [3026] */
    (xdc_Char)0x6f,  /* [3027] */
    (xdc_Char)0x72,  /* [3028] */
    (xdc_Char)0x2c,  /* [3029] */
    (xdc_Char)0xa,  /* [3030] */
    (xdc_Char)0x65,  /* [3031] */
    (xdc_Char)0x78,  /* [3032] */
    (xdc_Char)0x61,  /* [3033] */
    (xdc_Char)0x6d,  /* [3034] */
    (xdc_Char)0x69,  /* [3035] */
    (xdc_Char)0x6e,  /* [3036] */
    (xdc_Char)0x65,  /* [3037] */
    (xdc_Char)0x20,  /* [3038] */
    (xdc_Char)0x74,  /* [3039] */
    (xdc_Char)0x68,  /* [3040] */
    (xdc_Char)0x65,  /* [3041] */
    (xdc_Char)0x20,  /* [3042] */
    (xdc_Char)0x45,  /* [3043] */
    (xdc_Char)0x78,  /* [3044] */
    (xdc_Char)0x63,  /* [3045] */
    (xdc_Char)0x65,  /* [3046] */
    (xdc_Char)0x70,  /* [3047] */
    (xdc_Char)0x74,  /* [3048] */
    (xdc_Char)0x69,  /* [3049] */
    (xdc_Char)0x6f,  /* [3050] */
    (xdc_Char)0x6e,  /* [3051] */
    (xdc_Char)0x20,  /* [3052] */
    (xdc_Char)0x76,  /* [3053] */
    (xdc_Char)0x69,  /* [3054] */
    (xdc_Char)0x65,  /* [3055] */
    (xdc_Char)0x77,  /* [3056] */
    (xdc_Char)0x20,  /* [3057] */
    (xdc_Char)0x66,  /* [3058] */
    (xdc_Char)0x6f,  /* [3059] */
    (xdc_Char)0x72,  /* [3060] */
    (xdc_Char)0x20,  /* [3061] */
    (xdc_Char)0x74,  /* [3062] */
    (xdc_Char)0x68,  /* [3063] */
    (xdc_Char)0x65,  /* [3064] */
    (xdc_Char)0x20,  /* [3065] */
    (xdc_Char)0x74,  /* [3066] */
    (xdc_Char)0x69,  /* [3067] */
    (xdc_Char)0x2e,  /* [3068] */
    (xdc_Char)0x73,  /* [3069] */
    (xdc_Char)0x79,  /* [3070] */
    (xdc_Char)0x73,  /* [3071] */
    (xdc_Char)0x62,  /* [3072] */
    (xdc_Char)0x69,  /* [3073] */
    (xdc_Char)0x6f,  /* [3074] */
    (xdc_Char)0x73,  /* [3075] */
    (xdc_Char)0x2e,  /* [3076] */
    (xdc_Char)0x66,  /* [3077] */
    (xdc_Char)0x61,  /* [3078] */
    (xdc_Char)0x6d,  /* [3079] */
    (xdc_Char)0x69,  /* [3080] */
    (xdc_Char)0x6c,  /* [3081] */
    (xdc_Char)0x79,  /* [3082] */
    (xdc_Char)0x2e,  /* [3083] */
    (xdc_Char)0x61,  /* [3084] */
    (xdc_Char)0x72,  /* [3085] */
    (xdc_Char)0x6d,  /* [3086] */
    (xdc_Char)0x2e,  /* [3087] */
    (xdc_Char)0x6d,  /* [3088] */
    (xdc_Char)0x33,  /* [3089] */
    (xdc_Char)0x2e,  /* [3090] */
    (xdc_Char)0x48,  /* [3091] */
    (xdc_Char)0x77,  /* [3092] */
    (xdc_Char)0x69,  /* [3093] */
    (xdc_Char)0x20,  /* [3094] */
    (xdc_Char)0x6d,  /* [3095] */
    (xdc_Char)0x6f,  /* [3096] */
    (xdc_Char)0x64,  /* [3097] */
    (xdc_Char)0x75,  /* [3098] */
    (xdc_Char)0x6c,  /* [3099] */
    (xdc_Char)0x65,  /* [3100] */
    (xdc_Char)0x20,  /* [3101] */
    (xdc_Char)0x75,  /* [3102] */
    (xdc_Char)0x73,  /* [3103] */
    (xdc_Char)0x69,  /* [3104] */
    (xdc_Char)0x6e,  /* [3105] */
    (xdc_Char)0x67,  /* [3106] */
    (xdc_Char)0x20,  /* [3107] */
    (xdc_Char)0x52,  /* [3108] */
    (xdc_Char)0x4f,  /* [3109] */
    (xdc_Char)0x56,  /* [3110] */
    (xdc_Char)0x2e,  /* [3111] */
    (xdc_Char)0x0,  /* [3112] */
    (xdc_Char)0x45,  /* [3113] */
    (xdc_Char)0x5f,  /* [3114] */
    (xdc_Char)0x6e,  /* [3115] */
    (xdc_Char)0x6f,  /* [3116] */
    (xdc_Char)0x49,  /* [3117] */
    (xdc_Char)0x73,  /* [3118] */
    (xdc_Char)0x72,  /* [3119] */
    (xdc_Char)0x3a,  /* [3120] */
    (xdc_Char)0x20,  /* [3121] */
    (xdc_Char)0x69,  /* [3122] */
    (xdc_Char)0x64,  /* [3123] */
    (xdc_Char)0x20,  /* [3124] */
    (xdc_Char)0x3d,  /* [3125] */
    (xdc_Char)0x20,  /* [3126] */
    (xdc_Char)0x25,  /* [3127] */
    (xdc_Char)0x64,  /* [3128] */
    (xdc_Char)0x2c,  /* [3129] */
    (xdc_Char)0x20,  /* [3130] */
    (xdc_Char)0x70,  /* [3131] */
    (xdc_Char)0x63,  /* [3132] */
    (xdc_Char)0x20,  /* [3133] */
    (xdc_Char)0x3d,  /* [3134] */
    (xdc_Char)0x20,  /* [3135] */
    (xdc_Char)0x25,  /* [3136] */
    (xdc_Char)0x30,  /* [3137] */
    (xdc_Char)0x38,  /* [3138] */
    (xdc_Char)0x78,  /* [3139] */
    (xdc_Char)0x0,  /* [3140] */
    (xdc_Char)0x45,  /* [3141] */
    (xdc_Char)0x5f,  /* [3142] */
    (xdc_Char)0x4e,  /* [3143] */
    (xdc_Char)0x4d,  /* [3144] */
    (xdc_Char)0x49,  /* [3145] */
    (xdc_Char)0x3a,  /* [3146] */
    (xdc_Char)0x20,  /* [3147] */
    (xdc_Char)0x25,  /* [3148] */
    (xdc_Char)0x73,  /* [3149] */
    (xdc_Char)0x0,  /* [3150] */
    (xdc_Char)0x45,  /* [3151] */
    (xdc_Char)0x5f,  /* [3152] */
    (xdc_Char)0x68,  /* [3153] */
    (xdc_Char)0x61,  /* [3154] */
    (xdc_Char)0x72,  /* [3155] */
    (xdc_Char)0x64,  /* [3156] */
    (xdc_Char)0x46,  /* [3157] */
    (xdc_Char)0x61,  /* [3158] */
    (xdc_Char)0x75,  /* [3159] */
    (xdc_Char)0x6c,  /* [3160] */
    (xdc_Char)0x74,  /* [3161] */
    (xdc_Char)0x3a,  /* [3162] */
    (xdc_Char)0x20,  /* [3163] */
    (xdc_Char)0x25,  /* [3164] */
    (xdc_Char)0x73,  /* [3165] */
    (xdc_Char)0x0,  /* [3166] */
    (xdc_Char)0x45,  /* [3167] */
    (xdc_Char)0x5f,  /* [3168] */
    (xdc_Char)0x6d,  /* [3169] */
    (xdc_Char)0x65,  /* [3170] */
    (xdc_Char)0x6d,  /* [3171] */
    (xdc_Char)0x46,  /* [3172] */
    (xdc_Char)0x61,  /* [3173] */
    (xdc_Char)0x75,  /* [3174] */
    (xdc_Char)0x6c,  /* [3175] */
    (xdc_Char)0x74,  /* [3176] */
    (xdc_Char)0x3a,  /* [3177] */
    (xdc_Char)0x20,  /* [3178] */
    (xdc_Char)0x25,  /* [3179] */
    (xdc_Char)0x73,  /* [3180] */
    (xdc_Char)0x0,  /* [3181] */
    (xdc_Char)0x45,  /* [3182] */
    (xdc_Char)0x5f,  /* [3183] */
    (xdc_Char)0x62,  /* [3184] */
    (xdc_Char)0x75,  /* [3185] */
    (xdc_Char)0x73,  /* [3186] */
    (xdc_Char)0x46,  /* [3187] */
    (xdc_Char)0x61,  /* [3188] */
    (xdc_Char)0x75,  /* [3189] */
    (xdc_Char)0x6c,  /* [3190] */
    (xdc_Char)0x74,  /* [3191] */
    (xdc_Char)0x3a,  /* [3192] */
    (xdc_Char)0x20,  /* [3193] */
    (xdc_Char)0x25,  /* [3194] */
    (xdc_Char)0x73,  /* [3195] */
    (xdc_Char)0x0,  /* [3196] */
    (xdc_Char)0x45,  /* [3197] */
    (xdc_Char)0x5f,  /* [3198] */
    (xdc_Char)0x75,  /* [3199] */
    (xdc_Char)0x73,  /* [3200] */
    (xdc_Char)0x61,  /* [3201] */
    (xdc_Char)0x67,  /* [3202] */
    (xdc_Char)0x65,  /* [3203] */
    (xdc_Char)0x46,  /* [3204] */
    (xdc_Char)0x61,  /* [3205] */
    (xdc_Char)0x75,  /* [3206] */
    (xdc_Char)0x6c,  /* [3207] */
    (xdc_Char)0x74,  /* [3208] */
    (xdc_Char)0x3a,  /* [3209] */
    (xdc_Char)0x20,  /* [3210] */
    (xdc_Char)0x25,  /* [3211] */
    (xdc_Char)0x73,  /* [3212] */
    (xdc_Char)0x0,  /* [3213] */
    (xdc_Char)0x45,  /* [3214] */
    (xdc_Char)0x5f,  /* [3215] */
    (xdc_Char)0x73,  /* [3216] */
    (xdc_Char)0x76,  /* [3217] */
    (xdc_Char)0x43,  /* [3218] */
    (xdc_Char)0x61,  /* [3219] */
    (xdc_Char)0x6c,  /* [3220] */
    (xdc_Char)0x6c,  /* [3221] */
    (xdc_Char)0x3a,  /* [3222] */
    (xdc_Char)0x20,  /* [3223] */
    (xdc_Char)0x73,  /* [3224] */
    (xdc_Char)0x76,  /* [3225] */
    (xdc_Char)0x4e,  /* [3226] */
    (xdc_Char)0x75,  /* [3227] */
    (xdc_Char)0x6d,  /* [3228] */
    (xdc_Char)0x20,  /* [3229] */
    (xdc_Char)0x3d,  /* [3230] */
    (xdc_Char)0x20,  /* [3231] */
    (xdc_Char)0x25,  /* [3232] */
    (xdc_Char)0x64,  /* [3233] */
    (xdc_Char)0x0,  /* [3234] */
    (xdc_Char)0x45,  /* [3235] */
    (xdc_Char)0x5f,  /* [3236] */
    (xdc_Char)0x64,  /* [3237] */
    (xdc_Char)0x65,  /* [3238] */
    (xdc_Char)0x62,  /* [3239] */
    (xdc_Char)0x75,  /* [3240] */
    (xdc_Char)0x67,  /* [3241] */
    (xdc_Char)0x4d,  /* [3242] */
    (xdc_Char)0x6f,  /* [3243] */
    (xdc_Char)0x6e,  /* [3244] */
    (xdc_Char)0x3a,  /* [3245] */
    (xdc_Char)0x20,  /* [3246] */
    (xdc_Char)0x25,  /* [3247] */
    (xdc_Char)0x73,  /* [3248] */
    (xdc_Char)0x0,  /* [3249] */
    (xdc_Char)0x45,  /* [3250] */
    (xdc_Char)0x5f,  /* [3251] */
    (xdc_Char)0x72,  /* [3252] */
    (xdc_Char)0x65,  /* [3253] */
    (xdc_Char)0x73,  /* [3254] */
    (xdc_Char)0x65,  /* [3255] */
    (xdc_Char)0x72,  /* [3256] */
    (xdc_Char)0x76,  /* [3257] */
    (xdc_Char)0x65,  /* [3258] */
    (xdc_Char)0x64,  /* [3259] */
    (xdc_Char)0x3a,  /* [3260] */
    (xdc_Char)0x20,  /* [3261] */
    (xdc_Char)0x25,  /* [3262] */
    (xdc_Char)0x73,  /* [3263] */
    (xdc_Char)0x20,  /* [3264] */
    (xdc_Char)0x25,  /* [3265] */
    (xdc_Char)0x64,  /* [3266] */
    (xdc_Char)0x0,  /* [3267] */
    (xdc_Char)0x45,  /* [3268] */
    (xdc_Char)0x5f,  /* [3269] */
    (xdc_Char)0x69,  /* [3270] */
    (xdc_Char)0x6e,  /* [3271] */
    (xdc_Char)0x76,  /* [3272] */
    (xdc_Char)0x61,  /* [3273] */
    (xdc_Char)0x6c,  /* [3274] */
    (xdc_Char)0x69,  /* [3275] */
    (xdc_Char)0x64,  /* [3276] */
    (xdc_Char)0x54,  /* [3277] */
    (xdc_Char)0x69,  /* [3278] */
    (xdc_Char)0x6d,  /* [3279] */
    (xdc_Char)0x65,  /* [3280] */
    (xdc_Char)0x72,  /* [3281] */
    (xdc_Char)0x3a,  /* [3282] */
    (xdc_Char)0x20,  /* [3283] */
    (xdc_Char)0x49,  /* [3284] */
    (xdc_Char)0x6e,  /* [3285] */
    (xdc_Char)0x76,  /* [3286] */
    (xdc_Char)0x61,  /* [3287] */
    (xdc_Char)0x6c,  /* [3288] */
    (xdc_Char)0x69,  /* [3289] */
    (xdc_Char)0x64,  /* [3290] */
    (xdc_Char)0x20,  /* [3291] */
    (xdc_Char)0x54,  /* [3292] */
    (xdc_Char)0x69,  /* [3293] */
    (xdc_Char)0x6d,  /* [3294] */
    (xdc_Char)0x65,  /* [3295] */
    (xdc_Char)0x72,  /* [3296] */
    (xdc_Char)0x20,  /* [3297] */
    (xdc_Char)0x49,  /* [3298] */
    (xdc_Char)0x64,  /* [3299] */
    (xdc_Char)0x20,  /* [3300] */
    (xdc_Char)0x25,  /* [3301] */
    (xdc_Char)0x64,  /* [3302] */
    (xdc_Char)0x0,  /* [3303] */
    (xdc_Char)0x45,  /* [3304] */
    (xdc_Char)0x5f,  /* [3305] */
    (xdc_Char)0x6e,  /* [3306] */
    (xdc_Char)0x6f,  /* [3307] */
    (xdc_Char)0x74,  /* [3308] */
    (xdc_Char)0x41,  /* [3309] */
    (xdc_Char)0x76,  /* [3310] */
    (xdc_Char)0x61,  /* [3311] */
    (xdc_Char)0x69,  /* [3312] */
    (xdc_Char)0x6c,  /* [3313] */
    (xdc_Char)0x61,  /* [3314] */
    (xdc_Char)0x62,  /* [3315] */
    (xdc_Char)0x6c,  /* [3316] */
    (xdc_Char)0x65,  /* [3317] */
    (xdc_Char)0x3a,  /* [3318] */
    (xdc_Char)0x20,  /* [3319] */
    (xdc_Char)0x54,  /* [3320] */
    (xdc_Char)0x69,  /* [3321] */
    (xdc_Char)0x6d,  /* [3322] */
    (xdc_Char)0x65,  /* [3323] */
    (xdc_Char)0x72,  /* [3324] */
    (xdc_Char)0x20,  /* [3325] */
    (xdc_Char)0x6e,  /* [3326] */
    (xdc_Char)0x6f,  /* [3327] */
    (xdc_Char)0x74,  /* [3328] */
    (xdc_Char)0x20,  /* [3329] */
    (xdc_Char)0x61,  /* [3330] */
    (xdc_Char)0x76,  /* [3331] */
    (xdc_Char)0x61,  /* [3332] */
    (xdc_Char)0x69,  /* [3333] */
    (xdc_Char)0x6c,  /* [3334] */
    (xdc_Char)0x61,  /* [3335] */
    (xdc_Char)0x62,  /* [3336] */
    (xdc_Char)0x6c,  /* [3337] */
    (xdc_Char)0x65,  /* [3338] */
    (xdc_Char)0x20,  /* [3339] */
    (xdc_Char)0x25,  /* [3340] */
    (xdc_Char)0x64,  /* [3341] */
    (xdc_Char)0x0,  /* [3342] */
    (xdc_Char)0x45,  /* [3343] */
    (xdc_Char)0x5f,  /* [3344] */
    (xdc_Char)0x63,  /* [3345] */
    (xdc_Char)0x61,  /* [3346] */
    (xdc_Char)0x6e,  /* [3347] */
    (xdc_Char)0x6e,  /* [3348] */
    (xdc_Char)0x6f,  /* [3349] */
    (xdc_Char)0x74,  /* [3350] */
    (xdc_Char)0x53,  /* [3351] */
    (xdc_Char)0x75,  /* [3352] */
    (xdc_Char)0x70,  /* [3353] */
    (xdc_Char)0x70,  /* [3354] */
    (xdc_Char)0x6f,  /* [3355] */
    (xdc_Char)0x72,  /* [3356] */
    (xdc_Char)0x74,  /* [3357] */
    (xdc_Char)0x3a,  /* [3358] */
    (xdc_Char)0x20,  /* [3359] */
    (xdc_Char)0x54,  /* [3360] */
    (xdc_Char)0x69,  /* [3361] */
    (xdc_Char)0x6d,  /* [3362] */
    (xdc_Char)0x65,  /* [3363] */
    (xdc_Char)0x72,  /* [3364] */
    (xdc_Char)0x20,  /* [3365] */
    (xdc_Char)0x63,  /* [3366] */
    (xdc_Char)0x61,  /* [3367] */
    (xdc_Char)0x6e,  /* [3368] */
    (xdc_Char)0x6e,  /* [3369] */
    (xdc_Char)0x6f,  /* [3370] */
    (xdc_Char)0x74,  /* [3371] */
    (xdc_Char)0x20,  /* [3372] */
    (xdc_Char)0x73,  /* [3373] */
    (xdc_Char)0x75,  /* [3374] */
    (xdc_Char)0x70,  /* [3375] */
    (xdc_Char)0x70,  /* [3376] */
    (xdc_Char)0x6f,  /* [3377] */
    (xdc_Char)0x72,  /* [3378] */
    (xdc_Char)0x74,  /* [3379] */
    (xdc_Char)0x20,  /* [3380] */
    (xdc_Char)0x72,  /* [3381] */
    (xdc_Char)0x65,  /* [3382] */
    (xdc_Char)0x71,  /* [3383] */
    (xdc_Char)0x75,  /* [3384] */
    (xdc_Char)0x65,  /* [3385] */
    (xdc_Char)0x73,  /* [3386] */
    (xdc_Char)0x74,  /* [3387] */
    (xdc_Char)0x65,  /* [3388] */
    (xdc_Char)0x64,  /* [3389] */
    (xdc_Char)0x20,  /* [3390] */
    (xdc_Char)0x70,  /* [3391] */
    (xdc_Char)0x65,  /* [3392] */
    (xdc_Char)0x72,  /* [3393] */
    (xdc_Char)0x69,  /* [3394] */
    (xdc_Char)0x6f,  /* [3395] */
    (xdc_Char)0x64,  /* [3396] */
    (xdc_Char)0x20,  /* [3397] */
    (xdc_Char)0x25,  /* [3398] */
    (xdc_Char)0x64,  /* [3399] */
    (xdc_Char)0x0,  /* [3400] */
    (xdc_Char)0x45,  /* [3401] */
    (xdc_Char)0x5f,  /* [3402] */
    (xdc_Char)0x6d,  /* [3403] */
    (xdc_Char)0x69,  /* [3404] */
    (xdc_Char)0x73,  /* [3405] */
    (xdc_Char)0x6d,  /* [3406] */
    (xdc_Char)0x61,  /* [3407] */
    (xdc_Char)0x74,  /* [3408] */
    (xdc_Char)0x63,  /* [3409] */
    (xdc_Char)0x68,  /* [3410] */
    (xdc_Char)0x65,  /* [3411] */
    (xdc_Char)0x64,  /* [3412] */
    (xdc_Char)0x49,  /* [3413] */
    (xdc_Char)0x64,  /* [3414] */
    (xdc_Char)0x73,  /* [3415] */
    (xdc_Char)0x3a,  /* [3416] */
    (xdc_Char)0x20,  /* [3417] */
    (xdc_Char)0x43,  /* [3418] */
    (xdc_Char)0x6f,  /* [3419] */
    (xdc_Char)0x72,  /* [3420] */
    (xdc_Char)0x65,  /* [3421] */
    (xdc_Char)0x5f,  /* [3422] */
    (xdc_Char)0x49,  /* [3423] */
    (xdc_Char)0x64,  /* [3424] */
    (xdc_Char)0x3a,  /* [3425] */
    (xdc_Char)0x20,  /* [3426] */
    (xdc_Char)0x25,  /* [3427] */
    (xdc_Char)0x64,  /* [3428] */
    (xdc_Char)0x20,  /* [3429] */
    (xdc_Char)0x64,  /* [3430] */
    (xdc_Char)0x6f,  /* [3431] */
    (xdc_Char)0x65,  /* [3432] */
    (xdc_Char)0x73,  /* [3433] */
    (xdc_Char)0x20,  /* [3434] */
    (xdc_Char)0x6e,  /* [3435] */
    (xdc_Char)0x6f,  /* [3436] */
    (xdc_Char)0x74,  /* [3437] */
    (xdc_Char)0x20,  /* [3438] */
    (xdc_Char)0x6d,  /* [3439] */
    (xdc_Char)0x61,  /* [3440] */
    (xdc_Char)0x74,  /* [3441] */
    (xdc_Char)0x63,  /* [3442] */
    (xdc_Char)0x68,  /* [3443] */
    (xdc_Char)0x20,  /* [3444] */
    (xdc_Char)0x68,  /* [3445] */
    (xdc_Char)0x61,  /* [3446] */
    (xdc_Char)0x72,  /* [3447] */
    (xdc_Char)0x64,  /* [3448] */
    (xdc_Char)0x77,  /* [3449] */
    (xdc_Char)0x61,  /* [3450] */
    (xdc_Char)0x72,  /* [3451] */
    (xdc_Char)0x65,  /* [3452] */
    (xdc_Char)0x20,  /* [3453] */
    (xdc_Char)0x63,  /* [3454] */
    (xdc_Char)0x6f,  /* [3455] */
    (xdc_Char)0x72,  /* [3456] */
    (xdc_Char)0x65,  /* [3457] */
    (xdc_Char)0x20,  /* [3458] */
    (xdc_Char)0x49,  /* [3459] */
    (xdc_Char)0x64,  /* [3460] */
    (xdc_Char)0x3a,  /* [3461] */
    (xdc_Char)0x20,  /* [3462] */
    (xdc_Char)0x25,  /* [3463] */
    (xdc_Char)0x64,  /* [3464] */
    (xdc_Char)0x0,  /* [3465] */
    (xdc_Char)0x45,  /* [3466] */
    (xdc_Char)0x5f,  /* [3467] */
    (xdc_Char)0x69,  /* [3468] */
    (xdc_Char)0x6e,  /* [3469] */
    (xdc_Char)0x76,  /* [3470] */
    (xdc_Char)0x61,  /* [3471] */
    (xdc_Char)0x6c,  /* [3472] */
    (xdc_Char)0x69,  /* [3473] */
    (xdc_Char)0x64,  /* [3474] */
    (xdc_Char)0x49,  /* [3475] */
    (xdc_Char)0x6e,  /* [3476] */
    (xdc_Char)0x64,  /* [3477] */
    (xdc_Char)0x65,  /* [3478] */
    (xdc_Char)0x78,  /* [3479] */
    (xdc_Char)0x20,  /* [3480] */
    (xdc_Char)0x25,  /* [3481] */
    (xdc_Char)0x64,  /* [3482] */
    (xdc_Char)0x0,  /* [3483] */
    (xdc_Char)0x45,  /* [3484] */
    (xdc_Char)0x5f,  /* [3485] */
    (xdc_Char)0x67,  /* [3486] */
    (xdc_Char)0x61,  /* [3487] */
    (xdc_Char)0x74,  /* [3488] */
    (xdc_Char)0x65,  /* [3489] */
    (xdc_Char)0x49,  /* [3490] */
    (xdc_Char)0x6e,  /* [3491] */
    (xdc_Char)0x55,  /* [3492] */
    (xdc_Char)0x73,  /* [3493] */
    (xdc_Char)0x65,  /* [3494] */
    (xdc_Char)0x20,  /* [3495] */
    (xdc_Char)0x25,  /* [3496] */
    (xdc_Char)0x64,  /* [3497] */
    (xdc_Char)0x0,  /* [3498] */
    (xdc_Char)0x72,  /* [3499] */
    (xdc_Char)0x65,  /* [3500] */
    (xdc_Char)0x71,  /* [3501] */
    (xdc_Char)0x75,  /* [3502] */
    (xdc_Char)0x65,  /* [3503] */
    (xdc_Char)0x73,  /* [3504] */
    (xdc_Char)0x74,  /* [3505] */
    (xdc_Char)0x65,  /* [3506] */
    (xdc_Char)0x64,  /* [3507] */
    (xdc_Char)0x20,  /* [3508] */
    (xdc_Char)0x73,  /* [3509] */
    (xdc_Char)0x69,  /* [3510] */
    (xdc_Char)0x7a,  /* [3511] */
    (xdc_Char)0x65,  /* [3512] */
    (xdc_Char)0x20,  /* [3513] */
    (xdc_Char)0x69,  /* [3514] */
    (xdc_Char)0x73,  /* [3515] */
    (xdc_Char)0x20,  /* [3516] */
    (xdc_Char)0x74,  /* [3517] */
    (xdc_Char)0x6f,  /* [3518] */
    (xdc_Char)0x6f,  /* [3519] */
    (xdc_Char)0x20,  /* [3520] */
    (xdc_Char)0x62,  /* [3521] */
    (xdc_Char)0x69,  /* [3522] */
    (xdc_Char)0x67,  /* [3523] */
    (xdc_Char)0x3a,  /* [3524] */
    (xdc_Char)0x20,  /* [3525] */
    (xdc_Char)0x68,  /* [3526] */
    (xdc_Char)0x61,  /* [3527] */
    (xdc_Char)0x6e,  /* [3528] */
    (xdc_Char)0x64,  /* [3529] */
    (xdc_Char)0x6c,  /* [3530] */
    (xdc_Char)0x65,  /* [3531] */
    (xdc_Char)0x3d,  /* [3532] */
    (xdc_Char)0x30,  /* [3533] */
    (xdc_Char)0x78,  /* [3534] */
    (xdc_Char)0x25,  /* [3535] */
    (xdc_Char)0x78,  /* [3536] */
    (xdc_Char)0x2c,  /* [3537] */
    (xdc_Char)0x20,  /* [3538] */
    (xdc_Char)0x73,  /* [3539] */
    (xdc_Char)0x69,  /* [3540] */
    (xdc_Char)0x7a,  /* [3541] */
    (xdc_Char)0x65,  /* [3542] */
    (xdc_Char)0x3d,  /* [3543] */
    (xdc_Char)0x25,  /* [3544] */
    (xdc_Char)0x75,  /* [3545] */
    (xdc_Char)0x0,  /* [3546] */
    (xdc_Char)0x6f,  /* [3547] */
    (xdc_Char)0x75,  /* [3548] */
    (xdc_Char)0x74,  /* [3549] */
    (xdc_Char)0x20,  /* [3550] */
    (xdc_Char)0x6f,  /* [3551] */
    (xdc_Char)0x66,  /* [3552] */
    (xdc_Char)0x20,  /* [3553] */
    (xdc_Char)0x6d,  /* [3554] */
    (xdc_Char)0x65,  /* [3555] */
    (xdc_Char)0x6d,  /* [3556] */
    (xdc_Char)0x6f,  /* [3557] */
    (xdc_Char)0x72,  /* [3558] */
    (xdc_Char)0x79,  /* [3559] */
    (xdc_Char)0x3a,  /* [3560] */
    (xdc_Char)0x20,  /* [3561] */
    (xdc_Char)0x68,  /* [3562] */
    (xdc_Char)0x61,  /* [3563] */
    (xdc_Char)0x6e,  /* [3564] */
    (xdc_Char)0x64,  /* [3565] */
    (xdc_Char)0x6c,  /* [3566] */
    (xdc_Char)0x65,  /* [3567] */
    (xdc_Char)0x3d,  /* [3568] */
    (xdc_Char)0x30,  /* [3569] */
    (xdc_Char)0x78,  /* [3570] */
    (xdc_Char)0x25,  /* [3571] */
    (xdc_Char)0x78,  /* [3572] */
    (xdc_Char)0x2c,  /* [3573] */
    (xdc_Char)0x20,  /* [3574] */
    (xdc_Char)0x73,  /* [3575] */
    (xdc_Char)0x69,  /* [3576] */
    (xdc_Char)0x7a,  /* [3577] */
    (xdc_Char)0x65,  /* [3578] */
    (xdc_Char)0x3d,  /* [3579] */
    (xdc_Char)0x25,  /* [3580] */
    (xdc_Char)0x75,  /* [3581] */
    (xdc_Char)0x0,  /* [3582] */
    (xdc_Char)0x45,  /* [3583] */
    (xdc_Char)0x5f,  /* [3584] */
    (xdc_Char)0x66,  /* [3585] */
    (xdc_Char)0x72,  /* [3586] */
    (xdc_Char)0x65,  /* [3587] */
    (xdc_Char)0x71,  /* [3588] */
    (xdc_Char)0x4d,  /* [3589] */
    (xdc_Char)0x69,  /* [3590] */
    (xdc_Char)0x73,  /* [3591] */
    (xdc_Char)0x6d,  /* [3592] */
    (xdc_Char)0x61,  /* [3593] */
    (xdc_Char)0x74,  /* [3594] */
    (xdc_Char)0x63,  /* [3595] */
    (xdc_Char)0x68,  /* [3596] */
    (xdc_Char)0x3a,  /* [3597] */
    (xdc_Char)0x20,  /* [3598] */
    (xdc_Char)0x46,  /* [3599] */
    (xdc_Char)0x72,  /* [3600] */
    (xdc_Char)0x65,  /* [3601] */
    (xdc_Char)0x71,  /* [3602] */
    (xdc_Char)0x75,  /* [3603] */
    (xdc_Char)0x65,  /* [3604] */
    (xdc_Char)0x6e,  /* [3605] */
    (xdc_Char)0x63,  /* [3606] */
    (xdc_Char)0x79,  /* [3607] */
    (xdc_Char)0x20,  /* [3608] */
    (xdc_Char)0x6d,  /* [3609] */
    (xdc_Char)0x69,  /* [3610] */
    (xdc_Char)0x73,  /* [3611] */
    (xdc_Char)0x6d,  /* [3612] */
    (xdc_Char)0x61,  /* [3613] */
    (xdc_Char)0x74,  /* [3614] */
    (xdc_Char)0x63,  /* [3615] */
    (xdc_Char)0x68,  /* [3616] */
    (xdc_Char)0x3a,  /* [3617] */
    (xdc_Char)0x20,  /* [3618] */
    (xdc_Char)0x45,  /* [3619] */
    (xdc_Char)0x78,  /* [3620] */
    (xdc_Char)0x70,  /* [3621] */
    (xdc_Char)0x65,  /* [3622] */
    (xdc_Char)0x63,  /* [3623] */
    (xdc_Char)0x74,  /* [3624] */
    (xdc_Char)0x65,  /* [3625] */
    (xdc_Char)0x64,  /* [3626] */
    (xdc_Char)0x20,  /* [3627] */
    (xdc_Char)0x25,  /* [3628] */
    (xdc_Char)0x64,  /* [3629] */
    (xdc_Char)0x20,  /* [3630] */
    (xdc_Char)0x48,  /* [3631] */
    (xdc_Char)0x7a,  /* [3632] */
    (xdc_Char)0x2c,  /* [3633] */
    (xdc_Char)0x20,  /* [3634] */
    (xdc_Char)0x61,  /* [3635] */
    (xdc_Char)0x63,  /* [3636] */
    (xdc_Char)0x74,  /* [3637] */
    (xdc_Char)0x75,  /* [3638] */
    (xdc_Char)0x61,  /* [3639] */
    (xdc_Char)0x6c,  /* [3640] */
    (xdc_Char)0x3a,  /* [3641] */
    (xdc_Char)0x20,  /* [3642] */
    (xdc_Char)0x25,  /* [3643] */
    (xdc_Char)0x64,  /* [3644] */
    (xdc_Char)0x20,  /* [3645] */
    (xdc_Char)0x48,  /* [3646] */
    (xdc_Char)0x7a,  /* [3647] */
    (xdc_Char)0x2e,  /* [3648] */
    (xdc_Char)0x20,  /* [3649] */
    (xdc_Char)0x20,  /* [3650] */
    (xdc_Char)0x59,  /* [3651] */
    (xdc_Char)0x6f,  /* [3652] */
    (xdc_Char)0x75,  /* [3653] */
    (xdc_Char)0x20,  /* [3654] */
    (xdc_Char)0x6e,  /* [3655] */
    (xdc_Char)0x65,  /* [3656] */
    (xdc_Char)0x65,  /* [3657] */
    (xdc_Char)0x64,  /* [3658] */
    (xdc_Char)0x20,  /* [3659] */
    (xdc_Char)0x74,  /* [3660] */
    (xdc_Char)0x6f,  /* [3661] */
    (xdc_Char)0x20,  /* [3662] */
    (xdc_Char)0x6d,  /* [3663] */
    (xdc_Char)0x6f,  /* [3664] */
    (xdc_Char)0x64,  /* [3665] */
    (xdc_Char)0x69,  /* [3666] */
    (xdc_Char)0x66,  /* [3667] */
    (xdc_Char)0x79,  /* [3668] */
    (xdc_Char)0x20,  /* [3669] */
    (xdc_Char)0x54,  /* [3670] */
    (xdc_Char)0x69,  /* [3671] */
    (xdc_Char)0x6d,  /* [3672] */
    (xdc_Char)0x65,  /* [3673] */
    (xdc_Char)0x72,  /* [3674] */
    (xdc_Char)0x2e,  /* [3675] */
    (xdc_Char)0x69,  /* [3676] */
    (xdc_Char)0x6e,  /* [3677] */
    (xdc_Char)0x74,  /* [3678] */
    (xdc_Char)0x46,  /* [3679] */
    (xdc_Char)0x72,  /* [3680] */
    (xdc_Char)0x65,  /* [3681] */
    (xdc_Char)0x71,  /* [3682] */
    (xdc_Char)0x2e,  /* [3683] */
    (xdc_Char)0x6c,  /* [3684] */
    (xdc_Char)0x6f,  /* [3685] */
    (xdc_Char)0x20,  /* [3686] */
    (xdc_Char)0x74,  /* [3687] */
    (xdc_Char)0x6f,  /* [3688] */
    (xdc_Char)0x20,  /* [3689] */
    (xdc_Char)0x6d,  /* [3690] */
    (xdc_Char)0x61,  /* [3691] */
    (xdc_Char)0x74,  /* [3692] */
    (xdc_Char)0x63,  /* [3693] */
    (xdc_Char)0x68,  /* [3694] */
    (xdc_Char)0x20,  /* [3695] */
    (xdc_Char)0x74,  /* [3696] */
    (xdc_Char)0x68,  /* [3697] */
    (xdc_Char)0x65,  /* [3698] */
    (xdc_Char)0x20,  /* [3699] */
    (xdc_Char)0x61,  /* [3700] */
    (xdc_Char)0x63,  /* [3701] */
    (xdc_Char)0x74,  /* [3702] */
    (xdc_Char)0x75,  /* [3703] */
    (xdc_Char)0x61,  /* [3704] */
    (xdc_Char)0x6c,  /* [3705] */
    (xdc_Char)0x20,  /* [3706] */
    (xdc_Char)0x66,  /* [3707] */
    (xdc_Char)0x72,  /* [3708] */
    (xdc_Char)0x65,  /* [3709] */
    (xdc_Char)0x71,  /* [3710] */
    (xdc_Char)0x75,  /* [3711] */
    (xdc_Char)0x65,  /* [3712] */
    (xdc_Char)0x6e,  /* [3713] */
    (xdc_Char)0x63,  /* [3714] */
    (xdc_Char)0x79,  /* [3715] */
    (xdc_Char)0x2e,  /* [3716] */
    (xdc_Char)0x0,  /* [3717] */
    (xdc_Char)0x45,  /* [3718] */
    (xdc_Char)0x5f,  /* [3719] */
    (xdc_Char)0x70,  /* [3720] */
    (xdc_Char)0x72,  /* [3721] */
    (xdc_Char)0x69,  /* [3722] */
    (xdc_Char)0x6f,  /* [3723] */
    (xdc_Char)0x72,  /* [3724] */
    (xdc_Char)0x69,  /* [3725] */
    (xdc_Char)0x74,  /* [3726] */
    (xdc_Char)0x79,  /* [3727] */
    (xdc_Char)0x3a,  /* [3728] */
    (xdc_Char)0x20,  /* [3729] */
    (xdc_Char)0x54,  /* [3730] */
    (xdc_Char)0x68,  /* [3731] */
    (xdc_Char)0x72,  /* [3732] */
    (xdc_Char)0x65,  /* [3733] */
    (xdc_Char)0x61,  /* [3734] */
    (xdc_Char)0x64,  /* [3735] */
    (xdc_Char)0x20,  /* [3736] */
    (xdc_Char)0x70,  /* [3737] */
    (xdc_Char)0x72,  /* [3738] */
    (xdc_Char)0x69,  /* [3739] */
    (xdc_Char)0x6f,  /* [3740] */
    (xdc_Char)0x72,  /* [3741] */
    (xdc_Char)0x69,  /* [3742] */
    (xdc_Char)0x74,  /* [3743] */
    (xdc_Char)0x79,  /* [3744] */
    (xdc_Char)0x20,  /* [3745] */
    (xdc_Char)0x69,  /* [3746] */
    (xdc_Char)0x73,  /* [3747] */
    (xdc_Char)0x20,  /* [3748] */
    (xdc_Char)0x69,  /* [3749] */
    (xdc_Char)0x6e,  /* [3750] */
    (xdc_Char)0x76,  /* [3751] */
    (xdc_Char)0x61,  /* [3752] */
    (xdc_Char)0x6c,  /* [3753] */
    (xdc_Char)0x69,  /* [3754] */
    (xdc_Char)0x64,  /* [3755] */
    (xdc_Char)0x20,  /* [3756] */
    (xdc_Char)0x25,  /* [3757] */
    (xdc_Char)0x64,  /* [3758] */
    (xdc_Char)0x0,  /* [3759] */
    (xdc_Char)0x45,  /* [3760] */
    (xdc_Char)0x5f,  /* [3761] */
    (xdc_Char)0x65,  /* [3762] */
    (xdc_Char)0x78,  /* [3763] */
    (xdc_Char)0x63,  /* [3764] */
    (xdc_Char)0x65,  /* [3765] */
    (xdc_Char)0x70,  /* [3766] */
    (xdc_Char)0x74,  /* [3767] */
    (xdc_Char)0x69,  /* [3768] */
    (xdc_Char)0x6f,  /* [3769] */
    (xdc_Char)0x6e,  /* [3770] */
    (xdc_Char)0x3a,  /* [3771] */
    (xdc_Char)0x20,  /* [3772] */
    (xdc_Char)0x4c,  /* [3773] */
    (xdc_Char)0x25,  /* [3774] */
    (xdc_Char)0x64,  /* [3775] */
    (xdc_Char)0x20,  /* [3776] */
    (xdc_Char)0x72,  /* [3777] */
    (xdc_Char)0x65,  /* [3778] */
    (xdc_Char)0x61,  /* [3779] */
    (xdc_Char)0x73,  /* [3780] */
    (xdc_Char)0x6f,  /* [3781] */
    (xdc_Char)0x6e,  /* [3782] */
    (xdc_Char)0x3a,  /* [3783] */
    (xdc_Char)0x20,  /* [3784] */
    (xdc_Char)0x30,  /* [3785] */
    (xdc_Char)0x78,  /* [3786] */
    (xdc_Char)0x25,  /* [3787] */
    (xdc_Char)0x78,  /* [3788] */
    (xdc_Char)0x0,  /* [3789] */
    (xdc_Char)0x3c,  /* [3790] */
    (xdc_Char)0x2d,  /* [3791] */
    (xdc_Char)0x2d,  /* [3792] */
    (xdc_Char)0x20,  /* [3793] */
    (xdc_Char)0x63,  /* [3794] */
    (xdc_Char)0x6f,  /* [3795] */
    (xdc_Char)0x6e,  /* [3796] */
    (xdc_Char)0x73,  /* [3797] */
    (xdc_Char)0x74,  /* [3798] */
    (xdc_Char)0x72,  /* [3799] */
    (xdc_Char)0x75,  /* [3800] */
    (xdc_Char)0x63,  /* [3801] */
    (xdc_Char)0x74,  /* [3802] */
    (xdc_Char)0x3a,  /* [3803] */
    (xdc_Char)0x20,  /* [3804] */
    (xdc_Char)0x25,  /* [3805] */
    (xdc_Char)0x70,  /* [3806] */
    (xdc_Char)0x28,  /* [3807] */
    (xdc_Char)0x27,  /* [3808] */
    (xdc_Char)0x25,  /* [3809] */
    (xdc_Char)0x73,  /* [3810] */
    (xdc_Char)0x27,  /* [3811] */
    (xdc_Char)0x29,  /* [3812] */
    (xdc_Char)0x0,  /* [3813] */
    (xdc_Char)0x3c,  /* [3814] */
    (xdc_Char)0x2d,  /* [3815] */
    (xdc_Char)0x2d,  /* [3816] */
    (xdc_Char)0x20,  /* [3817] */
    (xdc_Char)0x63,  /* [3818] */
    (xdc_Char)0x72,  /* [3819] */
    (xdc_Char)0x65,  /* [3820] */
    (xdc_Char)0x61,  /* [3821] */
    (xdc_Char)0x74,  /* [3822] */
    (xdc_Char)0x65,  /* [3823] */
    (xdc_Char)0x3a,  /* [3824] */
    (xdc_Char)0x20,  /* [3825] */
    (xdc_Char)0x25,  /* [3826] */
    (xdc_Char)0x70,  /* [3827] */
    (xdc_Char)0x28,  /* [3828] */
    (xdc_Char)0x27,  /* [3829] */
    (xdc_Char)0x25,  /* [3830] */
    (xdc_Char)0x73,  /* [3831] */
    (xdc_Char)0x27,  /* [3832] */
    (xdc_Char)0x29,  /* [3833] */
    (xdc_Char)0x0,  /* [3834] */
    (xdc_Char)0x2d,  /* [3835] */
    (xdc_Char)0x2d,  /* [3836] */
    (xdc_Char)0x3e,  /* [3837] */
    (xdc_Char)0x20,  /* [3838] */
    (xdc_Char)0x64,  /* [3839] */
    (xdc_Char)0x65,  /* [3840] */
    (xdc_Char)0x73,  /* [3841] */
    (xdc_Char)0x74,  /* [3842] */
    (xdc_Char)0x72,  /* [3843] */
    (xdc_Char)0x75,  /* [3844] */
    (xdc_Char)0x63,  /* [3845] */
    (xdc_Char)0x74,  /* [3846] */
    (xdc_Char)0x3a,  /* [3847] */
    (xdc_Char)0x20,  /* [3848] */
    (xdc_Char)0x28,  /* [3849] */
    (xdc_Char)0x25,  /* [3850] */
    (xdc_Char)0x70,  /* [3851] */
    (xdc_Char)0x29,  /* [3852] */
    (xdc_Char)0x0,  /* [3853] */
    (xdc_Char)0x2d,  /* [3854] */
    (xdc_Char)0x2d,  /* [3855] */
    (xdc_Char)0x3e,  /* [3856] */
    (xdc_Char)0x20,  /* [3857] */
    (xdc_Char)0x64,  /* [3858] */
    (xdc_Char)0x65,  /* [3859] */
    (xdc_Char)0x6c,  /* [3860] */
    (xdc_Char)0x65,  /* [3861] */
    (xdc_Char)0x74,  /* [3862] */
    (xdc_Char)0x65,  /* [3863] */
    (xdc_Char)0x3a,  /* [3864] */
    (xdc_Char)0x20,  /* [3865] */
    (xdc_Char)0x28,  /* [3866] */
    (xdc_Char)0x25,  /* [3867] */
    (xdc_Char)0x70,  /* [3868] */
    (xdc_Char)0x29,  /* [3869] */
    (xdc_Char)0x0,  /* [3870] */
    (xdc_Char)0x45,  /* [3871] */
    (xdc_Char)0x52,  /* [3872] */
    (xdc_Char)0x52,  /* [3873] */
    (xdc_Char)0x4f,  /* [3874] */
    (xdc_Char)0x52,  /* [3875] */
    (xdc_Char)0x3a,  /* [3876] */
    (xdc_Char)0x20,  /* [3877] */
    (xdc_Char)0x25,  /* [3878] */
    (xdc_Char)0x24,  /* [3879] */
    (xdc_Char)0x46,  /* [3880] */
    (xdc_Char)0x25,  /* [3881] */
    (xdc_Char)0x24,  /* [3882] */
    (xdc_Char)0x53,  /* [3883] */
    (xdc_Char)0x0,  /* [3884] */
    (xdc_Char)0x57,  /* [3885] */
    (xdc_Char)0x41,  /* [3886] */
    (xdc_Char)0x52,  /* [3887] */
    (xdc_Char)0x4e,  /* [3888] */
    (xdc_Char)0x49,  /* [3889] */
    (xdc_Char)0x4e,  /* [3890] */
    (xdc_Char)0x47,  /* [3891] */
    (xdc_Char)0x3a,  /* [3892] */
    (xdc_Char)0x20,  /* [3893] */
    (xdc_Char)0x25,  /* [3894] */
    (xdc_Char)0x24,  /* [3895] */
    (xdc_Char)0x46,  /* [3896] */
    (xdc_Char)0x25,  /* [3897] */
    (xdc_Char)0x24,  /* [3898] */
    (xdc_Char)0x53,  /* [3899] */
    (xdc_Char)0x0,  /* [3900] */
    (xdc_Char)0x25,  /* [3901] */
    (xdc_Char)0x24,  /* [3902] */
    (xdc_Char)0x46,  /* [3903] */
    (xdc_Char)0x25,  /* [3904] */
    (xdc_Char)0x24,  /* [3905] */
    (xdc_Char)0x53,  /* [3906] */
    (xdc_Char)0x0,  /* [3907] */
    (xdc_Char)0x53,  /* [3908] */
    (xdc_Char)0x74,  /* [3909] */
    (xdc_Char)0x61,  /* [3910] */
    (xdc_Char)0x72,  /* [3911] */
    (xdc_Char)0x74,  /* [3912] */
    (xdc_Char)0x3a,  /* [3913] */
    (xdc_Char)0x20,  /* [3914] */
    (xdc_Char)0x25,  /* [3915] */
    (xdc_Char)0x24,  /* [3916] */
    (xdc_Char)0x53,  /* [3917] */
    (xdc_Char)0x0,  /* [3918] */
    (xdc_Char)0x53,  /* [3919] */
    (xdc_Char)0x74,  /* [3920] */
    (xdc_Char)0x6f,  /* [3921] */
    (xdc_Char)0x70,  /* [3922] */
    (xdc_Char)0x3a,  /* [3923] */
    (xdc_Char)0x20,  /* [3924] */
    (xdc_Char)0x25,  /* [3925] */
    (xdc_Char)0x24,  /* [3926] */
    (xdc_Char)0x53,  /* [3927] */
    (xdc_Char)0x0,  /* [3928] */
    (xdc_Char)0x53,  /* [3929] */
    (xdc_Char)0x74,  /* [3930] */
    (xdc_Char)0x61,  /* [3931] */
    (xdc_Char)0x72,  /* [3932] */
    (xdc_Char)0x74,  /* [3933] */
    (xdc_Char)0x49,  /* [3934] */
    (xdc_Char)0x6e,  /* [3935] */
    (xdc_Char)0x73,  /* [3936] */
    (xdc_Char)0x74,  /* [3937] */
    (xdc_Char)0x61,  /* [3938] */
    (xdc_Char)0x6e,  /* [3939] */
    (xdc_Char)0x63,  /* [3940] */
    (xdc_Char)0x65,  /* [3941] */
    (xdc_Char)0x3a,  /* [3942] */
    (xdc_Char)0x20,  /* [3943] */
    (xdc_Char)0x25,  /* [3944] */
    (xdc_Char)0x24,  /* [3945] */
    (xdc_Char)0x53,  /* [3946] */
    (xdc_Char)0x0,  /* [3947] */
    (xdc_Char)0x53,  /* [3948] */
    (xdc_Char)0x74,  /* [3949] */
    (xdc_Char)0x6f,  /* [3950] */
    (xdc_Char)0x70,  /* [3951] */
    (xdc_Char)0x49,  /* [3952] */
    (xdc_Char)0x6e,  /* [3953] */
    (xdc_Char)0x73,  /* [3954] */
    (xdc_Char)0x74,  /* [3955] */
    (xdc_Char)0x61,  /* [3956] */
    (xdc_Char)0x6e,  /* [3957] */
    (xdc_Char)0x63,  /* [3958] */
    (xdc_Char)0x65,  /* [3959] */
    (xdc_Char)0x3a,  /* [3960] */
    (xdc_Char)0x20,  /* [3961] */
    (xdc_Char)0x25,  /* [3962] */
    (xdc_Char)0x24,  /* [3963] */
    (xdc_Char)0x53,  /* [3964] */
    (xdc_Char)0x0,  /* [3965] */
    (xdc_Char)0x4c,  /* [3966] */
    (xdc_Char)0x57,  /* [3967] */
    (xdc_Char)0x5f,  /* [3968] */
    (xdc_Char)0x64,  /* [3969] */
    (xdc_Char)0x65,  /* [3970] */
    (xdc_Char)0x6c,  /* [3971] */
    (xdc_Char)0x61,  /* [3972] */
    (xdc_Char)0x79,  /* [3973] */
    (xdc_Char)0x65,  /* [3974] */
    (xdc_Char)0x64,  /* [3975] */
    (xdc_Char)0x3a,  /* [3976] */
    (xdc_Char)0x20,  /* [3977] */
    (xdc_Char)0x64,  /* [3978] */
    (xdc_Char)0x65,  /* [3979] */
    (xdc_Char)0x6c,  /* [3980] */
    (xdc_Char)0x61,  /* [3981] */
    (xdc_Char)0x79,  /* [3982] */
    (xdc_Char)0x3a,  /* [3983] */
    (xdc_Char)0x20,  /* [3984] */
    (xdc_Char)0x25,  /* [3985] */
    (xdc_Char)0x64,  /* [3986] */
    (xdc_Char)0x0,  /* [3987] */
    (xdc_Char)0x4c,  /* [3988] */
    (xdc_Char)0x4d,  /* [3989] */
    (xdc_Char)0x5f,  /* [3990] */
    (xdc_Char)0x74,  /* [3991] */
    (xdc_Char)0x69,  /* [3992] */
    (xdc_Char)0x63,  /* [3993] */
    (xdc_Char)0x6b,  /* [3994] */
    (xdc_Char)0x3a,  /* [3995] */
    (xdc_Char)0x20,  /* [3996] */
    (xdc_Char)0x74,  /* [3997] */
    (xdc_Char)0x69,  /* [3998] */
    (xdc_Char)0x63,  /* [3999] */
    (xdc_Char)0x6b,  /* [4000] */
    (xdc_Char)0x3a,  /* [4001] */
    (xdc_Char)0x20,  /* [4002] */
    (xdc_Char)0x25,  /* [4003] */
    (xdc_Char)0x64,  /* [4004] */
    (xdc_Char)0x0,  /* [4005] */
    (xdc_Char)0x4c,  /* [4006] */
    (xdc_Char)0x4d,  /* [4007] */
    (xdc_Char)0x5f,  /* [4008] */
    (xdc_Char)0x62,  /* [4009] */
    (xdc_Char)0x65,  /* [4010] */
    (xdc_Char)0x67,  /* [4011] */
    (xdc_Char)0x69,  /* [4012] */
    (xdc_Char)0x6e,  /* [4013] */
    (xdc_Char)0x3a,  /* [4014] */
    (xdc_Char)0x20,  /* [4015] */
    (xdc_Char)0x63,  /* [4016] */
    (xdc_Char)0x6c,  /* [4017] */
    (xdc_Char)0x6b,  /* [4018] */
    (xdc_Char)0x3a,  /* [4019] */
    (xdc_Char)0x20,  /* [4020] */
    (xdc_Char)0x30,  /* [4021] */
    (xdc_Char)0x78,  /* [4022] */
    (xdc_Char)0x25,  /* [4023] */
    (xdc_Char)0x78,  /* [4024] */
    (xdc_Char)0x2c,  /* [4025] */
    (xdc_Char)0x20,  /* [4026] */
    (xdc_Char)0x66,  /* [4027] */
    (xdc_Char)0x75,  /* [4028] */
    (xdc_Char)0x6e,  /* [4029] */
    (xdc_Char)0x63,  /* [4030] */
    (xdc_Char)0x3a,  /* [4031] */
    (xdc_Char)0x20,  /* [4032] */
    (xdc_Char)0x30,  /* [4033] */
    (xdc_Char)0x78,  /* [4034] */
    (xdc_Char)0x25,  /* [4035] */
    (xdc_Char)0x78,  /* [4036] */
    (xdc_Char)0x0,  /* [4037] */
    (xdc_Char)0x4c,  /* [4038] */
    (xdc_Char)0x4d,  /* [4039] */
    (xdc_Char)0x5f,  /* [4040] */
    (xdc_Char)0x70,  /* [4041] */
    (xdc_Char)0x6f,  /* [4042] */
    (xdc_Char)0x73,  /* [4043] */
    (xdc_Char)0x74,  /* [4044] */
    (xdc_Char)0x3a,  /* [4045] */
    (xdc_Char)0x20,  /* [4046] */
    (xdc_Char)0x73,  /* [4047] */
    (xdc_Char)0x65,  /* [4048] */
    (xdc_Char)0x6d,  /* [4049] */
    (xdc_Char)0x3a,  /* [4050] */
    (xdc_Char)0x20,  /* [4051] */
    (xdc_Char)0x30,  /* [4052] */
    (xdc_Char)0x78,  /* [4053] */
    (xdc_Char)0x25,  /* [4054] */
    (xdc_Char)0x78,  /* [4055] */
    (xdc_Char)0x2c,  /* [4056] */
    (xdc_Char)0x20,  /* [4057] */
    (xdc_Char)0x63,  /* [4058] */
    (xdc_Char)0x6f,  /* [4059] */
    (xdc_Char)0x75,  /* [4060] */
    (xdc_Char)0x6e,  /* [4061] */
    (xdc_Char)0x74,  /* [4062] */
    (xdc_Char)0x3a,  /* [4063] */
    (xdc_Char)0x20,  /* [4064] */
    (xdc_Char)0x25,  /* [4065] */
    (xdc_Char)0x64,  /* [4066] */
    (xdc_Char)0x0,  /* [4067] */
    (xdc_Char)0x4c,  /* [4068] */
    (xdc_Char)0x4d,  /* [4069] */
    (xdc_Char)0x5f,  /* [4070] */
    (xdc_Char)0x70,  /* [4071] */
    (xdc_Char)0x65,  /* [4072] */
    (xdc_Char)0x6e,  /* [4073] */
    (xdc_Char)0x64,  /* [4074] */
    (xdc_Char)0x3a,  /* [4075] */
    (xdc_Char)0x20,  /* [4076] */
    (xdc_Char)0x73,  /* [4077] */
    (xdc_Char)0x65,  /* [4078] */
    (xdc_Char)0x6d,  /* [4079] */
    (xdc_Char)0x3a,  /* [4080] */
    (xdc_Char)0x20,  /* [4081] */
    (xdc_Char)0x30,  /* [4082] */
    (xdc_Char)0x78,  /* [4083] */
    (xdc_Char)0x25,  /* [4084] */
    (xdc_Char)0x78,  /* [4085] */
    (xdc_Char)0x2c,  /* [4086] */
    (xdc_Char)0x20,  /* [4087] */
    (xdc_Char)0x63,  /* [4088] */
    (xdc_Char)0x6f,  /* [4089] */
    (xdc_Char)0x75,  /* [4090] */
    (xdc_Char)0x6e,  /* [4091] */
    (xdc_Char)0x74,  /* [4092] */
    (xdc_Char)0x3a,  /* [4093] */
    (xdc_Char)0x20,  /* [4094] */
    (xdc_Char)0x25,  /* [4095] */
    (xdc_Char)0x64,  /* [4096] */
    (xdc_Char)0x2c,  /* [4097] */
    (xdc_Char)0x20,  /* [4098] */
    (xdc_Char)0x74,  /* [4099] */
    (xdc_Char)0x69,  /* [4100] */
    (xdc_Char)0x6d,  /* [4101] */
    (xdc_Char)0x65,  /* [4102] */
    (xdc_Char)0x6f,  /* [4103] */
    (xdc_Char)0x75,  /* [4104] */
    (xdc_Char)0x74,  /* [4105] */
    (xdc_Char)0x3a,  /* [4106] */
    (xdc_Char)0x20,  /* [4107] */
    (xdc_Char)0x25,  /* [4108] */
    (xdc_Char)0x64,  /* [4109] */
    (xdc_Char)0x0,  /* [4110] */
    (xdc_Char)0x4c,  /* [4111] */
    (xdc_Char)0x4d,  /* [4112] */
    (xdc_Char)0x5f,  /* [4113] */
    (xdc_Char)0x62,  /* [4114] */
    (xdc_Char)0x65,  /* [4115] */
    (xdc_Char)0x67,  /* [4116] */
    (xdc_Char)0x69,  /* [4117] */
    (xdc_Char)0x6e,  /* [4118] */
    (xdc_Char)0x3a,  /* [4119] */
    (xdc_Char)0x20,  /* [4120] */
    (xdc_Char)0x73,  /* [4121] */
    (xdc_Char)0x77,  /* [4122] */
    (xdc_Char)0x69,  /* [4123] */
    (xdc_Char)0x3a,  /* [4124] */
    (xdc_Char)0x20,  /* [4125] */
    (xdc_Char)0x30,  /* [4126] */
    (xdc_Char)0x78,  /* [4127] */
    (xdc_Char)0x25,  /* [4128] */
    (xdc_Char)0x78,  /* [4129] */
    (xdc_Char)0x2c,  /* [4130] */
    (xdc_Char)0x20,  /* [4131] */
    (xdc_Char)0x66,  /* [4132] */
    (xdc_Char)0x75,  /* [4133] */
    (xdc_Char)0x6e,  /* [4134] */
    (xdc_Char)0x63,  /* [4135] */
    (xdc_Char)0x3a,  /* [4136] */
    (xdc_Char)0x20,  /* [4137] */
    (xdc_Char)0x30,  /* [4138] */
    (xdc_Char)0x78,  /* [4139] */
    (xdc_Char)0x25,  /* [4140] */
    (xdc_Char)0x78,  /* [4141] */
    (xdc_Char)0x2c,  /* [4142] */
    (xdc_Char)0x20,  /* [4143] */
    (xdc_Char)0x70,  /* [4144] */
    (xdc_Char)0x72,  /* [4145] */
    (xdc_Char)0x65,  /* [4146] */
    (xdc_Char)0x54,  /* [4147] */
    (xdc_Char)0x68,  /* [4148] */
    (xdc_Char)0x72,  /* [4149] */
    (xdc_Char)0x65,  /* [4150] */
    (xdc_Char)0x61,  /* [4151] */
    (xdc_Char)0x64,  /* [4152] */
    (xdc_Char)0x3a,  /* [4153] */
    (xdc_Char)0x20,  /* [4154] */
    (xdc_Char)0x25,  /* [4155] */
    (xdc_Char)0x64,  /* [4156] */
    (xdc_Char)0x0,  /* [4157] */
    (xdc_Char)0x4c,  /* [4158] */
    (xdc_Char)0x44,  /* [4159] */
    (xdc_Char)0x5f,  /* [4160] */
    (xdc_Char)0x65,  /* [4161] */
    (xdc_Char)0x6e,  /* [4162] */
    (xdc_Char)0x64,  /* [4163] */
    (xdc_Char)0x3a,  /* [4164] */
    (xdc_Char)0x20,  /* [4165] */
    (xdc_Char)0x73,  /* [4166] */
    (xdc_Char)0x77,  /* [4167] */
    (xdc_Char)0x69,  /* [4168] */
    (xdc_Char)0x3a,  /* [4169] */
    (xdc_Char)0x20,  /* [4170] */
    (xdc_Char)0x30,  /* [4171] */
    (xdc_Char)0x78,  /* [4172] */
    (xdc_Char)0x25,  /* [4173] */
    (xdc_Char)0x78,  /* [4174] */
    (xdc_Char)0x0,  /* [4175] */
    (xdc_Char)0x4c,  /* [4176] */
    (xdc_Char)0x4d,  /* [4177] */
    (xdc_Char)0x5f,  /* [4178] */
    (xdc_Char)0x70,  /* [4179] */
    (xdc_Char)0x6f,  /* [4180] */
    (xdc_Char)0x73,  /* [4181] */
    (xdc_Char)0x74,  /* [4182] */
    (xdc_Char)0x3a,  /* [4183] */
    (xdc_Char)0x20,  /* [4184] */
    (xdc_Char)0x73,  /* [4185] */
    (xdc_Char)0x77,  /* [4186] */
    (xdc_Char)0x69,  /* [4187] */
    (xdc_Char)0x3a,  /* [4188] */
    (xdc_Char)0x20,  /* [4189] */
    (xdc_Char)0x30,  /* [4190] */
    (xdc_Char)0x78,  /* [4191] */
    (xdc_Char)0x25,  /* [4192] */
    (xdc_Char)0x78,  /* [4193] */
    (xdc_Char)0x2c,  /* [4194] */
    (xdc_Char)0x20,  /* [4195] */
    (xdc_Char)0x66,  /* [4196] */
    (xdc_Char)0x75,  /* [4197] */
    (xdc_Char)0x6e,  /* [4198] */
    (xdc_Char)0x63,  /* [4199] */
    (xdc_Char)0x3a,  /* [4200] */
    (xdc_Char)0x20,  /* [4201] */
    (xdc_Char)0x30,  /* [4202] */
    (xdc_Char)0x78,  /* [4203] */
    (xdc_Char)0x25,  /* [4204] */
    (xdc_Char)0x78,  /* [4205] */
    (xdc_Char)0x2c,  /* [4206] */
    (xdc_Char)0x20,  /* [4207] */
    (xdc_Char)0x70,  /* [4208] */
    (xdc_Char)0x72,  /* [4209] */
    (xdc_Char)0x69,  /* [4210] */
    (xdc_Char)0x3a,  /* [4211] */
    (xdc_Char)0x20,  /* [4212] */
    (xdc_Char)0x25,  /* [4213] */
    (xdc_Char)0x64,  /* [4214] */
    (xdc_Char)0x0,  /* [4215] */
    (xdc_Char)0x4c,  /* [4216] */
    (xdc_Char)0x4d,  /* [4217] */
    (xdc_Char)0x5f,  /* [4218] */
    (xdc_Char)0x73,  /* [4219] */
    (xdc_Char)0x77,  /* [4220] */
    (xdc_Char)0x69,  /* [4221] */
    (xdc_Char)0x74,  /* [4222] */
    (xdc_Char)0x63,  /* [4223] */
    (xdc_Char)0x68,  /* [4224] */
    (xdc_Char)0x3a,  /* [4225] */
    (xdc_Char)0x20,  /* [4226] */
    (xdc_Char)0x6f,  /* [4227] */
    (xdc_Char)0x6c,  /* [4228] */
    (xdc_Char)0x64,  /* [4229] */
    (xdc_Char)0x74,  /* [4230] */
    (xdc_Char)0x73,  /* [4231] */
    (xdc_Char)0x6b,  /* [4232] */
    (xdc_Char)0x3a,  /* [4233] */
    (xdc_Char)0x20,  /* [4234] */
    (xdc_Char)0x30,  /* [4235] */
    (xdc_Char)0x78,  /* [4236] */
    (xdc_Char)0x25,  /* [4237] */
    (xdc_Char)0x78,  /* [4238] */
    (xdc_Char)0x2c,  /* [4239] */
    (xdc_Char)0x20,  /* [4240] */
    (xdc_Char)0x6f,  /* [4241] */
    (xdc_Char)0x6c,  /* [4242] */
    (xdc_Char)0x64,  /* [4243] */
    (xdc_Char)0x66,  /* [4244] */
    (xdc_Char)0x75,  /* [4245] */
    (xdc_Char)0x6e,  /* [4246] */
    (xdc_Char)0x63,  /* [4247] */
    (xdc_Char)0x3a,  /* [4248] */
    (xdc_Char)0x20,  /* [4249] */
    (xdc_Char)0x30,  /* [4250] */
    (xdc_Char)0x78,  /* [4251] */
    (xdc_Char)0x25,  /* [4252] */
    (xdc_Char)0x78,  /* [4253] */
    (xdc_Char)0x2c,  /* [4254] */
    (xdc_Char)0x20,  /* [4255] */
    (xdc_Char)0x6e,  /* [4256] */
    (xdc_Char)0x65,  /* [4257] */
    (xdc_Char)0x77,  /* [4258] */
    (xdc_Char)0x74,  /* [4259] */
    (xdc_Char)0x73,  /* [4260] */
    (xdc_Char)0x6b,  /* [4261] */
    (xdc_Char)0x3a,  /* [4262] */
    (xdc_Char)0x20,  /* [4263] */
    (xdc_Char)0x30,  /* [4264] */
    (xdc_Char)0x78,  /* [4265] */
    (xdc_Char)0x25,  /* [4266] */
    (xdc_Char)0x78,  /* [4267] */
    (xdc_Char)0x2c,  /* [4268] */
    (xdc_Char)0x20,  /* [4269] */
    (xdc_Char)0x6e,  /* [4270] */
    (xdc_Char)0x65,  /* [4271] */
    (xdc_Char)0x77,  /* [4272] */
    (xdc_Char)0x66,  /* [4273] */
    (xdc_Char)0x75,  /* [4274] */
    (xdc_Char)0x6e,  /* [4275] */
    (xdc_Char)0x63,  /* [4276] */
    (xdc_Char)0x3a,  /* [4277] */
    (xdc_Char)0x20,  /* [4278] */
    (xdc_Char)0x30,  /* [4279] */
    (xdc_Char)0x78,  /* [4280] */
    (xdc_Char)0x25,  /* [4281] */
    (xdc_Char)0x78,  /* [4282] */
    (xdc_Char)0x0,  /* [4283] */
    (xdc_Char)0x4c,  /* [4284] */
    (xdc_Char)0x4d,  /* [4285] */
    (xdc_Char)0x5f,  /* [4286] */
    (xdc_Char)0x73,  /* [4287] */
    (xdc_Char)0x6c,  /* [4288] */
    (xdc_Char)0x65,  /* [4289] */
    (xdc_Char)0x65,  /* [4290] */
    (xdc_Char)0x70,  /* [4291] */
    (xdc_Char)0x3a,  /* [4292] */
    (xdc_Char)0x20,  /* [4293] */
    (xdc_Char)0x74,  /* [4294] */
    (xdc_Char)0x73,  /* [4295] */
    (xdc_Char)0x6b,  /* [4296] */
    (xdc_Char)0x3a,  /* [4297] */
    (xdc_Char)0x20,  /* [4298] */
    (xdc_Char)0x30,  /* [4299] */
    (xdc_Char)0x78,  /* [4300] */
    (xdc_Char)0x25,  /* [4301] */
    (xdc_Char)0x78,  /* [4302] */
    (xdc_Char)0x2c,  /* [4303] */
    (xdc_Char)0x20,  /* [4304] */
    (xdc_Char)0x66,  /* [4305] */
    (xdc_Char)0x75,  /* [4306] */
    (xdc_Char)0x6e,  /* [4307] */
    (xdc_Char)0x63,  /* [4308] */
    (xdc_Char)0x3a,  /* [4309] */
    (xdc_Char)0x20,  /* [4310] */
    (xdc_Char)0x30,  /* [4311] */
    (xdc_Char)0x78,  /* [4312] */
    (xdc_Char)0x25,  /* [4313] */
    (xdc_Char)0x78,  /* [4314] */
    (xdc_Char)0x2c,  /* [4315] */
    (xdc_Char)0x20,  /* [4316] */
    (xdc_Char)0x74,  /* [4317] */
    (xdc_Char)0x69,  /* [4318] */
    (xdc_Char)0x6d,  /* [4319] */
    (xdc_Char)0x65,  /* [4320] */
    (xdc_Char)0x6f,  /* [4321] */
    (xdc_Char)0x75,  /* [4322] */
    (xdc_Char)0x74,  /* [4323] */
    (xdc_Char)0x3a,  /* [4324] */
    (xdc_Char)0x20,  /* [4325] */
    (xdc_Char)0x25,  /* [4326] */
    (xdc_Char)0x64,  /* [4327] */
    (xdc_Char)0x0,  /* [4328] */
    (xdc_Char)0x4c,  /* [4329] */
    (xdc_Char)0x44,  /* [4330] */
    (xdc_Char)0x5f,  /* [4331] */
    (xdc_Char)0x72,  /* [4332] */
    (xdc_Char)0x65,  /* [4333] */
    (xdc_Char)0x61,  /* [4334] */
    (xdc_Char)0x64,  /* [4335] */
    (xdc_Char)0x79,  /* [4336] */
    (xdc_Char)0x3a,  /* [4337] */
    (xdc_Char)0x20,  /* [4338] */
    (xdc_Char)0x74,  /* [4339] */
    (xdc_Char)0x73,  /* [4340] */
    (xdc_Char)0x6b,  /* [4341] */
    (xdc_Char)0x3a,  /* [4342] */
    (xdc_Char)0x20,  /* [4343] */
    (xdc_Char)0x30,  /* [4344] */
    (xdc_Char)0x78,  /* [4345] */
    (xdc_Char)0x25,  /* [4346] */
    (xdc_Char)0x78,  /* [4347] */
    (xdc_Char)0x2c,  /* [4348] */
    (xdc_Char)0x20,  /* [4349] */
    (xdc_Char)0x66,  /* [4350] */
    (xdc_Char)0x75,  /* [4351] */
    (xdc_Char)0x6e,  /* [4352] */
    (xdc_Char)0x63,  /* [4353] */
    (xdc_Char)0x3a,  /* [4354] */
    (xdc_Char)0x20,  /* [4355] */
    (xdc_Char)0x30,  /* [4356] */
    (xdc_Char)0x78,  /* [4357] */
    (xdc_Char)0x25,  /* [4358] */
    (xdc_Char)0x78,  /* [4359] */
    (xdc_Char)0x2c,  /* [4360] */
    (xdc_Char)0x20,  /* [4361] */
    (xdc_Char)0x70,  /* [4362] */
    (xdc_Char)0x72,  /* [4363] */
    (xdc_Char)0x69,  /* [4364] */
    (xdc_Char)0x3a,  /* [4365] */
    (xdc_Char)0x20,  /* [4366] */
    (xdc_Char)0x25,  /* [4367] */
    (xdc_Char)0x64,  /* [4368] */
    (xdc_Char)0x0,  /* [4369] */
    (xdc_Char)0x4c,  /* [4370] */
    (xdc_Char)0x44,  /* [4371] */
    (xdc_Char)0x5f,  /* [4372] */
    (xdc_Char)0x62,  /* [4373] */
    (xdc_Char)0x6c,  /* [4374] */
    (xdc_Char)0x6f,  /* [4375] */
    (xdc_Char)0x63,  /* [4376] */
    (xdc_Char)0x6b,  /* [4377] */
    (xdc_Char)0x3a,  /* [4378] */
    (xdc_Char)0x20,  /* [4379] */
    (xdc_Char)0x74,  /* [4380] */
    (xdc_Char)0x73,  /* [4381] */
    (xdc_Char)0x6b,  /* [4382] */
    (xdc_Char)0x3a,  /* [4383] */
    (xdc_Char)0x20,  /* [4384] */
    (xdc_Char)0x30,  /* [4385] */
    (xdc_Char)0x78,  /* [4386] */
    (xdc_Char)0x25,  /* [4387] */
    (xdc_Char)0x78,  /* [4388] */
    (xdc_Char)0x2c,  /* [4389] */
    (xdc_Char)0x20,  /* [4390] */
    (xdc_Char)0x66,  /* [4391] */
    (xdc_Char)0x75,  /* [4392] */
    (xdc_Char)0x6e,  /* [4393] */
    (xdc_Char)0x63,  /* [4394] */
    (xdc_Char)0x3a,  /* [4395] */
    (xdc_Char)0x20,  /* [4396] */
    (xdc_Char)0x30,  /* [4397] */
    (xdc_Char)0x78,  /* [4398] */
    (xdc_Char)0x25,  /* [4399] */
    (xdc_Char)0x78,  /* [4400] */
    (xdc_Char)0x0,  /* [4401] */
    (xdc_Char)0x4c,  /* [4402] */
    (xdc_Char)0x4d,  /* [4403] */
    (xdc_Char)0x5f,  /* [4404] */
    (xdc_Char)0x79,  /* [4405] */
    (xdc_Char)0x69,  /* [4406] */
    (xdc_Char)0x65,  /* [4407] */
    (xdc_Char)0x6c,  /* [4408] */
    (xdc_Char)0x64,  /* [4409] */
    (xdc_Char)0x3a,  /* [4410] */
    (xdc_Char)0x20,  /* [4411] */
    (xdc_Char)0x74,  /* [4412] */
    (xdc_Char)0x73,  /* [4413] */
    (xdc_Char)0x6b,  /* [4414] */
    (xdc_Char)0x3a,  /* [4415] */
    (xdc_Char)0x20,  /* [4416] */
    (xdc_Char)0x30,  /* [4417] */
    (xdc_Char)0x78,  /* [4418] */
    (xdc_Char)0x25,  /* [4419] */
    (xdc_Char)0x78,  /* [4420] */
    (xdc_Char)0x2c,  /* [4421] */
    (xdc_Char)0x20,  /* [4422] */
    (xdc_Char)0x66,  /* [4423] */
    (xdc_Char)0x75,  /* [4424] */
    (xdc_Char)0x6e,  /* [4425] */
    (xdc_Char)0x63,  /* [4426] */
    (xdc_Char)0x3a,  /* [4427] */
    (xdc_Char)0x20,  /* [4428] */
    (xdc_Char)0x30,  /* [4429] */
    (xdc_Char)0x78,  /* [4430] */
    (xdc_Char)0x25,  /* [4431] */
    (xdc_Char)0x78,  /* [4432] */
    (xdc_Char)0x2c,  /* [4433] */
    (xdc_Char)0x20,  /* [4434] */
    (xdc_Char)0x63,  /* [4435] */
    (xdc_Char)0x75,  /* [4436] */
    (xdc_Char)0x72,  /* [4437] */
    (xdc_Char)0x72,  /* [4438] */
    (xdc_Char)0x54,  /* [4439] */
    (xdc_Char)0x68,  /* [4440] */
    (xdc_Char)0x72,  /* [4441] */
    (xdc_Char)0x65,  /* [4442] */
    (xdc_Char)0x61,  /* [4443] */
    (xdc_Char)0x64,  /* [4444] */
    (xdc_Char)0x3a,  /* [4445] */
    (xdc_Char)0x20,  /* [4446] */
    (xdc_Char)0x25,  /* [4447] */
    (xdc_Char)0x64,  /* [4448] */
    (xdc_Char)0x0,  /* [4449] */
    (xdc_Char)0x4c,  /* [4450] */
    (xdc_Char)0x4d,  /* [4451] */
    (xdc_Char)0x5f,  /* [4452] */
    (xdc_Char)0x73,  /* [4453] */
    (xdc_Char)0x65,  /* [4454] */
    (xdc_Char)0x74,  /* [4455] */
    (xdc_Char)0x50,  /* [4456] */
    (xdc_Char)0x72,  /* [4457] */
    (xdc_Char)0x69,  /* [4458] */
    (xdc_Char)0x3a,  /* [4459] */
    (xdc_Char)0x20,  /* [4460] */
    (xdc_Char)0x74,  /* [4461] */
    (xdc_Char)0x73,  /* [4462] */
    (xdc_Char)0x6b,  /* [4463] */
    (xdc_Char)0x3a,  /* [4464] */
    (xdc_Char)0x20,  /* [4465] */
    (xdc_Char)0x30,  /* [4466] */
    (xdc_Char)0x78,  /* [4467] */
    (xdc_Char)0x25,  /* [4468] */
    (xdc_Char)0x78,  /* [4469] */
    (xdc_Char)0x2c,  /* [4470] */
    (xdc_Char)0x20,  /* [4471] */
    (xdc_Char)0x66,  /* [4472] */
    (xdc_Char)0x75,  /* [4473] */
    (xdc_Char)0x6e,  /* [4474] */
    (xdc_Char)0x63,  /* [4475] */
    (xdc_Char)0x3a,  /* [4476] */
    (xdc_Char)0x20,  /* [4477] */
    (xdc_Char)0x30,  /* [4478] */
    (xdc_Char)0x78,  /* [4479] */
    (xdc_Char)0x25,  /* [4480] */
    (xdc_Char)0x78,  /* [4481] */
    (xdc_Char)0x2c,  /* [4482] */
    (xdc_Char)0x20,  /* [4483] */
    (xdc_Char)0x6f,  /* [4484] */
    (xdc_Char)0x6c,  /* [4485] */
    (xdc_Char)0x64,  /* [4486] */
    (xdc_Char)0x50,  /* [4487] */
    (xdc_Char)0x72,  /* [4488] */
    (xdc_Char)0x69,  /* [4489] */
    (xdc_Char)0x3a,  /* [4490] */
    (xdc_Char)0x20,  /* [4491] */
    (xdc_Char)0x25,  /* [4492] */
    (xdc_Char)0x64,  /* [4493] */
    (xdc_Char)0x2c,  /* [4494] */
    (xdc_Char)0x20,  /* [4495] */
    (xdc_Char)0x6e,  /* [4496] */
    (xdc_Char)0x65,  /* [4497] */
    (xdc_Char)0x77,  /* [4498] */
    (xdc_Char)0x50,  /* [4499] */
    (xdc_Char)0x72,  /* [4500] */
    (xdc_Char)0x69,  /* [4501] */
    (xdc_Char)0x20,  /* [4502] */
    (xdc_Char)0x25,  /* [4503] */
    (xdc_Char)0x64,  /* [4504] */
    (xdc_Char)0x0,  /* [4505] */
    (xdc_Char)0x4c,  /* [4506] */
    (xdc_Char)0x44,  /* [4507] */
    (xdc_Char)0x5f,  /* [4508] */
    (xdc_Char)0x65,  /* [4509] */
    (xdc_Char)0x78,  /* [4510] */
    (xdc_Char)0x69,  /* [4511] */
    (xdc_Char)0x74,  /* [4512] */
    (xdc_Char)0x3a,  /* [4513] */
    (xdc_Char)0x20,  /* [4514] */
    (xdc_Char)0x74,  /* [4515] */
    (xdc_Char)0x73,  /* [4516] */
    (xdc_Char)0x6b,  /* [4517] */
    (xdc_Char)0x3a,  /* [4518] */
    (xdc_Char)0x20,  /* [4519] */
    (xdc_Char)0x30,  /* [4520] */
    (xdc_Char)0x78,  /* [4521] */
    (xdc_Char)0x25,  /* [4522] */
    (xdc_Char)0x78,  /* [4523] */
    (xdc_Char)0x2c,  /* [4524] */
    (xdc_Char)0x20,  /* [4525] */
    (xdc_Char)0x66,  /* [4526] */
    (xdc_Char)0x75,  /* [4527] */
    (xdc_Char)0x6e,  /* [4528] */
    (xdc_Char)0x63,  /* [4529] */
    (xdc_Char)0x3a,  /* [4530] */
    (xdc_Char)0x20,  /* [4531] */
    (xdc_Char)0x30,  /* [4532] */
    (xdc_Char)0x78,  /* [4533] */
    (xdc_Char)0x25,  /* [4534] */
    (xdc_Char)0x78,  /* [4535] */
    (xdc_Char)0x0,  /* [4536] */
    (xdc_Char)0x4c,  /* [4537] */
    (xdc_Char)0x4d,  /* [4538] */
    (xdc_Char)0x5f,  /* [4539] */
    (xdc_Char)0x62,  /* [4540] */
    (xdc_Char)0x65,  /* [4541] */
    (xdc_Char)0x67,  /* [4542] */
    (xdc_Char)0x69,  /* [4543] */
    (xdc_Char)0x6e,  /* [4544] */
    (xdc_Char)0x3a,  /* [4545] */
    (xdc_Char)0x20,  /* [4546] */
    (xdc_Char)0x68,  /* [4547] */
    (xdc_Char)0x77,  /* [4548] */
    (xdc_Char)0x69,  /* [4549] */
    (xdc_Char)0x3a,  /* [4550] */
    (xdc_Char)0x20,  /* [4551] */
    (xdc_Char)0x30,  /* [4552] */
    (xdc_Char)0x78,  /* [4553] */
    (xdc_Char)0x25,  /* [4554] */
    (xdc_Char)0x78,  /* [4555] */
    (xdc_Char)0x2c,  /* [4556] */
    (xdc_Char)0x20,  /* [4557] */
    (xdc_Char)0x66,  /* [4558] */
    (xdc_Char)0x75,  /* [4559] */
    (xdc_Char)0x6e,  /* [4560] */
    (xdc_Char)0x63,  /* [4561] */
    (xdc_Char)0x3a,  /* [4562] */
    (xdc_Char)0x20,  /* [4563] */
    (xdc_Char)0x30,  /* [4564] */
    (xdc_Char)0x78,  /* [4565] */
    (xdc_Char)0x25,  /* [4566] */
    (xdc_Char)0x78,  /* [4567] */
    (xdc_Char)0x2c,  /* [4568] */
    (xdc_Char)0x20,  /* [4569] */
    (xdc_Char)0x70,  /* [4570] */
    (xdc_Char)0x72,  /* [4571] */
    (xdc_Char)0x65,  /* [4572] */
    (xdc_Char)0x54,  /* [4573] */
    (xdc_Char)0x68,  /* [4574] */
    (xdc_Char)0x72,  /* [4575] */
    (xdc_Char)0x65,  /* [4576] */
    (xdc_Char)0x61,  /* [4577] */
    (xdc_Char)0x64,  /* [4578] */
    (xdc_Char)0x3a,  /* [4579] */
    (xdc_Char)0x20,  /* [4580] */
    (xdc_Char)0x25,  /* [4581] */
    (xdc_Char)0x64,  /* [4582] */
    (xdc_Char)0x2c,  /* [4583] */
    (xdc_Char)0x20,  /* [4584] */
    (xdc_Char)0x69,  /* [4585] */
    (xdc_Char)0x6e,  /* [4586] */
    (xdc_Char)0x74,  /* [4587] */
    (xdc_Char)0x4e,  /* [4588] */
    (xdc_Char)0x75,  /* [4589] */
    (xdc_Char)0x6d,  /* [4590] */
    (xdc_Char)0x3a,  /* [4591] */
    (xdc_Char)0x20,  /* [4592] */
    (xdc_Char)0x25,  /* [4593] */
    (xdc_Char)0x64,  /* [4594] */
    (xdc_Char)0x2c,  /* [4595] */
    (xdc_Char)0x20,  /* [4596] */
    (xdc_Char)0x69,  /* [4597] */
    (xdc_Char)0x72,  /* [4598] */
    (xdc_Char)0x70,  /* [4599] */
    (xdc_Char)0x3a,  /* [4600] */
    (xdc_Char)0x20,  /* [4601] */
    (xdc_Char)0x30,  /* [4602] */
    (xdc_Char)0x78,  /* [4603] */
    (xdc_Char)0x25,  /* [4604] */
    (xdc_Char)0x78,  /* [4605] */
    (xdc_Char)0x0,  /* [4606] */
    (xdc_Char)0x4c,  /* [4607] */
    (xdc_Char)0x44,  /* [4608] */
    (xdc_Char)0x5f,  /* [4609] */
    (xdc_Char)0x65,  /* [4610] */
    (xdc_Char)0x6e,  /* [4611] */
    (xdc_Char)0x64,  /* [4612] */
    (xdc_Char)0x3a,  /* [4613] */
    (xdc_Char)0x20,  /* [4614] */
    (xdc_Char)0x68,  /* [4615] */
    (xdc_Char)0x77,  /* [4616] */
    (xdc_Char)0x69,  /* [4617] */
    (xdc_Char)0x3a,  /* [4618] */
    (xdc_Char)0x20,  /* [4619] */
    (xdc_Char)0x30,  /* [4620] */
    (xdc_Char)0x78,  /* [4621] */
    (xdc_Char)0x25,  /* [4622] */
    (xdc_Char)0x78,  /* [4623] */
    (xdc_Char)0x0,  /* [4624] */
    (xdc_Char)0x4c,  /* [4625] */
    (xdc_Char)0x53,  /* [4626] */
    (xdc_Char)0x5f,  /* [4627] */
    (xdc_Char)0x63,  /* [4628] */
    (xdc_Char)0x70,  /* [4629] */
    (xdc_Char)0x75,  /* [4630] */
    (xdc_Char)0x4c,  /* [4631] */
    (xdc_Char)0x6f,  /* [4632] */
    (xdc_Char)0x61,  /* [4633] */
    (xdc_Char)0x64,  /* [4634] */
    (xdc_Char)0x3a,  /* [4635] */
    (xdc_Char)0x20,  /* [4636] */
    (xdc_Char)0x25,  /* [4637] */
    (xdc_Char)0x64,  /* [4638] */
    (xdc_Char)0x25,  /* [4639] */
    (xdc_Char)0x25,  /* [4640] */
    (xdc_Char)0x0,  /* [4641] */
    (xdc_Char)0x4c,  /* [4642] */
    (xdc_Char)0x53,  /* [4643] */
    (xdc_Char)0x5f,  /* [4644] */
    (xdc_Char)0x68,  /* [4645] */
    (xdc_Char)0x77,  /* [4646] */
    (xdc_Char)0x69,  /* [4647] */
    (xdc_Char)0x4c,  /* [4648] */
    (xdc_Char)0x6f,  /* [4649] */
    (xdc_Char)0x61,  /* [4650] */
    (xdc_Char)0x64,  /* [4651] */
    (xdc_Char)0x3a,  /* [4652] */
    (xdc_Char)0x20,  /* [4653] */
    (xdc_Char)0x25,  /* [4654] */
    (xdc_Char)0x64,  /* [4655] */
    (xdc_Char)0x2c,  /* [4656] */
    (xdc_Char)0x25,  /* [4657] */
    (xdc_Char)0x64,  /* [4658] */
    (xdc_Char)0x0,  /* [4659] */
    (xdc_Char)0x4c,  /* [4660] */
    (xdc_Char)0x53,  /* [4661] */
    (xdc_Char)0x5f,  /* [4662] */
    (xdc_Char)0x73,  /* [4663] */
    (xdc_Char)0x77,  /* [4664] */
    (xdc_Char)0x69,  /* [4665] */
    (xdc_Char)0x4c,  /* [4666] */
    (xdc_Char)0x6f,  /* [4667] */
    (xdc_Char)0x61,  /* [4668] */
    (xdc_Char)0x64,  /* [4669] */
    (xdc_Char)0x3a,  /* [4670] */
    (xdc_Char)0x20,  /* [4671] */
    (xdc_Char)0x25,  /* [4672] */
    (xdc_Char)0x64,  /* [4673] */
    (xdc_Char)0x2c,  /* [4674] */
    (xdc_Char)0x25,  /* [4675] */
    (xdc_Char)0x64,  /* [4676] */
    (xdc_Char)0x0,  /* [4677] */
    (xdc_Char)0x4c,  /* [4678] */
    (xdc_Char)0x53,  /* [4679] */
    (xdc_Char)0x5f,  /* [4680] */
    (xdc_Char)0x74,  /* [4681] */
    (xdc_Char)0x61,  /* [4682] */
    (xdc_Char)0x73,  /* [4683] */
    (xdc_Char)0x6b,  /* [4684] */
    (xdc_Char)0x4c,  /* [4685] */
    (xdc_Char)0x6f,  /* [4686] */
    (xdc_Char)0x61,  /* [4687] */
    (xdc_Char)0x64,  /* [4688] */
    (xdc_Char)0x3a,  /* [4689] */
    (xdc_Char)0x20,  /* [4690] */
    (xdc_Char)0x30,  /* [4691] */
    (xdc_Char)0x78,  /* [4692] */
    (xdc_Char)0x25,  /* [4693] */
    (xdc_Char)0x78,  /* [4694] */
    (xdc_Char)0x2c,  /* [4695] */
    (xdc_Char)0x25,  /* [4696] */
    (xdc_Char)0x64,  /* [4697] */
    (xdc_Char)0x2c,  /* [4698] */
    (xdc_Char)0x25,  /* [4699] */
    (xdc_Char)0x64,  /* [4700] */
    (xdc_Char)0x2c,  /* [4701] */
    (xdc_Char)0x30,  /* [4702] */
    (xdc_Char)0x78,  /* [4703] */
    (xdc_Char)0x25,  /* [4704] */
    (xdc_Char)0x78,  /* [4705] */
    (xdc_Char)0x0,  /* [4706] */
    (xdc_Char)0x78,  /* [4707] */
    (xdc_Char)0x64,  /* [4708] */
    (xdc_Char)0x63,  /* [4709] */
    (xdc_Char)0x2e,  /* [4710] */
    (xdc_Char)0x0,  /* [4711] */
    (xdc_Char)0x72,  /* [4712] */
    (xdc_Char)0x75,  /* [4713] */
    (xdc_Char)0x6e,  /* [4714] */
    (xdc_Char)0x74,  /* [4715] */
    (xdc_Char)0x69,  /* [4716] */
    (xdc_Char)0x6d,  /* [4717] */
    (xdc_Char)0x65,  /* [4718] */
    (xdc_Char)0x2e,  /* [4719] */
    (xdc_Char)0x0,  /* [4720] */
    (xdc_Char)0x41,  /* [4721] */
    (xdc_Char)0x73,  /* [4722] */
    (xdc_Char)0x73,  /* [4723] */
    (xdc_Char)0x65,  /* [4724] */
    (xdc_Char)0x72,  /* [4725] */
    (xdc_Char)0x74,  /* [4726] */
    (xdc_Char)0x0,  /* [4727] */
    (xdc_Char)0x43,  /* [4728] */
    (xdc_Char)0x6f,  /* [4729] */
    (xdc_Char)0x72,  /* [4730] */
    (xdc_Char)0x65,  /* [4731] */
    (xdc_Char)0x0,  /* [4732] */
    (xdc_Char)0x44,  /* [4733] */
    (xdc_Char)0x65,  /* [4734] */
    (xdc_Char)0x66,  /* [4735] */
    (xdc_Char)0x61,  /* [4736] */
    (xdc_Char)0x75,  /* [4737] */
    (xdc_Char)0x6c,  /* [4738] */
    (xdc_Char)0x74,  /* [4739] */
    (xdc_Char)0x73,  /* [4740] */
    (xdc_Char)0x0,  /* [4741] */
    (xdc_Char)0x44,  /* [4742] */
    (xdc_Char)0x69,  /* [4743] */
    (xdc_Char)0x61,  /* [4744] */
    (xdc_Char)0x67,  /* [4745] */
    (xdc_Char)0x73,  /* [4746] */
    (xdc_Char)0x0,  /* [4747] */
    (xdc_Char)0x45,  /* [4748] */
    (xdc_Char)0x72,  /* [4749] */
    (xdc_Char)0x72,  /* [4750] */
    (xdc_Char)0x6f,  /* [4751] */
    (xdc_Char)0x72,  /* [4752] */
    (xdc_Char)0x0,  /* [4753] */
    (xdc_Char)0x47,  /* [4754] */
    (xdc_Char)0x61,  /* [4755] */
    (xdc_Char)0x74,  /* [4756] */
    (xdc_Char)0x65,  /* [4757] */
    (xdc_Char)0x0,  /* [4758] */
    (xdc_Char)0x4c,  /* [4759] */
    (xdc_Char)0x6f,  /* [4760] */
    (xdc_Char)0x67,  /* [4761] */
    (xdc_Char)0x0,  /* [4762] */
    (xdc_Char)0x4d,  /* [4763] */
    (xdc_Char)0x61,  /* [4764] */
    (xdc_Char)0x69,  /* [4765] */
    (xdc_Char)0x6e,  /* [4766] */
    (xdc_Char)0x0,  /* [4767] */
    (xdc_Char)0x4d,  /* [4768] */
    (xdc_Char)0x65,  /* [4769] */
    (xdc_Char)0x6d,  /* [4770] */
    (xdc_Char)0x6f,  /* [4771] */
    (xdc_Char)0x72,  /* [4772] */
    (xdc_Char)0x79,  /* [4773] */
    (xdc_Char)0x0,  /* [4774] */
    (xdc_Char)0x52,  /* [4775] */
    (xdc_Char)0x65,  /* [4776] */
    (xdc_Char)0x67,  /* [4777] */
    (xdc_Char)0x69,  /* [4778] */
    (xdc_Char)0x73,  /* [4779] */
    (xdc_Char)0x74,  /* [4780] */
    (xdc_Char)0x72,  /* [4781] */
    (xdc_Char)0x79,  /* [4782] */
    (xdc_Char)0x0,  /* [4783] */
    (xdc_Char)0x53,  /* [4784] */
    (xdc_Char)0x74,  /* [4785] */
    (xdc_Char)0x61,  /* [4786] */
    (xdc_Char)0x72,  /* [4787] */
    (xdc_Char)0x74,  /* [4788] */
    (xdc_Char)0x75,  /* [4789] */
    (xdc_Char)0x70,  /* [4790] */
    (xdc_Char)0x0,  /* [4791] */
    (xdc_Char)0x53,  /* [4792] */
    (xdc_Char)0x79,  /* [4793] */
    (xdc_Char)0x73,  /* [4794] */
    (xdc_Char)0x74,  /* [4795] */
    (xdc_Char)0x65,  /* [4796] */
    (xdc_Char)0x6d,  /* [4797] */
    (xdc_Char)0x0,  /* [4798] */
    (xdc_Char)0x53,  /* [4799] */
    (xdc_Char)0x79,  /* [4800] */
    (xdc_Char)0x73,  /* [4801] */
    (xdc_Char)0x53,  /* [4802] */
    (xdc_Char)0x74,  /* [4803] */
    (xdc_Char)0x64,  /* [4804] */
    (xdc_Char)0x0,  /* [4805] */
    (xdc_Char)0x54,  /* [4806] */
    (xdc_Char)0x65,  /* [4807] */
    (xdc_Char)0x78,  /* [4808] */
    (xdc_Char)0x74,  /* [4809] */
    (xdc_Char)0x0,  /* [4810] */
    (xdc_Char)0x54,  /* [4811] */
    (xdc_Char)0x69,  /* [4812] */
    (xdc_Char)0x6d,  /* [4813] */
    (xdc_Char)0x65,  /* [4814] */
    (xdc_Char)0x73,  /* [4815] */
    (xdc_Char)0x74,  /* [4816] */
    (xdc_Char)0x61,  /* [4817] */
    (xdc_Char)0x6d,  /* [4818] */
    (xdc_Char)0x70,  /* [4819] */
    (xdc_Char)0x0,  /* [4820] */
    (xdc_Char)0x74,  /* [4821] */
    (xdc_Char)0x69,  /* [4822] */
    (xdc_Char)0x2e,  /* [4823] */
    (xdc_Char)0x0,  /* [4824] */
    (xdc_Char)0x73,  /* [4825] */
    (xdc_Char)0x79,  /* [4826] */
    (xdc_Char)0x73,  /* [4827] */
    (xdc_Char)0x62,  /* [4828] */
    (xdc_Char)0x69,  /* [4829] */
    (xdc_Char)0x6f,  /* [4830] */
    (xdc_Char)0x73,  /* [4831] */
    (xdc_Char)0x2e,  /* [4832] */
    (xdc_Char)0x0,  /* [4833] */
    (xdc_Char)0x42,  /* [4834] */
    (xdc_Char)0x49,  /* [4835] */
    (xdc_Char)0x4f,  /* [4836] */
    (xdc_Char)0x53,  /* [4837] */
    (xdc_Char)0x0,  /* [4838] */
    (xdc_Char)0x68,  /* [4839] */
    (xdc_Char)0x61,  /* [4840] */
    (xdc_Char)0x6c,  /* [4841] */
    (xdc_Char)0x2e,  /* [4842] */
    (xdc_Char)0x0,  /* [4843] */
    (xdc_Char)0x48,  /* [4844] */
    (xdc_Char)0x77,  /* [4845] */
    (xdc_Char)0x69,  /* [4846] */
    (xdc_Char)0x0,  /* [4847] */
    (xdc_Char)0x54,  /* [4848] */
    (xdc_Char)0x69,  /* [4849] */
    (xdc_Char)0x6d,  /* [4850] */
    (xdc_Char)0x65,  /* [4851] */
    (xdc_Char)0x72,  /* [4852] */
    (xdc_Char)0x0,  /* [4853] */
    (xdc_Char)0x43,  /* [4854] */
    (xdc_Char)0x61,  /* [4855] */
    (xdc_Char)0x63,  /* [4856] */
    (xdc_Char)0x68,  /* [4857] */
    (xdc_Char)0x65,  /* [4858] */
    (xdc_Char)0x0,  /* [4859] */
    (xdc_Char)0x6b,  /* [4860] */
    (xdc_Char)0x6e,  /* [4861] */
    (xdc_Char)0x6c,  /* [4862] */
    (xdc_Char)0x2e,  /* [4863] */
    (xdc_Char)0x0,  /* [4864] */
    (xdc_Char)0x43,  /* [4865] */
    (xdc_Char)0x6c,  /* [4866] */
    (xdc_Char)0x6f,  /* [4867] */
    (xdc_Char)0x63,  /* [4868] */
    (xdc_Char)0x6b,  /* [4869] */
    (xdc_Char)0x0,  /* [4870] */
    (xdc_Char)0x49,  /* [4871] */
    (xdc_Char)0x64,  /* [4872] */
    (xdc_Char)0x6c,  /* [4873] */
    (xdc_Char)0x65,  /* [4874] */
    (xdc_Char)0x0,  /* [4875] */
    (xdc_Char)0x49,  /* [4876] */
    (xdc_Char)0x6e,  /* [4877] */
    (xdc_Char)0x74,  /* [4878] */
    (xdc_Char)0x72,  /* [4879] */
    (xdc_Char)0x69,  /* [4880] */
    (xdc_Char)0x6e,  /* [4881] */
    (xdc_Char)0x73,  /* [4882] */
    (xdc_Char)0x69,  /* [4883] */
    (xdc_Char)0x63,  /* [4884] */
    (xdc_Char)0x73,  /* [4885] */
    (xdc_Char)0x0,  /* [4886] */
    (xdc_Char)0x51,  /* [4887] */
    (xdc_Char)0x75,  /* [4888] */
    (xdc_Char)0x65,  /* [4889] */
    (xdc_Char)0x75,  /* [4890] */
    (xdc_Char)0x65,  /* [4891] */
    (xdc_Char)0x0,  /* [4892] */
    (xdc_Char)0x53,  /* [4893] */
    (xdc_Char)0x65,  /* [4894] */
    (xdc_Char)0x6d,  /* [4895] */
    (xdc_Char)0x61,  /* [4896] */
    (xdc_Char)0x70,  /* [4897] */
    (xdc_Char)0x68,  /* [4898] */
    (xdc_Char)0x6f,  /* [4899] */
    (xdc_Char)0x72,  /* [4900] */
    (xdc_Char)0x65,  /* [4901] */
    (xdc_Char)0x0,  /* [4902] */
    (xdc_Char)0x53,  /* [4903] */
    (xdc_Char)0x77,  /* [4904] */
    (xdc_Char)0x69,  /* [4905] */
    (xdc_Char)0x0,  /* [4906] */
    (xdc_Char)0x54,  /* [4907] */
    (xdc_Char)0x61,  /* [4908] */
    (xdc_Char)0x73,  /* [4909] */
    (xdc_Char)0x6b,  /* [4910] */
    (xdc_Char)0x0,  /* [4911] */
    (xdc_Char)0x66,  /* [4912] */
    (xdc_Char)0x61,  /* [4913] */
    (xdc_Char)0x6d,  /* [4914] */
    (xdc_Char)0x69,  /* [4915] */
    (xdc_Char)0x6c,  /* [4916] */
    (xdc_Char)0x79,  /* [4917] */
    (xdc_Char)0x2e,  /* [4918] */
    (xdc_Char)0x0,  /* [4919] */
    (xdc_Char)0x61,  /* [4920] */
    (xdc_Char)0x72,  /* [4921] */
    (xdc_Char)0x6d,  /* [4922] */
    (xdc_Char)0x2e,  /* [4923] */
    (xdc_Char)0x0,  /* [4924] */
    (xdc_Char)0x6d,  /* [4925] */
    (xdc_Char)0x33,  /* [4926] */
    (xdc_Char)0x2e,  /* [4927] */
    (xdc_Char)0x0,  /* [4928] */
    (xdc_Char)0x49,  /* [4929] */
    (xdc_Char)0x6e,  /* [4930] */
    (xdc_Char)0x74,  /* [4931] */
    (xdc_Char)0x72,  /* [4932] */
    (xdc_Char)0x69,  /* [4933] */
    (xdc_Char)0x6e,  /* [4934] */
    (xdc_Char)0x73,  /* [4935] */
    (xdc_Char)0x69,  /* [4936] */
    (xdc_Char)0x63,  /* [4937] */
    (xdc_Char)0x73,  /* [4938] */
    (xdc_Char)0x53,  /* [4939] */
    (xdc_Char)0x75,  /* [4940] */
    (xdc_Char)0x70,  /* [4941] */
    (xdc_Char)0x70,  /* [4942] */
    (xdc_Char)0x6f,  /* [4943] */
    (xdc_Char)0x72,  /* [4944] */
    (xdc_Char)0x74,  /* [4945] */
    (xdc_Char)0x0,  /* [4946] */
    (xdc_Char)0x54,  /* [4947] */
    (xdc_Char)0x61,  /* [4948] */
    (xdc_Char)0x73,  /* [4949] */
    (xdc_Char)0x6b,  /* [4950] */
    (xdc_Char)0x53,  /* [4951] */
    (xdc_Char)0x75,  /* [4952] */
    (xdc_Char)0x70,  /* [4953] */
    (xdc_Char)0x70,  /* [4954] */
    (xdc_Char)0x6f,  /* [4955] */
    (xdc_Char)0x72,  /* [4956] */
    (xdc_Char)0x74,  /* [4957] */
    (xdc_Char)0x0,  /* [4958] */
    (xdc_Char)0x64,  /* [4959] */
    (xdc_Char)0x75,  /* [4960] */
    (xdc_Char)0x63,  /* [4961] */
    (xdc_Char)0x61,  /* [4962] */
    (xdc_Char)0x74,  /* [4963] */
    (xdc_Char)0x69,  /* [4964] */
    (xdc_Char)0x2e,  /* [4965] */
    (xdc_Char)0x0,  /* [4966] */
    (xdc_Char)0x43,  /* [4967] */
    (xdc_Char)0x54,  /* [4968] */
    (xdc_Char)0x4d,  /* [4969] */
    (xdc_Char)0x0,  /* [4970] */
    (xdc_Char)0x47,  /* [4971] */
    (xdc_Char)0x61,  /* [4972] */
    (xdc_Char)0x74,  /* [4973] */
    (xdc_Char)0x65,  /* [4974] */
    (xdc_Char)0x44,  /* [4975] */
    (xdc_Char)0x75,  /* [4976] */
    (xdc_Char)0x61,  /* [4977] */
    (xdc_Char)0x6c,  /* [4978] */
    (xdc_Char)0x43,  /* [4979] */
    (xdc_Char)0x6f,  /* [4980] */
    (xdc_Char)0x72,  /* [4981] */
    (xdc_Char)0x65,  /* [4982] */
    (xdc_Char)0x0,  /* [4983] */
    (xdc_Char)0x54,  /* [4984] */
    (xdc_Char)0x69,  /* [4985] */
    (xdc_Char)0x6d,  /* [4986] */
    (xdc_Char)0x65,  /* [4987] */
    (xdc_Char)0x72,  /* [4988] */
    (xdc_Char)0x53,  /* [4989] */
    (xdc_Char)0x75,  /* [4990] */
    (xdc_Char)0x70,  /* [4991] */
    (xdc_Char)0x70,  /* [4992] */
    (xdc_Char)0x6f,  /* [4993] */
    (xdc_Char)0x72,  /* [4994] */
    (xdc_Char)0x74,  /* [4995] */
    (xdc_Char)0x0,  /* [4996] */
    (xdc_Char)0x54,  /* [4997] */
    (xdc_Char)0x69,  /* [4998] */
    (xdc_Char)0x6d,  /* [4999] */
    (xdc_Char)0x65,  /* [5000] */
    (xdc_Char)0x73,  /* [5001] */
    (xdc_Char)0x74,  /* [5002] */
    (xdc_Char)0x61,  /* [5003] */
    (xdc_Char)0x6d,  /* [5004] */
    (xdc_Char)0x70,  /* [5005] */
    (xdc_Char)0x50,  /* [5006] */
    (xdc_Char)0x72,  /* [5007] */
    (xdc_Char)0x6f,  /* [5008] */
    (xdc_Char)0x76,  /* [5009] */
    (xdc_Char)0x69,  /* [5010] */
    (xdc_Char)0x64,  /* [5011] */
    (xdc_Char)0x65,  /* [5012] */
    (xdc_Char)0x72,  /* [5013] */
    (xdc_Char)0x0,  /* [5014] */
    (xdc_Char)0x57,  /* [5015] */
    (xdc_Char)0x75,  /* [5016] */
    (xdc_Char)0x67,  /* [5017] */
    (xdc_Char)0x65,  /* [5018] */
    (xdc_Char)0x6e,  /* [5019] */
    (xdc_Char)0x0,  /* [5020] */
    (xdc_Char)0x68,  /* [5021] */
    (xdc_Char)0x65,  /* [5022] */
    (xdc_Char)0x61,  /* [5023] */
    (xdc_Char)0x70,  /* [5024] */
    (xdc_Char)0x73,  /* [5025] */
    (xdc_Char)0x2e,  /* [5026] */
    (xdc_Char)0x0,  /* [5027] */
    (xdc_Char)0x48,  /* [5028] */
    (xdc_Char)0x65,  /* [5029] */
    (xdc_Char)0x61,  /* [5030] */
    (xdc_Char)0x70,  /* [5031] */
    (xdc_Char)0x42,  /* [5032] */
    (xdc_Char)0x75,  /* [5033] */
    (xdc_Char)0x66,  /* [5034] */
    (xdc_Char)0x0,  /* [5035] */
    (xdc_Char)0x48,  /* [5036] */
    (xdc_Char)0x65,  /* [5037] */
    (xdc_Char)0x61,  /* [5038] */
    (xdc_Char)0x70,  /* [5039] */
    (xdc_Char)0x4d,  /* [5040] */
    (xdc_Char)0x65,  /* [5041] */
    (xdc_Char)0x6d,  /* [5042] */
    (xdc_Char)0x0,  /* [5043] */
    (xdc_Char)0x74,  /* [5044] */
    (xdc_Char)0x69,  /* [5045] */
    (xdc_Char)0x6d,  /* [5046] */
    (xdc_Char)0x65,  /* [5047] */
    (xdc_Char)0x72,  /* [5048] */
    (xdc_Char)0x73,  /* [5049] */
    (xdc_Char)0x2e,  /* [5050] */
    (xdc_Char)0x0,  /* [5051] */
    (xdc_Char)0x64,  /* [5052] */
    (xdc_Char)0x6d,  /* [5053] */
    (xdc_Char)0x74,  /* [5054] */
    (xdc_Char)0x69,  /* [5055] */
    (xdc_Char)0x6d,  /* [5056] */
    (xdc_Char)0x65,  /* [5057] */
    (xdc_Char)0x72,  /* [5058] */
    (xdc_Char)0x2e,  /* [5059] */
    (xdc_Char)0x0,  /* [5060] */
    (xdc_Char)0x67,  /* [5061] */
    (xdc_Char)0x61,  /* [5062] */
    (xdc_Char)0x74,  /* [5063] */
    (xdc_Char)0x65,  /* [5064] */
    (xdc_Char)0x73,  /* [5065] */
    (xdc_Char)0x2e,  /* [5066] */
    (xdc_Char)0x0,  /* [5067] */
    (xdc_Char)0x47,  /* [5068] */
    (xdc_Char)0x61,  /* [5069] */
    (xdc_Char)0x74,  /* [5070] */
    (xdc_Char)0x65,  /* [5071] */
    (xdc_Char)0x48,  /* [5072] */
    (xdc_Char)0x77,  /* [5073] */
    (xdc_Char)0x69,  /* [5074] */
    (xdc_Char)0x0,  /* [5075] */
    (xdc_Char)0x47,  /* [5076] */
    (xdc_Char)0x61,  /* [5077] */
    (xdc_Char)0x74,  /* [5078] */
    (xdc_Char)0x65,  /* [5079] */
    (xdc_Char)0x4d,  /* [5080] */
    (xdc_Char)0x75,  /* [5081] */
    (xdc_Char)0x74,  /* [5082] */
    (xdc_Char)0x65,  /* [5083] */
    (xdc_Char)0x78,  /* [5084] */
    (xdc_Char)0x0,  /* [5085] */
    (xdc_Char)0x75,  /* [5086] */
    (xdc_Char)0x74,  /* [5087] */
    (xdc_Char)0x69,  /* [5088] */
    (xdc_Char)0x6c,  /* [5089] */
    (xdc_Char)0x73,  /* [5090] */
    (xdc_Char)0x2e,  /* [5091] */
    (xdc_Char)0x0,  /* [5092] */
    (xdc_Char)0x4c,  /* [5093] */
    (xdc_Char)0x6f,  /* [5094] */
    (xdc_Char)0x61,  /* [5095] */
    (xdc_Char)0x64,  /* [5096] */
    (xdc_Char)0x0,  /* [5097] */
    (xdc_Char)0x73,  /* [5098] */
    (xdc_Char)0x79,  /* [5099] */
    (xdc_Char)0x6e,  /* [5100] */
    (xdc_Char)0x63,  /* [5101] */
    (xdc_Char)0x73,  /* [5102] */
    (xdc_Char)0x2e,  /* [5103] */
    (xdc_Char)0x0,  /* [5104] */
    (xdc_Char)0x53,  /* [5105] */
    (xdc_Char)0x79,  /* [5106] */
    (xdc_Char)0x6e,  /* [5107] */
    (xdc_Char)0x63,  /* [5108] */
    (xdc_Char)0x53,  /* [5109] */
    (xdc_Char)0x65,  /* [5110] */
    (xdc_Char)0x6d,  /* [5111] */
    (xdc_Char)0x0,  /* [5112] */
    (xdc_Char)0x75,  /* [5113] */
    (xdc_Char)0x6e,  /* [5114] */
    (xdc_Char)0x69,  /* [5115] */
    (xdc_Char)0x63,  /* [5116] */
    (xdc_Char)0x61,  /* [5117] */
    (xdc_Char)0x63,  /* [5118] */
    (xdc_Char)0x68,  /* [5119] */
    (xdc_Char)0x65,  /* [5120] */
    (xdc_Char)0x2e,  /* [5121] */
    (xdc_Char)0x0,  /* [5122] */
    (xdc_Char)0x61,  /* [5123] */
    (xdc_Char)0x6d,  /* [5124] */
    (xdc_Char)0x6d,  /* [5125] */
    (xdc_Char)0x75,  /* [5126] */
    (xdc_Char)0x2e,  /* [5127] */
    (xdc_Char)0x0,  /* [5128] */
    (xdc_Char)0x41,  /* [5129] */
    (xdc_Char)0x4d,  /* [5130] */
    (xdc_Char)0x4d,  /* [5131] */
    (xdc_Char)0x55,  /* [5132] */
    (xdc_Char)0x0,  /* [5133] */
    (xdc_Char)0x74,  /* [5134] */
    (xdc_Char)0x69,  /* [5135] */
    (xdc_Char)0x2e,  /* [5136] */
    (xdc_Char)0x73,  /* [5137] */
    (xdc_Char)0x79,  /* [5138] */
    (xdc_Char)0x73,  /* [5139] */
    (xdc_Char)0x62,  /* [5140] */
    (xdc_Char)0x69,  /* [5141] */
    (xdc_Char)0x6f,  /* [5142] */
    (xdc_Char)0x73,  /* [5143] */
    (xdc_Char)0x2e,  /* [5144] */
    (xdc_Char)0x6b,  /* [5145] */
    (xdc_Char)0x6e,  /* [5146] */
    (xdc_Char)0x6c,  /* [5147] */
    (xdc_Char)0x2e,  /* [5148] */
    (xdc_Char)0x54,  /* [5149] */
    (xdc_Char)0x61,  /* [5150] */
    (xdc_Char)0x73,  /* [5151] */
    (xdc_Char)0x6b,  /* [5152] */
    (xdc_Char)0x2e,  /* [5153] */
    (xdc_Char)0x49,  /* [5154] */
    (xdc_Char)0x64,  /* [5155] */
    (xdc_Char)0x6c,  /* [5156] */
    (xdc_Char)0x65,  /* [5157] */
    (xdc_Char)0x54,  /* [5158] */
    (xdc_Char)0x61,  /* [5159] */
    (xdc_Char)0x73,  /* [5160] */
    (xdc_Char)0x6b,  /* [5161] */
    (xdc_Char)0x0,  /* [5162] */
};

/* --> xdc_runtime_Text_nodeTab__A */
const __T1_xdc_runtime_Text_nodeTab xdc_runtime_Text_nodeTab__A[62] = {
    {
        (xdc_Bits16)0x0,  /* left */
        (xdc_Bits16)0x0,  /* right */
    },  /* [0] */
    {
        (xdc_Bits16)0x1263,  /* left */
        (xdc_Bits16)0x1268,  /* right */
    },  /* [1] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x1271,  /* right */
    },  /* [2] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x1278,  /* right */
    },  /* [3] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x127d,  /* right */
    },  /* [4] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x1286,  /* right */
    },  /* [5] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x128c,  /* right */
    },  /* [6] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x1292,  /* right */
    },  /* [7] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x1297,  /* right */
    },  /* [8] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x129b,  /* right */
    },  /* [9] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12a0,  /* right */
    },  /* [10] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12a7,  /* right */
    },  /* [11] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12b0,  /* right */
    },  /* [12] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12b8,  /* right */
    },  /* [13] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12bf,  /* right */
    },  /* [14] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12c6,  /* right */
    },  /* [15] */
    {
        (xdc_Bits16)0x8001,  /* left */
        (xdc_Bits16)0x12cb,  /* right */
    },  /* [16] */
    {
        (xdc_Bits16)0x12d5,  /* left */
        (xdc_Bits16)0x12d9,  /* right */
    },  /* [17] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x12e2,  /* right */
    },  /* [18] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x12e7,  /* right */
    },  /* [19] */
    {
        (xdc_Bits16)0x8013,  /* left */
        (xdc_Bits16)0x12ec,  /* right */
    },  /* [20] */
    {
        (xdc_Bits16)0x8013,  /* left */
        (xdc_Bits16)0x12f0,  /* right */
    },  /* [21] */
    {
        (xdc_Bits16)0x8013,  /* left */
        (xdc_Bits16)0x12f6,  /* right */
    },  /* [22] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x12fc,  /* right */
    },  /* [23] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x1301,  /* right */
    },  /* [24] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x1307,  /* right */
    },  /* [25] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x130c,  /* right */
    },  /* [26] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x1317,  /* right */
    },  /* [27] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x131d,  /* right */
    },  /* [28] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x1327,  /* right */
    },  /* [29] */
    {
        (xdc_Bits16)0x8017,  /* left */
        (xdc_Bits16)0x132b,  /* right */
    },  /* [30] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x1330,  /* right */
    },  /* [31] */
    {
        (xdc_Bits16)0x801f,  /* left */
        (xdc_Bits16)0x1338,  /* right */
    },  /* [32] */
    {
        (xdc_Bits16)0x8020,  /* left */
        (xdc_Bits16)0x133d,  /* right */
    },  /* [33] */
    {
        (xdc_Bits16)0x8021,  /* left */
        (xdc_Bits16)0x12ec,  /* right */
    },  /* [34] */
    {
        (xdc_Bits16)0x8021,  /* left */
        (xdc_Bits16)0x1341,  /* right */
    },  /* [35] */
    {
        (xdc_Bits16)0x8021,  /* left */
        (xdc_Bits16)0x1353,  /* right */
    },  /* [36] */
    {
        (xdc_Bits16)0x8020,  /* left */
        (xdc_Bits16)0x135f,  /* right */
    },  /* [37] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x1278,  /* right */
    },  /* [38] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x1367,  /* right */
    },  /* [39] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x136b,  /* right */
    },  /* [40] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x12f0,  /* right */
    },  /* [41] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x1378,  /* right */
    },  /* [42] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x1385,  /* right */
    },  /* [43] */
    {
        (xdc_Bits16)0x8025,  /* left */
        (xdc_Bits16)0x1397,  /* right */
    },  /* [44] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x139d,  /* right */
    },  /* [45] */
    {
        (xdc_Bits16)0x802d,  /* left */
        (xdc_Bits16)0x13a4,  /* right */
    },  /* [46] */
    {
        (xdc_Bits16)0x802d,  /* left */
        (xdc_Bits16)0x13ac,  /* right */
    },  /* [47] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x13b4,  /* right */
    },  /* [48] */
    {
        (xdc_Bits16)0x8030,  /* left */
        (xdc_Bits16)0x13bc,  /* right */
    },  /* [49] */
    {
        (xdc_Bits16)0x8031,  /* left */
        (xdc_Bits16)0x12f0,  /* right */
    },  /* [50] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x13c5,  /* right */
    },  /* [51] */
    {
        (xdc_Bits16)0x8033,  /* left */
        (xdc_Bits16)0x13cc,  /* right */
    },  /* [52] */
    {
        (xdc_Bits16)0x8033,  /* left */
        (xdc_Bits16)0x13d4,  /* right */
    },  /* [53] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x13de,  /* right */
    },  /* [54] */
    {
        (xdc_Bits16)0x8036,  /* left */
        (xdc_Bits16)0x13e5,  /* right */
    },  /* [55] */
    {
        (xdc_Bits16)0x8011,  /* left */
        (xdc_Bits16)0x13ea,  /* right */
    },  /* [56] */
    {
        (xdc_Bits16)0x8038,  /* left */
        (xdc_Bits16)0x13f1,  /* right */
    },  /* [57] */
    {
        (xdc_Bits16)0x8013,  /* left */
        (xdc_Bits16)0x13f9,  /* right */
    },  /* [58] */
    {
        (xdc_Bits16)0x803a,  /* left */
        (xdc_Bits16)0x12f6,  /* right */
    },  /* [59] */
    {
        (xdc_Bits16)0x8013,  /* left */
        (xdc_Bits16)0x1403,  /* right */
    },  /* [60] */
    {
        (xdc_Bits16)0x803c,  /* left */
        (xdc_Bits16)0x1409,  /* right */
    },  /* [61] */
};

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Text_Module__diagsEnabled xdc_runtime_Text_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Text_Module__diagsIncluded xdc_runtime_Text_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Text_Module__diagsMask xdc_runtime_Text_Module__diagsMask__C = ((CT__xdc_runtime_Text_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Text_Module__gateObj xdc_runtime_Text_Module__gateObj__C = ((CT__xdc_runtime_Text_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Text_Module__gatePrms xdc_runtime_Text_Module__gatePrms__C = ((CT__xdc_runtime_Text_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Text_Module__id xdc_runtime_Text_Module__id__C = (xdc_Bits16)0x800f;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerDefined xdc_runtime_Text_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerObj xdc_runtime_Text_Module__loggerObj__C = ((CT__xdc_runtime_Text_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerFxn0 xdc_runtime_Text_Module__loggerFxn0__C = ((CT__xdc_runtime_Text_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerFxn1 xdc_runtime_Text_Module__loggerFxn1__C = ((CT__xdc_runtime_Text_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerFxn2 xdc_runtime_Text_Module__loggerFxn2__C = ((CT__xdc_runtime_Text_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerFxn4 xdc_runtime_Text_Module__loggerFxn4__C = ((CT__xdc_runtime_Text_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Text_Module__loggerFxn8 xdc_runtime_Text_Module__loggerFxn8__C = ((CT__xdc_runtime_Text_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Text_Module__startupDoneFxn xdc_runtime_Text_Module__startupDoneFxn__C = ((CT__xdc_runtime_Text_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Text_Object__count xdc_runtime_Text_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Text_Object__heap xdc_runtime_Text_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Text_Object__sizeof xdc_runtime_Text_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Text_Object__table xdc_runtime_Text_Object__table__C = 0;

/* nameUnknown__C */
__FAR__ const CT__xdc_runtime_Text_nameUnknown xdc_runtime_Text_nameUnknown__C = "{unknown-instance-name}";

/* nameEmpty__C */
__FAR__ const CT__xdc_runtime_Text_nameEmpty xdc_runtime_Text_nameEmpty__C = "{empty-instance-name}";

/* nameStatic__C */
__FAR__ const CT__xdc_runtime_Text_nameStatic xdc_runtime_Text_nameStatic__C = "{static-instance-name}";

/* isLoaded__C */
__FAR__ const CT__xdc_runtime_Text_isLoaded xdc_runtime_Text_isLoaded__C = 1;

/* charTab__C */
__FAR__ const CT__xdc_runtime_Text_charTab xdc_runtime_Text_charTab__C = ((CT__xdc_runtime_Text_charTab)xdc_runtime_Text_charTab__A);

/* nodeTab__C */
__FAR__ const CT__xdc_runtime_Text_nodeTab xdc_runtime_Text_nodeTab__C = ((CT__xdc_runtime_Text_nodeTab)xdc_runtime_Text_nodeTab__A);

/* charCnt__C */
__FAR__ const CT__xdc_runtime_Text_charCnt xdc_runtime_Text_charCnt__C = (xdc_Int16)0x142b;

/* nodeCnt__C */
__FAR__ const CT__xdc_runtime_Text_nodeCnt xdc_runtime_Text_nodeCnt__C = (xdc_Int16)0x3e;

/* unnamedModsLastId__C */
__FAR__ const CT__xdc_runtime_Text_unnamedModsLastId xdc_runtime_Text_unnamedModsLastId__C = (xdc_UInt16)0x4000;

/* registryModsLastId__C */
__FAR__ const CT__xdc_runtime_Text_registryModsLastId xdc_runtime_Text_registryModsLastId__C = (xdc_UInt16)0x7fff;

/* visitRopeFxn__C */
__FAR__ const CT__xdc_runtime_Text_visitRopeFxn xdc_runtime_Text_visitRopeFxn__C = ((CT__xdc_runtime_Text_visitRopeFxn)((xdc_Fxn)xdc_runtime_Text_visitRope__I));

/* visitRopeFxn2__C */
__FAR__ const CT__xdc_runtime_Text_visitRopeFxn2 xdc_runtime_Text_visitRopeFxn2__C = ((CT__xdc_runtime_Text_visitRopeFxn2)((xdc_Fxn)xdc_runtime_Text_visitRope2__I));


/*
 * ======== xdc.runtime.Timestamp INITIALIZERS ========
 */

/* Module__diagsEnabled__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__diagsEnabled xdc_runtime_Timestamp_Module__diagsEnabled__C = (xdc_Bits32)0x10;

/* Module__diagsIncluded__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__diagsIncluded xdc_runtime_Timestamp_Module__diagsIncluded__C = (xdc_Bits32)0x10;

/* Module__diagsMask__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__diagsMask xdc_runtime_Timestamp_Module__diagsMask__C = ((CT__xdc_runtime_Timestamp_Module__diagsMask)0);

/* Module__gateObj__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__gateObj xdc_runtime_Timestamp_Module__gateObj__C = ((CT__xdc_runtime_Timestamp_Module__gateObj)0);

/* Module__gatePrms__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__gatePrms xdc_runtime_Timestamp_Module__gatePrms__C = ((CT__xdc_runtime_Timestamp_Module__gatePrms)0);

/* Module__id__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__id xdc_runtime_Timestamp_Module__id__C = (xdc_Bits16)0x8010;

/* Module__loggerDefined__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerDefined xdc_runtime_Timestamp_Module__loggerDefined__C = 0;

/* Module__loggerObj__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerObj xdc_runtime_Timestamp_Module__loggerObj__C = ((CT__xdc_runtime_Timestamp_Module__loggerObj)0);

/* Module__loggerFxn0__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerFxn0 xdc_runtime_Timestamp_Module__loggerFxn0__C = ((CT__xdc_runtime_Timestamp_Module__loggerFxn0)0);

/* Module__loggerFxn1__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerFxn1 xdc_runtime_Timestamp_Module__loggerFxn1__C = ((CT__xdc_runtime_Timestamp_Module__loggerFxn1)0);

/* Module__loggerFxn2__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerFxn2 xdc_runtime_Timestamp_Module__loggerFxn2__C = ((CT__xdc_runtime_Timestamp_Module__loggerFxn2)0);

/* Module__loggerFxn4__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerFxn4 xdc_runtime_Timestamp_Module__loggerFxn4__C = ((CT__xdc_runtime_Timestamp_Module__loggerFxn4)0);

/* Module__loggerFxn8__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__loggerFxn8 xdc_runtime_Timestamp_Module__loggerFxn8__C = ((CT__xdc_runtime_Timestamp_Module__loggerFxn8)0);

/* Module__startupDoneFxn__C */
__FAR__ const CT__xdc_runtime_Timestamp_Module__startupDoneFxn xdc_runtime_Timestamp_Module__startupDoneFxn__C = ((CT__xdc_runtime_Timestamp_Module__startupDoneFxn)0);

/* Object__count__C */
__FAR__ const CT__xdc_runtime_Timestamp_Object__count xdc_runtime_Timestamp_Object__count__C = 0;

/* Object__heap__C */
__FAR__ const CT__xdc_runtime_Timestamp_Object__heap xdc_runtime_Timestamp_Object__heap__C = 0;

/* Object__sizeof__C */
__FAR__ const CT__xdc_runtime_Timestamp_Object__sizeof xdc_runtime_Timestamp_Object__sizeof__C = 0;

/* Object__table__C */
__FAR__ const CT__xdc_runtime_Timestamp_Object__table xdc_runtime_Timestamp_Object__table__C = 0;


/*
 * ======== xdc.runtime.Timestamp_SupportProxy INITIALIZERS ========
 */

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_Core_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_Core_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore FUNCTION STUBS ========
 */

/* query__E */
xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_query__E( xdc_Int qual ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_query__F(qual);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_Timer_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_Timer_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_TimerSupport_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_TimerSupport_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_TimestampProvider_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_TimestampProvider_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.family.arm.ducati.Wugen FUNCTION STUBS ========
 */

/* disableInterrupt__E */
xdc_Void ti_sysbios_family_arm_ducati_Wugen_disableInterrupt__E( xdc_UInt intNum ) 
{
    ti_sysbios_family_arm_ducati_Wugen_disableInterrupt__F(intNum);
}

/* enableInterrupt__E */
xdc_Void ti_sysbios_family_arm_ducati_Wugen_enableInterrupt__E( xdc_UInt intNum ) 
{
    ti_sysbios_family_arm_ducati_Wugen_enableInterrupt__F(intNum);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_ducati_Wugen_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_ducati_Wugen_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.family.arm.m3.Hwi FUNCTION STUBS ========
 */

/* getStackInfo__E */
xdc_Bool ti_sysbios_family_arm_m3_Hwi_getStackInfo__E( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth ) 
{
    return ti_sysbios_family_arm_m3_Hwi_getStackInfo__F(stkInfo, computeStackDepth);
}

/* Module_startup */
xdc_Int ti_sysbios_family_arm_m3_Hwi_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_family_arm_m3_Hwi_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.gates.GateHwi FUNCTION STUBS ========
 */

/* query__E */
xdc_Bool ti_sysbios_gates_GateHwi_query__E( xdc_Int qual ) 
{
    return ti_sysbios_gates_GateHwi_query__F(qual);
}


/*
 * ======== ti.sysbios.gates.GateMutex FUNCTION STUBS ========
 */

/* query__E */
xdc_Bool ti_sysbios_gates_GateMutex_query__E( xdc_Int qual ) 
{
    return ti_sysbios_gates_GateMutex_query__F(qual);
}


/*
 * ======== ti.sysbios.hal.Hwi FUNCTION STUBS ========
 */

/* getStackInfo__E */
xdc_Bool ti_sysbios_hal_Hwi_getStackInfo__E( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth ) 
{
    return ti_sysbios_hal_Hwi_getStackInfo__F(stkInfo, computeStackDepth);
}

/* Module_startup */
xdc_Int ti_sysbios_hal_Hwi_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_hal_Hwi_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_hal_Timer_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_hal_Timer_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.hal.ammu.AMMU FUNCTION STUBS ========
 */

/* invAll__E */
xdc_Void ti_sysbios_hal_ammu_AMMU_invAll__E( void ) 
{
    ti_sysbios_hal_ammu_AMMU_invAll__F();
}

/* dumpAmmu__E */
xdc_Void ti_sysbios_hal_ammu_AMMU_dumpAmmu__E( void ) 
{
    ti_sysbios_hal_ammu_AMMU_dumpAmmu__F();
}


/*
 * ======== ti.sysbios.hal.unicache.Cache FUNCTION STUBS ========
 */

/* dumpRegs__E */
xdc_Void ti_sysbios_hal_unicache_Cache_dumpRegs__E( void ) 
{
    ti_sysbios_hal_unicache_Cache_dumpRegs__F();
}

/* Module_startup */
xdc_Int ti_sysbios_hal_unicache_Cache_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_hal_unicache_Cache_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.heaps.HeapBuf FUNCTION STUBS ========
 */

/* free__E */
xdc_Void ti_sysbios_heaps_HeapBuf_free__E( ti_sysbios_heaps_HeapBuf_Handle __inst, xdc_Ptr block, xdc_SizeT size ) 
{
    ti_sysbios_heaps_HeapBuf_free__F((void*)__inst, block, size);
}

/* getStats__E */
xdc_Void ti_sysbios_heaps_HeapBuf_getStats__E( ti_sysbios_heaps_HeapBuf_Handle __inst, xdc_runtime_Memory_Stats* stats ) 
{
    ti_sysbios_heaps_HeapBuf_getStats__F((void*)__inst, stats);
}

/* Module_startup */
xdc_Int ti_sysbios_heaps_HeapBuf_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_heaps_HeapBuf_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_knl_Clock_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_knl_Clock_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_knl_Swi_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_knl_Swi_Module_startup__F(state);
}

/* Module_startup */
xdc_Int ti_sysbios_knl_Task_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_knl_Task_Module_startup__F(state);
}


/*
 * ======== ti.sysbios.syncs.SyncSem FUNCTION STUBS ========
 */

/* query__E */
xdc_Bool ti_sysbios_syncs_SyncSem_query__E( ti_sysbios_syncs_SyncSem_Handle __inst, xdc_Int qual ) 
{
    return ti_sysbios_syncs_SyncSem_query__F((void*)__inst, qual);
}

/* signal__E */
xdc_Void ti_sysbios_syncs_SyncSem_signal__E( ti_sysbios_syncs_SyncSem_Handle __inst ) 
{
    ti_sysbios_syncs_SyncSem_signal__F((void*)__inst);
}

/* wait__E */
xdc_Int ti_sysbios_syncs_SyncSem_wait__E( ti_sysbios_syncs_SyncSem_Handle __inst, xdc_UInt timeout, xdc_runtime_Error_Block* eb ) 
{
    return ti_sysbios_syncs_SyncSem_wait__F((void*)__inst, timeout, eb);
}


/*
 * ======== ti.sysbios.timers.dmtimer.Timer FUNCTION STUBS ========
 */

/* setAvailMask__E */
xdc_Void ti_sysbios_timers_dmtimer_Timer_setAvailMask__E( xdc_UArg arg ) 
{
    ti_sysbios_timers_dmtimer_Timer_setAvailMask__F(arg);
}

/* Module_startup */
xdc_Int ti_sysbios_timers_dmtimer_Timer_Module_startup__E( xdc_Int state )
{
    return ti_sysbios_timers_dmtimer_Timer_Module_startup__F(state);
}


/*
 * ======== xdc.runtime.Diags FUNCTION STUBS ========
 */

/* setMask__E */
xdc_Void xdc_runtime_Diags_setMask__E( xdc_String control ) 
{
    xdc_runtime_Diags_setMask__F(control);
}


/*
 * ======== xdc.runtime.Error FUNCTION STUBS ========
 */

/* check__E */
xdc_Bool xdc_runtime_Error_check__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_check__F(eb);
}

/* getData__E */
xdc_runtime_Error_Data* xdc_runtime_Error_getData__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_getData__F(eb);
}

/* getCode__E */
xdc_UInt16 xdc_runtime_Error_getCode__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_getCode__F(eb);
}

/* getId__E */
xdc_runtime_Error_Id xdc_runtime_Error_getId__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_getId__F(eb);
}

/* getMsg__E */
xdc_String xdc_runtime_Error_getMsg__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_getMsg__F(eb);
}

/* getSite__E */
xdc_runtime_Types_Site* xdc_runtime_Error_getSite__E( xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Error_getSite__F(eb);
}

/* init__E */
xdc_Void xdc_runtime_Error_init__E( xdc_runtime_Error_Block* eb ) 
{
    xdc_runtime_Error_init__F(eb);
}

/* print__E */
xdc_Void xdc_runtime_Error_print__E( xdc_runtime_Error_Block* eb ) 
{
    xdc_runtime_Error_print__F(eb);
}

/* raiseX__E */
xdc_Void xdc_runtime_Error_raiseX__E( xdc_runtime_Error_Block* eb, xdc_runtime_Types_ModuleId mod, xdc_String file, xdc_Int line, xdc_runtime_Error_Id id, xdc_IArg arg1, xdc_IArg arg2 ) 
{
    xdc_runtime_Error_raiseX__F(eb, mod, file, line, id, arg1, arg2);
}


/*
 * ======== xdc.runtime.Gate FUNCTION STUBS ========
 */

/* enterSystem__E */
xdc_IArg xdc_runtime_Gate_enterSystem__E( void ) 
{
    return xdc_runtime_Gate_enterSystem__F();
}

/* leaveSystem__E */
xdc_Void xdc_runtime_Gate_leaveSystem__E( xdc_IArg key ) 
{
    xdc_runtime_Gate_leaveSystem__F(key);
}


/*
 * ======== xdc.runtime.Log FUNCTION STUBS ========
 */

/* doPrint__E */
xdc_Void xdc_runtime_Log_doPrint__E( xdc_runtime_Log_EventRec* evRec ) 
{
    xdc_runtime_Log_doPrint__F(evRec);
}


/*
 * ======== xdc.runtime.Memory FUNCTION STUBS ========
 */

/* alloc__E */
xdc_Ptr xdc_runtime_Memory_alloc__E( xdc_runtime_IHeap_Handle heap, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Memory_alloc__F(heap, size, align, eb);
}

/* calloc__E */
xdc_Ptr xdc_runtime_Memory_calloc__E( xdc_runtime_IHeap_Handle heap, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Memory_calloc__F(heap, size, align, eb);
}

/* free__E */
xdc_Void xdc_runtime_Memory_free__E( xdc_runtime_IHeap_Handle heap, xdc_Ptr block, xdc_SizeT size ) 
{
    xdc_runtime_Memory_free__F(heap, block, size);
}

/* getStats__E */
xdc_Void xdc_runtime_Memory_getStats__E( xdc_runtime_IHeap_Handle heap, xdc_runtime_Memory_Stats* stats ) 
{
    xdc_runtime_Memory_getStats__F(heap, stats);
}

/* query__E */
xdc_Bool xdc_runtime_Memory_query__E( xdc_runtime_IHeap_Handle heap, xdc_Int qual ) 
{
    return xdc_runtime_Memory_query__F(heap, qual);
}

/* getMaxDefaultTypeAlign__E */
xdc_SizeT xdc_runtime_Memory_getMaxDefaultTypeAlign__E( void ) 
{
    return xdc_runtime_Memory_getMaxDefaultTypeAlign__F();
}

/* valloc__E */
xdc_Ptr xdc_runtime_Memory_valloc__E( xdc_runtime_IHeap_Handle heap, xdc_SizeT size, xdc_SizeT align, xdc_Char value, xdc_runtime_Error_Block* eb ) 
{
    return xdc_runtime_Memory_valloc__F(heap, size, align, value, eb);
}


/*
 * ======== xdc.runtime.Registry FUNCTION STUBS ========
 */

/* addModule__E */
xdc_runtime_Registry_Result xdc_runtime_Registry_addModule__E( xdc_runtime_Registry_Desc* desc, xdc_String modName ) 
{
    return xdc_runtime_Registry_addModule__F(desc, modName);
}

/* findByName__E */
xdc_runtime_Registry_Desc* xdc_runtime_Registry_findByName__E( xdc_String modName ) 
{
    return xdc_runtime_Registry_findByName__F(modName);
}

/* findByNamePattern__E */
xdc_runtime_Registry_Desc* xdc_runtime_Registry_findByNamePattern__E( xdc_String namePat, xdc_Int len, xdc_runtime_Registry_Desc* prev ) 
{
    return xdc_runtime_Registry_findByNamePattern__F(namePat, len, prev);
}

/* findById__E */
xdc_runtime_Registry_Desc* xdc_runtime_Registry_findById__E( xdc_runtime_Types_ModuleId mid ) 
{
    return xdc_runtime_Registry_findById__F(mid);
}

/* getMask__E */
xdc_Bool xdc_runtime_Registry_getMask__E( xdc_String name, xdc_runtime_Types_DiagsMask* mask ) 
{
    return xdc_runtime_Registry_getMask__F(name, mask);
}

/* isMember__E */
xdc_Bool xdc_runtime_Registry_isMember__E( xdc_runtime_Types_ModuleId mid ) 
{
    return xdc_runtime_Registry_isMember__F(mid);
}

/* getNextModule__E */
xdc_runtime_Registry_Desc* xdc_runtime_Registry_getNextModule__E( xdc_runtime_Registry_Desc* desc ) 
{
    return xdc_runtime_Registry_getNextModule__F(desc);
}

/* getModuleName__E */
xdc_String xdc_runtime_Registry_getModuleName__E( xdc_runtime_Registry_Desc* desc ) 
{
    return xdc_runtime_Registry_getModuleName__F(desc);
}

/* getModuleId__E */
xdc_runtime_Types_ModuleId xdc_runtime_Registry_getModuleId__E( xdc_runtime_Registry_Desc* desc ) 
{
    return xdc_runtime_Registry_getModuleId__F(desc);
}


/*
 * ======== xdc.runtime.Startup FUNCTION STUBS ========
 */

/* exec__E */
xdc_Void xdc_runtime_Startup_exec__E( void ) 
{
    xdc_runtime_Startup_exec__F();
}

/* rtsDone__E */
xdc_Bool xdc_runtime_Startup_rtsDone__E( void ) 
{
    return xdc_runtime_Startup_rtsDone__F();
}


/*
 * ======== xdc.runtime.SysStd FUNCTION STUBS ========
 */

/* abort__E */
xdc_Void xdc_runtime_SysStd_abort__E( xdc_String str ) 
{
    xdc_runtime_SysStd_abort__F(str);
}

/* exit__E */
xdc_Void xdc_runtime_SysStd_exit__E( xdc_Int stat ) 
{
    xdc_runtime_SysStd_exit__F(stat);
}

/* flush__E */
xdc_Void xdc_runtime_SysStd_flush__E( void ) 
{
    xdc_runtime_SysStd_flush__F();
}

/* putch__E */
xdc_Void xdc_runtime_SysStd_putch__E( xdc_Char ch ) 
{
    xdc_runtime_SysStd_putch__F(ch);
}

/* ready__E */
xdc_Bool xdc_runtime_SysStd_ready__E( void ) 
{
    return xdc_runtime_SysStd_ready__F();
}


/*
 * ======== xdc.runtime.System FUNCTION STUBS ========
 */

/* abort__E */
xdc_Void xdc_runtime_System_abort__E( xdc_String str ) 
{
    xdc_runtime_System_abort__F(str);
}

/* atexit__E */
xdc_Bool xdc_runtime_System_atexit__E( xdc_runtime_System_AtexitHandler handler ) 
{
    return xdc_runtime_System_atexit__F(handler);
}

/* exit__E */
xdc_Void xdc_runtime_System_exit__E( xdc_Int stat ) 
{
    xdc_runtime_System_exit__F(stat);
}

/* putch__E */
xdc_Void xdc_runtime_System_putch__E( xdc_Char ch ) 
{
    xdc_runtime_System_putch__F(ch);
}

/* flush__E */
xdc_Void xdc_runtime_System_flush__E( void ) 
{
    xdc_runtime_System_flush__F();
}

/* printf_va__E */
xdc_Int xdc_runtime_System_printf_va__E( xdc_String fmt, va_list __va ) 
{
    return xdc_runtime_System_printf_va__F(fmt, __va);
}

/* printf__E */
xdc_Int xdc_runtime_System_printf__E( xdc_String fmt, ... ) 
{
    xdc_Int __ret;

    va_list __va; va_start(__va, fmt);
    __ret = xdc_runtime_System_printf_va__F(fmt, __va);

    va_end(__va);
    return __ret;
}

/* aprintf_va__E */
xdc_Int xdc_runtime_System_aprintf_va__E( xdc_String fmt, va_list __va ) 
{
    return xdc_runtime_System_aprintf_va__F(fmt, __va);
}

/* aprintf__E */
xdc_Int xdc_runtime_System_aprintf__E( xdc_String fmt, ... ) 
{
    xdc_Int __ret;

    va_list __va; va_start(__va, fmt);
    __ret = xdc_runtime_System_aprintf_va__F(fmt, __va);

    va_end(__va);
    return __ret;
}

/* sprintf_va__E */
xdc_Int xdc_runtime_System_sprintf_va__E( xdc_Char buf[], xdc_String fmt, va_list __va ) 
{
    return xdc_runtime_System_sprintf_va__F(buf, fmt, __va);
}

/* sprintf__E */
xdc_Int xdc_runtime_System_sprintf__E( xdc_Char buf[], xdc_String fmt, ... ) 
{
    xdc_Int __ret;

    va_list __va; va_start(__va, fmt);
    __ret = xdc_runtime_System_sprintf_va__F(buf, fmt, __va);

    va_end(__va);
    return __ret;
}

/* asprintf_va__E */
xdc_Int xdc_runtime_System_asprintf_va__E( xdc_Char buf[], xdc_String fmt, va_list __va ) 
{
    return xdc_runtime_System_asprintf_va__F(buf, fmt, __va);
}

/* asprintf__E */
xdc_Int xdc_runtime_System_asprintf__E( xdc_Char buf[], xdc_String fmt, ... ) 
{
    xdc_Int __ret;

    va_list __va; va_start(__va, fmt);
    __ret = xdc_runtime_System_asprintf_va__F(buf, fmt, __va);

    va_end(__va);
    return __ret;
}

/* vprintf__E */
xdc_Int xdc_runtime_System_vprintf__E( xdc_String fmt, xdc_VaList va ) 
{
    return xdc_runtime_System_vprintf__F(fmt, va);
}

/* avprintf__E */
xdc_Int xdc_runtime_System_avprintf__E( xdc_String fmt, xdc_VaList va ) 
{
    return xdc_runtime_System_avprintf__F(fmt, va);
}

/* vsprintf__E */
xdc_Int xdc_runtime_System_vsprintf__E( xdc_Char buf[], xdc_String fmt, xdc_VaList va ) 
{
    return xdc_runtime_System_vsprintf__F(buf, fmt, va);
}

/* avsprintf__E */
xdc_Int xdc_runtime_System_avsprintf__E( xdc_Char buf[], xdc_String fmt, xdc_VaList va ) 
{
    return xdc_runtime_System_avsprintf__F(buf, fmt, va);
}

/* Module_startup */
xdc_Int xdc_runtime_System_Module_startup__E( xdc_Int state )
{
    return xdc_runtime_System_Module_startup__F(state);
}


/*
 * ======== xdc.runtime.Text FUNCTION STUBS ========
 */

/* cordText__E */
xdc_String xdc_runtime_Text_cordText__E( xdc_runtime_Text_CordAddr cord ) 
{
    return xdc_runtime_Text_cordText__F(cord);
}

/* ropeText__E */
xdc_String xdc_runtime_Text_ropeText__E( xdc_runtime_Text_RopeId rope ) 
{
    return xdc_runtime_Text_ropeText__F(rope);
}

/* matchRope__E */
xdc_Int xdc_runtime_Text_matchRope__E( xdc_runtime_Text_RopeId rope, xdc_String pat, xdc_Int* lenp ) 
{
    return xdc_runtime_Text_matchRope__F(rope, pat, lenp);
}

/* putLab__E */
xdc_Int xdc_runtime_Text_putLab__E( xdc_runtime_Types_Label* lab, xdc_Char** bufp, xdc_Int len ) 
{
    return xdc_runtime_Text_putLab__F(lab, bufp, len);
}

/* putMod__E */
xdc_Int xdc_runtime_Text_putMod__E( xdc_runtime_Types_ModuleId mid, xdc_Char** bufp, xdc_Int len ) 
{
    return xdc_runtime_Text_putMod__F(mid, bufp, len);
}

/* putSite__E */
xdc_Int xdc_runtime_Text_putSite__E( xdc_runtime_Types_Site* site, xdc_Char** bufp, xdc_Int len ) 
{
    return xdc_runtime_Text_putSite__F(site, bufp, len);
}


/*
 * ======== xdc.runtime.Timestamp FUNCTION STUBS ========
 */

/* get32__E */
xdc_Bits32 xdc_runtime_Timestamp_get32__E( void ) 
{
    return xdc_runtime_Timestamp_get32__F();
}

/* get64__E */
xdc_Void xdc_runtime_Timestamp_get64__E( xdc_runtime_Types_Timestamp64* result ) 
{
    xdc_runtime_Timestamp_get64__F(result);
}

/* getFreq__E */
xdc_Void xdc_runtime_Timestamp_getFreq__E( xdc_runtime_Types_FreqHz* freq ) 
{
    xdc_runtime_Timestamp_getFreq__F(freq);
}


/*
 * ======== ti.sysbios.BIOS_RtsGateProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.gates.GateMutex */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_BIOS_RtsGateProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_gates_GateMutex_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr ti_sysbios_BIOS_RtsGateProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_BIOS_RtsGateProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_gates_GateMutex_Object__create__S(oa, osz, aa, (ti_sysbios_gates_GateMutex___ParamsPtr)pa, sizeof(xdc_runtime_IGateProvider_Params), eb);
}

/* Object__delete__S */
void ti_sysbios_BIOS_RtsGateProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_gates_GateMutex_Object__delete__S(instp);
}

/* Params__init__S */
void ti_sysbios_BIOS_RtsGateProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_gates_GateMutex_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_BIOS_RtsGateProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_gates_GateMutex_Handle__label__S(obj, lab);
}

/* query__E */
xdc_Bool ti_sysbios_BIOS_RtsGateProxy_query__E( xdc_Int qual )
{
    return ti_sysbios_gates_GateMutex_query(qual);
}

/* enter__E */
xdc_IArg ti_sysbios_BIOS_RtsGateProxy_enter__E( ti_sysbios_BIOS_RtsGateProxy_Handle __inst )
{
    return ti_sysbios_gates_GateMutex_enter((ti_sysbios_gates_GateMutex_Handle)__inst);
}

/* leave__E */
xdc_Void ti_sysbios_BIOS_RtsGateProxy_leave__E( ti_sysbios_BIOS_RtsGateProxy_Handle __inst, xdc_IArg key )
{
    ti_sysbios_gates_GateMutex_leave((ti_sysbios_gates_GateMutex_Handle)__inst, key);
}


/*
 * ======== ti.sysbios.hal.Cache_CacheProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.hal.unicache.Cache */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Cache_CacheProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_hal_unicache_Cache_Module__startupDone__S();
}

/* enable__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_enable__E( xdc_Bits16 type )
{
    ti_sysbios_hal_unicache_Cache_enable(type);
}

/* disable__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_disable__E( xdc_Bits16 type )
{
    ti_sysbios_hal_unicache_Cache_disable(type);
}

/* inv__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_inv__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    ti_sysbios_hal_unicache_Cache_inv(blockPtr, byteCnt, type, wait);
}

/* wb__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_wb__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    ti_sysbios_hal_unicache_Cache_wb(blockPtr, byteCnt, type, wait);
}

/* wbInv__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_wbInv__E( xdc_Ptr blockPtr, xdc_SizeT byteCnt, xdc_Bits16 type, xdc_Bool wait )
{
    ti_sysbios_hal_unicache_Cache_wbInv(blockPtr, byteCnt, type, wait);
}

/* wbAll__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_wbAll__E( void )
{
    ti_sysbios_hal_unicache_Cache_wbAll();
}

/* wbInvAll__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_wbInvAll__E( void )
{
    ti_sysbios_hal_unicache_Cache_wbInvAll();
}

/* wait__E */
xdc_Void ti_sysbios_hal_Cache_CacheProxy_wait__E( void )
{
    ti_sysbios_hal_unicache_Cache_wait();
}


/*
 * ======== ti.sysbios.hal.Hwi_HwiProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.m3.Hwi */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Hwi_HwiProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_m3_Hwi_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr ti_sysbios_hal_Hwi_HwiProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_hal_Hwi_HwiProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_family_arm_m3_Hwi_Object__create__S(oa, osz, aa, (ti_sysbios_family_arm_m3_Hwi___ParamsPtr)pa, sizeof(ti_sysbios_interfaces_IHwi_Params), eb);
}

/* Object__delete__S */
void ti_sysbios_hal_Hwi_HwiProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_family_arm_m3_Hwi_Object__delete__S(instp);
}

/* Params__init__S */
void ti_sysbios_hal_Hwi_HwiProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_family_arm_m3_Hwi_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_hal_Hwi_HwiProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_family_arm_m3_Hwi_Handle__label__S(obj, lab);
}

/* getStackInfo__E */
xdc_Bool ti_sysbios_hal_Hwi_HwiProxy_getStackInfo__E( ti_sysbios_interfaces_IHwi_StackInfo* stkInfo, xdc_Bool computeStackDepth )
{
    return ti_sysbios_family_arm_m3_Hwi_getStackInfo(stkInfo, computeStackDepth);
}

/* startup__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_startup__E( void )
{
    ti_sysbios_family_arm_m3_Hwi_startup();
}

/* disable__E */
xdc_UInt ti_sysbios_hal_Hwi_HwiProxy_disable__E( void )
{
    return ti_sysbios_family_arm_m3_Hwi_disable();
}

/* enable__E */
xdc_UInt ti_sysbios_hal_Hwi_HwiProxy_enable__E( void )
{
    return ti_sysbios_family_arm_m3_Hwi_enable();
}

/* restore__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_restore__E( xdc_UInt key )
{
    ti_sysbios_family_arm_m3_Hwi_restore(key);
}

/* switchFromBootStack__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_switchFromBootStack__E( void )
{
    ti_sysbios_family_arm_m3_Hwi_switchFromBootStack();
}

/* post__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_post__E( xdc_UInt intNum )
{
    ti_sysbios_family_arm_m3_Hwi_post(intNum);
}

/* getTaskSP__E */
xdc_Char* ti_sysbios_hal_Hwi_HwiProxy_getTaskSP__E( void )
{
    return ti_sysbios_family_arm_m3_Hwi_getTaskSP();
}

/* disableInterrupt__E */
xdc_UInt ti_sysbios_hal_Hwi_HwiProxy_disableInterrupt__E( xdc_UInt intNum )
{
    return ti_sysbios_family_arm_m3_Hwi_disableInterrupt(intNum);
}

/* enableInterrupt__E */
xdc_UInt ti_sysbios_hal_Hwi_HwiProxy_enableInterrupt__E( xdc_UInt intNum )
{
    return ti_sysbios_family_arm_m3_Hwi_enableInterrupt(intNum);
}

/* restoreInterrupt__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_restoreInterrupt__E( xdc_UInt intNum, xdc_UInt key )
{
    ti_sysbios_family_arm_m3_Hwi_restoreInterrupt(intNum, key);
}

/* clearInterrupt__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_clearInterrupt__E( xdc_UInt intNum )
{
    ti_sysbios_family_arm_m3_Hwi_clearInterrupt(intNum);
}

/* getFunc__E */
ti_sysbios_interfaces_IHwi_FuncPtr ti_sysbios_hal_Hwi_HwiProxy_getFunc__E( ti_sysbios_hal_Hwi_HwiProxy_Handle __inst, xdc_UArg* arg )
{
    return ti_sysbios_family_arm_m3_Hwi_getFunc((ti_sysbios_family_arm_m3_Hwi_Handle)__inst, arg);
}

/* setFunc__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_setFunc__E( ti_sysbios_hal_Hwi_HwiProxy_Handle __inst, ti_sysbios_interfaces_IHwi_FuncPtr fxn, xdc_UArg arg )
{
    ti_sysbios_family_arm_m3_Hwi_setFunc((ti_sysbios_family_arm_m3_Hwi_Handle)__inst, fxn, arg);
}

/* getHookContext__E */
xdc_Ptr ti_sysbios_hal_Hwi_HwiProxy_getHookContext__E( ti_sysbios_hal_Hwi_HwiProxy_Handle __inst, xdc_Int id )
{
    return ti_sysbios_family_arm_m3_Hwi_getHookContext((ti_sysbios_family_arm_m3_Hwi_Handle)__inst, id);
}

/* setHookContext__E */
xdc_Void ti_sysbios_hal_Hwi_HwiProxy_setHookContext__E( ti_sysbios_hal_Hwi_HwiProxy_Handle __inst, xdc_Int id, xdc_Ptr hookContext )
{
    ti_sysbios_family_arm_m3_Hwi_setHookContext((ti_sysbios_family_arm_m3_Hwi_Handle)__inst, id, hookContext);
}

/* getIrp__E */
ti_sysbios_interfaces_IHwi_Irp ti_sysbios_hal_Hwi_HwiProxy_getIrp__E( ti_sysbios_hal_Hwi_HwiProxy_Handle __inst )
{
    return ti_sysbios_family_arm_m3_Hwi_getIrp((ti_sysbios_family_arm_m3_Hwi_Handle)__inst);
}


/*
 * ======== ti.sysbios.hal.Timer_TimerProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.ducati.Timer */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Timer_TimerProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_Timer_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr ti_sysbios_hal_Timer_TimerProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_hal_Timer_TimerProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_family_arm_ducati_Timer_Object__create__S(oa, osz, aa, (ti_sysbios_family_arm_ducati_Timer___ParamsPtr)pa, sizeof(ti_sysbios_interfaces_ITimer_Params), eb);
}

/* Object__delete__S */
void ti_sysbios_hal_Timer_TimerProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_family_arm_ducati_Timer_Object__delete__S(instp);
}

/* Params__init__S */
void ti_sysbios_hal_Timer_TimerProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_family_arm_ducati_Timer_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_hal_Timer_TimerProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_family_arm_ducati_Timer_Handle__label__S(obj, lab);
}

/* getNumTimers__E */
xdc_UInt ti_sysbios_hal_Timer_TimerProxy_getNumTimers__E( void )
{
    return ti_sysbios_family_arm_ducati_Timer_getNumTimers();
}

/* getStatus__E */
ti_sysbios_interfaces_ITimer_Status ti_sysbios_hal_Timer_TimerProxy_getStatus__E( xdc_UInt id )
{
    return ti_sysbios_family_arm_ducati_Timer_getStatus(id);
}

/* startup__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_startup__E( void )
{
    ti_sysbios_family_arm_ducati_Timer_startup();
}

/* getMaxTicks__E */
xdc_UInt32 ti_sysbios_hal_Timer_TimerProxy_getMaxTicks__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UInt32 periodCounts )
{
    return ti_sysbios_family_arm_ducati_Timer_getMaxTicks((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, periodCounts);
}

/* setNextTick__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_setNextTick__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UInt32 newPeriod, xdc_UInt32 countsPerTick )
{
    ti_sysbios_family_arm_ducati_Timer_setNextTick((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, newPeriod, countsPerTick);
}

/* start__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_start__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst )
{
    ti_sysbios_family_arm_ducati_Timer_start((ti_sysbios_family_arm_ducati_Timer_Handle)__inst);
}

/* stop__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_stop__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst )
{
    ti_sysbios_family_arm_ducati_Timer_stop((ti_sysbios_family_arm_ducati_Timer_Handle)__inst);
}

/* setPeriod__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_setPeriod__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UInt32 period )
{
    ti_sysbios_family_arm_ducati_Timer_setPeriod((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, period);
}

/* setPeriodMicroSecs__E */
xdc_Bool ti_sysbios_hal_Timer_TimerProxy_setPeriodMicroSecs__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UInt32 microsecs )
{
    return ti_sysbios_family_arm_ducati_Timer_setPeriodMicroSecs((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, microsecs);
}

/* getPeriod__E */
xdc_UInt32 ti_sysbios_hal_Timer_TimerProxy_getPeriod__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst )
{
    return ti_sysbios_family_arm_ducati_Timer_getPeriod((ti_sysbios_family_arm_ducati_Timer_Handle)__inst);
}

/* getCount__E */
xdc_UInt32 ti_sysbios_hal_Timer_TimerProxy_getCount__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst )
{
    return ti_sysbios_family_arm_ducati_Timer_getCount((ti_sysbios_family_arm_ducati_Timer_Handle)__inst);
}

/* getFreq__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_getFreq__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_runtime_Types_FreqHz* freq )
{
    ti_sysbios_family_arm_ducati_Timer_getFreq((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, freq);
}

/* getFunc__E */
ti_sysbios_interfaces_ITimer_FuncPtr ti_sysbios_hal_Timer_TimerProxy_getFunc__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UArg* arg )
{
    return ti_sysbios_family_arm_ducati_Timer_getFunc((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, arg);
}

/* setFunc__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_setFunc__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, ti_sysbios_interfaces_ITimer_FuncPtr fxn, xdc_UArg arg )
{
    ti_sysbios_family_arm_ducati_Timer_setFunc((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, fxn, arg);
}

/* trigger__E */
xdc_Void ti_sysbios_hal_Timer_TimerProxy_trigger__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst, xdc_UInt32 cycles )
{
    ti_sysbios_family_arm_ducati_Timer_trigger((ti_sysbios_family_arm_ducati_Timer_Handle)__inst, cycles);
}

/* getExpiredCounts__E */
xdc_UInt32 ti_sysbios_hal_Timer_TimerProxy_getExpiredCounts__E( ti_sysbios_hal_Timer_TimerProxy_Handle __inst )
{
    return ti_sysbios_family_arm_ducati_Timer_getExpiredCounts((ti_sysbios_family_arm_ducati_Timer_Handle)__inst);
}


/*
 * ======== ti.sysbios.hal.unicache.Cache_Module_GateProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.ducati.GateDualCore */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr ti_sysbios_hal_unicache_Cache_Module_GateProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_hal_unicache_Cache_Module_GateProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S(oa, osz, aa, (ti_sysbios_family_arm_ducati_GateDualCore___ParamsPtr)pa, sizeof(xdc_runtime_IGateProvider_Params), eb);
}

/* Object__delete__S */
void ti_sysbios_hal_unicache_Cache_Module_GateProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S(instp);
}

/* Params__init__S */
void ti_sysbios_hal_unicache_Cache_Module_GateProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_hal_unicache_Cache_Module_GateProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S(obj, lab);
}

/* query__E */
xdc_Bool ti_sysbios_hal_unicache_Cache_Module_GateProxy_query__E( xdc_Int qual )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_query(qual);
}

/* enter__E */
xdc_IArg ti_sysbios_hal_unicache_Cache_Module_GateProxy_enter__E( ti_sysbios_hal_unicache_Cache_Module_GateProxy_Handle __inst )
{
    return ti_sysbios_family_arm_ducati_GateDualCore_enter((ti_sysbios_family_arm_ducati_GateDualCore_Handle)__inst);
}

/* leave__E */
xdc_Void ti_sysbios_hal_unicache_Cache_Module_GateProxy_leave__E( ti_sysbios_hal_unicache_Cache_Module_GateProxy_Handle __inst, xdc_IArg key )
{
    ti_sysbios_family_arm_ducati_GateDualCore_leave((ti_sysbios_family_arm_ducati_GateDualCore_Handle)__inst, key);
}


/*
 * ======== ti.sysbios.heaps.HeapMem_Module_GateProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.gates.GateMutex */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_gates_GateMutex_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr ti_sysbios_heaps_HeapMem_Module_GateProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_heaps_HeapMem_Module_GateProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_gates_GateMutex_Object__create__S(oa, osz, aa, (ti_sysbios_gates_GateMutex___ParamsPtr)pa, sizeof(xdc_runtime_IGateProvider_Params), eb);
}

/* Object__delete__S */
void ti_sysbios_heaps_HeapMem_Module_GateProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_gates_GateMutex_Object__delete__S(instp);
}

/* Params__init__S */
void ti_sysbios_heaps_HeapMem_Module_GateProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_gates_GateMutex_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_heaps_HeapMem_Module_GateProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_gates_GateMutex_Handle__label__S(obj, lab);
}

/* query__E */
xdc_Bool ti_sysbios_heaps_HeapMem_Module_GateProxy_query__E( xdc_Int qual )
{
    return ti_sysbios_gates_GateMutex_query(qual);
}

/* enter__E */
xdc_IArg ti_sysbios_heaps_HeapMem_Module_GateProxy_enter__E( ti_sysbios_heaps_HeapMem_Module_GateProxy_Handle __inst )
{
    return ti_sysbios_gates_GateMutex_enter((ti_sysbios_gates_GateMutex_Handle)__inst);
}

/* leave__E */
xdc_Void ti_sysbios_heaps_HeapMem_Module_GateProxy_leave__E( ti_sysbios_heaps_HeapMem_Module_GateProxy_Handle __inst, xdc_IArg key )
{
    ti_sysbios_gates_GateMutex_leave((ti_sysbios_gates_GateMutex_Handle)__inst, key);
}


/*
 * ======== ti.sysbios.knl.Intrinsics_SupportProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.m3.IntrinsicsSupport */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Intrinsics_SupportProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDone__S();
}

/* maxbit__E */
xdc_UInt ti_sysbios_knl_Intrinsics_SupportProxy_maxbit__E( xdc_UInt bits )
{
    return ti_sysbios_family_arm_m3_IntrinsicsSupport_maxbit(bits);
}


/*
 * ======== ti.sysbios.knl.Task_SupportProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.m3.TaskSupport */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Task_SupportProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S();
}

/* start__E */
xdc_Ptr ti_sysbios_knl_Task_SupportProxy_start__E( xdc_Ptr curTask, ti_sysbios_interfaces_ITaskSupport_FuncPtr enter, ti_sysbios_interfaces_ITaskSupport_FuncPtr exit, xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_family_arm_m3_TaskSupport_start(curTask, enter, exit, eb);
}

/* swap__E */
xdc_Void ti_sysbios_knl_Task_SupportProxy_swap__E( xdc_Ptr* oldtskContext, xdc_Ptr* newtskContext )
{
    ti_sysbios_family_arm_m3_TaskSupport_swap(oldtskContext, newtskContext);
}

/* checkStack__E */
xdc_Bool ti_sysbios_knl_Task_SupportProxy_checkStack__E( xdc_Char* stack, xdc_SizeT size )
{
    return ti_sysbios_family_arm_m3_TaskSupport_checkStack(stack, size);
}

/* stackUsed__E */
xdc_SizeT ti_sysbios_knl_Task_SupportProxy_stackUsed__E( xdc_Char* stack, xdc_SizeT size )
{
    return ti_sysbios_family_arm_m3_TaskSupport_stackUsed(stack, size);
}

/* getStackAlignment__E */
xdc_UInt ti_sysbios_knl_Task_SupportProxy_getStackAlignment__E( void )
{
    return ti_sysbios_family_arm_m3_TaskSupport_getStackAlignment();
}

/* getDefaultStackSize__E */
xdc_SizeT ti_sysbios_knl_Task_SupportProxy_getDefaultStackSize__E( void )
{
    return ti_sysbios_family_arm_m3_TaskSupport_getDefaultStackSize();
}


/*
 * ======== ti.sysbios.timers.dmtimer.Timer_TimerSupportProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.ducati.TimerSupport */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_timers_dmtimer_Timer_TimerSupportProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDone__S();
}

/* enable__E */
xdc_Void ti_sysbios_timers_dmtimer_Timer_TimerSupportProxy_enable__E( xdc_UInt timerId, xdc_runtime_Error_Block* eb )
{
    ti_sysbios_family_arm_ducati_TimerSupport_enable(timerId, eb);
}


/*
 * ======== xdc.runtime.Main_Module_GateProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.gates.GateHwi */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Main_Module_GateProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_gates_GateHwi_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr xdc_runtime_Main_Module_GateProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const xdc_runtime_Main_Module_GateProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_gates_GateHwi_Object__create__S(oa, osz, aa, (ti_sysbios_gates_GateHwi___ParamsPtr)pa, sizeof(xdc_runtime_IGateProvider_Params), eb);
}

/* Object__delete__S */
void xdc_runtime_Main_Module_GateProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_gates_GateHwi_Object__delete__S(instp);
}

/* Params__init__S */
void xdc_runtime_Main_Module_GateProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_gates_GateHwi_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* xdc_runtime_Main_Module_GateProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_gates_GateHwi_Handle__label__S(obj, lab);
}

/* query__E */
xdc_Bool xdc_runtime_Main_Module_GateProxy_query__E( xdc_Int qual )
{
    return ti_sysbios_gates_GateHwi_query(qual);
}

/* enter__E */
xdc_IArg xdc_runtime_Main_Module_GateProxy_enter__E( xdc_runtime_Main_Module_GateProxy_Handle __inst )
{
    return ti_sysbios_gates_GateHwi_enter((ti_sysbios_gates_GateHwi_Handle)__inst);
}

/* leave__E */
xdc_Void xdc_runtime_Main_Module_GateProxy_leave__E( xdc_runtime_Main_Module_GateProxy_Handle __inst, xdc_IArg key )
{
    ti_sysbios_gates_GateHwi_leave((ti_sysbios_gates_GateHwi_Handle)__inst, key);
}


/*
 * ======== xdc.runtime.Memory_HeapProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.heaps.HeapMem */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Memory_HeapProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_heaps_HeapMem_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr xdc_runtime_Memory_HeapProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const xdc_runtime_Memory_HeapProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_heaps_HeapMem_Object__create__S(oa, osz, aa, (ti_sysbios_heaps_HeapMem___ParamsPtr)pa, sizeof(xdc_runtime_IHeap_Params), eb);
}

/* Object__delete__S */
void xdc_runtime_Memory_HeapProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_heaps_HeapMem_Object__delete__S(instp);
}

/* Params__init__S */
void xdc_runtime_Memory_HeapProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_heaps_HeapMem_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* xdc_runtime_Memory_HeapProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_heaps_HeapMem_Handle__label__S(obj, lab);
}

/* alloc__E */
xdc_Ptr xdc_runtime_Memory_HeapProxy_alloc__E( xdc_runtime_Memory_HeapProxy_Handle __inst, xdc_SizeT size, xdc_SizeT align, xdc_runtime_Error_Block* eb )
{
    return xdc_runtime_IHeap_alloc((xdc_runtime_IHeap_Handle)__inst, size, align, eb);
}

/* free__E */
xdc_Void xdc_runtime_Memory_HeapProxy_free__E( xdc_runtime_Memory_HeapProxy_Handle __inst, xdc_Ptr block, xdc_SizeT size )
{
    xdc_runtime_IHeap_free((xdc_runtime_IHeap_Handle)__inst, block, size);
}

/* isBlocking__E */
xdc_Bool xdc_runtime_Memory_HeapProxy_isBlocking__E( xdc_runtime_Memory_HeapProxy_Handle __inst )
{
    return xdc_runtime_IHeap_isBlocking((xdc_runtime_IHeap_Handle)__inst);
}

/* getStats__E */
xdc_Void xdc_runtime_Memory_HeapProxy_getStats__E( xdc_runtime_Memory_HeapProxy_Handle __inst, xdc_runtime_Memory_Stats* stats )
{
    xdc_runtime_IHeap_getStats((xdc_runtime_IHeap_Handle)__inst, stats);
}


/*
 * ======== xdc.runtime.System_Module_GateProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.gates.GateHwi */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_System_Module_GateProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_gates_GateHwi_Module__startupDone__S();
}

/* Object__create__S */
xdc_Ptr xdc_runtime_System_Module_GateProxy_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const xdc_runtime_System_Module_GateProxy___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    return ti_sysbios_gates_GateHwi_Object__create__S(oa, osz, aa, (ti_sysbios_gates_GateHwi___ParamsPtr)pa, sizeof(xdc_runtime_IGateProvider_Params), eb);
}

/* Object__delete__S */
void xdc_runtime_System_Module_GateProxy_Object__delete__S( Ptr instp ) 
{
    ti_sysbios_gates_GateHwi_Object__delete__S(instp);
}

/* Params__init__S */
void xdc_runtime_System_Module_GateProxy_Params__init__S( xdc_Ptr dst, const xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz )
{
    ti_sysbios_gates_GateHwi_Params__init__S(dst, src, psz, isz);
}

/* Handle__label__S */
xdc_runtime_Types_Label* xdc_runtime_System_Module_GateProxy_Handle__label__S( Ptr obj, xdc_runtime_Types_Label* lab )
{
    return ti_sysbios_gates_GateHwi_Handle__label__S(obj, lab);
}

/* query__E */
xdc_Bool xdc_runtime_System_Module_GateProxy_query__E( xdc_Int qual )
{
    return ti_sysbios_gates_GateHwi_query(qual);
}

/* enter__E */
xdc_IArg xdc_runtime_System_Module_GateProxy_enter__E( xdc_runtime_System_Module_GateProxy_Handle __inst )
{
    return ti_sysbios_gates_GateHwi_enter((ti_sysbios_gates_GateHwi_Handle)__inst);
}

/* leave__E */
xdc_Void xdc_runtime_System_Module_GateProxy_leave__E( xdc_runtime_System_Module_GateProxy_Handle __inst, xdc_IArg key )
{
    ti_sysbios_gates_GateHwi_leave((ti_sysbios_gates_GateHwi_Handle)__inst, key);
}


/*
 * ======== xdc.runtime.System_SupportProxy PROXY BODY ========
 */

/* DELEGATES TO xdc.runtime.SysStd */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_System_SupportProxy_Module__startupDone__S( void ) 
{
    return xdc_runtime_SysStd_Module__startupDone__S();
}

/* abort__E */
xdc_Void xdc_runtime_System_SupportProxy_abort__E( xdc_String str )
{
    xdc_runtime_SysStd_abort(str);
}

/* exit__E */
xdc_Void xdc_runtime_System_SupportProxy_exit__E( xdc_Int stat )
{
    xdc_runtime_SysStd_exit(stat);
}

/* flush__E */
xdc_Void xdc_runtime_System_SupportProxy_flush__E( void )
{
    xdc_runtime_SysStd_flush();
}

/* putch__E */
xdc_Void xdc_runtime_System_SupportProxy_putch__E( xdc_Char ch )
{
    xdc_runtime_SysStd_putch(ch);
}

/* ready__E */
xdc_Bool xdc_runtime_System_SupportProxy_ready__E( void )
{
    return xdc_runtime_SysStd_ready();
}


/*
 * ======== xdc.runtime.Timestamp_SupportProxy PROXY BODY ========
 */

/* DELEGATES TO ti.sysbios.family.arm.ducati.TimestampProvider */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Timestamp_SupportProxy_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDone__S();
}

/* get32__E */
xdc_Bits32 xdc_runtime_Timestamp_SupportProxy_get32__E( void )
{
    return ti_sysbios_family_arm_ducati_TimestampProvider_get32();
}

/* get64__E */
xdc_Void xdc_runtime_Timestamp_SupportProxy_get64__E( xdc_runtime_Types_Timestamp64* result )
{
    ti_sysbios_family_arm_ducati_TimestampProvider_get64(result);
}

/* getFreq__E */
xdc_Void xdc_runtime_Timestamp_SupportProxy_getFreq__E( xdc_runtime_Types_FreqHz* freq )
{
    ti_sysbios_family_arm_ducati_TimestampProvider_getFreq(freq);
}


/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_family_arm_ducati_GateDualCore_Object2__ s0; char c; } ti_sysbios_family_arm_ducati_GateDualCore___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C = {
    (Ptr)&ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_family_arm_ducati_GateDualCore___S1) - sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Object2__), /* objSize */
    (Ptr)&ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_family_arm_ducati_GateDualCore_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.family.arm.ducati.Timer OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_family_arm_ducati_Timer_Object2__ s0; char c; } ti_sysbios_family_arm_ducati_Timer___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_ducati_Timer_Object__DESC__C = {
    (Ptr)0, /* fxnTab */
    &ti_sysbios_family_arm_ducati_Timer_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_family_arm_ducati_Timer___S1) - sizeof(ti_sysbios_family_arm_ducati_Timer_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_family_arm_ducati_Timer_Object2__), /* objSize */
    (Ptr)&ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_family_arm_ducati_Timer_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.family.arm.m3.Hwi OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_family_arm_m3_Hwi_Object2__ s0; char c; } ti_sysbios_family_arm_m3_Hwi___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_family_arm_m3_Hwi_Object__DESC__C = {
    (Ptr)0, /* fxnTab */
    &ti_sysbios_family_arm_m3_Hwi_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_family_arm_m3_Hwi___S1) - sizeof(ti_sysbios_family_arm_m3_Hwi_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_family_arm_m3_Hwi_Object2__), /* objSize */
    (Ptr)&ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_family_arm_m3_Hwi_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.gates.GateHwi OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_gates_GateHwi_Object2__ s0; char c; } ti_sysbios_gates_GateHwi___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_gates_GateHwi_Object__DESC__C = {
    (Ptr)&ti_sysbios_gates_GateHwi_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_gates_GateHwi_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_gates_GateHwi___S1) - sizeof(ti_sysbios_gates_GateHwi_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_gates_GateHwi_Object2__), /* objSize */
    (Ptr)&ti_sysbios_gates_GateHwi_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_gates_GateHwi_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.gates.GateMutex OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_gates_GateMutex_Object2__ s0; char c; } ti_sysbios_gates_GateMutex___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_gates_GateMutex_Object__DESC__C = {
    (Ptr)&ti_sysbios_gates_GateMutex_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_gates_GateMutex_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_gates_GateMutex___S1) - sizeof(ti_sysbios_gates_GateMutex_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_gates_GateMutex_Object2__), /* objSize */
    (Ptr)&ti_sysbios_gates_GateMutex_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_gates_GateMutex_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.hal.Hwi OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_hal_Hwi_Object2__ s0; char c; } ti_sysbios_hal_Hwi___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_hal_Hwi_Object__DESC__C = {
    (Ptr)0, /* fxnTab */
    &ti_sysbios_hal_Hwi_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_hal_Hwi___S1) - sizeof(ti_sysbios_hal_Hwi_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_hal_Hwi_Object2__), /* objSize */
    (Ptr)&ti_sysbios_hal_Hwi_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_hal_Hwi_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.hal.Timer OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_hal_Timer_Object2__ s0; char c; } ti_sysbios_hal_Timer___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_hal_Timer_Object__DESC__C = {
    (Ptr)0, /* fxnTab */
    &ti_sysbios_hal_Timer_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_hal_Timer___S1) - sizeof(ti_sysbios_hal_Timer_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_hal_Timer_Object2__), /* objSize */
    (Ptr)&ti_sysbios_hal_Timer_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_hal_Timer_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.heaps.HeapBuf OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_heaps_HeapBuf_Object2__ s0; char c; } ti_sysbios_heaps_HeapBuf___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_heaps_HeapBuf_Object__DESC__C = {
    (Ptr)&ti_sysbios_heaps_HeapBuf_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_heaps_HeapBuf_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_heaps_HeapBuf___S1) - sizeof(ti_sysbios_heaps_HeapBuf_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_heaps_HeapBuf_Object2__), /* objSize */
    (Ptr)&ti_sysbios_heaps_HeapBuf_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_heaps_HeapBuf_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.heaps.HeapMem OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_heaps_HeapMem_Object2__ s0; char c; } ti_sysbios_heaps_HeapMem___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_heaps_HeapMem_Object__DESC__C = {
    (Ptr)&ti_sysbios_heaps_HeapMem_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_heaps_HeapMem_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_heaps_HeapMem___S1) - sizeof(ti_sysbios_heaps_HeapMem_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_heaps_HeapMem_Object2__), /* objSize */
    (Ptr)&ti_sysbios_heaps_HeapMem_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_heaps_HeapMem_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.knl.Clock OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_knl_Clock_Object2__ s0; char c; } ti_sysbios_knl_Clock___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Clock_Object__DESC__C = {
    (Ptr)-1, /* fxnTab */
    &ti_sysbios_knl_Clock_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_knl_Clock___S1) - sizeof(ti_sysbios_knl_Clock_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_knl_Clock_Object2__), /* objSize */
    (Ptr)&ti_sysbios_knl_Clock_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_knl_Clock_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.knl.Queue OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_knl_Queue_Object2__ s0; char c; } ti_sysbios_knl_Queue___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Queue_Object__DESC__C = {
    (Ptr)-1, /* fxnTab */
    &ti_sysbios_knl_Queue_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_knl_Queue___S1) - sizeof(ti_sysbios_knl_Queue_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_knl_Queue_Object2__), /* objSize */
    (Ptr)&ti_sysbios_knl_Queue_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_knl_Queue_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.knl.Semaphore OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_knl_Semaphore_Object2__ s0; char c; } ti_sysbios_knl_Semaphore___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Semaphore_Object__DESC__C = {
    (Ptr)-1, /* fxnTab */
    &ti_sysbios_knl_Semaphore_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_knl_Semaphore___S1) - sizeof(ti_sysbios_knl_Semaphore_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_knl_Semaphore_Object2__), /* objSize */
    (Ptr)&ti_sysbios_knl_Semaphore_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_knl_Semaphore_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.knl.Swi OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_knl_Swi_Object2__ s0; char c; } ti_sysbios_knl_Swi___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Swi_Object__DESC__C = {
    (Ptr)-1, /* fxnTab */
    &ti_sysbios_knl_Swi_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_knl_Swi___S1) - sizeof(ti_sysbios_knl_Swi_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_knl_Swi_Object2__), /* objSize */
    (Ptr)&ti_sysbios_knl_Swi_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_knl_Swi_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.knl.Task OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_knl_Task_Object2__ s0; char c; } ti_sysbios_knl_Task___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_knl_Task_Object__DESC__C = {
    (Ptr)-1, /* fxnTab */
    &ti_sysbios_knl_Task_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_knl_Task___S1) - sizeof(ti_sysbios_knl_Task_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_knl_Task_Object2__), /* objSize */
    (Ptr)&ti_sysbios_knl_Task_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_knl_Task_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.syncs.SyncSem OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_syncs_SyncSem_Object2__ s0; char c; } ti_sysbios_syncs_SyncSem___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_syncs_SyncSem_Object__DESC__C = {
    (Ptr)&ti_sysbios_syncs_SyncSem_Module__FXNS__C, /* fxnTab */
    &ti_sysbios_syncs_SyncSem_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_syncs_SyncSem___S1) - sizeof(ti_sysbios_syncs_SyncSem_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_syncs_SyncSem_Object2__), /* objSize */
    (Ptr)&ti_sysbios_syncs_SyncSem_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_syncs_SyncSem_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.timers.dmtimer.Timer OBJECT DESCRIPTOR ========
 */

/* Object__DESC__C */
typedef struct { ti_sysbios_timers_dmtimer_Timer_Object2__ s0; char c; } ti_sysbios_timers_dmtimer_Timer___S1;
__FAR__ const xdc_runtime_Core_ObjDesc ti_sysbios_timers_dmtimer_Timer_Object__DESC__C = {
    (Ptr)0, /* fxnTab */
    &ti_sysbios_timers_dmtimer_Timer_Module__root__V.link, /* modLink */
    sizeof(ti_sysbios_timers_dmtimer_Timer___S1) - sizeof(ti_sysbios_timers_dmtimer_Timer_Object2__), /* objAlign */
    0, /* objHeap */
    0, /* objName */
    sizeof(ti_sysbios_timers_dmtimer_Timer_Object2__), /* objSize */
    (Ptr)&ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C, /* prmsInit */
    sizeof(ti_sysbios_timers_dmtimer_Timer_Params), /* prmsSize */
};



/*
 * ======== ti.sysbios.BIOS SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_BIOS_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.BIOS_RtsGateProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_BIOS_RtsGateProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_BIOS_RtsGateProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_BIOS_RtsGateProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_BIOS_RtsGateProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_BIOS_RtsGateProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_BIOS_RtsGateProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_BIOS_RtsGateProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_BIOS_RtsGateProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_BIOS_RtsGateProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_BIOS_RtsGateProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_BIOS_RtsGateProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_BIOS_RtsGateProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_BIOS_RtsGateProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_BIOS_RtsGateProxy_Module_GateProxy_query
xdc_Bool ti_sysbios_BIOS_RtsGateProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_BIOS_RtsGateProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_gates_GateMutex_Module__FXNS__C;
}



/*
 * ======== ti.sysbios.family.arm.ducati.CTM SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_CTM_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.family.arm.ducati.Core SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_Core_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_Core_Module__startupDone__F();
}



/*
 * ======== ti.sysbios.family.arm.ducati.GateDualCore SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_family_arm_ducati_GateDualCore_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_family_arm_ducati_GateDualCore_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_family_arm_ducati_GateDualCore_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_family_arm_ducati_GateDualCore_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_family_arm_ducati_GateDualCore_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_family_arm_ducati_GateDualCore_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32808;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_family_arm_ducati_GateDualCore_Object__*)oa) + i;
    }

    if (ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_family_arm_ducati_GateDualCore_Object__*)ti_sysbios_family_arm_ducati_GateDualCore_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_family_arm_ducati_GateDualCore_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_family_arm_ducati_GateDualCore_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_family_arm_ducati_GateDualCore___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_family_arm_ducati_GateDualCore_Params prms;
    ti_sysbios_family_arm_ducati_GateDualCore_Object* obj;
    int iStat;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_family_arm_ducati_GateDualCore_Instance_init__F(obj, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C, obj, NULL, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C, obj, NULL, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_family_arm_ducati_GateDualCore_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C, *((ti_sysbios_family_arm_ducati_GateDualCore_Object**)instp), NULL, 0, FALSE);
    *((ti_sysbios_family_arm_ducati_GateDualCore_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.family.arm.ducati.Timer SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_family_arm_ducati_Timer_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_family_arm_ducati_Timer_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_family_arm_ducati_Timer_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_family_arm_ducati_Timer_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_family_arm_ducati_Timer_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_family_arm_ducati_Timer_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_family_arm_ducati_Timer_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_family_arm_ducati_Timer_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_family_arm_ducati_Timer_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_family_arm_ducati_Timer_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_family_arm_ducati_Timer_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_family_arm_ducati_Timer_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_family_arm_ducati_Timer_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_Timer_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_Timer_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_family_arm_ducati_Timer_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32809;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_family_arm_ducati_Timer_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_family_arm_ducati_Timer_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_family_arm_ducati_Timer_Object__*)oa) + i;
    }

    if (ti_sysbios_family_arm_ducati_Timer_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_family_arm_ducati_Timer_Object__*)ti_sysbios_family_arm_ducati_Timer_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_family_arm_ducati_Timer_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_family_arm_ducati_Timer_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_family_arm_ducati_Timer_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_family_arm_ducati_Timer_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_family_arm_ducati_Timer_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_family_arm_ducati_Timer_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_family_arm_ducati_Timer___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_family_arm_ducati_Timer_Params prms;
    ti_sysbios_family_arm_ducati_Timer_Object* obj;
    int iStat;

    ti_sysbios_family_arm_ducati_Timer_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_family_arm_ducati_Timer_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_family_arm_ducati_Timer_Instance_init__F(obj, args->id, args->tickFxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_family_arm_ducati_Timer_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_family_arm_ducati_Timer_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_family_arm_ducati_Timer_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_family_arm_ducati_Timer_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_ducati_Timer_Object__DESC__C, *((ti_sysbios_family_arm_ducati_Timer_Object**)instp), (xdc_Fxn)ti_sysbios_family_arm_ducati_Timer_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_family_arm_ducati_Timer_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.family.arm.ducati.TimerSupport SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDone__F();
}



/*
 * ======== ti.sysbios.family.arm.ducati.TimestampProvider SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDone__F();
}



/*
 * ======== ti.sysbios.family.arm.ducati.Wugen SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_ducati_Wugen_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_ducati_Wugen_Module__startupDone__F();
}



/*
 * ======== ti.sysbios.family.arm.m3.Hwi SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_family_arm_m3_Hwi_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_family_arm_m3_Hwi_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_family_arm_m3_Hwi_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_family_arm_m3_Hwi_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_family_arm_m3_Hwi_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_family_arm_m3_Hwi_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_family_arm_m3_Hwi_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_family_arm_m3_Hwi_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_family_arm_m3_Hwi_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_family_arm_m3_Hwi_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_family_arm_m3_Hwi_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_family_arm_m3_Hwi_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_family_arm_m3_Hwi_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_m3_Hwi_Module__startupDone__S( void ) 
{
    return ti_sysbios_family_arm_m3_Hwi_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_family_arm_m3_Hwi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32802;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_family_arm_m3_Hwi_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_family_arm_m3_Hwi_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_family_arm_m3_Hwi_Object__*)oa) + i;
    }

    if (ti_sysbios_family_arm_m3_Hwi_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_family_arm_m3_Hwi_Object__*)ti_sysbios_family_arm_m3_Hwi_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_family_arm_m3_Hwi_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_family_arm_m3_Hwi_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_family_arm_m3_Hwi_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_family_arm_m3_Hwi_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_family_arm_m3_Hwi_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_family_arm_m3_Hwi_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_family_arm_m3_Hwi___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_family_arm_m3_Hwi_Params prms;
    ti_sysbios_family_arm_m3_Hwi_Object* obj;
    int iStat;

    ti_sysbios_family_arm_m3_Hwi_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_family_arm_m3_Hwi_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_family_arm_m3_Hwi_Instance_init__F(obj, args->intNum, args->hwiFxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_m3_Hwi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_family_arm_m3_Hwi_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_m3_Hwi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_family_arm_m3_Hwi_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_family_arm_m3_Hwi_Object__DESC__C, *((ti_sysbios_family_arm_m3_Hwi_Object**)instp), (xdc_Fxn)ti_sysbios_family_arm_m3_Hwi_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_family_arm_m3_Hwi_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.family.arm.m3.IntrinsicsSupport SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.family.arm.m3.TaskSupport SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_family_arm_m3_TaskSupport_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.gates.GateHwi SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_gates_GateHwi_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_gates_GateHwi_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_gates_GateHwi_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_gates_GateHwi_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_gates_GateHwi_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_gates_GateHwi_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_gates_GateHwi_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_gates_GateHwi_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_gates_GateHwi_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_gates_GateHwi_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_gates_GateHwi_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_gates_GateHwi_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_gates_GateHwi_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_gates_GateHwi_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_gates_GateHwi_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_gates_GateHwi_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_gates_GateHwi_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_gates_GateHwi_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_gates_GateHwi_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_gates_GateHwi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32820;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_gates_GateHwi_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_gates_GateHwi_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_gates_GateHwi_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_gates_GateHwi_Object__*)oa) + i;
    }

    if (ti_sysbios_gates_GateHwi_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_gates_GateHwi_Object__*)ti_sysbios_gates_GateHwi_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_gates_GateHwi_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_gates_GateHwi_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_gates_GateHwi_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_gates_GateHwi_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_gates_GateHwi_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_gates_GateHwi_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_gates_GateHwi___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_gates_GateHwi_Params prms;
    ti_sysbios_gates_GateHwi_Object* obj;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_gates_GateHwi_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_gates_GateHwi_Instance_init__F(obj, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_gates_GateHwi_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_gates_GateHwi_Object__DESC__C, obj, NULL, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_gates_GateHwi_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_gates_GateHwi_Object__DESC__C, *((ti_sysbios_gates_GateHwi_Object**)instp), NULL, -1, FALSE);
    *((ti_sysbios_gates_GateHwi_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.gates.GateMutex SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_gates_GateMutex_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_gates_GateMutex_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_gates_GateMutex_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_gates_GateMutex_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_gates_GateMutex_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_gates_GateMutex_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_gates_GateMutex_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_gates_GateMutex_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_gates_GateMutex_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_gates_GateMutex_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_gates_GateMutex_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_gates_GateMutex_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_gates_GateMutex_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_gates_GateMutex_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_gates_GateMutex_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_gates_GateMutex_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_gates_GateMutex_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_gates_GateMutex_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_gates_GateMutex_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_gates_GateMutex_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32821;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_gates_GateMutex_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_gates_GateMutex_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_gates_GateMutex_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_gates_GateMutex_Object__*)oa) + i;
    }

    if (ti_sysbios_gates_GateMutex_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_gates_GateMutex_Object__*)ti_sysbios_gates_GateMutex_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_gates_GateMutex_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_gates_GateMutex_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_gates_GateMutex_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_gates_GateMutex_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_gates_GateMutex_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_gates_GateMutex_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_gates_GateMutex___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_gates_GateMutex_Params prms;
    ti_sysbios_gates_GateMutex_Object* obj;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_gates_GateMutex_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_gates_GateMutex_Instance_init__F(obj, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_gates_GateMutex_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_gates_GateMutex_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_gates_GateMutex_Instance_finalize__F, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_gates_GateMutex_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_gates_GateMutex_Object__DESC__C, *((ti_sysbios_gates_GateMutex_Object**)instp), (xdc_Fxn)ti_sysbios_gates_GateMutex_Instance_finalize__F, -1, FALSE);
    *((ti_sysbios_gates_GateMutex_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.hal.Cache SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Cache_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.hal.Cache_CacheProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool ti_sysbios_hal_Cache_CacheProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_hal_Cache_CacheProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_hal_unicache_Cache_Module__FXNS__C;
}


/*
 * ======== ti.sysbios.hal.Hwi SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_hal_Hwi_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_hal_Hwi_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_hal_Hwi_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_hal_Hwi_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_hal_Hwi_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_hal_Hwi_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_hal_Hwi_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_hal_Hwi_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_hal_Hwi_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_hal_Hwi_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_hal_Hwi_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_hal_Hwi_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_hal_Hwi_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_hal_Hwi_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_hal_Hwi_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_hal_Hwi_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_hal_Hwi_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_hal_Hwi_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Hwi_Module__startupDone__S( void ) 
{
    return ti_sysbios_hal_Hwi_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_hal_Hwi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32788;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_hal_Hwi_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_hal_Hwi_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_hal_Hwi_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_hal_Hwi_Object__*)oa) + i;
    }

    if (ti_sysbios_hal_Hwi_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_hal_Hwi_Object__*)ti_sysbios_hal_Hwi_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_hal_Hwi_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_hal_Hwi_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_hal_Hwi_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_hal_Hwi_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_hal_Hwi_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_hal_Hwi_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_hal_Hwi___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_hal_Hwi_Params prms;
    ti_sysbios_hal_Hwi_Object* obj;
    int iStat;

    ti_sysbios_hal_Hwi_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_hal_Hwi_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_hal_Hwi_Instance_init__F(obj, args->intNum, args->hwiFxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Hwi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_hal_Hwi_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_hal_Hwi_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Hwi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_hal_Hwi_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_hal_Hwi_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Hwi_Object__DESC__C, *((ti_sysbios_hal_Hwi_Object**)instp), (xdc_Fxn)ti_sysbios_hal_Hwi_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_hal_Hwi_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.hal.Hwi_HwiProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_hal_Hwi_HwiProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_hal_Hwi_HwiProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_hal_Hwi_HwiProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_hal_Hwi_HwiProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_hal_Hwi_HwiProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_hal_Hwi_HwiProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_hal_Hwi_HwiProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_hal_Hwi_HwiProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_hal_Hwi_HwiProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_hal_Hwi_HwiProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_hal_Hwi_HwiProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_hal_Hwi_HwiProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_hal_Hwi_HwiProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_hal_Hwi_HwiProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_hal_Hwi_HwiProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_hal_Hwi_HwiProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_hal_Hwi_HwiProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_hal_Hwi_HwiProxy_Module_GateProxy_query
xdc_Bool ti_sysbios_hal_Hwi_HwiProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_hal_Hwi_HwiProxy_Proxy__delegate__S( void )
{
    return 0;
}



/*
 * ======== ti.sysbios.hal.Timer SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_hal_Timer_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_hal_Timer_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_hal_Timer_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_hal_Timer_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_hal_Timer_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_hal_Timer_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_hal_Timer_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_hal_Timer_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_hal_Timer_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_hal_Timer_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_hal_Timer_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_hal_Timer_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_hal_Timer_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_hal_Timer_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_hal_Timer_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_hal_Timer_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_hal_Timer_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_hal_Timer_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_Timer_Module__startupDone__S( void ) 
{
    return ti_sysbios_hal_Timer_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_hal_Timer_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32789;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_hal_Timer_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_hal_Timer_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_hal_Timer_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_hal_Timer_Object__*)oa) + i;
    }

    if (ti_sysbios_hal_Timer_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_hal_Timer_Object__*)ti_sysbios_hal_Timer_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_hal_Timer_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_hal_Timer_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_hal_Timer_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_hal_Timer_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_hal_Timer_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_hal_Timer_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_hal_Timer___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_hal_Timer_Params prms;
    ti_sysbios_hal_Timer_Object* obj;
    int iStat;

    ti_sysbios_hal_Timer_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_hal_Timer_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_hal_Timer_Instance_init__F(obj, args->id, args->tickFxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_hal_Timer_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_hal_Timer_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_hal_Timer_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_hal_Timer_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_hal_Timer_Object__DESC__C, *((ti_sysbios_hal_Timer_Object**)instp), (xdc_Fxn)ti_sysbios_hal_Timer_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_hal_Timer_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.hal.Timer_TimerProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_hal_Timer_TimerProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_hal_Timer_TimerProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_hal_Timer_TimerProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_hal_Timer_TimerProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_hal_Timer_TimerProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_hal_Timer_TimerProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_hal_Timer_TimerProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_hal_Timer_TimerProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_hal_Timer_TimerProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_hal_Timer_TimerProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_hal_Timer_TimerProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_hal_Timer_TimerProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_hal_Timer_TimerProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_hal_Timer_TimerProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_hal_Timer_TimerProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_hal_Timer_TimerProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_hal_Timer_TimerProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_hal_Timer_TimerProxy_Module_GateProxy_query
xdc_Bool ti_sysbios_hal_Timer_TimerProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_hal_Timer_TimerProxy_Proxy__delegate__S( void )
{
    return 0;
}



/*
 * ======== ti.sysbios.hal.ammu.AMMU SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_ammu_AMMU_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.hal.unicache.Cache SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_hal_unicache_Cache_Module__startupDone__S( void ) 
{
    return ti_sysbios_hal_unicache_Cache_Module__startupDone__F();
}



/*
 * ======== ti.sysbios.hal.unicache.Cache_Module_GateProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_hal_unicache_Cache_Module_GateProxy_Module_GateProxy_query
xdc_Bool ti_sysbios_hal_unicache_Cache_Module_GateProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_hal_unicache_Cache_Module_GateProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C;
}



/*
 * ======== ti.sysbios.heaps.HeapBuf SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_heaps_HeapBuf_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_heaps_HeapBuf_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_heaps_HeapBuf_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_heaps_HeapBuf_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_heaps_HeapBuf_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_heaps_HeapBuf_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_heaps_HeapBuf_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_heaps_HeapBuf_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_heaps_HeapBuf_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_heaps_HeapBuf_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_heaps_HeapBuf_Module__startupDone__S( void ) 
{
    return ti_sysbios_heaps_HeapBuf_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_heaps_HeapBuf_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32814;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_heaps_HeapBuf_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_heaps_HeapBuf_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_heaps_HeapBuf_Object__*)oa) + i;
    }

    if (ti_sysbios_heaps_HeapBuf_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_heaps_HeapBuf_Object__*)ti_sysbios_heaps_HeapBuf_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_heaps_HeapBuf_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_heaps_HeapBuf_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_heaps_HeapBuf_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_heaps_HeapBuf_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_heaps_HeapBuf___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_heaps_HeapBuf_Params prms;
    ti_sysbios_heaps_HeapBuf_Object* obj;
    int iStat;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_heaps_HeapBuf_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_heaps_HeapBuf_Instance_init__F(obj, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_heaps_HeapBuf_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_heaps_HeapBuf_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_heaps_HeapBuf_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_heaps_HeapBuf_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_heaps_HeapBuf_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_heaps_HeapBuf_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_heaps_HeapBuf_Object__DESC__C, *((ti_sysbios_heaps_HeapBuf_Object**)instp), (xdc_Fxn)ti_sysbios_heaps_HeapBuf_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_heaps_HeapBuf_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.heaps.HeapMem SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_heaps_HeapMem_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_heaps_HeapMem_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_heaps_HeapMem_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_heaps_HeapMem_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_heaps_HeapMem_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_heaps_HeapMem_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_heaps_HeapMem_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_heaps_HeapMem_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_heaps_HeapMem_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_heaps_HeapMem_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_heaps_HeapMem_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_heaps_HeapMem_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_heaps_HeapMem_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_heaps_HeapMem_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_heaps_HeapMem_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_heaps_HeapMem_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_heaps_HeapMem_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_heaps_HeapMem_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_heaps_HeapMem_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_heaps_HeapMem_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32815;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_heaps_HeapMem_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_heaps_HeapMem_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_heaps_HeapMem_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_heaps_HeapMem_Object__*)oa) + i;
    }

    if (ti_sysbios_heaps_HeapMem_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_heaps_HeapMem_Object__*)ti_sysbios_heaps_HeapMem_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_heaps_HeapMem_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_heaps_HeapMem_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_heaps_HeapMem_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_heaps_HeapMem_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_heaps_HeapMem_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_heaps_HeapMem_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_heaps_HeapMem___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_heaps_HeapMem_Params prms;
    ti_sysbios_heaps_HeapMem_Object* obj;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_heaps_HeapMem_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_heaps_HeapMem_Instance_init__F(obj, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_heaps_HeapMem_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_heaps_HeapMem_Object__DESC__C, obj, NULL, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_heaps_HeapMem_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_heaps_HeapMem_Object__DESC__C, *((ti_sysbios_heaps_HeapMem_Object**)instp), NULL, -1, FALSE);
    *((ti_sysbios_heaps_HeapMem_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.heaps.HeapMem_Module_GateProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_heaps_HeapMem_Module_GateProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_heaps_HeapMem_Module_GateProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_heaps_HeapMem_Module_GateProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_heaps_HeapMem_Module_GateProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_heaps_HeapMem_Module_GateProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_heaps_HeapMem_Module_GateProxy_Module_GateProxy_query
xdc_Bool ti_sysbios_heaps_HeapMem_Module_GateProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_heaps_HeapMem_Module_GateProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_gates_GateMutex_Module__FXNS__C;
}



/*
 * ======== ti.sysbios.knl.Clock SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_knl_Clock_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_knl_Clock_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_knl_Clock_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_knl_Clock_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_knl_Clock_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_knl_Clock_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_knl_Clock_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_knl_Clock_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_knl_Clock_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_knl_Clock_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_knl_Clock_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_knl_Clock_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_knl_Clock_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_knl_Clock_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_knl_Clock_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_knl_Clock_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_knl_Clock_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_knl_Clock_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Clock_Module__startupDone__S( void ) 
{
    return ti_sysbios_knl_Clock_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_knl_Clock_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32792;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_knl_Clock_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_knl_Clock_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_knl_Clock_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_knl_Clock_Object__*)oa) + i;
    }

    if (ti_sysbios_knl_Clock_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_knl_Clock_Object__*)ti_sysbios_knl_Clock_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_knl_Clock_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_knl_Clock_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_knl_Clock_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_knl_Clock_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_knl_Clock_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_knl_Clock_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_knl_Clock___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_knl_Clock_Params prms;
    ti_sysbios_knl_Clock_Object* obj;

    ti_sysbios_knl_Clock_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_knl_Clock_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_knl_Clock_Instance_init__F(obj, args->clockFxn, args->timeout, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_knl_Clock_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Clock_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Clock_Instance_finalize__F, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_knl_Clock_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Clock_Object__DESC__C, *((ti_sysbios_knl_Clock_Object**)instp), (xdc_Fxn)ti_sysbios_knl_Clock_Instance_finalize__F, -1, FALSE);
    *((ti_sysbios_knl_Clock_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.knl.Idle SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Idle_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.knl.Intrinsics SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Intrinsics_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== ti.sysbios.knl.Intrinsics_SupportProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool ti_sysbios_knl_Intrinsics_SupportProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_knl_Intrinsics_SupportProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__FXNS__C;
}


/*
 * ======== ti.sysbios.knl.Queue SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_knl_Queue_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_knl_Queue_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_knl_Queue_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_knl_Queue_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_knl_Queue_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_knl_Queue_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_knl_Queue_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_knl_Queue_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_knl_Queue_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_knl_Queue_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_knl_Queue_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_knl_Queue_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_knl_Queue_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_knl_Queue_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_knl_Queue_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_knl_Queue_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_knl_Queue_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_knl_Queue_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Queue_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_knl_Queue_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32795;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_knl_Queue_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_knl_Queue_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_knl_Queue_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_knl_Queue_Object__*)oa) + i;
    }

    if (ti_sysbios_knl_Queue_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_knl_Queue_Object__*)ti_sysbios_knl_Queue_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_knl_Queue_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_knl_Queue_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_knl_Queue_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_knl_Queue_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_knl_Queue_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_knl_Queue_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_knl_Queue___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_knl_Queue_Params prms;
    ti_sysbios_knl_Queue_Object* obj;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_knl_Queue_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_knl_Queue_Instance_init__F(obj, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_knl_Queue_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Queue_Object__DESC__C, obj, NULL, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_knl_Queue_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Queue_Object__DESC__C, *((ti_sysbios_knl_Queue_Object**)instp), NULL, -1, FALSE);
    *((ti_sysbios_knl_Queue_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.knl.Semaphore SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_knl_Semaphore_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_knl_Semaphore_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_knl_Semaphore_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_knl_Semaphore_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_knl_Semaphore_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_knl_Semaphore_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_knl_Semaphore_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_knl_Semaphore_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_knl_Semaphore_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_knl_Semaphore_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_knl_Semaphore_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_knl_Semaphore_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_knl_Semaphore_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_knl_Semaphore_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_knl_Semaphore_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_knl_Semaphore_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_knl_Semaphore_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_knl_Semaphore_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Semaphore_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_knl_Semaphore_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32796;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_knl_Semaphore_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_knl_Semaphore_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_knl_Semaphore_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_knl_Semaphore_Object__*)oa) + i;
    }

    if (ti_sysbios_knl_Semaphore_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_knl_Semaphore_Object__*)ti_sysbios_knl_Semaphore_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_knl_Semaphore_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_knl_Semaphore_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_knl_Semaphore_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_knl_Semaphore_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_knl_Semaphore_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_knl_Semaphore_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_knl_Semaphore___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_knl_Semaphore_Params prms;
    ti_sysbios_knl_Semaphore_Object* obj;

    ti_sysbios_knl_Semaphore_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_knl_Semaphore_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    ti_sysbios_knl_Semaphore_Instance_init__F(obj, args->count, &prms);
    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_knl_Semaphore_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Semaphore_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Semaphore_Instance_finalize__F, -1, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_knl_Semaphore_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Semaphore_Object__DESC__C, *((ti_sysbios_knl_Semaphore_Object**)instp), (xdc_Fxn)ti_sysbios_knl_Semaphore_Instance_finalize__F, -1, FALSE);
    *((ti_sysbios_knl_Semaphore_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.knl.Swi SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_knl_Swi_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_knl_Swi_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_knl_Swi_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_knl_Swi_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_knl_Swi_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_knl_Swi_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_knl_Swi_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_knl_Swi_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_knl_Swi_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_knl_Swi_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_knl_Swi_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_knl_Swi_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_knl_Swi_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_knl_Swi_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_knl_Swi_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_knl_Swi_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_knl_Swi_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_knl_Swi_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Swi_Module__startupDone__S( void ) 
{
    return ti_sysbios_knl_Swi_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_knl_Swi_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32797;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_knl_Swi_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_knl_Swi_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_knl_Swi_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_knl_Swi_Object__*)oa) + i;
    }

    if (ti_sysbios_knl_Swi_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_knl_Swi_Object__*)ti_sysbios_knl_Swi_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_knl_Swi_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_knl_Swi_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_knl_Swi_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_knl_Swi_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_knl_Swi_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_knl_Swi_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_knl_Swi___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_knl_Swi_Params prms;
    ti_sysbios_knl_Swi_Object* obj;
    int iStat;

    ti_sysbios_knl_Swi_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_knl_Swi_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_knl_Swi_Instance_init__F(obj, args->fxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Swi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Swi_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_knl_Swi_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Swi_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Swi_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_knl_Swi_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Swi_Object__DESC__C, *((ti_sysbios_knl_Swi_Object**)instp), (xdc_Fxn)ti_sysbios_knl_Swi_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_knl_Swi_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.knl.Task SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_knl_Task_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_knl_Task_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_knl_Task_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_knl_Task_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_knl_Task_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_knl_Task_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_knl_Task_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_knl_Task_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_knl_Task_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_knl_Task_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_knl_Task_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_knl_Task_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_knl_Task_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_knl_Task_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_knl_Task_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_knl_Task_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_knl_Task_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_knl_Task_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_knl_Task_Module__startupDone__S( void ) 
{
    return ti_sysbios_knl_Task_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_knl_Task_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32798;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_knl_Task_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_knl_Task_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_knl_Task_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_knl_Task_Object__*)oa) + i;
    }

    if (ti_sysbios_knl_Task_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_knl_Task_Object__*)ti_sysbios_knl_Task_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_knl_Task_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_knl_Task_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_knl_Task_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_knl_Task_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_knl_Task_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_knl_Task_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_knl_Task___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_knl_Task_Params prms;
    ti_sysbios_knl_Task_Object* obj;
    int iStat;

    ti_sysbios_knl_Task_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_knl_Task_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_knl_Task_Instance_init__F(obj, args->fxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Task_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Task_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_knl_Task_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Task_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_knl_Task_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_knl_Task_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_knl_Task_Object__DESC__C, *((ti_sysbios_knl_Task_Object**)instp), (xdc_Fxn)ti_sysbios_knl_Task_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_knl_Task_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.knl.Task_SupportProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool ti_sysbios_knl_Task_SupportProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_knl_Task_SupportProxy_Proxy__delegate__S( void )
{
    return 0;
}


/*
 * ======== ti.sysbios.syncs.SyncSem SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_syncs_SyncSem_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_syncs_SyncSem_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_syncs_SyncSem_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_syncs_SyncSem_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_syncs_SyncSem_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_syncs_SyncSem_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_syncs_SyncSem_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_syncs_SyncSem_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_syncs_SyncSem_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_syncs_SyncSem_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_syncs_SyncSem_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_syncs_SyncSem_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_syncs_SyncSem_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_syncs_SyncSem_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_syncs_SyncSem_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_syncs_SyncSem_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_syncs_SyncSem_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_syncs_SyncSem_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_syncs_SyncSem_Module__startupDone__S( void ) 
{
    return 1;
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_syncs_SyncSem_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32825;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_syncs_SyncSem_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_syncs_SyncSem_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_syncs_SyncSem_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_syncs_SyncSem_Object__*)oa) + i;
    }

    if (ti_sysbios_syncs_SyncSem_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_syncs_SyncSem_Object__*)ti_sysbios_syncs_SyncSem_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_syncs_SyncSem_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_syncs_SyncSem_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_syncs_SyncSem_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_syncs_SyncSem_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_syncs_SyncSem_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_syncs_SyncSem_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_syncs_SyncSem___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_syncs_SyncSem_Params prms;
    ti_sysbios_syncs_SyncSem_Object* obj;
    int iStat;

    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_syncs_SyncSem_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_syncs_SyncSem_Instance_init__F(obj, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_syncs_SyncSem_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_syncs_SyncSem_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_syncs_SyncSem_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_syncs_SyncSem_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_syncs_SyncSem_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_syncs_SyncSem_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_syncs_SyncSem_Object__DESC__C, *((ti_sysbios_syncs_SyncSem_Object**)instp), (xdc_Fxn)ti_sysbios_syncs_SyncSem_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_syncs_SyncSem_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.timers.dmtimer.Timer SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_sysbios_timers_dmtimer_Timer_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_sysbios_timers_dmtimer_Timer_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_sysbios_timers_dmtimer_Timer_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_sysbios_timers_dmtimer_Timer_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_sysbios_timers_dmtimer_Timer_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_sysbios_timers_dmtimer_Timer_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_sysbios_timers_dmtimer_Timer_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_sysbios_timers_dmtimer_Timer_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_sysbios_timers_dmtimer_Timer_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_sysbios_timers_dmtimer_Timer_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_sysbios_timers_dmtimer_Timer_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_sysbios_timers_dmtimer_Timer_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_sysbios_timers_dmtimer_Timer_Module_GateProxy_query

/* Module__startupDone__S */
xdc_Bool ti_sysbios_timers_dmtimer_Timer_Module__startupDone__S( void ) 
{
    return ti_sysbios_timers_dmtimer_Timer_Module__startupDone__F();
}

/* Handle__label__S */
xdc_runtime_Types_Label* ti_sysbios_timers_dmtimer_Timer_Handle__label__S( xdc_Ptr obj, xdc_runtime_Types_Label* lab ) 
{
    lab->handle = obj;
    lab->modId = 32818;
    xdc_runtime_Core_assignLabel(lab, 0, 0);

    return lab;
}

/* Params__init__S */
xdc_Void ti_sysbios_timers_dmtimer_Timer_Params__init__S( xdc_Ptr prms, xdc_Ptr src, xdc_SizeT psz, xdc_SizeT isz ) 
{
    xdc_runtime_Core_assignParams__I(prms, (xdc_Ptr)(src ? src : &ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C), psz, isz);
}

/* Object__get__S */
xdc_Ptr ti_sysbios_timers_dmtimer_Timer_Object__get__S( xdc_Ptr oa, xdc_Int i ) 
{
    if (oa) {
        return ((ti_sysbios_timers_dmtimer_Timer_Object__*)oa) + i;
    }

    if (ti_sysbios_timers_dmtimer_Timer_Object__count__C == 0) {
        return NULL;
    }

    return ((ti_sysbios_timers_dmtimer_Timer_Object__*)ti_sysbios_timers_dmtimer_Timer_Object__table__C) + i;
}

/* Object__first__S */
xdc_Ptr ti_sysbios_timers_dmtimer_Timer_Object__first__S( void ) 
{
    xdc_runtime_Types_InstHdr *iHdr = (xdc_runtime_Types_InstHdr *)ti_sysbios_timers_dmtimer_Timer_Module__root__V.link.next;

    if (iHdr != (xdc_runtime_Types_InstHdr *)&ti_sysbios_timers_dmtimer_Timer_Module__root__V.link) {
        return iHdr + 1;
    }
    else {
        return NULL;
    }
}

/* Object__next__S */
xdc_Ptr ti_sysbios_timers_dmtimer_Timer_Object__next__S( xdc_Ptr obj ) 
{
    xdc_runtime_Types_InstHdr *iHdr = ((xdc_runtime_Types_InstHdr *)obj) - 1;

    if (iHdr->link.next != (xdc_runtime_Types_Link *)&ti_sysbios_timers_dmtimer_Timer_Module__root__V.link) {
        return (xdc_runtime_Types_InstHdr *)(iHdr->link.next) + 1;
    }
    else {
        return NULL;
    }
}

/* Object__create__S */
xdc_Ptr ti_sysbios_timers_dmtimer_Timer_Object__create__S (
    xdc_Ptr oa,
    xdc_SizeT osz,
    const xdc_Ptr aa,
    const ti_sysbios_timers_dmtimer_Timer___ParamsPtr pa,
    xdc_SizeT psz,
    xdc_runtime_Error_Block* eb )
{
    ti_sysbios_timers_dmtimer_Timer_Params prms;
    ti_sysbios_timers_dmtimer_Timer_Object* obj;
    int iStat;

    ti_sysbios_timers_dmtimer_Timer_Args__create* args = aa;
    /* common instance initialization */
    obj = xdc_runtime_Core_createObject__I(&ti_sysbios_timers_dmtimer_Timer_Object__DESC__C, oa, osz, &prms, (xdc_Ptr)pa, psz, eb);
    if (obj == NULL) {
        return NULL;
    }

    /* module-specific initialization */
    iStat = ti_sysbios_timers_dmtimer_Timer_Instance_init__F(obj, args->id, args->tickFxn, &prms, eb);
    if (xdc_runtime_Error_check(eb)) {
        xdc_runtime_Core_deleteObject__I(&ti_sysbios_timers_dmtimer_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_timers_dmtimer_Timer_Instance_finalize__F, iStat, (xdc_Bool)(oa != NULL));
        return NULL;
    }

    return obj;
}

/* Object__destruct__S */
xdc_Void ti_sysbios_timers_dmtimer_Timer_Object__destruct__S( xdc_Ptr obj ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_timers_dmtimer_Timer_Object__DESC__C, obj, (xdc_Fxn)ti_sysbios_timers_dmtimer_Timer_Instance_finalize__F, 0, TRUE);
}

/* Object__delete__S */
xdc_Void ti_sysbios_timers_dmtimer_Timer_Object__delete__S( xdc_Ptr instp ) 
{
    xdc_runtime_Core_deleteObject__I(&ti_sysbios_timers_dmtimer_Timer_Object__DESC__C, *((ti_sysbios_timers_dmtimer_Timer_Object**)instp), (xdc_Fxn)ti_sysbios_timers_dmtimer_Timer_Instance_finalize__F, 0, FALSE);
    *((ti_sysbios_timers_dmtimer_Timer_Handle*)instp) = NULL;
}


/*
 * ======== ti.sysbios.timers.dmtimer.Timer_TimerSupportProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool ti_sysbios_timers_dmtimer_Timer_TimerSupportProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr ti_sysbios_timers_dmtimer_Timer_TimerSupportProxy_Proxy__delegate__S( void )
{
    return 0;
}


/*
 * ======== ti.sysbios.utils.Load SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool ti_sysbios_utils_Load_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Assert SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Assert_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Core SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Core_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Defaults SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Defaults_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Diags SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Diags_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Error SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Error_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Gate SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Gate_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Log SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Log_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Main SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Main_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Main_Module_GateProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID xdc_runtime_Main_Module_GateProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL xdc_runtime_Main_Module_GateProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB xdc_runtime_Main_Module_GateProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK xdc_runtime_Main_Module_GateProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF xdc_runtime_Main_Module_GateProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ xdc_runtime_Main_Module_GateProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 xdc_runtime_Main_Module_GateProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 xdc_runtime_Main_Module_GateProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 xdc_runtime_Main_Module_GateProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 xdc_runtime_Main_Module_GateProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 xdc_runtime_Main_Module_GateProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ xdc_runtime_Main_Module_GateProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS xdc_runtime_Main_Module_GateProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create xdc_runtime_Main_Module_GateProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete xdc_runtime_Main_Module_GateProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter xdc_runtime_Main_Module_GateProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave xdc_runtime_Main_Module_GateProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query xdc_runtime_Main_Module_GateProxy_Module_GateProxy_query
xdc_Bool xdc_runtime_Main_Module_GateProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr xdc_runtime_Main_Module_GateProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_gates_GateHwi_Module__FXNS__C;
}



/*
 * ======== xdc.runtime.Memory SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Memory_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Memory_HeapProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID xdc_runtime_Memory_HeapProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL xdc_runtime_Memory_HeapProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB xdc_runtime_Memory_HeapProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK xdc_runtime_Memory_HeapProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF xdc_runtime_Memory_HeapProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ xdc_runtime_Memory_HeapProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 xdc_runtime_Memory_HeapProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 xdc_runtime_Memory_HeapProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 xdc_runtime_Memory_HeapProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 xdc_runtime_Memory_HeapProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 xdc_runtime_Memory_HeapProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ xdc_runtime_Memory_HeapProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS xdc_runtime_Memory_HeapProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create xdc_runtime_Memory_HeapProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete xdc_runtime_Memory_HeapProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter xdc_runtime_Memory_HeapProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave xdc_runtime_Memory_HeapProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query xdc_runtime_Memory_HeapProxy_Module_GateProxy_query
xdc_Bool xdc_runtime_Memory_HeapProxy_Proxy__abstract__S( void )
{
    return 1;
}
xdc_Ptr xdc_runtime_Memory_HeapProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_heaps_HeapMem_Module__FXNS__C;
}



/*
 * ======== xdc.runtime.Registry SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Registry_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Startup SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Startup_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.SysStd SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_SysStd_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.System SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_System_Module__startupDone__S( void ) 
{
    return xdc_runtime_System_Module__startupDone__F();
}



/*
 * ======== xdc.runtime.System_Module_GateProxy SYSTEM FUNCTIONS ========
 */

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID xdc_runtime_System_Module_GateProxy_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL xdc_runtime_System_Module_GateProxy_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB xdc_runtime_System_Module_GateProxy_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK xdc_runtime_System_Module_GateProxy_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF xdc_runtime_System_Module_GateProxy_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ xdc_runtime_System_Module_GateProxy_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 xdc_runtime_System_Module_GateProxy_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 xdc_runtime_System_Module_GateProxy_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 xdc_runtime_System_Module_GateProxy_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 xdc_runtime_System_Module_GateProxy_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 xdc_runtime_System_Module_GateProxy_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ xdc_runtime_System_Module_GateProxy_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS xdc_runtime_System_Module_GateProxy_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create xdc_runtime_System_Module_GateProxy_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete xdc_runtime_System_Module_GateProxy_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter xdc_runtime_System_Module_GateProxy_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave xdc_runtime_System_Module_GateProxy_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query xdc_runtime_System_Module_GateProxy_Module_GateProxy_query
xdc_Bool xdc_runtime_System_Module_GateProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr xdc_runtime_System_Module_GateProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&ti_sysbios_gates_GateHwi_Module__FXNS__C;
}



/*
 * ======== xdc.runtime.System_SupportProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool xdc_runtime_System_SupportProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr xdc_runtime_System_SupportProxy_Proxy__delegate__S( void )
{
    return (xdc_Ptr)&xdc_runtime_SysStd_Module__FXNS__C;
}


/*
 * ======== xdc.runtime.Text SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Text_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Timestamp SYSTEM FUNCTIONS ========
 */

/* Module__startupDone__S */
xdc_Bool xdc_runtime_Timestamp_Module__startupDone__S( void ) 
{
    return 1;
}



/*
 * ======== xdc.runtime.Timestamp_SupportProxy SYSTEM FUNCTIONS ========
 */

xdc_Bool xdc_runtime_Timestamp_SupportProxy_Proxy__abstract__S( void )
{
    return 0;
}
xdc_Ptr xdc_runtime_Timestamp_SupportProxy_Proxy__delegate__S( void )
{
    return 0;
}


/*
 * ======== INITIALIZATION ENTRY POINT ========
 */

extern int __xdc__init(void);
#ifdef __GNUC__
#if __GNUC__ >= 4
    __attribute__ ((externally_visible))
#endif
#endif
__FAR__ int (* volatile __xdc__init__addr)(void) = &__xdc__init;


/*
 * ======== PROGRAM GLOBALS ========
 */


/*
 * ======== CONSTANTS ========
 */

#pragma DATA_SECTION(xdc_runtime_IModule_Interface__BASE__C, ".const:xdc_runtime_IModule_Interface__BASE__C");
#pragma DATA_SECTION(xdc_runtime_IGateProvider_Interface__BASE__C, ".const:xdc_runtime_IGateProvider_Interface__BASE__C");
#pragma DATA_SECTION(ti_sysbios_interfaces_IIntrinsicsSupport_Interface__BASE__C, ".const:ti_sysbios_interfaces_IIntrinsicsSupport_Interface__BASE__C");
#pragma DATA_SECTION(ti_sysbios_interfaces_ICache_Interface__BASE__C, ".const:ti_sysbios_interfaces_ICache_Interface__BASE__C");
#pragma DATA_SECTION(xdc_runtime_IHeap_Interface__BASE__C, ".const:xdc_runtime_IHeap_Interface__BASE__C");
#pragma DATA_SECTION(xdc_runtime_knl_ISync_Interface__BASE__C, ".const:xdc_runtime_knl_ISync_Interface__BASE__C");
#pragma DATA_SECTION(xdc_runtime_ISystemSupport_Interface__BASE__C, ".const:xdc_runtime_ISystemSupport_Interface__BASE__C");
#pragma DATA_SECTION(xdc_runtime_ITimestampClient_Interface__BASE__C, ".const:xdc_runtime_ITimestampClient_Interface__BASE__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__FXNS__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__FXNS__C, ".const:ti_sysbios_gates_GateHwi_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__FXNS__C, ".const:ti_sysbios_gates_GateMutex_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__FXNS__C, ".const:ti_sysbios_hal_unicache_Cache_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__FXNS__C, ".const:ti_sysbios_heaps_HeapBuf_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__FXNS__C, ".const:ti_sysbios_heaps_HeapMem_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__FXNS__C, ".const:ti_sysbios_syncs_SyncSem_Module__FXNS__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__FXNS__C, ".const:xdc_runtime_SysStd_Module__FXNS__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__FXNS__C, ".const:xdc_runtime_Timestamp_Module__FXNS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__PARAMS__C, ".const:ti_sysbios_gates_GateHwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__PARAMS__C, ".const:ti_sysbios_gates_GateMutex_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__PARAMS__C, ".const:ti_sysbios_hal_Hwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__PARAMS__C, ".const:ti_sysbios_hal_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__PARAMS__C, ".const:ti_sysbios_heaps_HeapBuf_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__PARAMS__C, ".const:ti_sysbios_heaps_HeapMem_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__PARAMS__C, ".const:ti_sysbios_knl_Clock_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__PARAMS__C, ".const:ti_sysbios_knl_Queue_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__PARAMS__C, ".const:ti_sysbios_knl_Semaphore_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__PARAMS__C, ".const:ti_sysbios_knl_Swi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__PARAMS__C, ".const:ti_sysbios_knl_Task_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__PARAMS__C, ".const:ti_sysbios_syncs_SyncSem_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Instance_State_sem__O, ".const:ti_sysbios_gates_GateMutex_Instance_State_sem__O");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Instance_State_freeList__O, ".const:ti_sysbios_heaps_HeapBuf_Instance_State_freeList__O");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module_State_clockQ__O, ".const:ti_sysbios_knl_Clock_Module_State_clockQ__O");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Instance_State_pendQ__O, ".const:ti_sysbios_knl_Semaphore_Instance_State_pendQ__O");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module_State_inactiveQ__O, ".const:ti_sysbios_knl_Task_Module_State_inactiveQ__O");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module_State_terminatedQ__O, ".const:ti_sysbios_knl_Task_Module_State_terminatedQ__O");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module_State_taskList__O, ".const:ti_sysbios_utils_Load_Module_State_taskList__O");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module_State_gate__O, ".const:ti_sysbios_utils_Load_Module_State_gate__O");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__diagsEnabled__C, ".const:ti_sysbios_BIOS_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__diagsIncluded__C, ".const:ti_sysbios_BIOS_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__diagsMask__C, ".const:ti_sysbios_BIOS_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__gateObj__C, ".const:ti_sysbios_BIOS_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__gatePrms__C, ".const:ti_sysbios_BIOS_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__id__C, ".const:ti_sysbios_BIOS_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerDefined__C, ".const:ti_sysbios_BIOS_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerObj__C, ".const:ti_sysbios_BIOS_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerFxn0__C, ".const:ti_sysbios_BIOS_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerFxn1__C, ".const:ti_sysbios_BIOS_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerFxn2__C, ".const:ti_sysbios_BIOS_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerFxn4__C, ".const:ti_sysbios_BIOS_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__loggerFxn8__C, ".const:ti_sysbios_BIOS_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Module__startupDoneFxn__C, ".const:ti_sysbios_BIOS_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Object__count__C, ".const:ti_sysbios_BIOS_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Object__heap__C, ".const:ti_sysbios_BIOS_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Object__sizeof__C, ".const:ti_sysbios_BIOS_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_Object__table__C, ".const:ti_sysbios_BIOS_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_smpEnabled__C, ".const:ti_sysbios_BIOS_smpEnabled__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_cpuFreq__C, ".const:ti_sysbios_BIOS_cpuFreq__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_taskEnabled__C, ".const:ti_sysbios_BIOS_taskEnabled__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_swiEnabled__C, ".const:ti_sysbios_BIOS_swiEnabled__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_clockEnabled__C, ".const:ti_sysbios_BIOS_clockEnabled__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_heapSize__C, ".const:ti_sysbios_BIOS_heapSize__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_heapSection__C, ".const:ti_sysbios_BIOS_heapSection__C");
#pragma DATA_SECTION(ti_sysbios_BIOS_installedErrorHook__C, ".const:ti_sysbios_BIOS_installedErrorHook__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__id__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_CTM_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Object__count__C, ".const:ti_sysbios_family_arm_ducati_CTM_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_CTM_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_CTM_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_CTM_Object__table__C, ".const:ti_sysbios_family_arm_ducati_CTM_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__id__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_Core_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Object__count__C, ".const:ti_sysbios_family_arm_ducati_Core_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_Core_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_Core_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_Object__table__C, ".const:ti_sysbios_family_arm_ducati_Core_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_id__C, ".const:ti_sysbios_family_arm_ducati_Core_id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Core_E_mismatchedIds__C, ".const:ti_sysbios_family_arm_ducati_Core_E_mismatchedIds__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_Object__table__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_A_nestedEnter__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_E_invalidIndex__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_E_gateInUse__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_gateArrayAddress__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_numGates__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_numGates__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_initGates__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_initGates__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_GateDualCore_enableStats__C, ".const:ti_sysbios_family_arm_ducati_GateDualCore_enableStats__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__DESC__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__id__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_Timer_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__count__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_Object__table__C, ".const:ti_sysbios_family_arm_ducati_Timer_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_E_invalidTimer__C, ".const:ti_sysbios_family_arm_ducati_Timer_E_invalidTimer__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_E_notAvailable__C, ".const:ti_sysbios_family_arm_ducati_Timer_E_notAvailable__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_E_cannotSupport__C, ".const:ti_sysbios_family_arm_ducati_Timer_E_cannotSupport__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_anyMask__C, ".const:ti_sysbios_family_arm_ducati_Timer_anyMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Timer_startupNeeded__C, ".const:ti_sysbios_family_arm_ducati_Timer_startupNeeded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__id__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Object__count__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimerSupport_Object__table__C, ".const:ti_sysbios_family_arm_ducati_TimerSupport_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__id__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Object__count__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_Object__table__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_TimestampProvider_inpsel__C, ".const:ti_sysbios_family_arm_ducati_TimestampProvider_inpsel__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__diagsMask__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__gateObj__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__gatePrms__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__id__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerObj__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Object__count__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Object__heap__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Object__sizeof__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_ducati_Wugen_Object__table__C, ".const:ti_sysbios_family_arm_ducati_Wugen_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__DESC__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_hooks__A, ".const:ti_sysbios_family_arm_m3_Hwi_hooks__A");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__diagsMask__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__gateObj__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__gatePrms__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__id__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerObj__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_m3_Hwi_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__count__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__heap__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__sizeof__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_Object__table__C, ".const:ti_sysbios_family_arm_m3_Hwi_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport__C, ".const:ti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__C, ".const:ti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__C, ".const:ti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport__C, ".const:ti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_NUM_INTERRUPTS__C, ".const:ti_sysbios_family_arm_m3_Hwi_NUM_INTERRUPTS__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_NUM_PRIORITIES__C, ".const:ti_sysbios_family_arm_m3_Hwi_NUM_PRIORITIES__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_LM_begin__C, ".const:ti_sysbios_family_arm_m3_Hwi_LM_begin__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_LD_end__C, ".const:ti_sysbios_family_arm_m3_Hwi_LD_end__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_A_unsupportedMaskingOption__C, ".const:ti_sysbios_family_arm_m3_Hwi_A_unsupportedMaskingOption__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_alreadyDefined__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_alreadyDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_exception__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_exception__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_noIsr__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_noIsr__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_NMI__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_NMI__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_hardFault__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_hardFault__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_memFault__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_memFault__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_busFault__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_busFault__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_usageFault__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_usageFault__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_svCall__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_svCall__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_debugMon__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_debugMon__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_E_reserved__C, ".const:ti_sysbios_family_arm_m3_Hwi_E_reserved__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_excHookFunc__C, ".const:ti_sysbios_family_arm_m3_Hwi_excHookFunc__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_disablePriority__C, ".const:ti_sysbios_family_arm_m3_Hwi_disablePriority__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_priGroup__C, ".const:ti_sysbios_family_arm_m3_Hwi_priGroup__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_swiDisable__C, ".const:ti_sysbios_family_arm_m3_Hwi_swiDisable__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_swiRestoreHwi__C, ".const:ti_sysbios_family_arm_m3_Hwi_swiRestoreHwi__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_taskDisable__C, ".const:ti_sysbios_family_arm_m3_Hwi_taskDisable__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_taskRestoreHwi__C, ".const:ti_sysbios_family_arm_m3_Hwi_taskRestoreHwi__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_ccr__C, ".const:ti_sysbios_family_arm_m3_Hwi_ccr__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_hooks__C, ".const:ti_sysbios_family_arm_m3_Hwi_hooks__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_Hwi_excHandlerFunc__C, ".const:ti_sysbios_family_arm_m3_Hwi_excHandlerFunc__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsMask__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gateObj__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gatePrms__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__id__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerObj__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__count__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__heap__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__sizeof__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__table__C, ".const:ti_sysbios_family_arm_m3_IntrinsicsSupport_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__id__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Module__startupDoneFxn__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Object__count__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_Object__table__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_defaultStackSize__C");
#pragma DATA_SECTION(ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C, ".const:ti_sysbios_family_arm_m3_TaskSupport_stackAlignment__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__DESC__C, ".const:ti_sysbios_gates_GateHwi_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__PARAMS__C, ".const:ti_sysbios_gates_GateHwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__diagsEnabled__C, ".const:ti_sysbios_gates_GateHwi_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__diagsIncluded__C, ".const:ti_sysbios_gates_GateHwi_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__diagsMask__C, ".const:ti_sysbios_gates_GateHwi_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__gateObj__C, ".const:ti_sysbios_gates_GateHwi_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__gatePrms__C, ".const:ti_sysbios_gates_GateHwi_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__id__C, ".const:ti_sysbios_gates_GateHwi_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerDefined__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerObj__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerFxn0__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerFxn1__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerFxn2__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerFxn4__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__loggerFxn8__C, ".const:ti_sysbios_gates_GateHwi_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Module__startupDoneFxn__C, ".const:ti_sysbios_gates_GateHwi_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__count__C, ".const:ti_sysbios_gates_GateHwi_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__heap__C, ".const:ti_sysbios_gates_GateHwi_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__sizeof__C, ".const:ti_sysbios_gates_GateHwi_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateHwi_Object__table__C, ".const:ti_sysbios_gates_GateHwi_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__DESC__C, ".const:ti_sysbios_gates_GateMutex_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__PARAMS__C, ".const:ti_sysbios_gates_GateMutex_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__diagsEnabled__C, ".const:ti_sysbios_gates_GateMutex_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__diagsIncluded__C, ".const:ti_sysbios_gates_GateMutex_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__diagsMask__C, ".const:ti_sysbios_gates_GateMutex_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__gateObj__C, ".const:ti_sysbios_gates_GateMutex_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__gatePrms__C, ".const:ti_sysbios_gates_GateMutex_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__id__C, ".const:ti_sysbios_gates_GateMutex_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerDefined__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerObj__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerFxn0__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerFxn1__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerFxn2__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerFxn4__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__loggerFxn8__C, ".const:ti_sysbios_gates_GateMutex_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Module__startupDoneFxn__C, ".const:ti_sysbios_gates_GateMutex_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__count__C, ".const:ti_sysbios_gates_GateMutex_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__heap__C, ".const:ti_sysbios_gates_GateMutex_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__sizeof__C, ".const:ti_sysbios_gates_GateMutex_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_Object__table__C, ".const:ti_sysbios_gates_GateMutex_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_gates_GateMutex_A_badContext__C, ".const:ti_sysbios_gates_GateMutex_A_badContext__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__diagsEnabled__C, ".const:ti_sysbios_hal_Cache_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__diagsIncluded__C, ".const:ti_sysbios_hal_Cache_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__diagsMask__C, ".const:ti_sysbios_hal_Cache_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__gateObj__C, ".const:ti_sysbios_hal_Cache_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__gatePrms__C, ".const:ti_sysbios_hal_Cache_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__id__C, ".const:ti_sysbios_hal_Cache_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerDefined__C, ".const:ti_sysbios_hal_Cache_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerObj__C, ".const:ti_sysbios_hal_Cache_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerFxn0__C, ".const:ti_sysbios_hal_Cache_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerFxn1__C, ".const:ti_sysbios_hal_Cache_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerFxn2__C, ".const:ti_sysbios_hal_Cache_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerFxn4__C, ".const:ti_sysbios_hal_Cache_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__loggerFxn8__C, ".const:ti_sysbios_hal_Cache_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Module__startupDoneFxn__C, ".const:ti_sysbios_hal_Cache_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Object__count__C, ".const:ti_sysbios_hal_Cache_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Object__heap__C, ".const:ti_sysbios_hal_Cache_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Object__sizeof__C, ".const:ti_sysbios_hal_Cache_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_hal_Cache_Object__table__C, ".const:ti_sysbios_hal_Cache_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__DESC__C, ".const:ti_sysbios_hal_Hwi_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__PARAMS__C, ".const:ti_sysbios_hal_Hwi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__diagsEnabled__C, ".const:ti_sysbios_hal_Hwi_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__diagsIncluded__C, ".const:ti_sysbios_hal_Hwi_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__diagsMask__C, ".const:ti_sysbios_hal_Hwi_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__gateObj__C, ".const:ti_sysbios_hal_Hwi_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__gatePrms__C, ".const:ti_sysbios_hal_Hwi_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__id__C, ".const:ti_sysbios_hal_Hwi_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerDefined__C, ".const:ti_sysbios_hal_Hwi_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerObj__C, ".const:ti_sysbios_hal_Hwi_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerFxn0__C, ".const:ti_sysbios_hal_Hwi_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerFxn1__C, ".const:ti_sysbios_hal_Hwi_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerFxn2__C, ".const:ti_sysbios_hal_Hwi_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerFxn4__C, ".const:ti_sysbios_hal_Hwi_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__loggerFxn8__C, ".const:ti_sysbios_hal_Hwi_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Module__startupDoneFxn__C, ".const:ti_sysbios_hal_Hwi_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__count__C, ".const:ti_sysbios_hal_Hwi_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__heap__C, ".const:ti_sysbios_hal_Hwi_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__sizeof__C, ".const:ti_sysbios_hal_Hwi_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_Object__table__C, ".const:ti_sysbios_hal_Hwi_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_dispatcherAutoNestingSupport__C, ".const:ti_sysbios_hal_Hwi_dispatcherAutoNestingSupport__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_dispatcherSwiSupport__C, ".const:ti_sysbios_hal_Hwi_dispatcherSwiSupport__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_dispatcherTaskSupport__C, ".const:ti_sysbios_hal_Hwi_dispatcherTaskSupport__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_dispatcherIrpTrackingSupport__C, ".const:ti_sysbios_hal_Hwi_dispatcherIrpTrackingSupport__C");
#pragma DATA_SECTION(ti_sysbios_hal_Hwi_E_stackOverflow__C, ".const:ti_sysbios_hal_Hwi_E_stackOverflow__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__DESC__C, ".const:ti_sysbios_hal_Timer_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__PARAMS__C, ".const:ti_sysbios_hal_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__diagsEnabled__C, ".const:ti_sysbios_hal_Timer_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__diagsIncluded__C, ".const:ti_sysbios_hal_Timer_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__diagsMask__C, ".const:ti_sysbios_hal_Timer_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__gateObj__C, ".const:ti_sysbios_hal_Timer_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__gatePrms__C, ".const:ti_sysbios_hal_Timer_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__id__C, ".const:ti_sysbios_hal_Timer_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerDefined__C, ".const:ti_sysbios_hal_Timer_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerObj__C, ".const:ti_sysbios_hal_Timer_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerFxn0__C, ".const:ti_sysbios_hal_Timer_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerFxn1__C, ".const:ti_sysbios_hal_Timer_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerFxn2__C, ".const:ti_sysbios_hal_Timer_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerFxn4__C, ".const:ti_sysbios_hal_Timer_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__loggerFxn8__C, ".const:ti_sysbios_hal_Timer_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Module__startupDoneFxn__C, ".const:ti_sysbios_hal_Timer_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__count__C, ".const:ti_sysbios_hal_Timer_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__heap__C, ".const:ti_sysbios_hal_Timer_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__sizeof__C, ".const:ti_sysbios_hal_Timer_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_hal_Timer_Object__table__C, ".const:ti_sysbios_hal_Timer_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__diagsEnabled__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__diagsIncluded__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__diagsMask__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__gateObj__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__gatePrms__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__id__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerDefined__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerObj__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerFxn0__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerFxn1__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerFxn2__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerFxn4__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__loggerFxn8__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Module__startupDoneFxn__C, ".const:ti_sysbios_hal_ammu_AMMU_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Object__count__C, ".const:ti_sysbios_hal_ammu_AMMU_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Object__heap__C, ".const:ti_sysbios_hal_ammu_AMMU_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Object__sizeof__C, ".const:ti_sysbios_hal_ammu_AMMU_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_Object__table__C, ".const:ti_sysbios_hal_ammu_AMMU_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_configureAmmu__C, ".const:ti_sysbios_hal_ammu_AMMU_configureAmmu__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_numLargePages__C, ".const:ti_sysbios_hal_ammu_AMMU_numLargePages__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_numMediumPages__C, ".const:ti_sysbios_hal_ammu_AMMU_numMediumPages__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_numSmallPages__C, ".const:ti_sysbios_hal_ammu_AMMU_numSmallPages__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_numLinePages__C, ".const:ti_sysbios_hal_ammu_AMMU_numLinePages__C");
#pragma DATA_SECTION(ti_sysbios_hal_ammu_AMMU_mmuInitConfig__C, ".const:ti_sysbios_hal_ammu_AMMU_mmuInitConfig__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__diagsEnabled__C, ".const:ti_sysbios_hal_unicache_Cache_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__diagsIncluded__C, ".const:ti_sysbios_hal_unicache_Cache_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__diagsMask__C, ".const:ti_sysbios_hal_unicache_Cache_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__gateObj__C, ".const:ti_sysbios_hal_unicache_Cache_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__gatePrms__C, ".const:ti_sysbios_hal_unicache_Cache_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__id__C, ".const:ti_sysbios_hal_unicache_Cache_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerDefined__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerObj__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerFxn0__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerFxn1__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerFxn2__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerFxn4__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__loggerFxn8__C, ".const:ti_sysbios_hal_unicache_Cache_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn__C, ".const:ti_sysbios_hal_unicache_Cache_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Object__count__C, ".const:ti_sysbios_hal_unicache_Cache_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Object__heap__C, ".const:ti_sysbios_hal_unicache_Cache_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Object__sizeof__C, ".const:ti_sysbios_hal_unicache_Cache_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_Object__table__C, ".const:ti_sysbios_hal_unicache_Cache_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_E_exception__C, ".const:ti_sysbios_hal_unicache_Cache_E_exception__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_configureCache__C, ".const:ti_sysbios_hal_unicache_Cache_configureCache__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_enableCache__C, ".const:ti_sysbios_hal_unicache_Cache_enableCache__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__C, ".const:ti_sysbios_hal_unicache_Cache_maxLineModeBufSize__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_l1ocpConfig__C, ".const:ti_sysbios_hal_unicache_Cache_l1ocpConfig__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_l1secConfig__C, ".const:ti_sysbios_hal_unicache_Cache_l1secConfig__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_l2ocpConfig__C, ".const:ti_sysbios_hal_unicache_Cache_l2ocpConfig__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_l2secConfig__C, ".const:ti_sysbios_hal_unicache_Cache_l2secConfig__C");
#pragma DATA_SECTION(ti_sysbios_hal_unicache_Cache_l2CacheSupported__C, ".const:ti_sysbios_hal_unicache_Cache_l2CacheSupported__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__DESC__C, ".const:ti_sysbios_heaps_HeapBuf_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__PARAMS__C, ".const:ti_sysbios_heaps_HeapBuf_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C, ".const:ti_sysbios_heaps_HeapBuf_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C, ".const:ti_sysbios_heaps_HeapBuf_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__diagsMask__C, ".const:ti_sysbios_heaps_HeapBuf_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__gateObj__C, ".const:ti_sysbios_heaps_HeapBuf_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__gatePrms__C, ".const:ti_sysbios_heaps_HeapBuf_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__id__C, ".const:ti_sysbios_heaps_HeapBuf_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerObj__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C, ".const:ti_sysbios_heaps_HeapBuf_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Module__startupDoneFxn__C, ".const:ti_sysbios_heaps_HeapBuf_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__count__C, ".const:ti_sysbios_heaps_HeapBuf_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__heap__C, ".const:ti_sysbios_heaps_HeapBuf_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__sizeof__C, ".const:ti_sysbios_heaps_HeapBuf_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_Object__table__C, ".const:ti_sysbios_heaps_HeapBuf_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_nullBuf__C, ".const:ti_sysbios_heaps_HeapBuf_A_nullBuf__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_bufAlign__C, ".const:ti_sysbios_heaps_HeapBuf_A_bufAlign__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_invalidAlign__C, ".const:ti_sysbios_heaps_HeapBuf_A_invalidAlign__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_zeroBlockSize__C, ".const:ti_sysbios_heaps_HeapBuf_A_zeroBlockSize__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C, ".const:ti_sysbios_heaps_HeapBuf_A_zeroBlocks__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C, ".const:ti_sysbios_heaps_HeapBuf_A_zeroBufSize__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C, ".const:ti_sysbios_heaps_HeapBuf_A_invalidBufSize__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C, ".const:ti_sysbios_heaps_HeapBuf_A_noBlocksToFree__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_A_invalidFree__C, ".const:ti_sysbios_heaps_HeapBuf_A_invalidFree__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_E_size__C, ".const:ti_sysbios_heaps_HeapBuf_E_size__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C, ".const:ti_sysbios_heaps_HeapBuf_trackMaxAllocs__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C, ".const:ti_sysbios_heaps_HeapBuf_numConstructedHeaps__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__DESC__C, ".const:ti_sysbios_heaps_HeapMem_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__PARAMS__C, ".const:ti_sysbios_heaps_HeapMem_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__diagsEnabled__C, ".const:ti_sysbios_heaps_HeapMem_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__diagsIncluded__C, ".const:ti_sysbios_heaps_HeapMem_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__diagsMask__C, ".const:ti_sysbios_heaps_HeapMem_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__gateObj__C, ".const:ti_sysbios_heaps_HeapMem_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__gatePrms__C, ".const:ti_sysbios_heaps_HeapMem_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__id__C, ".const:ti_sysbios_heaps_HeapMem_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerDefined__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerObj__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerFxn0__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerFxn1__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerFxn2__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerFxn4__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__loggerFxn8__C, ".const:ti_sysbios_heaps_HeapMem_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Module__startupDoneFxn__C, ".const:ti_sysbios_heaps_HeapMem_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__count__C, ".const:ti_sysbios_heaps_HeapMem_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__heap__C, ".const:ti_sysbios_heaps_HeapMem_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__sizeof__C, ".const:ti_sysbios_heaps_HeapMem_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_Object__table__C, ".const:ti_sysbios_heaps_HeapMem_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_A_zeroBlock__C, ".const:ti_sysbios_heaps_HeapMem_A_zeroBlock__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_A_heapSize__C, ".const:ti_sysbios_heaps_HeapMem_A_heapSize__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_A_align__C, ".const:ti_sysbios_heaps_HeapMem_A_align__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_E_memory__C, ".const:ti_sysbios_heaps_HeapMem_E_memory__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_A_invalidFree__C, ".const:ti_sysbios_heaps_HeapMem_A_invalidFree__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_reqAlign__C, ".const:ti_sysbios_heaps_HeapMem_reqAlign__C");
#pragma DATA_SECTION(ti_sysbios_heaps_HeapMem_reqAlignMask__C, ".const:ti_sysbios_heaps_HeapMem_reqAlignMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__DESC__C, ".const:ti_sysbios_knl_Clock_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__PARAMS__C, ".const:ti_sysbios_knl_Clock_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Clock_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Clock_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__diagsMask__C, ".const:ti_sysbios_knl_Clock_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__gateObj__C, ".const:ti_sysbios_knl_Clock_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__gatePrms__C, ".const:ti_sysbios_knl_Clock_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__id__C, ".const:ti_sysbios_knl_Clock_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerDefined__C, ".const:ti_sysbios_knl_Clock_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerObj__C, ".const:ti_sysbios_knl_Clock_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Clock_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Clock_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Clock_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Clock_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Clock_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Clock_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__count__C, ".const:ti_sysbios_knl_Clock_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__heap__C, ".const:ti_sysbios_knl_Clock_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__sizeof__C, ".const:ti_sysbios_knl_Clock_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_Object__table__C, ".const:ti_sysbios_knl_Clock_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_LW_delayed__C, ".const:ti_sysbios_knl_Clock_LW_delayed__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_LM_tick__C, ".const:ti_sysbios_knl_Clock_LM_tick__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_LM_begin__C, ".const:ti_sysbios_knl_Clock_LM_begin__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_A_clockDisabled__C, ".const:ti_sysbios_knl_Clock_A_clockDisabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_A_badThreadType__C, ".const:ti_sysbios_knl_Clock_A_badThreadType__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_tickSource__C, ".const:ti_sysbios_knl_Clock_tickSource__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_tickMode__C, ".const:ti_sysbios_knl_Clock_tickMode__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_timerId__C, ".const:ti_sysbios_knl_Clock_timerId__C");
#pragma DATA_SECTION(ti_sysbios_knl_Clock_tickPeriod__C, ".const:ti_sysbios_knl_Clock_tickPeriod__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_funcList__A, ".const:ti_sysbios_knl_Idle_funcList__A");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Idle_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Idle_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__diagsMask__C, ".const:ti_sysbios_knl_Idle_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__gateObj__C, ".const:ti_sysbios_knl_Idle_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__gatePrms__C, ".const:ti_sysbios_knl_Idle_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__id__C, ".const:ti_sysbios_knl_Idle_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerDefined__C, ".const:ti_sysbios_knl_Idle_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerObj__C, ".const:ti_sysbios_knl_Idle_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Idle_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Idle_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Idle_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Idle_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Idle_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Idle_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Object__count__C, ".const:ti_sysbios_knl_Idle_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Object__heap__C, ".const:ti_sysbios_knl_Idle_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Object__sizeof__C, ".const:ti_sysbios_knl_Idle_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_Object__table__C, ".const:ti_sysbios_knl_Idle_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Idle_funcList__C, ".const:ti_sysbios_knl_Idle_funcList__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Intrinsics_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Intrinsics_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__diagsMask__C, ".const:ti_sysbios_knl_Intrinsics_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__gateObj__C, ".const:ti_sysbios_knl_Intrinsics_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__gatePrms__C, ".const:ti_sysbios_knl_Intrinsics_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__id__C, ".const:ti_sysbios_knl_Intrinsics_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerDefined__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerObj__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Intrinsics_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Intrinsics_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Object__count__C, ".const:ti_sysbios_knl_Intrinsics_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Object__heap__C, ".const:ti_sysbios_knl_Intrinsics_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Object__sizeof__C, ".const:ti_sysbios_knl_Intrinsics_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Intrinsics_Object__table__C, ".const:ti_sysbios_knl_Intrinsics_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__DESC__C, ".const:ti_sysbios_knl_Queue_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__PARAMS__C, ".const:ti_sysbios_knl_Queue_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Queue_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Queue_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__diagsMask__C, ".const:ti_sysbios_knl_Queue_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__gateObj__C, ".const:ti_sysbios_knl_Queue_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__gatePrms__C, ".const:ti_sysbios_knl_Queue_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__id__C, ".const:ti_sysbios_knl_Queue_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerDefined__C, ".const:ti_sysbios_knl_Queue_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerObj__C, ".const:ti_sysbios_knl_Queue_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Queue_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Queue_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Queue_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Queue_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Queue_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Queue_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__count__C, ".const:ti_sysbios_knl_Queue_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__heap__C, ".const:ti_sysbios_knl_Queue_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__sizeof__C, ".const:ti_sysbios_knl_Queue_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Queue_Object__table__C, ".const:ti_sysbios_knl_Queue_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__DESC__C, ".const:ti_sysbios_knl_Semaphore_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__PARAMS__C, ".const:ti_sysbios_knl_Semaphore_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Semaphore_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Semaphore_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__diagsMask__C, ".const:ti_sysbios_knl_Semaphore_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__gateObj__C, ".const:ti_sysbios_knl_Semaphore_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__gatePrms__C, ".const:ti_sysbios_knl_Semaphore_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__id__C, ".const:ti_sysbios_knl_Semaphore_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerDefined__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerObj__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Semaphore_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Semaphore_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__count__C, ".const:ti_sysbios_knl_Semaphore_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__heap__C, ".const:ti_sysbios_knl_Semaphore_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__sizeof__C, ".const:ti_sysbios_knl_Semaphore_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_Object__table__C, ".const:ti_sysbios_knl_Semaphore_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_LM_post__C, ".const:ti_sysbios_knl_Semaphore_LM_post__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_LM_pend__C, ".const:ti_sysbios_knl_Semaphore_LM_pend__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_A_noEvents__C, ".const:ti_sysbios_knl_Semaphore_A_noEvents__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_A_invTimeout__C, ".const:ti_sysbios_knl_Semaphore_A_invTimeout__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_A_badContext__C, ".const:ti_sysbios_knl_Semaphore_A_badContext__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_supportsEvents__C, ".const:ti_sysbios_knl_Semaphore_supportsEvents__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_eventPost__C, ".const:ti_sysbios_knl_Semaphore_eventPost__C");
#pragma DATA_SECTION(ti_sysbios_knl_Semaphore_eventSync__C, ".const:ti_sysbios_knl_Semaphore_eventSync__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__DESC__C, ".const:ti_sysbios_knl_Swi_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__PARAMS__C, ".const:ti_sysbios_knl_Swi_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_hooks__A, ".const:ti_sysbios_knl_Swi_hooks__A");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Swi_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Swi_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__diagsMask__C, ".const:ti_sysbios_knl_Swi_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__gateObj__C, ".const:ti_sysbios_knl_Swi_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__gatePrms__C, ".const:ti_sysbios_knl_Swi_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__id__C, ".const:ti_sysbios_knl_Swi_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerDefined__C, ".const:ti_sysbios_knl_Swi_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerObj__C, ".const:ti_sysbios_knl_Swi_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Swi_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Swi_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Swi_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Swi_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Swi_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Swi_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__count__C, ".const:ti_sysbios_knl_Swi_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__heap__C, ".const:ti_sysbios_knl_Swi_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__sizeof__C, ".const:ti_sysbios_knl_Swi_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_Object__table__C, ".const:ti_sysbios_knl_Swi_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_LM_begin__C, ".const:ti_sysbios_knl_Swi_LM_begin__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_LD_end__C, ".const:ti_sysbios_knl_Swi_LD_end__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_LM_post__C, ".const:ti_sysbios_knl_Swi_LM_post__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_A_swiDisabled__C, ".const:ti_sysbios_knl_Swi_A_swiDisabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_A_badPriority__C, ".const:ti_sysbios_knl_Swi_A_badPriority__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_numPriorities__C, ".const:ti_sysbios_knl_Swi_numPriorities__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_hooks__C, ".const:ti_sysbios_knl_Swi_hooks__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_taskDisable__C, ".const:ti_sysbios_knl_Swi_taskDisable__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_taskRestore__C, ".const:ti_sysbios_knl_Swi_taskRestore__C");
#pragma DATA_SECTION(ti_sysbios_knl_Swi_numConstructedSwis__C, ".const:ti_sysbios_knl_Swi_numConstructedSwis__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__DESC__C, ".const:ti_sysbios_knl_Task_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__PARAMS__C, ".const:ti_sysbios_knl_Task_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_hooks__A, ".const:ti_sysbios_knl_Task_hooks__A");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__diagsEnabled__C, ".const:ti_sysbios_knl_Task_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__diagsIncluded__C, ".const:ti_sysbios_knl_Task_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__diagsMask__C, ".const:ti_sysbios_knl_Task_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__gateObj__C, ".const:ti_sysbios_knl_Task_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__gatePrms__C, ".const:ti_sysbios_knl_Task_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__id__C, ".const:ti_sysbios_knl_Task_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerDefined__C, ".const:ti_sysbios_knl_Task_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerObj__C, ".const:ti_sysbios_knl_Task_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerFxn0__C, ".const:ti_sysbios_knl_Task_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerFxn1__C, ".const:ti_sysbios_knl_Task_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerFxn2__C, ".const:ti_sysbios_knl_Task_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerFxn4__C, ".const:ti_sysbios_knl_Task_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__loggerFxn8__C, ".const:ti_sysbios_knl_Task_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Module__startupDoneFxn__C, ".const:ti_sysbios_knl_Task_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__count__C, ".const:ti_sysbios_knl_Task_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__heap__C, ".const:ti_sysbios_knl_Task_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__sizeof__C, ".const:ti_sysbios_knl_Task_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_Object__table__C, ".const:ti_sysbios_knl_Task_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LM_switch__C, ".const:ti_sysbios_knl_Task_LM_switch__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LM_sleep__C, ".const:ti_sysbios_knl_Task_LM_sleep__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LD_ready__C, ".const:ti_sysbios_knl_Task_LD_ready__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LD_block__C, ".const:ti_sysbios_knl_Task_LD_block__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LM_yield__C, ".const:ti_sysbios_knl_Task_LM_yield__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LM_setPri__C, ".const:ti_sysbios_knl_Task_LM_setPri__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_LD_exit__C, ".const:ti_sysbios_knl_Task_LD_exit__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_E_stackOverflow__C, ".const:ti_sysbios_knl_Task_E_stackOverflow__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_E_spOutOfBounds__C, ".const:ti_sysbios_knl_Task_E_spOutOfBounds__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_badThreadType__C, ".const:ti_sysbios_knl_Task_A_badThreadType__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_badTaskState__C, ".const:ti_sysbios_knl_Task_A_badTaskState__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_noPendElem__C, ".const:ti_sysbios_knl_Task_A_noPendElem__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_taskDisabled__C, ".const:ti_sysbios_knl_Task_A_taskDisabled__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_badPriority__C, ".const:ti_sysbios_knl_Task_A_badPriority__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_A_badTimeout__C, ".const:ti_sysbios_knl_Task_A_badTimeout__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_numPriorities__C, ".const:ti_sysbios_knl_Task_numPriorities__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_defaultStackSize__C, ".const:ti_sysbios_knl_Task_defaultStackSize__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_defaultStackHeap__C, ".const:ti_sysbios_knl_Task_defaultStackHeap__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_allBlockedFunc__C, ".const:ti_sysbios_knl_Task_allBlockedFunc__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_initStackFlag__C, ".const:ti_sysbios_knl_Task_initStackFlag__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_deleteTerminatedTasks__C, ".const:ti_sysbios_knl_Task_deleteTerminatedTasks__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_hooks__C, ".const:ti_sysbios_knl_Task_hooks__C");
#pragma DATA_SECTION(ti_sysbios_knl_Task_numConstructedTasks__C, ".const:ti_sysbios_knl_Task_numConstructedTasks__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__DESC__C, ".const:ti_sysbios_syncs_SyncSem_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__PARAMS__C, ".const:ti_sysbios_syncs_SyncSem_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__diagsEnabled__C, ".const:ti_sysbios_syncs_SyncSem_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__diagsIncluded__C, ".const:ti_sysbios_syncs_SyncSem_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__diagsMask__C, ".const:ti_sysbios_syncs_SyncSem_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__gateObj__C, ".const:ti_sysbios_syncs_SyncSem_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__gatePrms__C, ".const:ti_sysbios_syncs_SyncSem_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__id__C, ".const:ti_sysbios_syncs_SyncSem_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerDefined__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerObj__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerFxn0__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerFxn1__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerFxn2__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerFxn4__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__loggerFxn8__C, ".const:ti_sysbios_syncs_SyncSem_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Module__startupDoneFxn__C, ".const:ti_sysbios_syncs_SyncSem_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__count__C, ".const:ti_sysbios_syncs_SyncSem_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__heap__C, ".const:ti_sysbios_syncs_SyncSem_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__sizeof__C, ".const:ti_sysbios_syncs_SyncSem_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_syncs_SyncSem_Object__table__C, ".const:ti_sysbios_syncs_SyncSem_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__DESC__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__DESC__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__PARAMS__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__diagsEnabled__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__diagsIncluded__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__diagsMask__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__gateObj__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__gatePrms__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__id__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerDefined__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerObj__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Module__startupDoneFxn__C, ".const:ti_sysbios_timers_dmtimer_Timer_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__count__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__heap__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__sizeof__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_Object__table__C, ".const:ti_sysbios_timers_dmtimer_Timer_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_A_notAvailable__C, ".const:ti_sysbios_timers_dmtimer_Timer_A_notAvailable__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_E_invalidTimer__C, ".const:ti_sysbios_timers_dmtimer_Timer_E_invalidTimer__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_E_notAvailable__C, ".const:ti_sysbios_timers_dmtimer_Timer_E_notAvailable__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_E_cannotSupport__C, ".const:ti_sysbios_timers_dmtimer_Timer_E_cannotSupport__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_E_freqMismatch__C, ".const:ti_sysbios_timers_dmtimer_Timer_E_freqMismatch__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_anyMask__C, ".const:ti_sysbios_timers_dmtimer_Timer_anyMask__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_checkFrequency__C, ".const:ti_sysbios_timers_dmtimer_Timer_checkFrequency__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_startupNeeded__C, ".const:ti_sysbios_timers_dmtimer_Timer_startupNeeded__C");
#pragma DATA_SECTION(ti_sysbios_timers_dmtimer_Timer_numTimerDevices__C, ".const:ti_sysbios_timers_dmtimer_Timer_numTimerDevices__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__diagsEnabled__C, ".const:ti_sysbios_utils_Load_Module__diagsEnabled__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__diagsIncluded__C, ".const:ti_sysbios_utils_Load_Module__diagsIncluded__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__diagsMask__C, ".const:ti_sysbios_utils_Load_Module__diagsMask__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__gateObj__C, ".const:ti_sysbios_utils_Load_Module__gateObj__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__gatePrms__C, ".const:ti_sysbios_utils_Load_Module__gatePrms__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__id__C, ".const:ti_sysbios_utils_Load_Module__id__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerDefined__C, ".const:ti_sysbios_utils_Load_Module__loggerDefined__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerObj__C, ".const:ti_sysbios_utils_Load_Module__loggerObj__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerFxn0__C, ".const:ti_sysbios_utils_Load_Module__loggerFxn0__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerFxn1__C, ".const:ti_sysbios_utils_Load_Module__loggerFxn1__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerFxn2__C, ".const:ti_sysbios_utils_Load_Module__loggerFxn2__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerFxn4__C, ".const:ti_sysbios_utils_Load_Module__loggerFxn4__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__loggerFxn8__C, ".const:ti_sysbios_utils_Load_Module__loggerFxn8__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Module__startupDoneFxn__C, ".const:ti_sysbios_utils_Load_Module__startupDoneFxn__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Object__count__C, ".const:ti_sysbios_utils_Load_Object__count__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Object__heap__C, ".const:ti_sysbios_utils_Load_Object__heap__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Object__sizeof__C, ".const:ti_sysbios_utils_Load_Object__sizeof__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_Object__table__C, ".const:ti_sysbios_utils_Load_Object__table__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_LS_cpuLoad__C, ".const:ti_sysbios_utils_Load_LS_cpuLoad__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_LS_hwiLoad__C, ".const:ti_sysbios_utils_Load_LS_hwiLoad__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_LS_swiLoad__C, ".const:ti_sysbios_utils_Load_LS_swiLoad__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_LS_taskLoad__C, ".const:ti_sysbios_utils_Load_LS_taskLoad__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_postUpdate__C, ".const:ti_sysbios_utils_Load_postUpdate__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_updateInIdle__C, ".const:ti_sysbios_utils_Load_updateInIdle__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_windowInMs__C, ".const:ti_sysbios_utils_Load_windowInMs__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_hwiEnabled__C, ".const:ti_sysbios_utils_Load_hwiEnabled__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_swiEnabled__C, ".const:ti_sysbios_utils_Load_swiEnabled__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_taskEnabled__C, ".const:ti_sysbios_utils_Load_taskEnabled__C");
#pragma DATA_SECTION(ti_sysbios_utils_Load_autoAddTasks__C, ".const:ti_sysbios_utils_Load_autoAddTasks__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__diagsEnabled__C, ".const:xdc_runtime_Assert_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__diagsIncluded__C, ".const:xdc_runtime_Assert_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__diagsMask__C, ".const:xdc_runtime_Assert_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__gateObj__C, ".const:xdc_runtime_Assert_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__gatePrms__C, ".const:xdc_runtime_Assert_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__id__C, ".const:xdc_runtime_Assert_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerDefined__C, ".const:xdc_runtime_Assert_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerObj__C, ".const:xdc_runtime_Assert_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerFxn0__C, ".const:xdc_runtime_Assert_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerFxn1__C, ".const:xdc_runtime_Assert_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerFxn2__C, ".const:xdc_runtime_Assert_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerFxn4__C, ".const:xdc_runtime_Assert_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__loggerFxn8__C, ".const:xdc_runtime_Assert_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Module__startupDoneFxn__C, ".const:xdc_runtime_Assert_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Object__count__C, ".const:xdc_runtime_Assert_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Object__heap__C, ".const:xdc_runtime_Assert_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Object__sizeof__C, ".const:xdc_runtime_Assert_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Assert_Object__table__C, ".const:xdc_runtime_Assert_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Assert_E_assertFailed__C, ".const:xdc_runtime_Assert_E_assertFailed__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__diagsEnabled__C, ".const:xdc_runtime_Core_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__diagsIncluded__C, ".const:xdc_runtime_Core_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__diagsMask__C, ".const:xdc_runtime_Core_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__gateObj__C, ".const:xdc_runtime_Core_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__gatePrms__C, ".const:xdc_runtime_Core_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__id__C, ".const:xdc_runtime_Core_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerDefined__C, ".const:xdc_runtime_Core_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerObj__C, ".const:xdc_runtime_Core_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerFxn0__C, ".const:xdc_runtime_Core_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerFxn1__C, ".const:xdc_runtime_Core_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerFxn2__C, ".const:xdc_runtime_Core_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerFxn4__C, ".const:xdc_runtime_Core_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__loggerFxn8__C, ".const:xdc_runtime_Core_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Core_Module__startupDoneFxn__C, ".const:xdc_runtime_Core_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Core_Object__count__C, ".const:xdc_runtime_Core_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Core_Object__heap__C, ".const:xdc_runtime_Core_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Core_Object__sizeof__C, ".const:xdc_runtime_Core_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Core_Object__table__C, ".const:xdc_runtime_Core_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Core_A_initializedParams__C, ".const:xdc_runtime_Core_A_initializedParams__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__diagsEnabled__C, ".const:xdc_runtime_Defaults_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__diagsIncluded__C, ".const:xdc_runtime_Defaults_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__diagsMask__C, ".const:xdc_runtime_Defaults_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__gateObj__C, ".const:xdc_runtime_Defaults_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__gatePrms__C, ".const:xdc_runtime_Defaults_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__id__C, ".const:xdc_runtime_Defaults_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerDefined__C, ".const:xdc_runtime_Defaults_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerObj__C, ".const:xdc_runtime_Defaults_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerFxn0__C, ".const:xdc_runtime_Defaults_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerFxn1__C, ".const:xdc_runtime_Defaults_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerFxn2__C, ".const:xdc_runtime_Defaults_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerFxn4__C, ".const:xdc_runtime_Defaults_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__loggerFxn8__C, ".const:xdc_runtime_Defaults_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Module__startupDoneFxn__C, ".const:xdc_runtime_Defaults_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Object__count__C, ".const:xdc_runtime_Defaults_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Object__heap__C, ".const:xdc_runtime_Defaults_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Object__sizeof__C, ".const:xdc_runtime_Defaults_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Defaults_Object__table__C, ".const:xdc_runtime_Defaults_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__diagsEnabled__C, ".const:xdc_runtime_Diags_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__diagsIncluded__C, ".const:xdc_runtime_Diags_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__diagsMask__C, ".const:xdc_runtime_Diags_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__gateObj__C, ".const:xdc_runtime_Diags_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__gatePrms__C, ".const:xdc_runtime_Diags_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__id__C, ".const:xdc_runtime_Diags_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerDefined__C, ".const:xdc_runtime_Diags_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerObj__C, ".const:xdc_runtime_Diags_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerFxn0__C, ".const:xdc_runtime_Diags_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerFxn1__C, ".const:xdc_runtime_Diags_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerFxn2__C, ".const:xdc_runtime_Diags_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerFxn4__C, ".const:xdc_runtime_Diags_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__loggerFxn8__C, ".const:xdc_runtime_Diags_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Module__startupDoneFxn__C, ".const:xdc_runtime_Diags_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Object__count__C, ".const:xdc_runtime_Diags_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Object__heap__C, ".const:xdc_runtime_Diags_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Object__sizeof__C, ".const:xdc_runtime_Diags_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Diags_Object__table__C, ".const:xdc_runtime_Diags_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Diags_setMaskEnabled__C, ".const:xdc_runtime_Diags_setMaskEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Diags_dictBase__C, ".const:xdc_runtime_Diags_dictBase__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__diagsEnabled__C, ".const:xdc_runtime_Error_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__diagsIncluded__C, ".const:xdc_runtime_Error_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__diagsMask__C, ".const:xdc_runtime_Error_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__gateObj__C, ".const:xdc_runtime_Error_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__gatePrms__C, ".const:xdc_runtime_Error_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__id__C, ".const:xdc_runtime_Error_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerDefined__C, ".const:xdc_runtime_Error_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerObj__C, ".const:xdc_runtime_Error_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerFxn0__C, ".const:xdc_runtime_Error_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerFxn1__C, ".const:xdc_runtime_Error_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerFxn2__C, ".const:xdc_runtime_Error_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerFxn4__C, ".const:xdc_runtime_Error_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__loggerFxn8__C, ".const:xdc_runtime_Error_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Error_Module__startupDoneFxn__C, ".const:xdc_runtime_Error_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Error_Object__count__C, ".const:xdc_runtime_Error_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Error_Object__heap__C, ".const:xdc_runtime_Error_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Error_Object__sizeof__C, ".const:xdc_runtime_Error_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Error_Object__table__C, ".const:xdc_runtime_Error_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Error_E_generic__C, ".const:xdc_runtime_Error_E_generic__C");
#pragma DATA_SECTION(xdc_runtime_Error_E_memory__C, ".const:xdc_runtime_Error_E_memory__C");
#pragma DATA_SECTION(xdc_runtime_Error_E_msgCode__C, ".const:xdc_runtime_Error_E_msgCode__C");
#pragma DATA_SECTION(xdc_runtime_Error_policy__C, ".const:xdc_runtime_Error_policy__C");
#pragma DATA_SECTION(xdc_runtime_Error_raiseHook__C, ".const:xdc_runtime_Error_raiseHook__C");
#pragma DATA_SECTION(xdc_runtime_Error_maxDepth__C, ".const:xdc_runtime_Error_maxDepth__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__diagsEnabled__C, ".const:xdc_runtime_Gate_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__diagsIncluded__C, ".const:xdc_runtime_Gate_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__diagsMask__C, ".const:xdc_runtime_Gate_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__gateObj__C, ".const:xdc_runtime_Gate_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__gatePrms__C, ".const:xdc_runtime_Gate_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__id__C, ".const:xdc_runtime_Gate_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerDefined__C, ".const:xdc_runtime_Gate_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerObj__C, ".const:xdc_runtime_Gate_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerFxn0__C, ".const:xdc_runtime_Gate_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerFxn1__C, ".const:xdc_runtime_Gate_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerFxn2__C, ".const:xdc_runtime_Gate_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerFxn4__C, ".const:xdc_runtime_Gate_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__loggerFxn8__C, ".const:xdc_runtime_Gate_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Module__startupDoneFxn__C, ".const:xdc_runtime_Gate_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Object__count__C, ".const:xdc_runtime_Gate_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Object__heap__C, ".const:xdc_runtime_Gate_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Object__sizeof__C, ".const:xdc_runtime_Gate_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Gate_Object__table__C, ".const:xdc_runtime_Gate_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__diagsEnabled__C, ".const:xdc_runtime_Log_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__diagsIncluded__C, ".const:xdc_runtime_Log_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__diagsMask__C, ".const:xdc_runtime_Log_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__gateObj__C, ".const:xdc_runtime_Log_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__gatePrms__C, ".const:xdc_runtime_Log_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__id__C, ".const:xdc_runtime_Log_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerDefined__C, ".const:xdc_runtime_Log_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerObj__C, ".const:xdc_runtime_Log_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerFxn0__C, ".const:xdc_runtime_Log_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerFxn1__C, ".const:xdc_runtime_Log_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerFxn2__C, ".const:xdc_runtime_Log_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerFxn4__C, ".const:xdc_runtime_Log_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__loggerFxn8__C, ".const:xdc_runtime_Log_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Log_Module__startupDoneFxn__C, ".const:xdc_runtime_Log_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Log_Object__count__C, ".const:xdc_runtime_Log_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Log_Object__heap__C, ".const:xdc_runtime_Log_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Log_Object__sizeof__C, ".const:xdc_runtime_Log_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Log_Object__table__C, ".const:xdc_runtime_Log_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_construct__C, ".const:xdc_runtime_Log_L_construct__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_create__C, ".const:xdc_runtime_Log_L_create__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_destruct__C, ".const:xdc_runtime_Log_L_destruct__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_delete__C, ".const:xdc_runtime_Log_L_delete__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_error__C, ".const:xdc_runtime_Log_L_error__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_warning__C, ".const:xdc_runtime_Log_L_warning__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_info__C, ".const:xdc_runtime_Log_L_info__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_start__C, ".const:xdc_runtime_Log_L_start__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_stop__C, ".const:xdc_runtime_Log_L_stop__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_startInstance__C, ".const:xdc_runtime_Log_L_startInstance__C");
#pragma DATA_SECTION(xdc_runtime_Log_L_stopInstance__C, ".const:xdc_runtime_Log_L_stopInstance__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__diagsEnabled__C, ".const:xdc_runtime_Main_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__diagsIncluded__C, ".const:xdc_runtime_Main_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__diagsMask__C, ".const:xdc_runtime_Main_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__gateObj__C, ".const:xdc_runtime_Main_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__gatePrms__C, ".const:xdc_runtime_Main_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__id__C, ".const:xdc_runtime_Main_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerDefined__C, ".const:xdc_runtime_Main_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerObj__C, ".const:xdc_runtime_Main_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerFxn0__C, ".const:xdc_runtime_Main_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerFxn1__C, ".const:xdc_runtime_Main_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerFxn2__C, ".const:xdc_runtime_Main_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerFxn4__C, ".const:xdc_runtime_Main_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__loggerFxn8__C, ".const:xdc_runtime_Main_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Main_Module__startupDoneFxn__C, ".const:xdc_runtime_Main_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Main_Object__count__C, ".const:xdc_runtime_Main_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Main_Object__heap__C, ".const:xdc_runtime_Main_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Main_Object__sizeof__C, ".const:xdc_runtime_Main_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Main_Object__table__C, ".const:xdc_runtime_Main_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__diagsEnabled__C, ".const:xdc_runtime_Memory_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__diagsIncluded__C, ".const:xdc_runtime_Memory_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__diagsMask__C, ".const:xdc_runtime_Memory_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__gateObj__C, ".const:xdc_runtime_Memory_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__gatePrms__C, ".const:xdc_runtime_Memory_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__id__C, ".const:xdc_runtime_Memory_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerDefined__C, ".const:xdc_runtime_Memory_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerObj__C, ".const:xdc_runtime_Memory_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerFxn0__C, ".const:xdc_runtime_Memory_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerFxn1__C, ".const:xdc_runtime_Memory_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerFxn2__C, ".const:xdc_runtime_Memory_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerFxn4__C, ".const:xdc_runtime_Memory_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__loggerFxn8__C, ".const:xdc_runtime_Memory_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Module__startupDoneFxn__C, ".const:xdc_runtime_Memory_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Object__count__C, ".const:xdc_runtime_Memory_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Object__heap__C, ".const:xdc_runtime_Memory_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Object__sizeof__C, ".const:xdc_runtime_Memory_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Memory_Object__table__C, ".const:xdc_runtime_Memory_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Memory_defaultHeapInstance__C, ".const:xdc_runtime_Memory_defaultHeapInstance__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__diagsEnabled__C, ".const:xdc_runtime_Registry_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__diagsIncluded__C, ".const:xdc_runtime_Registry_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__diagsMask__C, ".const:xdc_runtime_Registry_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__gateObj__C, ".const:xdc_runtime_Registry_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__gatePrms__C, ".const:xdc_runtime_Registry_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__id__C, ".const:xdc_runtime_Registry_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerDefined__C, ".const:xdc_runtime_Registry_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerObj__C, ".const:xdc_runtime_Registry_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerFxn0__C, ".const:xdc_runtime_Registry_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerFxn1__C, ".const:xdc_runtime_Registry_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerFxn2__C, ".const:xdc_runtime_Registry_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerFxn4__C, ".const:xdc_runtime_Registry_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__loggerFxn8__C, ".const:xdc_runtime_Registry_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Module__startupDoneFxn__C, ".const:xdc_runtime_Registry_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Object__count__C, ".const:xdc_runtime_Registry_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Object__heap__C, ".const:xdc_runtime_Registry_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Object__sizeof__C, ".const:xdc_runtime_Registry_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Registry_Object__table__C, ".const:xdc_runtime_Registry_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Startup_firstFxns__A, ".const:xdc_runtime_Startup_firstFxns__A");
#pragma DATA_SECTION(xdc_runtime_Startup_lastFxns__A, ".const:xdc_runtime_Startup_lastFxns__A");
#pragma DATA_SECTION(xdc_runtime_Startup_sfxnTab__A, ".const:xdc_runtime_Startup_sfxnTab__A");
#pragma DATA_SECTION(xdc_runtime_Startup_sfxnRts__A, ".const:xdc_runtime_Startup_sfxnRts__A");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__diagsEnabled__C, ".const:xdc_runtime_Startup_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__diagsIncluded__C, ".const:xdc_runtime_Startup_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__diagsMask__C, ".const:xdc_runtime_Startup_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__gateObj__C, ".const:xdc_runtime_Startup_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__gatePrms__C, ".const:xdc_runtime_Startup_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__id__C, ".const:xdc_runtime_Startup_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerDefined__C, ".const:xdc_runtime_Startup_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerObj__C, ".const:xdc_runtime_Startup_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerFxn0__C, ".const:xdc_runtime_Startup_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerFxn1__C, ".const:xdc_runtime_Startup_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerFxn2__C, ".const:xdc_runtime_Startup_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerFxn4__C, ".const:xdc_runtime_Startup_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__loggerFxn8__C, ".const:xdc_runtime_Startup_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Module__startupDoneFxn__C, ".const:xdc_runtime_Startup_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Object__count__C, ".const:xdc_runtime_Startup_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Object__heap__C, ".const:xdc_runtime_Startup_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Object__sizeof__C, ".const:xdc_runtime_Startup_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Startup_Object__table__C, ".const:xdc_runtime_Startup_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Startup_maxPasses__C, ".const:xdc_runtime_Startup_maxPasses__C");
#pragma DATA_SECTION(xdc_runtime_Startup_firstFxns__C, ".const:xdc_runtime_Startup_firstFxns__C");
#pragma DATA_SECTION(xdc_runtime_Startup_lastFxns__C, ".const:xdc_runtime_Startup_lastFxns__C");
#pragma DATA_SECTION(xdc_runtime_Startup_startModsFxn__C, ".const:xdc_runtime_Startup_startModsFxn__C");
#pragma DATA_SECTION(xdc_runtime_Startup_execImpl__C, ".const:xdc_runtime_Startup_execImpl__C");
#pragma DATA_SECTION(xdc_runtime_Startup_sfxnTab__C, ".const:xdc_runtime_Startup_sfxnTab__C");
#pragma DATA_SECTION(xdc_runtime_Startup_sfxnRts__C, ".const:xdc_runtime_Startup_sfxnRts__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__diagsEnabled__C, ".const:xdc_runtime_SysStd_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__diagsIncluded__C, ".const:xdc_runtime_SysStd_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__diagsMask__C, ".const:xdc_runtime_SysStd_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__gateObj__C, ".const:xdc_runtime_SysStd_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__gatePrms__C, ".const:xdc_runtime_SysStd_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__id__C, ".const:xdc_runtime_SysStd_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerDefined__C, ".const:xdc_runtime_SysStd_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerObj__C, ".const:xdc_runtime_SysStd_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerFxn0__C, ".const:xdc_runtime_SysStd_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerFxn1__C, ".const:xdc_runtime_SysStd_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerFxn2__C, ".const:xdc_runtime_SysStd_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerFxn4__C, ".const:xdc_runtime_SysStd_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__loggerFxn8__C, ".const:xdc_runtime_SysStd_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Module__startupDoneFxn__C, ".const:xdc_runtime_SysStd_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Object__count__C, ".const:xdc_runtime_SysStd_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Object__heap__C, ".const:xdc_runtime_SysStd_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Object__sizeof__C, ".const:xdc_runtime_SysStd_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_SysStd_Object__table__C, ".const:xdc_runtime_SysStd_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__diagsEnabled__C, ".const:xdc_runtime_System_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__diagsIncluded__C, ".const:xdc_runtime_System_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__diagsMask__C, ".const:xdc_runtime_System_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__gateObj__C, ".const:xdc_runtime_System_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__gatePrms__C, ".const:xdc_runtime_System_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__id__C, ".const:xdc_runtime_System_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerDefined__C, ".const:xdc_runtime_System_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerObj__C, ".const:xdc_runtime_System_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerFxn0__C, ".const:xdc_runtime_System_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerFxn1__C, ".const:xdc_runtime_System_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerFxn2__C, ".const:xdc_runtime_System_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerFxn4__C, ".const:xdc_runtime_System_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__loggerFxn8__C, ".const:xdc_runtime_System_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_System_Module__startupDoneFxn__C, ".const:xdc_runtime_System_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_System_Object__count__C, ".const:xdc_runtime_System_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_System_Object__heap__C, ".const:xdc_runtime_System_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_System_Object__sizeof__C, ".const:xdc_runtime_System_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_System_Object__table__C, ".const:xdc_runtime_System_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_System_A_cannotFitIntoArg__C, ".const:xdc_runtime_System_A_cannotFitIntoArg__C");
#pragma DATA_SECTION(xdc_runtime_System_maxAtexitHandlers__C, ".const:xdc_runtime_System_maxAtexitHandlers__C");
#pragma DATA_SECTION(xdc_runtime_System_extendFxn__C, ".const:xdc_runtime_System_extendFxn__C");
#pragma DATA_SECTION(xdc_runtime_Text_charTab__A, ".const:xdc_runtime_Text_charTab__A");
#pragma DATA_SECTION(xdc_runtime_Text_nodeTab__A, ".const:xdc_runtime_Text_nodeTab__A");
#pragma DATA_SECTION(xdc_runtime_Text_Module__diagsEnabled__C, ".const:xdc_runtime_Text_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__diagsIncluded__C, ".const:xdc_runtime_Text_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__diagsMask__C, ".const:xdc_runtime_Text_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__gateObj__C, ".const:xdc_runtime_Text_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__gatePrms__C, ".const:xdc_runtime_Text_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__id__C, ".const:xdc_runtime_Text_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerDefined__C, ".const:xdc_runtime_Text_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerObj__C, ".const:xdc_runtime_Text_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerFxn0__C, ".const:xdc_runtime_Text_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerFxn1__C, ".const:xdc_runtime_Text_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerFxn2__C, ".const:xdc_runtime_Text_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerFxn4__C, ".const:xdc_runtime_Text_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__loggerFxn8__C, ".const:xdc_runtime_Text_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Text_Module__startupDoneFxn__C, ".const:xdc_runtime_Text_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Text_Object__count__C, ".const:xdc_runtime_Text_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Text_Object__heap__C, ".const:xdc_runtime_Text_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Text_Object__sizeof__C, ".const:xdc_runtime_Text_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Text_Object__table__C, ".const:xdc_runtime_Text_Object__table__C");
#pragma DATA_SECTION(xdc_runtime_Text_nameUnknown__C, ".const:xdc_runtime_Text_nameUnknown__C");
#pragma DATA_SECTION(xdc_runtime_Text_nameEmpty__C, ".const:xdc_runtime_Text_nameEmpty__C");
#pragma DATA_SECTION(xdc_runtime_Text_nameStatic__C, ".const:xdc_runtime_Text_nameStatic__C");
#pragma DATA_SECTION(xdc_runtime_Text_isLoaded__C, ".const:xdc_runtime_Text_isLoaded__C");
#pragma DATA_SECTION(xdc_runtime_Text_charTab__C, ".const:xdc_runtime_Text_charTab__C");
#pragma DATA_SECTION(xdc_runtime_Text_nodeTab__C, ".const:xdc_runtime_Text_nodeTab__C");
#pragma DATA_SECTION(xdc_runtime_Text_charCnt__C, ".const:xdc_runtime_Text_charCnt__C");
#pragma DATA_SECTION(xdc_runtime_Text_nodeCnt__C, ".const:xdc_runtime_Text_nodeCnt__C");
#pragma DATA_SECTION(xdc_runtime_Text_unnamedModsLastId__C, ".const:xdc_runtime_Text_unnamedModsLastId__C");
#pragma DATA_SECTION(xdc_runtime_Text_registryModsLastId__C, ".const:xdc_runtime_Text_registryModsLastId__C");
#pragma DATA_SECTION(xdc_runtime_Text_visitRopeFxn__C, ".const:xdc_runtime_Text_visitRopeFxn__C");
#pragma DATA_SECTION(xdc_runtime_Text_visitRopeFxn2__C, ".const:xdc_runtime_Text_visitRopeFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__diagsEnabled__C, ".const:xdc_runtime_Timestamp_Module__diagsEnabled__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__diagsIncluded__C, ".const:xdc_runtime_Timestamp_Module__diagsIncluded__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__diagsMask__C, ".const:xdc_runtime_Timestamp_Module__diagsMask__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__gateObj__C, ".const:xdc_runtime_Timestamp_Module__gateObj__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__gatePrms__C, ".const:xdc_runtime_Timestamp_Module__gatePrms__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__id__C, ".const:xdc_runtime_Timestamp_Module__id__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerDefined__C, ".const:xdc_runtime_Timestamp_Module__loggerDefined__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerObj__C, ".const:xdc_runtime_Timestamp_Module__loggerObj__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerFxn0__C, ".const:xdc_runtime_Timestamp_Module__loggerFxn0__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerFxn1__C, ".const:xdc_runtime_Timestamp_Module__loggerFxn1__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerFxn2__C, ".const:xdc_runtime_Timestamp_Module__loggerFxn2__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerFxn4__C, ".const:xdc_runtime_Timestamp_Module__loggerFxn4__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__loggerFxn8__C, ".const:xdc_runtime_Timestamp_Module__loggerFxn8__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Module__startupDoneFxn__C, ".const:xdc_runtime_Timestamp_Module__startupDoneFxn__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Object__count__C, ".const:xdc_runtime_Timestamp_Object__count__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Object__heap__C, ".const:xdc_runtime_Timestamp_Object__heap__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Object__sizeof__C, ".const:xdc_runtime_Timestamp_Object__sizeof__C");
#pragma DATA_SECTION(xdc_runtime_Timestamp_Object__table__C, ".const:xdc_runtime_Timestamp_Object__table__C");
