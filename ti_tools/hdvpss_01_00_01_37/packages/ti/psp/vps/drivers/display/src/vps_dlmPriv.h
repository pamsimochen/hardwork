/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_dlmPriv.h
 *
 *  \brief VPS DLM internal header file containing instance and
 *  other object declaration and function declarations.
 */

#ifndef _VPS_DLMPRIV_H
#define _VPS_DLMPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Number of VPS display list manager instances.
 *
 *  Three is sufficient - One for each of the three independent VENCs: HD VENC1,
 *  HD_VENC2, SD VENC.
 *  Note: This macro can be changed as and when requirement changes.
 */
#define DLM_NUM_INSTANCE                (3u)

/**
 *  \brief Total number of clients for all the list put together.
 *
 *  8 Display paths - 1 x PRI, 1 x Aux, 3 x GRPX, 2 x Bypass, 1 SEC1.
 *  5 Write-back Capture - 2 x WB capture, 2 x Secondary, 1 SC WB2.
 *  1 Extra, in case!!
 *  Note: This macro can be changed as and when requirement changes.
 */
#define DLM_NUM_CLIENTS                 (14u)

/**
 * \brief Number of list control nodes per list per set.
 *
 * 1 SOC List control node.
 * 1 LM FID change List control node.
 * 1 SOR List control node.
 */
#define DLM_NODES_PER_LIST              (3u)

/**
 *  \brief Number of updates needed per descriptor set per client.
 *
 *  5 Since there are 5 sets of descriptors (Shadow config, Outbound data,
 *  Inbound data, non-shadow config and mosaic data descriptors), one need
 *  to update up to 5 links in the same set.
 */
#define DLM_NUM_UPDATE_PER_SET          (DLM_NODES_PER_CLIENTS)

/**
 *  \brief Total number of updates that can happen in the ISR when supporting
 *  linking and unlinking of multiple clients in one API per set.
 *
 *  DLM_NUM_UPDATE_PER_SET - Number of updates per set.
 *  DLM_NUM_CLIENTS        - Maximum all clients can be added or removed at the
 *                           same time.
 *
 *  2 - Used to break the list for the last client. One for linking the SOR RL
 *  descriptor with next set SOC SI descriptor and one more for linking
 *  the LM FID RL with SOR descriptors without SOR RL descritpor.
 */
#define DLM_MAX_UPDATES                 ((DLM_NUM_UPDATE_PER_SET               \
                                        * DLM_NUM_CLIENTS) + 2u)

/**
 *  \brief Start list control descriptor memory size per list set in bytes.
 *
 *  This is used to set the initial value of LM FID and sync with the odd field
 *  so that list starts from even field.
 *  Note: This descriptor is not part of the circular linked list.
 *
 *  1 Toggle LM FID control descriptor.
 *  1 Sync on Client control descriptor.
 *  1 Reload control descriptor to link to the next descriptor set.
 */
#define DLM_START_LIST_CTRL_DESC_MEMORY                                        \
                                ((1u * sizeof(VpsHal_VpdmaToggleLmFidDesc))    \
                               + (1u * sizeof(VpsHal_VpdmaSyncOnClientDesc))   \
                               + (1u * sizeof(VpsHal_VpdmaReloadDesc)))

/**
 *  \brief SOC list control descriptor memory size per list set in bytes.
 *
 *  1 Sync on Client control descriptor.
 *  1 Send Interrupt control descriptor.
 *  1 Reload control descriptor to link to the next descriptor set.
 */
#define DLM_SOC_LIST_CTRL_DESC_MEMORY                                          \
                                ((1u * sizeof(VpsHal_VpdmaSyncOnClientDesc))   \
                               + (1u * sizeof(VpsHal_VpdmaSendIntrDesc))       \
                               + (1u * sizeof(VpsHal_VpdmaReloadDesc)))

/**
 *  \brief LM FID list control descriptor memory size per list set in bytes.
 *
 *  1 Toggle LM FID control descriptor.
 *  1 Reload control descriptor to link to the next descriptor set.
 */
