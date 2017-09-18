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
* @file file_mng.h
* @brief File manager.
*/
#ifndef __FILE_MNG_H__
#define __FILE_MNG_H__

#include <sys_env_type.h>
int FileMngInit(void *ShareMem);
int FileMngExit();
int ReadGlobal(void *Buffer);
int GetReadIndexDmvaSaveLoad(Dmva_save_load_rules_t *pSaveLoad,char *ruleName);
int ReadGlobalDmvaTZ(void *Buffer,int setNo);
int ReadGlobalDmvaIMD(void *Buffer,int setNo);
int ReadGlobalDmvaOC(void *Buffer,int setNo);
int ReadGlobalDmvaSM(void *Buffer,int setNo);
int CopyDmvaTzParams(SysInfo *pDst,Dmva_Config_Data *pSrc);
int CopyDmvaImdParams(SysInfo *pDst,Dmva_Config_Data *pSrc);
int CopyDmvaOCParams(SysInfo *pDst,Dmva_Config_Data *pSrc);
int CopyDmvaSMParams(SysInfo *pDst,Dmva_Config_Data *pSrc);
int WriteGlobal(void *Buffer);
int WriteGlobalLog(void *Buffer);
int WriteGlobalDmvaLog(void *Buffer);
int WriteGlobalSys(void *Buffer);
int GetWriteIndexDmvaSaveLoad(Dmva_save_load_rules_t *pSaveLoad,char *ruleName);
int WriteGlobalDmvaTZ(void *Buffer,int setNo);
int WriteGlobalDmvaIMD(void *Buffer,int setNo);
int WriteGlobalDmvaOC(void *Buffer,int setNo);
int WriteGlobalDmvaSM(void *Buffer,int setNo);
int FileMngReset(void *ShareMem);
LogEntry_t* GetLog(int nPageNum, int nItemIndex);
DmvaLogEntry_t* GetDmvaLog(SysInfo *pSysInfo,int nPageNum, int nItemIndex);
int IsFileThreadQuit();
int AddLog(LogEntry_t *pLog);
int AddDmvaLog(DmvaLogEntry_t *pDmvaLog);

/* DMVA event delete fns */
int DmvaEventDelete(int eventIndex);
int DmvaEventDeleteStartEnd(int startIndex,int endIndex);
int DmvaEventDeleteAll();

/* DMVA event search fn */
int SearchStartEndDmvaEvents(SysInfo *pSysInfo);

/* clear system and access logs */
int ClearSystemLog();
int ClearAccessLog();
#endif   /* __FILE_MNG_H__ */
