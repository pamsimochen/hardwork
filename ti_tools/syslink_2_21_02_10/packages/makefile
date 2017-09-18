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
#

SYSLINK_REPO = $(SYSLINK_INSTALL_DIR)/packages
XDC = $(XDC_INSTALL_DIR)/xdc

#
#  Determine host OS (Windows or UNIX)
#
ifeq (,$(HOSTOS))
    ifeq (,$(findstring :,$(WINDIR)$(windir)$(COMSPEC)$(comspec)))
       HOSTOS := Linux
    else
       HOSTOS := Windows
    endif
endif

ifeq (Windows,$(HOSTOS))
    SHELL = $(XDC_INSTALL_DIR)/bin/sh.exe
else
    # assume Linux
endif

XDC_PATH := $(IPC_INSTALL_DIR)/packages
XDC_PATH := $(XDC_PATH);$(BIOS_INSTALL_DIR)/packages;$(SYSLINK_REPO)

#if FC_INSTALL_DIR is set, add it to XDCPATH
ifneq ("$(FC_INSTALL_DIR)", "")
    XDC_PATH := $(XDC_PATH);$(FC_INSTALL_DIR)/packages
endif

ifeq ("$(SDK)", "NONE")
    SYSLINK_SDK =
else ifeq ("$(SDK)", "EZSDK")
    SYSLINK_SDK = $(SDK)
endif

LOADER_OPT =
ifneq ("$(LOADER)", "")
    LOADER_OPT = SYSLINK_LOADER=$(LOADER)
endif

SYSLINK_PKGLIST = ti/syslink
SYSLINK_PKGLIST += ti/syslink/ipc/rtos ti/syslink/utils/rtos/bios6

ifeq ("$(DEVICE)","OMAP3530")
    SYSLINK_DEVICE=$(DEVICE)
    DEVICE_VARIANT=$(DEVICE)
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_C64P_DIR=$(CGT_C64P_INSTALL_DIR) \
        CGT_C64P_ELF_DIR=$(CGT_C64P_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE)
else ifeq ("$(DEVICE)","TI816X")
    SYSLINK_DEVICE=TI81XX
    DEVICE_VARIANT=TI816X
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_C674_ELF_DIR=$(CGT_C674_ELF_INSTALL_DIR) \
        CGT_M3_ELF_DIR=$(CGT_M3_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE_VARIANT)
ifneq ("$(TI81XXDSP_DMTIMER_FREQ)","")
    XDCARGS := $(XDCARGS) TI81XXDSP_DMTIMER_FREQ=$(TI81XXDSP_DMTIMER_FREQ)
endif
else ifeq ("$(DEVICE)","TI814X")
    SYSLINK_DEVICE=TI81XX
    DEVICE_VARIANT=TI814X
    device=ti814x
    ifneq ("$(CGT_A8_ELF_INSTALL_DIR)","")
        SYSLINK_PKGLIST += ti/syslink/procMgr/rtos/bios6
        SYSLINK_PKGLIST += ti/syslink/family/rtos/bios6/ti814x
    endif
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_C674_ELF_DIR=$(CGT_C674_ELF_INSTALL_DIR) \
        CGT_M3_ELF_DIR=$(CGT_M3_ELF_INSTALL_DIR) \
        CGT_A8_ELF_DIR=$(CGT_A8_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE_VARIANT)
ifneq ("$(TI81XXDSP_DMTIMER_FREQ)","")
    XDCARGS := $(XDCARGS) TI81XXDSP_DMTIMER_FREQ=$(TI81XXDSP_DMTIMER_FREQ)
endif
else ifeq ("$(DEVICE)","TI813X")
    SYSLINK_DEVICE=TI81XX
    DEVICE_VARIANT=TI813X
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_M3_ELF_DIR=$(CGT_M3_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE_VARIANT)
else ifeq ("$(DEVICE)","TI811X")
    SYSLINK_DEVICE=TI81XX
    DEVICE_VARIANT=TI811X
    device=ti811x
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_C674_ELF_DIR=$(CGT_C674_ELF_INSTALL_DIR) \
        CGT_M3_ELF_DIR=$(CGT_M3_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE_VARIANT)
ifneq ("$(TI81XXDSP_DMTIMER_FREQ)","")
    XDCARGS := $(XDCARGS) TI81XXDSP_DMTIMER_FREQ=$(TI81XXDSP_DMTIMER_FREQ)
