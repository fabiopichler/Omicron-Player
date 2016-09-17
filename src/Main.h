/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Core/Database.h"
#include "Core/SingleApp.h"
#include "Core/Global.h"
#include "Core/Update.h"
#include "Core/Theme.h"
#include "Gui/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QFontDatabase>
#include <QSystemTrayIcon>

class Main : public QObject
{
    Q_OBJECT

public:
    Main();
    ~Main();
    bool init(const int &argc);

public slots:
    void startMusicMode();
    void startRadioMode();
    void startRecorderMode();
    void startServerMode();
    void setWindowTitle(QString);
    void showError(QString);
    void showNotification(QString);
    void openSite();
    void about();
    void initConfigDialog();

private:
    void setupRadiolist();
    void updateTrayIconMenu();

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason);
    void checkUpdate();
    void receiveMessage(QStringList);
    void defaultConfig();

signals:
    void openMusic(QStringList);
    void addMusic(QStringList);
    void playStream();
    void pauseStream();
    void stopStream();
    void prevStream();
    void nextStream();

public:
    UpdateApp *updateApp;
    bool continueRunning;

private:
    QSystemTrayIcon *trayIcon;
    QSettings *iniSettings;
    MainWindow *window;
};
