
DLOAD
=====

The DLOAD subdirectory is a slightly modified copy of the RIDL (Reference
Implementation Dynamic Loader) software from the TI GForge site.  The current
implementation is version 2.0.0, and is available in:
	/db/toolsrc/library/vendors2005/ti/ridl/2.0.0/unified_ridl.tar.gz

You can diff or otherwise compare the DLOAD directory against the contents
of the corresponding directory in the RIDL source code to get a sense of
what changes needed to be made to RIDL in order to fit with the current
SysLink 2.x stream.

Also, the dlw_client subdirectory contains code taken from DLOAD/DLWRAPPER
subdirectory.  Code in DLOAD/DLWRAPPER is not actually built, since SysLink's
'client' is in dlw_client. 

