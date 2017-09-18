/** ==================================================================
 *  @file   msp_types.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/                                                  
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
 * msp_types.h
 * This header file contains the primitive data types used by MSP components.
 *
 * @path OMAPSW_SysDev\multimedia\services\msp_core\inc
 *
 * @rev 1.0
 */
/* -------------------------------------------------------------------------- 
 */
/* =========================================================================
 * ! ! Revision History ! =================================== ! ! 19-Mar-2008 
 * Abhishek Ranka (asranka@ti.com): Initial version
 * ========================================================================= */
#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#include <xdc/std.h>
#include <string.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
    /* ----- system and platform files ---------------------------- */
/*-------program files ----------------------------------------*/
    // #include <ti/timmosal/timm_osal_types.h>
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
#define NULL ((void *)0x0)
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
    typedef UInt8 MSP_U8;

    /* ========================================================================== 
     */
/**
 * MSP_S8 is an 8 bit signed quantity
*/
    /* ========================================================================== 
     */
    typedef Int8 MSP_S8;

    /* ========================================================================== 
     */
/**
 * MSP_U16 is a 16 bit unsigned quantity
*/
    /* ========================================================================== 
     */
    typedef UInt16 MSP_U16;

    /* ========================================================================== 
     */
/**
 * MSP_S16 is a 16 bit signed quantity
*/
    /* ========================================================================== 
     */
    typedef Int16 MSP_S16;

    /* ========================================================================== 
     */
/**
 * MSP_U32 is a 32 bit unsigned quantity
*/
    /* ========================================================================== 
     */
    typedef UInt32 MSP_U32;

    /* ========================================================================== 
     */
/**
 * MSP_S32 is a 32 bit signed quantity
*/
    /* ========================================================================== 
     */
    typedef Int32 MSP_S32;

    /* ========================================================================== 
     */
/**
 * MSP_STR type is a pointer to a NULL terminated string
*/
    /* ========================================================================== 
     */
    typedef char *MSP_STR;

    /* ========================================================================== 
     */
/**
 * MSP_PTR is a pointer to a void type
*/
    /* ========================================================================== 
     */
    typedef void *MSP_PTR;

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
    typedef UInt32 MSP_OPAQUE;

    /* ========================================================================== 
     */
/**
 * MSP handle: Internal definition is opaque to MSP Clients
*/
    /* ========================================================================== 
     */
    typedef void *MSP_HANDLE;

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
    typedef enum MSP_BOOL {
        MSP_FALSE = 0,
        MSP_TRUE
    } MSP_BOOL;

    // #define MSP_MEMALLOC(SIZE) malloc(SIZE, 32);
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
    typedef struct MSP_TICK64 {
        MSP_U32 unLowPart;
        MSP_U32 unHighPart;
    } MSP_TICK64;
#define MSP_NUMOFCOMPBUFS 3
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
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
    typedef MSP_U32 MSP_ERROR_TYPE;

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

    typedef enum {
        MSP_ERROR_EVENT,                                   // 0 
        MSP_DATA_EVENT,                                    // 1
        MSP_PROFILE_EVENT,                                 // 2
        MSP_OPEN_EVENT,                                    // 3
        MSP_CLOSE_EVENT,                                   // 4
        MSP_CTRLCMD_EVENT                                  // 5
    } MSP_EVENT_TYPE;

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
    typedef enum {
        MSP_CTRLCMD_START,
        MSP_CTRLCMD_PAUSE,
        MSP_CTRLCMD_RESUME,
        MSP_CTRLCMD_FLUSH,
        MSP_CTRLCMD_STOP,
        MSP_CTRLCMD_EXTENDED
    } MSP_CTRLCMD_TYPE;

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
    typedef enum {
        MSP_PROFILE_DEFAULT,
        MSP_PROFILE_HOST,
        MSP_PROFILE_REMOTE,
        MSP_PROFILE_EXTENDED
    } MSP_PROFILE_TYPE;

    /* ======================================================================= 
     */
    /* MSP_INDEXTYPE - MSP Index Type. These are codec specific and
     * enumerated in codec specific interface */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_SDMA_INDEXTYPE_CLRCHANCONFIG,
        MSP_SDMA_INDEXTYPE_GETXFERPROGRESS
    } MSP_INDEXTYPE;

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
    typedef struct {
        MSP_S32 nPortIndex;
        MSP_U32 unNumOfCompBufs;
        MSP_U8 *pBuf[MSP_NUMOFCOMPBUFS];
        MSP_U32 unBufSize[MSP_NUMOFCOMPBUFS];
        MSP_U32 unBufStride[MSP_NUMOFCOMPBUFS];
        MSP_U32 unBufOffset[MSP_NUMOFCOMPBUFS];
        MSP_U32 unMaxBufSize[MSP_NUMOFCOMPBUFS];
        MSP_TICK64 tTimeStamp;
        MSP_U32 unFlag;
        MSP_PTR pCodecData;
        MSP_PTR pAppBufPrivate;
    } MSP_BUFHEADER_TYPE;

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
    typedef struct {
        MSP_PTR pAppData;
         MSP_ERROR_TYPE(*MSP_callback) (MSP_PTR hMSP,
                                        MSP_PTR pAppData,
                                        MSP_EVENT_TYPE tEvent,
                                        MSP_OPAQUE nEventData1,
                                        MSP_OPAQUE nEventData2);
    } MSP_APPCBPARAM_TYPE;

    /* ======================================================================= 
     */
    /* MSP_COMPONENT_TYPE - MSP Component type, handle to this structure will 
     * be passed during component specific MSP init and for all subsequent
     * calls @param pCompPrivate : Placeholder for component specific object 
     * handle @param tAppCBParam : Application callback parameters to be
     * used during any asynchronous event notification to application.
     * @param nCompIndex : Index of this component in MSP Component list
     * @param (*open) : MSP component Open Implementation @param (*control)
     * : MSP component control Implementation @param (*process) : MSP
     * component process Implementation @param (*config) : MSP component
     * config Implementation @param (*query) : MSP component query
     * Implementation @param (*close) : MSP component close Implementation
     * @param (*deInit) : MSP component deInit Implementation */
    /* ======================================================================= 
     */
    typedef struct {
        MSP_PTR pCompPrivate;

        MSP_APPCBPARAM_TYPE tAppCBParam;

        MSP_S32 nCompIndex;

         MSP_ERROR_TYPE(*open) (MSP_HANDLE hMSP, MSP_PTR pCreateParam);

         MSP_ERROR_TYPE(*control) (MSP_HANDLE hMSP,
                                   MSP_CTRLCMD_TYPE tCmd, MSP_PTR pCmdParam);

         MSP_ERROR_TYPE(*process) (MSP_HANDLE hMSP,
                                   MSP_PTR pArg, MSP_BUFHEADER_TYPE * ptBufHdr);

         MSP_ERROR_TYPE(*config) (MSP_HANDLE hMSP,
                                  MSP_INDEXTYPE tConfigIndex,
                                  MSP_PTR pConfigParam);

         MSP_ERROR_TYPE(*query) (MSP_HANDLE hMSP,
                                 MSP_INDEXTYPE tQueryIndex,
                                 MSP_PTR pQueryParam);

         MSP_ERROR_TYPE(*close) (MSP_HANDLE hMSP);

         MSP_ERROR_TYPE(*deInit) (MSP_HANDLE hMSP);

        MSP_STR sName;
    } MSP_COMPONENT_TYPE;
/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_TYPES_H */
