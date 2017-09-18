#ifndef RIDL_CLIENT_H
#define RIDL_CLIENT_H

#include "dload.h"

/*---------------------------------------------------------------------------*/
/* Client Object instance.                                                   */
/*---------------------------------------------------------------------------*/
typedef struct
{
    /*-----------------------------------------------------------------------*/
    /* Contains filenames (type const char*) the system is in the process of */
    /* loading.  Used to detect cycles in incorrectly compiled ELF binaries. */
    /*-----------------------------------------------------------------------*/
    LOADER_OBJECT *loaderHandle;
    BOOL DLL_debug;
                                                                                   
} mockClient;

void* get_client_handle();
void set_dload_handle(void *client_handle, DLOAD_HANDLE handle);


#endif
