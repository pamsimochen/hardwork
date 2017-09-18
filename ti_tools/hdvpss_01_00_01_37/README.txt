
Steps to build
==============

- Open Rules.make

- Edit paths for the below variables according to your system environment

  CODEGEN_PATH_M3 := C:/PROGRA~1/TEXASI~1/TMS470~1.2
  hdvpss_PATH := C:/PROGRA~1/TEXASI~1/$(hdvpss_RELPATH)
  bios_PATH := C:/PROGRA~1/TEXASI~1/bios_6_xx_xx_xx
  xdc_PATH := C:/PROGRA~1/TEXASI~1/xdctools_3_xx_xx_xx
  ipc_PATH := C:/PROGRA~1/TEXASI~1/ipc_1_xx_xx_xx

  IMPORTANT:
  Make sure the paths specified above DO NOT have any spaces in them.

- Make sure "gmake" is in your system path.
  Typically when XDC is installed "gmake" is installed along with it and
  it becomes part of the system path by default.

- Open command prompt and type the below
  gmake -s all

  This will clean all and build all HDVPSS drivers as well as all HDVPSS
  examples

- TIP: Always use "-s" option with make to see user friendly prints
       during build.

- Optionally before build, edit the below in Rules.make file,
  depending on the kind of build you want to do:

  PLATFORM:=
  PROFILE_$(CORE):=

  Valid values for PLATFORM are:
  ti816x-evm - TI816x EVM with Si build
  ti816x-sim - TI816x simulator build
  ti814x-evm - TI814x EVM with Si build
  ti814x-sim - TI814x simulator build
  ti8107-evm - TI8107 EVM with Si build

  Valid values for PROFILE_$(CORE) are:
  debug                 - Debug build with NO compiler optimizations
                          and symbolic debug info included
  whole_program_debug   - Release build with FULL compiler optimizations
                          and symbolic debug info included

  $(CORE) should always be "m3vpss" for HDVPSS package.

  When building proxy client example, internally CORE is changed
  to m3video when requried for the app.
  In Rules.make, CORE should always be m3vpss.

- The generated files are placed in the below folders,
  Executables   - $(ROOTDIR)/<APP_NAME>/bin/$(PLATFORM)/<APP_NAME>_m3vpss_$(PROFILE).xem3
  Libraries     - $(ROOTDIR)/<MODULE_RELPATH>/lib/$(PLATFORM)/m3/$(PROFILE)/*.aem3
  Objects       - $(ROOTDIR)/<MODULE_RELPATH>/obj/$(PLATFORM)/m3/$(PROFILE)/*.oem3
  XDC configuro - $(ROOTDIR)/<APP_NAME>/obj/$(PLATFORM)/m3vpss/$(PROFILE)/<APP_NAME>_configuro/*
  Dependancies  - $(ROOTDIR)/<APP_NAME>/obj/$(PLATFORM)/m3vpss/$(PROFILE)/.deps/*.P

- To have a custom frame buffer heap size other than predefined in the package,
  the CUSTOM_MEM_FRAME_HEAP_SIZE variable could be set while building the application.
  The value provided is in MB.
  For example to set the frame buffer memory to 100MB use as

  gmake -s chains CUSTOM_MEM_FRAME_HEAP_SIZE=100

  Make sure to do a clean build while doing the above as it may not recompile the
  hdvpss libarary again.

- During development the below "gmake" targets can be also used be used for
  convenience:

  gmake -s hdvpss           - incrementally builds only HDVPSS drivers
  gmake -s examples         - incrementally builds HDVPSS drivers
                              and all examples

  gmake -s platforms        - incrementally builds for all supported PLATFORMs
  gmake -s profiles         - incrementally builds for all supported PROFILEs

  gmake -s all              - clean all and build all drivers, examples
  gmake -s allall           - incrementally builds for all supported PLATFORMs
                              AND PROFILEs

  gmake -s clean            - clean all drivers, examples
  gmake -s examplesclean    - clean all examples ONLY
  gmake -s platformsclean   - cleans for all supported PLATFORMs
  gmake -s profilesclean    - cleans for all supported PROFILEs
  gmake -s allclean         - cleans for all supported PLATFORMs AND PROFILEs

  gmake -s <examplename>    - incrementally builds HDVPSS drivers
                              and the specific example ONLY.

                              Values for <examplename> can be,
                              i2c
                              captureVip
                              chains
                              display
                              grpx
                              m2mSc
                              m2mDei
                              m2mNsf
                              proxy
