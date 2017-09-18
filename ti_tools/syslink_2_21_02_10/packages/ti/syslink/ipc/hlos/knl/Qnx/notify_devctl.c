/*
 *  @file   notify_devctl.c
 *
 *  @brief      OS-specific implementation of Notify driver for Qnx
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
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/GateSpinlock.h>

/* QNX specific header include */
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/proto.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* Module headers */
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/NotifyDrvDefs.h>
#include <ti/syslink/utils/OsalSemaphore.h>
#include <pthread.h>

/* Function prototypes */
int syslink_notify_sendevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_disable(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_restore(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_disableevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_enableevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_registereventsingle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_registerevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_unregistereventsingle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_unregisterevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_intlinregistered(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_eventavailable(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_threadattach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_threaddetach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/*!
 *  This function implements the callback registered with IPS. Here to pass
 *  event no. back to user function (so that it can do another level of
 *  demultiplexing of callbacks)
 */
Void _Notify_drvCallback (UInt16 procId,
                                 UInt16 lineId,
                                 UInt32 eventId,
                                 UArg   arg,
                                 UInt32 payload);

/* This function adds a data to a registered process. */
Int _NotifyDrv_addBufByPid (UInt16             procId,
                                   UInt16             lineId,
                                   UInt32             pid,
                                   UInt32             eventId,
                                   UInt32             data,
                                   Notify_FnNotifyCbck cbFxn,
                                   Ptr                param);

/* Attach a process to notify user support framework. */
Int NotifyDrv_attach (UInt32 pid);

/* Detach a process from notify user support framework. */
Int NotifyDrv_detach (UInt32 pid);
/* function to read driver memory */

/**/
/*Type definitions */ // TODO: need to move this to a header file
#define  MAX_PROCESSES          32u
#define  CACHE_NUM 10

typedef struct NotifyList {
	int index;
	int num_events;
	struct NotifyList *next;
	struct NotifyList *prev;
} NotifyList_t;

typedef struct WaitingReaders {
	int rcvid;
	struct WaitingReaders *next;
} WaitingReaders_t;

static NotifyList_t *nl_cache;
static int num_nl=0;
static WaitingReaders_t *wr_cache;
static int num_wr=0;

// Instead of constantly allocating and freeing the notifier structures
// we just cache a few of them, and recycle them instead.
// The cache count is set with CACHE_NUM above.

static NotifyList_t *get_nl()
{
	NotifyList_t *item;
	item = nl_cache;
	if (item != NULL) {
		nl_cache = nl_cache->next;
		num_nl--;
	} else {
		item = Memory_alloc(NULL, sizeof(NotifyList_t), 0, NULL);
	}
	return(item);
}

static void put_nl(NotifyList_t *item)
{
	if (num_nl >= CACHE_NUM) {
		Memory_free(NULL, item, sizeof(*item));
	} else {
		item->next = nl_cache;
		nl_cache = item;
		num_nl++;
	}
	return;
}

static WaitingReaders_t *get_wr()
{
	WaitingReaders_t *item;
	item = wr_cache;
	if (item != NULL) {
		wr_cache = wr_cache->next;
		num_wr--;
	} else {
		item = Memory_alloc(NULL, sizeof(WaitingReaders_t), 0, NULL);
	}
	return(item);
}

static void put_wr(WaitingReaders_t *item)
{
	if (num_wr >= CACHE_NUM) {
		Memory_free(NULL, item, sizeof(*item));
	} else {
		item->next = wr_cache;
		wr_cache = item;
		num_wr++;
	}
	return;
}

/*!
 *  @brief  Structure of Event callback argument passed to register fucntion.
 */
typedef struct NotifyDrv_EventCbck_tag {
    List_Elem          element;
    /*!< List element header */
    UInt16             procId;
    /*!< Processor identifier */
    UInt16             lineId;
    /*!< line identifier */
    UInt32             eventId;
    /*!< Event identifier */
    Notify_FnNotifyCbck func;
    /*!< Callback function for the event. */
    Ptr                param;
    /*!< User callback argument. */
    UInt32             pid;
    /*!< Process Identifier for user process. */
} NotifyDrv_EventCbck ;

/*!
 *  @brief  Keeps the information related to Event.
 */
typedef struct NotifyDrv_EventState_tag {
    List_Handle            bufList;
    /*!< Head of received event list. */
    UInt32                 pid;
    /*!< User process ID. */
    UInt32                 refCount;
    /*!< Reference count, used when multiple Notify_registerEvent are called
         from same process space (multi threads/processes). */
    WaitingReaders_t *head;
    WaitingReaders_t *tail;
} NotifyDrv_EventState;

/*!
 *  @brief  NotifyDrv Module state object
 */
typedef struct NotifyDrv_ModuleObject_tag {
    Bool                 isSetup;
    /*!< Indicates whether the module has been already setup */
    Bool                 openRefCount;
    /*!< Open reference count. */
    IGateProvider_Handle gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    List_Handle          eventCbckList;
    /*!< List containg callback arguments for all registered handlers from
         user mode. */
    List_Handle          singleEventCbckList;
    /*!< List containg callback arguments for all registered handlers from
         user mode for 'single' registrations. */
    NotifyDrv_EventState eventState [MAX_PROCESSES];
    /*!< List for all user processes registered. */
    pthread_t nt; // notifier thread
    pthread_mutex_t lock;      // protection between notifier and event
    pthread_cond_t  cond;
    NotifyList_t *head;
    NotifyList_t *tail;
    int run; // notifier thread must keep running
} NotifyDrv_ModuleObject;

/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @var    Notify_state
 *
 *  @brief  Notify state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
NotifyDrv_ModuleObject NotifyDrv_state =
{
	.gateHandle = NULL,
	.isSetup = FALSE,
	.eventCbckList = NULL,
	.singleEventCbckList = NULL,
	.openRefCount = 0,
	.nt = 0,
	.lock = PTHREAD_MUTEX_INITIALIZER,
	.cond = PTHREAD_COND_INITIALIZER,
	.head = NULL,
	.tail = NULL,
	.run  = 0
};

static NotifyList_t *find_nl(int index)
{
	NotifyList_t *item=NULL;
	item = NotifyDrv_state.head;
	while (item) {
		if (item->index == index)
			return(item);
		item = item->next;
	}
	return(item);
}

/* we have the right locks when calling this function */
static int enqueue_notify_list(int index)
{
	NotifyList_t *item;
	item = find_nl(index);
	if (item == NULL) {
		item = get_nl();
		if (item == NULL) {
			return(-1);
		}
		item->next = NULL;
		item->index = index;
		item->num_events=1;
		if (NotifyDrv_state.head == NULL) {
			NotifyDrv_state.head = item;
			NotifyDrv_state.tail = item;
			item->prev = NULL;
		}
		else {
			item->prev = NotifyDrv_state.tail;
			NotifyDrv_state.tail->next = item;
			NotifyDrv_state.tail = item;
		}
	}
	else {
		item->num_events++;
	}
	return(0);
}

/* we have the right locks when calling this function */
static inline int dequeue_notify_list_item(NotifyList_t *item)
{
	int index;
	if (item == NULL) {
		return(-1);
	}
	index = item->index;
	item->num_events--;
	if (item->num_events > 0) {
		return(index);
	}
	if (NotifyDrv_state.head == item) {
		// removing head
		NotifyDrv_state.head = item->next;
		if (NotifyDrv_state.head != NULL) {
			NotifyDrv_state.head->prev = NULL;
		}
		else {
			// removing head and tail
			NotifyDrv_state.tail = NULL;
		}
	}
	else {
		item->prev->next = item->next;
		if (item->next != NULL) {
			item->next->prev = item->prev;
		}
		else {
			// removing tail
			NotifyDrv_state.tail = item->prev;
		}
	}
	put_nl(item);
	return(index);
}

/* we have the right locks when calling this function */
static int enqueue_waiting_reader(int index, int rcvid)
{
	WaitingReaders_t *item;
	item = get_wr();
	if (item == NULL) {
		return(-1);
	}
	item->rcvid = rcvid;
	item->next = NULL;
	if (NotifyDrv_state.eventState [index].head == NULL) {
		NotifyDrv_state.eventState [index].head = item;
		NotifyDrv_state.eventState [index].tail = item;
	}
	else {
		NotifyDrv_state.eventState [index].tail->next = item;
		NotifyDrv_state.eventState [index].tail = item;
	}
	return(EOK);
}

/* we have the right locks when calling this function */
/* caller frees item */
static WaitingReaders_t *dequeue_waiting_reader(int index)
{
	WaitingReaders_t *item = NULL;
	if (NotifyDrv_state.eventState [index].head) {
		item = NotifyDrv_state.eventState [index].head;
		NotifyDrv_state.eventState [index].head = NotifyDrv_state.eventState [index].head->next;
		if (NotifyDrv_state.eventState [index].head == NULL) {
			NotifyDrv_state.eventState [index].tail = NULL;
		}
	}
	return(item);
}

static int find_available_reader_and_event(int *index, WaitingReaders_t **item)
{
	NotifyList_t *temp;
	if (NotifyDrv_state.head == NULL) {
		return(0);
	}
	temp = NotifyDrv_state.head;
	while (temp) {
		if (NotifyDrv_state.eventState [temp->index].head) {
			// event and reader found
			if (dequeue_notify_list_item(temp) >= 0) {
				*index = temp->index;
				*item = dequeue_waiting_reader(temp->index);
			}
			return(1);
		}
		temp = temp->next;
	}
	return(0);
}

static void deliver_notification(int index, int rcvid)
{
    Int                     status;
    NotifyDrv_EventPacket * uBuf     = NULL;
    IArg                    key;
    List_Object *           bufList   = NULL;
    List_Handle             list;

    GT_assert(curTrace, (index >= 0));

    list = NotifyDrv_state.eventState[index].bufList;
    uBuf = (NotifyDrv_EventPacket *)List_get(list);

    /*  Let the check remain at run-time. */
    if (uBuf != NULL) {
        status = MsgReply_r(rcvid, sizeof(*uBuf), uBuf, sizeof(*uBuf));
        GT_assert(curTrace, (status >= 0));

        /* Start tearing down event bufList if termination packet */
        if (uBuf->isExit == TRUE) {
            key = IGateProvider_enter (NotifyDrv_state.gateHandle);
            /* Last client being unregistered for this process. */
            NotifyDrv_state.eventState [index].pid = -1;

            /* Store in local variable to delete outside lock. */
            bufList = NotifyDrv_state.eventState [index].bufList;

            NotifyDrv_state.eventState [index].bufList = NULL;

            IGateProvider_leave (NotifyDrv_state.gateHandle, key);

            /* Last client being unregistered with Notify module. */
            List_delete (&bufList);
        }

        /* Free the processed event callback packet. */
        Memory_free (NULL, uBuf, sizeof(*uBuf));
    }
    else {
        MsgReply(rcvid, EOK, NULL, 0);
    }

    return;
}

static void *notifier_thread(void *arg)
{
    int status;
    int index = -1;
    WaitingReaders_t *item = NULL;
    Int ret;

    ret = pthread_mutex_lock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));

    while (NotifyDrv_state.run) {
        status = find_available_reader_and_event(&index, &item);
        if (status == 0) {
            status = pthread_cond_wait(&NotifyDrv_state.cond,
                &NotifyDrv_state.lock);
            if ((status != EOK) && (status != EINTR)) {
                // false wakeup
                break;
            }
            status = find_available_reader_and_event(&index, &item);
            if (status == 0) {
                continue;
            }
        }
        ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
        GT_assert (curTrace, (ret == 0));
        // we have unlocked, and now we have an event to deliver
        // we deliver one event at a time, relock, check and continue
        GT_assert(curTrace, (item != NULL));
        deliver_notification(index, item->rcvid);
        ret = pthread_mutex_lock(&NotifyDrv_state.lock);
        GT_assert (curTrace, (ret == 0));
        put_wr(item);
    }

    ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));
    return(NULL);
}

/*!
 *  @brief  Module setup function.
 *
 *  @sa     _NotifyDrv_setup
 */
Void
_NotifyDrv_setup (Void)
{
    UInt16 i;
    List_Params  listparams;
    Error_Block                    eb = 0;

    GT_0trace (curTrace, GT_ENTER, "_NotifyDrv_setup");

    List_Params_init (&listparams);
    NotifyDrv_state.eventCbckList = List_create (&listparams, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.eventCbckList == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NotifyDrv_setup",
                             Notify_E_FAIL,
                             "Failed to create event callback list!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        NotifyDrv_state.singleEventCbckList = List_create (&listparams, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NotifyDrv_state.singleEventCbckList == NULL) {
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NotifyDrv_setup",
                             Notify_E_FAIL,
                             "Failed to create single event callback list!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            NotifyDrv_state.gateHandle = (IGateProvider_Handle)
						GateSpinlock_create ((GateSpinlock_Handle) NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NotifyDrv_state.gateHandle == NULL) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_NotifyDrv_setup",
                                     Notify_E_FAIL,
                                     "Failed to create spinlock gate!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                for (i = 0 ; i < MAX_PROCESSES ; i++) {
                    NotifyDrv_state.eventState [i].bufList = NULL;
                    NotifyDrv_state.eventState [i].pid = -1;
                    NotifyDrv_state.eventState [i].refCount = 0;
                    NotifyDrv_state.eventState [i].head = NULL;
                    NotifyDrv_state.eventState [i].tail = NULL;
                }
								NotifyDrv_state.run = TRUE;
								if (pthread_create(&NotifyDrv_state.nt, NULL, notifier_thread, NULL) == EOK) {
                	NotifyDrv_state.isSetup = TRUE;
							}
							else {
								NotifyDrv_state.run = FALSE;
							}
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "_NotifyDrv_setup");
}