endif
else ifeq ("$(DEVICE)","OMAPL1XX")
    SYSLINK_DEVICE=$(DEVICE)
    DEVICE_VARIANT=$(DEVICE)
    XDCARGS = \
        DEVICE=$(DEVICE) \
        SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
        CGT_C674_DIR=$(CGT_C674_INSTALL_DIR) \
        CGT_C674_ELF_DIR=$(CGT_C674_ELF_INSTALL_DIR) \
        DEVICE_VARIANT=$(DEVICE)
endif

ifeq ("$(GPPOS)","Linux")
    SYSLINK_DRIVER = syslink-driver-linux
    SYSLINK_HLOS = syslink-hlos-linux
    KERNEL_VERSION = $(shell cat $(LINUXKERNEL)/include/config/kernel.release)
    INSTALL_DRIVER = install-driver-linux
else ifeq ("$(GPPOS)","Qnx")
    SYSLINK_DRIVER = syslink-driver-qnx
    SYSLINK_HLOS = syslink-hlos-qnx
    INSTALL_DRIVER = install-driver-qnx
    INSTALL_SAMPLES = install-samples-qnx
    ifneq ($(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe),)
        INSTALL_D = $(XDC_INSTALL_DIR)/bin/mkdir.exe -p
        INSTALL_M = $(XDC_INSTALL_DIR)/bin/cp.exe
        INSTALL_  = $(XDC_INSTALL_DIR)/bin/cp.exe
    else
        INSTALL_D = install -d
        INSTALL_M = install -m 0755
        INSTALL_  = install
    endif
endif #ifeq ("$(GPPOS)","Linux")

ifneq ($(wildcard $(QNX_INSTALL_DIR)),)
    QNX_PATH := $(QNX_INSTALL_DIR)/host/linux/x86/usr/bin
    QNX_PATH := $(QNX_PATH):$(QNX_INSTALL_DIR)/host/linux/x86/bin
    QNX_PATH := $(QNX_PATH):$(QNX_INSTALL_DIR)/host/linux/x86/sbin
    QNX_PATH := $(QNX_PATH):$(QNX_INSTALL_DIR)/host/linux/x86/usr/sbin
    QNX_PATH := $(QNX_PATH):$(QNX_INSTALL_DIR)/linux/x86/usr/photon/appbuilder

    export QNX_CONFIGURATION?=$(QNX_INSTALL_DIR)/etc/qnx
    export QNX_HOST?=$(QNX_INSTALL_DIR)/host/linux/x86
    export QNX_TARGET?=$(QNX_INSTALL_DIR)/target/qnx6
    export MAKEFLAGS=-I$(QNX_INSTALL_DIR)/target/qnx6/usr/include
    export LD_LIBRARY_PATH=$(QNX_INSTALL_DIR)/host/linux/x86/usr/lib
endif

syslink-driver: $(SYSLINK_DRIVER)
syslink-hlos: $(SYSLINK_HLOS)

install-driver: $(INSTALL_DRIVER)
install-samples: $(INSTALL_SAMPLES)


#
#  ======== syslink-driver ========
#
syslink-driver-linux:
	@echo "#"
	@echo "# Making $@ in ti/syslink/utils/hlos/knl/Linux"
	$(MAKE) -C ti/syslink/utils/hlos/knl/Linux \
            ARCH=arm CROSS_COMPILE=$(CGT_ARM_PREFIX) \
            KDIR=$(LINUXKERNEL) \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_PKGPATH="$(XDC_PATH)" \
            SYSLINK_VARIANT=$(DEVICE_VARIANT) $(LOADER_OPT) \
            SYSLINK_BUILD_DEBUG=$(SYSLINK_BUILD_DEBUG) \
            SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
            SYSLINK_TRACE_ENABLE=$(SYSLINK_TRACE_ENABLE)\
            SYSLINK_NOTIFYDRIVER=$(SYSLINK_NOTIFYDRIVER) \
            SYSLINK_TRANSPORT=$(SYSLINK_TRANSPORT) \
            SYSLINK_BUILDOS=$(GPPOS)

syslink-driver-qnx: PATH:=$(QNX_PATH):$(PATH)
syslink-driver-qnx:
	@echo "#"
	@echo "# Making $@ in ti/syslink/utils/hlos/knl/Qnx"
	$(MAKE) -C ti/syslink/utils/hlos/knl/Qnx \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_PKGPATH=$(subst ;,+,$(XDC_PATH)) \
            SYSLINK_VARIANT=$(DEVICE_VARIANT) $(LOADER_OPT) \
            SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
            SYSLINK_NOTIFYDRIVER=$(SYSLINK_NOTIFYDRIVER) \
            SYSLINK_TRANSPORT=$(SYSLINK_TRANSPORT) \
            SYSLINK_BUILDOS=$(GPPOS) \
            install

