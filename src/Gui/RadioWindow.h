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

#include "../Core/Database.h"
#include "../Core/RadioStream.h"
#include "../Core/Global.h"
#include "../Tools/RadioPlaylistManager.h"

#include "WindowBase.h"
#include "RadioUi.hpp"

#include <OmicronTK/Qt/Dialog.hpp>
#include <OmicronTK/Qt/VolumeControl.hpp>

#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QFile>
#include <QIODevice>
#include <QtCore>
#include <qfiledialog.h>
#include <QMenuBar>
#include <QProgressBar>
#include <QMovie>

class RadioWindow : public WindowBase
{
    Q_OBJECT

public:
    RadioWindow(QObject *);
    ~RadioWindow();

    bool init();

private:
    void createWidgets();
    void createLabels();
    void createButtons();
    void createEvents();
    void changePlaylist(int);
    void changeEvent(QEvent *);

private slots:
    void initEqualizer();
    void initPlaylist();
    void openLink();
    void changeFavorite();
    void showContextMenu(const QPoint &);
    void updatePlaylistStyle();
    void update(RadioStream::Event, QVariant);
    void update(QWORD, DWORD, DWORD, DWORD);
    void threadFinished(bool, bool);
    void displayRecordings();
    void displayError(QString);

signals:
    void showError(QString);
    void showNotification(QString);

private:
    friend class RadioStream;
    QObject *parentMain;
    RadioStream *radioStream;
    RadioPlaylist *playlist;
    RadioUi m_ui;
    bool showLoader;
};

class ShowRadioInfo : public OmicronTK::OTKQT::Dialog
{
    Q_OBJECT

public:
    ShowRadioInfo(QWidget *, QStringList radio);
    ~ShowRadioInfo();
};
