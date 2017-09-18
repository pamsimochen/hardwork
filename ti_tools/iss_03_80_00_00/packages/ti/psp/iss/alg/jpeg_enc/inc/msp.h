/** ==================================================================
 *  @file   msp.h                                                  
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
 * msp.h
 * This header file defines the public interface of MSP(Multimedia Service
 * Provider) to be used by multimedia clients. Please refer the specific codec
 * services interface as well.
 *
 * @path OMAPSW_SysDev\multimedia\services\msp_core\inc
 *
 * @rev 1.2
 */
/* -------------------------------------------------------------------------- 
 */
/* =========================================================================
 * ! ! Revision History ! =================================== ! 20-Mar-2008
 * Abhishek Ranka (asranka@ti.com): Updated MSP_ERROR_TYPE and ! added
 * MSP_ALLPORTINDEX ! ! 19-Mar-2008 Abhishek Ranka (asranka@ti.com): Added
 * new fields in ! MSP_BUFHEADER_TYPE structure, added detailed error
 * enumeration and updated ! MSP_Callback prototype ! ! 26-Nov-2007 Abhishek
 * Ranka (asranka@ti.com): Initial version
 * ========================================================================= */
#ifndef _MSP_H
#define _MSP_H

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
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <ti/psp/iss/core/msp_types.h>

    typedef Int8 TIMM_OSAL_S8;
    typedef Int16 TIMM_OSAL_S16;
    typedef signed long TIMM_OSAL_S32;

#define TIMM_OSAL_INT8_MIN 0xFF
#define TIMM_OSAL_INT8_MAX 0x7F

#define TIMM_OSAL_INT16_MIN 0xFFFF
#define TIMM_OSAL_INT16_MAX 0x7FFF

#define TIMM_OSAL_INT32_MIN 0xFFFFFFFF
#define TIMM_OSAL_INT32_MAX 0x7FFFFFFF

    typedef UInt8 TIMM_OSAL_U8;
    typedef UInt16 TIMM_OSAL_U16;
    typedef unsigned long TIMM_OSAL_U32;

#define TIMM_OSAL_UINT8_MIN 0
#define TIMM_OSAL_UINT8_MAX 0xFF

#define TIMM_OSAL_UINT16_MIN 0
#define TIMM_OSAL_UINT16_MAX 0xFFFF

#define TIMM_OSAL_UINT32_MIN 0
#define TIMM_OSAL_UINT32_MAX 0xFFFFFFFF

    typedef char TIMM_OSAL_CHAR;

    typedef void *TIMM_OSAL_HANDLE;
    typedef void *TIMM_OSAL_PTR;

    typedef enum TIMM_OSAL_BOOL {
        TIMM_OSAL_FALSE = 0,
        TIMM_OSAL_TRUE = !TIMM_OSAL_FALSE,
        TIMM_OSAL_BOOL_MAX = 0x7FFFFFFF
    } TIMM_OSAL_BOOL;

#define TIMM_OSAL_SUSPEND     (0xFFFFFFFF)
#define TIMM_OSAL_NO_SUSPEND  (0)

#define TIMM_OSAL_NULL 0

    typedef TIMM_OSAL_U32 TIMM_OSAL_ERRORTYPE;

    /* Definition of common error codes */
/** there is no error*/
#define TIMM_OSAL_ERR_NONE                 ((TIMM_OSAL_ERRORTYPE) 0x00000000)

/** There is no more memory available*/
#define TIMM_OSAL_ERR_ALLOC                ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000001))

#define TIMM_OSAL_ERR_OUT_OF_RESOURCE      ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000002))

/** Time out */
#define TIMM_OSAL_WAR_TIMEOUT              ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_WAR,TIMM_OSAL_COMP_GENERAL,0x000003))

#define TIMM_OSAL_ERR_PARAMETER            ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000004))

#define TIMM_OSAL_ERR_NOT_READY            ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000005))

#define TIMM_OSAL_ERR_OMX                  ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000006))

#define TIMM_OSAL_ERR_PIPE_FULL            ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000007))

#define TIMM_OSAL_ERR_PIPE_EMPTY           ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000008))

