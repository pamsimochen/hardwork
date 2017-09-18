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
TARGET = cmem

include $(ROOTDIR)/Rules.make

CC = $(MVTOOL_PREFIX)gcc
AR = $(MVTOOL_PREFIX)ar

CCuC = $(UCTOOL_PREFIX)gcc
ARuC = $(UCTOOL_PREFIX)ar

BIONIC	?= $(ANDROID_DIR)/bionic

CCbionic = $(CMD) $(BIONICTOOL_PREFIX)gcc
ARbionic = $(CMD) $(BIONICTOOL_PREFIX)ar

INCLUDEDIR = $(ROOTDIR)/include
LIBDIR	   = $(ROOTDIR)/lib

C_FLAGS += -I. -Wall

CFLAGS_BIONIC = -c  -x c  -fPIC -Wunused -Wall -fno-strict-aliasing  -fno-short-enums  -march=armv5t -Dfar=  -D_ANDROID_ -D_DEBUG_=1 -Dxdc_target_name__=Bionic -Dxdc_target_types__=google/targets/arm/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_4_4_3 -g -I$(BIONIC)/libc/arch-arm/include -I$(BIONIC)/libc/include -I$(BIONIC)/libstdc++/include -I$(BIONIC)/libc/kernel/common -I$(BIONIC)/libc/kernel/arch-arm -I$(BIONIC)/libm/include -I$(BIONIC)/libm/include/arch/arm -I.

COMP.b  = $(CCbionic) $(CFLAGS_BIONIC)
AR.b	= $(ARbionic) rc

COMP.c  = $(CC) $(C_FLAGS) $(CPP_FLAGS) -c
AR.c	= $(AR) rc

COMP.uC = $(CCuC) $(C_FLAGS) $(CPP_FLAGS) -c
AR.uC	= $(ARuC) rc

DBGTARGETS_BIONIC = $(LIBDIR)/$(TARGET)d.abnc
RELTARGETS_BIONIC = $(LIBDIR)/$(TARGET).abnc

BIONICTARGETS = $(DBGTARGETS_BIONIC) $(RELTARGETS_BIONIC)

GLIBCTARGETS = $(LIBDIR)/$(TARGET).a470MV $(LIBDIR)/$(TARGET)d.a470MV
UCLIBCTARGETS = $(LIBDIR)/$(TARGET).a470uC $(LIBDIR)/$(TARGET)d.a470uC

DBGTARGETS = $(LIBDIR)/$(TARGET)d.a470MV
RELTARGETS = $(LIBDIR)/$(TARGET).a470MV

DBGTARGETS_UC = $(LIBDIR)/$(TARGET)d.a470uC
RELTARGETS_UC = $(LIBDIR)/$(TARGET).a470uC

DBGCFLAGS = -g -D__DEBUG
RELCFLAGS = -O2

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)

TARGETHEADERS = $(addprefix $(INCLUDEDIR)/, $(HEADERS))

DBGOBJFILES = $(SOURCES:%.c=debug/%.o470MV) $(SOURCES:%.c=debug/%.o470uC)
RELOBJFILES = $(SOURCES:%.c=release/%.o470MV) $(SOURCES:%.c=release/%.o470uC)

DBGOBJFILES_BIONIC = $(SOURCES:%.c=debug/%.obnc)
RELOBJFILES_BIONIC = $(SOURCES:%.c=release/%.obnc)


.PHONY: clean debug release install all glibc uclibc bionic

glibc: $(GLIBCTARGETS) $(TARGETHEADERS)

uclibc: $(UCLIBCTARGETS) $(TARGETHEADERS)

all: glibc

install:

release: $(RELTARGETS) $(TARGETHEADERS)

debug: $(DBGTARGETS) $(TARGETHEADERS)

bionic: $(BIONICTARGETS) $(TARGETHEADERS)

$(filter %.abnc,$(RELTARGETS_BIONIC)): $(filter %.obnc,$(RELOBJFILES_BIONIC))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.b) $@ $^

$(filter %.a470MV,$(RELTARGETS)): $(filter %.o470MV,$(RELOBJFILES))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.c) $@ $^

$(filter %.a470uC,$(RELTARGETS_UC)): $(filter %.o470uC,$(RELOBJFILES))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.uC) $@ $^

$(filter %.abnc,$(DBGTARGETS_BIONIC)): $(filter %.obnc,$(DBGOBJFILES_BIONIC))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.b) $@ $^

$(filter %.a470MV,$(DBGTARGETS)): $(filter %.o470MV,$(DBGOBJFILES))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.c) $@ $^

$(filter %.a470uC,$(DBGTARGETS_UC)): $(filter %.o470uC,$(DBGOBJFILES))
	@mkdir -p $(ROOTDIR)/lib
	$(AR.uC) $@ $^

$(filter %.obnc,$(RELOBJFILES_BIONIC)): release/%.obnc: %.c $(HEADERS)
	@mkdir -p release
	$(COMP.b) $(RELCFLAGS) $(CFLAGS_BIONIC) -o $@ $<

$(filter %.o470MV,$(RELOBJFILES)): release/%.o470MV: %.c $(HEADERS)
	@mkdir -p release
	$(COMP.c) $(RELCFLAGS) -o $@ $<

$(filter %.o470uC,$(RELOBJFILES)): release/%.o470uC: %.c $(HEADERS)
	@mkdir -p release
	$(COMP.uC) $(RELCFLAGS) -o $@ $<

$(filter %.obnc,$(DBGOBJFILES_BIONIC)): debug/%.obnc: %.c $(HEADERS)
	@mkdir -p debug
	$(COMP.b) $(DBGCFLAGS) $(CFLAGS_BIONIC) -o $@ $<

$(filter %.o470MV,$(DBGOBJFILES)): debug/%.o470MV: %.c $(HEADERS)
	@mkdir -p debug
	$(COMP.c) $(DBGCFLAGS) -o $@ $<

$(filter %.o470uC,$(DBGOBJFILES)): debug/%.o470uC: %.c $(HEADERS)
	@mkdir -p debug
	$(COMP.uC) $(DBGCFLAGS) -o $@ $<

$(TARGETHEADERS): $(HEADERS)
	@echo Installing headers...
	@install -d $(INCLUDEDIR)
	@install $< $@

clean:
	-rm -rf release debug *~
	-rm -f $(DBGTARGETS) $(RELTARGETS)
#
#  @(#) ti.sdo.linuxutils.cmem; 2, 2, 0,1; 5-30-2012 11:50:08; /db/atree/library/trees/linuxutils/linuxutils-l01/src/ xlibrary

#

