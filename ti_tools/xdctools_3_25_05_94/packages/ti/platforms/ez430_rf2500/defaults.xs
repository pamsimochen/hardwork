var defaults = {
    "ti.mcu.msp430.runtime.utils.Led": {
	PORT:   "#PORT1",
	DIRCFG: "#PORT1DIR",
	RED:    0x1,
	GREEN:  0x2,
    },
    "ti.mcu.msp430.runtime.utils.Button": {
        PORT:     "#PORT1",  /* button on ez430_rf2500 is on port 1 */
        PORT_OUT: "#PORT1_OUT",
        PORT_REN: "#PORT1_REN",
        PORT_IE:  "#PORT1_IE",
        PORT_IFG: "#PORT1_IFG",

	SWITCH1: 0x4,       /* button is bit 2 (4 = 1<<2) of port 1 */
    },

    "ti.mcu.msp430.rf.Radio": {
	radio:  "#CC2500",
    },
};
