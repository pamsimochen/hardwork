/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vpdmaMosaic.h
 *
 *  \brief Utility functions that would be used to divide windows into
 *         sub-windows as required by VPDMA.
 *
 */

#ifndef _VPSHAL_VPDMAMOSAIC_H
#define _VPSHAL_VPDMAMOSAIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/vps.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_VpdmaMosaicWinFmt
 *  \brief Parameters that define a processed sub-window. For each window
 *         following parameters are updated/modified/consumed
 */
typedef struct
{
    UInt32                      startX;
    /**< [OUT]  Updated with horizontal start. */
    UInt32                      bufferOffsetX;
    /**< [OUT] Specifies the relative offset in terms of columns from the
               original window's buffers address for horizontal axis i.e. startX
               Could be used to arrive at the startX for the sub-window */
    UInt32                      startY;
    /**< [OUT]  Updated with vertical start. */
    UInt32                      bufferOffsetY;
    /**< [OUT] Specifies the relative offset in terms of rows from the original
               window's buffers address for vertical axis i.e. startY.
               Could be used to arrive at the startY for the sub-window */
    UInt32                      width;
    /**< [OUT]  Updated with width in pixels */
    UInt32                      height;
    /**< [OUT]  Updated with number of lines. */
    UInt32                      isActualChanReq;
    /**< [OUT]  Flag to indicate if actual VPDMA channel to be used or a FREE
                channel is required */
    UInt32                      freeChanIndex;
    /**< [OUT]  In cases where free channels is required, an relative positive
                integer starting with 0x0 */
    const Vps_WinFormat        *ipWindowFmt;
    /**< [OUT]  Pointer to window format specified by the applications -
                could be used to extract pitch, dataFormats etc... */
    UInt32                      frameIndex;
    /**< [OUT]  Specifies the index at which the windows was described by the
                application.
                i.e. offset for Vps_MultiWinParams.winFmt array at which the
                window was described prior to division */
    UInt32                      priority;
    /**< [OUT]  Derived from the application specified window definition.
                Used by the HAL to determine which part of the overlying windows
                to be displayed. Smaller priority windows will be displayed */
    UInt32                      subWindowInst;
    /**< [Reserved] Used for debug */
    UInt32                      isLastWin;
}VpsHal_VpdmaMosaicWinFmt;


/**
 *  struct VpsHal_VpdmaMosaicSplitWinParam
 *  \brief Parameters that define a width of the frames and maximum sub-windows
 *         that could be used.
 *         Updates the with details of split windows along with free channels
 *         required, among others
 */

typedef struct
{
    Int32                       numFreeChans;
    /**< [OUT]    Will be updated with the maximum number of free channels
                  required. This number includes the dummy channel required in
                  certain cases.
                  e.g. If for a given layout dummy channel is NOT required then
                  assuming that we would require numFreeChans = 5
                  In and similar layout where an dummy channel IS required then
                  the numFreeChans would be 6 */
    Int32                       numSplitWindows;
    /**< [OUT]    Provides the total number of windows after splitting.
                  Could be used as upper limit while accessing  splitWindows.*/
    Int32                       numWindowsFirstRow;
    /**< [OUT]    Specifies the number of windows on the first row. First row
                  means the least startY values of all the windows. */
    VpsHal_VpdmaMosaicWinFmt    *splitWindows;
    /**< [IN/OUT] Pointer to array of type VpsHal_VpdmaMosaicWinFmt. Where each
                  instance specify the windows parameters. Space for the array
                  and structures to be provided by the caller. */
    UInt32                      isDummyNeeded;
    /** [OUT]     Specifies if an dummy channel would be required as window size
                  is less than that of the frame boundary. */
    UInt32                      frameHeight;
    /**< [IN]     Specifies the frame height. A frame would contain multiple
                  windows */
    UInt32                      frameWidth;
    /**< [IN]     Specifies the frame width. A frame would contain multiple
                  windows */
    UInt32                      maxNumSplitWindows;
    /**< [IN]     Specifies the maximum number of split windows that can be
                  handled by the caller.
                  Implies that space pointed by splitWindows is allocated for
                  maxNumSplitWindows entries. */
    UInt32                      getNoWinsPerRow;
    /**< [IN]     Specifies, if you require to know the number of windows that
                  are present on each row. If set to TRUE, numWindowsPerRow
                  should not be NULL */
    Int32                       *numWindowsPerRow;
    /**< [OUT]    Specifies the number of windows on each row. This variable
                  is an pointer to array of size VPS_CFG_MAX_MULTI_WIN */
    Int32                       numRows;
    /**< [OUT]    Specifies the number of rows that have resulted after
                  splitting up of windows
                  -1 in case of errors */
}VpsHal_VpdmaMosaicSplitWinParam;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_vpdmaInitMosaic
 *  \brief  Initialize mosaic switch feature.
 *          Initializes locals and lists required for mosaic switch.
 *
 *  \param  None
 *
 *  \return Returns 0 on success else returns error value.
 */
Int32 VpsHal_vpdmaInitMosaic(void);


/**
 *  VpsHal_vpdmaDeInitMosaic
 *  \brief  De Initialize mosaic switch feature, Releases all acquired
 *          resources.
 *
 *  \param  None
 *
 *  \return Returns 0 on success else returns error value.
 */
Int32 VpsHal_vpdmaDeInitMosaic(void);


/**
 *  VpsHal_vpdmaSetupMosaicCfg
 *  \brief This function determines the sequence to program the VPDMA for mosaic
 *         display.
 *
 *         In cases where there is no video data between windows in a mosaic
 *         the VPDMA requires to be programmed in particular sequence.
 *
 *  \param multiWindows     [IN]        Application supplied window config.
 *  \param splitWinParam    [IN/OUT]    Caller provides a valid pointer and
 *                                      space required for other members.
 *                                      This function would determine if windows
 *                                      requires to be re-ordered and provide
 *                                      order of windows as expected by VPDMA.
 *                                      The number of windows could increase as
 *                                      a result.
 *  \return             Returns 0 on success else returns error value.
 */

Int32   VpsHal_vpdmaSetupMosaicCfg(
            Vps_MultiWinParams              *multiWindows,
            VpsHal_VpdmaMosaicSplitWinParam *splitWinParam);

#ifdef __cplusplus
}
#endif

#endif /* _VPSHAL_VPDMAMOSAIC_H */
