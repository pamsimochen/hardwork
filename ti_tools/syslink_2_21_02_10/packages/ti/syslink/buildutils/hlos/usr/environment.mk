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

# ---------------------------------------------------------------------------- #
# Enviornment flags                                                            #
# ---------------------------------------------------------------------------- #

# Platform defines
ifeq ("$(SYSLINK_PLATFORM)", "")
# Default value
SYSLINK_PLATFORM := TI81XX
endif # ifeq ("$(SYSLINK_PLATFORM)", "")

ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")
ifeq ("$(SYSLINK_VARIANT)", "")
#Default value
SYSLINK_VARIANT := TI816X
endif # ifeq ("$(SYSLINK_VARIANT)", "")
endif # ifeq ("$(SYSLINK_PLATFORM)", "TI81XX")

OBJSUFFIX := o
LIBSUFFIX := a
EXES_SUBDIR	:=

SYSLINK_LIB      := syslink.$(OBJSUFFIX)
SYSLINK_ARCHIVE  := syslink.$(LIBSUFFIX)


LIB_DIR          := $(SYSLINK_ROOT)/ti/syslink/lib

OBJS_ROOT_DIR    := $(SYSLINK_ROOT)/ti/syslink/obj
OBJS_DIR         := $(OBJS_ROOT_DIR)/.objs/usr

ifdef SYSLINK_VARIANT
SAMPLES_DIR      := $(SYSLINK_ROOT)/ti/syslink/obj/$(SYSLINK_VARIANT)/samples
SAMPLES_EXES_DIR := $(SYSLINK_ROOT)/ti/syslink/bin$(EXES_SUBDIR)/$(SYSLINK_VARIANT)/samples
else # ifdef SYSLINK_VARIANT
SAMPLES_DIR      := $(SYSLINK_ROOT)/ti/syslink/obj/$(SYSLINK_PLATFORM)/samples
SAMPLES_EXES_DIR := $(SYSLINK_ROOT)/ti/syslink/bin/$(EXES_SUBDIR)/$(SYSLINK_PLATFORM)/samples
endif # ifdef SYSLINK_VARIANT

SAMPLES_DIR_DBG  := $(SAMPLES_DIR)/debug
SAMPLES_DIR_REL  := $(SAMPLES_DIR)/release

