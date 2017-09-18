/** ==================================================================
 *  @file   iss_types.h                                                  
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
 *  \file iss_types.h
 *
 *  \brief ISS header file containing commonly used data types and macros.
 *
 */

#ifndef _ISS_TYPES_H
#define _ISS_TYPES_H

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/vps/fvid2.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/**
 *  \brief Typedef for HAL handle
 */
    typedef Void *IssHal_Handle;

/** \brief Typedef for the ISS HAL get config overlay size function pointer. */
    typedef UInt32(*IssHal_GetCfgOvlySize) (IssHal_Handle handle);
/** \brief Typedef for the ISS HAL create config overlay function pointer. */
    typedef Int32(*IssHal_CreateCfgOvly) (IssHal_Handle handle,
                                          Ptr configOvlyPtr);

/**
 *  enum Iss_ScanFormat
 *  \brief Enum for scan format.
 */
    typedef enum {
        ISS_SF_INTERLACED = FVID2_SF_INTERLACED,
    /**< Interlaced mode. */
        ISS_SF_PROGRESSIVE = FVID2_SF_PROGRESSIVE
    /**< Progressive mode. */
    } Iss_ScanFormat;

    /* 
     * =========== Error codes returned by ISS functions =============
     */
#define ISS_SOK                         (FVID2_SOK)
#define ISS_EFAIL                       (FVID2_EFAIL)
#define ISS_EBADARGS                    (FVID2_EBADARGS)
#define ISS_EINVALID_PARAMS             (FVID2_EINVALID_PARAMS)
#define ISS_EDEVICE_INUSE               (FVID2_EDEVICE_INUSE)
#define ISS_ETIMEOUT                    (FVID2_ETIMEOUT)
#define ISS_EALLOC                      (FVID2_EALLOC)
#define ISS_EOUT_OF_RANGE               (FVID2_EOUT_OF_RANGE)
#define ISS_EAGAIN                      (FVID2_EAGAIN)
#define ISS_EUNSUPPORTED_CMD            (FVID2_EUNSUPPORTED_CMD)
#define ISS_ENO_MORE_BUFFERS            (FVID2_ENO_MORE_BUFFERS)
#define ISS_EUNSUPPORTED_OPS            (FVID2_EUNSUPPORTED_OPS)
#define ISS_EDRIVER_INUSE               (FVID2_EDRIVER_INUSE)

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

    /* None */

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

    /* None */

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _ISS_TYPES_H */
