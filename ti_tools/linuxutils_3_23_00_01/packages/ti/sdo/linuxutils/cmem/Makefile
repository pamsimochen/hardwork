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

SUBDIRS = src apps

GLIBCSUBDIRS = $(addsuffix .glibc, $(SUBDIRS))
UCLIBCSUBDIRS = $(addsuffix .uclibc, $(SUBDIRS))
DEBUGSUBDIRS = $(addsuffix .debug, $(SUBDIRS))
RELEASESUBDIRS = $(addsuffix .release, $(SUBDIRS))
CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))


.PHONY: $(GLIBCSUBDIRS) $(UCLIBCSUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)

# keep glibc as the default target
glibc:	$(GLIBCSUBDIRS)

uclibc:	$(UCLIBCSUBDIRS)

all:	glibc

$(GLIBCSUBDIRS):
	@echo
	@echo Making glibc in subdirectory $(basename $@)...
	@$(MAKE) -C $(basename $@) glibc

$(UCLIBCSUBDIRS):
	@echo
	@echo Making uclibc in subdirectory $(basename $@)...
	@$(MAKE) -C $(basename $@) uclibc

bionic:
	@echo Making bionic in subdirectory src...
	@$(MAKE) -C src bionic
	@echo Making bionic in subdirectory apps...
	@$(MAKE) -C apps bionic


install: $(INSTALLSUBDIRS)

$(DEBUGSUBDIRS):
	@echo
	@echo Executing make debug in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) debug

debug:	$(DEBUGSUBDIRS)

$(RELEASESUBDIRS):
	@echo
	@echo Executing make release in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) release

release:	$(RELEASESUBDIRS)

$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) install

clean:	$(CLEANSUBDIRS)
	@rm -rf include
	@rm -rf lib

$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
#
#  @(#) ti.sdo.linuxutils.cmem; 2, 2, 0,1; 5-30-2012 11:50:07; /db/atree/library/trees/linuxutils/linuxutils-l01/src/ xlibrary

#

