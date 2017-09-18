/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-z63
 */

#ifndef ti_bios_support_Lck__INTERNAL__
#define ti_bios_support_Lck__INTERNAL__

#ifndef ti_bios_support_Lck__internalaccess
#define ti_bios_support_Lck__internalaccess
#endif

#include <ti/bios/support/Lck.h>

#undef xdc_FILE__
#ifndef xdc_FILE
#define xdc_FILE__ NULL
#else
#define xdc_FILE__ xdc_FILE
#endif

/* pend */
#undef ti_bios_support_Lck_pend
#define ti_bios_support_Lck_pend ti_bios_support_Lck_pend__F

/* post */
#undef ti_bios_support_Lck_post
#define ti_bios_support_Lck_post ti_bios_support_Lck_post__F

/* Module_startup */
#undef ti_bios_support_Lck_Module_startup
#define ti_bios_support_Lck_Module_startup ti_bios_support_Lck_Module_startup__F

/* Instance_init */
#undef ti_bios_support_Lck_Instance_init
#define ti_bios_support_Lck_Instance_init ti_bios_support_Lck_Instance_init__F

/* Instance_finalize */
#undef ti_bios_support_Lck_Instance_finalize
#define ti_bios_support_Lck_Instance_finalize ti_bios_support_Lck_Instance_finalize__F

/* per-module runtime symbols */
#undef Module__MID
#define Module__MID ti_bios_support_Lck_Module__id__C
#undef Module__DGSINCL
#define Module__DGSINCL ti_bios_support_Lck_Module__diagsIncluded__C
#undef Module__DGSENAB
#define Module__DGSENAB ti_bios_support_Lck_Module__diagsEnabled__C
#undef Module__DGSMASK
#define Module__DGSMASK ti_bios_support_Lck_Module__diagsMask__C
#undef Module__LOGDEF
#define Module__LOGDEF ti_bios_support_Lck_Module__loggerDefined__C
#undef Module__LOGOBJ
#define Module__LOGOBJ ti_bios_support_Lck_Module__loggerObj__C
#undef Module__LOGFXN0
#define Module__LOGFXN0 ti_bios_support_Lck_Module__loggerFxn0__C
#undef Module__LOGFXN1
#define Module__LOGFXN1 ti_bios_support_Lck_Module__loggerFxn1__C
#undef Module__LOGFXN2
#define Module__LOGFXN2 ti_bios_support_Lck_Module__loggerFxn2__C
#undef Module__LOGFXN4
#define Module__LOGFXN4 ti_bios_support_Lck_Module__loggerFxn4__C
#undef Module__LOGFXN8
#define Module__LOGFXN8 ti_bios_support_Lck_Module__loggerFxn8__C
#undef Module__G_OBJ
#define Module__G_OBJ ti_bios_support_Lck_Module__gateObj__C
#undef Module__G_PRMS
#define Module__G_PRMS ti_bios_support_Lck_Module__gatePrms__C
#undef Module__GP_create
#define Module__GP_create ti_bios_support_Lck_Module_GateProxy_create
#undef Module__GP_delete
#define Module__GP_delete ti_bios_support_Lck_Module_GateProxy_delete
#undef Module__GP_enter
#define Module__GP_enter ti_bios_support_Lck_Module_GateProxy_enter
#undef Module__GP_leave
#define Module__GP_leave ti_bios_support_Lck_Module_GateProxy_leave
#undef Module__GP_query
#define Module__GP_query ti_bios_support_Lck_Module_GateProxy_query

/* Object__sizingError */
#line 1 "Error_inconsistent_object_size_in_ti.bios.support.Lck"
typedef char ti_bios_support_Lck_Object__sizingError[sizeof(ti_bios_support_Lck_Object) > sizeof(ti_bios_support_Lck_Struct) ? -1 : 1];


#endif /* ti_bios_support_Lck__INTERNAL____ */
