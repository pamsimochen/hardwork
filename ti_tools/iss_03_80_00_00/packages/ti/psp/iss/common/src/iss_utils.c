/** ==================================================================
 *  @file   iss_utils.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/common/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \file iss_utils.c
 *
 * \brief Utility functions implementation file
 * This file defines the helper functions like create, add and remove nodes
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/iss/common/iss_config.h>
#include <ti/psp/iss/common/trace.h>
#include <ti/psp/iss/common/iss_utils.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/**
 *  \brief Flags used by memory pool manager to indicate availability of pool
 *  memory
 */
#define ISSUTILS_MEM_FLAG_FREE          (0xFFu)
#define ISSUTILS_MEM_FLAG_ALLOC         (0x00u)

/** \brief Time out to be used in sem pend. */
#define ISSUTILS_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/** \brief Number of link list object to create statically. */
#define ISSUTILS_NUM_LINK_LIST_OBJECTS  (150u)

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/**
 *  struct IssUtilsLinkListObj
 *  \brief Structure to the link list object information.
 */
typedef struct {
    UInt32 used;
    VpsUtils_LinkListType listType;
    VpsUtils_LinkAddMode addMode;
    VpsUtils_Node *headNode;
    VpsUtils_Node *tailNode;
    UInt32 numElements;
    UInt32 priorityCnt;
} IssUtilsLinkListObj;

/**
 *  struct IssUtilsObj
 *  \brief Structure to ISS utils object information.
 */
typedef struct {
    Semaphore_Handle poolSem;
    VpsUtils_Node nodePool[ISSUTILS_NUM_NODE_OBJECTS];
    UInt32 nodeFlag[ISSUTILS_NUM_NODE_OBJECTS];
    IssUtilsLinkListObj linkListPool[ISSUTILS_NUM_LINK_LIST_OBJECTS];
} IssUtilsObj;

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     dutilsLinkCircularPri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsLinkCircularPri(VpsUtils_Node * headNode,
                                            VpsUtils_Node * node,
                                            Void * data, UInt32 priority);
/* ===================================================================
 *  @func     dutilsLinkDoublePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void dutilsLinkDoublePri(IssUtilsLinkListObj * llobj,
                                VpsUtils_Node * node,
                                Void * data,
                                UInt32 priority,
                                UInt32 linkUniqePriNodes, Int32 * status);
/* ===================================================================
 *  @func     dutilsUnLinkCircularPri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsUnLinkCircularPri(VpsUtils_Node * headNode,
                                              VpsUtils_Node * node);
/* ===================================================================
 *  @func     dutilsUnLinkDoublePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void dutilsUnLinkDoublePri(IssUtilsLinkListObj * llobj,
                                  VpsUtils_Node * node);
/* ===================================================================
 *  @func     dutilsUnLinkDouble                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsUnLinkDouble(IssUtilsLinkListObj * llobj,
                                         VpsUtils_NodDir dir,
                                         VpsUtils_Node * node);
/* ===================================================================
 *  @func     dutilsLinkDouble                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsLinkDouble(IssUtilsLinkListObj * llobj,
                                       VpsUtils_Node * node,
                                       Void * data, VpsUtils_NodDir dir);

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

/** ISS Utils objects */
static IssUtilsObj IssUtilsObjects;

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/**
 *  VpsUtils_init
 *  \brief Initializes the Util objects and create pool semaphore
 *
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \returns                Returns 0 on success else returns error value
 */
/* ===================================================================
 *  @func     VpsUtils_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int VpsUtils_init(Ptr arg)
{
    UInt32 cnt;

    Int retVal = 0;

    Semaphore_Params semParams;

    /* Init node pool and mark flags as free */
    VpsUtils_memset(IssUtilsObjects.nodePool,
                    (UInt8) 0u, sizeof(IssUtilsObjects.nodePool));
    for (cnt = 0u; cnt < ISSUTILS_NUM_NODE_OBJECTS; cnt++)
    {
        IssUtilsObjects.nodeFlag[cnt] = ISSUTILS_MEM_FLAG_FREE;
    }

    /* Init list pool and mark flags as free */
    VpsUtils_memset(IssUtilsObjects.linkListPool,
                    (UInt8) 0u, sizeof(IssUtilsObjects.linkListPool));
    for (cnt = 0u; cnt < ISSUTILS_NUM_LINK_LIST_OBJECTS; cnt++)
    {
        IssUtilsObjects.linkListPool[cnt].used = ISSUTILS_MEM_FLAG_FREE;
    }

    /* Initialise the semaphore parameters and create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    IssUtilsObjects.poolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == IssUtilsObjects.poolSem)
    {
        retVal = -1;
    }

    return (retVal);
}

/**
 *  VpsUtils_deInit
 *  \brief De-Initializes the Util objects by removing pool semaphore
 *
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \return                 Returns 0 on success else returns error value
 */
