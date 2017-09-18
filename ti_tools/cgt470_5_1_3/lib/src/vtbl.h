/*****************************************************************************/
/* vtbl.h v5.1.3                                                             */
/* Copyright (c) 1996-2013 Texas Instruments Inc., all rights reserved       */
/*****************************************************************************/
/******************************************************************************
*                                                             \  ___  /       *
* Edison Design Group C++ Runtime                               /   \         *
*                                                            - | \^/ | -      *
* Copyright 1992-2011 Edison Design Group, Inc.                 \   /         *
* All rights reserved.  Consult your license                  /  | |  \       *
* regarding permissions and restrictions.                        [_]          *
*                                                                             *
******************************************************************************/
/*

Virtual function table entry layout.

*/

#ifndef _VTBL_H
#define _VTBL_H

#ifndef __EDG_IA64_ABI
/*** START TI ADD ***/
struct a_vtbl_entry
{
   short   delta;      /* OFFSET TO GET TO THE COMPLETE OBJECT   */
   void  (*func)();    /* VIRTUAL FUNCTION TO BE CALLED          */
};
/*** END TI ADD ***/
/*** START TI REMOVE ***/
/*** END TI REMOVE ***/

#else /* defined(__EDG_IA64_ABI) */

typedef __EDG_IA64_VTABLE_ENTRY_TYPE a_vtbl_entry;

#endif /* defined(__EDG_IA64_ABI) */

typedef
/*** START TI ADD ***/
const
/*** END TI ADD ***/
a_vtbl_entry *a_vtbl_entry_ptr;

#endif /* ifndef _VTBL_H */


/******************************************************************************
*                                                             \  ___  /       *
* Edison Design Group C++ Runtime                               /   \         *
*                                                            - | \^/ | -      *
* Copyright 1992-2011 Edison Design Group, Inc.                 \   /         *
* All rights reserved.  Consult your license                  /  | |  \       *
* regarding permissions and restrictions.                        [_]          *
*                                                                             *
******************************************************************************/
