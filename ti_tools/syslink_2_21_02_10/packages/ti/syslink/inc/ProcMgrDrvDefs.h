/** 
 *  @file   ProcMgrDrvDefs.h
 *
 *  @brief      Definitions of ProcMgrDrv types and structures.
 *
 *
 */
/* 
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */



#ifndef ProcMgrDrvDefs_H_0xf2ba
#define ProcMgrDrvDefs_H_0xf2ba


/* Module headers */
#include <ti/syslink/ProcMgr.h>
#include "_ProcMgr.h"
#include "ProcMgrCmdBase.h"
#include <ti/syslink/inc/IoctlDefs.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Base structure for ProcMgr command args. This needs to be the first
 *          field in all command args structures.
 */
typedef struct ProcMgr_CmdArgs_tag {
    Int                 apiStatus;
    /*!< Status of the API being called. */
} ProcMgr_CmdArgs;


/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for ProcMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for ProcMgr
 */
#define PROCMGR_BASE_CMD                 150

/*!
 *  @brief  Command for ProcMgr_getConfig
 */
#define CMD_PROCMGR_GETCONFIG               _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 1u,\
                                            ProcMgr_CmdArgsGetConfig)


/*!
 *  @brief  Command for ProcMgr_setup
 */
#define CMD_PROCMGR_SETUP                   _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 2u,\
                                            ProcMgr_CmdArgsSetup)
/*!
 *  @brief  Command for ProcMgr_setup
 */
#define CMD_PROCMGR_DESTROY                 _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 3u,\
                                            ProcMgr_CmdArgsDestroy)
/*!
 *  @brief  Command for ProcMgr_destroy
 */
#define CMD_PROCMGR_PARAMS_INIT              _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 4u,\
                                            ProcMgr_CmdArgsParamsInit)
/*!
 *  @brief  Command for ProcMgr_create
 */
#define CMD_PROCMGR_CREATE                  _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 5u,\
                                            ProcMgr_CmdArgsCreate)
/*!
 *  @brief  Command for ProcMgr_delete
 */
#define CMD_PROCMGR_DELETE                  _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 6u,\
                                            ProcMgr_CmdArgsDelete)
/*!
 *  @brief  Command for ProcMgr_open
 */
#define CMD_PROCMGR_OPEN                    _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 7u,\
                                            ProcMgr_CmdArgsOpen)
/*!
 *  @brief  Command for ProcMgr_close
 */
#define CMD_PROCMGR_CLOSE                   _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 8u,\
                                            ProcMgr_CmdArgsClose)
/*!
 *  @brief  Command for ProcMgr_getAttachParams
 */
#define CMD_PROCMGR_GETATTACHPARAMS         _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 9u,\
                                            ProcMgr_CmdArgsGetAttachParams)
/*!
 *  @brief  Command for ProcMgr_attach
 */
#define CMD_PROCMGR_ATTACH                  _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 10u,\
                                            ProcMgr_CmdArgsAttach)
/*!
 *  @brief  Command for ProcMgr_detach
 */
#define CMD_PROCMGR_DETACH                  _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 11u,\
                                            ProcMgr_CmdArgsDetach)
/*!
 *  @brief  Command for ProcMgr_load
 */
#define CMD_PROCMGR_LOAD                    _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 12u,\
                                            ProcMgr_CmdArgsLoad)
/*!
 *  @brief  Command for ProcMgr_unload
 */
#define CMD_PROCMGR_UNLOAD                  _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 13u,\
                                            ProcMgr_CmdArgsUnload)
/*!
 *  @brief  Command for ProcMgr_getStartParams
 */
#define CMD_PROCMGR_GETSTARTPARAMS          _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 14u,\
                                            ProcMgr_CmdArgsGetStartParams)
/*!
 *  @brief  Command for ProcMgr_start
 */
#define CMD_PROCMGR_START                   _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 15u,\
                                            ProcMgr_CmdArgsStart)
/*!
 *  @brief  Command for ProcMgr_stop
 */
#define CMD_PROCMGR_STOP                    _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 16u,\
                                            ProcMgr_CmdArgsStop)
/*!
 *  @brief  Command for ProcMgr_getState
 */
#define CMD_PROCMGR_GETSTATE                 _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 17u,\
                                            ProcMgr_CmdArgsGetState)
/*!
 *  @brief  Command for ProcMgr_read
 */
#define CMD_PROCMGR_READ                     _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 18u,\
                                            ProcMgr_CmdArgsRead)
/*!
 *  @brief  Command for ProcMgr_write
 */
#define CMD_PROCMGR_WRITE                   _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 19u,\
                                            ProcMgr_CmdArgsWrite)
