/*****************************************************************************/
/* vars.cpp v5.1.3                                                           */
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

Performs initialization of global variables used by the runtime.

*/

#define EXTERN /* empty */
#define VAR_INITIALIZERS 1

#include "basics.h"
#include "runtime.h"
/*** START TI REMOVE ***/
/* We do not need main.h to build TI RTS */
/*** END TI REMOVE ***/
#include "vec_newdel.h"
#include "eh.h"



/******************************************************************************
*                                                             \  ___  /       *
* Edison Design Group C++ Runtime                               /   \         *
*                                                            - | \^/ | -      *
* Copyright 1992-2011 Edison Design Group, Inc.                 \   /         *
* All rights reserved.  Consult your license                  /  | |  \       *
* regarding permissions and restrictions.                        [_]          *
*                                                                             *
******************************************************************************/
