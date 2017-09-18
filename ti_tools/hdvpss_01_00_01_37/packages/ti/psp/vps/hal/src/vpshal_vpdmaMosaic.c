/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vpdmaMosaic.c
 *
 *  \brief Utility functions that would be used to divide windows into
 *         sub-windows as required by VPDMA.
 *
 */

/**
 *  Algorithm
 *
 *  Case 1  Windows overlap on the vertical plane - Pip mode ignored
 *  +-------------------------------+
 *  +                  +---------+  +
 *  +    +---------+   +         +  +
 *  +    +         +   +  Win 2  +  +
 *  +    +   Win 1 +   +         +  +
 *  +    +         +   +---------+  +
 *  +    +---------+                +
 *  +                               +
 *  +                               +
 *  +-------------------------------+
 *  1. Ignore the X value and Build a list of Y co-ordinates, essentially
 *      each would result in 2 nodes for each window, with 1 node for top and
 *      1 node for bottom
 *  2. Using this list break Win 1
 *  3. Using this list break Win 2
 *  4. Order the windows from Right to left - An ordered insertion is preformed
 *     where the order value = Y * Big Constant + X
 *
 *  Case 2  Pip
 *  +-------------------------------+
 *  ++-----------------------------++
 *  ++    +---------+              ++
 *  ++    +         +     Win 1    ++
 *  ++    +   Win 2 +              ++
 *  ++    +         +              ++
 *  ++    +---------+              ++
 *  ++-----------------------------++
 *  +                               +
 *  +-------------------------------+
 *  1. Perform steps in case 1
 *  2. Ignore the Y co-ordinates of split windows and generate a splitter list
 *     list based on X co-ordinate values.
 *  3. Using this list break Win 1 and Win 2
 *  4. Order the windows from Right to left - An ordered insertion is preformed
 *     where the order value = Y * Big Constant + X
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdmaMosaic.h>

/* ========================================================================== */
/*                              Local Configurations                          */
/* ========================================================================== */

#define VPS_MAXIMUM_MULTIPLE_WINDOWS        (VPS_CFG_MAX_MULTI_WIN * 4u * 2u)
/**< Defines the maximum number of windows that could be supported at anytime.
     4u - Each window will use 2 nodes (left, right) for X splitter list and
          2 more nodes (top, bottom) for Y splitter list.
     2u - For worst case splitting of windows. */
#define VPS_MAXIMUM_WINDOWS_SUPPORTED       (VPS_CFG_MAX_MULTI_WIN * 2u)
/**< Defines the maximum number of sub-windows - after breaking up overlapping
     windows
     2u - For worst case splitting of windows. */

/* ========================================================================== */
/*                              Local Defines                                 */
/* ========================================================================== */
#define VPS_MOSAIC_WINDOW_NODE_FREE         (0x01u)
/**< Value used to mark node as free */
#define VPS_MOSAIC_WINDOW_NODE_ALLOCATED    (0x02u)
/**< Value used to mark node as allocated */
#define VPS_MOSAIC_ON_X_AXIS                (0x01u)
/**< Split up windows on X axis - Ignore Y axis */
#define VPS_MOSAIC_ON_Y_AXIS                (0x02u)
/**< Split up windows on Y axis - Ignore X axis */
#define VPS_MOSAIC_SHIFT_Y_BY_11            (0x0Bu)
/**< Used to convert X, Y co-ordinate values to a single plane value */
#define VPS_MOSAIC_SHIFT_X_BY_11            (0x0Bu)
/**< Used to convert X, Y co-ordinate values to a single plane value */
#define VPS_VALUE_0                         (0x0)
/**< Indicates value 0x0 */
#define VPS_VALUE_1                         (0x1u)
/**< Indicates value 0x0 */
#define VPS_MOSAIC_WINDOW_ORDER_SEED        (0x64u)
/**< Seed value used to order windows - higher the value, bigger the window */
#define mosaicTrace                         (VPS_ENABLE_TRACE_HAL_MOSAIC_SWITCH)
/**< Indirection for trace macro - reduce the length of define */
#if ((mosaicTrace & GT_INFO) == GT_INFO)
/**< Trace enable macro */
#define VPS_MOSAIC_ENABLE_TRACE
/**< Controls the prints of split windows */
#endif


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/**
 *  struct splitterNodeDef
 *  \brief Structure that defines a node in the splitter list.
 */
typedef struct
{
    UInt32          value;
    /**< Nodes co-ordinates, Y in case of non-PIP and X in case of PIP */
    UInt32          row;
    /**< Y value in case PIP mode - We will be interested to split windows that
         are on the same row when trying to split on X axis */
    UInt32          priority;
    /**< Priority of the corresponding window. */
    VpsUtils_Node   nodesPlaceHolder;
    /**< Required by utils functions that implements lists */
}splitterNodeDef;


/**
 *  struct splitWindowsNodeDef
 *  \brief Structure that defines a node, which describe an split window .
 */
typedef struct
{
    VpsHal_VpdmaMosaicWinFmt    window;
    /**< Place holder for processed window */
    VpsUtils_Node               nodesPlaceHolder;
    /**< Place holder for node - required by linked list */
}splitWindowsNodeDef;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static splitterNodeDef      *allocateSplitterNode(void);
static splitWindowsNodeDef  *allocateSplitWindowNode(void);
static Int32    determineDummyChanReq(
                    Vps_MultiWinParams              *multiWindows,
                    VpsHal_VpdmaMosaicSplitWinParam *splitWinParam);
static Int32    makeYAxisSplitterList(
                    const Vps_WinFormat             *winFmt,
                    VpsHal_VpdmaMosaicSplitWinParam *winParam,
                    UInt32                          numWindows,
                    VpsUtils_Handle                 yAxisListHndl,
                    UInt32                          *changeInPri);
static  Int32   makeXAxisSplitterList(
                    VpsUtils_Handle                 splitWinListHndl,
                    VpsUtils_Handle                 splitterListHndl);
static Int32    splitWindowsOnYAxis(
                    const Vps_WinFormat             *winFmt,
                    UInt32                          numWindows,
                    VpsUtils_Handle                 splitterHndl,
                    VpsUtils_Handle                 splitWindowListHndl);
static Int32    splitWindowsOnXAxis(
                    VpsUtils_Handle                 splitterHndl,
                    VpsUtils_Handle                 winListHndl,
                    VpsUtils_Handle                 splitWinListHndl);
static Int32    removeOverlappingWindow(VpsUtils_Handle winListHndl);
static Int32    determineWindowOrder(
                    VpsUtils_Handle                 splitWinListHndl,
                    VpsHal_VpdmaMosaicSplitWinParam *splitWinParam);
#ifdef VPS_MOSAIC_ENABLE_TRACE
static void     printSplitterList(VpsUtils_Handle splitterHndl,UInt32 rowAndClmn);
static void     printSplitWindows(VpsUtils_Handle winListHndl,UInt32 otherParams);
static void     printWindowsSplitParams(VpsHal_VpdmaMosaicSplitWinParam *param);
#endif /* VPS_MOSAIC_ENABLE_TRACE */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static VpsUtils_Handle      gYAxisSplitterHndl;
/**< Handle to splitter list that defines co-ordinates that should be used
    to split all windows */
static VpsUtils_Handle      gXAxisSplitterHndl;
/**< Handle to splitter list that defines co-ordinates that should be used
    to split all windows */
static splitterNodeDef      gSpaceForSplitterList[VPS_MAXIMUM_MULTIPLE_WINDOWS];
/**< Place holder for splitter list nodes */
static UInt32               gSplitterNodesFreeIndex;
/**< Defines the free splitter nodes */
static splitWindowsNodeDef  gSpaceForSplitWindow[VPS_MAXIMUM_WINDOWS_SUPPORTED];
/**< Space to hold window definitions after splitting */
static UInt32               gSplitWindowsFreeIndex;
/**< Defines the free window nodes - to used to split window */
static VpsUtils_Handle      gYAxisSplitWinListHndl;
/**< Master list that holds split windows in NON-PIP modes, in case of PIP
     mode, this list holds list of windows split on Y axis*/
static VpsUtils_Handle      gXAxisSplitWinListHndl;
/**< Master list that holds split windows in PIP mode */
static Semaphore_Handle     gGuard;
/**< A guard to ensure exclusive access to data structures and globals */
static UInt32               isInitialized   =   FALSE;
/**< Variable to track the state of utility */

