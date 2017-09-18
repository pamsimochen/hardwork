/*
 *  @file   SlaveLoaderOS.c
 *
 *  @brief      OS-specific slave loader that uses the ProcMgr module
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


/* OS-specific headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/SysLink.h>
#include <ti/ipc/MultiProc.h>

/* Sample app headers */
#include <ti/syslink/samples/hlos/slaveLoader/SlaveLoader.h>
#include <ti/syslink/samples/hlos/common/SysLinkSamples.h>


/** ============================================================================
 *  Function declarations
 *  ============================================================================
 */
static Void SlaveLoader_printUsageInfo (Void);


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
int
main (int argc, char ** argv)
{
    Int     status       = 0;
    Bool    startupProc  = FALSE;
    Bool    shutdownProc = FALSE;
    UInt16  procId       = MultiProc_INVALIDID;
    String  mapFile      = NULL;
    Int     remoteArgc   = 0;
    String  *remoteArgv  = NULL;
    Int     i;

    SysLink_setup ();

    /* Execute common startup functionality for all sample applications */
    SysLinkSamples_startup ();

    if ((argc == 2) && (strcmp (argv [1], "--help") == 0)) {
        status = -1;
        SlaveLoader_printUsageInfo ();
    }
    else if (argc < 2) {
        status = -1;
        SlaveLoader_printUsageInfo ();
    }
    else {
        if ((argc >= 3) && (strcmp(argv[1], "list") == 0)
            && (strcmp(argv[2], "map") == 0)) {

            SlaveLoader_printStatus(TRUE);
            status = -1; /* No need to do anything else. */

        } else if ((argc >= 2) && (strcmp(argv[1], "list") == 0)) {
            SlaveLoader_printStatus(FALSE);
            status = -1; /* No need to do anything else. */
        }
        else if ((strcmp (argv [1], "all") == 0)) {
            if (argc < 4) {
                status = -1;
                Osal_printf("[File path] and <Core name> need to be provided"
                            " for 'all'!\n");
                SlaveLoader_printUsageInfo();
            }
            /* Startup and shutdown slave core. */
            startupProc  = TRUE;
            shutdownProc = TRUE;
        }
        else if ((strcmp(argv[1], "startup") == 0) ||
                (strcmp(argv[1], "load") == 0)) {
            if (argc < 4) {
                status = -1;
                Osal_printf("[File path] and <Core name> need to be provided"
                            " for 'startup' and 'load'!\n");
                SlaveLoader_printUsageInfo();
            }
            /* Only startup slave core. */
            startupProc = TRUE;
        }
        else if ((strcmp(argv[1], "shutdown") == 0) ||
                (strcmp(argv[1], "stop") == 0) ||
                (strcmp(argv[1], "unload") == 0) ||
                (strcmp(argv[1], "powerdown") == 0)) {
            if (argc < 3) {
                status = -1;
                Osal_printf("<Core name> needs to be provided for"
                            " 'shutdown', 'powerup', 'start', 'stop', 'unload'"
                            " and 'powerdown'!\n");
                SlaveLoader_printUsageInfo ();
            }
            else if (argc > 4) {
                Osal_printf("Ignoring extra passed arguments!\n");
                SlaveLoader_printUsageInfo ();
            }

            /* Only shutdown slave core. */
            shutdownProc = TRUE;
        }
        else if (   (strcmp (argv [1], "powerup")  == 0)
                 || (strcmp (argv [1], "start")    == 0)) {
            if (argc < 3) {
                status = -1;
                Osal_printf("<Core name> needs to be provided for"
                            " 'shutdown', 'powerup', 'start', 'stop', 'unload'"
                            " and 'powerdown'!\n");
                SlaveLoader_printUsageInfo ();
            }
            else if (argc > 3) {
                Osal_printf("Ignoring extra passed arguments!\n");
                SlaveLoader_printUsageInfo ();
            }
            /* Only startup slave core. */
            startupProc = TRUE;
        }
        else {
            status = -1;
            SlaveLoader_printUsageInfo ();
        }

        /* Validate passed core name and get its ID. */
        if (status >= 0) {
            procId = MultiProc_getId (argv [2]);
            if (procId == MultiProc_INVALIDID) {
                status = -1;
                Osal_printf("Invalid <Core name> specified!\n");
                SlaveLoader_printUsageInfo ();
            }
        }
    }

    if (status >= 0) {
        if (startupProc == TRUE) {
            if (argc > 4 && strcmp(argv[4], "--args") != 0) {
                mapFile = argv[4];
                for (i = 5; i < argc; i++) {
                    argv[i - 1] = argv[i];
                }
                argc--;
            }
            if (argc > 4) {
                if (strcmp(argv[4], "--args") == 0) {
                    for (i = 5; i <= argc; i++) {
                        argv[i - 1] = argv[i];
                    }
                    argc--;
                    remoteArgc = argc - 3;
                    remoteArgv = &argv[3];
                }
            }

            status = SlaveLoader_startupArgs (procId, argv[1], argv[3], mapFile,
                                          remoteArgc, remoteArgv);
        }
    }

    /* Add getchar only if both starting up and shutting down. */
    if ((startupProc == TRUE) && (shutdownProc == TRUE)) {
        Osal_printf("Press enter to continue and perform shutdown ...\n");
        getchar ();
    }

    if ((procId != MultiProc_INVALIDID) && (shutdownProc == TRUE)) {
        /*
         * if startupProc == TRUE, mapFile may have already been set above
         * otherwise it's NULL
         */
        if (startupProc != TRUE) {
            if (argc >= 4) {
                mapFile = argv[3];
            }
        }

        status = SlaveLoader_shutdown(procId, argv [1], mapFile);
    }

    /* Execute common shutdown functionality for all sample applications */
    SysLinkSamples_shutdown();

    SysLink_destroy();

    return 0;
}


static Void SlaveLoader_printUsageInfo (Void)
{
    UInt16 numProcs;
    UInt16 i;

    Osal_printf ("Usage:\n");
    Osal_printf ("slaveloader --help\n");
    Osal_printf ("slaveloader list [map]\n");
    Osal_printf (
        "slaveloader <startup|load|all> <core name> <file path> [map file] [--args <core args>]\n");
    Osal_printf ("slaveloader <shutdown|unload> <core name> [map file]\n");
    Osal_printf ("slaveloader <powerup|powerdown|start|stop> <core name>\n");
    Osal_printf ("\n");
    Osal_printf ("Supported core names:\n");

    numProcs = MultiProc_getNumProcessors();
    for (i = 0; (i < numProcs) && (i != MultiProc_self()); i++) {
        if (SysLinkSamples_isAvailableProcId(i)) {
            Osal_printf ("- %s\n", MultiProc_getName(i));
        }
    }

    Osal_printf ("\n");
    Osal_printf ("Note: [File path] argument is only required when first"
                 " argument is 'startup', 'load' or 'all'\n");
    Osal_printf ("Note: 'list' prints the state of all slave cores\n");
    Osal_printf ("Note: 'list map' also prints the memory map tables\n");
}
