/** 
 *  @file   SysLinkSamples.h
 *
 *  @brief      Interface for common features required by all SysLink samples
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


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/* Maximum number of processors supported by samples */
#define SysLinkSamples_MAXPROCESSORS 10

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to execute the startup for common SysLink sample
 *          application features
 */
Void SysLinkSamples_startup (Void);


/*!
 *  @brief  Function to execute the shutdown for common SysLink sample
 *          application features
 */
Void SysLinkSamples_shutdown (Void);


/*!
 *  @brief  Function to check if specified procId is available
 */
Bool SysLinkSamples_isAvailableProcId (UInt32 procId);


/*!
 *  @brief  Function to set the specified procIds to be run in the sample app.
 *          If runAll is TRUE, then all are run as per the platform
 *          specification
 */
Int SysLinkSamples_setToRunProcIds (UInt16 numProcs,
                                    UInt16 procIds [],
                                    Bool   runAll);


/*!
 *  @brief  Function to check if specified procId is to be run in the sample app
 */
Bool SysLinkSamples_toRunProcId (UInt32 procId);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
