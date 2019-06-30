/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

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