/* ===================================================================
 *  @func     VpsUtils_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int VpsUtils_deInit(Ptr arg)
{
    /* Delete the Semaphore created for Pool objects */
    Semaphore_delete(&IssUtilsObjects.poolSem);

    return (0);
}

/**
 *  VpsUtils_memset
 *  \brief Sets the memory with the given value. Access memory as byte.
 *  Returns the memory pointer.
 *
 *  \param mem              Destination memory pointer
 *  \param ch               Byte value to fill with
 *  \param byteCount        Number of bytes to fill
 *
 *  \return                 The destination memory pointer
 */
/* ===================================================================
 *  @func     VpsUtils_memset                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void *VpsUtils_memset(Ptr mem, UInt8 ch, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != mem));

    return memset(mem, ch, byteCount);
}

/**
 *  VpsUtils_memsetw
 *  \brief Sets the memory with the given value. Access memory as word.
 *  Hence memory pointer should be aligned to 4 byte boundary
 *  Returns the memory pointer.
 *
 *  \param mem              Destination memory pointer
 *  \param word             Word value to fill with
 *  \param wordCount        Number of words to fill
 *
 *  \return                 The destination memory pointer
 */
/* ===================================================================
 *  @func     VpsUtils_memsetw                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void *VpsUtils_memsetw(Ptr mem, UInt32 word, UInt32 wordCount)
{
    UInt32 count;

    UInt32 *memPtr = NULL;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != mem));

    memPtr = (UInt32 *) mem;
    for (count = 0; count < wordCount; count++)
    {
        memPtr[count] = word;
    }

    return (mem);
}

/**
 *  VpsUtils_memcpy
 *  \brief Copies source memory into destination memory. Access memory as byte.
 *  Returns the destination memory pointer.
 *
 *  \param dest             Destination memory pointer
 *  \param src              Source memory pointer
 *  \param byteCount        Number of bytes to copy
 *
 *  \return                 The destination memory pointer
 */
/* ===================================================================
 *  @func     VpsUtils_memcpy                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void *VpsUtils_memcpy(Ptr dest, const Void * src, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != dest));
    GT_assert(IssUtilsTrace, (NULL != src));

    return memcpy(dest, src, byteCount);
}

/**
 *  VpsUtils_memcmp
 *  \brief Compare memory block 1 with memory block 2. Access memory as byte.
 *  Returns 0 if two memories are  identical.
 *
 *  \param mem1             memory block 1
 *  \param mem2              memory block 2
 *  \param byteCount        Number of bytes to compare
 *
 *  \return                 0 if two memory are identical other return 1
 */
/* ===================================================================
 *  @func     VpsUtils_memcmp                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int VpsUtils_memcmp(const Void * mem1, const Void * mem2, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != mem1));
    GT_assert(IssUtilsTrace, (NULL != mem2));

    return memcmp(mem1, mem2, byteCount);
}

/**
 *  VpsUtils_createLinkList
 *  \brief Creates a link list object with the provided properties.
 *
 *  \param listType         List type - circular/double link list.
 *  \param addMode          Node addition mode - Top/Bottom/Priority based.
 *
 *  \return                 List handle if success else returns NULL.
 */
