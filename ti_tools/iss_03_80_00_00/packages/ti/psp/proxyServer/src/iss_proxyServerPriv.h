/** ==================================================================
 *  @file   iss_proxyServerPriv.h                                                  
 *                                                                    
 *  @path   /ti/psp/proxyServer/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file iss_proxyServerPriv.h
 *
 *  \brief This file defines the data types that would be used to co-relate the
 *         commands from the remote agent and FVID2 API. In addition to types,
 *         enumerations, configurations required by Proxy Server.
 *
 *         This file defines data structures that would represent FVID2 calls.
 *         These types are expected to be used by the proxy server alone.
 *         The general idea is as depicted below.
 *
 *         < Proxy Agent Command - of type ISS_ProxyServerCommands >
 *         < RESERVED - of type UInt32 >
 *         < Notification Number to be used - of type SYSLINK_NOTIFY_TYPE>
 *         < Return Value - of type UInt32 >
 *         < First parameter of FVID2 Function call >
 *         < :  >
 *         < Last parameter of FVID2 Function call >
 */

#ifndef _ISS_PROXY_SERVER_LOCALS_H
#define _ISS_PROXY_SERVER_LOCALS_H

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vpsutils_que.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* FVID2 APIs */
    /* ========================================================================== 
     */
/**
 * \internal These FVID2 APIs are externed here explicitly to ensure that changes
 *           to FVID2 API should mandate an update to this header (at minimum)
 */
    extern Int32 FVID2_init(Ptr args);

    extern Int32 FVID2_deInit(Ptr args);

    extern FVID2_Handle FVID2_create(UInt32 drvId,
                                     UInt32 instanceId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_CbParams * cbParams);

    extern Int32 FVID2_delete(FVID2_Handle handle, Ptr deleteArgs);

    extern Int32 FVID2_control(FVID2_Handle handle,
                               UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs);

    extern Int32 FVID2_queue(FVID2_Handle handle,
                             FVID2_FrameList * frameList, UInt32 streamId);

    extern Int32 FVID2_dequeue(FVID2_Handle handle,
                               FVID2_FrameList * frameList,
                               UInt32 streamId, UInt32 timeout);

    extern Int32 FVID2_processFrames(FVID2_Handle handle,
                                     FVID2_ProcessList * processList);

    extern Int32 FVID2_getProcessedFrames(FVID2_Handle handle,
                                          FVID2_ProcessList * processList,
                                          UInt32 timeout);

    /* ========================================================================== 
     */
    /* Proxy Server Configurations */
    /* ========================================================================== 
     */
#define ISS_PSRV_TASK_CTRL_Q_DEPTH     (0x0Au)
/**< Configures the depth of Q, i,e, number of notifies that this task serves */
#define ISS_PSRV_TASK_DISPLAY_Q_DEPTH  (0x0Au)
/**< Configures the depth of Q, i,e, number of notifies that this task serves */
#define ISS_PSRV_TASK_CAPTURE_Q_DEPTH  (0x0Au)
/**< Configures the depth of Q, i,e, number of notifies that this task serves */
#define ISS_PSRV_TASK_GRAPHCS_Q_DEPTH  (0x0Au)
/**< Configures the depth of Q, i,e, number of notifies that this task serves */
#define ISS_PSRV_TASK_M2M_Q_DEPTH      (0x0Au)
/**< Configures the depth of Q, i,e, number of notifies that this task serves */
#define ISS_PSRV_TASK_MAX_Q_DEPTH      (0x0Au)
/**< The maximum Q depth of all the tasks */
#define ISS_PSRV_NTFY_TASK_MAX_Q_DEPTH  (ISS_PSRV_NUMBER_OF_TASKS * \
                                         ISS_PSRV_TASK_MAX_Q_DEPTH)
/**< The maximum no of callback that could wait for the callback task to
     process the IO completion callbacks */

    /* ========================================================================== 
     */
    /* Defines that identify state of Proxy Server */
    /* ========================================================================== 
     */
#define ISS_PSRV_UNINITIALIAZED        (0x0u)
/**< state identifier - un-initialized */
#define ISS_PSRV_INITIALIAZED          (0x1u)
/**< state identifier - initialized */

    /* ========================================================================== 
     */
    /* Defines that configures tasks parameters */
    /* ========================================================================== 
     */
#define ISS_PSRV_TASK_STACK_DEPTH      (0x14336u)
/**< Defines the size of the tasks */

    /* ========================================================================== 
     */
    /* Generic Defines required by proxy server */
    /* ========================================================================== 
     */
