/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _NSF_LINK_H_
#define _NSF_LINK_H_


#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/vps/vps_m2mNsf.h>


#define NSF_LINK_MAX_OUT_QUE (2)


typedef struct
{
    System_LinkInQueParams      inQueParams;

    /* Since NSF link can have 2 output queues, incoming channels will
     * be splitted in half automatically if user enables both the queues.
     * Channels {0 to (incomingChannels/2 - 1)} will goto output queue 0 and
     * channels {incomingChannels/2 to incomingChannels} will goto output queue 1.
     * If only 1 output queue is enabled, incoming channels will not be
     * splitted and sent only to output queue 0.
     * For e.g.:
     * Incoming channels = 16, numOutQue = 1 -> outQueue0 = 16, outQueue1 = 0
     * Incoming channels = 16, numOutQue = 2 -> outQueue0 = 8, outQueue1 = 8
     * Incoming channels = 8, numOutQue = 1 -> outQueue0 = 8, outQueue1 = 0
     * Incoming channels = 8, numOutQue = 2 -> outQueue0 = 4, outQueue1 = 4
     */
    UInt32                      numOutQue;
    System_LinkOutQueParams     outQueParams[NSF_LINK_MAX_OUT_QUE];

    Bool                        bypassNsf; /* applied for all Chs, do only Chroma DS */
    Bool                        tilerEnable;

    /* send even fields only to another link on another queue */
    Bool                        enableEvenFieldOutput;
    System_LinkOutQueParams     enableEvenFieldOutputQueParams;

} NsfLink_CreateParams;

Int32 NsfLink_init();
Int32 NsfLink_deInit();

#endif
