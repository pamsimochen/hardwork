/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "vpsdrv_devicePriv.h"
#ifdef PROFILE
#include <ti/sysbios/knl/Clock.h>
#endif

Int32 Vps_deviceInit (Vps_DeviceInitParams * pPrm)
{
    Int32                   status = FVID2_SOK;
    Vps_PlatformId          platformId = Vps_platformGetId();
    Vps_PlatformBoardId     boardId;

    memset(&gVps_deviceObj, 0, sizeof(gVps_deviceObj));

    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        status = FVID2_EFAIL;
        Vps_printf(" %s UNKNOWN platformID\r\n", __FUNCTION__);
    }
#ifdef PROFILE
    Vps_printf(" %d: Vps_deviceI2cInit >>>\r\n", Clock_getTicks());
#endif
    if ((FVID2_SOK == status) && (TRUE == pPrm->isI2cInitReq))
    {
        status = Vps_deviceI2cInit(pPrm);
        if (FVID2_SOK == status)
        {
            boardId = Vps_platformGetBoardId();
            if (boardId >= VPS_PLATFORM_BOARD_MAX)
            {
                status = FVID2_EFAIL;
                Vps_printf (" %s boardId > VPS_PLATFORM_BOARD_MAX \r\n",__FUNCTION__);
            }
        }
        else
        {
            Vps_printf ("%s:Vps_deviceI2cInit failed at line %d: \r\n",__FUNCTION__,__LINE__);
        }
#ifdef PROFILE
        Vps_printf(" %d: Vps_deviceI2cInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
        Vps_printf(" %d: Vps_thsFiltersInit >>>\r\n", Clock_getTicks());
#endif
        if (((VPS_PLATFORM_ID_EVM_TI816x == platformId) || (VPS_PLATFORM_ID_EVM_TI8107 == platformId)) &&
            (FVID2_SOK == status))
        {
            status = Vps_thsFiltersInit(Vps_platformGetI2cInstId());
            if(status != FVID2_SOK)
            {
                Vps_printf (" ERROR %s:Vps_thsFiltersInit failed:%d: \r\n",
                    __FILE__,__LINE__);
            }
        }
#ifdef PROFILE
        Vps_printf(" %d: Vps_thsFiltersInit <<<\r\n", Clock_getTicks());
#endif
#ifdef PROFILE
        Vps_printf(" %d: Vps_tvp5158Init >>>\r\n", Clock_getTicks());
#endif
        if (VPS_PLATFORM_BOARD_VS == boardId && FVID2_SOK == status)
        {
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp5158Init >>>\r\n", Clock_getTicks());
#endif
            status = Vps_tvp5158Init();
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp5158Init <<<\r\n", Clock_getTicks());
#endif
            if(status != FVID2_SOK)
            {
                Vps_printf (" ERROR %s:Vps_tvp5158Init failed:%d: \r\n",
                    __FILE__,__LINE__);
            }
            else
            {
#ifdef PROFILE
                Vps_printf(" %d: Vps_hdmi9022aInit >>>\r\n", Clock_getTicks());
#endif
                status = Vps_hdmi9022aInit();
                if(status != FVID2_SOK)
                {
                    Vps_printf (" ERROR %s:Vps_hdmi9022aInit failed:%d: \r\n",
                        __FILE__,__LINE__);
                }
            }
#ifdef PROFILE
                Vps_printf(" %d: Vps_hdmi9022aInit <<<\r\n", Clock_getTicks());
#endif
        }
        else if (VPS_PLATFORM_BOARD_VC == boardId && FVID2_SOK == status)
        {
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp7002Init >>>\r\n", Clock_getTicks());
#endif
            status = Vps_tvp7002Init();
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp7002Init <<<\r\n", Clock_getTicks());
#endif
            if(status != FVID2_SOK)
            {
                Vps_printf (" ERROR %s:Vps_tvp7002Init failed:%d: \r\n",
                    __FILE__,__LINE__);
            }
            else
            {
#ifdef PROFILE
                Vps_printf(" %d: Vps_sii9135Init >>>\r\n", Clock_getTicks());
#endif
                status = Vps_sii9135Init();
#ifdef PROFILE
                Vps_printf(" %d: Vps_sii9135Init <<<\r\n", Clock_getTicks());
#endif

                if(status != FVID2_SOK)
                {
                    Vps_printf (" ERROR %s:Vps_sii9135Init failed:%d: \r\n",
                        __FILE__,__LINE__);
                }
                else
                {
#ifdef PROFILE
                    Vps_printf(" %d: Vps_hdmi9022aInit >>>\r\n", Clock_getTicks());
#endif
                    status = Vps_hdmi9022aInit();
#ifdef PROFILE
                    Vps_printf(" %d: Vps_hdmi9022aInit <<<\r\n", Clock_getTicks());
#endif
                    if(status != FVID2_SOK)
                    {
                        Vps_printf (" ERROR %s:Vps_hdmi9022aInit failed:%d: \r\n",
                            __FILE__,__LINE__);
                    }
                }
            }
        }
        else if (VPS_PLATFORM_BOARD_CATALOG == boardId && FVID2_SOK == status)
        {
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp7002Init >>>\r\n", Clock_getTicks());
#endif
            status = Vps_tvp7002Init();
            if(status != FVID2_SOK)
            {
                Vps_printf (" ERROR %s:Vps_tvp7002Init failed:%d: \r\n",
                    __FILE__,__LINE__);
            }
#ifdef PROFILE
            Vps_printf(" %d: Vps_tvp7002Init <<<\r\n", Clock_getTicks());
#endif
            if (VPS_PLATFORM_ID_EVM_TI816x == platformId)
            {
#ifdef PROFILE
            Vps_printf(" %d: Vps_hdmi9022aInit >>>\r\n", Clock_getTicks());
#endif
                status = Vps_hdmi9022aInit();
                if(status != FVID2_SOK)
                {
                    Vps_printf (" ERROR %s:Vps_tvp7002Init failed:%d: \r\n",
                        __FILE__,__LINE__);
                }
#ifdef PROFILE
            Vps_printf(" %d: Vps_hdmi9022aInit <<<\r\n", Clock_getTicks());
#endif

            }
        }
        else
        {
            if (VPS_PLATFORM_BOARD_NETCAM != boardId)
            {
                Vps_printf (" %s Daughter card not detected/connected! \r\n",__FUNCTION__);
            }
        }
    }

    return (status);
}

