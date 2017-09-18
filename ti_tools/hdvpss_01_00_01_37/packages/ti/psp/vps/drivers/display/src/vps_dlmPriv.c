/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_dlmPriv.c
 *
 *  \brief VPS DLM internal file containing helper functions and
 *  allocation objects and functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/drivers/display/vps_dlm.h>
#include <ti/psp/vps/drivers/display/src/vps_dlmPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Total descriptor memory size required per list set in bytes. */
#define DLM_LIST_CTRL_DESC_MEMORY       (DLM_START_LIST_CTRL_DESC_MEMORY       \
                                       + DLM_SOC_LIST_CTRL_DESC_MEMORY         \
                                       + DLM_LM_FID_LIST_CTRL_DESC_MEMORY      \
                                       + DLM_SOR_LIST_CTRL_DESC_MEMORY)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  DLM memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    VpsDlm_List                 listPool[DLM_NUM_INSTANCE];
    /**< Memory pool for the list objects. */

    VpsDlm_Client               clientPool[DLM_NUM_CLIENTS];
    /**< Memory pool for the client objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any client object. */
    UInt32                      clientPoolFlag[DLM_NUM_CLIENTS];
    /**< The flag variable represents whether a client memory is allocated
         or not. */
    VpsUtils_PoolParams         clientPoolPrm;
    /**< Pool params for client object memory. */

    Semaphore_Handle            lockSem;
    /**< Semaphore for protecting allocation and freeing of memory pool
         objects. */
} VpsDlm_PoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpsDlmLinkListCtrlDesc(VpsDlm_List *listObj);
static Void vpsDlmUnLinkListCtrlDesc(VpsDlm_List *listObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief DLM pool objects used for storing pool memories, pool
 *  flags and pool handles.
 */
static VpsDlm_PoolObj VpsDlmPoolObj;

/**
 *  \brief Priority table for the various list control nodes used for
 *  seamless addition to link list.
 */
static const UInt32 VpsDlmListCtrlNodePriorityTbl[DLM_NODES_PER_LIST] =
{
    DLM_SOC_PRIORITY,
    DLM_LMFID_PRIORITY,
    DLM_SOR_PRIORITY
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vpsDlmPrivInit
 *  Initializes DLM objects, allocates memory etc.
 *
 *  Returns VPS_SOK on success else returns appropriate error code.
 */
Int32 vpsDlmPrivInit(void)
{
    Int32               retVal = VPS_SOK;
    UInt8              *tempPtr;
    UInt32              cnt, setCnt;
    VpsDlm_List        *listObj;
    Semaphore_Params    semParams;

    /* Init objects */
    VpsDlmPoolObj.lockSem = NULL;
    VpsUtils_initPool(
        &VpsDlmPoolObj.clientPoolPrm,
        (Void *) VpsDlmPoolObj.clientPool,
        DLM_NUM_CLIENTS,
        sizeof (VpsDlm_Client),
        VpsDlmPoolObj.clientPoolFlag,
        DlmTrace);
    for (cnt = 0u; cnt < DLM_NUM_INSTANCE; cnt++)
    {
        /* Init variables */
        listObj = &VpsDlmPoolObj.listPool[cnt];
        listObj->listNum = DLM_INVALID_LIST_NUM;
        listObj->listSem = NULL;
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            listObj->descMem[setCnt] = NULL;
        }
    }

    /* Allocate memory, split and assign pointers */
    for (cnt = 0u; cnt < DLM_NUM_INSTANCE; cnt++)
    {
        listObj = &VpsDlmPoolObj.listPool[cnt];

        /* Create Semaphore for each of the list - Init value is 1 since
         * used as mutex (to project critical region when updating common
         * variables of list across clients) */
        Semaphore_Params_init(&semParams);
        listObj->listSem = Semaphore_create(1u, &semParams, NULL);
        if (NULL == listObj->listSem)
        {
            GT_0trace(DlmTrace, GT_ERR, "List semaphore create failed!!\n");
            retVal = VPS_EALLOC;
            break;
        }

        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            listObj->descMem[setCnt] = VpsUtils_allocDescMem(
                                           DLM_LIST_CTRL_DESC_MEMORY,
                                           VPSHAL_VPDMA_DESC_BYTE_ALIGN);
            if (NULL == listObj->descMem[setCnt])
            {
                GT_0trace(DlmTrace, GT_ERR, "Desc memory alloc failed!!\n");
                retVal = VPS_EALLOC;
                break;
            }

            /* Assign List Control memory to Start LM FID/SOC descriptors */
            tempPtr = (UInt8 *) listObj->descMem[setCnt];
            listObj->startLmfidDesc[setCnt] =
                (VpsHal_VpdmaToggleLmFidDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaToggleLmFidDesc);
            listObj->startSocDesc[setCnt] =
                (VpsHal_VpdmaSyncOnClientDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaSyncOnClientDesc);
            listObj->startRldDesc[setCnt] =
                (VpsHal_VpdmaReloadDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaReloadDesc);

            /* Assign List Control memory to SOC descriptors */
            listObj->listCtrlNode[setCnt][DLM_SOC_IDX].nodeAddr =
                (Void *) tempPtr;
            listObj->socDesc[setCnt] =
                (VpsHal_VpdmaSyncOnClientDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaSyncOnClientDesc);
            listObj->siDesc[setCnt] = (VpsHal_VpdmaSendIntrDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaSendIntrDesc);
            listObj->listCtrlNode[setCnt][DLM_SOC_IDX].nodeSize =
                DLM_SOC_LIST_CTRL_DESC_MEMORY;
            listObj->listCtrlNode[setCnt][DLM_SOC_IDX].nodeRldDesc =
                (VpsHal_VpdmaReloadDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaReloadDesc);

            /* Assign List Control memory to LM FID descriptors */
            listObj->listCtrlNode[setCnt][DLM_LMFID_IDX].nodeAddr =
                (Void *) tempPtr;
            listObj->lmfidDesc[setCnt] =
                (VpsHal_VpdmaToggleLmFidDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaToggleLmFidDesc);
            listObj->listCtrlNode[setCnt][DLM_LMFID_IDX].nodeSize =
                DLM_LM_FID_LIST_CTRL_DESC_MEMORY;
            listObj->listCtrlNode[setCnt][DLM_LMFID_IDX].nodeRldDesc =
                (VpsHal_VpdmaReloadDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaReloadDesc);

            /* Assign List Control memory to SOR descriptors */
            listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeAddr =
                (Void *) tempPtr;
            listObj->sorDesc[setCnt] = (VpsHal_VpdmaSyncOnRegDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaSyncOnRegDesc);
            listObj->sorIntrCtrlRldDesc[setCnt] =
                (VpsHal_VpdmaReloadDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaReloadDesc);
            /* Leave out the actual list control reload descriptor as there
             * is an intermediate link */
            listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeSize =
                DLM_SOR_LIST_CTRL_DESC_MEMORY - sizeof(VpsHal_VpdmaReloadDesc);
            listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeRldDesc =
                (VpsHal_VpdmaReloadDesc *) tempPtr;
            tempPtr += sizeof (VpsHal_VpdmaReloadDesc);
        }

        if (VPS_SOK != retVal)
        {
            break;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Create Pool Semaphore */
        VpsDlmPoolObj.lockSem = Semaphore_create(1u, &semParams, NULL);
        if (NULL == VpsDlmPoolObj.lockSem)
        {
            GT_0trace(DlmTrace, GT_ERR, "Semaphore create failed!!\n");
            retVal = VPS_EALLOC;
        }
    }

    /* Deinit if error occurs */
    if (VPS_SOK != retVal)
    {
        vpsDlmPrivDeInit();
    }

    return (retVal);
}



/**
 *  vpsDlmPrivDeInit
 *  Deallocates memory allocated by init function.
 *
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDlmPrivDeInit(void)
{
    UInt32          cnt, setCnt;
    VpsDlm_List    *listObj;

    /* Delete the Semaphore created for Pool objects */
    if (NULL != VpsDlmPoolObj.lockSem)
    {
        Semaphore_delete(&VpsDlmPoolObj.lockSem);
        VpsDlmPoolObj.lockSem = NULL;
    }

    for (cnt = 0u; cnt < DLM_NUM_INSTANCE; cnt++)
    {
        listObj = &VpsDlmPoolObj.listPool[cnt];

        /* Delete the Semaphores created for each of the list */
        if (NULL != listObj->listSem)
        {
            Semaphore_delete(&listObj->listSem);
            listObj->listSem = NULL;
        }

        /* Free descriptor memory */
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            if (NULL != listObj->descMem[setCnt])
            {
                VpsUtils_freeDescMem(
                    listObj->descMem[setCnt],
                    DLM_LIST_CTRL_DESC_MEMORY);
                listObj->descMem[setCnt] = NULL;
            }
        }
    }

    return (VPS_SOK);
}



