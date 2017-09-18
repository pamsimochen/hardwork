/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_BUF_API Frame buffer exchange API

    APIs defined in this file are used by links-and-chains example to exchange
    frames between two tasks

    Internally this consists of two queues
    - empty or input queue
    - full or output queue

    The queue implementation uses fixed size array based queue data structure,
    with mutual exclusion built inside the queue implementation.

    Optional blocking of Get and/or Put operation is possible

    The element that can be inserted/extracted from the queue is of
    type FVID2_Frame *

    The basic operation in the example is as below

    - When a producer task needs to output some data, it first 'gets' a empty frame
      to output the data from the buffer handle.
    - The task outputs the data to the empty frame
    - The task then 'puts' this data as full data into the buffer handle
    - The consumer task, then 'gets' this full frame from the buffer handle
    - After using or consuming this frame, it 'puts' this frame as empty frame into
      this buffer handle.
    - This way frames are exchanged between a producer and consumer.

    @{
*/

/**
    \file vpsutils_buf.h
    \brief Frame buffer exchange API
*/

#ifndef _VPSUTILS_BUF_H_
#define _VPSUTILS_BUF_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vpsutils_que.h>

/** \brief Maximum size of buffer in the queue. */
#define VPSUTILS_BUF_MAX_QUE_SIZE       (FVID2_MAX_FVID_FRAME_PTR*4)

/**
    \brief Frame Buffer Handle
*/
typedef struct
{
    VpsUtils_QueHandle emptyQue;
    /**< Empty or input queue */

    VpsUtils_QueHandle fullQue;
    /**< Full or output queue */


    FVID2_Frame *emptyQueMem[VPSUTILS_BUF_MAX_QUE_SIZE];
    /**< Memory for empty que data */

    FVID2_Frame *fullQueMem[VPSUTILS_BUF_MAX_QUE_SIZE];
    /**< Memory for empty que data */

} VpsUtils_BufHndl;

/**
    \brief Create a frame buffer handle

    When blockOnGet/blockOnPut is TRUE a semaphore gets allocated internally.
    In order to reduce resource usuage keep this as FALSE if application
    doesnt plan to use the blocking API feature.

    \param pHndl        [OUT] Created handle
    \param blockOnGet   [IN]  Enable blocking on 'get' API
    \param blockOnPut   [IN]  Enable blocking on 'put' API

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufCreate(
                VpsUtils_BufHndl *pHndl,
                Bool blockOnGet,
                Bool blockOnPut);

/**
    \brief Delete frame buffer handle

    Free's resources like semaphore allocated during create

    \param pHndl    [IN] Buffer handle

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufDelete(VpsUtils_BufHndl *pHndl);

/**
    \brief Get frame's from empty queue

    This API is used to get multiple frames in a single API call.
    FVID2_FrameList.numFrames is set to number of frames
    that are returned.

    When during create
    - 'blockOnGet' = TRUE
      - timeout can be BIOS_WAIT_FOREVER or BIOS_NO_WAIT
    - 'blockOnGet' = FALSE
      - timeout must be BIOS_NO_WAIT

    \param pHndl        [IN] Buffer handle
    \param pFrameList   [OUT] Frame's returned by the API
    \param timeout      [IN] BIOS_NO_WAIT or BIOS_WAIT_FOREVER

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufGetEmpty(
                VpsUtils_BufHndl *pHndl,
                FVID2_FrameList *pFrameList,
                UInt32 timeout);


/**
    \brief Put frame's into full queue

    This API is used to return multiple frames in a single API call.
    FVID2_FrameList.numFrames is set to number of frames
    that are to be returned.

    When during create
    - 'blockOnPut' = TRUE
      - API will block until space is available in the queue to put the frames
    - 'blockOnPut' = FALSE
      - API will return error in case space is not available in the queue
        to put the frames

    \param pHndl        [IN] Buffer handle
    \param pFrameList   [IN] Frame's to be put

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufPutFull(
                VpsUtils_BufHndl *pHndl,
                FVID2_FrameList *pFrameList);

/**
    \brief Get frame's from full queue

    This API is used to get multiple frame's in a single API call.
    FVID2_FrameList.numFrames is set to number of frames
    that are returned.

    When during create
    - 'blockOnGet' = TRUE
      - timeout can be BIOS_WAIT_FOREVER or BIOS_NO_WAIT
    - 'blockOnGet' = FALSE
      - timeout must be BIOS_NO_WAIT

    \param pHndl        [IN] Buffer handle
    \param pFrameList   [OUT] Frame's returned by the API
    \param timeout      [IN] BIOS_NO_WAIT or BIOS_WAIT_FOREVER

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufGetFull(
                VpsUtils_BufHndl *pHndl,
                FVID2_FrameList *pFrameList,
                UInt32 timeout);

/**
    \brief Put frame's into empty queue

    This API is used to return multiple frames in a single API call.
    FVID2_FrameList.numFrames is set to number of frames
    that are to be returned.

    When during create
    - 'blockOnPut' = TRUE
      - API will block until space is available in the queue to put the frames
    - 'blockOnPut' = FALSE
      - API will return error in case space is not available in the queue
        to put the frames

    \param pHndl        [IN] Buffer handle
    \param pFrameList   [IN] Frame's to be put

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufPutEmpty(
                VpsUtils_BufHndl *pHndl,
                FVID2_FrameList *pFrameList);

/**
    \brief Get a frame from empty queue

    Same as VpsUtils_bufGetEmpty() except that only a single frame is returned

    \param pHndl        [IN] Buffer handle
    \param pFrame       [OUT] Frame that is returned by the API
    \param timeout      [IN] BIOS_NO_WAIT or BIOS_WAIT_FOREVER

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufGetEmptyFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame **pFrame,
                UInt32 timeout);

/**
    \brief Get a frame from full queue

    Same as VpsUtils_bufGetFull() except that only a single frame is returned

    \param pHndl        [IN] Buffer handle
    \param pFrame       [OUT] Frame that is returned by the API
    \param timeout      [IN] BIOS_NO_WAIT or BIOS_WAIT_FOREVER

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufGetFullFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame **pFrame,
                UInt32 timeout);

/**
    \brief Put a frame into full queue

    Same as VpsUtils_bufPutFull() except that only a single frame is put

    \param pHndl        [IN] Buffer handle
    \param pFrame       [OUT] Frame that is to be returned to the queue

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufPutFullFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame *pFrame);

/**
    \brief Put a frame into empty queue

    Same as VpsUtils_bufPutEmpty() except that only a single frame is put

    \param pHndl        [IN] Buffer handle
    \param pFrame       [OUT] Frame that is to be returned to the queue

    \return FVID2_SOK on success, else failure
*/
Int32 VpsUtils_bufPutEmptyFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame *pFrame);


