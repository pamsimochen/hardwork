# Where to copy the resulting executables and data to (when executing 'make
# install') in a proper file structure. This EXEC_DIR should either be visible
# from the target, or you will have to copy this (whole) directory onto the
# target filesystem.
EXEC_DIR=/opt/mv_pro_4.0/montavista/pro/devkit/arm/v5t_le/target/opt/dvevm


# The prefix to be added before the GNU compiler tools (optionally including
# path), i.e. "arm_v5t_le-" or "/opt/bin/arm_v5t_le-".

# For "kernel_org" builds
MVTOOL_PREFIX=/db/toolsrc/library/vendors2005/cs/arm/arm-2008q1-126/bin/arm-none-linux-gnueabi-

# Equivalent path for uClibc compiler tools
UCTOOL_PREFIX=/db/toolsrc/library/vendors2005/opensource/buildroot/10122007/build_arm/staging_dir/usr/bin/arm-linux-


# The directory that points to your kernel source directory. This is used
# for building the cmemk.ko kernel module, as the kernel's build system
# gets invoked. Note that this also means that the below C_FLAGS etc. will
# be ignored when building this particular module.

# For TI814X 2.6.37 Linux kernel
#LINUXKERNEL_INSTALL_DIR=/db/toolsrc/library/vendors2005/kernel_org/arm/TI814X-LINUX-PSP-04.01.00.03/src/kernel/linux-04.01.00.03

# For TI816X 2.6.37 Linux kernel
#LINUXKERNEL_INSTALL_DIR=/db/toolsrc/library/vendors2005/kernel_org/arm/TI816X-LINUX-PSP-04.00.00.10/src/kernel/linux-04.00.00.10

# For OMAP3530 2.6.37 Linux kernel
LINUXKERNEL_INSTALL_DIR=/db/toolsrc/library/vendors2005/kernel_org/arm/AM35x-OMAP35x-LINUX-PSP-04.02.00.07/src/kernel/linux-04.02.00.07

#
# The following are kept around for TI developer convenience ...
#
# For OMAPL138 (Freon) 2.6.29 Linux kernel
#LINUXKERNEL_INSTALL_DIR=/db/toolsrc/library/vendors2005/kernel_org/arm/KERNEL_03.20.00.01/DaVinci-PSP-SDK-03.20.00.01/da850/linux-03.20.00.01

# For OMAP3530 2.6.31 Linux kernel
#LINUXKERNEL_INSTALL_DIR=/db/toolsrc/library/vendors2005/kernel_org/arm/KERNEL_03.00.00.03_enable-iommu-for-iva2/AM35x-OMAP35x-PSP-SDK-03.00.00.03/OMAP35x/linux-03.00.00.03


# If we're a subcomponent, allow larger product settings to override our
# defaults.  The variable $(RULES_MAKE) can be overridden on the 'make'
# command-line if the location below is not correct (for example, when this
# module is not part of an overlying DVSDK).
RULES_MAKE := ../../../../../../../../Rules.make
-include $(RULES_MAKE)


# Whether or not to use the udev pseudo filesystem to generate the /dev
# filesystem for cmem. This depends on your kernel configuration. If udev is
# not used, the /dev/cmem file has to be created manually using mknod.
USE_UDEV=1

