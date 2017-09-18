/** ==================================================================
 *  @file   iss_proxyServer.h                                                  
 *                                                                    
 *  @path   /ti/psp/proxyServer/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \defgroup ISS_PROXYSERVER_API Proxy Server Interface
 *
 * @{
 */

/**
 *  \file iss_proxyServer.h
 *
 *  \brief Proxy Server Interface
 *
 *         This interface file is expected to be used by remote clients/agents
 *         that require to make FVID2 calls remotely.
 *
 *         This Agent relies on TIs SYSLink for inter-processor communication
 *         Please refer the design document, that came with this release for the
 *         architecture/other dependencies of this module.
 *         Interfaces defined in this file is NOT a replacement for FVID2
 *         interfaces, these interfaces build upon FVID2 interfaces to provide
 *         a mechanism to exercise FVID2 interface remotely.
 *
 *  \par   FVID2 Interface dependency
 *         This interface builds upon FVID2 interface. User is expected to be
 *         aware of FVID2 interface.
 *
 */

#ifndef _ISS_PROXY_SERVER_H
#define _ISS_PROXY_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Proxy Server configurations defines */
    /* NOT EXPECTED TO CHANGE THESE VALUES */
    /* ========================================================================== 
     */
#define ISS_PSRV_MAX_NO_NOTIFY         (32u)
/**< Maximum number of stream supported by Proxy Server. This define
     could not be changed arbitrarily, there is dependency on the number
     of notify available with sysLink. Please refer the sysLink documentation
     for details. */
#define ISS_PSRV_MAX_NO_OF_CORES       (12u)
/**< Maximum number of cores that Proxy Server could serve.*/
#define ISS_PSRV_NUMBER_OF_TASKS       (0x5u)
/**< Used to specify the number of task that process requests from a remote
     agent / client.
     Designed to be 5. Notify management would require change if this changes */

    /* ========================================================================== 
     */
    /* Data Types required by remote all */
    /* ========================================================================== 
     */

/**
 * @brief  This enumeration identifies the types of callbacks.
 *
 *         On completion of an IO request OR on encountering an error, the proxy
 *         Server would notify the remote agents / clients via an previously
 *         allocate notification number during FVID2 stream creation.
 *
 *         Since notification happens on ONE notification channel Remote agents
 *         / clients could use this enumeration to differentiate between an
 *         error or IO completion notifications.
 */
    typedef enum {
        ISS_FVID2_IO_CALLBACK = (0x01u),
    /**< Specifies that callback is for IO completion */
        ISS_FVID2_ERR_CALLBACK = (0x02u)
    /**< Specifies that the callback is for Error */
    } ISS_PSrvCallbackType;

/**
 * @brief  Place holder for IO Request Callback
 *
 *         Remote applications requires to be notified on completion of an IO
 *         request on HDISSS sub-system. The structure defined here is an place
 *         holder that requires to be allocated by the remote agent / client,
 *         and passed to the proxy server. Proxy Server would update and
 *         update / maintain this structure.
 *
 */

    typedef struct {
        UInt32 cbType;
    /**< [OUT] Remote agents / clients could use this to determine if the
               current notification is for IO completion event or error event.
               #ISS_PSrvCallbackType list the possible values that this could
               take. */
        UInt32 sysLnkNtyNo;
    /**< [Reserved] for remote agent, expected to be used by Proxy Server.
        Remote agents are expected not to alter this */
        UInt32 sysLnkProcId;
    /**< [Reserved] for the remote agent, expected to be used by Proxy Server.
        Remote agents are expected not to alter this */
        UInt32 callbackRtnValue;
    /**< [Reserved] Reserved for now, to be used if return values requires to
                   be handled in the HDISSS sub-system */
        FVID2_Handle fvid2Handle;
    /**< [OUT] Handle to HDISSS. Remote clients/agents should not alter this.*/
        FVID2_CbFxn appCallBack;
    /**< [OUT] Callback provided by the apps running on remote processor.
        Expected to be used by the remote agent to notify apps - left to remote
        agent to determine the use.
        Proxy Server will not alter this. */
        Ptr appData;
    /**< [OUT] Argument for the apps provided callback on the remote agent.
        Proxy Server will not alter this. */
        Ptr reserved;
    /**< [Reserved] Future requirements/enhancements if any */
    } ISS_PSrvCallback;

