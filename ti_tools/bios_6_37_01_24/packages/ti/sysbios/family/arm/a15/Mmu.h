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

#ifndef ti_sysbios_family_arm_a15_Mmu__include
#define ti_sysbios_family_arm_a15_Mmu__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_a15_Mmu__top__
#endif

#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif

#define ti_sysbios_family_arm_a15_Mmu___VERS 160


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/family/arm/a15/package/package.defs.h>

#include <xdc/runtime/IModule.h>
#include <xdc/runtime/Assert.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* NUM_LEVEL1_ENTRIES */
#define ti_sysbios_family_arm_a15_Mmu_NUM_LEVEL1_ENTRIES (4)

/* NUM_LEVEL2_ENTRIES */
#define ti_sysbios_family_arm_a15_Mmu_NUM_LEVEL2_ENTRIES (512)

/* DescriptorType */
enum ti_sysbios_family_arm_a15_Mmu_DescriptorType {
    ti_sysbios_family_arm_a15_Mmu_DescriptorType_INVALID0 = 0,
    ti_sysbios_family_arm_a15_Mmu_DescriptorType_BLOCK = 1,
    ti_sysbios_family_arm_a15_Mmu_DescriptorType_INVALID1 = 2,
    ti_sysbios_family_arm_a15_Mmu_DescriptorType_TABLE = 3
};
typedef enum ti_sysbios_family_arm_a15_Mmu_DescriptorType ti_sysbios_family_arm_a15_Mmu_DescriptorType;