/* ===================================================================
 *  @func     VpsUtils_createLinkList                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_Handle VpsUtils_createLinkList(VpsUtils_LinkListType listType,
                                        VpsUtils_LinkAddMode addMode)
{
    UInt32 cnt;

    IssUtilsLinkListObj *llobj = NULL;

    Int32 retVal = ISS_SOK;

    if (ISSUTILS_LLT_CIRCULAR == listType && ISSUTILS_LAM_PRIORITY != addMode)
    {
        retVal = ISS_EFAIL;
        GT_0trace(IssUtilsTrace, GT_DEBUG, "Non-Priority circular link"
                  "list not Supported\n");

    }
    if (ISS_SOK == retVal)
    {
        /* Get a free link list object */
        Semaphore_pend(IssUtilsObjects.poolSem, ISSUTILS_SEM_TIMEOUT);
        for (cnt = 0u; cnt < ISSUTILS_NUM_LINK_LIST_OBJECTS; cnt++)
        {
            if (ISSUTILS_MEM_FLAG_FREE ==
                IssUtilsObjects.linkListPool[cnt].used)
            {
                llobj = &IssUtilsObjects.linkListPool[cnt];
                llobj->used = ISSUTILS_MEM_FLAG_ALLOC;
                break;
            }
        }
        Semaphore_post(IssUtilsObjects.poolSem);
    }
    if (NULL != llobj)
    {
        /* Initialize the variables */
        llobj->listType = listType;
        llobj->addMode = addMode;
        llobj->headNode = NULL;
        llobj->tailNode = NULL;
        llobj->numElements = 0u;
        llobj->priorityCnt = 0;

    }
    return (llobj);
}

/**
 *  VpsUtils_deleteLinkList
 *  \brief Deletes a link list object.
 *
 *  \param handle           Link list handle.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
/* ===================================================================
 *  @func     VpsUtils_deleteLinkList                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 VpsUtils_deleteLinkList(VpsUtils_Handle handle)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    GT_assert(IssUtilsTrace, (ISSUTILS_MEM_FLAG_FREE != llobj->used));
    GT_assert(IssUtilsTrace, (NULL == llobj->headNode));
    GT_assert(IssUtilsTrace, (NULL == llobj->tailNode));

    /* Free link list object */
    Semaphore_pend(IssUtilsObjects.poolSem, ISSUTILS_SEM_TIMEOUT);
    llobj->used = ISSUTILS_MEM_FLAG_FREE;
    llobj->headNode = NULL;
    llobj->tailNode = NULL;
    llobj->numElements = 0u;
    llobj->priorityCnt = 0u;
    Semaphore_post(IssUtilsObjects.poolSem);

    return (0);
}

/**
 *  VpsUtils_unLinkAllNodes
 *  \brief Releases all nodes without modifying any of the property.
 *         CAUTION - Memory is NOT de-allocated, its the responsibility of the
 *                   caller to ensure de-allocation of memory.
 *
 *  \param handle           Link list handle.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
/* ===================================================================
 *  @func     VpsUtils_unLinkAllNodes                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 VpsUtils_unLinkAllNodes(VpsUtils_Handle handle)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    GT_assert(IssUtilsTrace, (ISSUTILS_MEM_FLAG_FREE != llobj->used));

    /* Free link list object */
    Semaphore_pend(IssUtilsObjects.poolSem, ISSUTILS_SEM_TIMEOUT);
    llobj->headNode = NULL;
    llobj->tailNode = NULL;
    llobj->numElements = 0u;
    llobj->priorityCnt = 0;
    Semaphore_post(IssUtilsObjects.poolSem);

    return (0);
}

/**
 *  VpsUtils_linkNodePri
 *  \brief Links a node to the linked list according to the list type
 *  The memory to the node object should be allocated by the caller. This
 *  is used for link list with priority.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param priority         Priority of the node used for priority based
 *                          addition of nodes. Priority is in descending order
 *                          the value. So 0 is the highest priority and is
 *                          added to the top of the node.
 *                          Nodes with the same priority are always added to
 *                          the bottom of the existing nodes with same
 *                          priority.
 *                          For non-priority based modes, this parameter
 *                          is ignored and could be set to 0.
 */
/* ===================================================================
 *  @func     VpsUtils_linkNodePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_linkNodePri(VpsUtils_Handle handle,
                          VpsUtils_Node * node, UInt32 priority)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));
    GT_assert(IssUtilsTrace, (NULL != node));

    llobj = (IssUtilsLinkListObj *) handle;

    if (ISSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsLinkCircularPri(llobj->headNode,
                                                node, node->data, priority);
    }
    else if (ISSUTILS_LLT_DOUBLE == llobj->listType)
    {
        dutilsLinkDoublePri(llobj, node, node->data, priority, FALSE, NULL);
    }

    return;
}

/**
 *  VpsUtils_linkUniqePriNode
 *  \brief Very similar to VpsUtils_linkNodePri, except that on equal priority
 *          nodes will not be inserted. An error (ISS_EBADARGS) would be
 *          returned.
 *          Applicable for double linked list only.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param priority         Priority of the node used for priority based
 *                          addition of nodes. Priority is in descending order
 *                          the value. So 0 is the highest priority and is
 *                          added to the top of the node.
 *                          Nodes with the same priority are always added to
 *                          the bottom of the existing nodes with same
 *                          priority.
 *                          For non-priority based modes, this parameter
 *                          is ignored and could be set to 0.
 */
