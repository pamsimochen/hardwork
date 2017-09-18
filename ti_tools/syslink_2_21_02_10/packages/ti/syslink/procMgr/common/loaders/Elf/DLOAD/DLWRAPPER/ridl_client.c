#include "ridl_client.h"


/*****************************************************************************/
/* get_client_handle()                                                       */
/*****************************************************************************/
void* get_client_handle()
{
    mockClient *pClient = DLIF_malloc(sizeof(mockClient));
    return ((void *)pClient);
}

/*****************************************************************************/
/* set_dload_handle()                                                        */
/*****************************************************************************/
void set_dload_handle(void *client_handle, DLOAD_HANDLE handle)
{
   mockClient *obj = (mockClient *)client_handle;
   obj->loaderHandle = (LOADER_OBJECT *)handle;
   obj->DLL_debug = FALSE;
} 

