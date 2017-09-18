/*
 *  Copyright 2013 by Texas Instruments Incorporated.
 *
 */

/*
 *  ======== Settings.xdc ========
 *
 */


/*!
 *  ======== Settings ========
 *  Allows selection between TI and MOTOROLA cof libraries.
 */
metaonly module Settings {

    /*! 
     * Mode: {TI, MOTOROLA}. 
     *
     * TI. The function getLibs() returns cof_ti.lib libraries.
     *
     * MOTOROLA. The function getLibs() returns cof_mot.lib libraries.
     */
    enum Mode {TI, MOTOROLA};

    /*! 
     * Determines the libraries contributed by this package. 
     * 
     * Default is TI
     */
    config Mode mode = TI;

}
/*
 *  @(#) ti.targets.omf.cof; 1,0,0,132; 12-20-2013 16:13:27; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

