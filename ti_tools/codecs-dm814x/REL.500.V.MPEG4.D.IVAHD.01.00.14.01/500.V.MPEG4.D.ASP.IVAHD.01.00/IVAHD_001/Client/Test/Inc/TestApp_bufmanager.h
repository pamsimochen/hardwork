/*
********************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
********************************************************************************
*/

/**
*****************************************************************************
* @file   <TestApp_bufmanager.h >
*
* @brief  This File contains function definitions which implements a sample
*         buffer management  algorithm for XDm 1.0 compliance on the test-app
*         side.
*
* @author:  Ashish Singh
*
* @version 0.0 (July 2008)  : Initial version
*
* @version 0.1 (Apr 2010) : Code commenting and cleanup
*                           [Ananya]
* @version 0.2 (Oct 2010) : Cleared doxygen warning
******************************************************************************
*/

/* ------compilation control switches -------------------------*/
#ifndef _TESTAPP_BUFFMANAGER_H_
#define _TESTAPP_BUFFMANAGER_H_

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>

#define my_Memalign(align, size) memalign(align, size)

#define MAX_BUFF_ELEMENTS 4
#define MAX_BUFS_IN_FRAME 3
#define ADD_DISPLAY_BUFS  0

/**
 ******************************************************************************
 *  @enum       BUFFMGR_BufferStatus
 *  @brief      This enum contains Status of the buffer elements
 *  @remarks    none
 ******************************************************************************
*/
typedef enum
{
  BUFFMGR_BUFFER_FREE = 0,
    /**<
    * This indicates the buffer element is free, available for use.
    */

  BUFFMGR_BUFFER_USED = 1
    /**<
    * This indicates the buffer element is in use.
    */
} BUFFMGR_BufferStatus;

/**
 ******************************************************************************
 *  @struct BuffEle
 *
 *  @brief  This tells about Each element in the buffer array.
 *          Every time a new buffer is requested buffer manager
 *          module returns a pointer to one of the elements
 *          defined below from the buffer array
 *
 *  @param  bufId :
 *
 *  @param  bufStatus :
 *
 *  @param  bufSize[MAX_BUFS_IN_FRAME] :
 *
 *  @param  buf[MAX_BUFS_IN_FRAME] :
 *
 ******************************************************************************
*/
typedef struct BuffEle
{
  XDAS_UInt32           bufId;
  BUFFMGR_BufferStatus  bufStatus;
  XDM2_BufSize          bufSize[MAX_BUFS_IN_FRAME];
  volatile XDAS_UInt8  *buf[MAX_BUFS_IN_FRAME];

} BUFFMGR_buffEle;

typedef BUFFMGR_buffEle* BUFFMGR_buffEleHandle;

extern BUFFMGR_buffEle buffArray[MAX_BUFF_ELEMENTS];

/**
********************************************************************************
* @fn BUFFMGR_Init(XDAS_Int32   numBufs, XDM2_BufSize *bufSizes,
*              XDAS_Int32   *memType )
*
*
* @brief  TI's (Texas Instrument) implementation of buffer manager
*         initialization module
*
*         The BUFFMGR_Init function is called by the test application to
*         initialize the global buffer element array to default and to allocate
*         required number of memory data for reference and output buffers.
*         The maximum required dpb size is defined by the supported profile &
*         level.
*
*  @param[in] numBufs   : Number of buffers to be allocated.
*
*  @param[in] bufSizes  : Address of array containing the buffer sizes of the
*                         numbufs.
*  @param[in] memType   : Pointer to the memory type to tell whether it is
*                         Tiled memory or not
*
*
*
*
*
*  @return Success(0)/failure(-1) in allocating and initialising
*
********************************************************************************
*/

extern XDAS_Int32 BUFFMGR_Init(XDAS_Int32 numBufs, XDM2_BufSize *bufSizes,
                               XDAS_Int32 *memType);


/**
********************************************************************************
* @fn  BUFFMGR_ReInit(XDAS_Int32   numRefBufs,XDAS_Int32   numOutBufs,
*        XDM2_BufSize *bufSizes,XDAS_Int32   *memType,
*        sAppControlParams  appControlPrms,XDAS_UInt32 decodeHeader)
*
* @brief TI's (Texas Instrument) implementation of buffer manager
*        re-initialization module
*
*        The BUFFMGR_ReInit function allocates global luma and chroma buffers
*        and allocates entire space to first element. This element will be used
*        in first frame decode. After the picture's height and width and its
*        luma and chroma buffer requirements are obtained the global luma and
*        chroma buffers are re-initialised to other elements in the
*        buffer arary.
*
* @param[in] numRefBufs : Number of Buffers that will work as reference
*                          buffers for decoding of a frame.
* @param[in] numOutBufs : pointer to SeqParams.
*
* @param[in] *bufSizes  : Pointer to array containing the buffer sizes.
*
* @param[in] *memType   : Pointer to the array containing memory type
*                          (Tiler/Non-Tiler)
*
* @return Success(0)/failure(-1) in allocating and initialising
*
********************************************************************************
*/


extern XDAS_Int32 BUFFMGR_ReInit
(
   XDAS_Int32 numRefBufs,
   XDAS_Int32 numOutBufs,
   XDM2_BufSize *bufSizes,
   XDAS_Int32 *memType,
   XDAS_Int32  startBufIdx
);

/**
********************************************************************************
* @fn BUFFMGR_GetFreeBuffer()
*
* @brief  TI's (Texas Instrument) implementation of buffer manager
*         re-initialization module
*
*         The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
*         global buffer array and returns the address of that element. Incase
*         if none of the elements are free then it returns NULL
*
* @param : None
*
* @return Valid buffer element address
*         (address of the first empty Buffer Array)
*         or NULL incase if no buffers are empty
*
********************************************************************************
*/
extern BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer();

/**
********************************************************************************
* @fn   BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[])
*
* @brief  TI's (Texas Instrument) implementation of buffer manager
*         re-initialization module
*
*         The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
*         which are released by the test-app. "0" is not a valid buffer Id
*         hence this function keeps moving until it encounters a buffer Id
*         as zero or it hits the MAX_BUFF_ELEMENTS
*
* @param[in] bufffId  : ID of the buffer to be released
*
* @return None
*
********************************************************************************
*/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[]);

/**
********************************************************************************
* @fn   BUFFMGR_ReleaseAllBuffers()
*
* @brief  implementation of buffer manager  re-initialization module
*
*         The BUFFMGR_ReleaseAllBuffers function will set the status of
*         all buffer elements in the buffArray[] to free. This is called
*         for cases when application does not set freebufid of all buffers
*         typically when stream has not finished but app wants to end decoding.
*
* @param  : None
*
* @return None
********************************************************************************
*/

void BUFFMGR_ReleaseAllBuffers();

/**
********************************************************************************
* @fn   BUFFMGR_DeInit(XDAS_Int32 numRefBufs,XDAS_Int32 numOutBufs,
*                              sAppControlParams    appControlPrms)
*
* @brief  TI's (Texas Instrument) implementation of buffer manager
*         re-initialization module
*
*         The BUFFMGR_DeInit function releases all memory allocated by buffer
*         manager.
*
* @param[in] numRefBufs  : Number of reference buffers
* @param[in] numOutBufs  : Number of output buffers
* @param[in] appControlPrms  : config structure
*
* @return None
*
********************************************************************************
*/
void BUFFMGR_DeInit
(
   XDAS_Int32 numRefBufs,
   XDAS_Int32 numOutBufs
);

#endif

