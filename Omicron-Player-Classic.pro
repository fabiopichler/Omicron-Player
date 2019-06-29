#*******************************************************************************
#  Omicron Player Classic
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

include(./config.pri)

QT = core widgets gui network sql xml
TEMPLATE = app
CONFIG += c++11

CONFIG(debug, debug|release) {

    BUILDDIR = debug
    DEBUG = -d
    unix:MAKEFILE = Makefile.debug

} else {

    BUILDDIR = release
    unix:MAKEFILE = Makefile.release
    RESOURCES += ../qrc/Files.qrc

}

TARGET = "omicron-player-classic$${DEBUG}"
OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
DESTDIR = build

unix:INCLUDEPATH += $${BASS_INCLUDE_PATH_UNIX} "$${OTK11_QT_UNIX}/project/include" "$${OTK11_LUA_UNIX}/project/include"
win32:INCLUDEPATH += $${BASS_INCLUDE_PATH_WIN} "$${OTK11_QT_WIN}\\project\\include" "$${OTK11_LUA_WIN}\\project\\include"

LIBS += -lbass -lbass_fx -lbasscd -lbassenc -ltags
LIBS += -lOmicronTK11+Qt$${DEBUG} -lOmicronTK11+Qt_Lua$${DEBUG} -lOmicronTK11+Qt_Network$${DEBUG} -lOmicronTK11+Lua$${DEBUG}

linux:LIBS += -Wl,-rpath=./lib -Wl,-rpath="./$${TARGET}"
win32:LIBS += -lUser32 -lAdvapi32

contains(QMAKE_HOST.arch, x86_64) {

    unix:LIBS += -L$${BASS_LIB_PATH_UNIX64}
    win32:LIBS += -L$${BASS_LIB_PATH_WIN64}

    unix:LIBS += -L"$${OTK11_QT_UNIX}/linux64/build"
    win32:LIBS += -L"$${OTK11_QT_WIN}/win64/build"

    unix:LIBS += -L"$${OTK11_LUA_UNIX}/linux64/build"
    win32:LIBS += -L"$${OTK11_LUA_WIN}/win64/build"

} else {

    unix:LIBS += -L$${BASS_LIB_PATH_UNIX32}
    win32:LIBS += -L$${BASS_LIB_PATH_WIN32}

    unix:LIBS += -L"$${OTK11_QT_UNIX}/linux32/build"
    win32:LIBS += -L"$${OTK11_QT_WIN}/win32/build"

    unix:LIBS += -L"$${OTK11_LUA_UNIX}/linux32/build"
    win32:LIBS += -L"$${OTK11_LUA_WIN}/win32/build"

}

win32:RC_FILE = rc/Win.rc

SOURCES += \
    src/Main.cpp \
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