/*!
 *  @brief  Module destroy function.
 *
 *  @sa     _NotifyDrv_setup
 */
Void
_NotifyDrv_destroy (Void)
{
    NotifyDrv_EventPacket * packet;
    NotifyDrv_EventCbck *   cbck;
    UInt32                  i;
    Int                     ret;

    GT_0trace (curTrace, GT_ENTER, "_NotifyDrv_destroy");

    for (i = 0 ; i < MAX_PROCESSES ; i++) {
        NotifyDrv_state.eventState [i].bufList = NULL;
        NotifyDrv_state.eventState [i].pid = -1;
        NotifyDrv_state.eventState [i].refCount = 0;
        if (NotifyDrv_state.eventState [i].bufList != NULL) {
            /* Free event packets for any received but unprocessed events. */
            while (List_empty (NotifyDrv_state.eventState [i].bufList) != TRUE){
                packet = (NotifyDrv_EventPacket *)
                              List_get (NotifyDrv_state.eventState [i].bufList);
                if (packet != NULL){
                    Memory_free (NULL, packet, sizeof(*packet));
                }
            }
            List_delete (&(NotifyDrv_state.eventState [i].bufList));
        }
    }

    /* Clear any event registrations that were not unregistered. */
    if (NotifyDrv_state.eventCbckList != NULL) {
        while (List_empty (NotifyDrv_state.eventCbckList) != TRUE) {
            cbck = (NotifyDrv_EventCbck *)
                                    List_get (NotifyDrv_state.eventCbckList);
            if (cbck != NULL){
                Memory_free (NULL,
                             cbck,
                             sizeof (NotifyDrv_EventCbck));
            }
        }
        List_delete (&(NotifyDrv_state.eventCbckList));
    }

    /* Clear any event registrations that were not unregistered from single
     * list.
     */
    if (NotifyDrv_state.singleEventCbckList != NULL) {
        while (List_empty (NotifyDrv_state.singleEventCbckList) != TRUE) {
            cbck = (NotifyDrv_EventCbck *)
                                List_get (NotifyDrv_state.singleEventCbckList);
            if (cbck != NULL){
                Memory_free (NULL,
                             cbck,
                             sizeof (NotifyDrv_EventCbck));
            }
        }
        List_delete (&(NotifyDrv_state.singleEventCbckList));
    }

    if (NotifyDrv_state.gateHandle != NULL) {
        GateSpinlock_delete ((GateSpinlock_Handle *)
                                       &(NotifyDrv_state.gateHandle));
    }

    NotifyDrv_state.isSetup = FALSE ;
	NotifyDrv_state.run = FALSE;
	// run through and destroy the thread, and all outstanding
	// notify structures
    ret = pthread_mutex_lock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));
	pthread_cond_signal(&NotifyDrv_state.cond);
	ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));
	pthread_join(NotifyDrv_state.nt, NULL);
	ret = pthread_mutex_lock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));
	while (NotifyDrv_state.head != NULL) {
		int index;
		WaitingReaders_t *item;
		index = dequeue_notify_list_item(NotifyDrv_state.head);
		if (index < 0)
			break;
		item = dequeue_waiting_reader(index);
		while (item) {
			put_wr(item);
			item = dequeue_waiting_reader(index);
		}
	}
    NotifyDrv_state.head = NULL ;
    NotifyDrv_state.tail = NULL ;
    ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
    GT_assert (curTrace, (ret == 0));

    GT_0trace (curTrace, GT_LEAVE, "_NotifyDrv_destroy");
}


