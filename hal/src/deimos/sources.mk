HAL_SRC_PHOBOS_PATH = src/deimos

CSRC += $(call target_files,$(HAL_SRC_DEIMOS_PATH)/,*.c)
CPPSRC += $(call target_files,$(HAL_SRC_DEIMOS_PATH)/,*.cpp)

ASRC +=

CPPFLAGS += -std=gnu++11
