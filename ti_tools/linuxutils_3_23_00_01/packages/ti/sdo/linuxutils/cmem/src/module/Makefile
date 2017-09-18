#
#  Copyright 2012 by Texas Instruments Incorporated.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 of the License.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
#

ROOTDIR = ../..

# We conditionally include $(ROOTDIR)/Rules.make because when the
# kernel make system invokes us, it won't be found... but that's okay.
-include $(ROOTDIR)/Rules.make

TARGET = cmemk.ko

obj-y += cmemk.o

EXTRA_CFLAGS +=	-DUSE_UDEV=$(USE_UDEV) \
		-DMAX_POOLS=$(MAX_POOLS)

MAKE_ENV = ARCH=arm CROSS_COMPILE=$(MVTOOL_PREFIX)
MAKE_ENV_BIONIC = ARCH=arm CROSS_COMPILE=$(BIONICTOOL_PREFIX)

debug: EXTRA_CFLAGS += -D__DEBUG
uclibc: MAKE_ENV = ARCH=arm CROSS_COMPILE=$(UCTOOL_PREFIX)

.PHONY: clean debug release install default glibc uclibc

default: glibc
glibc: release
uclibc: release

docs:

release:
	@echo Making module release...
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

debug:
	@echo Making module debug...
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

bionic:
	@echo Making module bionic...
	make -C $(LINUXKERNEL_INSTALL_DIR) M=`pwd` $(MAKE_ENV_BIONIC) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

install:
	@echo Making module install...
	install -d $(EXEC_DIR)
	install $(TARGET) $(EXEC_DIR)

clean:
	-rm -rf *.o *mod* *.ko .cmem* .tmp*
#
#  @(#) ti.sdo.linuxutils.cmem; 2, 2, 0,1; 5-30-2012 11:50:08; /db/atree/library/trees/linuxutils/linuxutils-l01/src/ xlibrary

#