Int32 Vps_deviceDeInit()
{
    Int32                   status = FVID2_SOK;
    Vps_PlatformId          platformId = Vps_platformGetId();
    Vps_PlatformBoardId     boardId = Vps_platformGetBoardId();

    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX) ||
        (boardId >= VPS_PLATFORM_BOARD_MAX))
    {
        status = FVID2_EFAIL;
    }

    if ((VPS_PLATFORM_ID_EVM_TI816x == platformId) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
    {
        status |= Vps_thsFiltersDeInit();
    }

    if (VPS_PLATFORM_BOARD_VS == boardId)
    {
        status |= Vps_tvp5158DeInit();
        status |= Vps_hdmi9022aDeInit();
    }
    else if (VPS_PLATFORM_BOARD_VC == boardId)
    {
        status |= Vps_tvp7002DeInit();
        status |= Vps_sii9135DeInit();
        status |= Vps_hdmi9022aDeInit();
    }
    else if (VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
        status |= Vps_tvp7002DeInit();
        if (VPS_PLATFORM_ID_EVM_TI816x == platformId)
        {
            status |= Vps_hdmi9022aDeInit();
        }
    }
    else
    {
        status |= FVID2_EFAIL;
    }

    Vps_deviceI2cDeInit ();
    return status;
}

