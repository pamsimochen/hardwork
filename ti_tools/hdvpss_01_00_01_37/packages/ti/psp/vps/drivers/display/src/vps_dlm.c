/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_dlm.c
 *
 *  \brief Display List Manager file
 *  This file implements the Display List Manager APIs
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Timestamp.h>

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

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static VpsDlm_List *vpsDlmCheckStartStopPrms(const VpsDlm_Request *reqObj);
static Int32 vpsDlmProcessStartStopReq(VpsDlm_List *listObj,
                                       const VpsDlm_Request *reqObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief Priority table for the various client descriptor nodes used for
 *  seamless addition to link list.
 */
static const UInt32 VpsDlmDescNodePriorityTbl[DLM_NODES_PER_CLIENTS] =
{
    DLM_SHADOW_PRIORITY,
    DLM_IN_DATA_PRIORITY,
    DLM_OUT_DATA_PRIORITY,
    DLM_NSHADOW_PRIORITY,
    DLM_MOSAIC_DATA_PRIORITY
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsDlm_init
 *  \brief VPS Display List Manager init function.
 */
Int32 VpsDlm_init(Ptr arg)
{
    Int32       retVal;

    retVal = vpsDlmPrivInit();

    return (retVal);
}



/**
 *  VpsDlm_deInit
 *  \brief VPS Display List Manager exit function.
 */
Int32 VpsDlm_deInit(Ptr arg)
{
    Int32       retVal;

    retVal = vpsDlmPrivDeInit();

    return (retVal);
}



/**
 *  VpsDlm_register
 *  \brief Registers the clients with DLM.
 */
VpsDlm_Handle VpsDlm_register(VpsDlm_ClientType clientType,
                              UInt32 isFbMode,
                              VpsDlm_ClientCbFxn cbFxn,
                              Ptr arg)
{
    UInt32          setCnt, nodeCnt;
    VpsDlm_Client  *client;

    /* Allocate new client object */
    client = vpsDlmAllocClientObject();
    if (NULL != client)
    {
        client->clientType = clientType;
        client->isFbMode = isFbMode;
        /*
         * Clients present in the same core will register callback whereas
         * clients present in other cores registers without callback function
         */
        if (NULL == cbFxn)
        {
            client->state.isLocalClient = FALSE;
        }
        else        /* Local Client */
        {
            client->state.isLocalClient = TRUE;
        }
        client->cbFxn = cbFxn;
        client->arg = arg;

        /* Initialize variables */
        client->listObj = NULL;
        client->state.isRegistered = TRUE;
        client->state.isDescMemSet = FALSE;
        client->state.isStarted = FALSE;
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
            {
                client->descNode[setCnt][nodeCnt].priority =
                    ((setCnt << 16u) | VpsDlmDescNodePriorityTbl[nodeCnt]);
            }

            /*
             * Note: Change the priority of IN data descriptors when frame
             * buffer mode is selected so that it falls after SOC list control
             * descriptors.
             */
            if (TRUE == client->isFbMode)
            {
                client->descNode[setCnt][DLM_IN_DATA_IDX].priority =
                    ((setCnt << 16u) | (DLM_SOC_PRIORITY +  1u));
            }
        }
    }

    return ((VpsDlm_Handle) client);
}



/**
 *  VpsDlm_unRegister
 *  \brief Unregisters the DLM client and deallocates the client object.
 */
Int32 VpsDlm_unRegister(VpsDlm_Handle handle)
{
    Int32           retVal = VPS_SOK;
    UInt32          setCnt, nodeCnt;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));

    client = (VpsDlm_Client *) handle;
    if (TRUE == client->state.isRegistered)
    {
        /* Stop if already running */
        if (TRUE == client->state.isStarted)
        {
            GT_0trace(DlmTrace, GT_INFO,
                "Implicitly stopping client while unregistering...\n");
            VpsDlm_stopClient(client);
        }

        /* Clear descriptor memories if already set */
        if (TRUE == client->state.isDescMemSet)
        {
            GT_0trace(DlmTrace, GT_INFO,
                "Implicitly clearing client memory while unregistering...\n");
            VpsDlm_clrDescMem(client);
        }

        /* Reset variables */
        client->listObj = NULL;
        client->state.isRegistered = FALSE;
        client->state.isLocalClient = TRUE;
        client->clientType = DLM_CT_GENERIC;
        client->isFbMode = FALSE;
        client->cbFxn = NULL;
        client->arg = NULL;
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
            {
                client->descNode[setCnt][nodeCnt].priority = 0u;
            }
        }

        /* Free up client object */
        retVal = vpsDlmFreeClientObject(client);
    }
    else
    {
        GT_0trace(DlmTrace, GT_ERR, "Client not registered!!\n");
        retVal = VPS_EFAIL;
    }

    return (retVal);
}



/**
 *  VpsDlm_setDescMem
 *  \brief Sets the various descriptor memories for the client.
 */
