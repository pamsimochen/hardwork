/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
    \defgroup VPSEXAMPLE_SC_LINK_API Sc Link API

    Sc Link can be used to take input from a link and after doing the scaling
    put into output queue.

    Each frame output is put in its corresponding output queue.

    @{
*/

/**
    \file scLink.h
    \brief Scalar Link API
*/

#ifndef _SC_LINK_H_
#define _SC_LINK_H_

#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>

/** \brief DEI Scaler output ID */
#define SC_LINK_OUT_QUE_SC      (0)

/** \brief Max DEI outputs/output queues */
#define SC_LINK_MAX_OUT_QUE     (1)

/**
    \brief Sc link create parameters
*/
typedef struct
{
    Sys_ScCfgId                 scCfgId;
    /**< System configuration ID. */
    System_LinkInQueParams      inQueParams;
    /**< Input queue information. */
    System_LinkOutQueParams     outQueParams[SC_LINK_MAX_OUT_QUE];
    /**< Output queue information. */
    UInt32                      numOutChs;
    /**< Scalar link will get N input channels from the previous link.  It can
      *  then output M channels out of N  input channels for next link. where
      *  M <= N
      */
    UInt32                      outChStart;
    /**< Out of N channels scalar is getting where the M should start for
      *  output queue.  If N  is 8 and M is 3 M can be 0-2, 1-3, 2-4,3-5,4-6
      *  or 5-7. where outchStart can be 0,1,2,3,4,5
      */
} ScLink_CreateParams;

/**
    \brief Sc link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 ScLink_init();

/**
    \brief Sc link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 ScLink_deInit();

#endif

/*@}*/

