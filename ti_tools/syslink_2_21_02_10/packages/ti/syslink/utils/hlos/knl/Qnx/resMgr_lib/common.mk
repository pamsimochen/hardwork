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

NAME = syslinkmgr

define PINFO
PINFO DESCRIPTION=SysLink QNX Resource Manager library
endef

# don't install the binaries, they are copied in base makefile
INSTALLDIR = /dev/null

# Override definitions in base Makefile if required
SYSLINK_USE_SYSMGR := 0

# source path
EXTRA_SRCVPATH += \
        $(SYSLINK_ROOT)/ti/syslink/cfg \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/DLOAD \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/DLOAD_SYM \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/TMS470_DLOAD_DYN \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/dlw_client \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/C60_DLOAD_DYN \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/C60_DLOAD_REL \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/TMS470_DLOAD_REL \
        $(SYSLINK_ROOT)/ti/syslink/family/common \
        $(SYSLINK_ROOT)/ti/syslink/family/common/ti81xx/ti81xxdsp \
        $(SYSLINK_ROOT)/ti/syslink/family/common/ti81xx/ti81xxducati \
        $(SYSLINK_ROOT)/ti/syslink/family/common/ti81xx/ti81xxducati/ti81xxvideom3 \
        $(SYSLINK_ROOT)/ti/syslink/family/common/ti81xx/ti81xxducati/ti81xxvpssm3 \
        $(SYSLINK_ROOT)/ti/syslink/family/hlos/knl/ti81xx \
        $(SYSLINK_ROOT)/ti/syslink/family/hlos/knl/ti81xx/ti81xxdsp \
        $(SYSLINK_ROOT)/ti/syslink/family/hlos/knl/ti81xx/ti81xxducati/ti81xxvideom3 \
        $(SYSLINK_ROOT)/ti/syslink/family/hlos/knl/ti81xx/ti81xxducati/ti81xxvpssm3 \
        $(SYSLINK_ROOT)/ti/syslink/family/hlos/knl/ti81xx/$(SYSLINK_BUILDOS) \
        $(SYSLINK_ROOT)/ti/syslink/utils/common \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos/knl \
        $(SYSLINK_ROOT)/ti/syslink/utils/hlos/knl/osal/$(SYSLINK_BUILDOS) \
        $(SYSLINK_ROOT)/ti/syslink/osal/posix \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl/notifyDrivers \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl/notifyDrivers/arch \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl/notifyDrivers/arch/ti81xx \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl/transports \
        $(SYSLINK_ROOT)/ti/syslink/ipc/hlos/knl/shMem

EXCLUDE_OBJS = test_arm_reloc.o test_c60_reloc.o Platform.o \
        GateMP.o GateHWSpinlock.o GatePeterson.o \
	GateHwSem.o Ipc_qnx.o

# include path
EXTRA_INCVPATH += \
        $(SYSLINK_ROOT) \
        $(SYSLINK_ROOT)/ti/syslink/procMgr/common/loaders/Elf/DLOAD/DLOAD_API \
        $(subst +, ,$(SYSLINK_PKGPATH))

include $(MKFILES_ROOT)/qtargets.mk
OPTIMIZE__gcc=$(OPTIMIZE_NONE_gcc)

ifeq ("$(SYSLINK_PROFILE)","DEBUG")
CCFLAGS += -DDEBUG -DSYSLINK_BUILD_DEBUG -DSYSLINK_TRACE_ENABLE
endif

ifeq ("$(SYSLINK_BUILD_OPTIMIZE)", "1")
CCFLAGS += -DSYSLINK_BUILD_OPTIMIZE
endif

# Choose the Notify Driver to be used.
ifeq ("$(SYSLINK_NOTIFYDRIVER)", "NOTIFYDRIVERCIRC")
CCFLAGS += -DSYSLINK_NOTIFYDRIVER_CIRC -DUSE_SYSLINK_NOTIFY
else
# The default is NOTIFYDRIVERSHM
CCFLAGS += -DSYSLINK_NOTIFYDRIVER_SHM
endif

# Choose the MessageQ Transport to be used.
ifeq ("$(SYSLINK_TRANSPORT)", "TRANSPORTSHMNOTIFY")
CCFLAGS += -DSYSLINK_TRANSPORT_SHMNOTIFY
else ifeq ("$(SYSLINK_TRANSPORT)", "TRANSPORTSHMCIRC")
CCFLAGS += -DSYSLINK_TRANSPORT_SHMCIRC
else
# The default is TRANSPORTSHM
CCFLAGS += -DSYSLINK_TRANSPORT_SHM
endif

ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")

CCFLAGS += -DSYSLINK_PLATFORM_TI81XX
CCFLAGS += -DSYSLINK_LOADER_ELF
CCFLAGS += -DARM_TARGET
CCFLAGS += -DC60_TARGET
CCFLAGS += -DLOADER_DEBUG=1

ifeq ("$(SYSLINK_VARIANT)", "TI814X")
CCFLAGS += -DSYSLINK_VARIANT_TI814X
endif

ifeq ("$(SYSLINK_VARIANT)", "TI811X")
CCFLAGS += -DSYSLINK_VARIANT_TI811X
endif

endif

CCFLAGS += -DSYSLINK_BUILDOS_QNX -DSYSLINK_BUILD_HLOS
CCFLAGS += -DUSE_SYSLINK_NOTIFY -DRESOURCE_MANAGER
