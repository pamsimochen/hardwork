/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 * \file vps_evtMgr.c
 *
 * \brief Event Manager file
 * This file implements Event Manager for VPS
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/src/vps_intcRegOffset.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/cslr/cslr_hd_vps.h>
#include <ti/sysbios/knl/Clock.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VPS_VIP0_VIP1_ENABLE_DSS_INTC_MASK          (0x00300000)
#define VEM_MAX_LOG             (200)

/**
 *  \brief Semaphore time out value
 */
#define VEM_SEM_TIMEOUT                     (BIOS_WAIT_FOREVER)

#define VEM_REGSIZE_SHIFT                   (5u)

/**
 *  Maximum number of clients for Event Manager
 */
#define VEM_MAX_CLIENTS                     (50u)
#define VEM_MAX_CLIENT_EVENTS               (10u)

/**
 *  \brief Flags used by clientinfo structures memory pool manager to
 *  indicate availability of pool memory
 */
#define VEM_MEM_FLAG_FREE                   (0xFFFFFFFFu)
#define VEM_MEM_FLAG_ALLOC                  (0x00000000u)

/**
 * \brief Bit position that would be used to enable client interrupts
 */
#define VEM_CLIENT_EVENT_ENABLE_MASK        (0x7u)

#define regw(reg, val)      ((*(volatile UInt32 *)(reg)) = (val))
#define regr(reg)           (*(volatile UInt32 *)(reg))


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vem_EventGroupInfo
 *  \brief Structure to keep track of the information related to a group. One
 *  instance of eventGroupinfo will be used for list of events of single group
 *  stored in a single register. It keeps bit position where event group starts
 *  in the register, difference between bit position of two events and registers
 *  to set event, clear event, enable event, disable event.
 */
typedef struct
{
    UInt32  maxEvents;
    /**< Maximum number of events in this event group */
    UInt32  startBit;
    /**< Location of the event group within the register */
    UInt32  bitOffset;
    /**< Defines the offset between two consecutive events within the
         register */
    UInt32  statusReg;
    UInt32  vpdmaStatClrReg;
    UInt32  dssStatClrReg;

    UInt32  vpdmaEnableReg;
    UInt32  dssEnableReg;
    UInt32  vpdmaDisableReg;
    UInt32  dssDisableReg;

    UInt32 numClients;
    UInt32 isDssOnlyIntr;
} Vem_EventGroupInfo;

/**
 * struct Vem_EventManager
 * \brief structure to keep track of information regarding
 * event manager.
 */
typedef struct
{
    Vem_EventGroupInfo      egInfo[VEM_EG_MAX];
    /**< Pointer to the event group structure */
    VpsUtils_Node          *headNode;
    /**< Pointer to the head node of the priority linked list */
    Semaphore_Handle        sem;
    /**< Semaphore handle */
    Hwi_Handle              hwiHandle;
    /**< Handle to Hardware Interrupt */
    Hwi_Handle              hwiVipHandle;
    /**< Handle to Vip Overflow Interrupt */
    Vem_VipCbFxn            captOverflowCallback;
    /**< Capture VIP overflow callback funtion */
    UInt32                  numIntr;
    /**< Total number of interrupts occured since last init */

    UInt32                  numClient[VEM_MAX_LOG];
    UInt32                  timestamp[VEM_MAX_LOG];
    UInt32                  dssRegStatBefore[VEM_MAX_LOG];
    UInt32                  vpdmaRegStatBefore[VEM_MAX_LOG];
    UInt32                  dssRegStatAfter[VEM_MAX_LOG];
    UInt32                  vpdmaRegStatAfter[VEM_MAX_LOG];
    UInt32                  referenceCount[VEM_EG_MAX];
    /**< There could be cases where in an event could span across multiple
         registers. However, the control for this interrupt could be just 1 bit
         (As in case of client interrupt). Use this count to determine, if the
         interrupt could be disabled. */
} Vem_EventManager;

/**
 *  struct Vem_ClientInfo
 *  \brief This structure keeps track of the client information.
 *  It will be filled up when client registers for the event callback to the
 *  event manager. Client can register one or multiple events of a single event
 *  group with a callback. So if any of one or multiple events occurs, event
 *  manager calls the callback function of the client. For each client, this
 *  structure keeps track of register to be checked for the event, register in
 *  which event is to cleared. It also keeps track of eventGroup, priority,
 *  client callback etc.
 *
 *  rawSetReg - Pointer to array containing register addresses in which event
 *               manager will check for the event. Size of this array will be
 *               the number of event for which client has registered the
 *               callback.
 *  rawClearReg - Pointer to array containing register addresses in which event
 *                manager will clear the event. Size of this array will be
 *                the number of event for which client has registered the
 *                callback.
 */