/* DescriptorAttrs */
struct ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs {
    ti_sysbios_family_arm_a15_Mmu_DescriptorType type;
    xdc_Bool nsTable;
    xdc_UInt8 apTable;
    xdc_Bool xnTable;
    xdc_Bool pxnTable;
    xdc_Bool noExecute;
    xdc_Bool privNoExecute;
    xdc_Bool contiguous;
    xdc_Bool notGlobal;
    xdc_Bool accessFlag;
    xdc_UInt8 shareable;
    xdc_UInt8 accPerm;
    xdc_Bool nonSecure;
    xdc_UInt8 attrIndx;
    xdc_UInt8 reserved;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* Module_State */
typedef xdc_UInt64 __T1_ti_sysbios_family_arm_a15_Mmu_Module_State__firstLevelTableBuf;
typedef xdc_UInt64 *__ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__firstLevelTableBuf;
typedef __ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__firstLevelTableBuf __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__firstLevelTableBuf;
typedef xdc_UInt64 __T1_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf;
typedef xdc_UInt64 *__ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf;
typedef __ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf __T2_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf;
typedef xdc_UInt64 *__ARRAY2_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf[4];
typedef __ARRAY2_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf;
typedef xdc_UInt __T1_ti_sysbios_family_arm_a15_Mmu_Module_State__mairRegAttr;
typedef xdc_UInt __ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__mairRegAttr[8];
typedef __ARRAY1_ti_sysbios_family_arm_a15_Mmu_Module_State__mairRegAttr __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__mairRegAttr;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsEnabled ti_sysbios_family_arm_a15_Mmu_Module__diagsEnabled__C;

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsIncluded ti_sysbios_family_arm_a15_Mmu_Module__diagsIncluded__C;

/* Module__diagsMask */
typedef xdc_Bits16* CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__diagsMask ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C;

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_a15_Mmu_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__gateObj ti_sysbios_family_arm_a15_Mmu_Module__gateObj__C;

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_a15_Mmu_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__gatePrms ti_sysbios_family_arm_a15_Mmu_Module__gatePrms__C;

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_a15_Mmu_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__id ti_sysbios_family_arm_a15_Mmu_Module__id__C;

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerDefined ti_sysbios_family_arm_a15_Mmu_Module__loggerDefined__C;

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerObj ti_sysbios_family_arm_a15_Mmu_Module__loggerObj__C;

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn0 ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn0__C;

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn1 ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn1__C;

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn2 ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn2__C;

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn4 ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn4__C;

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn8 ti_sysbios_family_arm_a15_Mmu_Module__loggerFxn8__C;

/* Module__startupDoneFxn */
typedef xdc_Bool (*CT__ti_sysbios_family_arm_a15_Mmu_Module__startupDoneFxn)(void);
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Module__startupDoneFxn ti_sysbios_family_arm_a15_Mmu_Module__startupDoneFxn__C;

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_a15_Mmu_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Object__count ti_sysbios_family_arm_a15_Mmu_Object__count__C;

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_a15_Mmu_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Object__heap ti_sysbios_family_arm_a15_Mmu_Object__heap__C;

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_a15_Mmu_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Object__sizeof ti_sysbios_family_arm_a15_Mmu_Object__sizeof__C;

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_a15_Mmu_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_Object__table ti_sysbios_family_arm_a15_Mmu_Object__table__C;

/* A_nullPointer */
#define ti_sysbios_family_arm_a15_Mmu_A_nullPointer (ti_sysbios_family_arm_a15_Mmu_A_nullPointer__C)
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_a15_Mmu_A_nullPointer;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_A_nullPointer ti_sysbios_family_arm_a15_Mmu_A_nullPointer__C;

/* A_unknownDescType */
#define ti_sysbios_family_arm_a15_Mmu_A_unknownDescType (ti_sysbios_family_arm_a15_Mmu_A_unknownDescType__C)
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_a15_Mmu_A_unknownDescType;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_A_unknownDescType ti_sysbios_family_arm_a15_Mmu_A_unknownDescType__C;

/* defaultAttrs */
#define ti_sysbios_family_arm_a15_Mmu_defaultAttrs (ti_sysbios_family_arm_a15_Mmu_defaultAttrs__C)
typedef ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs CT__ti_sysbios_family_arm_a15_Mmu_defaultAttrs;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_defaultAttrs ti_sysbios_family_arm_a15_Mmu_defaultAttrs__C;

/* enableMMU */
#ifdef ti_sysbios_family_arm_a15_Mmu_enableMMU__D
#define ti_sysbios_family_arm_a15_Mmu_enableMMU (ti_sysbios_family_arm_a15_Mmu_enableMMU__D)
#else
#define ti_sysbios_family_arm_a15_Mmu_enableMMU (ti_sysbios_family_arm_a15_Mmu_enableMMU__C)
typedef xdc_Bool CT__ti_sysbios_family_arm_a15_Mmu_enableMMU;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_enableMMU ti_sysbios_family_arm_a15_Mmu_enableMMU__C;
#endif

/* mairRegAttr */
#define ti_sysbios_family_arm_a15_Mmu_mairRegAttr (ti_sysbios_family_arm_a15_Mmu_mairRegAttr__C)
typedef xdc_UInt __T1_ti_sysbios_family_arm_a15_Mmu_mairRegAttr;
typedef xdc_UInt __ARRAY1_ti_sysbios_family_arm_a15_Mmu_mairRegAttr[8];
typedef __ARRAY1_ti_sysbios_family_arm_a15_Mmu_mairRegAttr __TA_ti_sysbios_family_arm_a15_Mmu_mairRegAttr;
typedef __TA_ti_sysbios_family_arm_a15_Mmu_mairRegAttr CT__ti_sysbios_family_arm_a15_Mmu_mairRegAttr;
__extern __FAR__ const CT__ti_sysbios_family_arm_a15_Mmu_mairRegAttr ti_sysbios_family_arm_a15_Mmu_mairRegAttr__C;


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_a15_Mmu_Module_startup ti_sysbios_family_arm_a15_Mmu_Module_startup__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_Module_startup__E, "ti_sysbios_family_arm_a15_Mmu_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_a15_Mmu_Module_startup__E( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_Module_startup__F, "ti_sysbios_family_arm_a15_Mmu_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_a15_Mmu_Module_startup__F( xdc_Int state );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_Module_startup__R, "ti_sysbios_family_arm_a15_Mmu_Module_startup")
__extern xdc_Int ti_sysbios_family_arm_a15_Mmu_Module_startup__R( xdc_Int state );

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_Module__startupDone__S, "ti_sysbios_family_arm_a15_Mmu_Module__startupDone")
__extern xdc_Bool ti_sysbios_family_arm_a15_Mmu_Module__startupDone__S( void );

/* disable__E */
#define ti_sysbios_family_arm_a15_Mmu_disable ti_sysbios_family_arm_a15_Mmu_disable__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_disable__E, "ti_sysbios_family_arm_a15_Mmu_disable")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_disable__E( void );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_disable__F, "ti_sysbios_family_arm_a15_Mmu_disable")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_disable__F( void );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_disable__R( void );

/* enable__E */
#define ti_sysbios_family_arm_a15_Mmu_enable ti_sysbios_family_arm_a15_Mmu_enable__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_enable__E, "ti_sysbios_family_arm_a15_Mmu_enable")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_enable__E( void );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_enable__F, "ti_sysbios_family_arm_a15_Mmu_enable")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_enable__F( void );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_enable__R( void );

/* initDescAttrs__E */
#define ti_sysbios_family_arm_a15_Mmu_initDescAttrs ti_sysbios_family_arm_a15_Mmu_initDescAttrs__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_initDescAttrs__E, "ti_sysbios_family_arm_a15_Mmu_initDescAttrs")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_initDescAttrs__E( ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* descAttrs );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_initDescAttrs__F, "ti_sysbios_family_arm_a15_Mmu_initDescAttrs")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_initDescAttrs__F( ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* descAttrs );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_initDescAttrs__R( ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* descAttrs );