#define DLM_LM_FID_LIST_CTRL_DESC_MEMORY                                       \
                                ((1u * sizeof(VpsHal_VpdmaToggleLmFidDesc))    \
                               + (1u * sizeof(VpsHal_VpdmaReloadDesc)))

/**
 *  \brief SOR list control descriptor memory size per list set in bytes.
 *
 *  1 Sync On Register control descriptor.
 *  1 Intermediate Reload control descriptor to link to the next reload desc.
 *  1 Reload control descriptor to link to the next descriptor set.
 *
 *  Note:
 *  Reason for using two RL descriptor in list control descriptor:
 *  When clients are added on top of other clients, the list control
 *  reload descriptor is always re-programmed to link to the added client's next
 *  descriptor set. If we were to use a single reload descriptor, the VPDMA
 *  would have already copied the list control descriptor to it's internal
 *  memory. So reprogramming the reload descriptor will not take effect
 *  immediately. Hence one more reload descriptor is added so that VPDMA copies
 *  only SOR and intermediate RL descriptor in to it's internal memory and
 *  when the SI ISR signals the VPDMA to proceed, after reprogramming reload
 *  descriptor, the VPDMA will fetch the actual reload descriptor and start
 *  processing the newly added descriptors immediately!!
 */
#define DLM_SOR_LIST_CTRL_DESC_MEMORY                                          \
                                    ((1u * sizeof(VpsHal_VpdmaSyncOnRegDesc))  \
                                   + (2u * sizeof(VpsHal_VpdmaReloadDesc)))

/*
 *  Default priority for the different types of nodes present in a list layout.
 *  This will determine how a list is formed. This priority is used to
 *  seamlessly link descriptors in a circular prioritized link list.
 */
/** \brief Client's outbound data descriptor priority. */
#define DLM_OUT_DATA_PRIORITY           (10u)
/** \brief Client's inbound data descriptor priority. */
#define DLM_IN_DATA_PRIORITY            (20u)
/** \brief Client's shadow descriptor priority. */
#define DLM_SHADOW_PRIORITY             (30u)
/** \brief List control's SOC descriptor priority. */
#define DLM_SOC_PRIORITY                (40u)
/** \brief Client's non-shadow descriptor priority. */
#define DLM_NSHADOW_PRIORITY            (50u)
/**
 *  \brief List control's LM FID descriptor priority.
 *  Caution: Breaking the list after removing the last client depends on the
 *  fact that LMFID descriptors are inbetween SOC and SOR descriptors.
 */
#define DLM_LMFID_PRIORITY              (60u)
/** \brief Client's Mosaic data descriptor priority. */
#define DLM_MOSAIC_DATA_PRIORITY        (70u)
/** \brief List control's SOR descriptor priority. */
#define DLM_SOR_PRIORITY                (80u)

/*
 *  Index to the list control descriptor node array for various nodes
 *  in a list.
 *  Caution: If this order is changed, then initialization of
 *  DlmListCtrlNodePriorityTbl table should also be changed.
 */
/** \brief Sync On Client descriptor node index. */
#define DLM_SOC_IDX                     (0u)
/** \brief List Manager FID change descriptor node index. */
#define DLM_LMFID_IDX                   (1u)
/** \brief Sync on Register descriptor node index. */
#define DLM_SOR_IDX                     (2u)

/** \brief Time out to be used in sem pend. */
#define DLM_SEM_TIMEOUT                 (BIOS_WAIT_FOREVER)

/**
 *  \brief Flag used to indicate availability of list or not.
 *
 *  Note: This should be other than valid list numbers from 0 to
 *  VPSHAL_VPDMA_MAX_LIST.
 */
#define DLM_INVALID_LIST_NUM            (0xFFu)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**< Typedef for list structure. */
typedef struct VpsDlm_List_t VpsDlm_List;

/**< Typedef for client structure. */
typedef struct VpsDlm_Client_t VpsDlm_Client;

/**
 *  \brief Flags for various states of DLM clients.
 */
