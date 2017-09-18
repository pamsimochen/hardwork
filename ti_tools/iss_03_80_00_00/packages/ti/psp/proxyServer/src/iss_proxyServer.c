/** ==================================================================
 *  @file   iss_proxyServer.c                                                  
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
 *  \file iss_proxyServer.c
 *
 *  \brief This file implements the functions / tasks that constitute ISS
 *         Proxy Server
 *
 *         Proxy Server provides a RPC for HDISSS sub-system. Applications
 *         / agents running on different core (referred as agents) could make
 *         FVID2 Calls which would be executed in host processor (processor
 *         where ISS sub-system is hosted)
 */

#define SYSLINKS_IPC_NOTIFY_INCLUDED
#define ISS_PS_TASK_4_CB

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/iss.h>

#include <ti/psp/proxyServer/iss_proxyServer.h>
#include <ti/psp/proxyServer/src/iss_proxyServerPriv.h>

/* ========================================================================== 
 */
/* DEBUG Macros */
/* ========================================================================== 
 */
#define PSTrace (GT_TraceState_Enable | GT_ERR | GT_INFO | GT_ENTER | GT_LEAVE)

/* ========================================================================== 
 */
/* Local Macros */
/* ========================================================================== 
 */
#define ISS_PSRV_IPC_LINE_ID_USED   (0x0u)

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */
static ISS_PSrvTaskObject gPSrvTaskObjInstances[ISS_PSRV_NUMBER_OF_TASKS];

/**< Instances of task objects
     0. used for control task. Serves commands received on reserved notify.
     1. used for display task, Servers commands received on display stream
     2. used for capture task, Servers commands received on capture stream
     3. used for graphics task, Servers commands received on graphics stream
     4. used for mem2mem task, Servers commands received on mem to mem stream
     */

static ISS_PSrvNtyNumsPerProcInst gPSrvNtyNo4AllCores[ISS_PSRV_MAX_NO_OF_CORES];

/**< Place holder to store the notify number that would be used by different
     cores, allocated to a core on creation of a stream in a given core. */
static Semaphore_Handle gPSrvGuardNtyNo4AllCores;

/**< Semaphore used to guard the access to notification numbers. This semaphore
     will not used in main IO request context, will be used while FVID2_create
     and FVID2_delete */

static Ptr gPSrvQSpace[ISS_PSRV_NUMBER_OF_TASKS][ISS_PSRV_TASK_MAX_Q_DEPTH];

/**< Place holder for Q of all the tasks, allocates the space for max depth */

static UInt8
    gPSrvTskStackSpace[ISS_PSRV_NUMBER_OF_TASKS][ISS_PSRV_TASK_STACK_DEPTH];
/**< Place holder for the tasks stacks */

#ifdef ISS_PS_TASK_4_CB
static ISS_PSrvTaskObject gPSrvNtfyClientTaskObj;

/**< Instance of task which handles notification to clients on completion of IO
     */
static Ptr gPSrvNtfyClientQSpace[ISS_PSRV_NTFY_TASK_MAX_Q_DEPTH];

/**< Place holder for Q of all the tasks, allocates the space for max depth */

static UInt8 gPSrvNtfyClientTaskStackSpace[ISS_PSRV_TASK_STACK_DEPTH];

/**< Place holder for the tasks stacks */
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */

/* ========================================================================== 
 */
/* Local Functions prototype */
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     psValidateInitParams                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 psValidateInitParams(ISS_PSrvInitParams * pValInitParams);

/**< Validate the user supplied params */
/* ===================================================================
 *  @func     psCtrlTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psCtrlTask(UInt32 taskArgs, UInt32 taskArgsUnused);

/**< Task for the main control that addresses requests raised on the fixed
     notify */
/* ===================================================================
 *  @func     psGenericTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psGenericTask(UInt32 taskArgs, UInt32 taskArgsUnused);

/**< Tasks for DISPLAY, CAPTURE, GRAPHICS and MEM2MEM */
#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
/* ===================================================================
 *  @func     ntyCbCtrl                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void ntyCbCtrl(UInt16 procId,
                      UInt16 lineId, UInt32 eventNo, UArg arg, UInt32 payload);
/**< Callback that would attached to reserved notify event */
/* ===================================================================
 *  @func     ntyCbGeneric                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void ntyCbGeneric(UInt16 procId,
                         UInt16 lineId,
                         UInt32 eventNo, UArg arg, UInt32 payload);
/**< Callback that would attached to each allocated notify event */
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
/* ===================================================================
 *  @func     completionCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 completionCb(FVID2_Handle handle, Ptr appData, Ptr reserved);

/**< Function used to inform remote agent / client on completion of an IO */
/* ===================================================================
 *  @func     errorCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 errorCb(FVID2_Handle handle, Ptr appData, Ptr errData,
                     Ptr reserved);
/**< Function used to inform remote agent / client on Error */
/* ===================================================================
 *  @func     allocSysLnkNtyNumber                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 allocSysLnkNtyNumber(UInt32 sysLnkProcId);

/**< Function to allocate a new notification number */
/* ===================================================================
 *  @func     deallocSysLnkNtyNumber                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void deallocSysLnkNtyNumber(UInt32 sysLnkProcId, UInt32 sysLnkNtyNo);

/**< Function to release a notification number */
/* ===================================================================
 *  @func     initSysLnkNtyShm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 initSysLnkNtyShm(ISS_PSrvInitParams * pSysLnkNtyInitParams);

/**< Function to initialize sysLink with user supplied params */
/* ===================================================================
 *  @func     createFvid2Stream                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void createFvid2Stream(ISS_PSrvFvid2CreateParams * creatParam,
                              Uint32 remoteProcId);
/**< Function to create an FVID 2 stream */
/* ===================================================================
 *  @func     psCallFvid2Api                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 psCallFvid2Api(Void * simplexCommand,
                            ISS_PSrvTaskObject * pSrvTsk,
                            UInt32 procId, UInt32 eventNo);
/**< Function to convert remote agent / client command to a FVID2 call */
/* ===================================================================
 *  @func     initializeClientNtfyTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 initializeClientNtfyTask(ISS_PSrvInitParams *
                                      pSysLnkNtyInitParams);
/**< Function to create task that hanle notification to clients on completion */
/* ===================================================================
 *  @func     psNtfyClientTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psNtfyClientTask(UInt32 taskArgs, UInt32 taskArgsUnused);

/**< Task that hanle notification to clients on completion of a IO request */
/* ========================================================================== 
 */
/* Functions types that are exposed to host system alone */
/* ========================================================================== 
 */

/**
 * \brief ISS_PSrvInit
 *        This function is called to initialize the Proxy Server. Called in the
 *        host system once.
 *
 * \param   psInitParams   [IN] Pointer of type ISS_PSrvInitParams
 *
 * \return  VPS_SOK if successful, else suitable error code
 */

