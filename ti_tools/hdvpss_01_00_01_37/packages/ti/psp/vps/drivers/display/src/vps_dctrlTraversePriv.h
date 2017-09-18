/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file Vps_dctrlInt.h
 *
 * \brief VPS Display Controller internal header file
 * This file exposes the APIs of the VPS Display Controller Traverser to
 * the main display Controller driver.
 *
 */

#ifndef _VPS_DCTRLTRAVERSE_H
#define _VPS_DCTRLTRAVERSE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 * \brief Function to initialize traverser with the given node. It
 *  will reset the stack and push the node at the top of the stack
 *
 * \param Pointer to Node
 *
 * \return None.
 */
Void dcInitTraverser(Dc_NodeInfo *node);

/**
 * \brief Function to get the top element of the stack. It just returns
 *        top element without removing it.
 *
 * \param stNum     [OUT] This keeps track of whether node is visited or
 *                       not while traversing.
 *
 * \return Pointer to nodeinfo structure or null if stack is empty.
 */
Dc_NodeInfo *dcStackPeak(UInt32 *stNum);

/**
 * \brief Function to get the parent of the stack peak. It just returns
 *        top element without removing it.
 *
 * \return Pointer to nodeinfo structure or null if stack is empty.
 */
Dc_NodeInfo *dcStackPeakParent();

/**
 * \brief Function to traverse the tree and return next enabled and
 *        untraversed node.
 *
 * \param inForward [IN] Flag to indicate whether traversing is from
 *                       top to bottom or bottom to top.
 *
 * \return Pointer to nodeinfo structure or null if stack is empty.
 */
Dc_NodeInfo *dcGetNextTraversedNode(UInt32 isForward);

/**
 * \brief Function to return next enabled and
 *        untraversed node of the given node.
 *
 * \param currNode  [IN] Current Node
 * \param inForward [IN] Flag to indicate whether traversing is from
 *                       top to bottom or bottom to top.
 *
 * \return Pointer to nodeinfo structure or null if stack is empty.
 */
Dc_NodeInfo *dcGetNextChildNode(Dc_NodeInfo *currNode,
                                UInt32 isForward);

/**
 * \brief Function to check whether the given node is last node on top
 *        or on bottom side.
 *
 * \param currNode  [IN] Current Node
 * \param inForward [IN] Flag to indicate whether to check inputs or outputs.
 *
 * \return true if it is last node or false.
 */
Int32 dcIsLastNode(Dc_NodeInfo *currNode, UInt32 isForward);

/**
 * \brief Function to get the parent node of the top of the stack.
 *
\ * \return Pointer to nodeinfo structure or null if stack is empty.
 */
Dc_NodeInfo* dcStackGetParentNode();

/**
 * \brief Function to get the leaf nodes of the given node in
 *        either forward or backward direction.
 *
 * \param currNode  [IN] Current Node
 * \param endNodes  [OUT]array of numbers of leaf nodes.
 * \param numNodes  [OUT]Number of leaf nodes
 * \param inForward [IN] Flag to indicate whether to check inputs or outputs.
 *
 * \return true if it is last node or false.
 */
Int32 dcGetLeafNodes(Dc_NodeInfo *currNode,
                     UInt32 *endNodes,
                     UInt32 *numNodes,
                     UInt32 isForward);



#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_DCTRLTRAVERSE_H */