/* ========================================================================== */
/*                          Local Functions                                   */
/* ========================================================================== */
/**
 *  allocateSplitterNode
 *  \brief Splitter Lists node allocator.
 *
 *  Allocates a splitter node from the pool of nodes. Note that there would NO
 *  de-allocator node function, the user should ensure gSplitterNodesFreeIndex
 *  should be set to 0x0, before calling for the very first time.
 *
 *  Where required to de-allocate ALL allocated nodes, gSplitterNodesFreeIndex
 *  should be reset to 0x0
 *
 *  Expected to be called by a single TASK at a time. i.e. NO exclusive access
 *  techniques used - to minimize the overheads
 *
 *  \param arg          None.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static splitterNodeDef *allocateSplitterNode(void)
{
    splitterNodeDef   *rtnValue   = NULL;

    if (gSplitterNodesFreeIndex < VPS_MAXIMUM_MULTIPLE_WINDOWS)
    {
        rtnValue = &gSpaceForSplitterList[gSplitterNodesFreeIndex];
        gSplitterNodesFreeIndex++;
    }
    return (rtnValue);
}


/**
 *  allocateSplitWindowNode
 *  \brief Allocates a node that would be used to describe a split window.
 *
 *  Allocates a node from the pool of nodes. Note that there would NO
 *  de-allocator node function, the user should ensure gSplitWindowsFreeIndex
 *  should be set to 0x0, before calling for the very first time.
 *
 *  Where required to de-allocate ALL allocated nodes, gSplitWindowsFreeIndex
 *  should be reset to 0x0
 *
 *  Expected to be called by a single TASK at a time. i.e. NO exclusive access
 *  techniques used - to minimize the overheads
 *
 *  \param arg          None.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static splitWindowsNodeDef *allocateSplitWindowNode(void)
{
    splitWindowsNodeDef *rtnValue   =   NULL;

    if(gSplitWindowsFreeIndex < VPS_MAXIMUM_WINDOWS_SUPPORTED)
    {
        rtnValue = &gSpaceForSplitWindow[gSplitWindowsFreeIndex];
        rtnValue->window.isActualChanReq    =   FALSE;
        gSplitWindowsFreeIndex++;
    }

    return (rtnValue);
}


/**
 *  determineDummyChanReq
 *  \brief Checks if a dummy channel is required.
 *
 *  Expected to be called when the no of windows is just 1
 *
 *  \param multiWindows     [IN]    Application supplied window config.
 *  \param splitWinParam    [OUT]   Updated with Windows details and others
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32    determineDummyChanReq(
                    Vps_MultiWinParams              *multiWindows,
                    VpsHal_VpdmaMosaicSplitWinParam *splitWinParam)
{
    const Vps_WinFormat     *winFmt     =   NULL;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>determineDummyChanReq");
    winFmt =   multiWindows->winFmt;
    GT_assert(mosaicTrace, (winFmt != NULL));
    splitWinParam->numFreeChans         =   VPS_VALUE_0;
    splitWinParam->numSplitWindows      =   VPS_VALUE_1;
    splitWinParam->numWindowsFirstRow   =   VPS_VALUE_1;
    splitWinParam->numRows              =   VPS_VALUE_1;
    if (splitWinParam->getNoWinsPerRow != 0x0)
    {
        splitWinParam->numWindowsPerRow[0x0]   =   VPS_VALUE_1;
    }
    /* Check if application has provided enough space to accommodate this
       window definition */
    if (splitWinParam->maxNumSplitWindows < VPS_VALUE_1)
    {
        /* Out of space - let the caller know */
        return(VPS_ENO_MORE_BUFFERS);
    }
    /* Build the first window */
    splitWinParam->splitWindows[0x0].startX   =   winFmt[0x0].winStartX;
    splitWinParam->splitWindows[0x0].startY   =   winFmt[0x0].winStartY;
    splitWinParam->splitWindows[0x0].width    =   winFmt[0x0].winWidth;
    splitWinParam->splitWindows[0x0].height   =   winFmt[0x0].winHeight;
    splitWinParam->splitWindows[0x0].bufferOffsetY    = 0x0;
    splitWinParam->splitWindows[0x0].bufferOffsetX    = 0x0;
    splitWinParam->splitWindows[0x0].isActualChanReq  = TRUE;
    splitWinParam->splitWindows[0x0].ipWindowFmt      = &(winFmt[0x0]);
    splitWinParam->splitWindows[0x0].frameIndex       = 0x0;

    /* Just a single window, look for requirement of a dummy channel */
    if (((winFmt[0x0].winStartX + winFmt[0x0].winWidth) <
                                                    splitWinParam->frameWidth)
        ||
        ((winFmt[0x0].winStartY + winFmt[0x0].winHeight) <
                                                    splitWinParam->frameHeight))
    {
        /* If we require to count the dummy channel as an window
           increment the numSplitWindows here */
        /* If we require to count the dummy channel as an free channel
           add the numFreeChans assignment here */
        /* If an entry is required for dummy channel in the split windows
           list add the entry here */
        splitWinParam->isDummyNeeded    =   TRUE;
        GT_0trace(mosaicTrace, GT_INFO, "Dummy channel required");
    }
    else
    {
        splitWinParam->isDummyNeeded    =   FALSE;
    }
    splitWinParam->splitWindows[0x0].isLastWin = TRUE;

    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<determineDummyChanReq");
    return(VPS_SOK);
}


/**
 *  makeYAxisSplitterList
 *  \brief Generates a ordered list (ascending), whose nodes describe the
 *         value of Y coordinates.
 *
 *  \param winFmt           [IN] Pointer to a array of pointers of type
 *                               Vps_WinFormat, with numWindows of
 *                               entries.
 *  \param numWindows       [IN] Specifies the number of instance described by
 *                               winFmt.
 *  \param yAxisListHndl    [IN] Handle to the list, to place the nodes.
 *  \param changeInPri      [OUT]Flag to indicate change in priority of windows
 *                               could be used to determine PIP mode.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32    makeYAxisSplitterList(
                    const Vps_WinFormat             *winFmt,
                    VpsHal_VpdmaMosaicSplitWinParam *winParam,
                    UInt32                          numWindows,
                    VpsUtils_Handle                 yAxisListHndl,
                    UInt32                          *changeInPri)
{
    Int32           rtnValue    =   VPS_SOK;
    splitterNodeDef *topYNode   =   NULL;
    splitterNodeDef *botYNode   =   NULL;
    UInt32          index, winPri;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>makeYAxisSplitterList");

    GT_assert(mosaicTrace, (winFmt != NULL));
    GT_assert(mosaicTrace, (winParam != NULL));
    GT_assert(mosaicTrace, (changeInPri != NULL));
    *changeInPri    =   FALSE;
    winPri          =   winFmt[VPS_VALUE_0].priority;
    for(index = 0x0; index < numWindows; index++)
    {
        if (winPri != winFmt[index].priority)
        {
            *changeInPri    =   TRUE;
        }

        /* Ensure that sub-window is with in the frame boundary */
        if (((winFmt[index].winStartX + winFmt[index].winWidth) >
                                                        winParam->frameWidth) ||
            ((winFmt[index].winStartY + winFmt[index].winHeight) >
                                                        winParam->frameHeight))
        {
            /* Window exceeds frames size */
            rtnValue    = VPS_EINVALID_PARAMS;
            GT_1trace(mosaicTrace, GT_ERR, "Window %d size > frame size",index);
            break;
        }
        /* Ensure that sub-window starts at even boundary */
        if (winFmt[index].winStartX & 0x01u)
        {
            /* Window should start from even offset */
            rtnValue = VPS_EINVALID_PARAMS;
            GT_1trace(mosaicTrace, GT_ERR,
                "Window %d should start at even offset\n", index);
            break;
        }
        GT_assert(mosaicTrace, (yAxisListHndl != NULL));
        topYNode    =   allocateSplitterNode();
        botYNode    =   allocateSplitterNode();
        if ((topYNode == NULL) || (botYNode == NULL))
        {
            rtnValue    = VPS_EALLOC;
            /* Ensure to re-set the allocated index.
               variable gSplitterNodesFreeIndex is accessed in a single
               context no protection required at this point
               No need to free the node, the gSplitterNodesFreeIndex will
               determined used/un-used nodes */
            gSplitterNodesFreeIndex   = VPS_VALUE_0;
            GT_0trace(mosaicTrace, GT_ERR, "Could not allocate y splitter");
            break;
        }
        /* Ignore X value, use Y. Add them in ascending order */
        /* Top Y Value */
        topYNode->value =   winFmt[index].winStartY;
        /* Bottom Y value */
        botYNode->value =   winFmt[index].winStartY + winFmt[index].winHeight;
        topYNode->priority = winFmt[index].priority;
        botYNode->priority = winFmt[index].priority;
        topYNode->nodesPlaceHolder.data =
            &topYNode->value;
        VpsUtils_linkUniqePriNode(
            yAxisListHndl,
            &(topYNode->nodesPlaceHolder),
            topYNode->value);               /* Priority */

        botYNode->nodesPlaceHolder.data =
            &botYNode->value;
        VpsUtils_linkUniqePriNode(
            yAxisListHndl,
            &(botYNode->nodesPlaceHolder),
            botYNode->value);               /* Priority */
    }
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<makeYAxisSplitterList");
    return (rtnValue);
}


