/** 
 *  @file   _ListMP.h
 *
 *  @brief      Defines for shared memory doubly linked list.
 *
 *              This module implements the ListMP.
 *              ListMP is a linked-list based module designed to be used in a
 *              multi-processor environment.  It is designed to provide a means
 *              of communication between different processors.
 *              processors.
 *              This module is instance based. Each instance requires a small
 *              piece of shared memory. This is specified via the #sharedAddr
 *              parameter to the create. The proper #sharedAddrSize parameter
 *              can be determined via the #sharedMemReq call. Note: the
 *              parameters to this function must be the same that will used to
 *              create or open the instance.
 *              The ListMP module uses a #ti.sdo.utils.NameServer instance
 *              to store instance information when an instance is created and
 *              the name parameter is non-NULL. If a name is supplied, it must
 *              be unique for all ListMP instances.
 *              The #create also initializes the shared memory as needed. The
 *              shared memory must be initialized to 0 or all ones
 *              (e.g. 0xFFFFFFFFF) before the ListMP instance is created.
 *              Once an instance is created, an open can be performed. The #open
 *              is used to gain access to the same ListMP instance.
 *              Generally an instance is created on one processor and opened
 *              on the other processor.
 *              The open returns a ListMP instance handle like the create,
 *              however the open does not modify the shared memory. Generally an
 *              instance is created on one processor and opened on the other
 *              processor.
 *              There are two options when opening the instance:
 *              @li Supply the same name as specified in the create. The
 *              ListMP module queries the NameServer to get the needed
 *              information.
 *              @li Supply the same #sharedAddr value as specified in the
 *              create.
 *              If the open is called before the instance is created, open
 *              returns NULL.
 *              There is currently a list of restrictions for the module:
 *              @li Both processors must have the same endianness. Endianness
 *              conversion may supported in a future version of ListMP.
 *              @li The module will be made a gated module
 *
 *
 */
/* 
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */



#ifndef LISTMP_H_0xA413
#define LISTMP_H_0xA413

/* Utilities headers */
#include <ti/ipc/ListMP.h>
#include <ti/ipc/SharedRegion.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Module Id
 * =============================================================================
 */
/*!
 *  @def    LISTMP_MODULEID
 *  @brief  Unique module ID.
 */
#define LISTMP_MODULEID            (0xa413)

/*!
 *  @def    ListMP_CREATED
 *  @brief  Created tag
 */
#define ListMP_CREATED              0x12181964



/* =============================================================================
 *  Structures
 * =============================================================================
 */
/*!
 *  @def    ListMP_Attrs
 *  @brief  Shared memory attributes
 */
typedef struct ListMP_Attrs_tag {
    Bits32             status;
    SharedRegion_SRPtr gateMPAddr;
    ListMP_Elem        head;
}ListMP_Attrs;

/*!
 *  @brief  Structure defining config parameters for the ListMP module.
 */
typedef struct ListMP_Config_tag {
    UInt maxRuntimeEntries;
    /*!< Maximum number of ListMP's that can be dynamically created and
         added to the NameServer. */
    UInt maxNameLen;
    /*!< Maximum length of name */
} ListMP_Config;


/*!
 *  @brief  Structure defining processor related information for the
 *          ListMP module.
 */
typedef struct ListMP_ProcAttrs_tag {
    Bool   creator;   /*!< Creator or opener */
    UInt16 procId;    /*!< Processor Identifier */
    UInt32 openCount; /*!< How many times it is opened on a processor */
} ListMP_ProcAttrs;


/* =============================================================================
 *  Functions to create the module
 * =============================================================================
 */
/*  Function to get configuration parameters to setup
 *  the ListMP module.
 */
Void ListMP_getConfig (ListMP_Config * cfgParams);

/* Function to setup the ListMP module.  */
Int ListMP_setup (ListMP_Config * config) ;

/* Function to destroy the ListMP module. */
Int ListMP_destroy (void);



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* LISTMP_H_0xF414 */