/* ===================================================================
 *  @func     ISS_PSrvInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 ISS_PSrvInit(ISS_PSrvInitParams * pPsInitParams)
{
    Int32 psInitRtnVal = VPS_EDEVICE_INUSE;

    UInt32 index = 0x0;

    Uint32 counter = ISS_PSRV_TASK_MAX_Q_DEPTH + 1u;

    ISS_PSrvTaskObject *pPSrvTskInstance = NULL;

    Semaphore_Params semParams;

    Task_Params taskParams;

    static Uint32 state = ISS_PSRV_UNINITIALIAZED;

    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>ISS_PSrvInit\n");
    GT_0trace(PSTrace, GT_INFO, "PS : Initializing Proxy Server\n");

    /* Validate initialization parameters */
    if (pPsInitParams == NULL)
    {
        psInitRtnVal = VPS_EINVALID_PARAMS;
        GT_0trace(PSTrace, GT_ERR, "PS : Error - Init params is NULL\n");
    }
    else
    {
        psInitRtnVal = psValidateInitParams(pPsInitParams);
    }

    if ((state == ISS_PSRV_UNINITIALIAZED) && (psInitRtnVal == VPS_SOK))
    {
        VpsUtils_memset(gPSrvQSpace, 0x0, sizeof(gPSrvQSpace));
        VpsUtils_memset(gPSrvTaskObjInstances,
                        0x0, sizeof(gPSrvTaskObjInstances));
        /* Notify number are initialized to 0xFF, during de-alloc on error we 
         * do not keep track of list of allocated resources. We assume all
         * resources were allocated and blindly release. 0x0 would be a valid 
         * notify number. 255 is highly unlikely */
        VpsUtils_memset(gPSrvNtyNo4AllCores, 0xFF, sizeof(gPSrvNtyNo4AllCores));
        GT_0trace(PSTrace, GT_INFO, "PS : Creating Semaphores, Qs and Tasks\n");
        /**
         *  For each task
         *  Create a counting semaphore with count equals to q depth.
         *  Create the Q
         *  Create the task
         */
        for (index = 0x0; index < ISS_PSRV_NUMBER_OF_TASKS; index++)
        {
            pPSrvTskInstance = &(gPSrvTaskObjInstances[index]);
            counter = ISS_PSRV_TASK_MAX_Q_DEPTH;

            /* Initialize semaphores for all tasks */
            Semaphore_Params_init(&semParams);
            pPSrvTskInstance->tSem = Semaphore_create(ISS_PSRV_TASK_MAX_Q_DEPTH,
                                                      &semParams, NULL);
            if (NULL == pPSrvTskInstance->tSem)
            {
                GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not acquire\n");
                GT_0trace(PSTrace, GT_ERR, "PS : counting semaphore\n");
                psInitRtnVal = VPS_EALLOC;
                break;
            }
            /* Ensure the semaphores are taken - timeout is not required.
             * But, Just in case */
            while (counter)
            {
                if ((Semaphore_pend(pPSrvTskInstance->tSem, 500u)) != TRUE)
                {
                    GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not take\n");
                    GT_1trace(PSTrace, GT_ERR, "semaphore for handle %d\n",
                              pPSrvTskInstance->tSem);
                    psInitRtnVal = VPS_EOUT_OF_RANGE;
                    break;
                }
                counter--;
            }

            /* 
             *  Initialize Q for all tasks. NON BLOCKING Q.
             *  Q Get will be used in Tasks context, if we see an error, task
             *  was woken up by someone else. Go back and pend on the semaphore.
             *  Q Put will be used by notify callbacks, if we see an error the Q
             *  is full, report the same to agent and exit.
             */
            psInitRtnVal =
                VpsUtils_queCreate(&(pPSrvTskInstance->spaceHolder4QHnld),
                                   ISS_PSRV_TASK_MAX_Q_DEPTH,
                                   ((Ptr) & (gPSrvQSpace[index][0])),
                                   ISSUTILS_QUE_FLAG_NO_BLOCK_QUE);
            if (psInitRtnVal != VPS_SOK)
            {
                psInitRtnVal = VPS_EALLOC;
                GT_1trace(PSTrace, GT_ERR, "PS : Error - Create Q for %d\n",
                          index);
                break;
            }
            pPSrvTskInstance->cmdQHndl = &(pPSrvTskInstance->spaceHolder4QHnld);

            /* 
             * Create Task, the very first task would be main control task.
             *      the entry function for this task would be different as it
             *      requires to handle special cases.
             */
            Task_Params_init(&taskParams);
            taskParams.priority = pPsInitParams->taskPriority[index];
            taskParams.stack = (Ptr) & (gPSrvTskStackSpace[index][0]);
            taskParams.stackSize = ISS_PSRV_TASK_STACK_DEPTH;
            taskParams.arg0 = (UArg) pPSrvTskInstance;

            pPSrvTskInstance->taskInstance = (ISS_PSrvHostTaskType) index;

            if (index == 0x00)
            {
                pPSrvTskInstance->taskOwnHndl = Task_create(psCtrlTask,
                                                            &taskParams, NULL);
            }
            else
            {
                pPSrvTskInstance->taskOwnHndl = Task_create(psGenericTask,
                                                            &taskParams, NULL);
            }

            if (pPSrvTskInstance->taskOwnHndl == NULL)
            {
                GT_1trace(PSTrace, GT_ERR, "PS : Error -Create Task for %d\n",
                          index);
                psInitRtnVal = VPS_EALLOC;
                break;
            }
            psInitRtnVal = VPS_SOK;
        }

        /* Acquire a semaphore to guard the structure gPSrvNtyNo4AllCores
         * used to store notification numbers for all the cores. Done here to 
         * ensure easy error handling */
        Semaphore_Params_init(&semParams);
        gPSrvGuardNtyNo4AllCores = Semaphore_create(0x1, &semParams, NULL);
        if (gPSrvGuardNtyNo4AllCores == NULL)
        {
            GT_0trace(PSTrace, GT_ERR, "PS : Error -Could not acquire sem\n");
            psInitRtnVal = VPS_EALLOC;
        }
#ifdef ISS_PS_TASK_4_CB
        /* Initialize the task that handles IO completion callbacks */
        if (psInitRtnVal == VPS_SOK)
        {
            psInitRtnVal = initializeClientNtfyTask(pPsInitParams);
        }
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */
        /* 
         * Build the list of notify numbers available
         * Attach the callback for the reserved notify
         *
         */
        GT_0trace(PSTrace, GT_INFO, "PS : Initializing IPC Notify\n");
        if (psInitRtnVal == VPS_SOK)
        {
            /* Yield this task, let the created tasks run and initialize
             * themselfs, before we receive any command from the clients */
            Task_yield();
            /* 
             * Initialize sysLink notify and attach handles for reserved
             * notify and associate it with control task.
             *
             * At this point we have no error, if there are any errors with in
             * this function, this function would clear up notify.
             */
            psInitRtnVal = initSysLnkNtyShm(pPsInitParams);
        }

        /* Release all acquired resources acquired */
        if (psInitRtnVal != VPS_SOK)
        {
            /* Step 1 1.1 Delete tasks 1.2 Delete Qs 1.3 Delete semaphores */
            for (index = 0x0; index < ISS_PSRV_NUMBER_OF_TASKS; index++)
            {
                pPSrvTskInstance = &(gPSrvTaskObjInstances[index]);

                if (pPSrvTskInstance->taskOwnHndl != 0x0)
                {
                    Task_delete(&(pPSrvTskInstance->taskOwnHndl));
                }
                if (pPSrvTskInstance->cmdQHndl != 0x0)
                {
                    VpsUtils_queDelete(pPSrvTskInstance->cmdQHndl);
                }
                if (pPSrvTskInstance->tSem != 0x0)
                {
                    Semaphore_delete(&(pPSrvTskInstance->tSem));
                }
                if (gPSrvGuardNtyNo4AllCores != NULL)
                {
                    Semaphore_delete(&gPSrvGuardNtyNo4AllCores);
                }
            }
        }                                                  /* Error check -
                                                            * release
                                                            * acquired
                                                            * resources */
        else
        {
            GT_0trace(PSTrace, GT_INFO, "PS : Initialized Notify\n");
            GT_0trace(PSTrace, GT_INFO, "PS : Initialized Proxy Server\n");
            /* Everything went well update your state */
            state = ISS_PSRV_INITIALIAZED;
        }

    }                                                      /* Initialization
                                                            * check */
    else
    {
        GT_0trace(PSTrace, GT_ERR, "PS :Error - PS is already initialized\n");
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<ISS_PSrvInit\n");
    return (psInitRtnVal);
}

/* ========================================================================== 
 */
/* Local Functions */
/* ========================================================================== 
 */

/**
 * \brief psValidateInitParams
 *        This function is expected to be used to validate application supplied
 *        host configurations.
 *
 * \param   pValInitParams   [IN] Pointer of type ISS_PSrvInitParams
 *
 * \return  VPS_SOK if successful, else suitable error code
 */

/* ===================================================================
 *  @func     psValidateInitParams                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 psValidateInitParams(ISS_PSrvInitParams * pValInitParams)
{
    UInt32 index, noOfNtyIndex;

    Int32 valRtnVal = VPS_SOK;

    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>psValidateInitParams\n");

    if ((pValInitParams->sysLnkNoOfCores >= ISS_PSRV_MAX_NO_OF_CORES) ||
        (pValInitParams->sysLnkNoOfCores < 1u))
    {
        valRtnVal = VPS_EBADARGS;
        GT_0trace(PSTrace, GT_ERR, "PS : Error -Incorrect number of cores\n");
    }

    for (index = 0x0; index < pValInitParams->sysLnkNoOfCores; index++)
    {
        if ((pValInitParams->sysLnkNoOfNtyEvt[index] >= ISS_PSRV_MAX_NO_NOTIFY)
            || (pValInitParams->sysLnkNoOfNtyEvt[index] < 1u))
        {
            GT_0trace(PSTrace, GT_ERR, "PS : Error - No of notify event \n");
            GT_0trace(PSTrace, GT_ERR, "PS : should be between 1 and \n");
            GT_0trace(PSTrace, GT_ERR, "PS : ISS_PSRV_MAX_NO_NOTIFY \n");
            GT_1trace(PSTrace, GT_ERR, "PS : for core %d\n", index);
            valRtnVal = VPS_EBADARGS;
            break;
        }
        /* Check if any of the notify events is reserved notify event,
         * failure to do so, could throttle control task */
        for (noOfNtyIndex = 0x0;
             noOfNtyIndex < pValInitParams->sysLnkNoOfNtyEvt[index];
             noOfNtyIndex++)
        {
            if (pValInitParams->sysLnkNtyEvtNo[index][noOfNtyIndex] ==
                pValInitParams->resSysLnkNtyNo[index])
            {
                GT_0trace(PSTrace, GT_ERR, "PS : Error - Reserved event \n");
                GT_0trace(PSTrace, GT_ERR, "PS : given as an event to be\n");
                GT_0trace(PSTrace, GT_ERR, "PS : allocated for client \n");
                GT_1trace(PSTrace, GT_ERR, "PS : from core %d\n", index);
                valRtnVal = VPS_EBADARGS;
                break;
            }
        }
        if (valRtnVal != VPS_SOK)
        {
            break;
        }
    }