typedef struct
{
    UInt32                  statusReg[VEM_MAX_CLIENT_EVENTS];
    UInt32                  vpdmaStatClrReg[VEM_MAX_CLIENT_EVENTS];
    UInt32                  dssStatClrReg[VEM_MAX_CLIENT_EVENTS];
    UInt32                  statMask[VEM_MAX_CLIENT_EVENTS];
    UInt32                  dssMask[VEM_MAX_CLIENT_EVENTS];
    UInt32                  vpdmaMask[VEM_MAX_CLIENT_EVENTS];
    UInt32                  isDssOnlyIntr[VEM_MAX_CLIENT_EVENTS];
    /**< Pointer to the array containing the masks using which event in
         the rawSetReg will be checked or will be cleared in the rawClearReg */
    Vem_ClientCbFxn         clientCb;
    /**< Client's callback function. This function will be called by the
         event manager whenever client registered event occurs */
    Ptr                     arg;
    /**< client's private data to be passed to it when callback function is
         called */
    Vem_EventGroup          eventGroup;
    /**< This tells the event group into which registered event belongs */
    UInt32                  allEvents[VEM_MAX_CLIENT_EVENTS];
    /**< List of events for which client has registered callback */
    UInt32                  setEvents[VEM_MAX_CLIENT_EVENTS];
    /**< This array contains the list of events occurred at the time of
         interrupt. It will be passed down to the clients.*/
    UInt32                  numEvents;
    /**< Count of events for client has registered callback. */
    Vem_Priority            priority;
    /**< Priority of the client */
    Vem_EventManager       *parent;
    /**< Pointer to the instance of the event manager to which this client
         is registered. */
    UInt32                  numIntr;
    /**< Keeps track of number of interrupts occured for this client */

    UInt32                  timestamp[VEM_MAX_LOG];
} Vem_ClientInfo;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vemFillClientInfo(Vem_ClientInfo *clientInfo,
                              Vem_EventGroup eventGroup,
                              const UInt32 *event,
                              UInt32 numEvents,
                              Vem_Priority priority,
                              Vem_ClientCbFxn callback,
                              Void *arg);

static Void vemDeleteClientInfo(Vem_ClientInfo *clientInfo);

static Vem_ClientInfo *vemCreateClientInfo(VpsUtils_Node **node);

static Void vemDeleteNode(VpsUtils_Node *emNode);

static Void vemAddNode(VpsUtils_Node *newNode, Vem_Priority priority);

static void Vem_masterISR(UArg arg);

#ifdef __DEBUG__
static Void vemPrintList(Vem_EventManager *em);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief Array group info structure to keep track of group information. One
 *  instance of eventGroupinfo will be used for list of events of single group
 *  stored in a single register.
 */
Vem_EventManager gVem_obj = {
    {
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL0_INT_STAT,
         VPDMA_INT_CHANNEL0_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL0_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL0_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL1_INT_STAT,
         VPDMA_INT_CHANNEL1_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL1_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL1_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL2_INT_STAT,
         VPDMA_INT_CHANNEL2_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL2_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL2_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL3_INT_STAT,
         VPDMA_INT_CHANNEL3_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL3_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL3_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL4_INT_STAT,
         VPDMA_INT_CHANNEL4_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL4_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL4_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL5_INT_STAT,
         VPDMA_INT_CHANNEL5_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL5_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL5_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS, 0, 1, VPDMA_INT_CHANNEL6_INT_STAT,
         VPDMA_INT_CHANNEL6_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CHANNEL6_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CHANNEL6_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_MAX_CLIENTS, 0, 1, VPDMA_INT_CLIENT0_INT_STAT,
         VPDMA_INT_CLIENT0_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CLIENT0_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CLIENT0_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_MAX_CLIENTS, 0, 1, VPDMA_INT_CLIENT1_INT_STAT,
         VPDMA_INT_CLIENT1_INT_STAT, VPS_INTC_ENABLED_CLR_REG1,
         VPDMA_INT_CLIENT1_INT_MASK, VPS_INTC_ENABLE_SET_REG1,
         VPDMA_INT_CLIENT1_INT_MASK, VPS_INTC_ENABLE_CLR_REG1,
         0u, FALSE},
        {VPSHAL_VPDMA_MAX_LIST, 0, 2, VPDMA_INT_LIST0_INT_STAT,
         VPDMA_INT_LIST0_INT_STAT, VPS_INTC_ENABLED_CLR_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLE_SET_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLE_CLR_REG0,
         0u, FALSE},
        {VPSHAL_VPDMA_MAX_LIST, 1, 2, VPDMA_INT_LIST0_INT_STAT,
         VPDMA_INT_LIST0_INT_STAT, VPS_INTC_ENABLED_CLR_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLE_SET_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLE_CLR_REG0,
         0, FALSE},
        {VPSHAL_VPDMA_MAX_SI_SOURCE, 16, 1, VPDMA_INT_LIST0_INT_STAT,
         VPDMA_INT_LIST0_INT_STAT, VPS_INTC_ENABLED_CLR_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLE_SET_REG0,
         VPDMA_INT_LIST0_INT_MASK, VPS_INTC_ENABLED_CLR_REG0,
         0u, FALSE},
        {VEM_EE_MAX, 16, 1, VPS_INTC_ENABLED_CLR_REG1,
         0u, VPS_INTC_ENABLED_CLR_REG1,
         0u, VPS_INTC_ENABLE_SET_REG1,
         0u, VPS_INTC_ENABLE_CLR_REG1,
         0u, TRUE},
        {VEM_OE_MAX, 17, 1, VPS_INTC_ENABLED_CLR_REG0,
         0u, VPS_INTC_ENABLED_CLR_REG0,
         0u, VPS_INTC_ENABLE_SET_REG0,
         0u, VPS_INTC_ENABLE_CLR_REG0,
         0u, TRUE}
    },
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/**
 *  \brief Pool of memory for the clientinfo structure and flags for each
 *  instance to indicate whether it is free or allocated
 */
