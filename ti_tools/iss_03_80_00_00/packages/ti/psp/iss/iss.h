/** ==================================================================
 *  @file   iss.h
 *
 *  @path   /ti/psp/iss/ 
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \defgroup ISS_DRV_FVID2_ISS_COMMON ISS - Common API
 *
 * @{
 */

/**
 *  \file vps.h
 *
 *  \brief ISS - Common API
 */
#ifndef _ISS_H
#define _ISS_H

/* ==========================================================================
 */
/* Include Files */
/* ==========================================================================
 */

#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/fvid2.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ==========================================================================
     */
    /* Macros & Typedefs */
    /* ==========================================================================
     */

/** \brief HDVPSS driver version string. */
#define ISS_VERSION_STRING              "ISS_01_00_07_00"

/** \brief HDVPSS driver version number. */
#define ISS_VERSION_NUMBER              (0x01000700u)

/** \brief Driver ID base for the capture driver class. */
#define ISS_CAPT_DRV_BASE               (0x10000100u)

/** \brief Driver ID base for the M2M driver class. */
#define ISS_M2M_DRV_BASE                (0x10000200u)

/** \brief Driver ID base for video  encoder driver class. */
#define ISS_SENSOR_DRV_BASE             (0x10000500u)

/** \brief Driver ID base for platform driver class. */
#define ISS_VID_SYSTEM_DRV_BASE         (0x10000600u)

/** \brief Driver ID base for the custom/user driver class. */
#define ISS_USER_DRV_BASE               (0x10001000u)

/** \brief VIP capture driver ID used at the time of FVID2 create. */
#define FVID2_ISS_CAPT_DRV          	(ISS_CAPT_DRV_BASE + 0x00000000u)

/** \brief Scalar Writeback M2M driver ID used at the time of FVID2 create. */
#define FVID2_ISS_M2M_SC_DRV            (ISS_M2M_DRV_BASE + 0x00000001u)

/** \brief Post Process M2M driver ID used at the time of FVID2 create. */
#define FVID2_ISS_M2M_ISP_DRV           (ISS_M2M_DRV_BASE + 0x00000002u)

#define ISS_H3A_SIZE 					(0u) //3110400u)
#define ISS_BSC_SIZE 					(0u) //8192*2)
#define ISS_HST_SIZE 					(0u)
#define ISS_CAR_SIZE 					(0u)

#define BLANKING_DATA_SIZE				(ISS_H3A_SIZE + ISS_BSC_SIZE + ISS_HST_SIZE + ISS_CAR_SIZE)

//#define VSTAB_MULT_COEFFICIENT	1.2
#define VSTAB_SCALE_NUMERATOR		(6)
#define VSTAB_SCALE_DENOMINATOR		(5)

    /*
     *  IOCTLs Base address.
     */
/** \brief IOCTL base address for the IOCTLs common to display, capture and
 *  mem to mem drivers. */
#define ISS_COMMON_IOCTL_BASE           (FVID2_USER_BASE + 0x10000000u)
/** \brief IOCTL base address for the capture driver IOCTLs. */
#define ISS_CAPT_IOCTL_BASE             (FVID2_USER_BASE + 0x10020000u)
/** \brief IOCTL base address for the M2M driver IOCTLs. */
#define ISS_M2M_IOCTL_BASE              (FVID2_USER_BASE + 0x10030000u)
/** \brief IOCTL base address for the video  encoder driver IOCTLs. */
#define ISS_SENSOR_IOCTL_BASE          	(FVID2_USER_BASE + 0x10070000u)
/** \brief IOCTL base address for the video  platform driver IOCTLs. */
#define ISS_VID_SYSTEM_IOCTL_BASE       (FVID2_USER_BASE + 0x10080000u)
/**
 *  \brief IOCTL base address for the advanced debug capabilities.
 *  Caution: To be used with care.
 */
#define ISS_ADV_IOCTL_BASE              (FVID2_USER_BASE + 0x10070000u)

/**
 *  \brief VPS video buffer alignment. All application buffer address and
 *  line pitch should be aligned to this byte boundary.
 */
#define ISS_BUFFER_ALIGNMENT            (16u)

    /*
     *  Common IOCTLS for display, M2M and capture drivers
     */

/**
  \addtogroup ISS_DRV_FVID2_IOCTL_COMMON
  @{
*/

/**
 *  \brief Set the scalar coefficients.
 *
 *  \param cmdArgs       [IN]  const Iss_ScCoeffParams *
 *  \param cmdStatusArgs [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
*/
#define IOCTL_ISS_SET_COEFFS            (ISS_COMMON_IOCTL_BASE + 0x0000u)

    /* @} */

    /*
     *  Advanced IOCTLS for display, M2M and capture drivers.
     */

