/*
 *  @file   SyslinkTraceDaemon.c
 *
 *  @brief  Daemon for Syslink trace
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <ti/syslink/Std.h>
#include <std_qnx.h>

/* OSAL & Utils headers */
#include <OsalPrint.h>
#include <UsrUtilsDrv.h>
#include <Memory.h>

#include <sys/procmgr.h>
#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* Use the Ducati HEAP3 for trace buffer
 * SYSM3 VA address would be 0x81FE0000, APPM3 is 0x81FF0000
 */
#define SYSM3_TRACE_BUFFER_PHYS_ADDR    0x9AFE0000
#define APPM3_TRACE_BUFFER_PHYS_ADDR    0X9AFF0000
//#define TESLA_TRACE_BUFFER_PHYS_ADDR    0x9CEF0000

#define TRACE_BUFFER_SIZE               0x10000

#define TIMEOUT_SECS                    1

sem_t semPrint;    /* Semaphore to allow only one thread to print at once */

/* pull char from queue */
Void printSysM3Traces (Void *arg)
{
    Int               status              = 0;
    Memory_MapInfo    traceinfo;
    UInt32            numOfBytesInBuffer  = 0;
    volatile UInt32 * readPointer;
    volatile UInt32 * writePointer;
    Char            * traceBuffer;

    Osal_printf ("\nSpawning SysM3 trace thread\n ");

    /* Get the user virtual address of the buffer */
    traceinfo.src  = SYSM3_TRACE_BUFFER_PHYS_ADDR;
    traceinfo.size = TRACE_BUFFER_SIZE;
    status = Memory_map (&traceinfo);
    readPointer = (volatile UInt32 *)traceinfo.dst;
    writePointer = (volatile UInt32 *)(traceinfo.dst + 0x4);
    traceBuffer = (Char *)(traceinfo.dst + 0x8);

    /* Initialze read indexes to zero */
    *readPointer = 0;
    *writePointer = 0;
    do {
        do {
           sleep (TIMEOUT_SECS);
        } while (*readPointer == *writePointer);

        sem_wait(&semPrint);    /* Acquire exclusive access to printing */
        if ( *readPointer < *writePointer ) {
            numOfBytesInBuffer = (*writePointer) - (*readPointer);
        } else {
            numOfBytesInBuffer = ((TRACE_BUFFER_SIZE - 8) - (*readPointer)) + (*writePointer);
        }

        Osal_printf ("\n[SYSM3]: ");
        while ( numOfBytesInBuffer-- ) {
            if ((*readPointer) == (TRACE_BUFFER_SIZE - 8)){
                (*readPointer) = 0;
            }

            Osal_printf ("%c", traceBuffer[*readPointer]);
            if (traceBuffer[*readPointer] == '\n') {
                Osal_printf ("[SYSM3]: ");
            }

            (*readPointer)++;
        }
        sem_post(&semPrint);    /* Release exclusive access to printing */

    } while(1);

    Osal_printf ("Leaving printSysM3Traces thread function \n");
    return;
}


/* pull char from queue */
Void printAppM3Traces (Void *arg)
{
    Int               status              = 0;
    Memory_MapInfo    traceinfo;
    UInt32            numOfBytesInBuffer  = 0;
    volatile UInt32 * readPointer;
    volatile UInt32 * writePointer;
    Char            * traceBuffer;

    Osal_printf ("\nSpawning AppM3 trace thread\n ");

    /* Get the user virtual address of the buffer */
    traceinfo.src  = APPM3_TRACE_BUFFER_PHYS_ADDR;
    traceinfo.size = TRACE_BUFFER_SIZE;
    status = Memory_map (&traceinfo);
    readPointer = (volatile UInt32 *)traceinfo.dst;
    writePointer = (volatile UInt32 *)(traceinfo.dst + 0x4);
    traceBuffer = (Char *)(traceinfo.dst + 0x8);

    /* Initialze read and write indexes to zero */
    *readPointer = 0;
    *writePointer = 0;
    do {
        do {
           sleep (TIMEOUT_SECS);
        } while (*readPointer == *writePointer);

        sem_wait(&semPrint);    /* Acquire exclusive access to printing */
        if ( *readPointer < *writePointer ) {
            numOfBytesInBuffer = *writePointer - *readPointer;
        } else {
            numOfBytesInBuffer = ((TRACE_BUFFER_SIZE - 8) - *readPointer) + *writePointer;
        }

        Osal_printf ("\n[APPM3]: ");
        while ( numOfBytesInBuffer-- ) {
            if (*readPointer >= (TRACE_BUFFER_SIZE - 8)){
                *readPointer = 0;
            }

            Osal_printf ("%c", traceBuffer[*readPointer]);
            if (traceBuffer[*readPointer] == '\n') {
                Osal_printf ("[APPM3]: ");
            }

            (*readPointer)++;
        }
        sem_post(&semPrint);    /* Release exclusive access to printing */

    } while(1);

    Osal_printf ("Leaving printAppM3Traces thread function \n");
    return;
}

