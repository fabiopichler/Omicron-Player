/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

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
    void playNewMusic(QStringList);
    void startTagTimer(int);
    void stopTagTimer();
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
    QWidget *parent;
    QTimer *timerTag, *cdTimer;
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
    int &getCurrentIndex();
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