/**
 *  makeXAxisSplitterList
 *  \brief Generates a ordered list (ascending), whose nodes describe the
 *         value of X coordinates. Expects the window list as double linked list
 *
 *  \param splitWinListHndl    [IN]     Handle to the list, whose nodes describe
 *                                      window co-ordinates.
 *  \param splitterListHndl    [OUT]    Handle to the list, to place the nodes.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static  Int32   makeXAxisSplitterList(
                    VpsUtils_Handle     splitWinListHndl,
                    VpsUtils_Handle     splitterListHndl)
{
    Int32                       rtnValue    =   VPS_SOK;
    splitterNodeDef             *leftXNode  =   NULL;
    splitterNodeDef             *rightXNode =   NULL;
    VpsUtils_Node               *winNode    =   NULL;
    VpsHal_VpdmaMosaicWinFmt    *window     =   NULL;
    UInt32                      nodePri;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>makeXAxisSplitterList");
    GT_assert(mosaicTrace, (splitWinListHndl != NULL));
    GT_assert(mosaicTrace, (splitterListHndl != NULL));

    winNode =   VpsUtils_getHeadNode(splitWinListHndl);
    while(winNode != NULL)
    {
        window = (VpsHal_VpdmaMosaicWinFmt*)winNode->data;
        GT_assert(mosaicTrace, (window != NULL));
        leftXNode   =   allocateSplitterNode();
        rightXNode  =   allocateSplitterNode();
        if ((leftXNode == NULL) || (rightXNode == NULL))
        {
            rtnValue    = VPS_EALLOC;
            /* Ensure to re-set the allocated index.
               variable gSplitterNodesFreeIndex is accessed in a single
               context no protection required at this point
               No need to free the node, the gSplitterNodesFreeIndex will
               determined used/un-used nodes */
            gSplitterNodesFreeIndex   = VPS_VALUE_0;
            break;
        }
        /* Use X. Add them in ascending order */
        /* Top Row i.e. top X Value */
        leftXNode->value    =   window->startX;
        /* Last Row i.e. bottom X Value */
        rightXNode->value   =   window->startX + window->width;

        /* We would be interested in the row on which splitter lies. This
           would be used to determine if a window requires to be split on
           X axis. */
        /* Further we just require to know the starting Y value as the
           windows are expected to split in Y axis first and then on X axis
           in case of PIP mode */
        leftXNode->row  =   window->startY;
        rightXNode->row =   window->startY;

        leftXNode->priority = window->priority;
        rightXNode->priority = window->priority;

        /* Y * constant + X Ordering with primary index as row and secondary
           index as column */
        nodePri =   (((leftXNode->value+0x01u) << VPS_MOSAIC_SHIFT_X_BY_11)
                        + leftXNode->row);
        leftXNode->nodesPlaceHolder.data = leftXNode;

        VpsUtils_linkUniqePriNode(
            splitterListHndl,
            &(leftXNode->nodesPlaceHolder),
            nodePri);                        /* Priority */

        nodePri =   (((rightXNode->value+0x01u) <<VPS_MOSAIC_SHIFT_X_BY_11)
                      + rightXNode->row);
        rightXNode->nodesPlaceHolder.data = rightXNode;
        VpsUtils_linkUniqePriNode(
            splitterListHndl,
            &(rightXNode->nodesPlaceHolder),
            nodePri);                         /* Priority */
        /* Next window please */
        winNode =   winNode->next;
    }
    GT_0trace(mosaicTrace, GT_LEAVE, ">>>>makeXAxisSplitterList");
    return (rtnValue);
}

/**
 *  splitWindowsOnYAxis
 *  \brief Splits the overlapping windows into individual windows. Overlaps
 *         should be inferred on following conditions.
 *         In two adjacent windows, vertical Boundary of window one is not
 *         followed by vertical boundary of the second window.
 *
 *  \param winFmt       [IN] Pointer to a array of pointers of type
 *                           Vps_WinFormat, with numWindows of entries.
 *  \param numWindows   [IN] Specifies number of pointer described by winFmt.
 *  \param splitterHndl [IN] Handle to the list whose nodes define the overlap
 *                           points
 *  \param splitWindowListHndl [IN/OUT] Handle to an empty list. On successful
 *                           completion of this function, the nodes of this list
 *                           describe individual windows that do not overlap on
 *                           Y axis.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32    splitWindowsOnYAxis(
                    const Vps_WinFormat             *winFmt,
                    UInt32                          numWindows,
                    VpsUtils_Handle                 splitterHndl,
                    VpsUtils_Handle                 splitWindowListHndl)
{
    Int32                   rtnValue    =   VPS_SOK;
    UInt32                  index, windowYValue, windowHeight, winPri;
    UInt32                  splitNodeData;
    VpsUtils_Node           *splitterNode;
    splitWindowsNodeDef     *subWindow    =   NULL;
#ifdef VPS_MOSAIC_ENABLE_TRACE
    UInt32  subWindowInst = 1;
#endif /* VPS_MOSAIC_ENABLE_TRACE */

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>splitWindowsOnYAxis");
    GT_assert(mosaicTrace, (winFmt != NULL));
    GT_assert(mosaicTrace, (splitterHndl != NULL));
    GT_assert(mosaicTrace, (splitWindowListHndl != NULL));

    for(index = 0x0; ((index < numWindows) && (rtnValue == VPS_SOK)); index++)
    {
#ifdef VPS_MOSAIC_ENABLE_TRACE
        subWindowInst = 1;
#endif /* VPS_MOSAIC_ENABLE_TRACE */

        splitterNode = VpsUtils_getHeadNode(splitterHndl);

        if (splitterNode == NULL)
        {
            /* Splitter list is empty - error */
            rtnValue    =   VPS_EFAIL;
            break;
        }
        windowYValue    = winFmt[index].winStartY;
        windowHeight    = winFmt[index].winHeight;
        splitNodeData   = *((UInt32 *)(splitterNode->data));
        /* Go upto beginning of window definition in splitter list */
        while(splitNodeData < windowYValue)
        {
            splitterNode = splitterNode->next;
            if (splitterNode == NULL)
            {
                /* Done with all nodes */
                break;
            }
            splitNodeData   = *((UInt32 *)(splitterNode->data));
        }
        if (splitterNode != NULL)
        {
            /* The first split entry at this position should be caused by this
               window under question - check that and advance */
            if (splitNodeData == windowYValue)
            {
                splitterNode = splitterNode->next;
                if (splitterNode == NULL)
                {
                    /* Error condition there should be an end marker node of
                       this window at least */
                    rtnValue    =   VPS_EFAIL;
                    break;
                }
                while (splitterNode != NULL)
                {
                    splitNodeData   = *((UInt32 *)(splitterNode->data));

                    if (splitNodeData < (windowHeight + windowYValue))
                    {
                         subWindow = allocateSplitWindowNode();
                         if (subWindow != NULL)
                         {
#ifdef VPS_MOSAIC_ENABLE_TRACE
                            subWindow->window.subWindowInst = subWindowInst++;
#endif /* VPS_MOSAIC_ENABLE_TRACE */
                            /* Offset = new sub window Y - Y of original
                                                                window */
                            subWindow->window.bufferOffsetY =
                                    (windowYValue - winFmt[index].winStartY);
                            subWindow->window.bufferOffsetX = VPS_VALUE_0;

                            subWindow->window.frameIndex    = index;
                            subWindow->window.startY        = windowYValue;
                            subWindow->window.height        =
                                           splitNodeData - windowYValue;

                            windowYValue            = subWindow->window.startY +
                                                      subWindow->window.height;

                            windowHeight            = windowHeight      -
                                                      subWindow->window.height;
                            /* Other parameters remain the same */
                            subWindow->window.startX = winFmt[index].winStartX;
                            subWindow->window.width  = winFmt[index].winWidth;
                            subWindow->window.ipWindowFmt = &(winFmt[index]);
                            subWindow->window.priority = winFmt[index].priority;

                            /* Insert the sub-window node in ordered list
                               Y * 0x4000 + X */
                            winPri    =   ((subWindow->window.startY + 0x01) <<
                                             VPS_MOSAIC_SHIFT_Y_BY_11)  -
                                             subWindow->window.startX;
                            subWindow->nodesPlaceHolder.data =
                                &(subWindow->window);
                            VpsUtils_linkNodePri(
                                splitWindowListHndl,
                                &(subWindow->nodesPlaceHolder),
                                winPri);
                            /* Move on to next split */
                            splitterNode = splitterNode->next;
                         }
                         else
                         {
                            /* Error-exceeded the maximum numberof sub-windows*/
                            rtnValue    =   VPS_EALLOC;
                            break;
                         }
                    }
                    else if (splitNodeData == (windowHeight + windowYValue))
                    {
                        /* Last sub-window of this window under division */
                        subWindow = allocateSplitWindowNode();
                        if (subWindow != NULL)
                        {
#ifdef VPS_MOSAIC_ENABLE_TRACE
                            subWindow->window.subWindowInst = subWindowInst++;
#endif /* VPS_MOSAIC_ENABLE_TRACE */
                            /* Offset = new sub window Y - Y of original
                                                                window */
                            subWindow->window.bufferOffsetY =
                                    (windowYValue - winFmt[index].winStartY);
                            subWindow->window.bufferOffsetX = VPS_VALUE_0;

                            subWindow->window.frameIndex   = index;
                            subWindow->window.startY       = windowYValue;
                            subWindow->window.height       = windowHeight;
                            /* Other parameters remain the same */
                            subWindow->window.startX = winFmt[index].winStartX;
                            subWindow->window.width  = winFmt[index].winWidth;
                            subWindow->window.ipWindowFmt = &(winFmt[index]);
                            subWindow->window.priority = winFmt[index].priority;
                            /* Insert the sub-window node in ordered list
                               Y * 0x4000 + X
                               Adding 0x01 to V to ignore 0x00 value of Y */
                            winPri    =   ((subWindow->window.startY + 0x01) <<
                                             VPS_MOSAIC_SHIFT_Y_BY_11)  -
                                             subWindow->window.startX;
                            subWindow->nodesPlaceHolder.data =
                                &(subWindow->window);
                            VpsUtils_linkNodePri(
                                splitWindowListHndl,
                                &(subWindow->nodesPlaceHolder),
                                winPri);
                            /* Done splitting this window-move to next window */
                            break;
                        }
                        else
                        {
                            /* Error-exceeded the maximum numberof sub-windows*/
                            rtnValue    =   VPS_EALLOC;
                            break;
                        }
                    }
                    else
                    {
                        /* Error - The end points of this window is not
                                   logged in splitter list */
                        rtnValue    =   VPS_EFAIL;
                        break;
                    }
                } /* End of splitter list - to be applied on a window */
            }
            else
            {
                /* Error condition - the starting position of this window is not
                   marked in the splitter list */
                rtnValue    =   VPS_EFAIL;
                break;
            } /* End of find window co-ordinate check in splitter list */
        } /* End of splitter window list */
    } /* End of loop for the windows */
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<splitWindowsOnYAxis");
    return (rtnValue);
}


