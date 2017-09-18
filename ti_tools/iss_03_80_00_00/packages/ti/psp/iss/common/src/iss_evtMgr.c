/** ==================================================================
 *  @file   iss_evtMgr.c
 *
 *  @path   /ti/psp/iss/common/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \file iss_evtMgr.c
 *
 * \brief Event Manager file
 * This file implements Event Manager for ISS
 *
 */

/* ==========================================================================
 */
/* Include Files */
/* ==========================================================================
 */

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/iss/common/trace.h>
#include <ti/psp/iss/common/iss_utils.h>
#include <ti/psp/iss/common/iss_evtMgr.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>
#include <ti/psp/iss/hal/iss/isp/common/inc/isp_common.h>

/* ==========================================================================
 */
/* Macros & Typedefs */
/* ==========================================================================
 */

#define IemTrace 0

/**
 *  \brief Semaphore time out value
 */
#define IEM_SEM_TIMEOUT                     (BIOS_WAIT_FOREVER)

/**
 *  Maximum number of clients for Event Manager
 */
#define IEM_MAX_CLIENTS                     (50u)
#define IEM_MAX_CLIENT_EVENTS               (10u)

/**
 *  \brief Flags used by clientinfo structures memory pool manager to
 *  indicate availability of pool memory
 */
#define IEM_MEM_FLAG_FREE                   (0xFFFFFFFFu)
#define IEM_MEM_FLAG_ALLOC                  (0x00000000u)

#define IEM_ISS_HL_IRQ_REG_DIFF             (0x10)
#define IEM_ISP5_BANK_REG_DIFF              (0x10)

#define regw(reg, val)      ((*(volatile UInt32 *)(reg)) = (val))
#define regr(reg)           (*(volatile UInt32 *)(reg))

/* ==========================================================================
 */
/* Structure Declarations */
/* ==========================================================================
 */

/**
 * struct Iem_EventManager
 * \brief structure to keep track of information regarding
 * event manager.
 */
typedef struct {
    VpsUtils_Node *headNode[IEM_IRQ_MAX];
    /**< Pointer to the head node of the priority linked list */
    Semaphore_Handle sem;
    /**< Semaphore handle */
    Hwi_Handle hwiHandle[IEM_IRQ_MAX];
    /**< Handle to Hardware Interrupt */
    UInt32 numIntr[IEM_IRQ_MAX];
    /**< Total number of interrupts occured since last init */
} Iem_EventManager;

/**
 *  struct Iem_ClientInfo
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
typedef struct {
    UInt32 hlStatReg;
    UInt32 hlStatClrReg;
    UInt32 hlMask;

    UInt32 evntStatReg;
    UInt32 evntStatClrReg;
    UInt32 evntMask;

    Iem_ClientCbFxn clientCb;
    /**< Client's callback function. This function will be called by the
         event manager whenever client registered event occurs */
    Ptr arg;
    /**< client's private data to be passed to it when callback function is
         called */

    Iem_IrqNum irqNum;
    Iem_BankNum bankId;
    Iem_Event event;
    Iem_Priority priority;

    UInt32 numIntr;

    Iem_EventManager *parent;
} Iem_ClientInfo;

/* ==========================================================================
 */
/* Function Declarations */
/* ==========================================================================
 */

static Void vemDeleteClientInfo(Iem_ClientInfo * clientInfo);
static Iem_ClientInfo *vemCreateClientInfo(VpsUtils_Node ** node);
static Void vemDeleteNode(VpsUtils_Node * emNode, UInt32 irqNum);
static Void vemAddNode(VpsUtils_Node * newNode, Iem_Priority priority, UInt32 irqNum);
void Iem_masterISR(UArg arg);

#ifdef __DEBUG__
static Void vemPrintList(Iem_EventManager * em);
#endif

/* ==========================================================================
 */
/* Global Variables */
/* ==========================================================================
 */

/**
 *  \brief Array group info structure to keep track of group information. One
 *  instance of eventGroupinfo will be used for list of events of single group
 *  stored in a single register.
 */
