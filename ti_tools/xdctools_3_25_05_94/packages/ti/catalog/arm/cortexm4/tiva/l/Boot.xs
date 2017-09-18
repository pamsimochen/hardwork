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
 *  ======== Boot.xs ========
 */

var Boot = null;
var Program = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
    Boot = this;

    /* Assign setters to the Clock configs. */
    var GetSet = xdc.module("xdc.services.getset.GetSet");

    GetSet.init(Boot);

    GetSet.onSet(this, "configureClocks", updateFrequency);
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Program = xdc.useModule('xdc.cfg.Program');

    /* only install Boot_init if using XDC runtime */
    if (Program.build.rtsName === null) {
        return;
    }

    /*
     * Install Boot_init as a FirstFxn
     * It would be faster to install it as a Reset functions
     * but Asserts don't work until AFTER cinit
     */

    var Startup = xdc.useModule('xdc.runtime.Startup');
    Startup.firstFxns.$add(Boot.init);
}

/*
 *  ======== viewInitModule ========
 *  Display the module properties in ROV
 */
function viewInitModule(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Boot = xdc.useModule('ti.catalog.arm.cortexm4.tiva.l.Boot');
    var modCfg = Program.getModuleConfig(Boot.$name);

    view.configureClocks        = modCfg.configureClocks
    view.disableWatchdog        = modCfg.disableWatchdog;
}

/*
 *  ======== getFrequency ========
 */
function getFrequency()
{
    if (Boot.configureClocks == false) {
        /* no way to know */
        return (0);
    }

    return (0);
}


/* Array of listeners, used by 'registerFreqListener' and 'updateFrequency'. */
var listeners = new Array();

/*
 *  ======== registerFreqListener ========
 *  Called by other modules (e.g., BIOS), to register themselves to listen
 *  for changes to the device frequency configured by the Boot module.
 */
function registerFreqListener(listener)
{
    listeners[listeners.length] = listener;
}

/*
 *  ======== updateFrequency ========
 *  Update all of the listeners whenever the clock configuration changes
 *  (assuming configureClocks is true).
 */
function updateFrequency(field, val)
{
    /* Don't fire the update if configureClocks is false. */
    if (!Boot.configureClocks) {
        return;
    }

    Boot.computedCpuFrequency = getFrequency();;

    /* Notify each of the listeners of the new frequency value. */
    for each (var listener in listeners) {
        listener.fireFrequencyUpdate(Boot.computedCpuFrequency);
    }
}
