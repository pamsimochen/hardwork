/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_PRIV_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_PRIV_COMMON_H_

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/src_bios6/utils/utils_prf.h>
#include <mcfw/src_bios6/utils/utils_tsk.h>
#include <mcfw/src_bios6/utils/utils_buf.h>
#include <mcfw/src_bios6/utils/utils_mem.h>
#include <mcfw/src_bios6/utils/utils_tiler.h>
#include <mcfw/src_bios6/utils/utils_bit_buf.h>
#include <mcfw/src_bios6/utils/utils_buf_ext.h>

#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/interfaces/link_api/system_common.h>
#include <mcfw/interfaces/link_api/system_tiler.h>
#include <mcfw/interfaces/link_api/systemLink_common.h>

/* needed in common since Vps_CaptRtParams is part of System_FrameInfo which
 * is shared between Video-M3 and VPSS-M3 */
#include <ti/psp/vps/vps_capture.h>
#include <ti/psp/devices/vps_device.h>
#include <ti/psp/platforms/vps_platform.h>

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

/**
 * @def   SYSTEM_LINK_FRAMES_PER_CH
 * @brief COntrols the default number of buffers allocated per channel in each link
 */
#ifdef MEMORY_256MB
	#define SYSTEM_LINK_FRAMES_PER_CH  	 (5)	
#else 
	#define SYSTEM_LINK_FRAMES_PER_CH    (6)
#endif

#define SYSTEM_LINK_MAX_FRAMES_PER_CH    (8)

#define SYSTEM_TSK_PRI_HIGH              (13)
#define SYSTEM_MSGQ_TSK_PRI_HIGH         (13)
#define IVASVR_TSK_PRI                   (13)
#define NULL_LINK_TSK_PRI                (2)
#define DUP_LINK_TSK_PRI                 (12)
#define MERGE_LINK_TSK_PRI               (12)
#define NULL_SRC_LINK_TSK_PRI            (2)
#define SYSTEM_TSK_PRI                   (2)
#define IPC_LINK_TSK_PRI                 (12)
#define SYSTEM_MSGQ_TSK_PRI              (2)
#define ENC_LINK_TSK_PRI                 (2)
#define DEC_LINK_TSK_PRI                 (2)
#define IVASVR_TSK_PRI                   (13)              // highest
                                                           // priority
#define DISPLAY_LINK_TSK_PRI             (4)
#define SCLR_LINK_TSK_PRI              	 (6)
#define DEI_LINK_TSK_PRI                 (8)
#define SW_MS_LINK_TSK_PRI               (8)
#define CAPTURE_LINK_TSK_PRI             (10)
#define NSF_LINK_TSK_PRI                 (12)
#define ALG_LINK_TSK_PRI                 (8)
#define SELECT_LINK_TSK_PRI              (8)
#define MUX_LINK_TSK_PRI                 (8)
/* Priority of the SCD should be lowest when ALGLing has only SCD enabled */
#define ALG_LINK_TSK_PRI_SCD             (1)

#define CAMERA_LINK_TSK_PRI              (12)
#define MJPEG_LINK_TSK_PRI               (11)
#define OSD_LINK_TSK_PRI                 (8)
#define MD_TSK_PRI                       (1)               // lowest priority
#define VSTAB_LINK_TSK_PRI               (8)
#define FD_LINK_TSK_PRI                  (2)
#define SWOSD_LINK_TSK_PRI               (6)
#define VNF_LINK_TSK_PRI                 (13)
#define MCTNF_LINK_TSK_PRI               (4)
#if defined(TI_8107_BUILD)
#define GLBCE_LINK_TSK_PRI               (11)
#else
#define GLBCE_LINK_TSK_PRI               (13)
#endif
#define WDR_LINK_TSK_PRI				 (13)
#define ISP_LINK_TSK_PRI                 (12)
#define VA_LINK_TSK_PRI                  (6)
#define VA_LINK_PROCESSTSK_PRI           (6)
#define MP_SCALAR_LINK_TSK_PRI           (6)
#define RVM_LINK_TSK_PRI				 (6)

#define SYSTEM_TSK_STACK_SIZE_LARGE      (32*KB)
#define SYSTEM_TSK_STACK_SIZE_MEDIUM     (16*KB)
#define SYSTEM_TSK_STACK_SIZE_SMALL      ( 8*KB)
#define SYSTEM_TSK_STACK_SIZE_MAX        (SYSTEM_TSK_STACK_SIZE_LARGE)