#ifdef ISS_PS_TASK_4_CB
    if (valRtnVal == VPS_SOK)
    {
        for (index = 0x0; index < ISS_PSRV_NUMBER_OF_TASKS; index++)
        {
            if (pValInitParams->completionCbTaskPri <=
                pValInitParams->taskPriority[index])
            {
                valRtnVal = VPS_EBADARGS;
                GT_0trace(PSTrace, GT_ERR, "PS : Error - Completion Tasks \n");
                GT_0trace(PSTrace, GT_ERR, "priority is <= one/more of task\n");
                GT_0trace(PSTrace, GT_ERR, "that process FIVD commands\n");
                break;
            }
        }
    }
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */

    /** No need to validate tasks priorities as BIOS will assert on wrong
        priorities */
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<psValidateInitParams\n");
    return (valRtnVal);
}

/**
 * \brief psCtrlTask
 *        This function implements the main control task, when proxy server is
 *        initialized this task would receive the first few commands from the
 *        remote agent / client. This task would honor simplex.
 *
 *        This task would try to look for stream create command followed by
 *        stream delete command, followed by other commands. Its expected that
 *        remote agents / clients would use this task primarily for creation and
 *        deletion of streams.
 *        IO would be handled by stream specific tasks.
 *
 *
 * \param   taskArgs           [IN]  Pointer to the control tasks variables..
 * \param   taskArgsUnused     [IN]  Un-used.
 *
 * \return  Never.
 */
/* ===================================================================
 *  @func     psCtrlTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psCtrlTask(UInt32 taskArgs, UInt32 taskArgsUnused)
{
    ISS_PSrvTaskObject *pPSrvCtrlTask = NULL;

    ISS_PSrvCommandStruc *pCmdStruct;

    Int32 rtnVal;

    GT_assert(GT_DEFAULT_MASK, taskArgs != NULL);

    pPSrvCtrlTask = (ISS_PSrvTaskObject *) taskArgs;
    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>psCtrlTask\n");

    /* 
     * Step 1. Pend on the semaphore that would be released by NotifyCallbacks
     * Step 2. De Q 1 command
     * Step 3. De-Code the command
     * Step 4. Convert to FVID2 API call
     * Step 5. Update the return value
     * Step 6. Loop
     */
    /* Tasks main loop */
    while (TRUE)
    {
        rtnVal = 0x0;
        pCmdStruct = NULL;
        /* Loop required, on errors we would break out first loop. Providing
         * a clean exit */
        while (TRUE)
        {
            GT_0trace(PSTrace, GT_INFO, "PS : Wating for control command\n");
            /* Step 1. Pend on the semaphore - remote agents / clients would
             * wake us up on a notify */
            if ((Semaphore_pend(pPSrvCtrlTask->tSem, BIOS_WAIT_FOREVER)) ==
                FALSE)
            {
                /* Can occur only on time outs - which should not happen.
                 * Nothing much could be done 0 go back and wait */
                GT_0trace(PSTrace, GT_ERR, "PS : sem take - timed out\n");
                GT_0trace(PSTrace, GT_ERR, "PS : Going back to wait\n");
                break;
            }
            GT_0trace(PSTrace, GT_INFO, "PS : Got a control command\n");
            /* Step 2. - Get one command */
            rtnVal = VpsUtils_queGet(pPSrvCtrlTask->cmdQHndl,
                                     (Ptr *) (&pCmdStruct),
                                     0x01u, BIOS_NO_WAIT);
            if ((rtnVal != 0x0) || (pCmdStruct == NULL))
            {
                /* 
                 * This should not occurs - unless some one woke us up with
                 * no request - Discard this command - go back and wait for
                 * another command
                 */
                GT_0trace(PSTrace, GT_ERR, "PS : Some one woke us up \n");
                GT_0trace(PSTrace, GT_ERR, "PS : no command available\n");
                GT_0trace(PSTrace, GT_ERR, "PS : Ignoring\n");
                break;
            }

            /* Step 3. through Step 5. */
            if (pCmdStruct->cmdType == ISS_FVID2_CMDTYPE_SIMPLEX)
            {
                ISS_PSrvFvid2CmdOverlay *cmdPtr =
                    (ISS_PSrvFvid2CmdOverlay *) pCmdStruct->simplexCmdArg;

                GT_0trace(PSTrace, GT_INFO, "PS:Received simplex command\n");

                if (pCmdStruct->simplexCmdArg == NULL)
                {
                    pCmdStruct->returnValue = VPS_EBADARGS;
                    break;
                }

                if (cmdPtr->command == VPS_FVID2_CREATE)
                {
                    GT_0trace(PSTrace, GT_INFO, "PS : To create a stream,");
                    GT_1trace(PSTrace, GT_INFO, "from core %d\n",
                              pCmdStruct->sysLnkProcId);
                    createFvid2Stream((ISS_PSrvFvid2CreateParams *) pCmdStruct->
                                      simplexCmdArg, pCmdStruct->sysLnkProcId);
                    break;
                }
                else if (cmdPtr->command == ISS_FVID2_DE_INIT)
                {
                    ISS_PSrvFvid2DeInitParams *deInitParams =
                        (ISS_PSrvFvid2DeInitParams *) pCmdStruct->simplexCmdArg;

                    GT_0trace(PSTrace, GT_INFO, "PS : To deintialize,");
                    GT_1trace(PSTrace, GT_INFO, "from core %d\n",
                              pCmdStruct->sysLnkProcId);
                    deInitParams->returnValue =
                        FVID2_deInit(deInitParams->args);

                    GT_1trace(PSTrace, GT_INFO, "PS : [Done] - with %d\n",
                              deInitParams->returnValue);
                    break;
                }                                          /* Check for
                                                            * create / delete 
                                                            * / DeInit / Init 
                                                            */
                else if (cmdPtr->command == ISS_FVID2_INIT)
                {
                    ISS_PSrvFvid2InitParams *initParams =
                        (ISS_PSrvFvid2InitParams *) pCmdStruct->simplexCmdArg;

                    GT_0trace(PSTrace, GT_INFO, "PS : To initialize,");
                    GT_1trace(PSTrace, GT_INFO, "from core %d\n",
                              pCmdStruct->sysLnkProcId);

                    initParams->returnValue = FVID2_init(initParams->args);

                    GT_1trace(PSTrace, GT_INFO, "PS : [Done] - with %d\n",
                              initParams->returnValue);
                    break;
                }
                else
                {
                    if (cmdPtr->command == ISS_FVID2_GET_FIRMWARE_VERSION)
                    {
                        ISS_PSrvGetStatusVerCmdParams *verParams =
                            (ISS_PSrvGetStatusVerCmdParams *)
                            pCmdStruct->simplexCmdArg;
                        verParams->version = VPS_VERSION_NUMBER;
                        verParams->returnValue = VPS_SOK;
                    }
                    else
                    {
                        GT_0trace(PSTrace, GT_INFO,
                                  "PS : Got un-recognized command - Ignoring");
                    }
                }
            }
            else
            {
                GT_0trace(PSTrace, GT_INFO, "PS : Received Composite \n");
                GT_0trace(PSTrace, GT_INFO, "PS : Command not supported \n");
                GT_0trace(PSTrace, GT_INFO, "PS : in control task \n");
                /* Not supported for now */
                /* Let the remote agents / clients know about this */
                pCmdStruct->returnValue = VPS_EUNSUPPORTED_CMD;
            }                                              /* End of simplex
                                                            * command
                                                            * execution */
        }                                                  /* Error catcher
                                                            * loop */
    }                                                      /* Main infinite
                                                            * loop */
}

/**
 * \brief psGenericTask
 *        This function implements the IO tasks, representing display, capture,
 *        graphics and memory to memory streams. Its expected that remote agent
 *        clients use these streams for IO requests.
 *
 *        These tasks would not honor command VPS_FVID2_CREATE and
 *        VPS_FVID2_DELETE
 *
 * \param   taskArgs           [IN]  Pointer to the tasks variables..
 * \param   taskArgsUnused     [IN]  Un-used.
 *
 * \return  Never.
 */

