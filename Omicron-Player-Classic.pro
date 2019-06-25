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
!android:DESTDIR = Build

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

win32:RC_FILE = rc/Win.rc

SOURCES += \
    src/Main.cpp \
    src/Core/SingleApp.cpp \
    src/Core/Update.cpp \
    src/Gui/MusicUi.cpp \
    src/Gui/MusicWindow.cpp \
    src/Gui/RadioUi.cpp \
    src/Gui/RadioWindow.cpp \
    src/Gui/RecorderUi.cpp \
    src/Gui/WindowBase.cpp \
    src/Tools/Equalizer.cpp \
    src/Core/Database.cpp \
    src/Core/NetPlaylist.cpp \
    src/Core/MusicStream.cpp \
    src/Core/RadioStream.cpp \
    src/Tools/ConfigDialog.cpp \
    src/Tools/ConfigPages.cpp \
    src/Core/Global.cpp \
    src/Core/Windows/FileAssociation.cpp \
    src/Tools/MusicPlaylistManager.cpp \
    src/Tools/RadioPlaylistManager.cpp \
    src/Core/Fade.cpp \
    src/Core/RecorderStream.cpp \
    src/Gui/RecorderWindow.cpp \
    src/Core/Stream.cpp \
    src/Gui/AboutDialog.cpp

HEADERS += \
    src/Main.h \
    src/Gui/MusicUi.h \
    src/Gui/RadioUi.hpp \
    src/Gui/RecorderUi.hpp \
    src/Gui/WindowBase.h \
    src/Version.h \
    src/Core/SingleApp.h \
    src/Core/Update.h \
    src/Gui/MusicWindow.h \
    src/Gui/RadioWindow.h \
    src/Tools/Equalizer.h \
    src/Core/Database.h \
    src/Core/NetPlaylist.h \
    src/Core/MusicStream.h \
    src/Core/RadioStream.h \
    src/Tools/ConfigDialog.h \
    src/Tools/ConfigPages.h \
    src/Core/Global.h \
    src/Core/Windows/FileAssociation.h \
    src/Tools/MusicPlaylistManager.h \
    src/Tools/RadioPlaylistManager.h \
    src/Core/Fade.h \
    src/Core/RecorderStream.h \
    src/Gui/RecorderWindow.h \
    src/Core/Stream.h \
    src/Gui/AboutDialog.hpp