/**
 * @brief  Place holder for Error Callback
 *
 *         Remote applications requires to be notified on encountering an errors
 *         in the stream.
 *         The structure defined here is an place holder that requires to be
 *         allocated by the remote agent/client, and passed to the proxy server
 *         Proxy Server would update and update / maintain this structure.
 */

    typedef struct {
        UInt32 cbType;
    /**< [OUT] Remote agents / clients could use this to determine if the
               current notification is for IO completion event or error event.
               #ISS_PSrvCallbackType list the possible values that this could
               take. */
        UInt32 sysLnkNtyNo;
    /**< [Reserved] for remote agent expected to be used by Proxy Server.
        Agents are expected not alter this. */
        UInt32 sysLnkProcId;
    /**< [Reserved] for the remote agent, expected to be used by Proxy Server.
        Remote agents are expected not to alter this */
        UInt32 callbackRtnValue;
    /**< [Reserved] Reserved for now, to be used if return values requires to
                   be handled in the HDISSS sub-system */
        FVID2_Handle fvid2Handle;
    /**< [OUT] Handle to HDISSS. Remote clients/agents should not alter this.*/
        FVID2_ErrCbFxn appCallBack;
    /**< [OUT] Error Callback provided by the apps running on remote processor.
        Expected to be used by the remote agent to notify apps - left to remote
        agent to determine the use.
        Proxy Server will not alter this. */
        Ptr errList;
    /**< [OUT] Argument for the apps provided callback on the remote agent.
        Proxy Server will not alter this. */
        Ptr reserved;
    /**< [Reserved] Future requirements/enhancements if any */
    } ISS_PSrvErrorCallback;

/**
 * @brief  This enumeration defines the types of commands that are supported by
 *         Proxy Server. The interpretation of the command structure
 *         (by Proxy Server) is defined type of command.
 *
 *         A simplex command would indicate that a an single command is to be
 *         executed.
 *         A composite command would indicate that there are more the 1
 *         command that requires to be executed. Typically multiple Q and DQ
 */
    typedef enum {
        ISS_FVID2_CMDTYPE_SIMPLEX = (0x01u),
    /**< Specifies that command is a simplex command i.e. just 1 command */
        ISS_FVID2_CMDTYPE_COMPOSITE = (0x02u),
    /**< Specifies that the command is compound command and all the commands
        in the command structure should be addressed by the proxy server */
        ISS_FVID2_CMDTYPEMAX
    /**< Rear enumeration guard */
    } ISS_PSrvCommandType;

