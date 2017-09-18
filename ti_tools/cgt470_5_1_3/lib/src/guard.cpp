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

C++ functions to for one-time initialization guard variables.

*/

#include "basics.h"
#include "runtime.h"

#ifdef __EDG_IA64_ABI

/*
These functions are used by the front end only if
IA64_ABI_USE_GUARD_ACQUIRE_RELEASE is TRUE.
Note that as written these functions are suitable only for single
threaded implementations.  Multi-threaded implementations must
take additional precautions.
*/

#ifdef __EDG_IA64_ABI_USE_INT_STATIC_INIT_GUARD

/*
Use the ARM EABI semantics for guard variables (see 4.4.2 of the ARM EABI).
In particular, use the least significant bit to control object construction.
*/

#define ARM_OBJECT_SUCCESSFULLY_CONSTRUCTED  0x1
                              /* TRUE if the object guarded by this
                                 variable has been successfully constructed. */
#define ARM_UNUSED_BIT  0x2   /* Available for use by guard acquisition
                                 and release functions.  Unused in this
                                 implementation. */
#define ARM_GUARD_BIT_MASK (ARM_OBJECT_SUCCESSFULLY_CONSTRUCTED | \
                            ARM_UNUSED_BIT)

EXTERN_C int ABI_NAMESPACE::__cxa_guard_acquire(an_ia64_guard_ptr guard)
/*
If the guard variable indicates that the guarded variable has already
been initialized, return 0.  Otherwise, return 1.
*/
{
  int  initialize = FALSE;

  /* A full implementation would wait until the object is no longer
     under construction, but we only check once. */
  if ((*guard & ARM_OBJECT_SUCCESSFULLY_CONSTRUCTED) == 0) {
    initialize = TRUE;
  }  /* if */
  return initialize;
}  /* __cxa_guard_acquire */


EXTERN_C void ABI_NAMESPACE::__cxa_guard_release(an_ia64_guard_ptr guard)
/*
Called when the initialization of the guarded object is complete.
*/
{
  /* Set the guard variable to indicate that the initialization is complete.
     A multi-threaded implementation would then release the lock. */
  *guard |= ARM_OBJECT_SUCCESSFULLY_CONSTRUCTED;
}  /* __cxa_guard_release */


EXTERN_C void ABI_NAMESPACE::__cxa_guard_abort(an_ia64_guard_ptr guard)
/*
The initialization of the guarded object has been aborted due to an
exception being thrown.  Reset the guard so that the initialization 
will be tried again.
*/
{
  *guard &= ~ARM_GUARD_BIT_MASK;
}  /* __cxa_guard_abort */

#else /* !__EDG_IA64_ABI_USE_INT_STATIC_INIT_GUARD */

EXTERN_C int ABI_NAMESPACE::__cxa_guard_acquire(an_ia64_guard_ptr guard)
/*
If the guard variable indicates that the guarded variable has already
been initialized, return 0.  Otherwise, return 1.
*/
{
  char *first_byte = (char *)guard;
  int  initialize = FALSE;

  if (*first_byte == 0) {
    initialize = TRUE;
  }  /* if */
  return initialize;
}  /* __cxa_guard_acquire */


EXTERN_C void ABI_NAMESPACE::__cxa_guard_release(an_ia64_guard_ptr guard)
/*
Called when the initialization of the guarded object is complete.
*/
{
  /* Set the guard variable to indicate that the initialization is complete.
     A multi-threaded implementation would then release the lock. */
  char *first_byte = (char *)guard;
  *first_byte = 1;
}  /* __cxa_guard_release */


EXTERN_C void ABI_NAMESPACE::__cxa_guard_abort(an_ia64_guard_ptr guard)
/*
The initialization of the guarded object has been aborted due to an
exception being thrown.  Reset the guard so that the initialization 
will be tried again.
*/
{
  *(char *)guard = 0;
}  /* __cxa_guard_abort */

#endif /* __EDG_IA64_ABI_USE_INT_STATIC_INIT_GUARD */

#endif /* ifdef __EDG_IA64_ABI */

/******************************************************************************
*                                                             \  ___  /       *
* Edison Design Group C++ Runtime                               /   \         *
*                                                            - | \^/ | -      *
* Copyright 1992-2011 Edison Design Group, Inc.                 \   /         *
* All rights reserved.  Consult your license                  /  | |  \       *
* regarding permissions and restrictions.                        [_]          *
*                                                                             *
******************************************************************************/
