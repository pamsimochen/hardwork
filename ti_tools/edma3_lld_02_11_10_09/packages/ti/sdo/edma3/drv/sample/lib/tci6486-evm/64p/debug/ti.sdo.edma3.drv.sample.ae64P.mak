#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C64P{1,0,7.2,3
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P.dep
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P.dep: ;
endif

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P: package/package_ti.sdo.edma3.drv.sample.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -s oe64P $< -C   -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P: package/package_ti.sdo.edma3.drv.sample.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P -n $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -s oe64P $< -C  -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P.dep
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P.dep: ;
endif

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P: src/platforms/sample_tci6486_cfg.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -s oe64P $< -C   -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P: src/platforms/sample_tci6486_cfg.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P -n $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -s oe64P $< -C  -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P.dep
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P.dep: ;
endif

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P: src/platforms/sample_tci6486_int_reg.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -s oe64P $< -C   -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P: src/platforms/sample_tci6486_int_reg.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P -n $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -s oe64P $< -C  -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P.dep
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P.dep: ;
endif

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P: src/sample_cs.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -s oe64P $< -C   -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P: src/sample_cs.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P -n $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -s oe64P $< -C  -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P.dep
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P.dep: ;
endif

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P: src/sample_init.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -s oe64P $< -C   -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P: | .interfaces
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P: src/sample_init.c lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle64P -n $< ...
	$(ti.targets.elf.C64P.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -s oe64P $< -C  -n -s --symdebug:none -qq -pdsw225 -mv64p --abi=eabi -eo.oe64P -ea.se64P  -mi10 -mo  -D_DEBUG_=1 -DCHIP_TCI6486 -Dxdc_target_name__=C64P -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_2_3 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C64P.rootDir)/include -fs=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src -fr=./package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P: export C_DIR=
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P: PATH:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)
package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P: Path:=$(ti.targets.elf.C64P.rootDir)/bin/;$(PATH)

clean,e64P ::
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.se64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.se64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.se64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.se64P
	-$(RM) package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.se64P

lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P: package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/package/package_ti.sdo.edma3.drv.sample.oe64P package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_cfg.oe64P package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/platforms/sample_tci6486_int_reg.oe64P package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_cs.oe64P package/lib/lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample/src/sample_init.oe64P lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak

clean::
	-$(RM) lib/tci6486-evm/64p/debug/ti.sdo.edma3.drv.sample.ae64P.mak