/* ===================================================================
 *  @func     psGenericTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psGenericTask(UInt32 taskArgs, UInt32 taskArgsUnused)
{

    /* 
     * 1. Pend on the semaphore that would be released by NotifyCallbacks
     * 2. De Q
     * 3. De-Code the command
     * 4. Convert to FVID2 API call
     * 5. Update the return value
     * 6. Loop
     */

    ISS_PSrvTaskObject *pPSrvIoTask = NULL;

    ISS_PSrvCommandStruc *pIoCmdStruct;

    Int32 rtnVal;

    GT_assert(GT_DEFAULT_MASK, taskArgs != NULL);

    pPSrvIoTask = (ISS_PSrvTaskObject *) taskArgs;
    GT_1trace(PSTrace, GT_ENTER, "PS: >>>>psGenericTask inst %d\n",
              pPSrvIoTask->taskInstance);
    /* 
     * Step 1. Pend on the semaphore that would be released by NotifyCallbacks
     * Step 2. De Q 1 command
     * Step 3. De-Code the command
     * Step 4. Convert to FVID2 API call
     * Step 5. Update the return value
     * Step 6. Loop
     */
    /* Tasks main loop */
    while (TRUE)
    {
        rtnVal = 0x0;
        pIoCmdStruct = NULL;
        /* Loop required, on errors we would break out first loop. Providing
         * a clean exit */
        while (TRUE)
        {
            GT_1trace(PSTrace, GT_INFO, "PS : Waiting for command - in %d\n",
                      pPSrvIoTask->taskInstance);
            /* Step 1. Pend on the semaphore - remote agents / clients would
             * wake us up on a notify */
            if ((Semaphore_pend(pPSrvIoTask->tSem, BIOS_WAIT_FOREVER)) == FALSE)
            {
                /* Can occur only on time outs - which should not happen.
                 * Nothing much could be done 0 go back and wait */
                break;
            }
            GT_0trace(PSTrace, GT_INFO, "PS : Got a command\n");
            /* Step 2. - Get one command */
            rtnVal = VpsUtils_queGet(pPSrvIoTask->cmdQHndl,
                                     (Ptr *) (&pIoCmdStruct),
                                     0x01u, BIOS_NO_WAIT);
            if ((rtnVal != 0x0) || (pIoCmdStruct == NULL))
            {
                /* 
                 * This should not occurs - unless some one woke us up with
                 * no request - Discard this command - go back and wait for
                 * another command
                 */
                GT_0trace(PSTrace, GT_ERR, "PS : Some one woke us up \n");
                GT_0trace(PSTrace, GT_ERR, "PS : no command available\n");
                GT_0trace(PSTrace, GT_ERR, "PS : Ignoring\n");
                break;
            }
            /* Step 3. through Step 5. */
            if (pIoCmdStruct->cmdType == VPS_FVID2_CMDTYPE_COMPOSITE)
            {
                GT_0trace(PSTrace, GT_INFO, "PS : Composite cmd received");
                GT_1trace(PSTrace, GT_INFO, "PS : by %d\n",
                          pPSrvIoTask->taskInstance);
                /* 
                 * Step A. Figure out the number of commands
                 * Step B. extract the simplex commands
                 * Step C. execute FVID2 API
                 * Step D. If inter command delay is required ensure yield the
                 *         task.
                 */
                if (pIoCmdStruct->compositeCmdArgs != NULL)
                {
                    UInt32 counter = 0x0;

                    UInt32 yield = pIoCmdStruct->yieldAfterNCmds;

                    Void *psimplexCmd;

                    if (yield == 0x0)
                    {
                        /* Biggest positive number - will yield after maximum
                         * number of commands possible */
                        yield = 0x7FFFFFFFu;
                    }

                    for (counter = 0x0;
                         counter <= pIoCmdStruct->noOfCommands; counter++)
                    {
                        GT_0trace(PSTrace, GT_INFO, "PS:Processing command");
                        GT_1trace(PSTrace, GT_INFO, "PS:on %d\n",
                                  pPSrvIoTask->taskInstance);

                        psimplexCmd = (Void *)
                            ((pIoCmdStruct->compositeCmdArgs)[counter]);
                        psCallFvid2Api(psimplexCmd,
                                       pPSrvIoTask,
                                       pIoCmdStruct->sysLnkProcId,
                                       pIoCmdStruct->sysLnkNtyNo);
                        if ((counter % yield) == 0x0)
                        {
                            GT_1trace(PSTrace, GT_INFO, "PS: Task %d yields\n",
                                      pPSrvIoTask->taskInstance);
                            Task_yield();
                        }
                    }
                    break;
                }
            }
            else if (pIoCmdStruct->cmdType == ISS_FVID2_CMDTYPE_SIMPLEX)
            {
                /* 
                 * This function will update the return value to the remote
                 * agent / client
                 */
                GT_1trace(PSTrace, GT_INFO, "PS : Got simplex command on\n",
                          pPSrvIoTask->taskInstance);
                psCallFvid2Api(pIoCmdStruct->simplexCmdArg,
                               pPSrvIoTask,
                               pIoCmdStruct->sysLnkProcId,
                               pIoCmdStruct->sysLnkNtyNo);
                break;
            }
            else
            {
                GT_0trace(PSTrace, GT_INFO, "PS : Got un-recognized command\n");
                GT_2trace(PSTrace, GT_INFO,
                          "From Processor %d, On notify no %d\n",
                          pIoCmdStruct->sysLnkProcId,
                          pIoCmdStruct->sysLnkNtyNo);
            }
        }
    }
}

#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
/**
 * \brief ntyCbCtrl
 *        This function will be called when remote agents / clients request an
 *        control (create / delete stream) via reserved notify
 *
 *        This callback is very similar to generic callback, with exception that
 *        remote process ID is stored, this would be used by the control task to
 *        determine if this command is to be honored.
 *
 *        Proxy Server should / will respond to commands from remote processors
 *        that are registered with proxy server during initialization. By
 *        limiting the PS to check on creation of the streams, its ensured the
 *        PS will talk to registered processors.
 *        If other processors could copy the FVID2 handle created by registered
 *        processor, then commands would be honored. Its upto other remote proc
 *        to ensure that FVID2 handles are not shared with other processors.
 *
 * \param   procId    [IN]  Will be stored, used by command processing task.
 * \param   lineId    [IN]  No used for now.
 * \param   eventNo   [IN]  Number of the event that caused this callback
 * \param   arg       [IN]  Points to taks that would process this command
 * \param   payload   [IN]  Points to the actual command structure of type
 *                          ISS_PSrvCommandStruc *
 *
 * \return  None
 */

/* ===================================================================
 *  @func     ntyCbCtrl                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void ntyCbCtrl(UInt16 procId,
                      UInt16 lineId, UInt32 eventNo, UArg arg, UInt32 payload)
{
    ISS_PSrvTaskObject *pPSrvTaskHndl = (ISS_PSrvTaskObject *) arg;

    ISS_PSrvCommandStruc *pCmdStruct = (ISS_PSrvCommandStruc *) payload;

    /* If the arg is NULL something is wrong in the system */
    GT_assert(GT_DEFAULT_MASK, arg != NULL);

    if (pCmdStruct != NULL)
    {
        /* Store the Processor ID, The control task would require this to
         * make a choice to service this command or not */
        pCmdStruct->sysLnkProcId = procId;
    }
    /* Otherwise put it in the Q let the task figure out all error conditions 
     */

    /* 
     * The Q operation will disable the interrupts before inserting into Q.
     * The timeout should be BIOS_NO_WAIT, Q implementation relies on semaphore
     * to achieve the timeout functionality
     */
    if (VpsUtils_quePut(pPSrvTaskHndl->cmdQHndl, ((Ptr) payload), BIOS_NO_WAIT)
        == ISS_SOK)
    {
        Semaphore_post(pPSrvTaskHndl->tSem);
    }
    else
    {
        ((ISS_PSrvCommandStruc *) payload)->returnValue = ISS_EDRIVER_INUSE;
    }
}

/**
 * \brief ntyCbGeneric
 *        This function will be called when remote agents / clients request an
 *        operation via notify. This function in turn would pass on the command
 *        to appropriate task.
 *
 *        payLoad will point to the command structure and arg will point to task
 *        that would process this command. During notify registration the
 *        control task would have identified the task that would handle this
 *        command based on the class of stream.
 *
 *
 * \param   procId    [IN]  Processor ID not used.
 * \param   lineId    [IN]  No used for now.
 * \param   eventNo   [IN]  Number of the event that caused this callback
 * \param   arg       [IN]  Points to taks that would process this command
 * \param   payload   [IN]  Points to the actual command structure of type
 *                          ISS_PSrvCommandStruc *
 *
 * \return  None
 */
