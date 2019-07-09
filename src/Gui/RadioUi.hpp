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
    class Label;
}

}

class RadioWindow;
class RadioStream;

class RadioPlaylist;

class QWidget;
class QMenuBar;
class QAction;
class QPushButton;
class QLabel;
class QMovie;
class QProgressBar;
class QLineEdit;
class QMovie;

class RadioUi
{
    RadioUi(RadioWindow *);

    bool init(RadioStream *);
    void createMenuBar();

    friend class RadioWindow;

    RadioWindow *m_radioWindow;
    RadioPlaylist *m_playlistView;

    OmicronTK::OTKLUA::LuaState m_luaState;

    QWidget *m_centralWidget;

    QMenuBar *m_menuBar;

    QAction *m_openLinkAction,
        *m_displayRecordingsAction,
        *m_exitAction,
        *m_musicModeAction,
        *m_recorderModeAction,
        *m_serverModeAction,
        *m_playlistManagerAction,
        *m_equalizerAction,
        *m_configAction,
        *m_checkUpdateAction,
        *m_websiteAction,
        *m_facebookAction,
        *m_aboutAction;

    QPushButton *m_playButton,
        *m_stopButton,
        *m_prevButton,
        *m_nextButton,
        *m_recordButton,
        *m_openLinkButton,
        *m_radioPlaylistButton,
        *m_changeFavoriteButton,
        *m_allPlaylistsModeButton,
        *m_customPlaylistModeButton,
        *m_favoriteModeButton;

    QLabel *m_timeLabel,
        *m_statusLabel,
        *m_nameLabel;

    OmicronTK::OTKQT::Label *m_streamTitleLabel;
    QMovie *m_loaderMovie;
    QProgressBar *m_leftChProgressBar,
        *m_rightChProgressBar,
        *m_bufferProgressBar;

    QLineEdit *m_searchLineEdit;
    OmicronTK::OTKQT::VolumeControl *m_volumeControl;
};
