/** ==================================================================
 *  @file   iss_utils.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/common/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file iss_utils.h
 *
 *  \brief ISS Utility functions header file
 *  This file declares the functions required to create, add and remove nodes.
 *  Also provides various memset functions.
 *
 */

#ifndef _ISS_UTILS_H
#define _ISS_UTILS_H

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/iss/common/trace.h>
#include <ti/psp/iss/common/iss_types.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/**
 *  \brief Number of node objects to allocate statically to be used to
 *  create double link list of clients. This is used to avoid dynamic
 *  allocation as needed by link list programming and hence can be
 *  tested without malloc or OS calls. This also reduces run-time
 *  over head normally caused by dynamic allocation.
 *
 *  VEM Requirement:
 *  There could be 8 list complete interrupt registration and
 *  16 send interrupt registration. We will have some 6 more objects
 *  extra in case the requirement changes.
 */
#define ISSUTILS_NUM_NODE_OBJECTS       ((8u + 16u + 6u))

/**< \brief Typedef for ISS Utils handle. */
    typedef Void *VpsUtils_Handle;

/**< \brief Typedef for ISS Utils Queue Handle. */
    typedef VpsUtils_Handle VpsUtils_QHandle;

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

/**< \brief Typedef for Node structure. */
    typedef struct VpsUtils_Node_t VpsUtils_Node;

/**
 *  struct VpsUtils_Node_t
 *  \brief Self referential structure for double link list.
 */
    struct VpsUtils_Node_t {
        VpsUtils_Node *next;
    /**< Pointer to the next node. */
        VpsUtils_Node *prev;
    /**< Pointer to the previous node. */
        Void *data;
    /**< Node data pointer. */
        UInt32 priority;
    /**< Priority of the node. Used for priority based linked list. */
    };

/**< \brief Typedef for Queue Object. */
    typedef VpsUtils_Node VpsUtils_QElem;

/**
 *  struct VpsUtils_LinkListType
 *  \brief Enums for the type of link list to be created.
 */
    typedef enum {
        ISSUTILS_LLT_DOUBLE,
    /**< Double link list. */
        ISSUTILS_LLT_CIRCULAR
    /**< Circular link list using double link list. */
    } VpsUtils_LinkListType;

/**
 *  struct VpsUtils_LinkAddMode
 *  \brief Enums for the different modes of adding a node to a link list.
 */
    typedef enum {
        ISSUTILS_LAM_TOP,
    /**< Add nodes to the top of the list. */
        ISSUTILS_LAM_BOTTOM,
    /**< Add nodes to the bottom of the list. */
        ISSUTILS_LAM_PRIORITY
    /**< Add nodes to the list based on ascending order of priority.
         Nodes with the same priority are always added to the bottom of the
         existing nodes with same priority. */
    } VpsUtils_LinkAddMode;

/**
 *  struct VpsUtils_LinkAddMode
 *  \brief Enums for the different modes of adding a node to a link list.
 */
    typedef enum {
        ISSUTILS_NODE_DIR_HEAD,
    /**< Add nodes to the top of the list. */
        ISSUTILS_NODE_DIR_TAIL
    /**< Add nodes to the bottom of the list. */
    } VpsUtils_NodDir;

/**
 *  struct VpsUtils_PoolParams
 *  \brief Create parameters for the fixed size pool.
 */
    typedef struct {
        Void *mem;
    /**< Pointer to the pool memory. */
        UInt32 numElem;
    /**< Number of elements in the pool. */
        UInt32 elemSize;
    /**< Size of each element in bytes. */
        UInt32 *flag;
    /**< Array of flag variable used by pool manager to indicate whether a pool
         element is allocated or not. The size of this array should be
         equal to the number of elements in this pool. */
        UInt32 numFreeElem;
    /**< Count to keep track of the number of free elements in the pool. */
    } VpsUtils_PoolParams;

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

/**
 *  VpsUtils_init
 *  \brief Initializes the Util objects and create pool semaphore.
 *
 *  \param arg              Not used currently. Meant for future purpose.
 *
 *  \returns                Returns 0 on success else returns error value.
 */
    Int VpsUtils_init(Ptr arg);

