/*****************************************************************************/
/* dlw_client.c                                                              */
/*                                                                           */
/* RIDL implementation of client functions required by dynamic loader API.   */
/* Please see list of client-required API functions in dload_api.h.          */
/*                                                                           */
/* This implementation of RIDL is expected to run on the DSP.  It uses C6x   */
/* RTS functions for file I/O and memory management (both host and target    */
/* memory).                                                                  */
/* A loader that runs on a GPP for the purposes of loading C6x code onto a   */
/* DSP will likely need to re-write all of the functions contained in this   */
/* module.                                                                   */
/*                                                                           */
/*****************************************************************************/


/* Standard headers for SysLink */
#include "ArrayList.h"
#include "dload_api.h"
#if defined(SYSLINK_BUILD_HLOS)
#include <ti/syslink/Std.h>
#if defined (__KERNEL__)
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include <string.h>
#endif /* #if defined (__KERNEL__) */
#endif /* #if defined(SYSLINK_BUILD_HLOS) */
#if defined(SYSLINK_BUILD_RTOS)
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#endif /* #if defined(SYSLINK_BUILD_RTOS) */
//#include <dload.h>
#include <dlw_dsbt.h>
#include <ti/syslink/inc/knl/_ElfLoader.h>
/*-------------------------   OSAL and utils   -------------------------------*/
#include <ti/syslink/utils/Trace.h>

/*---------------------------------------------------------------------------*/
/* Global flag to control debug output.                                      */
/*---------------------------------------------------------------------------*/
BOOL debugging_on = 0;
BOOL profiling_on = 0;

/*****************************************************************************/
/* Client Provided File I/O                                                  */
/*****************************************************************************/
/*****************************************************************************/
/* DLIF_FSEEK() - Seek to a position in specified file.                      */
/*****************************************************************************/
int DLIF_fseek(LOADER_FILE_DESC *stream, int32_t offset, int origin)
{
   return ElfLoaderFile_seek(NULL, stream, offset, (LoaderFile_Pos)origin);
}

/*****************************************************************************/
/* DLIF_FTELL() - Return the current position in the given file.             */
/*****************************************************************************/
int32_t DLIF_ftell(LOADER_FILE_DESC *stream)
{
   return ElfLoaderFile_tell(NULL, stream);
}

/*****************************************************************************/
/* DLIF_FREAD() - Read data from file into a host-accessible data buffer     */
/*      that can be accessed through "ptr".                                  */
/*****************************************************************************/
size_t DLIF_fread(void *ptr, size_t size, size_t nmemb,
                  LOADER_FILE_DESC *stream)
{
   return ElfLoaderFile_read(NULL, stream, ptr, size, nmemb);
}

/*****************************************************************************/
/* DLIF_FCLOSE() - Close a file that was opened on behalf of the core        */
/*      loader. Core loader has ownership of the file pointer, but client    */
/*      has access to file system.                                           */
/*****************************************************************************/
int32_t DLIF_fclose(LOADER_FILE_DESC *fd)
{
   return ElfLoaderFile_close(NULL, fd);
}

/*****************************************************************************/
/* Client Provided Host Memory Management                                    */
/*****************************************************************************/
/*****************************************************************************/
/* DLIF_MALLOC() - Allocate host memory suitable for loader scratch space.   */
/*****************************************************************************/
void* DLIF_malloc(size_t size)
{
   return ElfLoaderMem_alloc(NULL, size*sizeof(uint8_t));
}

/*****************************************************************************/
/* DLIF_FREE() - Free host memory previously allocated with DLIF_malloc().   */
/*****************************************************************************/
void DLIF_free(void* ptr)
{
    /*
     * TBD: DLIF_free() does not pass back a size!, so
     * 1 is passed here:
     *   to prevent assert in MemoryOS_free; and
     *   since MemoryOS_free in kernel does not actually use the size arg.
     */
   ElfLoaderMem_free(NULL, ptr, 1);
}

/*****************************************************************************/
/* Client Provided Target Memory Management                                  */
/*****************************************************************************/

/*****************************************************************************/
/* RIDL-Specific hack to facilitate target memory allocation.                */
/*****************************************************************************/
/*****************************************************************************/
/* DLIF_ALLOCATE() - Return the load address of the segment/section          */
/*      described in its parameters and record the run address in            */
/*      run_address field of DLOAD_MEMORY_REQUEST.                           */
/*****************************************************************************/
BOOL DLIF_allocate(void* client_handle, struct DLOAD_MEMORY_REQUEST *targ_req)
{
    BOOL retval = TRUE;

    if (ElfLoaderTrgMem_alloc(NULL, targ_req) < 0) {
        retval = FALSE;
    }

    return retval;
}

