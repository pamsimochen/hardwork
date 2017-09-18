#
#  ======== makefile ========
#

include ../products.mak
UNZIP = $(XDC_INSTALL_DIR)/bin/unzip
EXAMPLES = $(wildcard ex??_*)
EXEC_DIR = $(CURDIR)/install

# determine which shell make is using
ifeq ("$(SHELL)","sh.exe")
    # use Windows commands
    MKSH = CMD
else
    # use Linux commands
    MKSH = SH
endif

# determine which example archive to use
ifeq ("$(DEVICE)","OMAPL1XX")
    EXDEV = OMAPL138
else
    EXDEV = $(DEVICE)
endif

ifeq ("$(MKSH)","SH")
    EXGPPOS := $(shell echo $(GPPOS) | tr A-Z a-z)
else
    EXGPPOS = $(GPPOS)
endif

ifeq ("$(LOADER)","COFF")
    EXLDR = coff
else
    EXLDR = elf
endif

ifeq ("$(SDK)","NONE")
    EXSDK =
else
ifeq ("$(MKSH)","SH")
    EXSDK := _$(shell echo $(SDK) | tr A-Z a-z)
else
    EXSDK = _$(SDK)
endif
endif

ARCHIVES = $(wildcard archive/$(EXDEV)_$(EXGPPOS)_$(EXLDR)$(EXSDK)/ex??_*)


all:
	@echo "#"
	@echo "# Making all examples..."
ifeq ("$(MKSH)","SH")
	@for d in $(EXAMPLES); do $(MAKE) -C $$d all; done
else
	@if "$(EXAMPLES)"=="" ( \
            echo "There are no examples to build." \
        ) else ( \
            @for %%d in ($(EXAMPLES)) do $(MAKE) -C %%d all \
        )
endif

help:
	@echo "make all        # build all examples"
	@echo "make clean      # clean all examples (does not delete them)"
	@echo "make clobber    # delete all example"
	@echo "make extract    # unzip examples from archive folder"
	@echo "make install    # install examples onto target file system"
	@echo ""
	@echo "Use the EXEC_DIR variable to override install folder"
	@echo "make EXEC_DIR=/tmp install"
	@echo ""
	@echo "Use -n option to make for dry run"
	@echo "make -n all"
	@echo ""
	@echo "The extract goal will unzip the appropriate examples based"
	@echo "on your settings in the products.mak file. Note: unzip will"
	@echo "prompt before overwritting existing files."
	@echo ""

clean::
	@echo "#"
	@echo "# Cleaning all examples..."
ifeq ("$(MKSH)","SH")
	@for d in $(EXAMPLES); do $(MAKE) -C $$d clean; done
else
	@if not "$(EXAMPLES)"=="" \
	@for %%d in ($(EXAMPLES)) do $(MAKE) -C %%d clean
endif

clobber::
	@echo "#"
	@echo "# Deleting all examples..."
ifeq ("$(MKSH)","SH")
	@for d in $(EXAMPLES); do rm -rf $$d; done
else
	@if not "$(EXAMPLES)"=="" \
	@for %%d in ($(EXAMPLES)) do rmdir /s /q %%d
endif

# Note: the unzip command will prompt user when target file exists.
extract:
	@echo "#"
	@echo "# Unzipping all examples..."
ifeq ("$(MKSH)","SH")
	@for f in $(ARCHIVES); do $(UNZIP) $$f; done
else
	@if "$(ARCHIVES)"=="" ( \
            echo "No appropriate examples found in archive folder." \
        ) else ( \
            @for %%f in ($(ARCHIVES)) do $(UNZIP) %%f \
        )
endif

install:
	@echo "#"
	@echo "# Installing examples into $(EXEC_DIR)"
ifeq ("$(MKSH)","SH")
	@for d in $(EXAMPLES); do \
             $(MAKE) -C $$d install EXEC_DIR=$(EXEC_DIR); \
         done
else
	@for %%d in ($(EXAMPLES)) do \
             $(MAKE) -C %%d install EXEC_DIR=$(EXEC_DIR)
endif
	@$(INSTALL_M) scripts/runall.sh $(EXEC_DIR)


# portable install commands
ifneq ($(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe),)
    INSTALL_M = $(XDC_INSTALL_DIR)/bin/cp.exe
else
    INSTALL_M = install -m 0755
endif

