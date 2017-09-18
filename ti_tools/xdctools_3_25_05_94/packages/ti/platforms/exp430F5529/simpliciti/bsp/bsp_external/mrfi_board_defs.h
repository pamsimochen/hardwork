/*
 *  ======== mrfi_board_defs.h ========
 *
 *   MRFI (Minimal RF Interface)
 *   Board definition file.
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Radios : CC2500, CC1100, CC1101
 */

#ifndef MRFI_BOARD_DEFS_H
#define MRFI_BOARD_DEFS_H

#include "bsp.h"

#if (defined MRFI_CC2500)
  #define MRFI_BOARD_RSSI_OFFSET    72   /* no units */
#elif (defined MRFI_CC1100)
  #define MRFI_BOARD_RSSI_OFFSET    79   /* no units */
#elif (defined MRFI_CC1101)
  #define MRFI_BOARD_RSSI_OFFSET    74   /* no units */
#else
  #error "ERROR: RSSI offset value not defined for this radio"
#endif


/* Wait period in RX state before RSSI becomes valid.*/
/* Worst case time for RSSI to become valid is :
 * 757us - @ 2.4 kbps
 * 155us - @ 250 kbps
 * These numbers are from Design Note DN505 with added safety margin.
 */
#define MRFI_BOARD_RSSI_VALID_DELAY_US    1000

#if (!defined MRFI_CC1100) && \
    (!defined MRFI_CC1101) && \
    (!defined MRFI_CC2500) && \
    (!defined MRFI_CC2420)
#error "ERROR: A compatible radio must be specified for the EXP430F5529 board."
#endif


/* -------------------------GDO0 Pin configuration --------------------------*/
#define __mrfi_GDO0_BIT__                3
#define MRFI_CONFIG_GDO0_PIN_AS_INPUT() st( P2DIR &= ~BV(__mrfi_GDO0_BIT__);)
#define MRFI_GDO0_PIN_IS_HIGH()         (P2IN & BV(__mrfi_GDO0_BIT__))

#define MRFI_GDO0_INT_VECTOR             PORT2_VECTOR
#define MRFI_ENABLE_GDO0_INT()           st( P2IE  |=  BV(__mrfi_GDO0_BIT__); )
#define MRFI_DISABLE_GDO0_INT()          st( P2IE  &= ~BV(__mrfi_GDO0_BIT__); )
#define MRFI_GDO0_INT_IS_ENABLED()        (  P2IE  &   BV(__mrfi_GDO0_BIT__) )
#define MRFI_CLEAR_GDO0_INT_FLAG()       st( P2IFG &= ~BV(__mrfi_GDO0_BIT__); )
#define MRFI_GDO0_INT_FLAG_IS_SET()       (  P2IFG &   BV(__mrfi_GDO0_BIT__) )
#define MRFI_CONFIG_GDO0_RISING_EDGE_INT() st(P2IES &= ~BV(__mrfi_GDO0_BIT__);)
#define MRFI_CONFIG_GDO0_FALLING_EDGE_INT() st(P2IES |=  BV(__mrfi_GDO0_BIT__);)

/* -------------------------GDO2 Pin configuration --------------------------*/
#define __mrfi_GDO2_BIT__                4
#define MRFI_CONFIG_GDO2_PIN_AS_INPUT()  st( P2DIR &= ~BV(__mrfi_GDO2_BIT__);)
#define MRFI_GDO2_PIN_IS_HIGH()          (P2IN & BV(__mrfi_GDO2_BIT__))

#define MRFI_GDO2_INT_VECTOR             PORT2_VECTOR
#define MRFI_ENABLE_GDO2_INT()           st( P2IE  |=  BV(__mrfi_GDO2_BIT__); )
#define MRFI_DISABLE_GDO2_INT()          st( P2IE  &= ~BV(__mrfi_GDO2_BIT__); )
#define MRFI_GDO2_INT_IS_ENABLED()        (  P2IE  &   BV(__mrfi_GDO2_BIT__) )
#define MRFI_CLEAR_GDO2_INT_FLAG()       st( P2IFG &= ~BV(__mrfi_GDO2_BIT__); )
#define MRFI_GDO2_INT_FLAG_IS_SET()       (  P2IFG &   BV(__mrfi_GDO2_BIT__) )
#define MRFI_CONFIG_GDO2_RISING_EDGE_INT() st(P2IES &= ~BV(__mrfi_GDO2_BIT__);)
#define MRFI_CONFIG_GDO2_FALLING_EDGE_INT() st(P2IES |=  BV(__mrfi_GDO2_BIT__);)


