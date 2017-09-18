var defaults = {
    "ti.mcu.msp430.runtime.utils.Led": {
        PORT:   0x263,          /* port 8 PORTD*_H */
	DIRCFG: 0x265,
	RED:    0x2,
	GREEN:  0x4,
    },

    "ti.mcu.msp430.runtime.utils.Button": {
        PORT:     0x202,  /* button 1 on exp5529 is on port 1 (PORTA*_L) */
        PORT_OUT: 0x202,
        PORT_REN: 0x206,
        PORT_IE:  0x21A,
        PORT_IFG: 0x21C,
	
	SWITCH1:  128,     /* button 1 is bit 7 (128 = 1<<7) of port 1 */
    },
};
