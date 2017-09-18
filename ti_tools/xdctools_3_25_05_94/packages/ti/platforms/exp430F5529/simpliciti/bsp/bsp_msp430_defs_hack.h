/*
 *  ======== bsp_msp430_defs_hack.h ========
 *
 *   BSP (Board Support Package)
 *   MCU : Texas Instruments MSP430 family
 *   Microcontroller definition file.
 */

#ifndef BSP_MSP430_DEFS_H
#define BSP_MSP430_DEFS_H

#define BSP_MCU_MSP430

/* ---------------------- Compiler specifics ---------------------- */
#if (defined __TI_COMPILER_VERSION__) && (defined __MSP430__)
#define BSP_COMPILER_CODE_COMPOSER

#include <msp430.h>

#define __bsp_ISTATE_T__            unsigned short
#define __bsp_ISR_FUNCTION__(f,v)   __bsp_QUOTED_PRAGMA__(vector=v) __interrupt void f(void)

#define BSP_EARLY_INIT(void)  int _system_pre_init(void)

#include <intrinsics.h>
#define __bsp_ENABLE_INTERRUPTS__()       __enable_interrupt()
#define __bsp_DISABLE_INTERRUPTS__()      __disable_interrupt()
#define __bsp_INTERRUPTS_ARE_ENABLED__()  (__get_SR_register() & GIE)

#define __bsp_GET_ISTATE__()              __get_interrupt_state()
#define __bsp_RESTORE_ISTATE__(x)         __set_interrupt_state(x)

#define __bsp_QUOTED_PRAGMA__(x)          _Pragma(#x)

#else
#error "ERROR: Unknown compiler."
#endif

/* ---------------------- Commmon definitions ---------------------- */
#define __bsp_LITTLE_ENDIAN__   1
#define __bsp_CODE_MEMSPACE__   /* blank */
#define __bsp_XDATA_MEMSPACE__  /* blank */

#if !defined(__TI_COMPILER_VERSION__) || !defined( _STDINT_H_)
/* the following are already defined by stdint.h */
typedef   signed char     int8_t;
typedef   signed short    int16_t;
typedef   signed long     int32_t;

typedef   unsigned char   uint8_t;
typedef   unsigned short  uint16_t;
typedef   unsigned long   uint32_t;
#endif

#ifndef NULL
#define NULL 0
#endif

#endif