#define TIMM_OSAL_ERR_PIPE_DELETED         ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x000009))

#define TIMM_OSAL_ERR_PIPE_RESET           ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x00000A))

#define TIMM_OSAL_ERR_GROUP_DELETED        ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x00000B))

#define TIMM_OSAL_ERR_UNKNOWN              ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_GENERAL,0x00000C))

#define TIMM_OSAL_ERR_SEM_CREATE_FAILED    ((TIMM_OSAL_ERRORTYPE) TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR,TIMM_OSAL_COMP_SEMAPHORE,0x000001))

    // #endif
/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
 /*--------data declarations -----------------------------------*/
    /* ========================================================================== 
     */
/**
 * NULL
*/
    /* ========================================================================== 
     */
#ifndef NULL
#define NULL ((void *)TIMM_OSAL_NULL)
#endif
#define uint64 UInt64
#define uint32 UInt32
#define uint16 UInt16
#define uint8 UInt8
#define int64 Int64
#define int32 Int32
#define int16 Int16
#define int8 Int8

/*******************************************************************************
* Data Types
*******************************************************************************/
    /* ========================================================================== 
     */
/**
 * MSP_U8 is an 8 bit unsigned quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_U8 MSP_U8;

    /* ========================================================================== 
     */
/**
 * MSP_S8 is an 8 bit signed quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_S8 MSP_S8;

    /* ========================================================================== 
     */
/**
 * MSP_U16 is a 16 bit unsigned quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_U16 MSP_U16;

    /* ========================================================================== 
     */
/**
 * MSP_S16 is a 16 bit signed quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_S16 MSP_S16;

    /* ========================================================================== 
     */
/**
 * MSP_U32 is a 32 bit unsigned quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_U32 MSP_U32;

    /* ========================================================================== 
     */
/**
 * MSP_S32 is a 32 bit signed quantity
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_S32 MSP_S32;

    /* ========================================================================== 
     */
/**
 * MSP_STR type is a pointer to a NULL terminated string
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_CHAR* MSP_STR;

    /* ========================================================================== 
     */
/**
 * MSP_PTR is a pointer to a void type
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_PTR MSP_PTR;

    /* ========================================================================== 
     */
/**
 * MSP_OPAQUE is a type that is big enough for any of the following types
 * to be safely cast to and from. If required to be intepreted by external
 * modules, the intepretation rules must be clearly published.
 * MSP_U8, MSP_U16, MSP_U32
 * MSP_S8, MSP_S16, MSP_S32
 * MSP_PTR, Any pointer type
 * MSP_BOOL
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_U32 MSP_OPAQUE;

    /* ========================================================================== 
     */
/**
 * MSP handle: Internal definition is opaque to MSP Clients
*/
    /* ========================================================================== 
     */
    // typedef TIMM_OSAL_PTR MSP_HANDLE;

/*******************************************************************************
* Enumerated Types
*******************************************************************************/
    /* ========================================================================== 
     */
/**
 * MSP_BOOL is a boolean quantity
*/
    /* ========================================================================== 
     */
    /* typedef enum MSP_BOOL { MSP_FALSE = 0, MSP_TRUE }MSP_BOOL; */

/*******************************************************************************
* Strutures
*******************************************************************************/
    /* ======================================================================= 
     */
    /* MSP_TICK64 - 64-bit signed time stamp type (assuming 32-bit
     * platforms). @param unLowPart : Lower 32-bits of the signed 64 bit
     * tick value @param unHighPart: Higher 32-bits of the signed 64 bit
     * tick value */
    /* ======================================================================= 
     */
    /* typedef struct MSP_TICK64 { MSP_U32 unLowPart; MSP_U32 unHighPart;
     * }MSP_TICK64; */

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
    /* ========================================================================== 
     */
/**
 * MSP_ERROR_TYPE: Classified MSP error enumeration: [MSB] B3 B2 B1 B0 [LSB]
 * B0: Degree of Error: Failure / Warning / No error (2, 1, 0) 
 * B1: MSP Core Errors
 * B2,3: MSP component specific extended errors if any, for extended error info
*/
    /* ========================================================================== 
     */
    // typedef MSP_U32 MSP_ERROR_TYPE;