/**
 *  vpsDlmAllocListObject
 *  \brief Returns the list object pointer with the corresponding list number.
 *
 *  If this is the first client registering with a list, then it allocates a
 *  free list object from memory pool and registers the list SI and LC
 *  interrupt callbacks with the event manager.
 *  For non-first client for a list, it just finds out the list object
 *  corresponding to that list number and returns the list object pointer.
 */
VpsDlm_List *vpsDlmAllocListObject(UInt8 listNum,
                                   UInt32 lmfid,
                                   VpsHal_VpdmaChannel socChNum)
{
    UInt32          cnt;
    UInt32          nodeCnt;
    UInt32          eventNum;
    VpsDlm_List    *listObj = NULL;
    UInt32          firstTime = TRUE;
    UInt32          setCnt, updateCnt;
    UInt32          retVal;

    /* Check for valid list number */
    GT_assert(DlmTrace, (listNum < VPSHAL_VPDMA_MAX_LIST));

    Semaphore_pend(VpsDlmPoolObj.lockSem, DLM_SEM_TIMEOUT);

    /* Find out a free list or already allocated list for the given listNum */
    for (cnt = 0u; cnt < DLM_NUM_INSTANCE; cnt++)
    {
        if (DLM_INVALID_LIST_NUM == VpsDlmPoolObj.listPool[cnt].listNum)
        {
            /* Got a free list, but still iterate till last list object to see
             * if the list with listNum is already allocated */
            listObj = &VpsDlmPoolObj.listPool[cnt];
        }
        if (listNum == VpsDlmPoolObj.listPool[cnt].listNum)
        {
            /* Already allocated list */
            listObj = &VpsDlmPoolObj.listPool[cnt];
            firstTime = FALSE;
            break;
        }
    }

    /* If this is first time, allocate send interrupts */
    if ((TRUE == firstTime) && (NULL != listObj))
    {
        /* Get free SI events */
        retVal = Vrm_allocSendIntr(listObj->siEventNum, DLM_NUM_DESC_SET);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DlmTrace, GT_ERR, "Alloc SI failed!!\n");
            listObj->listNum = DLM_INVALID_LIST_NUM;
            listObj = NULL;
        }
    }

    /* If this is first time, then initialize list variables and register to
     * event manager */
    if ((TRUE == firstTime) && (NULL != listObj))
    {
        /* Create the handle to the descriptor circular link list */
        listObj->descListHandle = VpsUtils_createLinkList(
                                      VPSUTILS_LLT_CIRCULAR,
                                      VPSUTILS_LAM_PRIORITY);
        /* Create the handle to the calback double link list */
        listObj->cbListHandle = VpsUtils_createLinkList(
                                      VPSUTILS_LLT_DOUBLE,
                                      VPSUTILS_LAM_TOP);
        if ((NULL == listObj->descListHandle) ||
            (NULL == listObj->cbListHandle))
        {
            GT_0trace(DlmTrace, GT_ERR, "Create link list failed!!\n");
            Vrm_releaseSendIntr(listObj->siEventNum, DLM_NUM_DESC_SET);
            if (NULL != listObj->descListHandle)
            {
                VpsUtils_deleteLinkList(listObj->descListHandle);
                listObj->descListHandle = NULL;
            }
            if (NULL != listObj->cbListHandle)
            {
                VpsUtils_deleteLinkList(listObj->cbListHandle);
                listObj->cbListHandle = NULL;
            }

            listObj->listNum = DLM_INVALID_LIST_NUM;
            listObj = NULL;
        }
    }

    if ((TRUE == firstTime) && (NULL != listObj))
    {
        listObj->listNum = listNum;
        listObj->isListRunning = FALSE;

        /* Initialize variables */
        listObj->numClients = 0u;
        listObj->totalUpdates = 0u;
        listObj->startUpdates = FALSE;
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            listObj->numUpdates[setCnt] = 0u;
            for (updateCnt = 0u; updateCnt < DLM_MAX_UPDATES; updateCnt++)
            {
                listObj->updateRldAddr[setCnt][updateCnt] = NULL;
                listObj->updateRldSize[setCnt][updateCnt] = 0u;
                listObj->updateRldDesc[setCnt][updateCnt] = NULL;
            }
        }
        listObj->totalClientCbUpdates = 0u;
        for (updateCnt = 0u; updateCnt < DLM_NUM_CLIENTS; updateCnt++)
        {
            listObj->updateClients[updateCnt] = NULL;
        }

        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            /* Initialize start LM FID list control descriptors.
             * Set LM FID to 1 so that when the actual LM FID for the first set
             * happens, FID will toggle from 1 to 0 corresponding to the first
             * field. */
            VpsHal_vpdmaCreateLmFidCtrlDesc(
                listObj->startLmfidDesc[setCnt],
                lmfid,
                VPSHAL_VPDMA_LM_FID_CHANGE_1);

            /* Initialize start SOC list control descriptors.
             * Set SOC to wait for client FID to toggle from 0 to 1. So that
             * the first shadow register programming happens in an odd field
             * and hence the data transfer will happen after the even set SOC
             * wait for FID from 1 to 0. */
            VpsHal_vpdmaCreateSOCCtrlDesc(
                listObj->startSocDesc[setCnt],
                socChNum,
                VPSHAL_VPDMA_SOC_FID_CHANGE01,
                0u,
                0u);

            /* Initialize SOC list control descriptors */
            if (setCnt & 1)
            {
                /* For odd set, set SOC to wait for client FID to toggle from
                 * 0 to 1. This client's frame start should be set to VENC FID
                 * change. */
                VpsHal_vpdmaCreateSOCCtrlDesc(
                    listObj->socDesc[setCnt],
                    socChNum,
                    VPSHAL_VPDMA_SOC_FID_CHANGE01,
                    0u,
                    0u);
            }
            else
            {
                /* For even set, set SOC to wait for client FID to toggle from
                 * 1 to 0. This client's frame start should be set to VENC FID
                 * change. */
                VpsHal_vpdmaCreateSOCCtrlDesc(
                    listObj->socDesc[setCnt],
                    socChNum,
                    VPSHAL_VPDMA_SOC_FID_CHANGE10,
                    0u,
                    0u);
            }
            VpsHal_vpdmaCreateSICtrlDesc(
                listObj->siDesc[setCnt],
                listObj->siEventNum[setCnt]);

            /* Initialize LM FID list control descriptors */
            if (setCnt & 1)
            {
                /* For odd set, set LM FID to 1 */
                VpsHal_vpdmaCreateLmFidCtrlDesc(
                    listObj->lmfidDesc[setCnt],
                    lmfid,
                    VPSHAL_VPDMA_LM_FID_CHANGE_1);
            }
            else
            {
                /* For even set, set LM FID to 0 */
                VpsHal_vpdmaCreateLmFidCtrlDesc(
                    listObj->lmfidDesc[setCnt],
                    lmfid,
                    VPSHAL_VPDMA_LM_FID_CHANGE_0);
            }

            /* Initialize SOR list control descriptors */
            /* Put a SOR descriptor so that VPDMA can sync with master DLM */
            VpsHal_vpdmaCreateSORCtrlDesc(listObj->sorDesc[setCnt], listNum);
            /* Link intermediate reload desc to actual reload desc */
            VpsHal_vpdmaCreateRLCtrlDesc(
                listObj->sorIntrCtrlRldDesc[setCnt],
                listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeRldDesc,
                sizeof(VpsHal_VpdmaReloadDesc));

            for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_LIST; nodeCnt++)
            {
                listObj->listCtrlNode[setCnt][nodeCnt].priority =
                    ((setCnt << 16u) | VpsDlmListCtrlNodePriorityTbl[nodeCnt]);
            }
        }

        /* Link the list control descriptors based on priority */
        vpsDlmLinkListCtrlDesc(listObj);

        /* Register to Event manager to get Send Interrupt (SI) callbacks */
        listObj->vemSendIntr = Vem_register(
                                   VEM_EG_SI,
                                   listObj->siEventNum,
                                   DLM_NUM_DESC_SET,
                                   VEM_PRIORITY0,
                                   &vpsDlmSendInterruptIsr,
                                   (Ptr) listObj);
        /* Register to Event manager to get List Completion callbacks */
        eventNum = listNum;
        listObj->vemListComp = Vem_register(
                                   VEM_EG_LISTCOMPLETE,
                                   &eventNum,
                                   1,
                                   VEM_PRIORITY0,
                                   &vpsDlmListCompleteIsr,
                                   (Ptr) listObj);
        if ((NULL == listObj->vemSendIntr) || (NULL == listObj->vemListComp))
        {
            GT_0trace(DlmTrace, GT_ERR, "VEM registeration failed!!\n");
            /* Unregister from Event manager if any one fails */
            if (NULL != listObj->vemSendIntr)
            {
                Vem_unRegister(listObj->vemSendIntr);
            }
            if (NULL != listObj->vemListComp)
            {
                Vem_unRegister(listObj->vemListComp);
            }

            /* Remove the already added nodes from list */
            vpsDlmUnLinkListCtrlDesc(listObj);

            /* Free-up send interrupts */
            Vrm_releaseSendIntr(listObj->siEventNum, DLM_NUM_DESC_SET);

            /* Free-up link list object */
            VpsUtils_deleteLinkList(listObj->descListHandle);
            VpsUtils_deleteLinkList(listObj->cbListHandle);
            listObj->descListHandle = NULL;
            listObj->cbListHandle = NULL;

            listObj->listNum = DLM_INVALID_LIST_NUM;
            listObj = NULL;
        }
    }

    if (NULL != listObj)
    {
        listObj->numClients++;
    }
    else            /* Alloc failed */
    {
        GT_1trace(DlmTrace, GT_ERR,
            "List object alloc failed for List: %d!!\n", listNum);
    }

    Semaphore_post(VpsDlmPoolObj.lockSem);

    return (listObj);
}