/*****************************************************************************/
/* DLIF_RELEASE() - Unmap or free target memory that was previously          */
/*      allocated by DLIF_allocate().                                        */
/*****************************************************************************/
BOOL DLIF_release(void* client_handle, struct DLOAD_MEMORY_SEGMENT* ptr)
{
    BOOL retval = TRUE;

    if (ElfLoaderTrgMem_free(NULL, ptr) < 0) {
        retval = FALSE;
    }

    return retval;
}

/*****************************************************************************/
/* DLIF_COPY() - Copy data from file to host-accessible memory.              */
/*      Returns a host pointer to the data in the host_address field of the  */
/*      DLOAD_MEMORY_REQUEST object.                                         */
/*****************************************************************************/
BOOL DLIF_copy(void *client_handle, struct DLOAD_MEMORY_REQUEST* targ_req)
{
    BOOL retval = TRUE;

    if (ElfLoaderTrgWrite_copy (client_handle, targ_req) < 0) {
        retval = FALSE;
    }

    return retval;
}

/*****************************************************************************/
/* DLIF_READ() - Read content from target memory address into host-          */
/*      accessible buffer.                                                   */
/*****************************************************************************/
BOOL DLIF_read(void *client_handle, void *buffPtr, size_t size, size_t nmemb, TARGET_ADDRESS src)
{
    BOOL retval = TRUE;

    if (ElfLoaderTrgWrite_read (client_handle, buffPtr, (UInt32)size, (UInt32)nmemb, src) < 0) {
        retval = FALSE;
    }

   return retval;
}

/*****************************************************************************/
/* DLIF_WRITE() - Write updated (relocated) segment contents to target       */
/*      memory.                                                              */
/*****************************************************************************/
BOOL DLIF_write(void *client_handle, struct DLOAD_MEMORY_REQUEST* req)
{

    BOOL retval = TRUE;

    if (ElfLoaderTrgWrite_write (client_handle, req) < 0) {
        retval = FALSE;
    }

    return retval;
}

/*****************************************************************************/
/* DLIF_MEMCPY() - Write updated (relocated) segment contents to target      */
/*      memory.                                                              */
/*****************************************************************************/
BOOL DLIF_memcpy(void *client_handle, void *to, void *from, size_t size)
{
   BOOL retval = TRUE;

   struct DLOAD_MEMORY_REQUEST memReq;
   struct DLOAD_MEMORY_SEGMENT memSeg;

   /*
    * Fake up a memReq record, setting only those fields used by ElfLoaderTrgWrite_write()
    */
   memReq.is_loaded = FALSE;
   memReq.segment = &memSeg;
   memReq.host_address = from;
   memSeg.target_address = to;
   memSeg.objsz_in_bytes = size;

    if (ElfLoaderTrgWrite_write (client_handle, &memReq) < 0) {
        retval = FALSE;
    }

    return retval;
}

/*****************************************************************************/
/* DLIF_EXECUTE() - Transfer control to specified target address.            */
/*****************************************************************************/
int32_t DLIF_execute(TARGET_ADDRESS exec_addr)
{

   /*
    * Do nothing here: Processor_start will handle this separately.
    *
    * Note: DLOAD doesn't check DLIF_execute return value, so this fxn
    * should ideally be of type void.
    *
    */

   return (1);
}