#if 0
/* pull char from queue */
Void printTeslaTraces (Void *arg)
{
    Int               status              = 0;
    Memory_MapInfo    traceinfo;
    UInt32            numOfBytesInBuffer  = 0;
    volatile UInt32 * readPointer;
    volatile UInt32 * writePointer;
    Char            * traceBuffer;

    Osal_printf ("\nSpawning Tesla trace thread\n ");

    /* Get the user virtual address of the buffer */
    traceinfo.src  = TESLA_TRACE_BUFFER_PHYS_ADDR;
    traceinfo.size = TRACE_BUFFER_SIZE;
    status = Memory_map (&traceinfo);
    readPointer = (volatile UInt32 *)traceinfo.dst;
    writePointer = (volatile UInt32 *)(traceinfo.dst + 0x4);
    traceBuffer = (Char *)(traceinfo.dst + 0x8);

    /* Initialze read indexes to zero */
    *readPointer = 0;
    *writePointer = 0;
    do {
        do {
           sleep (TIMEOUT_SECS);
        } while (*readPointer == *writePointer);

        sem_wait(&semPrint);    /* Acquire exclusive access to printing */
        if ( *readPointer < *writePointer ) {
            numOfBytesInBuffer = (*writePointer) - (*readPointer);
        } else {
            numOfBytesInBuffer = ((TRACE_BUFFER_SIZE - 8) - (*readPointer)) + (*writePointer);
        }

        Osal_printf ("\n[DSP]: ");
        while ( numOfBytesInBuffer-- ) {
            if ((*readPointer) == (TRACE_BUFFER_SIZE - 8)){
                (*readPointer) = 0;
            }

            Osal_printf ("%c", traceBuffer[*readPointer]);
            if (traceBuffer[*readPointer] == '\n') {
                Osal_printf ("[DSP]: ");
            }

            (*readPointer)++;
        }
        sem_post(&semPrint);    /* Release exclusive access to printing */

    } while(1);

    Osal_printf ("Leaving printTeslaTraces thread function \n");
    return;
}
#endif

Int main (Int argc, Char * argv [])
{
    pthread_t   thread_sys; /* server thread object */
    pthread_t   thread_app; /* server thread object */
    //pthread_t   thread_dsp; /* server thread object */

    Osal_printf ("Spawning Ducati-Tesla Trace daemon...\n");

    /* background the process */
    procmgr_daemon(0, PROCMGR_DAEMON_NOCLOSE|PROCMGR_DAEMON_NODEVNULL);

    sem_init(&semPrint, 0, 1);

    MemoryOS_setup ();

    pthread_create (&thread_sys, NULL, (Void *)&printSysM3Traces,
                    NULL);
    pthread_create (&thread_app, NULL, (Void *)&printAppM3Traces,
                    NULL);
#if 0
    pthread_create (&thread_dsp, NULL, (Void *)&printTeslaTraces,
                    NULL);
#endif

    pthread_join (thread_sys, NULL);
    Osal_printf ("SysM3 trace thread exited\n");
    pthread_join (thread_app, NULL);
    Osal_printf ("AppM3 trace thread exited\n");
#if 0
    pthread_join (thread_dsp, NULL);
    Osal_printf ("Tesla trace thread exited\n");
#endif
    MemoryOS_destroy ();

    sem_destroy(&semPrint);

    return 0;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
