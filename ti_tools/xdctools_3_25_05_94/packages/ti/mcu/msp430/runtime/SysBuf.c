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
 *  ======== SysBuf.c ========
 */

#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Gate.h>

#include "package/internal/SysBuf.xdc.h"

/*
 *  ======== SysBuf_abort ========
 */
Void SysBuf_abort(CString str)
{
    Char ch;    
    
    if (SysBuf_bufSize != 0 && str != NULL) {
        while ((ch = *str++) != '\0') {
            SysBuf_putch(ch);
        }
    }
}

/*
 *  ======== SysBuf_exit ========
 */
Void SysBuf_exit(Int stat)
{
    SysBuf_flush();
}

/*
 *  ======== SysBuf_flush ========
 */
Void SysBuf_flush()
{
    Int i;
    
    for (i = 0; i < SysBuf_bufSize; i++) {
        Char c;
        if (module->outidx >= SysBuf_bufSize) {
            module->outidx = 0;
        }
        c = module->outbuf[module->outidx++];
        if (c != '\0') {
            extern int putchar(int c);
            putchar(c);
        }
    }
}

/*
 *  ======== SysBuf_putch ========
 */
Void SysBuf_putch(Char ch)
{
    IArg key;
    
    if (SysBuf_bufSize != 0) {

        key = Gate_enterSystem();

        if (module->outidx >= SysBuf_bufSize) {
            module->outidx = 0;
        }

        module->outbuf[module->outidx++] = ch;

        Gate_leaveSystem(key);
    }    
}

/*
 *  ======== SysBuf_ready ========
 */
Bool SysBuf_ready()
{
    return (SysBuf_bufSize != 0 ? TRUE : FALSE);
}

