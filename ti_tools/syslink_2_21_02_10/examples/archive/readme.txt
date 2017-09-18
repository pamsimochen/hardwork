SysLink Examples
==========================
Look in the platform folders for SysLink examples. Here is a brief
description of each example. Please note, not all examples are available
for each platform.

Extract the examples in a folder of your choice and update the paths to
the necessary components in the products.mak for each of the examples. You
can also extract all the examples for a given device/platform by issuing a 
"make extract" from the top-level SysLink installation.

To build the examples, individually run "make all" for each examples. SysLink
must be configured and built for the device/platform being used. Run 
"make syslink" from the top-level SysLink installation before attempting to 
build the examples. You can also issue "make examples" from the top-level 
SysLink installation to build all the examples.

To install the examples on the device's target file system, individually run 
"make install" for each examples. Set EXEC_DIR to the location of the target
file-system. A "make install' can also be issued from the top-level SysLink
installation.

==========================
ex01_helloworld         Simple one-shot notify event from slave to host.
ex02_messageq           Use MessageQ module between host and slave.
ex03_notify             Use Notify module to communicate between host and slave.
ex04_sharedregion       Use SharedRegion to pass a buffer between the cores.
ex05_heapbufmp          Use HeapBufMP to pass a buffer between host and slave.
ex06_listmp             Use ListMP to add to the list that is used by the cores.
ex07_gatemp             Use GateMP to protect a shared buffer between the cores.
ex08_ringio             Use RingIO to pass data from host to slave.
ex09_readwrite          ProcMgr read/write example
ex33_umsg               Inter-processor Unidirectional Messaging (Umsg) module
ex34_radar              Illustrate umsg library between host and salve
ex63_vpssammu           VPSS master core, AMMU configuration, virtual memory map
ex64_metal              Load and run a non-bios executable.
ex65_overlay            Slave executable example with overlay sections