/**
 *  VpsUtils_deInit
 *  \brief De-Initializes the Util objects by removing pool semaphore.
 *
 *  \param arg              Not used currently. Meant for future purpose.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
    Int VpsUtils_deInit(Ptr arg);

/**
 *  VpsUtils_memset
 *  \brief Sets the memory with the given value. Access memory as byte.
 *  Returns the memory pointer.
 *
 *  \param mem              Destination memory pointer.
 *  \param ch               Byte value to fill with.
 *  \param byteCount        Number of bytes to fill.
 *
 *  \return                 The destination memory pointer.
 */
    void *VpsUtils_memset(Ptr mem, UInt8 ch, UInt32 byteCount);

/**
 *  VpsUtils_memsetw
 *  \brief Sets the memory with the given value. Access memory as word.
 *  Hence memory pointer should be aligned to 4 byte boundary
 *  Returns the memory pointer.
 *
 *  \param mem              Destination memory pointer.
 *  \param word             Word value to fill with.
 *  \param wordCount        Number of words to fill.
 *
 *  \return                 The destination memory pointer.
 */
    void *VpsUtils_memsetw(Ptr mem, UInt32 word, UInt32 wordCount);

/**
 *  VpsUtils_memcpy
 *  \brief Copies source memory into destination memory. Access memory as byte.
 *  Returns the destination memory pointer.
 *
 *  \param dest             Destination memory pointer.
 *  \param src              Source memory pointer.
 *  \param byteCount        Number of bytes to copy.
 *
 *  \return                 The destination memory pointer.
 */
    void *VpsUtils_memcpy(Ptr dest, const Void * src, UInt32 byteCount);

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
                                            VpsUtils_LinkAddMode addMode);
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
    int VpsUtils_memcmp(const Void * mem1, const Void * mem2, UInt32 byteCount);

/**
 *  VpsUtils_deleteLinkList
 *  \brief Deletes a link list object.
 *
 *  \param handle           Link list handle.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
    Int32 VpsUtils_deleteLinkList(VpsUtils_Handle handle);

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
                              VpsUtils_Node * node, UInt32 priority);

/**
 *  VpsUtils_linkUniqePriNode
 *  \brief Very similar to VpsUtils_linkNode, except that on equal priority
 *          nodes will not be inserted. An error (ISS_EBADARGS) would be
 *          returned.
 *          Applicable for double linked list only.
 *  ToDo Update to handle circular list also.
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
                                    VpsUtils_Node * node, UInt32 priority);

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
    Int32 VpsUtils_unLinkAllNodes(VpsUtils_Handle handle);

/**
 *  VpsUtils_unLinkNodePri
 *  \brief Unlinks the node from the list. Used for the Priority linklists.
 *
 *  \param handle           Link list handle.
 *  \param node             Node pointer to be unlinked from the list.
 */
    Void VpsUtils_unLinkNodePri(VpsUtils_Handle handle, VpsUtils_Node * node);

 /**
 *  VpsUtils_unLinkNode
 *  \brief Unlinks the node from the list. Used for Non-priority linked lists
 *
 *  \param handle           Link list handle.
 *  \param node             Node pointer to be unlinked from the list.
 */
    Void VpsUtils_unLinkNode(VpsUtils_Handle handle, VpsUtils_Node * node);

/**
 *  VpsUtils_linkNodeToHead
 *  \brief                  Link the node to the head of the double linked list.
 *                          No priority
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 *
 */
    Void VpsUtils_linkNodeToHead(VpsUtils_Handle handle, VpsUtils_Node * node);

/**
 *  VpsUtils_linkNodeToTail
 *  \brief                  Link the node to the tail of the double linked list.
 *                          No priority
 *
 *  \param handle           Link list handle.
 *  \param node             Node object pointer used for linking.
 */
    Void VpsUtils_linkNodeToTail(VpsUtils_Handle handle, VpsUtils_Node * node);

/**
 *  VpsUtils_unLinkNodeFromHead
 *  \brief                  Returns the node from head. Removes the  node from
 *                          the list.
 *
 *  \param handle           Link list handle.
 *  \return                 Pointer to unlinked node.
 *
 */
    VpsUtils_Node *VpsUtils_unLinkNodeFromHead(VpsUtils_Handle handle);