/**
 * @brief  Proxy Server Commands
 *
 *         This enumeration defines the commands that would be honored by the
 *         HDISSS sub-system/Proxy Server. These commands is expected to be
 *         exercised on a remote system that shares memory with the HDISSS
 *         subsystem.
 *
 *         Each of these commands requires the function arguments in a specific
 *         format, type definitions like ISS_PSrvFvid2InitParams,
 *         ISS_PSrvFvid2CreateParams, define the expected order.
 *
 *         Application/Driver running on the remote processor is referred as
 *         an remote agent / client.
 */
    typedef enum {
        ISS_FVID2_INIT = (0x01u),
    /**< The very first command to ISS Subsystem. Expected to be called once,
        either remotely or locally. Advised to exercise this command before
        any operation. An return error code (TBD) will mean that FVID2 has
        been initialized earlier and other FVID2 calls will be honored by HDISSS
        sub-system */
        ISS_FVID2_DE_INIT = (0x02u),
    /**< The last control command, used to terminate FVID2 */
        ISS_FVID2_CREATE = (0x03u),
    /**< Creates the named stream, calls FVID2 API FVID2_create. */
        ISS_FVID2_DELETE = (0x04u),
    /**< Deletes the names stream, calls FVID2 API FVID2_delete */
        ISS_FVID2_CONTROL = (0x05u),
    /**< To control a stream, calls FVID2 API FVID2_control */
        ISS_FVID2_QUEUE = (0x06u),
    /**< To submit buffer that would be consumed by the driver, calls
           FVID2 API FVID2_queue */
        ISS_FVID2_DEQUEUE = (0x07u),
    /**< To retrieve the buffers that were submitted, calls the FVID2 API
           FVID2_processFrames */
        ISS_FVID2_PROCESS_FRAMES = (0x08u),
    /**< Similar to ISS_FVID2_QUEUE, expected to be used with memory-to-memory
           driver. Calls the FVID2 API FVID2_processFrames */
        ISS_FVID2_GET_PROCESSED_FRAMES = (0x09u),
    /**< Similar to ISS_FVID2_DEQUEUE, expected to be used with memory-to-memory
           driver. Calls the FVID2 API FVID2_getProcessedFrames */
        ISS_FVID2_GET_FIRMWARE_VERSION = (0x0Au)
    /**< Command used to determine the current version of the firmware, running
         on the host processor.
         */
    } ISS_PSrvCommands;

/**
 * @brief  Proxy Server Host tasks type
 *
 *          All client commands are processed by the host in a tasks context.
 *          Host spawns 4 tasks that would process these commands, typically
 *          one for display, capture, memory to memory and graphics.
 *          Applications could configure the host to associate an FVID2 channel
 *          with any of the tasks mentioned below.
 *
 *          Note that there would a control task that would process commands
 *          FVID2_init, FVID2_deInit and FVID2_create.
 *
 *          Please refer the user guide that came with this release for details.
 *
 */
    typedef enum {
        ISS_FVID2_TASK_TYPE_LOWER_GUARD = (0x00u),
    /**< Lower guard */
        ISS_FVID2_TASK_TYPE_1 = (0x01u),
    /**< Specifies that the command should be processed by task 1 */
        ISS_FVID2_TASK_TYPE_2 = (0x02u),
    /**< Specifies that the command should be processed by task 2 */
        ISS_FVID2_TASK_TYPE_3 = (0x03u),
    /**< Specifies that the command should be processed by task 3 */
        ISS_FVID2_TASK_TYPE_4 = (0x04u),
    /**< Specifies that the command should be processed by task 4 */
        ISS_FVID2_TASK_TYPE_UPPER_GUARD
    /**< Upper guard */
    } ISS_PSrvHostTaskType;