/* ------------------------- SPI configuration --------------------------*/
/* CSn Pin Configuration; using 3-pin SPI mode, but need for reseting radio */
#define __mrfi_SPI_CSN_GPIO_BIT__           6
#define MRFI_SPI_CONFIG_CSN_PIN_AS_OUTPUT() st( P2DIR |=  BV(__mrfi_SPI_CSN_GPIO_BIT__); )
#define MRFI_SPI_DRIVE_CSN_HIGH()  st(P2OUT |=  BV(__mrfi_SPI_CSN_GPIO_BIT__);)
#define MRFI_SPI_DRIVE_CSN_LOW()   st(P2OUT &= ~BV(__mrfi_SPI_CSN_GPIO_BIT__);)
#define MRFI_SPI_CSN_IS_HIGH()       ( P2OUT & BV(__mrfi_SPI_CSN_GPIO_BIT__))

/* SCLK Pin Configuration */
#define __mrfi_SPI_SCLK_GPIO_BIT__       2
#define MRFI_SPI_CONFIG_SCLK_PIN_AS_OUTPUT()  st( P3DIR |=  BV(__mrfi_SPI_SCLK_GPIO_BIT__); )
#define MRFI_SPI_DRIVE_SCLK_HIGH() st(P3OUT |=  BV(__mrfi_SPI_SCLK_GPIO_BIT__);)
#define MRFI_SPI_DRIVE_SCLK_LOW()  st(P3OUT &= ~BV(__mrfi_SPI_SCLK_GPIO_BIT__);)

/* SI Pin Configuration */
#define __mrfi_SPI_SI_GPIO_BIT__         0
#define MRFI_SPI_CONFIG_SI_PIN_AS_OUTPUT()  st( P3DIR |=  BV(__mrfi_SPI_SI_GPIO_BIT__); )
#define MRFI_SPI_DRIVE_SI_HIGH()  st( P3OUT |=  BV(__mrfi_SPI_SI_GPIO_BIT__); )
#define MRFI_SPI_DRIVE_SI_LOW()   st( P3OUT &= ~BV(__mrfi_SPI_SI_GPIO_BIT__); )

/* SO Pin Configuration */
#define __mrfi_SPI_SO_GPIO_BIT__         1
#define MRFI_SPI_CONFIG_SO_PIN_AS_INPUT()   st( P3DIR &= ~BV(__mrfi_SPI_SO_GPIO_BIT__);) 
#define MRFI_SPI_SO_IS_HIGH()           ( P3IN & BV(__mrfi_SPI_SO_GPIO_BIT__) )

/* SPI Port Configuration */
#define MRFI_SPI_CONFIG_PORT()  st( P3SEL |= BV(__mrfi_SPI_SCLK_GPIO_BIT__) |  \
                                             BV(__mrfi_SPI_SI_GPIO_BIT__)   |  \
                                             BV(__mrfi_SPI_SO_GPIO_BIT__); )

/* NOTE: not setting P2SEL for bit 6, relying on the default of zero! */

/* read/write macros */
#define MRFI_SPI_WRITE_BYTE(x)        st( UCB0IFG &= ~UCRXIFG;  UCB0TXBUF = x; )
#define MRFI_SPI_READ_BYTE()          UCB0RXBUF
#define MRFI_SPI_WAIT_DONE()          while(!(UCB0IFG & UCRXIFG));

/* SPI critical section macros */
typedef bspIState_t mrfiSpiIState_t;
#define MRFI_SPI_ENTER_CRITICAL_SECTION(x)    BSP_ENTER_CRITICAL_SECTION(x)
#define MRFI_SPI_EXIT_CRITICAL_SECTION(x)     BSP_EXIT_CRITICAL_SECTION(x)


/*
 *  Radio SPI Settings
 *  ------------------
 *  Data Order      :  MSB transmitted first
 *  Clock Polarity  :  low when idle
 *  Clock Phase     :  sample leading edge
 */

/* initialization macro */
#define MRFI_SPI_INIT() \
st ( \
  UCB0CTL1 = UCSWRST;                           \
  UCB0CTL1 = UCSWRST | UCSSEL1;                 \
  UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;   \
  UCB0BR0  = 2;                                 \
  UCB0BR1  = 0;                                 \
  MRFI_SPI_CONFIG_PORT();                       \
  UCB0CTL1 &= ~UCSWRST;                         \
)

#define MRFI_SPI_IS_INITIALIZED()     (UCB0CTL0 & UCMST)

#ifndef BSP_BOARD_EXP5529
#error "ERROR: Mismatch between specified board and MRFI configuration."
#endif

#endif
