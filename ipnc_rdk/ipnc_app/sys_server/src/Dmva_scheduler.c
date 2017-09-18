/* ===========================================================================
* @path $(IPNCPATH)\sys_adm\system_server
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file schedule_mng.c
* @brief Schedule manager.
*/

#include <stdio.h>
#include <sys/time.h>
#include <sys_env_type.h>

SemHandl_t gSemDmvaSchedule = NULL;
static pthread_t gDmvaScheduleThread;

/* Get the current time */
struct tm *dmvaSchedGetCurTime(void)
{
    time_t timep;
    struct tm *p;

    time(&timep);
    p = localtime(&timep);
    return p;
}

/* check if we need to trigger new DMVA schedule */
int checkDmvaSchedule(SysInfo *pSysInfo)
{
    int i;
    int schIndex = -1;
    int curTimeInMin;
    int curDay;
    struct tm *pCurTime;
    static int zeroMinCnt;

    // Get the current time
    pCurTime     = dmvaSchedGetCurTime();
    curTimeInMin = (pCurTime->tm_hour * 60) + pCurTime->tm_min;

    if(curTimeInMin == 0)
    {
	    zeroMinCnt ++;
	}
	else
	{
	    zeroMinCnt = 0;
	}

    if(pCurTime->tm_wday == 0)
    {
        curDay = DMVA_SCH_DAY_SUN;
    }
    else
    {
        curDay = pCurTime->tm_wday;
    }

    #ifdef DMVA_SCH_DBG
    fprintf(stderr," DMVA SCH > checking DMVA schedule at %2d:%2d:%2d\n",
            pCurTime->tm_hour,pCurTime->tm_min,pCurTime->tm_sec);
    #endif

    for(i = 0;i < DMVA_SCH_MAX_NUM_SCHEDULE;i ++)
    {
        // check if the schedule is active
        if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].isActive != 1)
        {
            continue;
        }

        // check the schedule day
        if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay != DMVA_SCH_DAY_ALL)
        {
            // NOT everyday
            if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay == DMVA_SCH_DAY_WEEKDAYS)
            {
                // ALL week days
                if((curDay < DMVA_SCH_DAY_MON) ||
                   (curDay > DMVA_SCH_DAY_FRI))
                {
                    #ifdef DMVA_SCH_DBG
                    fprintf(stderr," DMVA SCH > rule no:%d start day:%d end day:%d,cur day:%d\n",
                            i,pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay,
                            pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay,curDay);
                    #endif

                    continue;
                }
            }
            else if(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay == DMVA_SCH_DAY_WEEKEND)
                 {
                     // week end
                     if((curDay != DMVA_SCH_DAY_SAT) &&
                        (curDay != DMVA_SCH_DAY_SUN))
                     {
                         #ifdef DMVA_SCH_DBG
                          fprintf(stderr," DMVA SCH > rule no:%d start day:%d end day:%d,cur day:%d\n",
                              i,pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay,
                              pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay,curDay);
                         #endif

                         continue;
                     }
                 }
                 else
                 {
                     // any day of the week
                     if((curDay < pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay) ||
                        (curDay > pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay))
                     {
                         #ifdef DMVA_SCH_DBG
                          fprintf(stderr," DMVA SCH > rule no:%d start day:%d end day:%d,cur day:%d\n",
                              i,pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startDay,
                              pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endDay,curDay);
                         #endif

                         continue;
                     }
                 }
        }

        // Check the start time
        if((curTimeInMin >= pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].startTimeInMin) &&
           (curTimeInMin < pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].endTimeInMin))
        {
            if(pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule != i)
            {
                pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule = schIndex = i;

                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > starting new schedule(%s) at %d day and %d time\n",
                        pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[i].ruleLabel,curDay,curTimeInMin);
                #endif

                break;
            }
        }
    }

    if(schIndex == -1)
    {
        // check the end time
        if(pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule != -1)
        {
			if(zeroMinCnt == 1)
			{
			    curTimeInMin = 24 * 60;
			}

            if(curTimeInMin >=
               pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule].endTimeInMin)
            {
                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > stopping schedule at %d day and %d time\n",
                        curDay,curTimeInMin);
                #endif

                schIndex = -2;
                pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule = -1;
            }
        }
    }

    return schIndex;
}

/* start new schedule */
int dmvaStartNewSchedule(SysInfo *pSysInfo)
{
    int schIndex;
    int loadIndex;

    schIndex  = pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule;
    loadIndex = pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].loadSetting;

    switch(pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].ruleType)
    {
        case DMVA_SCH_RULETYPE_TZ:
            if(loadIndex != -1)
            {
                // Load the TZ params
                SetDmvaTZLoad(loadIndex);

                // set detect mode to TZ
                SetDmvaCfgCTDImdSmetaTzOc(8);

                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > changing detect mode to TRIP ZONE\n");
                #endif
            }

            break;

        case DMVA_SCH_RULETYPE_IMD:
            if(loadIndex != -1)
            {
                // Load the IMD params
                SetDmvaIMDLoad(loadIndex);

                // set detect mode to IMD
                SetDmvaCfgCTDImdSmetaTzOc(2);

                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > changing detect mode to IMD\n");
                #endif
            }

            break;

        case DMVA_SCH_RULETYPE_OC:
            if(loadIndex != -1)
            {
                // Load the OC params
                SetDmvaOCLoad(loadIndex);

                // set detect mode to OC
                SetDmvaCfgCTDImdSmetaTzOc(16);

                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > changing detect mode to OC\n");
                #endif
            }

            break;

        case DMVA_SCH_RULETYPE_SMETA:
            if(loadIndex != -1)
            {
                // Load the SMETA params
                SetDmvaSMETALoad(loadIndex);

                // set detect mode to SEMTA
                SetDmvaCfgCTDImdSmetaTzOc(4);

                #ifdef DMVA_SCH_DBG
                fprintf(stderr," DMVA SCH > changing detect mode to SMETA\n");
                #endif
            }

            break;

        default:
            #ifdef DMVA_SCH_DBG
            fprintf(stderr," DMVA SCH > Invalid rule type\n");
            #endif

            break;
    }

    if(loadIndex == -1)
    {
        #ifdef DMVA_SCH_DBG
        fprintf(stderr," DMVA SCH > couldn't find the load rule with the index %d\n",loadIndex);
        #endif

        pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule    = -1;
        pSysInfo->dmva_config.dmvaScheduleMgr.curRuleType    = -1;
        pSysInfo->dmva_config.dmvaScheduleMgr.curLoadSetting = -1;
        return -1;
    }
    else
    {
        pSysInfo->dmva_config.dmvaScheduleMgr.curRuleType    = pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].ruleType;
        pSysInfo->dmva_config.dmvaScheduleMgr.curLoadSetting = loadIndex;
    }

    return 0;
}

