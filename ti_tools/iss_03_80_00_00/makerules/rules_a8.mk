# Filename: rules_a8.mk
#
# Make rules for A8 - This file has all the common rules and defines required
#                     for Cortex-A8 ISA
#
# This file needs to change when:
#     1. Code generation tool chain changes (currently it uses CodeSourcery)
#     2. Internal switches (which are normally not touched) has to change
#     3. XDC specific switches change
#     4. a rule common for A8 ISA has to be added or modified

# Set compiler/archiver/linker commands and include paths
CODEGEN_INCLUDE = $(CODEGEN_PATH_A8)
CC = $(CODEGEN_PATH_A8)/bin/arm-none-linux-gnueabi-gcc
AR = $(CODEGEN_PATH_A8)/bin/arm-none-linux-gnueabi-ar
LNK = $(CODEGEN_PATH_A8)/bin/arm-none-linux-gnueabi-ld

# Internal CFLAGS - normally doesn't change
CFLAGS_INTERNAL = -fPIC -fno-strict-aliasing -MD -MF $(DEPFILE).P -march=armv5t -Dfar= -D_DEBUG_=1 -DMULTICHANNEL_OPT=1
CFLAGS_DIROPTS =

# XDC specific CFLAGS
CFLAGS_XDCINTERNAL = -Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h  -Dxdc_bld__profile_$(PROFILE_$(CORE))  -Dxdc_bld__vers_1_0_4_3_3
LNKFLAGS_INTERNAL_PROFILE =

# Following 'if...' block is for an application; to add a #define for each
#   component in the build. This is required to know - at compile time - which
#   components are on which core.
ifndef MODULE_NAME
  # Derive list of all packages from each of the components needed by the app
  PKG_LIST_LOCAL = $(foreach COMP,$(COMP_LIST_$(CORE)),$($(COMP)_PKG_LIST))

  # Defines for the app and cfg source code to know which components/packages
  # are included in the build for the local CORE...
  CFLAGS_APP_DEFINES = $(foreach PKG,$(PKG_LIST_LOCAL),-D_LOCAL_$(PKG)_)
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_LOCAL),-D_BUILD_$(PKG)_)

  ifeq ($(CORE),m3vpss)
    PKG_LIST_REMOTE = $(foreach COMP,$(COMP_LIST_m3video),$($(COMP)_PKG_LIST))
    CFLAGS_APP_DEFINES += -D_LOCAL_CORE_m3vpss_
  endif
  ifeq ($(CORE),m3video)
    PKG_LIST_REMOTE = $(foreach COMP,$(COMP_LIST_m3vpss),$($(COMP)_PKG_LIST))
    CFLAGS_APP_DEFINES += -D_LOCAL_CORE_m3video_
  endif
  ifeq ($(CORE),a8host)
    PKG_LIST_REMOTE = $(foreach COMP,$(COMP_LIST_m3video),$($(COMP)_PKG_LIST))
    PKG_LIST_REMOTE += $(foreach COMP,$(COMP_LIST_m3vpss),$($(COMP)_PKG_LIST))
    CFLAGS_APP_DEFINES += -D_LOCAL_CORE_a8host_
  endif

  # Defines for the app and cfg source code to know which components/packages
  # are included in the build for the remote CORE...
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_REMOTE),-D_REMOTE_$(PKG)_)
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_REMOTE),-D_BUILD_$(PKG)_)
endif

# Assemble CFLAGS from all other CFLAGS definitions
_CFLAGS = $(CFLAGS_GLOBAL_$(CORE)) $(CFLAGS_INTERNAL) $(CFLAGS_XDCINTERNAL) $(CFLAGS_LOCAL_COMMON) $(CFLAGS_LOCAL_$(CORE)) $(CFLAGS_LOCAL_$(PLATFORM)) $(CFLAGS_LOCAL_$(SOC)) $(CFLAGS_APP_DEFINES) $(CFLAGS_COMP_COMMON) $(CFLAGS_GLOBAL_$(PLATFORM))

# Add some necessary INCLUDES from CodeSourcery
INCLUDES += -I$(CODEGEN_PATH_A8)/arm-none-linux-gnueabi/libc/usr/include

