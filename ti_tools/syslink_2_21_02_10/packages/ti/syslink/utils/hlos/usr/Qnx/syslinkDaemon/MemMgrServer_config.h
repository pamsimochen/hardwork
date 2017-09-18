/** 
 *  @file   MemMgrServer_config.h
 *
 *  @brief  MemMgr Server configuration file
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


#ifndef _MEMMGRSERVER_CONFIG_H_
#define _MEMMGRSERVER_CONFIG_H_

#if defined (__cplusplus)
extern "C" {
#endif

/* App defines */
#define NSRN_NOTIFY_EVENTNO         7
#define TRANSPORT_NOTIFY_EVENTNO    8
#define NSRN_NOTIFY_EVENTNO1        22
#define TRANSPORT_NOTIFY_EVENTNO1   23

#define RCMSERVER_NAME              "MEMALLOCMGRSERVER"
#define SYSM3_PROC_NAME             "SysM3"
#define APPM3_PROC_NAME             "AppM3"

//#define SYSM3_FILE_NAME             "./RCMClient_MPUSYS_Test_Core0.xem3"
#define SYSM3_FILE_NAME             "./RCMServer_MPUSYS_Test_Core0.xem3"
//#define SYSM3_FILE_NAME             "./Notify_MPUSYS_reroute_Test_Core0.xem3"
//#define APPM3_FILE_NAME             "./RCMClient_MPUAPP_Test_Core1.xem3"
#define APPM3_FILE_NAME             "./RCMServer_MPUAPP_Test_Core1.xem3"

/*
 *  The shared memory is going to split between
 *
 *  MPU - SysM3 sample
 *  Notify:       0xA0000000 - 0xA0003FFF
 *  GatePeterson: 0xA0004000 - 0xA0004FFF
 *  HeapBuf:      0xA0005000 - 0xA0008FFF
 *  MessageQ NS:  0xA0009000 - 0xA0009FFF
 *  Transport:    0xA000A000 - 0xA000BFFF
 *
 *  MPU - AppM3 sample
 *  Notify:       0xA0000000 - 0xA0003FFF
 *  GatePeterson: 0xA0055000 - 0xA0055FFF
 *  HeapBuf:      0xA0056000 - 0xA0059FFF
 *  MessageQ NS:  0xA005A000 - 0xA005AFFF
 *  Transport:    0xA005B000 - 0xA005CFFF
 */

/* Shared Memory Area for MPU - SysM3 */
#define SHAREDMEM                   0xA0000000
#define SHAREDMEMSIZE               0x55000
#define SHAREDMEMNUMBER             0

/* Shared Memory Area for MPU - AppM3 */
#define SHAREDMEM1                  0xA0055000
#define SHAREDMEMSIZE1              0x55000
#define SHAREDMEMNUMBER1            1

/* Memory for the Notify Module */
#define NOTIFYMEM                   SHAREDMEM
#define NOTIFYMEMSIZE               0x4000

/* Memory a GatePeterson instance */
//#define GATEPETERSONMEM             (NOTIFYMEM + NOTIFYMEMSIZE)
#define GATEPETERSONMEM             (NOTIFYMEM + NOTIFYMEMSIZE + 0x40000)
#define GATEPETERSONMEMSIZE         0x1000

/* Memory a HeapBuf instance */
#define HEAPBUFMEM                  (GATEPETERSONMEM + GATEPETERSONMEMSIZE)
#define HEAPBUFMEMSIZE              0x4000

/* Memory for NameServerRemoteNotify */
#define NSRN_MEM                    (HEAPBUFMEM + HEAPBUFMEMSIZE)
#define NSRN_MEMSIZE                0x1000

/* Memory a Transport instance */
#define TRANSPORTMEM                (NSRN_MEM + NSRN_MEMSIZE)
#define TRANSPORTMEMSIZE            0x2000

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* _MEMMGRSERVER_CONFIG_H_ */