/****Base Error Codes****/
    /* No error */
#define MSP_ERROR_NONE 0x00
    /* Component warning / Notification. Could be recovered by retrying */
#define MSP_ERROR_WARN 0x01
    /* General Component failure */
#define MSP_ERROR_FAIL 0x02

/****Specific Warnings****/
    /* MSP component is set for an unsupported index */
#define MSP_ERROR_INVALIDINDEX              (0x01 << 8 | MSP_ERROR_WARN)
    /* MSP component is set with an invalid configuration */
#define MSP_ERROR_INVALIDCONFIG             (0x02 << 8 | MSP_ERROR_WARN)
    /* MSP component is sent an invalid command */
#define MSP_ERROR_INVALIDCMD                (0x03 << 8 | MSP_ERROR_WARN)
    /* MSP component configuration is not compatible with current profile */
#define MSP_ERROR_PROFILENOTCOMPATIBLE      (0x04 << 8 | MSP_ERROR_WARN)
    /* MSP component or underlying services is not ready or available */
#define MSP_ERROR_NOTREADY                  (0x05 << 8 | MSP_ERROR_WARN)
    /* Not enough data available */
#define MSP_ERROR_UNDERFLOW                 (0x06 << 8 | MSP_ERROR_WARN)
    /* Buffer cannot hold data available */
#define MSP_ERROR_OVERFLOW                  (0x07 << 8 | MSP_ERROR_WARN)
    /* Dynamic resources are not available presently */
#define MSP_ERROR_DYNRESOURCEUNAVAILABLE    (0x08 << 8 | MSP_ERROR_WARN)

/****Specific Errors****/
    /* Fatal Error. Corruption in handle or working data. Undefined error
     * current instance not recoverable, need to cleanup */
#define MSP_ERROR_FATAL                     (0x01 << 8 | MSP_ERROR_FAIL)
    /* NULL pointer error */
#define MSP_ERROR_NULLPTR                   (0x02 << 8 | MSP_ERROR_FAIL)
    /* Insufficent resources for operation */
#define MSP_ERROR_NOTENOUGHRESOURCES        (0x03 << 8 | MSP_ERROR_FAIL)
    /* The hardware failed to respond as expected */
#define MSP_ERROR_HARDWARE                  (0x04 << 8 | MSP_ERROR_FAIL)
    /* The hardware failed to respond as expected */
#define MSP_ERROR_NOTSUPPORTED                  (0x05 << 8 | MSP_ERROR_FAIL)

    /* ========================================================================== 
     */
/**
 * MSP_NUMOFCOMPBUFS is number of component buffers described by
 * MSP_BUFHEADER_TYPE. Typically, intended for Y,U,V or R,G,B component buffers
 * of a given planar image
*/
    /* ========================================================================== 
     */
#define MSP_NUMOFCOMPBUFS 3

    /* ========================================================================== 
     */
/**
 * MSP_ALLPORTINDEX is the nPortIndex to be used if any port specific operations
 * is to be applied on all ports
*/
    /* ========================================================================== 
     */
#define MSP_ALLPORTINDEX (-1)

