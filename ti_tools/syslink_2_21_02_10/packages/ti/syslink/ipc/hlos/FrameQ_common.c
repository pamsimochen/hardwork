/*
 *  @file   FrameQ_common.c
 *
 *  @brief      Implements FrameQ  common functions independent of knl and user
 *              virtual space.
 *
 *
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */




/* Standard headers */
#include <ti/syslink/Std.h>

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Module level headers */
#include <ti/syslink/FrameQBufMgr.h>


#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/FrameQ.h>


/*!
 * @brief API to to find out the specified interface type is supported or not.
 * @param type  interface type.
 */
Int32 FrameQ_isSupportedInterface(UInt32 type)
{
    Int32  status = TRUE;

    switch (type)
    {
        case FrameQ_INTERFACE_SHAREDMEM:
            /* Supported interface */
        break;
        default:
            /* unsupported interface type specifiec */
            status = FALSE;
    }

    return (status);
}




/******************************************************************************
 *                            Helper API
 ******************************************************************************/

/*
 * @brief API  to get the pointer to the extended header.Apps has to use this
 *        API to get  pointer  to their application defined exteneded header
 *        that starts after the bsee frame header.
 *
 * @param buf      frame.
 */
Ptr FrameQ_getExtendedHeaderPtr(FrameQ_Frame frame)
{
    Ptr     extHeaderPtr = NULL;
    UInt32  offset;

    GT_assert (curTrace, (NULL != frame));

    offset = (  sizeof (FrameQ_FrameHeader)
              + (   (frame->numFrameBuffers - 1)
                  * sizeof (FrameQ_FrameBufInfo)));

    extHeaderPtr = (Ptr) ( (UInt32)frame + offset);

    return(extHeaderPtr);
}

/*
 * @brief Function to find out the number of frame buffers in a frame .
 *
 * @param buf      frame.
 */
UInt32
FrameQ_getNumFrameBuffers (FrameQ_Frame frame )
{
    UInt32 numbufs = 0 ;

    GT_1trace (curTrace,
               GT_ENTER,
               "FrameQ_getNumFrameBuffers",
               frame);

    GT_assert (curTrace, (NULL != frame));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frame == NULL) {
        /*! @retval 0  frame passed is null
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getNumFrameBuffers",
                             FrameQ_E_INVALIDARG,
                             "frame passed is null !");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        numbufs = frame->numFrameBuffers;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getNumFrameBuffers",numbufs);

    /*! @retval number of frame buffers*/
    return (numbufs) ;
}

/*
 * @brief Function to retrieve framebuffer  identified by frameBufNum from
 *        a given frame.
 *        Here no validation is performed for frameBufNum. User is expected to
 *        pass the right value for it.
 *       ( APPs can use getNumFrameBuffers API to know the number of frame
 *         buffers associated with the frame)
 * @param buf              frame.
 * @param frameBufNum      frame buffer number.
 */
Ptr
FrameQ_getFrameBuffer(FrameQ_Frame frame, UInt32 frameBufNum )
{
    Ptr                  frmBuffer = NULL;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQ_getFrameBuffer",
               frame,
               frameBufNum);


    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frame->numFrameBuffers < frameBufNum) {
        /*! @retval NULL  frameBufNum provided should be less
         * from 0 to number of frame buffers -1
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getFrameBuffer",
                             FrameQ_E_INVALIDARG,
                             "frameBufNum provided should be less from 0 to "
                             "number of frame buffers -1 .!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
        frmBuffer = (Ptr) frameBufInfo[frameBufNum].bufPtr;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getFrameBufSize",frmBuffer);

    /*! @retval frame buffer address  */
    return (frmBuffer) ;
}


/*
 * @brief Function to get the   size of the framebuffer
 *
 * @param buf              frame.
 * @param frameBufNum      frame buffer number.
 */
UInt32
FrameQ_getFrameBufSize(FrameQ_Frame frame, UInt32 frameBufNum )
{
    UInt32 size  = 0;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQ_getFrameBufSize",
               frame,
               frameBufNum);

    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frame->numFrameBuffers < frameBufNum) {
        /*! @retval 0  frameBufNum provided should be less
         * from 0 to number of frame buffers -1
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getFrameBufSize",
                             FrameQ_E_INVALIDARG,
                             "frameBufNum provided should be less from 0 to "
                             "number of frame buffers -1 .!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        size = frameBufInfo[frameBufNum].bufSize;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getFrameBufSize",size);

    /*! @retval size  frame buffer size */
    return (size) ;
}


/*
 * @brief Function to get the  valid data size of a framebuffer
 *       identified by the frame buffer number in a given frame.
 *
 * @param buf              frame.
 * @param frameBufNum      frame buffer number.
 */
