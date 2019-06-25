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
    TARGET = "Omicron-Player-3-Debug"
    unix:MAKEFILE = Makefile.debug

} else {

    BUILDDIR = Release
    TARGET = "Omicron-Player-3"
    unix:MAKEFILE = Makefile.release
    RESOURCES += ../qrc/Files.qrc

}

OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
!android:DESTDIR = ../Build

unix:INCLUDEPATH += "/mnt/projects/Frameworks/bass-linux" "/mnt/projects/MyFrameworks/C++/OmicronTK11+Lua/project/include" "/mnt/projects/MyFrameworks/C++/OmicronTK11+Qt/project/include"
win32:INCLUDEPATH += "D:/Frameworks/bass"

contains(QMAKE_HOST.arch, x86_64) {

    win32:LIBS += -L"D:/Frameworks/bass/x64" -lbass -lbass_fx -lbasscd -lbassenc -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -L"/mnt/projects/Frameworks/bass-linux/x64" -lbass -lbass_fx -lbasscd -lbassenc -ltags
        LIBS += -Wl,-rpath=./lib -Wl,-rpath="/mnt/projects/Frameworks/bass-linux/x64"

        LIBS += -L"/mnt/projects/MyFrameworks/C++/OmicronTK11+Lua/linux64/build" -lOmicronTK11_Lua-d
        LIBS += -L"/mnt/projects/MyFrameworks/C++/OmicronTK11+Qt/linux64/build" -lOmicronTK11+Qt-d -lOmicronTK11+Qt_Lua-d
        LIBS += -L"/mnt/projects/MyFrameworks/C++/OmicronTK11/linux64/build" -L"/mnt/projects/MyFrameworks/C++/OmicronTK11+SQLite/linux64/build"
    }

} else {

    win32:LIBS += -L"D:/Frameworks/bass" -lbass -lbass_fx -lbasscd -lbassenc -ltags -lUser32 -lAdvapi32

    linux:!android {
        LIBS += -Wl,-rpath=./lib -L"/mnt/projects/Frameworks/bass-linux" -lbass -lbass_fx -lbasscd -lbassenc -ltags
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
    Gui/MusicUi.cpp \
    Gui/MusicWindow.cpp \
    Gui/RadioUi.cpp \
    Gui/RadioWindow.cpp \
    Gui/RecorderUi.cpp \
    Gui/WindowBase.cpp \
    Tools/Equalizer.cpp \
    Core/Database.cpp \
    Core/NetPlaylist.cpp \
    Core/MusicStream.cpp \
    Core/RadioStream.cpp \
    Tools/ConfigDialog.cpp \
    Tools/ConfigPages.cpp \
    Core/Global.cpp \
    Core/Windows/FileAssociation.cpp \
    Tools/MusicPlaylistManager.cpp \
    Tools/RadioPlaylistManager.cpp \
    Core/Fade.cpp \
    Core/RecorderStream.cpp \
    Gui/RecorderWindow.cpp \
    Core/Stream.cpp \
    Gui/AboutDialog.cpp

HEADERS += Main.h \
    Gui/MusicUi.h \
    Gui/RadioUi.hpp \
    Gui/RecorderUi.hpp \
    Gui/WindowBase.h \
    Version.h \
    Core/SingleApp.h \
    Core/Update.h \
    Gui/MusicWindow.h \
    Gui/RadioWindow.h \
    Tools/Equalizer.h \
    Core/Database.h \
    Core/NetPlaylist.h \
    Core/MusicStream.h \
    Core/RadioStream.h \
    Tools/ConfigDialog.h \
    Tools/ConfigPages.h \
    Core/Global.h \
    Core/Windows/FileAssociation.h \
    Tools/MusicPlaylistManager.h \
    Tools/RadioPlaylistManager.h \
    Core/Fade.h \
    Core/RecorderStream.h \
    Gui/RecorderWindow.h \
    Core/Stream.h \
    Gui/AboutDialog.hpp