/*!
 *  @brief  Command for ProcMgr_control
 */
#define CMD_PROCMGR_CONTROL                 _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 20u,\
                                            ProcMgr_CmdArgsControl)
/*!
 *  @brief  Command for ProcMgr_translateAddr
 */
#define CMD_PROCMGR_TRANSLATEADDR           _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 22u,\
                                            ProcMgr_CmdArgsTranslateAddr)
/*!
 *  @brief  Command for ProcMgr_getSymbolAddress
 */
#define CMD_PROCMGR_GETSYMBOLADDRESS        _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 23u,\
                                            ProcMgr_CmdArgsGetSymbolAddress)
/*!
 *  @brief  Command for ProcMgr_map
 */
#define CMD_PROCMGR_MAP                      _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 24u,\
                                            ProcMgr_CmdArgsMap)
/*!
 *  @brief  Command for ProcMgr_unmap
 */
#define CMD_PROCMGR_UNMAP                   _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 25u,\
                                            ProcMgr_CmdArgsUnmap)

/*!
 *  @brief  Command for ProcMgr_getProcInfo
 */
#define CMD_PROCMGR_GETPROCINFO             _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 27u,\
                                            ProcMgr_CmdArgsGetProcInfo)
/*!
 *  @brief  Command for ProcMgr_getSectionInfo
 */
#define CMD_PROCMGR_GETSECTIONINFO          _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 28u,\
                                            ProcMgr_CmdArgsGetSectionInfo)
/*!
 *  @brief  Command for ProcMgr_getSectionData
 */
#define CMD_PROCMGR_GETSECTIONDATA          _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 29u,\
                                            ProcMgr_CmdArgsGetSectionData)
/*!
 *  @brief  Command for ProcMgr_getLoadedFileId
 */
#define CMD_PROCMGR_GETLOADEDFILEID         _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 30u,\
                                            ProcMgr_CmdArgsGetLoadedFileId)

/*!
 *  @brief  Command for ProcMgr_getLoadedFileId
 */
#define CMD_PROCMGR_CONFIGSYSMEMMAP         _IOWR(PRCMGRCMDBASE,\
                                            PROCMGR_BASE_CMD + 31u,\
                                            ProcMgr_CmdArgsGetLoadedFileId)

/*  ----------------------------------------------------------------------------
 *  Command arguments for ProcMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for ProcMgr_getConfig
 */
typedef struct ProcMgr_CmdArgsGetConfig_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Config *    cfg;
    /*!< Pointer to the ProcMgr module configuration structure in which the
         default config is to be returned. */
} ProcMgr_CmdArgsGetConfig;

/*!
 *  @brief  Command arguments for ProcMgr_setup
 */
typedef struct ProcMgr_CmdArgsSetup_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Config *    cfg;
    /*!< Optional ProcMgr module configuration. If provided as NULL, default
         configuration is used. */
} ProcMgr_CmdArgsSetup;

/*!
 *  @brief  Command arguments for ProcMgr_destroy
 */
typedef struct ProcMgr_CmdArgsDestroy_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
} ProcMgr_CmdArgsDestroy;

/*!
 *  @brief  Command arguments for ProcMgr_Params_init
 */
typedef struct ProcMgr_CmdArgsParamsInit_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object. */
    ProcMgr_Params *    params;
    /*!< Pointer to the ProcMgr instance params structure in which the default
         params is to be returned. */
} ProcMgr_CmdArgsParamsInit;

/*!
 *  @brief  Command arguments for ProcMgr_create
 */
typedef struct ProcMgr_CmdArgsCreate_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    UInt16              procId;
    /*!< Processor ID represented by this ProcMgr instance */
    UInt16              maxMemoryRegions;
    /*!< Maxmimum memory mappings for this ProcMgr instance */
    ProcMgr_Params      params;
    /*!< ProcMgr instance configuration parameters. */
    ProcMgr_Handle      handle;
    /*!< Handle to the created ProcMgr object */
} ProcMgr_CmdArgsCreate;

/*!
 *  @brief  Command arguments for ProcMgr_delete
 */
typedef struct ProcMgr_CmdArgsDelete_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Pointer to Handle to the ProcMgr object */
} ProcMgr_CmdArgsDelete;

/*!
 *  @brief  Command arguments for ProcMgr_open
 */
typedef struct ProcMgr_CmdArgsOpen_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    UInt16              procId;
    /*!< Processor ID represented by this ProcMgr instance */
    UInt16              maxMemoryRegions;
    /*!< Maxmimum memory mappings for this ProcMgr instance */
    ProcMgr_Handle      handle;
    /*!< Handle to the opened ProcMgr object. */
} ProcMgr_CmdArgsOpen;