Int32 VpsDlm_setDescMem(VpsDlm_Handle handle, const VpsDlm_DescInfo *descInfo)
{
    Int32           retVal = VPS_SOK;
    UInt32          setCnt, nodeCnt;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));
    GT_assert(DlmTrace, (NULL != descInfo));

    client = (VpsDlm_Client *) handle;
    /* Check if descriptors are already set */
    GT_assert(DlmTrace, (TRUE != client->state.isDescMemSet));

    /* Assign the node parameters for all the descriptor sets */
    client->state.isDescMemSet = TRUE;
    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
        {
            if (0u != descInfo->descSize[setCnt][nodeCnt])
            {
                GT_assert(DlmTrace,
                    (NULL != descInfo->descAddr[setCnt][nodeCnt]));
                GT_assert(DlmTrace,
                    (NULL != descInfo->rldDescAddr[setCnt][nodeCnt]));
            }
            client->descNode[setCnt][nodeCnt].nodeAddr =
                descInfo->descAddr[setCnt][nodeCnt];
            client->descNode[setCnt][nodeCnt].nodeSize =
                descInfo->descSize[setCnt][nodeCnt];
            client->descNode[setCnt][nodeCnt].nodeRldDesc =
                descInfo->rldDescAddr[setCnt][nodeCnt];
        }
    }

    return (retVal);
}



/**
 *  VpsDlm_clrDescMem
 *  \brief Clears the descriptor memories for the client already set by
 *  calling VpsDlm_setDescMem.
 */
Int32 VpsDlm_clrDescMem(VpsDlm_Handle handle)
{
    UInt32          setCnt, nodeCnt;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));

    client = (VpsDlm_Client *) handle;
    /* Check if descriptors are already set */
    GT_assert(DlmTrace, (TRUE == client->state.isDescMemSet));

    /* Stop if already running */
    if (TRUE == client->state.isStarted)
    {
        GT_0trace(DlmTrace, GT_INFO,
            "Implicitly stopping client while clearing memory...\n");
        VpsDlm_stopClient(client);
    }

    /* Reset variables */
    client->state.isDescMemSet = FALSE;
    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
        {
            client->descNode[setCnt][nodeCnt].nodeAddr = NULL;
            client->descNode[setCnt][nodeCnt].nodeSize = 0u;
            client->descNode[setCnt][nodeCnt].nodeRldDesc = NULL;
        }
    }

    return (VPS_SOK);
}



/**
 *  VpsDlm_updateDescMem
 *  \brief Updates the descriptor memory of the client with the new descriptor
 *  memory.
 */
Int32 VpsDlm_updateDescMem(VpsDlm_Handle handle,
                           UInt32 descSet,
                           const VpsDlm_UpdateDescInfo *descInfo)
{
    Int32           retVal = VPS_SOK;
    UInt32          nodeCnt;
    VpsDlm_List    *listObj;
    VpsDlm_Client  *client;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));
    GT_assert(DlmTrace, (NULL != descInfo));
    GT_assert(DlmTrace, (descSet < DLM_NUM_DESC_SET));

    client = (VpsDlm_Client *) handle;
    listObj = client->listObj;
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    /* Check if descriptors are already set and clients have started */
    GT_assert(DlmTrace, (TRUE == client->state.isDescMemSet));
    GT_assert(DlmTrace, (TRUE == client->state.isStarted));

    for (nodeCnt = 0u; nodeCnt < DLM_NODES_PER_CLIENTS; nodeCnt++)
    {
        /* Link the node if it is needed */
        if (0u != descInfo->descSize[nodeCnt])
        {
            GT_assert(DlmTrace, (NULL != descInfo->descAddr[nodeCnt]));
            GT_assert(DlmTrace, (NULL != descInfo->rldDescAddr[nodeCnt]));

            /* Add a node if not added previously */
            if (0u == client->descNode[descSet][nodeCnt].nodeSize)
            {
                /* Link the node to the link list */
                client->descNode[descSet][nodeCnt].nodeObj.data =
                    &client->descNode[descSet][nodeCnt];
                VpsUtils_linkNodePri(
                    listObj->descListHandle,
                    &client->descNode[descSet][nodeCnt].nodeObj,
                    client->descNode[descSet][nodeCnt].priority);
            }

            /* Get the next and previous node data/info */
            GT_assert(DlmTrace,
                (NULL != client->descNode[descSet][nodeCnt].nodeObj.next));
            GT_assert(DlmTrace,
                (NULL != client->descNode[descSet][nodeCnt].nodeObj.prev));
            nextNode = client->descNode[descSet][nodeCnt].nodeObj.next->data;
            prevNode = client->descNode[descSet][nodeCnt].nodeObj.prev->data;
            GT_assert(DlmTrace, (NULL != nextNode));
            GT_assert(DlmTrace, (NULL != prevNode));

            /* Link previous node to current node's new descriptor address */
            VpsHal_vpdmaCreateRLCtrlDesc(
                prevNode->nodeRldDesc,
                descInfo->descAddr[nodeCnt],
                descInfo->descSize[nodeCnt]);

            /* Link the client's reload to next descriptor */
            VpsHal_vpdmaCreateRLCtrlDesc(
                descInfo->rldDescAddr[nodeCnt],
                nextNode->nodeAddr,
                nextNode->nodeSize);
        }
        else    /* Descriptor not required in list */
        {
            /* Unlink the node if already added */
            if (0u != client->descNode[descSet][nodeCnt].nodeSize)
            {
                /* Get the next and previous node data/info */
                GT_assert(DlmTrace,
                    (NULL != client->descNode[descSet][nodeCnt].nodeObj.next));
                GT_assert(DlmTrace,
                    (NULL != client->descNode[descSet][nodeCnt].nodeObj.prev));
                nextNode =
                    client->descNode[descSet][nodeCnt].nodeObj.next->data;
                prevNode =
                    client->descNode[descSet][nodeCnt].nodeObj.prev->data;
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
                    &client->descNode[descSet][nodeCnt].nodeObj);
            }
        }

        /* Update the new info in client object */
        client->descNode[descSet][nodeCnt].nodeAddr =
            descInfo->descAddr[nodeCnt];
        client->descNode[descSet][nodeCnt].nodeSize =
            descInfo->descSize[nodeCnt];
        client->descNode[descSet][nodeCnt].nodeRldDesc =
            descInfo->rldDescAddr[nodeCnt];
    }

    return (retVal);
}