/* isEnabled__E */
#define ti_sysbios_family_arm_a15_Mmu_isEnabled ti_sysbios_family_arm_a15_Mmu_isEnabled__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_isEnabled__E, "ti_sysbios_family_arm_a15_Mmu_isEnabled")
__extern xdc_Bool ti_sysbios_family_arm_a15_Mmu_isEnabled__E( void );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_isEnabled__F, "ti_sysbios_family_arm_a15_Mmu_isEnabled")
__extern xdc_Bool ti_sysbios_family_arm_a15_Mmu_isEnabled__F( void );
__extern xdc_Bool ti_sysbios_family_arm_a15_Mmu_isEnabled__R( void );

/* setMAIR__E */
#define ti_sysbios_family_arm_a15_Mmu_setMAIR ti_sysbios_family_arm_a15_Mmu_setMAIR__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setMAIR__E, "ti_sysbios_family_arm_a15_Mmu_setMAIR")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setMAIR__E( xdc_UInt attrIndx, xdc_UInt attr );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setMAIR__F, "ti_sysbios_family_arm_a15_Mmu_setMAIR")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setMAIR__F( xdc_UInt attrIndx, xdc_UInt attr );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setMAIR__R( xdc_UInt attrIndx, xdc_UInt attr );

/* setFirstLevelDesc__E */
#define ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__E, "ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__E( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__F, "ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__F( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc__R( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );

/* setSecondLevelDesc__E */
#define ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__E
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__E, "ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__E( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__F, "ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__F( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc__R( xdc_Ptr virtualAddr, xdc_UInt64 phyAddr, ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs* attrs );

/* init__I */
#define ti_sysbios_family_arm_a15_Mmu_init ti_sysbios_family_arm_a15_Mmu_init__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_init__I, "ti_sysbios_family_arm_a15_Mmu_init")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_init__I( void );

/* enableAsm__I */
#define ti_sysbios_family_arm_a15_Mmu_enableAsm ti_sysbios_family_arm_a15_Mmu_enableAsm__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_enableAsm__I, "ti_sysbios_family_arm_a15_Mmu_enableAsm")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_enableAsm__I( void );

/* disableAsm__I */
#define ti_sysbios_family_arm_a15_Mmu_disableAsm ti_sysbios_family_arm_a15_Mmu_disableAsm__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_disableAsm__I, "ti_sysbios_family_arm_a15_Mmu_disableAsm")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_disableAsm__I( void );

/* writeMAIRAsm__I */
#define ti_sysbios_family_arm_a15_Mmu_writeMAIRAsm ti_sysbios_family_arm_a15_Mmu_writeMAIRAsm__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_writeMAIRAsm__I, "ti_sysbios_family_arm_a15_Mmu_writeMAIRAsm")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_writeMAIRAsm__I( xdc_UInt attrIndx, xdc_UInt attr );

/* initFirstLevelTableBuf__I */
#define ti_sysbios_family_arm_a15_Mmu_initFirstLevelTableBuf ti_sysbios_family_arm_a15_Mmu_initFirstLevelTableBuf__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_initFirstLevelTableBuf__I, "ti_sysbios_family_arm_a15_Mmu_initFirstLevelTableBuf")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_initFirstLevelTableBuf__I( xdc_UInt64* firstLevelTableBuf, xdc_UInt64** secondLevelTableBuf );

/* initSecondLevelTableBuf__I */
#define ti_sysbios_family_arm_a15_Mmu_initSecondLevelTableBuf ti_sysbios_family_arm_a15_Mmu_initSecondLevelTableBuf__I
xdc__CODESECT(ti_sysbios_family_arm_a15_Mmu_initSecondLevelTableBuf__I, "ti_sysbios_family_arm_a15_Mmu_initSecondLevelTableBuf")
__extern xdc_Void ti_sysbios_family_arm_a15_Mmu_initSecondLevelTableBuf__I( xdc_UInt64** secondLevelTableBuf );


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_a15_Mmu_Module_startupDone() ti_sysbios_family_arm_a15_Mmu_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_a15_Mmu_Object_heap() ti_sysbios_family_arm_a15_Mmu_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_a15_Mmu_Module_heap() ti_sysbios_family_arm_a15_Mmu_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_a15_Mmu_Module__id ti_sysbios_family_arm_a15_Mmu_Module_id( void ) 
{
    return ti_sysbios_family_arm_a15_Mmu_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_a15_Mmu_Module_hasMask( void ) 
{
    return ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C != NULL;
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_a15_Mmu_Module_getMask( void ) 
{
    return ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C != NULL ? *ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C : 0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_a15_Mmu_Module_setMask( xdc_Bits16 mask ) 
{
    if (ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C != NULL) *ti_sysbios_family_arm_a15_Mmu_Module__diagsMask__C = mask;
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_family_arm_a15_Mmu__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_a15_Mmu__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_a15_Mmu__internalaccess))

#ifndef ti_sysbios_family_arm_a15_Mmu__include_state
#define ti_sysbios_family_arm_a15_Mmu__include_state

/* Module_State */
struct ti_sysbios_family_arm_a15_Mmu_Module_State {
    __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__firstLevelTableBuf firstLevelTableBuf;
    __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__secondLevelTableBuf secondLevelTableBuf;
    __TA_ti_sysbios_family_arm_a15_Mmu_Module_State__mairRegAttr mairRegAttr;
};

/* Module__state__V */
extern struct ti_sysbios_family_arm_a15_Mmu_Module_State__ ti_sysbios_family_arm_a15_Mmu_Module__state__V;

#endif /* ti_sysbios_family_arm_a15_Mmu__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_a15_Mmu__nolocalnames)

#ifndef ti_sysbios_family_arm_a15_Mmu__localnames__done
#define ti_sysbios_family_arm_a15_Mmu__localnames__done

/* module prefix */
#define Mmu_NUM_LEVEL1_ENTRIES ti_sysbios_family_arm_a15_Mmu_NUM_LEVEL1_ENTRIES
#define Mmu_NUM_LEVEL2_ENTRIES ti_sysbios_family_arm_a15_Mmu_NUM_LEVEL2_ENTRIES
#define Mmu_DescriptorType ti_sysbios_family_arm_a15_Mmu_DescriptorType
#define Mmu_DescriptorAttrs ti_sysbios_family_arm_a15_Mmu_DescriptorAttrs
#define Mmu_Module_State ti_sysbios_family_arm_a15_Mmu_Module_State
#define Mmu_DescriptorType_INVALID0 ti_sysbios_family_arm_a15_Mmu_DescriptorType_INVALID0
#define Mmu_DescriptorType_BLOCK ti_sysbios_family_arm_a15_Mmu_DescriptorType_BLOCK
#define Mmu_DescriptorType_INVALID1 ti_sysbios_family_arm_a15_Mmu_DescriptorType_INVALID1
#define Mmu_DescriptorType_TABLE ti_sysbios_family_arm_a15_Mmu_DescriptorType_TABLE
#define Mmu_A_nullPointer ti_sysbios_family_arm_a15_Mmu_A_nullPointer
#define Mmu_A_unknownDescType ti_sysbios_family_arm_a15_Mmu_A_unknownDescType
#define Mmu_defaultAttrs ti_sysbios_family_arm_a15_Mmu_defaultAttrs
#define Mmu_enableMMU ti_sysbios_family_arm_a15_Mmu_enableMMU
#define Mmu_mairRegAttr ti_sysbios_family_arm_a15_Mmu_mairRegAttr
#define Mmu_disable ti_sysbios_family_arm_a15_Mmu_disable
#define Mmu_enable ti_sysbios_family_arm_a15_Mmu_enable
#define Mmu_initDescAttrs ti_sysbios_family_arm_a15_Mmu_initDescAttrs
#define Mmu_isEnabled ti_sysbios_family_arm_a15_Mmu_isEnabled
#define Mmu_setMAIR ti_sysbios_family_arm_a15_Mmu_setMAIR
#define Mmu_setFirstLevelDesc ti_sysbios_family_arm_a15_Mmu_setFirstLevelDesc
#define Mmu_setSecondLevelDesc ti_sysbios_family_arm_a15_Mmu_setSecondLevelDesc
#define Mmu_Module_name ti_sysbios_family_arm_a15_Mmu_Module_name
#define Mmu_Module_id ti_sysbios_family_arm_a15_Mmu_Module_id
#define Mmu_Module_startup ti_sysbios_family_arm_a15_Mmu_Module_startup
#define Mmu_Module_startupDone ti_sysbios_family_arm_a15_Mmu_Module_startupDone
#define Mmu_Module_hasMask ti_sysbios_family_arm_a15_Mmu_Module_hasMask
#define Mmu_Module_getMask ti_sysbios_family_arm_a15_Mmu_Module_getMask
#define Mmu_Module_setMask ti_sysbios_family_arm_a15_Mmu_Module_setMask
#define Mmu_Object_heap ti_sysbios_family_arm_a15_Mmu_Object_heap
#define Mmu_Module_heap ti_sysbios_family_arm_a15_Mmu_Module_heap

#endif /* ti_sysbios_family_arm_a15_Mmu__localnames__done */
#endif