/**
 * Handler for devctl() messages for notify module.
 *
 * Handles special devctl() messages that we export for control.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	switch (msg->i.dcmd)
	{

	 			case DCMD_NOTIFY_SENDEVENT:
				 {
					return syslink_notify_sendevent( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_DISABLE:
				 {
					return syslink_notify_disable( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_RESTORE:
				 {
					return syslink_notify_restore( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_DISABLEEVENT:
				 {
					return syslink_notify_disableevent( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_ENABLEEVENT:
				 {
					return syslink_notify_enableevent( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_REGISTEREVENTSINGLE:
				 {
					return syslink_notify_registereventsingle( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_REGISTEREVENT:
				 {
					return syslink_notify_registerevent( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_UNREGISTEREVENTSINGLE:
				 {
					return syslink_notify_unregistereventsingle( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_UNREGISTEREVENT:
				 {
					return syslink_notify_unregisterevent( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_GETCONFIG:
				 {
					return syslink_notify_getconfig( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_SETUP:
				 {
					return syslink_notify_setup( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_DESTROY:
				 {
					return syslink_notify_destroy( ctp, msg, ocb);
				 }
				 break;
				 case DCMD_NOTIFY_ATTACH:
				 {
					return syslink_notify_attach( ctp, msg, ocb);
				 }
				 break;
				 case DCMD_NOTIFY_DETACH:
				 {
					return syslink_notify_detach( ctp, msg, ocb);
				 }
				 break;
				 case DCMD_NOTIFY_SHAREDMEMREQ:
				 {
					return syslink_notify_sharedmemreq( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_INTLINEREGISTERED:
				 {
					return syslink_notify_intlinregistered( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_EVENTAVAILABLE:
				 {
					return syslink_notify_eventavailable( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_THREADATTACH:
				 {
					return syslink_notify_threadattach( ctp, msg, ocb);
				 }
				 break;

				 case DCMD_NOTIFY_THREADDETACH:
				 {
					return syslink_notify_threaddetach( ctp, msg, ocb);
				 }
				 break;

	 default:
		fprintf( stderr, "Invalid DEVCTL for notify 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (Notify_CmdArgsDisable)));

}

/**
 * Handler for notify send event API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_sendevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsSendEvent *		cargs = (Notify_CmdArgsSendEvent *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsSendEvent *		out  = (Notify_CmdArgsSendEvent *) (_DEVCTL_DATA (msg->o));

    out->commonArgs.apiStatus = Notify_sendEvent (cargs->procId,
                                           cargs->lineId,
                                           cargs->eventId,
                                           cargs->payload,
                                           cargs->waitClear);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                /* Notify_E_DRIVERINIT and Notify_E_NOTREADY are run-time
                 * failures.
                 */
                if (   (out->commonArgs.apiStatus < 0)
                    && (out->commonArgs.apiStatus != Notify_E_NOTINITIALIZED)
                    && (out->commonArgs.apiStatus != Notify_E_EVTNOTREGISTERED)
                    && (out->commonArgs.apiStatus != Notify_E_EVTDISABLED)) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifyDrv_ioctl",
                                         out->commonArgs.apiStatus,
                                         "Kernel-side Notify_sendEvent failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


            /* copy_to_user is not needed for Notify_sendEvent, since nothing
             * is to be returned back.
             */

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsSendEvent)));

}

/**
 * Handler for notify disable API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_disable(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsDisable *		cargs = (Notify_CmdArgsDisable *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsDisable *		out  = (Notify_CmdArgsDisable *) (_DEVCTL_DATA (msg->o));

    out->flags = Notify_disable (cargs->procId, cargs->lineId);
    cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsDisable)));

}

/**
 * Handler for notify restore API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_restore(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsRestore *		cargs = (Notify_CmdArgsRestore *) (_DEVCTL_DATA (msg->i));
	//Notify_CmdArgsRestore *		out  = (Notify_CmdArgsRestore *) (_DEVCTL_DATA (msg->o));

	Notify_restore (cargs->procId, cargs->lineId, cargs->key);
	cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsRestore)));

}

/**
 * Handler for notify disableevent API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_disableevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsDisableEvent *		cargs = (Notify_CmdArgsDisableEvent *) (_DEVCTL_DATA (msg->i));
	//Notify_CmdArgsDisableEvent *		out  = (Notify_CmdArgsDisableEvent *) (_DEVCTL_DATA (msg->o));

	Notify_disableEvent (cargs->procId,
										 cargs->lineId,
										 cargs->eventId);
	cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsDisableEvent)));
}

/**
 * Handler for notify enable event API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_enableevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsEnableEvent *		cargs = (Notify_CmdArgsEnableEvent *) (_DEVCTL_DATA (msg->i));
	//Notify_CmdArgsEnableEvent *		out  = (Notify_CmdArgsEnableEvent *) (_DEVCTL_DATA (msg->o));

    Notify_enableEvent (cargs->procId,
                                    cargs->lineId,
                                    cargs->eventId);
    cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsEnableEvent)));


}

/**
 * Handler for notify register event single  API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_registereventsingle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsRegisterEvent *		cargs = (Notify_CmdArgsRegisterEvent *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsRegisterEvent *		out  = (Notify_CmdArgsRegisterEvent *) (_DEVCTL_DATA (msg->o));
	NotifyDrv_EventCbck *		cbck;

    cbck = Memory_alloc (NULL,
                                     sizeof (NotifyDrv_EventCbck),
                                     0u,
                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (cbck == NULL) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    out->commonArgs.apiStatus = Notify_E_MEMORY ;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "NotifyDrv_ioctl",
                                        out->commonArgs.apiStatus,
                                        "Memory_alloc failed for event "
                                        "registration structure");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    cbck->procId = cargs->procId;
                    cbck->lineId = cargs->lineId;
                    cbck->eventId = cargs->eventId;
                    cbck->func   = cargs->fnNotifyCbck;
                    cbck->param  = cargs->cbckArg;
                    cbck->pid    = cargs->pid;
                    out->commonArgs.apiStatus = Notify_registerEventSingle (cargs->procId,
                                                         cargs->lineId,
                                                         cargs->eventId,
                                                        _Notify_drvCallback,
                                                        (UArg) cbck);
                    /* This check is needed at run-time also to propagate the
                     * status to user-side. This must not be optimized out.
                     */
                    if (out->commonArgs.apiStatus < 0) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        Memory_free (NULL, cbck, sizeof (NotifyDrv_EventCbck));
                        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDrv_ioctl",
                             out->commonArgs.apiStatus,
                             "Kernel-side Notify_registerEventSingle failed");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                    else {
                        /* Add the cbck to the event cbck list */
                        List_elemClear (&(cbck->element));
                        List_put (NotifyDrv_state.singleEventCbckList,
                                  &(cbck->element));
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


            /* copy_to_user is not needed for Notify_registerEvent, since
             * nothing is to be returned back.
             */
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsRegisterEvent)));


}


