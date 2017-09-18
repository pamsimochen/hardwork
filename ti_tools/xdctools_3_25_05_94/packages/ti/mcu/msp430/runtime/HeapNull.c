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
 *  ======== HeapNull.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

#include "package/internal/HeapNull.xdc.h"

/*
 *  ======== HeapNull_Instance_init ========
 *  Set-up the object's fields.
 */
Void HeapNull_Instance_init(HeapNull_Object *obj, const HeapNull_Params *params)
{
}

/*
 *  ======== HeapNull_alloc ========
 *  This is a simple growth-only heap. This function returns the current
 *  location of the top of unused buffer.
 */
Ptr HeapNull_alloc(HeapNull_Object *obj, SizeT size, SizeT align,
    Error_Block *eb)
{
    return (NULL);
}

/*
 *  ======== HeapNull_free ========
 *  This is a growth-only heap, so free does nothing.
 */
Void HeapNull_free(HeapNull_Object *obj, Ptr block, SizeT size)
{
}

/*
 *  ======== HeapNull_isBlocking ======== 
 */
Bool HeapNull_isBlocking(HeapNull_Object *obj)
{
    return (FALSE);
}

/*
 *  ======== HeapNull_getStats ======== 
 *  Return the stats.
 */
Void HeapNull_getStats(HeapNull_Object *obj, Memory_Stats *stats)
{
    stats->totalSize         = 0;
    stats->totalFreeSize     = 0;
    stats->largestFreeSize   = 0;
}
