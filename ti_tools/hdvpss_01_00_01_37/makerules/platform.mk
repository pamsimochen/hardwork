# Filename: platforms.mk
#
# Platforms make file - Platform/SoC/targets are defined/derived in this file
#
# This file needs to change when:
#     1. a new platform/SoC is added, which also might have its own cores/ISAs
#

#
# Derive SOC from PLATFORM
#

# ti816x (Netra) catalog EVM
ifeq ($(PLATFORM),ti816x-evm)
 SOC = ti816x
endif

# ti816x (Netra) VC EVM
ifeq ($(PLATFORM),ti816x-evm-vc)
 SOC = ti816x
endif

# ti816x (Netra) VS EVM
ifeq ($(PLATFORM),ti816x-evm-vs)
 SOC = ti816x
endif

# ti816x (Netra) Simulator
ifeq ($(PLATFORM),ti816x-sim)
 SOC = ti816x
endif

# ti814x (Centaurus) Catalog EVM
ifeq ($(PLATFORM),ti814x-evm)
 SOC = ti814x
endif

# ti814x (Centaurus) Simulator
ifeq ($(PLATFORM),ti814x-sim)
 SOC = ti814x
endif

# ti8107 (DM385) EVM
ifeq ($(PLATFORM),ti8107-evm)
 SOC = ti8107
endif


# Derive Target/ISA from CORE

# m3vpss
ifeq ($(CORE),m3vpss)
 ISA = m3
endif

# m3video
ifeq ($(CORE),m3video)
 ISA = m3
endif

# a8host
ifeq ($(CORE),a8host)
 ISA = a8
endif

# DSP ; To do: change ISA to more meaningful value (c64x, c674x, etc)
ifeq ($(CORE),dsp)
 ISA = dsp
endif

#
# Derive XDC/ISA specific settings
#

ifeq ($(ISA),m3)
  ifeq ($(FORMAT),ELF)
    TARGET_XDC = ti.targets.arm.elf.M3
    FORMAT_EXT = e
  else
    TARGET_XDC = ti.targets.arm.M3
  endif
  ifeq ($(SOC),ti816x)
    ifeq ($(CORE),m3video)
      PLATFORM_XDC = "ti.platforms.evmTI816X:core0"
    else
      PLATFORM_XDC = "ti.platforms.evmTI816X:core1"
    endif
  endif

  ifeq ($(SOC),ti814x)
    ifeq ($(CORE),m3video)
      PLATFORM_XDC = "ti.platforms.evmDM8148:core0"
    else
      PLATFORM_XDC = "ti.platforms.evmDM8148:core1"
    endif
  endif

  ifeq ($(SOC),ti8107)
    ifeq ($(CORE),m3video)
      PLATFORM_XDC = "ti.platforms.evmDM8148:core0"
    else
      PLATFORM_XDC = "ti.platforms.evmDM8148:core1"
    endif
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
endif

ifeq ($(ISA),a8)
  TARGET_XDC = gnu.targets.arm.GCArmv5T
  PLATFORM_XDC = "host.platforms.arm"

  # Define the file extensions
  OBJEXT = ov5T
  LIBEXT = av5T
  EXEEXT = xv5T
  ASMEXT = sv5T
endif

ifeq (sim,$(findstring sim,$(PLATFORM)))
  OMX_PLATFORM = sim
endif

ifeq (ti816x-evm,$(findstring ti816x-evm,$(PLATFORM)))
 # Don't know why platform define for XDC is separate for A8. TO DO.
 ifeq ($(CORE),a8host)
  OMX_PLATFORM = evmSi
 else
  OMX_PLATFORM = evmSi816X
 endif
endif

ifeq ($(CORE),m3vpss)
  CFGARGS_XDC = \"{mode: \\\"$(IPC_MODE)\\\", coreName: \\\"VPSS-M3\\\", tiler: \\\"DucatiTilerMemMgr\\\", platformMem: \\\"$(PLATFORM_MEM)\\\", maxResolution: \\\"$(MAX_RESOLUTION)\\\", omxBuildPlatform: \\\"$(OMX_PLATFORM)\\\"}\"
endif
ifeq ($(CORE),m3video)
  CFGARGS_XDC = \"{mode: \\\"$(IPC_MODE)\\\", coreName: \\\"VIDEO-M3\\\", tiler: \\\"DucatiTilerMemMgr\\\", platformMem: \\\"$(PLATFORM_MEM)\\\", maxResolution: \\\"$(MAX_RESOLUTION)\\\", omxBuildPlatform: \\\"$(OMX_PLATFORM)\\\"}\"
endif
ifeq ($(CORE),a8host)
  CFGARGS_XDC = \"{mode: \\\"$(IPC_MODE)\\\", coreName:\\\"HOST\\\", tiler: \\\"NullTilerMemMgr\\\", platformMem: \\\"$(PLATFORM_MEM)\\\", maxResolution: \\\"$(MAX_RESOLUTION)\\\", omxBuildPlatform: \\\"$(OMX_PLATFORM)\\\"}\"
endif

# Nothing beyond this point
