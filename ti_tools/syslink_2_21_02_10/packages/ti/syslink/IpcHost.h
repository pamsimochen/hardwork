/** 
 *  @file   IpcHost.h
 *
 *  @brief  This file contains APIs and defines that are used by applications
 *          only on the host-side of the device.
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



#ifndef _IPCHOST_H__
#define _IPCHOST_H__


#if defined (__cplusplus)
extern "C" {
#endif

/**
 *  @file       IpcHost.h
 *
 *  @remarks    The typical flow for an IPC-using application is to first
 *              load the processor (typically via slaveloader or the ProcMgr
 *              APIs), and then use the Ipc_control() API to attach/detach
 *              from the running slave.
 *
 *  @par Example:
 *
 *  @code
 *      // invoke the SysLink load callback
 *      remoteProcId = MultiProc_getId("DSP");
 *      status = Ipc_control(remoteProcId, Ipc_CONTROLCMD_LOADCALLBACK, NULL);
 *
 *      // invoke the SysLink start callback
 *      status = Ipc_control(remoteProcId, Ipc_CONTROLCMD_STARTCALLBACK, NULL);
 *
 *      // IPC is established, app-specific code is here
 *
 *      // invoke the SysLink stop callback
 *      status = Ipc_control(remoteProcId, Ipc_CONTROLCMD_STOPCALLBACK, NULL);
 *  @endcode
 */

/**
 *  @brief      Use the stop terminate policy
 *
 *  When the host process termiantes (e.g. CTRL-C, SIGKILL), this
 *  terminate policy will stop the remote processor and place it
 *  into reset. The terminate action is carried out by the SysLink
 *  driver on behalf of the terminated process.
 */
#define Ipc_TERMINATEPOLICY_STOP 1


/**
 *  @brief      Use the notify terminate policy
 *
 *  When the host process termiantes (e.g. CTRL-C, SIGKILL), this
 *  terminate policy will send a notify event to the remote processor.
 *  The lineId and eventId are determined by the remote executable
 *  through configuration parameters (See ti/syslink/ipc/rtos/SysLink.xdc).
 */
#define Ipc_TERMINATEPOLICY_NOTIFY 2


/**
 *  @brief      Argument for the Ipc_CONTROLCMD_SETTERMINATEPOLICY command
 *
 *  Initialize a structure of this type with the desired terminate
 *  policy. Pass the address of this structure as the @c arg parameter
 *  to Ipc_control() along with this command type. This must be done before
 *  calling the load callback.
 *
 *  @par Example:
 *
 *  @code
 *      Ipc_Terminate terminateConfig;
 *
 *      // set the requested terminate policy
 *      terminateConfig.policy = Ipc_TERMINATEPOLICY_STOP;
 *      Ipc_control(procId, Ipc_CONTROLCMD_SETTERMINATEPOLICY,
 *              (Ptr)(&terminateConfig));
 *
 *      // now invoke the load callback
 *      Ipc_control(procId, Ipc_CONTROLCMD_LOADCALLBACK, NULL);
 *  @endcode
 *
 *  @sa Ipc_TERMINATEPOLICY_STOP
 *  @sa Ipc_TERMINATEPOLICY_NOTIFY
 */
typedef struct {
    Int policy;                 /**< terminate policy */
} Ipc_Terminate;


/**
 *  @brief      Control command ID for load callback.
 *
 *  This command must be sent (via Ipc_control()) before using IPC
 *  with the slave, and after loading the slave.
 *
 *  This call ensures resources (e.g. Shared Region 0) are visible
 *  to the slave (for example, by enabling the slave-side MMU).
 *  This call then typically enables the slave-side call to
 *  Ipc_start() to succeed.
 *
 *  When ProcMgr is used to load the slave (the typical case),
 *  the @c arg passed to Ipc_control() should be NULL.
 *
 *  When ProcMgr is <i>not</i> used to load the slave (supported,
 *  but not the typical case), the @c arg passed to Ipc_control()
 *  must be a pointer to the slave-side address of the
 *  @c _Ipc_ResetVector symbol.  This is often obtained by
 *  inspecting the slave-side executable's .map file.
 *
 *  @sa         Ipc_control()
 */