/**
 * Handler for notify register event API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_registerevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsRegisterEvent *		cargs = (Notify_CmdArgsRegisterEvent *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsRegisterEvent *		out  = (Notify_CmdArgsRegisterEvent *) (_DEVCTL_DATA (msg->o));

            NotifyDrv_EventCbck *       cbck;


                cbck = Memory_alloc (NULL,
                                     sizeof (NotifyDrv_EventCbck),
                                     0u,
                                     NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (cbck == NULL) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    out->commonArgs.apiStatus = Notify_E_MEMORY ;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "NotifyDrv_ioctl",
                                        out->commonArgs.apiStatus,
                                        "Memory_alloc failed for event "
                                        "registration structure");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    cbck->procId = cargs->procId;
                    cbck->lineId = cargs->lineId;
                    cbck->eventId = cargs->eventId;
                    cbck->func   = cargs->fnNotifyCbck;
                    cbck->param  = cargs->cbckArg;
                    cbck->pid    = cargs->pid;
                    out->commonArgs.apiStatus = Notify_registerEvent (cargs->procId,
                                                   cargs->lineId,
                                                   cargs->eventId,
                                                   _Notify_drvCallback,
                                                   (UArg) cbck);
                    /* This check is needed at run-time also to propagate the
                     * status to user-side. This must not be optimized out.
                     */
                    if (out->commonArgs.apiStatus < 0) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        Memory_free (NULL, cbck, sizeof (NotifyDrv_EventCbck));
                        GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDrv_ioctl",
                                     out->commonArgs.apiStatus,
                                     "Kernel-side Notify_registerEvent failed");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                    else {
                        /* Add the cbck to the event cbck list */
                        List_elemClear (&(cbck->element));
                        List_put (NotifyDrv_state.eventCbckList,
                                  &(cbck->element));
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


            /* copy_to_user is not needed for Notify_registerEvent, since
             * nothing is to be returned back.
             */
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsRegisterEvent)));

}

/**
 * Handler for notify unregister event single API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_unregistereventsingle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsUnregisterEvent *		cargs = (Notify_CmdArgsUnregisterEvent *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsUnregisterEvent *		out  = (Notify_CmdArgsUnregisterEvent *) (_DEVCTL_DATA (msg->o));

				Bool						  found = FALSE;
				UInt32						  pid;
				List_Elem * 				  cbck	= NULL;
				IArg						  key;

					key = IGateProvider_enter (NotifyDrv_state.gateHandle) ;
					pid = cargs->pid;
					List_traverse (cbck, NotifyDrv_state.singleEventCbckList) {
						GT_assert (curTrace, (cbck != NULL));
						if (	(((NotifyDrv_EventCbck *) cbck)->procId ==
																	 cargs->procId)
							&&	(((NotifyDrv_EventCbck *) cbck)->lineId ==
																	 cargs->lineId)
							&&	(((NotifyDrv_EventCbck *) cbck)->eventId ==
																	cargs->eventId)
							&&	(((NotifyDrv_EventCbck *) cbck)->pid == pid)) {
							found = TRUE;
							break;
						}
					}
					IGateProvider_leave (NotifyDrv_state.gateHandle, key);

					if (found == FALSE) {
						/* This does not impact return status of this function,
						 * so retVal comment is not used.
						 */
						out->commonArgs.apiStatus = Notify_E_NOTFOUND;
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Could not find the registered event");
					}
					else {
						out->commonArgs.apiStatus = Notify_unregisterEventSingle(cargs->procId,
															  cargs->lineId,
															  cargs->eventId);
						/* This check is needed at run-time also to propagate the
						 * status to user-side. This must not be optimized out.
						 */
						if (out->commonArgs.apiStatus < 0) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
							/* This does not impact return status of this function,
							 * so retVal comment is not used.
							 */
							GT_setFailureReason (curTrace,
												GT_4CLASS,
												"NotifyDrv_ioctl",
												out->commonArgs.apiStatus,
												"Kernel-side Notify_unregisterEvent"
												" failed");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
						}
						else {
							key = IGateProvider_enter (NotifyDrv_state.gateHandle);
							List_remove (NotifyDrv_state.singleEventCbckList, cbck);
							IGateProvider_leave (NotifyDrv_state.gateHandle, key);
							Memory_free (NULL, cbck, sizeof (NotifyDrv_EventCbck));
						}
					}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsUnregisterEvent)));

}

/**
 * Handler for notify unregister event API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_unregisterevent(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsUnregisterEvent *		cargs = (Notify_CmdArgsUnregisterEvent *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsUnregisterEvent *		out  = (Notify_CmdArgsUnregisterEvent *) (_DEVCTL_DATA (msg->o));

			Bool                          found = FALSE;
            UInt32                        pid;
            List_Elem *                   cbck  = NULL;
            IArg                          key;

                key  = IGateProvider_enter (NotifyDrv_state.gateHandle) ;
                pid = cargs->pid;
                List_traverse (cbck, NotifyDrv_state.eventCbckList) {
                    GT_assert (curTrace, (cbck != NULL));
                    if (    (((NotifyDrv_EventCbck *) cbck)->func ==
                                                          cargs->fnNotifyCbck)
                        &&  (((NotifyDrv_EventCbck *) cbck)->param ==
                                                                cargs->cbckArg)
                        &&  (((NotifyDrv_EventCbck *) cbck)->procId ==
                                                                 cargs->procId)
                        &&  (((NotifyDrv_EventCbck *) cbck)->lineId ==
                                                                 cargs->lineId)
                        &&  (((NotifyDrv_EventCbck *) cbck)->eventId ==
                                                                cargs->eventId)
                        &&  (((NotifyDrv_EventCbck *) cbck)->pid == pid)) {
                        found = TRUE;
                        break;
                    }
                }
                IGateProvider_leave (NotifyDrv_state.gateHandle, key);

                if (found == FALSE) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    out->commonArgs.apiStatus = Notify_E_NOTFOUND;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifyDrv_ioctl",
                                         out->commonArgs.apiStatus,
                                         "Could not find the registered event");
                }
                else {
                    out->commonArgs.apiStatus = Notify_unregisterEvent (cargs->procId,
                                                     cargs->lineId,
                                                     cargs->eventId,
                                                     _Notify_drvCallback,
                                                     (Ptr) cbck);
                    /* This check is needed at run-time also to propagate the
                     * status to user-side. This must not be optimized out.
                     */
                    if (out->commonArgs.apiStatus < 0) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "NotifyDrv_ioctl",
                                            out->commonArgs.apiStatus,
                                            "Kernel-side Notify_unregisterEvent"
                                            " failed");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                    else {
                        key = IGateProvider_enter (NotifyDrv_state.gateHandle);
                        List_remove (NotifyDrv_state.eventCbckList, cbck);
                        IGateProvider_leave (NotifyDrv_state.gateHandle, key);
                        Memory_free (NULL, cbck, sizeof (NotifyDrv_EventCbck));
                    }
                }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsUnregisterEvent)));
}

