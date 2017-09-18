###############################################################################
# Unified Dynamic Loader Reference Implementation Makefile Settings
###############################################################################

###############################################################################
# Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
#
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the
# distribution.
#
# Neither the name of Texas Instruments Incorporated nor the names of
# its contributors may be used to endorse or promote products derived
# from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################

###############################################################################
# The following variables must be set for the tool makefile to work correctly
# (although they can be set to nothing):
#
# TARG_VPATH             - Modules/subdirectories specific to target
# TARG_SRCFILES          - Additional files used by this target (.c .h)
# TARG_TOOL_NAME_PRFX    - Prefix for the tool name
# TARG_UNIT_TEST_FILES   - Additional files used for unit testing by this 
#                          target
#
##############################################################################

TARG_TOOL_NAME_PRFX    = dlti
TARG_VPATH             = C60_DLOAD_REL C60_DLOAD_DYN \
                         TMS470_DLOAD_REL TMS470_DLOAD_DYN
TARG_SRCFILES          = virtual_targets.h \
                         c60_reloc.c c60_elf32.h \
                         c60_dynamic.h c60_dynamic.c \
			 arm_reloc.c arm_elf32.h \
                         arm_dynamic.h arm_dynamic.c 
TARG_UNIT_TEST_FILES   = test_c60_reloc.h test_c60_reloc.cpp \
			 test_arm_reloc.h test_arm_reloc.cpp	

TARG_OPTS              = -DARM_TARGET -DC60_TARGET 

# Choose which target compiler we want to use to build the unified loader with.
# Can only choose one, but should be able to load any dynamic object file that
# is configured to be supported (ARM, C6x, etc.)
# Preferred method is to define this from the "gmake" command via 
# "TARGET_CC=ARM" or "TARGET_CC=C60"
#TARGET_CC                 = ARM 
TARGET_CC                 = C60


ifeq ($(TARGET_CC),C60)
CC=cl6x
else
CC=cl470
endif


##############################################################################
# Generic Shared Makefile
##############################################################################
include ./Makefile.gen
