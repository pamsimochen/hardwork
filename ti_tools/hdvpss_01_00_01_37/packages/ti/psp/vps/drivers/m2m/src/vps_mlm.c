/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_mlm.c
 *
 * \brief Mem2Mem List Manager
 * This file implements List Manager for the Memory to Memory driver.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Number of Memory2Memory driver List Manager Instances. Current MLM
 *  implementation supports 6 mem2mem drivers so this is fixed to 6.
 *  Note: This macro can be changed as and when requirement changes.
 */
#define MLM_NUM_INSTANCES               (6u)

/**
 *  \brief This enum defines maximum number of handles an mlm instance can
 *  support. This enum is mainly used in allocating control descriptor memory.
 *  Instead of using a single free pool of free control descriptors, MLM
 *  reserves set of free control descriptors per instance. This is because
 *  all the required set of control descriptors are created when client calls
 *  register API, so that it is not required to create these descriptors when
 *  mlm_submit API is called. Also, mem2mem driver is never going to enqueue
 *  more than these number of requests. This needs to be changed when number
 *  handles supported by the mem2mem driver is changed.
 */
#define MLM_NUM_REQ_PER_INST            (10u)

/**
 * \brief Defined maximum number of Send Interrupt control descriptor.
 */
#define MLM_NUM_SI_CTRL_DESC            (1u)

/**
 * \brief Defined maximum number of Reload Control descriptor in the MLM's
 *  descriptor memory.
 */
#define MLM_NUM_RL_CTRL_DESC            (1u)

#define MLM_SOC_EVENT                   (VPSHAL_VPDMA_SOC_NOTACTIVE)
#define MLM_SOC_LINE_COUNT              (0u)
#define MLM_SOC_PIXEL_COUNT             (0u)

#define MLM_SEM_TIMEOUT                 (BIOS_WAIT_FOREVER)

/**
 *  \brief VEM client priority for the Send interrupt and list
 *  complete interrupt
 */
#define MLM_VEM_SI_PRIORITY             (VEM_PRIORITY0)
#define MLM_VEM_LC_PRIORITY             (VEM_PRIORITY1)

/**
 *  \brief MLM keeps a list of control descriptos, which it attaches at
 *  the end of every request. This macro is used for defining size of these
 *  set of control descriptors.
 */
#define MLM_CTRL_DESC_MEMORY         (                                         \
            (MLM_NUM_SI_CTRL_DESC * sizeof(VpsHal_VpdmaSendIntrDesc)) +        \
            (MLM_MAX_CHANNELS * sizeof(VpsHal_VpdmaSyncOnClientDesc)) +        \
            (MLM_NUM_RL_CTRL_DESC * sizeof(VpsHal_VpdmaReloadDesc)))


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Mlm_InstInfo
 * \brief Structure containing information regarding each instances of mlm.
 */
typedef struct Mlm_InstInfo_t
{
    UInt32               instNum;
    /**< MLM Instance Number. Each instance of the MLM is given an instance
         number. */
    UInt32               listNum;
    /**< VPDMA List Number. Each MLM instance requires a dedicated list.
         It uses this list to submit the request memory to memory driver
         request for the operations to the VPDMA.*/
    UInt32               isListAllocated;
    /**< Flag to indicate whether the list is allocated or not for this mlm
         instance. */
    UInt32               siEventNum[MLM_NUM_SI_CTRL_DESC];
    /**< VPDMA Send Interrupt number. Mlm uses send interrupt control
         descriptor to give request completion callback to the clients. */
    UInt32               numSiSrc;
    /**< Number of send interrupt sources to be used for this instance of
         the mlm */
    Mlm_ClientInfo       clientInfo;
    /**< Client Information Structure */
    UInt8               *descPtr;
    /**< Pointer to the start of the instance descriptors */
    UInt32               ctrlDescSize;
    /**< This keeps track of ctrl descriptor size. Depending on the
         linking Si and reload descriptor is available or not and
         number of sync on channel control descriptors, size
         of the descriptors will change */
    UInt32               isRegistered;
    /**< Indicates whethe client is registered or not. It is also used to
         indicate the free MLM instance, so if the client is not registered
         to an instance, that instance is free and client can register to
         this MLM instance. */
    VpsUtils_Handle      freeCtrlDesc;
    /**< Queue of free control descriptor memory. MLM keeps a pool of
         memory for control descriptors. This queue is a list of free
         control descriptor pool. */
    VpsUtils_Handle      requestQueue;
    /**< Queue of allocated control descriptor memory. MLM keeps a pool of
         memory for control descriptors. This queue is a list of allocated
         control descriptor pool. */
    VpsUtils_Handle      activeQueue;
    /**< Queue of requests, which are submitted for the processing to
         the VPDMA */
    UInt32               isListActive;
    /**< Indicates whether list is active or not. Flag to indicate
         whether the given list is submitted to the VPDMA
         or not. This is mainly used when there are no request in the
         queue and client calls submit function. Since this flag will be set
         in this case, list can be directly submitted to the VPDMA. */
    Void                *vemSendIntrHandle;
    /**< Vem Handle for the Send Interrupt event handler */
    Void                *vemListCmplHandle;
    /**< Vem Handle for the List Complete event handler */
} Mlm_InstInfo;

