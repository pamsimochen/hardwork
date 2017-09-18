vpath % /data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/packages/ti/sysbios/
vpath %.c /data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/

CCOPTS    = --endian=little -mv7M3 --abi=eabi -q -ms --opt_for_speed=2  --program_level_compile -o3 -g --optimize_with_debug

XDC_ROOT  = /data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/

BIOS_ROOT = /data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/packages/ti/sysbios/

BIOS_DEFS =  -Dti_sysbios_BIOS_swiEnabled__D=TRUE -Dti_sysbios_BIOS_taskEnabled__D=TRUE -Dti_sysbios_BIOS_clockEnabled__D=TRUE -Dti_sysbios_knl_Swi_numPriorities__D=16 -Dti_sysbios_knl_Task_deleteTerminatedTasks__D=FALSE -Dti_sysbios_knl_Task_numPriorities__D=16 -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_TIMER -Dti_sysbios_knl_Clock_TICK_MODE=ti_sysbios_knl_Clock_TickMode_PERIODIC -Dti_sysbios_family_arm_m3_Hwi_dispatcherSwiSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherTaskSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherAutoNestingSupport__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_dispatcherIrpTrackingSupport__D=TRUE -Dti_sysbios_knl_Semaphore_supportsEvents__D=FALSE -Dti_sysbios_family_arm_m3_Hwi_enableException__D=TRUE -Dti_sysbios_family_arm_m3_Hwi_disablePriority__D=16U -Dti_sysbios_family_arm_ducati_GateDualCore_enableStats__D=FALSE

BIOS_INC  = -I"/data/datalocal_videoapps01/ipnc_tools/basic_tools/bios_6_33_05_46/packages/"

CC = /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5//bin/cl470 -c $(CCOPTS) -I /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5//include
ASM = /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5//bin/cl470 -c $(CCOPTS) -I /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5//include
AR = /data/datalocal_videoapps01/ipnc_tools/basic_tools/cgt470_4_9_5//bin/ar470 rq

DEL = rm -f

define RM
	$(if $(wildcard $1),$(DEL) $1,:)
endef

define ASSEMBLE
	@echo asmem3 $< ...
	@$(ASM) $(BIOS_DEFS) -I "/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/" -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_target_name__=M3 $(BIOS_INC) $<
endef

all: sysbios.lib

m3_Clobber_asm.obj: family/arm/m3/Clobber_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_Clobber_asm.obj

m3_IntrinsicsSupport_asm.obj: family/arm/m3/IntrinsicsSupport_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_IntrinsicsSupport_asm.obj

m3_TaskSupport_asm.obj: family/arm/m3/TaskSupport_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_TaskSupport_asm.obj

m3_Hwi_asm.obj: family/arm/m3/Hwi_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_Hwi_asm.obj

m3_Hwi_asm_switch.obj: family/arm/m3/Hwi_asm_switch.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=m3_Hwi_asm_switch.obj

ducati_CTM_asm.obj: family/arm/ducati/CTM_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=ducati_CTM_asm.obj

ducati_Core_asm.obj: family/arm/ducati/Core_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=ducati_Core_asm.obj

omap4430_Power_saveCpu.obj: family/arm/ducati/omap4430/Power_saveCpu.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=omap4430_Power_saveCpu.obj

omap4430_Power_resumeCpu.obj: family/arm/ducati/omap4430/Power_resumeCpu.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=omap4430_Power_resumeCpu.obj

dmtimer_Timer_asm.obj: timers/dmtimer/Timer_asm.sv7M makefile
	@-$(call RM, $@)
	$(ASSEMBLE) --output_file=dmtimer_Timer_asm.obj