/**
 *  splitWindowsOnXAxis
 *  \brief Splits the overlapping windows into individual windows. Overlaps
 *         should be inferred on following conditions.
 *         In a given column if there are more than one window, then these
 *         windows are said to overlap on X axis.
 *
 *  \param splitterHndl     [IN] Handle to a list whose nodes define the
 *                               overlap points
 *  \param winListHndl      [IN] Handle to a list, whose nodes defines the
 *                               windows.
 *
 *  \param splitWinListHndl [OUT]Handle to an empty list. On successful
 *                               completion of this function, the nodes of this
 *                               list describe individual windows that do not
 *                               overlap on X axis.
 *
 *  \return             Returns 0 on success else returns error value.
 */

static Int32    splitWindowsOnXAxis(
                    VpsUtils_Handle splitterHndl,
                    VpsUtils_Handle winListHndl,
                    VpsUtils_Handle splitWinListHndl)
{
    Int32           rtnValue        =   VPS_EFAIL;
    UInt32          windowXValue, windowWidth;
    UInt32          winPri;
    VpsUtils_Node   *splitterNode,  *window;
    splitterNodeDef *splitNodeData;
    VpsHal_VpdmaMosaicWinFmt        *currWin;
    splitWindowsNodeDef             *subWindow    =   NULL;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>splitWindowsOnXAxis");

    GT_assert(mosaicTrace, (splitterHndl != NULL));
    GT_assert(mosaicTrace, (winListHndl != NULL));
    GT_assert(mosaicTrace, (splitWinListHndl != NULL));

    window = VpsUtils_unLinkNodeFromHead(winListHndl);

    /* Starting with first window - split all windows */
    while(window != NULL)
    {
        /* Get the first window */
        rtnValue    =   VPS_SOK;
        currWin =   (VpsHal_VpdmaMosaicWinFmt *)window->data;
        if (currWin == NULL)
        {
            rtnValue    =   VPS_EFAIL;
            GT_0trace(mosaicTrace, GT_ERR, "Could not get the window");
            break;
        }
        windowXValue    =   currWin->startX;
        windowWidth     =   currWin->width;
        /* For this window - walk the splitter list and split the window */
        splitterNode = VpsUtils_getHeadNode(splitterHndl);
        while(splitterNode != NULL)
        {
            /* Looking for node the splits the current window. Take note
               that we will be interested to split the window only if the
               splitter and window lie on the same row.*/
            splitNodeData    =   (splitterNodeDef *)(splitterNode->data);
            if (splitNodeData == NULL)
            {
                /* Should not occur */
                rtnValue    =   VPS_EFAIL;
                GT_0trace(mosaicTrace, GT_ERR, "Splitter node does not ");
                GT_0trace(mosaicTrace, GT_ERR, "have splitter data");
                break;
            }
            if (((splitNodeData->value > currWin->startX) &&
                 (splitNodeData->value < (currWin->startX + currWin->width))) &&
                (splitNodeData->row == currWin->startY))
            {
                if (splitNodeData->priority < currWin->priority)
                {
                    /* The current window requires to be split as is lies on the
                       same row as the splitter and there is an another window
                       that overlaps this window */
                    break;
                }
            }
            splitterNode    = splitterNode->next;
        }

        /* Check if this windows requires to be split - else add the current
           window and move on to next window */
        if ((splitterNode == NULL) && (rtnValue == VPS_SOK))
        {
            /* Insert the sub-window node in ordered list
               Y * 0x4000 + X */
            winPri    =   ((currWin->startY + 0x01) << VPS_MOSAIC_SHIFT_Y_BY_11)
                            - currWin->startX;

            VpsUtils_linkNodePri(
                splitWinListHndl,
                window,
                winPri);
        }

        while((splitterNode != NULL) && (rtnValue == VPS_SOK))
        {
            splitNodeData    =   (splitterNodeDef *)(splitterNode->data);
            if (splitNodeData == NULL)
            {
                /* Should not occur */
                rtnValue    =   VPS_EFAIL;
                GT_0trace(mosaicTrace, GT_ERR, "Splitter node does not ");
                GT_0trace(mosaicTrace, GT_ERR, "have splitter data");
                break;
            }
            if (splitNodeData->priority > currWin->priority)
            {
                /* Look at the next splitter node */
                splitterNode = splitterNode->next;
                continue;
            }
            /* The next node should either divide this window - or - should
               be equals to edge of this window - else an error - edge of
               this window is not marked in the splitter list */
            if ((splitNodeData->value < (windowXValue + windowWidth)) &&
                (splitNodeData->row == currWin->startY))
            {
                subWindow = allocateSplitWindowNode();
                if (subWindow == NULL)
                {
                    /* Error - Out of sub / divided windows */
                    rtnValue    =   VPS_EALLOC;
                    GT_0trace(mosaicTrace, GT_ERR, "Out of sub windows ");
                    break;
                }
                subWindow->window.startX        = windowXValue;
                subWindow->window.width         = splitNodeData->value -
                                                      windowXValue;
                /* Offset = X of window - X of splitter */
                subWindow->window.bufferOffsetX = windowXValue -currWin->startX;
                windowWidth     -= subWindow->window.width;
                windowXValue    = splitNodeData->value;
            }
            else if (((splitNodeData->value == (windowXValue + windowWidth))) &&
                     (splitNodeData->row == currWin->startY))
            {
                subWindow = allocateSplitWindowNode();
                if (subWindow == NULL)
                {
                    /* Error - Out of sub / divided windows */
                    rtnValue    =   VPS_EALLOC;
                    GT_0trace(mosaicTrace, GT_ERR, "Out of sub windows ");
                    break;
                }
                subWindow->window.startX        = windowXValue;
                subWindow->window.width         = windowWidth;
                /* Offset = X of original window + X of splitter */
                subWindow->window.bufferOffsetX = windowXValue -currWin->startX;
                windowWidth     = 0x0;
                windowXValue    = 0x0;
            }
            else
            {
                /* Look at the next splitter node */
                splitterNode = splitterNode->next;
                continue;
            }

            /* Copy the params that we do not intended to change */
            subWindow->window.startY        =  currWin->startY;
            subWindow->window.bufferOffsetY =  currWin->bufferOffsetY;
            subWindow->window.height        =  currWin->height;
            subWindow->window.ipWindowFmt   =  currWin->ipWindowFmt;
            subWindow->window.frameIndex    =  currWin->frameIndex;
            subWindow->window.priority      =  currWin->priority;

            /* Move on to next node on the splitter list */
            splitterNode = splitterNode->next;
            /* Insert the sub-window node in ordered list
               Y * 0x4000 + X */
            winPri    =   ((subWindow->window.startY + 0x01) <<
                             VPS_MOSAIC_SHIFT_Y_BY_11)  -
                             subWindow->window.startX;
            subWindow->nodesPlaceHolder.data =
                &(subWindow->window);
            VpsUtils_linkNodePri(
                splitWinListHndl,
                &(subWindow->nodesPlaceHolder),
                winPri);
        }
        if (rtnValue != VPS_SOK)
        {
            /* Error - report it back to the caller */
            break;
        }
        /* Done with processing current window - move to next */
        window = VpsUtils_unLinkNodeFromHead(winListHndl);
    }
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<splitWindowsOnXAxis");
    return (rtnValue);
}