/**
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 */
typedef struct Mlm_QueueObj_t
{
    VpsUtils_Node             qElem;
    /**< VPS utils queue element used in node addition. */
    UInt8                    *descPtr;
    /**< Pointer to the start of the complete control descriptors */
    Ptr                       ctrlDescPtr;
    /**< Pointer to the memory where control descriptors for this
         queue elements will be stored */
    Ptr                       siDescPtr;
    /**< Points the memory location where Send Interrupt descriptor
         is to be created */
    Ptr                       rlDescPtr;
    /**< Points the memory location where reload descriptor is to be created */
    UInt32                    ctrlDescSize;
    /**< Keeps track of the size of the control descriptors */
    Mlm_SubmitReqInfo        *submitReqInfo;
    /**< Structure containing submit request information */
    Mlm_InstInfo             *mlmInstInfo;
    /**< Pointer to the mlmInstInfo structure */
} Mlm_QueueObj;

typedef struct Mlm_Obj_t
{
    Semaphore_Handle  semHandle;
    /**< Semaphore handle for protecting register/unregister API */
} Mlm_Obj;


/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

static Mlm_InstInfo *mlmGetFreeMlmInstance(void);

static Int32 mlmAllocResource(Mlm_InstInfo *mlmInstInfo);

static Void mlmFreeResource(Mlm_InstInfo *mlmInstInfo);

static Int32 mlmAllocQueues(Mlm_InstInfo *mlmInstInfo);

static Int32 mlmFreeQueues(Mlm_InstInfo *mlmInstInfo);

static Void mlmCreateCtrlDescs(Mlm_InstInfo *mlmInstInfo);

static Void mlmSendIntrIsr(const UInt32 *event, UInt32 numEvents, Ptr arg);

static Void mlmListCompleteIsr(const UInt32 *event, UInt32 numEvents, Ptr arg);

static mlmGetRlDescInfo(Mlm_InstInfo *mlmInstInfo,
                        Mlm_QueueObj *qObj);

/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

/**
 * \brief global object of mlm containing all parameters not specific to
 *  any instance
 */
static Mlm_Obj MlmObj;

/**
 *  \brief Global variables of MLM Instaces
 */
static Mlm_InstInfo MlmInstInfo[MLM_NUM_INSTANCES];

/**
 *  \brief A fixed set of queue objests are available for mlm instances.
 *  This has been pre-allocated in order to avoid changing utils file
 *  for number of queue objects. Also these are allocated per instances
 *  instead of set of queue objects.
 */
static Mlm_QueueObj MlmQueueObj[MLM_NUM_INSTANCES][MLM_NUM_REQ_PER_INST];


/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */


/**
 *  Mlm_init
 *  \brief Function to initialize VPS Memory List Manager. It initializes
 *  all global variables and keeps it ready.
 *
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Mlm_init(Ptr arg)
{
    Int32               retVal = 0;
    UInt32              instCnt;
    Mlm_InstInfo       *mlmInstInfo = NULL;
    Semaphore_Params    semParams;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");

    /* For each of instances of MLM, create a queue of free control
       descriptor and allocated descriptor */
    for (instCnt = 0u; instCnt < MLM_NUM_INSTANCES; instCnt ++)
    {
        mlmInstInfo = &(MlmInstInfo[instCnt]);

        GT_assert(MlmTrace, (NULL != mlmInstInfo));

        /* Initialize the instance with zero */
        VpsUtils_memset(mlmInstInfo, 0u, sizeof(Mlm_InstInfo));

        /* Initialize all bool variables to FALSE */
        mlmInstInfo->isRegistered = FALSE;
        mlmInstInfo->isListAllocated = FALSE;
        mlmInstInfo->isListActive = FALSE;
        mlmInstInfo->instNum = instCnt;
    }

    /* Get the semaphore handle */
    Semaphore_Params_init(&semParams);
    MlmObj.semHandle = Semaphore_create(1u, &semParams, NULL);
    if (NULL == MlmObj.semHandle)
    {
        retVal = -1;
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (retVal);
}



/**
 *  Mlm_deInit
 *  \brief Function to de-initialize VPS Memory List Manager.
 *
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Mlm_deInit(Ptr arg)
{
    GT_0trace(MlmTrace, GT_ENTER, ">>>>");

    Semaphore_delete(&(MlmObj.semHandle));

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (0);
}



/**
 *  Mlm_register
 *  \brief Function to register memory driver to the MLM.
 *
 *  \param clientInfo    Pointer to client Info structure
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              returns handle to the mlm client in case of success
 *                       returns null in case of error.
 */