/**
 *  VpsDlm_stopClient
 *  \brief Removes the client descriptors from the corresponding list
 *  descriptor circular link and de-allocates list objects.
 */
Int32 VpsDlm_stopClient(VpsDlm_Handle handle)
{
    Int32           retVal = VPS_SOK;
    VpsDlm_List    *listObj;
    VpsDlm_Client  *client;
    VpsDlm_Request  reqObj;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));

    client = (VpsDlm_Client *) handle;
    listObj = client->listObj;
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    /* Call start/stop function */
    reqObj.numStart = 0u;
    reqObj.numStop = 1u;
    reqObj.stopClients[0u] = handle;
    reqObj.listNum = listObj->listNum;
    reqObj.lmfid = 0u;
    reqObj.socChNum = (VpsHal_VpdmaChannel) 0;
    retVal = VpsDlm_startStopClients(&reqObj);

    return (retVal);
}



/**
 *  VpsDlm_startStopClients
 *  \brief Adds and removes multiple client descriptors to the corresponding
 *  list descriptor circular link at one go.
 */
Int32 VpsDlm_startStopClients(const VpsDlm_Request *reqObj)
{
    Int32               retVal = VPS_SOK;
    UInt32              cnt, setCnt, updateCnt;
    UInt32              cookie;
    VpsDlm_List        *listObj = NULL;
    VpsDlm_Client      *client;
    VpsDlm_Node        *headNodeData;
    VpsUtils_Node      *headNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != reqObj));
    /* Check for valid list number */
    GT_assert(DlmTrace, (reqObj->listNum < VPSHAL_VPDMA_MAX_LIST));
    /* Check if number of clients to start/stop exceeds max limit */
    GT_assert(DlmTrace, (reqObj->numStart < DLM_MAX_START_STOP_CLIENTS));
    GT_assert(DlmTrace, (reqObj->numStop < DLM_MAX_START_STOP_CLIENTS));
    /* Atleast one operation should be performed */
    GT_assert(DlmTrace, !((reqObj->numStart == 0u) && (reqObj->numStop == 0u)));

    /* Check the parameters and get the corresponding list object */
    listObj = vpsDlmCheckStartStopPrms(reqObj);
    if (NULL == listObj)
    {
        GT_0trace(DlmTrace, GT_ERR, "Invalid start/stop params\n");
        retVal = VPS_EALLOC;
    }

    if (NULL != listObj)
    {
        Semaphore_pend(listObj->listSem, DLM_SEM_TIMEOUT);
    }

    if (VPS_SOK == retVal)
    {
        /* Remove the stop clients from callback list */
        for (cnt = 0u; cnt < reqObj->numStop; cnt++)
        {
            client = reqObj->stopClients[cnt];

            /* Remove the client from callback list, if client is local */
            if (TRUE == client->state.isLocalClient)
            {
                /* Disable global interrupts */
                cookie = Hwi_disable();

                VpsUtils_unLinkNode(listObj->cbListHandle, &client->cbNodeObj);

                /* Restore global interrupts */
                Hwi_restore(cookie);
            }
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Process the request */
        retVal = vpsDlmProcessStartStopReq(listObj, reqObj);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DlmTrace, GT_ERR, "Error processing request\n");
        }
    }

    /* Submit the list to VPDMA if this is the first client/list not running */
    if ((VPS_SOK == retVal) && (FALSE == listObj->isListRunning))
    {
        GT_1trace(DlmTrace, GT_DEBUG, "Posting List: %d\n", listObj->listNum);

        /* Can't have zero updates */
        GT_assert(DlmTrace, (0u != listObj->totalUpdates));

        for (updateCnt = 0u;
             updateCnt < listObj->totalClientCbUpdates;
             updateCnt++)
        {
            client = listObj->updateClients[updateCnt];
            /* NULL pointer check */
            GT_assert(DlmTrace, (NULL != client));
            client->cbNodeObj.data = client;
            VpsUtils_linkNodeToHead(
                listObj->cbListHandle,
                &client->cbNodeObj);
            listObj->updateClients[updateCnt] = NULL;
        }
        listObj->totalClientCbUpdates = 0u;

        /* Since list is not running update the updates before posting */
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            for (updateCnt = 0u;
                 updateCnt < (listObj->numUpdates[setCnt]);
                 updateCnt++)
            {
                VpsHal_vpdmaCreateRLCtrlDesc(
                    listObj->updateRldDesc[setCnt][updateCnt],
                    listObj->updateRldAddr[setCnt][updateCnt],
                    listObj->updateRldSize[setCnt][updateCnt]);
            }
            /* Reset variable so that next ISR won't accidentally redo
             * again */
            GT_assert(DlmTrace,
                (listObj->numUpdates[setCnt] <= listObj->totalUpdates));
            listObj->totalUpdates -= listObj->numUpdates[setCnt];
            listObj->numUpdates[setCnt] = 0u;
        }

        /* Submit the list to VPDMA if this is the first client */
        listObj->isListRunning = TRUE;

        /* Link the first LM FID reload to the top most descriptor */
        headNode = VpsUtils_getHeadNode(listObj->descListHandle);
        headNodeData = (VpsDlm_Node *) headNode->data;
        VpsHal_vpdmaCreateRLCtrlDesc(
            listObj->startRldDesc[0u],
            headNodeData->nodeAddr,
            headNodeData->nodeSize);

        /* Submit the list by posting the first LM FID descriptor */
        VpsHal_vpdmaPostList(
            listObj->listNum,
            VPSHAL_VPDMA_LT_NORMAL,
            listObj->startLmfidDesc[0u],
            DLM_START_LIST_CTRL_DESC_MEMORY,
            TRUE);
    }

    if (VPS_SOK == retVal)
    {
        /* Free up list object for stop clients */
        for (cnt = 0u; cnt < reqObj->numStop; cnt++)
        {
            client = reqObj->stopClients[cnt];
            retVal |= vpsDlmFreeListObject(listObj);
            client->listObj = NULL;
        }
    }

    /* Free-up the memories if error occurs */
    if (VPS_SOK != retVal)
    {
        if (NULL != listObj)
        {
            /* Remove free list object and reset states for start clients */
            for (cnt = 0u; cnt < reqObj->numStart; cnt++)
            {
                client = reqObj->startClients[cnt];

                vpsDlmUnlinkClientDesc(client, 0u);

                if (TRUE == client->state.isLocalClient)
                {
                    VpsUtils_unLinkNode(
                        listObj->cbListHandle,
                        &client->cbNodeObj);
                }

                vpsDlmFreeListObject(listObj);
                client->listObj = NULL;
                client->state.isStarted = FALSE;
            }
        }
    }

    if (NULL != listObj)
    {
        Semaphore_post(listObj->listSem);
    }

    return (retVal);
}



