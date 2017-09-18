#ifndef BUFFMANAGER_H_
#define BUFFMANAGER_H_
/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/** @mainpage HDVICP2.0 Based MPEG4 SP Encoder 
* @brief "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's 
*  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw 
*  video into a high/main/baseline profile bit-stream. Based on ISO/IEC      
*  14496-10." 
* Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
* ALL RIGHTS RESERVED 
*/
/**
********************************************************************************
 * @file <buffermanager.h>
 *
 * @brief This File contains all structure definitions and function prototypes
 *        for handling buffer management
 *
 *******************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <ti/xdais/xdas.h>
#ifdef HOSTCORTEXM3_OMAP4
#include "tilerBuf.h"
#endif
/**
 * Number of buffers to store the input frames
 */
#define MAX_BUFF_ELEMENTS 5
#define MAX_BUFS_IN_FRAME 5

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
********************************************************************************
 * @func BUFFMGR_Init()
 * @brief  TII's (Texas Instrument India) implementation of buffer manager 
 *        initialization module
 *
 *         The BUFFMGR_Init function is called by the test application to
 *         initialise the global buffer element array to default and to allocate
 *         required number of memory data for reference and output buffers.
 *         The maximum required dpb size is defined by the supported profile & 
 *         level.
 *
 * @param  numRefBufs
 *         Number of buffers to be allocated
 *
 * @param  bufSizes
 *         Address of array containing the buffer sizes of the numbufs
 *
 * @return Success(0)/failure(-1) in allocating and initialising
 *
********************************************************************************
*/
XDAS_Int32 BUFFMGR_Init(XDAS_Int32 numRefBufs, XDAS_Int32 numBufs, 
                        XDM2_BufSize *bufSizes,
                        XDAS_Int8 lumaTilerSpace, XDAS_Int8 chromaTilerSpace);

/**
********************************************************************************
 *  @func     BUFFMGR_ReInit
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          re-initialization module
 *
 *          The BUFFMGR_ReInit function allocates global luma and chroma buffers
 *          and allocates entire space to first element. This element will be
 *          used in first frame decode. After the picture's height and width and
 *          its luma and chroma buffer requirements are obtained the global luma
 *          and chroma buffers are re-initialised to other elements in the
 *          buffer array.
 *
 *  @param[in]  numRefBufs : Number of reference buffers to be allocated
 *
 *  @param[in]  numOutBufs : Number of output buffers
 *
 *  @param[in]  bufSizes   : Pointer to the XDM2_BufSize structure
 *
 *  @return Success(0)/failure(-1) in allocating and initialising
********************************************************************************
*/
XDAS_Int32 BUFFMGR_ReInit
(
  XDAS_Int32 numRefBufs,
  XDAS_Int32 numOutBufs,
  XDM2_BufSize *bufSizes
);

/**
********************************************************************************
 *  @func     BUFFMGR_Init
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_GetFreeBuffer module
 *
 *          The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
 *          global buffer array and returns the address of that element. Incase
 *          if none of the elements are free then it returns NULL
 *
 *  @return Address of the first empty Buffer Array if success else NULL
********************************************************************************
*/
BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer();

/**
********************************************************************************
 *  @func     BUFFMGR_ReleaseBuffer
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_ReleaseBuffer module
 *
 *          The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
 *          which are released by the test-app. "0" is not a valid buffer Id
 *          hence this function keeps moving until it encounters a buffer Id as
 *          zero or it hits the MAX_BUFF_ELEMENTS
 *
 *  @param  bufffId    Array of buffer ID's holds the index of the available
 *                     buffers, includes both the buffers locked by the codec
 *                     as well the ones that are free
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[]);

/**
********************************************************************************
 *  @func     BUFFMGR_DeInit
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_DeInit module
 *
 *          The BUFFMGR_DeInit function releases all memory allocated by buffer
 *          manager.
 *
 *  @param[in]  numRefBufs : Number of reference buffers to be allocated
 *
 *  @param[in]  numOutBufs : Number of output buffers
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_DeInit
(
  XDAS_Int32 numRefBufs,
  XDAS_Int32 numOutBufs
);

#endif

