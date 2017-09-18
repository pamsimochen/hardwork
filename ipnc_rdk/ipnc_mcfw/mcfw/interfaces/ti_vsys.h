/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup MCFW_API
    \defgroup MCFW_SYS_API McFW System Level (VSYS) API

    @{
*/

/**
    \file ti_vsys.h
    \brief McFW System Level (VSYS) API
*/

#ifndef __TI_VSYS_H__
#define __TI_VSYS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "ti_media_common_def.h"
#include "common_def/ti_vsys_common_def.h"
#include "demos/mcfw_api_demos/multich_usecase/ti_mcfw_ipnc_main.h"

/* =============================================================================
 * Defines
 * =============================================================================
 */

/** \brief Floor a integer value. */
#define VsysUtils_floor(val, align)  (((val) / (align)) * (align))

/** \brief Align a integer value. */
#define VsysUtils_align(val, align)  VsysUtils_floor(((val) + (align)-1), (align))

/* =============================================================================
 * Enums
 * =============================================================================
 */


/**
    \brief McFW Sub-system's or modules
*/
typedef enum
{
    VCAP,
    /**< Video Capture Sub-system */

    VENC,
    /**< Video Encode Sub-system */

    VDEC,
    /**< Video Decode Sub-system */

    VDIS,
    /**< Video Display Sub-system */

    VSYS_MODULES_MAX
    /**< Max sub-system's in McFW */

} VSYS_MODULES_E;

/**
    \brief System use-case's

    Inter-connection of sub-system's and internal HW processing
    block's depends on the system use-case that is selected.
*/
typedef enum
{
    VSYS_USECASE_MULTICHN_PROGRESSIVE_VCAP_VDIS_VENC_VDEC,
    /**< Video Capture to Video Encode(Progressive)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Surveillence multi-ch D1 DVR use-case.
        Scaled <CIF or any resolution> Secondary output, encode / bits out is enabled based on runtime flag.
    */

    VSYS_USECASE_MULTICHN_INTERLACED_VCAP_VDIS_VENC_VDEC,
    /**< Video Capture to Video Encode(Inetrlaced)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Survillence multi-ch D1 DVR use-case.
    */

    VSYS_USECASE_MULTICHN_VCAP_VENC,
    /**< Video Capture to Video Encode use-case. \n
        This is for Survillence multi-ch D1 DVS use-case.

        One HD Display (On-chip HDMI) can be optionally enabled for local preview
     */

    VSYS_USECASE_MULTICHN_HD_VCAP_VENC,
    /**< Video Capture to Video Encode use-case for 4x 1080p HD. \n
        This is for Survillence multi-ch HD-DVS use-case.

        One HD Display (On-chip HDMI) can be optionally enabled for local preview
     */

    VSYS_USECASE_MULTICHN_VDEC_VDIS,
    /**< A8 to Video Display use-case,\n
        It will support up to 32D1 to display.
    */

    VSYS_USECASE_MULTICHN_VCAP_VDIS,
    /**< Video Capture to Video Display use-case, Video Encode and Decode is kept disabled. \n
        Use-full for HW Board Check out for multi-ch D1 system.
    */

    VSYS_USECASE_MULTICHN_PROGRESSIVE_VCAP_VDIS_VENC_VDEC_NON_D1,
    /**< Video Capture to Video Encode(Progressive)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Surveillence multi-ch Non D1 <CIF> DVR use-case.
    */

    VSYS_USECASE_MULTICHN_CUSTOM,
    /**<
    */

    VSYS_USECASE_MULTICHN_PROGRESSIVE_VCAP_VDIS_VENC_VDEC_4CH,
    /**< Video Capture to Video Encode(Progressive)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Surveillence multi-ch D1 DVR use-case.
        Scaled <CIF or any resolution> Secondary output, encode / bits out is enabled based on runtime flag.
        Capture is limited to 4D1 in this usecase
    */

    VSYS_USECASE_MULTICHN_PROGRESSIVE_VCAP_VDIS_VENC_VDEC_8CH,
    /**< On DM814X only, Video Capture to Video Encode(Progressive)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Surveillence multi-ch D1 DVR use-case.
    */

    VSYS_USECASE_MULTICHN_PROGRESSIVE_VCAP_VDIS_VENC_VDEC_16CH_NRT,
    /**< On DM814X only, Video Capture to Video Encode(Progressive)+Video Display and Video Decode+Video Display use-case. All sub-systems are enabled. \n
        This is for Surveillence multi-ch D1 DVR use-case.
    */

    VSYS_USECASE_MULTICHN_TRISTREAM_LOWPWR,
    /**< Video Capture to Video Encode use-case for 1080p60+D1+JPEG. \n
        This is for Survillence tri stream IPNC use-case.
     */

    VSYS_USECASE_MULTICHN_TRISTREAM_FULLFTR,
    /**< Video Capture to Video Encode use-case for 1080p60+D1+JPEG. \n
        This is for Survillence tri stream IPNC use-case with VNF applied on first stream.
        This is a full feature usecase with VNF and Facedetect applied
     */

    VSYS_USECASE_DUALCHN_DISPLAY,
    /**< Video Capture to Display use-case for 1080p60. \n
        This is for Survillence Single stream IPNC use-case.
     */

	VSYS_USECASE_TRISTREAM_SMARTANALYTICS,
    /**< Video Capture to Video Encode use-case for 1080p60+D1+JPEG. \n
        This is for Survillence tri stream IPNC use-case with VNF applied on first stream.
        This is a full feature usecase with VNF and Facedetect applied
     */

	VSYS_USECASE_ENC_A8_DEC,
    /**< Video Capture to Video Encode use-case for 1080p60+D1+JPEG. \n
        This is for Survillence tri stream IPNC use-case with VNF applied on first stream.
        This is a full feature usecase with VNF and Facedetect applied
     */

	VSYS_USECASE_MULTICHN_QUADSTREAM_NF,
    /**< Video Capture to Video Encode use-case for 1080p60+D1+JPEG. \n
        This is for Survillence tri stream IPNC use-case with VNF applied on first stream.
        This is a full feature usecase with VNF and Facedetect applied
     */

	VSYS_USECASE_DUALCHN_DEMO,
    /**< Video Capture to Video Encode use-case for 1080p30+1080p30. \n
        This is for Survillence Dual stream IPNC use-case.
     */

    VSYS_USECASE_MCTNF_DEMO,
    /**< Video Capture to MCTNF to VNF to Display. Display shows non-NSF and NSF output side by side. \n
        This is for Survillence IPNC MCTNF demo.
     */
	 
    VSYS_USECASE_RVM_DEMO,
    /**< Rear View Module Demo usecase. \n
        This is for Survillence IPNC MCTNF demo.
     */	 
	 
    VSYS_USECASE_MAX
    /**< Maximum use-case ID */

} VSYS_USECASES_E;

/* =============================================================================
 * Structures
 * =============================================================================
 */

/**
    \brief Top level System Configuration structure
*/
typedef struct
{
    Bool   enableCapture;
    /**< Enable/Disable VCAP subsystem */

    Bool   enableNsf;
    /**< Enable/Disable Noise Filter HW Block in VCAP subsystem */

    Bool   enableOsd;
    /**< Enable/Disable Osd in VCAP subsystem */

    Bool   enableScd;
    /**< Enable/Disable Scd (basic tamper detection) in VCAP subsystem */

    Bool   enableEncode;
    /**< Enable/Disable VENC subsystem */

    Bool   enableDecode;
    /**< Enable/Disable VDEC subsystem */

    Bool   enableNullSrc;
    /**< Enable/Disable NULL Source in framework */

    UInt32 enableAVsync;
    /**<Enable/Disable AvSync  */

    Bool   cifonly;
    /**< Number of channels */
    UInt32 numChs;

    UInt32 numDeis;
    /**< Number of DEIs to use in VCAP Sub-system */

    UInt32 numSwMs;
    /**< Number of SW Mosaic's to use in VDIS Sub-system */

    UInt32 numDisplays;
    /**< Number of Display's to use in VDIS Sub-system */

    VSYS_USECASES_E systemUseCase;
    /**< System use-case to use */

    Bool enableSecondaryOut;
    /**< System use-case to use */

    Bool enableMjpegEnc;
    /**< System use-case to use */

    Bool enableSclr;
    /** Enable scalar 5 for scaling **/
} VSYS_PARAMS_S;

typedef enum {

    VSYS_SR0_SHAREMEM = 0,

    VSYS_SR1_SHAREMEM,

    VSYS_SR2_SHAREMEM,

    VSYS_SR3_SHAREMEM
} VSYS_SR_DESC;

/**
    \brief Allocated buffer info
*/
typedef struct {

    UInt8  *physAddr;
    /**< Physical address */

    UInt8  *virtAddr;
    /**< Virtual address */

    UInt32  srPtr;
    /**< Shared region Pointer SRPtr */

} Vsys_AllocBufInfo;

/**
    \brief Top level System Configuration structure
*/
typedef struct {

    UInt32 streamId;
    /**< Stream Id */

    UInt32 transparencyEnable;
    /** Transparency Enable */

    UInt32 dateEnable;
    /**< Date Enable */

    UInt32 timeEnable;
    /**< Time Enable */

    UInt32 logoEnable;
    /**< Logo Enable */

    UInt32 logoPos;
    /**< Logo Position */

    UInt32 textEnable;
    /**< Text Enable */

    UInt32 textPos;
    /**< Text Position */

    UInt32 detailedInfo;
    /**< Detailed Info */

    UInt8 *pUsrString;
    /**< User String */

    UInt32 bitRate;
    /**< Codec Bit Rate */

    UInt32 rateControl;
    /**< Codec Rate Control */

    UInt32 frameRate;
    /**< Codec Frame Rate */

    UInt32 dateFormat;
    /**< Date format */

    UInt32 datePos;
    /**< Date position */

    UInt32 timeFormat;
    /**< Time format */

    UInt32 timePos;
    /**< Time position */

	UInt32 histEnable;
    /**< Histogram Enable */

} Vsys_swOsdPrm;

typedef enum {

    VSYS_SWOSDGUIPRM,
    /**< Set SW OSD GUI params */

    VSYS_SWOSDBR,
    /**< Set SW OSD codec Bit Rate */

    VSYS_SWOSDRC,
    /**< Set SW OSD codec Rate Control */

    VSYS_SWOSDFR,
    /**< Set SW OSD codec Frame Rate */

    VSYS_SWOSDDATETIME,
    /**< Set SW OSD Date Time params */

    VSYS_SWOSDHISTEN
    /**< Set SW OSD Histogram Enable */
} VSYS_SWOSD_SETPARAM;

typedef enum {

    VSYS_VAGUI_SETUP,
    /**< Set VA GUI params */

	VSYS_VAGUI_SETUPALGOFRAMERATE,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPALGODETECTFREQ,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPEVTRECORDENABLE,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPSMETATRACKENABLE,
	/**< Set VA GUI params */

    VSYS_VAGUI_SETUPTZPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPMAINPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPCTDPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPOCPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPSMETAPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPIMDPRM,
	/**< Set VA GUI params */

	VSYS_VAGUI_SETUPENCROIENABLE
	/**< Set VA GUI params */

} VSYS_VA_GUISETPARAM;

/**
    \brief Top level System Configuration structure
*/
typedef struct {

    UInt32 fdetect;
    /**< FD enable */

    UInt32 startX;
    /**< FD ROI start X */

    UInt32 startY;
    /**< FD ROI start Y */

    UInt32 width;
    /**< FD ROI width */

    UInt32 height;
    /**< FD ROI height */

    UInt32 fdconflevel;
    /**< FD confidence level */

    UInt32 fddirection;
    /**< FD direction */

    UInt32 pmask;
    /**< FD privacy mask */

} Vsys_fdPrm;


/**
    \brief Board Type detect call
*/
typedef struct {

    UInt32  boardRev;
    /**< boardRev */

    UInt32  cpuRev;
    /**< CPU Rev */

    UInt32  boardId;
    /** < Board ID */
} VSYS_PLATFORM_INFO_S;

/**
    \brief Top level System Configuration structure
*/
typedef struct {

	UInt32 dccSize;
    /**< DCC Buffer Size */

} Vsys_dccPrm;


/**
    \brief VSYS Mux link input queue/channel to output channel map
*/

typedef struct VSYS_MUX_IN_Q_CHAN_MAP_INFO_S {

    UInt32 inQueId;
    /**< Input queue identifier. Should be less than MUX_LINK_MAX_IN_QUE.
            Also this should be less then number of input queues defined while
            creating an instance of mux link. i.e. MUX_LINK_MAX_IN_QUE */

    UInt32 inChNum;
    /**< Input queues associated channel number, should be less than 
            SYSTEM_MAX_CH_PER_OUT_QUE */

} VSYS_MUX_IN_Q_CHAN_MAP_INFO_S;

/* =============================================================================
 * APIs
 * =============================================================================
 */

/**
    \brief Set VSYS_PARAMS_S to default parameters

    Call this before calling Vsys_init() and then overide
    the required parameters in user application.

    \param pContext     [OUT] VSYS_PARAMS_S initialized to default values

    \return ERROR_NONE on success
*/
Int32 Vsys_params_init(VSYS_PARAMS_S * pContext);

/**
    \brief Initialize system for a given system level configuration

    This should be the first McFW API that should be happen

    \param pContext     [IN] System configuration

    \return ERROR_NONE on success
*/
Int32 Vsys_init(VSYS_PARAMS_S * pContext);

/**
    \brief Create and setup processing block's based on sub-system configuration

    This API should be called after sub-system init's ( Vxxx_init() ).
    This API will allocate and setup all HW resource in order for the system
    use-case to run.
    Once this API succeds now the sub-system's can be started to start the system execution

    \return ERROR_NONE on success
*/
Int32 Vsys_create();

/**
    \brief Delete and de-init processing block's based on sub-system configuration

    This API should be called after all sub-system have been stopped ( Vxxx_stop() ).
    This API will de-allocate and bring all HW resources to a known state.

    Once this API succeds now the sub-system's can be exited using Vxxx_exit() APIs

    \return ERROR_NONE on success
*/
Int32 Vsys_delete();


/**
    \brief System de-init

    This is last API to be called. This API should be called after all sus-system exits have been
    called (Vxxx_exit()).

    \return ERROR_NONE on success
*/
Int32 Vsys_exit();


/**
    \brief Print detailed system statistics

    This is useful system debugging.

    \return ERROR_NONE on success
*/
Int32 Vsys_printDetailedStatistics();

/**
    \brief Print detailed buffer statistics of links <useful for debugging>

    This is useful system debugging.

    \return ERROR_NONE on success
*/
Int32 Vsys_printBufferStatistics();

/**
    \brief Allocate contigous buffer from a shared memory

    \param srRegId  [IN] Shared region ID
    \param bufSize  [IN] Buffer size in bytes
    \param bufAlign [IN] Buffer alignment in bytes
    \param bufInfo  [OUT] Allocated buffer info

    \return ERROR_NONE on success
*/
Int32 Vsys_allocBuf(UInt32 srRegId, UInt32 bufSize, UInt32 bufAlign, Vsys_AllocBufInfo *bufInfo);

/**
    \brief Free the buffer allocated from the shared region

    \param srRegId  [IN] Shared region ID
    \param virtAddr [IN] Buffer virtual address
    \param bufSize  [IN] Size of the buffer in bytes

    \return ERROR_NONE on success
*/
Int32 Vsys_freeBuf(UInt32 srRegId, UInt8 *virtAddr, UInt32 bufSize);

/**
    \brief Free the buffer allocated from the shared region

    \param srRegId  [IN] Shared region ID
    \param virtAddr [IN] Buffer virtual address
    \param bufSize  [IN] Size of the buffer in bytes

    \return ERROR_NONE on success
*/
VSYS_USECASES_E Vsys_getSystemUseCase();

/**
    \brief Free the buffer allocated from the shared region

    \param srRegId  [IN] Shared region ID
    \param virtAddr [IN] Buffer virtual address
    \param bufSize  [IN] Size of the buffer in bytes

    \return ERROR_NONE on success
*/
Int32 Vsys_setSwOsdPrm(VSYS_SWOSD_SETPARAM swOsdSetPrm,
                       Vsys_swOsdPrm * pSwOsdPrm);

/**
    \brief Free the buffer allocated from the shared region

    \param srRegId  [IN] Shared region ID
    \param virtAddr [IN] Buffer virtual address
    \param bufSize  [IN] Size of the buffer in bytes

    \return ERROR_NONE on success
*/
Int32 Vsys_setFdPrm(Vsys_fdPrm * pFdPrm);

/**
    \brief This function is called by McFW when it receives any event from the slave processor

    \param callback [IN] User specified callback
    \param appData  [IN] User specified application data pointer which is also returned during the callback

    \return ERROR_NONE on success
*/

/**
 * \brief:
 *      This function gives CPU ID, board type and revision.
 * \input:
 *      VSYS_PLATFORM_INFO_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_getPlatformInfo(VSYS_PLATFORM_INFO_S *pInfo);

Int32 Vsys_registerEventHandler(VSYS_EVENT_HANDLER_CALLBACK callback, Ptr appData);

/**
 * \brief:
 *      This function gives core status. It can be called to check if all the
 *      cores are alive or any of the cores is crashed.
 * \input:
        coreStatus structure to be populated
 * \output:
 *      coreStatus structure updated with info on each core
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_getCoreStatus(VSYS_CORE_STATUS_TBL_S *coreStatusTbl);

/**
 * \brief:
 *      This function gives context info.
 * \input:
 *      VSYS_PARAMS_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Void Vsys_getContext(VSYS_PARAMS_S * contextInfo);

/**
 * \brief:
 *      This function gives context info.
 * \input:
 *      VSYS_PARAMS_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_datetimeCreate();

/**
 * \brief:
 *      This function gives context info.
 * \input:
 *      VSYS_PARAMS_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_datetimeDelete();
/**
 * \brief:
 *      This function configures display controller.
 * \input:
 *      VSYS_PARAMS_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_configureDisplay();

/**
 * \brief:
 *      This function de-configures display controller.
 * \input:
 *      VSYS_PARAMS_S sturcture
 * \output:
 *      NA
 * \return
*       ERROR_NOERROR       --  while success
*       ERROR_CODE          --  refer for err defination
*/
Int32 Vsys_deConfigureDisplay();

Void Vsys_switchFormatSD(Int32 standard);


/**
 * \brief:
 *      Function to get the current map between input queue, input channel no
 *      to output channel number.
 *
 * \input:
 *      instId              -- Instace of Mux link
 *                              if this value is 0xFFFFFFFF, mux id defined in
 *                              gVsysModuleContext.muxId would be used.
 *      mapParams           -- Pointer to an array of size 
 *                              SYSTEM_MAX_CH_PER_OUT_QUE, of type 
 *                              VSYS_MUX_IN_Q_CHAN_MAP_INFO_S.
 * \output:
 *      NA
 *
 * \return
 *       ERROR_NONE         --  while success
 *       ERROR_CODE         --  refer for err defination
 */
Int32 Vsys_getMuxQChanMap(UInt32 instId, 
                            VSYS_MUX_IN_Q_CHAN_MAP_INFO_S *mapParams);

/**
 * \brief:
 *      Function to apply the specified map between input queue, input channel 
 *      number output channel number.
 *
 * \input:
 *      instId              -- Instace of Mux link
 *                              if this value is 0xFFFFFFFF, mux id defined in
 *                              gVsysModuleContext.muxId would be used.
 *      mapParams           -- Pointer to an array of size 
 *                              SYSTEM_MAX_CH_PER_OUT_QUE, of type 
 *                              VSYS_MUX_IN_Q_CHAN_MAP_INFO_S. Which describe 
 *                              the map.
 * \output:
 *      NA
 *
 * \return
 *       ERROR_NONE         --  while success
 *       ERROR_CODE         --  refer for err defination
 */
Int32 Vsys_setMuxQChanMap(UInt32 instId, 
                            VSYS_MUX_IN_Q_CHAN_MAP_INFO_S *mapParams);

/**
 * \brief:
 *      Print the current map between input queue, input channel to output 
 *      channel map.
 *
 * \input:
 *      instId              -- Instace of Mux link
 *                              if this value is 0xFFFFFFFF, mux id defined in
 *                              gVsysModuleContext.muxId would be used.
  * \output:
 *      NA
 *
 * \return
 *       ERROR_NONE         --  while success
 *       ERROR_CODE         --  refer for err defination
 */
Int32 Vsys_printMuxQChanMap(UInt32 instId);

/**
 * \brief:
 *      Set the Dup link Delay param,this ensures the frame on input queue is formwarded only after
 *      all the other queueus have returned back the frame. 
 *
 * \input:
 *      linkId              -- Dup Link Id
 *  	queuId				-- Queue on which delay is required
 *  	syncQueueId			-- QueueId on which delay is observed.
 * \output:
 *      NA
 *
 * \return
 *       ERROR_NONE         --  while success
 *       ERROR_CODE         --  refer for err defination
 */
Int32 Vsys_setDupDelayPrm(UInt32 linkId,UInt32 queueId,UInt32 syncQueueId);

/**
 * \brief:
 *      Change Noise filter mode dynamically between DSS NF and ISS NF
 *
 * \input:
 *      channelId           -- channelId
 *  	noisefilterMode		-- Noise filter mode
 * \output:
 *      NA
 *
 * \return
 *       ERROR_NONE         --  while success
 *       ERROR_CODE         --  refer for err defination
 */
Int32 Vsys_setNoiseFilter(UInt32 channelId,UI_NF_MODE noisefilterMode); 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


/* @} */
