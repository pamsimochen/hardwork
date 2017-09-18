/** 
 *  @file   GateMPSupportNull.h
 *
 *  @brief      A GateMPSupportNull instance can be used to enforce both local and remote context
 *  context protection.  That is, entering a GateMPSupportNull can prevent preemption by
 *  another thread running on the same processor and simultaneously prevent a
 *  remote processor from entering the same gate.  GateMPSupportNull's are typically used
 *  to protect reads/writes to a shared resource, such as shared memory.
 *
 *  Creating a GateMPSupportNull requires supplying the following configuration
 *      - Instance name (see #GateMPSupportNull_Params::name)
 *      - Region id (see #GateMPSupportNull_Params::regionId)
 *  In addition, the following parameters should be configured as necessary:
 *      - The level of local protection (interrupt, thread, tasklet, process
 *        or none) can be configured using the #GateMPSupportNull_Params::localProtect
 *        config parameter.
 *      - The type of remote system gate that can be used.  Most devices will
 *        typically have a single type of system gate so this configuration
 *        should typically be left alone.  See #GateMPSupportNull_Params::remoteProtect for more
 *        information.
 *  Once created GateMPSupportNull allows the gate to be opened on another processor
 *  using #GateMPSupportNull_open and the name that was used in #GateMPSupportNull_create.
 *
 *  A GateMPSupportNull can be entered and left using #GateMPSupportNull_enter} and #GateMPSupportNull_leave
 *  like any other gate that implements the IGateProvider interface.
 *
 *  GateMPSupportNull has the following proxies - RemoteSystemProxy, RemoteCustom1Proxy
 *  and RemoteCustom2Proxy which are automatically plugged with device-specific
 *  delegates that implement multiple processor mutexes using a variety of
 *  hardware mechanisms.
 *
 *  GateMPSupportNull creates a default system gate whose handle may be obtained
 *  using #GateMPSupportNull_getDefaultRemote.  Most IPC modules typically use this gate
 *  by default if they require gates and no instance gate is configured by the
 *  user.a
 *
 *
 *
 */
/* 
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



#ifndef ti_ipc_GateMPSupportNull__include
#define ti_ipc_GateMPSupportNull__include


#include <ti/ipc/GateMP.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    GateMPSupportNull_S_BUSY
 *  @brief  The resource is still in use  */
#define GateMPSupportNull_S_BUSY               2

/*!
 *  @def    GateMPSupportNull_S_ALREADYSETUP
 *  @brief  The module has been already setup  */
#define GateMPSupportNull_S_ALREADYSETUP       1

/*!
 *  @def    GateMPSupportNull_S_SUCCESS
 *  @brief  Operation is successful.
 */
#define GateMPSupportNull_S_SUCCESS            0

/*!
 *  @def    GateMPSupportNull_E_FAIL
 *  @brief  Generic failure.
 */
#define GateMPSupportNull_E_FAIL              -1

/*!
 *  @def    GateMPSupportNull_E_INVALIDARG
 *  @brief  Argument passed to function is invalid.
 */
#define GateMPSupportNull_E_INVALIDARG          -2

/*!
 *  @def    GateMPSupportNull_E_MEMORY
 *  @brief  Operation resulted in memory failure.
 */
#define GateMPSupportNull_E_MEMORY              -3

/*!
 *  @def    GateMPSupportNull_E_ALREADYEXISTS
 *  @brief  The specified entity already exists.
 */
#define GateMPSupportNull_E_ALREADYEXISTS       -4

/*!
 *  @def    GateMPSupportNull_E_NOTFOUND
 *  @brief  Unable to find the specified entity.
 */
#define GateMPSupportNull_E_NOTFOUND            -5

/*!
 *  @def    GateMPSupportNull_E_TIMEOUT
 *  @brief  Operation timed out.
 */
#define GateMPSupportNull_E_TIMEOUT             -6

/*!
 *  @def    GateMPSupportNull_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateMPSupportNull_E_INVALIDSTATE        -7

/*!
 *  @def    GateMPSupportNull_E_OSFAILURE
 *  @brief  A failure occurred in an OS-specific call  */
#define GateMPSupportNull_E_OSFAILURE           -8

/*!
 *  @def    GateMPSupportNull_E_RESOURCE
 *  @brief  Specified resource is not available  */
#define GateMPSupportNull_E_RESOURCE            -9

/*!
 *  @def    GateMPSupportNull_E_RESTART
 *  @brief  Operation was interrupted. Please restart the operation  */
#define GateMPSupportNull_E_RESTART             -10


/*!
 *  @brief  A set of local context protection levels
 *
 *  Each member corresponds to a specific local processor gates used for
 *  local protection.
 *
 *  For Linux users, the following are the mapping for the constants
 *      - INTERRUPT -> TODO
 *      - TASKLET   -> TODO
 *      - THREAD    -> TODO
 *      - PROCESS   -> TODO
 *
 *  For SYS/BIOS users, the following are the mappings for the constants
 *      - INTERRUPT -> GateHwi: disables interrupts
 *      - TASKLET   -> GateSwi: disables Swis (software interrupts)
 *      - THREAD    -> GateMutexPri: based on Semaphores
 *      - PROCESS   -> GateMutexPri: based on Semaphores
 */