/* ===================================================================
 *  @func     ntyCbGeneric                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void ntyCbGeneric(UInt16 procId,
                         UInt16 lineId,
                         UInt32 eventNo, UArg arg, UInt32 payload)
{
    ISS_PSrvTaskObject *pPSrvTaskHndl = (ISS_PSrvTaskObject *) arg;

    ISS_PSrvCommandStruc *pCmdStruct = (ISS_PSrvCommandStruc *) payload;

    /* If the arg is NULL something is wrong in the system */
    GT_assert(GT_DEFAULT_MASK, arg != NULL);
    if (pCmdStruct != NULL)
    {
        /* Store the Processor ID, The control task would require this to
         * make a choice to service this command or not */
        pCmdStruct->sysLnkProcId = procId;
        pCmdStruct->sysLnkNtyNo = eventNo;
    }
    /* 
     * The Q operation will disable the interrupts before inserting into Q.
     * The timeout should be BIOS_NO_WAIT, Q implementation relies on semaphore
     * to achieve the timeout functionality
     */
    if (VpsUtils_quePut(pPSrvTaskHndl->cmdQHndl, ((Ptr) payload), BIOS_NO_WAIT)
        == ISS_SOK)
    {
        Semaphore_post(pPSrvTaskHndl->tSem);
    }
    else
    {
        ((ISS_PSrvCommandStruc *) payload)->returnValue = ISS_EDRIVER_INUSE;
    }
}

#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
/**
 * \brief completionCb
 *        This function will be called to by FVID2 on completion of an IO
 *        request.
 *
 *        This function would inturn notify remote agent / client of IO
 *        IO completion. The appData will point to a structure of type
 *        ISS_PSrvCallback, which would have been populated during the stream
 *        creation.
 *
 * \param   handle    [IN]  Handle to the FVID2 stream, not used now.
 * \param   appData   [IN]  Pointer to structure of type ISS_PSrvCallback.
 * \param   reserved  [IN]  Reserved.
 *
 * \return  None
 */
/* ===================================================================
 *  @func     completionCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 completionCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32 compCbHndl = ISS_SOK;

#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
#ifndef ISS_PS_TASK_4_CB

    /* 
     * appData will point to structure of type ISS_PSrvCallback. This structures
     * defines the notify number which should be used to notify.
     * For the remote agent / client, the same structure defines the remote apps
     * callback and its argument.
     * Remote agent could call this callback to indicate completion
     */
    ISS_PSrvCallback *pPSrvCb = (ISS_PSrvCallback *) appData;

    GT_assert(GT_DEFAULT_MASK, handle != NULL);
    GT_assert(GT_DEFAULT_MASK, appData != NULL);

    /* The handle should be remote processors shm - upcasted handle */
    compCbHndl = Notify_sendEvent(pPSrvCb->sysLnkProcId,
                                  ISS_PSRV_IPC_LINE_ID_USED,
                                  pPSrvCb->sysLnkNtyNo, (UInt32) appData, TRUE);
#else                                                      /* ISS_PS_TASK_4_CB 
                                                            */
    if (VpsUtils_quePut(gPSrvNtfyClientTaskObj.cmdQHndl, appData, BIOS_NO_WAIT)
        == ISS_SOK)
    {
        Semaphore_post(gPSrvNtfyClientTaskObj.tSem);
    }
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
    return (compCbHndl);
}

/* ===================================================================
 *  @func     errorCb                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 errorCb(FVID2_Handle handle, Ptr appData, Ptr errData,
                     Ptr reserved)
{
    return (ISS_SOK);
}

/**
 * \brief psCallFvid2Api
 *        This function will be called to convert an Proxy Server command to an
 *        FVID2 call and make FVID2 call.
 *
 *        This primary requirement of this function is to be re-entrant. This
 *        function could be called by any of the active Proxy Server tasks.
 *
 * \pre This function expects that simplexCommand points to one of the simplex
 *      command. This function cannot handle composite commands.
 *
 * \param   simplexCommand    [IN]  Pointer to a Proxy Server command.
 * \param   pSrvTsk           [IN]  Pointer to task descriptor.
 * \param   procId            [IN]  Id of the processor that command was
 *                                      received on.
 * \param   pSrvTsk           [IN]  Event number on which the command was
 *                                      received.
 *
 * \return  None
 */
/* ===================================================================
 *  @func     psCallFvid2Api                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 psCallFvid2Api(Void * simplexCommand,
                            ISS_PSrvTaskObject * pSrvTsk,
                            UInt32 procId, UInt32 eventNo)
{
    Int32 fivd2RtnValue = ISS_EFAIL;

    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>psCallFvid2Api\n");
    if (simplexCommand == NULL)
    {
        /* Breaking the multiple return RULE, in this function there are 2
         * returns, one here and other at the last of this function Another
         * level of indentation was not justified to meet this rule */
        GT_0trace(PSTrace, GT_INFO, "PS : Received an NULL command\n");
        return fivd2RtnValue;
    }

    /* 
     * Following steps are performed
     * 1. Determine the FVID2 command
     * 2. Call the FVID2 API
     * 3. Update the return value in the space provided by remote agent / client
     * 4. Return the FVID2 API return value
     */
    switch (((ISS_PSrvFvid2CmdOverlay *) simplexCommand)->command)
    {
        case ISS_FVID2_QUEUE:
        {
            ISS_PSrvFvid2QueueParams *qParams = (ISS_PSrvFvid2QueueParams *)
                simplexCommand;
            GT_0trace(PSTrace, GT_INFO, "PS : FVID2 - Queuing up\n");
            qParams->returnValue = FVID2_queue(qParams->fvid2Handle,
                                               qParams->frameList,
                                               qParams->streamId);
            fivd2RtnValue = qParams->returnValue;
            GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n",
                      fivd2RtnValue);
            break;
        }
        case ISS_FVID2_DEQUEUE:
        {
            ISS_PSrvFvid2DequeueParams *dQParams =
                (ISS_PSrvFvid2DequeueParams *) simplexCommand;
            GT_0trace(PSTrace, GT_INFO, "PS : FVID2 - De-Queuing\n");
            dQParams->returnValue = FVID2_dequeue(dQParams->fvid2Handle,
                                                  dQParams->frameList,
                                                  dQParams->streamId,
                                                  dQParams->timeout);
            fivd2RtnValue = dQParams->returnValue;
            GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n",
                      fivd2RtnValue);
            break;
        }
        case ISS_FVID2_PROCESS_FRAMES:
        {
            ISS_PSrvFvid2ProcessFramesParams *prsFrameParams = NULL;

            prsFrameParams =
                (ISS_PSrvFvid2ProcessFramesParams *) simplexCommand;

            GT_0trace(PSTrace, GT_INFO, "PS :FVID2-Queuing up to process\n");
            prsFrameParams->returnValue =
                FVID2_processFrames(prsFrameParams->fvid2Handle,
                                    prsFrameParams->processList);
            fivd2RtnValue = prsFrameParams->returnValue;
            GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n",
                      fivd2RtnValue);
            break;
        }
        case ISS_FVID2_GET_PROCESSED_FRAMES:
        {
            ISS_PSrvFvid2GetProcessedFramesParams *gPrsFrameParams = NULL;

            GT_0trace(PSTrace, GT_INFO, "PS :FVID2-Getting processed frames\n");
            gPrsFrameParams = (ISS_PSrvFvid2GetProcessedFramesParams *)
                simplexCommand;

            gPrsFrameParams->returnValue =
                FVID2_getProcessedFrames(gPrsFrameParams->fvid2Handle,
                                         gPrsFrameParams->processList,
                                         gPrsFrameParams->timeout);
            fivd2RtnValue = gPrsFrameParams->returnValue;
            GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n",
                      fivd2RtnValue);
            break;
        }
        case ISS_FVID2_CONTROL:
        {
            ISS_PSrvFvid2ControlParams *ctrlParams = NULL;

            ctrlParams = (ISS_PSrvFvid2ControlParams *) simplexCommand;
            GT_0trace(PSTrace, GT_INFO, "PS : FVID2 - Control\n");
            ctrlParams->returnValue = FVID2_control(ctrlParams->fvid2Handle,
                                                    ctrlParams->cmd,
                                                    ctrlParams->cmdArgs,
                                                    ctrlParams->cmdStatusArgs);
            fivd2RtnValue = ctrlParams->returnValue;
            GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n",
                      fivd2RtnValue);
            break;
        }
        case ISS_FVID2_DELETE:
        {
            /* 
             * De-allocate a notification number
             * De-Register notification
             * execute FVID2 delete
             * Update the return value
             */
            ISS_PSrvFvid2DeleteParams *delParams =
                (ISS_PSrvFvid2DeleteParams *) simplexCommand;

            fivd2RtnValue = ISS_EBADARGS;
            GT_0trace(PSTrace, GT_INFO, "PS : To Deleate a stream,from core\n");
            GT_1trace(PSTrace, GT_INFO, "PS : %d \n", procId);
            GT_0trace(PSTrace, GT_INFO, "PS : Deleting FVID2 stream\n");
            fivd2RtnValue = FVID2_delete(delParams->fvid2Handle,
                                         delParams->deleteArgs);
            if (fivd2RtnValue == ISS_SOK)
            {
                GT_1trace(PSTrace, GT_INFO, "PS :Un-registering for event %d\n",
                          eventNo);

#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
                if (Notify_unregisterEvent(procId,
                                           ISS_PSRV_IPC_LINE_ID_USED,
                                           eventNo,
                                           ntyCbGeneric,
                                           (UArg) pSrvTsk) != Notify_S_SUCCESS)
                {
                    fivd2RtnValue = ISS_EFAIL;
                }
                else
                {
                    /* De-alloc only on success, else we might corrupt
                     * another newly create stream */
                    deallocSysLnkNtyNumber(procId, eventNo);
                }
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
            }
            delParams->returnValue = fivd2RtnValue;
            break;
        }
        default:
        {
            GT_0trace(PSTrace, GT_INFO, "PS : FVID2-Un-Recognized command\n");
            /* Let the remote agent / client know that the command is not
             * supported */
            ((ISS_PSrvFvid2CmdOverlay *) simplexCommand)->returnValue =
                ISS_EUNSUPPORTED_CMD;
        }
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<psCallFvid2Api\n");
    return (fivd2RtnValue);
}

