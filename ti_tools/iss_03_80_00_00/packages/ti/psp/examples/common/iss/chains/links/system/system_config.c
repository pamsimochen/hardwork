/** ==================================================================
 *  @file   system_config.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/system/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file system_config.c
 *
 *  \brief
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

static const Sys_DeiLinkCfg gSysDeiCfgTbl[] = {
    {
     SYS_DEI_CFG_1CH,                                      /* deiCfgId */
     1u,                                                   /* numCh */
     {
      {1920u},                                             /* deiOutWidth[0] */
      {1920u}                                              /* deiOutWidth[1] */
      },
     {
      {1080u},                                             /* deiOutHeight[0] 
                                                            */
      {1080u}                                              /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_2CH,                                      /* deiCfgId */
     2u,                                                   /* numCh */
     {
      {960u, 960u},                                        /* deiOutWidth[0] */
      {960u, 960u}                                         /* deiOutWidth[1] */
      },
     {
      {640u, 640u},                                        /* deiOutHeight[0] 
                                                            */
      {640u, 640u}                                         /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_4CH,                                      /* deiCfgId */
     4u,                                                   /* numCh */
     {
      {720u, 720u, 720u, 720u},                            /* deiOutWidth[0] */
      {720u, 720u, 720u, 720u}                             /* deiOutWidth[1] */
      },
     {
      {480u, 480u, 480u, 480u},                            /* deiOutHeight[0] 
                                                            */
      {480u, 480u, 480u, 480u}                             /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_4CH_DUALCHAIN,                            /* deiCfgId */
     4u,                                                   /* numCh */
     {
      {720u, 720u, 360u, 360u},                            /* deiOutWidth[0] */
      {720u, 720u, 720u, 720u}                             /* deiOutWidth[1] */
      },
     {
      {480u, 480u, 240u, 240u},                            /* deiOutHeight[0] 
                                                            */
      {480u, 480u, 480u, 480u}                             /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_6CH,                                      /* deiCfgId */
     6u,                                                   /* numCh */
     {
      {640u, 640u, 640u, 640u, 640u, 640u},                /* deiOutWidth[0] */
      {720u, 720u, 720u, 720u, 720u, 720u}                 /* deiOutWidth[1] */
      },
     {
      {360u, 360u, 360u, 360u, 360u, 360u},                /* deiOutHeight[0] 
                                                            */
      {480u, 480u, 480u, 480u, 480u, 480u}                 /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_6CH_DUALCHAIN,                            /* deiCfgId */
     6u,                                                   /* numCh */
     {
      {480u, 480u, 480u, 480u, 480u, 480u},                /* deiOutWidth[0] */
      {480u, 480u, 480u, 480u, 480u, 480u}                 /* deiOutWidth[1] */
      },
     {
      {360u, 360u, 360u, 360u, 360u, 360u},                /* deiOutHeight[0] 
                                                            */
      {360u, 360u, 360u, 360u, 360u, 360u}                 /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_8CH,                                      /* deiCfgId */
     8u,                                                   /* numCh */
     {
      {720u, 720u, 360u, 360u, 720u, 720u, 360u, 360u},    /* deiOutWidth[0] */
      {720u, 720u, 720u, 720u, 720u, 720u, 720u, 720u}     /* deiOutWidth[1] */
      },
     {
      {480u, 480u, 240u, 240u, 480u, 480u, 240u, 240u},    /* deiOutHeight[0] 
                                                            */
      {480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u}     /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     SYS_DEI_CFG_8CH_DUALCHAIN,                            /* deiCfgId */
     8u,                                                   /* numCh */
     {
      {480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u},    /* deiOutWidth[0] */
      {480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u}     /* deiOutWidth[1] */
      },
     {
      {270u, 270u, 270u, 270u, 270u, 270u, 270u, 270u},    /* deiOutHeight[0] 
                                                            */
      {270u, 270u, 270u, 270u, 270u, 270u, 270u, 270u}     /* deiOutHeight[1] 
                                                            */
      }
     },
    {
     /* Note: 4 channels will come from Scalar, hence only 8 are used */
     SYS_DEI_CFG_8CH_12CHMODE,                             /* deiCfgId */
     8u,                                                   /* numCh */
     {
      {480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u},    /* deiOutWidth[0] */
      {480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u}     /* deiOutWidth[1] */
      },
     {
      {360u, 360u, 360u, 360u, 360u, 360u, 360u, 360u},    /* deiOutHeight[0] 
                                                            */
      {360u, 360u, 360u, 360u, 360u, 360u, 360u, 360u}     /* deiOutHeight[1] 
                                                            */
      }
     }
};

static const Sys_ScLinkCfg gSysScCfgTbl[] = {
    {
     /* Note: 8 channels from DEI will go to dislay.  Scalar will get 8
      * channels out of which any 4 channels will go to display rest 4
      * channels will be immediately returned back to input link. */
     SYS_SC_CFG_8CH_12CHMODE,                              /* scCfgId */
     8u,                                                   /* numCh */
     {
      480u, 480u, 480u, 480u, 480u, 480u, 480u, 480u       /* scOutWidth[0] */
      },
     {
      360u, 360u, 360u, 360u, 360u, 360u, 360u, 360u       /* scOutHeight[0] */
      }
     }
};

static const Sys_DispLinkCfg gSysDispCfgTbl[] = {
    {
     SYS_DISP_CFG_1CH,                                     /* dispCfgId */
     1u,                                                   /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {0u},                                                 /* dispWinStartX */
     {0u}                                                  /* dispWinStartY */
     },
    {
     SYS_DISP_CFG_2CH,                                     /* dispCfgId */
     2u,                                                   /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {0u, 960u},                                           /* dispWinStartX */
     {220u, 220u}                                          /* dispWinStartY */
     },
    {
     SYS_DISP_CFG_4CH,                                     /* dispCfgId */
     4u,                                                   /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {240u, 960u, 240u, 960u},                             /* dispWinStartX */
     {60u, 60u, 540u, 540u}                                /* dispWinStartY */
     },
    {
     SYS_DISP_CFG_6CH,                                     /* dispCfgId */
     6u,                                                   /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {0u, 640u, 1280u, 0u, 640u, 1280u},                   /* dispWinStartX */
     {180u, 180u, 180u, 540u, 540u, 540u}                  /* dispWinStartY */
     },
    {
     SYS_DISP_CFG_8CH,                                     /* dispCfgId */
     8u,                                                   /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {60u, 780u, 1500u, 1500u, 60u, 780u, 1500u, 1500u},   /* dispWinStartX */
     {60u, 60u, 60u, 300u, 540u, 540u, 540u, 780u}         /* dispWinStartY */
     },
    {
     SYS_DISP_CFG_12CH,                                    /* dispCfgId */
     12u,                                                  /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {0u, 480u, 960u, 1440u, 0u, 480u, 960u, 1440u, 0u, 480u, 960u, 1440u}, /* dispWinStartX 
                                                                             */
     {0u, 0u, 0u, 0u, 360u, 360u, 360u, 360u, 720u, 720u, 720u, 720u}   /* dispWinStartY 
                                                                         */
     },
    {
     SYS_DISP_CFG_16CH,                                    /* dispCfgId */
     16u,                                                  /* numCh */
     1920u,                                                /* dispWidth */
     1080u,                                                /* dispHeight */
     {0u, 480u, 960u, 1440u, 0u, 480u, 960u, 1440u, 0u, 480u, 960u, 1440u, 0u, 480u, 960u, 1440u},  /* dispWinStartX 
                                                                                                     */
     {0u, 0u, 0u, 0u, 270u, 270u, 270u, 270u, 540u, 540u, 540u, 540u, 810u, 810u, 810u, 810u}   /* dispWinStartY 
                                                                                                 */
     }
};

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

const Sys_DeiLinkCfg *Sys_getDeiLinkCfg(Sys_DeiCfgId deiCfgId)
{
    UInt32 count;

    const Sys_DeiLinkCfg *pCfg = NULL;

    for (count = 0u;
         count < (sizeof(gSysDeiCfgTbl) / sizeof(Sys_DeiLinkCfg)); count++)
    {
        if (gSysDeiCfgTbl[count].deiCfgId == deiCfgId)
        {
            pCfg = &gSysDeiCfgTbl[count];
            break;
        }
    }

    return (pCfg);
}

const Sys_ScLinkCfg *Sys_getScLinkCfg(Sys_ScCfgId scCfgId)
{
    UInt32 count;

    const Sys_ScLinkCfg *pCfg = NULL;

    for (count = 0u;
         count < (sizeof(gSysScCfgTbl) / sizeof(Sys_ScLinkCfg)); count++)
    {
        if (gSysScCfgTbl[count].scCfgId == scCfgId)
        {
            pCfg = &gSysScCfgTbl[count];
            break;
        }
    }

    return (pCfg);
}

const Sys_DispLinkCfg *Sys_getDispLinkCfg(Sys_DispCfgId dispCfgId)
{
    UInt32 count;

    const Sys_DispLinkCfg *pCfg = NULL;

    for (count = 0u;
         count < (sizeof(gSysDispCfgTbl) / sizeof(Sys_DispLinkCfg)); count++)
    {
        if (gSysDispCfgTbl[count].dispCfgId == dispCfgId)
        {
            pCfg = &gSysDispCfgTbl[count];
            break;
        }
    }

    return (pCfg);
}