#
#  ======== syslink-hlos ========
#
syslink-hlos-linux:
	@echo "#"
	@echo "# Making $@ ti/syslink/utils/hlos/usr/Linux"
	$(MAKE) -C ti/syslink/utils/hlos/usr/Linux \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            TOOLCHAIN_PREFIX=$(CGT_ARM_PREFIX) \
            SYSLINK_PKGPATH="$(XDC_PATH)" $(LOADER_OPT) \
	    SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
            SYSLINK_BUILDOS=$(GPPOS)

syslink-hlos-qnx: PATH:=$(QNX_PATH):$(PATH)
syslink-hlos-qnx:
	@echo "#"
	@echo "# Making $@ ti/syslink/utils/hlos/usr/Qnx"
	$(MAKE) -C ti/syslink/utils/hlos/usr/Qnx \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_PKGPATH=$(IPC_INSTALL_DIR)/packages \
	    SYSLINK_BUILD_OPTIMIZE=$(SYSLINK_BUILD_OPTIMIZE) \
            SYSLINK_BUILDOS=$(GPPOS) \
            install

#
#  ======== syslink-rtos ========
#
syslink-rtos:
	@echo "#"
	@echo "# Making $@ in ti/syslink"
	$(XDC) XDCBUILDCFG=$(CURDIR)/_config.bld XDCPATH="$(XDC_PATH)" \
            XDCARGS="$(XDCARGS)" .libraries -P $(SYSLINK_PKGLIST)


ifeq ("$(GPPOS)","Linux")
GPPOS_LINUX_OR_ANDROID	:= "Linux"
else
GPPOS_LINUX_OR_ANDROID	:=
endif

#
#  ======== syslink-samples ========
#
ifneq ("$(GPPOS_LINUX_OR_ANDROID)","")

SAMPLE_VARS = SYSLINK_ROOT=$(SYSLINK_REPO) TOOLCHAIN_PREFIX=$(CGT_ARM_PREFIX) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) SYSLINK_PKGPATH="$(XDC_PATH)" \
            SYSLINK_SDK=$(SYSLINK_SDK) SYSLINK_VARIANT=$(DEVICE_VARIANT) \
            SYSLINK_BUILDOS=$(GPPOS)

Common:
	@echo "#"
	@echo "# Making $@ in ti/syslink/samples/hlos/common/usr/Linux"
	$(MAKE) -C ti/syslink/samples/hlos/common/usr/Linux $(SAMPLE_VARS)

syslink-samples-hlos: SlaveLoader
SlaveLoader: Common
	@echo "#"
	@echo "# Making $@ in ti/syslink/samples/hlos/slaveLoader/usr/Linux"
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoader/usr/Linux $(SAMPLE_VARS)

else ifeq ("$(GPPOS)","Qnx")

Common: PATH:=$(QNX_PATH):$(PATH)
Common:
	@echo "#"
	@echo "# Making $@ in ti/syslink/samples/hlos/common/usr/Qnx"
	$(MAKE) -C ti/syslink/samples/hlos/common/usr/Qnx \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PKGPATH=$(subst ;,+,$(XDC_PATH)) \
            SYSLINK_BUILDOS=$(GPPOS) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_VARIANT=$(DEVICE_VARIANT) \
            install

SlaveLoader: Common
SlaveLoader: PATH:=$(QNX_PATH):$(PATH)
SlaveLoader:
	@echo "#"
	@echo "# Making $@ in ti/syslink/samples/hlos/slaveLoaderlib/usr/Qnx"
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoaderlib/usr/Qnx \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PKGPATH=$(subst ;,+,$(XDC_PATH)) \
            SYSLINK_BUILDOS=$(GPPOS) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_VARIANT=$(DEVICE_VARIANT) \
            install

syslink-samples-hlos: slaveLoader
slaveLoader: Common SlaveLoader
slaveLoader: PATH:=$(QNX_PATH):$(PATH)
slaveLoader:
	@echo "#"
	@echo "# Making $@ ti/syslink/samples/hlos/slaveLoader/usr/Qnx"
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoader/usr/Qnx \
            SYSLINK_ROOT=$(SYSLINK_REPO) \
            SYSLINK_PKGPATH=$(subst ;,+,$(XDC_PATH)) \
            SYSLINK_BUILDOS=$(GPPOS) \
            SYSLINK_PLATFORM=$(SYSLINK_DEVICE) \
            SYSLINK_VARIANT=$(DEVICE_VARIANT) \
            install

