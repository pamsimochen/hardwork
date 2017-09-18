/** ==================================================================
 *  @file   msp_jpege_common.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/jpeg_enc/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ==================================================================== Texas 
 * Instruments OMAP(TM) Platform Software (c) Copyright Texas Instruments,
 * Incorporated. All Rights Reserved. Use of this software is controlled by
 * the terms and conditions found in the license agreement under which this
 * software has been supplied.
 * ==================================================================== */
/* -------------------------------------------------------------------------- 
 */
/* 
 * msp_jpeg_common.h
 * The MSP Jpeg Encode header file defines some common macros used.
 *
 * @path alg\jpeg_enc\inc\
 *
 * @rev 1.0
 */
/* -------------------------------------------------------------------------- 
 */
/* =========================================================================
 * ! ! Revision History !
 * ======================================================================== ! 
 * 05-Feb-2009 Phanish HS [phanish.hs@ti.com]: Initial version
 * ========================================================================= */
#ifndef _MSP_JPEG_COMMON_H_
#define _MSP_JPEG_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
    /* ----- system and platform files ---------------------------- */
/*-------program files ----------------------------------------*/
#include "msp.h"
#include "../msp_jpege.h"
#include "jpeg_enc.h"
/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/*--------macros ----------------------------------------------*/

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

    /* Task-related Events */
#define endEVENT                                (0x00004000)
#define jpegeendEVENT                       (0x00000030)
#define JpegStartEvent                        (0x00000040)
#define jpegeInPortBufEVENT              (0x00001000)
#define jpegeOutPortBufEVENT            (0x00000100)
#define OutputDmaEvent                      (0x00000200)
#define JpegAbortEvent                       (0x00000050)

#define encodeDoneEVENT                      (0x00040000)
    // #define __DEBUG_STEPS__

#ifdef __DEBUG_STEPS__
#define Step0_Event           0x00000001
#define Step1_Event           0x00000002
#define Step2_Event           0x00000003
#define Step3_Event           0x00000004
#endif

#define MAX_JPEG_BUFS          4
#define MSP_JPEG_PRIORITY      13
#define MSP_JPEG_STACK        (8*1024)
#define MSP_JPEG_PIPE_SIZE 5

/******************************************************************************
* Debug Trace defines
******************************************************************************/
#define MSP_JPEGE_Entering()
#define MSP_JPEGE_Exiting(ARG)

    /* ======================================================================= 
     */
/**
 * @def Memory resource related macros
 */
    /* ======================================================================= 
     */
#define MSP_Malloc1(_size, _bytesalign) \
  MSP_MallocExtn(_size, TIMM_OSAL_TRUE, _bytesalign, 0, NULL)

#define MSP_JPEGE_Malloc(SIZE, BYTEALIGNMENT)      MSP_Malloc1(SIZE, BYTEALIGNMENT)
#define MSP_JPEGE_Free(PTR)        if(PTR != NULL) MSP_Free(PTR)
#define MSP_JPEGE_Memset(PTR, SIZE, VAL)   MSP_Memset(PTR, SIZE, VAL)
#define MSP_JPEGE_Memcpy(PTR_Dst, PTR_Src, USIZE)   MSP_Memcpy(PTR_Dst, PTR_Src, USIZE)

    /* ======================================================================= 
     */
/**
 * @def MSP_JPEGE_ASSERT  - Macro to check Parameters. Exit with passed status
 * argument if the condition assert fails. Note that use of this requires
 * a locally declared variable "tErr" of MSP_ERROR_TYPE and a label named
 * "EXIT" typically at the end of the function
 */
    /* ======================================================================= 
     */

#define MSP_JPEGE_ASSERT   MSP_JPEGE_PARAMCHECK
#define MSP_JPEGE_REQUIRE MSP_JPEGE_PARAMCHECK
#define MSP_JPEGE_ENSURE   MSP_JPEGE_PARAMCHECK

#define MSP_JPEGE_PARAMCHECK(C,V)  if (!(C)) { \
    tErr = V;\
    goto EXIT; \
}

    /* ======================================================================= 
     */
/**
 * @def MSP_JPEGE_ASSERT_N  - Macro to check Parameters. Exit with passed status
 * argument if the condition assert fails. Note that use of this requires
 * a locally declared variable "tErr" of MSP_ERROR_TYPE and a label named
 * "EXIT" typically at the end of the function
 */
    /* ======================================================================= 
     */

#define MSP_JPEGE_ASSERT_N   MSP_JPEGE_PARAMCHECK_N
#define MSP_JPEGE_REQUIRE_N MSP_JPEGE_PARAMCHECK_N
#define MSP_JPEGE_ENSURE_N   MSP_JPEGE_PARAMCHECK_N

#define MSP_JPEGE_PARAMCHECK_N(C,V,N)  if (!(C)) { \
    tErr = V;\
    goto EXIT_##N; \
}

    /* ======================================================================= 
     */
    /* MSP_JPEGE_CLIENT_HANDLETYPE - This is the struct for the client
     * specific @param nPorts : Number of entries in the PortDefinition
     * array @param pMspJpegEncHandle : Component handle as returned by
     * MSP_Init @param stMspJpegEncCreateParam : Parameters passed through
     * Create parameter @param pMspJpegEncDataPipe : Pipes for holding
     * buffers that are yet to be processed @param eMspJpegEncProfile : JPEGE 
     * Client profile @param instanceOpened : */
    /* ======================================================================= 
     */
    typedef struct {
    /**< Number of entries in the PortDefinition array */
        MSP_U32 nPorts;

    /** Parameters passed through Create parameter */
        MSP_JPEGE_CREATE_PARAM stMspJpegEncCreateParam;

    /** Pipes for holding buffers that are yet to be processed */
        TIMM_OSAL_PTR pMspJpegEncInDataPipe;

    /** Pipes for holding buffers that are yet to be processed */
        TIMM_OSAL_PTR pMspJpegEncOutDataPipe;

    /** JPEGE Client profile */
        MSP_PROFILE_TYPE eProfile;
        JPEGParams *pJpegParams;
        MSP_BUFHEADER_TYPE *pInBufHdrBeingProcessed;
        MSP_BUFHEADER_TYPE *pOutBufHdrBeingProcessed;

        /* Event-related Parameters */
        TIMM_OSAL_PTR pMspJpegEvent;
        /* New Task handle */
        TIMM_OSAL_PTR pTaskId;
        void * /* RM_HANDLE */ pRMHandle;
        TIMM_OSAL_PTR pMspJpegDoneSem;

        MSP_BOOL bCloseInvoked;
        MSP_BOOL bDeInitInvoked;
        MSP_BOOL bJpegStarted;

        Interrupt_Handle_T *pIntHandle;
        Interrupt_Handle_T *pSimcopDmaIntHandle;
#ifdef __DEBUG_STEPS__
        Interrupt_Handle_T *pIntHandleStep0;
        Interrupt_Handle_T *pIntHandleStep1;
        Interrupt_Handle_T *pIntHandleStep2;
        Interrupt_Handle_T *pIntHandleStep3;
        MSP_U32 ulIrqID;
#endif
    } MSP_JPEGE_CLIENT_HANDLETYPE;

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_JPEG_COMMON_H_ 
                                                            */
