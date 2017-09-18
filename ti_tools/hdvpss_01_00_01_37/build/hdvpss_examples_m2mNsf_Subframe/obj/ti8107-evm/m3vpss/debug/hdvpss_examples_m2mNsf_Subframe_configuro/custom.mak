## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em3 linker.cmd \
  package/cfg/m2mNsf_subframe_pem3.oem3 \

linker.cmd: package/cfg/m2mNsf_subframe_pem3.xdl
	$(SED) 's"^\"\(package/cfg/m2mNsf_subframe_pem3cfg.cmd\)\"$""\"/proj/vsi/users/anith/DM812x/IPNetCam/Release/ti_tools/hdvpss_01_00_01_37_patched/build/hdvpss_examples_m2mNsf_Subframe/obj/ti8107-evm/m3vpss/debug/hdvpss_examples_m2mNsf_Subframe_configuro/\1\""' package/cfg/m2mNsf_subframe_pem3.xdl > $@