/*--------data declarations -----------------------------------*/
/*******************************************************************************
* Data Types
*******************************************************************************/
/*******************************************************************************
* Enumerated Types
*******************************************************************************/
    /* ======================================================================= 
     */
    /* MSP_EVENT_TYPE - Asynchronous Event Notification from MSP to its
     * client @param MSP_ERROR_EVENT : Current operation (indicated by
     * nEventData2) has resulted in error nEventData1 @param MSP_DATA_EVENT
     * : Data processing completed for buffer represented by nEventData1
     * @param MSP_PROFILE_EVENT : Change in Profile, new profile indicated by 
     * * nEventData1. Profile change can happen at any time (init, open or
     * during processing) @param MSP_OPEN_EVENT : MSP_open is completed
     * @param MSP_CLOSE_EVENT : MSP_close is completed @param
     * MSP_CTRLCMD_EVENT : MSP_control command indicated by nEventData1 for
     * port nEventData2 is completed */
    /* ======================================================================= 
     */
    /* typedef enum { MSP_ERROR_EVENT, // 0 MSP_DATA_EVENT, // 1
     * MSP_PROFILE_EVENT, // 2 MSP_OPEN_EVENT, // 3 MSP_CLOSE_EVENT, // 4
     * MSP_CTRLCMD_EVENT // 5 }MSP_EVENT_TYPE; */

    /* ======================================================================= 
     */
    /* MSP_CTRLCMD_TYPE - Control commands used with MSP_control() API
     * @param MSP_CTRLCMD_START : Command to ready MSP component for
     * processing @param MSP_CTRLCMD_PAUSE : Command to pause data
     * processing @param MSP_CTRLCMD_RESUME: Command to resume data
     * processing @param MSP_CTRLCMD_FLUSH : Command to flush all or
     * specific port (indicated by pCmdParam, MSP_ALLPORTINDEX for all ports)
     * @param MSP_CTRLCMD_STOP : Command to halt data processing * @param
     * MSP_CTRLCMD_EXTENDED : Refer to specific MSP component interface for any 
     * extended commands beyond this */
    /* ======================================================================= 
     */
    /* typedef enum { MSP_CTRLCMD_START, MSP_CTRLCMD_PAUSE,
     * MSP_CTRLCMD_RESUME, MSP_CTRLCMD_FLUSH, MSP_CTRLCMD_STOP,
     * MSP_CTRLCMD_EXTENDED }MSP_CTRLCMD_TYPE; */

    /* ======================================================================= 
     */
    /* MSP_PROFILE_TYPE - Profile type enumeration @param
     * MSP_PROFILE_DEFAULT : Default Profile, MSP component will auto-select 
     * the most optimal profile for current configuration @param
     * MSP_PROFILE_HOST : Data Processing on Host processor @param
     * MSP_PROFILE_REMOTE : Data Processing on Remote processor or a
     * hardware accelarator @param MSP_PROFILE_EXTENDED : Refer to specific
     * MSP component interface for any extended profiles beyond this */
    /* ======================================================================= 
     */
    /* typedef enum { MSP_PROFILE_DEFAULT, MSP_PROFILE_HOST,
     * MSP_PROFILE_REMOTE, MSP_PROFILE_EXTENDED }MSP_PROFILE_TYPE; */

    /* ======================================================================= 
     */
    /* MSP_INDEXTYPE - MSP Index Type. These are codec specific and
     * enumerated in codec specific interface */
    /* ======================================================================= 
     */
    /* typedef enum { MSP_SDMA_INDEXTYPE_CLRCHANCONFIG,
     * MSP_SDMA_INDEXTYPE_GETXFERPROGRESS }MSP_INDEXTYPE; */

