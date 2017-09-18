/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <ti/psp/vps/vps.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>

/** \brief Maximum frame buffer memory pool size */
#ifdef TI_816X_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE    (209*MB)
#endif

#ifdef TI_814X_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE    (123*MB)
#endif

#ifdef TI_8107_BUILD
#define VPSUTILS_MEM_FRAME_HEAP_SIZE    (123*MB)
#endif

#ifdef CUSTOM_MEM_FRAME_HEAP_SIZE
#undef VPSUTILS_MEM_FRAME_HEAP_SIZE
#define VPSUTILS_MEM_FRAME_HEAP_SIZE    (CUSTOM_MEM_FRAME_HEAP_SIZE*MB)
#endif

/* Memory pool */
#pragma DATA_ALIGN(gVpsUtils_heapMemFrame, VPS_BUFFER_ALIGNMENT)
#pragma DATA_SECTION(gVpsUtils_heapMemFrame,".bss:frameBuffer");
UInt32 gVpsUtils_heapMemFrame[VPSUTILS_MEM_FRAME_HEAP_SIZE/sizeof(UInt32)];


Int32 VpsUtils_memInit()
{
    VpsUtils_memInit_internal(gVpsUtils_heapMemFrame, sizeof(gVpsUtils_heapMemFrame));

  return 0;
}
