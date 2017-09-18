/*
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <xdc/std.h>
#include <ti/xdais/ires.h>

#include <ti/sdo/fc/rman/rman.h>

#include "edmamgr.h"
#include "edmamgr_heap.h"

static IRES_ProtocolRevision _iresEDMA3ChanRevision =
        IRES_EDMA3CHAN_PROTOCOLREVISION_2_0_0;

#define EDMA_MGR_MAX_NUM_CHANNELS     32

EdmaMgr_Channel EdmaMgr_channels[EDMA_MGR_MAX_NUM_CHANNELS];

extern __FAR__ int32_t *ti_sdo_fc_edmamgr_region2Instance;
extern __FAR__ EDMA3_GblConfigParams *ti_sdo_fc_edmamgr_edma3GblConfigParams;
extern __FAR__ EDMA3_InstanceInitConfig *ti_sdo_fc_edmamgr_edma3RegionConfig;

/*********************************************************************************
 * FUNCTION PURPOSE: Init EdmaMgr resources
 *********************************************************************************
  DESCRIPTION:      This function initializes RMAN resouces

  Parameters :      Inputs: proc_id          : core ID
                            edma_config      : pointer to EDMA3 instance init
                                               configuration
                    Output: EdmaMgr_SUCCESS if initialization successful;
                            Error code otherwise
 *********************************************************************************/
int32_t EdmaMgr_init(int32_t proc_id, void *edma_config)
{
    IRES_Status ires_status;
    uint32_t edmaInstanceId;
    int32_t i;

    if (proc_id < 0 || proc_id >= EDMA3_MAX_REGIONS) {
        return EdmaMgr_ERROR_INVARG;
    }

    if (ti_sdo_fc_edmamgr_region2Instance != NULL) {
        edmaInstanceId = ti_sdo_fc_edmamgr_region2Instance[proc_id];

        if (ti_sdo_fc_edmamgr_edma3GblConfigParams != NULL) {
            EDMA3_PARAMS.globalConfig =
                    (EDMA3_GblConfigParams *) &
                    ti_sdo_fc_edmamgr_edma3GblConfigParams[edmaInstanceId];
        }
        else if (EDMA3_PARAMS.globalConfig == NULL) {
            return EdmaMgr_ERROR_INVCFG;
        }

        if (edma_config != NULL) {
            EDMA3_InstanceInitConfig *custom_config =
                    (EDMA3_InstanceInitConfig *) edma_config;

            EDMA3_PARAMS.regionConfig =
                    (EDMA3_InstanceInitConfig *) &
                    custom_config[EDMA3_MAX_REGIONS * edmaInstanceId + proc_id];
        }
        else if (ti_sdo_fc_edmamgr_edma3RegionConfig != NULL) {
            EDMA3_PARAMS.regionConfig =
                    (EDMA3_InstanceInitConfig *) &
                    ti_sdo_fc_edmamgr_edma3RegionConfig[EDMA3_MAX_REGIONS *
                    edmaInstanceId + proc_id];
        }
        else if (EDMA3_PARAMS.regionConfig == NULL) {
            return EdmaMgr_ERROR_INVCFG;
        }
    }
    else if (EDMA3_PARAMS.globalConfig || EDMA3_PARAMS.regionConfig == NULL) {
        return EdmaMgr_ERROR_INVCFG;
    }

    EdmaMgr_heap_create();

    RMAN_PARAMS.allocFxn = &EdmaMgr_heap_alloc;
    RMAN_PARAMS.freeFxn = &EdmaMgr_heap_free;
    EDMA3_PARAMS.allocFxn = &EdmaMgr_heap_alloc;
    EDMA3_PARAMS.freeFxn = &EdmaMgr_heap_free;
    ECPY_CFG_PARAMS.allocFxn = &EdmaMgr_heap_alloc;
    ECPY_CFG_PARAMS.freeFxn = &EdmaMgr_heap_free;

    /* specify EDMA instance ID */
    ti_sdo_fc_edma3_EDMA3_physicalId = edmaInstanceId;

    /* RMAN init */
    ires_status = RMAN_init();

    if (IRES_OK != ires_status) {
        return EdmaMgr_ERROR_RMANINIT;
    }

    memset(EdmaMgr_channels, 0,
            EDMA_MGR_MAX_NUM_CHANNELS * sizeof(EdmaMgr_Channel));
    for (i = 0; i < EDMA_MGR_MAX_NUM_CHANNELS; i++) {
        EdmaMgr_channels[i].edmaArgs.size = sizeof(IRES_EDMA3CHAN_ProtocolArgs);
        EdmaMgr_channels[i].edmaArgs.mode = IRES_PERSISTENT;
        EdmaMgr_channels[i].edmaArgs.numTccs = 1;
        EdmaMgr_channels[i].edmaArgs.paRamIndex = IRES_EDMA3CHAN_PARAM_ANY;
        EdmaMgr_channels[i].edmaArgs.tccIndex = IRES_EDMA3CHAN_TCC_ANY;
        EdmaMgr_channels[i].edmaArgs.qdmaChan = IRES_EDMA3CHAN_CHAN_NONE;
        EdmaMgr_channels[i].edmaArgs.edmaChan = IRES_EDMA3CHAN_EDMACHAN_ANY;
        EdmaMgr_channels[i].edmaArgs.contiguousAllocation = TRUE;
        EdmaMgr_channels[i].edmaArgs.shadowPaRamsAllocation = FALSE;
        EdmaMgr_channels[i].edmaArgs.numPaRams = 0;
    }
    return EdmaMgr_SUCCESS;
}

