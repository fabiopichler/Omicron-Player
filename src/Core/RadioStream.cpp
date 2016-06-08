/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RadioStream.h"
#include "iostream"
#include <QElapsedTimer>

RadioStream::RadioStream(QWidget *parent) : StreamBase()
{
    this->parent = parent;

    mprev = false;
    mnext = false;
    newRadio = false;
    isQuickLink = false;
    iswma = false;
    mrecord = false;
    stopFadeOut = false;
    fade = nullptr;
    recordFile = nullptr;
    recordTime = 0;
    selectedUrl = 0;
    reconnect = 0;

    playlist = new RadioPlaylist(parent, playlistMode);
    statusTimer = new QTimer(this);
    metaTimer = new QTimer(this);

    playlistMode = Database::value("RadioMode", "playlistMode").toInt();
    QString recordPath = Database::value("Config", "recordPath").toString();

    if (!QDir().exists(recordPath))
        QDir().mkdir(recordPath);

    createEvents();
}

RadioStream::~RadioStream()
{
    stopFadeOut = true;
    stop(true);

    delete statusTimer;
    delete metaTimer;
}

void RadioStream::loadPlaylist(const int &arg)
{
    playlist->clear();

    if (arg == 0)
    {
        QList<QStringList> list = Database::getRadioFavorites();

        for (int i = 0; i < list.length(); i++)
        {
            list[i].push_back("");
            playlist->addRow(list[i]);
        }

        return;
    }

    QString playlistPath = Database::value("RadioMode", "playlist").toString();

    if (playlistPath.isEmpty())
        playlistPath = Global::getConfigPath("RadioList."+RadioPlaylistExt);

    for (int i = 0; i < arg; i++)
    {
        QFile file(i == 0 ? playlistPath : Global::getRadiolistPath());
        if (!file.exists())
            continue;

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");

            while (!stream.atEnd())
            {
                QStringList list;
                QRegExp regExp("<title>(.*)<desc>(.*)<genre>(.*)<contry>(.*)<url1>(.*)<url2>(.*)<url3>(.*)<url4>(.*)<url5>(.*)<turl>");

                if (regExp.indexIn(stream.readLine()) < 0)
                    continue;

                list << regExp.cap(1);
                list << regExp.cap(3);
                list << regExp.cap(4);
                list << regExp.cap(5);
                list << regExp.cap(6);
                list << regExp.cap(7);
                list << regExp.cap(8);
                list << regExp.cap(9);
                list << regExp.cap(2);

                playlist->addRow(list);
            }
            file.close();
        }
        else
        {
            QMessageBox::warning(nullptr, "Erro!", "Erro ao abrir o Playlist!");
        }
    }
}

void RadioStream::openLink(const QString &arg)
{
    quickLink = arg;

    if (isRunning())
        mplay = false;
    else
        play();
}

void RadioStream::doMeta()
{
    QString text;
    if (iswma)
    {
        const char *meta=BASS_ChannelGetTags(stream, 11/*BASS_TAG_WMA_META*/);
        if (meta)
        {
            text = meta;
            text.remove("caption=", Qt::CaseInsensitive);
        }
    }
    else
    {
        const char *meta=BASS_ChannelGetTags(stream,BASS_TAG_META);
        if (meta)
        {
            const char *p=strstr(meta,"StreamTitle='");
            if (p)
            {
                const char *p2=strstr(p,"';");
                if (p2)
                {
                    char *t=_strdup(p+13);
                    t[p2-(p+13)]=0;
                    text = Global::cStrToQString(t);
                    delete t;
                }
            }
        }
        else
        {
            meta=BASS_ChannelGetTags(stream,BASS_TAG_OGG);
            if (meta)
            {
                const char *artist=NULL,*title=NULL,*p=meta;
                for (;*p;p+=strlen(p)+1)
                {
                    if (!strncasecmp(p,"artist=",7))
                        artist=p+7;

                    if (!strncasecmp(p,"title=",6))
                        title=p+6;
                }

                if (title && artist && strcmp(title, "") != 0 && strcmp(artist, "") != 0)
                    text = Global::cStrToQString(artist) + " - " + Global::cStrToQString(title);

                else if (title && strcmp(title, "") != 0)
                    text = Global::cStrToQString(title);

                else if (artist && strcmp(artist, "") != 0)
                    text = Global::cStrToQString(artist);
            }
        }
    }
    emit updateValues(StreamTitleLabel, (text.isEmpty() ? "---" : text));
}

