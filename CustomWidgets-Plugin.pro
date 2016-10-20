#*******************************************************************************
#  FPM Player
#
#  Copyright (c) 2016, Fábio Pichler
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

TARGET = "designer/FP-CustomWidgets"
OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
!android:DESTDIR = Build

win32:RC_FILE = CustomWidgets-Plugin/Win.rc

HEADERS = CustomWidgets-Plugin/MainPlugin.h \
    CustomWidgets-Plugin/MyComboBoxPlugin.h \
    CustomWidgets-Plugin/MyLabelPlugin.h \
    CustomWidgets-Plugin/MySliderPlugin.h \
    src/Gui/Widgets/MyComboBox.h \
    src/Gui/Widgets/MyLabel.h \
    src/Gui/Widgets/MySlider.h

SOURCES = CustomWidgets-Plugin/MainPlugin.cpp \
    CustomWidgets-Plugin/MyComboBoxPlugin.cpp \
    CustomWidgets-Plugin/MyLabelPlugin.cpp \
    CustomWidgets-Plugin/MySliderPlugin.cpp \
    src/Gui/Widgets/MyComboBox.cpp \
    src/Gui/Widgets/MyLabel.cpp \
    src/Gui/Widgets/MySlider.cpp

LIBS += -L. 
