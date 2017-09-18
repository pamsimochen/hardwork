#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>

/*
 * EDMA3 critical section functions
 * Sample functions showing the implementation of critical section entry/exit
 * routines and various semaphore related routines (all OS depenedent). These
 * implementations MUST be provided by the user / application, using the EDMA3
 * Resource Manager, for its correct functioning.
 * */

static unsigned int region_id;

void edma3OsSetRegionId(int id) {
    region_id= id;
    return;
}

#ifdef IP_RUN_TASK_BASED

void edma3OsProtectEntry (unsigned int edma3InstanceId, int level, unsigned int *intState){
    return;
}

void edma3OsProtectExit (unsigned int edma3InstanceId, int level, unsigned int intState){
    return;
}

EDMA3_DRV_Result edma3OsSemCreate(int initVal,
        const Semaphore_Params *semParams,
        EDMA3_OS_Sem_Handle *hSem){
    *hSem= (EDMA3_OS_Sem_Handle)1;
    return EDMA3_DRV_SOK;
}

EDMA3_DRV_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem){
    return EDMA3_DRV_SOK;
}

EDMA3_DRV_Result edma3OsSemTake(EDMA3_OS_Sem_Handle hSem, int mSecTimeout){
    return EDMA3_DRV_SOK;
}

EDMA3_DRV_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem){
    return EDMA3_DRV_SOK;
}

void edma3OsSemaphore_Params_init(Semaphore_Params *semParams){
    return;
}

#else

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>

extern unsigned int VICP_EDMA3_ccXferCompInt[][EDMA3_MAX_REGIONS];
extern unsigned int VICP_EDMA3_ccErrorInt[];
extern unsigned int VICP_EDMA3_tcErrorInt[][EDMA3_MAX_TC];

/**
 * Shadow Region on which the executable is running. Its value is
 * populated with the DSP Instance Number here in this case.
 */

void edma3OsSemaphore_Params_init(Semaphore_Params *semParams){
    Semaphore_Params_init(semParams);
}

/**
 * \brief   EDMA3 OS Protect Entry
 *
 *      This function saves the current state of protection in 'intState'
 *      variable passed by caller, if the protection level is
 *      EDMA3_OS_PROTECT_INTERRUPT. It then applies the requested level of
 *      protection.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is disabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, '*intState' specifies the
 *      Transfer Controller number whose interrupt needs to be disabled.
 *
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is memory location where current state of protection is
 *      saved for future use while restoring it via edma3OsProtectExit() (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectEntry (unsigned int edma3InstanceId,
        int level, unsigned int *intState)
{
    if (((level == EDMA3_OS_PROTECT_INTERRUPT)
            || (level == EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR))
            && (intState == NULL))
    {
        return;
    }
    else
    {
        switch (level)
        {
        /* Disable all (global) interrupts */
        case EDMA3_OS_PROTECT_INTERRUPT :
            *intState = Hwi_disable();
            break;

            /* Disable scheduler */
        case EDMA3_OS_PROTECT_SCHEDULER :
            Task_disable();
            break;

            /* Disable EDMA3 transfer completion interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION :
            Hwi_disableInterrupt(VICP_EDMA3_ccXferCompInt[edma3InstanceId][region_id]);
            break;

            /* Disable EDMA3 CC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR :
            Hwi_disableInterrupt(VICP_EDMA3_ccErrorInt[edma3InstanceId]);
            break;

            /* Disable EDMA3 TC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR :
            switch (*intState)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                /* Fall through... */
                /* Disable the corresponding interrupt */
                Hwi_disableInterrupt(VICP_EDMA3_tcErrorInt[edma3InstanceId][*intState]);
                break;

            default:
                break;
            }

            break;

            default:
                break;
        }
    }
}