/**
 * Handler for notify get config API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


		Notify_CmdArgsGetConfig *		cargs = (Notify_CmdArgsGetConfig *) (_DEVCTL_DATA (msg->i));
		//Notify_CmdArgsGetConfig *		out  = (Notify_CmdArgsGetConfig *) (_DEVCTL_DATA (msg->o));

        Notify_Config             *cfg = (Notify_Config *)(cargs+1);
        Notify_getConfig (cfg);
	cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

		SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(Notify_CmdArgsGetConfig));
		SETIOV(&ctp->iov[1], cfg, sizeof(Notify_Config));

	return _RESMGR_NPARTS(2);

}

/**
 * Handler for notify setup API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsSetup *		cargs = (Notify_CmdArgsSetup *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsSetup *		out  = (Notify_CmdArgsSetup *) (_DEVCTL_DATA (msg->o));

    Notify_Config         *cfg = (Notify_Config*)(cargs+1);

       out->commonArgs.apiStatus = Notify_setup (cfg);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (out->commonArgs.apiStatus < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifyDrv_ioctl",
                             out->commonArgs.apiStatus,
                                         "Kernel-side Notify_setup failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    if (out->commonArgs.apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_NOTIFY_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsSetup)));

}

/**
 * Handler for notify destroy API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	Notify_CmdArgsDestroy *		out  = (Notify_CmdArgsDestroy *) (_DEVCTL_DATA (msg->o));

	out->commonArgs.apiStatus = Notify_destroy ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
				if (out->commonArgs.apiStatus < 0) {
					GT_setFailureReason (curTrace,
										 GT_4CLASS,
										 "NotifyDrv_ioctl",
										 out->commonArgs.apiStatus,
										 "Kernel-side Notify_destroy failed");
				}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    if (out->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_NOTIFY_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsDestroy)));

}

/**
 * Handler for notify attach API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsAttach *		cargs = (Notify_CmdArgsAttach *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsAttach *		out  = (Notify_CmdArgsAttach *) (_DEVCTL_DATA (msg->o));

	Ptr 				 knlsharedAddr;


	knlsharedAddr = Memory_translate (cargs->sharedAddr,
											   Memory_XltFlags_Phys2Virt);

	out->commonArgs.apiStatus = Notify_attach (cargs->procId,
											knlsharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
					if (out->commonArgs.apiStatus < 0) {
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Kernel-side Notify_attach failed");
					}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsAttach)));
}

/**
 * Handler for notify detach API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsDetach *		cargs = (Notify_CmdArgsDetach *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsDetach *		out  = (Notify_CmdArgsDetach *) (_DEVCTL_DATA (msg->o));

	out->commonArgs.apiStatus = Notify_detach (cargs->procId);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
					if (out->commonArgs.apiStatus < 0) {
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Kernel-side Notify_detach failed");
					}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */


	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsDetach)));
}

/**
 * Handler for notify shared mem req API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsSharedMemReq *		cargs = (Notify_CmdArgsSharedMemReq *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsSharedMemReq *		out  = (Notify_CmdArgsSharedMemReq *) (_DEVCTL_DATA (msg->o));

				Ptr 					   knlsharedAddr;

				knlsharedAddr  = Memory_translate (cargs->sharedAddr,
											   Memory_XltFlags_Phys2Virt);
				out->commonArgs.apiStatus = Notify_sharedMemReq (cargs->procId,
												  knlsharedAddr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
					if (out->commonArgs.apiStatus < 0) {
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Kernel-side Notify_sharedMemReq failed");
					}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsSharedMemReq)));
}

/**
 * Handler for notify int lin registered API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_intlinregistered(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsIntLineRegistered *		cargs = (Notify_CmdArgsIntLineRegistered *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsIntLineRegistered *		out  = (Notify_CmdArgsIntLineRegistered *) (_DEVCTL_DATA (msg->o));

	out->isRegistered = Notify_intLineRegistered (cargs->procId,
															   cargs->lineId);
	cargs->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsIntLineRegistered)));
}

/**
 * Handler for notify event available API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_eventavailable(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsEventAvailable *		cargs = (Notify_CmdArgsEventAvailable *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsEventAvailable *		out  = (Notify_CmdArgsEventAvailable *) (_DEVCTL_DATA (msg->o));

	out->available = Notify_eventAvailable (cargs->procId,
															   cargs->lineId,
															   cargs->eventId);
	out->commonArgs.apiStatus = Notify_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsEventAvailable)));
}

/**
 * Handler for notify thread attach API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_threadattach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsThreadAttach *		cargs = (Notify_CmdArgsThreadAttach *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsThreadAttach *		out  = (Notify_CmdArgsThreadAttach *) (_DEVCTL_DATA (msg->o));

					out->commonArgs.apiStatus = NotifyDrv_attach (cargs->pid);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
					if (out->commonArgs.apiStatus < 0) {
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Kernel-side NotifyDrv_attach failed");
					}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    if (out->commonArgs.apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_NOTIFY_THREADDETACH, (int)cargs->pid, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsThreadAttach)));
}

/**
 * Handler for notify thread detach API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_notify_threaddetach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	Notify_CmdArgsThreadDetach *		cargs = (Notify_CmdArgsThreadDetach *) (_DEVCTL_DATA (msg->i));
	Notify_CmdArgsThreadDetach *		out  = (Notify_CmdArgsThreadDetach *) (_DEVCTL_DATA (msg->o));

					out->commonArgs.apiStatus = NotifyDrv_detach (cargs->pid);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
					if (out->commonArgs.apiStatus < 0) {
						GT_setFailureReason (curTrace,
											 GT_4CLASS,
											 "NotifyDrv_ioctl",
											 out->commonArgs.apiStatus,
											 "Kernel-side NotifyDrv_detach failed");
					}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    if (out->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_NOTIFY_THREADDETACH, (int)cargs->pid, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(Notify_CmdArgsDisable)));
}


/** ============================================================================
 *  Internal functions
 *  ============================================================================
 */
