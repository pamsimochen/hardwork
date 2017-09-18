/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
/**
 *  @file       ti/sdo/fc/edmamgr/edmamgr.h
 *
 *  @brief      EdmaMgr Definitions - Functional library for use
 *              with the EDMA3 DMA peripheral.
 *
 *  The intent of the EdmaMgr component is to provide a simple and intuitive
 *  interface for acquiring and using EDMA3 resources for asynchronous data
 *  transfers.
 *
 *  Once the framework provides the initial configuration to EdmaMgr_init(),
 *  malloc/free style APIs are used to obtain/release resources. The actual
 *  transfer has been abstracted to multiple APIs which can transfer data of
 *  various dimensions.
 */

/**
 *  @defgroup   ti_sdo_fc_edmamgr_EdmaMgr EdmaMgr
 *
 *  @brief      EDMA3 Functional library
 */
#ifndef ti_sdo_fc_edmamgr_EdmaMgr_
#define ti_sdo_fc_edmamgr_EdmaMgr_

/** @ingroup    ti_sdo_fc_ecpy_EdmaMgr */
/*@{*/

#include <stdint.h>
#include <string.h>

#include <ti/sdo/fc/ecpy/ecpy.h>
#include <ti/xdais/ires.h>
#include <ti/sdo/fc/ires/edma3chan/ires_edma3Chan.h>
#include <ti/sdo/fc/edma3/edma3_config.h>


/**
 *  @brief      Handle to an EdmaMgr instance.
 */
typedef void *EdmaMgr_Handle;


/** @cond INTERNAL */
typedef struct {
  IRES_EDMA3CHAN_Handle        edmaHandle;
  ECPY_Handle                  ecpyHandle;
  IRES_EDMA3CHAN_ProtocolArgs  edmaArgs;
  Bool                         resActive;
  Bool                         xferPending;
  IRES_YieldFxn                yieldFxn;
  IRES_YieldArgs               yieldArgs;
  IRES_YieldContext            yieldContext;
} EdmaMgr_Channel;
/** @endcond */

#define EdmaMgr_SUCCESS           0  /**< Success */
#define EdmaMgr_ERROR_INVARG     -1  /**< Failure, Invalid Argument */
#define EdmaMgr_ERROR_INVCFG     -2  /**< Failure, Invalid Configuration */
#define EdmaMgr_ERROR_RMANINIT   -3  /**< Failure, RMAN initialization */
#define EdmaMgr_ERROR_INVHANDLE  -4  /**< Failure, Invalid Handle */
#define EdmaMgr_ERROR_FREE       -5  /**< Failure to free a resource */

/**
 *  @brief      Initialize EdmaMgr
 *
 *  @param[in]  proc_id         Processor ID. This ID is used to index into the
 *                              EDMA3_InstanceInitConfig array pointed to by the
 *                              ti_sdo_fc_edmamgr_edma3RegionConfig variable.
 *
 *  @param[in]  edma3_config    Optional edma3_config which will override the
 *                              configuration which is pointed to by
 *                              ti_sdo_fc_edmamgr_edma3RegionConfig.
 *
 *  @retval     EdmaMgr_SUCCESS         @copydoc EdmaMgr_SUCCESS
 *  @retval     EdmaMgr_ERROR_INVARG    @copydoc EdmaMgr_ERROR_INVARG
 *  @retval     EdmaMgr_ERROR_INVCFG    @copydoc EdmaMgr_ERROR_INVCFG
 *  @retval     EdmaMgr_ERROR_RMANINIT  @copydoc EdmaMgr_ERROR_RMANINIT
 *
 *  @remarks    @c edma3_config may be NULL to accept default values.
 *
 *  @remarks    The FC product includes an example (ex23_edmamgr) which
 *              demonstrates how to set up a @c ti_sdo_fc_edma3RegionConfig
 *              variable.
 */
int32_t EdmaMgr_init
(
  int32_t  proc_id,
  void    *edma3_config
);


/**
 *  @brief      Allocate an EdmaMgr channel
 *
 *  @param[in]  max_linked_transfers    Maximum linked transfers
 *
 *  @retval     NULL        Failure to allocate the channel
 *  @retval     non-NULL    Success
 *
 *  @pre        EdmaMgr_init() must have first been called.
 *
 */
EdmaMgr_Handle EdmaMgr_alloc
(
  int32_t max_linked_transfers
);


/**
 *  @brief      Frees an EdmaMgr channel.
 *
 *  @param[in]  h       Handle returned from EdmaMgr_alloc().
 *
 *  @retval     EdmaMgr_SUCCESS         @copydoc EdmaMgr_SUCCESS
 *  @retval     EdmaMgr_ERROR_INVHANDLE @copydoc EdmaMgr_ERROR_INVHANDLE
 *  @retval     EdmaMgr_ERROR_FREE      @copydoc EdmaMgr_ERROR_FREE
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 */
int32_t EdmaMgr_free
(
  EdmaMgr_Handle    h
);


