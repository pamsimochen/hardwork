/** 
 *  @file   GateProcess.h
 *
 *  @brief      Process level gate
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



#ifndef ti_syslink_utils_GateProcess
#define ti_syslink_utils_GateProcess


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Status codes
 * =============================================================================
 */

/*!
 *  @def    GateProcess_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GateProcess_E_INVALIDARG        (-1)

/*!
 *  @def    GateProcess_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GateProcess_E_MEMORY            (-2)

/*!
 *  @def    GateProcess_E_BUSY
 *  @brief  The name is already registered or not.
 */
#define GateProcess_E_BUSY              (-3)

/*!
 *  @def    GateProcess_E_FAIL
 *  @brief  Generic failure.
 */
#define GateProcess_E_FAIL              (-4)

/*!
 *  @def    GateProcess_E_NOTFOUND
 *  @brief  Name not found in the nameserver.
 */
#define GateProcess_E_NOTFOUND          (-5)

/*!
 *  @def    GateProcess_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateProcess_E_INVALIDSTATE      (-6)

/*!
 *  @def    GateProcess_E_INUSE
 *  @brief  Indicates that the instance is in use.
 */
#define GateProcess_E_INUSE             (-7)

/*!
 *  @def    GateProcess_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GateProcess_S_SUCCESS           (0)


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*! @brief  Object for GateProcess */
typedef struct GateProcess_Object GateProcess_Object;

/*!
 *  @brief  Handle for GateProcess
 */
typedef GateProcess_Object * GateProcess_Handle;

/*!
 *  @brief  Create parameter structure
 */
typedef struct {
    UInt    reserved;
} GateProcess_Params;


/*!
 *  @brief  Shared memory structure used by instance
 */
typedef struct {
    Char _f1[16];
} GateProcess_ShMem;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*
 *  ======== GateProcess_enter ========
 */
IArg GateProcess_enter(GateProcess_Handle handle);

/*
 *  ======== GateProcess_leave ========
 */
Void GateProcess_leave(GateProcess_Handle handle, IArg key);

/*
 *  ======== GateProcess_Params_init ========
 */
Void GateProcess_Params_init(GateProcess_Params *params);

/*
 *  ======== GateProcess_create ========
 */
GateProcess_Handle GateProcess_create(
        Ptr                             sharedAddr,
        const GateProcess_Params *      params);

/*
 *  ======== GateProcess_delete ========
 */
Int GateProcess_delete(GateProcess_Handle *pHandle);

/*
 *  ======== GateProcess_open ========
 */
GateProcess_Handle GateProcess_open(Ptr sharedAddr);

/*
 *  ======== GateProcess_close ========
 */
Int GateProcess_close(GateProcess_Handle *pHandle);

/*
 *  ======== GateProcess_shMemSize ========
 */
SizeT GateProcess_shMemSize(Void);


#if defined (__cplusplus)
}
#endif

#endif /* ti_syslink_utils_GateProcess */
