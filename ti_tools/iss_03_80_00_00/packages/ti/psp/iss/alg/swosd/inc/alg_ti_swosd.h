/** ==================================================================
 *  @file   alg_ti_swosd.h                                                  
 *                                                                    
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/alg/sw_osd/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SWOSD_OSD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SWOSD_OSD_H_

#define OSA_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )

#define SWOSD_SOK	 (0)
#define SWOSD_EFAIL	 (-1)

#define SWOSD_WINMAX     		(24)
#define SWOSD_MAXNUMHNDL 		(12)	//(8)

/* OSD return Values */
#define SWOSD_NULLHANDLE		(-2)
#define SWOSD_REACHEDMAX		(-3)
#define SWOSD_NOTENABLED		(-4)
#define SWOSD_INVALIDPARAM		(-5)

#define SWOSD_FORMATYUV422i		(0)
#define SWOSD_FORMATYUV420p		(1)
#define SWOSD_RRETSUCCESS		(0)
#define SWOSD_RRETFAIL			(-1)
#define SWOSD_RBOOLFALSE		-1
#define SWOSD_RBOOLTRUE		 	0

#ifndef SWOSD_NULL
#define SWOSD_NULL		        0
#endif

#define SWOSD_SETBMPWINENABLE		0x0001
#define SWOSD_SETBMPMAINWINPRM 		0x0002
#define SWOSD_SETBMPWINCHGTRANS		0x0003
#define SWOSD_SETBMPWINADDR			0x0004
#define SWOSD_SETBMPWINXY			0x0005
#define SWOSD_SETMAINWINDOW			0x0006
#define SWOSD_EXIT					0x0007
#define SWOSD_SETBMPTRANSPARENCY	0x0008

#define SWOSD_VID_MSG_KEY			0x0F864426
#define SWOSD_VID_MSG_KEY1	        0x0F864428

#define SWOSD_BMPWINENABLE			1
#define SWOSD_BMPWINDISABLE			0
#define SWOSD_BMPTRANSENABLE		1
#define SWOSD_BMPTRANSDISABLE		0

// #define SWOSD_DEBUGPRINT
#undef  SWOSD_DEBUGPRINT

typedef struct {

    int enable;
    int format;
    int startX;
    int startY;
    int width;
    int height;
    int lineOffset;
    int transperencyVal;
    int transperencyRange;
    int transperencyEnable;
    int resizeEnable;
    int userTransparancy;
    char *bmpWinAddr;
    char *bmpWinAddrUV;

} SWOSD_BmpWinPrm;

typedef struct {

    int format;
    int width;
    int height;
    int lineOffsetY;
	int lineOffsetUV;
} SWOSD_MainWinPrm;

typedef struct {

    char format;
    int charWidth;
    int charHeight;
    int totFontWidth;
    int totFontHeight;
    int totFontLineoffset;
    char *fontAddr;
    char *fontAddrUV;
    int *fontDB_Y;
    int *fontDB_UV;
} SWOSD_Fontdata;

typedef struct {

    SWOSD_MainWinPrm mainWinPrm;
    SWOSD_BmpWinPrm bmpWinPrm[SWOSD_WINMAX];
    SWOSD_Fontdata *fontInfoprm;
    int numBmpWin;
    int activate;
    int memType; //System_MemoryType ::::  Tiled / non-tiled 
} SWOSD_Hndl;

typedef struct {

    int msgType;
    SWOSD_Hndl *osdHandle;
    char *winAddr;
    char *winAddrUV;
    int winEnable;
    int winID;
    int format;
    int width;
    int height;
    int lineOffsetY;
	int lineOffsetUV;
    int X;
    int Y;
    int transVal;
    int transRange;
    int transEnable;
} SWOSD_Msg;

/* 
 * This API Initialize the parameters required to OSD Window */
/* ===================================================================
 *  @func     SWOSD_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_init(int edmaChId, int dmaQueue);

/* 
 * This API sets the Number of windows */
/* ===================================================================
 *  @func     SWOSD_setNumWindows                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setNumWindows(SWOSD_Hndl * Hndl, int numWindow);

/* 
 * This API sets the Main Window Structure */
/* ===================================================================
 *  @func     SWOSD_setMainWinPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setMainWinPrm(SWOSD_Hndl * Hndl, SWOSD_MainWinPrm * prm);

/* 
 * This API sets the Main Window Structure */
/* ===================================================================
 *  @func     SWOSD_setMainWindowPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setMainWindowPrm(SWOSD_Hndl * Hndl, SWOSD_MainWinPrm * prm);

/* 
 * This API Initialize the bitmap window structure based on bitmap window Id. */