/*******************************************************************************
* Strutures
*******************************************************************************/
    /* ======================================================================= 
     */
    /* MSP_BUFHEADER_TYPE - MSP Buffer header. BufferType can be overridden
     * by specific codec services, refer to specific codec interfaces @param 
     * nPortIndex : Port Index to which this buffer is set @param
     * unNumOfCompBufs : Number of component buffers @param pBufs : Array of 
     * component buffers @param unBufSize : Number of bytes in each of
     * component buffers @param unBufStride : Stride (in bytes) to access
     * next line of data @param unMaxBufSize : Maximum size (in bytes) of
     * each component buffer @param tTimeStamp : Timestamp associated with
     * the buffer, MSP component can transmit this timestamp to corresponding 
     * output buffer @param unFlag : Buffer meta data (bit-field) @param
     * pCodecData : Additional codec data, NULL if none (refer to specific
     * codec services interface for data structure details) @param
     * pAppBufPrivate : Application private data associated with the buffer
     * (read only to MSP components) */
    /* ======================================================================= 
     */
    /* typedef struct { MSP_S32 nPortIndex; MSP_U32 unNumOfCompBufs; MSP_U8
     * *pBuf[MSP_NUMOFCOMPBUFS]; MSP_U32 unBufSize[MSP_NUMOFCOMPBUFS];
     * MSP_U32 unBufStride[MSP_NUMOFCOMPBUFS]; MSP_U32
     * unBufOffset[MSP_NUMOFCOMPBUFS]; MSP_U32
     * unMaxBufSize[MSP_NUMOFCOMPBUFS]; MSP_TICK64 tTimeStamp; MSP_U32
     * unFlag; MSP_PTR pCodecData; MSP_PTR pAppBufPrivate;
     * }MSP_BUFHEADER_TYPE; */

    /* ======================================================================= 
     */
    /* MSP_APPCBPARAM_TYPE - Structure used to pass in the callback function
     * @param pAppData : Application private data to determine its context
     * during MSP callback @param MSP_Callback : Single callback from MSP for 
     * asynchronous events @cbparam hMSP : MSP handle @cbparam pAppData :
     * Application specific data to retrieve context @cbparam tEvent : Event
     * type for which this notification is given @cbparam nEventData1 : Event 
     * Specific opaque data - refer below @cbparam nEventData2 : Event
     * Specific opaque data - refer below [] -- Represents the data is
     * component specific, refer to its interface tEvent nEventData1
     * nEventData2
     * ------------------------------------------------------------------------
     * MSP_ERROR_EVENT MSP_ERROR_TYPE MSP_EVENT_TYPE (Current event operation
     * which resulted in error) MSP_DATA_EVENT [MSP_BUFHEADER_TYPE*] -
     * MSP_PROFILE_EVENT MSP_PROFILE_TYPE - MSP_OPEN_EVENT - - MSP_CLOSE_EVENT 
     * - - MSP_CTRLCMD_EVENT MSP_CTRLCMD_TYPE [MSP_S32 nPortIndex] (for port
     * specific cmd) */
    /* ======================================================================= 
     */
    /* typedef struct { MSP_PTR pAppData; MSP_ERROR_TYPE
     * (*MSP_callback)(MSP_PTR hMSP, MSP_PTR pAppData, MSP_EVENT_TYPE tEvent,
     * MSP_OPAQUE nEventData1, MSP_OPAQUE nEventData2); }MSP_APPCBPARAM_TYPE; */

/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
    /* MSP Internal, not to be included by application directly */
#include "msp_component.h"

    /* =========================================================================== */
/**
 * @fn MSP_init() This function does the one time initialization operation.
 *
 * @calltype            : Synchronous completion
 *
 * @param *hMSP        : Pointer to the MSP handle specific to MSP component
 *
 * @param sCompName    : NULL terminated string containing name of MSP component
 *
 * @param tProfile     : Profile with which the MSP component must be initialized
 *
 * @param ptCallBacks  : Application params for notifying asynchronous events
 *
 * @pre                 : none
 *
 * @post               : MSP component successfully initialized
 */
    /* =========================================================================== */
    MSP_ERROR_TYPE MSP_init(MSP_HANDLE * hMSP,
                            MSP_STR sCompName,
                            MSP_PROFILE_TYPE tProfile,
                            MSP_APPCBPARAM_TYPE * ptAppCBParam);

    /* =========================================================================== */
/**
 * @mac MSP_open() This macro opens the MSP component for specific create parameters
 * Once opened, the component can be closed using MSP_close and opened again if
 * there is a change in create parameters. MSP_deInit() and MSP_init() is not 
 * required
 *
 * @prototype          : MSP_ERROR_TYPE MSP_open(MSP_HANDLE hMSP,
 *                                               MSP_PTR pCreateParam)
 *
 * @calltype           : Asynchronous completion notified via MSP_callback
 *
 * @param hMSP        : MSP handle
 *
 * @param pCreateParam: MSP Component specific create parameters
 *
 * @pre                : Valid MSP handle
 *
 * @post              : MSP component successfully opened with the given create
 * parameters
 */
    /* =========================================================================== */
#define MSP_open(                                          \
        hMSP,                                              \
        pCreateParam)                                      \
        ((MSP_COMPONENT_TYPE*)hMSP)->open(                \
        hMSP,                                              \
        pCreateParam)

    /* =========================================================================== */