/**
  \addtogroup ISS_DRV_FVID2_IOCTL_ADVANCED
  @{
*/

/**
 *  \brief Read the scalar hardware configuration.
 *
 *  This ioctl can be used to read the actual hardware registers of the
 *  scalar.
 *  This is for the advanced user for the hardware debug capability.
 *  For the multiple channel mode of the driver data is returned from the
 *  overlay memory instead of actual hardware registers.
 *
 *  \param cmdArgs       [OUT] Iss_ScRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_ISS_READ_ADV_SC_CFG       (ISS_ADV_IOCTL_BASE + 0x0000u)

/**
 *  \brief Write the scalar hardware configuration.
 *
 *  This ioctl can be used to write the actual hardware registers of the
 *  scalar.
 *  This is for the advanced user for the hardware debug capability.
 *  For the multiple channel mode of the driver data is written to the
 *  overlay memory instead of actual hardware registers.
 *
 *  \param cmdArgs       [IN]  const Iss_ScRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_ISS_WRITE_ADV_SC_CFG      (ISS_ADV_IOCTL_BASE + 0x0001u)

/**
 *  \brief Read the De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to read the actual hardware registers of the
 *  De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [OUT] Iss_DeiRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_ISS_READ_ADV_IPIPE_CFG       (ISS_ADV_IOCTL_BASE + 0x0012u)

/**
 *  \brief Write the De-Interlacer hardware configuration.
 *
 *  This ioctl can be used to write the actual hardware registers of the
 *  De-interlacer.
 *  This is for the advanced user for the hardware debug capability.
 *
 *  \param cmdArgs       [IN]  const Iss_DeiRdWrAdvCfg *
 *  \param cmdArgsStatus [OUT] NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_ISS_WRITE_ADV_IPIPE_CFG      (ISS_ADV_IOCTL_BASE + 0x0013u)

    /* @} */

/** \brief Floor a integer value. */
#define VpsUtils_floor(val, align)  (((val) / (align)) * (align))

/** \brief Align a integer value. */
#define VpsUtils_align(val, align)  VpsUtils_floor(((val) + (align)-1), (align))

/** \brief 8-bit Tiler container pitch in bytes. */
#define ISSUTILS_TILER_CNT_8BIT_PITCH   (16u * 1024u)

/** \brief 16-bit Tiler container pitch in bytes. */
#define ISSUTILS_TILER_CNT_16BIT_PITCH  (32u * 1024u)

/** \brief 32-bit Tiler container pitch in bytes. */
#define ISSUTILS_TILER_CNT_32BIT_PITCH  (32u * 1024u)


#define REG_TILER_DMM_OR1   		  (0x4E000224)
#define TILER_X_FLIP_VAL   			  (0x01)
#define TILER_Y_FLIP_VAL   			  (0x02)
#define TILER_XY_SWAP_VAL 			  (0x04)



/**
 *  enum Iss_VpdmaMemoryType
 *  \brief Enum for buffer memory type.
 */
    typedef enum {
        ISS_NONTILEDMEM = 0,
    /**< 1D non-tiled memory. */
        ISS_TILEDMEM,
    /**< 2D tiled memory. */
        ISS_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
    } Iss_MemoryType;

/**
 *  enum Iss_MemRotationType
 *  \brief This enum is used to define the memory data rotation and mirroring
 *  type. The rotation and mirroring is only valid if the memory type is tiler
 *  container mode. For non-tiler and tiler page mode, rotation and mirroring
 *  is not valid
 */
    typedef enum {
        ISS_MEM_0_ROTATION = 0,
    /**< no rotation and mirroring. */
        ISS_MEM_180_ROTATION_MIRRORING,
    /**< 180 degree rotation with mirroring. */
        ISS_MEM_0_ROTATION_MIRRORING,
    /**< 0 degree rotation with mirroring. */
        ISS_MEM_180_ROTATION,
    /**< 180 degree rotation. */
        ISS_MEM_270_ROTATION_MIRRORING,
    /**< 270 degree rotation with mirroring. */
        ISS_MEM_270_ROTATION,
    /**< 270 degree rotation. */
        ISS_MEM_90_ROTATION,
    /**< 90 degree rotation. */
        ISS_MEM_90_ROTATION_MIRRORING,
    /**< 90 degree rotation with mirroring. */
        ISS_MEM_ROTATION_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
    } Iss_MemRotationType;

    /* ==========================================================================
     */
    /* Structure Declarations */
    /* ==========================================================================
     */

/**
 * struct Iss_FrameParams
 * \brief Structure for setting the frame parameters like frame height, width,
 * pitch and memory type.
 */
    typedef struct {
        UInt32 width;
    /**< Width of frame in pixels. */
        UInt32 height;
    /**< Height of frame in lines. */
        UInt32 pitch[FVID2_MAX_PLANES];
    /**< Pitch for each planes in bytes. Only required planes needs to be
         populated. */
        UInt32 memType;
    /**< VPDMA memory type - Tiled buffer or normal non-tiled buffer.
         For valid values see #Iss_VpdmaMemoryType. */
    } Iss_FrameParams;

