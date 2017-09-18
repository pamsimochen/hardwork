/** 
 *  @file   FrameQDefs.h
 *
 *  @brief      Defines configurable elements for FrameQ module.  (Deprecated)
 *
 *  @frameqDeprecated
 *
 *
 */
/* 
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



#ifndef FRAMEQDEFS_H_0x6e6f
#define FRAMEQDEFS_H_0x6e6f


#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * macros & defines
 * =============================================================================
 */

/*!
 *  @brief  Macro used in FrameQ for sorting of passed parameters in V API.
 *          In FrameQ_allocv case this denotes the number of frames that can be
 *          allocated in one call.
 *          In FrameQ_putv case this denotes the number of frames that can be
 *          put in one call.
 *          In FrameQ_getv case this denotes the number of frames that can be
 *          retrieved in one call.
 */
#define FrameQ_MAX_FRAMESINVAPI           (64u)

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Structure to frame buffer informantion in frame header.
 */
typedef struct Frame_FrameBufInfo_tag {
    UInt32 bufPtr;
    /*!<Pointer to the frame buffer.*/
    UInt32 bufSize;
    /*!<Size of the buffer. */
    UInt32 startOffset;
    /*!<Start offset in the buffer from where valid data starts.*/
    UInt32 validSize;
    /*!<Valid size in the buffer from the zeroth offset in the frame buffer*/
    UInt32 pixelFormat;
    UInt32 height;
    UInt32 width;
    UInt32 stride;

}Frame_FrameBufInfo;

/*!
 *  @brief  Structure of the base frame header. FrameQ maintains  list of frames
 *          to be retrieved by FrameQ  reader client. Application is expected to
 *          use Helper API  to extract fileds and exteneded header pointer.
 */
typedef struct  Frame_FrameHeader_Tag {
    UInt32  reserved0;
    /*!<reserverd field */
    UInt32  reserved1;
    /*!<reserverd field */
    UInt16  baseHeaderSize;
    /*!< Size of the base frame header.*/
    UInt16  headerSize;
    /*!< Size of the frame  header(total size of base header
     *   and extended header).
     */
    UInt32  frmAllocaterId;
    /*!< Denotes the FrameQBufMgr Id from which frame is allocated. */
    UInt32 frameBufType;
    /* Denotes the buffer type i.e Tiler buffer or shared region 1D buffer*/
    UInt8   freeFrmQueueNo;
    /*!< Free quque No from which frame is allocated in FrameQBufMgr.*/
    UInt8   numFrameBuffers;
    /*!<Number of framebuffers in frame. */
    Frame_FrameBufInfo  frameBufInfo[1];
    /*!< Information of Frame buffers. see Frame_FrameBufInfo*/
} Frame_FrameHeader;

/*!
 *  @brief  Defines the type for a frame pointer
 */
typedef Frame_FrameHeader * Frame;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /*FRAMEQDEFS_H_0x6e6f*/
