/*
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  @file   ti/syslink/family/hlos/knl/ti81xx/SysLink_cfg.c
 *
 *  @brief  SysLink configuration
 */

#include <ti/syslink/Std.h>

#include <ti/ipc/GateMP.h>
#include <ti/ipc/NameServer.h>

#include <ti/syslink/inc/_GateMP_qnx.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/inc/_RingIOShm.h>
#include <ti/syslink/inc/_SysLink.h>
#include <ti/syslink/inc/GateHWSpinlock_qnx.h>


GateHWSpinlock_Config GateHWSpinlock_defCfg = {
    .defaultProtection  = GateHWSpinlock_LocalProtect_INTERRUPT,
    .baseAddr           = 0x480CA800,   /* device physical address */
    .size               = 0x1000,
    .numLocks           = 64            /* max supported on netra */
};

GateMP_Config GateMP_defCfg = {
    .numResources       = 32,
    .defaultProtection  = GateMP_LocalProtect_PROCESS,
    .maxNameLen         = 32,  /* TODO */
    .maxRunTimeEntries  = NameServer_ALLOWGROWTH
};

/* RingIO module configuration */
RingIO_Config RingIO_defCfg = {
    .maxNameLen      = 32u,
    .maxInstances    = 32u
};

/* RingIO module configuration */
RingIOShm_Config RingIOShm_defCfg = {
    .notifyEventNo   = 5u
};

SysLink_ModuleCfg SysLink_modCfgAry[] = {
    {
        GateHWSpinlock_CONFIG,
        &GateHWSpinlock_defCfg,
        sizeof(GateHWSpinlock_Config)
    },
    {   GateMP_CONFIG,
        &GateMP_defCfg,
        sizeof(GateMP_Config)
    },
    {   RingIO_CONFIG,
        &RingIO_defCfg,
        sizeof(RingIO_Config)
    },
    {   RingIOShm_CONFIG,
        &RingIOShm_defCfg,
        sizeof(RingIOShm_Config)
    }
};

Int SysLink_modCfgAryLen = sizeof(SysLink_modCfgAry)/sizeof(SysLink_ModuleCfg);