#define SYSTEM_DEFAULT_TSK_STACK_SIZE    (SYSTEM_TSK_STACK_SIZE_LARGE)

#define SYSTEM_TSK_STACK_SIZE            (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define CAPTURE_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define CAMERA_LINK_TSK_STACK_SIZE       (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define MJPEG_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define NSF_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DEI_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define VSTAB_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DISPLAY_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DISPLAY_HWMS_LINK_TSK_STACK_SIZE (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SW_MS_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SCLR_LINK_TSK_STACK_SIZE         (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define IPC_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define NULL_SRC_LINK_TSK_STACK_SIZE     (SYSTEM_TSK_STACK_SIZE_SMALL)
#define NULL_LINK_TSK_STACK_SIZE         (SYSTEM_TSK_STACK_SIZE_SMALL)
#define DUP_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define MERGE_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define DEC_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define ENC_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define ALG_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SYSTEM_MSGQ_TSK_STACK_SIZE       (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define MD_TSK_STACK_SIZE                (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SELECT_LINK_TSK_STACK_SIZE       (SYSTEM_TSK_STACK_SIZE_SMALL)
#define FD_LINK_TSK_STACK_SIZE           (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define SWOSD_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define VNF_LINK_TSK_STACK_SIZE        	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define MCTNF_LINK_TSK_STACK_SIZE        (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define GLBCE_LINK_TSK_STACK_SIZE     	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define WDR_LINK_TSK_STACK_SIZE     	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define ISP_LINK_TSK_STACK_SIZE		 	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define VA_LINK_TSK_STACK_SIZE			 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define VA_LINK_PROCESSTSK_STACK_SIZE	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define MUX_LINK_TSK_STACK_SIZE          (SYSTEM_TSK_STACK_SIZE_SMALL)
#define MP_SCLR_LINK_TSK_STACK_SIZE      (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#define RVM_LINK_TSK_STACK_SIZE      	 (SYSTEM_DEFAULT_TSK_STACK_SIZE)

typedef struct {
    Vps_CaptRtParams captureRtParams;
    UInt32 captureChannelNum;
    UInt32 cameraChannelNum;
    UInt32 nsfChannelNum;
    UInt32 vnfChannelNum;
    UInt32 glbceChannelNum;
    UInt32 wdrChannelNum;
    UInt32 displayHwMsRepeatCount;

    UInt32 dupCount;
    FVID2_Frame *pDupOrgFrame;
    Void *pOrgListMPElem;
    UInt32 mergeChannelNum;
    volatile UInt32 vdecRefCount;
    FVID2_Frame *pVdecOrgFrame;

    UInt32 allocPoolID;
    /**< Pool frame from which buf was originally alloced */
    UInt32 rtChInfoUpdate;
    System_LinkChInfo rtChInfo;

    UInt64 ts64; /* 64-bit timestamp in units of msecs */
    UInt32 swMsBlankOutBuf; /* Flag to indicate if
                             * SwMs needs to blank
                             * the output buffer */

    UInt32 selectOrgChannelNum; /* original channeNum at input of select Link */
    UInt32 isMpFrame; /* Flag used to indicate if the frame is Mega Pixel frame.
                       * If an MP Frame, 31st bit would be set and 0-30 would
                       * have the associated channel number
                       */
    UInt32 isDataTypeChange;

    UInt32 muxOrgQId;
    UInt32 muxOrgChNum;
    UInt32 muxDupCount;
    FVID2_Frame *pMuxDupOrgFrame;

} System_FrameInfo;



typedef Int32(*System_GetLinkInfoCb) (Utils_TskHndl * pTsk,
                                      System_LinkInfo * info);
typedef Int32(*System_LinkGetOutputFramesCb) (Utils_TskHndl * pTsk,
                                              UInt16 queId,
                                              FVID2_FrameList * pFrameList);
typedef Int32(*System_LinkPutEmptyFramesCb) (Utils_TskHndl * pTsk,
                                             UInt16 queId,
                                             FVID2_FrameList * pFrameList);
typedef Int32(*System_LinkGetOutputBitBufsCb) (Utils_TskHndl * pTsk,
                                               UInt16 queId,
                                               Bitstream_BufList * pBitBufList);
