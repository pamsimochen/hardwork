/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_vpdma_lm.h
*
* Brief: This file contains the Register Description for VPDMA List Manager Module
*
*********************************************************************/
#ifndef _CSLR_VPS_VPDMA_LM_H_
#define _CSLR_VPS_VPDMA_LM_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/

typedef struct  {
    volatile Uint16             descPtr;
    /**< Descriptor Pointer: points to the location of the next descriptor to
         be processed in the list.
         Not used in current design. */
    volatile Uint16             chBlock;
    /**< This register contains the current status of the channel that it is
         associated with.
         There are two different methods for loading values into
         this register.
         The first is by hardware, whenever an access on the list
         is rejected by a DMA channel, that channels number is loaded into the
         registers to enable the sync controller to signal a sync condition when
         that channel has completed it transactions. When the channel blocking
         condition is cleared, the registers is returned to the active channel
         value (0x01FE).
         Firmware may also write to this register to set conditions or signal
         flags for debugging or for any predefined control activities.

          Code                    Indication
          ----------------------------------------------------------------------
          0x1FF                   List inactive
          0x1FE                   List Active and free running
          0x1FD                   Illegal Control Descriptor
          0x1FC                   Configuration Descriptor Operation
          0x1FB                   Regular List operation
          0x1FA                   List Queue Empty Operation
          0x1FD - 0x1F0           List Active, debug label
          0x1EF - 0x000           List blocked by channel

         The values held in this register are useful for debug to indicate the
         last operation which was run on the list. These are invalidated if
         the list is stopped for any reason. Memory variables LSTDSC (0-7) are
         more useful as they are only updated when a new routine is entered and
         they are not cleared if the list is disabled. */
    volatile Uint16             descLocationLo;
    /**< Low short of location of List Descriptors in external memory.
         Not used in current design. */
    volatile Uint16             descLocationHi;
    /**< Upper short of location of List Descriptors in external memory.
         Not used in current design. */
    volatile Uint16             cfg;
    /**< Not used in current design. */
    volatile Uint16             rsvd0[3u];
    /**< Reserved. */
} CSL_Vps_vpdmaLmInfo0Regs;

typedef struct  {
    volatile Uint16             srcAddrLo;
    volatile Uint16             srcAddrHi;
    volatile Uint16             clientCtrl;
    volatile Uint16             rsvd0;
    volatile Uint16             listSize;
    volatile Uint16             rsvd1;
    volatile Uint16             listType;
    volatile Uint16             rsvd2;
} CSL_Vps_vpdmaLmInfo1Regs;

typedef struct  {
    CSL_Vps_vpdmaLmInfo0Regs    listInfo0[8u];
    CSL_Vps_vpdmaLmInfo1Regs    listInfo1[8u];
    volatile Uint16             rsvd0[80u];
    volatile Uint16             curAddrLo;
    volatile Uint16             curAddrHi;
    volatile Uint16             curLineCount;
    volatile Uint16             curLineStride;
    volatile Uint16             nextAddrLo;
    volatile Uint16             nextAddrHi;
    volatile Uint16             rsvd1[10u];
    volatile Uint16             jumpAddr;
    volatile Uint16             newListJump;
    volatile Uint16             regListJump;
    volatile Uint16             selfListJump;
    volatile Uint16             compListJump;
    volatile Uint16             rsvd2[9u];
    volatile Uint16             intrCmd;
    volatile Uint16             rsvd3;
    volatile Uint16             syncMask[8u];
    volatile Uint16             syncMaskAll;
    volatile Uint16             listSyncMask;
    volatile Uint16             listService;
    volatile Uint16             timerSetup;
    volatile Uint16             pid;
    volatile Uint16             clientNumOvrd;
    volatile Uint16             curList;
    volatile Uint16             syncClear;
} CSL_Vps_vpdmaLmRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_vpdmaLmRegs       *CSL_VpsVpdmaLmRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* None */

#endif
