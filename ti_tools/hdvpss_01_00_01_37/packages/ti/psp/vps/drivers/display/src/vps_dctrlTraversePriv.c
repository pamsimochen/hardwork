/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_dctrl.c
 *
 * \brief Display Controller Traverser Source File
 * This file implements HAL APIs
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/drivers/display/vps_dlm.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlPriv.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlTraversePriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct Dc_Stack_t
{
    Dc_NodeInfo    *node[DC_NUM_NODES];
    /**< Array of node pointers */
    Int32           stNum[DC_NUM_NODES];
    /**< Keeps track of which child of this node is next to be visited, on
         the stack for a node. */
    UInt8           isVisited[DC_NUM_NODES];
    /**< Flag keeps track of whether given node is visited or not */
    Int32           top;
    /**< Top marker of the stack */
} Dc_Stack;


/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

/**
 * \brief Function to reset the stack used in the traverser.
 *
 * \return 0 on success -1 on error.
 */
static Void dcResetStack(void);

/**
 * \brief Function to Push the node on the top of the stack.
 *
 * \param Pointer to Node
 *
 * \return None.
 */
static Void dcStackPush(Dc_NodeInfo *node);

/**
 * \brief Function to pop and remove last element from the top of the stack.
 *
 * \param Pointer to Node
 *
 * \return None.
 */
static Void dcStackPop();

/**
 * \brief Function to check given node is already visited while traversing.
 *
 * \param currNode  [IN] Current Node
 *
 * \return true if node is visited or false.
 */
static UInt32 dcIsVisited(Dc_NodeInfo *node);


/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

Dc_Stack DcStack;


/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */


Void dcInitTraverser(Dc_NodeInfo *node)
{
    /* Initialize Stack */
    dcResetStack();

    /* Push root node onto stack */
    dcStackPush(node);
}



/* Get the top node on the stack */
Dc_NodeInfo * dcStackPeak(UInt32 *stNum)
{
    *stNum = DcStack.stNum[DcStack.top];
    return (DcStack.node[DcStack.top]);
}



Dc_NodeInfo *dcStackPeakParent()
{
    Dc_NodeInfo *node = NULL;

    if (DcStack.top > 0)
    {
        node = DcStack.node[DcStack.top - 1u];
    }

    return node;
}



Dc_NodeInfo *dcGetNextTraversedNode(UInt32 isForward)
{
    UInt32          nodeNum;
    Dc_NodeInfo    *currNode = NULL, *nextNode = NULL;

    while (DcStack.top > -1)
    {
        /* Get the stack top node */
        currNode = dcStackPeak(&nodeNum);

        if(FALSE == dcIsVisited(currNode))
        {
            /* If current node is ot visited, return it to the caller. */
            break;
        }
        else
        {
            /* Get the Next Node */
            nextNode = dcGetNextChildNode(currNode, isForward);

            /* If next node is not null, push it onto stack so that it can
             * be traversed */
            if (NULL != nextNode)
            {
                /* Push the start node onto stack */
                dcStackPush(nextNode);
            }
            else
            {
                /* Remove the node from the stack */
                dcStackPop();
            }
        }
    }
    if (-1 == DcStack.top)
    {
        currNode = NULL;
    }

    if (NULL != currNode)
    {
        nodeNum = currNode->nodeNum;
        DcStack.isVisited[nodeNum] = 1;
    }

    return (currNode);
}



/* Function to get the next enabled node for the currNode */
Dc_NodeInfo *dcGetNextChildNode(Dc_NodeInfo *currNode,
                                UInt32 isForward)
{
    Dc_NodeInfo *node = NULL;
    UInt32       isEnabled = TRUE;
    UInt32       nodeNum, nextNodeNum;

    nodeNum = DcStack.top;
    nextNodeNum = DcStack.stNum[nodeNum];
    do
    {
        /* Traversing from input Node to Venc Nodes */
        if (TRUE == isForward)
        {
            /* Get the next node from the output nodes */
            if (nextNodeNum < currNode->output.numNodes)
            {
                node = currNode->output.node[nextNodeNum];
                isEnabled = currNode->output.isEnabled[nextNodeNum];
            }
            else
            {
                break;
            }
        }
        else  /* Traversing from Venc Nodes to input node */
        {
            /* Get the next node from the input nodes */
            if (nextNodeNum < currNode->input.numNodes)
            {
                node = currNode->input.node[(nextNodeNum)];
                isEnabled = currNode->input.isEnabled[nextNodeNum];
            }
            else
            {
                break;
            }
        }
        nextNodeNum += 1u;
    } while (FALSE == isEnabled);

    if (FALSE == isEnabled)
    {
        node = NULL;
    }
    else
    {
        DcStack.stNum[nodeNum] = nextNodeNum;
    }
    return (node);
}



Int32 dcIsLastNode(Dc_NodeInfo *currNode, UInt32 isForward)
{
    UInt32 ret = TRUE;

    if (((TRUE == isForward) && (0 != currNode->output.numNodes)) ||
        ((FALSE == isForward) && (0 != currNode->input.numNodes)))
    {
        ret = FALSE;
    }

    return (ret);
}



Dc_NodeInfo* dcStackGetParentNode()
{
    UInt32 nodeIdx;
    Dc_NodeInfo *node = NULL;

    if (0 < DcStack.top)
    {
        nodeIdx = (DcStack.top - 1u);
        node = DcStack.node[nodeIdx];
    }

    return (node);
}



Int32 dcGetLeafNodes(Dc_NodeInfo *currNode,
                     UInt32 *endNodes,
                     UInt32 *numNodes,
                     UInt32 isForward)
{
    Int32           retVal = -1;

    GT_assert(DcTrace, (NULL != endNodes));
    GT_assert(DcTrace, (NULL != numNodes));
    GT_assert(DcTrace, (NULL != currNode));

    /* Initialize number of nodes to zero */
    *numNodes = 0;

    /* Initialize the stack */
    dcInitTraverser(currNode);

    /* Get the next node to be checked for the last node */
    currNode = dcGetNextTraversedNode(isForward);
    while (NULL != currNode)
    {
        /* If it is LastNode, store it into endNodes array */
        if (TRUE == dcIsLastNode(currNode, isForward))
        {
            endNodes[(*numNodes)] = currNode->nodeNum;
            (*numNodes) ++;
        }
        /* dcGetNextTraversedNode */
        currNode = dcGetNextTraversedNode(isForward);
    }
    retVal = 0;

    return (retVal);
}



static Void dcResetStack(void)
{
    VpsUtils_memset(&DcStack, 0, sizeof(DcStack));
    DcStack.top = -1;
    /* Initialize All stack numbers with -1 */
    VpsUtils_memset(DcStack.stNum, 0, sizeof(UInt32) * DC_NUM_NODES);
    VpsUtils_memset(DcStack.isVisited, 0, sizeof(DC_NUM_NODES));
}



/* Push a node on the stack */
static Void dcStackPush(Dc_NodeInfo *node)
{
    DcStack.top ++;
    DcStack.node[DcStack.top] = node;
}



/* Remove top node from the stack */
static Void dcStackPop()
{
    /* Remove the node from the stack */
    DcStack.stNum[DcStack.top] = 0;
    DcStack.node[DcStack.top] = NULL;
    DcStack.top --;
}



static UInt32 dcIsVisited(Dc_NodeInfo *node)
{
    UInt32 visited = TRUE;

    if (0 == DcStack.isVisited[node->nodeNum])
    {
        visited = FALSE;
    }

    return (visited);
}


