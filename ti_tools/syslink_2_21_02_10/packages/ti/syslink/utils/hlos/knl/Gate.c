/*
 *  @file   Gate.c
 *
 *  @brief      gate wrapper implementation
 *
 *
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




/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module level headers */
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/Gate.h>

#ifdef __QNX__

#include <sys/neutrino.h>
#include <pthread.h>

pthread_mutex_t staticMutex = PTHREAD_RMUTEX_INITIALIZER;

#else
/* Check to see if RT kernel is being used and use mutex instead of disbaling irq */
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif

#if defined (CONFIG_PREEMPT_RT)

#include <linux/mutex.h>

DEFINE_MUTEX(staticMutex);
struct task_struct *staticOwner = NULL;
int                 staticDepth = 0;

#endif /* defined (CONFIG_PREEMPT_RT) */

#endif

/* Structure defining internal object for the Gate Peterson.*/
struct Gate_Object {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
};

/* Function to enter a Gate */
IArg  Gate_enterSystem (void)
{
    unsigned long flags;

#ifndef __QNX__
#if defined (CONFIG_PREEMPT_RT)
  if( staticOwner != get_current() ) {
    mutex_lock_interruptible(&staticMutex);
    staticOwner = get_current();
  }
  staticDepth++;
#else
    local_irq_save (flags);
#endif /*!defined (CONFIG_PREEMPT_RT) */
#else
    int ret;

    ret = pthread_mutex_lock(&staticMutex);
    GT_assert(curTrace, (ret == 0));
    flags = 0;
#endif

    return (IArg)flags;
}


/* Function to leave a Gate */
Void Gate_leaveSystem (IArg key)
{
#ifndef __QNX__
#if defined (CONFIG_PREEMPT_RT)
  staticDepth--;
  if( !staticDepth ) {
    staticOwner = NULL;
    mutex_unlock(&staticMutex);
  }
#else
    local_irq_restore ((unsigned long) key);
#endif /* !defined (CONFIG_PREEMPT_RT) */
#else
    int ret;

    ret = pthread_mutex_unlock (&staticMutex);
    GT_assert(curTrace, (ret == 0));
#endif
}

/* Match with IGateProvider */
static inline IArg _Gate_enterSystem (struct Gate_Object * obj)
{
    (Void) obj;
    return Gate_enterSystem ();
}

/* Match with IGateProvider */
static inline Void _Gate_leaveSystem (struct Gate_Object * obj, IArg key)
{
    (Void) obj;
    Gate_leaveSystem (key);
}

struct Gate_Object Gate_systemObject =
{
    .enter = (IGateProvider_ENTER)_Gate_enterSystem,
    .leave = (IGateProvider_LEAVE)_Gate_leaveSystem,
};

IGateProvider_Handle Gate_systemHandle = (IGateProvider_Handle)&Gate_systemObject;
