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

    OmicronTK::OTKLUA::LuaState m_luaState;

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

    OmicronTK::OTKQT::Label *m_timeLabel,
        *m_statusLabel;

    QPushButton *m_recordButton,
        *m_stopButton,
        *m_playButton,
        *m_deleteButton;

    OmicronTK::OTKQT::ComboBox *m_deviceComboBox,
        *m_encoderComboBox,
        *m_bitrateComboBox;

    OmicronTK::OTKQT::Slider *m_timeSlider;
};