endif # ifeq ("$(GPPOS)","Qnx")

install-driver-linux:
	@echo "#"
	@echo "# Installing SysLink kernel driver in $(EXEC_DIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/dsp"
	install -d $(EXEC_DIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/dsp
	install -m 0755 $(SYSLINK_REPO)/ti/syslink/bin/$(DEVICE)/syslink.ko $(EXEC_DIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/dsp

install-driver-qnx:
	@echo "#"
	@echo "# Installing SysLink Device Manager in $(EXEC_DIR)/ti/syslink/driver"
	$(INSTALL_D) $(EXEC_DIR)/ti/syslink/driver/debug
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/bin/qnx/$(device)/debug/syslink_drv $(EXEC_DIR)/ti/syslink/driver/debug
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/utils/hlos/knl/Qnx/resMgr/run_syslink_drv.sh $(EXEC_DIR)/ti/syslink/driver/debug
	$(INSTALL_D) $(EXEC_DIR)/ti/syslink/driver/release
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/bin/qnx/$(device)/release/syslink_drv $(EXEC_DIR)/ti/syslink/driver/release
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/utils/hlos/knl/Qnx/resMgr/run_syslink_drv.sh $(EXEC_DIR)/ti/syslink/driver/release
	@echo "#"
	@echo "# Installing SysLink Shared Library in $(EXEC_DIR)/ti/syslink/lib"
	$(INSTALL_D) $(EXEC_DIR)/ti/syslink/lib
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/lib/qnx/libsyslink_g.so.1 $(EXEC_DIR)/ti/syslink/lib
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/lib/qnx/libsyslink.so.1 $(EXEC_DIR)/ti/syslink/lib

install-samples-qnx: install-samplelibs-qnx
install-samplelibs-qnx:
	@echo "#"
	@echo "# Installing Sample Shared Libraries in $(EXEC_DIR)/ti/syslink/lib"
	$(INSTALL_D) $(EXEC_DIR)/ti/syslink/lib
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/samples/hlos/common/usr/Qnx/arm/so.g.le.v7/libsyslink_common_g.so $(EXEC_DIR)/ti/syslink/lib/libsyslink_common_g.so.1
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/samples/hlos/common/usr/Qnx/arm/so.le.v7/libsyslink_common.so $(EXEC_DIR)/ti/syslink/lib/libsyslink_common.so.1
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/samples/hlos/slaveLoaderlib/usr/Qnx/arm/so.g.le.v7/libsyslink_slave_loader_g.so $(EXEC_DIR)/ti/syslink/lib/libsyslink_slave_loader_g.so.1
	$(INSTALL_M) $(SYSLINK_REPO)/ti/syslink/samples/hlos/slaveLoaderlib/usr/Qnx/arm/so.le.v7/libsyslink_slave_loader.so $(EXEC_DIR)/ti/syslink/lib/libsyslink_slave_loader.so.1

#
#  ======== clean ========
#
clean::
	@echo "#"
	@echo "# Cleaning packages..."
ifeq (Linux,$(HOSTOS))
	$(MAKE) -C ti/syslink/utils/hlos/knl/Linux \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/utils/hlos/usr/Linux \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/samples/hlos/common/usr/Linux \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoader/usr/Linux \
            SYSLINK_ROOT=$(CURDIR) clean
endif
	rm -rf $(CURDIR)/ti/syslink/obj
	rm -rf $(CURDIR)/ti/syslink/lib
	rm -rf $(CURDIR)/ti/syslink/bin

ifneq ($(wildcard $(QNX_INSTALL_DIR)),)
	$(MAKE) -C ti/syslink/utils/hlos/knl/Qnx \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/utils/hlos/usr/Qnx \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/samples/hlos/common/usr/Qnx \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoaderlib/usr/Qnx \
            SYSLINK_ROOT=$(CURDIR) clean
	$(MAKE) -C ti/syslink/samples/hlos/slaveLoader/usr/Qnx \
            SYSLINK_ROOT=$(CURDIR) clean
	rm -rf $(CURDIR)/ti/install
endif

	$(XDC) XDCBUILDCFG=$(CURDIR)/_config.bld XDCPATH="$(XDC_PATH)" \
            clean -Pr $(CURDIR)

ifeq (Linux,$(HOSTOS))
	find $(CURDIR)/ti/syslink -name ".xdcenv.mak" -exec rm {} \;
else
	cmd /C del /S /A:H .xdcenv.mak
endif
