#
#   Copyright (c) 2012-2013, Texas Instruments Incorporated
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
include /Source/ipnc_rdk/Rules.make

#
#  ======== products.mak ========
#

# Optional: recommended to install all dependent components in one folder.
#
DEPOT = $(TOOLS_INSTALL_DIR)


# Path to required dependencies
#
XDAIS_INSTALL_DIR       = $(xdais_PATH)
LINUXUTILS_INSTALL_DIR  = $(linuxutils_PATH)
EDMA3_LLD_INSTALL_DIR   = $(edma3lld_PATH)
XDC_INSTALL_DIR         = $(xdc_PATH)
BIOS_INSTALL_DIR        = $(bios_PATH)

# Path to various cgtools
#
ti.targets.C64P = $(CODEGEN_PATH_DSP)
ti.targets.C674 = $(CODEGEN_PATH_DSP)

ti.targets.elf.C64P = $(CODEGEN_PATH_DSP)
ti.targets.elf.C64T = $(CODEGEN_PATH_DSP)
ti.targets.elf.C66 = $(CODEGEN_PATH_DSP)
ti.targets.elf.C66_big_endian ?=
ti.targets.elf.C674 = $(CODEGEN_PATH_DSP)

ti.targets.arm.elf.M3 = $(CODEGEN_PATH_M3)
ti.targets.arm.elf.M4 = $(CODEGEN_PATH_M3)