/*!
 *  @brief  Command arguments for ProcMgr_close
 */
typedef struct ProcMgr_CmdArgsClose_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
} ProcMgr_CmdArgsClose;

/*!
 *  @brief  Command arguments for ProcMgr_getAttachParams
 */
typedef struct ProcMgr_CmdArgsGetAttachParams_tag {
    ProcMgr_CmdArgs         commonArgs;
    /*!< Common command args */
    ProcMgr_Handle          handle;
    /*!< Handle to the ProcMgr object. */
    ProcMgr_AttachParams *  params;
    /*!< Pointer to the ProcMgr attach params structure in which the default
         params is to be returned. */
} ProcMgr_CmdArgsGetAttachParams;

/*!
 *  @brief  Command arguments for ProcMgr_attach
 */
typedef struct ProcMgr_CmdArgsAttach_tag {
    ProcMgr_CmdArgs         commonArgs;
    /*!< Common command args */
    ProcMgr_Handle          handle;
    /*!< Handle to the ProcMgr object. */
    ProcMgr_AttachParams *  params;
    /*!< Optional ProcMgr attach parameters.  */
} ProcMgr_CmdArgsAttach;

/*!
 *  @brief  Command arguments for ProcMgr_detach
 */
typedef struct ProcMgr_CmdArgsDetach_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
} ProcMgr_CmdArgsDetach;

/*!
 *  @brief  Command arguments for ProcMgr_load
 */
typedef struct ProcMgr_CmdArgsLoad_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    String              imagePath;
    /*!< Full file path */
    UInt32              imagePathLen;
    /*!< Length of image path */
    UInt32              argc;
    /*!< Number of arguments */
    String *            argv;
    /*!< String array of arguments */
    Ptr                 params;
    /*!< Loader specific parameters */
    UInt32              fileId;
    /*!< Return parameter: ID of the loaded file */
} ProcMgr_CmdArgsLoad;

/*!
 *  @brief  Command arguments for ProcMgr_unload
 */
typedef struct ProcMgr_CmdArgsUnload_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    UInt32              fileId;
    /*!< ID of the loaded file to be unloaded */
} ProcMgr_CmdArgsUnload;

/*!
 *  @brief  Command arguments for ProcMgr_getStartParams
 */
typedef struct ProcMgr_CmdArgsGetStartParams_tag {
    ProcMgr_CmdArgs         commonArgs;
    /*!< Common command args */
    ProcMgr_Handle          handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_StartParams *   params;
    /*!< Pointer to the ProcMgr start params structure in which the default
         params is to be returned. */
} ProcMgr_CmdArgsGetStartParams;

/*!
 *  @brief  Command arguments for ProcMgr_start
 */
typedef struct ProcMgr_CmdArgsStart_tag {
    ProcMgr_CmdArgs         commonArgs;
    /*!< Common command args */
    ProcMgr_Handle          handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_StartParams *   params;
    /*!< Optional ProcMgr start parameters. */
} ProcMgr_CmdArgsStart;

/*!
 *  @brief  Command arguments for ProcMgr_stop
 */
typedef struct ProcMgr_CmdArgsStop_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
} ProcMgr_CmdArgsStop;

/*!
 *  @brief  Command arguments for ProcMgr_getState
 */
typedef struct ProcMgr_CmdArgsGetState_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!<  Handle to the ProcMgr object */
    ProcMgr_State       procMgrState;
    /*!< Current state of the ProcMgr object. */
} ProcMgr_CmdArgsGetState;

/*!
 *  @brief  Command arguments for ProcMgr_read
 */
typedef struct ProcMgr_CmdArgsRead_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    UInt32              procAddr;
    /*!< Address in space processor's address space of the memory region to
         read from. */
    UInt32              numBytes;
    /*!< IN/OUT parameter. As an IN-parameter, it takes in the number of bytes
         to be read. When the function returns, this parameter contains the
         number of bytes actually read. */
    Ptr                 buffer;
    /*!< User-provided buffer in which the slave processor's memory contents
         are to be copied. */
} ProcMgr_CmdArgsRead;

/*!
 *  @brief  Command arguments for ProcMgr_write
 */
typedef struct ProcMgr_CmdArgsWrite_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    UInt32              procAddr;
    /*!< Address in space processor's address space of the memory region to
         write into. */
    UInt32              numBytes;
    /*!< IN/OUT parameter. As an IN-parameter, it takes in the number of bytes
         to be written. When the function returns, this parameter contains the
         number of bytes actually written. */
    Ptr                 buffer;
    /*!< User-provided buffer from which the data is to be written into the
         slave processor's memory. */
} ProcMgr_CmdArgsWrite;