# Object file creation
# The first $(CC) generates the dependency make files for each of the objects
# The second $(CC) compiles the source to generate object
$(OBJ_PATHS): $(OBJDIR)/%.$(OBJEXT): %.c
	$(ECHO) \# Compiling $< to $@ ...
	$(CC) $(_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -o $(OBJDIR)/$(basename $(notdir $<)).$(OBJEXT) $<

# Archive flags - normally doesn't change
ARFLAGS = cr

# Archive/library file creation
$(LIBDIR)/$(MODULE_NAME).$(LIBEXT) : $(OBJ_PATHS)
	$(ECHO) \#
	$(ECHO) \# Archiving $(OBJ_PATHS) into $@...
	$(ECHO) \#
	$(AR) $(ARFLAGS) $@ $(OBJ_PATHS)

# Linker options and rules
LNKFLAGS_INTERNAL_COMMON =  -lpthread -lrt -L$(CODEGEN_PATH_A8)/arm-none-linux-gnueabi/lib

# Assemble Linker flags from all other LNKFLAGS definitions
_LNKFLAGS = $(LNKFLAGS_INTERNAL_COMMON) $(LNKFLAGS_INTERNAL_PROFILE) $(LNKFLAGS_GLOBAL_$(CORE)) $(LNKFLAGS_LOCAL_COMMON) $(LNKFLAGS_LOCAL_$(CORE))

# Path of the RTS library - normally doesn't change for a given tool-chain
RTSLIB_PATH =
LIB_PATHS +=

LNK_LIBS = $(addprefix -l,$(LIB_PATHS))
# Linker - to create executable file
$(BINDIR)/$(APP_NAME)_$(CORE)_$(PROFILE_$(CORE)).$(EXEEXT) : $(OBJ_PATHS) $(LIB_PATHS) link_dep_extension $(LNKCMD_FILE)
	$(ECHO) \# Linking into $@
	$(ECHO) \#
#	$(CC) $(OBJ_PATHS) $(OBJDIR)/$(CFG_COBJ_XDC) $(LIB_PATHS) $(LNKCMD_FILE) $(_LNKFLAGS) -o $@
	$(CC) $(OBJ_PATHS) $(LIB_PATHS) $(EXTLIBS_a8host) $(_LNKFLAGS) -o $@
	$(ECHO) \#
	$(ECHO) \# $@ created.
	$(ECHO) \#

# Blank link dependent step extension, in case nothing is defined by rules extend
#  include file
link_dep_extension:

# XDC specific - assemble XDC-Configuro command
CONFIGURO_CMD = $(xdc_PATH)/xs xdc.tools.configuro --generationOnly -o $(CONFIGURO_DIR) -t $(TARGET_XDC) -p $(PLATFORM_XDC) \
               --cfgArgs $(CFGARGS_XDC) -r $(PROFILE_$(CORE)) -b $(CONFIG_BLD_XDC_$(ISA)) $(XDC_CFG_FILE_NAME)
_XDC_GREP_STRING = \"$(XDC_GREP_STRING)\"
EGREP_CMD = $(EGREP) -ivw $(XDC_GREP_STRING) $(XDCLNKCMD_FILE)

# Invoke configuro for the rest of the components
#  NOTE: 1. String handling is having issues with various make versions when the
#           cammand is directly tried to be given below. Hence, as a work-around,
#           the command is re-directed to a file (shell or batch file) and then
#           executed
#        2. The linker.cmd file generated, includes the libraries generated by
#           XDC. An egrep to search for these and omit in the .cmd file is added
#           after configuro is done
#$(CFG_CFILE_XDC) : $(XDC_CFG_FILE)
xdc_configuro : $(XDC_CFG_FILE)
	$(ECHO) \# Invoking configuro...
	$(ECHO) -e $(CONFIGURO_CMD) > $(DEST_ROOT)/maketemp_configuro_cmd_$(CORE).bat
	$(CHMOD) a+x $(DEST_ROOT)/maketemp_configuro_cmd_$(CORE).bat
	$(DEST_ROOT)/maketemp_configuro_cmd_$(CORE).bat
#	$(CP) $(XDCLNKCMD_FILE) $(LNKCMD_FILE)
#	$(ECHO) @ $(EGREP_CMD) > maketemp_egrep_cmd.bat
#	./maketemp_egrep_cmd.bat | $(CYGWINPATH)/bin/tail -n+3 > $(LNKCMD_FILE)
	$(EGREP_CMD) > $(LNKCMD_FILE)
#	$(EGREP) -iv "$(XDC_GREP_STRING)" $(XDCLNKCMD_FILE) > $(LNKCMD_FILE)
	$(ECHO) \# Configuro done!

$(LNKCMD_FILE) :
#	$(CP) $(XDCLNKCMD_FILE) $(LNKCMD_FILE)
#	$(ECHO) @ $(EGREP_CMD) > maketemp_egrep_cmd.bat
#	./maketemp_egrep_cmd.bat | $(CYGWINPATH)/bin/tail -n+3 > $(LNKCMD_FILE)
#	$(EGREP_CMD) > $(LNKCMD_FILE)

# Include rules extension make file
-include $(ROOTDIR)/makerules/rules_a8_extend.mk

# Include dependency make files that were generated by $(CC)
-include $(SRCS:%.c=$(DEPDIR)/%.P)

# Nothing beyond this point
