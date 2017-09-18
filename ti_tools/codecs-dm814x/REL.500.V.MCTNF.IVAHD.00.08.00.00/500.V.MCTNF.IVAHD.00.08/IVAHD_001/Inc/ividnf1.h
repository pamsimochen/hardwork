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
 *
 */

/**
 *  @file       ividnf1.h
 *
 *  @brief      This header defines all types, constants, and functions
 *              shared by all implementations of the video noise filter
 *              algorithms.
 */
/**
 *  @defgroup   ti_xdais_dm_IVIDNF1   IVIDNF1 - XDM Video Noise Filter Interface
 *
 *  This is the XDM IVIDNF1 Video Noise Filter Interface.
 */

#ifndef ti_xdais_dm_IVIDNF1_
#define ti_xdais_dm_IVIDNF1_

#include <ti/xdais/ialg.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include <ti/xdais/dm/ivideo.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @ingroup    ti_xdais_dm_IVIDNF1 */
/*@{*/

#define IVIDNF1_EOK       XDM_EOK             /**< @copydoc XDM_EOK */
#define IVIDNF1_EFAIL     XDM_EFAIL           /**< @copydoc XDM_EFAIL */
#define IVIDNF1_EUNSUPPORTED XDM_EUNSUPPORTED /**< @copydoc XDM_EUNSUPPORTED */


/**
 *  @brief      This must be the first field of all IVIDNF1
 *              instance objects
 */
typedef struct IVIDNF1_Obj {
    struct IVIDNF1_Fxns *fxns;
} IVIDNF1_Obj;


/**
 *  @brief      Opaque handle to an IVIDNF1 objects.
 */
typedef struct IVIDNF1_Obj  *IVIDNF1_Handle;

/**
 *  @brief      Defines the creation time parameters for
 *              all IVIDNF1 instance objects
 *
 *  @extensibleStruct
 */
typedef struct IVIDNF1_Params {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 filterPreset;    /**< Filter preset. */
    XDAS_Int32 maxHeight;       /**< Maximum video height in pixels. */
    XDAS_Int32 maxWidth;        /**< Maximum video width in pixels. */
    XDAS_Int32 dataEndianness;  /**< Endianness of output data.
                                 *
                                 *   @sa    XDM_DataFormat
                                 */
    XDAS_Int32 inputChromaFormat;/**< Chroma format for the input buffer.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 inputContentType;/**< Video content type of the buffer being
                                 *   filtered.
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDNF1_Fxns::control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              #IVIDNF1_Fxns::process() to
                                 *              set the
                                 *              IVIDNF1_DynamicParams::getDataFxn
                                 *              and
                                 *              IVIDNF1_DynamicParams::getDataHandle.
                                 *              Else, the alg can return
                                 *              error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @remarks   If a subframe mode is provided,
                                 *              the application must call
                                 *              IVIDNF1_Fxns::control() with
                                 *              #XDM_SETPARAMS id prior to
                                 *              #IVIDNF1_Fxns::process() to
                                 *              set the
                                 *              IVIDNF1_DynamicParams::putDataFxn,
                                 *              IVIDNF1_DynamicParams::putDataHandle
                                 *              (and optionally
                                 *              IVIDNF1_DynamicParams::getBufferFxn,
                                 *              and
                                 *              IVIDNF1_DynamicParams::getBufferHandle).
                                 *              Else, the alg can return
                                 *              error.
                                 *
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 numInputDataUnits; /**< Number of input slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c inputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks  Ignored if @c inputDataMode
                                 *             is set to full frame mode.
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c outputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks  Ignored if @c outputDataMode
                                 *             is set to full frame mode.
                                 */
} IVIDNF1_Params;


/**
 *  @brief      This structure defines the algorithm parameters that can be
 *              modified after creation via control() calls
 *
 *  @remarks    It is not necessary that a given implementation support all
 *              dynamic parameters to be configurable at run time.  If a
 *              particular algorithm does not support run-time updates to
 *              a parameter that the application is attempting to change
 *              at runtime, it may indicate this as an error.
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDNF1_Fxns::control()
 */
