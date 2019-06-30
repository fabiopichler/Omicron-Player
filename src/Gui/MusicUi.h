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