/**
 * @mac MSP_control() This macro is used to control the MSP component
 *
 * @prototype           : MSP_ERROR_TYPE MSP_control(MSP_HANDLE hMSP,
 *                                                   MSP_CTRLCMD_TYPE tCmd,
 *                                                   MSP_PTR pCmdParam)
 *
 * @calltype            : Asynchronous completion notified via MSP_callback
 *
 * @param hMSP         : MSP handle
 *
 * @param tCmd         : MSP control command
 *
 * @param pCmdParam    : Command parameter corresponding to the command sent
 *
 * @pre                 : MSP component successfully opened
 *
 * @post               : Control specific
 */
    /* =========================================================================== */
#ifdef MSP_USE_MACROS

#define MSP_control(                                       \
        hMSP,                                              \
        tCmd,                                              \
        pCmdParam)                                         \
        ((MSP_COMPONENT_TYPE*)hMSP)->control(              \
        hMSP,                                              \
        (MSP_CTRLCMD_TYPE)tCmd,                            \
        pCmdParam)
#else

    MSP_ERROR_TYPE MSP_control(MSP_HANDLE hMSP, MSP_CTRLCMD_TYPE tCmd,
                               MSP_PTR pCmdParam);

#endif

    /* =========================================================================== */
/**
 * @mac MSP_process() This macro provides MSP component with data to process
 *
 * @prototype           : MSP_ERROR_TYPE MSP_process(MSP_HANDLE hMSP,
 *                                                   MSP_PTR pArg,
 *                                                   MSP_BUFHEADER_TYPE *ptBufHdr)
 *
 * @calltype            : Asynchronous completion notified via MSP_callback
 *
 * @param hMSP         : MSP handle
 *
 * @param pArg         : Process related parameters which MSP component expects.
 * Refer to component specific interface spec.
 *
 * @param ptBufHdr     : Buffer header describing the data to be processsed.
 * Refer component specific interface for actual data structure expected.
 *
 * @pre                 : MSP component successfully opened and started
 *
 *                      : ptBufHdr sent to MSP_process must be in HEAP (i.e.
 *                        valid accross MSP client and MSP component contexts)
 *
 * @post               : Refer MSP component specific interface spec
 */
    /* =========================================================================== */
#ifdef MSP_USE_MACROS
#define MSP_process(                                       \
        hMSP,                                              \
        pArg,                                              \
        ptBufHdr)                                          \
        ((MSP_COMPONENT_TYPE*)hMSP)->process(              \
        hMSP,                                              \
        pArg,                                              \
        ptBufHdr)
#else
    MSP_ERROR_TYPE MSP_process(MSP_HANDLE hMSP, MSP_PTR pArg,
                               MSP_BUFHEADER_TYPE * ptBufHdr);
#endif

    /* =========================================================================== */
/**
 * @mac MSP_config() This macro configures MSP component for a particular setting
 *
 * @prototype          : MSP_ERROR_TYPE MSP_config(MSP_HANDLE hMSP,
 *                                                 MSP_INDEXTYPE tConfigIndex,
 *                                                 MSP_PTR pConfigParam)
 *
 * @calltype           : Synchronous completion
 *
 * @param hMSP        : MSP handle
 *
 * @param tConfigIndex: Index of setting to be configured
 *
 * @param pConfigParam: Parameters describing the settings
 *
 * @pre                : MSP component successfully initialized
 *
 * @post               : Refer MSP component specific interface spec
 */
    /* =========================================================================== */
#ifdef MSP_USE_MACROS
#define MSP_config(                                     \
        hMSP,                                           \
        tConfigIndex,                                   \
        pConfigParam)                                   \
        ((MSP_COMPONENT_TYPE*)hMSP)->config(            \
        hMSP,                                           \
        (MSP_INDEXTYPE)tConfigIndex,                    \
        pConfigParam)
#else
    MSP_ERROR_TYPE MSP_config(MSP_HANDLE hMSP, MSP_INDEXTYPE tConfigIndex,
                              MSP_PTR pConfigParam);
#endif
    /* =========================================================================== */