/**
 *  vpsDlmFreeListObject
 *  \brief Just decrements client count variable.
 */
Int32 vpsDlmFreeListObject(VpsDlm_List *listObj)
{
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    Semaphore_pend(VpsDlmPoolObj.lockSem, DLM_SEM_TIMEOUT);

    /* Make sure that we don't subtract from 0 and end up with a large number */
    GT_assert(DlmTrace, (0u != listObj->numClients));
    listObj->numClients--;

    Semaphore_post(VpsDlmPoolObj.lockSem);

    return (VPS_SOK);
}



/**
 *  vpsDlmFreeListResource
 *  \brief Frees-up the list object and unregisters from event manager if this
 *  is the last client else returns error.
 */
Int32 vpsDlmFreeListResource(VpsDlm_List *listObj)
{
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    Semaphore_pend(VpsDlmPoolObj.lockSem, DLM_SEM_TIMEOUT);

    /* For last client in list, free-up the list object */
    if (0 == listObj->numClients)
    {
        listObj->listNum = DLM_INVALID_LIST_NUM;
        listObj->isListRunning = FALSE;

        /* Unregister from Event manager */
        Vem_unRegister(listObj->vemSendIntr);
        Vem_unRegister(listObj->vemListComp);

        /* Remove the already added nodes from list */
        vpsDlmUnLinkListCtrlDesc(listObj);

        /* Free-up send interrupts */
        Vrm_releaseSendIntr(listObj->siEventNum, DLM_NUM_DESC_SET);

        /* Free-up link list object */
        VpsUtils_deleteLinkList(listObj->descListHandle);
        VpsUtils_deleteLinkList(listObj->cbListHandle);
        listObj->descListHandle = NULL;
        listObj->cbListHandle = NULL;
    }
    else
    {
        GT_0trace(DlmTrace, GT_ERR, "Still clients active in the list!!\n");
        retVal = VPS_EFAIL;
    }

    Semaphore_post(VpsDlmPoolObj.lockSem);

    return (retVal);
}