/*!
 *  @brief      This function implements the callback registered with IPS. Here
 *              to pass event no. back to user function (so that it can do
 *              another level of demultiplexing of callbacks)
 *
 *  @param      procId    processor Id from which interrupt is received
 *  @param      lineId    Interrupt line ID to be used
 *  @param      eventId   eventId registered
 *  @param      arg       argument to call back
 *  @param      payload   payload received
 *
 *  @sa
 */
Void
_Notify_drvCallback (UInt16 procId,
                     UInt16 lineId,
                     UInt32 eventId,
                     UArg   arg,
                     UInt32 payload)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32                   status = 0;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    NotifyDrv_EventCbck *   cbck;

    GT_4trace (curTrace,
               GT_ENTER,
               "_Notify_drvCallback",
               procId,
               eventId,
               arg,
               payload);

    GT_assert (curTrace, (NotifyDrv_state.isSetup == TRUE));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.isSetup == FALSE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Notify_drvCallback",
                             Notify_E_FAIL,
                             "The Notify OS driver was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cbck = (NotifyDrv_EventCbck *) arg;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        _NotifyDrv_addBufByPid (procId,
                                lineId,
                                cbck->pid,
                                eventId,
                                payload,
                                cbck->func,
                                cbck->param);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Notify_drvCallback",
                                 Notify_E_MEMORY,
                                 "Failed to add callback packet for pid");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "_Notify_drvCallback");
}


/*!
 *  @brief      This function adds a data to a registered process.
 *
 *  @param      pid       Process ID of current process
 *  @param      eventId   eventId registered
 *  @param      data      Data to be added
 *  @param      cbFxn     Callback function
 *  @param      param     Parameter associated with callback function.
 *
 *  @sa
 */
