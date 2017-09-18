/** ==================================================================
 *  @file   iss_evtMgr.h
 *
 *  @path   /ti/psp/iss/common/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \file   iss_evtMgr.h
 *
 * \brief  Event Manager Header File.
 *
 * This file contains Data Structures and Functions definitions for
 * Event Manager. It exposes these API and data structures to its clients.
 *
 */

#ifndef _ISS_EVTMGR_H
#define _ISS_EVTMGR_H

/* ==========================================================================
 */
/* Include Files */
/* ==========================================================================
 */

#include <ti/psp/iss/iss.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
==========================================================================
     Macros & Typedefs
==========================================================================
*/

    /* Macro for the default priority. Default priority is the medium one. */
#define IEM_DEFAULT_PRIORITY     (IEM_PRIORITY2)

    /* ISSS M3 IRQ */
#define IEM_IRQNUMBER0            (32)
#define IEM_IRQNUMBER1            (33)
#define IEM_IRQNUMBER2            (34)
#define IEM_IRQNUMBER3            (35)
#define IEM_IRQNUMBER4            (36)
#define IEM_IRQNUMBER5            (37)


typedef enum {
    IEM_IRQ_NUM_0 = 0,
    IEM_IRQ_NUM_1 = 1,
    IEM_IRQ_NUM_2 = 2,
    IEM_IRQ_NUM_3 = 3,
    IEM_IRQ_NUM_4 = 4,
    IEM_IRQ_NUM_5 = 5,
    IEM_IRQ_MAX
} Iem_IrqNum;

typedef enum {
    IEM_IRQ_BANK_NUM_ISP_IRQ0 = 0,
    IEM_IRQ_BANK_NUM_ISP_IRQ1,
    IEM_IRQ_BANK_NUM_ISP_IRQ2,
    IEM_IRQ_BANK_NUM_ISP_IRQ3,
    IEM_IRQ_BANK_NUM_CSI2A_IRQ,
    IEM_IRQ_BANK_NUM_CSI2B_IRQ,
    IEM_IRQ_BANK_NUM_CCP2_IRQ0,
    IEM_IRQ_BANK_NUM_CCP2_IRQ1,
    IEM_IRQ_BANK_NUM_CCP2_IRQ2,
    IEM_IRQ_BANK_NUM_CCP2_IRQ3,
    IEM_IRQ_BANK_NUM_CBUFF_IRQ,
    IEM_IRQ_BANK_NUM_BTE_IRQ,
    IEM_IRQ_BANK_NUM_SIMCOP_IRQ0,
    IEM_IRQ_BANK_NUM_SIMCOP_IRQ1,
    IEM_IRQ_BANK_NUM_SIMCOP_IRQ2,
    IEM_IRQ_BANK_NUM_SIMCOP_IRQ3,
    IEM_IRQ_BANK_NUM_CCP2_IRQ8,
    IEM_IRQ_BANK_NUM_HS_VS_IRQ,
    IEM_IRQ_BANK_MAX,
} Iem_BankNum;


typedef enum {
    IEM_EVENT_ISIF_VDINT0 = 0,
    IEM_EVENT_ISIF_VDINT1 = 1,
    IEM_EVENT_ISIF_VDINT2 = 2,
    IEM_EVENT_ISIF_2DLSC = 3,
    IEM_EVENT_IPIPE_INT = 4,
    IEM_EVENT_IPIPE_INT_LAST_PIX = 5,
    IEM_EVENT_IPIPE_INT_DMA = 6,
    IEM_EVENT_IPIPE_INT_BSC = 7,
    IEM_EVENT_IPIPE_INT_HST = 8,
    IEM_EVENT_IPIPEIF_INT = 9,
    IEM_EVENT_AEW_INT = 10,
    IEM_EVENT_AF_INT = 11,
    IEM_EVENT_H3A_INT = 12,
    IEM_EVENT_RSZ_INT = 13,
    IEM_EVENT_RSZ_INT_LAST_PIX = 14,
    IEM_EVENT_RSZ_INT_DMA = 15,
    IEM_EVENT_RSZ_INT_CYC_RSZA = 16,
    IEM_EVENT_RSZ_INT_CYC_RSZB = 17,
    IEM_EVENT_RSZ_INT_FIFO_OVF = 18,
    IEM_EVENT_RSZ_INT_FIFO_IN_BLK_ERR = 19,
    IEM_EVENT_RSZ_INT_EOF0 = 22,
    IEM_EVENT_RSZ_INT_EOF1 = 23,
    IEM_EVENT_H3A_INT_EOF = 24,
    IEM_EVENT_IPIPE_INT_EOF = 25,
    IEM_EVENT_IPIPE_DPC_IN = 27,
    IEM_EVENT_IPIPE_DPC_RENEW0 = 28,
    IEM_EVENT_IPIPE_DPC_RENEW1 = 29,
    IEM_EVENT_OCP_ERR = 31
} Iem_Event;

