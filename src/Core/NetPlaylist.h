/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QtXml>

class NetPlaylist : public QObject
{
    Q_OBJECT

public:
    NetPlaylist();
    ~NetPlaylist();
    void getUrl(QString &, const bool &, const bool &);

private:
    QString xspf(const QString &);
    QString wpl(const QString &);
    QString asx(const QString &);

private slots:
    void startChecking(QString);
    void replyFinished(QNetworkReply *);

signals:
    void startCheck(QString);
    void updateStatus(QVariant);

private:
    QNetworkAccessManager *manager;
    QString url, type;
    QString newURL;
    bool finished;
};
