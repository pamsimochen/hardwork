#
# Copyright (c) 2012, Texas Instruments Incorporated
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# *  Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# *  Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# *  Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

ROOTDIR = ../..
TARGET = $(notdir $(shell pwd))

include $(ROOTDIR)/Rules.make

CC = $(MVTOOL_PREFIX)gcc

CCuC = $(UCTOOL_PREFIX)gcc

BIONIC	?= $(ANDROID_DIR)/bionic

CCbionic = $(CMD) $(BIONICTOOL_PREFIX)gcc

C_FLAGS += -Wall -I$(ROOTDIR)/include

CFLAGS_BIONIC = -c  -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -fno-short-enums  -march=armv5t -Dfar=  -D_ANDROID_ -D_DEBUG_=1 -Dxdc_target_name__=Bionic -Dxdc_target_types__=google/targets/arm/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_4_4_3 -g -I$(BIONIC)/libc/arch-arm/include -I$(BIONIC)/libc/include -I$(BIONIC)/libstdc++/include -I$(BIONIC)/libc/kernel/common -I$(BIONIC)/libc/kernel/arch-arm -I$(BIONIC)/libm/include -I$(BIONIC)/libm/include/arch/arm -I. -I../../include

COMP.b  = $(CCbionic) $(CFLAGS_BIONIC)
#LINK.b	= $(CCbionic) $(LD_FLAGS)  -nostdlib  -Wl,-T,$(BIONIC_MISC)/armelf.x $(BIONIC_TARGETOBJS)/lib/crtbegin_dynamic.o /db/builds/cring/arm-eabi-4.4.3/lib/gcc/arm-eabi/4.4.3/libgcc.a $(BIONIC_TARGETOBJS)/lib/crtend_android.o $(BIONIC_TARGETOBJS)/lib/libc.so

LINK.b	= $(CCbionic) $(LD_FLAGS)  -nostdlib  -Wl,-T,$(BIONIC_MISC)/armelf.x $(BIONIC_TARGETOBJS)/lib/crtbegin_dynamic.o $(BIONIC_TARGETOBJS)/lib/crtend_android.o $(BIONIC_TARGETOBJS)/lib/libc.so -Wl,-z,noexecstack  -Wl,--fix-cortex-a8 -Wl,-dynamic-linker,/system/bin/linker

COMP.c = $(CC) $(C_FLAGS) $(CPP_FLAGS) -c
LINK.c = $(CC) $(LD_FLAGS)

COMP.uC = $(CCuC) $(C_FLAGS) $(CPP_FLAGS) -c
LINK.uC = $(CCuC) $(LD_FLAGS)

GLIBCTARGETS = release/$(TARGET) debug/$(TARGET)d
UCLIBCTARGETS = release/$(TARGET).x470uC debug/$(TARGET)d.x470uC

DBGTARGETS = debug/$(TARGET)d
RELTARGETS = release/$(TARGET)

DBGTARGETS_UC = debug/$(TARGET)d.x470uC
RELTARGETS_UC = release/$(TARGET).x470uC

DBGTARGETS_BIONIC = debug/$(TARGET)d.xbnc
RELTARGETS_BIONIC = release/$(TARGET).xbnc

DBGCFLAGS = -g -D__DEBUG
RELCFLAGS = -O2

DBGLDFLAGS = $(ROOTDIR)/lib/cmemd.a470MV $(ROOTDIR)/lib/cmemd.a470uC
RELLDFLAGS = $(ROOTDIR)/lib/cmem.a470MV $(ROOTDIR)/lib/cmem.a470uC

DBGLDFLAGS_BIONIC = $(ROOTDIR)/lib/cmemd.abnc
RELLDFLAGS_BIONIC = $(ROOTDIR)/lib/cmem.abnc

SOURCES = $(wildcard *.c)

