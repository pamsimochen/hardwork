/* 
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

/*
 *  ======== Examples.xdc ========
 */
import xdc.tools.product.IProductTemplate;

/*
 *  ======== Examples ========
 *  NOTE: deviceId filtering is necessary to eliminate Generic and Custom
 *  device which are not currently supported for C2000 devices.
 *
 *  To find all "abstract" deviceId's in a CCS product you can use the
 *  following:
 *      cd <CCS_install_directory>/eclipse
 *      find . -name "*.xml" -exec grep com.ti.ccstudio.deviceModel {} \;
 *
 *  The output will look something like:
 *              id="com.ti.ccstudio.deviceModel.C2000.CustomC2000Device"
 *              id="com.ti.ccstudio.deviceModel.C2000.GenericC28xxDevice"
 *              id="com.ti.ccstudio.deviceModel.C2000.GenericC27xxDevice"
 *              id="com.ti.ccstudio.deviceModel.C5400.CustomC5400Device"
 *                  :
 */
metaonly module Examples inherits IProductTemplate 
{
    config IProductTemplate.TemplateGroup root = {
        id          : "ti.sdo.ipc.examples.root",
        name        : "IPC Examples",
        description : "IPC Examples"
    };

    config IProductTemplate.TemplateGroup generic = {
        id          : "ti.sdo.ipc.examples.root.generic",
        name        : "Generic IPC Examples",
        description : "Generic IPC Examples",
        groups      : ["ti.sdo.ipc.examples.root"]
    };

    override config IProductTemplate.TemplateGroup templateGroupArr [] =
        [root, generic];

}
/*
 *  @(#) ti.sdo.ipc.examples; 1,0,0,; 5-10-2013 12:35:13; /db/vtree/library/trees/ipc/ipc-i15/src/ xlibrary

 */

