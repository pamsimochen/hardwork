#ifndef _FIRMWARE_H
#define _FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void FIRMWARE_activateIMCOP(void);
void FIRMWARE_initEdmaController(Uint16 *tccArray, Uint32 numChan, Uint32 q_num, Uint32 *drae0, Uint32 *drae0h);
void FIRMWARE_resetIMCOP (void);
void FIRMWARE_clearEvent(Uint16 *tccArray, Uint32 numChan);
void FIRMWARE_IMX0_start(Uint16 cmd_ofst);
void FIRMWARE_IMX0_wait(void);
void FIRMWARE_IMX1_start(Uint16 cmd_ofst);
void FIRMWARE_IMX1_wait(void);

void IMGBUF_switch(Uint64 buffers, Uint64 connections);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif

