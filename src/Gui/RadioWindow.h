/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"
#include "../Core/RadioStream.h"
#include "../Core/Global.h"
#include "../Gui/DialogBase.h"
#include "../Tools/RadioPlaylistManager.h"
#include "ErrorWindow.h"
#include "Widget.h"

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

class RadioWindow : public Widget
{
    Q_OBJECT

public:
    RadioWindow(QObject *, QWidget *);
    ~RadioWindow();

private:
    void createMenuBar();
    void createWidgets();
    void createLabels();
    void createButtons();
    void createLayouts();
    void createEvents();
    void changePlaylist(int);

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
    QWidget *parent;
    Widget *infoWidget;
    RadioStream *radioStream;
    RadioPlaylist *playlist;
    QMenu *fileMenu, *modeMenu, *toolsMenu, *settingsMenu, *aboutMenu;
    QMenuBar *menuBar;
    QAction *openLinkAction, *displayRecordingsAction, *exitAction, *musicModeAction, *playlistManagerAction,
        *equalizerAction, *configAction, *checkUpdateAction, *websiteAction, *aboutAction;
    QPushButton *playButton, *stopButton, *prevButton, *nextButton, *recordButton, *openLinkButton, *playlistButton,
        *changeFavoriteButton, *allPlaylistsButton, *customPlaylistButton, *favoriteButton;
    QLabel *timeLabel, *statusLabel, *nameLabel, *streamTitleLabel, *loaderLabel;
    QMovie *loaderMovie;
    QProgressBar *leftChProgressBar, *rightChProgressBar, *bufferProgressBar;
    QLineEdit *searchLineEdit;
    ErrorWindow *errorWindow;
    bool showLoader;
};

class ShowRadioInfo : public DialogBase
{
    Q_OBJECT

public:
    ShowRadioInfo(QWidget *, QStringList radio);
    ~ShowRadioInfo();
};