/* ===================================================================
 *  @func     VpsUtils_linkUniqePriNode                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 VpsUtils_linkUniqePriNode(VpsUtils_Handle handle,
                                VpsUtils_Node * node, UInt32 priority)
{
    IssUtilsLinkListObj *llobj;

    Int32 rtnValue = ISS_SOK;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));
    GT_assert(IssUtilsTrace, (NULL != node));

    llobj = (IssUtilsLinkListObj *) handle;

    if (ISSUTILS_LAM_PRIORITY != llobj->addMode)
    {
        if (ISSUTILS_LAM_TOP == llobj->addMode)
        {
            llobj->priorityCnt--;
        }
        else if (ISSUTILS_LAM_BOTTOM == llobj->addMode)
        {
            llobj->priorityCnt++;
        }
        priority = llobj->priorityCnt;
    }

    if (ISSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsLinkCircularPri(llobj->headNode,
                                                node, node->data, priority);
    }
    else if (ISSUTILS_LLT_DOUBLE == llobj->listType)
    {
        dutilsLinkDoublePri(llobj, node, node->data, priority, TRUE, &rtnValue);
        if (rtnValue == ISS_SOK)
        {
            llobj->numElements++;
        }
    }

    return (rtnValue);
}

/**
 *  VpsUtils_unLinkNodePri
 *  \brief Unlinks the node from the list. Used for the Priority linklists.
 *
 *  \param handle           Link list handle.
 *  \param node             Node pointer to be unlinked from the list.
 */
/* ===================================================================
 *  @func     VpsUtils_unLinkNodePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_unLinkNodePri(VpsUtils_Handle handle, VpsUtils_Node * node)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));
    GT_assert(IssUtilsTrace, (NULL != node));

    llobj = (IssUtilsLinkListObj *) handle;
    if (ISSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsUnLinkCircularPri(llobj->headNode, node);
    }
    else if (ISSUTILS_LLT_DOUBLE == llobj->listType)
    {
        dutilsUnLinkDoublePri(llobj, node);
    }
    return;
}

/**
 *  VpsUtils_unLinkNode
 *  \brief Unlinks the node from the list. Used for Non-priority linked lists
 *
 *  \param handle           Link list handle.
 *  \param node             Node pointer to be unlinked from the list.
 */
/* ===================================================================
 *  @func     VpsUtils_unLinkNode                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_unLinkNode(VpsUtils_Handle handle, VpsUtils_Node * node)
{
    IssUtilsLinkListObj *llobj;

    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_HEAD;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));
    GT_assert(IssUtilsTrace, (NULL != node));

    llobj = (IssUtilsLinkListObj *) handle;

    dutilsUnLinkDouble(llobj, dir, node);

    return;
}

/**
 *  VpsUtils_linkNodeToTail
 *  \brief                  Link the node to the tail of the double linked list.
 *                          No priority
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_linkNodeToTail                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_linkNodeToTail(VpsUtils_Handle handle, VpsUtils_Node * node)
{
    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_TAIL;

    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != node));
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    dutilsLinkDouble(llobj, node, node->data, dir);
}

/**
 *  VpsUtils_linkNodeToHead
 *  \brief                  Link the node to the head of the double linked list.
 *                          No priority
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_linkNodeToHead                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_linkNodeToHead(VpsUtils_Handle handle, VpsUtils_Node * node)
{
    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_HEAD;

    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != node));
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    dutilsLinkDouble(llobj, node, node->data, dir);
}

/**
 *  VpsUtils_unLinkNodeFromHead
 *  \brief                  Returns the node from head. Removes the  node from
 *                          the list.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_unLinkNodeFromHead                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_Node *VpsUtils_unLinkNodeFromHead(VpsUtils_Handle handle)
{
    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_HEAD;

    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */

    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    return (dutilsUnLinkDouble(llobj, dir, NULL));
}

