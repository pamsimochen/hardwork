#   
#   @file   common.mk
#
#   @brief  QNX common makefile
#
#
#   ============================================================================
#
#   Copyright (c) 2008-2012, Texas Instruments Incorporated
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   Contact information for paper mail:
#   Texas Instruments
#   Post Office Box 655303
#   Dallas, Texas 75265
#   Contact information: 
#   http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
#   DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
#   ============================================================================
#   
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

NAME = syslink

define PINFO
PINFO DESCRIPTION=SysLink QNX client library
endef

# don't install the binaries, they are copied in base makefile
INSTALLDIR = /dev/null

# to override the usage of syslink memory manager
SYSLINK_USE_SYSMGR := 0

# source path
EXTRA_SRCVPATH += \
        $(SYSLINK_ROOT)/ti/syslink/osal/posix \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/usr \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/usr/Qnx \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos \
        $(SYSLINK_ROOT)/ti/syslink/utils/common \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr/Qnx \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos/usr/osal/Qnx \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/hlos/usr \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/hlos/usr/Qnx \
        $(SYSLINK_ROOT)/ti/syslink/cfg \
        $(SYSLINK_ROOT)/ti/syslink

EXCLUDE_OBJS = SyslinkMemMgr.o RingIO.o \
        RingIOShm.o SysLinkCfg.o GateMP.o Ipc.o GateHWSpinlock.o \
        GatePeterson.o FrameQ_ShMem_common.o Cache.o

# include path
EXTRA_INCVPATH += \
        $(SYSLINK_ROOT) \
        $(subst ;, ,$(SYSLINK_PKGPATH))

include $(MKFILES_ROOT)/qtargets.mk
OPTIMIZE__gcc=$(OPTIMIZE_NONE_gcc)

ifeq ("$(SYSLINK_PROFILE)","DEBUG")
CCFLAGS += -DDEBUG -DSYSLINK_BUILD_DEBUG -DSYSLINK_TRACE_ENABLE
endif

ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "1")
CCFLAGS += -DSYSLINK_BUILD_OPTIMIZE
endif

ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")
CCFLAGS += -DSYSLINK_PLATFORM_TI81XX
endif

CCFLAGS += -DSYSLINK_BUILDOS_QNX -DSYSLINK_BUILD_HLOS