Vem_ClientInfo   gVem_clientInfoMemPool[VEM_MAX_CLIENTS];
UInt32           gVem_clientInfoFlag[VEM_MAX_CLIENTS];

/**
 *  \brief Pool of memory for the Node
 */
static VpsUtils_Node    VemNode[VEM_MAX_CLIENTS];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  \brief Initializes the VPS Event Manager. It marks all the client info
 *  instances in the memory as available and register interrupt handler
 */
Int32 Vem_init(Ptr arg)
{
    Int32               retVal      = 0;
    UInt32              cnt;
    Semaphore_Params    semParams;
    Hwi_Params          hwiParams = {0};

    /* Init node pool and mark flags as free */
    VpsUtils_memset(
        gVem_clientInfoMemPool,
        (UInt8)0,
        sizeof (gVem_clientInfoMemPool));
    for (cnt = 0u; cnt < VEM_MAX_CLIENTS; cnt++)
    {
        gVem_clientInfoFlag[cnt] = VEM_MEM_FLAG_FREE;
    }

    /* Initialise the semaphore parameters and create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    gVem_obj.sem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == gVem_obj.sem)
    {
        retVal = -1;
    }

    /* Clear out any pending interrupts */
    Hwi_clearInterrupt(VEM_IRQNUMBER_INTC3);

    /* Clear out pending interrupts at DSS and VPDMA Level and disable
       all interrupts */
    for (cnt = 0u; cnt < VEM_EG_MAX; cnt ++)
    {
        /* Disable Interrupts at the DSS and VPDMA level */
        if (FALSE == gVem_obj.egInfo[cnt].isDssOnlyIntr)
        {
            regw(gVem_obj.egInfo[cnt].vpdmaDisableReg, 0x0u);
            /* Clean the pending interrupts */
            regw(gVem_obj.egInfo[cnt].vpdmaStatClrReg, 0xFFFFFFFFu);
        }

        regw(gVem_obj.egInfo[cnt].dssDisableReg, 0xFFFFFFFFu);

        /* Clean the pending interrupts */
        regw(gVem_obj.egInfo[cnt].dssStatClrReg, 0xFFFFFFFFu);

        /* Set the reference count to 0x0 */
        gVem_obj.referenceCount[cnt] = 0x0;
    }

    gVem_obj.headNode = NULL;
    gVem_obj.numIntr = 0u;
    memset(gVem_obj.timestamp         , 0, sizeof(gVem_obj.timestamp         ));
    memset(gVem_obj.numClient         , 0, sizeof(gVem_obj.numClient         ));
    memset(gVem_obj.dssRegStatBefore  , 0, sizeof(gVem_obj.dssRegStatBefore  ));
    memset(gVem_obj.vpdmaRegStatBefore, 0, sizeof(gVem_obj.vpdmaRegStatBefore));
    memset(gVem_obj.dssRegStatAfter   , 0, sizeof(gVem_obj.dssRegStatAfter   ));
    memset(gVem_obj.vpdmaRegStatAfter , 0, sizeof(gVem_obj.vpdmaRegStatAfter ));

    /* ----------------- Register ISR --------------- */
    Hwi_Params_init(&hwiParams);
    gVem_obj.hwiHandle = Hwi_create(VEM_IRQNUMBER_INTC3, Vem_masterISR, &hwiParams, 0);
    if (NULL == gVem_obj.hwiHandle)
    {
        GT_1trace(VemTrace, GT_ERR, "%s: Cannot Register IRQ", __FUNCTION__);
        retVal = -1;
    }

    return retVal;
}



/**
 *  \brief De-Initializes event Manager. It deletes semaphore and un-registers
 *  the IRQ handle and removes all the client info instance from the linked
 *  list.
 */
Int32 Vem_deInit(Ptr arg)
{
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;

    /* ------------- Un-register ISR ------------- */
    Hwi_delete(&(gVem_obj.hwiHandle));

    Semaphore_delete(&(gVem_obj.sem));

    /* Free Entire Linked List */
    tempNode = gVem_obj.headNode;
    while (NULL != tempNode)
    {
        emNode = tempNode;
        tempNode = tempNode->next;
        vemDeleteClientInfo((Vem_ClientInfo *)emNode->data);
        vemDeleteNode(emNode);
    }
    return (0);
}

static void Vem_vipOverflowISR(UArg arg)
{
    /* Clear the DSS VIP interrupt */
    regw(VPS_INTC2_ENABLED_CLR_REG0, VPS_VIP0_VIP1_ENABLE_DSS_INTC_MASK);

    if(NULL != gVem_obj.captOverflowCallback)
    {
        gVem_obj.captOverflowCallback();
    }

    regw(VPS_INTC_EOI_ADDR, VPS_INTC2_NUM);

}

/**
 *  \brief Master ISR function. This is interrupt handler registered for the
 *  interrupt. When called, it will search for all events of all
 *  clients to see if it has occurred. If any event is occurred, it will
 *  clear that event and call the callback function of the client.
 */
