/*
 *  @file   SlaveLoader.c
 *
 *  @brief      Slave Loader application.
 *
 *
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



/* Standard headers */
#include <ti/syslink/Std.h>
#include <stdio.h>

#if !defined(__KERNEL__)
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#endif /* if !defined(__KERNEL__) */

/* OSAL & Utils headers */
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/IpcHost.h>

/* Module level headers */
#include <ti/syslink/ProcMgr.h>
#include <ti/ipc/MultiProc.h>

/* Sample app headers */
#include <ti/syslink/samples/hlos/common/SysLinkSamples.h>
#include "SlaveLoader.h"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

typedef struct MapStruct {
    UInt32 slaveVirt;
    UInt32 slavePhys;
    UInt32 masterPhys;
    UInt32 size;
    UInt32 mapMask;
    int isCached;
} MapStruct;


static Int SlaveLoader_mapByFile(ProcMgr_Handle handle, String mapFile,
        UInt16 procId, int mapFlag);
static int isMapLine(char *line, UInt16 procId);
static int initFromLine(char *line, MapStruct *mapStruct);
static void dumpMapInfo(ProcMgr_Handle handle);

#define DUMPMAPINFO 0

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to execute the startup for SlaveLoader sample application
 *  with remote args
 */
Int
SlaveLoader_startupArgs (UInt16 procId, String action, String filePath,
        String mapFile, Int remoteArgc, String remoteArgv[])
{
    Int                          status = 0;
    ProcMgr_Handle               handle = NULL;
    ProcMgr_AttachParams         attachParams;
    ProcMgr_StartParams          startParams;
    UInt32                       fileId;
    Int                          i;

    status = ProcMgr_open (&handle, procId);

    if (status >= 0) {
        if ((strcmp(action, "startup") == 0) ||
            (strcmp(action, "all") == 0) ||
            (strcmp(action, "powerup") == 0) ||
            (strcmp(action, "load") == 0)) {

            ProcMgr_getAttachParams(NULL, &attachParams);
            /* Default params will be used if NULL is passed. */
            status = ProcMgr_attach(handle, &attachParams);
            if (status < 0) {
                Osal_printf ("ProcMgr_attach failed [0x%x]\n", status);
            }
            else {
                Osal_printf ("Attached to slave procId %d.\n", procId);
            }
        }

        if ((status >= 0) && (mapFile != NULL) &&
                ((strcmp(action, "startup") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "load") == 0))) {
            /* Parse map file and ProcMgr_map all entries */
#if DUMPMAPINFO
            Osal_printf("------before-------\n");
            dumpMapInfo(handle);
            Osal_printf("-------------------\n");
#endif

            status = SlaveLoader_mapByFile(handle, mapFile, procId, TRUE);
            if (status < 0) {
                Osal_printf ("SlaveLoader_mapByFile failed [0x%x]\n", status);
            }
            else {
                Osal_printf ("Mapped entries in %s to slave procId %d.\n",
                        mapFile, procId);
#if DUMPMAPINFO
                Osal_printf("------after-------\n");
                dumpMapInfo(handle);
                Osal_printf("-------------------\n");
#endif
            }
        }

        if ((status >= 0) && ((strcmp(action, "startup") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "load") == 0))) {

            if (remoteArgc > 0) {
                Osal_printf("Loading '%s", remoteArgv[0]);
                for (i = 1; i < remoteArgc; i++) {
                    Osal_printf(" %s", remoteArgv[i]);
                }
                Osal_printf("' to procId %d.\n", procId);
            }
            else {
                Osal_printf("Loading procId %d.\n", procId);
            }

            status = ProcMgr_load(handle, filePath, remoteArgc, remoteArgv,
                                  NULL, &fileId);
            if (status < 0) {
                Osal_printf ("Error in ProcMgr_load [0x%x]\n", status);
            }
            else {
                Osal_printf ("Loaded file %s on slave procId %d.\n", filePath, procId);
            }
        }

        if ((status >= 0)
            &&  ((strcmp (action, "startup") == 0) ||
                 (strcmp (action, "all") == 0) ||
                 (strcmp (action, "start") == 0))) {
            ProcMgr_getStartParams(handle, &startParams);
            status = ProcMgr_start(handle, &startParams);
            if (status < 0) {
                Osal_printf("ProcMgr_start failed [0x%x]\n", status);
            }
            else {
                Osal_printf("Started slave procId %d.\n", procId);
            }
        }

        /*
         * Since ProcMgr_open() succeeded, unconditionally ProcMgr_close()
         * Note that we don't check the return value here, and instead return
         * status based on success/failure above.
         */
        if (ProcMgr_close(&handle) < 0) {
            Osal_printf("ProcMgr_close() failed\n");
        }
    }

    return (status);
}

