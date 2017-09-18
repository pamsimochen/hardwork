/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_PRF_API Profiling API
    @{
*/

/**
    \file vpsutils_prf.h
    \brief Profiling API
*/


/*
    - APIs to measure and print elasped time @ 64-bit precision
        - VpsUtils_prfTsXxxx

    - APIs to measure and print CPU load at task, HWI, SWI, global level
        - VpsUtils_prfLoadXxxx
*/

#ifndef _VPSUTILS_PRF_H_
#define _VPSUTILS_PRF_H_


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/psp/vps/vps.h>

/** \brief Maximum supported profiling objects */
#define VPSUTILS_PRF_MAX_HNDL     (64)

/**
  \brief Profiling load object
*/
typedef struct {
    Int32       cpuLoad;
    /**< CPU load  */
    Int32       hwiLoad;
    /**< HWI load  */
    Int32       swiLoad;
    /**< SWI load  */
    Int32       tskLoad;
    /**< TSK load  */
}VpsUtils_PrfLoad;

/**
  \brief Typedef for the loadupdate function for the user
*/
typedef Void (*VpsUtils_loadUpdate) (VpsUtils_PrfLoad *);

/**
  \brief Profiling time-stamp object
*/
typedef struct {
    char                    name[32];
    /**< Name  */
    Bool                    isAlloc;
    /**< Indicates if the object is initialized (used). */
    UInt64                  startTs;
    /**< Start time stamp value */
    UInt64                  totalTs;
    /**< Total time stamp value */
    UInt32                  count;
    /**< Number of times time stamp delta is calculated */
    UInt32                  numFrames;
    /**< Total number of frames being used for time-stamping */
} VpsUtils_PrfTsHndl;

/**
 *  \brief Update the CPU load information for all profile handles
*/
Void VpsUtils_prfLoadUpdate();

/**
 *  \brief Initializes the profiling utility.
 *
 *  This function must be called before using any peformance or Timestamp utils
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfInit();

/**
 *  \brief De-initializes the profiling utility.
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfDeInit();

/**
 *  \brief Creates the handle for the time stamp taking.
 *
 *  \param name     [IN] Name of the time stamp object
 *
 *  \return Valid handle on success, else NULL
*/
VpsUtils_PrfTsHndl *VpsUtils_prfTsCreate(char *name);

/**
 *  \brief Deletes the handle for the timestamp.
 *
 *  \param pHndl    [IN] Handle to be deleted
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfTsDelete(VpsUtils_PrfTsHndl *pHndl);

/**
 *  \brief Start taking the timestamp.
 *
 *  \param pHndl    [IN] Handle to be time stamp object
 *
 *  \return Initial time stamp value on success
*/
UInt64 VpsUtils_prfTsBegin(VpsUtils_PrfTsHndl *pHndl);

/**
 *  \brief Stop taking the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
*/
UInt64 VpsUtils_prfTsEnd(VpsUtils_PrfTsHndl *pHndl, UInt32 numFrames);

/**
 *  \brief Calculates the difference between the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param startTime [IN] Start time stamp value
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
*/
UInt64 VpsUtils_prfTsDelta(VpsUtils_PrfTsHndl *pHndl, UInt64 startTime, UInt32 numFrames);

/**
 *  \brief Resets the timestamp counter for that handle.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfTsReset(VpsUtils_PrfTsHndl *pHndl);

/**
 *  \brief Gets the 64-bit timer ticks
 *
 *  \return Current 64-bit timer ticks value on success
*/
UInt64 VpsUtils_prfTsGet64();

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
Int32 VpsUtils_prfTsPrint(VpsUtils_PrfTsHndl *pHndl, Bool resetAfterPrint);

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
Int32 VpsUtils_prfTsPrintAll(Bool resetAfterPrint);

/**
 *  \brief Registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be registered for load
 *                              calculation
 *  \param name            [IN] Name to be associated with the registered handle
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfLoadRegister(Task_Handle pTsk,
                               char *name);
/**
 *  \brief Un-registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be unregistered for
 *                              load calculation
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfLoadUnRegister(Task_Handle pTsk);

/**
 *  \brief Prints loads for all the registered tasks. Also prints information
 *         for each task depending on the specified printTskLoad parameter.
 *
 *  \param printTskLoad    [IN] Indicates whether load information for each
 *                              registered task should be printed.
 *
 *  \return 0 on success, else failure
*/
Int32 VpsUtils_prfLoadPrintAll(Bool printTskLoad);

/**
 *  \brief Start taking the performance load for all the registered tasks.
*/
Void VpsUtils_prfLoadCalcStart();

/**
 *  \brief Stop taking the load for all the registered tasks.
*/
Void VpsUtils_prfLoadCalcStop();

/**
 *  \brief Reset the load calculation mainly for next cycle of run.
*/
Void  VpsUtils_prfLoadCalcReset();

#endif /* ifndef _VPSUTILS_PRF_H_ */

/*@}*/