/**
 *  VpsDlm_getNumActiveClients
 *  \brief Returns the number of clients running on a given list.
 */
UInt32 VpsDlm_getNumActiveClients(UInt8 listNum)
{
    UInt32          numClients = 0u;
    VpsDlm_List    *listObj;

    /* Check for valid list number */
    GT_assert(DlmTrace, (listNum < VPSHAL_VPDMA_MAX_LIST));

    listObj = vpsDlmGetListObject(listNum);
    if (NULL != listObj)
    {
        /* Get the num clients running if list is valid. Else return zero
         * as the list might have been stopped after stopping last client. */
        numClients = listObj->numClients;
    }

    return (numClients);
}



/**
 *  VpsDlm_unLinkDctrlClient
 *  \brief Unlinks the display controller client from the list.
 */
Int32 VpsDlm_unLinkDctrlClient(VpsDlm_Handle handle)
{
    Int32           retVal = VPS_SOK;
    UInt32          setCnt, nodeCnt, nextSet;
    UInt32          numNodes;
    VpsDlm_List    *listObj;
    VpsDlm_Client  *client;
    VpsDlm_Node    *nextNode, *prevNode;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));

    client = (VpsDlm_Client *) handle;
    listObj = client->listObj;
    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != listObj));

    /* Check if this is a DCTRL client - supported only for DCTRL client */
    GT_assert(DlmTrace, (DLM_CT_DCTRL_MUX == client->clientType));
    /* Check if descriptors are already set and clients have started */
    GT_assert(DlmTrace, (TRUE == client->state.isDescMemSet));
    GT_assert(DlmTrace, (TRUE == client->state.isStarted));

    /* Remove the client from callback list, if client is local */
    if (TRUE == client->state.isLocalClient)
    {
        VpsUtils_unLinkNode(listObj->cbListHandle, &client->cbNodeObj);
    }

    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
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
                VpsHal_vpdmaCreateRLCtrlDesc(
                    prevNode->nodeRldDesc,
                    nextNode->nodeAddr,
                    nextNode->nodeSize);

                /* Unlink the node from the link list */
                VpsUtils_unLinkNodePri(
                    listObj->descListHandle,
                    &client->descNode[setCnt][nodeCnt].nodeObj);
            }
        }
    }
    client->state.isStarted = FALSE;
    client->listObj = NULL;

    /* Check if the last client is also removed. If so then break the
     * link list so that the list stops. */
    numNodes = VpsUtils_getNumNodes(listObj->descListHandle);
    if ((DLM_NUM_DESC_SET * DLM_NODES_PER_LIST) == numNodes)
    {
        /* Last client in List - only one left. */
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            /* Figure out the next set */
            if ((DLM_NUM_DESC_SET - 1u) == setCnt)
            {
                nextSet = 0u;
            }
            else
            {
                nextSet = setCnt + 1u;
            }

            /* Link SOR reload descriptor to next set SOC SI descriptor
             * without SOC descriptor.
             * Note: This should be updated in the next set of updates
             * so that when we update in SI, the current SOR RL is
             * updated!! */
            VpsHal_vpdmaCreateRLCtrlDesc(
                listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeRldDesc,
                listObj->siDesc[nextSet],
                (listObj->listCtrlNode[nextSet][DLM_SOC_IDX].nodeSize -
                    sizeof(VpsHal_VpdmaSyncOnClientDesc)));

            /* Link LMFID reload descriptor to SOR descriptor without
             * the reload descriptor so that the list will stop */
            VpsHal_vpdmaCreateRLCtrlDesc(
                listObj->listCtrlNode[setCnt][DLM_LMFID_IDX].nodeRldDesc,
                listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeAddr,
                (listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeSize -
                    sizeof(VpsHal_VpdmaReloadDesc)));
        }
    }

    /* Make sure that we don't subtract from 0 and end up with a large number */
    GT_assert(DlmTrace, (0u != listObj->numClients));
    listObj->numClients--;

    return (retVal);
}



