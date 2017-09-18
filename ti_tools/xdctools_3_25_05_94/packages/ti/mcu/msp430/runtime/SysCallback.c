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
 *  ======== SysCallback.c ========
 */
#include <xdc/runtime/Startup.h>
#include <stdlib.h>

#include "package/internal/SysCallback.xdc.h"

/*
 *  ======== SysCallback_Module_startup ========
 */
Int SysCallback_Module_startup(Int state)
{
    if (SysCallback_initFxn != 0) {
        SysCallback_initFxn();
    }

    return (Startup_DONE);
}

/*
 *  ======== SysCallback_abort ========
 */
Void SysCallback_abort(CString str)
{
    if (str != NULL) {
        Char ch;
        
        while ((ch = *str++) != '\0') {
            SysCallback_putch(ch);
        }
    }

    /* simply return because System_abort() calls abort() next */
    return;
}

/*
 *  ======== SysCallback_exit ========
 *  Nothing to do here because System_exit() calls exit() which
 *  is already defined below.
 */
Void SysCallback_exit(Int stat)
{
}

/*
 *  ======== SysCallback_flush ========
 */
Void SysCallback_flush()
{
}

/*
 *  ======== SysCallback_putch ========
 */
Void SysCallback_putch(Char ch)
{
    if (SysCallback_putCharFxn != 0) {
        SysCallback_putCharFxn(ch);
    }
}

/*
 *  ======== SysCallback_ready ========
 */
Bool SysCallback_ready()
{
    return (TRUE);
}