//================================================================================================================
// public slots
//================================================================================================================

void RadioStream::play()
{
    emit playButtonEnabled(false);
    emit stopButtonEnabled(true);

    if (!isRunning())
    {
        playlist->play();
        start();
    }
}

void RadioStream::stop(bool force)
{
    if (isRunning() && (!mrecord || force))
    {
        mstop = true;
        wait();
    }
    if (mrecord)
        stopRecord();
}

void RadioStream::prev()
{
    if (isRunning())
    {
        mprev = true;
        mplay = false;
    }
    else
    {
        selectedUrl = 0;
        playlist->prev();
        emit updateValues(RadioName, playlist->getCurrentIndex());
    }
}

void RadioStream::next()
{
    if (isRunning())
    {
        mnext = true;
        mplay = false;
    }
    else
    {
        selectedUrl = 0;
        playlist->next();
        emit updateValues(RadioName, playlist->getCurrentIndex());
    }
}

void RadioStream::record()
{
    if (iswma)
        return;

    stopRecord();
    emit recordButtonEnabled(false);

    if (isRunning())
    {
        if (fileType != "MP3" && fileType != "AAC")
        {
            stop(true);
            mrecord = true;
            play();
        }
        else
        {
            mrecord = true;
        }
    }
    else
    {
        mrecord = true;
        play();
    }
}

void RadioStream::stopRecord()
{
    if (recordFile)
    {
        mrecord = false;
        recordTime = 0;
        emit recordButtonEnabled(true);
        emit updateValues(Recording, false);

        recordFile->flush();
        recordFile->close();

        QString path(QFileInfo(recordFile->fileName()).absolutePath());
        recordFile->rename(path + "/Record_" + recordFileName + "." + fileType.toLower());

        delete recordFile;
        recordFile = nullptr;
        recordFileName.clear();
    }
}

void RadioStream::playNewRadio(int row, int selected)
{
    selectedUrl = selected;
    playlist->setCurrentIndex(row);
    emit updateValues(RadioName, row);

    if (isRunning())
    {
        newRadio = true;
        mplay = false;
    }
    else
    {
        play();
    }
}

void RadioStream::updateStatus()
{
    int len = statusList.length();

    if (len > 0)
    {
        if (statusListCount >= len)
            statusListCount = 0;

        emit updateValues(StatusLabel, statusList[statusListCount]);
        statusListCount++;
    }
}

//================================================================================================================
// private slots
//================================================================================================================

void RadioStream::createFade()
{
    fade = new Fade("RadioConfig", stopFadeOut);
}

//================================================================================================================
// private
//================================================================================================================

void RadioStream::createEvents()
{
    connect(playlist, SIGNAL(playNewRadio(int)), this, SLOT(playNewRadio(int)));
    connect(&netPlaylist, &NetPlaylist::updateStatus, [=](QVariant arg) { emit updateValues(StatusLabel, arg); });

    connect(this, SIGNAL(startStatusTimer(int)), statusTimer, SLOT(start(int)));
    connect(this, SIGNAL(stopStatusTimer()), statusTimer, SLOT(stop()));
    connect(statusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));

    connect(this, SIGNAL(startMetaTimer(int)), metaTimer, SLOT(start(int)));
    connect(this, SIGNAL(stopMetaTimer()), metaTimer, SLOT(stop()));
    connect(metaTimer, &QTimer::timeout, [=]() { doMeta(); });

    connect(this, SIGNAL(newFade()), this, SLOT(createFade()), Qt::QueuedConnection);
}

bool RadioStream::startRecord()
{
    QString newRecordPath = Database::value("Config", "recordPath").toString();

    if (Database::value("Config", "recordSubDir").toBool())
    {
        newRecordPath += "/" + (isQuickLink ? "Quick Link"
                                            : playlist->getCurrentTitle().replace(QRegExp("[/:*?\"<>|]"), "-")
                                              .replace("\\", "-").replace(QRegExp("[-]+"), "-")) + "/";
        newRecordPath.replace("//", "/");

        if (!QDir().exists(newRecordPath))
            QDir().mkdir(newRecordPath);
    }

    recordFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH.mm.ss");
    recordFile = new QFile(newRecordPath + "Recording... (" + recordFileName + ")");
    recordTime = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));

    if (!recordFile->open(QIODevice::WriteOnly))
    {
        stopRecord();
        return true;
    }

    emit recordButtonEnabled(false);
    emit updateValues(Recording, true);
    return false;
}