/**
 * \brief allocSysLnkNtyNumber
 *        This function will be called to allocate un-used notification number.
 *        Note that this function will not be aware of the system wide usage
 *        of notify numbers. During initialization of Proxy Server, notify
 *        numbers would be specified.
 *
 *        Employees a simple linear search to look for un-used notify numbers.
 *
 * \pre   This functions assumes deallocSysLnkNtyNumber and allocSysLnkNtyNumber
 *        will called from a single task. This function cannot be used interrupt
 *        context.
 *        If this functions requires to be executed in interrupt context or
 *        multi-tasks context, we would require to ensure adequate protection
 *        before using gPSrvNtyNo4AllCores.
 *
 *
 * \param   None
 *
 * \return  ISS_EALLOC if no notification numbers are available other wise a +
 *          number.
 */

/* ===================================================================
 *  @func     allocSysLnkNtyNumber                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 allocSysLnkNtyNumber(UInt32 sysLnkProcId)
{
    Int32 returnValue = ISS_EALLOC;

    UInt32 procIdIndex, ntyNoIndex;

    ISS_PSrvNtyNumObject *pProcNtyNoInst;

    GT_assert(GT_DEFAULT_MASK, sysLnkProcId < ISS_PSRV_MAX_NO_OF_CORES);
    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>allocSysLnkNtyNumber\n");
    GT_0trace(PSTrace, GT_INFO, "PS : Trying to allocate a notify event\n");
    GT_1trace(PSTrace, GT_INFO, "PS : no for processor %d\n", sysLnkProcId);
    for (procIdIndex = 0x0;
         procIdIndex < ISS_PSRV_MAX_NO_OF_CORES; procIdIndex++)
    {
        if (gPSrvNtyNo4AllCores[procIdIndex].determinedProcId == sysLnkProcId)
        {
            pProcNtyNoInst = gPSrvNtyNo4AllCores[procIdIndex].ntyNoInst;
            Semaphore_pend(gPSrvGuardNtyNo4AllCores, BIOS_WAIT_FOREVER);

            for (ntyNoIndex = 0;
                 ntyNoIndex < gPSrvNtyNo4AllCores[procIdIndex].ntyNumCountMax;
                 ntyNoIndex++)
            {
                if ((pProcNtyNoInst[ntyNoIndex].status) == ISS_PSRV_NOTIFY_FREE)
                {
                    pProcNtyNoInst[ntyNoIndex].status
                        = ISS_PSRV_NOTIFY_OCCUPIED;

                    returnValue = pProcNtyNoInst[ntyNoIndex].ntyNumber;
                    GT_1trace(PSTrace, GT_INFO, "PS : Allocated notify no %d\n",
                              returnValue);
                    break;
                }
            }
            Semaphore_post(gPSrvGuardNtyNo4AllCores);
            if (returnValue != ISS_EALLOC)
            {
                break;
            }
        }
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<allocSysLnkNtyNumber\n");
    return (returnValue);
}

/**
 * \brief deallocSysLnkNtyNumber
 *        This function will be called to de-allocate used notification number.
 *
 * \pre   This functions assumes deallocSysLnkNtyNumber and allocSysLnkNtyNumber
 *        will called from a single task. This function cannot be used interrupt
 *        context.
 *        If this functions requires to be executed in interrupt context or
 *        multi-tasks context, we would require to ensure adequate protection
 *        before using gPSrvNtyNo4AllCores.
 *
 *
 * \param   sysLnkNtyNo    Notification number that was allocated using
 *                         allocSysLnkNtyNumber ()
 *
 * \return  None.
 */

/* ===================================================================
 *  @func     deallocSysLnkNtyNumber                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void deallocSysLnkNtyNumber(UInt32 sysLnkProcId, UInt32 sysLnkNtyNo)
{
    UInt32 procIdIndex, ntyNoIndex;

    ISS_PSrvNtyNumObject *pProcNtyNoInst;

    GT_assert(GT_DEFAULT_MASK, sysLnkProcId < ISS_PSRV_MAX_NO_OF_CORES);
    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>deallocSysLnkNtyNumber\n");
    GT_2trace(PSTrace, GT_ENTER, "PS:De-Allocating event no %d for proc %d\n",
              sysLnkNtyNo, sysLnkProcId);
    for (procIdIndex = 0x0;
         procIdIndex < ISS_PSRV_MAX_NO_OF_CORES; procIdIndex++)
    {
        if (gPSrvNtyNo4AllCores[procIdIndex].determinedProcId == sysLnkProcId)
        {
            pProcNtyNoInst = gPSrvNtyNo4AllCores[procIdIndex].ntyNoInst;
            Semaphore_pend(gPSrvGuardNtyNo4AllCores, BIOS_WAIT_FOREVER);

            for (ntyNoIndex = 0;
                 ntyNoIndex < gPSrvNtyNo4AllCores[procIdIndex].ntyNumCountMax;
                 ntyNoIndex++)
            {
                if ((pProcNtyNoInst[ntyNoIndex].ntyNumber) == sysLnkNtyNo)
                {
                    if (pProcNtyNoInst[ntyNoIndex].status !=
                        ISS_PSRV_NOTIFY_FREE)
                    {
                        pProcNtyNoInst[ntyNoIndex].status
                            = ISS_PSRV_NOTIFY_FREE;
                        GT_1trace(PSTrace, GT_INFO,
                                  "PS : DeAllocated notify %d\n",
                                  pProcNtyNoInst[ntyNoIndex].ntyNumber);
                    }
                    else
                    {
                        GT_1trace(PSTrace, GT_INFO,
                                  "PS : %d-is already released\n",
                                  pProcNtyNoInst[ntyNoIndex].ntyNumber);
                    }
                    break;
                }
            }
            Semaphore_post(gPSrvGuardNtyNo4AllCores);
        }
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<deallocSysLnkNtyNumber\n");
    return;
}

/**
 * \brief createFvid2Stream
 *        This function will be called to de-allocate used notification number.
 *
 * \pre   This functions assumes deallocSysLnkNtyNumber and allocSysLnkNtyNumber
 *        will called from a single task. This function cannot be used interrupt
 *        context.
 *        If this functions requires to be executed in interrupt context or
 *        multi-tasks context, we would require to ensure adequate protection
 *        before using gPSrvNtyNo4AllCores.
 *
 *
 * \param   sysLnkNtyNo    Notification number that was allocated using
 *                         allocSysLnkNtyNumber ()
 *
 * \return  None.
 */