Iem_EventManager gIem_obj;

/**
 *  \brief Pool of memory for the clientinfo structure and flags for each
 *  instance to indicate whether it is free or allocated
 */
Iem_ClientInfo gIem_clientInfoMemPool[IEM_MAX_CLIENTS];

UInt32 gIem_clientInfoFlag[IEM_MAX_CLIENTS];

/**
 *  \brief Pool of memory for the Node
 */
static VpsUtils_Node IemNode[IEM_MAX_CLIENTS];

extern iss_regs_ovly iss_regs; //ISS TOP
extern isp_regs_ovly isp_regs; //ISS5

/* ==========================================================================
 */
/* Function Definitions */
/* ==========================================================================
 */

/**
 *  \brief Initializes the ISS Event Manager. It marks all the client info
 *  instances in the memory as available and register interrupt handler
 */
Int32 Iem_init(Ptr arg)
{
    Int32 retVal = 0;
    UInt32 cnt;

    Semaphore_Params semParams;
    Hwi_Params hwiParams[IEM_IRQ_MAX] = { 0 };

    /* Init node pool and mark flags as free */
    VpsUtils_memset(gIem_clientInfoMemPool,
                    (UInt8) 0, sizeof(gIem_clientInfoMemPool));
    for (cnt = 0u; cnt < IEM_MAX_CLIENTS; cnt++)
    {
        gIem_clientInfoFlag[cnt] = IEM_MEM_FLAG_FREE;
    }

    /* Initialise the semaphore parameters and create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    gIem_obj.sem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == gIem_obj.sem)
    {
        retVal = -1;
    }

    /* ----------------- Register ISR --------------- */
    for (cnt = 0u; cnt < IEM_IRQ_MAX; cnt ++)
    {
        gIem_obj.headNode[cnt] = NULL;
        gIem_obj.numIntr[cnt] = 0u;

        Hwi_clearInterrupt(IEM_IRQNUMBER0 + cnt);

        Hwi_Params_init(&hwiParams[cnt]);

        hwiParams[cnt].arg = (UArg)cnt;
	gIem_obj.hwiHandle[cnt] = NULL;

#if 0
        gIem_obj.hwiHandle[cnt] =
            Hwi_create(IEM_IRQNUMBER0 + cnt, Iem_masterISR, &hwiParams[cnt], 0);

        if (NULL == gIem_obj.hwiHandle[cnt])
        {
            System_printf("%s: Cannot Register IRQ %d", __FUNCTION__, IEM_IRQNUMBER0 + cnt);
            GT_1trace(IemTrace, GT_ERR, "%s: Cannot Register IRQ", __FUNCTION__);
            retVal = -1;
        }
#endif
    }

    Iem_disableAll();
    cnt = 0;
    Hwi_clearInterrupt(IEM_IRQNUMBER0 + cnt);

    Hwi_Params_init(&hwiParams[cnt]);

    hwiParams[cnt].arg = (UArg)cnt;

    gIem_obj.hwiHandle[cnt] =
        Hwi_create(IEM_IRQNUMBER0 + cnt, Iem_masterISR, &hwiParams[cnt], 0);

    if (NULL == gIem_obj.hwiHandle[cnt])
    {
        System_printf("%s: Cannot Register IRQ %d", __FUNCTION__, IEM_IRQNUMBER0 + cnt);
        GT_1trace(IemTrace, GT_ERR, "%s: Cannot Register IRQ", __FUNCTION__);
        retVal = -1;
    }

    return retVal;
}

/**
 *  \brief De-Initializes event Manager. It deletes semaphore and un-registers
 *  the IRQ handle and removes all the client info instance from the linked
 *  list.
 */