/**
 *  vpsDlmAllocClientObject
 *  \brief Allocates client object from free pool.
 */
VpsDlm_Client *vpsDlmAllocClientObject(void)
{
    VpsDlm_Client      *client;

    client = (VpsDlm_Client *)
        VpsUtils_alloc(
            &VpsDlmPoolObj.clientPoolPrm,
            sizeof (VpsDlm_Client),
            DlmTrace);
    if (NULL == client)
    {
        GT_0trace(DlmTrace, GT_ERR, "Alloc client object failed!!\n");
    }

    return (client);
}



/**
 *  vpsDlmFreeClientObject
 *  \brief Frees the already allocated client object.
 */
Int32 vpsDlmFreeClientObject(VpsDlm_Client *client)
{
    Int32           retVal;

    retVal = VpsUtils_free(&VpsDlmPoolObj.clientPoolPrm, client, DlmTrace);
    if (VPS_SOK != retVal)
    {
        GT_0trace(DlmTrace, GT_ERR, "Free client object failed!!\n");
    }

    return (retVal);
}



/**
 *  vpsDlmGetListObject
 *  \brief Gets the list object pointer for the given list number.
 */
VpsDlm_List *vpsDlmGetListObject(UInt8 listNum)
{
    UInt32          cnt;
    VpsDlm_List    *listObj = NULL;

    /* Check for valid list number */
    GT_assert(DlmTrace, (listNum < VPSHAL_VPDMA_MAX_LIST));

    /* Find a matching list object */
    for (cnt = 0u; cnt < DLM_NUM_INSTANCE; cnt++)
    {
        if (listNum == VpsDlmPoolObj.listPool[cnt].listNum)
        {
            /* Get the num clients running */
            listObj = &VpsDlmPoolObj.listPool[cnt];
            break;
        }
    }

    return (listObj);
}



