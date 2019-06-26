/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Update.h"
#include "../Core/Database.h"
#include <iostream>
#include <QFileInfo>
#include <QProcess>

UpdateApp::UpdateApp(QObject *parent, QSettings *iniSettings) : QObject(parent)
{
    this->iniSettings = iniSettings;
    blockRequest = false;
    playlistFile = nullptr;
    currentDate = QDate::currentDate();

    updateManager = new QNetworkAccessManager(this);
    downloadManager = new QNetworkAccessManager(this);
    startTimer = new QTimer(this);
    startTimer->setSingleShot(true);

    connect(updateManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedChecking(QNetworkReply*)));
    connect(startTimer, SIGNAL(timeout()), this, SLOT(startChecking()));
    connect(this, SIGNAL(showNotification(QString)), parent, SLOT(showNotification(QString)));

    int checkUpdate = Database::value("Version", "updates_check", 1).toInt();

    if (checkUpdate == 0)
        return;
    else if (checkUpdate == 2)
        checkUpdate = 7;
    else if (checkUpdate != 1)
        checkUpdate = 1;

    QDate lastCheck(QDate::fromString(Database::value("Version", "updates_lastCheck").toString(), "yyyy-MM-dd"));
    lastCheck = lastCheck.addDays(checkUpdate);

    if (lastCheck <= currentDate)
        startTimer->start(1000);
}

UpdateApp::~UpdateApp()
{
    requestAborted = true;

    delete updateManager;
    delete downloadManager;
    delete startTimer;

    if (playlistFile)
    {
        playlistFile->close();
        playlistFile->remove();
        delete playlistFile;
    }
}

//================================================================================================================
// public slots
//================================================================================================================

void UpdateApp::startChecking(const bool &arg)
{
    stdCout("Checking for updates...");

    if (arg)
        emit showNotification("Verificando por atualizações. Aguarde...");

    alert = arg;
    QString p("?version="+CurrentVersion);

    QNetworkRequest request;
    request.setUrl(QUrl((newURL.isEmpty() ? CheckUpdate+p : (newURL.contains("version=") ? newURL : newURL+p))));
    request.setRawHeader("User-Agent", QString(AppNameId+"-"+CurrentVersion).toLocal8Bit());

    updateManager->get(request);
}

//================================================================================================================
// private
//================================================================================================================

void UpdateApp::downloadPlaylist(const QUrl &url)
{
    QString fileName = "RadioList_" + newPlaylistDate + ".7z";

    playlistFile = new QFile(Global::getConfigPath(fileName));

    if (!playlistFile->open(QIODevice::WriteOnly))
    {
        qDebug() << "Error: " << playlistFile->errorString();
        delete playlistFile;
        playlistFile = nullptr;
        return;
    }

    requestAborted = false;
    blockRequest = true;

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", QString(AppNameId+"-"+CurrentVersion).toLocal8Bit());

    downloadReply = downloadManager->get(request);

    connect(downloadReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(downloadReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

//================================================================================================================
// private slots
//================================================================================================================

void UpdateApp::readyRead()
{
    if (playlistFile)
        playlistFile->write(downloadReply->readAll());
}

void UpdateApp::finishedChecking(QNetworkReply *reply)
{
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray data = reply->readAll();

    if (code == 200)
    {
        QJsonObject json = QJsonDocument::fromJson(data).object();
        QJsonObject app = json.value("app").toObject();
        QJsonObject radioPlaylist = json.value("radioPlaylist").toObject();

        QString version = app["currentVersion"].toString();
        newPlaylistName = radioPlaylist["fileName"].toString();
        newPlaylistDate = radioPlaylist["date"].toString();
        QUrl radiolistUrl = radioPlaylist["url"].toString();

        if (!version.isEmpty())
        {
            stdCout("Checking update is completed");
            Database::setValue("Version", "updates_lastCheck", currentDate.toString("yyyy-MM-dd"));

            QDate listInstalled(QDate::fromString(iniSettings->value("Radiolist/Date").toString(), "yyyy-MM-dd"));
            QDate dateChecked(QDate::fromString(newPlaylistDate, "yyyy-MM-dd"));

            if (!blockRequest && Database::value("Config", "autoDlRadioList").toBool()
                                                 && !radiolistUrl.isEmpty() && listInstalled < dateChecked)
                downloadPlaylist(radiolistUrl);

            if (version == CurrentVersion && alert)
            {
                QMessageBox::information(nullptr,"Info",QString("<h3>O %1 está atualizado.</h3>Versão atual: %2").arg(AppName,version));
            }
            else if (version != CurrentVersion)
            {
                stdCout("New version available: " + version + "\n");

                if (QMessageBox::warning(nullptr,"Info",QString("<h2>Nova versão disponível</h2>Uma nova versão do <strong>%1</strong> "
                                                           "está disponível,<br>é altamente recomendável que instale a atualização,"
                                                           "<br>pois contém várias melhorias e correções de bugs.<br><br>"
                                                           "<strong>Versão instalada:</strong> %2<br>"
                                                           "<strong>Nova versão:</strong> %3<br><br>Deseja acessar a internet e "
                                                           "baixar a última versão?").arg(AppName,CurrentVersion,version),
                                                           QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
                    QDesktopServices::openUrl(QUrl(DownloadUpdate));
            }

            reply->deleteLater();
            return;
        }
    }
    else if (code == 301 || code == 302)
    {
        newURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        startChecking(alert);
        reply->deleteLater();
        return;
    }

    stdCout("Unable to check for updates. Code: " + QString::number(code));
    reply->deleteLater();

    if (alert)
        QMessageBox::warning(nullptr,"Info",QString("Não foi possível verificar se há atualizações.\nVerifique sua conexão à "
                                               "internet e tente novamente.\nCódigo do erro: %1").arg(code));
}

void UpdateApp::downloadFinished()
{
    int code = downloadReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    playlistFile->flush();
    playlistFile->close();

    if (!requestAborted && downloadReply->error() == QNetworkReply::NoError)
    {
        QString args("\""+Global::getAppPath()+"7zr\" x -y \""+playlistFile->fileName()
                     +"\" -o\""+Global::getConfigPath()+"\"");

        if (QProcess::execute(args) == 0)
        {
            iniSettings->setValue("Radiolist/NewFileName", newPlaylistName);
            iniSettings->setValue("Radiolist/NewDate", newPlaylistDate);
            emit showNotification("A Lista de Rádios foi atualizada e\nserá ativada, após reiniciar o programa.");
        }
        else
        {
            qDebug() << "Erro ao Extrair o novo RadioList";
        }
    }

    downloadReply->deleteLater();
    playlistFile->remove();
    delete playlistFile;
    playlistFile = nullptr;

    if (!requestAborted && (code == 301 || code == 302))
    {
        downloadPlaylist(downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());
        return;
    }

    blockRequest = false;
}