typedef Int32(*System_LinkPutEmptyBitBufsCb) (Utils_TskHndl * pTsk,
                                              UInt16 queId,
                                              Bitstream_BufList * pBitBufList);

/**
 * \brief LINK Instance Info
 *  Each of the LINKs are expected to register with "system" with the following
 *  information.
 *  Using these links the system would form a chain with mutiple LINKs
 */
typedef struct System_LinkObj {
    Utils_TskHndl *pTsk;

    System_LinkGetOutputFramesCb linkGetFullFrames;
    /**< Function expected to be called by the LINK to get the input frames */
    System_LinkPutEmptyFramesCb linkPutEmptyFrames;
    /**< Function expected to be called by the LINK to return received frame,
         Once processed by the LINK */
    System_LinkGetOutputBitBufsCb linkGetFullBitBufs;
    /**< Function expected to be called by the LINK to get the input bitbufs */
    System_LinkPutEmptyBitBufsCb linkPutEmptyBitBufs;
    /**< Function expected to be called by the LINK to return received bitbufs,
         Once processed by the LINK */
    System_GetLinkInfoCb getLinkInfo;
    /**<  Function that returns the LINKs output channel configurations */
} System_LinkObj;

/**
 * \brief System Task Descriptor
 *
 */
typedef struct {
    Utils_MbxHndl mbx;
    Task_Handle tsk;
    Task_FuncPtr chainsMainFunc;
    System_LinkObj linkObj[SYSTEM_LINK_ID_MAX];

    volatile Bool haltExecution;

} System_CommonObj;


extern const char *gSystem_nameScanFormat[];

extern const char *gSystem_nameMemoryType[];

extern const char *gSystem_nameOnOff[];


extern System_CommonObj gSystem_objCommon;



Int32 System_init();
Int32 System_deInit();

Int32 System_initCommon();
Int32 System_deInitCommon();

Void System_initLinks();
Void System_deInitLinks();

Int32 System_getLinksFullFrames(UInt32 linkId, UInt16 queId,
                                FVID2_FrameList * pFrameList);
Int32 System_putLinksEmptyFrames(UInt32 linkId, UInt16 queId,
                                 FVID2_FrameList * pFrameList);

Int32 System_getLinksFullBufs(UInt32 linkId, UInt16 queId,
                              Bitstream_BufList * pBufList);
Int32 System_putLinksEmptyBufs(UInt32 linkId, UInt16 queId,
                               Bitstream_BufList * pBufList);

Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd);

Int32 System_registerLink(UInt32 linkId, System_LinkObj * pTskObj);

Utils_TskHndl *System_getLinkTskHndl(UInt32 linkId);

void System_memPrintHeapStatus();
int System_resumeExecution();
int System_haltExecution();

int System_enumAssertCheck();

Int32 System_linkControl_local(UInt32 linkId, UInt32 cmd, Void * pPrm,
                               UInt32 prmSize, Bool waitAck);
Int32 System_sendLinkCmd_local(UInt32 linkId, UInt32 cmd);
Int32 System_linkGetInfo_local(UInt32 linkId, System_LinkInfo * info);

Int32 System_linkControl_remote(UInt32 linkId, UInt32 cmd, Void * pPrm,
                                UInt32 prmSize, Bool waitAck);

Int32 SystemLink_init();
Int32 SystemLink_deInit();

/**
    \brief Start the links and chain system

    This API is typically called in main() just before BIOS_start().

    This API creates a system task that starts executing immediately when BIOS_start()
    gets called.

    This API does the below system init
    - FVID2 init
    - Buffer memory allocator init
    - Optional tiler memory allocator init
    - EVM specific init
    - System task specific init
    - Mailbox, other utility functionality init

    After completing this init it calls the user supplied
    'chainsMainFunc'

    Inside the 'chainsMainFunc' user should init the required links.
    Use System_linkXxxx APIs to connect, create, start and control a chains of links

    \return FVID2_SOK on success
*/
Int32 System_start(Task_FuncPtr chainsMainFunc);

void DM81XX_EDMA3_clrShadowRgn0(int chId);
int System_getEdmaCh(SystemCommon_edmaChPrm *pEdmaPrm);
int System_putEdmaCh(SystemCommon_edmaChPrm *pEdmaPrm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
