/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiWrbkPath.h
 *
 *  \brief VPS Core header file for DEI HQ/ DEI write back path.
 *  This core is used by both writeback capture as well as in M2M drivers.
 *  In capture mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_DEIWRBKPATH_H
#define _VPSCORE_DEIWRBKPATH_H

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

/** \brief Instance ID for WB0 path. */
#define VCORE_DWP_INST_WB0              (0u)
/** \brief Instance ID for WB1 path. */
#define VCORE_DWP_INST_WB1              (1u)
/**
 *  \brief Maximum number of instance. Change this macro accordingly when
 *  number of instance is changed.
 */
#define VCORE_DWP_NUM_INST              (2u)

/** \brief Maximum number of handles supported per DWP path instance. */
#define VCORE_DWP_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channels supported for all handles.
 *  When the core is configured in capture mode, this is always 1.
 *  This is only used in mem-mem driver mode where software context makes sense.
 */
#define VCORE_DWP_MAX_CHANNELS          (40u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_DwpInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  instId;
    /**< DWP Path Instance. */
    UInt32                  maxHandle;
    /**< Maximum number of handles/channels to be supported per instance. */
    VpsHal_VpdmaChannel     vpdmaCh;
    /**< DEI-SC writeback VPDMA channel number required by core. */
} Vcore_DwpInitParams;

/**
 *  struct Vcore_DwpRtParams
 *  \brief DWP runtime configuration parameters.
 */
typedef struct
{
    Vps_FrameParams        *outFrmPrms;
    /**< Frame params for DEI writeback output frame.
         Pass NULL if no change is required or this output is not used
         by an instance. */
} Vcore_DwpRtParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vcore_dwpInit
 *  \brief DWP core init function.
 *  Initializes DWP core objects, allocates memory etc.
 *  This function should be called before calling any of DWP core API's.
 *
 *  \param numInst      [IN] Number of instance objects to be initialized.
 *  \param initPrms     [IN] Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstance.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_dwpInit(UInt32 numInst,
                    const Vcore_DwpInitParams *initPrms,
                    Ptr arg);

/**
 *  Vcore_dwpDeInit
 *  \brief DWP core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_dwpDeInit(Ptr arg);

/**
 *  Vcore_dwpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_dwpGetCoreOps(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_DEIWRBKPATH_H */