typedef enum GateMPSupportNull_LocalProtect {
    GateMPSupportNull_LocalProtect_NONE      = 0,
    /*!< Use no local protection */

    GateMPSupportNull_LocalProtect_INTERRUPT = 1,
    /*!< Use the INTERRUPT local protection level */

    GateMPSupportNull_LocalProtect_TASKLET   = 2,
    /*!< Use the TASKLET local protection level */

    GateMPSupportNull_LocalProtect_THREAD    = 3,
    /*!< Use the THREAD local protection level */

    GateMPSupportNull_LocalProtect_PROCESS   = 4
    /*!< Use the PROCESS local protection level */

} GateMPSupportNull_LocalProtect;


/*!
 *  @brief  Type of remote Gate
 *
 *  Each member corresponds to a specific type of remote gate.
 *  Each enum value corresponds to the following remote protection levels:
 *      - NONE      -> No remote protection (the GateMPSupportNull instance will exclusively
 *                     offer local protection configured in #GateMPSupportNull_Params::localProtect
 *      - SYSTEM    -> Use the SYSTEM remote protection level (default for remote
 *                     protection
 *      - CUSTOM1   -> Use the CUSTOM1 remote protection level
 *      - CUSTOM2   -> Use the CUSTOM2 remote protection level
 */
typedef enum GateMPSupportNull_RemoteProtect {
    GateMPSupportNull_RemoteProtect_NONE     = 0,
    /*!< No remote protection (the GateMPSupportNull instance will exclusively
     *  offer local protection configured in #GateMPSupportNull_Params::localProtect)
     */

    GateMPSupportNull_RemoteProtect_SYSTEM   = 1,
    /*!< Use the SYSTEM remote protection level (default remote protection) */

    GateMPSupportNull_RemoteProtect_CUSTOM1  = 2,
    /*!< Use the CUSTOM1 remote protection level */

    GateMPSupportNull_RemoteProtect_CUSTOM2  = 3
    /*!< Use the CUSTOM2 remote protection level */

} GateMPSupportNull_RemoteProtect;

/*!
 *  @brief  GateMPSupportNull_Handle type
 */
typedef struct GateMPSupportNull_Object *GateMPSupportNull_Handle;

/*!
 *  @brief  Structure defining parameters for the GateMPSupportNull module.
 */
typedef struct GateMPSupportNull_Params {
    Bits32                          resourceId;
    UInt16                          regionId;
    Bool                            openFlag;
    Ptr                             sharedAddr;
    GateMPSupportNull_LocalProtect  localProtect;
    GateMPSupportNull_RemoteProtect remoteProtect;
} GateMPSupportNull_Params;

/* =============================================================================
 *  GateMPSupportNull Module-wide Functions
 * =============================================================================
 */
/*!
 *  @brief      Create a GateMPSupportNull instance
 *
 *  The params structure should be initialized using GateMPSupportNull_Params_init.
 *
 *  @param[in]  params      GateMPSupportNull parameters
 *
 *  @return     GateMPSupportNull Handle
 */
static inline
GateMPSupportNull_Handle GateMPSupportNull_create (
                                               IGateProvider_Handle localGate,
                                         const GateMPSupportNull_Params *params)
{
        return NULL;
}

/*!
 *  @brief      Delete a created GateMPSupportNull instance
 *
 *  @param[in,out]  handlePtr       Pointer to GateMPSupportNull handle
 *
 *  @return     GateMPSupportNull Status
 */
static inline
Int GateMPSupportNull_delete(GateMPSupportNull_Handle *handlePtr)
{
        return 0;
}

/*!
 *  @brief      Initialize a GateMPSupportNull parameters struct
 *
 *  @param[out] params      Pointer to GateMPSupportNull parameters
 *
 */
static inline
Void GateMPSupportNull_Params_init(GateMPSupportNull_Params *params)
{
}

/*!
 *  @brief      Amount of shared memory required for creation of each instance
 *
 *  @param[in]  params      Pointer to the parameters that will be used in
 *                          the create.
 *
 *  @return     Number of MAUs needed to create the instance.
 */
static inline
SizeT GateMPSupportNull_sharedMemReq(const IGateMPSupport_Params *params)
{
        return 0;
}


/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 *  @return     Number of instances configured.
 */
static inline
UInt32 GateMPSupportNull_getNumInstances(Void)
{
    return (1);
}

/*!
 *  @brief  Function to initialize the locks
 *          module.
 *
 */
static inline
Void GateMPSupportNull_locksinit(Void)
{
    /* Do nothing*/
}

/* =============================================================================
 *  GateMPSupportNull Per-instance functions
 * =============================================================================
 */

/*!
 *  @brief      Enter the GateMPSupportNull
 *
 *  @param[in]  handle      GateMPSupportNull handle
 *
 *  @return     key that must be used to leave the gate
 */
static inline IArg GateMPSupportNull_enter(GateMPSupportNull_Handle handle)
{
        return (IArg) 0;
}

/*!
 *  @brief      Leave the GateMPSupportNull
 *
 *  @param[in]  handle      GateMPSupportNull handle
 *  @param[in]  key         key returned from GateMPSupportNull_enter
 */
static inline
Void GateMPSupportNull_leave(GateMPSupportNull_Handle handle, IArg key)
{
}

static inline
Bits32 *GateMPSupportNull_getReservedMask(Void)
{
    /* This gate doesn't allow reserving resources */
    return (NULL);
}

Void GateMPSupportNull_setReserved(UInt32 lockNum)
{

}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
#endif /* ti_ipc_GateMPSupportNull__include */