static void Vem_masterISR(UArg arg)
{
    VpsUtils_Node *tempNode = NULL;
    Vem_ClientInfo *clientInfo = NULL;
    Vem_EventManager *em = NULL;
    UInt32  regValue, numEvents;
    UInt32 cnt;
    UInt32 numClients=0;

    em = (Vem_EventManager *)&gVem_obj;
    tempNode = em->headNode;

    em->timestamp[em->numIntr%VEM_MAX_LOG] = Clock_getTicks();

    em->dssRegStatBefore[em->numIntr%VEM_MAX_LOG]   = regr(VPS_INTC_ENABLED_CLR_REG0);
    em->vpdmaRegStatBefore[em->numIntr%VEM_MAX_LOG] = regr(VPDMA_INT_LIST0_INT_STAT);

    /*
     * Note:
     * ----
     * Clear all DSS (level 1) interrupt status for VPDMA interrupts (level 2)
     * first. This is required because if we clear this interrupt at the
     * time of servicing VPDMA interrupts (level 2), we might miss to service
     * some of VPDMA interrupts which occurs after checking them in the while
     * loop of client and before clearing the DSS and the next VPDMA interrupt.
     *
     * For example, assume we check for VPDMA SI0 and SI1 in this ISR and SI0 is
     * checked first. Now at time t0, SI1 occurred and we enter this ISR.
     * After we check SI0 has occurred or not, SI0 happens. This will
     * set the already set DSS interrupt to 1. But since we have already
     * checked for this event, we go ahead and service SI1 and at that time
     * clear the DSS interupt which was set for both SI0 and SI1. Hence
     * SI0 will not be serviced again till we get this ISR for some other
     * event!!
     */
    /* Traverse through the entire list of registered clients */
    tempNode = em->headNode;
    while (NULL != tempNode)
    {
        clientInfo = (Vem_ClientInfo *) tempNode->data;
        GT_assert(VemTrace, (NULL != clientInfo));
        for (cnt = 0; cnt < clientInfo->numEvents; cnt++)
        {
            /* Clear the status only for 2 level interrupts. Let the
             * single level interrupts be handled as is in the next
             * while loop. */
            if (FALSE == clientInfo->isDssOnlyIntr[cnt])
            {
                regValue = regr(clientInfo->dssStatClrReg[cnt]);
                if (0u != (regValue & (clientInfo->dssMask[cnt])))
                {
                    regw(clientInfo->dssStatClrReg[cnt],
                         clientInfo->dssMask[cnt]);
                }
            }
        }

        /* Move to the next client */
        tempNode = tempNode->next;
    }

    /* Traverse through the entire list of registered clients */
    tempNode = em->headNode;
    while (NULL != tempNode)
    {
        numEvents = 0u;
        clientInfo = (Vem_ClientInfo *) tempNode->data;
        GT_assert(VemTrace, (NULL != clientInfo));
        for (cnt = 0; cnt < clientInfo->numEvents; cnt++)
        {
            /* See if any of registered events has occurred */
            regValue = regr(clientInfo->statusReg[cnt]);
            if (0u != (regValue & (clientInfo->statMask[cnt])))
            {
                /* Make an array of events which has occurred. */
                clientInfo->setEvents[numEvents++] =
                    clientInfo->allEvents[cnt];

                /* Clear the event */
                if (FALSE == clientInfo->isDssOnlyIntr[cnt])
                {
                    /* We have already cleared DSS interrupts (level 1),
                     * just clear VPDMA interrupts (level 2) */
                    regw(clientInfo->vpdmaStatClrReg[cnt],
                         clientInfo->vpdmaMask[cnt]);
                }
                else
                {
                    /* Clear DSS only interrupts */
                    regw(clientInfo->dssStatClrReg[cnt],
                         clientInfo->dssMask[cnt]);
                }
            }
        }

        /* Call callback if any event occurs for the client */
        if (0u != numEvents)
        {
            clientInfo->clientCb(
                clientInfo->setEvents,
                numEvents,
                clientInfo->arg);

            /* Increment total number of interrupts */
            clientInfo->timestamp[clientInfo->numIntr%VEM_MAX_LOG] = Clock_getTicks();
            clientInfo->numIntr ++;

            numClients++;
        }

        /* Move to next client */
        tempNode = tempNode->next;
    }

    em->dssRegStatAfter[em->numIntr%VEM_MAX_LOG]   = regr(VPS_INTC_ENABLED_CLR_REG0);
    em->vpdmaRegStatAfter[em->numIntr%VEM_MAX_LOG] = regr(VPDMA_INT_LIST0_INT_STAT);

    regw(VPS_INTC_EOI_ADDR, VPS_INTC_NUM);

    em->numClient[em->numIntr%VEM_MAX_LOG] = numClients;

    /* Increment total number of interrupts */
    em->numIntr++;

    return;
}



/**
 *  \brief This function is used to register call back function for a specific
 *  event. It takes group number and event number within the group as the
 *  argument and returns handle.
 */