/**
 * @brief  Command Structure for Proxy Server
 *
 *         A remote agent that requires to instruct the proxy sever, would
 *         require to use the structure below. An instance of this structure 
 *         required to be populated and notified to proxy server.
 *
 *         This structure will define the type of command (one or multiple),
 *         and a pointer to actual FVID2 command (array of pointer in case of
 *         multiple commands)
 */

    typedef struct {
        UInt32 cmdType;
    /**< [IN] Specifies the command type. Only ISS_FVID2_CMDTYPE_SIMPLEX is
             supported at this point.
             Rage of values is defined by #ISS_PSrvCommandType */
        Uint32 yieldAfterNCmds;
    /**< [IN] Used only when the cmdType is ISS_FVID2_CMDTYPE_COMPOSITE,
             As the commands from the clients are processed in task context and
             there are 4 tasks to process a clients request. Each FVID2 channel
             is associated with a task, In cases where IO on a given FVID2
             channel is very high which could potentially cause CPU starve on
             other IO FVID2 channels.
             In these conditions remote agents / clients could decide to YIELD
             the CPU (to other task) after executing N number of commands of
             FVID2 channel.
             This is expected to be used in conjunction with priority of tasks
             to achieve real time balance between FVID2 channels. */
        UInt32 sysLnkProcId;
    /**< [Reserved] Reserved for remote agents, would be used by proxy server to
                   hold the Proc Id of the remote processor in case of command
                   ISS_FVID2_CREATE */
        UInt32 sysLnkNtyNo;
    /**< [Reserved] Reserved for remote agents, would be used by proxy server to
                   hold the event number associated with remote processor in
                   case of command ISS_FVID2_DELETE */

        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder return value, if Proxy Server could not process this
              command.
              e.g.
              Un-Supported command type
              second command received while executing first command
              etc... */
        UInt32 noOfCommands;
    /**< [IN] Applicable in case of ISS_FVID2_CMDTYPE_COMPOSITE, specifies the
             number of commands
             \warning This should be 1 always. Kept for future enhancements */
        Void *simplexCmdArg;
    /**< [IN] Used only when the cmdType is ISS_FVID2_CMDTYPE_SIMPLEX, this
             pointer will point to a structure defined by the command.
             e.g if the command is ISS_FVID2_QUEUE
             ISS_PSrvCommandStruc.cmdType  =   ISS_FVID2_CMDTYPE_SIMPLEX
             and
             ((ISS_PSrvFvid2QueueParams *)simplexCmdArg)->command =
             ISS_FVID2_QUEUE
             */
        Void **compositeCmdArgs;
    /**< [IN] Used only when the cmdType is ISS_FVID2_CMDTYPE_COMPOSITE, this
             will point to array of void pointer, number elements in the array
             will be equals to noOfCommands.
             Type of each of the pointer contained in array will match the
             command used.

             \warning Commands ISS_FVID2_CREATE and ISS_FVID2_DELETE will not
                      supported in the composite type command.
             \warning This command is NOT supported on reserved notify event */
    } ISS_PSrvCommandStruc;

/**
 * @brief  Determine the status of proxy server and revision.
 *
 *         This structure defines the structures that would be used query the
 *         status of the proxy server and query the revision of the firmware.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case
              ISS_FVID2_GET_FIRMWARE_VERSION OR
              ISS_FVID2_GET_STATUS.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for commands executions return value.
               FVID2_SOK on success, an negative otherwise */
        UInt32 status;
    /**< [OUT] Update for command ISS_FVID2_GET_STATUS.
               Positive indicates proxy server is ready to receive FIVD2
               commands, from the clients */
        UInt32 version;
    /**< [OUT] Update for command ISS_FVID2_GET_FIRMWARE_VERSION.
               Its expected to be interpreted as 4 byte Hexadecimal value.
               */
    } ISS_PSrvGetStatusVerCmdParams;

/**
 * @brief  FVID2 Initialization parameters
 *
 *         This structure defines the structures that would be used during
 *         FVID2 initialization.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_INIT.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        Ptr args;
    /**< [IN] First Argument required by the FVID2 API */
    } ISS_PSrvFvid2InitParams;

/**
 * @brief  FVID2 De Initialization parameters
 *
 *         This structure defines the structures that would be used during FVID2
 *         de initialization.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_DE_INIT.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        Ptr args;
    /**< [IN] First Argument required by the FVID2 API */
    } ISS_PSrvFvid2DeInitParams;

/**
 * @brief  FVID2 stream creation parameters
 *
 *         This structure defines the structures that would be used during FVID2
 *         create stream.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_CREATE.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        UInt32 hostTaskInstance;
    /**< [IN] Specifies class of stream */
        UInt32 sysLnkNtyNo;
    /**< [OUT] Proxy Server allocates a notification number, remote clients /
              agents are expected to use this number for future transactions
              on this stream.
              Please refer sysLink for details IPC notifications.
              Note that type of eventId is defined as UInt16 by sysLink, its
              explicitly defined as UInt32. */
        FVID2_Handle fvid2Handle;
    /**< [OUT] Place holder for FVID2 API return value */
        UInt32 drvId;
    /**< [IN] First Argument required by the FVID2 API - Refer FVID2_create */
        UInt32 instanceId;
    /**< [IN] Second Argument required by the FVID2 API - Refer FVID2_create */
        Ptr createArgs;
    /**< [IN] Third Argument required by the  FVID2 API - Refer FVID2_create */
        Ptr createStatusArgs;
    /**< [IN] Fourth Argument required by the FVID2 API - Refer FVID2_create */
        FVID2_CbParams *cbParams;
    /**< [IN] Fivth Argument required by the FVID2 API - Refer FVID2_create */
        ISS_PSrvCallback *ioReqCb;
    /**< [IN] Place holder defined in the remote system which will be
        populated by the Proxy Server and passed back to remote agent on
        completion of an IO request.*/
        ISS_PSrvErrorCallback *errCb;
    /**< [IN] Place holder defined in the remote system which will be
        populated by the Proxy Server and passed back to remote agent on a error
        Please refer FVID2 API header for details */
    } ISS_PSrvFvid2CreateParams;

