/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_dlm.h
 *
 *  \brief VPS Display List Manager header file
 *  This file exposes the APIs of the VPS Display List Manager to the upper
 *  client drivers.
 *
 */

#ifndef _VPS_DLM_H
#define _VPS_DLM_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Number of descriptor sets that will be allocated per client. */
#define DLM_NUM_DESC_SET                (DC_NUM_DESC_SET)

/**
 * \brief Number of descriptor nodes per client per set.
 * 1 Shadow descriptor node.
 * 1 Inbound data descriptor node.
 * 1 Outbound data descriptor node.
 * 1 Non-shadow descriptor node.
 * 1 Mosaic data descriptor node for non-first row mosaic descriptors.
 */
#define DLM_NODES_PER_CLIENTS           (DC_DESC_PER_CLIENT_PER_SET)

/*
 *  Index to the client descriptor node array for various nodes in a client.
 *  Caution: If this order is changed, then initialization of
 *  DlmDescNodePriorityTbl table should also be changed.
 */
/** \brief Shadow descriptor node index. */
#define DLM_SHADOW_IDX                  (DC_SHADOW_IDX)
/** \brief Inbound data descriptor node index. */
#define DLM_IN_DATA_IDX                 (DC_IN_DATA_IDX)
/** \brief Outbound data descriptor node index. */
#define DLM_OUT_DATA_IDX                (DC_OUT_DATA_IDX)
/** \brief Non-shadow descriptor node index. */
#define DLM_NSHADOW_IDX                 (DC_NSHADOW_IDX)
/** \brief Mosaic data descriptor node index. */
#define DLM_MOSAIC_DATA_IDX             (DC_MOSAIC_DATA_IDX)

/**
 *  \brief Maximum number of clients to start/stop at one go.
 *
 *  1 Actual client (like bypass or DEI clients)
 *  2 DCTRL RT client (like VCOMP and/or CIG RT)
 *  1 DCTRL one-shot client (like MUX/Blender settings)
 *  1 Extra, in case!!
 */
#define DLM_MAX_START_STOP_CLIENTS      (5u)

/** \brief Typedef for DLM handle. */
typedef Void *VpsDlm_Handle;

/**
 *  \brief VPS display list manager callback function prototype. Each client
 *  driver should register a callback to the list manager.
 *
 *  curSet: The set for which the current interrupt has occurred.
 *
 *  timeStamp: Timestamp at which the SI occurs. Used here so that all the
 *  clients can use the same time stamp.
 *
 *  arg: Private argument which is passed by the client at the time of client
 *  registration for any identification by the client. This can be used by
 *  clients which registers multiple times with the same callback function.
 */
typedef Int32 (*VpsDlm_ClientCbFxn) (UInt32 curSet, UInt32 timeStamp, Ptr arg);

/**
 *  \brief Typedef for display control's set descriptor info structure contains
 *  parameters required to set descriptors information.
 */
typedef Dc_DescInfo VpsDlm_DescInfo;

/**
 *  \brief Typedef for display control's set descriptor info structure contains
 *  parameters required to update the descriptors information at runtime in DLM
 *  ISR context.
 */
typedef Dc_UpdateDescInfo VpsDlm_UpdateDescInfo;

/**
 *  \brief DLM client types used to register with the DLM. This will
 *  determine how the client descriptors are linked on the list layout if
 *  any special requirement is needed.
 */
typedef enum
{
    DLM_CT_PRI = 0,
    /**< Primary display client. */
    DLM_CT_AUX,
    /**< Auxiliary display client. */
    DLM_CT_BP0,
    /**< Bypass path 0 display client. */
    DLM_CT_BP1,
    /**< Bypass path 1 display client. */
    DLM_CT_SD,
    /**< SD display client through Secondary 1 path. */
    DLM_CT_GRPX0,
    /**< Graphics 0 display client. */
    DLM_CT_GRPX1,
    /**< Graphics 1 display client. */
    DLM_CT_GRPX2,
    /**< Graphics 2 display client. */
    DLM_CT_WB0,
    /**< Primary writeback client. */
    DLM_CT_WB1,
    /**< Auxilary writeback client. */
    DLM_CT_SEC0_WB,
    /**< Secondary 0 writeback client. */
    DLM_CT_SEC1_WB,
    /**< Secondary 1 writeback client. */
    DLM_CT_SC_WB2,
    /**< Scalar writeback client. */
    DLM_CT_DCTRL_MUX,
    /**< Display controller MUX client. */
    DLM_CT_DCTRL_RT,
    /**< Display controller runtime client. */
    DLM_CT_GENERIC
    /**< Generic client. This can be used by any client if default list layout
         priority is sufficient. */
} VpsDlm_ClientType;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure defining the request object used in start/stop operation.
 */