/**
 *  VpsUtils_unLinkNodeFromTail
 *  \brief                  Returns the node from tail. Removes the  node from
 *                          the list.
 *
 *  \param handle           Link list handle.
 *  \return                 Pointer to unlinked node.
 *
 */
    VpsUtils_Node *VpsUtils_unLinkNodeFromTail(VpsUtils_Handle handle);

/**
 *  VpsUtils_getHeadNode
 *  \brief                  Returns the reference to the headNode. Does
 *                          not remove the node from the head.
 *
 *  \param handle           Link list handle.
 */
    VpsUtils_Node *VpsUtils_getHeadNode(VpsUtils_Handle handle);

/**
 *  VpsUtils_getTailNode
 *  \brief                  Returns the reference to the TailNode. Does
 *                          not remove the node from the head.
 *
 *  \param handle           Link list handle.
 *  \return                 Reference  to tail node.  Acutally  node is not
 *                          unlinked from list.
 *
 */
    VpsUtils_Node *VpsUtils_getTailNode(VpsUtils_Handle handle);

/**
 *  VpsUtils_isListEmpty
 *  \brief Checks whether a list is empty or not.
 *
 *  \param handle           List handle.
 *
 *  \return                 TRUE if List is empty else returns FALSE.
 */
    UInt32 VpsUtils_isListEmpty(VpsUtils_Handle handle);

/**
 *  VpsUtils_getNumNodes
 *  \brief Returns the number of nodes present in a list.
 *
 *  \param handle           List handle.
 *
 *  \return                 Number of nodes present in a list.
 */
    UInt32 VpsUtils_getNumNodes(VpsUtils_Handle handle);

/**
 *  VpsUtils_getNodeCnt
 *  \brief Returns the number of nodes in the link list
 *
 *  \param handle           Link list handle.

 *
 *  \return                 Returns the number of nodes in the list.
 */
    UInt32 VpsUtils_getNodeCnt(VpsUtils_Handle handle);
/**
 *  VpsUtils_createQ
 *  \brief Creates a Queue object.
 *
 *  \return                 List handle if success else returns NULL.
 */
    VpsUtils_QHandle VpsUtils_createQ(void);

/**
 *  VpsUtils_deleteQ
 *  \brief Deletes a Queue object.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
    Int32 VpsUtils_deleteQ(VpsUtils_QHandle handle);

/**
 *  VpsUtils_queue
 *  \brief Adds the data to the queue.
 *  The memory to the node object should be allocated by the caller.
 *
 *  \param handle           Queue handle.
 *  \param qElem            Queue Element object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 */
    Void VpsUtils_queue(VpsUtils_QHandle handle,
                        VpsUtils_QElem * qElem, Ptr data);

/**
 *  VpsUtils_dequeue
 *  \brief Removes a element from the queue.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Returns the removed data pointer.
 */
    Void *VpsUtils_dequeue(VpsUtils_QHandle handle);
/**
 *  VpsUtils_queueBack
 *  \brief Adds the data to start of the queue.
 *  The memory to the node object should be allocated by the caller.
 *
 *  \param handle           Queue handle.
 *  \param qElem            Queue Element object pointer used for linking.
 *  \param data             Data pointer to be added to the list.
 *                          This should be unique.
 */
    Void VpsUtils_queueBack(VpsUtils_QHandle handle,
                            VpsUtils_QElem * qElem, Void * data);

/**
 *  VpsUtils_isQEmpty
 *  \brief Checks whether a queue is empty or not.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 TRUE if queue is empty else returns FALSE.
 */
    UInt32 VpsUtils_isQEmpty(VpsUtils_QHandle handle);

/**
 *  VpsUtils_getNumQElem
 *  \brief Returns the number of Queue Elements present in a Queue.
 *
 *  \param handle           Queue handle.
 *
 *  \return                 Number of Queue Elements present in a Queue.
 */
    UInt32 VpsUtils_getNumQElem(VpsUtils_QHandle handle);