/*!
 *  @brief  Function to execute the startup for SlaveLoader sample application
 */
Int
SlaveLoader_startup (UInt16 procId, String action, String filePath,
        String mapFile)
{
   return SlaveLoader_startupArgs(procId, action, filePath, mapFile, 0, NULL);
}


/*!
 *  @brief  Function to execute the shutdown for SlaveLoader sample application
 */
Int
SlaveLoader_shutdown(UInt16 procId, String action, String mapFile)
{
    Int             status = 0;
    ProcMgr_Handle  handle = NULL;
    UInt32          fileId;

    status = ProcMgr_open(&handle, procId);

    if (status >= 0) {
        if ((strcmp(action, "shutdown") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "stop") == 0)) {
            status = ProcMgr_stop(handle);
            Osal_printf("Stopped slave procId %d.\n", procId);
        }

        if ((strcmp(action, "shutdown") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "unload") == 0)) {
            fileId = ProcMgr_getLoadedFileId(handle);
            status = ProcMgr_unload(handle, fileId) ;
            Osal_printf("Unloaded slave procId %d.\n", procId);
        }

        if ((mapFile != NULL) &&
                ((strcmp(action, "shutdown") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "unload") == 0))) {
            /* Parse map file and ProcMgr_unmap all entries */
#if DUMPMAPINFO
            Osal_printf("------before-------\n");
            dumpMapInfo(handle);
            Osal_printf("-------------------\n");
#endif

            status = SlaveLoader_mapByFile(handle, mapFile, procId, FALSE);
            if (status < 0) {
                Osal_printf ("SlaveLoader_mapByFile failed [0x%x]\n", status);
            }
            else {
                Osal_printf ("Mapped entries in %s to slave procId %d.\n",
                        mapFile, procId);
#if DUMPMAPINFO
                Osal_printf("------after-------\n");
                dumpMapInfo(handle);
                Osal_printf("-------------------\n");
#endif
            }
        }

        if ((strcmp(action, "shutdown") == 0) ||
                (strcmp(action, "all") == 0) ||
                (strcmp(action, "powerdown") == 0)) {
            status = ProcMgr_detach(handle);
            Osal_printf("Detached from slave procId %d.\n", procId);
        }

        /*
         * Since ProcMgr_open() succeeded, unconditionally ProcMgr_close()
         * Note that we don't check the return value here, and instead return
         * status based on success/failure above.
         */
        if (ProcMgr_close(&handle) < 0) {
            Osal_printf("ProcMgr_close() failed\n");
        }
    }

    return 0;
}


/*!
 *  @brief  Function to print the current status of all slave cores.
 */