/*********************************************************************************
 * FUNCTION PURPOSE: Allocate EdmaMgr handle
 *********************************************************************************
  DESCRIPTION:      This function allocates an EDMA channel

  Parameters :      Inputs: max_linked_transfer  : maximum number of linked
                                                   transfers required for the
                                                   channel.

                    Output: Valid EdmaMgr handle on success;
                            NULL handle if there is an error.
 *********************************************************************************/
EdmaMgr_Handle EdmaMgr_alloc(int32_t max_linked_transfers)
{
    IRES_ResourceDescriptor resDesc;
    IRES_Status ires_status;
    int32_t scratchId = -1;
    int32_t i;

    /* Find free channel */
    for (i = 0; i < EDMA_MGR_MAX_NUM_CHANNELS; i++)
        if (EdmaMgr_channels[i].edmaArgs.numPaRams == 0)
            break;

    if (i >= EDMA_MGR_MAX_NUM_CHANNELS)
        return ((EdmaMgr_Handle)NULL);

    EdmaMgr_channels[i].edmaArgs.numPaRams = max_linked_transfers;

    /* Prepare IRES resource descriptor */
    resDesc.resourceName = IRES_EDMA3CHAN_PROTOCOLNAME;
    resDesc.revision = &_iresEDMA3ChanRevision;
    resDesc.protocolArgs = (IRES_ProtocolArgs *)&(EdmaMgr_channels[i].edmaArgs);
    resDesc.handle = (IRES_Handle)EdmaMgr_channels[i].edmaHandle;

    /* Allocate EDMA Resources */
    ires_status =
            RMAN_allocateResources((Int)EdmaMgr_channels, &resDesc, 1,
            scratchId);
    if (ires_status != IRES_OK) {
        EdmaMgr_channels[i].edmaArgs.numPaRams = 0;
        return ((EdmaMgr_Handle)NULL);
    }
    EdmaMgr_channels[i].edmaHandle = (IRES_EDMA3CHAN_Handle) resDesc.handle;

    /* Create ECPY handle */
    EdmaMgr_channels[i].ecpyHandle =
            ECPY_createHandle((IRES_EDMA3CHAN2_Handle)resDesc.handle,
            (IALG_Handle)&EdmaMgr_channels[i]);
    if (EdmaMgr_channels[i].ecpyHandle == NULL) {
        RMAN_freeAllocatedResources((Int)EdmaMgr_channels, &resDesc, 1,
                scratchId);
        EdmaMgr_channels[i].edmaHandle = NULL;
        EdmaMgr_channels[i].edmaArgs.numPaRams = 0;
        return ((EdmaMgr_Handle)NULL);
    }
    EdmaMgr_channels[i].xferPending = FALSE;

    /* HW assign */
    ECPY_activate(EdmaMgr_channels[i].ecpyHandle);
    ECPY_setEarlyCompletionMode(EdmaMgr_channels[i].ecpyHandle, FALSE);

    return ((EdmaMgr_Handle)&EdmaMgr_channels[i]);
}

/*********************************************************************************
 * FUNCTION PURPOSE: Free EdmaMgr handle
 *********************************************************************************
  DESCRIPTION:      This function frees an EDMA channel

  Parameters :      Inputs: h   : EdmaMgr handle

                    Output: EdmaMgr_SUCCESS if free is successful;
                            Error code otherwise.
 *********************************************************************************/
int32_t EdmaMgr_free(EdmaMgr_Handle h)
{
    EdmaMgr_Channel *chan = (EdmaMgr_Channel *)h;
    IRES_ResourceDescriptor resDesc;
    IRES_Status ires_status;
    int32_t scratchId = -1;
    int32_t i, ret_val = EdmaMgr_SUCCESS;

    /* Verify that this is a valid handle? */
    for (i = 0; i < EDMA_MGR_MAX_NUM_CHANNELS; i++)
        if (chan == &EdmaMgr_channels[i])
            break;

    if (i >= EDMA_MGR_MAX_NUM_CHANNELS)
        return EdmaMgr_ERROR_INVHANDLE;

    /* Make sure all transfers have completed */
    EdmaMgr_wait(h);

    /* Prepare IRES resource descriptor */
    resDesc.resourceName = IRES_EDMA3CHAN_PROTOCOLNAME;
    resDesc.revision = &_iresEDMA3ChanRevision;
    resDesc.protocolArgs = (IRES_ProtocolArgs *)&(chan->edmaArgs);
    resDesc.handle = (IRES_Handle)chan->edmaHandle;

    ECPY_deleteHandle(chan->ecpyHandle);

    ires_status =
            RMAN_freeAllocatedResources((Int)EdmaMgr_channels, &resDesc, 1,
            scratchId);
    if (ires_status != IRES_OK) {
        ret_val = EdmaMgr_ERROR_FREE;
    }

    chan->ecpyHandle = NULL;
    chan->edmaHandle = NULL;
    chan->xferPending = FALSE;

    chan->edmaArgs.numPaRams = 0;

    return ret_val;
}