UInt32
FrameQ_getFrameBufValidSize(FrameQ_Frame frame, UInt32 frameBufNum )
{
    UInt32 size  = 0;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQ_getFrameBufValidSize",
               frame,
               frameBufNum);

    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frame->numFrameBuffers < frameBufNum) {
        /*! @retval 0  frameBufNum provided should be less
         * from 0 to number of frame buffers -1
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getFrameBufValidSize",
                             FrameQ_E_INVALIDARG,
                             "frameBufNum provided should be less from 0 to "
                             "number of frame buffers -1 .!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        size = frameBufInfo[frameBufNum].validSize;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getFrameBufValidSize", size);

    /*! @retval size  frame buffer valid size */
    return (size) ;
}

/*
 * @brief Function to get the valid data start offset in framebuffer
 *       identified by the frame buffer number in a given frame.
 *
 * @param buf              frame.
 * @param frameBufNum      frame buffer number.
 */
UInt32
FrameQ_getFrameBufDataStartOffset(FrameQ_Frame  frame,
                                  UInt32        frameBufNum )
{
    UInt32              offSet  = -1;
    FrameQ_FrameBufInfo *frameBufInfo;

    GT_2trace (curTrace,
               GT_ENTER,
               "FrameQ_getFrameBufDataStartOffset",
               frame,
               frameBufNum);

    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frame->numFrameBuffers < frameBufNum) {
        /*! @retval 0xFFFFFFFF  frameBufNum provided should be less
         * from 0 to number of frame buffers -1
         */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQ_getFrameBufDataStartOffset",
                             FrameQ_E_INVALIDARG,
                             "frameBufNum provided should be less from 0 to "
                             "number of frame buffers -1 .!");
    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        offSet = frameBufInfo[frameBufNum].startOffset;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_getFrameBufDataStartOffset", offSet);

    /*! @retval size  frame buffer data start offset in the frame buffer */
    return (offSet) ;
}

/*
 * @brief Function to set the valid data size of a framebuffer
 *       identified by the frame buffer number in a given frame.
 *
 * @param buf              frame.
 * @param frameBufNum      frame buffer number.
 * @param validDataSize    valid data size in the buffer.
 */
Int32
FrameQ_setFrameBufValidSize(FrameQ_Frame  frame,
                            UInt32        frameBufNum,
                            UInt32        validDataSize )
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQ_FrameBufInfo     *frameBufInfo;

    GT_3trace (curTrace,
               GT_ENTER,
               "FrameQ_setFrameBufValidSize",
               frame,
               frameBufNum,
               validDataSize);

    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frameBufInfo[frameBufNum].bufSize <
            (validDataSize + frameBufInfo[frameBufNum].startOffset) ) {

        /*! @retval FrameQ_E_INVALIDARG  dataStartOffset + validsize
         *  is more than the  bufSize .
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                      GT_4CLASS,
                      "FrameQ_setFrameBufValidSize",
                      status,
                      "(dataStartOffset+validSize) is more than the bufSize.!");

    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        frameBufInfo[frameBufNum].validSize = validDataSize;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_setFrameBufValidSize",status);

    /*! @retval FrameQ_S_SUCCESS Successfully set valid size of a frame buffer */
    return (status) ;
}

/*
 * @brief Function to set the valid data start offset of a framebuffer
 *        identified by the frame buffer number in a given frame.
 *
 * @param buf              Frame.
 * @param frameBufNum      Frame buffer number.
 * @param dataStartOffset  Valid data start offset in the frame buffer.
 */
Int32
FrameQ_setFrameBufDataStartOffset (FrameQ_Frame  frame,
                                   UInt32        frameBufNum,
                                   UInt32        dataStartOffset)
{
    Int32                   status  = FrameQ_S_SUCCESS;
    FrameQ_FrameBufInfo     *frameBufInfo;

    GT_3trace (curTrace,
               GT_ENTER,
               "FrameQ_setFrameBufDataStartOffset",
               frame,
               frameBufNum,
               dataStartOffset);

    GT_assert (curTrace, (NULL != frame));

    GT_assert (curTrace, (frame->numFrameBuffers > frameBufNum)) ;

    frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (frameBufInfo[frameBufNum].bufSize <
            (dataStartOffset + frameBufInfo[frameBufNum].validSize)) {

        /*! @retval FrameQ_E_INVALIDARG  dataStartOffseti + validsize  is more
         *  than the bufSize .
         */
        status = FrameQ_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                   GT_4CLASS,
                   "FrameQ_setFrameBufDataStartOffset",
                   status,
                   "(dataStartOffset + validSize) is more than the bufSize.!");

    }
    else {
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/
        frameBufInfo[frameBufNum].startOffset = dataStartOffset;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /*#if !defined(SYSLINK_BUILD_OPTIMIZE)*/

    GT_1trace (curTrace, GT_LEAVE, "FrameQ_setFrameBufDataStartOffset",status);

    /*! @retval FrameQ_S_SUCCESS Successfully set data start offset of a
     * frame buffer.
     */
    return (status) ;
}
