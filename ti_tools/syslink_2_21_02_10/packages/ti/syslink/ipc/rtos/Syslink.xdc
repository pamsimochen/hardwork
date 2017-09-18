/** 
 *  @file   Syslink.xdc
 *
 *  @brief      Logic to plug RingIO & FrameQ specific code in Ipc.
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


/*
 *  ======== Syslink.xdc ========
 */
package ti.syslink.ipc.rtos

/*!
 *  ======== Syslink ========
 */
@Template("./Syslink.xdt")

metaonly module Syslink {

    /*!
     *  Notify event id to be used for the terminate event.
     *
     *  When a SysLink application is terminated, the SysLink driver will
     *  send a notify terminate event to all attached slaves to inform them
     *  that the application has terminated. The event id used for this
     *  terminate event is specified by this config param.
     *
     *  The payload of the terminate event is the runtime id of the
     *  terminated application.
     *
     */
    config UInt32 terminateEventId = 30;

    /*!
     *  Notify line id to be used for the terminate event.
     *
     *  The SysLink driver uses Notify_sendEvent() to send the terminate
     *  event. This config param specifies which lineId to use when sending
     *  this event.
     */
    config UInt16 terminateEventLineId = 0;

    /*!
     *  Work around for linker bug regarding placement of .plt section.
     *
     *  Set this config param to false to disable the work around fix.
     */
    config Bool pltSectFix = true;
}
