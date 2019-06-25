/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

/*!
 *
 * \author Fábio Pichler
 * \copyright (c) 2015-2019, Fábio Pichler
 *
 */

#pragma once

#include "Core/Database.h"
#include "Core/SingleApp.h"
#include "Core/Global.h"
#include "Core/Update.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QFontDatabase>
#include <QSystemTrayIcon>

//! Classe responsável pelo processamento inicial e principal do programa.
/*!
 * Esta classe é responsável por inicializar a biblioteca BASS, plugins, Banco de Dados,
 * arquivos de configurações e etc.
 *
 * Também é responsável pelo processamento principal e por finalizar o programa.
 */

class WindowBase;

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
    void setWindowTitle(QString);
    void showError(QString);
    void showNotification(QString);
    void restart();

private:
    void setupRadiolist();
    void updateTrayIconMenu();

private slots:

    void trayIconActivated(QSystemTrayIcon::ActivationReason);
    void checkUpdate();
    void receiveMessage(QVector<QString>);
    void defaultConfig();

signals:
    //! Abrir músicas no "modo música"
    void openMusic(QVector<QString>);

    //! Adiciona músicas ao playlist do "modo músicas"
    void addMusic(QVector<QString>);

    //! Reproduzir o stream.
    void playStream();

    //! Pausa o stream atual.
    void pauseStream();

    //! Para o stream atual.
    void stopStream();

    //! Stream anteiror.
    void prevStream();

    //! Próximo stream.
    void nextStream();

public:
    UpdateApp *updateApp;
    bool continueRunning;

private:
    QSystemTrayIcon *trayIcon;
    QSettings *iniSettings;
    WindowBase *window;
};