/**
 *  enum Iem_Priority
 *  \brief enum for specifying the priority of the client. Event
 *  Manager always calls the callback functions of the clients in the order of
 *  priority so that highest priority callback will get executed much earlier.
 *  When client registers itself to the event manager, it also specifies the
 *  priority of itself. event manager puts the client in the appropriate
 *  location in the linked list as per the given priority. This enum data
 *  type defines the event groups. This enum data type is type casted to
 *  unsigned int so it must not have any negative value
 * */
typedef enum {
    /**< Highest */
    IEM_PRIORITY0 = 0u,
    IEM_PRIORITY1,
    IEM_PRIORITY2,
    IEM_PRIORITY3,
    /**< Lowest */
    IEM_PRIORITY4
} Iem_Priority;

/**
 *  \brief This typedef defines the ISS Event manager callback function
 *  prototype. Client registers this callback with the event manager and when
 *  event occurs,
 *  event manager calls this callback.
 *
 *  \param  event       This is an array of events occurred when interrupt came.
 *                      Event manager passes this list to the client.
 *  \param  numEvents   This tells number of events occurred.
 *  \param  arg         Client's Private Data
 */
typedef Void(*Iem_ClientCbFxn) (
                const UInt32 event,
                Ptr arg);

    /* ==========================================================================
     */
    /* Structure Declarations */
    /* ==========================================================================
     */

    /* None */

    /* ==========================================================================
     */
    /* Function Declarations */
    /* ==========================================================================
     */

/**
 *  Iem_init
 *  \brief This function initializes event manager. It creates the semaphore,
 *  initializes pointers and sets the memory to zero. It also register ISR
 *  for the ISS interrupt. It returns pass or
 *  fail based on initialization passed or failed
 *
 *  \param arg            Not used currently. For the future use.
 *  \return               Returns 0 on success else returns error value
 */
Int Iem_init(Ptr arg);

/**
 *  Iem_deInit
 *  \brief This function de-initializes the event manager.
 *
 *  \param arg            Not used currently. For the future use.
 *  \return               Returns 0 on success else returns error value
 */
Int Iem_deInit(Ptr arg);

/**
 *  Iem_register
 *  \brief Function to register callback to the Event Manager. Client passes
 *  the set of events of a group with the callback and priority and
 *  this function adds a new node at the appropriate position as per
 *  the priority in the list of nodes. It also stores the list of
 *  events, registers for the events and masks in the the client info
 *  structure. It returns handle of the registered callback.
 *
 *  \param eventGroup       Group of the event passed in the second argument
 *  \param event            List of event for which client requested callback
 *  \param numEvents        Number of events in the second argument
 *  \param priority         Priority of this callback
 *  \param callback         Callback function of the client
 *  \param arg              Private Data of the client
 *
 *  \return                 It returns the handle of the registered callback.
 */
Void *Iem_register(
        Iem_IrqNum      irqNum,
        Iem_BankNum     bankId,
        Iem_Event       event,
        Iem_Priority    priority,
        Iem_ClientCbFxn cbFxn,
        Void            *priv);

/**
 *  Iem_unRegister
 *  \brief Function to un-register callback from the event manager. It removes
 *  node from the linked list and frees up memory allocated to the node
 *  and client info structure. It takes handle to the callback and
 *  returns status of the whether callback is removed.
 *
 *  \param handle           Handle to the callback
 *  \return                 Returns 0 on success else returns error value
 */
Int Iem_unRegister(Void * handle);


Void Iem_enableInt(Void * handle);

Void Iem_disableInt(Void * handle);

Void Iem_disableAll();

#ifdef __cplusplus
}
#endif
#endif                                                     /* End of #ifndef
                                                            * _ISS_EVTMGR_H */
