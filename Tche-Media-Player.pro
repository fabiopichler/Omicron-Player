#*******************************************************************************
#  Tchê Media Player
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

QT = core widgets gui network sql xml
TEMPLATE = app
CONFIG += c++11

CONFIG += mobility
MOBILITY =

CONFIG(debug, debug|release) {

    BUILDDIR = Debug
    TARGET = "Tche-Media-Player-Debug"
    unix:MAKEFILE = Makefile.debug

} else {

    BUILDDIR = Release
    TARGET = "Tche-Media-Player"
    unix:MAKEFILE = Makefile.release
    RESOURCES += qrc/Files.qrc

}

OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
!android:DESTDIR = Build

unix:INCLUDEPATH += "/home/Frameworks/bass"
win32:INCLUDEPATH += "D:/Frameworks/bass"

contains(QMAKE_HOST.arch, x86_64) {

    win32:LIBS += -L"D:/Frameworks/bass/x64" -lbass -lbass_fx -lbasscd -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -Wl,-rpath=./lib -L"/home/Frameworks/bass/x64" -lbass -lbass_fx -lbasscd -ltags
    }

} else {

    win32:LIBS += -L"D:/Frameworks/bass" -lbass -lbass_fx -lbasscd -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -Wl,-rpath=./lib -L"/home/Frameworks/bass" -lbass -lbass_fx -lbasscd -ltags
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

SOURCES += src/Main.cpp \
    src/Core/SingleApp.cpp \
    src/Core/Update.cpp \
    src/Gui/MusicWindow.cpp \
    src/Gui/RadioWindow.cpp \
    src/Gui/TitleBar.cpp \
    src/Tools/Equalizer.cpp \
    src/Core/Database.cpp \
    src/Core/StreamBase.cpp \
    src/Core/NetPlaylist.cpp \
    src/Core/MusicStream.cpp \
    src/Core/RadioStream.cpp \
    src/Gui/DialogBase.cpp \
    src/Gui/DropArea.cpp \
    src/Tools/ConfigDialog.cpp \
    src/Tools/ConfigPages.cpp \
    src/Core/Global.cpp \
    src/Gui/ErrorWindow.cpp \
    src/Core/Windows/FileAssociation.cpp \
    src/Tools/MusicPlaylistManager.cpp \
    src/Tools/RadioPlaylistManager.cpp \
    src/Core/Theme.cpp \
    src/Gui/MainWindow.cpp \
    src/Gui/VolumeControl.cpp \
    src/Gui/Widgets.cpp \
    src/Core/Fade.cpp

HEADERS += src/Main.h \
    src/Version.h \
    src/Core/SingleApp.h \
    src/Core/Update.h \
    src/Gui/MusicWindow.h \
    src/Gui/RadioWindow.h \
    src/Gui/TitleBar.h \
    src/Tools/Equalizer.h \
    src/Core/Database.h \
    src/Core/StreamBase.h \
    src/Core/NetPlaylist.h \
    src/Core/MusicStream.h \
    src/Core/RadioStream.h \
    src/Gui/DialogBase.h \
    src/Gui/DropArea.h \
    src/Tools/ConfigDialog.h \
    src/Tools/ConfigPages.h \
    src/Core/Global.h \
    src/Gui/ErrorWindow.h \
    src/Core/Windows/FileAssociation.h \
    src/Tools/MusicPlaylistManager.h \
    src/Tools/RadioPlaylistManager.h \
    src/Core/Theme.h \
    src/Gui/MainWindow.h \
    src/Gui/VolumeControl.h \
    src/Gui/Widgets.h \
    src/Core/Fade.h

win32:RC_FILE = rc/Win.rc