/**
 *  VpsUtils_unLinkNodeFromTail
 *  \brief                  Returns the node from tail. Removes the  node from
 *                          the list.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_unLinkNodeFromTail                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_Node *VpsUtils_unLinkNodeFromTail(VpsUtils_Handle handle)
{
    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_TAIL;

    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    return (dutilsUnLinkDouble(llobj, dir, NULL));
}

/**
 *  VpsUtils_getHeadNode
 *  \brief                  Returns the reference to the headNode. Does
 *                          not remove the node from the head.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_getHeadNode                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_Node *VpsUtils_getHeadNode(VpsUtils_Handle handle)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;

    return (llobj->headNode);
}

/**
 *  VpsUtils_getTailNode
 *  \brief                  Returns the reference to the TailNode. Does
 *                          not remove the node from the head.
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 *
 */
/* ===================================================================
 *  @func     VpsUtils_getTailNode                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_Node *VpsUtils_getTailNode(VpsUtils_Handle handle)
{
    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    return (llobj->tailNode);
}

/**
 *  VpsUtils_isListEmpty
 *  \brief Checks whether a list is empty or not.
 *
 *  \param handle           List handle.
 *
 *  \return                 TRUE if List is empty else returns FALSE.
 */
/* ===================================================================
 *  @func     VpsUtils_isListEmpty                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 VpsUtils_isListEmpty(VpsUtils_Handle handle)
{
    UInt32 isEmpty = FALSE;

    IssUtilsLinkListObj *llobj;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    if (NULL == llobj->headNode)
    {
        isEmpty = TRUE;
    }

    return (isEmpty);
}

/**
 *  VpsUtils_getNumNodes
 *  \brief Returns the number of nodes present in a list.
 *
 *  \param handle           List handle.
 *
 *  \return                 Number of nodes present in a list.
 */
/* ===================================================================
 *  @func     VpsUtils_getNumNodes                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 VpsUtils_getNumNodes(VpsUtils_Handle handle)
{
    VpsUtils_Node *node = NULL;

    IssUtilsLinkListObj *llobj;

    UInt32 numNodes = 0u;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    if (ISSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        node = llobj->headNode;
        while (NULL != node)
        {
            numNodes++;
            node = node->next;
            /* Not supposed to have NULL in circular list */
            GT_assert(IssUtilsTrace, (NULL != node));

            /* Check if we have come to the end of the list */
            if (node == llobj->headNode)
            {
                break;
            }
        }
    }
    else if (ISSUTILS_LLT_DOUBLE == llobj->listType)
    {
        node = llobj->headNode;
        while (NULL != node)
        {
            numNodes++;
            node = node->next;
        }
    }

    return (numNodes);
}

/**
 *  VpsUtils_createQ
 *  \brief Creates a Queue object.
 *
 *  \return                 List handle if success else returns NULL.
 */
/* ===================================================================
 *  @func     VpsUtils_createQ                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
VpsUtils_QHandle VpsUtils_createQ(void)
{
    return VpsUtils_createLinkList(ISSUTILS_LLT_DOUBLE, ISSUTILS_LAM_BOTTOM);
}

/**
 *  VpsUtils_deleteQ
 *  \brief Deletes a Queue object.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
/* ===================================================================
 *  @func     VpsUtils_deleteQ                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 VpsUtils_deleteQ(VpsUtils_QHandle handle)
{
    return VpsUtils_deleteLinkList(handle);
}

/**
 *  VpsUtils_queue
 *  \brief Adds the data to the queue. Add the node to the tail of the queue.
 *
 *
 *  \param handle           Queue handle.
 *  \param qElem            Queue Element object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 */
/* ===================================================================
 *  @func     VpsUtils_queue                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_queue(VpsUtils_QHandle handle, VpsUtils_QElem * qElem, Ptr data)
{
    IssUtilsLinkListObj *llobj;

    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_TAIL;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    dutilsLinkDouble(llobj, qElem, data, dir);
}

/**
 *  VpsUtils_dequeue
 *  \brief Removes a element from the queue. Removes the element from the head
 &  of the queue.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Returns the removed data pointer.
 */
