
INCLUDE_DIRS += $(HAL_SRC_DEIMOS_PATH)

CPPFLAGS += -fPIC
INCLUDE_DIRS += /usr/include/x86_64-linux-gnu/qt5 /usr/include/x86_64-linux-gnu/qt5/QtWidgets /usr/include/x86_64-linux-gnu/qt5/QtGui /usr/include/x86_64-linux-gnu/qt5/QtCore /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64


# Define the prefix to this directory.
# Note: The name must be unique within this build and should be
#       based on the root of the project
HAL_SRC_DEIMOS_PATH = $(TARGET_HAL_PATH)/src/deimos