typedef struct
{
    UInt32                      isRegistered;
    /**< Client registered or not. */
    UInt32                      isDescMemSet;
    /**< Indicates whether descriptor and other memories are set using set
         descriptor memory API. */
    UInt32                      isStarted;
    /**< Client is added in the circular descriptor list or not. */
    UInt32                      isLocalClient;
    /**< Indicates whether the client is local or not. */
} VpsDlm_States;

/**
 *  \brief Structure to store the information about the descriptor node so that
 *  client addition and removal becomes seamless.
 */
typedef struct
{
    Void                       *nodeAddr;
    /**< Descriptor start address. Used by the previous node to
         program it's RL descriptor. */
    UInt32                      nodeSize;
    /**< Size of the descriptor in bytes. Used by the previous node to
         program it's RL descriptor. */
    VpsHal_VpdmaReloadDesc     *nodeRldDesc;
    /**< Pointer to the Reload descriptor in the descriptor. Used to link to
         the next node. */
    UInt32                      priority;
    /**< Priority of the node. */
    VpsUtils_Node               nodeObj;
    /**< Node object to avoid memory allocation. */
} VpsDlm_Node;

/**
 *  \brief Structure to store per list information.
 */
struct VpsDlm_List_t
{
    UInt8                       listNum;
    /**< The VPDMA list to which this list object belongs to. */
    UInt32                      isListRunning;
    /**< Flag to indicate whether the list is running or not. */
    UInt32                      siEventNum[DLM_NUM_DESC_SET];
    /**< Set of Send Interrupt event numbers registered with the event
         manager. This allocated from the resource manager. */
    UInt32                      numClients;
    /**< Count of clients attached with this list. */

    volatile UInt32             totalUpdates;
    /**< Total number of updates including all set = sumof(numUpdates). */
    volatile UInt32             startUpdates;
    /**< Flag to indicate when the updates should start. When clients are
         added or removed, they are updated to the running list only in the
         last set SI. */
    volatile UInt32             numUpdates[DLM_NUM_DESC_SET];
    /**< Number of updates that needs to be programmed and linked/unlinked
         in SI/LC ISR for each of the sets. */
    Void                       *updateRldAddr[DLM_NUM_DESC_SET]
                                             [DLM_MAX_UPDATES];
    /**< Set of reload address that has to be updated in updateRldDesc in the
         SI ISR when link/unlink client in ISR flag is set. */
    UInt32                      updateRldSize[DLM_NUM_DESC_SET]
                                             [DLM_MAX_UPDATES];
    /**< Set of reload payload size that has to be updated in updateRldDesc in
         the SI ISR when link/unlink client in ISR flag is set. */
    VpsHal_VpdmaReloadDesc     *updateRldDesc[DLM_NUM_DESC_SET]
                                             [DLM_MAX_UPDATES];
    /**< Set of reload descriptor address that has to be programmed
         in the SI ISR when link/unlink client in ISR flag is set. */

    volatile UInt32             totalClientCbUpdates;
    /**< Total number of clients to add to the callback list from the ISR. */
    VpsDlm_Client              *updateClients[DLM_NUM_CLIENTS];
    /**< List of clients that need to be added to callback list in ISR. */

    void                       *descMem[DLM_NUM_DESC_SET];
    /**< Points to the descriptor memory for each set - used for memory
         allocation and free. */
    VpsHal_VpdmaToggleLmFidDesc    *startLmfidDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the Toggle LM FID control descriptor of LM FID list
         control descriptors. */
    VpsHal_VpdmaSyncOnClientDesc   *startSocDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the SOC control descriptor of SOC list
         control descriptors. */
    VpsHal_VpdmaReloadDesc     *startRldDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the Reload control descriptor of start LM FID list
         control descriptors. */