DBGOBJFILES = $(SOURCES:%.c=debug/%.o470MV) $(SOURCES:%.c=debug/%.o470uC)
RELOBJFILES = $(SOURCES:%.c=release/%.o470MV) $(SOURCES:%.c=release/%.o470uC)

DBGOBJFILES_BIONIC = $(SOURCES:%.c=debug/%.obnc)
RELOBJFILES_BIONIC = $(SOURCES:%.c=release/%.obnc)

.PHONY: clean debug release install bionic

glibc: $(GLIBCTARGETS)

uclibc: $(UCLIBCTARGETS)

all: glibc

install:
	install -d $(EXEC_DIR)
	install $(RELTARGETS) $(EXEC_DIR)
	install $(DBGTARGETS) $(EXEC_DIR)

release: $(RELTARGETS)

debug: $(DBGTARGETS)

bionic: $(DBGTARGETS_BIONIC) $(RELTARGETS_BIONIC)

$(filter %apitest,$(RELTARGETS)): $(filter %.o470MV,$(RELOBJFILES)) \
    $(filter %.a470MV,$(RELLDFLAGS))
	$(LINK.c) -o $@ $^

$(filter %.x470uC,$(RELTARGETS_UC)): $(filter %.o470uC,$(RELOBJFILES)) \
    $(filter %.a470uC,$(RELLDFLAGS))
	$(LINK.uC) -o $@ $^

$(filter %.xbnc,$(RELTARGETS_BIONIC)): $(filter %.obnc,$(RELOBJFILES_BIONIC)) \
    $(filter %.abnc,$(RELLDFLAGS_BIONIC))
	$(LINK.b) -o $@ $^

$(filter %apitestd,$(DBGTARGETS)): $(filter %.o470MV,$(DBGOBJFILES)) \
    $(filter %.a470MV,$(DBGLDFLAGS))
	$(LINK.c) -o $@ $^

$(filter %.x470uC,$(DBGTARGETS_UC)): $(filter %.o470uC,$(DBGOBJFILES)) \
    $(filter %.a470uC,$(DBGLDFLAGS))
	$(LINK.uC) -o $@ $^

$(filter %.xbnc,$(DBGTARGETS_BIONIC)): $(filter %.obnc,$(DBGOBJFILES_BIONIC)) \
    $(filter %.abnc,$(DBGLDFLAGS_BIONIC))
	$(LINK.b) -o $@ $^

$(filter %.o470MV,$(RELOBJFILES)): release/%.o470MV: %.c
	@mkdir -p release
	$(COMP.c) $(RELCFLAGS) -o $@ $<

$(filter %.o470uC,$(RELOBJFILES)): release/%.o470uC: %.c
	@mkdir -p release
	$(COMP.uC) $(RELCFLAGS) -o $@ $<

$(filter %.obnc,$(RELOBJFILES_BIONIC)): release/%.obnc: %.c
	@mkdir -p release
	$(COMP.b) $(RELCFLAGS_BIONIC) -o $@ $<

$(filter %.o470MV,$(DBGOBJFILES)): debug/%.o470MV: %.c
	@mkdir -p debug
	$(COMP.c) $(DBGCFLAGS) -o $@ $<

$(filter %.o470uC,$(DBGOBJFILES)): debug/%.o470uC: %.c
	@mkdir -p debug
	$(COMP.uC) $(DBGCFLAGS) -o $@ $<

$(filter %.obnc,$(DBGOBJFILES_BIONIC)): debug/%.obnc: %.c
	@mkdir -p debug
	$(COMP.b) $(DBGCFLAGS_BIONIC) -o $@ $<

clean:
	-rm -rf release debug *~
	-rm -f $(DBGTARGETS) $(RELTARGETS)
	-rm -f $(DBGTARGETS_BIONIC) $(RELTARGETS_BIONIC)
#
#  @(#) ti.sdo.linuxutils.cmem; 2, 2, 0,1; 5-30-2012 11:50:07; /db/atree/library/trees/linuxutils/linuxutils-l01/src/ xlibrary

#

