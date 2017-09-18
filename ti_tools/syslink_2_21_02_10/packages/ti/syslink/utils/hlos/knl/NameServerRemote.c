/*
 *  @file   NameServerRemote.c
 *
 *  @brief      <add single line description>.
 *
 *              <add detailed description (optional)>
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

/* Osal & Utility headers */
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/NameServerRemote.h>
#include <ti/syslink/utils/Trace.h>


/* =============================================================================
 * Struct & Enums
 * =============================================================================
 */
/* Structure defining object for the NameServer remote driver */
struct NameServerRemote_Object {
    NameServerRemote_GetFxn get;
    /* Function to get data from remote nameserver */
    Ptr                     obj;
    /* Implementation specific object */
};

/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief      Function to get data from remote name server.
 *
 *  @param      obj             Remote driver object.
 *  @param      instanceName    Nameserver instance name.
 *  @param      name            Name of the entry.
 *  @param      value           Pointer to the value.
 *  @param      valueLen        Length oif value.
 *  @param      reserved        Reserved value. Can be passed as NULL.
 */
Int
NameServerRemote_get (NameServerRemote_Handle   handle,
                      String                    instanceName,
                      String                    name,
                      Ptr                       value,
                      UInt32 *                  valueLen,
                      Ptr                       reserved)
{
    Int status = NameServer_S_SUCCESS;

    GT_5trace (curTrace,
               GT_ENTER,
               "NameServerRemote_get",
               handle,
               instanceName,
               name,
               value,
               valueLen);

    GT_assert (curTrace, (handle       != NULL));
    GT_assert (curTrace, (instanceName != NULL));
    GT_assert (curTrace, (name         != NULL));
    GT_assert (curTrace, (value        != NULL));
    GT_assert (curTrace, (valueLen     != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemote_get",
                             status,
                             "handle is null!");
    }
    else if (instanceName == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemote_get",
                             status,
                             "instanceName is null!");
    }
    else if (name == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemote_get",
                             status,
                             "name is null!");
    }
    else if (value == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemote_get",
                             status,
                             "value is null!");
    }
    else if (valueLen == NULL) {
        status = NameServer_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerRemote_get",
                             status,
                             "valueLen is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = handle->get (handle,
                              instanceName,
                              name,
                              value,
                              valueLen,
                              NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NameServerRemote_get", status);

    /*! @retval NameServer_S_SUCCESS Operation successfully completed */
    return status;
}