Void *Vem_register(Vem_EventGroup eventGroup,
                   const UInt32 *event,
                   UInt32 numEvents,
                   Vem_Priority priority,
                   Vem_ClientCbFxn callback,
                   Void *arg)
{
    UInt32 cnt;
    VpsUtils_Node *newNode = NULL;
    Vem_ClientInfo *clientInfo = NULL;
    Vem_EventGroupInfo *egInfo = gVem_obj.egInfo;

    /* Check for the error conditions */
    GT_assert(VemTrace, (NULL != event));
    GT_assert(VemTrace, (0u != numEvents));
    GT_assert(VemTrace, (NULL != callback));
    GT_assert(VemTrace, (numEvents <= egInfo[eventGroup].maxEvents));

    for (cnt = 0u; cnt < numEvents; cnt++)
    {
        GT_assert(VemTrace,
            (event[cnt] < egInfo[eventGroup].maxEvents));
    }

    Semaphore_pend(gVem_obj.sem, (UInt32)VEM_SEM_TIMEOUT);


    /* Allocate memory for the client info structure */
    clientInfo = vemCreateClientInfo(&newNode);

    if ((NULL != clientInfo) && (NULL != newNode))
    {
        newNode->data = (Void *)clientInfo;

        clientInfo->parent = &(gVem_obj);
        clientInfo->numIntr = 0u;

        /* Fill up the details about events in the client info structure */
        vemFillClientInfo(
            clientInfo,
            eventGroup,
            event,
            numEvents,
            priority,
            callback,
            arg);

        /* Add the node in the linked list as per the priority */
        vemAddNode(newNode, priority);

        /* Since there are multiple clients that depend on the single client
           interrupt source in DSS, maintain a reference count */
        if (VEM_EG_CLIENT == eventGroup)
        {
            gVem_obj.referenceCount[VEM_EG_CLIENT]++;
        }
    }

    Semaphore_post(gVem_obj.sem);

#ifdef __DEBUG__
    vemPrintList(clientInfo->parent);
#endif

    return ((Void *)newNode);
}

Int32 Vem_registerVipIntr(Vem_VipCbFxn callback)
{

     Int32                       retVal      = 0;
     Hwi_Params             hwiParams = {0};


     /* Clear out any pending interrupts */
     Hwi_clearInterrupt(VEM_IRQNUMBER_INTC2);


    /* ----------------- Register ISR --------------- */
    Hwi_Params_init(&hwiParams);
    hwiParams.priority = 0;
    /* priority zero is used to configure a zero latency interrupt */
    gVem_obj.hwiVipHandle = Hwi_create(VEM_IRQNUMBER_INTC2,
        Vem_vipOverflowISR, &hwiParams, 0);
    if (NULL == gVem_obj.hwiVipHandle)
    {
        GT_1trace(VemTrace, GT_ERR, "%s: Cannot Register IRQ", __FUNCTION__);
        retVal = -1;
    }

     gVem_obj.captOverflowCallback = callback;

     /*Enable VIP0 and VIP1 Interrupts only in DSS INTC for ISS-M3 Interrupt register*/
     regw(VPS_INTC2_ENABLE_SET_REG0, VPS_VIP0_VIP1_ENABLE_DSS_INTC_MASK);

    return retVal;

}

void Vem_unRegisterVipIntr( )
{
    if(gVem_obj.hwiVipHandle != NULL)
    {
        /* ------------- Un-register ISR ------------- */
        Hwi_delete(&(gVem_obj.hwiVipHandle));
    }
}

/**
 *  Int Em_unRegister(Void *handle)
 *  \brief  Function to un-register callback from the event manager. It removes
 *          node from the linked list and frees up memory allocated to the node
 *          and client info structure. It takes handle to the callback and
 *          returns status of the whether callback is removed.
 *          must be valid handle to the callback already registered
 *
 *  \param  handle      Handle to the callback
 *
 *  \return             Indicates whether callback is removed or not.
 */