    VpsHal_VpdmaSyncOnClientDesc   *socDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the SOC control descriptor of SOC list
         control descriptors. */
    VpsHal_VpdmaSendIntrDesc   *siDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the Send Interrupt control descriptor of SOC list
         control descriptors. */
    VpsHal_VpdmaToggleLmFidDesc    *lmfidDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the Toggle LM FID control descriptor of LM FID list
         control descriptors. */
    VpsHal_VpdmaSyncOnRegDesc  *sorDesc[DLM_NUM_DESC_SET];
    /**< Pointer to Sync on Register control descriptor of SOR list
         control descriptors. */
    VpsHal_VpdmaReloadDesc     *sorIntrCtrlRldDesc[DLM_NUM_DESC_SET];
    /**< Pointer to the intermediate Reload control descriptor of SOR list
         control descriptors. This is used so that when reload descriptors are
         changed in ISR context (when clients are added/removed), this will
         ensure that the updated reload descriptor are consumed by VPDMA when
         SOR is signaled. Otherwise, the VPDMA will use the already loaded
         reload descriptor which will lead to undesirable results. */

    VpsUtils_Handle             descListHandle;
    /**< Handle to the circular prioritized link list object used to link
         the nodes to form the desired list layout. */
    VpsUtils_Handle             cbListHandle;
    /**< Handle to the link list object used to store the callback clients. */
    VpsDlm_Node                 listCtrlNode[DLM_NUM_DESC_SET]
                                            [DLM_NODES_PER_LIST];
    /**< List control nodes used for linking list control descriptors. */

    Semaphore_Handle            listSem;
    /**< Semaphore used to protect list specific critical region. */

    Void                       *vemSendIntr;
    /**< Event manager handle for SI callback. */
    Void                       *vemListComp;
    /**< Event manager handle for list complete callback. */
};

/**
 *  \brief Structure to store per client information.
 */
struct VpsDlm_Client_t
{
    VpsDlm_List                *listObj;
    /**< List object to which this client belongs to. */
    VpsDlm_States               state;
    /**< Flag to indicate various client states as defined by VpsDlm_States. */
    VpsDlm_ClientType           clientType;
    /**< DLM client type used to determine the priority in adding the client
         descriptors to the list. */
    UInt32                      isFbMode;
    /**< Flag to indicate whether the client is working in frame buffer mode
        or in streaming mode. */
    VpsDlm_ClientCbFxn          cbFxn;
    /**< Client's callback function pointer registered with VpsDlm_register. */
    Ptr                         arg;
    /**< Argument that has to be passed in the client's callback function. This
         is initialized with the arg parameter when each of the clients
         registers. */

    VpsUtils_Node               cbNodeObj;
    /**< Node object for callback link list to avoid memory allocation. */
    VpsDlm_Node                 descNode[DLM_NUM_DESC_SET]
                                        [DLM_NODES_PER_CLIENTS];
    /**< Client nodes used for linking client descriptors. */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 vpsDlmPrivInit(void);
Int32 vpsDlmPrivDeInit(void);

VpsDlm_List *vpsDlmAllocListObject(UInt8 listNum,
                                   UInt32 lmfid,
                                   VpsHal_VpdmaChannel socChNum);
Int32 vpsDlmFreeListObject(VpsDlm_List *listObj);
Int32 vpsDlmFreeListResource(VpsDlm_List *listObj);

VpsDlm_Client *vpsDlmAllocClientObject(void);
Int32 vpsDlmFreeClientObject(VpsDlm_Client *client);

VpsDlm_List *vpsDlmGetListObject(UInt8 listNum);

Void vpsDlmSendInterruptIsr(const UInt32 *eventList, UInt32 numEvents, Ptr arg);
Void vpsDlmListCompleteIsr(const UInt32 *eventList, UInt32 numEvents, Ptr arg);

UInt32 vpsDlmLinkClientDesc(VpsDlm_Client *client, UInt32 startIndex);
UInt32 vpsDlmUnlinkClientDesc(VpsDlm_Client *client, UInt32 startIndex);

Void vpsDlmRotateSorRlUpdates(VpsDlm_List *listObj, UInt32 numUpdates);


#ifdef __cplusplus
}
#endif

#endif /*  _VPS_DLMPRIV_H */