static Int32    removeOverlappingWindow(VpsUtils_Handle winListHndl)
{
    Int32                       rtnValue = VPS_SOK;
    VpsUtils_Node              *winNode, *otherWinNode, *nextWinNode, *temp;
    VpsHal_VpdmaMosaicWinFmt   *window, *otherWindow;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>removeOverlappingWindow");
    GT_assert(mosaicTrace, (winListHndl != NULL));

    /* Take one window at a time and compare with all other window if it is
     * overlapping. */
    winNode = VpsUtils_getHeadNode(winListHndl);
    while (winNode != NULL)
    {
        window = (VpsHal_VpdmaMosaicWinFmt *) winNode->data;

        /* Traverse the window list */
        otherWinNode = winNode->next;
        nextWinNode = winNode->next;
        while (otherWinNode != NULL)
        {
            otherWindow = (VpsHal_VpdmaMosaicWinFmt *) otherWinNode->data;

            /* Already windows are split on Y axis and sorted out */
            if (window->startY != otherWindow->startY)
            {
                /* Move to next row windows */
                break;
            }

            /* Check if the windows overlap */
            if (((window->startX >= otherWindow->startX) &&
                    (window->startX <
                        (otherWindow->startX + otherWindow->width))) ||
                (((window->startX + window->width) > otherWindow->startX) &&
                    ((window->startX + window->width) <
                        (otherWindow->startX + otherWindow->width))) ||
                ((otherWindow->startX >= window->startX) &&
                    (otherWindow->startX < (window->startX + window->width))) ||
                (((otherWindow->startX + otherWindow->width) >
                        window->startX) &&
                    ((otherWindow->startX + otherWindow->width) <
                        (window->startX + window->width))))
            {
                /* Windows overlap!! Depending on priority decide which window
                 * to remove */
                if (window->priority > otherWindow->priority)
                {
                    /* Remove the current window. Also break from the search
                     * as the current node is removed!! */
                    VpsUtils_unLinkNodePri(winListHndl, winNode);
                    break;
                }
                else
                {
                    /* Remove the other window - Remove the next window and
                     * then take the next window node. */
                    if (nextWinNode == otherWinNode)
                    {
                        /* Removing the next node, hence store the next to
                         * next node */
                        nextWinNode = otherWinNode->next;
                    }
                    temp = otherWinNode->next;
                    VpsUtils_unLinkNodePri(winListHndl, otherWinNode);
                    otherWinNode = temp;
                }
            }
            else
            {
                /* Move to next window for comparison with current window */
                otherWinNode = otherWinNode->next;
            }
        }

        winNode = nextWinNode;
    }

    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<removeOverlappingWindow");

    return (rtnValue);
}

/**
 *  determineWindowOrder
 *  \brief Orders the split windows and determines the type of channels to be
 *         used. Computes the number of windows in the first row.
 *
 *         Orders the windows from Right to Left beginning with Top Right End
 *
 *         Assumption is that splitWinListHndl is an ordered list that defines
 *         nodes of the window which are ordered with formula (startY * constant
 *         + startX)
 *
 *  \param splitWinListHndl [IN]    Handle to the list whose nodes define the
 *                                  split windows.
 *  \param splitWinParam    [IN/OUT]Pointer of type
 *                                  VpsHal_VpdmaMosaicSplitWinParam.
 *                                  Will update numWindows to provides the
 *                                  total number of windows after splitting,
 *                                  starting with 0x0.
 *                                  Will update numFreeChans with the maximum
 *                                  number of free channels required.
 *                                  Will update
 *  \return             Returns 0 on success else returns error value.
 */
