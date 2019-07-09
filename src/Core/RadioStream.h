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
#include "Stream.h"
#include "NetPlaylist.h"
#include "../Tools/Equalizer.h"

#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <QtCore>
#include <bass.h>
#include <bass_fx.h>
#include <qfiledialog.h>
#include <QTreeView>
#include <QStandardItemModel>

class RadioPlaylist;
class NetworkStream;

class RadioStream : public Stream
{
    Q_OBJECT

public:
    enum Event
    {
        StatusLabel,
        NameLabel,
        StreamTitleLabel,
        BufferProgressBar,
        ShowLoaderMovie,
        RadioName,
        Recording,
        WebSearch
    };

    static RadioStream *instance() { return self; }

    RadioStream(QWidget *);
    ~RadioStream();
    void loadPlaylist(const int &arg = 2);
    void openLink(const QString &);
    void doMeta();

public slots:
    void play();
    void stop(bool force = false);
    void prev();
    void next();
    void record();
    void stopRecord();
    void playNewRadio(int, int selected = 0);
    void updateStatus();
    void webSearch();

private slots:
    void newConnectionSlot();
    void statusProc(const void *buffer, DWORD length);

private:
    void createEvents();
    bool startRecord();
    bool buffering(int &);
    void run() Q_DECL_OVERRIDE;

signals:
    void updateValue(RadioStream::Event, QVariant);
    void updateInfo(QWORD, DWORD, DWORD, DWORD);
    void threadFinished(bool, bool);
    void playButtonEnabled(bool);
    void recordButtonEnabled(bool);
    void stopButtonEnabled(bool);
    void startStatusTimer(int);
    void stopStatusTimer();
    void startMetaTimer(int);
    void stopMetaTimer();
    void showNotification(QString);
    void showError(QString);
    void showErrorDlg(QString);
    void newConnection();

public:
    bool mrecord, isQuickLink;
    int playlistMode;
    RadioPlaylist *playlist;

private:
    static RadioStream *self;
    QWidget *parent;
    NetworkStream *connection;
    bool newRadio, mnext;
    QString bitrate, status;
    QTimer *statusTimer, *metaTimer;
    QStringList statusList;
    short statusListCount;
    bool iswma;
    QString fileType, recordFileName, quickLink, musicName;
    QFile *recordFile;
    int selectedUrl, reconnect;
    double recordTime;
};

//================================================================================================================
// class NetworkStream
//================================================================================================================

class NetworkStream : public QThread
{
    Q_OBJECT

public:
    NetworkStream();
    ~NetworkStream();

    void create(const QString &);
    bool connecting(const bool &, const bool &);
    int completed();
    const HSTREAM &getStream() const;

    void fadeIn(const int &);
    void fadeOut();

private:
    void run() Q_DECL_OVERRIDE;
    static void CALLBACK statusProc(const void *buffer, DWORD length, void *user);

signals:
    void statusProcSignal(const void *buffer, DWORD length);

private:
    HSTREAM stream;
    QMutex mutex;
    long waitTimeout;
    int errorCode;
    bool mstop, complete, isFadeOut;
    QString streamUrl;
    NetPlaylist netPlaylist;
};

//================================================================================================================
// class RadioPlaylist
//================================================================================================================

class RadioPlaylist : public QTreeView
{
    Q_OBJECT

public:
    RadioPlaylist(QWidget *, const int &);
    ~RadioPlaylist();
    void play();
    void prev();
    void next();
    void addRow(const QStringList &);
    void clear();
    int length();
    bool isEmpty();
    void setCurrentIndex(const int &arg);
    int &getCurrentIndex();
    void removeRow(const int &);
    QStringList getRow(const int &);
    QString getRadioName(const int &);
    QString getRadioUrl(const int &);
    QStringList getAllUrls(const int &);
    QString getCurrentTitle();
    QString getCurrentUrl(const int &url = 0);

public slots:
    void selectRow(int);
    void textFilterChanged(QString);

private:
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

signals:
    void updateValue(RadioStream::Event, QVariant);
    void selectRowSignal(int);
    void playNewRadio(int);

private:
    QStandardItemModel* model;
    QSortFilterProxyModel *proxyModel;
    QWidget *parent;
    const int &playlistMode;
    int currentIndex[4];
    bool filteredText;
};