/*****************************************************************************/
/* Client Provided Communication Mechanisms to assist with creation of       */
/* DLLView debug information.  Client needs to know exactly when a segment   */
/* is being loaded or unloaded so that it can keep its debug information     */
/* up to date.                                                               */
/*****************************************************************************/
/*****************************************************************************/
/* DLIF_LOAD_DEPENDENT() - Perform whatever maintenance is needed in the     */
/*      client when loading of a dependent file is initiated by the core     */
/*      loader.  Open the dependent file on behalf of the core loader,       */
/*      then invoke the core loader to get it into target memory. The core   */
/*      loader assumes ownership of the dependent file pointer and must ask  */
/*      the client to close the file when it is no longer needed.            */
/*                                                                           */
/*      If debug support is needed under the Braveheart model, then create   */
/*      a host version of the debug module record for this object.  This     */
/*      version will get updated each time we allocate target memory for a   */
/*      segment that belongs to this module.  When the load returns, the     */
/*      client will allocate memory for the debug module from target memory  */
/*      and write the host version of the debug module into target memory    */
/*      at the appropriate location.  After this takes place the new debug   */
/*      module needs to be added to the debug module list.  The client will  */
/*      need to update the tail of the DLModules list to link the new debug  */
/*      module onto the end of the list.                                     */
/*                                                                           */
/*****************************************************************************/
int DLIF_load_dependent(void* client_handle, const char* so_name)
{

#if defined (DLOAD_PORTED)

   _ElfLoader_Object *clientObj = (_ElfLoader_Object *)client_handle;

   /*------------------------------------------------------------------------*/
   /* Find the path and file name associated with the given so_name in the   */
   /* client's file registry.                                                */
   /*------------------------------------------------------------------------*/
   /* If we can't find the so_name in the file registry (or if the registry  */
   /* is not implemented yet), we'll open the file using the so_name.        */
   /*------------------------------------------------------------------------*/
   int to_ret = 0;
   FILE* fp = fopen(so_name, "rb");

   /*------------------------------------------------------------------------*/
   /* We need to make sure that the file was properly opened.                */
   /*------------------------------------------------------------------------*/
   if (!fp)
   {
      DLIF_error(DLET_FILE, "Can't open dependent file '%s'.\n", so_name);
      return 0;
   }

   /*------------------------------------------------------------------------*/
   /* If the dynamic loader is providing debug support for a DLL View plug-  */
   /* in or script of some sort, then we are going to create a host version  */
   /* of the debug module record for the so_name module.                     */
   /*------------------------------------------------------------------------*/
   /* In the Braveheart view of the world, debug support is only to be       */
   /* provided if the DLModules symbol is defined in the base image.         */
   /* We will set up a DLL_debug flag when the command to load the base      */
   /* image is issued.                                                       */
   /*------------------------------------------------------------------------*/
   if (DLL_debug)
      DLDBG_add_host_record(client_handle, so_name);

   /*------------------------------------------------------------------------*/
   /* Tell the core loader to proceed with loading the module.               */
   /*------------------------------------------------------------------------*/
   /* Note that the client is turning ownership of the file pointer over to  */
   /* the core loader at this point. The core loader will need to ask the    */
   /* client to close the dependent file when it is done using the dependent */
   /* file pointer.                                                          */
   /*------------------------------------------------------------------------*/
   to_ret = DLOAD_load(clientObj->dloadHandle, fp);

   /*------------------------------------------------------------------------*/
   /* If the dependent load was successful, update the DLModules list in     */
   /* target memory as needed.                                               */
   /*------------------------------------------------------------------------*/
   if (to_ret != 0)
   {
      /*---------------------------------------------------------------------*/
      /* We will need to copy the information from our host version of the   */
      /* debug record into actual target memory.                             */
      /*---------------------------------------------------------------------*/
      if (DLL_debug)
      {
         /*---------------------------------------------------------------*/
         /* Allocate target memory for the module's debug record.  Use    */
         /* host version of the debug information to determine how much   */
         /* target memory we need and how it is to be filled in.          */
         /*---------------------------------------------------------------*/
         /* Note that we don't go through the normal API functions to get */
         /* target memory and write the debug information since we're not */
         /* dealing with object file content here.  The DLL View debug is */
         /* supported entirely on the client side.                        */
         /*---------------------------------------------------------------*/
         DLDBG_add_target_record(client_handle, to_ret);
      }
   }

   /*------------------------------------------------------------------------*/
   /* Report failure to load dependent.                                      */
   /*------------------------------------------------------------------------*/
   else
      DLIF_error(DLET_MISC, "Failed load of dependent file '%s'.\n", so_name);

   return to_ret;

#else  // #if defined (DLOAD_PORTED)

   DLIF_error(DLET_MISC, "DLIF_load_dependent not implemented!!!\n");

   return 0;

#endif  // #if defined (DLOAD_PORTED)
}