/* stop the schedule */
int dmvaStopSchedule(SysInfo *pSysInfo)
{
    // set detect mode to NONE
    SetDmvaCfgCTDImdSmetaTzOc(0);

    pSysInfo->dmva_config.dmvaScheduleMgr.curRuleType    = -1;
    pSysInfo->dmva_config.dmvaScheduleMgr.curLoadSetting = -1;

    #ifdef DMVA_SCH_DBG
    fprintf(stderr," DMVA SCH > changing detect mode to NONE\n");
    #endif

    return 0;
}

/* DMVA schedule thread */

void *dmvaScheduleThrFxn(void *args)
{
    void    *status = NULL;
    int     schIndex;
    SysInfo *pSysInfo = (SysInfo *)args;

    pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule = -1;

    // Main loop of the DMVA schedule thread
    while(IsFileThreadQuit() == 0)
    {
        // check if DMVA scheduler is enabled
        if(pSysInfo->dmva_config.dmvaScheduleMgr.enable == 1)
        {
            // check if we need to trigger new DMVA schedule
            SemWait(gSemDmvaSchedule);
            schIndex = checkDmvaSchedule(pSysInfo);
            SemRelease(gSemDmvaSchedule);

            if(schIndex != -1)
            {
                if(schIndex == -2)
                {
                     // cur schedule ended and NO new schedule started
                     if(dmvaStopSchedule(pSysInfo) == -1)
                     {
                          #ifdef DMVA_SCH_DBG
                          fprintf(stderr," DMVA SCH > cannot stop the schedule\n");
                          #endif
                     }
                }
                else
                {
                    // New schedule is started
                    if(dmvaStartNewSchedule(pSysInfo) == -1)
                    {
                        #ifdef DMVA_SCH_DBG
                        fprintf(stderr," DMVA SCH > cannot start the schedule with the index %d\n",
                                schIndex);
                        #endif
                    }
                }
            }
            else
            {
                schIndex  = pSysInfo->dmva_config.dmvaScheduleMgr.curSchedule;

                if(schIndex != -1)
                {
                    // check if the current rule type is changed
                    if((pSysInfo->dmva_config.dmvaScheduleMgr.curRuleType !=
                        pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].ruleType) ||
                       (pSysInfo->dmva_config.dmvaScheduleMgr.curLoadSetting !=
                        pSysInfo->dmva_config.dmvaScheduleMgr.dmvaSchedule[schIndex].loadSetting))
                    {
                        #ifdef DMVA_SCH_DBG
                        fprintf(stderr," DMVA SCH > The current rule type/load setting is changed\n");
                        #endif

                        if(dmvaStartNewSchedule(pSysInfo) == -1)
                        {
                            #ifdef DMVA_SCH_DBG
                            fprintf(stderr," DMVA SCH > cannot start the schedule with the index %d\n",
                                    schIndex);
                            #endif
                        }
                    }
                }// if(schIndex != -1)
            }
        }// if(pSysInfo->dmva_config.dmvaScheduleMgr.enable == 1)

        // sleep for 30 seconds
        sleep(30);
    }// while(IsFileThreadQuit() == 0)

    return status;
}

/* DMVA Scheduler Init fn */
int DmvaScheduleMngInit(SysInfo *pSysInfo)
{
    if(gSemDmvaSchedule == NULL)
    {
        gSemDmvaSchedule = MakeSem();
    }

    if(gSemDmvaSchedule == NULL)
    {
        #ifdef DMVA_SCH_DBG
        fprintf(stderr," DMVA SCH > Dmva semaphore creation failed\n");
        #endif

        return -1;
    }

    #ifdef DMVA_SCH_DBG
    fprintf(stderr," DMVA SCH > created Dmva semaphore with handle = %d\n",gSemDmvaSchedule);
    #endif

    // Create DMVA Scheduler thread
    if(pthread_create(&gDmvaScheduleThread,NULL,dmvaScheduleThrFxn,pSysInfo))
    {
        DestroySem(gSemDmvaSchedule);

        #ifdef DMVA_SCH_DBG
        fprintf(stderr," DMVA SCH > schedule thread create failed\n");
        #endif

        return -1;
    }

    #ifdef DMVA_SCH_DBG
    fprintf(stderr," DMVA SCH > schedule thread create successful\n");
    #endif

    return 0;
}