void CALLBACK RadioStream::statusProc(const void *buffer, DWORD length, void *user)
{
    RadioStream *instance = static_cast<RadioStream *>(user);

    if (buffer && !length)
    {
        instance->status = Global::cStrToQString(static_cast<const char *>(buffer));
        emit instance->updateValues(StatusLabel, instance->status);
    }
    else if (instance->mrecord)
    {
        if (!instance->recordFile && buffer && length > 0 && instance->startRecord())
            return;

        if (buffer && length > 0)
            instance->recordFile->write(static_cast<const char *>(buffer), length);

        else if (instance->recordFile)
            instance->stopRecord();
    }
}

bool RadioStream::buffering(int &_progress)
{
    DWORD progress;

    if (iswma)
        progress = BASS_StreamGetFilePosition(stream, 1000/*BASS_FILEPOS_WMA_BUFFER*/);
    else
        progress = BASS_StreamGetFilePosition(stream,BASS_FILEPOS_BUFFER) * 100 / BASS_StreamGetFilePosition(stream,BASS_FILEPOS_END);

    _progress = progress;

    if (progress > 75 || !BASS_StreamGetFilePosition(stream,BASS_FILEPOS_CONNECTED))
    {
        QString s;
        if (iswma)
        {
            const char *tags=BASS_ChannelGetTags(stream, 8/*BASS_TAG_WMA*/);
            for (;*tags;tags+=strlen(tags)+1)
            {
                if (!strncasecmp(tags,"title=",6))
                    s = Global::cStrToQString(tags+6);

                if (s.isEmpty())
                    s = playlist->getCurrentTitle();

                if (!strncasecmp(tags,"author=",7))
                {
                    QString url = QUrl(Global::cStrToQString(tags+7)).toString();

                    if (!url.isEmpty())
                    {
                        if (url.contains(QRegExp("^http[s]?://", Qt::CaseInsensitive)))
                            s += QString(" (<a href=\"%1\">%1</a>)").arg(url);
                        else
                            s += " - " + url;
                    }
                }

                if (!strncasecmp(tags,"bitrate=",8))
                    bitrate = QString::number(atoi(tags+8)/1024);
            }
        }
        else
        {
            const char *icy = BASS_ChannelGetTags(stream,BASS_TAG_ICY);
            if (!icy) icy = BASS_ChannelGetTags(stream,BASS_TAG_HTTP);

            if (icy)
            {
                for (;*icy;icy+=strlen(icy)+1)
                {
                    if (!strncasecmp(icy,"icy-name:",9))
                        s = Global::cStrToQString(icy+9);

                    if (s.isEmpty())
                        s = playlist->getCurrentTitle();

                    if (!strncasecmp(icy,"icy-url:",8))
                    {
                        QString url = QUrl(Global::cStrToQString(icy+8).trimmed()).toString();

                        if (!url.isEmpty())
                        {
                            if (!url.contains(QRegExp("^http[s]?://", Qt::CaseInsensitive)))
                                url.push_front("http://");

                            s += QString(" (<a href=\"%1\">%2</a>)")
                                       .arg(url, QString(url).remove(QRegExp("^http[s]?://", Qt::CaseInsensitive)));
                        }
                    }

                    if (!strncmp(icy, "icy-br:", 7))
                        bitrate = Global::cStrToQString(icy+7);
                }
            }
        }

        emit updateValues(NameLabel, (s.isEmpty() ? "---" : s));
        return true;
    }

    emit updateValues(BufferProgressBar, static_cast<int>(progress));
    return false;
}