Mlm_Handle Mlm_register(const Mlm_ClientInfo *clientInfo,
                        Ptr arg)
{
    Int32            retVal = 0;
    Mlm_Handle       mlmHandle = NULL;
    Mlm_InstInfo    *mlmInstInfo = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");

    GT_assert(MlmTrace, (NULL != clientInfo));

    /* Get the semaphore */
    Semaphore_pend(MlmObj.semHandle, (UInt32)MLM_SEM_TIMEOUT);

    /* Find a free mlm instance */
    mlmInstInfo = mlmGetFreeMlmInstance();

    if (NULL != mlmInstInfo)
    {
        /* Since only one send interrupt is used in the set of control
           descriptors, Following is set to one. This should be changed
           when using multiple send interrupts per control descriptor set. */
        mlmInstInfo->numSiSrc = MLM_NUM_SI_CTRL_DESC;

        /* Get the free VPDMA list and Send Interrupt source number from
           the resource manager */
        retVal = mlmAllocResource(mlmInstInfo);
    }
    else
    {
        GT_setFailureReason(MlmTrace,
                            GT_INFO,
                            (Char *)__FUNCTION__,
                            retVal,
                            (Char *)"There are no free Mlm Instances\n");
        /* Since there are no free mlm instances, return value should be
           negative so that no other processing will be done */
        retVal = -1;
    }

    if (0 == retVal)
    {
        /* Register Event Handles for the Send interrupts with the
           higher priority than list complete */
        mlmInstInfo->vemSendIntrHandle = Vem_register(
                                   VEM_EG_SI,
                                   mlmInstInfo->siEventNum,
                                   mlmInstInfo->numSiSrc,
                                   MLM_VEM_SI_PRIORITY,
                                   mlmSendIntrIsr,
                                   (Ptr) mlmInstInfo);
        /* Register List Complete Event Handler */
        mlmInstInfo->vemListCmplHandle = Vem_register(
                                   VEM_EG_LISTCOMPLETE,
                                   &(mlmInstInfo->listNum),
                                   1u,
                                   MLM_VEM_LC_PRIORITY,
                                   mlmListCompleteIsr,
                                   (Ptr) mlmInstInfo);

        if ((NULL == mlmInstInfo->vemSendIntrHandle) ||
            (NULL == mlmInstInfo->vemListCmplHandle))
        {
            GT_setFailureReason(MlmTrace,
                                GT_INFO,
                                (Char *)__FUNCTION__,
                                retVal,
                                (Char *)"VEM Registration Failed\n");
            retVal = -1;
        }
    }

    if (0 == retVal)
    {
        /* Create two queues, one for storing free memory pool and other
           for allocated memory pools.
           Get the nodes from the VpsUtils for this mlm instance and add
           them as the free queue.
           This will also set pointer in the queue object at the
           appropriate location in mlm control descriptors */
        retVal = mlmAllocQueues(mlmInstInfo);

        if (0 != retVal)
        {
            GT_setFailureReason(MlmTrace,
                                GT_INFO,
                                (Char *)__FUNCTION__,
                                retVal,
                                (Char *)"Queue Allocation Failed\n");
        }
    }

    if (0 == retVal)
    {
        /* *Store client information structure in mlm instance structure */
        VpsUtils_memcpy(&(mlmInstInfo->clientInfo),
                        (Ptr) clientInfo,
                        sizeof(Mlm_ClientInfo));

        mlmInstInfo->isRegistered = TRUE;

        mlmInstInfo->ctrlDescSize = MLM_CTRL_DESC_MEMORY;

        /* Create Send Interrupt control descriptor in the descriptor memory */
        mlmCreateCtrlDescs(mlmInstInfo);

        mlmHandle = (Mlm_InstInfo *)mlmInstInfo;
    }

    /* Free up the resources if return value is negative */
    if (0 != retVal)
    {
        if (NULL != mlmInstInfo)
        {
            mlmFreeQueues(mlmInstInfo);

            mlmFreeResource(mlmInstInfo);

            if (NULL != mlmInstInfo->vemSendIntrHandle)
            {
                Vem_unRegister(mlmInstInfo->vemSendIntrHandle);
            }
            if (NULL != mlmInstInfo->vemListCmplHandle)
            {
                Vem_unRegister(mlmInstInfo->vemListCmplHandle);
            }
        }
    }

    /* Release Semaphore */
    Semaphore_post(MlmObj.semHandle);

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
    return (mlmHandle);
}



/**
 *  Mlm_unRegister
 *  \brief Function to un-register memory driver to the MLM.
 *
 *  \param mlmHandle     Handle to Memory to Memory driver client
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Mlm_unRegister(Mlm_Handle mlmHandle)
{
    Int32           retVal = 0;
    Mlm_InstInfo   *mlmInstInfo = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");

    GT_assert (MlmTrace, (NULL != mlmHandle));

    /* Get the semaphore */
    Semaphore_pend(MlmObj.semHandle, (UInt32)MLM_SEM_TIMEOUT);

    mlmInstInfo = (Mlm_InstInfo *)mlmHandle;

    /* Note: Make sure that there are no request, which are active or
       submitted to mlm but no active. If it is the case, currently
       mlm asserts for these case. This should be taken care by the
       memory driver. */
    GT_assert(MlmTrace,
              (TRUE == VpsUtils_isListEmpty(mlmInstInfo->activeQueue)));
    GT_assert(MlmTrace,
              (TRUE == VpsUtils_isListEmpty(mlmInstInfo->requestQueue)));

    /* Free up the queues */
    mlmFreeQueues(mlmInstInfo);

    /* FRee the resources */
    mlmFreeResource(mlmInstInfo);

    /* Clear the clientInfo structure */
    VpsUtils_memset((Ptr)&(mlmInstInfo->clientInfo),
                    0,
                    sizeof(Mlm_ClientInfo));

    if (NULL != mlmInstInfo->vemSendIntrHandle)
    {
        Vem_unRegister(mlmInstInfo->vemSendIntrHandle);
        mlmInstInfo->vemSendIntrHandle = NULL;
    }
    if (NULL != mlmInstInfo->vemListCmplHandle)
    {
        Vem_unRegister(mlmInstInfo->vemListCmplHandle);
        mlmInstInfo->vemListCmplHandle = NULL;
    }

    mlmInstInfo->isRegistered = FALSE;

    /* Release Semaphore */
    Semaphore_post(MlmObj.semHandle);

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (retVal);
}



