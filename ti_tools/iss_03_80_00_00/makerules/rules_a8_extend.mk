# Filename: rules_a8_extend.mk
#
# Make rules for A8 - This file has any extension of make rules for Cortex-A8 ISA
#
# This file needs to change when:
#     1. an extended rule common for A8 ISA has to be added or modified

CROSS_COMPILE = $(CODEGEN_PATH_A8)/bin/arm-none-linux-gnueabi-
OSAL_INSTALL_DIR = $(osal_PATH)

link_dep_extension: linuxdistbld syslink_usrspace

# The following is for Linuxdist package (OSAL)
linuxdistbld:
	$(ECHO) \# Building linuxdist - $@...
	$(MKDIR) -p $(OSAL_INSTALL_DIR)/packages/linuxdist/build/lib
	$(MAKE) -C $(OSAL_INSTALL_DIR)/packages/linuxdist/build \
            OSAL_INSTALL_DIR=$(OSAL_INSTALL_DIR) \
            CROSS_COMPILE=$(CROSS_COMPILE) \
            PRECONFIG=$(OSAL_INSTALL_DIR)/packages/linuxdist/preconfig/production \
            CFLAGS=-O2
	$(MKDIR) -p $(OSAL_INSTALL_DIR)/packages/linuxdist/cstubs/lib
	$(MAKE) -C $(OSAL_INSTALL_DIR)/packages/linuxdist/cstubs \
            OSAL_INSTALL_DIR=$(OSAL_INSTALL_DIR) \
            CROSS_COMPILE=$(CROSS_COMPILE) \
            CFLAGS=-O2

# The following is to build syslink HLOS user space library
syslink_usrspace:
	$(ECHO) \# Building Syslink HLOS user space library
	cd $(syslink_PATH)/ti/syslink/utils/hlos/usr/Linux && $(MAKE) \
            ARCH=arm \
            CROSS_COMPILE=$(CROSS_COMPILE) \
            TOOLCHAIN_PREFIX=$(CROSS_COMPILE) \
            SYSLINK_PLATFORM=TI81XX \
            SYSLINK_VARIANT=TI816X \
            SYSLINK_LOADER=ELF \
            SYSLINK_ROOT=$(syslink_PATH) \
            SYSLINK_PKGPATH=$(ipc_PATH)/packages

# Nothing beyond this point