void RadioStream::showTimedout()
{
    QString errDlg = "<div style=\"font-weight:bold;margin-bottom:7px;\">["
                   + QTime::currentTime().toString("HH:mm") + "] Erro ao reproduzir a Web Rádio!</div>"
                   "<div><strong>Código do erro:</strong> 40</div><div style=\"margin-bottom:7px;\">"
                   "<strong>Mensagem:</strong> Connection timedout</div>";

    QString time = "[" + QTime::currentTime().toString("HH:mm") + "] ";

    if (Database::value("Config", "errorNotification").toString() == "dialog")
        emit showErrorDlg(errDlg);
    else if (Database::value("Config", "errorNotification").toString() == "systray")
        emit showError(time + "Erro ao reproduzir a Web Rádio!\nCode: 40, Connection timedout");

    stdCout(time + "Error when playing back the file.\nCode: 40, Connection timedout");
}

void RadioStream::run()
{
    mstop = false;
    reconnect = 0;

    do
    {
        DWORD act = 0;
        int errorCode = 0;
        isQuickLink = false;
        QString currentUrl;

        if (quickLink.isEmpty())
        {
            currentUrl = playlist->getCurrentUrl(selectedUrl);
        }
        else
        {
            isQuickLink = true;
            currentUrl = quickLink;
            quickLink.clear();
        }

        if (currentUrl.isEmpty())
            break;

        mplay = true;
        int timeout = Database::value("Config", "net_timeout", 20000).toInt();
        QElapsedTimer timer;
        timer.start();

        emit updateValues(StatusLabel, "Conectando...");
        emit updateValues(ShowLoaderMovie, true);

        for (int i = 0; i < 2; i++)
        {
            stream = BASS_StreamCreateURL(currentUrl.toLocal8Bit().constData(), 0,
                                        BASS_STREAM_BLOCK|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE,
                                        statusProc, static_cast<void *>(this));

            errorCode = BASS_ErrorGetCode();

            if (i == 0 && errorCode == 41)
                netPlaylist.getUrl(currentUrl, mstop, mplay, timeout, timer);

            if (mstop || errorCode != 41 || currentUrl.isEmpty())
                break;

            if (timer.hasExpired(timeout))
            {
                showTimedout();
                mstop = true;
                break;
            }
        }

        if (mstop)
            break;

        iswma = !!BASS_ChannelGetTags(stream, 8/*BASS_TAG_WMA*/);

        if (iswma)
        {
            mrecord = false;
            emit recordButtonEnabled(false);
        }

        if (errorCode == 0)
        {
            while (mplay && mstop == false)
            {
                int progress;

                if (buffering(progress))
                    break;

                if (timer.hasExpired(timeout) && progress < 6)
                {
                    showTimedout();
                    mstop = true;
                    break;
                }

                msleep(50);
            }

            emit updateValues(ShowLoaderMovie, false);
            if (mstop)
                break;

            bool ok;
            int _bitrate = bitrate.toInt(&ok);
            BASS_CHANNELINFO info;
            BASS_ChannelGetInfo(stream, &info);

            if ((!ok || _bitrate < 8) && !iswma)
                bitrate = QString("%1").arg(BASS_StreamGetFilePosition(stream, BASS_FILEPOS_END) * 8
                                            / BASS_GetConfig(BASS_CONFIG_NET_BUFFER));

            switch (info.ctype)
            {
                case BASS_CTYPE_STREAM_OGG:       fileType = "OGG";  break;
                case BASS_CTYPE_STREAM_MP1:       fileType = "MP1";  break;
                case BASS_CTYPE_STREAM_MP2:       fileType = "MP2";  break;
                case BASS_CTYPE_STREAM_MP3:       fileType = "MP3";  break;
                case BASS_CTYPE_STREAM_AIFF:      fileType = "AIFF"; break;
                case BASS_CTYPE_STREAM_WAV_PCM:   fileType = "WAV";  break;
                case BASS_CTYPE_STREAM_WAV_FLOAT: fileType = "WAV";  break;
                case BASS_CTYPE_STREAM_WAV:       fileType = "WAV";  break;
                case 70144:                       fileType = "OPUS"; break;
                case 68352:                       fileType = "AAC";  break;
                case 66304:                       fileType = "WMA";  break;
            }

            statusList.clear();
            statusList << bitrate + " kbps";
            statusList << fileType;
            statusList << status;

            statusList.removeAll("");
            statusList.removeDuplicates();
            status.clear();
            bitrate.clear();

            statusListCount = 0;
            updateStatus();
            doMeta();

            if (Database::value("Config", "radio_notifiSysTray").toBool())
                emit showNotification((isQuickLink ? "Link Rápido" : playlist->getCurrentTitle()));

            setupDSP_EQ();

            emit startMetaTimer(1000);
            emit startStatusTimer(5000);
            emit newFade();

            while (!fade)
                msleep(10);

            fade->in(stream, getVolume());

            while ((act = BASS_ChannelIsActive(stream)) && mplay && mstop == false)
            {
                emit updateValues(static_cast<QWORD>(BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream,BASS_POS_BYTE)) - recordTime),
                                  BASS_ChannelGetLevel(stream),
                                  (iswma ? BASS_StreamGetFilePosition(stream, 1000/*BASS_FILEPOS_WMA_BUFFER*/) :
                                           BASS_StreamGetFilePosition(stream, BASS_FILEPOS_BUFFER)
                                                *100/BASS_StreamGetFilePosition(stream, BASS_FILEPOS_END)), act);

                msleep(20);
            }

            fade->out(stream);
            fade =  nullptr;
            stream = 0;
            isQuickLink = false;
            emit stopMetaTimer();
            emit stopStatusTimer();
            emit threadFinished(false, !quickLink.isEmpty());
        }

        if (!quickLink.isEmpty())
            continue;

        if (!mstop)
        {
            if (BASS_ErrorGetCode() != 0)
            {
                msleep(1000);
                QString time = "[" + QTime::currentTime().toString("HH:mm") + "] ";

                if (Database::value("Config", "errorNotification").toString() == "dialog")
                    emit showErrorDlg(Global::getErrorHtml(time + "Erro ao reproduzir a Web Rádio!"));
                else if (Database::value("Config", "errorNotification").toString() == "systray")
                    emit showError(Global::getErrorText(time + "Erro ao reproduzir a Web Rádio!"));

                stdCout(Global::getErrorText(time + "Error when playing back the file."));
            }

            if (mprev)
            {
                playlist->prev();
                emit updateValues(RadioName, playlist->getCurrentIndex());
                mprev = false;
                selectedUrl = 0;
            }
            else if (newRadio)
            {
                newRadio = false;
            }
            else if (mnext || Database::value("Config", "radioMode").toInt() == 2)
            {
                playlist->next();
                emit updateValues(RadioName, playlist->getCurrentIndex());
                mnext = false;
                selectedUrl = 0;
            }
            else if (Database::value("Config", "radioMode").toInt() == 0 || (++reconnect > 2))
            {
                mstop = true;
                break;
            }
        }
    } while (!mstop);

    isQuickLink = false;
    iswma = false;
    mrecord = false;
    mstop = false;
    reconnect = 0;
    emit threadFinished(true, false);
}