/**
 *  Mlm_submit
 *  \brief Function to submit a requst to the MLM.
 *
 *  \param mlmHandle     Handle to Memory to Memory driver client
 *  \param submit        Pointer to submitRequestInfo structure containing
 *                       pointer to the list to be submitted.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Mlm_submit(Mlm_Handle mlmHandle, Mlm_SubmitReqInfo *submitReq)
{
    Int32                retVal = 0;
    UInt32               cookie;
    UInt8               *ctrlDescPtr;
    Mlm_InstInfo        *mlmInstInfo = NULL;
    VpsUtils_Node       *qElem = NULL, *prevQElem = NULL;
    Mlm_QueueObj        *qObj = NULL, *prevQObj = NULL;
    Mlm_SubmitReqInfo   *prevSmReq;
    UInt32               isEmpty = FALSE;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");

    GT_assert (MlmTrace, (NULL != mlmHandle));
    GT_assert (MlmTrace, (NULL != submitReq));

    mlmInstInfo = (Mlm_InstInfo *)mlmHandle;

    /* Disable the interrupts */
    cookie = Hwi_disable();

    if (MLM_SRT_USE_SOC == submitReq->submitReqType)
    {
        /* This condition checking is needed only if mlm adds sync
           on channel control descritor */
        if ((FALSE == mlmInstInfo->clientInfo.isChannelStatic) &&
            ((submitReq->numChannels > MLM_MAX_CHANNELS) ||
            (0u == submitReq->numChannels)))
        {
            GT_setFailureReason(MlmTrace,
                                GT_INFO,
                                (Char *)__FUNCTION__,
                                retVal,
                                (Char *)"Wrong Number of Channels\n");
            retVal = -1;
        }
    }

    /* Get a free Queue element from the element queue */
    qElem = VpsUtils_unLinkNodeFromHead(mlmInstInfo->freeCtrlDesc);

    if (NULL == qElem)
    {
        GT_setFailureReason(MlmTrace,
                            GT_INFO,
                            (Char *)__FUNCTION__,
                            retVal,
                            (Char *)"No Free Control Desc Memory\n");
        retVal = -1;
    }

    if (0 == retVal)
    {
        qObj = (Mlm_QueueObj *) (qElem->data);

        GT_assert(MlmTrace, (NULL != qObj));

        /* Assign pointer to requestinfo to qObject */
        qObj->submitReqInfo = submitReq;

        /* Create Dummy descriptor for the last client */
        VpsHal_vpdmaCreateDummyDesc(qObj->rlDescPtr);

        /* Calculate the Control Descriptor pointer and size of the
           mlm control descriptors and store them in the queue object */
        mlmGetRlDescInfo(mlmInstInfo, qObj);

        /* Create Reload Descriptor in the last request to point
           to mlm control descriptors */
        VpsHal_vpdmaCreateRLCtrlDesc(submitReq->lastRlDescAddr,
                                     qObj->ctrlDescPtr,
                                     qObj->ctrlDescSize);

        if (TRUE == mlmInstInfo->isListActive)
        {
            /* Check to see if requests can be linked or not */
            isEmpty = VpsUtils_isListEmpty(mlmInstInfo->requestQueue);

            /* if request queue is not empty, link this request with the
               other request */
            if (FALSE == isEmpty)
            {
                /* Get the tail node from the request queue */
                prevQElem = VpsUtils_getTailNode(mlmInstInfo->requestQueue);
                GT_assert (MlmTrace, (NULL != prevQElem));

                prevQObj = (Mlm_QueueObj *) (prevQElem->data);
                GT_assert (MlmTrace, (NULL != prevQObj));

                prevSmReq = prevQObj->submitReqInfo;
                GT_assert (MlmTrace, (NULL != prevSmReq));

                /* Add size of the reload descriptor to the last requests
                   reload descriptor */
                if (MLM_SRT_USE_SOC == prevSmReq->submitReqType)
                {
                    /* Update the size of the descriptors here */
                    /* If the previous request requires use of Soc,
                       point reload to soc control descriptor and size should
                       be all descriptors. */
                    ctrlDescPtr = prevQObj->ctrlDescPtr;
                    prevQObj->ctrlDescSize += (MLM_NUM_SI_CTRL_DESC *
                        sizeof(VpsHal_VpdmaSendIntrDesc)) + (MLM_NUM_RL_CTRL_DESC *
                        sizeof(VpsHal_VpdmaReloadDesc));
                }
                else
                {
                    /* If the previous request does not require use of Soc,
                       point reload to Send Interrupt control descriptor
                       and size should be one less control descriptor. */
                    ctrlDescPtr = prevQObj->siDescPtr;
                    prevQObj->ctrlDescSize += (MLM_NUM_SI_CTRL_DESC *
                    sizeof(VpsHal_VpdmaSendIntrDesc));
                }

                VpsHal_vpdmaCreateRLCtrlDesc(
                            prevQObj->submitReqInfo->lastRlDescAddr,
                            ctrlDescPtr,
                            prevQObj->ctrlDescSize);

                /* Point to reload descriptor of the control descriptor set
                   to this new request */
                VpsHal_vpdmaCreateRLCtrlDesc(
                            prevQObj->rlDescPtr,
                            submitReq->reqStartAddr,
                            submitReq->firstChannelSize);
            }
            /* Add this request into the request queue */
            VpsUtils_linkNodeToTail(mlmInstInfo->requestQueue, qElem);
        }
        else /* Submit the request as the list is not active */
        {
            /* Request and active Queue should be empty when list
               is not active */
            GT_assert(MlmTrace,
                (TRUE == VpsUtils_isListEmpty(mlmInstInfo->requestQueue)));
            GT_assert(MlmTrace,
                (TRUE == VpsUtils_isListEmpty(mlmInstInfo->activeQueue)));

            /* Put the request into the active queue */
            VpsUtils_linkNodeToTail(mlmInstInfo->activeQueue, qElem);

            /* Submit the list to the VPDMA */
            VpsHal_vpdmaPostList(mlmInstInfo->listNum,
                                 VPSHAL_VPDMA_LT_NORMAL,
                                 submitReq->reqStartAddr,
                                 submitReq->firstChannelSize,
                                 TRUE);

            /* Mark the list as active now */
            mlmInstInfo->isListActive = TRUE;
        }
    }
    /* Enable interrupts */
    Hwi_restore(cookie);

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (retVal);
}



