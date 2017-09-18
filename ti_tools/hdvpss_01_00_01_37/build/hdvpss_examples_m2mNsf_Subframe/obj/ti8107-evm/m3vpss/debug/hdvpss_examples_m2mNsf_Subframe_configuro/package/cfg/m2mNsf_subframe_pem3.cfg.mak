# invoke SourceDir generated makefile for m2mNsf_subframe.pem3
m2mNsf_subframe.pem3: .libraries,m2mNsf_subframe.pem3
.libraries,m2mNsf_subframe.pem3: package/cfg/m2mNsf_subframe_pem3.xdl
	$(MAKE) -f package/cfg/m2mNsf_subframe_pem3.src/makefile.libs

clean::
	$(MAKE) -f package/cfg/m2mNsf_subframe_pem3.src/makefile.libs clean

