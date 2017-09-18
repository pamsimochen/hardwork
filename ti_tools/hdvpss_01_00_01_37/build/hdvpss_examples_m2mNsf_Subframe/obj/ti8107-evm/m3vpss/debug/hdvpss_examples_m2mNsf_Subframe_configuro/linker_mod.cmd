/*
 * Do not modify this file; it is automatically generated from the template
 * linkcmd.xdt in the ti.targets.elf package and will be overwritten.
 */

/*
 * put '"'s around paths because, without this, the linker
 * considers '-' as minus operator, not a file name character.
 */


-l"/proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/build/hdvpss_examples_m2mNsf_Subframe/obj/ti8107-evm/m3vpss/debug/hdvpss_examples_m2mNsf_Subframe_configuro/package/cfg/m2mNsf_subframe_pem3.oem3"
-l"/proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/build/hdvpss_examples_m2mNsf_Subframe/obj/ti8107-evm/m3vpss/debug/hdvpss_examples_m2mNsf_Subframe_configuro/package/cfg/m2mNsf_subframe_pem3.src/sysbios/sysbios.lib"
-l"/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/ti/targets/arm/rtsarm/lib/ti.targets.arm.rtsarm.aem3"
-l"/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/ti/targets/arm/rtsarm/lib/boot.aem3"
-l"/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/ti/targets/arm/rtsarm/lib/auto_init.aem3"

--retain="*(xdc.meta)"

/* C6x Elf symbols */
--symbol_map __TI_STACK_SIZE=__STACK_SIZE
--symbol_map __TI_STACK_BASE=__stack
--symbol_map _stack=__stack



--args 0x0
-heap  0x0
-stack 0x4000

MEMORY
{
    LINUX_MEM : org = 0x80000000, len = 0x82a0000
    SR0 : org = 0x882a0000, len = 0x1000000
    DSS_M3_CODE_MEM : org = 0x8c7a0000, len = 0x3500000
    DDR3_RAM : org = 0x892a0000, len = 0x3500000
    DEBUG_MEM : org = 0x8fea0000, len = 0x100000
    TILER_8BIT_MEM : org = 0xa0000000, len = 0x8000000
    HDVPSS_DESC_MEM : org = 0xafc00000, len = 0x200000
    HDVPSS_SHARED_MEM : org = 0xafe00000, len = 0x200000
    HOST_VPSS_NOITFYMEM : org = 0xafb00000, len = 0x100000
    FRAME_BUFFER_MEM : org = 0xa8000000, len = 0x7b00000
    L2_RAM : org = 0x20004000, len = 0x10000
    L2_ROM : org = 0x0, len = 0x4000
    OCMC0_RAM : org = 0x300000, len = 0x20000
    OCMC1_RAM : org = 0x400000, len = 0x20000
}

/*
 * Linker command file contributions from all loaded packages:
 */

/* Content from xdc.services.global (null): */

/* Content from xdc (null): */

/* Content from xdc.corevers (null): */

/* Content from xdc.shelf (null): */

/* Content from xdc.services.spec (null): */

/* Content from xdc.services.intern.xsr (null): */

/* Content from xdc.services.intern.gen (null): */

/* Content from xdc.services.intern.cmd (null): */

/* Content from xdc.bld (null): */

/* Content from ti.targets (null): */

/* Content from ti.targets.arm (null): */

/* Content from ti.targets.arm.elf (null): */

/* Content from xdc.rov (null): */

/* Content from xdc.runtime (null): */

/* Content from ti.targets.arm.rtsarm (null): */

/* Content from ti.sysbios.interfaces (null): */

/* Content from ti.sysbios.family (null): */

/* Content from ti.sysbios.family.arm (ti/sysbios/family/arm/linkcmd.xdt): */
--retain "*(.vecs)"

/* Content from xdc.services.getset (null): */

/* Content from xdc.runtime.knl (null): */

/* Content from ti.sysbios.hal.ammu (ti/sysbios/hal/ammu/linkcmd.xdt): */
ti_sysbios_hal_ammu_AMMU_mmu = 1073743872;

/* Content from ti.catalog.arm.cortexm3 (null): */

/* Content from ti.catalog.peripherals.hdvicp2 (null): */

/* Content from ti.catalog (null): */

/* Content from ti.catalog.arm.peripherals.timers (null): */

/* Content from xdc.platform (null): */

/* Content from xdc.cfg (null): */

/* Content from ti.catalog.c6000 (null): */

/* Content from ti.catalog.arm.cortexa8 (null): */

/* Content from ti.platforms.evmDM8148 (null): */

/* Content from ti.sysbios (null): */

/* Content from ti.sysbios.hal (null): */

/* Content from ti.sysbios.knl (null): */

/* Content from ti.sysbios.family.arm.m3 (ti/sysbios/family/arm/m3/linkcmd.xdt): */

/* Content from ti.sysbios.timers.dmtimer (null): */

/* Content from ti.sysbios.family.arm.ducati (null): */

/* Content from ti.sysbios.gates (null): */

/* Content from ti.sysbios.heaps (null): */

/* Content from ti.sysbios.utils (null): */

/* Content from ti.sysbios.syncs (null): */

/* Content from ti.sysbios.xdcruntime (null): */

/* Content from ti.sysbios.hal.unicache (ti/sysbios/hal/unicache/linkcmd.xdt): */
ti_sysbios_hal_unicache_Cache_cache = 1073741824;

/* Content from hdvpss_examples_m2mNsf_Subframe_configuro (null): */

/* Content from xdc.services.io (null): */


/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;
TSK_idle = ti_sysbios_knl_Task_Object__table__V + 0;

SECTIONS
{
    .text: load >> DSS_M3_CODE_MEM
    .stack: load > DDR3_RAM
    GROUP: load > DDR3_RAM
    {
        .bss:
        .neardata:
        .rodata:
    }
    .cinit: load > DDR3_RAM
    .init_array: load > DDR3_RAM
    .const: load >> DSS_M3_CODE_MEM
    .data: load >> DDR3_RAM
    .fardata: load >> DDR3_RAM
    .switch: load >> DDR3_RAM
    .sysmem: load > DDR3_RAM
    .far: load >> DDR3_RAM
    .args: load > DDR3_RAM align = 0x4, fill = 0 {_argsize = 0x0; }
    .cio: load >> DDR3_RAM
    .bootVecs:  type = DSECT
    .vecs: load > 0x800, type = DSECT
    .resetVecs: load > 0x800
    .bss:extMemNonCache:heap: load >> HDVPSS_DESC_MEM
    .bss:extMemNonCache:remoteDebugCoreShm: load >> HDVPSS_DESC_MEM
    .bss:extMemNonCache:notify: load >> HOST_VPSS_NOITFYMEM, type = NOLOAD
    .bss:frameBuffer: load >> FRAME_BUFFER_MEM, type = NOLOAD
    .bss:tilerBuffer: load >> TILER_8BIT_MEM, type = NOLOAD
    .plt: load >> DSS_M3_CODE_MEM
    .ducatiBoot: load > 0x0
    .ducatiGates: load > 0x7f0, type = NOINIT
    xdc.meta: load >> DSS_M3_CODE_MEM, type = COPY

}