/**
 *  VpsDlm_freeListResource
 *  \brief Frees the list objects and related handles for the list.
 */
Int32 VpsDlm_freeListResource(UInt8 listNum)
{
    Int32           retVal = VPS_SOK;
    VpsDlm_List    *listObj;

    /* Check for valid list number */
    GT_assert(DlmTrace, (listNum < VPSHAL_VPDMA_MAX_LIST));

    listObj = vpsDlmGetListObject(listNum);
    if (NULL != listObj)
    {
        retVal = vpsDlmFreeListResource(listObj);
    }
    else
    {
        GT_0trace(DlmTrace, GT_ERR, "Invalid list number\n");
        retVal = VPS_EFAIL;
    }

    return (retVal);
}



/**
 *  vpsDlmSendInterruptIsr
 *  \brief DLM Send Interrupt ISR.
 *
 *  This is the callback function registered with the event manager to get
 *  SI callback for each of the list. For all the lists managed by the DLM,
 *  this is the same function which is registered with the event manager.
 *  The arg parameter is used to get the corresponding list object.
 */
Void vpsDlmSendInterruptIsr(const UInt32 *eventList, UInt32 numEvents, Ptr arg)
{
    UInt32          curSet = 0u, nextSet;
    UInt32          cnt, updateCnt;
    UInt32          timeStamp;
    UInt32          updateClientCb = FALSE;
    VpsDlm_List    *listObj;
    VpsDlm_Client  *client;
    VpsUtils_Node  *node;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != eventList));
    GT_assert(DlmTrace, (NULL != arg));

    /* Only registered DLM_NUM_DESC_SET events, hence can't be more than that */
    GT_assert(DlmTrace, (numEvents <= DLM_NUM_DESC_SET));

    GT_1trace(DlmTrace, GT_DEBUG, "SI for source: %d\n", *eventList);

    /* Get time stamp */
    timeStamp = Timestamp_get32();

    /*
     * Process only the ISR for which only one SI occurred. If multiple SI
     * occurred (due to some system delay), then we are not very sure where the
     * VPDMA is currently processing i.e. which set it is currently working up
     * on.
     *
     * For instance, if the ISR is for set 1, this means that VPDMA has
     * processed set 1 (that's why we got a SI!!) and is currently working on
     * set 0. So it is pretty safe to update set 1 when set 1 SI occurs.
     *
     * But if the event manager detects that two status bits corresponding to
     * set 0 and set 1 are set, then VPDMA could be processing any one of the
     * descriptor sets. We can't determine which one since there is no
     * mechanism to find out the sequence in which interrupts are occurring!!
     */
    listObj = (VpsDlm_List *) arg;
    if (1u == numEvents)
    {
        /* Findout for which SI (which set) the ISR is called for */
        for (cnt = 0u; cnt < DLM_NUM_DESC_SET; cnt++)
        {
            if ((*eventList) == listObj->siEventNum[cnt])
            {
                curSet = cnt;
                break;
            }
        }

        /* Check if new clients to be linked/unlinked in list */
        if (0u != listObj->totalUpdates)
        {
            /* Always start updating descriptors at the time of last set ISR
             * so that clients descriptors always starts with set 0 or ends
             * with last set */
            if ((DLM_NUM_DESC_SET - 1u) == curSet)
            {
                GT_1trace(DlmTrace, GT_DEBUG,
                    "Start updating descriptors in SI: %d\n", listObj->listNum);
                listObj->startUpdates = TRUE;
            }

            /* Check if descriptors are to be updated for this SI */
            if (TRUE == listObj->startUpdates)
            {
                /* Figure out the next set */
                if ((DLM_NUM_DESC_SET - 1u) == curSet)
                {
                    nextSet = 0u;
                }
                else
                {
                    nextSet = curSet + 1u;
                }
                GT_2trace(DlmTrace, GT_DEBUG,
                    "Updating descriptors in SI(%d) for set: %d\n",
                    listObj->listNum,
                    nextSet);

                /* Update the next set */
                for (updateCnt = 0u;
                     updateCnt < listObj->numUpdates[nextSet];
                     updateCnt++)
                {
                    /* Program required reload descriptors - these are
                     * determined in caller's context, only updating happens
                     * in ISR */
                    VpsHal_vpdmaCreateRLCtrlDesc(
                        listObj->updateRldDesc[nextSet][updateCnt],
                        listObj->updateRldAddr[nextSet][updateCnt],
                        listObj->updateRldSize[nextSet][updateCnt]);
                }
                /* Reset variable so that next ISR won't accidentally redo
                 * again */
                GT_assert(DlmTrace,
                    (listObj->numUpdates[nextSet] <= listObj->totalUpdates));
                listObj->totalUpdates -= listObj->numUpdates[nextSet];
                listObj->numUpdates[nextSet] = 0u;

                /* Add clients to callback list */
                updateClientCb = TRUE;

                /* Check if updates are completed */
                if (0u == listObj->totalUpdates)
                {
                    listObj->startUpdates = FALSE;
                }
            }
        }

        /* Call local client's callback functions.
         * This is called after programming the reload descriptors so that the
         * list is programmed as soon as the ISR is called without any delay */
        node = VpsUtils_getHeadNode(listObj->cbListHandle);
        while (NULL != node)
        {
            client = (VpsDlm_Client *) node->data;
            /* NULL pointer check */
            GT_assert(DlmTrace, (NULL != client));
            /* Caution: Do this before calling client callback function.
             * Reason: DCTRL client could remove the node by calling
             * VpsDlm_unLinkDctrlClient function and hence node->next
             * could become NULL eventhough there are clients after this
             * node!! */
            node = node->next;

            client->cbFxn(curSet, timeStamp, client->arg);
        }

        /* Add clients to callback list after processing the ISR so that
         * callbacks will be called from next SI. */
        if (TRUE == updateClientCb)
        {
            for (updateCnt = 0u;
                 updateCnt < listObj->totalClientCbUpdates;
                 updateCnt++)
            {
                client = listObj->updateClients[updateCnt];
                /* NULL pointer check */
                GT_assert(DlmTrace, (NULL != client));
                client->cbNodeObj.data = client;
                VpsUtils_linkNodeToHead(
                    listObj->cbListHandle,
                    &client->cbNodeObj);
                listObj->updateClients[updateCnt] = NULL;
            }
            listObj->totalClientCbUpdates = 0u;
        }
    }
    else            /* More than one event occurred */
    {
        GT_2trace(DlmTrace, GT_ERR,
            "SI occurred for more than one event: (%d, %d)\n",
            eventList[0u],
            eventList[1u]);
    }

    /* Signal Sync On Register from Master DLM ISR so that the list continues */
    VpsHal_vpdmaSetSyncList(listObj->listNum);
}