typedef struct IVIDNF1_DynamicParams {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 inputHeight;     /**< Input frame height. */
    XDAS_Int32 inputWidth;      /**< Input frame width. */
    XDAS_Int32 captureWidth;    /**< DEFAULT(0): use imagewidth as
                                 *   pitch else use given capture
                                 *   width for pitch provided it
                                 *   is greater than image width.
                                 */
    XDAS_Int32 numPastRef;       /**< Number of past refrence frames
                                 *    used for motion search
                                 */
    XDAS_Int32 numFutureRef;     /**< Number of future reference frames
                                 *    used for motion search
                                 */
    XDM_DataSyncPutFxn putDataFxn; /**< Optional datasync "put data" function.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              algorithm.  The algorithm
                                 *              calls this function when data
                                 *              has been put into an output
                                 *              buffer.
                                 */
    XDM_DataSyncHandle putDataHandle;/**< Datasync "put data" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              algorithm must provide when
                                 *              calling the app-registered
                                 *              IVIDNF1_DynamicParams.putDataFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDNF1_DynamicParams.putDataFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDM_DataSyncGetFxn getDataFxn;/**< Datasync "get data" function.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              algorithm.  The algorithm
                                 *              calls this function to get
                                 *              partial video buffer(s)
                                 *              from the app/framework.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 */
    XDM_DataSyncHandle getDataHandle;/**< Datasync "get data" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              algorithm must provide when
                                 *              calling @c getDataFxn.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDNF1_DynamicParams.getDataFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDM_DataSyncGetBufferFxn getBufferFxn;/**< Datasync "get buffer" function.
                                 *
                                 *   @remarks   This function is provided
                                 *              by the app/framework to the
                                 *              algorithm.  The algorithm
                                 *              calls this function to obtain
                                 *              partial compressed bit-stream
                                 *              data buffers from the
                                 *              app/framework.
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 */
    XDM_DataSyncHandle getBufferHandle;/**< Datasync "get buffer" handle
                                 *
                                 *   @remarks   This is a handle which the
                                 *              algorithm must provide when
                                 *              calling the app-registered
                                 *              IVIDNF1_DynamicParam.getBufferFxn().
                                 *
                                 *   @remarks   Apps/frameworks that don't
                                 *              support datasync should set
                                 *              this to NULL.
                                 *
                                 *   @remarks   For an algorithm, this handle
                                 *              is read-only; it must not be
                                 *              modified when calling
                                 *              the app-registered
                                 *              IVIDNF1_DynamicParams.getBufferFxn().
                                 *
                                 *   @remarks   The app/framework can use
                                 *              this handle to differentiate
                                 *              callbacks from different
                                 *              algorithms.
                                 */
    XDAS_Int32 lateAcquireArg;  /**< Argument used during late acquire.
                                 *
                                 *   @remarks   For all control() commands
                                 *              other than
                                 *              #XDM_SETLATEACQUIREARG, this
                                 *              field is ignored and can
                                 *              therefore be set by the
                                 *              caller to any value.
                                 *
                                 *   @remarks   This field is used to
                                 *              provide the
                                 *              'late acquire' arg required by
                                 *              #XDM_SETLATEACQUIREARG.
                                 *
                                 *   @remarks   Late acquire support is
                                 *              an optional feature for
                                 *              the algorithm.  If the
                                 *              algorithm supports late
                                 *              acquisition of resources,
                                 *              and the application has supplied
                                 *              a lateAcquireArg value (via
                                 *              #XDM_SETLATEACQUIREARG), then the
                                 *              algorithm must also provide this
                                 *              @c lateAcquireArg value when
                                 *              requesting resources (i.e.
                                 *              during their call to
                                 *              acquire() when requesting
                                 *              the resource).
                                 */
} IVIDNF1_DynamicParams;