/*****************************************************************************/
/* DLIF_UNLOAD_DEPENDENT() - Perform whatever maintenance is needed in the   */
/*      client when unloading of a dependent file is initiated by the core   */
/*      loader.  Invoke the DLOAD_unload() function to get the core loader   */
/*      to release any target memory that is associated with the dependent   */
/*      file's segments.                                                     */
/*****************************************************************************/
void DLIF_unload_dependent(void* client_handle, uint32_t file_handle)
{
#if defined (DLOAD_PORTED)
   _ElfLoader_Object *clientObj = (_ElfLoader_Object *)client_handle;

   /*------------------------------------------------------------------------*/
   /* If the specified module is no longer needed, DLOAD_load() will spin    */
   /* through the object descriptors associated with the module and free up  */
   /* target memory that was allocated to any segment in the module.         */
   /*------------------------------------------------------------------------*/
   /* If DLL debugging is enabled, find module debug record associated with  */
   /* this module and remove it from the DLL debug list.                     */
   /*------------------------------------------------------------------------*/
   if (DLOAD_unload(clientObj->dloadHandle, file_handle))
   {
      DSBT_release_entry(file_handle);
      if (DLL_debug) DLDBG_rm_target_record(client_handle, file_handle);
   }
#else

   DLIF_error(DLET_MISC, "DLIF_unload_dependent not implemented!!!\n");

#endif
}

/*****************************************************************************/
/* Client Provided API Functions to Support Logging Warnings/Errors          */
/*****************************************************************************/
/*---------------------------------------------------------------------------*/
/* DLIF_trace() - Trace message output, possibly using alternative to printf.*/
/* Note: This will only print if TRACE_CLASS >= 2.                           */
/*---------------------------------------------------------------------------*/
void     DLIF_trace(const char *fmt, ...)
{
#if defined (SYSLINK_TRACE_ENABLE)

#if defined(SYSLINK_BUILDOS_LINUX)
   va_list ap;

   if ((((curTrace & GT_TRACESTATE_MASK) >> GT_TRACESTATE_SHIFT)
        ==  GT_TraceState_Enable) &&
       ((curTrace & GT_TRACECLASS_MASK) >= GT_2CLASS))  {

      printk ("DLOAD: ");
      va_start(ap,fmt);
      vprintk(fmt,ap);
      va_end(ap);
   }
#endif /* #if defined(SYSLINK_BUILD_HLOS) */

#if defined(SYSLINK_BUILD_RTOS)
   VaList va;
   if ((((curTrace & GT_TRACESTATE_MASK) >> GT_TRACESTATE_SHIFT)
        ==  GT_TraceState_Enable) &&
       ((curTrace & GT_TRACECLASS_MASK) >= GT_2CLASS))  {

      System_printf ("DLOAD: ");
      va_start(va,fmt);
      System_vprintf((String)fmt,va);
      va_end(va);
   }

#endif /* #if defined(SYSLINK_BUILD_RTOS) */

#endif /* if defined (SYSLINK_TRACE_ENABLE) */
}

/*****************************************************************************/
/* DLIF_WARNING() - Write out a warning message from the core loader.        */
/*****************************************************************************/
void DLIF_warning(LOADER_WARNING_TYPE wtype, const char *fmt, ...)
{
#if defined(SYSLINK_BUILDOS_LINUX)
   va_list ap;

   printk ("<< DLOAD >> WARNING: ");

   va_start(ap,fmt);
   vprintk(fmt,ap);
   va_end(ap);
#endif /* #if defined(SYSLINK_BUILDOS_LINUX) */

#if defined(SYSLINK_BUILD_RTOS)
   VaList va;

   System_printf ("<< DLOAD >> WARNING: ");

   va_start(va,fmt);
   System_vprintf ((String)fmt,va);
   va_end(va);
#endif /* #if defined(SYSLINK_BUILD_RTOS) */


}

/*****************************************************************************/
/* DLIF_ERROR() - Write out an error message from the core loader.           */
/*****************************************************************************/
void DLIF_error(LOADER_ERROR_TYPE etype, const char *fmt, ...)
{
#if defined(SYSLINK_BUILDOS_LINUX)
    va_list ap;

   printk ("<< DLOAD >> ERROR: ");

   va_start(ap,fmt);
   vprintk(fmt,ap);
   va_end(ap);
#endif /* #if defined(SYSLINK_BUILDOS_LINUX) */
#if defined(SYSLINK_BUILD_RTOS)
   VaList va;

   System_printf ("<< DLOAD >> ERROR: ");

   va_start(va,fmt);
   System_vprintf ((String)fmt,va);
   va_end(va);
#endif /* #if defined(SYSLINK_BUILD_RTOS) */

   GT_assert (curTrace, TRUE);
}

/*****************************************************************************
 * END API FUNCTION DEFINITIONS
 *****************************************************************************/
