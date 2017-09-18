/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 * \file vps_utils.c
 *
 * \brief Utility functions implementation file
 * This file defines the helper functions like create, add and remove nodes
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Size of descriptor memory pool in bytes. */
#define VPSUTILS_MAX_DESC_MEM           (1984u * 1024u)

/**
 *  \brief Flags used by memory pool manager to indicate availability of pool
 *  memory
 */
#define VPSUTILS_MEM_FLAG_FREE          (0xFFu)
#define VPSUTILS_MEM_FLAG_ALLOC         (0x00u)

/** \brief Time out to be used in sem pend. */
#define VPSUTILS_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/** \brief Number of link list object to create statically. */
#define VPSUTILS_NUM_LINK_LIST_OBJECTS  (150u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsUtilsLinkListObj
 *  \brief Structure to the link list object information.
 */
typedef struct
{
    UInt32                  used;
    VpsUtils_LinkListType   listType;
    VpsUtils_LinkAddMode    addMode;
    VpsUtils_Node          *headNode;
    VpsUtils_Node          *tailNode;
    UInt32                  numElements;
    UInt32                  priorityCnt;
} VpsUtilsLinkListObj;

/**
 *  struct VpsUtilsObj
 *  \brief Structure to VPS utils object information.
 */
typedef struct
{
    Semaphore_Handle    poolSem;
    HeapMem_Handle      descMemHeap;
    UInt32             *descMemAddr;
    UInt32              descMemSize;
    UInt32              minFreeDescMem;
    VpsUtils_Node       nodePool[VPSUTILS_NUM_NODE_OBJECTS];
    UInt32              nodeFlag[VPSUTILS_NUM_NODE_OBJECTS];
    VpsUtilsLinkListObj linkListPool[VPSUTILS_NUM_LINK_LIST_OBJECTS];
} VpsUtilsObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static VpsUtils_Node *dutilsLinkCircularPri(VpsUtils_Node *headNode,
                                            VpsUtils_Node *node,
                                            Void *data,
                                            UInt32 priority);
static Void dutilsLinkDoublePri(VpsUtilsLinkListObj *llobj,
                                VpsUtils_Node *node,
                                Void *data,
                                UInt32 priority,
                                UInt32 linkUniqePriNodes,
                                Int32 *status);
static VpsUtils_Node *dutilsUnLinkCircularPri(VpsUtils_Node *headNode,
                                           VpsUtils_Node *node);
static Void dutilsUnLinkDoublePri(VpsUtilsLinkListObj *llobj,
                                         VpsUtils_Node *node);
static VpsUtils_Node *dutilsUnLinkDouble(VpsUtilsLinkListObj *llobj,
                                              VpsUtils_NodDir dir,
                                              VpsUtils_Node *node);
static VpsUtils_Node *dutilsLinkDouble(VpsUtilsLinkListObj *llobj,
                                            VpsUtils_Node *node,
                                            Void *data,
                                            VpsUtils_NodDir dir);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** VPS Utils objects */
VpsUtilsObj gVpsUtilsObjects;

static UInt32 gVpsUtilsDescMemPool[VPSUTILS_MAX_DESC_MEM / sizeof(UInt32)];
/** Align descriptor memory with that of VPDMA requirement. */
#pragma DATA_ALIGN(gVpsUtilsDescMemPool, VPSHAL_VPDMA_DESC_BYTE_ALIGN);
#pragma DATA_SECTION(gVpsUtilsDescMemPool,".bss:extMemNonCache:heap");


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsUtils_init
 *  \brief Initializes the Util objects and create pool semaphore
 *
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \returns                Returns 0 on success else returns error value
 */
