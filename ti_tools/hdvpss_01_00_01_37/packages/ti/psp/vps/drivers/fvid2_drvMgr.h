/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file fvid2_drvMgr.h
 *
 *  \brief VPS FVID2 driver manager header file.
 *  This file exposes internal functions of driver management functionality.
 *
 */

#ifndef _FVID2_DRVMGR_H
#define _FVID2_DRVMGR_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief FVID2 Driver handle. */
typedef Ptr Fdrv_Handle;

/** \brief Typedef for callback function parameters. */
typedef struct FVID2_DrvCbParams_t FVID2_DrvCbParams;

/**
 *  \brief Typedef for FVID2 driver callback function prototype. This will be
 *  called by the driver and then the FVID2 driver manager will route the
 *  callback to the application.
 *
 *  fdmData: FVID2 driver manager internal data passed to driver during create
 *  call.
 *
 *  reserved: For future use. Not used currently. This will be set to NULL.
 */
typedef Int32 (*FVID2_FdmCbFxn) (Ptr fdmData, Ptr reserved);

/**
 *  \brief Typedef for FVID2 driver error callback function prototype.
 *  This will be called by the driver and then the FVID2 driver manager
 *  will route the error callback to the application.
 *
 *  fdmData: FVID2 driver manager internal data passed to driver during create
 *  call.
 *
 *  errList: Error data passed to the application.
 *
 *  reserved: For future use. Not used currently. This will be set to NULL.
 */
typedef Int32 (*FVID2_FdmErrCbFxn) (Ptr fdmData, Void *errList, Ptr reserved);


/** \brief Typedef for FVID2 create function pointer. */
typedef Fdrv_Handle (*FVID2_DrvCreate) (UInt32 drvId,
                                        UInt32 instanceId,
                                        Ptr createArgs,
                                        Ptr createStatusArgs,
                                        const FVID2_DrvCbParams *fdmCbParams);

/** \brief Typedef for FVID2 delete function pointer. */
typedef Int32 (*FVID2_DrvDelete) (Fdrv_Handle handle, Ptr deleteArgs);

/** \brief Typedef for FVID2 control function pointer. */
typedef Int32 (*FVID2_DrvControl) (Fdrv_Handle handle,
                                   UInt32 cmd,
                                   Ptr cmdArgs,
                                   Ptr cmdStatusArgs);

/** \brief Typedef for FVID2 queue function pointer. */
typedef Int32 (*FVID2_DrvQueue) (Fdrv_Handle handle,
                                 FVID2_FrameList *frameList,
                                 UInt32 streamId);

/** \brief Typedef for FVID2 dequeue function pointer. */
typedef Int32 (*FVID2_DrvDequeue) (Fdrv_Handle handle,
                                   FVID2_FrameList *frameList,
                                   UInt32 streamId,
                                   UInt32 timeout);

/** \brief Typedef for FVID2 process frames function pointer. */
typedef Int32 (*FVID2_DrvProcessFrames) (Fdrv_Handle handle,
                                         FVID2_ProcessList *processList);

/** \brief Typedef for FVID2 get processed frames function pointer. */
typedef Int32 (*FVID2_DrvGetProcessedFrames) (Fdrv_Handle handle,
                                              FVID2_ProcessList *processList,
                                              UInt32 timeout);


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct FVID2_DrvCbParams_t
 *  \brief Structure for setting callback function parameters.
 */
struct FVID2_DrvCbParams_t
{
    FVID2_FdmCbFxn      fdmCbFxn;
    /**< FDM callback function used by the driver to initimate any
         operation has completed or not. */
    FVID2_FdmErrCbFxn   fdmErrCbFxn;
    /**< FDM error callback function used by the driver to initimate
         any error occuring at the time of streaming. */
    Void               *errList;
    /**< Pointer to a valid framelist or processlist where the driver
         copies the aborted/error packet. */
    Ptr                 fdmData;
    /**< FDM specific data which is returned in the callback function
         as it is. */
    Ptr                 reserved;
    /**< For future use. Not used currently. Set this to NULL. */
};

/**
 *  struct FVID2_DrvOps
 *  \brief Structure to store driver function pointers.
 */
typedef struct
{
    UInt32                          drvId;
    /**< Unique driver Id. */
    FVID2_DrvCreate                 create;
    /**< FVID2 create function pointer. */
    FVID2_DrvDelete                 delete;
    /**< FVID2 delete function pointer. */
    FVID2_DrvControl                control;
    /**< FVID2 control function pointer. */
    FVID2_DrvQueue                  queue;
    /**< FVID2 queue function pointer. */
    FVID2_DrvDequeue                dequeue;
    /**< FVID2 dequeue function pointer. */
    FVID2_DrvProcessFrames          processFrames;
    /**< FVID2 process frames function pointer. */
    FVID2_DrvGetProcessedFrames     getProcessedFrames;
    /**< FVID2 get processed frames function pointer. */
} FVID2_DrvOps;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  FVID2_registerDriver
 *  \brief FVID2 register driver function.
 *
 *  This function registers a driver with the FVID2 driver manager.
 *
 *  \param drvOps       Driver function table pointer containing driver
 *                      function pointers and driver name. The driver name
 *                      should be unique - two or more drivers can't have the
 *                      same driver name.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 FVID2_registerDriver(const FVID2_DrvOps *drvOps);

/**
 *  FVID2_unRegisterDriver
 *  \brief FVID2 unregister driver function.
 *
 *  This function unregisters a driver from the FVID2 driver manager.
 *
 *  \param drvOps       Driver function table pointer containing driver
 *                      function pointers and driver name.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 FVID2_unRegisterDriver(const FVID2_DrvOps *drvOps);

Int32 FVID2_checkFrameList(const FVID2_FrameList *frameList, UInt32 maxFrames);
Int32 FVID2_checkDqFrameList(const FVID2_FrameList *frameList,
                             UInt32 maxFrames);
Int32 FVID2_checkProcessList(const FVID2_ProcessList *processList,
                             UInt32 inLists,
                             UInt32 outLists,
                             UInt32 maxFrames,
                             Bool isMosaicMode);

Void FVID2_copyFrameList(FVID2_FrameList *dest, FVID2_FrameList *src);
Void FVID2_duplicateFrameList(FVID2_FrameList *dest, FVID2_FrameList *src);
Void FVID2_copyProcessList(FVID2_ProcessList *dest, FVID2_ProcessList *src);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _FVID2_DRVMGR_H */
