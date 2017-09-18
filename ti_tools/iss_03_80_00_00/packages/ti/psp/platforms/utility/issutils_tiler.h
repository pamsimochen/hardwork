/** ==================================================================
 *  @file   issutils_tiler.h                                                  
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
    \defgroup ISSUTILS_TILER_API Tiler allocator API
    @{
*/

/**
 *  \file issutils_tiler.h
 *
 *  \brief Tiler allocator API
*/

#ifndef _ISS_TILER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_TILER_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/platforms/utility/issutils_mem.h>

/** \brief Tiler CPU virtual address */
#define ISSUTILS_TILER_CPU_VIRT_ADDR  (0x60000000)

/** \brief Tiler physical memory size */
#define ISSUTILS_TILER_PHYS_MEM_SIZE (128*MB)

/** \brief Indicates 8-bit tiled */
#define ISSUTILS_TILER_CNT_8BIT     (0)
/** \brief Indicates 16-bit tiled */
#define ISSUTILS_TILER_CNT_16BIT    (1)
/** \brief Indicates 32-bit tiled */
#define ISSUTILS_TILER_CNT_32BIT    (2)
/** \brief Indicates number of tiled types (8-bit/16-bit/32-bit)  */
#define ISSUTILS_TILER_CNT_MAX      (3)

/** \brief Indicates direction -- copy to DDR */
#define ISSUTILS_TILER_COPY_TO_DDR      (0)
/** \brief Indicates direction -- copy from DDR */
#define ISSUTILS_TILER_COPY_FROM_DDR    (1)

/** \brief Indicates normal orientation */
#define ISSUTILS_TILER_ORI_NONE     (0x00u)
/** \brief Indicates horizontal-flipped orientation */
#define ISSUTILS_TILER_ORI_X_FLIP   (0x01u)
/** \brief Indicates vertical-flipped orientation */
#define ISSUTILS_TILER_ORI_Y_FLIP   (0x02u)
/** \brief Indicates horizontal and vertical flipped orientation */
#define ISSUTILS_TILER_ORI_XY_SWAP  (0x04u)

/**
 *  \brief Initialize the tiler
 *
 *  \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerInit                                               
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
Int32 IssUtils_tilerInit();

/**
 *  \brief De-initialize the tiler
 *
 *  \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerDeInit                                               
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
Int32 IssUtils_tilerDeInit();

/**
 *  \brief Enable/disable debug log for tiler APIs
 *
 *  \param enable       [IN] Indicates whether the debug log is to be enabled
 *
 *  \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerDebugLogEnable                                               
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
Int32 IssUtils_tilerDebugLogEnable(UInt32 enable);

/**
 *  \brief Alloc from tiler space.
 *
 *  Value can be passed directly to VPDMA
 *
 *  \param cntMode      [IN] container mode
 *  \param width        [IN] width in BYTES
 *  \param height       [IN] height in lines
 *
 *  \return Tiler address
*/
/* ===================================================================
 *  @func     IssUtils_tilerAlloc                                               
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
UInt32 IssUtils_tilerAlloc(UInt32 cntMode, UInt32 width, UInt32 height);

/**
 *  \brief Free all previously allocated tiler frames
 *
 *  \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerFreeAll                                               
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
Int32 IssUtils_tilerFreeAll();

/**
 *  \brief Convert tilerAddr to CPU addr
 *
 *  \param tilerAddr    [IN] address got via IssUtils_tilerAlloc
 *
 *  \return CPU virtual address
*/
/* ===================================================================
 *  @func     IssUtils_tilerAddr2CpuAddr                                               
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
UInt32 IssUtils_tilerAddr2CpuAddr(UInt32 tilerAddr);

/**
 *  \brief Allocate a frame in tiler space
 *
 *  Use FVID2_Format to allocate a frame.
 *  Fill FVID2_Frame fields like channelNum based on FVID2_Format
 *
 *  \param  pFormat     [IN] Data format information
 *  \param  pFrame      [OUT] Initialzed FVID2_Frame structure
 *  \param  numFrames   [IN] Number of frames to allocate
 *
 *  \return FVID2_SOK on success, else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerFrameAlloc                                               
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
Int32 IssUtils_tilerFrameAlloc(FVID2_Format * pFormat,
                               FVID2_Frame * pFrame, UInt16 numFrames);

/**
 *  \brief Copy between tiler Address space and non-tiler DDR address
 *
 *  This API internally converts tilerAddr to cpuAddr
 *
 *  \param dir          [IN] ISSUTILS_TILER_COPY_TO_DDR or
 *                           ISSUTILS_TILER_COPY_FROM_DDR
 *  \param tilerAddr    [IN] tiler address returned during IssUtils_tilerAlloc()
 *                           or IssUtils_tilerGetAddr()
 *  \param dataWidth    [IN] data width in bytes
 *  \param dataHeight   [IN] data height in lines
 *  \param ddrAddr      [IN] Non tiled DDR address
 *  \param ddrPitch     [IN] Pitch to be used for data in non-tiled space
 *                           in bytes
 *
 *  \return FVID2_SOK on sucess, else failure
*/
/* ===================================================================
 *  @func     IssUtils_tilerCopy                                               
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
Int32 IssUtils_tilerCopy(UInt32 dir, UInt32 tilerAddr, UInt32 dataWidth,
                         UInt32 dataHeight, UInt8 * ddrAddr, UInt32 ddrPitch);

/**
 *  \brief Get tiler Address
 *
 *  \param cntMode      [IN] container mode
 *  \param startX       [IN] X-coordinate in BYTES
 *  \param startY       [IN] Y-coordinate in LINES
 *
 *  \return tiler address
*/
/* ===================================================================
 *  @func     IssUtils_tilerGetAddr                                               
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
UInt32 IssUtils_tilerGetAddr(UInt32 cntMode, UInt32 startX, UInt32 startY);

/**
 *  \brief Get tiler address after applying the orientation.
 *
 *  \param tilerAddr    [IN] 0 degree tiler address returned during
 *                           IssUtils_tilerAlloc()
 *  \param cntMode      [IN] Container mode.
 *  \param oriFlag      [IN] Orientation flag representing S, Y', X' bits
 *  \param width        [IN] Buffer width
 *  \param height       [IN] Buffer height
 *
 *  \return Tiler address after applying the necessary orientation.
*/
/* ===================================================================
 *  @func     IssUtils_tilerGetOriAddr                                               
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
UInt32 IssUtils_tilerGetOriAddr(UInt32 tilerAddr,
                                UInt32 cntMode,
                                UInt32 oriFlag, UInt32 width, UInt32 height);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