//================================================================================================================
// class RadioPlaylist
//================================================================================================================

RadioPlaylist::RadioPlaylist(QWidget *parent, const int &_playlistMode) : QTreeView(parent), playlistMode(_playlistMode)
{
    this->parent = parent;
    filteredText = false;
    currentIndex[0] = Database::value("RadioMode", "indexAll").toInt();
    currentIndex[1] = Database::value("RadioMode", "indexCustom").toInt();
    currentIndex[2] = Database::value("RadioMode", "indexFavorites").toInt();
    currentIndex[3] = 0;

    model = new QStandardItemModel(0, 9);
    model->setHeaderData(0, Qt::Horizontal, "Nome");
    model->setHeaderData(1, Qt::Horizontal, "Gênero");
    model->setHeaderData(2, Qt::Horizontal, "País");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setRootIsDecorated(false);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setFixedHeight(165);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setModel(proxyModel);

    setColumnWidth(0,210);
    setColumnWidth(1,120);
    setColumnWidth(2,110);

    setColumnHidden(3,true);
    setColumnHidden(4,true);
    setColumnHidden(5,true);
    setColumnHidden(6,true);
    setColumnHidden(7,true);
    setColumnHidden(8,true);

    connect(this, SIGNAL(selectRowSignal(int)), this, SLOT(selectRow(int)));
    connect(this, &QTreeView::doubleClicked, [=]() {
        getCurrentIndex() = selectionModel()->selectedRows()[0].row();
        emit playNewRadio(getCurrentIndex());
    });
}

