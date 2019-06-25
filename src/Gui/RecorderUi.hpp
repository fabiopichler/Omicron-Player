/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <OmicronTK11/Lua/LuaState.hpp>

namespace OmicronTK11 {

namespace OTKQT {
    class Label;
    class ComboBox;
    class Slider;
}

}

class RecorderWindow;
class RecorderStream;

class RecordList;

class QWidget;
class QMenuBar;
class QAction;
class QPushButton;
class QLabel;
class QMovie;
class QProgressBar;
class QLineEdit;

class RecorderUi
{
    RecorderUi(RecorderWindow *);

    bool init(RecorderStream *);
    void createMenuBar();

    friend class RecorderWindow;

    RecorderWindow *m_recorderWindow;
    RecordList *m_playlistView;

    OmicronTK11::OTK11::LuaState m_luaState;

    QWidget *m_centralWidget;

    QMenuBar *m_menuBar;

    QAction *m_displayRecordingsAction,
        *m_exitAction,
        *m_radioModeAction,
        *m_serverModeAction,
        *m_musicModeAction,
        *m_equalizerAction,
        *m_checkUpdateAction,
        *m_websiteAction,
        *m_facebookAction,
        *m_aboutAction,
        *m_configAction;

    QProgressBar *m_leftChProgressBar,
        *m_rightChProgressBar;

    OmicronTK11::OTKQT::Label *m_timeLabel,
        *m_statusLabel;

    QPushButton *m_recordButton,
        *m_stopButton,
        *m_playButton,
        *m_deleteButton;

    OmicronTK11::OTKQT::ComboBox *m_deviceComboBox,
        *m_encoderComboBox,
        *m_bitrateComboBox;

    OmicronTK11::OTKQT::Slider *m_timeSlider;
};