/**
 *  vpsDlmLinkClientDesc
 *  \brief Performs descriptor linking of a client to the circular linked list.
 *
 *  This only does the software linking, the actual hardware linking
 *  (programming of Reload descriptors) is done in the SI ISR or caller
 *  function depending on whether the list is running or not.
 */
UInt32 vpsDlmLinkClientDesc(VpsDlm_Client *client, UInt32 startIndex)
{
    UInt32          setCnt;
    UInt32          nodeCnt;
    VpsDlm_List    *listObj;
    UInt32          updateCnt, totalUpdates = 0u;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != client));

    /* Get the list object handle */
    listObj = client->listObj;
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        updateCnt = startIndex;
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
        {
            /* Link the node only if it is needed */
            if (0u != client->descNode[setCnt][nodeCnt].nodeSize)
            {
                /* Link the node to the link list */
                client->descNode[setCnt][nodeCnt].nodeObj.data =
                    &client->descNode[setCnt][nodeCnt];
                VpsUtils_linkNodePri(
                    listObj->descListHandle,
                    &client->descNode[setCnt][nodeCnt].nodeObj,
                    client->descNode[setCnt][nodeCnt].priority);

                /* Get the next and previous node data/info */
                GT_assert(DlmTrace,
                    (NULL != client->descNode[setCnt][nodeCnt].nodeObj.next));
                GT_assert(DlmTrace,
                    (NULL != client->descNode[setCnt][nodeCnt].nodeObj.prev));
                nextNode = client->descNode[setCnt][nodeCnt].nodeObj.next->data;
                prevNode = client->descNode[setCnt][nodeCnt].nodeObj.prev->data;
                GT_assert(DlmTrace, (NULL != nextNode));
                GT_assert(DlmTrace, (NULL != prevNode));

                /* Link the previous node to current node */
                listObj->updateRldAddr[setCnt][updateCnt] =
                    client->descNode[setCnt][nodeCnt].nodeAddr;
                listObj->updateRldSize[setCnt][updateCnt] =
                    client->descNode[setCnt][nodeCnt].nodeSize;
                listObj->updateRldDesc[setCnt][updateCnt] =
                    prevNode->nodeRldDesc;
                updateCnt++;
                totalUpdates++;

                /* Program the current node's reload to next node */
                VpsHal_vpdmaCreateRLCtrlDesc(
                    client->descNode[setCnt][nodeCnt].nodeRldDesc,
                    nextNode->nodeAddr,
                    nextNode->nodeSize);
            }
        }
    }

    /* Return the count of updates done */
    return (totalUpdates);
}