/* ===================================================================
 *  @func     createFvid2Stream                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void createFvid2Stream(ISS_PSrvFvid2CreateParams * creatParam,
                              Uint32 remoteProcId)
{
    FVID2_Handle fvid2Hndl = NULL;

    Int32 allocatedNtyNo = ISS_EALLOC;

    UInt32 callbacksGiven = FALSE;

    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>createFvid2Stream\n");
    /* Loop to enable return on error */
    while (TRUE)
    {
        /* NULL Checks */
        if (creatParam == NULL)
        {
            break;
        }

        /* 
         * Step A  Validate stream type and pointers
         * Step B. Allocate a notification number
         * Step C. Associate notify handler with a class task.
         * Step D. Populate completion and error callback info, for non-display
         *          controller stream type.
         * Step E. execute FVID2 create
         */

        /* Step A. Validate class of stream */

        if ((creatParam->hostTaskInstance <= ISS_FVID2_TASK_TYPE_LOWER_GUARD) &&
            (creatParam->hostTaskInstance >= ISS_FVID2_TASK_TYPE_UPPER_GUARD))
        {
            /* Wrong stream type */
            creatParam->fvid2Handle = NULL;
            *((Int32 *) (creatParam->createStatusArgs)) = ISS_EUNSUPPORTED_OPS;
            GT_0trace(PSTrace, GT_ERR, "PS : Error - wrong stream type\n");
            break;
        }

        /* Check for the pointer required by Proxy Server, other params will
         * be be validated by FVID2 API call. Rely on it */
        if (((creatParam->errCb != NULL) && (creatParam->ioReqCb != NULL)) &&
            (creatParam->cbParams != NULL))
        {
            /* Callbacks are specified, swap the callbacks with our own
             * callbacks */
            callbacksGiven = TRUE;
        }
        /* Step B. Allocate a notification number */
        allocatedNtyNo = allocSysLnkNtyNumber(remoteProcId);

        if ((allocatedNtyNo > ISS_PSRV_MAX_NO_NOTIFY) ||
            (allocatedNtyNo < 0x0u))
        {
            /* Out of notification numbers */
            creatParam->fvid2Handle = NULL;
            *((Int32 *) (creatParam->createStatusArgs)) =
                creatParam->ioReqCb->sysLnkNtyNo;
            GT_0trace(PSTrace, GT_ERR, "PS : No more notification numbers\n");
            GT_1trace(PSTrace, GT_ERR, "PS : cmd from core\n", remoteProcId);
            break;
        }
        /* Step C. Associate notify with right class of stream handling task */
#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
        if (Notify_registerEvent((UInt16) remoteProcId,
                                 ISS_PSRV_IPC_LINE_ID_USED,
                                 allocatedNtyNo,
                                 ntyCbGeneric,
                                 (UArg)
                                 &
                                 (gPSrvTaskObjInstances
                                  [creatParam->hostTaskInstance])) !=
            Notify_S_SUCCESS)
        {
            deallocSysLnkNtyNumber(remoteProcId, allocatedNtyNo);
            *((Int32 *) (creatParam->createStatusArgs)) = ISS_EALLOC;
            GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not register\n");
            break;

        }
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
        /* The DISPLAY Controller will not have callbacks */
        if (callbacksGiven == TRUE)
        {
            creatParam->ioReqCb->sysLnkNtyNo = allocatedNtyNo;
            creatParam->errCb->sysLnkNtyNo = allocatedNtyNo;
            /* Step D populate completion and error callbacks */
            creatParam->ioReqCb->appCallBack = creatParam->cbParams->cbFxn;
            creatParam->ioReqCb->appData = creatParam->cbParams->appData;
            creatParam->ioReqCb->sysLnkProcId = remoteProcId;
            creatParam->ioReqCb->cbType = ISS_FVID2_IO_CALLBACK;

            creatParam->errCb->appCallBack = creatParam->cbParams->errCbFxn;
            creatParam->errCb->errList = creatParam->cbParams->errList;
            creatParam->errCb->sysLnkProcId = remoteProcId;
            creatParam->errCb->cbType = ISS_FVID2_ERR_CALLBACK;

            creatParam->cbParams->cbFxn = completionCb;
            creatParam->cbParams->appData = creatParam->ioReqCb;
            creatParam->cbParams->errCbFxn = errorCb;
            creatParam->cbParams->errList = creatParam->errCb;
        }
        /* Step E Call FVID2 API */
        creatParam->sysLnkNtyNo = allocatedNtyNo;
        fvid2Hndl = FVID2_create(creatParam->drvId,
                                 creatParam->instanceId,
                                 creatParam->createArgs,
                                 creatParam->createStatusArgs,
                                 creatParam->cbParams);
        if (fvid2Hndl != NULL)
        {
            /* Only if callback were supplied */
            if (callbacksGiven == TRUE)
            {
                creatParam->ioReqCb->fvid2Handle = fvid2Hndl;
                creatParam->errCb->fvid2Handle = fvid2Hndl;
            }
            /** Ensure that this is last assignment as client would poll on
                this member */
            creatParam->fvid2Handle = fvid2Hndl;
        }
        else
        {
#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
            Notify_unregisterEvent((UInt16) remoteProcId,
                                   ISS_PSRV_IPC_LINE_ID_USED,
                                   allocatedNtyNo,
                                   ntyCbGeneric,
                                   (UArg)
                                   &
                                   (gPSrvTaskObjInstances
                                    [creatParam->hostTaskInstance]));
#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
            deallocSysLnkNtyNumber(remoteProcId, allocatedNtyNo);

            creatParam->fvid2Handle = NULL;
        }

        break;
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<createFvid2Stream\n");
}

/**
 * \brief initSysLnkNtyShm
 *        This function will be called at initialization time, to acquire
 *        sysLinks Notify handles for each of the specified cores.
 *
 * \param   pSysLnkNtyInitParams    sysLink initialization parameters, supplied
 *                                  application during initialization of PS
 *
 * \return  ISS_SOK if successful, error code otherwise.
 */

/* ===================================================================
 *  @func     initSysLnkNtyShm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 initSysLnkNtyShm(ISS_PSrvInitParams * pSysLnkNtyInitParams)
{
    /* 
     *  For each core
     *      Step 0. Store the no of events per core, max nty count etc...
     *      Step 1. Get the procId of the remote core
     *      Step 3. Register callback on the reserved notify
     *      Step 4. Ensure that all task context has access to all handles.
     *              each task might require to notify all cores.
     *      Step 5. If any error, dealloc the created handles and return error
     */
    Int32 psRtnVal = ISS_SOK;

#ifdef SYSLINKS_IPC_NOTIFY_INCLUDED
    UInt32 procIdIndex;

    UInt32 ntyIndex;

    Semaphore_pend(gPSrvGuardNtyNo4AllCores, BIOS_WAIT_FOREVER);
    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>initSysLnkNtyShm\n");
    for (procIdIndex = 0x0;
         procIdIndex < pSysLnkNtyInitParams->sysLnkNoOfCores; procIdIndex++)
    {
        /* Step 0 and 1 */
        gPSrvNtyNo4AllCores[procIdIndex].ntyNumCountMax =
            pSysLnkNtyInitParams->sysLnkNoOfNtyEvt[procIdIndex];
        gPSrvNtyNo4AllCores[procIdIndex].determinedProcId =
            MultiProc_getId(pSysLnkNtyInitParams->sysLnkCoreNames[procIdIndex]);
        for (ntyIndex = 0x0;
             ntyIndex < gPSrvNtyNo4AllCores[procIdIndex].ntyNumCountMax;
             ntyIndex++)
        {
            gPSrvNtyNo4AllCores[procIdIndex].ntyNoInst[ntyIndex].ntyNumber =
                pSysLnkNtyInitParams->sysLnkNtyEvtNo[procIdIndex][ntyIndex];
            gPSrvNtyNo4AllCores[procIdIndex].ntyNoInst[ntyIndex].status =
                ISS_PSRV_NOTIFY_FREE;
        }
        /* Step 3. Register callback on the reserved notify */
        psRtnVal =
            Notify_registerEvent(gPSrvNtyNo4AllCores[procIdIndex].
                                 determinedProcId, ISS_PSRV_IPC_LINE_ID_USED,
                                 pSysLnkNtyInitParams->
                                 resSysLnkNtyNo[procIdIndex], ntyCbCtrl,
                                 (UArg) & (gPSrvTaskObjInstances[0x0]));
        if (psRtnVal != Notify_S_SUCCESS)
        {
            psRtnVal = ISS_EALLOC;
            GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not attach\n");
            GT_2trace(PSTrace, GT_ERR, "PS : handler event %d for core %d\n",
                      pSysLnkNtyInitParams->resSysLnkNtyNo[procIdIndex],
                      procIdIndex);
            break;
        }
    }
    /* Required as the count would have exceeded the max proc count */
    procIdIndex--;

    /* Step 4 */
    if (psRtnVal < ISS_SOK)
    {
        GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not register a cb\n");
        GT_1trace(PSTrace, GT_ERR, "PS : rsvd event no for core %d\n",
                  procIdIndex);
        while (procIdIndex)
        {
            Notify_unregisterEvent(gPSrvNtyNo4AllCores[procIdIndex].
                                   determinedProcId, ISS_PSRV_IPC_LINE_ID_USED,
                                   pSysLnkNtyInitParams->
                                   resSysLnkNtyNo[procIdIndex], ntyCbCtrl,
                                   (UArg) & (gPSrvTaskObjInstances[0x0]));
            procIdIndex--;
        }
        Notify_unregisterEvent(gPSrvNtyNo4AllCores[procIdIndex].
                               determinedProcId, ISS_PSRV_IPC_LINE_ID_USED,
                               pSysLnkNtyInitParams->
                               resSysLnkNtyNo[procIdIndex], ntyCbCtrl,
                               (UArg) & (gPSrvTaskObjInstances[0x0]));
    }
    else
    {
        psRtnVal = ISS_SOK;
    }

    Semaphore_post(gPSrvGuardNtyNo4AllCores);