/**
 * \brief Function to get the free mlm instance from the list of mlm instances
 */
static Mlm_InstInfo *mlmGetFreeMlmInstance(void)
{
    UInt32 mlmInstCnt;
    Mlm_InstInfo *mlmInstInfo = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    /* Find free mlm instance structure */
    for (mlmInstCnt = 0u; mlmInstCnt < MLM_NUM_INSTANCES; mlmInstCnt ++)
    {
        mlmInstInfo = &(MlmInstInfo[mlmInstCnt]);

        if (FALSE == mlmInstInfo->isRegistered)
        {
            /* Found a free mlm Instace */
            break;
        }
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
    return (mlmInstInfo);
}



/**
 *  \brief Function to allocate required resources from the resource
 *  manager for given mlm instance.
 */
static Int32 mlmAllocResource(Mlm_InstInfo *mlmInstInfo)
{
    Int32 retVal = 0;
    UInt8 listNum;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    GT_assert(MlmTrace, (NULL != mlmInstInfo));
    GT_assert(MlmTrace, (TRUE != mlmInstInfo->isListAllocated));

    retVal = Vrm_allocList(&listNum, VRM_LIST_TYPE_M2M);

    if (0 == retVal)
    {
        mlmInstInfo->listNum = listNum;

        retVal = Vrm_allocSendIntr(mlmInstInfo->siEventNum,
                                   mlmInstInfo->numSiSrc);

        if (0 != retVal)
        {
            /* There is some error occured so free up the list and return
               error */
            Vrm_releaseList(mlmInstInfo->listNum);
        }
        else
        {
            mlmInstInfo->isListAllocated = TRUE;
        }
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
    return (retVal);
}



static Void mlmFreeResource(Mlm_InstInfo *mlmInstInfo)
{
    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    GT_assert(MlmTrace, (NULL != mlmInstInfo));

    if (TRUE == mlmInstInfo->isListAllocated)
    {
        Vrm_releaseList(mlmInstInfo->listNum);

        Vrm_releaseSendIntr(mlmInstInfo->siEventNum, mlmInstInfo->numSiSrc);

        mlmInstInfo->isListAllocated = FALSE;
    }
    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
}



/**
 *  \brief Function to allocate the queues and queue elements for the given
 *  mlm instance
 */
static Int32 mlmAllocQueues(Mlm_InstInfo *mlmInstInfo)
{
    Int32           retVal = -1;
    UInt32          elemCnt, mlmInstNum;
    UInt32          rlDescOffset, siDescOffset, ctrlDescOffset;
    Mlm_QueueObj    *qObj = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    GT_assert(MlmTrace, (NULL != mlmInstInfo));

    /* Create free, allocated and active queues for this mlm instance */
    mlmInstInfo->freeCtrlDesc = VpsUtils_createLinkList(VPSUTILS_LLT_DOUBLE,
                                                        VPSUTILS_LAM_PRIORITY);
    mlmInstInfo->requestQueue = VpsUtils_createLinkList(VPSUTILS_LLT_DOUBLE,
                                                        VPSUTILS_LAM_PRIORITY);
    mlmInstInfo->activeQueue = VpsUtils_createLinkList(VPSUTILS_LLT_DOUBLE,
                                                       VPSUTILS_LAM_PRIORITY);
    mlmInstInfo->descPtr = VpsUtils_allocDescMem(
                               MLM_NUM_REQ_PER_INST * MLM_CTRL_DESC_MEMORY,
                               VPSHAL_VPDMA_DESC_BYTE_ALIGN);

    /* Calculate Offsets for the Send interrupt control desc. and reload
       ctrl desc. Reload Control Descriptor is stored at the end of
       control descripor memory and Send interrupt control descriptor
       is just before Reload Control Descriptor */
    rlDescOffset = MLM_CTRL_DESC_MEMORY - (MLM_NUM_RL_CTRL_DESC *
                    sizeof(VpsHal_VpdmaReloadDesc));
    siDescOffset = rlDescOffset - (MLM_NUM_SI_CTRL_DESC *
                    sizeof(VpsHal_VpdmaSendIntrDesc));
    /* Assuming atleast single control descriptor is available */
    ctrlDescOffset = siDescOffset - sizeof(VpsHal_VpdmaSyncOnChannelDesc);

    if ((NULL != mlmInstInfo->freeCtrlDesc) &&
        (NULL != mlmInstInfo->requestQueue) &&
        (NULL != mlmInstInfo->activeQueue) &&
        (NULL != mlmInstInfo->descPtr))
    {
        /* Get the mlm instace number */
        mlmInstNum = mlmInstInfo->instNum;

        for (elemCnt = 0u; elemCnt < MLM_NUM_REQ_PER_INST; elemCnt ++)
        {
            /* Get the Queue Object */
            qObj = &(MlmQueueObj[mlmInstNum][elemCnt]);

            GT_assert(MlmTrace, (NULL != qObj));

            /* Assign Memory Pool as the data to the queue element */
            qObj->qElem.data = (Void *)(qObj);

            qObj->descPtr = mlmInstInfo->descPtr +
                                (elemCnt * MLM_CTRL_DESC_MEMORY);

            /* Store the Pointer to sync on channel, send interrupt and
               reload control descriptor. Assuming there is atleast
               single control descriptor. */
            qObj->ctrlDescPtr = (Ptr) (qObj->descPtr + ctrlDescOffset);
            qObj->siDescPtr = (Ptr) (qObj->descPtr + siDescOffset);
            qObj->rlDescPtr = (Ptr) (qObj->descPtr + rlDescOffset);

            /* Initialize pointer to instance info structure in the
               queue object */
            qObj->mlmInstInfo = mlmInstInfo;

            /* Add these control descriptors memories to the queue of
               free descriptors */
            VpsUtils_linkNodeToHead(mlmInstInfo->freeCtrlDesc, &(qObj->qElem));

            retVal = 0;
        }
    }
    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (retVal);
}



/**
 *  \brief Function to free the queues and queue elements for the given
 *  mlm instance
 */
static Int32 mlmFreeQueues(Mlm_InstInfo *mlmInstInfo)
{
    Int32           retVal = -1;
    UInt32          qElemCnt;
    VpsUtils_QElem *qElem = NULL;
    Mlm_QueueObj    *qObj = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    GT_assert(MlmTrace, (NULL != mlmInstInfo));
    GT_assert(MlmTrace, (NULL != mlmInstInfo->freeCtrlDesc));
    GT_assert(MlmTrace, (NULL != mlmInstInfo->requestQueue));
    GT_assert(MlmTrace, (NULL != mlmInstInfo->activeQueue));

    /* Free up the Queues */
    if (NULL != mlmInstInfo->freeCtrlDesc)
    {
        for (qElemCnt = 0u; qElemCnt < MLM_NUM_REQ_PER_INST; qElemCnt ++)
        {
            /* Assumption here is that when this function is called, all the
               requests are completed and all control descriptors memory for
               all request are free and queued in the free queue so that they
               can be dequeued here from the free queue and released. */
            qElem = VpsUtils_unLinkNodeFromHead(mlmInstInfo->freeCtrlDesc);
            GT_assert(MlmTrace, (NULL != qElem));

            qObj = (Mlm_QueueObj *)(qElem->data);

            qObj->descPtr = NULL;
            qObj->ctrlDescPtr = NULL;
            qObj->submitReqInfo = NULL;
            qObj->rlDescPtr = NULL;
            qObj->siDescPtr = NULL;
            qObj->ctrlDescSize = 0u;

            qElem->data = NULL;
        }

        VpsUtils_deleteLinkList(mlmInstInfo->freeCtrlDesc);
        mlmInstInfo->freeCtrlDesc = NULL;
    }
    if (NULL != mlmInstInfo->requestQueue)
    {
        VpsUtils_deleteLinkList(mlmInstInfo->requestQueue);
        mlmInstInfo->requestQueue = NULL;
    }
    if (NULL != mlmInstInfo->activeQueue)
    {
        VpsUtils_deleteLinkList(mlmInstInfo->activeQueue);
        mlmInstInfo->activeQueue = NULL;
    }
    if (NULL != mlmInstInfo->descPtr)
    {
        VpsUtils_freeDescMem(
            mlmInstInfo->descPtr,
            MLM_NUM_REQ_PER_INST * MLM_CTRL_DESC_MEMORY);
        mlmInstInfo->descPtr = NULL;
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");

    return (retVal);
}



/**
 *  \brief Function to create Control descriptors in the descriptors memory.
 *  This is created once because these control descriptors will not be changed
 *  after client is registered. Only Reload Control descriptor at the
 *  end of the mlm control descriptor will be changed to point to new memory
 */
static Void mlmCreateCtrlDescs(Mlm_InstInfo *mlmInstInfo)
{
    UInt32          elemCnt, siCnt;
    Mlm_QueueObj   *qObj = NULL;
    UInt8          *ctrlDescPtr = NULL;
    UInt32          mlmInst, chanCnt;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    GT_assert(MlmTrace, (NULL != mlmInstInfo));

    mlmInst = mlmInstInfo->instNum;

    for (elemCnt = 0u; elemCnt < MLM_NUM_REQ_PER_INST; elemCnt ++)
    {
        qObj = &(MlmQueueObj[mlmInst][elemCnt]);

        ctrlDescPtr = (UInt8 *)qObj->siDescPtr;
        for (siCnt = 0u; siCnt < mlmInstInfo->numSiSrc; siCnt++)
        {
            /* Create Send Interrupt Control Descriptor */
            VpsHal_vpdmaCreateSICtrlDesc((Ptr)ctrlDescPtr,
                                         mlmInstInfo->siEventNum[0u]);
            ctrlDescPtr = ctrlDescPtr + sizeof(VpsHal_VpdmaSendIntrDesc);
        }

        /* If the channels are static, create the control descriptors
           here itself, so that there is no need to create them everytime */
        if (TRUE == mlmInstInfo->clientInfo.isChannelStatic)
        {
            ctrlDescPtr = (UInt8 *)qObj->siDescPtr;
            GT_assert (MlmTrace, (NULL != ctrlDescPtr));

            /* Create Sync on Channel Control descriptors */
            for (chanCnt = 0u; chanCnt < mlmInstInfo->clientInfo.numChannels;
                    chanCnt ++)
            {
                ctrlDescPtr = ctrlDescPtr -
                                sizeof(VpsHal_VpdmaSyncOnChannelDesc);
                GT_assert (MlmTrace, (NULL != ctrlDescPtr));

                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    (Ptr)ctrlDescPtr,
                    mlmInstInfo->clientInfo.channelNum[chanCnt]);
            }
            /* Store Pointer to the control descriptors */
            qObj->ctrlDescPtr = ctrlDescPtr;

            /* Store the size of the control descriptors. Here the
               size includes only set of sync on channel control
               descriptors. It is because if this is the last
               request, there is no need to have SI control
               descriptor. List Complete Interrupt will provide
               client callback. */
            qObj->ctrlDescSize =
                    (UInt8 *)qObj->siDescPtr - (UInt8 *)ctrlDescPtr;
        }

        /* Here Configuration descriptor is used as a dummy control
           descriptor. For the last request, these is no need to have a
           relead control descriptor. So instead of reload
           descriptor, dummy configuration descriptor is created. */
        VpsHal_vpdmaCreateDummyDesc(qObj->rlDescPtr);
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
}


//#define HDVPSS_MLM_DEBUG_PRINT

static Void mlmSendIntrIsr(const UInt32 *event, UInt32 numEvents, Ptr arg)
{
    Mlm_InstInfo *mlmInstInfo = (Mlm_InstInfo *)arg;
    VpsUtils_Node *qElem = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    if (TRUE == mlmInstInfo->isListActive)
    {
        /* Just call the callback function of the client */
        /* Remove the request from the active queue */
        qElem = VpsUtils_unLinkNodeFromHead(mlmInstInfo->activeQueue);

        if(qElem==NULL || mlmInstInfo->clientInfo.reqCompleteCb==NULL)
        {
            #ifdef HDVPSS_MLM_DEBUG_PRINT
            Vps_rprintf(" #### HDVPSS: MLM: ERROR: qElem == NULL in mlmSendIntrIsr() !!! #### \n");
            #endif
        }
        else
        {
            GT_assert(MlmTrace, (NULL != qElem));
            GT_assert(MlmTrace, (NULL != mlmInstInfo->clientInfo.reqCompleteCb));

            /* Call the request completion callback function of
               the client */
            mlmInstInfo->clientInfo.reqCompleteCb(
                    ((Mlm_QueueObj *)(qElem->data))->submitReqInfo);
            /* Set the request pointer to null */
            ((Mlm_QueueObj *)(qElem->data))->submitReqInfo = NULL;

            /* Add the free queue node to the free list */
            VpsUtils_linkNodeToHead(mlmInstInfo->freeCtrlDesc, qElem);
        }
    }
    else
    {
        /* TODO: How to handle a situation where List Complete happens
           first then send Interrupt */

        /* There is some problem */
        #ifdef HDVPSS_MLM_DEBUG_PRINT
        Vps_rprintf(" #### HDVPSS: MLM: WARNING: mlmSendIntrIsr() mlmInstInfo->isListActive == FALSE !!! #### \n");
        #endif
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
}



static Void mlmListCompleteIsr(const UInt32 *event, UInt32 numEvents, Ptr arg)
{
    Mlm_InstInfo  *mlmInstInfo = (Mlm_InstInfo *)arg;
    UInt32         isEmpty;
    VpsUtils_Node *qElem = NULL;
    Mlm_QueueObj  *qObj = NULL;

    GT_0trace(MlmTrace, GT_ENTER, ">>>>");
    /* There is no need to disable interrupts here as VPS gives single
       interrupt to the processor and VEM gives callback to its client
       one by one. If there are multiple interrupts, this should be
       protected */
    if (TRUE == mlmInstInfo->isListActive)
    {
        /* If active queue is not free, either the send interrupt missed or
           list complete interrupt callback called before send interrupt.
           In this case, just call callback api for all the active requests
           and free up the queue */
        isEmpty = VpsUtils_isListEmpty(mlmInstInfo->activeQueue);
        if (FALSE == isEmpty)
        {
            do
            {
                /* Remove the request from the active queue */
                qElem = VpsUtils_unLinkNodeFromHead(mlmInstInfo->activeQueue);
                GT_assert(MlmTrace, (NULL != qElem));

                qObj = (Mlm_QueueObj *)(qElem->data);
                GT_assert(MlmTrace, (NULL != qObj));

                GT_assert(MlmTrace,
                    (NULL != mlmInstInfo->clientInfo.reqCompleteCb));

                /* Call the request completion callback function of
                   the client */
                mlmInstInfo->clientInfo.reqCompleteCb(
                        qObj->submitReqInfo);
                /* Set the request pointer to null */
                qObj->submitReqInfo = NULL;

                /* Add the free queue node to the free list */
                VpsUtils_linkNodeToHead(mlmInstInfo->freeCtrlDesc, qElem);

                isEmpty = VpsUtils_isListEmpty(mlmInstInfo->activeQueue);
            } while(FALSE == isEmpty);

            /* Active queue is free now */
        }

        /* If the request queue is not free, submit new request to the VPDMA
           and mode all queue elements from request queue to active queue */
        isEmpty = VpsUtils_isListEmpty(mlmInstInfo->requestQueue);
        if (FALSE == isEmpty)
        {
            do
            {
                /* Remove the request from the active queue */
                qElem = VpsUtils_unLinkNodeFromHead(mlmInstInfo->requestQueue);
                GT_assert(MlmTrace, (NULL != qElem));

                VpsUtils_linkNodeToTail(mlmInstInfo->activeQueue, qElem);

                isEmpty = VpsUtils_isListEmpty(mlmInstInfo->requestQueue);
            } while (FALSE == isEmpty);

            qElem = VpsUtils_getHeadNode(mlmInstInfo->activeQueue);

            qObj = (Mlm_QueueObj *)(qElem->data);

            /* Submit the list to the VPDMA */
            VpsHal_vpdmaPostList(
                mlmInstInfo->listNum,
                VPSHAL_VPDMA_LT_NORMAL,
                qObj->submitReqInfo->reqStartAddr,
                qObj->submitReqInfo->firstChannelSize,
                TRUE);
        }
        else
        {
            /* Since there are no request in the request queue, mark
               the list as nonactive */
            mlmInstInfo->isListActive = FALSE;
        }
    }
    else
    {
        // There is some problem
        #ifdef HDVPSS_MLM_DEBUG_PRINT
        Vps_rprintf(" #### HDVPSS: MLM: WARNING: mlmListCompleteIsr() mlmInstInfo->isListActive == FALSE !!! #### \n");
        #endif
    }

    GT_0trace(MlmTrace, GT_LEAVE, "<<<<");
}



static mlmGetRlDescInfo(Mlm_InstInfo *mlmInstInfo,
                        Mlm_QueueObj *qObj)
{
    UInt8               *tempDescPtr;
    UInt32               chanCnt;
    UInt32               numChannels;
    Mlm_SubmitReqInfo   *submitReq;

    GT_assert(MlmTrace, (NULL != mlmInstInfo));
    GT_assert(MlmTrace, (NULL != qObj));

    submitReq = qObj->submitReqInfo;

    /* Create Reload Descriptor in the last reload descriptor of
       the request */
    if (MLM_SRT_USE_SOC == submitReq->submitReqType)
    {
        /* Get the number of channels either from clientInfo structure
           or from submitreqInfo depending on whether channels are static
           or not. */
        if (FALSE == mlmInstInfo->clientInfo.isChannelStatic)
        {
            numChannels = submitReq->numChannels;
        }
        else
        {
            numChannels = mlmInstInfo->clientInfo.numChannels;
        }

        /* Get the pointer to the Send Interrupt control descriptor */
        tempDescPtr = (UInt8 *)qObj->siDescPtr;
        GT_assert (MlmTrace, (NULL != tempDescPtr));

        for (chanCnt = 0u; chanCnt < numChannels; chanCnt++)
        {
            tempDescPtr = tempDescPtr - sizeof(VpsHal_VpdmaSyncOnChannelDesc);
            GT_assert (MlmTrace, (NULL != tempDescPtr));

            /* Create SyncOnChannel Control Descriptor at this
               pointer only if channels are non-static */
            if (FALSE == mlmInstInfo->clientInfo.isChannelStatic)
            {
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    (Ptr)tempDescPtr,
                    submitReq->channelNum[chanCnt]);
            }
        }
        /* Store Pointer to the control descriptors */
        qObj->ctrlDescPtr = tempDescPtr;

        /* Store the size of the control descriptors. Here the
           size includes only set of sync on channel control
           descriptors. It is because if this is the last
           request, there is no need to have SI control
           descriptor. List Complete Interrupt will provide
           client callback. */
        qObj->ctrlDescSize =
                (UInt8 *)qObj->siDescPtr - tempDescPtr;
    }
    else
    {
        /* Store Pointer where Reload Pointer should point */
        qObj->ctrlDescPtr = qObj->rlDescPtr;
        /* Control Descriptor size is just one reload descriptor */
        qObj->ctrlDescSize = VPSHAL_VPDMA_CTRL_DESC_SIZE;
    }
}