Void
SlaveLoader_printStatus(Bool map)
{
    Int                 status = 0;
    ProcMgr_Handle      handle = NULL;
    UInt16              numProcs;
    int                 i;
    ProcMgr_State       state;
    char                strState[32];

    Osal_printf ("Current status of slave cores:\n");
    numProcs = MultiProc_getNumProcessors();
    for (i = 0 ; (i < numProcs) && (i != MultiProc_self()); i++) {
        if (SysLinkSamples_isAvailableProcId (i)) {
            status = ProcMgr_open(&handle, i);
            if (status >= 0) {
                state = ProcMgr_getState(handle);
                switch (state) {
                    case ProcMgr_State_Unknown:
                        strncpy (strState, "Unknown", 32u);
                        break;
                    case ProcMgr_State_Powered:
                        strncpy (strState, "Powered", 32u);
                        break;
                    case ProcMgr_State_Reset:
                        strncpy (strState, "Reset", 32u);
                        break;
                    case ProcMgr_State_Loaded:
                        strncpy (strState, "Loaded", 32u);
                        break;
                    case ProcMgr_State_Running:
                        strncpy (strState, "Running", 32u);
                        break;
                    case ProcMgr_State_Unavailable:
                        strncpy (strState, "Unavailable", 32u);
                        break;
                    case ProcMgr_State_EndValue:
                        /* Not a valid value. */
                        break;
                }
                Osal_printf("Slave core %d: %8s [%s]\n", i,
                        MultiProc_getName(i), strState);

                /* dump map info */
                if (map) {
                    dumpMapInfo(handle);
                }

                status = ProcMgr_close(&handle);
            }
        }
    }
}

static void dumpMapInfo(ProcMgr_Handle handle)
{
    Int status;
    char strBootMode[32];
    ProcMgr_ProcInfo *procInfo;
    UInt32 maxMemoryRegions;
    Int procInfoSize;
    int j;

    maxMemoryRegions = ProcMgr_getMaxMemoryRegions(handle);
    procInfoSize = sizeof(ProcMgr_ProcInfo) +
                   (maxMemoryRegions * sizeof(ProcMgr_MappedMemEntry));
    procInfo = Memory_alloc(NULL, procInfoSize, 0, NULL);

    status = ProcMgr_getProcInfo(handle, procInfo);
    if (status >= 0) {
        switch (procInfo->bootMode) {
            case ProcMgr_BootMode_Boot:
                strncpy (strBootMode, "Boot", 32u);
                break;
            case ProcMgr_BootMode_NoLoad_Pwr:
                strncpy (strBootMode, "NoLoad_Pwr", 32u);
                break;
            case ProcMgr_BootMode_NoLoad_NoPwr:
                strncpy (strBootMode, "NoLoad_NoPwr", 32u);
                break;
            case ProcMgr_BootMode_NoBoot:
                strncpy (strBootMode, "NoBoot", 32u);
                break;
            default:
                strncpy (strBootMode, "Undefined", 32u);
                break;
        }

        Osal_printf("  bootMode - %s\n", strBootMode);
        Osal_printf("  numMemEntries - %d\n", procInfo->numMemEntries);

        for (j = 0; j < procInfo->maxMemoryRegions; j++) {
            if (procInfo->memEntries[j].inUse) {
                Osal_printf("  memEntry %d - srcAddrType [0x%x], "
                        "mask [0x%x]\n    addrs:\n"
                        "      MKV: 0x%x, MUV: 0x%x, MP: 0x%x\n"
                        "      SV: 0x%x, SP: 0x%x\n"
                        "    size[0x%x], cached [%d], mapped [%d], "
                        "mask [0x%x], refs [0x%x]\n", j,
                        procInfo->memEntries[j].srcAddrType,
                        procInfo->memEntries[j].mapMask,
                        procInfo->memEntries[j].info.
                            addr[ProcMgr_AddrType_MasterKnlVirt],
                        procInfo->memEntries[j].info.
                            addr[ProcMgr_AddrType_MasterUsrVirt],
                        procInfo->memEntries[j].info.
                            addr[ProcMgr_AddrType_MasterPhys],
                        procInfo->memEntries[j].info.
                            addr[ProcMgr_AddrType_SlaveVirt],
                        procInfo->memEntries[j].info.
                            addr[ProcMgr_AddrType_SlavePhys],
                        procInfo->memEntries[j].info.size,
                        procInfo->memEntries[j].info.isCached,
                        procInfo->memEntries[j].info.isMapped,
                        procInfo->memEntries[j].info.mapMask,
                        procInfo->memEntries[j].info.refCount);
            }
        }
    }
    Memory_free(NULL, procInfo, procInfoSize);

    return;
}


#define MAXLINE 100