static Int32    determineWindowOrder(
                    VpsUtils_Handle                 splitWinListHndl,
                    VpsHal_VpdmaMosaicSplitWinParam *splitWinParam)
{
    Int32                       rtnValue        =   VPS_EFAIL;
    UInt32                      freeChans       =   VPS_VALUE_0;
    UInt32                      index           =   VPS_VALUE_0;
    VpsHal_VpdmaMosaicWinFmt    *currWin, *nextWin, *lastWinOnRow;
    VpsUtils_Node               *nodePtr        =   NULL;
    UInt32                      firstRow        =   TRUE, lastWinIdx;
    Int32                       row             =   -1;
    UInt32                      winsPerRow      =   VPS_VALUE_0;


    GT_0trace(mosaicTrace, GT_ENTER, ">>>>determineWindowOrder");
    GT_assert(mosaicTrace, (splitWinParam != NULL));
    GT_assert(mosaicTrace, (splitWinParam->splitWindows != NULL));

    splitWinParam->numFreeChans         =   VPS_VALUE_0;
    splitWinParam->numSplitWindows      =   VPS_VALUE_0;
    splitWinParam->numWindowsFirstRow   =   VPS_VALUE_1;
    if (splitWinParam->getNoWinsPerRow != 0x0)
    {
        splitWinParam->numWindowsPerRow[0x0]   =   winsPerRow;
    }
    nextWin =   NULL;
    currWin =   NULL;
    nodePtr =   VpsUtils_unLinkNodeFromHead(splitWinListHndl);
    /* For the very first channel - assume that we would require free channel */
    /* The assumption is that the windows are ordered Y * <positive no> + X
       in ascending order. */
    if(nodePtr != NULL)
    {
        row             =   VPS_VALUE_0;
        currWin         =   (VpsHal_VpdmaMosaicWinFmt *)nodePtr->data;
        lastWinOnRow    =   currWin;
        lastWinIdx      = 0;
        if (currWin != NULL)
        {
            splitWinParam->numSplitWindows++;
            winsPerRow++;
            currWin->isActualChanReq    =   FALSE;
            currWin->isLastWin = FALSE;
        }
    }
    splitWinParam->isDummyNeeded        =   FALSE;
    while(currWin != NULL)
    {
        currWin->isLastWin = FALSE;

        rtnValue    =   VPS_SOK;
        /* Check if application has provided enough space to accommodate this
           window definition */
        if (splitWinParam->numSplitWindows > splitWinParam->maxNumSplitWindows)
        {
            /* Out of space - let the caller know */
            rtnValue    =   VPS_ENO_MORE_BUFFERS;
            break;
        }
        /* Get the next window - used to determine if the current window
           requires to use actual / free channel */
        nodePtr =   VpsUtils_unLinkNodeFromHead(splitWinListHndl);
        if (nodePtr != NULL)
        {
            splitWinParam->numSplitWindows++;
            nextWin =   (VpsHal_VpdmaMosaicWinFmt *)nodePtr->data;
            /* Check if the current window and next window are on same Y axis */
            if (currWin->startY == nextWin->startY)
            {
                if (firstRow == TRUE)
                {
                    splitWinParam->numWindowsFirstRow =
                                                splitWinParam->numSplitWindows;
                }
                /* The current channel requires to be free, since we have
                   a predecessor */
                currWin->isActualChanReq    =   FALSE;
                currWin->freeChanIndex      =   freeChans++;

                winsPerRow++;
            }
            else if (nextWin->startY > currWin->startY)
            {
                if (splitWinParam->getNoWinsPerRow != 0x0)
                {
                    splitWinParam->numWindowsPerRow[row] = winsPerRow;
                }
                /* New row - First window - Requires an actual channel */
                currWin->isActualChanReq    =   TRUE;
                row++;
                winsPerRow = VPS_VALUE_1;
                GT_assert(mosaicTrace, (row < VPS_CFG_MAX_MULTI_WIN));
                /* Remember the maximum number of free chans required */
                if (freeChans > splitWinParam->numFreeChans)
                {
                    splitWinParam->numFreeChans = freeChans;
                }
#ifndef VPS_CFG_VPDMA_MOSAIC_USE_DIFF_FREE_CH
                /* Reset the free channel counter - effectively re-using the
                 * free channels on new row. */
                freeChans   =   VPS_VALUE_0;
#endif
                firstRow = FALSE;
                /* Remember the last window of the NEW row */
                if (nextWin != NULL)
                {
                    lastWinIdx = index + 1u;
                }
                lastWinOnRow    = nextWin;
                nodePtr = nodePtr;
            }
            if (splitWinParam->getNoWinsPerRow != 0x0)
            {
                splitWinParam->numWindowsPerRow[row] = winsPerRow;
            }
        }
        else
        {
            currWin->isActualChanReq    =   TRUE;
            nextWin                     =   NULL;
            /*
             * Remember the maximum number of free chans required
             */
            if (freeChans > splitWinParam->numFreeChans)
            {
                splitWinParam->numFreeChans = freeChans;
            }
            if (firstRow == TRUE)
            {
                splitWinParam->numWindowsFirstRow =
                                            splitWinParam->numSplitWindows;
            }
            if (splitWinParam->getNoWinsPerRow != 0x0)
            {
                splitWinParam->numWindowsPerRow[row] = winsPerRow;
            }
        }

        /* Copy the window parameters */
        splitWinParam->splitWindows[index].startX   =   currWin->startX;
        splitWinParam->splitWindows[index].startY   =   currWin->startY;
        splitWinParam->splitWindows[index].width    =   currWin->width;
        splitWinParam->splitWindows[index].height   =   currWin->height;
        splitWinParam->splitWindows[index].frameIndex       =
                                                    currWin->frameIndex;
        splitWinParam->splitWindows[index].bufferOffsetY    =
                                                    currWin->bufferOffsetY;
        splitWinParam->splitWindows[index].bufferOffsetX    =
                                                    currWin->bufferOffsetX;
        splitWinParam->splitWindows[index].isActualChanReq  =
                                                    currWin->isActualChanReq;
        splitWinParam->splitWindows[index].freeChanIndex    =
                                                    currWin->freeChanIndex;
        splitWinParam->splitWindows[index].ipWindowFmt      =
                                                    currWin->ipWindowFmt;
        splitWinParam->splitWindows[index].isLastWin = FALSE;
#ifdef VPS_MOSAIC_ENABLE_TRACE
        splitWinParam->splitWindows[index].subWindowInst    =
                                                    currWin->subWindowInst;
#endif /* VPS_MOSAIC_ENABLE_TRACE */

        index++;
        currWin         =   nextWin;
    }

    if (rtnValue == VPS_SOK)
    {
        /* Determine is a dummy channel is required at the end */
        GT_assert(mosaicTrace, (lastWinOnRow != NULL));
        if (lastWinOnRow != NULL)
        {
            if (((lastWinOnRow->startX + lastWinOnRow->width) <
                    splitWinParam->frameWidth)
                ||
                ((lastWinOnRow->startY + lastWinOnRow->height) <
                    splitWinParam->frameHeight))
            {
                /* If we require to count the dummy channel as an window
                   increment the numSplitWindows here */
                /* If we require to count the dummy channel as an free channel
                   add the numFreeChans assignment here */
                /* If an entry is required for dummy channel in the split
                   windows list add the entry here */
                splitWinParam->isDummyNeeded    =   TRUE;
                GT_0trace(mosaicTrace, GT_INFO, "Dummy window is required ");
            }

            splitWinParam->splitWindows[lastWinIdx].isLastWin = TRUE;
        }
    }
    splitWinParam->numRows = row;
    if (row != -1)
    {
        splitWinParam->numRows++;
    }
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<determineWindowOrder");

    return rtnValue;
}

/* ========================================================================== */
/*                              Debug Function                                */
/* ========================================================================== */

#ifdef VPS_MOSAIC_ENABLE_TRACE

static void printSplitWindows(VpsUtils_Handle winListHndl, UInt32 otherParams)
{
    VpsUtils_Node               *winNode;
    VpsHal_VpdmaMosaicWinFmt    *window;
    UInt32                      numWindows  =   VPS_VALUE_0;

    winNode =   VpsUtils_getHeadNode(winListHndl);

    while(winNode != NULL)
    {
        window = (VpsHal_VpdmaMosaicWinFmt*)winNode->data;
        GT_1trace(mosaicTrace, GT_INFO, "%d x ", window->width);
        GT_1trace(mosaicTrace, GT_INFO, "%d ", window->height);
        GT_1trace(mosaicTrace, GT_INFO, "(%d, ", window->startX);
        GT_1trace(mosaicTrace, GT_INFO, "%d)", window->startY);
        GT_1trace(mosaicTrace, GT_INFO, " bufferOffsetX = %d",
                                                        window->bufferOffsetX);
        GT_1trace(mosaicTrace, GT_INFO, " bufferOffsetY = %d",
                                                        window->bufferOffsetY);
        if (otherParams == TRUE)
        {
            GT_1trace(mosaicTrace, GT_INFO, " Window = %d",
                                                            window->frameIndex);
            GT_1trace(mosaicTrace, GT_INFO, " Sub Window = %d",
                                                        window->subWindowInst);
            if(window->isActualChanReq == FALSE)
            {
                GT_1trace(mosaicTrace, GT_INFO, " Free channel no = %d",
                                                        window->freeChanIndex);
            }
            else
            {
                GT_0trace(mosaicTrace, GT_INFO, " Actual channel required");
            }
        }
        GT_0trace(mosaicTrace, GT_INFO, " Actual channel required");
        winNode =   winNode->next;
        numWindows++;
        GT_1trace(mosaicTrace, GT_INFO, " Window = %d\n", numWindows);
    }
    GT_1trace(mosaicTrace, GT_INFO, "Total Number windows = %d\n", numWindows);
}

static void printSplitterList(VpsUtils_Handle splitterHndl, UInt32 rowAndClmn)
{
    VpsUtils_Node           *splitterNode;
    splitterNodeDef         *data;
    GT_assert(mosaicTrace, (splitterHndl != NULL));
    GT_0trace(mosaicTrace, GT_INFO, "The splitter list is\n");

    splitterNode = VpsUtils_getHeadNode(splitterHndl);
    while(splitterNode != NULL)
    {
        if (rowAndClmn  ==  FALSE)
        {
            GT_1trace(mosaicTrace, GT_INFO, "%d\n",
                                            *((UInt32 *)(splitterNode->data)));
        }
        else
        {
            data    =   (splitterNodeDef *)splitterNode->data;
            GT_1trace(mosaicTrace, GT_INFO, "X = %d,", data->value);
            GT_1trace(mosaicTrace, GT_INFO, "Y = %d,", data->row);
            GT_1trace(mosaicTrace, GT_INFO, "P = %d\n", data->priority);

        }
        splitterNode = splitterNode->next;
    }
    GT_0trace(mosaicTrace, GT_INFO, "End of List");
    GT_0trace(mosaicTrace, GT_INFO, "\n");
}


