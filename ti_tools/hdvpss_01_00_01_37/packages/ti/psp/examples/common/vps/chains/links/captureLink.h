/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
    \defgroup VPSEXAMPLE_CAPTURE_LINK_API Capture Link API

    Capture Link can be used to instantiate capture upto
    VPS_CAPT_INST_MAX VIP instances.

    Each instance can have upto two outputs.

    The frames from these capture outputs can be put in upto
    four output queue.

    Each output queue can inturn to be connected to a link like
    display or DEI or NSF.

    @{
*/

/**
    \file captureLink.h
    \brief Capture Link API
*/

#ifndef _CAPTURE_H_
#define _CAPTURE_H_

#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/vps/vps_capture.h>
#include <ti/psp/devices/vps_videoDecoder.h>

/** \brief Max outputs per VIP instance */
#define CAPTURE_LINK_MAX_OUTPUT_PER_INST  (2)

/** \brief Max output queues in the capture link */
#define CAPTURE_LINK_MAX_OUT_QUE          (4)

/** \brief Max Channels per output queue */
#define CAPTURE_LINK_MAX_CH_PER_OUT_QUE   (16)


/**
    \brief Link CMD: Detect capture video source format

    This command can make the capture link wait until video
    source is detect on all the expected input video sources

    \param UInt32 timeout  [IN] BIOS_WAIT_FOREVER or BIOS_NO_WAIT
*/
#define CAPTURE_LINK_CMD_DETECT_VIDEO    (0x1000)

/** \brief Command to detect overflow and force reset */
#define CAPTURE_LINK_CMD_FORCE_RESET     (0x1001)

/** \brief Command to print advanced statistics */
#define CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS   (0x1002)

/** \brief Command to change brightness by provided value */
#define CAPTURE_LINK_CMD_CHANGE_BRIGHTNESS      (0x1003)

/** \brief Command to change contrast by provided value */
#define CAPTURE_LINK_CMD_CHANGE_CONTRAST        (0x1004)

/** \brief Command to change saturation by provided value */
#define CAPTURE_LINK_CMD_CHANGE_SATURATION      (0x1005)

/** \brief Command to halt execution */
#define CAPTURE_LINK_CMD_HALT_EXECUTION         (0x1006)

/**
    \brief Capture output parameters
*/
typedef struct
{
    FVID2_DataFormat    dataFormat;
    /**< output data format, YUV422, YUV420, RGB */

    Bool                scEnable;
    /**< TRUE: enable scaling, FALSE: disable scaling */

    UInt32              scOutWidth;
    /**< Scaler output width */

    UInt32              scOutHeight;
    /**< Scaler output height */

    UInt32              outQueId;
    /**< Link output que ID to which this VIP instance output frames are put */

    UInt32              subFrameEnable;
    /**< Enable sub frame based capture */

} CaptureLink_OutParams;

/**
    \brief VIP instance information
*/
typedef struct
{
    UInt32                        vipInstId;
    /**< VIP capture driver instance ID */

    UInt32                        videoDecoderId;
    /**< Video decoder instance ID */

    FVID2_DataFormat              inDataFormat;
    /**< Input source data format, RGB or YUV422 */

    FVID2_Standard                standard;
    /**< Required video standard */

    UInt32                        numOutput;
    /**< Number of outputs */

    CaptureLink_OutParams         outParams[CAPTURE_LINK_MAX_OUTPUT_PER_INST];
    /**< Information about each output */

    Bool                         frameCaptureMode;
    /**< To determine if field based or frame based capture mode is requried.
         This is true for only interlaced captures. */
    Bool                          fieldsMerged;
    /**< This is to determine whether fields will be merged or separate in case
         frame capture is enabled above. */
} CaptureLink_VipInstParams;

/**
    \brief Capture Link create parameters
*/
typedef struct
{
    UInt16                    numVipInst;
    /**< Number of VIP instances in this link */

    CaptureLink_VipInstParams vipInst[VPS_CAPT_INST_MAX];
    /**< VIP instance information */

    UInt32                    numOutQue;
    /**< Since capture link can have 2 output queues, incoming channels will
     * be splitted in half automatically if user enables both the queues.
     * Channels {0 to (incomingChannels/2 - 1)} will goto output queue 0 and
     * channels {incomingChannels/2 to incomingChannels} will goto output queue 1.
     * If only 1 output queue is enabled, incoming channels will not be
     * splitted and sent only to output queue 0.
     * For e.g.:
     * Incoming channels = 16, numOutQue = 1 -> outQueue0 = 16, outQueue1 = 0
     * Incoming channels = 16, numOutQue = 2 -> outQueue0 = 8, outQueue1 = 8
     * Incoming channels = 8, numOutQue = 1 -> outQueue0 = 8, outQueue1 = 0
     * Incoming channels = 8, numOutQue = 2 -> outQueue0 = 4, outQueue1 = 4
     */

    System_LinkOutQueParams   outQueParams[CAPTURE_LINK_MAX_OUT_QUE];
    /**< Output queue information */

    Bool                      tilerEnable;
    /**< Enable/disable tiler */

    Bool captureScSwitchEnable;
    /**< Enables switching of output sizes for scaler. */
} CaptureLink_CreateParams;

/**
    \brief Capture link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 CaptureLink_init();

/**
    \brief Capture link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 CaptureLink_deInit();


#endif

/*@}*/