/**
 *  vpsDlmUnlinkClientDesc
 *  \brief Performs descriptor un-linking of a client from the circular linked
 *  list.
 *
 *  This only does the software unlinking, the actual hardware unlinking
 *  (programming of Reload descriptors) is done in the SI/LC ISR.
 */
UInt32 vpsDlmUnlinkClientDesc(VpsDlm_Client *client, UInt32 startIndex)
{
    UInt32          setCnt;
    UInt32          nodeCnt;
    VpsDlm_List    *listObj;
    UInt32          updateCnt, totalUpdates = 0u;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != client));

    /* Get the list object handle */
    listObj = client->listObj;
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        updateCnt = startIndex;
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
        {
            /* Unlink the node only if it was already added */
            if (0u != client->descNode[setCnt][nodeCnt].nodeSize)
            {
                /* Get the next and previous node data/info */
                GT_assert(DlmTrace,
                    (NULL != client->descNode[setCnt][nodeCnt].nodeObj.next));
                GT_assert(DlmTrace,
                    (NULL != client->descNode[setCnt][nodeCnt].nodeObj.prev));
                nextNode = client->descNode[setCnt][nodeCnt].nodeObj.next->data;
                prevNode = client->descNode[setCnt][nodeCnt].nodeObj.prev->data;
                GT_assert(DlmTrace, (NULL != nextNode));
                GT_assert(DlmTrace, (NULL != prevNode));

                /* Link the previous node to next node */
                listObj->updateRldAddr[setCnt][updateCnt] =
                    nextNode->nodeAddr;
                listObj->updateRldSize[setCnt][updateCnt] =
                    nextNode->nodeSize;
                listObj->updateRldDesc[setCnt][updateCnt] =
                    prevNode->nodeRldDesc;
                updateCnt++;
                totalUpdates++;

                /* Unlink the node from the link list */
                VpsUtils_unLinkNodePri(
                    listObj->descListHandle,
                    &client->descNode[setCnt][nodeCnt].nodeObj);
            }
        }
    }

    /* Return the count of updates done */
    return (totalUpdates);
}



