
# Add include to all objects built for this target
INCLUDE_DIRS += $(RTXGUI_MODULE_PATH)/inc


INCLUDE_DIRS += /usr/include/x86_64-linux-gnu/qt5 /usr/include/x86_64-linux-gnu/qt5/QtWidgets /usr/include/x86_64-linux-gnu/qt5/QtGui /usr/include/x86_64-linux-gnu/qt5/QtCore /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++-64 /usr/include/x86_64-linux-gnu/qt5/QtQuick /usr/include/x86_64-linux-gnu/qt5/QtQml
CPPFLAGS += -DQT_NO_DEBUG_OUTPUT -DCONFIG_PC_PLATFORM -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CPPFLAGS += -fPIC