/*
 *  The below declarations are placed here as only a reference
 *  (hence the comment block).
 *
 *  These functions are defined further below.
 */
#if defined(EDMAMGR_INLINE_ALL)
#include <ti/sdo/fc/edmamgr/edmamgr_xfer.h>
#else
/**
 *  @brief      Wait for transfers to complete
 *
 *  @param[in]  h       Handle returned from EdmaMgr_alloc().
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    This function waits for all transfers on a specific channel
 *              to complete.  It is a blocking call.
 */
void EdmaMgr_wait
(
  EdmaMgr_Handle    h
);


/**
 *  @brief      Perform a single 1D->1D transfer
 *
 *  @param[in]  h           Handle returned from EdmaMgr_alloc().
 *  @param[in]  src         Source address
 *  @param[in]  dst         Destination address
 *  @param[in]  num_bytes   Number of bytes to transfer
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    Perform a single 1D->1D transfer of @c num_bytes bytes from
 *              @c src address to @c dst address.
 *
 *  @sa EdmaMgr_copy1D1DLinked
 */
int32_t EdmaMgr_copy1D1D
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst,
  int32_t           num_bytes
);

/**
 *  @brief      Perform a 1D->2D transfer
 *
 *  @param[in]  h           Handle returned from EdmaMgr_alloc().
 *  @param[in]  src         Source address
 *  @param[in]  dst         Destination address
 *  @param[in]  num_bytes   Number of bytes to transfer
 *  @param[in]  num_lines   Number of lines
 *  @param[in]  pitch       Pitch
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    Perform a 1D->2D transfer of @c num_bytes bytes.  The source
 *              is one dimensional, and the destination is two dimensional.
 *
 *  @remarks    After every line of @c num_bytes is transferred, the @c src
 *              address is incremented by @c num_bytes and the @c dst address
 *              is incremented by @c pitch bytes.
 */
int32_t EdmaMgr_copy1D2D
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst,
  int32_t           num_bytes,
  int32_t           num_lines,
  int32_t           pitch
);

/**
 *  @brief      Perform a 2D->1D transfer
 *
 *  @param[in]  h           Handle returned from EdmaMgr_alloc().
 *  @param[in]  src         Source address
 *  @param[in]  dst         Destination address
 *  @param[in]  num_bytes   Number of bytes to transfer
 *  @param[in]  num_lines   Number of lines
 *  @param[in]  pitch       Pitch
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    Perform a 2D->1D transfer of @c num_bytes bytes.  The source
 *              is two dimensional, and the destination is one dimensional.
 *
 *  @remarks    After every line of @c num_bytes is transferred, the @c src
 *              address is incremented by @c pitch and the @c dst address
 *              is incremented by @c num_bytes bytes.
 */
int32_t EdmaMgr_copy2D1D
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst,
  int32_t           num_bytes,
  int32_t           num_lines,
  int32_t           pitch
);

/**
 *  @brief      Perform a 2D->2D transfer of buffers with the same pitch
 *
 *  @param[in]  h           Handle returned from EdmaMgr_alloc().
 *  @param[in]  src         Source address
 *  @param[in]  dst         Destination address
 *  @param[in]  num_bytes   Number of bytes to transfer
 *  @param[in]  num_lines   Number of lines
 *  @param[in]  pitch       Pitch
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    Perform a 2D->2D transfer of @c num_bytes bytes.  The source
 *              and destination are two dimensional.
 *
 *  @remarks    After every line of @c num_bytes is transferred, the @c src
 *              address and @c dst address is incremented by @c pitch bytes.
 */
int32_t EdmaMgr_copy2D2D
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst,
  int32_t           num_bytes,
  int32_t           num_lines,
  int32_t           pitch
);

/**
 *  @brief      Perform a 2D->2D transfer of buffers with different pitches
 *
 *  @param[in]  h           Handle returned from EdmaMgr_alloc().
 *  @param[in]  src         Source address
 *  @param[in]  dst         Destination address
 *  @param[in]  num_bytes   Number of bytes to transfer
 *  @param[in]  num_lines   Number of lines
 *  @param[in]  src_pitch   Source buffer pitch
 *  @param[in]  dst_pitch   Destination buffer pitch
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @remarks    Perform a 2D->2D transfer of @c num_bytes bytes.  The source
 *              and destination are two dimensional.
 *
 *  @remarks    After every line of @c num_bytes is transferred, the @c src
 *              address is incremented by @c src_pitch bytes and the @c dst
 *              address is incremented by @c dst_pitch bytes.
 */
int32_t EdmaMgr_copy2D2DSep
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst,
  int32_t           num_bytes,
  int32_t           num_lines,
  int32_t           src_pitch,
  int32_t           dst_pitch
);

