#*******************************************************************************
#  Omicron Media Player
#
#  Copyright (c) 2015-2019, Fábio Pichler
#  All rights reserved.
#
#  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
#  Author: Fábio Pichler
#  Website: http://fabiopichler.net
#
#*******************************************************************************/

TEMPLATE = lib
QT = core widgets gui designer
CONFIG += plugin c++11

CONFIG(debug, debug|release) {

    BUILDDIR = Debug-Plugin
    unix:MAKEFILE = Makefile-Plugin.debug

} else {

    BUILDDIR = Release-Plugin
    unix:MAKEFILE = Makefile-Plugin.release

}

TARGET = "FP-CustomWidgets"
OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
!android:DESTDIR = ../../Build/designer

win32:RC_FILE = Plugin.rc

HEADERS = Plugin.h \
    MyComboBoxPlugin.h \
    MyLabelPlugin.h \
    MySliderPlugin.h \
    ../../src/Gui/Widgets/MyComboBox.h \
    ../../src/Gui/Widgets/MyLabel.h \
    ../../src/Gui/Widgets/MySlider.h

SOURCES = Plugin.cpp \
    MyComboBoxPlugin.cpp \
    MyLabelPlugin.cpp \
    MySliderPlugin.cpp \
    ../../src/Gui/Widgets/MyComboBox.cpp \
    ../../src/Gui/Widgets/MyLabel.cpp \
    ../../src/Gui/Widgets/MySlider.cpp

LIBS += -L. 
