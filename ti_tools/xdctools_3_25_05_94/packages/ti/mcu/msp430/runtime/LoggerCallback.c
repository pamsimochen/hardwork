/* --COPYRIGHT--,ESD
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
 *  This program and the accompanying materials are made available under the 
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at 
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * --/COPYRIGHT--*/
/*
 *  ======== LoggerCallback.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/System.h>

#include "package/internal/LoggerCallback.xdc.h"

/*
 *  ======== Instance_init =========
 */
Void LoggerCallback_Instance_init(LoggerCallback_Object *obj,
    const LoggerCallback_Params *prms)
{
    obj->enabled = TRUE;
}

/*
 *  ======== disable ========
 */
Bool LoggerCallback_disable(LoggerCallback_Object *obj)
{
    Bool prev = obj->enabled;
    obj->enabled = FALSE;
    return (prev);
}

/*
 *  ======== enable ========
 */
Bool LoggerCallback_enable(LoggerCallback_Object *obj)
{
    Bool prev = obj->enabled;
    obj->enabled = TRUE;
    return (prev);
}

/*
 *  ======== write0 =========
 */
Void LoggerCallback_write0(LoggerCallback_Object *obj, Types_Event evt,
    Types_ModuleId mid)
{
    LoggerCallback_write4(obj, evt, mid, 0, 0, 0, 0);
}

/*
 *  ======== write1 =========
 */
Void LoggerCallback_write1(LoggerCallback_Object *obj, Types_Event evt,
    Types_ModuleId mid, IArg a1)
{
    LoggerCallback_write4(obj, evt, mid, a1, 0, 0, 0);
}

/*
 *  ======== write2 =========
 */
Void LoggerCallback_write2(LoggerCallback_Object *obj, Types_Event evt,
    Types_ModuleId mid, IArg a1, IArg a2)
{
    LoggerCallback_write4(obj, evt, mid, a1, a2, 0, 0);
}

/*
 *  ======== write4 =========
 */
Void LoggerCallback_write4(LoggerCallback_Object *obj, Types_Event evt,
    Types_ModuleId mid, IArg a1, IArg a2, IArg a3, IArg a4)
{
    Log_EventRec evrec;
    
    LoggerCallback_TimestampProxy_get64(&evrec.tstamp);

    evrec.serial = 0;
    evrec.evt = Types_makeEvent(Log_getEventId(evt), mid);
    evrec.arg[0] = a1;
    evrec.arg[1] = a2;
    evrec.arg[2] = a3;
    evrec.arg[3] = a4;

    if (LoggerCallback_callbackFxn != NULL) {
        LoggerCallback_callbackFxn(&evrec);
    }
}

/*
 *  ======== write8 =========
 */
Void LoggerCallback_write8(LoggerCallback_Object *obj, Types_Event evt,
    Types_ModuleId mid, IArg a1, IArg a2, IArg a3, IArg a4, IArg a5, IArg a6,
    IArg a7, IArg a8)
{
    System_abort("FATAL ERROR: LoggerCallback: only 4 arguments are supported\n");
}