Int32 Iem_deInit(Ptr arg)
{
    UInt32 cnt;
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;

    Semaphore_delete(&(gIem_obj.sem));

    /* ------------- Un-register ISR ------------- */
    for (cnt = 0u; cnt < IEM_IRQ_MAX; cnt ++)
    {
        if (NULL != gIem_obj.hwiHandle[cnt])
            Hwi_delete(&(gIem_obj.hwiHandle[cnt]));

        /* Free Entire Linked List */
        tempNode = gIem_obj.headNode[cnt];
        while (NULL != tempNode)
        {
            emNode = tempNode;
            tempNode = tempNode->next;
            vemDeleteClientInfo((Iem_ClientInfo *) emNode->data);
            vemDeleteNode(emNode, cnt);
        }
    }

    return (0);
}

/**
 *  \brief Master ISR function. This is interrupt handler registered for the
 *  interrupt. When called, it will search for all events of all
 *  clients to see if it has occurred. If any event is occurred, it will
 *  clear that event and call the callback function of the client.
 */
void Iem_masterISR(UArg arg)
{
    VpsUtils_Node *tempNode = NULL;
    Iem_ClientInfo *clientInfo = NULL;
    Iem_EventManager *em = NULL;
    UInt32 irqNum;

    irqNum = (UInt32) arg;
    em = (Iem_EventManager *) &gIem_obj;

    Hwi_disableInterrupt(IEM_IRQNUMBER0 + irqNum);

    tempNode = em->headNode[irqNum];
    while (NULL != tempNode)
    {
        clientInfo = (Iem_ClientInfo *) tempNode->data;
        GT_assert(IemTrace, (NULL != clientInfo));

        if (clientInfo->hlMask & regr(clientInfo->hlStatReg))
        {
            regw(clientInfo->hlStatClrReg, clientInfo->hlMask);
        }

        /* Move to the next client */
        tempNode = tempNode->next;
    }

    tempNode = em->headNode[irqNum];
    while (NULL != tempNode)
    {
        clientInfo = (Iem_ClientInfo *) tempNode->data;
        GT_assert(IemTrace, (NULL != clientInfo));

        if (clientInfo->evntMask & regr(clientInfo->evntStatReg))
        {
            /* Clear the status and call the callback */
            regw(clientInfo->evntStatClrReg, clientInfo->evntMask);

            clientInfo->clientCb(
                        clientInfo->event,
                        clientInfo->arg);

            clientInfo->numIntr ++;
        }

        /* Move to next client */
        tempNode = tempNode->next;
    }

    //regw(VPS_INTC_EOI_ADDR, VPS_INTC_NUM);

    Hwi_clearInterrupt(IEM_IRQNUMBER0 + irqNum);
    Hwi_enableInterrupt(IEM_IRQNUMBER0 + irqNum);

    return;
}

/**
 *  \brief This function is used to register call back function for a specific
 *  event. It takes group number and event number within the group as the
 *  argument and returns handle.
 */