/**
 *  @brief      Defines the input arguments for all IVIDNF1 instance
 *              process function
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDNF1_Fxns::process()
 */
typedef struct IVIDNF1_InArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 inBufID;         /**< Identifier to attach with the corresponding
                                 *   input frames to be filtered.
                                 *
                                 *   @remarks   This is a uniqe ID asssociated with
                                 *              input buffer provided to the algorithm.
                                 *              This ID must be provided by the application.
                                 *              The input buffer will host one unfiltered
                                 *              video frame.
                                 *
                                 *   @remarks   Zero (0) is <b>not</b> a
                                 *              supported value.  This value
                                 *              is reserved for cases when
                                 *              there is no input buffer
                                 *              provided.
                                 *
                                 *   @sa IVIDNF1_OutArgs::freeInBufID.
                                 */
    XDAS_Int32 outBufID;        /**< Identifier to attach with the corresponding
                                 *   input frames to be encoded.
                                 *
                                 *   @remarks   This is a uniqe ID asssociated with
                                 *              output buffer provided to the algorithm.
                                 *              This ID must be provided by the application.
                                 *              The output buffer will host one filtered
                                 *              video frame.
                                 *
                                 *   @remarks   Zero (0) is <b>not</b> a
                                 *              supported value.  This value
                                 *              is reserved for cases when
                                 *              there is no output buffer
                                 *              provided.
                                 *
                                 *   @sa IVIDNF1_OutArgs::freeOutBufID.
                                 */
} IVIDNF1_InArgs;


/**
 *  @brief      Defines instance status parameters
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDNF1_Fxns::control()
 */
typedef struct IVIDNF1_Status {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
    XDM1_SingleBufDesc data;    /**< Buffer descriptor for data passing.
                                 *
                                 *   @remarks   If this field is not used,
                                 *              the application <b>must</b>
                                 *              set @c data.buf to NULL.
                                 *
                                 *   @remarks   This buffer can be used as
                                 *              either input or output,
                                 *              depending on the command.
                                 *
                                 *   @remarks   The buffer will be provided
                                 *              by the application, and
                                 *              returned to the application
                                 *              upon return of the
                                 *              IVIDNF1_Fxns.control()
                                 *              call.  The algorithm must
                                 *              not retain a pointer to this
                                 *              data.
                                 *
                                 *   @sa #XDM_GETVERSION
                                 */

    XDAS_Int32  filterPreset;    /**< Filter preset. */
    XDAS_Int32 inputChromaFormat;/**< Chroma format for the input buffer.
                                 *
                                 *   @sa XDM_ChromaFormat
                                 */
    XDAS_Int32 inputContentType; /**< Video content type of the buffer being
                                 *   filtered.
                                 *
                                 *   @sa IVIDEO_ContentType
                                 */
    XDAS_Int32 inputDataMode;   /**< Input data mode.
                                 *
                                 *   @sa IVIDNF1_Params.inputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 outputDataMode;  /**< Output data mode.
                                 *
                                 *   @sa IVIDNF1_Params.outputDataMode
                                 *   @sa IVIDEO_DataMode
                                 */
    XDAS_Int32 numInputDataUnits; /**< Number of input slices/rows.
                                 *
                                 *   @remarks   Units depend on the
                                 *              IVIDNF1_Params.inputDataMode,
                                 *              like number of
                                 *              slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDNF1_Params.inputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDNF1_Params.inputDataMode
                                 */
    XDAS_Int32 numOutputDataUnits;/**< Number of output slices/rows.
                                 *
                                 *   @remarks  Units depend on the
                                 *             @c outputDataMode, like number of
                                 *             slices/rows/blocks etc.
                                 *
                                 *   @remarks   Ignored if
                                 *              IVIDNF1_Params.outputDataMode
                                 *              is set to full frame mode.
                                 *
                                 *   @sa IVIDNF1_Params.outputDataMode
                                 */
   XDAS_Int32 configurationID;  /**< Configuration ID of given MCTNF implementation.
                                 *
                                 *   @remarks   This is used to differentiate
                                 *              multiple images of a vendor.
                                 *              It can be used by the
                                 *              framework to optimize the
                                 *              save/restore overhead of any
                                 *              resources used.
                                 *
                                 *   @remarks   This can be useful in
                                 *              multichannel use case
                                 *              scenarios.
                                 *
                                 */
    XDM1_AlgBufInfo bufInfo;    /**< Input and output buffer information.
                                 *
                                 *   @remarks   This field provides the
                                 *              application with the algorithm's
                                 *              buffer requirements.  The
                                 *              requirements may vary depending
                                 *              on the current configuration
                                 *              of the algorithm instance.
                                 *
                                 *   @sa IVIDNF1_Params
                                 *   @sa XDM1_AlgBufInfo
                                 *   @sa IVIDNF1_Fxns.process()
                                 */
    IVIDNF1_DynamicParams nfDynamicParams;  /**< Video Noise Filter dynamic
                                 *   parameters.
                                 *
                                 *   @remarks   In case of extened dynamic
                                 *              Params, alg can check the
                                 *              size of status or
                                 *              DynamicParams and return
                                 *              the parameters accordingly.
                                 */
    XDAS_Int32 startX;           /**< startX points to the filtered data
                                 *    in the output buffer provided by
                                 *    the application, along the X direction.
                                 *    The application would be providing a
                                 *    buffer bigger in size to the input
                                 *    resolution of the frame. This is
                                 *    to accomodate padding pixels. This
                                 *    will be populated after GETBUF_INFO
                                 *    control call.
                                 */

    XDAS_Int32 startY;           /**< startY points to the filtered data
                                 *    in the output buffer provided by
                                 *    the application, along the Y direction.
                                 *    The application would be providing a
                                 *    buffer bigger in size to the input
                                 *    resolution of the frame. This is
                                 *    to accomodate padding pixels. This
                                 *    will be populated after GETBUF_INFO
                                 *    control call.
                                 */

} IVIDNF1_Status;