static void printWindowsSplitParams(VpsHal_VpdmaMosaicSplitWinParam *param)
{
    UInt32  index;

    for (index = 0x0; index < param->numSplitWindows; index++)
    {
        if (param->splitWindows[index].ipWindowFmt != NULL)
        {
            GT_1trace(mosaicTrace, GT_INFO, "%d x ",
                                            param->splitWindows[index].width);
            GT_1trace(mosaicTrace, GT_INFO, "%d",
                                            param->splitWindows[index].height);
            GT_1trace(mosaicTrace, GT_INFO, " (%d, ",
                                            param->splitWindows[index].startX);
            GT_1trace(mosaicTrace, GT_INFO, "%d)",
                                            param->splitWindows[index].startY);
            GT_1trace(mosaicTrace, GT_INFO, "bufferOffsetX  = %d ",
                                    param->splitWindows[index].bufferOffsetX);
            GT_1trace(mosaicTrace, GT_INFO, "bufferOffsetY  = %d ",
                                    param->splitWindows[index].bufferOffsetY);

            GT_1trace(mosaicTrace, GT_INFO, "Window = %d ",
                                        param->splitWindows[index].frameIndex);
            GT_1trace(mosaicTrace, GT_INFO, "Sub Window = %d ",
                                    param->splitWindows[index].subWindowInst);
            if(param->splitWindows[index].isActualChanReq == FALSE)
            {
                GT_1trace(mosaicTrace, GT_INFO, "Free channel no = %d ",
                                    param->splitWindows[index].freeChanIndex);
            }
            else
            {
                GT_0trace(mosaicTrace, GT_INFO, "Actual channel required");
            }
        }
        else
        {
            GT_1trace(mosaicTrace, GT_INFO, "Dummy Channel at index %d ",
                                                                        index);
            GT_1trace(mosaicTrace, GT_INFO, "Free channel no = %d",
                                    param->splitWindows[index].freeChanIndex);
        }
        GT_0trace(mosaicTrace, GT_INFO, "\n");
    }
}
#endif /* VPS_MOSAIC_ENABLE_TRACE */


/* ========================================================================== */
/*                                  APIs                                      */
/* ========================================================================== */

/**
 *  VpsHal_vpdmaInitMosaic
 *  \brief  Initialize mosaic switch feature.
 *          Initializes locals and lists required for mosaic switch.
 */
Int32 VpsHal_vpdmaInitMosaic(void)
{
    Int32               rtnValue    =   VPS_EUNSUPPORTED_CMD;
    Semaphore_Params    semParams;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>VpsHal_vpdmaInitMosaic");
    gSplitterNodesFreeIndex =   VPS_VALUE_0;
    gSplitWindowsFreeIndex  =   VPS_VALUE_0;
    if (isInitialized == FALSE)
    {
        rtnValue    =   VPS_SOK;
        while(TRUE)
        {
            gYAxisSplitterHndl  =   VpsUtils_createLinkList(
                                        VPSUTILS_LLT_DOUBLE,
                                        VPSUTILS_LAM_PRIORITY);
            gXAxisSplitterHndl  =   VpsUtils_createLinkList(
                                        VPSUTILS_LLT_DOUBLE,
                                        VPSUTILS_LAM_PRIORITY);
            if ((gYAxisSplitterHndl == NULL) || (gXAxisSplitterHndl == NULL))
            {
                /* Negative value is used in de-allocation,
                   return value would be updated to VPS_EALLOC.
                   EXPLICITLY hard-coding value to differentiate w.r.t
                   error codes*/
                GT_0trace(mosaicTrace, GT_ERR,"Could not create splitter list");
                rtnValue    =   -1;
                break;
            }
            gYAxisSplitWinListHndl  =   VpsUtils_createLinkList(
                                            VPSUTILS_LLT_DOUBLE,
                                            VPSUTILS_LAM_PRIORITY);
            gXAxisSplitWinListHndl  =   VpsUtils_createLinkList(
                                            VPSUTILS_LLT_DOUBLE,
                                            VPSUTILS_LAM_PRIORITY);
            if ((gYAxisSplitWinListHndl == NULL) ||
                (gXAxisSplitWinListHndl == NULL))
            {
                /* Negative value is used in de-allocation,
                   return value would be updated to VPS_EALLOC.
                   EXPLICITLY hard-coding value to differentiate w.r.t
                   error codes*/
                GT_0trace(mosaicTrace, GT_ERR,"Could not create splitwin list");
                rtnValue    =   -2;
                break;
            }

            Semaphore_Params_init(&semParams);
            gGuard  =   Semaphore_create(1u, &semParams, NULL);
            if (gGuard == NULL)
            {
                /* Negative value is used in de-allocation,
                   return value would be updated to VPS_EALLOC.
                   EXPLICITLY hard-coding value to differentiate w.r.t
                   error codes*/
                GT_0trace(mosaicTrace, GT_ERR, "Could not create semaphore");
                rtnValue    =   -3;
            }
            break;
        }
        /* Handle errors */
        switch(rtnValue)
        {
            case 0:
            {
                /* Everything went well */
                isInitialized   =   TRUE;
                break;
            }
            default:
            {
                /* Something grossly wrong */
                /* Explicit fall through case */
                rtnValue = VPS_EFAIL;
                if (gGuard != NULL)
                {
                    Semaphore_delete(&gGuard);
                }
            }
            case -3:
            {
                /* Explicit fall through case */
            }
            case -2:
                /* Explicit fall through case */
            case -1:
            {
                rtnValue = VPS_EALLOC;
                if (gYAxisSplitWinListHndl != NULL)
                {
                    VpsUtils_deleteLinkList(gYAxisSplitWinListHndl);
                }
                if (gXAxisSplitWinListHndl != NULL)
                {
                    VpsUtils_deleteLinkList(gXAxisSplitWinListHndl);
                }
                if (gYAxisSplitterHndl != NULL)
                {
                    VpsUtils_deleteLinkList(gYAxisSplitterHndl);
                }
                if (gXAxisSplitterHndl != NULL)
                {
                    VpsUtils_deleteLinkList(gXAxisSplitterHndl);
                }
            }
        }
    }
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<VpsHal_vpdmaInitMosaic");
    return (rtnValue);
}


/**
 *  VpsHal_vpdmaDeInitMosaic
 *  \brief  De Initialize mosaic switch feature, Releases all acquired
 *          resources.
 */
Int32 VpsHal_vpdmaDeInitMosaic(void)
{
    Int32               rtnValue    =   VPS_EUNSUPPORTED_CMD;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>VpsHal_vpdmaDeInitMosaic");
    if (isInitialized == TRUE)
    {
        gSplitterNodesFreeIndex =   VPS_VALUE_0;
        gSplitWindowsFreeIndex  =   VPS_VALUE_0;

        /* Get exclusive assess before de-initializing */
        Semaphore_pend(gGuard, BIOS_WAIT_FOREVER);

        rtnValue = VpsUtils_deleteLinkList(gXAxisSplitterHndl);
        if (rtnValue == VPS_SOK)
        {
            rtnValue = VpsUtils_deleteLinkList(gYAxisSplitterHndl);
        }
        if (rtnValue == VPS_SOK)
        {
            rtnValue = VpsUtils_deleteLinkList(gXAxisSplitWinListHndl);
        }
        if (rtnValue == VPS_SOK)
        {
            rtnValue = VpsUtils_deleteLinkList(gYAxisSplitWinListHndl);
        }
        if (rtnValue == VPS_SOK)
        {
            Semaphore_post(gGuard);
            Semaphore_delete(&gGuard);
            isInitialized = FALSE;
        }
    }
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<VpsHal_vpdmaDeInitMosaic");
    return (rtnValue);
}


