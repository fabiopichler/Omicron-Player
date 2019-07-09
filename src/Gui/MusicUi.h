/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#pragma once

#include <OmicronTK/Lua/LuaState.hpp>

namespace OmicronTK {

namespace OTKQT {
    class VolumeControl;
    class Slider;
}

}

class MusicStream;

class QObject;
class QWidget;
class QMenuBar;
class QAction;
class QLineEdit;
class QPushButton;
class QLabel;
class QProgressBar;
class QStandardItemModel;
class QSortFilterProxyModel;

class MusicWindow;
class MusicPlaylist;

class MusicButton;
class PlaylistButton;

class MusicUi
{
    MusicUi(MusicWindow *);

    bool init(MusicStream *);
    void createMenuBar();

    void changeRepeatStyle(int);
    void changeRandomStyle(bool);
    void changeTabStyle(int mode);

    friend class MusicWindow;

    MusicWindow *m_musicWindow;
    MusicPlaylist *m_playlistView;
    QWidget *m_centralWidget;

    QMenuBar *m_menuBar;
    QAction *m_openCDAction,
        *m_openMusicAction,
        *m_addMusicAction,
        *m_openDirAction,
        *m_addDirAction,
        *m_openPlaylistAction,
        *m_clearPlaylistAction,
        *m_exitAction,
        *m_radioModeAction,
        *m_recorderModeAction,
        *m_playlistAction,
        *m_equalizerAction,
        *m_configAction,
        *m_checkUpdateAction,
        *m_websiteAction,
        *m_facebookAction,
        *m_aboutAction;

    QLabel *m_currentTrackLabel,
        *m_totalTracksLabel,
        *m_currentTagLabel,
        *m_totalTimeLabel,
        *m_fileTypeLabel,
        *m_timeLabel;

    QPushButton *m_playButton,
        *m_pauseButton,
        *m_stopButton,
        *m_prevButton,
        *m_nextButton,
        *m_changeFavoriteButton,
        *m_repeatButton,
        *m_randomButton,
        *m_playlistManagerButton;

    OmicronTK::OTKQT::Slider *m_timeSlider;
    OmicronTK::OTKQT::VolumeControl *m_volumeControl;
    QProgressBar *m_leftChProgressBar,
        *m_rightChProgressBar;

    QLineEdit *m_searchLineEdit;

    QPushButton *m_musicTabButton;
    QPushButton *m_playlistTabButton;
    QPushButton *m_favoritesTabButton;

    OmicronTK::OTKLUA::LuaState m_luaState;
};