/* ===================================================================
 *  @func     VpsUtils_dequeue                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void *VpsUtils_dequeue(VpsUtils_QHandle handle)
{
    VpsUtils_Node *node;

    Void *data = NULL;

    IssUtilsLinkListObj *llobj;

    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_HEAD;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    node = dutilsUnLinkDouble(llobj, dir, NULL);
    if (NULL != node)
    {
        data = node->data;
    }
    else
    {
        data = NULL;
    }
    return data;
}

/**
 *  VpsUtils_queueBack
 *  \brief Adds the data to start of the queue. Queue the data back to the
 *  head of the queue.
 *
 *  \param handle           Queue handle.
 *  \param qElem            Queue Element object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 */
/* ===================================================================
 *  @func     VpsUtils_queueBack                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void VpsUtils_queueBack(VpsUtils_QHandle handle,
                        VpsUtils_QElem * qElem, Void * data)
{
    IssUtilsLinkListObj *llobj;

    VpsUtils_NodDir dir = ISSUTILS_NODE_DIR_HEAD;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    llobj = (IssUtilsLinkListObj *) handle;
    dutilsLinkDouble(llobj, qElem, data, dir);
}

/**
 *  VpsUtils_isQEmpty
 *  \brief Checks whether a queue is empty or not.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 TRUE if queue is empty else returns FALSE.
 */
/* ===================================================================
 *  @func     VpsUtils_isQEmpty                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 VpsUtils_isQEmpty(VpsUtils_QHandle handle)
{
    return VpsUtils_isListEmpty(handle);
}

/**
 *  VpsUtils_getNumQElem
 *  \brief Returns the number of Queue Elements present in a Queue.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Number of Queue Elements present in a Queue.
 */
/* ===================================================================
 *  @func     VpsUtils_getNumQElem                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 VpsUtils_getNumQElem(VpsUtils_QHandle handle)
{
    return VpsUtils_getNumNodes(handle);
}

/**
 *  dutilsLinkCircularPri
 *  \brief Links a node to a circular link list based on priority.
 *  For nodes with same priority, the new node will be added to the last.
 */
/* ===================================================================
 *  @func     dutilsLinkCircularPri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsLinkCircularPri(VpsUtils_Node * headNode,
                                            VpsUtils_Node * node,
                                            Void * data, UInt32 priority)
{
    VpsUtils_Node *curNode;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != node));

    node->data = data;
    node->priority = priority;

    /* Check if this is the first node. */
    if (NULL != headNode)
    {
        /* Add to the list based on priority */
        curNode = headNode;
        do
        {
            /* Nodes with the same priority are always added to the bottom of 
             * the existing nodes with same priority. */
            if (priority < curNode->priority)
            {
                if (curNode == headNode)
                {
                    /* Adding to the top of the list */
                    headNode = node;
                }
                break;
            }

            /* Move to next node */
            curNode = curNode->next;
            /* Not supposed to have NULL in circular list */
            GT_assert(IssUtilsTrace, (NULL != curNode));
        } while (curNode != headNode);
        /* Check if we have come to the end of the list */

        /* Add the node before the current node as we have traversed one
         * extra node. */
        node->next = curNode;
        node->prev = curNode->prev;

        /* Not supposed to have NULL in circular list */
        GT_assert(IssUtilsTrace, (NULL != curNode->prev->next));
        GT_assert(IssUtilsTrace, (NULL != curNode->prev));
        curNode->prev->next = node;
        curNode->prev = node;
    }
    else
    {
        /* First node. Add to as head node */
        headNode = node;
        node->next = node;
        node->prev = node;
    }

    return (headNode);
}

/**
 *  dutilsLinkDoublePri
 *  \brief Links a node to a double link list based on priority.
 *  For nodes with same priority, the new node will be added to the last.
 *
 *  ToDo - Remove multiple return statements - Have one exit point for the
 *         Function.
 */
