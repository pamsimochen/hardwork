/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <string.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/vps/vps.h>

/* See vpsutils_que.h for function documentation  */

Int32 VpsUtils_queCreate ( VpsUtils_QueHandle * handle,
                           UInt32 maxElements, Ptr queueMem, UInt32 flags )
{
    Semaphore_Params semParams;

    /*
     * init handle to 0's
     */
    memset ( handle, 0, sizeof ( *handle ) );

    /*
     * init handle with user parameters
     */
    handle->maxElements = maxElements;
    handle->flags = flags;

    /*
     * queue data element memory cannot be NULL
     */
    GT_assert( GT_DEFAULT_MASK,  queueMem != NULL );

    handle->queue = queueMem;

    if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_GET )
    {
        /*
         * user requested block on que get
         */

        /*
         * create semaphore for it
         */

        Semaphore_Params_init ( &semParams );
        semParams.mode = Semaphore_Mode_BINARY;

        handle->semRd = Semaphore_create ( 0, &semParams, NULL );

        GT_assert( GT_DEFAULT_MASK,  handle->semRd != NULL );
    }

    if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_PUT )
    {
        /*
         * user requested block on que put
         */

        /*
         * create semaphore for it
         */

        Semaphore_Params_init ( &semParams );
        semParams.mode = Semaphore_Mode_BINARY;

        handle->semWr = Semaphore_create ( 0, &semParams, NULL );

        GT_assert( GT_DEFAULT_MASK,  handle->semWr != NULL );
    }
    return 0;
}

Int32 VpsUtils_queDelete ( VpsUtils_QueHandle * handle )
{
    if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_GET )
    {
        /*
         * user requested block on que get
         */

        /*
         * delete associated semaphore
         */

        Semaphore_delete ( &handle->semRd );
    }
    if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_PUT )
    {
        /*
         * user requested block on que put
         */

        /*
         * delete associated semaphore
         */

        Semaphore_delete ( &handle->semWr );
    }

    return 0;
}

Int32 VpsUtils_quePut ( VpsUtils_QueHandle * handle, Ptr data, Int32 timeout )
{
    Int32 status = -1;          /* init status to error */
    UInt32 cookie;

    do
    {
        /*
         * disable interrupts
         */
        cookie = Hwi_disable (  );

        if ( handle->count < handle->maxElements )
        {
            /*
             * free space available in que
             */

            /*
             * insert element
             */
            handle->queue[handle->curWr] = data;

            /*
             * increment put pointer
             */
            handle->curWr = ( handle->curWr + 1 ) % handle->maxElements;

            /*
             * increment count of number element in que
             */
            handle->count++;

            /*
             * restore interrupts
             */
            Hwi_restore ( cookie );

            /*
             * mark status as success
             */
            status = 0;

            if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_GET )
            {
                /*
                 * blocking on que get enabled
                 */

                /*
                 * post semaphore to unblock, blocked tasks
                 */
                Semaphore_post ( handle->semRd );
            }

            /*
             * exit, with success
             */
            break;

        }
        else
        {
            /*
             * que is full
             */

            /*
             * restore interrupts
             */
            Hwi_restore ( cookie );

            if ( timeout == BIOS_NO_WAIT )
                break;  /* non-blocking function call, exit with error */

            if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_PUT )
            {
                /*
                 * blocking on que put enabled
                 */

                /*
                 * take semaphore and block until timeout occurs or
                 * semaphore is posted
                 */
                if ( !Semaphore_pend ( handle->semWr, timeout ) )
                    break;  /* timeout happend, exit with error  */

                /*
                 * received semaphore, recheck for available space in the que
                 */
            }
            else
            {
                /*
                 * blocking on que put disabled
                 */

                /*
                 * exit with error
                 */
                break;
            }
        }
    }
    while ( 1 );

    return status;
}

Int32 VpsUtils_queGet ( VpsUtils_QueHandle * handle, Ptr * data,
                        UInt32 minCount, Int32 timeout )
{
    Int32 status = -1;          /* init status to error */
    UInt32 cookie;

    /*
     * adjust minCount between 1 and handle->maxElements
     */
    if ( minCount == 0 )
        minCount = 1;
    if ( minCount > handle->maxElements )
        minCount = handle->maxElements;

    do
    {
        /*
         * disable interrupts
         */
        cookie = Hwi_disable (  );

        if ( handle->count >= minCount )
        {
            /*
             * data elements available in que is >=
             * minimum data elements requested by user
             */

            /*
             * extract the element
             */
            *data = handle->queue[handle->curRd];

            /*
             * increment get pointer
             */
            handle->curRd = ( handle->curRd + 1 ) % handle->maxElements;

            /*
             * decrmeent number of elements in que
             */
            handle->count--;

            /*
             * restore interrupts
             */
            Hwi_restore ( cookie );

            /*
             * set status as success
             */
            status = 0;

            if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_PUT )
            {
                /*
                 * blocking on que put enabled
                 */

                /*
                 * post semaphore to unblock, blocked tasks
                 */
                Semaphore_post ( handle->semWr );
            }

            /*
             * exit with success
             */
            break;

        }
        else
        {
            /*
             * no elements or not enough element (minCount) in que to extract
             */

            /*
             * restore interrupts
             */
            Hwi_restore ( cookie );

            if ( timeout == BIOS_NO_WAIT )
                break;  /* non-blocking function call, exit with error  */

            if ( handle->flags & VPSUTILS_QUE_FLAG_BLOCK_QUE_GET )
            {
                /*
                 * blocking on que get enabled
                 */

                /*
                 * take semaphore and block until timeout occurs or
                 * semaphore is posted
                 */

                if ( !Semaphore_pend ( handle->semRd, timeout ) )
                    break;  /* timeout happened, exit with error */

                /*
                 * received semaphore, check que again
                 */
            }
            else
            {
                /*
                 * blocking on que get disabled
                 */

                /*
                 * exit with error
                 */
                break;
            }
        }
    }
    while ( 1 );

    return status;
}

UInt32 VpsUtils_queIsEmpty ( VpsUtils_QueHandle * handle )
{
    UInt32 isEmpty;
    UInt32 cookie;

    /*
     * disable interrupts
     */
    cookie = Hwi_disable (  );

    /*
     * check if que is empty
     */
    if ( handle->count )
        isEmpty = FALSE;    /* not empty  */
    else
        isEmpty = TRUE; /* empty  */

    /*
     * restore interrupts
     */
    Hwi_restore ( cookie );

    return isEmpty;
}

Int32 VpsUtils_quePeek ( VpsUtils_QueHandle * handle, Ptr * data )
{
    Int32 status = -1;          /* init status as error  */
    UInt32 cookie;

    *data = NULL;

    /*
     * disable interrupts
     */
    cookie = Hwi_disable (  );

    if ( handle->count )
    {
        /*
         * que is not empty
         */

        /*
         * get value of top element, but do not extract it from que
         */
        *data = handle->queue[handle->curRd];

        /*
         * set status as success
         */
        status = 0;
    }

    /*
     * restore interrupts
     */
    Hwi_restore ( cookie );

    return status;
}