/**
 * \brief   EDMA3 OS Protect Exit
 *
 *      This function undoes the protection enforced to original state
 *      as is specified by the variable 'intState' passed, if the protection
 *      level is EDMA3_OS_PROTECT_INTERRUPT.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is enabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, 'intState' specifies the
 *      Transfer Controller number whose interrupt needs to be enabled.
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is original state of protection at time when the
 *      corresponding edma3OsProtectEntry() was called (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectExit (unsigned int edma3InstanceId,
        int level, unsigned int intState)
{
    switch (level)
    {
    /* Enable all (global) interrupts */
    case EDMA3_OS_PROTECT_INTERRUPT :
        Hwi_restore(intState);
        break;

        /* Enable scheduler */
    case EDMA3_OS_PROTECT_SCHEDULER :
        Task_enable();
        break;

        /* Enable EDMA3 transfer completion interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION :
        Hwi_enableInterrupt(VICP_EDMA3_ccXferCompInt[edma3InstanceId][region_id]);
        break;

        /* Enable EDMA3 CC error interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR :
        Hwi_enableInterrupt(VICP_EDMA3_ccErrorInt[edma3InstanceId]);
        break;

        /* Enable EDMA3 TC error interrupt only */
    case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR :
        switch (intState)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            /* Fall through... */
            /* Enable the corresponding interrupt */
            Hwi_enableInterrupt(VICP_EDMA3_tcErrorInt[edma3InstanceId][intState]);
            break;

        default:
            break;
        }

        break;

        default:
            break;
    }
}


/**
 * Counting Semaphore related functions (OS dependent) should be
 * called/implemented by the application. A handle to the semaphore
 * is required while opening the resource manager instance.
 */

/**
 * \brief   EDMA3 OS Semaphore Create
 *
 *      This function creates a counting semaphore with specified
 *      attributes and initial value. It should be used to create a semaphore
 *      with initial value as '1'. The semaphore is then passed by the user
 *      to the EDMA3 RM for proper sharing of resources.
 * \param   initVal [IN] is initial value for semaphore
 * \param   semParams [IN] is the semaphore attributes.
 * \param   hSem [OUT] is location to recieve the handle to just created
 *      semaphore
 * \return  EDMA3_RM_SOK if succesful, else a suitable error code.
 */
EDMA3_RM_Result edma3OsSemCreate(int initVal,
        const Semaphore_Params *semParams,
        EDMA3_OS_Sem_Handle *hSem)
{
    EDMA3_RM_Result semCreateResult = EDMA3_RM_SOK;

    if(NULL == hSem)
    {
        semCreateResult = EDMA3_RM_E_INVALID_PARAM;
    }
    else
    {
        *hSem = (EDMA3_OS_Sem_Handle)Semaphore_create(initVal, semParams, NULL);
        if ( (*hSem) == NULL )
        {
            semCreateResult = EDMA3_RM_E_SEMAPHORE;
        }
    }

    return semCreateResult;
}


/**
 * \brief   EDMA3 OS Semaphore Delete
 *
 *      This function deletes or removes the specified semaphore
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \param   hSem [IN] handle to the semaphore to be deleted
 * \return  EDMA3_RM_SOK if succesful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_RM_Result semDeleteResult = EDMA3_RM_SOK;

    if(NULL == hSem)
    {
        semDeleteResult = EDMA3_RM_E_INVALID_PARAM;
    }
    else
    {
        Semaphore_delete(hSem);
    }

    return semDeleteResult;
}


/**
 * \brief   EDMA3 OS Semaphore Take
 *
 *      This function takes a semaphore token if available.
 *      If a semaphore is unavailable, it blocks currently
 *      running thread in wait (for specified duration) for
 *      a free semaphore.
 * \param   hSem [IN] is the handle of the specified semaphore
 * \param   mSecTimeout [IN] is wait time in milliseconds
 * \return  EDMA3_RM_Result if successful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemTake(EDMA3_OS_Sem_Handle hSem, int mSecTimeout)
{
    EDMA3_RM_Result semTakeResult = EDMA3_RM_SOK;
    unsigned short semPendResult;

    if(NULL == hSem)
    {
        semTakeResult = EDMA3_RM_E_INVALID_PARAM;
    }
    else
    {
        semPendResult = Semaphore_pend(hSem, mSecTimeout);
        if (semPendResult == FALSE)
        {
            semTakeResult = EDMA3_RM_E_SEMAPHORE;
        }
    }

    return semTakeResult;
}


/**
 * \brief   EDMA3 OS Semaphore Give
 *
 *      This function gives or relinquishes an already
 *      acquired semaphore token
 * \param   hSem [IN] is the handle of the specified semaphore
 * \return  EDMA3_RM_Result if successful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem)
{
    EDMA3_RM_Result semGiveResult = EDMA3_RM_SOK;

    if(NULL == hSem)
    {
        semGiveResult = EDMA3_RM_E_INVALID_PARAM;
    }
    else
    {
        Semaphore_post(hSem);
    }

    return semGiveResult;
}

#endif