Int VpsUtils_init(Ptr arg)
{
    Int                 retVal = VPS_SOK;
    UInt32              cnt;
    UInt32             *descMemAddr;
    UInt32              descMemSize;
    Semaphore_Params    semParams;
    HeapMem_Params      heapParams;

    descMemAddr = &gVpsUtilsDescMemPool[0u];
    descMemSize = VPSUTILS_MAX_DESC_MEM;

    /* Init node pool and mark flags as free */
    gVpsUtilsObjects.poolSem = NULL;
    gVpsUtilsObjects.descMemHeap = NULL;
    gVpsUtilsObjects.descMemAddr = NULL;
    gVpsUtilsObjects.descMemSize = 0u;
    VpsUtils_memset(
        gVpsUtilsObjects.nodePool,
        (UInt8) 0u,
        sizeof (gVpsUtilsObjects.nodePool));
    for (cnt = 0u; cnt < VPSUTILS_NUM_NODE_OBJECTS; cnt++)
    {
        gVpsUtilsObjects.nodeFlag[cnt] = VPSUTILS_MEM_FLAG_FREE;
    }

    /* Init list pool and mark flags as free */
    VpsUtils_memset(
        gVpsUtilsObjects.linkListPool,
        (UInt8) 0u,
        sizeof (gVpsUtilsObjects.linkListPool));
    for (cnt = 0u; cnt < VPSUTILS_NUM_LINK_LIST_OBJECTS; cnt++)
    {
        gVpsUtilsObjects.linkListPool[cnt].used = VPSUTILS_MEM_FLAG_FREE;
    }

    /* Initialise the semaphore parameters and create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    gVpsUtilsObjects.poolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == gVpsUtilsObjects.poolSem)
    {
        GT_0trace(VpsUtilsTrace, GT_ERR, "Pool semaphore create failed!!\n");
        retVal = FVID2_EALLOC;
    }

    if (VPS_SOK == retVal)
    {
        /* Create memory pool heap */
        HeapMem_Params_init(&heapParams);

        heapParams.buf = descMemAddr;
        heapParams.size = descMemSize;
        gVpsUtilsObjects.descMemHeap = HeapMem_create(&heapParams, NULL);
        if (NULL == gVpsUtilsObjects.descMemHeap)
        {
            GT_0trace(VpsUtilsTrace, GT_ERR, "Desc heap create failed!!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            gVpsUtilsObjects.descMemAddr = descMemAddr;
            gVpsUtilsObjects.descMemSize = descMemSize;
            gVpsUtilsObjects.minFreeDescMem = descMemSize;
        }
    }

    if (VPS_SOK != retVal)
    {
        VpsUtils_deInit(NULL);
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
Int VpsUtils_deInit(Ptr arg)
{
    UInt32          totalFreeSize;

    /* Delete the Semaphore created for Pool objects */
    if (NULL != gVpsUtilsObjects.poolSem)
    {
        Semaphore_delete(&gVpsUtilsObjects.poolSem);
        gVpsUtilsObjects.poolSem = NULL;
    }

    /* Delete descriptor memory pool heap handle */
    if (NULL != gVpsUtilsObjects.descMemHeap)
    {
        /* Check for memory leak */
        totalFreeSize = VpsUtils_getDescMemHeapFreeSpace();
        if (totalFreeSize != gVpsUtilsObjects.descMemSize)
        {
            GT_1trace(VpsUtilsTrace, GT_ERR,
                "%d bytes memory leak in descriptor heap!!\n",
                (gVpsUtilsObjects.descMemSize - totalFreeSize));
        }

        HeapMem_delete(&gVpsUtilsObjects.descMemHeap);
        gVpsUtilsObjects.descMemHeap = NULL;
    }

    return (VPS_SOK);
}

/**
 *  VpsUtils_allocDescMem
 *  \brief Allocates memory from descriptor memory pool. This section of memory
 *  is non-cached and physically contiguous.
 *
 *  \param size   [IN]      Size in bytes to allocate.
 *  \param align  [IN]      Alignment in bytes.
 *
 *  \return                 Returns memory pointer on success else returns
 *                          NULL.
 */
void *VpsUtils_allocDescMem(UInt32 size, UInt32 align)
{
    void       *addr;
    UInt32      totalFreeSize;

    Semaphore_pend(gVpsUtilsObjects.poolSem, VPSUTILS_SEM_TIMEOUT);

    /* allocate memory */
    addr = HeapMem_alloc(gVpsUtilsObjects.descMemHeap, size, align, NULL);

    /* Store the minimum free space for statistics */
    totalFreeSize = VpsUtils_getDescMemHeapFreeSpace();
    if (totalFreeSize < gVpsUtilsObjects.minFreeDescMem)
    {
        gVpsUtilsObjects.minFreeDescMem = totalFreeSize;
    }

    Semaphore_post(gVpsUtilsObjects.poolSem);

    return (addr);
}

/**
 *  VpsUtils_freeDescMem
 *  \brief Free previously allocate descriptor memory pointer.
 *
 *  \param addr   [IN]      memory pointer to free
 *  \param size   [IN]      Size in bytes of the memory.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsUtils_freeDescMem(void *addr, UInt32 size)
{
    Int32       retVal = VPS_SOK;

    Semaphore_pend(gVpsUtilsObjects.poolSem, VPSUTILS_SEM_TIMEOUT);

    /* Free previously allocated memory */
    HeapMem_free(gVpsUtilsObjects.descMemHeap, addr, size);

    Semaphore_post(gVpsUtilsObjects.poolSem);

    return (retVal);
}

/**
 *  VpsUtils_getDescMemHeapFreeSpace
 *  \brief Returns the descriptor memory heap free space in bytes.
 *
 *  \return                 Returns free space in bytes.
 */
UInt32 VpsUtils_getDescMemHeapFreeSpace(void)
{
    UInt32          totalFreeSize = 0u;
    Memory_Stats    stats;

    if (NULL != gVpsUtilsObjects.descMemHeap)
    {
        HeapMem_getStats(gVpsUtilsObjects.descMemHeap, &stats);
        totalFreeSize = (UInt32) (stats.totalFreeSize);
    }

    return (totalFreeSize);
}

/**
 *  VpsUtils_getDescMemHeapMinFreeSpace
 *  \brief Returns the minimum recorded descriptor memory heap free space
 *  in bytes. This could be used to analyze the descriptor memory heap usage.
 *
 *  \return                 Returns free space in bytes.
 */
UInt32 VpsUtils_getDescMemHeapMinFreeSpace(void)
{
    return (gVpsUtilsObjects.minFreeDescMem);
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
void *VpsUtils_memset(Ptr mem, UInt8 ch, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != mem));

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
void *VpsUtils_memsetw(Ptr mem, UInt32 word, UInt32 wordCount)
{
    UInt32 count;
    UInt32 *memPtr = NULL;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != mem));

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
void *VpsUtils_memcpy(Ptr dest, const Void *src, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != dest));
    GT_assert(VpsUtilsTrace, (NULL != src));

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
int VpsUtils_memcmp(const Void *mem1, const Void *mem2, UInt32 byteCount)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != mem1));
    GT_assert(VpsUtilsTrace, (NULL != mem2));

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
VpsUtils_Handle VpsUtils_createLinkList(VpsUtils_LinkListType listType,
                                        VpsUtils_LinkAddMode addMode)
{
    UInt32                  cnt;
    VpsUtilsLinkListObj    *llobj = NULL;
    Int32                   retVal = VPS_SOK;


    if (VPSUTILS_LLT_CIRCULAR == listType &&
        VPSUTILS_LAM_PRIORITY != addMode)
    {
        retVal = VPS_EFAIL;
        GT_0trace(VpsUtilsTrace, GT_DEBUG, "Non-Priority circular link"
                    "list not Supported\n");

    }
    if (VPS_SOK == retVal)
    {
        /* Get a free link list object */
        Semaphore_pend(gVpsUtilsObjects.poolSem, VPSUTILS_SEM_TIMEOUT);
        for (cnt = 0u; cnt < VPSUTILS_NUM_LINK_LIST_OBJECTS; cnt++)
        {
            if (VPSUTILS_MEM_FLAG_FREE ==
                gVpsUtilsObjects.linkListPool[cnt].used)
            {
                llobj = &gVpsUtilsObjects.linkListPool[cnt];
                llobj->used = VPSUTILS_MEM_FLAG_ALLOC;
                break;
            }
        }
        Semaphore_post(gVpsUtilsObjects.poolSem);
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
Int32 VpsUtils_deleteLinkList(VpsUtils_Handle handle)
{
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
    GT_assert(VpsUtilsTrace, (VPSUTILS_MEM_FLAG_FREE != llobj->used));
    GT_assert(VpsUtilsTrace, (NULL == llobj->headNode));
    GT_assert(VpsUtilsTrace, (NULL == llobj->tailNode));

    /* Free link list object */
    Semaphore_pend(gVpsUtilsObjects.poolSem, VPSUTILS_SEM_TIMEOUT);
    llobj->used = VPSUTILS_MEM_FLAG_FREE;
    llobj->headNode = NULL;
    llobj->tailNode = NULL;
    llobj->numElements = 0u;
    llobj->priorityCnt = 0u;
    Semaphore_post(gVpsUtilsObjects.poolSem);

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
Int32 VpsUtils_unLinkAllNodes(VpsUtils_Handle handle)
{
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
    GT_assert(VpsUtilsTrace, (VPSUTILS_MEM_FLAG_FREE != llobj->used));

    /* Free link list object */
    Semaphore_pend(gVpsUtilsObjects.poolSem, VPSUTILS_SEM_TIMEOUT);
    llobj->headNode = NULL;
    llobj->tailNode = NULL;
    llobj->numElements = 0u;
    llobj->priorityCnt = 0;
    Semaphore_post(gVpsUtilsObjects.poolSem);

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
Void VpsUtils_linkNodePri(VpsUtils_Handle handle,
                       VpsUtils_Node *node,
                       UInt32 priority)
{
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));
    GT_assert(VpsUtilsTrace, (NULL != node));

    llobj = (VpsUtilsLinkListObj *) handle;

    if (VPSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsLinkCircularPri(
                              llobj->headNode,
                              node,
                              node->data,
                              priority);
    }
    else if (VPSUTILS_LLT_DOUBLE == llobj->listType)
    {
        dutilsLinkDoublePri(
                        llobj,
                        node,
                        node->data,
                        priority,
                        FALSE,
                        NULL);
    }

    return;
}

/**
 *  VpsUtils_linkUniqePriNode
 *  \brief Very similar to VpsUtils_linkNodePri, except that on equal priority
 *          nodes will not be inserted. An error (VPS_EBADARGS) would be
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
Int32 VpsUtils_linkUniqePriNode(VpsUtils_Handle handle,
                       VpsUtils_Node *node,
                       UInt32 priority)
{
    VpsUtilsLinkListObj    *llobj;
    Int32                  rtnValue =   VPS_SOK;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));
    GT_assert(VpsUtilsTrace, (NULL != node));

    llobj = (VpsUtilsLinkListObj *) handle;

    if (VPSUTILS_LAM_PRIORITY != llobj->addMode)
    {
        if (VPSUTILS_LAM_TOP == llobj->addMode)
        {
            llobj->priorityCnt--;
        }
        else if (VPSUTILS_LAM_BOTTOM == llobj->addMode)
        {
            llobj->priorityCnt++;
        }
        priority = llobj->priorityCnt;
    }

    if (VPSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsLinkCircularPri(
                              llobj->headNode,
                              node,
                              node->data,
                              priority);
    }
    else if (VPSUTILS_LLT_DOUBLE == llobj->listType)
    {
        dutilsLinkDoublePri(
                            llobj,
                            node,
                            node->data,
                            priority,
                            TRUE,
                            &rtnValue);
        if (rtnValue == VPS_SOK)
        {
            llobj->numElements++;
        }
    }

    return(rtnValue);
}

/**
 *  VpsUtils_unLinkNodePri
 *  \brief Unlinks the node from the list. Used for the Priority linklists.
 *
 *  \param handle           Link list handle.
 *  \param node             Node pointer to be unlinked from the list.
 */
Void VpsUtils_unLinkNodePri(VpsUtils_Handle handle, VpsUtils_Node *node)
{
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));
    GT_assert(VpsUtilsTrace, (NULL != node));

    llobj = (VpsUtilsLinkListObj *) handle;
    if (VPSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        llobj->headNode = dutilsUnLinkCircularPri(llobj->headNode, node);
    }
    else if (VPSUTILS_LLT_DOUBLE == llobj->listType)
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
Void VpsUtils_unLinkNode(VpsUtils_Handle handle, VpsUtils_Node *node)
{
    VpsUtilsLinkListObj    *llobj;
    VpsUtils_NodDir         dir = VPSUTILS_NODE_DIR_HEAD;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));
    GT_assert(VpsUtilsTrace, (NULL != node));

    llobj = (VpsUtilsLinkListObj *) handle;

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
Void VpsUtils_linkNodeToTail(VpsUtils_Handle handle,
                       VpsUtils_Node *node)
{
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_TAIL;
    VpsUtilsLinkListObj    *llobj;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != node));
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
Void VpsUtils_linkNodeToHead(VpsUtils_Handle handle,
                       VpsUtils_Node *node)
{
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_HEAD;
    VpsUtilsLinkListObj    *llobj;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != node));
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
VpsUtils_Node *VpsUtils_unLinkNodeFromHead(VpsUtils_Handle handle)
{
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_HEAD;
    VpsUtilsLinkListObj    *llobj;
    /* NULL pointer check */

    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
VpsUtils_Node *VpsUtils_unLinkNodeFromTail(VpsUtils_Handle handle)
{
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_TAIL;
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
VpsUtils_Node *VpsUtils_getHeadNode(VpsUtils_Handle handle)
{
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;

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
VpsUtils_Node *VpsUtils_getTailNode(VpsUtils_Handle handle)
{
    VpsUtilsLinkListObj    *llobj;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
UInt32 VpsUtils_isListEmpty(VpsUtils_Handle handle)
{
    UInt32                  isEmpty = FALSE;
    VpsUtilsLinkListObj    *llobj;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
UInt32 VpsUtils_getNumNodes(VpsUtils_Handle handle)
{
    VpsUtils_Node          *node = NULL;
    VpsUtilsLinkListObj    *llobj;
    UInt32                  numNodes = 0u;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
    if (VPSUTILS_LLT_CIRCULAR == llobj->listType)
    {
        node = llobj->headNode;
        while (NULL != node)
        {
            numNodes++;
            node = node->next;
            /* Not supposed to have NULL in circular list */
            GT_assert(VpsUtilsTrace, (NULL != node));

            /* Check if we have come to the end of the list */
            if (node == llobj->headNode)
            {
                break;
            }
        }
    }
    else if (VPSUTILS_LLT_DOUBLE == llobj->listType)
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
VpsUtils_QHandle VpsUtils_createQ(void)
{
    return VpsUtils_createLinkList(VPSUTILS_LLT_DOUBLE, VPSUTILS_LAM_BOTTOM);
}

/**
 *  VpsUtils_deleteQ
 *  \brief Deletes a Queue object.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Returns 0 on success else returns error value.
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
Void VpsUtils_queue(VpsUtils_QHandle handle,
                    VpsUtils_QElem *qElem,
                     Ptr data)
{
    VpsUtilsLinkListObj    *llobj;
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_TAIL;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
Void *VpsUtils_dequeue(VpsUtils_QHandle handle)
{
    VpsUtils_Node  *node;
    Void           *data = NULL;
    VpsUtilsLinkListObj    *llobj;
     VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_HEAD;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
Void VpsUtils_queueBack(VpsUtils_QHandle handle,
                        VpsUtils_QElem *qElem,
                        Void *data)
{
    VpsUtilsLinkListObj    *llobj;
    VpsUtils_NodDir dir = VPSUTILS_NODE_DIR_HEAD;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    llobj = (VpsUtilsLinkListObj *) handle;
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
UInt32 VpsUtils_getNumQElem(VpsUtils_QHandle handle)
{
    return VpsUtils_getNumNodes(handle);
}

/**
 *  dutilsLinkCircularPri
 *  \brief Links a node to a circular link list based on priority.
 *  For nodes with same priority, the new node will be added to the last.
 */
static VpsUtils_Node *dutilsLinkCircularPri(VpsUtils_Node *headNode,
                                            VpsUtils_Node *node,
                                            Void *data,
                                            UInt32 priority)
{
    VpsUtils_Node   *curNode;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != node));

    node->data = data;
    node->priority = priority;

    /* Check if this is the first node. */
    if (NULL != headNode)
    {
        /* Add to the list based on priority */
        curNode = headNode;
        do
        {
            /* Nodes with the same priority are always added to the bottom
             * of the existing nodes with same priority. */
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
            GT_assert(VpsUtilsTrace, (NULL != curNode));
        } while (curNode != headNode);
        /* Check if we have come to the end of the list */

        /* Add the node before the current node as we have traversed one
         * extra node. */
        node->next = curNode;
        node->prev = curNode->prev;

        /* Not supposed to have NULL in circular list */
        GT_assert(VpsUtilsTrace, (NULL != curNode->prev));
        GT_assert(VpsUtilsTrace, (NULL != curNode->prev->next));
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
static Void dutilsLinkDoublePri(VpsUtilsLinkListObj *llobj,
                                VpsUtils_Node *node,
                                Void *data,
                                UInt32 priority,
                                UInt32 linkUniqePriNodes,
                                Int32 *status)
{
    VpsUtils_Node *curNode, *prevNode = NULL;
    Int32 retVal = VPS_SOK;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != node));

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
        /* Nodes with the same priority are always added to the bottom
         * of the existing nodes with same priority. */
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
        GT_assert(VpsUtilsTrace, (NULL != status));
        if (prevNode != NULL)
        {
            if (priority == prevNode->priority)
            {
                *status = VPS_EBADARGS;
            }
        }
    }
    if (VPS_SOK == *status)
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
            *status = VPS_SOK;
        }
    }
}



/**
 *  dutilsUnLinkCircularPri
 *  \brief Unlinks a node from a circular link list.
 */
static VpsUtils_Node *dutilsUnLinkCircularPri(VpsUtils_Node *headNode,
                                              VpsUtils_Node *node)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != headNode));
    GT_assert(VpsUtilsTrace, (NULL != node));

    /* Not supposed to have NULL in circular list */
    GT_assert(VpsUtilsTrace, (NULL != node->next));
    GT_assert(VpsUtilsTrace, (NULL != node->prev));

    /* Link the node's previous node to node's next node */
    node->prev->next = node->next;
    node->next->prev = node->prev;

    if (node == headNode)
    {
        /* Unlinking head node */
        if ((headNode == headNode->next) || (headNode == headNode->prev))
        {
            /* Unlinking last node */
            GT_assert(VpsUtilsTrace,
                ((headNode == headNode->next) && (headNode == headNode->prev)));
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
static Void dutilsUnLinkDoublePri(VpsUtilsLinkListObj *llobj,
                                         VpsUtils_Node *node)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != llobj));
    GT_assert(VpsUtilsTrace, (NULL != node));

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
static VpsUtils_Node *dutilsUnLinkDouble(VpsUtilsLinkListObj *llobj,
                                              VpsUtils_NodDir dir,
                                              VpsUtils_Node *nodeToBeRem)
{
    VpsUtils_Node *node = NULL, *headNode, *tailNode, *listNode;
    UInt32 nodeCnt;
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != llobj));

    if (NULL == nodeToBeRem)
    {
        if (NULL == llobj->headNode && NULL == llobj->tailNode)
        {
            node = NULL;
        }
         /* Check for the last node in the list */
        else if ((NULL != llobj->headNode) &&
            (NULL == llobj->headNode->next && NULL == llobj->headNode->prev))
        {
            node = llobj->headNode;
            llobj->tailNode = NULL;
            llobj->headNode = NULL;
        }
        else if (NULL != llobj->headNode &&
                 NULL != llobj->tailNode &&
                 VPSUTILS_NODE_DIR_HEAD == dir)
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
                 NULL != llobj->tailNode &&
                 VPSUTILS_NODE_DIR_TAIL == dir)
        {
            /* point to the tail node. */
            tailNode = llobj->tailNode;
            /* tailnode of the list points to the previous node */
            llobj->tailNode = tailNode->prev;
            /* next node of the previous node points to the tail node of list */
            if (NULL != llobj->tailNode)
            {
                llobj->tailNode->next = NULL;
            }
            else
            {
                 llobj->headNode = NULL;
            }
            /*  return the tail node */
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
UInt32 VpsUtils_getNodeCnt(VpsUtils_Handle handle)
{
    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != handle));

    return(((VpsUtilsLinkListObj *) handle)->numElements);
}

/**
 *  dutilsLinkDouble
 *  \brief Links a node to a double link list either at head of the list or
 *  at tail of the list.
 *
 *
 */
static VpsUtils_Node *dutilsLinkDouble(VpsUtilsLinkListObj *llobj,
                                            VpsUtils_Node *node,
                                            Void *data,
                                            VpsUtils_NodDir dir)
{
    VpsUtils_Node *headNode, *tailNode = NULL;

    /* NULL pointer check */
    GT_assert(VpsUtilsTrace, (NULL != node));
    GT_assert(VpsUtilsTrace, (NULL != llobj));

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
             NULL != llobj->tailNode &&
             VPSUTILS_NODE_DIR_HEAD == dir)
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
             NULL != llobj->tailNode &&
             VPSUTILS_NODE_DIR_TAIL == dir)
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
