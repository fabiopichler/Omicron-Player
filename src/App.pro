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

#-------------------------------------------------
#
# Project created by QtCreator 2014-11-07 T19:14:32
#
#-------------------------------------------------

QT = core widgets gui network sql xml uitools
TEMPLATE = app
CONFIG += c++11

CONFIG += mobility
MOBILITY =

CONFIG(debug, debug|release) {

    BUILDDIR = Debug
    TARGET = "FPM-Player-Debug"
    unix:MAKEFILE = Makefile.debug

} else {

    BUILDDIR = Release
    TARGET = "FPM-Player"
    unix:MAKEFILE = Makefile.release
    RESOURCES += ../qrc/Files.qrc

}

OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
!android:DESTDIR = ../Build

unix:INCLUDEPATH += "/home/Frameworks/bass"
win32:INCLUDEPATH += "D:/Frameworks/bass"

contains(QMAKE_HOST.arch, x86_64) {

    win32:LIBS += -L"D:/Frameworks/bass/x64" -lbass -lbass_fx -lbasscd -lbassenc -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -Wl,-rpath=./lib -L"/home/Frameworks/bass/x64" -lbass -lbass_fx -lbasscd -lbassenc -ltags
    }

} else {

    win32:LIBS += -L"D:/Frameworks/bass" -lbass -lbass_fx -lbasscd -lbassenc -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -Wl,-rpath=./lib -L"/home/Frameworks/bass" -lbass -lbass_fx -lbasscd -lbassenc -ltags
    }
}

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {

    LIBS += -L"/home/Frameworks/bass/android-arm" -lbass -lbass_fx -ltags

    ANDROID_EXTRA_LIBS = \
        /home/Frameworks/bass/android-arm/libbass_fx.so \
        /home/Frameworks/bass/android-arm/libbass.so \
        /home/Frameworks/bass/android-arm/libtags.so
}

contains(ANDROID_TARGET_ARCH,x86) {

    LIBS += -L"/home/Frameworks/bass/android-x86" -lbass -lbass_fx -ltags

    ANDROID_EXTRA_LIBS = \
        /home/Frameworks/bass/android-x86/libbass_fx.so \
        /home/Frameworks/bass/android-x86/libbass.so \
        /home/Frameworks/bass/android-x86/libtags.so

}

win32:RC_FILE = ../rc/Win.rc

SOURCES += Main.cpp \
    Core/SingleApp.cpp \
    Core/Update.cpp \
    Gui/MusicWindow.cpp \
    Gui/RadioWindow.cpp \
    Gui/TitleBar.cpp \
    Tools/Equalizer.cpp \
    Core/Database.cpp \
    Core/NetPlaylist.cpp \
    Core/MusicStream.cpp \
    Core/RadioStream.cpp \
    Gui/DialogBase.cpp \
    Gui/DropArea.cpp \
    Tools/ConfigDialog.cpp \
    Tools/ConfigPages.cpp \
    Core/Global.cpp \
    Gui/ErrorWindow.cpp \
    Core/Windows/FileAssociation.cpp \
    Tools/MusicPlaylistManager.cpp \
    Tools/RadioPlaylistManager.cpp \
    Core/Theme.cpp \
    Gui/MainWindow.cpp \
    Gui/VolumeControl.cpp \
    Core/Fade.cpp \
    Core/Directory.cpp \
    Core/RecorderStream.cpp \
    Gui/RecorderWindow.cpp \
    Core/Stream.cpp \
    Gui/Widgets/MyWidget.cpp \
    Gui/Widgets/MyComboBox.cpp \
    Gui/Widgets/MySlider.cpp \
    Gui/Widgets/MyLabel.cpp

HEADERS += Main.h \
    Version.h \
    Core/SingleApp.h \
    Core/Update.h \
    Gui/MusicWindow.h \
    Gui/RadioWindow.h \
    Gui/TitleBar.h \
    Tools/Equalizer.h \
    Core/Database.h \
    Core/NetPlaylist.h \
    Core/MusicStream.h \
    Core/RadioStream.h \
    Gui/DialogBase.h \
    Gui/DropArea.h \
    Tools/ConfigDialog.h \
    Tools/ConfigPages.h \
    Core/Global.h \
    Gui/ErrorWindow.h \
    Core/Windows/FileAssociation.h \
    Tools/MusicPlaylistManager.h \
    Tools/RadioPlaylistManager.h \
    Core/Theme.h \
    Gui/MainWindow.h \
    Gui/VolumeControl.h \
    Core/Fade.h \
    Core/Directory.h \
    Core/RecorderStream.h \
    Gui/RecorderWindow.h \
    Core/Stream.h \
    Gui/Widgets/MyWidget.h \
    Gui/Widgets/MyComboBox.h \
    Gui/Widgets/MySlider.h \
    Gui/Widgets/MyLabel.h

