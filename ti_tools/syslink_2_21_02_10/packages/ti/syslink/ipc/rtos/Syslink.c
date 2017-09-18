/*
 *  @file   Syslink.c
 *
 *  @brief      Logic to plug RingIO & FrameQ specific code in Ipc.
 *
 *
 *
 *  ============================================================================
 *
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
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */


#include <xdc/std.h>
#include <ti/ipc/Ipc.h>
#include <ti/sdo/ipc/_SharedRegion.h>
#include <ti/syslink/ipc/rtos/Syslink.h>

/* =============================================================================
 * Macros
 * =============================================================================
 */

/* This tag is used as an identifier by Ipc_writeConfig and Ipc_readConfig */
#define SLAVE_CONFIG_TAG 0xDADA0000


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

typedef struct Syslink_SlaveModuleConfig
{
    UInt16      sharedRegionNumEntries;
    UInt16      terminateEventLineId;
    UInt32      terminateEventId;
} Syslink_SlaveModuleConfig;


/*
 *  ======== Syslink_attach ========
 *  Syslink specific initialization for Ipc_attach() hook
 */
Int ti_syslink_ipc_rtos_Syslink_attach(UArg userObj, UInt16 remoteProcId)
{
    Int status = 0;
    Syslink_SlaveModuleConfig slaveModuleConfig;

    /* dummy reference to user object */
    (Void)userObj;

    /* make the SysLink configuration available to remote processor */
    slaveModuleConfig.sharedRegionNumEntries =
            ti_sdo_ipc_SharedRegion_numEntries;
    slaveModuleConfig.terminateEventLineId = Syslink_terminateEventLineId;
    slaveModuleConfig.terminateEventId = Syslink_terminateEventId;

    status = Ipc_writeConfig(remoteProcId, SLAVE_CONFIG_TAG,
            (Ptr)(&slaveModuleConfig), sizeof(Syslink_SlaveModuleConfig));

    if (status < 0) {
        status = Ipc_E_FAIL;
    }

    return(status);
}

/*
 *  ======== Syslink_detach ========
 */
Int ti_syslink_ipc_rtos_Syslink_detach(UArg userObj, UInt16 remoteProcId)
{
    Int         status = 0;

    /* dummy reference to user object */
    (Void)userObj;

    /* free up the memory allocated in the attach hook */
    status = Ipc_writeConfig(remoteProcId, SLAVE_CONFIG_TAG,
            (Ptr)NULL, sizeof(Syslink_SlaveModuleConfig));

    if (status < 0) {
        status = Ipc_E_FAIL;
    }

    return(status);
}
