PLATFORM_MODULE_PATH ?= $(PROJECT_ROOT)/platform
include $(call rwildcard,$(PLATFORM_MODULE_PATH)/,include.mk)

PLATFORM_LIB_DIR = $(BUILD_PATH_BASE)/platform/$(BUILD_TARGET_PLATFORM)
PLATFORM_LIB_DEP = $(PLATFORM_LIB_DIR)/libplatform.a
