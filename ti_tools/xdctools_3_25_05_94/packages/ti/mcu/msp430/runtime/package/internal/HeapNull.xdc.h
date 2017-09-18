/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z63
 */

#ifndef ti_mcu_msp430_runtime_HeapNull__INTERNAL__
#define ti_mcu_msp430_runtime_HeapNull__INTERNAL__

#ifndef ti_mcu_msp430_runtime_HeapNull__internalaccess
#define ti_mcu_msp430_runtime_HeapNull__internalaccess
#endif

#include <ti/mcu/msp430/runtime/HeapNull.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* getStats */
#undef ti_mcu_msp430_runtime_HeapNull_getStats
#define ti_mcu_msp430_runtime_HeapNull_getStats ti_mcu_msp430_runtime_HeapNull_getStats__F

/* alloc */
#undef ti_mcu_msp430_runtime_HeapNull_alloc
#define ti_mcu_msp430_runtime_HeapNull_alloc ti_mcu_msp430_runtime_HeapNull_alloc__F

/* free */
#undef ti_mcu_msp430_runtime_HeapNull_free
#define ti_mcu_msp430_runtime_HeapNull_free ti_mcu_msp430_runtime_HeapNull_free__F

/* isBlocking */
#undef ti_mcu_msp430_runtime_HeapNull_isBlocking
#define ti_mcu_msp430_runtime_HeapNull_isBlocking ti_mcu_msp430_runtime_HeapNull_isBlocking__F

/* Module_startup */
#undef ti_mcu_msp430_runtime_HeapNull_Module_startup
#define ti_mcu_msp430_runtime_HeapNull_Module_startup ti_mcu_msp430_runtime_HeapNull_Module_startup__F

/* Instance_init */
#undef ti_mcu_msp430_runtime_HeapNull_Instance_init
#define ti_mcu_msp430_runtime_HeapNull_Instance_init ti_mcu_msp430_runtime_HeapNull_Instance_init__F

/* Instance_finalize */
#undef ti_mcu_msp430_runtime_HeapNull_Instance_finalize
#define ti_mcu_msp430_runtime_HeapNull_Instance_finalize ti_mcu_msp430_runtime_HeapNull_Instance_finalize__F

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_mcu_msp430_runtime_HeapNull_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_mcu_msp430_runtime_HeapNull_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_mcu_msp430_runtime_HeapNull_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_mcu_msp430_runtime_HeapNull_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_mcu_msp430_runtime_HeapNull_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_mcu_msp430_runtime_HeapNull_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_mcu_msp430_runtime_HeapNull_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_mcu_msp430_runtime_HeapNull_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_mcu_msp430_runtime_HeapNull_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_mcu_msp430_runtime_HeapNull_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_mcu_msp430_runtime_HeapNull_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_mcu_msp430_runtime_HeapNull_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_mcu_msp430_runtime_HeapNull_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_mcu_msp430_runtime_HeapNull_Module_GateProxy_query

/* Object__sizingError */
#line 1 "Error_inconsistent_object_size_in_ti.mcu.msp430.runtime.HeapNull"
typedef char ti_mcu_msp430_runtime_HeapNull_Object__sizingError[sizeof(ti_mcu_msp430_runtime_HeapNull_Object) > sizeof(ti_mcu_msp430_runtime_HeapNull_Struct) ? -1 : 1];


#endif /* ti_mcu_msp430_runtime_HeapNull__INTERNAL____ */