/* ===================================================================
 *  @func     dutilsLinkDoublePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void dutilsLinkDoublePri(IssUtilsLinkListObj * llobj,
                                VpsUtils_Node * node,
                                Void * data,
                                UInt32 priority,
                                UInt32 linkUniqePriNodes, Int32 * status)
{
    VpsUtils_Node *curNode, *prevNode = NULL;

    Int32 retVal = ISS_SOK;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != node));

    node->data = data;
    node->priority = priority;

    if (NULL == status)
    {
        status = &retVal;
    }
    /* Add to the list based on priority */
    curNode = llobj->headNode;
    while (NULL != curNode)
    {
        /* Nodes with the same priority are always added to the bottom of the 
         * existing nodes with same priority. */
        if (priority < curNode->priority)
        {
            break;
        }

        /* Move to next node */
        prevNode = curNode;
        curNode = curNode->next;
    }
    /* If unique priority check is enabled - check for uniqueness */
    if (linkUniqePriNodes == TRUE)
    {
        GT_assert(IssUtilsTrace, (NULL != status));
        if (prevNode != NULL)
        {
            if (priority == prevNode->priority)
            {
                *status = ISS_EBADARGS;
            }
        }
    }
    if (ISS_SOK == *status)
    {
        /* Add the node between current and previous nodes */
        node->next = curNode;
        node->prev = prevNode;
        if (NULL != prevNode)
        {
            prevNode->next = node;
        }
        else
        {
            /* Adding to the top of the list */
            llobj->headNode = node;
        }

        if (NULL != curNode)
        {
            curNode->prev = node;
        }
        else
        {
            llobj->tailNode = node;
        }
        if (NULL != status)
        {
            *status = ISS_SOK;
        }
    }
}

/**
 *  dutilsUnLinkCircularPri
 *  \brief Unlinks a node from a circular link list.
 */
/* ===================================================================
 *  @func     dutilsUnLinkCircularPri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsUnLinkCircularPri(VpsUtils_Node * headNode,
                                              VpsUtils_Node * node)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != headNode));
    GT_assert(IssUtilsTrace, (NULL != node));

    /* Not supposed to have NULL in circular list */
    GT_assert(IssUtilsTrace, (NULL != node->next));
    GT_assert(IssUtilsTrace, (NULL != node->prev));

    /* Link the node's previous node to node's next node */
    node->prev->next = node->next;
    node->next->prev = node->prev;

    if (node == headNode)
    {
        /* Unlinking head node */
        if ((headNode == headNode->next) || (headNode == headNode->prev))
        {
            /* Unlinking last node */
            GT_assert(IssUtilsTrace,
                      ((headNode == headNode->next) &&
                       (headNode == headNode->prev)));
            headNode = NULL;
        }
        else
        {
            /* Make next node as head node */
            headNode = node->next;
        }
    }

    /* Reset node memory */
    node->next = NULL;
    node->prev = NULL;

    return (headNode);
}

/**
 *  dutilsUnLinkDoublePri
 *  \brief Unlinks a node from a double link list.
 */
/* ===================================================================
 *  @func     dutilsUnLinkDoublePri                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Void dutilsUnLinkDoublePri(IssUtilsLinkListObj * llobj,
                                  VpsUtils_Node * node)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != llobj));
    GT_assert(IssUtilsTrace, (NULL != node));

    if (NULL == node->prev)
    {
        /* Removing head node */
        llobj->headNode = node->next;
    }
    else
    {
        /* Removing non-head node */
        node->prev->next = node->next;
    }

    if (NULL != node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        llobj->tailNode = NULL;
    }

    /* Reset node memory */
    node->next = NULL;
    node->prev = NULL;
}

/**
 *  dutilsUnLinkDouble
 *  \brief Unlinks a node from a double link list.
 */
