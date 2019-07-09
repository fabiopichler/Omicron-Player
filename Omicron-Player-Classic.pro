#********************************************************************************
#
#  Omicron Player Classic
#
#  Author: Fábio Pichler
#  Website: http://fabiopichler.net
#  License: BSD 3-Clause License
#
#  Copyright (c) 2015-2019, Fábio Pichler
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
#  * Neither the name of Omicron Player Classic nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#********************************************************************************

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