/**
 *  vpsDlmListCompleteIsr
 *  \brief DLM List completion ISR used to release any pending stop operation.
 *
 *  This is the callback function registered with the event manager to get list
 *  completion callback for each of the list. For all the lists managed by the
 *  DLM, this is the same function which is registered with the event manager.
 *  The arg parameter is used to get the corresponding list object.
 */
Void vpsDlmListCompleteIsr(const UInt32 *eventList, UInt32 numEvents, Ptr arg)
{
    UInt32          setCnt, updateCnt;
    VpsDlm_List    *listObj;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != eventList));
    GT_assert(DlmTrace, (NULL != arg));

    /* Only registered 1 SI event, hence can't be more/less than that */
    GT_assert(DlmTrace, (1u == numEvents));

    listObj = (VpsDlm_List *) arg;
    /* Check if SI is received for the proper handle and list number */
    GT_assert(DlmTrace, *(eventList) == listObj->listNum);

    GT_1trace(DlmTrace, GT_DEBUG,
        "List Complete for List: %d\n", listObj->listNum);

    /* Unlink any client which was missed out by SI ISR. This can happen for the
     * last client removal from the list */
    if (0u != listObj->totalUpdates)
    {
        GT_1trace(DlmTrace, GT_DEBUG,
            "Removing client from List: %d\n", listObj->listNum);

        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            for (updateCnt = 0u;
                 updateCnt < listObj->numUpdates[setCnt];
                 updateCnt++)
            {
                /* Program required reload descriptors - these are determined in
                 * caller's context, only updating happens in ISR */
                VpsHal_vpdmaCreateRLCtrlDesc(
                    listObj->updateRldDesc[setCnt][updateCnt],
                    listObj->updateRldAddr[setCnt][updateCnt],
                    listObj->updateRldSize[setCnt][updateCnt]);
            }

            /* Reset variable so that next ISR won't accidentally redo again */
            GT_assert(DlmTrace,
                    (listObj->numUpdates[setCnt] <= listObj->totalUpdates));
            listObj->totalUpdates -= listObj->numUpdates[setCnt];
            listObj->numUpdates[setCnt] = 0u;
        }

        GT_assert(DlmTrace, (0u == listObj->totalUpdates));
        listObj->startUpdates = FALSE;
    }

    /* Inform the display controller that the list is complete so that
     * it could free-up the list objects. */
    Dc_dlmListCompleteCb(listObj->listNum);
}



