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

#include "../Core/RecorderStream.h"
#include "../Core/Global.h"
#include "../Tools/Equalizer.h"

#include <OmicronTK/Qt/Slider.hpp>
#include <OmicronTK/Qt/Widget.hpp>

#include "WindowBase.h"
#include "RecorderUi.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QFile>
#include <QtCore>
#include <qfiledialog.h>
#include <QMenuBar>
#include <QProgressBar>

class QComboBox;

class RecorderWindow : public WindowBase
{
    Q_OBJECT

public:
    RecorderWindow(QObject *);
    ~RecorderWindow();

    bool init();

private:
    void createMenuBar();
    void createWidgets();
    void createComboBox();
    void createLabels();
    void createButtons();
    void createEvents();

private slots:
    void deviceChanged(int);
    void encoderChanged(int);
    void bitrateChanged(int);
    void initEqualizer();
    void update(RecorderStream::Event, QVariant = QVariant());
    void update(QWORD, DWORD);
    void startedStream();
    void endOfStream();
    void displayRecordings();
    void displayError(QString);

signals:
    void showError(QString);
    void showNotification(QString);

private:
    QObject *parentMain;
    RecorderStream *recorderStream;
    RecordList *recordList;
    RecorderUi m_ui;
};
