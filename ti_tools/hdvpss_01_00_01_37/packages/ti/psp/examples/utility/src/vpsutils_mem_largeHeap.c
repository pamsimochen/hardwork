/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <ti/psp/vps/vps.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>


#ifdef TI_816X_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP    (704*MB)
#endif

#ifdef TI_814X_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP    (123*MB)
#endif

#ifdef TI_8107_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP    (123*MB)
#endif

#ifdef CUSTOM_MEM_FRAME_HEAP_SIZE
#undef VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP
#define VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP     (CUSTOM_MEM_FRAME_HEAP_SIZE*MB)
#endif

/* Memory pool */
#pragma DATA_ALIGN(gVpsUtils_heapMemFrame_largeHeap, VPS_BUFFER_ALIGNMENT)
#pragma DATA_SECTION(gVpsUtils_heapMemFrame_largeHeap,".bss:frameBuffer");
UInt32 gVpsUtils_heapMemFrame_largeHeap[VPSUTILS_MEM_FRAME_HEAP_SIZE_LARGE_HEAP/sizeof(UInt32)];


Int32 VpsUtils_memInit_largeHeap()
{
    VpsUtils_memInit_internal(gVpsUtils_heapMemFrame_largeHeap, sizeof(gVpsUtils_heapMemFrame_largeHeap));

  return 0;
}