#define Ipc_CONTROLCMD_LOADCALLBACK  (0xBABE0000)


/**
 *  @brief      Control command ID for start callback.
 *
 *  This command must be sent (via Ipc_control()) before using IPC
 *  with the slave, and after sending the
 *  #Ipc_CONTROLCMD_LOADCALLBACK command.
 *
 *  This call performs a handshake with the slave processor,
 *  typically enabling the slave-side call to Ipc_attach() to
 *  succeed.
 *
 *  When sending this command, the @c arg passed to Ipc_control()
 *  should be NULL.
 *
 *  @sa         Ipc_control()
 */
#define Ipc_CONTROLCMD_STARTCALLBACK (0xBABE0001)


/**
 *  @brief      Control command ID for stop callback.
 *
 *  This command must be sent (via Ipc_control()) to detach from
 *  an attached slave.
 *
 *  This call first performs a handshake with the slave processor,
 *  typically enabling the slave-side calls to Ipc_detach() and
 *  Ipc_stop() to succeed.  It then may make resources
 *  (e.g. Shared Region 0) inaccessible to the slave (for example
 *  by disabling the slave's MMU).
 *
 *  When ProcMgr is used to load the slave (the typical case),
 *  the @c arg passed to Ipc_control() should be NULL.
 *
 *  When ProcMgr is <i>not</i> used to load the slave (supported,
 *  but not the typical case), the @c arg passed to Ipc_control()
 *  must be a pointer to the slave-side address of the
 *  @c _Ipc_ResetVector symbol.  This is often obtained by
 *  inspecting the slave-side executable's .map file.
 *
 *  @sa         Ipc_control()
 */
#define Ipc_CONTROLCMD_STOPCALLBACK  (0xBABE0002)


/**
 *  @brief      Control command ID for specifying the terminate handling policy
 *
 *  Specify the terminate policy to be used by SysLink in the
 *  event that the application terminates. The SysLink driver
 *  will perform the requested terminate actions on behalf of
 *  the terminated application. This is typically used to handle
 *  cleanup of resources.
 *
 *  The @c arg passed to Ipc_control() must be pointer to an
 *  Ipc_Terminate structure.
 *
 *  @sa         Ipc_control()
 */
#define Ipc_CONTROLCMD_SETTERMINATEPOLICY  (0xBABE0003)


/**
 *  @brief      Perform IPC-related control operations with a specific slave.
 *
 *  @param[in]  procId  Remote processor ID
 *  @param[in]  cmdId   Command ID
 *  @param[in]  arg     Argument.  The value of @c arg depends on the command
 *                      being sent in @c cmdId.
 *
 *  @remarks    Each specific @c cmdId describes its usage of the @c arg param.
 *
 *  @sa         Ipc_CONTROLCMD_LOADCALLBACK
 *  @sa         Ipc_CONTROLCMD_STARTCALLBACK
 *  @sa         Ipc_CONTROLCMD_STOPCALLBACK
 *  @sa         Ipc_CONTROLCMD_SETTERMINATEPOLICY
 */
Int Ipc_control (UInt16 procId, Int32 cmdId, Ptr arg);

/**
 *  @brief      Return a runtime id for the given application key
 *
 *  When the application calls SysLink_setup(), there is an internal
 *  runtime id assigned to the application's process id. Use this API
 *  to retrieve the assigned runtime id. This API may be called multiple
 *  times but will always return the same runtime id for the current
 *  process id. This ensures that all levels of the software stack will be
 *  assigned the same runtime id.
 *
 *  There are 255 available runtime ids: 1 - 255. Runtime ids are allocated
 *  by looking for the next larger unused runtime id. The search will wrap
 *  around as needed.
 *
 *  @param[out] rtid    Remote processor ID
 *
 *  @retval     Ipc_S_SUCCESS           Operation succeeded
 *  @retval     Ipc_E_FAIL              No more runtime ids available
 */
Int Ipc_runtimeId(UInt8 *rtid);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _IPCHOST_H__ */
