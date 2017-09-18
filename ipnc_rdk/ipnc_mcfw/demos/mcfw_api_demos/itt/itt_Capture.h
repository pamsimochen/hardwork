/** ==================================================================
 *  @file   itt_Capture.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/demos/mcfw_api_demos/itt/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*
 * Notify from A8 to VPSS M3
 */

#ifndef _ITT_CAPTURE_H_
#define _ITT_CAPTURE_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <osa_thr.h>

#define ITT_ERROR(...) \
  fprintf(stderr, " ERROR  (%s|%s|%d): ", __FILE__, __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__);

#define ITT_STATUS_OK      0                               // /< Status : OK
#define ITT_STATUS_EFAIL   -1                              // /< Status :
                                                           // Generic error
    /* SWOSD Timer Thread priority and stack size */
#define ITT_TSK_PRI					(7)
#define ITT_TSK_STACK_SIZE			(32)

    typedef Void *(*ThrEntryFunc) (Void *);

    typedef pthread_t ThrId;

    typedef struct _TaskCtx {
        ThrId handle;
        Int8 exitFlag;
    } TaskCtx;

    typedef struct {

        pthread_t *hndl;
        pthread_attr_t *thread_attr;
        // OSAL_TaskEntryFunc entryFunc;
        unsigned int pri;
        unsigned int stackSize;

    } OSAL_TaskHandle;

    Int32 Itt_thrCreate(TaskCtx * ctx, ThrEntryFunc entryFunc, Uint32 pri,
                        Uint32 stackSize);

    Int32 Itt_thrDelete(TaskCtx * ctx);

    Int32 Itt_TaskCreate();

    int Itt_TaskDelete();

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* _DM81XX_A8_NOTIFY_H_
                                                            */