/**
 *  @brief      Defines the run time output arguments for all IVIDNF1
 *              instance objects
 *
 *  @extensibleStruct
 *
 *  @sa         IVIDNF1_Fxns::process()
 */
typedef struct IVIDNF1_OutArgs {
    XDAS_Int32 size;            /**< @sizeField */
    XDAS_Int32 extendedError;   /**< @extendedErrorField */
   XDAS_Int32 displayID[IVIDEO2_MAX_IO_BUFFERS]; /**< This is an
                                 *   array of outBufID's corresponding to the
                                 *   buffers that have been unlocked in the
                                 *   current process call.
                                 *
                                 *   @remarks   The buffers released by the
                                 *              algorithm are indicated by
                                 *              their non-zero ID (previously
                                 *              provided via
                                 *              IVIDNF1_InArgs#outBufID).
                                 *
                                 *   @remarks   A value of zero (0) indicates
                                 *              an invalid ID.  The first zero
                                 *              entry in array will indicate
                                 *              end of valid displayIDs within
                                 *              the array.  Hence the
                                 *              application can stop searching
                                 *              the array when it encounters the
                                 *              first zero entry.
                                 *
                                 *   @remarks   If no buffer was filtered in
                                 *              the process call,
                                 *              @c displayID[0] will
                                 *              have a value of zero.
                                 *
                                 *   @sa IVIDNF1_InArgs#outBufID
                                 */
   XDAS_Int32 freeInBufID[IVIDEO2_MAX_IO_BUFFERS]; /**< This is an
                                 *   array of inBufID's corresponding to the
                                 *   buffers that have been unlocked in the
                                 *   current process call.
                                 *
                                 *   @remarks   The buffers released by the
                                 *              algorithm are indicated by
                                 *              their non-zero ID (previously
                                 *              provided via
                                 *              IVIDNF1_InArgs#inBufID).
                                 *
                                 *   @remarks   A value of zero (0) indicates
                                 *              an invalid ID.  The first zero
                                 *              entry in array will indicate
                                 *              end of valid freeInBufID within
                                 *              the array.  Hence the
                                 *              application can stop searching
                                 *              the array when it encounters the
                                 *              first zero entry.
                                 *
                                 *   @remarks   If no buffer was unlocked in
                                 *              the process call,
                                 *              @c freeInBufID[0] will
                                 *              have a value of zero.
                                 *
                                 *   @sa IVIDNF1_InArgs#inBufID
                                 */
   XDAS_Int32 freeOutBufID[IVIDEO2_MAX_IO_BUFFERS]; /**< This is an
                                 *   array of outBufID's corresponding to the
                                 *   buffers that have been unlocked in the
                                 *   current process call.
                                 *
                                 *   @remarks   The buffers released by the
                                 *              algorithm are indicated by
                                 *              their non-zero ID (previously
                                 *              provided via
                                 *              IVIDNF1_InArgs#outBufID).
                                 *
                                 *   @remarks   A value of zero (0) indicates
                                 *              an invalid ID.  The first zero
                                 *              entry in array will indicate
                                 *              end of valid freeOutBufIDs within
                                 *              the array.  Hence the
                                 *              application can stop searching
                                 *              the array when it encounters the
                                 *              first zero entry.
                                 *
                                 *   @remarks   If no buffer was unlocked in
                                 *              the process call,
                                 *              @c freeOutBufID[0] will
                                 *              have a value of zero.
                                 *
                                 *   @sa IVIDNF1_InArgs#outBufID
                                 */
} IVIDNF1_OutArgs;