/**
 * @brief  FVID2 stream deletion parameters
 *
 *         This structure defines the structures that would be used during FVID2
 *         stream deletion
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_DELETE.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] Place holder for FVID2 stream handle */
        Ptr deleteArgs;
    /**< [IN] First Argument required by the FVID2 API */
    } ISS_PSrvFvid2DeleteParams;

/**
 * @brief  FVID2 stream control parameters
 *
 *         This structure defines the structures that would be used to
 *         control a previously  created stream
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_CONTROL.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] FVID2 stream handle */
        UInt32 cmd;
    /**< [IN] command refer FVID2 API */
        Ptr cmdArgs;
    /**< [IN] First Argument required by the FVID2 API refer FVID2 API */
        Ptr cmdStatusArgs;
    /**< [IN] Second Argument required by the FVID2 API refer FVID2 API */
    } ISS_PSrvFvid2ControlParams;

/**
 * @brief  FVID2 queue IO requests
 *
 *         This structure defines the structures that would be used queue an IO
 *         request.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_QUEUE.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] FVID2 stream handle */
        FVID2_FrameList *frameList;
    /**< [IN] First Argument required by the FVID2 API refer FVID2 API */
        UInt32 streamId;
    /**< [IN] Second Argument required by the FVID2 API refer FVID2 API */
    } ISS_PSrvFvid2QueueParams;

/**
 * @brief  FVID2 dequeue IO requests
 *
 *         This structure defines the structures that would be used dequeue
 *         completed IO request.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_DEQUEUE.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] FVID2 stream handle */
        FVID2_FrameList *frameList;
    /**< [IN] First Argument required by the FVID2 API refer FVID2 API */
        UInt32 streamId;
    /**< [IN] Second Argument required by the FVID2 API refer FVID2 API */
        UInt32 timeout;
    /**< [IN] third Argument required by the FVID2 API refer FVID2 API */
    } ISS_PSrvFvid2DequeueParams;

/**
 * @brief  FVID2 Process Frames
 *
 *         This structure defines the structures that would be used submit
 *         frames to be processed.
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case ISS_FVID2_PROCESS_FRAMES.
              #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] FVID2 stream handle */
        FVID2_ProcessList *processList;
    /**< [IN] First Argument required by the FVID2 API refer FVID2 API */
    } ISS_PSrvFvid2ProcessFramesParams;

/**
 * @brief  FVID2 Retrieve Processed Frames
 *
 *         This structure defines the structures that would be used retrieve
 *         processed frames
 *         Remote clients / agents allocates the memory for this structure,
 *         initializes the function parameters and pass a pointer to this struct
 *
 */
    typedef struct {
        UInt32 command;
    /**< [IN] Command from the agent - in this case
             ISS_FVID2_GET_PROCESSED_FRAMES.
             #ISS_PSrvCommands Lists all supported commands */
        UInt32 reserved;
    /**< [Reserved] Reserved for future use */
        Int32 returnValue;
    /**< [OUT] Place holder for FVID2 API return value */
        FVID2_Handle fvid2Handle;
    /**< [IN] FVID2 stream handle */
        FVID2_ProcessList *processList;
    /**< [OUT] First Argument required by the FVID2 API refer FVID2 API */
        UInt32 timeout;
    /**< [IN] third Argument required by the FVID2 API refer FVID2 API */
    } ISS_PSrvFvid2GetProcessedFramesParams;

    /* ========================================================================== 
     */
    /* Functions/data types that are exposed to host system only */
    /* Remote agents / clients SHOULD not use these interfaces.  */
    /* ========================================================================== 
     */