Int SlaveLoader_mapByFile(ProcMgr_Handle handle, String mapFile, UInt16 procId,
        int mapFlag)
{
    FILE *f;
    char line[MAXLINE];
    MapStruct mapStruct;
    ProcMgr_AddrInfo addrInfo;
    Int status = 0;
    ProcMgr_ProcInfo *procInfo;
    UInt32 maxMemoryRegions;
    Int procInfoSize;
    int j;

    f = fopen(mapFile, "r");

    if (f) {
        while (fgets(line, MAXLINE, f)) {
            if (isMapLine(line, procId)) {
                /* applicable, parse it */
                if (initFromLine(line, &mapStruct)) {
                    if (mapFlag) {
                        /* map! */
                        Osal_printf("Mapping SV: 0x%x, SP: 0x%x, MP: 0x%x,\n"
                                "  size 0x%x, mask 0x%x, cached %d\n",
                                mapStruct.slaveVirt, mapStruct.slavePhys,
                                mapStruct.masterPhys, mapStruct.size,
                                mapStruct.mapMask, mapStruct.isCached);

                        addrInfo.addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;
                        addrInfo.addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                        addrInfo.addr[ProcMgr_AddrType_MasterPhys] =
                            mapStruct.masterPhys;
                        addrInfo.addr[ProcMgr_AddrType_SlaveVirt] =
                            mapStruct.slaveVirt;  /* Is this output?!? */
                        addrInfo.addr[ProcMgr_AddrType_SlavePhys] = -1u;
                        addrInfo.size = mapStruct.size;
                        addrInfo.isCached = mapStruct.isCached;

                        status = ProcMgr_map(handle, mapStruct.mapMask,
                                &addrInfo, ProcMgr_AddrType_MasterPhys);
                        if (status < 0) {
                            Osal_printf("ProcMgr_map failed [0x%x]\n", status);
                        }
                        else {
                            Osal_printf("ProcMgr_map succeeded\n");
                        }
                    }
                    else {
                        /* unmap! */
                        Osal_printf("Unmapping SV: 0x%x, SP: 0x%x, MP: 0x%x,\n"
                                "  size 0x%x, mask 0x%x, cached %d\n",
                                mapStruct.slaveVirt, mapStruct.slavePhys,
                                mapStruct.masterPhys, mapStruct.size,
                                mapStruct.mapMask, mapStruct.isCached);

                        addrInfo.addr[ProcMgr_AddrType_MasterKnlVirt] = -1u;

                        /* if unmapping MasterKnlVirt, need to look it up */
                        if (mapStruct.mapMask & ProcMgr_MASTERKNLVIRT) {

                            maxMemoryRegions =
                                ProcMgr_getMaxMemoryRegions(handle);
                            procInfoSize = sizeof(ProcMgr_ProcInfo) +
                                           (maxMemoryRegions *
                                            sizeof(ProcMgr_MappedMemEntry));
                            procInfo = Memory_alloc(NULL, procInfoSize,
                                                    0, NULL);

                            status = ProcMgr_getProcInfo(handle, procInfo);
                            if (status < 0) {
                                Osal_printf("Unable to find MKV entry\n");
                            }
                            else {
                                for (j = 0; j < procInfo->maxMemoryRegions;
                                        j++) {
                                    if ((procInfo->memEntries[j].inUse) &&
                                            (procInfo->memEntries[j].info.
                                            addr[ProcMgr_AddrType_MasterPhys]
                                            == mapStruct.masterPhys) &&
                                            (procInfo->memEntries[j].info.size ==
                                            mapStruct.size) &&
                                            (procInfo->memEntries[j].info.isCached
                                            == mapStruct.isCached)) {
                                        /* found it */
                                        addrInfo.addr
                                            [ProcMgr_AddrType_MasterKnlVirt] =
                                            procInfo->memEntries[j].info.
                                            addr[ProcMgr_AddrType_MasterKnlVirt];
                                Osal_printf("Found MKV entry (0x%x)\n",
                                        addrInfo.addr
                                            [ProcMgr_AddrType_MasterKnlVirt]);
                                        break;
                                    }
                                }
                            }

                            Memory_free(NULL, procInfo, procInfoSize);
                        }

                        addrInfo.addr[ProcMgr_AddrType_MasterUsrVirt] = -1u;
                        addrInfo.addr[ProcMgr_AddrType_MasterPhys] =
                            mapStruct.masterPhys;
                        addrInfo.addr[ProcMgr_AddrType_SlaveVirt] =
                            mapStruct.slaveVirt;
                        addrInfo.addr[ProcMgr_AddrType_SlavePhys] = -1u;
                        addrInfo.size = mapStruct.size;
                        addrInfo.isCached = mapStruct.isCached;

                        status = ProcMgr_unmap(handle, mapStruct.mapMask,
                                &addrInfo, ProcMgr_AddrType_MasterPhys);
                        if (status < 0) {
                            Osal_printf("ProcMgr_unmap failed [0x%x]\n",
                                    status);
                        }
                        else {
                            Osal_printf("ProcMgr_unmap succeeded\n");
                        }
                    }
                }
                else {
                    Osal_printf("malformed line, skipping...\n   %s", line);
                }
            }
        }

        fclose(f);
    }
    else {
        Osal_printf("Unable to open %s... skipping...\n");
    }

    /* regardless, we return 'success'.  Malformed lines are skipped */
    return (TRUE);
}