/**
 *  vpsDlmRotateSorRlUpdates
 *  \brief Rotates the SOR RL update across the sets.
 *
 *  Why are we doing this?
 *  Since we are updating the next updates in the current SI ISR, if the current
 *  set reload descriptor needs to be updated, it won''t happen. Hence put
 *  the current set updates for SOR RL to the next set so that the updates
 *  will happen properly in SI ISR!!
 */
Void vpsDlmRotateSorRlUpdates(VpsDlm_List *listObj, UInt32 numUpdates)
{
    UInt32                  updateCnt, setCnt;
    Void                   *tempUpdateRldAddr = NULL;
    UInt32                  tempUpdateRldSize = 0u;
    VpsHal_VpdmaReloadDesc *tempUpdateRldDesc = NULL;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));
    GT_assert(DlmTrace, (numUpdates < DLM_MAX_UPDATES));

    for (updateCnt = 0u; updateCnt < numUpdates; updateCnt++)
    {
        /* Check if update reload descriptor matches with SOR RL descriptor */
        if (listObj->updateRldDesc[0u][updateCnt] ==
            listObj->listCtrlNode[0u][DLM_SOR_IDX].nodeRldDesc)
        {
            /*
             * Update contains SOR RL. So rotate i.e. copy current RL to
             * next set and so on!!
             */
            /* Store the last set values */
            tempUpdateRldAddr =
                listObj->updateRldAddr[(DC_NUM_DESC_SET - 1u)][updateCnt];
            tempUpdateRldSize =
                listObj->updateRldSize[(DC_NUM_DESC_SET - 1u)][updateCnt];
            tempUpdateRldDesc =
                listObj->updateRldDesc[(DC_NUM_DESC_SET - 1u)][updateCnt];

            /* Copy in reverse order except for the first set */
            for (setCnt = (DLM_NUM_DESC_SET - 1u); setCnt > 0u; setCnt--)
            {
                listObj->updateRldAddr[setCnt][updateCnt] =
                    listObj->updateRldAddr[setCnt - 1u][updateCnt];
                listObj->updateRldSize[setCnt][updateCnt] =
                    listObj->updateRldSize[setCnt - 1u][updateCnt];
                listObj->updateRldDesc[setCnt][updateCnt] =
                    listObj->updateRldDesc[setCnt - 1u][updateCnt];
            }

            /* For the first set, use the stored value */
            listObj->updateRldAddr[0u][updateCnt] = tempUpdateRldAddr;
            listObj->updateRldSize[0u][updateCnt] = tempUpdateRldSize;
            listObj->updateRldDesc[0u][updateCnt] = tempUpdateRldDesc;
        }
    }

    return;
}



