/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/MusicStream.h"
#include "../Core/Global.h"
#include "../Tools/MusicPlaylistManager.h"
#include "../Tools/Equalizer.h"
#include "DropArea.h"
#include "ErrorWindow.h"
#include "VolumeControl.h"

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

class MusicWindow : public DropArea
{
    Q_OBJECT

public:
    MusicWindow(QObject *, QWidget *);
    ~MusicWindow();

private:
    void createMenuBar();
    void createWidgets();
    void createLabels();
    void createButtons();
    void createLayouts();
    void createEvents();
    void changeRepeatStyle(const short &);
    void changeRandomStyle(const bool &);
    void loadPlaylist(int plMode, const bool &disableCdMode = false, const bool &load = true,
                        const bool &stop = true, const QString &playlistName = QString());
    QStringList fileDialog();

private slots:
    void initEqualizer();
    void initPlaylist(bool play = false);
    void playNewMusic(QStringList);
    void openMusic(QStringList list = QStringList());
    void addMusic(QStringList list = QStringList());
    void openDirectory();
    void addDirectory(bool clearPl = false);
    void openCD();
    void changeFavorite();
    void showContextMenu(const QPoint &);
    void clearPlaylist();
    void totals(QWORD);
    void updatePlaylistStyle(bool arg = false);
    void update(MusicStream::Event, QVariant value = QVariant());
    void update(QWORD, DWORD);
    void threadFinished();
    void displayError(QString);

signals:
    void showError(QString);
    void showNotification(QString);

private:
    QObject *parentMain;
    QWidget *uiWidget;
    MusicStream *musicStream;
    MusicPlaylist *playlist;
    QAction *openMusicAction,*addMusicAction,*openDirAction,*addDirAction,*openCDAction,*openPlaylistAction,
        *clearPlaylistAction,*exitAction,*radioModeAction,*recorderModeAction,*serverModeAction,
        *playlistAction,*equalizerAction,*checkUpdateAction,*websiteAction, *facebookAction, *aboutAction,
        *configAction;
    QPushButton *playButton,*pauseButton,*stopButton,*prevButton,*nextButton,*repeatButton,*randomButton,
        *playlistButton,*playlistModeButton,*musicModeButton,*favoriteButton, *changeFavoriteButton;
    QLabel *timeLabel,*totalTimeLabel,*currentTrackLabel,*totalTracksLabel,*currentTagLabel,
        *fileTypeLabel;
    QSlider *timeSlider;
    QProgressBar *leftChProgressBar, *rightChProgressBar;
    QLineEdit *searchLineEdit;
    VolumeControl *volumeControl;
    ErrorWindow *errorWindow;
    bool notDisableButtons, updateListStyle;
};