Int Vem_unRegister(Void *handle)
{
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;
    Vem_ClientInfo *clientInfo = NULL;
    UInt32 cnt, regValue;
    Int index = 0, found = 0;
    Vem_EventGroupInfo *egInfo = NULL;
    Vem_EventManager *em = NULL;

    /* Error condition Checking */
    GT_assert(VemTrace, (NULL != handle));

    tempNode = (VpsUtils_Node *)handle;

    GT_assert(VemTrace, (NULL != tempNode->data));
    em = ((Vem_ClientInfo *)(tempNode->data))->parent;
    GT_assert(VemTrace, (NULL != em));

    egInfo = em->egInfo;
    GT_assert(VemTrace, (NULL != egInfo));
    /* Check to see if the node given is in the linked list */
    emNode = em->headNode;
    GT_assert(VemTrace, (NULL != emNode));
    found = 0;
    while (NULL != emNode)
    {
        if (tempNode == emNode)
        {
            found = 1;
            break;
        }
        emNode = emNode->next;
    }
    GT_assert(VemTrace, (0 != found));

    clientInfo = (Vem_ClientInfo *)tempNode->data;
    GT_assert(VemTrace, (NULL != clientInfo));
    Semaphore_pend(em->sem, (UInt32)VEM_SEM_TIMEOUT);

    /* Delete the Node */
    vemDeleteNode(tempNode);

    /* Disable events in the VPS registers */
    for (cnt = 0u; cnt < clientInfo->numEvents; cnt++)
    {
        if ((VEM_EG_CHANNEL == clientInfo->eventGroup) ||
           (VEM_EG_CLIENT == clientInfo->eventGroup))
        {
            index = clientInfo->eventGroup +
                        (clientInfo->allEvents[cnt] >> 5u);
        }
        else
        {
            index = (Int)clientInfo->eventGroup;
        }

        egInfo[index].numClients --;
        /* Disable the event at the VPDMA */
        if (FALSE == egInfo[index].isDssOnlyIntr)
        {
            regValue = regr(egInfo[index].vpdmaDisableReg);
            regValue &= (~clientInfo->vpdmaMask[cnt]);
            regw(egInfo[index].vpdmaDisableReg, regValue);

            /* Clear the any pending event at the VPDMA */
            /*regValue = regr(egInfo[index].vpdmaStatClrReg);
            regValue &= (~clientInfo->vpdmaMask[cnt]);*/
            regw(egInfo[index].vpdmaStatClrReg, clientInfo->vpdmaMask[cnt]);
        }

        if((VEM_EG_LISTCOMPLETE == clientInfo->eventGroup) ||
           (VEM_EG_LISTNOTIFY == clientInfo->eventGroup))
        {
            regw(egInfo[index].dssDisableReg, clientInfo->dssMask[cnt]);

            /* Clear the any pending event at the DSS. For the
               ListComplete and ListNotify interrupts, there are no
               groups in dss register, so it can be cleared here */
            regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);
        }
        else if(VEM_EG_SI == clientInfo->eventGroup)
        {
            /* Disable At the DSS only if there are no clients registered */
            if (0 == egInfo[index].numClients)
            {
                regw(egInfo[index].dssDisableReg, clientInfo->dssMask[cnt]);
                /* Clear any pending status register */
                regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);
            }
        }
        else if((VEM_EG_CHANNEL == clientInfo->eventGroup) ||
                (VEM_EG_CLIENT == clientInfo->eventGroup))
        {
            /* Disable At the DSS only if there are no clients registered */
            if (0 == egInfo[index].numClients)
            {
                /* In case of client interrupt, check if there are any other
                   clients which are still registered with VEM */
                if (VEM_EG_CLIENT == clientInfo->eventGroup)
                {
                    if (em->referenceCount[VEM_EG_CLIENT] == 0x01)
                    {
                        regw(egInfo[index].dssDisableReg,
                            clientInfo->dssMask[cnt]);
                        /* Clear any pending status register */
                        regw(egInfo[index].dssStatClrReg,
                            clientInfo->dssMask[cnt]);
                    }
                    em->referenceCount[VEM_EG_CLIENT]--;
                }
                else
                {
                    regw(egInfo[index].dssDisableReg, clientInfo->dssMask[cnt]);
                    /* Clear any pending status register */
                    regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);
                }
            }
        }
        clientInfo->dssMask[cnt] = 0u;
        clientInfo->vpdmaMask[cnt] = 0u;
        clientInfo->statMask[cnt] = 0u;
    }

    /* Free up memory allocated to event arrays and clientInfo structure */
    vemDeleteClientInfo(clientInfo);

    Semaphore_post(em->sem);

#ifdef __DEBUG__
    vemPrintList(clientInfo->parent);
#endif

    return (0);
}



/**
 *  \brief Function to add a node to the linked list of clients at the
 *  appropriate position as per the given priority.
 */
Void vemAddNode(VpsUtils_Node *newNode, Vem_Priority priority)
{
    UInt32 cookie;
    VpsUtils_Node *curr = NULL, *prev = NULL;
    Vem_EventManager *em = NULL;

    GT_assert(VemTrace, (NULL != newNode));

    em = ((Vem_ClientInfo *)(newNode->data))->parent;
    GT_assert(VemTrace, (NULL != em));

    newNode->prev = newNode->next = NULL;
    curr = em->headNode;
    prev = NULL;

    /* Search for place to insert new node as per priority */
    while ((NULL != curr) &&
          (((Vem_ClientInfo *)curr->data)->priority < priority))
    {
        prev = curr;
        curr = curr->next;
    }

    /* Disable Interrupt */
    cookie = Hwi_disable();

    /* Update pointers */
    newNode->next = curr;
    newNode->prev = prev;

    if (NULL == prev)
    {
        em->headNode = newNode;
    }
    else
    {
        prev->next = newNode;
    }

    if (NULL != curr)
    {
        curr->prev = newNode;
    }

    /* Enable Interrupt */
    Hwi_restore(cookie);

}



/**
 *  \brief Function to remove the node from the linked list and delete memory
 *  allocated to it.
 */
Void vemDeleteNode(VpsUtils_Node *emNode)
{
    UInt32 cookie;
    VpsUtils_Node *curr = NULL, *prev = NULL;
    Vem_EventManager *em = NULL;

    GT_assert(VemTrace, (NULL != emNode));

    em = ((Vem_ClientInfo *)(emNode->data))->parent;
    prev = emNode->prev;
    curr = emNode->next;

    /* Disable Interrupts */
    cookie = Hwi_disable();

    if (NULL != prev)
    {
        if(NULL != curr)
        {
            prev->next = curr;
        }
        else
        {
            prev->next = NULL;
        }

    }
    else
    {
        em->headNode = curr;
        if (NULL != em->headNode)
        {
            em->headNode->prev = NULL;
        }
    }
    if (NULL != curr)
    {
        curr->prev = prev;
    }

    /* Enable Interrupts */
    Hwi_restore(cookie);
}