Int
_NotifyDrv_addBufByPid (UInt16             procId,
                        UInt16             lineId,
                        UInt32             pid,
                        UInt32             eventId,
                        UInt32             data,
                        Notify_FnNotifyCbck cbFxn,
                        Ptr                param)
{
    Int32                   status = 0;
    Bool                    flag   = FALSE;
    Bool                    isExit = FALSE;
    NotifyDrv_EventPacket * uBuf   = NULL;
    IArg                    key;
    UInt32                  i;
    WaitingReaders_t *      item;
    Int                     ret;

    GT_5trace (curTrace,
               GT_ENTER,
               "_NotifyDrv_addBufByPid",
               procId,
               pid,
               eventId,
               data,
               cbFxn);

    GT_assert (curTrace, (NotifyDrv_state.isSetup == TRUE));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.isSetup == FALSE) {
        /*! @retval Notify_E_FAIL The Notify OS driver was not setup */
        status = Notify_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_NotifyDrv_addBufByPid",
                             status,
                             "The Notify OS driver was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (NotifyDrv_state.gateHandle);
        /* Find the registration for this callback */
        for (i = 0 ; i < MAX_PROCESSES ; i++) {
            if (NotifyDrv_state.eventState [i].pid == pid) {
                flag = TRUE;
                break;
            }
        }
        IGateProvider_leave (NotifyDrv_state.gateHandle, key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (flag != TRUE) {
            /*! @retval Notify_E_NOTFOUND Could not find a registered handler
                                          for this process. */
            status = Notify_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_NotifyDrv_addBufByPid",
                                 status,
                                 "Could not find a registered handler "
                                 "for this process.!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
		 /* Allocate memory for the buf */
            uBuf = (NotifyDrv_EventPacket *)Memory_alloc (NULL, sizeof (NotifyDrv_EventPacket), 0, NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (uBuf == NULL) {
                /*! @retval Notify_E_MEMORY Failed to allocate memory for event
                                    packet for received callback. */
                status = Notify_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_NotifyDrv_addBufByPid",
                                     status,
                                     "Failed to allocate memory for event"
                                     " packet for received callback.!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                List_elemClear (&(uBuf->element));
                GT_assert (curTrace,
                           (NotifyDrv_state.eventState [i].bufList != NULL));

                uBuf->procId  = procId;
                uBuf->lineId  = lineId;
                uBuf->data    = data;
                uBuf->eventId = eventId;
                uBuf->func    = cbFxn;
                uBuf->param   = param;
                uBuf->isExit  = FALSE;
                if (uBuf->eventId == (UInt32) -1) {
                    uBuf->isExit = TRUE;
                    isExit = TRUE;
                }

                List_put (NotifyDrv_state.eventState [i].bufList,
                          &(uBuf->element));
                ret = pthread_mutex_lock(&NotifyDrv_state.lock);
                GT_assert (curTrace, (ret == 0));
                item = dequeue_waiting_reader(i);
                if (item) {
                    // there is a waiting reader
                    deliver_notification(i, item->rcvid);
                    put_wr(item);
                    ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                    GT_assert (curTrace, (ret == 0));
                    status = Notify_S_SUCCESS;
                }
                else {
                    if (enqueue_notify_list(i) < 0) {
                        ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                        GT_assert (curTrace, (ret == 0));
                        status = Notify_E_MEMORY;
                        GT_setFailureReason (curTrace,
                               	GT_4CLASS,
                               	"_NotifyDrv_addBufByPid",
                               	status,
                               	"Failed to allocate memory for notifier");
                    }
                    else {
                        status = Notify_S_SUCCESS;
                        pthread_cond_signal(&NotifyDrv_state.cond);
                        ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                        GT_assert (curTrace, (ret == 0));
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_NotifyDrv_addBufByPid", status);

    return status;
}

/*!
 *  @brief      Attach a process to notify user support framework.
 *
 *  @param      pid    Process identifier
 *
 *  @sa         NotifyDrv_detach
 */
Int
NotifyDrv_attach (UInt32 pid)
{
    Int32                status   = Notify_S_SUCCESS;
    Bool                 flag     = FALSE;
    Bool                 isInit   = FALSE;
    List_Object *        bufList  = NULL;
    IArg                 key      = 0;
    List_Params          listparams;
    UInt32               i;
    Error_Block                    eb;

    GT_1trace (curTrace, GT_ENTER, "NotifyDrv_attach", pid);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.isSetup == FALSE) {
        /*! @retval Notify_E_FAIL The Notify OS driver was not setup */
        status = Notify_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDrv_attach",
                             Notify_E_FAIL,
                             "The Notify OS driver was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (NotifyDrv_state.gateHandle);
        for (i = 0 ; (i < MAX_PROCESSES) ; i++) {
            if (NotifyDrv_state.eventState [i].pid == pid) {
                NotifyDrv_state.eventState [i].refCount++;
                isInit = TRUE;
                status = Notify_S_ALREADYSETUP;
                break;
            }
        }

        if (isInit == FALSE) {
            List_Params_init (&listparams);
            /*
             * This List is deleted during shutdown by syslink_notify_read if
             * no reader thread is trying to perform a read when the termination
             * packet is added. Otherwise it would be deleted in the context
             * of NotifyDrv_detach.
             */
            bufList = List_create (&listparams, &eb) ;

                /* Search for an available slot for user process. */
                for (i = 0 ; i < MAX_PROCESSES ; i++) {
                    if (NotifyDrv_state.eventState [i].pid == -1) {

                        NotifyDrv_state.eventState [i].pid = pid;
                        NotifyDrv_state.eventState [i].refCount = 1;
                        NotifyDrv_state.eventState [i].bufList =
                                                                bufList;
                        flag = TRUE;
                        break;
                    }
                }

            /* No free slots found. Let this check remain at run-time,
             * since it is dependent on user environment.
             */
            if (flag != TRUE) {
                    /*! @retval Notify_E_RESOURCE Maximum number of
                     supported user clients have already been registered. */
                    status = Notify_E_RESOURCE;
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "NotifyDrv_attach",
                                      status,
                                      "Maximum number of supported user"
                                          " clients have already been "
                                          "registered.");
                    if (bufList != NULL) {
                        List_delete (&bufList);
                    }

                }

        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    IGateProvider_leave (NotifyDrv_state.gateHandle, key);

    GT_1trace (curTrace, GT_LEAVE, "NotifyDrv_attach", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
    return status ;
}


/*!
 *  @brief      Detach a process from notify user support framework.
 *
 *  @param      pid    Process identifier
 *
 *  @sa         NotifyDrv_attach
 */
Int
NotifyDrv_detach (UInt32 pid)
{
    Int32                status    = Notify_S_SUCCESS;
    Bool                 flag      = FALSE;
    UInt32               i;
    IArg                 key;

    GT_1trace (curTrace, GT_ENTER, "NotifyDrv_detach", pid);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.isSetup == FALSE) {
        /*! @retval Notify_E_FAIL The Notify OS driver was not setup */
        status = Notify_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDrv_detach",
                             Notify_E_FAIL,
                             "The Notify OS driver was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (NotifyDrv_state.gateHandle);

        for (i = 0 ; i < MAX_PROCESSES ; i++) {
            if (NotifyDrv_state.eventState [i].pid == pid) {
                if (NotifyDrv_state.eventState [i].refCount == 1) {
                    NotifyDrv_state.eventState [i].refCount = 0;

                    flag = TRUE;
                    break;
                }
                else {
                    NotifyDrv_state.eventState [i].refCount--;
                    status = Notify_S_ALREADYSETUP;
                }
            }
        }
        IGateProvider_leave (NotifyDrv_state.gateHandle, key);

        if (flag == TRUE) {
            /* Send the termination packet to notify thread */
            status = _NotifyDrv_addBufByPid (0, /* Ignored. */
                                             0,
                                             (UInt32) pid,
                                             (UInt32) -1,
                                             (UInt32) 0,
                                             NULL,
                                             NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDrv_detach",
                                     status,
                                     "Failed to send termination packet.");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }

        if (flag != TRUE) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (i == MAX_PROCESSES) {
                /*! @retval Notify_E_NOTFOUND The specified user process was
                         not found registered with Notify Driver module. */
                status = Notify_E_NOTFOUND;
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "NotifyDrv_detach",
                                  status,
                                  "The specified user process was not found"
                                  " registered with Notify Driver module.");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "NotifyDrv_detach", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed */
    return status;
}

 /**
  * Handler for devctl() messages.
  *
  * Handles special devctl() messages that we export for control.
  *
  * \param ctp	 Thread's associated context information.
  * \param msg	 The actual devctl() message.
  * \param ocb	 OCB associated with client's session.
  *
  * \return POSIX errno value.
  *
  * \retval EOK 	 Success.
  * \retval ENOTSUP  Unsupported devctl().
  */
int syslink_notify_read(
    resmgr_context_t *  ctp,
    io_read_t *         msg,
    syslink_ocb_t *     ocb)
{

    Int32                   status   = Notify_S_SUCCESS;
    Bool                    flag     = FALSE;
    UInt32                  i;
    NotifyList_t *          item;
    Int                     ret;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NotifyDrv_state.isSetup == FALSE) {
        /*! @retval 0 The Notify OS driver was not setup */
        status = Notify_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDrv_read",
                             Notify_E_FAIL,
                             "The Notify OS driver was not setup!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        for (i = 0 ; i < MAX_PROCESSES ; i++) {
            if (NotifyDrv_state.eventState [i].pid == ocb->pid) {
                flag = TRUE;
                break;
            }
        }

        /*  Let the check remain at run-time. */
        if (flag == TRUE) {
          /* Let the check remain at run-time for handling any run-time
          * race conditions.
          */
          if (NotifyDrv_state.eventState [i].bufList != NULL) {
              ret = pthread_mutex_lock(&NotifyDrv_state.lock);
              GT_assert (curTrace, (ret == 0));
              item = find_nl(i);
              if (dequeue_notify_list_item(item) < 0) {
                  status = enqueue_waiting_reader(i, ctp->rcvid);
                  if (status == EOK) {
                      pthread_cond_signal(&NotifyDrv_state.cond);
                      ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                      GT_assert (curTrace, (ret == 0));
                      return(_RESMGR_NOREPLY);
                  }
                  ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                  GT_assert (curTrace, (ret == 0));
              }
              else {
                  ret = pthread_mutex_unlock(&NotifyDrv_state.lock);
                  GT_assert (curTrace, (ret == 0));
                  deliver_notification(i, ctp->rcvid);
              }
          }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "NotifyDrv_read", _RESMGR_NOREPLY);
    /*! @retval Number-of-bytes-read Number of bytes read. */
    return(_RESMGR_NOREPLY);
}
