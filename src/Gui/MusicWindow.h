/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
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
#include "MusicUi.h"

#include "WindowBase.h"

#include <OmicronTK11/Qt/DropArea.hpp>
#include <OmicronTK11/Qt/VolumeControl.hpp>

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

class MusicWindow : public WindowBase
{
    Q_OBJECT

public:
    MusicWindow(QObject *);
    ~MusicWindow();

    bool init();

private:
    void createWidgets();
    void createLabels();
    void createButtons();
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
    MusicUi m_ui;
    MusicStream *musicStream;
    MusicPlaylist *playlist;
    bool notDisableButtons, updateListStyle;
};