/**
 *  vpsDlmLinkListCtrlDesc
 *  \brief Performs descriptor linking of a list control descriptors to the
 *  circular linked list.
 */
static Void vpsDlmLinkListCtrlDesc(VpsDlm_List *listObj)
{
    UInt32          setCnt;
    UInt32          nodeCnt;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_LIST; nodeCnt++)
        {
            /* Link the node only if it is needed */
            if (0u != listObj->listCtrlNode[setCnt][nodeCnt].nodeSize)
            {
                /* Link the node to the link list */
                listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.data =
                    &listObj->listCtrlNode[setCnt][nodeCnt];
                VpsUtils_linkNodePri(
                    listObj->descListHandle,
                    &listObj->listCtrlNode[setCnt][nodeCnt].nodeObj,
                    listObj->listCtrlNode[setCnt][nodeCnt].priority);

                /* Get the next and previous node data/info */
                GT_assert(DlmTrace,
                (NULL != listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.next));
                GT_assert(DlmTrace,
                (NULL != listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.prev));
                nextNode =
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.next->data;
                prevNode =
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.prev->data;
                GT_assert(DlmTrace, (NULL != nextNode));
                GT_assert(DlmTrace, (NULL != prevNode));

                /* Link the previous node to current node */
                VpsHal_vpdmaCreateRLCtrlDesc(
                    prevNode->nodeRldDesc,
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeAddr,
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeSize);

                /* Program the current node's reload to next node */
                VpsHal_vpdmaCreateRLCtrlDesc(
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeRldDesc,
                    nextNode->nodeAddr,
                    nextNode->nodeSize);
            }
        }
    }

    return;
}



/**
 *  vpsDlmUnLinkListCtrlDesc
 *  \brief Performs descriptor unlinking of a list control descriptors to the
 *  circular linked list.
 */
static Void vpsDlmUnLinkListCtrlDesc(VpsDlm_List *listObj)
{
    UInt32          setCnt;
    UInt32          nodeCnt;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_LIST; nodeCnt++)
        {
            /* Unlink the node only if it was already added */
            if (0u != listObj->listCtrlNode[setCnt][nodeCnt].nodeSize)
            {
                /* Get the next and previous node data/info */
                GT_assert(DlmTrace,
                (NULL != listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.next));
                GT_assert(DlmTrace,
                (NULL != listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.prev));
                nextNode =
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.next->data;
                prevNode =
                    listObj->listCtrlNode[setCnt][nodeCnt].nodeObj.prev->data;
                GT_assert(DlmTrace, (NULL != nextNode));
                GT_assert(DlmTrace, (NULL != prevNode));

                /* Link the previous node to next node */
                VpsHal_vpdmaCreateRLCtrlDesc(
                    prevNode->nodeRldDesc,
                    nextNode->nodeAddr,
                    nextNode->nodeSize);

                /* Unlink the node from the link list */
                VpsUtils_unLinkNodePri(
                    listObj->descListHandle,
                    &listObj->listCtrlNode[setCnt][nodeCnt].nodeObj);
            }
        }
    }

    return;
}