/**
 *  VpsUtils_initPool
 *  \brief Initializes the pool object structure.
 *
 *  \param params           Pool parameters to be initialized.
 *  \param mem              Pointer to the pool memory.
 *  \param numElem          Number of elements in the pool.
 *  \param elemSize         Size of each element in bytes.
 *  \param flag             Array of flag variable used by pool manager to
 *                          indicate whether a pool element is allocated
 *                          or not. The size of this array should be
 *                          equal to the number of elements in this pool.
 *  \param traceMask        Trace mask used in trace prints.
 */
    static inline Void VpsUtils_initPool(VpsUtils_PoolParams * params,
                                         Void * mem,
                                         UInt32 numElem,
                                         UInt32 elemSize,
                                         UInt32 * flag, UInt32 traceMask) {
        UInt32 cnt;

        /* NULL pointer check */
         GT_assert(traceMask, (NULL != params));
         GT_assert(traceMask, (NULL != mem));
         GT_assert(traceMask, (NULL != flag));

        /* Init pool parameters */
         params->mem = mem;
         params->numElem = numElem;
         params->elemSize = elemSize;
         params->flag = flag;
         params->numFreeElem = numElem;

        /* Set pool flags as free */
        for (cnt = 0u; cnt < params->numElem; cnt++)
        {
            params->flag[cnt] = FALSE;
        } return;
    }

/**
 *  VpsUtils_alloc
 *  \brief Allocates one element from the pool.
 *
 *  \param params           Pool parameters.
 *  \param size             Size in bytes to allocate.
 *  \param traceMask        Trace mask used in trace prints.
 *
 *  \return                 Returns memory pointer on success else returns
 *                          NULL.
 */
    static inline Void *VpsUtils_alloc(VpsUtils_PoolParams * params,
                                       UInt32 size, UInt32 traceMask) {
        UInt32 cnt;

        UInt32 cookie;

        Void *allocMem = NULL;

        /* NULL pointer check */
        GT_assert(traceMask, (NULL != params));
        /* Check if the requested size if within each fixed size element */
        GT_assert(traceMask, (size <= params->elemSize));
        GT_assert(traceMask, (0u != size));

        /* Disable global interrupts */
        cookie = Hwi_disable();

        for (cnt = 0u; cnt < params->numElem; cnt++)
        {
            if (FALSE == params->flag[cnt])
            {
                allocMem =
                    (Void *) ((UInt32) params->mem + params->elemSize * cnt);
                params->flag[cnt] = TRUE;

                /* Decrement free count. Assert if it is zero as it can never 
                 * happen. */
                GT_assert(traceMask, (0u != params->numFreeElem));
                params->numFreeElem--;
                break;
            }
        }

        /* Restore global interrupts */
        Hwi_restore(cookie);

        return (allocMem);
    }

/**
 *  VpsUtils_free
 *  \brief Frees the element and returns to the pool.
 *
 *  \param params           Pool parameters.
 *  \param mem              Memory pointer to deallocate.
 *  \param traceMask        Trace mask used in trace prints.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
    static inline Int32 VpsUtils_free(VpsUtils_PoolParams * params,
                                      const Void * mem, UInt32 traceMask) {
        UInt32 cnt;

        Int32 retVal = ISS_EFAIL;

        UInt32 cookie;

        /* NULL pointer check */
        GT_assert(traceMask, (NULL != params));
        GT_assert(traceMask, (NULL != mem));

        /* Disable global interrupts */
        cookie = Hwi_disable();

        for (cnt = 0u; cnt < params->numElem; cnt++)
        {
            if ((Void *) ((UInt32) params->mem + params->elemSize * cnt) == mem)
            {
                /* Check if the memory is already allocated */
                GT_assert(traceMask, (TRUE == params->flag[cnt]));
                params->flag[cnt] = FALSE;

                /* Increment free count. Assert if it is more than num
                 * elements as it can never happen. */
                params->numFreeElem++;
                GT_assert(traceMask, (params->numFreeElem <= params->numElem));

                retVal = ISS_SOK;
                break;
            }
        }

        /* Restore global interrupts */
        Hwi_restore(cookie);

        return (retVal);
    }

#ifdef __cplusplus
}
#endif

#endif                                                     /* #ifndef
                                                            * _ISS_UTILS_H */