/**
 *  \brief Function to allocate memory for the client information structure
 *  and other arrays.
 */
Vem_ClientInfo *vemCreateClientInfo(VpsUtils_Node **node)
{
    Vem_ClientInfo *clientInfo=NULL;
    UInt32 cnt;

    for (cnt = 0u; cnt < VEM_MAX_CLIENTS; cnt++)
    {
        if(VEM_MEM_FLAG_FREE == gVem_clientInfoFlag[cnt])
        {
            clientInfo = &(gVem_clientInfoMemPool[cnt]);
            *node = &VemNode[cnt];
            gVem_clientInfoFlag[cnt] = VEM_MEM_FLAG_ALLOC;
            break;
        }
    }

    return (clientInfo);
}



/**
 *  \brief Function to free up memory for the client information structure
 *  and other arrays.
 */
static Void vemDeleteClientInfo(Vem_ClientInfo *clientInfo)
{
    UInt32 cnt;
    GT_assert(VemTrace, (NULL != clientInfo));

    for (cnt = 0u; cnt < VEM_MAX_CLIENTS; cnt++)
    {
        if(clientInfo == &(gVem_clientInfoMemPool[cnt]))
        {
            gVem_clientInfoFlag[cnt] = VEM_MEM_FLAG_FREE;
            break;
        }
    }
}



/**
 *  \brief Function to fill up client information structure.
 */
Void vemFillClientInfo(Vem_ClientInfo *clientInfo,
                       Vem_EventGroup eventGroup,
                       const UInt32 *event,
                       UInt32 numEvents,
                       Vem_Priority priority,
                       Vem_ClientCbFxn callback,
                       Void *arg)
{
    UInt32 cnt, index, eventOffset;
    UInt32 regValue = 0;
    Vem_EventGroupInfo *egInfo = clientInfo->parent->egInfo;

    GT_assert(VemTrace, (NULL != clientInfo));
    GT_assert(VemTrace, (NULL != event));
    GT_assert(VemTrace, (NULL != callback));
    GT_assert(VemTrace, (NULL != egInfo));

    clientInfo->eventGroup  = eventGroup;
    clientInfo->numEvents   = numEvents;
    for (cnt = 0u; cnt < numEvents; cnt++)
    {
        if ((VEM_EG_CHANNEL == eventGroup) ||
            (VEM_EG_CLIENT == eventGroup))
        {
            index = (UInt32)eventGroup + (event[cnt] >> VEM_REGSIZE_SHIFT);
            eventOffset = event[cnt] & ((1u << VEM_REGSIZE_SHIFT) - 1u);
        }
        else
        {
            index = (UInt32)eventGroup;
            eventOffset = event[cnt];
        }
        clientInfo->allEvents[cnt]  = event[cnt];
        /* Store the registers from which event status can be checked or into
           which event status can be cleared */
        clientInfo->statusReg[cnt] = egInfo[index].statusReg;
        clientInfo->vpdmaStatClrReg[cnt]= egInfo[index].vpdmaStatClrReg;
        clientInfo->dssStatClrReg[cnt]= egInfo[index].dssStatClrReg;

        clientInfo->statMask[cnt] = 1 << ((eventOffset *
                                  egInfo[index].bitOffset) +
                                  egInfo[index].startBit);

        /* Enable the event interrupt in the enable set register */
        GT_3trace(VemTrace,
                  GT_INFO,
                  "%s: Enable Reg %x mask %x",
                  __FUNCTION__,
                  egInfo[index].vpdmaEnableReg,
                  clientInfo->statMask[cnt]);

        if((VEM_EG_LISTCOMPLETE == eventGroup) ||
           (VEM_EG_LISTNOTIFY == eventGroup))
        {
            /* Since the list complete and list notify are at the
               same position in DSS as well in VPDMA, second mask
               is same as the first mask */
            clientInfo->dssMask[cnt] = clientInfo->statMask[cnt];

            GT_3trace(VemTrace,
                      GT_INFO,
                      "%s: Enable Reg %x mask %x",
                      __FUNCTION__,
                      egInfo[index].dssEnableReg,
                      clientInfo->dssMask[cnt]);

            /* Clean any pending interrupts on this event */
            regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);

            /* Enable the event interrupt in the enable set register */
            /* DSS registers can written directly as writing 0 has no effect */
            regw(egInfo[index].dssEnableReg, clientInfo->dssMask[cnt]);
        }
        else if((VEM_EG_SI == eventGroup))
        {
            clientInfo->dssMask[cnt] = 1u << egInfo[index].startBit;

            /* Clean any pending interrupts on this event */
            regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);

            /* Enable interrupt */
            regw(egInfo[index].dssEnableReg, clientInfo->dssMask[cnt]);
            GT_4trace(VemTrace,
                      GT_INFO,
                      "%s: Enable Reg %x Value %x RegValue %x",
                      __FUNCTION__,
                      egInfo[index].dssEnableReg,
                      regValue,
                      regr(egInfo[index].dssEnableReg));
        }
        else if((VEM_EG_CHANNEL == eventGroup) ||
                (VEM_EG_CLIENT == eventGroup))
        {
            if (VEM_EG_CLIENT == eventGroup)
            {
                /* Unlike channel interrupts, there is 1 bit that controls
                   clients interrupt generation */
                clientInfo->dssMask[cnt] = (1u << VEM_CLIENT_EVENT_ENABLE_MASK);
            }
            else
            {
                clientInfo->dssMask[cnt] = (1u << index);
            }

            /* Clean any pending interrupts on this event */
            regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);

            /* Enable interrupt */
            regw(egInfo[index].dssEnableReg, clientInfo->dssMask[cnt]);
            GT_4trace(VemTrace,
                      GT_INFO,
                      "%s: Enable Reg %x Value %x RegValue %x",
                      __FUNCTION__,
                      egInfo[index].dssEnableReg,
                      regValue,
                      regr(egInfo[index].dssEnableReg));
        }
        else if ((VEM_EG_OTHERS == eventGroup) ||
                 (VEM_EG_ERROR == eventGroup))
        {
            clientInfo->dssMask[cnt] = clientInfo->statMask[cnt];

            /* Clean any pending interrupts on this event */
            regw(egInfo[index].dssStatClrReg, clientInfo->dssMask[cnt]);

            /* Enable interrupt */
            regw(egInfo[index].dssEnableReg, clientInfo->dssMask[cnt]);
            GT_4trace(VemTrace,
                      GT_INFO,
                      "%s: Enable Reg %x Value %x RegValue %x",
                      __FUNCTION__,
                      egInfo[index].dssEnableReg,
                      regValue,
                      regr(egInfo[index].dssEnableReg));
        }

        /* Enable Event at the Vpdma Level */
        if (FALSE == egInfo[index].isDssOnlyIntr)
        {
            clientInfo->vpdmaMask[cnt] = clientInfo->statMask[cnt];

            /* Clean any pending interrupts on this event */
            regw(egInfo[index].vpdmaStatClrReg, clientInfo->vpdmaMask[cnt]);

            /* VPDMA interrupt registers are read-modify-write registers */
            regValue = regr(egInfo[index].vpdmaEnableReg);
            regValue |= clientInfo->vpdmaMask[cnt];

            /* Enable interrupt */
            regw(egInfo[index].vpdmaEnableReg, regValue);

            /* VPDMA mask is same as the statMask for these interrupts */
            clientInfo->vpdmaMask[cnt] = clientInfo->statMask[cnt];
        }
        GT_4trace(VemTrace,
                  GT_INFO,
                  "%s: Enable Reg %x mask %x RegValue %x",
                  __FUNCTION__,
                  egInfo[index].vpdmaEnableReg,
                  clientInfo->vpdmaMask[cnt],
                  regr(egInfo[index].vpdmaEnableReg));

        /* Increment the number of clients for this event */
        egInfo[index].numClients ++;
        clientInfo->isDssOnlyIntr[cnt] = egInfo[index].isDssOnlyIntr;
    }

    clientInfo->clientCb    = callback;
    clientInfo->priority    = priority;

    if(NULL != arg)
    {
        clientInfo->arg = arg;
    }
    else
    {
        clientInfo->arg = NULL;
    }
}