Void VpsDlm_setDispMode(VpsDlm_Handle handle, UInt32 isFbMode)
{
    UInt32          setCnt;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != handle));

    client = (VpsDlm_Client *) handle;

    if (0 != isFbMode)
    {
        client->isFbMode = TRUE;
    }
    else
    {
        client->isFbMode = FALSE;
    }

    /*
     * Note: Change the priority of IN data descriptors when frame
     * buffer mode is selected so that it falls after SOC list control
     * descriptors.
     */
    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        if (TRUE == client->isFbMode)
        {
            client->descNode[setCnt][DLM_IN_DATA_IDX].priority =
                ((setCnt << 16u) | (DLM_SOC_PRIORITY +  1u));
        }
        else
        {
            client->descNode[setCnt][DLM_IN_DATA_IDX].priority =
                ((setCnt << 16u) | VpsDlmDescNodePriorityTbl[DLM_IN_DATA_IDX]);
        }
    }
}

/**
 *  vpsDlmCheckStartStopPrms
 *  Checks the DLM start/stop request and allocates list objects for start
 *  clients.
 */
static VpsDlm_List *vpsDlmCheckStartStopPrms(const VpsDlm_Request *reqObj)
{
    Int32           retVal = VPS_SOK;
    UInt32          cnt, errCnt;
    VpsDlm_List    *listObj = NULL, *tempListObj = NULL;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != reqObj));

    /* Perform a basic error check for start clients */
    if (reqObj->numStart > 0u)
    {
        for (cnt = 0u; cnt < reqObj->numStart; cnt++)
        {
            client = reqObj->startClients[cnt];
            /* NULL pointer check */
            GT_assert(DlmTrace, (NULL != client));
            /* Check if descriptors are allocated and client not started */
            GT_assert(DlmTrace, (TRUE == client->state.isDescMemSet));
            GT_assert(DlmTrace, (TRUE != client->state.isStarted));

            /* Get a list object (free or already assigned to a list)
             * for the client to attach with */
            listObj = (VpsDlm_List *) vpsDlmAllocListObject(
                                          reqObj->listNum,
                                          reqObj->lmfid,
                                          reqObj->socChNum);
            if (NULL == listObj)
            {
                GT_0trace(DlmTrace, GT_ERR, "Alloc list object failed!!\n");
                retVal = VPS_EALLOC;
                if (NULL != tempListObj)
                {
                    for (errCnt = 0u; errCnt < cnt; errCnt++)
                    {
                        vpsDlmFreeListObject(tempListObj);
                    }
                }
                break;
            }
            else
            {
                tempListObj = listObj;
            }
        }
    }

    /* Perform a basic error check for stop clients */
    if ((VPS_SOK == retVal) && (reqObj->numStop > 0u))
    {
        for (cnt = 0u; cnt < reqObj->numStop; cnt++)
        {
            client = reqObj->stopClients[cnt];
            /* NULL pointer check */
            GT_assert(DlmTrace, (NULL != client));
            /* Check if client is running */
            GT_assert(DlmTrace, (TRUE == client->state.isStarted));

            listObj = client->listObj;
            /* NULL pointer check */
            GT_assert(DlmTrace, (NULL != listObj));

            /* Check if all the handles belong to the same list */
            GT_assert(DlmTrace, (reqObj->listNum == listObj->listNum));
        }
    }

    if (NULL != listObj)
    {
        /* Can't get a request when another request is pending */
        GT_assert(DlmTrace, (0u == listObj->totalUpdates));
    }

    return (listObj);
}



/**
 *  vpsDlmProcessStartStopReq
 *  Process the DLM start/stop request by linking the start clients
 *  and unlinking the stop clients from the list.
 */