/*!
 *  @brief  Command arguments for ProcMgr_control
 */
typedef struct ProcMgr_CmdArgsControl_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    Int32               cmd;
    /*!< Device specific processor command */
    Ptr                 arg;
    /*!< Arguments specific to the type of command. */
} ProcMgr_CmdArgsControl;

/*!
 *  @brief  Command arguments for ProcMgr_translateAddr
 */
typedef struct ProcMgr_CmdArgsTranslateAddr_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    Ptr                 dstAddr;
    /*!< Return parameter: Pointer to receive the translated address. */
    ProcMgr_AddrType    dstAddrType;
    /*!< Destination address type requested */
    Ptr                 srcAddr;
    /*!< Source address in the source address space */
    ProcMgr_AddrType    srcAddrType;
    /*!< Source address type */
} ProcMgr_CmdArgsTranslateAddr;

/*!
 *  @brief  Command arguments for ProcMgr_getSymbolAddress
 */
typedef struct ProcMgr_CmdArgsGetSymbolAddress_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    UInt32              fileId;
    /*!< ID of the file received from the load function */
    String              symbolName;
    /*!< Name of the symbol */
    UInt32              symValue;
    /*!< Return parameter: Symbol address */
} ProcMgr_CmdArgsGetSymbolAddress;

/*!
 *  @brief  Command arguments for ProcMgr_map
 */
typedef struct ProcMgr_CmdArgsMap_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_MapMask     mapType;
    /*!< Type of mapping. */
    ProcMgr_AddrInfo *  addrInfo;
    /*!< address information */
    ProcMgr_AddrType    srcAddrType;
    /*!< Address type of source address */
} ProcMgr_CmdArgsMap;

/*!
 *  @brief  Command arguments for ProcMgr_unmap
 */
typedef struct ProcMgr_CmdArgsUnmap_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_MapMask     mapType;
    /*!< Type of mapping. */
    ProcMgr_AddrInfo *  addrInfo;
    /*!< address information */
    ProcMgr_AddrType    srcAddrType;
    /*!< Address type of source address */
} ProcMgr_CmdArgsUnmap;


/*!
 *  @brief  Command arguments for ProcMgr_getProcInfo
 */
typedef struct ProcMgr_CmdArgsGetProcInfo_tag {
    ProcMgr_CmdArgs     commonArgs;
    /*!< Common command args */
    ProcMgr_Handle      handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_ProcInfo *  procInfo;
    /*!< Pointer to the ProcInfo object to be populated. */
    UInt32              maxMemEntries;
    /*!< Maximum number of memEntries in ProcInfo */
} ProcMgr_CmdArgsGetProcInfo;


/*!
 *  @brief  Command arguments for ProcMgr_CmdArgsGetSectionInfo
 */
typedef struct ProcMgr_CmdArgsGetSectionInfo_tag {
    ProcMgr_CmdArgs        commonArgs;
    /*!< Common command args */
    ProcMgr_Handle         handle;
    /*!< Handle to the ProcMgr object */
    ProcMgr_SectionInfo *  sectionInfo;
    /*!< Pointer to the SectionInfo object to be populated. */
    String                 sectionName;
    /*!< Section name */
    UInt32                 fileId;
    /*!< File identifier */
} ProcMgr_CmdArgsGetSectionInfo;


/*!
 *  @brief  Command arguments for ProcMgr_CmdArgsGetSectionData
 */
typedef struct ProcMgr_CmdArgsGetSectionData_tag {
    ProcMgr_CmdArgs        commonArgs;
    /*!< Common command args */
    ProcMgr_Handle         handle;
    /*!< Handle to the ProcMgr object */
    UInt32                 fileId;
    /*!< File identifier */
    ProcMgr_SectionInfo *  sectionInfo;
    /*!< Pointer to the SectInfo populated by _getSectionInfo. */
    Ptr                    buffer;
    /*!< buffer */
} ProcMgr_CmdArgsGetSectionData;


/*!
 *  @brief  Command arguments for ProcMgr_CmdArgsGetLoadedFileId
 */
typedef struct ProcMgr_CmdArgsGetLoadedFileId_tag {
    ProcMgr_CmdArgs        commonArgs;
    /*!< Common command args */
    ProcMgr_Handle         handle;
    /*!< Handle to the ProcMgr object */
    UInt32                 fileId;
    /*!< File Identifier*/
} ProcMgr_CmdArgsGetLoadedFileId;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* ProcMgrDrvDefs_H_0xf2ba */