/**
 * @mac MSP_query() This macro is used to query MSP component for a particular
 * setting
 *
 * @prototype           : MSP_ERROR_TYPE MSP_query(MSP_HANDLE hMSP,
 *                                                 MSP_INDEXTYPE tQueryIndex,
 *                                                 MSP_PTR pQueryParam)
 *
 * @calltype            : Synchronous completion
 *
 * @param hMSP         : MSP handle
 *
 * @param tQueryIndex  : Index of setting to be queried
 *
 * @param pQueryParam  : Setting specific queried parameters populated by MSP
 * component
 *
 * @pre                 : MSP component successfully initialized
 *
 * @post               : Refer MSP component specific interface spec
 */
    /* =========================================================================== */
#ifdef MSP_USE_MACROS
#define MSP_query(                                          \
        hMSP,                                               \
        tQueryIndex,                                        \
        pQueryParam)                                        \
        ((MSP_COMPONENT_TYPE*)hMSP)->query(                 \
        hMSP,                                               \
        (MSP_INDEXTYPE)tQueryIndex,                         \
        pQueryParam)
#else

    MSP_ERROR_TYPE MSP_query(MSP_HANDLE hMSP, MSP_INDEXTYPE tQueryIndex,
                             MSP_PTR pQueryParam);

#endif                                                     // MSP_USE_MACROS

    /* =========================================================================== */
/**
 * @mac MSP_close() This macro closes MSP component
 *
 * @prototype           : MSP_ERROR_TYPE MSP_close(MSP_HANDLE hMSP)
 *
 * @calltype            : Asynchronous completion notified via MSP_callback
 *
 * @param hMSP         : MSP handle
 *
 * @pre                 : Valid MSP handle
 *
 * @post               : MSP component in initialized state, can be opened
 * again with a new set of create parameters. Refer MSP component specific
 * interface spec
 */
    /* =========================================================================== */
#define MSP_close(                                        \
        hMSP)                                             \
        ((MSP_COMPONENT_TYPE*)hMSP)->close(               \
        hMSP)

    /* =========================================================================== */
/**
 * @fn MSP_deInit() This function deinitializes the MSP component
 *
 * @calltype            : Synchronous completion
 *
 * @param hMSP         : MSP handle
 *
 * @pre                 : Valid MSP handle
 *
 * @post               : MSP handle becomes invalid. Must initialize MSP again
 * for the desired component
 */
    /* =========================================================================== */
    MSP_ERROR_TYPE MSP_deInit(MSP_HANDLE hMSP);

    /* =========================================================================== */
/**
 * @fn MSP_getName() This function returns Name of the MSP component
 *
 * @calltype            : Synchronous completion
 *
 * @param hMSP          : MSP handle
 *
 * @pre                 : Valid MSP handle
 *
 * @post                : None
 */
    /* =========================================================================== */
#define MSP_getName(hMSP)       (((MSP_COMPONENT_TYPE*)hMSP)->sName)

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
    /* =========================================================================== */
/**
 * @fn MSP_MallocExtn() - Allocate a chunk of memory.
 *
 *  @ param size             :Size of the memory to be allocated.
 *
 *  @ param bBlockContiguous :Whether a physically contiguous block is required.
 *
 *  @ param unBlockAlignment :Alignment of the memory chunk that will be 
 *                             allocated.
 *
 *  @ param tMemSegId        :Memory segment from wich memory will be allocated.
 *
 *  @ param hHeap            :Heap from which memory will be allocated.
 */
    /* =========================================================================== */
    TIMM_OSAL_PTR MSP_MallocExtn(TIMM_OSAL_U32 size,
                                 TIMM_OSAL_BOOL bBlockContiguous,
                                 TIMM_OSAL_U32 unBlockAlignment,
                                 TIMM_OSAL_U32 tMemSegId, TIMM_OSAL_PTR hHeap);

    /* =========================================================================== */
/**
 * @fn MSP_Free() - Free a previously allocated chunk of memory. 
 *
 *  @ param pData        :Pointer to the memory chunk that has to be freed.
 */
    /* =========================================================================== */
    void MSP_Free(TIMM_OSAL_PTR pData);

    /* =========================================================================== */
