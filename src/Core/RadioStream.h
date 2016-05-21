/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Global.h"
#include "Database.h"
#include "StreamBase.h"
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

class RadioStream : public StreamBase
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
        Recording
    };

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

private:
    void createEvents();
    bool startRecord();
    static void CALLBACK statusProc(const void *buffer, DWORD length, void *user);
    bool buffering(int &);
    void showTimedout();
    void run();

signals:
    void updateValues(RadioStream::Event, QVariant);
    void updateValues(QWORD, DWORD, DWORD, DWORD);
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

public:
    bool mrecord, isQuickLink;
    int playlistMode;
    RadioPlaylist *playlist;

private:
    QWidget *parent;
    bool newRadio, mnext;
    QString bitrate, status;
    QTimer *statusTimer, *metaTimer;
    QStringList statusList;
    short statusListCount;
    NetPlaylist netPlaylist;
    bool iswma;
    QString fileType, recordFileName, quickLink;
    QFile *recordFile;
    int selectedUrl, reconnect;
    double recordTime;
};

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
    void keyPressEvent(QKeyEvent *);

signals:
    void updateValues(RadioStream::Event, QVariant);
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

