/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"
#include "../Core/RadioStream.h"
#include "../Core/Global.h"
#include "../Tools/RadioPlaylistManager.h"

#include "WindowBase.h"
#include "RadioUi.hpp"

#include <OmicronTK11/Qt/Dialog.hpp>
#include <OmicronTK11/Qt/VolumeControl.hpp>

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

class ShowRadioInfo : public OmicronTK11::OTKQT::Dialog
{
    Q_OBJECT

public:
    ShowRadioInfo(QWidget *, QStringList radio);
    ~ShowRadioInfo();
};