BIOS.obj: BIOS.c hal/Hwi.c hal/Hwi_stack.c hal/Hwi_startup.c hal/Timer.c hal/Cache.c knl/Clock.c knl/Idle.c knl/Intrinsics.c knl/Queue.c knl/Semaphore.c knl/Swi.c knl/Swi_andn.c knl/Task.c family/arm/m3/Hwi.c family/arm/m3/TaskSupport.c family/arm/ducati/Core.c family/arm/ducati/GateDualCore.c family/arm/ducati/Timer.c family/arm/ducati/TimerSupport.c family/arm/ducati/TimestampProvider.c family/arm/ducati/Wugen.c heaps/HeapBuf.c heaps/HeapMem.c timers/dmtimer/Timer.c gates/GateHwi.c gates/GateMutex.c utils/Load.c syncs/SyncSem.c hal/unicache/Cache.c hal/ammu/AMMU.c makefile
	@-$(call RM, $@)
	@echo clem3 $< ...
	@$(CC) $(BIOS_DEFS) -I "/data/datalocal_videoapps01/ipnc_tools/basic_tools/xdctools_3_23_03_53/packages/" -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_target_name__=M3 $(BIOS_INC) \
	$(BIOS_ROOT)BIOS.c \
	$(BIOS_ROOT)hal/Hwi.c \
	$(BIOS_ROOT)hal/Hwi_stack.c \
	$(BIOS_ROOT)hal/Hwi_startup.c \
	$(BIOS_ROOT)hal/Timer.c \
	$(BIOS_ROOT)hal/Cache.c \
	$(BIOS_ROOT)knl/Clock.c \
	$(BIOS_ROOT)knl/Idle.c \
	$(BIOS_ROOT)knl/Intrinsics.c \
	$(BIOS_ROOT)knl/Queue.c \
	$(BIOS_ROOT)knl/Semaphore.c \
	$(BIOS_ROOT)knl/Swi.c \
	$(BIOS_ROOT)knl/Swi_andn.c \
	$(BIOS_ROOT)knl/Task.c \
	$(BIOS_ROOT)family/arm/m3/Hwi.c \
	$(BIOS_ROOT)family/arm/m3/TaskSupport.c \
	$(BIOS_ROOT)family/arm/ducati/Core.c \
	$(BIOS_ROOT)family/arm/ducati/GateDualCore.c \
	$(BIOS_ROOT)family/arm/ducati/Timer.c \
	$(BIOS_ROOT)family/arm/ducati/TimerSupport.c \
	$(BIOS_ROOT)family/arm/ducati/TimestampProvider.c \
	$(BIOS_ROOT)family/arm/ducati/Wugen.c \
	$(BIOS_ROOT)heaps/HeapBuf.c \
	$(BIOS_ROOT)heaps/HeapMem.c \
	$(BIOS_ROOT)timers/dmtimer/Timer.c \
	$(BIOS_ROOT)gates/GateHwi.c \
	$(BIOS_ROOT)gates/GateMutex.c \
	$(BIOS_ROOT)utils/Load.c \
	$(BIOS_ROOT)syncs/SyncSem.c \
	$(BIOS_ROOT)hal/unicache/Cache.c \
	$(BIOS_ROOT)hal/ammu/AMMU.c \

sysbios.lib: BIOS.obj m3_Clobber_asm.obj m3_IntrinsicsSupport_asm.obj m3_TaskSupport_asm.obj m3_Hwi_asm.obj m3_Hwi_asm_switch.obj ducati_CTM_asm.obj ducati_Core_asm.obj omap4430_Power_saveCpu.obj omap4430_Power_resumeCpu.obj dmtimer_Timer_asm.obj
	@echo arem3 $^ ...
	@-$(call RM, $@)
	@$(AR) $@ $^

clean:
	@-$(call RM, sysbios.lib)
	@-$(call RM, BIOS.obj)
	@-$(call RM, m3_Clobber_asm.obj)
	@-$(call RM, m3_IntrinsicsSupport_asm.obj)
	@-$(call RM, m3_TaskSupport_asm.obj)
	@-$(call RM, m3_Hwi_asm.obj)
	@-$(call RM, m3_Hwi_asm_switch.obj)
	@-$(call RM, ducati_CTM_asm.obj)
	@-$(call RM, ducati_Core_asm.obj)
	@-$(call RM, omap4430_Power_saveCpu.obj)
	@-$(call RM, omap4430_Power_resumeCpu.obj)
	@-$(call RM, dmtimer_Timer_asm.obj)