Void *Iem_register(
        Iem_IrqNum      irqNum,
        Iem_BankNum     bankId,
        Iem_Event       event,
        Iem_Priority    priority,
        Iem_ClientCbFxn cbFxn,
        Void            *priv)
{
    UInt32              regAddr;
    VpsUtils_Node      *newNode = NULL;
    Iem_ClientInfo     *clientInfo = NULL;

    /* Check for the error conditions */
    GT_assert(IemTrace, (NULL != cbFxn));

    Semaphore_pend(gIem_obj.sem, (UInt32) IEM_SEM_TIMEOUT);

    /* Allocate memory for the client info structure */
    clientInfo = vemCreateClientInfo(&newNode);

    if ((NULL != clientInfo) && (NULL != newNode))
    {
        newNode->data = (Void *) clientInfo;

        clientInfo->parent = &(gIem_obj);
        clientInfo->numIntr = 0u;

        /* Currently only ISP interrupts are supported */
        if (bankId <= IEM_IRQ_BANK_NUM_ISP_IRQ3)
        {
            /* Enable Interrupt at Top */
            regAddr = (UInt32)&iss_regs->ISS_HL_IRQENABLE_SET_0;
            regAddr = regAddr + (IEM_ISS_HL_IRQ_REG_DIFF * irqNum);
            regw(regAddr, (1 << bankId));

            /* Enable Interrupt at ISP level */
            regAddr = (UInt32)&isp_regs->ISP5_IRQENABLE_SET_0;
            regAddr = regAddr + (IEM_ISP5_BANK_REG_DIFF * bankId);
            regw(regAddr, (1 << event));

            regAddr = (UInt32)&iss_regs->ISS_HL_IRQSTATUS_0;
            regAddr = regAddr + (IEM_ISS_HL_IRQ_REG_DIFF * irqNum);
            clientInfo->hlStatReg = regAddr;
            clientInfo->hlStatClrReg = regAddr;
            clientInfo->hlMask = (1 << bankId);

            regAddr = (UInt32)&isp_regs->ISP5_IRQSTATUS_0;
            regAddr = regAddr + (IEM_ISP5_BANK_REG_DIFF * bankId);
            clientInfo->evntStatReg = regAddr;
            clientInfo->evntStatClrReg = regAddr;
            clientInfo->evntMask = (1 << event);

            clientInfo->irqNum = irqNum;
            clientInfo->bankId = bankId;
            clientInfo->event = event;
            clientInfo->priority = priority;
            clientInfo->clientCb = cbFxn;
        }

        /* Add the node in the linked list as per the priority */
        vemAddNode(newNode, priority, irqNum);
    }

    Semaphore_post(gIem_obj.sem);

#ifdef __DEBUG__
    vemPrintList(clientInfo->parent);
#endif

    return ((Void *) newNode);
}

