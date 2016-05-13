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

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QDesktopServices>
#include <QSettings>

class UpdateApp : public QObject
{
    Q_OBJECT

public:
    UpdateApp(QObject *parent, QSettings *iniSettings);
    ~UpdateApp();

public slots:
    void startChecking(const bool &arg = false);

private:
    void downloadPlaylist(const QUrl &);

private slots:
    void readyRead();
    void finishedChecking(QNetworkReply *reply);
    void downloadFinished();

signals:
    void showNotification(QString);

private:
    QSettings *iniSettings;
    QTimer *startTimer;
    QNetworkAccessManager *updateManager, *downloadManager;
    QNetworkReply *downloadReply;
    QString newURL, newPlaylistDate, newPlaylistName;
    QFile *playlistFile;
    bool alert, blockRequest, requestAborted;
    int currentDay;
};