/* ===================================================================
 *  @func     SWOSD_setBmpWinPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setBmpWinPrm(SWOSD_Hndl * Hndl, int bmpWinId, SWOSD_BmpWinPrm * prm);

/* 
 * This API Eanble the window to draw */
/* ===================================================================
 *  @func     SWOSD_setBmpWinEnable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setBmpWinEnable(SWOSD_Hndl * Hndl, int bmpWinId, int enable);

/* 
 * This API changes the window parameter such as startX and startY */
/* ===================================================================
 *  @func     SWOSD_setBmpchangeWinXYPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setBmpchangeWinXYPrm(SWOSD_Hndl * Hndl, int bmpWinId, int startX,
                               int startY);

/* 
 * This API will change the OSD window Transperency Value and Transperancy
 * Range */
/* ===================================================================
 *  @func     SWOSD_winChangeTransperency                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_winChangeTransperency(SWOSD_Hndl * Hndl, int bmpWinId,
                                int bmpTransValue, int bmpTransRange);

/* 
 * This API Sets the transperency */
/* ===================================================================
 *  @func     SWOSD_setBmpTransperancy                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setBmpTransperancy(SWOSD_Hndl * Hndl, int Transparency, int WinId);

/* 
 * This API will update the bmp window address */
/* ===================================================================
 *  @func     SWOSD_setBmpWinAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setBmpWinAddr(SWOSD_Hndl * Hndl, char *WinAddr, char *WinAddrUV,
                        int WinId);

/* 
 * This will update main window parameters */
/* ===================================================================
 *  @func     SWOSD_setPrivMainWinPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setPrivMainWinPrm(SWOSD_Hndl * Hndl, int format, int width,
                            int hieght, int offsetY, int offsetUV);

/* 
 * This API will draw the bmp window */
/* ===================================================================
 *  @func     SWOSD_winDraw                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_winDraw(SWOSD_Hndl * Hndl, short *mainWinAddrY, short *mainWinAddrUV,
                  char *bmpWinAddr, char *bmpAddrUV, int bmpWinId);

/* 
 * This API converts string to YUV data */
/* ===================================================================
 *  @func     SWOSD_MakeOsdwinstring                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_MakeOsdwinstring(SWOSD_Fontdata * fontInfo, char *pInputstr,
                           void *pBuff, SWOSD_BmpWinPrm * pWinPrm,
                           int stringWidth);

/* 
 * This API Recieves the OSD related message */
/* ===================================================================
 *  @func     SWOSD_MsgRecvHandler                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_MsgRecvHandler();

/* 
 * This API Uninitialize the parameters during Initialization */
/* ===================================================================
 *  @func     SWOSD_exit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_exit();

/* SWOSD private API's */
/* ===================================================================
 *  @func     SWOSD_setPrivBmpWinEnable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setPrivBmpWinEnable(SWOSD_Hndl * Hndl, int WinId, int enable);

/* ===================================================================
 *  @func     SWOSD_setPrivBmpchangeWinXYPrm                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setPrivBmpchangeWinXYPrm(SWOSD_Hndl * Hndl, int bmpWinId, int startX,
                                   int startY);

/* ===================================================================
 *  @func     SWOSD_setPrivwinChangeTransperency                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setPrivwinChangeTransperency(SWOSD_Hndl * Hndl, int bmpWinId,
                                       int bmpTransValue, int bmpTransRange);

/* ===================================================================
 *  @func     SWOSD_setPrivBmpWinAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_setPrivBmpWinAddr(SWOSD_Hndl * Hndl, char *bmpWinAddr,
                            char *bmpWinAddrUV, int bmpWinId);

/* ===================================================================
 *  @func     SWOSD_getOSDHndls                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
SWOSD_Hndl *SWOSD_getOSDHndls(int *numHndls);

/* ===================================================================
 *  @func     SWOSD_createHandle                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_createHandle(SWOSD_Hndl ** osdHndl, int *indexHndl);

/* ===================================================================
 *  @func     SWOSD_deleteHandle                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_deleteHandle(int indexHndl);

/* ===================================================================
 *  @func     SWOSD_winDrawHandle                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_winDrawHandle(SWOSD_Hndl * Hndl, short *mainWinAddrY,
                        short *mainWinAddrUV);

/* ===================================================================
 *  @func     SWOSD_createFontDatabase                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SWOSD_createFontDatabase(int *fontDatabaseY, int *fontDatabaseUV,
                             char *swosd_stringPattern,
                             SWOSD_Fontdata * fontInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _SWOSD_OSD_H_ */