Void Iem_disableAll()
{
    iss_regs->ISS_HL_IRQENABLE_CLR_0 = 0xFFFFFFFFu;
    isp_regs->ISP5_IRQENABLE_CLR_0 = 0xFFFFFFFFu;
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
Int Iem_unRegister(Void * handle)
{
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;
    Iem_ClientInfo *clientInfo = NULL;
    UInt32 found = 0, regAddr;
    Iem_EventManager *em = NULL;

    /* Error condition Checking */
    GT_assert(IemTrace, (NULL != handle));

    tempNode = (VpsUtils_Node *) handle;

    clientInfo = (Iem_ClientInfo *) (tempNode->data);
    em = ((Iem_ClientInfo *) (tempNode->data))->parent;

    GT_assert(IemTrace, (NULL != clientInfo));
    GT_assert(IemTrace, (NULL != em));

    /* Check to see if the node given is in the linked list */
    emNode = em->headNode[clientInfo->irqNum];
    GT_assert(IemTrace, (NULL != emNode));
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
    GT_assert(IemTrace, (0 != found));

    Semaphore_pend(em->sem, (UInt32) IEM_SEM_TIMEOUT);

    /* Disable Events */
    if (clientInfo->bankId <= IEM_IRQ_BANK_NUM_ISP_IRQ3)
    {
        /* Disable Interrupt at Top */
        regAddr = (UInt32)&iss_regs->ISS_HL_IRQENABLE_CLR_0;
        regAddr = regAddr + (IEM_ISS_HL_IRQ_REG_DIFF * clientInfo->irqNum);
        regw(regAddr, (1 << clientInfo->bankId));

        /* Disable Interrupt at ISP level */
        regAddr = (UInt32)&isp_regs->ISP5_IRQENABLE_CLR_0;
        regAddr = regAddr + (IEM_ISP5_BANK_REG_DIFF * clientInfo->bankId);
        regw(regAddr, (1 << clientInfo->event));

        regw(clientInfo->evntStatClrReg, clientInfo->evntMask);
        regw(clientInfo->hlStatClrReg, clientInfo->hlMask);
    }

    /* Delete the Node */
    vemDeleteNode(tempNode, clientInfo->irqNum);

    /* Free up memory allocated to event arrays and clientInfo structure */
    vemDeleteClientInfo(clientInfo);

    Semaphore_post(em->sem);

#ifdef __DEBUG__
    vemPrintList(clientInfo->parent);
#endif

    return (0);
}

Void Iem_enableInt(Void * handle)
{
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;
    Iem_ClientInfo *clientInfo = NULL;
    UInt32 found = 0, regAddr;
    Iem_EventManager *em = NULL;

    /* Error condition Checking */
    GT_assert(IemTrace, (NULL != handle));

    tempNode = (VpsUtils_Node *) handle;

    clientInfo = (Iem_ClientInfo *) (tempNode->data);
    em = ((Iem_ClientInfo *) (tempNode->data))->parent;

    GT_assert(IemTrace, (NULL != clientInfo));
    GT_assert(IemTrace, (NULL != em));

    /* Check to see if the node given is in the linked list */
    emNode = em->headNode[clientInfo->irqNum];
    GT_assert(IemTrace, (NULL != emNode));
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
    GT_assert(IemTrace, (0 != found));

    Semaphore_pend(em->sem, (UInt32) IEM_SEM_TIMEOUT);

    /* Disable Events */
    if (clientInfo->bankId <= IEM_IRQ_BANK_NUM_ISP_IRQ3)
    {
        /* Enable Interrupt at ISP level */
        regAddr = (UInt32)&isp_regs->ISP5_IRQENABLE_SET_0;
        regAddr = regAddr + (IEM_ISP5_BANK_REG_DIFF * clientInfo->bankId);
        regw(regAddr, (1 << clientInfo->event));
    }

    Semaphore_post(em->sem);
}

Void Iem_disableInt(Void * handle)
{
    VpsUtils_Node *tempNode = NULL, *emNode = NULL;
    Iem_ClientInfo *clientInfo = NULL;
    UInt32 found = 0, regAddr;
    Iem_EventManager *em = NULL;

    /* Error condition Checking */
    GT_assert(IemTrace, (NULL != handle));

    tempNode = (VpsUtils_Node *) handle;

    clientInfo = (Iem_ClientInfo *) (tempNode->data);
    em = ((Iem_ClientInfo *) (tempNode->data))->parent;

    GT_assert(IemTrace, (NULL != clientInfo));
    GT_assert(IemTrace, (NULL != em));

    /* Check to see if the node given is in the linked list */
    emNode = em->headNode[clientInfo->irqNum];
    GT_assert(IemTrace, (NULL != emNode));
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
    GT_assert(IemTrace, (0 != found));

    Semaphore_pend(em->sem, (UInt32) IEM_SEM_TIMEOUT);

    /* Disable Events */
    if (clientInfo->bankId <= IEM_IRQ_BANK_NUM_ISP_IRQ3)
    {
        /* Disable Interrupt at ISP level */
        regAddr = (UInt32)&isp_regs->ISP5_IRQENABLE_CLR_0;
        regAddr = regAddr + (IEM_ISP5_BANK_REG_DIFF * clientInfo->bankId);
        regw(regAddr, (1 << clientInfo->event));
    }

    Semaphore_post(em->sem);
}


/**
 *  \brief Function to add a node to the linked list of clients at the
 *  appropriate position as per the given priority.
 */
Void vemAddNode(VpsUtils_Node * newNode, Iem_Priority priority, UInt32 irqNum)
{
    UInt32 cookie;
    VpsUtils_Node *curr = NULL, *prev = NULL;
    Iem_EventManager *em = NULL;

    GT_assert(IemTrace, (NULL != newNode));

    em = ((Iem_ClientInfo *) (newNode->data))->parent;
    GT_assert(IemTrace, (NULL != em));

    newNode->prev = newNode->next = NULL;
    curr = em->headNode[irqNum];
    prev = NULL;

    /* Search for place to insert new node as per priority */
    while ((NULL != curr) &&
           (((Iem_ClientInfo *) curr->data)->priority < priority))
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
        em->headNode[irqNum] = newNode;
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
Void vemDeleteNode(VpsUtils_Node * emNode, UInt32 irqNum)
{
    UInt32 cookie;

    VpsUtils_Node *curr = NULL, *prev = NULL;

    Iem_EventManager *em = NULL;

    GT_assert(IemTrace, (NULL != emNode));

    em = ((Iem_ClientInfo *) (emNode->data))->parent;
    prev = emNode->prev;
    curr = emNode->next;

    /* Disable Interrupts */
    cookie = Hwi_disable();

    if (NULL != prev)
    {
        if (NULL != curr)
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
        em->headNode[irqNum] = curr;
        if (NULL != em->headNode[irqNum])
        {
            em->headNode[irqNum]->prev = NULL;
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
Iem_ClientInfo *vemCreateClientInfo(VpsUtils_Node ** node)
{
    Iem_ClientInfo *clientInfo = NULL;

    UInt32 cnt;

    for (cnt = 0u; cnt < IEM_MAX_CLIENTS; cnt++)
    {
        if (IEM_MEM_FLAG_FREE == gIem_clientInfoFlag[cnt])
        {
            clientInfo = &(gIem_clientInfoMemPool[cnt]);
            *node = &IemNode[cnt];
            gIem_clientInfoFlag[cnt] = IEM_MEM_FLAG_ALLOC;
            break;
        }
    }

    return (clientInfo);
}

/**
 *  \brief Function to free up memory for the client information structure
 *  and other arrays.
 */
static Void vemDeleteClientInfo(Iem_ClientInfo * clientInfo)
{
    UInt32 cnt;

    GT_assert(IemTrace, (NULL != clientInfo));

    for (cnt = 0u; cnt < IEM_MAX_CLIENTS; cnt++)
    {
        if (clientInfo == &(gIem_clientInfoMemPool[cnt]))
        {
            gIem_clientInfoFlag[cnt] = IEM_MEM_FLAG_FREE;
            break;
        }
    }
}


#ifdef __DEBUG__
Void vemPrintList(Iem_EventManager * em)
{
    VpsUtils_Node *tempNode = em->headNode;

    Iem_ClientInfo *clientInfo = NULL;

    UInt32 nodeNum = 0u, cnt;

    while (NULL != tempNode)
    {
        clientInfo = (Iem_ClientInfo *) tempNode->data;

        GT_2trace(IemTrace,
                  GT_INFO, "%s:Node Number %d", __FUNCTION__, nodeNum++);
        GT_2trace(IemTrace,
                  GT_INFO,
                  "%s:  Node Number %d", __FUNCTION__, clientInfo->priority);
        GT_2trace(IemTrace,
                  GT_INFO,
                  "%s:  Number of Registered Events %d",
                  __FUNCTION__, clientInfo->numEvents);
        GT_2trace(IemTrace,
                  GT_INFO,
                  "%s:  Event Group                 %d",
                  __FUNCTION__, clientInfo->eventGroup);
        for (cnt = 0u; cnt < clientInfo->numEvents; cnt++)
        {
            GT_2trace(IemTrace,
                      GT_INFO,
                      "%s:    Event Number   %d",
                      __FUNCTION__, clientInfo->allEvents[cnt]);
            GT_2trace(IemTrace,
                      GT_INFO,
                      "%s:    Raw Set Reg    %d",
                      __FUNCTION__, clientInfo->statusReg[cnt]);
            GT_2trace(IemTrace,
                      GT_INFO,
                      "%s:    Raw Clear Reg  %d",
                      __FUNCTION__, clientInfo->vpdmaStatClrReg[cnt]);
            GT_2trace(IemTrace,
                      GT_INFO,
                      "%s:    mask           %x",
                      __FUNCTION__, clientInfo->mask[cnt]);
            GT_2trace(IemTrace,
                      GT_INFO, "%s:    -----------------", __FUNCTION__);
        }
        GT_2trace(IemTrace, GT_INFO, "%s:===============\n", __FUNCTION__);
        tempNode = tempNode->next;
    }
}
#endif
