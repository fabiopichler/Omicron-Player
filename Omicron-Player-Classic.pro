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

exists(./config.local.pri) {
    include(./config.local.pri)
} else {
    include(./config.pri)
}

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
    RESOURCES += qrc/Files.qrc

}

TARGET = "omicron-player-classic$${DEBUG}"
OBJECTS_DIR = obj/$${BUILDDIR}
MOC_DIR = moc/$${BUILDDIR}
RCC_DIR = rcc/$${BUILDDIR}
DESTDIR = build

DEFINES += TARGET=\\\"$${TARGET}\\\"

unix:INCLUDEPATH += $${BASS_INCLUDE_PATH_UNIX} "$${OTKQT_UNIX}/project/include" "$${OTKLUA_UNIX}/project/include"
win32:INCLUDEPATH += $${BASS_INCLUDE_PATH_WIN} "$${OTKQT_WIN}\\project\\include" "$${OTKLUA_WIN}\\project\\include"

LIBS += -lbass -lbass_fx -lbasscd -lbassenc -ltags
LIBS += -lOmicronTK+Qt$${DEBUG} -lOmicronTK+Qt_Lua$${DEBUG} -lOmicronTK+Qt_Network$${DEBUG} -lOmicronTK+Lua$${DEBUG}

win32:LIBS += -lUser32 -lAdvapi32 -lShlwapi

contains(QMAKE_HOST.arch, x86_64) {

    unix:ARCHITECTURE = "lib64"
    unix:ARCHITECTURE_DEB = "lib/x86_64-linux-gnu"

    unix:LIBS += -L$${BASS_LIB_PATH_UNIX64}
    win32:LIBS += -L$${BASS_LIB_PATH_WIN64}

    unix:LIBS += -L"$${OTKQT_UNIX}/linux64/build"
    win32:LIBS += -L"$${OTKQT_WIN}/win64/build"

    unix:LIBS += -L"$${OTKLUA_UNIX}/linux64/build"
    win32:LIBS += -L"$${OTKLUA_WIN}/win64/build"

} else {

    unix:ARCHITECTURE = "lib"
    unix:ARCHITECTURE_DEB = "lib/i386-linux-gnu"

    unix:LIBS += -L$${BASS_LIB_PATH_UNIX32}
    win32:LIBS += -L$${BASS_LIB_PATH_WIN32}

    unix:LIBS += -L"$${OTKQT_UNIX}/linux32/build"
    win32:LIBS += -L"$${OTKQT_WIN}/win32/build"

    unix:LIBS += -L"$${OTKLUA_UNIX}/linux32/build"
    win32:LIBS += -L"$${OTKLUA_WIN}/win32/build"

}

unix {
    DEFINES += ARCHITECTURE=\\\"$${ARCHITECTURE}\\\" ARCHITECTURE_DEB=\\\"$${ARCHITECTURE_DEB}\\\"
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../$${ARCHITECTURE}/$${TARGET}/lib\',-z,origin' '-Wl,-rpath,\'\$$ORIGIN/../$${ARCHITECTURE_DEB}/$${TARGET}/lib\',-z,origin'
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