/**
 * @fn MSP_Memset() - Sets each byte in the given chunk of memory to the
 *                          given value.
 *
 *  @ param pBuffer        :Pointer to the start of the memory chunk.
 *
 *  @ param uValue         :Value to be set.
 *
 *  @ param uSize          :Size from the start pointer till which the value 
 *                          will be set.
 */
    /* =========================================================================== */
    TIMM_OSAL_U32 MSP_Memset(TIMM_OSAL_PTR pBuffer,
                             TIMM_OSAL_U8 uValue, TIMM_OSAL_U32 uSize);

    /* =========================================================================== */
/**
 * @fn MSP_Memcpy() - Copy a memory chunk.
 *
 *  @ param pBufDst        :Destination memory address.
 *
 *  @ param pBufSrc        :Source memory address.
 *
 *  @ param uSize          :Size of memory to be copied.
 */
    /* =========================================================================== */
    TIMM_OSAL_U32 MSP_Memcpy(TIMM_OSAL_PTR pBufDst,
                             TIMM_OSAL_PTR pBufSrc, TIMM_OSAL_U32 uSize);

    /* ======================================================================= 
     */
/**
 * Memory resource related macros
 */

    /* ======================================================================= 
     */
#define MSP_MEMALLOC(SIZE)      MSP_MallocExtn(SIZE, TIMM_OSAL_TRUE, 4, 0, NULL)

#define MSP_MEMFREE(PTR)        MSP_Free(PTR)

#define MSP_MEMSET(PTR, SIZE)   MSP_Memset(PTR, 0, SIZE)

    /* ======================================================================= 
     */
/**
 * MSP Trace macros
 */
    /* ======================================================================= 
     */
#ifdef MSP_TRACE_ENABLE
#define MSP_TRACE(ARGS,...) MSP_PRINT(ARGS,##__VA_ARGS__)
#else
#define MSP_TRACE(ARGS,...)
#endif

#define MSP_ERROR(S) MSP_PRINT("%s [%s]:: %d : %s\n",__FILE__, __FUNCTION__, __LINE__, "ERROR:" ##S)
    extern int Vps_printf(char *format, ...);

#define MSP_PRINT(ARGS,...) Vps_printf(ARGS,##__VA_ARGS__)
    /* ======================================================================= 
     */
/**
 * MSP_ASSERT  - Macro to check Parameters. Exit with passed status 
 * argument if the condition assert fails. Note that use of this requires
 * a locally declared variable "tErr" of MSP_ERROR_TYPE and a label named
 * "EXIT" typically at the end of the function
 */
    /* ======================================================================= 
     */
#define MSP_ASSERT  MSP_PARAMCHECK
#define MSP_REQUIRE MSP_PARAMCHECK
#define MSP_ENSURE  MSP_PARAMCHECK

#define MSP_PARAMCHECK(C,V)  if (!(C)) { tErr = V;\
    MSP_ERROR(#C); goto EXIT; }

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
    typedef enum {
        MSP_CONFIG_ROT_0 = 0 << 0,
        MSP_CONFIG_ROT_FLIP = 1 << 0,
        MSP_CONFIG_ROT_MIRROR = 1 << 1,
        MSP_CONFIG_ROT_90 = 1 << 2,
        MSP_CONFIG_ROT_90_FLIP = MSP_CONFIG_ROT_90 | MSP_CONFIG_ROT_FLIP,
        MSP_CONFIG_ROT_90_MIRROR = MSP_CONFIG_ROT_90 | MSP_CONFIG_ROT_MIRROR,
        MSP_CONFIG_ROT_180 = MSP_CONFIG_ROT_FLIP | MSP_CONFIG_ROT_MIRROR,
        MSP_CONFIG_ROT_270 =
            MSP_CONFIG_ROT_90 | MSP_CONFIG_ROT_MIRROR | MSP_CONFIG_ROT_FLIP
    } MSP_CONFIG_ROT_TYPE;

/*--------macros ----------------------------------------------*/

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_H */