/**
 *  VpsHal_vpdmaSetupMosaicCfg
 *  \brief This function determines the sequence to program the VPDMA in case
 *         of mosaic display. Following steps highlight the sequence of
 *         operation
 *         Step A - Check the list
 *         Step B - Handle single window condition
 *         Step C - Handle case of multiple window
 *         Step C.1 - Handle PIP
 *         Step C.1.1 Generate splitter list on X and Y axis
 *         Step C.1.2 Apply Y splitter list on all windows
 *         Step C.2 - Handle mosaic
 *         Step C.2.1 - Generate a splitter list on X axis
 *         Step C.2.2 - Apply the splitter on list of windows
 *         Step C.2.3 - Remove the overlapping low priority windows
 *         Step C.2.4 - Order the split windows
 */

Int32   VpsHal_vpdmaSetupMosaicCfg(
            Vps_MultiWinParams              *multiWindows,
            VpsHal_VpdmaMosaicSplitWinParam *splitWinParam)
{
    const Vps_WinFormat         *winFmt =   NULL;
    Int32           rtnValue            =   VPS_SOK;
    UInt32          pipMode             =   FALSE;

    GT_0trace(mosaicTrace, GT_ENTER, ">>>>VpsHal_vpdmaSetupMosaicCfg");

    GT_assert(mosaicTrace, (splitWinParam != NULL));
    GT_assert(mosaicTrace, (multiWindows != NULL));
    GT_assert(mosaicTrace, (splitWinParam->splitWindows != NULL));

    if (splitWinParam->getNoWinsPerRow != 0x0)
    {
        GT_assert(mosaicTrace, (splitWinParam->numWindowsPerRow != NULL));
    }
    winFmt =   multiWindows->winFmt;
    GT_assert(mosaicTrace, (winFmt != NULL));

    /* Initialize locals with apps window config */
    Semaphore_pend(gGuard, BIOS_WAIT_FOREVER);
    /* Clear up node pools */
    gSplitterNodesFreeIndex =   VPS_VALUE_0;
    gSplitWindowsFreeIndex  =   VPS_VALUE_0;

    if (isInitialized == FALSE)
    {
        GT_0trace(mosaicTrace, GT_ERR, "Not Initialized ");
        rtnValue    =   VPS_EUNSUPPORTED_CMD;
    }
    else if (((VpsUtils_getNodeCnt(gYAxisSplitterHndl) > VPS_VALUE_0) ||
             (VpsUtils_getNodeCnt(gYAxisSplitWinListHndl) > VPS_VALUE_0)) ||
            ((VpsUtils_getNodeCnt(gXAxisSplitterHndl) > VPS_VALUE_0) ||
             (VpsUtils_getNodeCnt(gXAxisSplitWinListHndl) > VPS_VALUE_0)))
    {
        /* Ensure that lists are clean */
        GT_0trace(mosaicTrace, GT_ERR, "Lists are not clean ");
        rtnValue    =   VPS_EFAIL;
    }
    else if (multiWindows->numWindows  == VPS_VALUE_1)
    {
        GT_0trace(mosaicTrace, GT_INFO, "Single windows - scenario");
        if (((winFmt[0].winStartX + winFmt[0].winWidth) >
                                                splitWinParam->frameWidth) ||
            ((winFmt[0].winStartY + winFmt[0].winHeight) >
                                                splitWinParam->frameHeight))
        {
            /* Window exceeds frames size */
            rtnValue    = VPS_EINVALID_PARAMS;
            GT_1trace(mosaicTrace, GT_ERR, "Window %d size > frame size",0);
        }
        /* Ensure that sub-window starts at even boundary */
        if (winFmt[0u].winStartX & 0x01u)
        {
            /* Window should start from even offset */
            rtnValue = VPS_EINVALID_PARAMS;
            GT_1trace(mosaicTrace, GT_ERR,
                "Window %d should start at even offset\n", 0u);
        }
        if (FVID2_SOK == rtnValue)
        {
            /* Step B - Handle single window condition */
            determineDummyChanReq(multiWindows, splitWinParam);
#ifdef VPS_MOSAIC_ENABLE_TRACE
            printWindowsSplitParams(splitWinParam);
#endif /* #ifdef VPS_MOSAIC_ENABLE_TRACE */
        }
    }
    else
    {
        while (TRUE)
        {
            GT_0trace(mosaicTrace, GT_INFO, "Multiple windows - scenario");
            GT_0trace(mosaicTrace, GT_INFO, "Generating splitter lists");
            /* Step C.1.1 - Generate the Y and X splitter list */
            rtnValue =  makeYAxisSplitterList(
                            winFmt,
                            splitWinParam,
                            multiWindows->numWindows,
                            gYAxisSplitterHndl,
                            &pipMode);
            if ((rtnValue != VPS_SOK) ||
                (VpsUtils_getNodeCnt(gYAxisSplitterHndl) <= VPS_VALUE_0))
            {
                GT_0trace(mosaicTrace, GT_ERR,"Could not make splitter list");
                break;
            }
#ifdef VPS_MOSAIC_ENABLE_TRACE
            printSplitterList(gYAxisSplitterHndl, FALSE);
#endif /* VPS_MOSAIC_ENABLE_TRACE */

            /* Step C.1.2 - Using this list split all windows on Y axis */
            rtnValue =  splitWindowsOnYAxis(
                            winFmt,
                            multiWindows->numWindows,
                            gYAxisSplitterHndl,
                            gYAxisSplitWinListHndl);
            if (rtnValue != VPS_SOK)
            {
                break;
            }
#ifdef VPS_MOSAIC_ENABLE_TRACE
            printSplitWindows(gYAxisSplitWinListHndl, FALSE);
#endif /* VPS_MOSAIC_ENABLE_TRACE */
            if (pipMode == TRUE)
            {
                /* Step C - Handle mosaic */
                /* Step C.2.1 - Generate splitter list on X axis */
                rtnValue =  makeXAxisSplitterList(
                                gYAxisSplitWinListHndl,
                                gXAxisSplitterHndl);
                if (rtnValue != VPS_SOK)
                {
                    break;
                }
#ifdef VPS_MOSAIC_ENABLE_TRACE
            printSplitterList(gXAxisSplitterHndl, TRUE);
#endif /* VPS_MOSAIC_ENABLE_TRACE */
                /* Step C.2.2 - Using this list split all windows on X axis */
                rtnValue =  splitWindowsOnXAxis(
                                gXAxisSplitterHndl,
                                gYAxisSplitWinListHndl,
                                gXAxisSplitWinListHndl);
                if (rtnValue != VPS_SOK)
                {
                    break;
                }
#ifdef VPS_MOSAIC_ENABLE_TRACE
                printSplitWindows(gXAxisSplitWinListHndl, FALSE);
#endif /* VPS_MOSAIC_ENABLE_TRACE */

                /* Step C.2.3 - Remove the overlapping low priority windows */
                rtnValue = removeOverlappingWindow(gXAxisSplitWinListHndl);
                if (rtnValue != VPS_SOK)
                {
                    break;
                }
#ifdef VPS_MOSAIC_ENABLE_TRACE
                printSplitWindows(gXAxisSplitWinListHndl, FALSE);
#endif /* VPS_MOSAIC_ENABLE_TRACE */

                /* Step C.2.4 - Order the windows */
                rtnValue =  determineWindowOrder(
                                gXAxisSplitWinListHndl,
                                splitWinParam);
            }
            else
            {
                /* Step C.2.4 - Order the windows */
                rtnValue =  determineWindowOrder(
                                gYAxisSplitWinListHndl,
                                splitWinParam);
                break;
            }
            break;
        } /* Error loop while true */
        /* Clear up the lists */
        VpsUtils_unLinkAllNodes(gXAxisSplitterHndl);
        VpsUtils_unLinkAllNodes(gYAxisSplitterHndl);
        VpsUtils_unLinkAllNodes(gXAxisSplitWinListHndl);
        VpsUtils_unLinkAllNodes(gYAxisSplitWinListHndl);
    }
    Semaphore_post(gGuard);
    GT_0trace(mosaicTrace, GT_INFO, "Start of final list of windows\n");
#ifdef VPS_MOSAIC_ENABLE_TRACE
    printWindowsSplitParams(splitWinParam);
#endif /* VPS_MOSAIC_ENABLE_TRACE */
    GT_0trace(mosaicTrace, GT_INFO, "End of final list of windows\n");
    GT_0trace(mosaicTrace, GT_LEAVE, "<<<<VpsHal_vpdmaSetupMosaicCfg");
    return (rtnValue);
}
