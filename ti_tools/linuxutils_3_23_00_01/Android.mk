LOCAL_PATH:= $(call my-dir)
include $(LOCAL_PATH)/../config/mm_config.mk

# Library suffix name
SUFFIX := bnc

###############################################################################
# CMEM
###############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE        := timm_cmem
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE_CLASS  := STATIC_LIBRARIES
LOCAL_MODULE_TAGS   := optional

LOCAL_SRC_FILES := android_binaries/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)

# Specify variables to be exported to the sub-build
define $(LOCAL_MODULE)_set_config_vars
    $(call timm_set_config_var2,ANDROID_DIR,ANDROID_INSTALL_DIR) \
    $(call timm_set_config_var1,LINUXKERNEL_INSTALL_DIR) \
    $(call timm_set_config_var2,BIONICTOOL_PREFIX,TOOLCHAIN_PREFIX) \
    $(call timm_set_config_var2,ANDROID_CFLAGS,CFLAGS)
endef

# Build against the Android stack
define $(LOCAL_MODULE)_rebuild
$(LOCAL_PATH)/$(LOCAL_SRC_FILES) :
	$(MAKE) -C $(LOCAL_PATH)/packages/ti/sdo/linuxutils/cmem/src/module \
	$(call $(LOCAL_MODULE)_set_config_vars) \
	clean
	$(MAKE) -C $(LOCAL_PATH)/packages/ti/sdo/linuxutils/cmem/src/interface \
	$(call $(LOCAL_MODULE)_set_config_vars) \
	clean
	$(MAKE) -C $(LOCAL_PATH)/packages/ti/sdo/linuxutils/cmem/src/module bionic \
	$(call $(LOCAL_MODULE)_set_config_vars)
	$(MAKE) -C $(LOCAL_PATH)/packages/ti/sdo/linuxutils/cmem/src/interface bionic \
	$(call $(LOCAL_MODULE)_set_config_vars)
	mkdir -p $$(dir $$@)
	cp -fp $(LOCAL_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a$(SUFFIX) $$@
endef
$(eval $(call $(LOCAL_MODULE)_rebuild))

include $(BUILD_PREBUILT)

# Cleanup
$(LOCAL_MODULE)_set_config_vars :=
$(LOCAL_MODULE)_rebuild         :=
include $(LOCAL_PATH)/../config/mm_config_cleanup.mk