/* ===================================================================
 *  @func     dutilsUnLinkDouble                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsUnLinkDouble(IssUtilsLinkListObj * llobj,
                                         VpsUtils_NodDir dir,
                                         VpsUtils_Node * nodeToBeRem)
{
    VpsUtils_Node *node = NULL, *headNode, *tailNode, *listNode;

    UInt32 nodeCnt;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != llobj));

    if (NULL == nodeToBeRem)
    {
        if (NULL == llobj->headNode && NULL == llobj->tailNode)
        {
            node = NULL;
        }
        /* Check for the last node in the list */
        else if ((NULL != llobj->headNode) &&
                 (NULL == llobj->headNode->next &&
                  NULL == llobj->headNode->prev))
        {
            node = llobj->headNode;
            llobj->tailNode = NULL;
            llobj->headNode = NULL;
        }
        else if (NULL != llobj->headNode &&
                 NULL != llobj->tailNode && ISSUTILS_NODE_DIR_HEAD == dir)
        {
            /* Point headnode to the head of the list */
            headNode = llobj->headNode;
            /* headnode points to now next in the list */
            llobj->headNode = headNode->next;
            /* prev of the new headnode point to headNode of the list */
            if (NULL != llobj->headNode)
            {
                llobj->headNode->prev = NULL;
            }
            else
            {
                llobj->tailNode = NULL;
            }
            /* return the detached node */
            node = headNode;
        }
        else if (NULL != llobj->headNode &&
                 NULL != llobj->tailNode && ISSUTILS_NODE_DIR_TAIL == dir)
        {
            /* point to the tail node. */
            tailNode = llobj->tailNode;
            /* tailnode of the list points to the previous node */
            llobj->tailNode = tailNode->prev;
            /* next node of the previous node points to the tail node of list 
             */
            if (NULL != llobj->tailNode)
            {
                llobj->tailNode->next = NULL;
            }
            else
            {
                llobj->headNode = NULL;
            }
            /* return the tail node */
            node = tailNode;
        }
        else
        {
            /* TODO to do assert */
        }
    }
    else
    {
        listNode = llobj->headNode;
        for (nodeCnt = 0; nodeCnt < llobj->numElements; nodeCnt++)
        {
            if (listNode == nodeToBeRem)
            {
                /* Last node in the list */
                if ((llobj->headNode == listNode) &&
                    (llobj->tailNode == listNode))
                {
                    llobj->headNode = NULL;
                    llobj->tailNode = NULL;
                }
                else if (llobj->headNode == listNode)
                {
                    llobj->headNode = listNode->next;
                    llobj->headNode->prev = NULL;
                }
                else if (llobj->tailNode == listNode)
                {
                    llobj->tailNode = listNode->prev;
                    llobj->tailNode->next = NULL;
                }
                else
                {
                    listNode->prev->next = listNode->next;
                    listNode->next->prev = listNode->prev;
                }
                node = listNode;
                break;
            }
            listNode = listNode->next;
        }
    }
    if (NULL != node)
    {
        /* Reset node memory */
        node->next = NULL;
        node->prev = NULL;
        llobj->numElements--;
    }

    return (node);
}

/**
 *  VpsUtils_getNumNodes
 *  \brief Returns the number of nodes present in a list.
 *
 *  \param handle           List handle.
 *
 *  \return                 Number of nodes present in a list.
 */
/* ===================================================================
 *  @func     VpsUtils_getNodeCnt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 VpsUtils_getNodeCnt(VpsUtils_Handle handle)
{
    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != handle));

    return (((IssUtilsLinkListObj *) handle)->numElements);
}

/**
 *  dutilsLinkDouble
 *  \brief Links a node to a double link list either at head of the list or
 *  at tail of the list.
 *
 *
 */
/* ===================================================================
 *  @func     dutilsLinkDouble                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static VpsUtils_Node *dutilsLinkDouble(IssUtilsLinkListObj * llobj,
                                       VpsUtils_Node * node,
                                       Void * data, VpsUtils_NodDir dir)
{
    VpsUtils_Node *headNode, *tailNode = NULL;

    /* NULL pointer check */
    GT_assert(IssUtilsTrace, (NULL != node));
    GT_assert(IssUtilsTrace, (NULL != llobj));

    node->data = data;
    /* check for the first element in the list */
    if (NULL == llobj->headNode && NULL == llobj->tailNode)
    {
        /* Add the first element in the list */
        /* head node points to new element */
        llobj->headNode = node;
        /* tail node also points to new element */
        llobj->tailNode = node;
        /* next of node points to tail */
        node->next = NULL;
        /* prev of node points to head */
        node->prev = NULL;
    }
    else if (NULL != llobj->headNode &&
             NULL != llobj->tailNode && ISSUTILS_NODE_DIR_HEAD == dir)
    {
        headNode = llobj->headNode;
        /* headNode now points to new node */
        llobj->headNode = node;
        /* previous of new node points to headNode */
        node->prev = NULL;
        /* next of new points to prev head node */
        node->next = headNode;
        /* prev of previous head node head points to new node */
        headNode->prev = node;

    }
    else if (NULL != llobj->headNode &&
             NULL != llobj->tailNode && ISSUTILS_NODE_DIR_TAIL == dir)
    {

        tailNode = llobj->tailNode;
        /* next of new node points to tail of list */
        node->next = NULL;
        /* tail of list points to new node */
        llobj->tailNode = node;
        /* next of previous tial node points to new tail node */
        tailNode->next = node;
        /* previous of new tail node points to previous tail node */
        node->prev = tailNode;
    }
    llobj->numElements++;
    return (node);
}