static Int32 vpsDlmProcessStartStopReq(VpsDlm_List *listObj,
                                       const VpsDlm_Request *reqObj)
{
    Int32           retVal = VPS_SOK;
    UInt32          cnt, setCnt, nextSet;
    UInt32          startIndex, numNodes;
    UInt32          numUpdates, totalUpdates;
    VpsDlm_Client  *client;

    /* NULL pointer check */
    GT_assert(DlmTrace, (NULL != reqObj));
    GT_assert(DlmTrace, (NULL != listObj));

    startIndex = 0u;
    totalUpdates = 0u;

    /*
     * Caution: Stop clients should be unlinked first and then link start
     * clients - don't change this order.
     */
    /* Unlink all the stop clients */
    for (cnt = 0u; cnt < reqObj->numStop; cnt++)
    {
        client = reqObj->stopClients[cnt];

        /* Remove client's descriptors from list */
        numUpdates = vpsDlmUnlinkClientDesc(client, startIndex);
        startIndex += (numUpdates / DLM_NUM_DESC_SET);
        totalUpdates += numUpdates;
        client->state.isStarted = FALSE;
    }

    /* Link all the start clients */
    for (cnt = 0u; cnt < reqObj->numStart; cnt++)
    {
        client = reqObj->startClients[cnt];
        client->listObj = listObj;

        /* Add client's descriptors in the list */
        numUpdates = vpsDlmLinkClientDesc(client, startIndex);
        startIndex += (numUpdates / DLM_NUM_DESC_SET);
        totalUpdates += numUpdates;

        client->state.isStarted = TRUE;
    }

    /* Check if the last client is also removed. If so then break the
     * link list so that the list stops. */
    numNodes = VpsUtils_getNumNodes(listObj->descListHandle);
    if ((DLM_NUM_DESC_SET * DLM_NODES_PER_LIST) == numNodes)
    {
        /* Last client in List - only one left. */
        for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
        {
            /* Figure out the next set */
            if ((DLM_NUM_DESC_SET - 1u) == setCnt)
            {
                nextSet = 0u;
            }
            else
            {
                nextSet = setCnt + 1u;
            }

            /* Link SOR reload descriptor to next set SOC SI descriptor
             * without SOC descriptor.
             * Note: This should be updated in the next set of updates
             * so that when we update in SI, the current SOR RL is
             * updated!! */
            listObj->updateRldAddr[setCnt][(totalUpdates / DLM_NUM_DESC_SET)] =
                (UInt32 *) listObj->siDesc[nextSet];
            listObj->updateRldSize[setCnt][(totalUpdates / DLM_NUM_DESC_SET)] =
               (listObj->listCtrlNode[nextSet][DLM_SOC_IDX].nodeSize -
                    sizeof(VpsHal_VpdmaSyncOnClientDesc));
            listObj->updateRldDesc[setCnt][(totalUpdates / DLM_NUM_DESC_SET)] =
                listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeRldDesc;

            /* Link LMFID reload descriptor to SOR descriptor without
             * the reload descriptor so that the list will stop */
            listObj->updateRldAddr[setCnt]
                                  [(totalUpdates / DLM_NUM_DESC_SET) + 1u] =
                (UInt32 *) listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeAddr;
            listObj->updateRldSize[setCnt]
                                  [(totalUpdates / DLM_NUM_DESC_SET) + 1u] =
               (listObj->listCtrlNode[setCnt][DLM_SOR_IDX].nodeSize -
                    sizeof(VpsHal_VpdmaReloadDesc));
            listObj->updateRldDesc[setCnt]
                                  [(totalUpdates / DLM_NUM_DESC_SET) + 1u] =
                listObj->listCtrlNode[setCnt][DLM_LMFID_IDX].nodeRldDesc;
        }

        /* Update at the end so that / DLM_NUM_DESC_SET while indexing
         * array above don't cause issue */
        totalUpdates += DLM_NUM_DESC_SET;   /* For SOR RL update */
        totalUpdates += DLM_NUM_DESC_SET;   /* For LMFID RL update */
    }

    /* Rotate the SOR RL updates if present */
    vpsDlmRotateSorRlUpdates(listObj, (totalUpdates / DLM_NUM_DESC_SET));

    /* Add the client to callback list, if client is local */
    listObj->totalClientCbUpdates = 0u;
    for (cnt = 0u; cnt < reqObj->numStart; cnt++)
    {
        client = reqObj->startClients[cnt];

        if (TRUE == client->state.isLocalClient)
        {
            GT_assert(DlmTrace,
                (listObj->totalClientCbUpdates < DLM_NUM_CLIENTS));
            listObj->updateClients[listObj->totalClientCbUpdates] = client;
            listObj->totalClientCbUpdates++;
        }
    }

    listObj->startUpdates = FALSE;
    for (setCnt = 0u; setCnt < DLM_NUM_DESC_SET; setCnt++)
    {
        /* Update variables and let the SI ISR add the client to list */
        listObj->numUpdates[setCnt] = (totalUpdates / DLM_NUM_DESC_SET);
    }
    /* Note: This should be updated last after updating for per set */
    listObj->totalUpdates = totalUpdates;

    return (retVal);
}