RadioPlaylist::~RadioPlaylist()
{
    Database::setValue("RadioMode", "indexAll", currentIndex[0]);
    Database::setValue("RadioMode", "indexCustom", currentIndex[1]);
    Database::setValue("RadioMode", "indexFavorites", currentIndex[2]);

    delete model;
    delete proxyModel;
}

void RadioPlaylist::play()
{
    if (getCurrentIndex() >= proxyModel->rowCount())
        getCurrentIndex() = 0;

    emit selectRowSignal(getCurrentIndex());
}

void RadioPlaylist::prev()
{
    getCurrentIndex()--;

    if (getCurrentIndex() < 0 || getCurrentIndex() >= proxyModel->rowCount())
        getCurrentIndex() = proxyModel->rowCount() - 1;

    emit selectRowSignal(getCurrentIndex());
}

void RadioPlaylist::next()
{
    getCurrentIndex()++;

    if (getCurrentIndex() >= proxyModel->rowCount())
        getCurrentIndex() = 0;

    emit selectRowSignal(getCurrentIndex());
}

void RadioPlaylist::addRow(const QStringList &arg)
{
    QList<QStandardItem *> item;
    item << new QStandardItem(arg[0]);
    item << new QStandardItem(arg[1]);
    item << new QStandardItem(arg[2]);
    item << new QStandardItem(arg[3]);
    item << new QStandardItem(arg[4]);
    item << new QStandardItem(arg[5]);
    item << new QStandardItem(arg[6]);
    item << new QStandardItem(arg[7]);
    item << new QStandardItem(arg[8]);
    model->appendRow(item);
}

void RadioPlaylist::clear()
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());
}

int RadioPlaylist::length()
{
    return proxyModel->rowCount();
}

bool RadioPlaylist::isEmpty()
{
    return !model->hasChildren();
}

void RadioPlaylist::setCurrentIndex(const int &arg)
{
    getCurrentIndex() = arg;
}

int &RadioPlaylist::getCurrentIndex()
{
    return currentIndex[filteredText ? 3 : playlistMode];
}

void RadioPlaylist::removeRow(const int &row)
{
    if (proxyModel->hasChildren() && row < proxyModel->rowCount())
        proxyModel->removeRow(row);
}

QStringList RadioPlaylist::getRow(const int &row)
{
    QStringList list;

    for (int i = 0; i < 9; i++)
        list << proxyModel->index(row,i).data().toString();

    return list;
}

QString RadioPlaylist::getRadioName(const int &arg)
{
    return proxyModel->index(arg,0).data().toString().replace("<", "&lt;");
}

QString RadioPlaylist::getRadioUrl(const int &arg)
{
    return proxyModel->index(arg,3).data().toString();
}

QStringList RadioPlaylist::getAllUrls(const int &idx)
{
    QStringList url;

    for (int i = 3; i < 8; i++)
        url << proxyModel->index(idx, i).data().toString();

    url.removeAll("");
    return url;
}

QString RadioPlaylist::getCurrentTitle()
{
    return proxyModel->index(getCurrentIndex(), 0).data().toString();
}

QString RadioPlaylist::getCurrentUrl(const int &url)
{
    return proxyModel->index(getCurrentIndex(), url+3).data().toString();
}

//================================================================================================================
// public slots
//================================================================================================================

void RadioPlaylist::selectRow(int arg)
{
    selectionModel()->setCurrentIndex(proxyModel->index(arg,0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void RadioPlaylist::textFilterChanged(QString arg)
{
    if (arg.isEmpty())
    {
        filteredText = false;
    }
    else
    {
        filteredText = true;
        getCurrentIndex() = 0;
    }

    proxyModel->setFilterRegExp(QRegExp(arg, Qt::CaseInsensitive, QRegExp::FixedString));
    emit selectRowSignal(getCurrentIndex());
    emit updateValues(RadioStream::RadioName, getCurrentIndex());
}

//================================================================================================================
// private
//================================================================================================================

void RadioPlaylist::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return)
    {
        getCurrentIndex() = selectionModel()->selectedRows()[0].row();
        emit playNewRadio(getCurrentIndex());
        event->accept();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}