/**
 *  @brief      Perform a group of linked 1D->1D transfers
 *
 *  @param[in]  h               Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[]           Array of source addresses
 *  @param[in]  dst[]           Array of destination addresses
 *  @param[in]  num_bytes[]     Array of number of bytes to transfer
 *  @param[in]  num_transfers   Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @sa EdmaMgr_copy1D1D
 */
int32_t EdmaMgr_copy1D1DLinked
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_bytes[],
  int32_t           num_transfers
);

/**
 *  @brief      Perform a group of linked 1D->2D transfers
 *
 *  @param[in]  h               Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[]           Array of source addresses
 *  @param[in]  dst[]           Array of destination addresses
 *  @param[in]  num_bytes[]     Array of number of bytes to transfer
 *  @param[in]  num_lines[]     Array of lines to transfer
 *  @param[in]  pitch[]         Array of pitches
 *  @param[in]  num_transfers   Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @sa EdmaMgr_copy1D2D
 */
int32_t EdmaMgr_copy1D2DLinked
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_bytes[],
  int32_t           num_lines[],
  int32_t           pitch[],
  int32_t           num_transfers
);

/**
 *  @brief      Perform a group of linked 2D->1D transfers
 *
 *  @param[in]  h               Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[]           Array of source addresses
 *  @param[in]  dst[]           Array of destination addresses
 *  @param[in]  num_bytes[]     Array of number of bytes to transfer
 *  @param[in]  num_lines[]     Array of lines to transfer
 *  @param[in]  pitch[]         Array of pitches
 *  @param[in]  num_transfers   Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @sa EdmaMgr_copy2D1D
 */
int32_t EdmaMgr_copy2D1DLinked
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_bytes[],
  int32_t           num_lines[],
  int32_t           pitch[],
  int32_t           num_transfers
);

/**
 *  @brief      Perform a group of linked 2D->2D transfers
 *
 *  @param[in]  h               Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[]           Array of source addresses
 *  @param[in]  dst[]           Array of destination addresses
 *  @param[in]  num_bytes[]     Array of number of bytes to transfer
 *  @param[in]  num_lines[]     Array of lines to transfer
 *  @param[in]  pitch[]         Array of pitches
 *  @param[in]  num_transfers   Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @sa EdmaMgr_copy2D2D
 */
int32_t EdmaMgr_copy2D2DLinked
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_bytes[],
  int32_t           num_lines[],
  int32_t           pitch[],
  int32_t           num_transfers
);

/**
 *  @brief      Perform a group of linked 2D->2D transfers with different
 *              pitches
 *
 *  @param[in]  h               Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[]           Array of source addresses
 *  @param[in]  dst[]           Array of destination addresses
 *  @param[in]  num_bytes[]     Array of number of bytes to transfer
 *  @param[in]  num_lines[]     Array of lines to transfer
 *  @param[in]  src_pitch[]     Array of source pitches
 *  @param[in]  dst_pitch[]     Array of destination pitches
 *  @param[in]  num_transfers   Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @sa EdmaMgr_copy2D2DSep
 */
int32_t EdmaMgr_copy2D2DSepLinked
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_bytes[],
  int32_t           num_lines[],
  int32_t           src_pitch[],
  int32_t           dst_pitch[],
  int32_t           num_transfers
);

/**
 *  @brief      Perform a fast copy. This API inherits the transfer configuration
 *              of a previous transfer and only modifies the @c src and @c dst
 *              addresses.
 *
 *  @param[in]  h     Handle returned from EdmaMgr_alloc().
 *  @param[in]  src   Source address
 *  @param[in]  dst   Destination address
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @pre        A non-linked transfer must have been completed on handle @c h
 *              before calling this function.
 *
 *  @sa EdmaMgr_copyLinkedFast
 */
int32_t EdmaMgr_copyFast
(
  EdmaMgr_Handle    h,
  void *restrict    src,
  void *restrict    dst
);

/**
 *  @brief      Perform a linked fast copy. This API inherits the transfer
 *              configuration of a previous transfer and only modifies the
 *              @c src and @c dst addresses.
 *
 *  @param[in]  h     Handle returned from EdmaMgr_alloc().
 *  @param[in]  src[] Array of source addresses
 *  @param[in]  dst[] Array of destination addresses
 *  @param[in]  num_transfers Number of transfers
 *
 *  @pre        @c h must be a handle successfully returned from EdmaMgr_alloc().
 *
 *  @pre        A linked transfer must have been completed on handle @c h
 *              before calling this function.
 *
 *  @sa EdmaMgr_copyLinkedFast
 */
int32_t EdmaMgr_copyLinkedFast
(
  EdmaMgr_Handle    h,
  void *restrict    src[],
  void *restrict    dst[],
  int32_t           num_transfers
);
#endif

/*@}*/

#endif /* ti_sdo_fc_edmamgr_EdmaMgr_ */