/**
    \brief Peek into empty queue

    This only peeks at the top of the queue but does not remove the
    frame from the queue

    \param pHndl        [IN] Buffer handle

    \return frame pointer is frame is present in the queue, else NULL
*/
static inline FVID2_Frame *VpsUtils_bufPeekEmpty(VpsUtils_BufHndl *pHndl)
{
    FVID2_Frame *pFrame;

    VpsUtils_quePeek(&pHndl->emptyQue, (Ptr*)&pFrame);

    return pFrame;
}

/**
    \brief Peek into full queue

    This only peeks at the top of the queue but does not remove the
    frame from the queue

    \param pHndl        [IN] Buffer handle

    \return frame pointer is frame is present in the queue, else NULL
*/
static inline FVID2_Frame *VpsUtils_bufPeekFull(VpsUtils_BufHndl *pHndl)
{
    FVID2_Frame *pFrame;

    VpsUtils_quePeek(&pHndl->fullQue, (Ptr*)&pFrame);

    return pFrame;
}

#endif

/*@}*/

/**
    \defgroup VPSEXAMPLE_API Sample Example API

    The API defined in this module are utility APIs OUTSIDE of the FVID2 drivers.

    Example code makes use of these APIs to implement sample application which
    demonstrate the driver in different ways.
*/

/**
    \ingroup VPSEXAMPLE_API
    \defgroup VPSUTILS_API Sample Example - Utility library API
*/