int isMapLine(char *line, UInt16 procId)
{
    int  retval = FALSE;
    char procName[32];

    if (line[0] != '#') {
        strncpy(procName, MultiProc_getName(procId), 32);
        if (strncmp(procName, line, strlen(procName)) == 0) {
            retval = TRUE;
        }
    }

    return (retval);
}

int initFromLine(char *line, MapStruct *mapStruct)
{
    int retval = FALSE;
    char *p;

    p = strtok(line, " ,");
    if (p) {
        /* procName */
        p = strtok(NULL, " ,");
        if (p) {
            /* slaveVirt, in hex */
            mapStruct->slaveVirt = strtoul(p, NULL, 16);
            if (mapStruct->slaveVirt == 0) {
                /* error, exit */
                Osal_printf("Error parsing slaveVirt\n");
                p = NULL;
            }
            else {
                p = strtok(NULL, " ,");
            }
        }
        if (p) {
            /* slavePhys, in hex */
            mapStruct->slavePhys = strtoul(p, NULL, 16);
            if (mapStruct->slavePhys == 0) {
                /* error, exit */
                Osal_printf("Error parsing slavePhys\n");
                p = NULL;
            }
            else {
                p = strtok(NULL, " ,");
            }
        }
        if (p) {
            /* masterPhys, in hex */
            mapStruct->masterPhys = strtoul(p, NULL, 16);
            if (mapStruct->masterPhys == 0) {
                /* error, exit */
                Osal_printf("Error parsing masterPhys\n");
                p = NULL;
            }
            else {
                p = strtok(NULL, " ,");
            }
        }
        if (p) {
            /* size, in hex */
            mapStruct->size = strtoul(p, NULL, 16);
            if (mapStruct->size == 0) {
                /* error, exit */
                Osal_printf("Error parsing size\n");
                p = NULL;
            }
            else {
                p = strtok(NULL, " ,");
            }
        }
        if (p) {
            /* Mask */
            mapStruct->mapMask = strtoul(p, NULL, 16);
            if (mapStruct->mapMask == 0) {
                /* error, exit */
                Osal_printf("Error parsing mask\n");
                p = NULL;
            }
            else {
                p = strtok(NULL, " ,");
            }
        }
        if (p) {
            /* cached */
            errno = 0;
            mapStruct->isCached = strtoul(p, NULL, 16);
            if ((errno != 0) ||
                    ((mapStruct->isCached != 0) && mapStruct->isCached != 1)) {
                /* error, exit */
                Osal_printf("Error parsing cached\n");
                p = NULL;
            }
            else {
                /* that's it! */
                retval = TRUE;
            }
        }
    }

    return (retval);
}