#endif                                                     /* SYSLINKS_IPC_NOTIFY_INCLUDED 
                                                            */
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<initSysLnkNtyShm\n");
    return psRtnVal;
}

/**
 * \brief initializeClientNtfyTask
 *        This function will be called at initialization time, to create task
 *        the handles notification to clients on completed IO.
 *
 * \param   pPsNtfyInitParams    Priority of task callback handler task
 *
 * \return  ISS_SOK if successful, error code otherwise.
 */
/* ===================================================================
 *  @func     initializeClientNtfyTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 initializeClientNtfyTask(ISS_PSrvInitParams * pPsNtfyInitParams)
{
    Int32 rtnValue = ISS_SOK;

    ISS_PSrvTaskObject *pPSrvNtfyTsk = NULL;

    UInt32 counter;

    Semaphore_Params semParams;

    Task_Params taskParams;

#ifdef ISS_PS_TASK_4_CB
    while (TRUE)
    {
        /* 
         * Create semaphore used for sync between IO completion CB and task that
         *      notifies clients of completion.
         * Create and initialize Q, that holds completed IO details.
         * Create the task that notifies clients on completion of IO request.
         */
        pPSrvNtfyTsk = &(gPSrvNtfyClientTaskObj);
        counter = ISS_PSRV_NTFY_TASK_MAX_Q_DEPTH;

        Semaphore_Params_init(&semParams);
        pPSrvNtfyTsk->tSem = Semaphore_create(ISS_PSRV_NTFY_TASK_MAX_Q_DEPTH,
                                              &semParams, NULL);
        if (NULL == pPSrvNtfyTsk->tSem)
        {
            GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not acquire\n");
            GT_0trace(PSTrace, GT_ERR, "PS : counting semaphore\n");
            rtnValue = ISS_EALLOC;
            break;
        }
        /* Ensure the semaphores are taken - timeout is not required. But,
         * Just in case */
        while (counter)
        {
            if ((Semaphore_pend(pPSrvNtfyTsk->tSem, 500u)) != TRUE)
            {
                GT_0trace(PSTrace, GT_ERR, "PS : Error - Could not take\n");
                GT_1trace(PSTrace, GT_ERR, "semaphore for handle %d\n",
                          pPSrvNtfyTsk->tSem);
                rtnValue = ISS_EOUT_OF_RANGE;
                break;
            }
            counter--;
        }

        /* 
         *  NON BLOCKING Q. Filled by completionCb, consumed by this task
         *  Q Get will be used in Tasks context, if we see an error, task
         *  was woken up by rouge completionCb else, Go back and pend on the sem.
         *  Q Put will be used by completionCb, if we see an error the Q
         *  is full, one of the client is dead/too slow.
         */
        rtnValue = VpsUtils_queCreate(&(pPSrvNtfyTsk->spaceHolder4QHnld),
                                      ISS_PSRV_NTFY_TASK_MAX_Q_DEPTH,
                                      ((Ptr) & (gPSrvNtfyClientQSpace[0])),
                                      ISSUTILS_QUE_FLAG_NO_BLOCK_QUE);
        if (rtnValue != ISS_SOK)
        {
            rtnValue = ISS_EALLOC;
            GT_0trace(PSTrace, GT_ERR,
                      "PS : Error - Could not create Q for \n");
            GT_0trace(PSTrace, GT_ERR, "Completion task\n");
            break;
        }
        pPSrvNtfyTsk->cmdQHndl = &(pPSrvNtfyTsk->spaceHolder4QHnld);

        Task_Params_init(&taskParams);
        taskParams.priority = pPsNtfyInitParams->completionCbTaskPri;
        taskParams.stack = (Ptr) & (gPSrvNtfyClientTaskStackSpace[0]);
        taskParams.stackSize = ISS_PSRV_TASK_STACK_DEPTH;
        taskParams.arg0 = (UArg) pPSrvNtfyTsk;

        pPSrvNtfyTsk->taskInstance = ISS_FVID2_TASK_TYPE_UPPER_GUARD;

        pPSrvNtfyTsk->taskOwnHndl = Task_create(psNtfyClientTask,
                                                &taskParams, NULL);
        break;
    }
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */
    return rtnValue;
}

/**
 * \brief psNtfyClientTask
 *        This task would wait on semaphore, to be released by callback on
 *        completion of IO. Using the details queued by the IO callback, will
 *        notify appropriate client of IO completion.
 *
 * \param   pPsNtfyInitParams    Priority of task
 *
 * \return  ISS_SOK if successful, error code otherwise.
 */
/* ===================================================================
 *  @func     psNtfyClientTask                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void psNtfyClientTask(UInt32 taskArgs, UInt32 taskArgsUnused)
{
#ifdef ISS_PS_TASK_4_CB
    ISS_PSrvTaskObject *pPSrvIoTask = NULL;

    ISS_PSrvCallback *pPSrvCb = NULL;

    UInt32 tempVar;

    Int32 rtnVal;

    GT_assert(GT_DEFAULT_MASK, taskArgs != NULL);

    pPSrvIoTask = (ISS_PSrvTaskObject *) taskArgs;
    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>psNtfyClientTask\n");
    /* 
     * Step 1. Pend on the semaphore that would be released by completionCb
     * Step 2. De Q
     * Step 3. Notify the client
     */
    /* Tasks main loop */
    while (TRUE)
    {
        rtnVal = 0x0;
        /* Loop required, on errors we would break out first loop. Providing
         * a clean exit */
        while (TRUE)
        {
            GT_0trace(PSTrace, GT_INFO, "PS : Waiting for completion CB\n");
            /* Step 1. Pend on the semaphore - remote agents / clients would
             * wake us up on a notify */
            if ((Semaphore_pend(pPSrvIoTask->tSem, BIOS_WAIT_FOREVER)) == FALSE)
            {
                /* Can occur only on time outs - which should not happen.
                 * Nothing much could be done 0 go back and wait */
                break;
            }
            GT_0trace(PSTrace, GT_INFO, "PS : Got a CB\n");
            /* Step 2. - Get one command */
            rtnVal = VpsUtils_queGet(pPSrvIoTask->cmdQHndl,
                                     (Ptr *) (&tempVar), 0x01u, BIOS_NO_WAIT);
            if ((rtnVal != 0x0) || (tempVar == 0x0))
            {
                /* 
                 * This should not occurs - unless some one woke us up with
                 * no CB - Discard this - go back and wait for another CB.
                 */
                GT_0trace(PSTrace, GT_ERR, "PS : Some one woke us up \n");
                GT_0trace(PSTrace, GT_ERR, "PS : no CB available\n");
                GT_0trace(PSTrace, GT_ERR, "PS : Ignoring\n");
            }
            else
            {
                pPSrvCb = (ISS_PSrvCallback *) tempVar;

                rtnVal = Notify_sendEvent(pPSrvCb->sysLnkProcId,
                                          ISS_PSRV_IPC_LINE_ID_USED,
                                          pPSrvCb->sysLnkNtyNo,
                                          (UInt32) tempVar, TRUE);
            }
            break;
        }
    }
#endif                                                     /* ISS_PS_TASK_4_CB 
                                                            */
}

/**
 * Fix Me  TBD Yet to be done Wish list
 *
 * 1. Covert all absolute address passed by remote agent to M3 ISSS virtual
 *    memory.
 *    Assumption is on M3 MMU will be disabled, however if enabled, PS will not
 *    break. However the individual drivers might also requires updates to
 *    handle enabled MMU - Nothing to fix
 *
 * 2. On FVID2 stream error, not handled in this. Might require an update on
 *     FVID2 interface.
 * 8. DE-Initialize - There could be scenarios where clients/agent still have
 *    active streams. What to do.
 *    Deleting by force could render that stream useless, further initializing
 *    again (with same or diff params) could mark the used notify ID as free
 *    which could break that stream again.
 * 10. Add another API that would allow addition of clients even after the
 *     proxy server has been initialized.
 */
