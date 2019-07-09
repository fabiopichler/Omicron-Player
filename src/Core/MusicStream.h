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

#include "Global.h"
#include "Database.h"
#include "Stream.h"
#include "Fade.h"
#include "../Tools/MusicPlaylistManager.h"
#include "../Tools/Equalizer.h"

#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <QtCore>
#include <bass.h>
#include <bass_fx.h>
#include <tags.h>
#include <qfiledialog.h>

class MusicPlaylist;

class MusicStream : public Stream
{
    Q_OBJECT

public:
    enum Event
    {
        CurrentTag,
        CurrentSound,
        PlaylistLength,
        FileTypeLabel
    };

    static MusicStream *instance() { return self; }

    MusicStream(QWidget *);
    ~MusicStream();
    void setupCDMode(const bool &, const int &drive = -1);
    void loadPlaylist(const int &, const bool &disableCdMode = false, const bool &load = true,
                      const bool &_stop = true, QString listName = QString());

public slots:
    void play();
    void pause();
    void stop();
    void prev();
    void next();
    short changeRepeat();
    bool changeRandom();
    void playNewMusic(QVariant);

private slots:
    void updateTag();
    void updateSoundPosition();
    void updateCDMode();
    void createFade();

private:
    void createEvents();
    void updateTrackList();
    void run() Q_DECL_OVERRIDE;

signals:
    void updateValue(MusicStream::Event, QVariant value = QVariant());
    void updateInfo(QWORD, DWORD);
    void updatePlaylistStyle();
    void setTotals(QWORD);
    void threadFinished();
    void playButtonEnabled(bool);
    void pauseButtonEnabled(bool);
    void stopButtonEnabled(bool);
    void initPlaylist(bool);
    void playNewMusic(QVector<QString>);
    void startTagTimer(int);
    void stopTagTimer();
    void startSoundPositionTimer(int);
    void stopSoundPositionTimer();
    void startCDTimer(int);
    void stopCDTimer();
    void showNotification(QString);
    void showError(QString);
    void showErrorDlg(QString);
    void newFade();

public:
    bool isMusic, isCDMode;
    int playlistMode;
    MusicPlaylist *playlist;
    static QStringList musicPlaylist;

private:
    static MusicStream *self;
    QWidget *parent;
    QTimer *timerTag, *soundPositionTimer, *cdTimer;
    Fade *fade;
    QStringList tagList;
    short tagListCount;
    bool newSound, random;
    int repeat;
    static int currentDrive;
};

class MusicPlaylist : public QTreeView
{
    Q_OBJECT

public:
    MusicPlaylist(QWidget *, const int &, const int &, const bool &, bool &);
    ~MusicPlaylist();
    void play();
    void prev();
    void next();
    void addRow(QString);
    void clear();
    void removeRow(const int &);
    int length();
    bool isEmpty();
    void setCurrentIndex(int);
    int getCurrentIndex();
    QString getCurrentTitle();
    QString getCurrentFile();
    QString getRow(const int &);

public slots:
    void selectRow(int);
    void textFilterChanged(QString);

private:
    bool random();
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

signals:
    void selectRowSignal(int);
    void playNewMusic(QVariant);
    void updateValue(MusicStream::Event, QVariant);

private:
    QStandardItemModel* model;
    QSortFilterProxyModel *proxyModel;
    QWidget *parent;
    int currentIndex[4];
    const int &playlistMode;
    const int &mrepeat;
    const bool &mrandom;
    bool &mstop, filteredText;
};
