/*****************************************************************************
 * Copyright (c) 2008 Texas Instruments Incorporated.
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 *
 *
 * DMAC library helper
 *
 *  This file must not be modified by application user
 *  This file may be updated or dropped if DMAC library interface changes
 *  in future.
 ****************************************************************************/


#ifndef __DMACHLPR_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __DMACHLPR_H__


#include "dmac.h"
#include "vicp_edma3.h"

#define DMAC_init() \
        VICP_EDMA3_init()

#define DMAC_exit() \
        VICP_EDMA3_deinit()

#define DMAC_allocChannel(ch_req) \
        VICP_EDMA3_allocChannel(ch_req)

#define DMAC_freeChannel(ch) \
        VICP_EDMA3_freeChannel(ch)

#define DMAC_clearChanInt(ch) \
        VICP_EDMA3_clearChanInt(ch)

#define DMAC_allocParamEntry(pa_req) \
        VICP_EDMA3_allocParamEntry(pa_req)

#define DMAC_initParamEntry(entry, srcAd, dstAd, arraySize, arrayCount, frameCount,\
        inputArrayUpdate, outputArrayUpdate, inputFrameUpdate,\
        outputFrameUpdate, staticEntry, syncType, txfrCompleteCode,\
        tccCompleteMode, txfrCompleteChain, txfrCompleteIntr,\
        intTxfrCompleteChain, intTxfrCompleteIntr) \
        VICP_EDMA3_initParamEntry(entry, srcAd, dstAd, arraySize, arrayCount, frameCount,\
                inputArrayUpdate, outputArrayUpdate, inputFrameUpdate,\
                outputFrameUpdate, staticEntry, syncType, txfrCompleteCode,\
                tccCompleteMode, txfrCompleteChain, txfrCompleteIntr,\
                intTxfrCompleteChain, intTxfrCompleteIntr)

#define DMAC_link(param1, param2)\
        VICP_EDMA3_link(param1, param2)

#define DMAC_copyParamEntry(dstEntry, srcEntry) \
        VICP_EDMA3_copyParamEntry(dstEntry, srcEntry)

#define DMAC_getParamEntryNum(entry) \
        DMAC_getParamEntryNum(entry)

#define DMAC_freeParamEntry(entry) \
        VICP_EDMA3_freeParamEntry(entry)

#define DMAC_mapChanToQ(chan, queue) \
        VICP_EDMA3_mapChanToQ(chan, queue)

#define DMAC_mapChanToInputQ(chan) \
        VICP_EDMA3_mapChanToInputQ(chan)

#define DMAC_mapChanToOutputQ(chan) \
        VICP_EDMA3_mapChanToOutputQ(chan)


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DMACHLPR_H_ */