#define ISS_PSRV_NOTIFY_OCCUPIED       (0x01u)
/**< Define used by the server to identify if a given notify number is used */
#define ISS_PSRV_NOTIFY_FREE           (0x02u)
/**< Define used by the server to identify if a given notify number is free */
#define ISS_PSRV_NTY_NO_IDX            (0x01u)
/**< Column in the 2D array that holds notification number */
#define ISS_PSRV_NTY_USED_IDX          (0x0u)
/**< Column in the 2D array that specifies if notification no is free */
#define ISS_PSRV_NTY_CORE_NO           (0x0u)
/**< Identifies the position used to store remote core number */
#define ISS_PSRV_NTY_CORE_NO_HANDLE    (0x01u)
/**< Identifies the position used to store notify handle for a remote
     core number */
#define ISS_PSRV_CONTROL_TASK          (0x01u)
/**< Define used as index into array of task desriptor. 0x0 is control task */
#define ISS_PSRV_DISPLAY_TASK          (0x02u)
/**< Define used as index into array of task desriptor. 0x0 is Display task */
#define ISS_PSRV_CAPTURE_TASK          (0x04u)
/**< Define used as index into array of task desriptor. 0x0 is Capture task */
#define ISS_PSRV_GRAPHICS_TASK         (0x08u)
/**< Define used as index into array of task desriptor. 0x0 is Graphics task */
#define ISS_PSRV_M2M_TASK              (0x10u)
/**< Define used as index into array of task desriptor. 0x0 is Memory to
     memory task */

    /* ========================================================================== 
     */
    /* Firmware version */
    /* ========================================================================== 
     */

/**
 * @brief  Command interpretation structure
 *         This structure defines the first couple of elements of command
 *         structure from the remote agent.
 *         i.e. This strucutre is essentially defines first 2 common elements
 *         of each of the command structure.Which would be used for error checks
 *         error check is. An display stream will not handle command for the
 *         graphics stream.
 */
    typedef struct ISS_PSrvFvid2CmdOverlay_t {
        ISS_PSrvCommands command;
    /**< [IN] Command from the agent */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< Return value of the FIVD2 API on error code on un-handled command */

    } ISS_PSrvFvid2CmdOverlay;

/**
 * @brief  Private data for each of the tasks.
 *         This structure defines the place holder resources used by each task.
 *         cmdQHndl will point to an array based queue
 *         tSem is used to idicate to perticular task that an remote agent has
 *             issued an request (available in the Q) and the task is to
 *             process the received request.
 *             A counting semaphore will be used and the max count will depend
 *             on the number of streams the task will process.
 *         tPriority Holds the priority of the task, reserved for future
 *         enhancement.
 */
    typedef struct ISS_PSrvTaskObject_t {
        VpsUtils_QueHandle spaceHolder4QHnld;
    /**< Space for Q handle */
        VpsUtils_QueHandle *cmdQHndl;
    /**< Handle to the queue, returned by Q create function */
        Semaphore_Handle tSem;
    /**< Semaphore that the task would pend on */
        Task_Handle taskOwnHndl;
    /**< Handle to the task */
        ISS_PSrvHostTaskType taskInstance;
    /**< Host task instance */
    } ISS_PSrvTaskObject;

/**
 * @brief  Notify Number Object, used to store the notify number and is state
 *         allocated / free.
 *
 *         Is the basic block used to construct a datastructure to holds all the
 *         notification numbers for all cores and its status. (allocated / free)
 */
    typedef struct ISS_PSrvNtyNumObject_t {
        UInt32 status;
    /**< Status of the notification number - allocated / free */
        UInt32 ntyNumber;
    /**< Notification number, supplied by apps initializing Proxy Server */
    } ISS_PSrvNtyNumObject;

/**
 * @brief  Place holder for all notification numbers of a processor.
 *
 *         Holds all the notifications numbers for a given processors with
 *         processor identifier and maximum number of notify numbers requested.
 */
    typedef struct ISS_PSrvNtyNumsPerProcInst_t {
        ISS_PSrvNtyNumObject ntyNoInst[ISS_PSRV_MAX_NO_NOTIFY];
    /**< Maximum number of notification number supported for each processor */
        UInt32 ntyNumCountMax;
    /**< Maximum number of notification number configured by application */
        UInt32 determinedProcId;
    /**< Associated processor number */
    } ISS_PSrvNtyNumsPerProcInst;

#ifdef __cplusplus
}
#endif
#endif                                                     /* _ISS_PROXY_SERVER_LOCALS_H 
                                                            */
