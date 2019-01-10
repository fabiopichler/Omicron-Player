/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "NetPlaylist.h"

NetPlaylist::NetPlaylist()
{
    finished = false;
    manager = new QNetworkAccessManager(this);
    connect(this, SIGNAL(startCheck(QString)), this, SLOT(startChecking(QString)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

NetPlaylist::~NetPlaylist()
{
    delete manager;
}

void NetPlaylist::getUrl(QString &link, const bool &stop, const bool &play)
{
    if (link.contains(".xspf"))
        type = "xspf";

    else if (link.contains(".wpl"))
        type = "wpl";

    else if (link.contains(".asx"))
        type = "asx";

    emit startCheck(link);
    while (!finished && !stop && play)
        QThread::msleep(10);

    link = url;
    url.clear();
    type.clear();
    finished = false;
}

//================================================================================================================
// private
//================================================================================================================

QString NetPlaylist::xspf(const QString &s)
{
    QXmlStreamReader reader(s);
    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name().contains("location", Qt::CaseInsensitive))
            {
                reader.readNext();

                if(reader.atEnd())
                    break;

                if(reader.isCharacters())
                {
                    return reader.text().toString();
                }
            }
        }

        reader.readNext();
    }
    return QString();
}

QString NetPlaylist::wpl(const QString &s)
{
    QXmlStreamReader reader(s);
    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name().contains("media", Qt::CaseInsensitive))
            {
                QString s;
                QXmlStreamAttributes attributes = reader.attributes();
                s = attributes.value("src").toString();

                if (s.isEmpty())
                    s = attributes.value("SRC").toString();

                if (s.isEmpty())
                    s = attributes.value("Src").toString();

                return s;
            }
        }
        reader.readNext();
    }
    return QString();
}

QString NetPlaylist::asx(const QString &s)
{
    QString t;
    QXmlStreamReader reader(s);
    while (!reader.atEnd())
    {
        if (reader.isStartElement())
        {
            if (reader.name().contains("ref", Qt::CaseInsensitive))
            {
                QXmlStreamAttributes attributes = reader.attributes();
                t = attributes.value("href").toString();

                if (t.isEmpty())
                    t = attributes.value("HREF").toString();

                if (t.isEmpty())
                    t = attributes.value("Href").toString();

                break;
            }
        }
        reader.readNext();
    }

    if (t.isEmpty())
    {
        QStringList list = s.split("\r\n");
        for (int i = 0; i < list.length(); i++)
        {
            if (QUrl(list[i]).isValid())
                return list[i];
        }
    }

    return t;
}

//================================================================================================================
// private slots
//================================================================================================================

void NetPlaylist::startChecking(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    manager->get(request);
}

void NetPlaylist::replyFinished(QNetworkReply *reply)
{
    QString status("HTTP ");
    status += reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() + " ";
    status += reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    emit updateStatus(status);

    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (code == 200)
    {
        QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

        if (type == "xspf" || contentType.contains("application/xspf+xml"))
            url = xspf(reply->readAll());

        else if (type == "wpl" || contentType.contains("application/vnd.ms-wpl"))
            url = wpl(reply->readAll());

        else if (type == "asx" || contentType.contains(QRegExp("video/x-ms-asf|audio/x-mpegurl")))
            url = asx(reply->readAll());

        finished = true;
    }
    else if (code == 301 || code == 302)
    {
        startChecking(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString());
    }
    else
    {
        qDebug() << "Error downloading the playlist! Code: " << code;
        finished = true;
    }

    reply->deleteLater();
}
