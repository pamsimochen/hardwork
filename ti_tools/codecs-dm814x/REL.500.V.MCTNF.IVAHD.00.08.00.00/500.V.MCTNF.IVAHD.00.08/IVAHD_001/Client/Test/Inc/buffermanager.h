/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

#ifndef BUFFMANAGER_H_
#define BUFFMANAGER_H_

/**
 *****************************************************************************
 * @file buffermanager.h
 *
 * @brief This is header module for buffer manager functionality
 *
 * @author: MMCODECS
 *
 * @version 0.0 (Jan 2008) : Base version created
 *                           [MMCODECS]
 * @version 0.1 (Jan 2013) : Modified to supply input buf and output buf
 *                           [Shyam Jagannathan]
 *****************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <ti/xdais/xdas.h>

#define MAX_BUFF_ELEMENTS 35
#define MAX_BUFS_IN_FRAME 35
#define BUFFMGR_IN_BUF  1
#define BUFFMGR_OUT_BUF 0
/**
 *  @brief      Status of the buffer elements.
 */
typedef enum {
    BUFFMGR_BUFFER_FREE = 0,
    BUFFMGR_BUFFER_USED = 1
} BUFFMGR_BufferStatus;

/**
 *  @brief      Each element in the buffer array.
 */
typedef struct BuffEle
{
    XDAS_UInt32 bufId;
    BUFFMGR_BufferStatus bufStatus;
    XDAS_UInt32 bufSize[MAX_BUFS_IN_FRAME];
    volatile XDAS_UInt8 *buf[MAX_BUFS_IN_FRAME];
} BUFFMGR_buffEle;

typedef BUFFMGR_buffEle* BUFFMGR_buffEleHandle;

extern BUFFMGR_buffEle buffArray[MAX_BUFF_ELEMENTS];

/**
*@func BUFFMGR_Init()
*        The BUFFMGR_Init function is called by the test application to
*        initialise the global buffer element array to default and to allocate
*        required number of memory data for reference and output buffers.
*        The maximum required dpb size is defined by the supported profile &
*        level.
*/
//extern XDAS_Int32 BUFFMGR_Init(XDAS_Int32 numBufs, XDM2_BufSize *bufSizes);
extern XDAS_Int32 BUFFMGR_Init
(
    XDAS_Int32    numInBufs,
    XDM2_BufSize *inBufSizes,
    XDAS_Int32    numOutBufs,
    XDM2_BufSize *outBufSizes,
    XDAS_Int8     lumaTilerSpace,
    XDAS_Int8     chromaTilerSpace
);

/**
*@func BUFFMGR_GetFreeBuffer()
*        The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
*        global buffer array and returns the address of that element. Incase
*        if none of the elements are free then it returns NULL
*/
extern BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer(XDAS_UInt32 In_nOut);

/**
*@func BUFFMGR_ReleaseBuffer(buffId)
*        The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
*        which are released by the test-app. "0" is not a valid buffer Id
*        hence this function keeps moving until it encounters a buffer Id
*        as zero or it hits the MAX_BUFF_ELEMENTS
*/
extern void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[], XDAS_UInt32 In_nOut);

void BUFFMGR_ReleaseAllBuffers();

#endif

