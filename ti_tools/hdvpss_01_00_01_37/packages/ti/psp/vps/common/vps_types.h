/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vps_types.h
 *
 *  \brief VPS header file containing commonly used data types and macros.
 *
 */

#ifndef _VPS_TYPES_H
#define _VPS_TYPES_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Typedef for HAL handle
 */
typedef Void *VpsHal_Handle;

/** \brief Typedef for the VPS HAL get config overlay size function pointer. */
typedef UInt32 (*VpsHal_GetCfgOvlySize) (VpsHal_Handle handle);
/** \brief Typedef for the VPS HAL create config overlay function pointer. */
typedef Int32 (*VpsHal_CreateCfgOvly) (VpsHal_Handle handle, Ptr configOvlyPtr);

/**
 *  enum Vps_ScanFormat
 *  \brief Enum for scan format.
 */
typedef enum
{
    VPS_SF_INTERLACED = FVID2_SF_INTERLACED,
    /**< Interlaced mode. */
    VPS_SF_PROGRESSIVE = FVID2_SF_PROGRESSIVE
    /**< Progressive mode. */
} Vps_ScanFormat;

/*
 * =========== Error codes returned by VPS functions =============
 */
#define VPS_SOK                         (FVID2_SOK)
#define VPS_EFAIL                       (FVID2_EFAIL)
#define VPS_EBADARGS                    (FVID2_EBADARGS)
#define VPS_EINVALID_PARAMS             (FVID2_EINVALID_PARAMS)
#define VPS_EDEVICE_INUSE               (FVID2_EDEVICE_INUSE)
#define VPS_ETIMEOUT                    (FVID2_ETIMEOUT)
#define VPS_EALLOC                      (FVID2_EALLOC)
#define VPS_EOUT_OF_RANGE               (FVID2_EOUT_OF_RANGE)
#define VPS_EAGAIN                      (FVID2_EAGAIN)
#define VPS_EUNSUPPORTED_CMD            (FVID2_EUNSUPPORTED_CMD)
#define VPS_ENO_MORE_BUFFERS            (FVID2_ENO_MORE_BUFFERS)
#define VPS_EUNSUPPORTED_OPS            (FVID2_EUNSUPPORTED_OPS)
#define VPS_EDRIVER_INUSE               (FVID2_EDRIVER_INUSE)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_TYPES_H */
