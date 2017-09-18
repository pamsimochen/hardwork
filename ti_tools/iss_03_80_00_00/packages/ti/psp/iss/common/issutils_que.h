/** ==================================================================
 *  @file   issutils_que.h                                                  
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
  \file issutils_que.h
  \brief Utils layer - Array based queue
*/

#ifndef _ISSUTILS_QUE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSUTILS_QUE_H_

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

/**
  \ingroup ISS_DRV_UTIL_API
  \defgroup ISS_DRV_UTIL_AQUE_API Utils layer - Array based queue

  Implementation of a array based queue with support for optional
  blocking on queue empty or queue full.
  Interally takes care of critical section locking so that it can be
  used from task, ISR context without any additional mutex logic.

  @{
*/

/** Do not block on que get and que put,
  returns error if que is empty or full respectively
*/
#define ISSUTILS_QUE_FLAG_NO_BLOCK_QUE    (0x00000000)

/** Block on que put if que is full */
#define ISSUTILS_QUE_FLAG_BLOCK_QUE_PUT   (0x00000001)

/** Block on que get if que is empty */
#define ISSUTILS_QUE_FLAG_BLOCK_QUE_GET   (0x00000002)

/** Block on que put if que is full, Block on que get if que is empty  */
#define ISSUTILS_QUE_FLAG_BLOCK_QUE       (0x00000003)

/**
  \brief Queue Handle

  Typically user does not need to know internals of queue handle
  data structure
*/
typedef struct {
    UInt32 curRd;
  /**< Current read index */

    UInt32 curWr;
  /**< Current write index  */

    UInt32 count;
  /**< Count of element in queue  */

    UInt32 maxElements;
  /**< Max elements that be present in the queue  */

    Ptr *queue;
  /**< Address of data area of the queue elements */

    Semaphore_Handle semRd;
  /**< Read semaphore */

    Semaphore_Handle semWr;
  /**< Write semaphore  */

    UInt32 flags;
  /**< Controls how APIs behave internally,
    i.e blocking wait or non-blocking */

} VpsUtils_QueHandle;

/**
  \brief Create a queue handle

  The size of queueMem llocate by the user should be maxElements*sizeof(Ptr)

  \param  handle        [ O] Initialized queue handle
  \param  maxElements   [I ] Maximum elements that can reside in the queue
                          at any given point of time
  \param  queueMem      [I ] Address of queue element data area
  \param  flags         [I ] ISSUTILS_QUE_FLAG_xxxx

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     VpsUtils_queCreate                                               
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
Int32 VpsUtils_queCreate(VpsUtils_QueHandle * handle,
                         UInt32 maxElements, Ptr queueMem, UInt32 flags);

/**
  \brief Delete queue handle

  Releases all resources allocated during queue create

  \param  handle        [I ] Queue handle

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     VpsUtils_queDelete                                               
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
Int32 VpsUtils_queDelete(VpsUtils_QueHandle * handle);

/**
  \brief Add a element into the queue

  \param handle   [I ] Queue Handle
  \param data     [I ] data element to insert
  \param timeout  [I ] BIOS_NO_WAIT: non-blocking,
                        if queue is full error is returned \n
                       BIOS_WAIT_FOREVER: Blocking,
                        if queue is full function blocks until
                        atleast one element in the queue is free
                        for inserting new element

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     VpsUtils_quePut                                               
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
Int32 VpsUtils_quePut(VpsUtils_QueHandle * handle, Ptr data, Int32 timeout);

/**
  \brief Get a element from the queue

  \param handle   [I ] Queue Handle
  \param data     [ O] extracted data element from the queue
  \param minCount [I ] Data will be extracted only if
                        atleast 'minCount' elements are present in the queue
  \param timeout  [I ] BIOS_NO_WAIT: non-blocking,
                       if queue is empty error is returned \n
                       BIOS_WAIT_FOREVER: Blocking, if queue is
                       empty function blocks until
                       atleast 'minCount' elemetns in the queue are available

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     VpsUtils_queGet                                               
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
Int32 VpsUtils_queGet(VpsUtils_QueHandle * handle,
                      Ptr * data, UInt32 minCount, Int32 timeout);

/**
  \brief Peek at the first element from the queue, but do not extract it

  \param handle   [I ] Queue Handle
  \param data     [ O] "peeked" data element from the queue

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     VpsUtils_quePeek                                               
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
Int32 VpsUtils_quePeek(VpsUtils_QueHandle * handle, Ptr * data);

/**
  \brief Returns TRUE is queue is empty else retunrs false

  \param handle   [I ] Queue Handle

  \return Returns TRUE is queue is empty else retunrs FALSE
*/
/* ===================================================================
 *  @func     VpsUtils_queIsEmpty                                               
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
UInt32 VpsUtils_queIsEmpty(VpsUtils_QueHandle * handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