typedef struct
{
    UInt32                      numStart;
    /**< Number of clients to start. This represents the valid client handles
         in startClients array. */
    UInt32                      numStop;
    /**< Number of clients to stop. This represents the valid client handles
         in stopClients array. */
    VpsDlm_Handle               startClients[DLM_MAX_START_STOP_CLIENTS];
    /**< List of client handles to start. */
    VpsDlm_Handle               stopClients[DLM_MAX_START_STOP_CLIENTS];
    /**< List of client handles to stop. */
    UInt8                       listNum;
    /**< List number to which the client is to be attached with.
         This should be a valid list number supported by VPDMA starting from
         zero. Also this should match the list already allocated for all the
         stop clients. */
    UInt32                      lmfid;
    /**< LM FID to be used in LM FID control descriptor.
         This is allocated by display controller. */
    VpsHal_VpdmaChannel         socChNum;
    /**< VPDMA channel to be used in SOC control descriptor.
         This is allocated by display controller. */
} VpsDlm_Request;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsDlm_init
 *  \brief VPS Display List Manager init function.
 *
 *  Initializes DLM objects, creates and initiates semaphore used by DLM.
 *  This function should be called before calling any of DLM's API's.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_init(Ptr arg);

/**
 *  VpsDlm_deInit
 *  \brief VPS Display List Manager exit function.
 *
 *  Deletes semaphore objects used by DLM.
 *  This functions should be called to free-up the memory allocated for
 *  semaphores.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_deInit(Ptr arg);

/**
 *  VpsDlm_register
 *  \brief Registers the clients with DLM.
 *
 *  This function registers the client callback with DLMand allocates client
 *  objects.
 *
 *  The client can be either present in the same core as DLM resides or it can
 *  reside in some other core. This is differentiated by the callback parameter
 *  value.
 *  For local client, callback parameter should be non-NULL. For remote client
 *  callback parameter should be NULL.
 *  VpsDlm_init should be called prior to this.
 *  The state for the client changes to registered and appropriate
 *  client objects are allocated.
 *
 *  \param clientType DLM client type used to determine the priority in adding
 *                    the client descriptors to the list.
 *  \param isFbMode   Flag to indicate whether the client is working in
 *                    frame buffer mode or in streaming mode. This will
 *                    determine where the inbound data descriptor will be
 *                    placed in list layout.
 *                    TRUE - Frame buffer mode. In data descriptor placed after
 *                    SOC descriptors.
 *                    FALSE - Streaming mode. In data descriptor placed before
 *                    SOC descriptors.
 *  \param cbFxn      Pointer to the client callback function. A value of NULL
 *                    indicates that the client is a remote client (client
 *                    present in some other core compared to where this DLM
 *                    resides). Non-NULL value indicates that the client is a
 *                    local client (client present in the same core in which
 *                    this DLM is residing).
 *  \param arg        Private argument which will be passed in the callback
 *                    function for any identification by the client. This can be
 *                    used by clients which registers multiple times with the
 *                    same callback function. This parameter can also be NULL as
 *                    DLM does not validate the value.
 *
 *  \return           Returns the DLM client handle to be used for further API
 *                    calls. A value of NULL implies that the call has failed.
 */
VpsDlm_Handle VpsDlm_register(VpsDlm_ClientType clientType,
                              UInt32 isFbMode,
                              VpsDlm_ClientCbFxn cbFxn,
                              Ptr arg);

/**
 *  VpsDlm_unRegister
 *  \brief Unregisters the DLM client and deallocates the client object.
 *
 *  If this function is called before stopping or clearing the descriptor
 *  memory, then this function stops and clears the descriptor memory
 *  automatically.
 *
 *  VpsDlm_register should be called prior to this call.
 *  The state for the client changes to unregistered.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_unRegister(VpsDlm_Handle handle);

/**
 *  VpsDlm_setDescMem
 *  \brief Sets the various descriptor memories for the client.
 *
 *  The memory for the descriptors should be allocated by the client and only
 *  pointers are used by the DLM to link to the next client.
 *  Note: At the end of each descriptor memory, the client should allocate
 *  memory for one reload descriptor which will be used by the DLM to link to
 *  the next client in the list layout.
 *  Caution: This reload memory should not be touched by the client drivers -
 *  this is exclusively for DLM internal purpose only.
 *
 *  VpsDlm_register should be called prior to this call.
 *  The state for the client changes to descriptor memory set.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *  \param descInfo     Pointer to VpsDlm_DescInfo structure containing
 *                      the various descriptor memory parameters like
 *                      descriptor start address, descriptor size, reload
 *                      address.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_setDescMem(VpsDlm_Handle handle, const VpsDlm_DescInfo *descInfo);

/**
 *  VpsDlm_clrDescMem
 *  \brief Clears the descriptor memories for the client already set by
 *  calling VpsDlm_setDescMem.
 *
 *  Client should have been registered and descriptor memories set prior to
 *  this call.
 *  The state for the client changes to memory clear. If the client is
 *  already running, it stops the client before clearing the descriptors.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_clrDescMem(VpsDlm_Handle handle);

/**
 *  VpsDlm_updateDescMem
 *  \brief Updates the descriptor memory of the client with the new descriptor
 *  memory.
 *
 *  Caution: This function should be called from DLM callback context only.
 *  Also client drivers should update the ping set in pong interrupt and pong
 *  set in ping interrupt for proper display operation.
 *
 *  Client should have been registered and descriptor memories set prior to
 *  this call.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *  \param descSet      Descriptor set where the new descriptor will be updated.
 *  \param descInfo     Pointer to VpsDlm_UpdateDescInfo structure containing
 *                      the various descriptor memory parameters like
 *                      descriptor start address, descriptor size, reload
 *                      address for the requested set.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_updateDescMem(VpsDlm_Handle handle,
                           UInt32 descSet,
                           const VpsDlm_UpdateDescInfo *descInfo);

/**
 *  VpsDlm_stopClient
 *  \brief Removes the client descriptors from the corresponding list
 *  descriptor circular link and de-allocates list objects.
 *
 *  If this is the last client, then this function stops the list by breaking
 *  the circular list.
 *  For non-last client, removal of the clients descriptor from the running
 *  list is done in the Send Interrupt ISR context.
 *  Client should have started prior to this call.
 *
 *  The state for the client changes to stopped.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_stopClient(VpsDlm_Handle handle);

/**
 *  VpsDlm_startStopClients
 *  \brief Adds and removes multiple client descriptors to the corresponding
 *  list descriptor circular link at one go.
 *
 *  Client Addtion:
 *  ---------------
 *  Adds the client descriptors to the corresponding list descriptor
 *  circular link.
 *
 *  It allocates appropriate list objects. If this is the first client of a
 *  list, then it allocates a free list object else it will search for an
 *  already existing list with the same list number.
 *
 *  If this is the first client, then this function submits the descriptor to
 *  VPDMA. For non-first client, addition of the clients descriptor to the top
 *  of the running list is done in the Send Interrupt ISR context.
 *  After the start, the local clients can expect callback for every Send
 *  Interrupt. Remote clients can expect callback from the Send Interrupt
 *  received by the corresponding slave DLM on that core.
 *
 *  When a client (remote/local) gets a callback for set 0, it shall do the
 *  following:
 *  1. Mark the buffer programmed in set 1 as completed.
 *  2. Program the next buffer in descriptor set 1.
 *
 *  Client should have registered and set descriptors prior to
 *  this call. It is assumed that the client drivers have actually filled the
 *  data descriptors and configuration descriptors with valid data prior to
 *  calling this function.
 *
 *  The state for the client changes to started.
 *
 *  Client Removal:
 *  --------------
 *  If this is the last client, then this function stops the list by breaking
 *  the circular list.
 *  For non-last client, removal of the clients descriptor from the running
 *  list is done in the Send Interrupt ISR context.
 *  Client should have started prior to this call.
 *
 *  The state for the client changes to stopped.
 *
 *  \param reqObj       Pointer to the DLM request containing handles of clients
 *                      to be started/stopped.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_startStopClients(const VpsDlm_Request *reqObj);

/**
 *  VpsDlm_getNumActiveClients
 *  \brief Returns the number of clients running on a given list.
 *
 *  \param listNum      List number to search for.
 *
 *  \return             Returns the number of active clients.
 */
UInt32 VpsDlm_getNumActiveClients(UInt8 listNum);

/**
 *  VpsDlm_unLinkDctrlClient
 *  \brief Unlinks the display controller client from the list.
 *
 *  This is used to stop the non-showdow DCTRL client used to configure the
 *  mux and blender enable/disable.
 *  Caution: This function should be called from DLM callback context only.
 *  Also from the pong intterupt only.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_unLinkDctrlClient(VpsDlm_Handle handle);

/**
 *  VpsDlm_freeListResource
 *  \brief Frees the list objects and related handles for the list.
 *
 *  This is called by the DCTRL to free-up the list objects, SI, unregister
 *  from the event manager when the last client is removed from the list.
 *  Note: Stopping the last client will not free-up the list object. Hence
 *  the DCTRL should call this function once it gets the list complete
 *  interrupt.
 *  This should be called from task context only.
 *
 *  \param listNum      List number to free up the resource.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsDlm_freeListResource(UInt8 listNum);

/**
 *  VpsDlm_SetDispMode
 *  \brief Sets the display mode.
 *
 *  This is used to set the display mode to either
 *  Frame Buffer or streaming mode.
 *
 *  \param handle       Valid client handle returned by VpsDlm_register.
 *  \param isFbMode     Flag to indicate whether it is FbMode or not
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Void VpsDlm_setDispMode(VpsDlm_Handle handle, UInt32 isFbMode);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_DLM_H */