/**
 *  struct Iss_CropConfig
 *  \brief Structure containing crop configuration - used in Scalar and VCOMP.
 */
    typedef struct {
        UInt32 cropStartX;
    /**< Horizontal offset from which picture needs to be cropped. */
        UInt32 cropStartY;
    /**< Vertical offset from which picture needs to be cropped. */
        UInt32 cropWidth;
    /**< Width of the picture to be cropped. */
        UInt32 cropHeight;
    /**< Height of the picture to be cropped. */
    } Iss_CropConfig;

/**
 *  struct Iss_PosConfig
 *  \brief Structure containing position configuration - used in VCOMP and CIG.
 */
    typedef struct {
        UInt32 startX;
    /**< Horizontal offset from which picture needs to be positioned. */
        UInt32 startY;
    /**< Vertical offset from which picture needs to be positioned. */
    } Iss_PosConfig;

/**
 *  struct Iss_SubFrameParams
 *  \brief Configuration for sub-frame level processing  at create time.
 */
    typedef struct {
        UInt32 subFrameModeEnable;
    /**< TRUE : SubFrame level processing is done for this channel
         FALSE: Frame level processing is done for this channel. */
        UInt32 numLinesPerSubFrame;
    /**< Number of lines in a subframe. */
    } Iss_SubFrameParams;

    /* ==========================================================================
     */
    /* Include HW specific config constants, structures */
    /* ==========================================================================
     */

#include <ti/psp/iss/iss_cfgSc.h>

    /* ==========================================================================
     */
    /* Function Declarations */
    /* ==========================================================================
     */

/**
 *  \brief Prints to Shared memory and CCS console
 *
 *  This function prints the provided formatted string to shared memory and CCS
 *  console
 *
 *  \param format       [IN] Formatted string followed by variable arguments
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_printf(char *format, ...);

/**
 *  \brief Prints to Shared memory only
 *
 *  This function prints the provided formatted string to shared memory only
 *
 *  \param format       [IN] Formatted string followed by variable arguments
 *
 *  \return FVID2_SOK on success, else appropriate FVID2 error code on failure.
 */
    Int32 Iss_rprintf(char *format, ...);

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef _ISS_H */
/* @} *//**
\mainpage  ISS Drivers

\par IMPORTANT NOTE
     <b>
     The interfaces defined in this package are bound to change.
     Kindly treat the interfaces as work in progress.
     Release notes/user guide list the additional limitation/restriction
     of this module/interfaces.
     </b> See also \ref TI_DISCLAIMER.

ISS Drivers allow users to make use of all ISS hardware features like
1080P capture.
This document has detailed API description that user's can use to make use
of the ISS drivers.

<b>
  Also refer to ISS driver user guide for detailed features,
  limitations and usage description.
</b>

The ISS driver API can be broadly divided into the following categories
  - <b> FVID2 API </b> (See \ref ISS_DRV_FVID2_API) <br>
    API used to create, control and use the different ISS drivers

  - <b> Capture API </b> (See \ref ISS_DRV_FVID2_CAPTURE_API) <br>
    API for video capture drivers

  - <b> Links and chains example API </b> (See \ref ISSEXAMPLE_LINKS_AND_CHAIN_API ) <br>
    APIs used by chains application for connecting drivers to each other in other create
    different application scenarios like multi-chanmel capture + display,
    single channel capture + display and so on
*//**
 \page  TI_DISCLAIMER  TI Disclaimer

 \htmlinclude ti_disclaim.htm
*//**
  \defgroup ISS_DRV_FVID2_IOCTL           ISS - All IOCTL's
*//**
  \ingroup ISS_DRV_FVID2_IOCTL
  \defgroup ISS_DRV_FVID2_IOCTL_FVID2     FVID2 - Common IOCTL's
*//**
  \ingroup ISS_DRV_FVID2_IOCTL
  \defgroup ISS_DRV_FVID2_IOCTL_COMMON    ISS - Common IOCTL's
*//**
  \ingroup ISS_DRV_FVID2_IOCTL
  \defgroup ISS_DRV_FVID2_IOCTL_ADVANCED  ISS - Advanced IOCTL's

*//**
  \ingroup ISS_DRV_FVID2_IOCTL
  \defgroup ISS_DRV_FVID2_IOCTL_CAPTURE VP Capture IOCTL's

  In addition to the IOCTLs listed below the following common IOCTLs
  are applicable to this driver

  - \ref FVID2_START
  - \ref FVID2_STOP
*/
