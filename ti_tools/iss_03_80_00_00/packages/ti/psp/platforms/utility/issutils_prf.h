/** ==================================================================
 *  @file   issutils_prf.h                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \ingroup ISSUTILS_API
    \defgroup ISSUTILS_PRF_API Profiling API
    @{
*/

/**
    \file issutils_prf.h
    \brief Profiling API
*/

/* 
 * - APIs to measure and print elasped time @ 64-bit precision -
 * IssUtils_prfTsXxxx
 * 
 * - APIs to measure and print CPU load at task, HWI, SWI, global level -
 * IssUtils_prfLoadXxxx */

#ifndef _ISSUTILS_PRF_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSUTILS_PRF_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/psp/iss/iss.h>

/** \brief Maximum supported profiling objects */
#define ISSUTILS_PRF_MAX_HNDL     (64)

/**
  \brief Profiling load object
*/
typedef struct {
    Int32 cpuLoad;
    /**< CPU load  */
    Int32 hwiLoad;
    /**< HWI load  */
    Int32 swiLoad;
    /**< SWI load  */
    Int32 tskLoad;
    /**< TSK load  */
} IssUtils_PrfLoad;

/**
  \brief Typedef for the loadupdate function for the user
*/
typedef Void(*IssUtils_loadUpdate) (IssUtils_PrfLoad *);

/**
  \brief Profiling time-stamp object
*/
typedef struct {
    char name[32];
    /**< Name  */
    Bool isAlloc;
    /**< Indicates if the object is initialized (used). */
    UInt64 startTs;
    /**< Start time stamp value */
    UInt64 totalTs;
    /**< Total time stamp value */
    UInt32 count;
    /**< Number of times time stamp delta is calculated */
    UInt32 numFrames;
    /**< Total number of frames being used for time-stamping */
} IssUtils_PrfTsHndl;

/**
 *  \brief Update the CPU load information for all profile handles
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadUpdate                                               
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
Void IssUtils_prfLoadUpdate();

/**
 *  \brief Initializes the profiling utility.
 *
 *  This function must be called before using any peformance or Timestamp utils
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfInit                                               
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
Int32 IssUtils_prfInit();

/**
 *  \brief De-initializes the profiling utility.
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfDeInit                                               
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
Int32 IssUtils_prfDeInit();

/**
 *  \brief Creates the handle for the time stamp taking.
 *
 *  \param name     [IN] Name of the time stamp object
 *
 *  \return Valid handle on success, else NULL
*/
/* ===================================================================
 *  @func     IssUtils_prfTsCreate                                               
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
IssUtils_PrfTsHndl *IssUtils_prfTsCreate(char *name);

/**
 *  \brief Deletes the handle for the timestamp.
 *
 *  \param pHndl    [IN] Handle to be deleted
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfTsDelete                                               
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
Int32 IssUtils_prfTsDelete(IssUtils_PrfTsHndl * pHndl);

/**
 *  \brief Start taking the timestamp.
 *
 *  \param pHndl    [IN] Handle to be time stamp object
 *
 *  \return Initial time stamp value on success
*/
/* ===================================================================
 *  @func     IssUtils_prfTsBegin                                               
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
UInt64 IssUtils_prfTsBegin(IssUtils_PrfTsHndl * pHndl);

/**
 *  \brief Stop taking the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
*/
/* ===================================================================
 *  @func     IssUtils_prfTsEnd                                               
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
UInt64 IssUtils_prfTsEnd(IssUtils_PrfTsHndl * pHndl, UInt32 numFrames);

/**
 *  \brief Calculates the difference between the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param startTime [IN] Start time stamp value
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
*/
/* ===================================================================
 *  @func     IssUtils_prfTsDelta                                               
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
UInt64 IssUtils_prfTsDelta(IssUtils_PrfTsHndl * pHndl, UInt64 startTime,
                           UInt32 numFrames);

/**
 *  \brief Resets the timestamp counter for that handle.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfTsReset                                               
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
Int32 IssUtils_prfTsReset(IssUtils_PrfTsHndl * pHndl);

/**
 *  \brief Gets the 64-bit timer ticks
 *
 *  \return Current 64-bit timer ticks value on success
*/
UInt64 IssUtils_prfTsGet64();

/**
 *  \brief Prints the timestamp difference and resets the counter thereafter
 *         depending on specified resetAfterPrint parameter.
 *
 *  \param pHndl           [IN] Handle to be time stamp object
 *  \param resetAfterPrint [IN] Indicates whether time stamp values should be
 *                              reset after printing
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfTsPrint                                               
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
Int32 IssUtils_prfTsPrint(IssUtils_PrfTsHndl * pHndl, Bool resetAfterPrint);

/**
 *  \brief Prints the timestamp difference for all registered handles and resets
 *         the counter thereafter depending on specified resetAfterPrint
 *         parameter.
 *
 *  \param resetAfterPrint [IN] Indicates whether time stamp values should be
 *                              reset after printing
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfTsPrintAll                                               
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
Int32 IssUtils_prfTsPrintAll(Bool resetAfterPrint);

/**
 *  \brief Registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be registered for load
 *                              calculation
 *  \param name            [IN] Name to be associated with the registered handle
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadRegister                                               
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
Int32 IssUtils_prfLoadRegister(Task_Handle pTsk, char *name);

/**
 *  \brief Un-registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be unregistered for
 *                              load calculation
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadUnRegister                                               
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
Int32 IssUtils_prfLoadUnRegister(Task_Handle pTsk);

/**
 *  \brief Prints loads for all the registered tasks. Also prints information
 *         for each task depending on the specified printTskLoad parameter.
 *
 *  \param printTskLoad    [IN] Indicates whether load information for each
 *                              registered task should be printed.
 *
 *  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadPrintAll                                               
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
Int32 IssUtils_prfLoadPrintAll(Bool printTskLoad);

/**
 *  \brief Start taking the performance load for all the registered tasks.
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadCalcStart                                               
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
Void IssUtils_prfLoadCalcStart();

/**
 *  \brief Stop taking the load for all the registered tasks.
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadCalcStop                                               
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
Void IssUtils_prfLoadCalcStop();

/**
 *  \brief Reset the load calculation mainly for next cycle of run.
*/
/* ===================================================================
 *  @func     IssUtils_prfLoadCalcReset                                               
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
Void IssUtils_prfLoadCalcReset();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* ifndef
                                                            * _ISSUTILS_PRF_H_ 
                                                            */

/* @} */