#ifdef __DEBUG__
Void vemPrintList(Vem_EventManager *em)
{
    VpsUtils_Node *tempNode = em->headNode;
    Vem_ClientInfo *clientInfo = NULL;
    UInt32 nodeNum = 0u, cnt;

    while (NULL != tempNode)
    {
        clientInfo = (Vem_ClientInfo *)tempNode->data;

        GT_2trace(VemTrace,
                  GT_INFO,
                  "%s:Node Number %d",
                  __FUNCTION__,
                  nodeNum++);
        GT_2trace(VemTrace,
                  GT_INFO,
                  "%s:  Node Number %d",
                  __FUNCTION__,
                  clientInfo->priority);
        GT_2trace(VemTrace,
                  GT_INFO,
                  "%s:  Number of Registered Events %d",
                  __FUNCTION__,
                  clientInfo->numEvents);
        GT_2trace(VemTrace,
                  GT_INFO,
                  "%s:  Event Group                 %d",
                  __FUNCTION__,
                  clientInfo->eventGroup);
        for (cnt = 0u; cnt < clientInfo->numEvents; cnt++)
        {
            GT_2trace(VemTrace,
                      GT_INFO,
                      "%s:    Event Number   %d",
                      __FUNCTION__,
                      clientInfo->allEvents[cnt]);
            GT_2trace(VemTrace,
                      GT_INFO,
                      "%s:    Raw Set Reg    %d",
                      __FUNCTION__,
                      clientInfo->statusReg[cnt]);
            GT_2trace(VemTrace,
                      GT_INFO,
                      "%s:    Raw Clear Reg  %d",
                      __FUNCTION__,
                      clientInfo->vpdmaStatClrReg[cnt]);
            GT_2trace(VemTrace,
                      GT_INFO,
                      "%s:    mask           %x",
                      __FUNCTION__,
                      clientInfo->mask[cnt]);
            GT_2trace(VemTrace,
                      GT_INFO,
                      "%s:    -----------------",
                      __FUNCTION__);
        }
        GT_2trace(VemTrace,
                  GT_INFO,
                  "%s:===============\n",
                  __FUNCTION__);
        tempNode = tempNode->next;
    }
}
#endif

