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

#ifndef Q_OS_ANDROID
    #include <basscd.h>
#endif

class MusicPlaylist;

class MusicStream : public StreamBase
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

    MusicStream(QWidget *);
    ~MusicStream();
    void setupCDMode(const bool &, const int &drive = -1);
    void loadPlaylist(const int &, const bool &disableCdMode = false, const bool &load = true,
                      const bool &_stop = true, QString listName = QString());
    double getPosition();

public slots:
    void play();
    void pause();
    void stop();
    void prev();
    void next();
    short changeRepeat();
    bool changeRandom();
    void playNewMusic(QVariant);
    void setPosition(int);

private slots:
    void updateTag();
    void updateCDMode();

private:
    void createEvents();
    void updateTrackList();
    void run();

signals:
    void updateValues(MusicStream::Event, QVariant value = QVariant());
    void updateValues(QWORD, DWORD);
    void updatePlaylistStyle();
    void setTotals(QWORD);
    void threadFinished();
    void playButtonEnabled(bool);
    void pauseButtonEnabled(bool);
    void stopButtonEnabled(bool);
    void initPlaylist(bool);
    void playNewMusic(QStringList);
    void startTagTimer(int);
    void stopTagTimer();
    void startCDTimer(int);
    void stopCDTimer();
    void showNotification(QString);
    void showError(QString);
    void showErrorDlg(QString);

public:
    bool isMusic, isCDMode;
    int playlistMode;
    MusicPlaylist *playlist;
    static QStringList musicPlaylist;

private:
    QWidget *parent;
    QTimer *timerTag, *cdTimer;
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
    int &getCurrentIndex();
    QString getCurrentTitle();
    QString getCurrentFile();
    QString getRow(const int &);

public slots:
    void selectRow(int);
    void textFilterChanged(QString);

private:
    bool random();
    void keyPressEvent(QKeyEvent *);

signals:
    void selectRowSignal(int);
    void playNewMusic(QVariant);
    void updateValues(MusicStream::Event, QVariant);

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