/**
 *  @brief      Defines the control commands for the IVIDNF1 module
 *
 *  @remarks    This ID can be extended in IMOD interface for
 *              additional controls.
 *
 *  @sa         XDM_CmdId
 *
 *  @sa         IVIDNF1_Fxns::control()
 */
typedef  IALG_Cmd IVIDNF1_Cmd;


/**
 *  @brief      Defines all of the operations on IVIDNF1 objects
 */
typedef struct IVIDNF1_Fxns {
    IALG_Fxns   ialg;             /**< XDAIS algorithm interface.
                                   *
                                   *   @sa      IALG_Fxns
                                   */

/**
 *  @brief      Basic video noise filter call
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in,out] inBufs       Input video buffer descriptors.
 *  @param[in,out] outBufs      Output buffer descriptors.  The algorithm
 *                              may modify the output buffer pointers.
 *  @param[in]  inArgs          Input arguments.  This is a required
 *                              parameter.
 *  @param[out] outArgs         Ouput results.  This is a required parameter.
 *
 *  @remarks    process() is a blocking call.  When process() returns, the
 *              algorithm's processing is complete.
 *
 *  @pre        @c inBufs must not be NULL, and must point to a valid
 *              IVIDEO2_BufDesc structure.
 *
 *  @pre        @c inBufs->numPlanes will indicate the total number of input
 *              buffers supplied for input frame in the @c inBufs->planeDesc[]
 *              array.
 *
 *  @pre        @c inBufs->numMetaPlanes will indicate the total number of input
 *              buffers supplied for meta data planes in the
 *              @c inBufs->metadataPlaneDesc[] array.
 *
 *  @pre        @c outBufs must not be NULL, and must point to a valid
 *              IVIDEO2_BufDesc structure.
 *
 *  @pre        @c outBufs->numPlanes will indicate the total number of output
 *              buffers supplied for output frame in the @c outBufs->planeDesc[]
 *              array.
 *
 *  @pre        @c outBufs->numMetaPlanes will indicate the total number of output
 *              buffers supplied for meta data planes in the
 *              @c outBufs->metadataPlaneDesc[] array.
 *
 *  @pre        @c inArgs must not be NULL, and must point to a valid
 *              IVIDNF1_InArgs structure.
 *
 *  @pre        @c outArgs must not be NULL, and must point to a valid
 *              IVIDNF1_OutArgs structure.
 *
 *  @pre        The buffers in @c inBuf and @c outBuf are physically
 *              contiguous and owned by the calling application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c inArgs.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of
 *              @c inBufs, with the exception of @c inBufs.bufDesc[].accessMask.
 *              That is, the data and buffers pointed to by these parameters
 *              must be treated as read-only.
 *
 *  @post       The algorithm <b>must</b> appropriately set/clear the
 *              IVIDEO2_BufDesc.planeDesc[].accessMask and
 *              IVIDEO2_BufDesc.metadataPlaneDesc[].accessMask fields in
 *              @c inBufs to indicate the mode in which each of the respective
 *              buffers were read.
 *              For example, if the algorithm only read from
 *              @c inBufs.planeDesc[0].buf using the algorithm processor, it
 *              could utilize #XDM_SETACCESSMODE_READ to update the appropriate
 *              @c accessMask fields.
 *              The application <i>may</i> utilize these
 *              returned values to appropriately manage cache.
 *
 *  @post       The buffers in @c inBufs and outBufs are
 *              owned by the calling application.
 *
 *  @retval     #IVIDNF1_EOK           @copydoc IVIDNF1_EOK
 *  @retval     #IVIDNF1_EFAIL         @copydoc IVIDNF1_EFAIL
 *                                      See IVIDNF1_Status.extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     #IVIDNF1_EUNSUPPORTED  @copydoc IVIDNF1_EUNSUPPORTED
 *
 *  @todo       Need to review these comments.  Not sure @c inBufs and
 *              @c outBufs are correctly described.
 */
    XDAS_Int32 (*process)(IVIDNF1_Handle handle, IVIDEO2_BufDesc *inBufs,
            IVIDEO2_BufDesc *outBufs, IVIDNF1_InArgs *inArgs,
            IVIDNF1_OutArgs *outArgs);


/**
 *  @brief      Control behavior of an algorithm
 *
 *  @param[in]  handle          Handle to an algorithm instance.
 *  @param[in]  id              Command id.  See #XDM_CmdId.
 *  @param[in]  params          Dynamic parameters.  This is a required
 *                              parameter.
 *  @param[out] status          Output results.  This is a required parameter.
 *
 *  @pre        @c handle must be a valid algorithm instance handle.
 *
 *  @pre        @c params must not be NULL, and must point to a valid
 *              IVIDNF1_DynamicParams structure.
 *
 *  @pre        @c status must not be NULL, and must point to a valid
 *              IVIDNF1_Status structure.
 *
 *  @pre        If a buffer is provided in the @c status->data field,
 *              it must be physically contiguous and owned by the calling
 *              application.
 *
 *  @post       The algorithm <b>must not</b> modify the contents of @c params.
 *              That is, the data pointed to by this parameter must be
 *              treated as read-only.
 *
 *  @post       If a buffer was provided in the @c status->data field,
 *              it is owned by the calling application.
 *
 *  @retval     #IVIDNF1_EOK           @copydoc IVIDNF1_EOK
 *  @retval     #IVIDNF1_EFAIL         @copydoc IVIDNF1_EFAIL
 *                                      See IVIDNF1_Status.extendedError
 *                                      for more detailed further error
 *                                      conditions.
 *  @retval     #IVIDNF1_EUNSUPPORTED  @copydoc IVIDNF1_EUNSUPPORTED
 */
    XDAS_Int32 (*control)(IVIDNF1_Handle handle, IVIDNF1_Cmd id,
            IVIDNF1_DynamicParams *params, IVIDNF1_Status *status);

} IVIDNF1_Fxns;


/*@}*/

#ifdef __cplusplus
}
#endif

#endif