/**
 * @brief  Proxy Server initialization configurations
 *         This structure defines the configurable options that are supported.
 *
 *         With these configurations, the host system could configure to
 *         use required notification numbers, depth of the tasks, etc...
 *
 *  \warning There should be one-to-one relation between instances of following
 *          member of this structure
 *          sysLnkCoreNames
 *          sysLnkNtyHndls
 *          sysLnkNoOfNtyEvt
 *          sysLnkNtyEvtNo
 *          resSysLnkNtyNo
 */
    typedef struct {
        UInt32 confIpcNtyDriver;
    /**< [IN] Not used now. Expected to be used in future.
            In scenarios where sysLink has... TBD */
        UInt32 sysLnkNoOfCores;
    /**< [IN] Number of agents / clients running on different cores, which
             requires Proxy Servers services */
        Char *sysLnkCoreNames[ISS_PSRV_MAX_NO_OF_CORES];
    /**< [IN] Names of the cores specified during initialization of sysLink /
             IPC-Notify driver. Proxy Server would use these names to extract
             processor identifier as required by sysLink / IPC-Notify.
             An incorrect name will result initialization failure. */
        UInt32 sysLnkNoOfNtyEvt[ISS_PSRV_MAX_NO_OF_CORES];
    /**< [IN] The maximum number of events / (translates to maximum number of
             independent FVID2 channels) that would be required on each core.
             Note that there is 1to1 relation between names of the cores and no
             of events required.
             Total count should be number of notify events required + 1
             1 is for the reserved notify.
             DO NOT Specify the reserved notify number here.
             \warning The shared memory required would depend on the number of
             events. Please refer sysLinks documentation on memory requirements
             for the shared memory drivers. */
        UInt32 sysLnkNtyEvtNo[ISS_PSRV_MAX_NO_OF_CORES][ISS_PSRV_MAX_NO_NOTIFY];
    /**< [IN] Notify event number that would be supported for each core.
             Note that there is 1to1 relation between number of events and
             notify event numbers.
             There is 1to1 relation between names of the cores and notify
             numbers. */
        UInt32 resSysLnkNtyNo[ISS_PSRV_MAX_NO_OF_CORES];
    /**< [IN] Specifies the reserved notify number. This notify number is to be
             used by remote agents / clients to initialize FVID2, DeIniitalize
             FVID2 and create FVID2 channel
             Note that there is 1to1 relation between names of the cores and
             reserved notify numbers. */
        UInt32 taskPriority[ISS_PSRV_NUMBER_OF_TASKS];
    /**< [IN] Priority of the tasks that process FVID2 commands, first element
             specifies priority of  controlTask, followed by priority of other
             tasks
             Its recommended that priority of control task be the highest */
        UInt32 completionCbTaskPri;
    /**< [IN] On completion of a request (display of a frame completed, when
             Display driver is being used), completion requires to be notified
             to clients. This notification is done by a task. 
             The priority of this task should be highest in system. */
    } ISS_PSrvInitParams;

/**
 * \brief ISS_PSrvInit
 *
 *          This function is called by the host system to initialize the
 *          proxy server.
 *
 * \param   pPsInitParams   [IN] Pointer of type ISS_PSrvInitParams
 *
 * \return  ISS_SOK if successful, else suitable error code
 */

    Int32 ISS_PSrvInit(ISS_PSrvInitParams * pPsInitParams);

    /* @} */

#ifdef __cplusplus
}
#endif
#endif                                                     /* _ISS_PROXY_SERVER_H 
                                                            */
