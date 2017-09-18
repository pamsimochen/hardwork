/*
 *  @file   Dev.c
 *
 *  @brief      Device Helper Utilities
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
#include <ti/syslink/utils/Trace.h>

/* -------------------------- Generic includes ------------------------------ */
#include <errno.h>
#include <fcntl.h>
#include <time.h>

/* this module's header file */
#include <ti/syslink/utils/Dev.h>


#define LOOPCOUNT 100

/* =============================================================================
 * Struct & Enums
 * =============================================================================
 */


/* =============================================================================
 * Globals
 * =============================================================================
 */


/* =============================================================================
 * APIs
 * =============================================================================
 */

Int32 Dev_pollOpen(String deviceFilename, Int flags)
{
    int handle;
    int loopCount = 0;
    int sleepRet = 0;
    struct timespec time;

    GT_2trace(curTrace, GT_ENTER, "Dev_pollOpen", deviceFilename, flags);

    time.tv_sec = 0;
    time.tv_nsec = 1000000;    /* 1000000 nanoseconds = 1 millisecond */
    do {
        handle = open(deviceFilename, flags);
        if (handle < 0) {
            sleepRet = nanosleep(&time, NULL);
        }
        loopCount++;
    } while (handle < 0 &&
             (errno == ENODEV || errno == ENOENT || errno == ENXIO) &&
             loopCount < LOOPCOUNT &&
             sleepRet == 0);

    if (handle < 0) {
        /* GT Trace is not started yet, use stderr instead (perror uses it) */
        fprintf(stderr, "Dev_pollOpen: Couldn't open ");
        perror(deviceFilename);
        fprintf(stderr, "    Please ensure that SysLink's device driver module (syslink.ko) is installed\n");
    }

    GT_1trace(curTrace, GT_LEAVE, "Dev_pollOpen", handle);

    return handle;
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */


