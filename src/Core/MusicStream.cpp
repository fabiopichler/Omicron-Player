/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MusicStream.h"
#include "iostream"

int MusicStream::currentDrive = -1;
QStringList MusicStream::musicPlaylist;

MusicStream::MusicStream(QWidget *parent) : Stream()
{
    this->parent = parent;
    mprev = false;
    isMusic = false;
    isCDMode = false;
    newSound = false;
    repeat = Database::value("MusicMode", "repeat", 0).toInt();
    random = Database::value("MusicMode", "random", false).toBool();
    playlistMode = -2;
    tagListCount = 0;
    fade = nullptr;

    playlist = new MusicPlaylist(parent, playlistMode, repeat, random, mstop);
    timerTag = new QTimer(this);
    soundPositionTimer = new QTimer(this);

#ifndef Q_OS_ANDROID
    cdTimer = new QTimer(this);
#endif

    createEvents();

    if (currentDrive > -1)
        setupCDMode(true, currentDrive);

    tagList << AppName << QString("Versão " + CurrentVersion) << "Desenvolvido por Fábio Pichler";
    emit startTagTimer(5000);
}

MusicStream::~MusicStream()
{
    if (isCDMode)
    {
        int curdrive = currentDrive;
        setupCDMode(false);
        currentDrive = curdrive;
    }
    else
    {
        if (playlistMode == 1)
        {
            musicPlaylist.clear();
            for (int i  = 0; i < playlist->length(); i++)
                musicPlaylist << playlist->getRow(i);
        }
        setupCDMode(false);
    }

    if (Database::value("MusicConfig", "continuePlaying").toBool() && isRunning())
        Database::setValue("MusicMode", "soundPosition", static_cast<int>(getPosition()));

    stop();

    delete timerTag;
    delete soundPositionTimer;

#ifndef Q_OS_ANDROID
    delete cdTimer;
#endif
}

void MusicStream::setupCDMode(const bool &active, const int &drive)
{
#ifndef Q_OS_ANDROID
    if (active && !isCDMode)
    {
        if (currentDrive == -1)
            playlist->setCurrentIndex(0);

        isCDMode = true;
        currentDrive = drive;
        BASS_CD_SetInterface(BASS_CD_IF_AUTO);
        emit startCDTimer(100);
        updateTrackList();
    }
    else if (!active && isCDMode)
    {
        emit stopCDTimer();
        stop();
        BASS_StreamFree(stream);
        BASS_CD_Release(currentDrive);
        stream = 0;
        currentDrive = -1;
        isCDMode = false;
        playlist->clear();
        playlist->setCurrentIndex(0);
    }
#endif
}

void MusicStream::loadPlaylist(const int &mode, const bool &disableCdMode, const bool &load,
                               const bool &_stop, QString listName)
{
    static bool _isCDMode = false,
                initialized = true;
    static int _currentDrive = -1;

    if (disableCdMode)
        _isCDMode = false;

    if (playlistMode == 1 && mode != 1)
    {
        if (isCDMode)
        {
            _currentDrive = currentDrive;
            setupCDMode(false);
            _isCDMode = true;
        }
        else
        {
            for (int i  = 0; i < playlist->length(); i++)
                musicPlaylist << playlist->getRow(i);
        }
    }

    playlistMode = mode;
    playlist->clear();

    if (_stop && !Database::value("MusicConfig", "continuePlayingTab").toBool())
        stop();

    if (!load)
        return;

    if (mode == 0)
    {
        if (listName.isEmpty())
        {
            listName = Database::value("MusicMode", "playlist").toString();

            if (!listName.isEmpty() && !QFile::exists(listName))
                Database::clear("MusicMode", "playlist");
        }

        if (listName.isEmpty())
            listName = Global::getConfigPath("Playlist."+PlaylistExt);

        QFile file(listName);
        if (!file.exists())
            return;

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");

            while (!stream.atEnd())
                playlist->addRow(stream.readLine());

            stream.flush();
            file.close();
        }
        else
        {
            QMessageBox::warning(nullptr, "Erro!", "Erro ao abrir o Playlist!");
        }
    }
    else if (mode == 1)
    {
        if (_isCDMode)
        {
            _isCDMode = false;
            setupCDMode(true, _currentDrive);
            return;
        }

        QStringList arguments;

        if (initialized && musicPlaylist.isEmpty())
        {
            arguments = QApplication::arguments();
            arguments.removeAt(0);
        }
        else
        {
            arguments = musicPlaylist;
            musicPlaylist.clear();
        }

        initialized = false;

        if (!arguments.isEmpty())
        {
            for (int i = 0; i < arguments.length(); i++)
            {
                QString args = arguments.at(i);

                if (args.contains(QRegExp("."+PlaylistExt+"$", Qt::CaseInsensitive)))
                {
                    playlist->clear();
                    emit playNewMusic(QStringList(args));
                    return;
                }

                if (!args.contains(QRegExp("^-|."+PlaylistExt+"$", Qt::CaseInsensitive)))
                    playlist->addRow(args);
            }
            if (Database::value("MusicMode", "soundPosition").toInt() > 0)
                Database::setValue("MusicMode", "soundPosition", 0);
        }
    }
    else
    {
        QStringList list = Database::getMusicFavorites();

        for (int i = 0; i < list.length(); i++)
            playlist->addRow(list[i]);
    }
}

//================================================================================================================
// public slots
//================================================================================================================

void MusicStream::play()
{
    if (playlist->isEmpty())
    {
        emit initPlaylist(true);
    }
    else
    {
        if (mpause)
        {
            mpause = false;
            BASS_ChannelPlay(stream, FALSE);
            emit playButtonEnabled(false);
            emit pauseButtonEnabled(true);
        }

        if (!isRunning())
        {
            playlist->play();
            start();
        }
    }
}

void MusicStream::pause()
{
    mpause = true;
    BASS_ChannelPause(stream);
    emit playButtonEnabled(true);
    emit pauseButtonEnabled(false);
}

void MusicStream::stop()
{
    if (isRunning())
    {
        mstop = true;
        wait();
    }
}

void MusicStream::prev()
{
    if (isRunning())
    {
        mprev = true;
        mplay = false;
    }
    else if (!playlist->isEmpty())
    {
        playlist->prev();
    }
}

void MusicStream::next()
{
    if (isRunning())
        mplay = false;
    else if (!playlist->isEmpty())
        playlist->next();
}

short MusicStream::changeRepeat()
{
    if (repeat == 0)
    {
        repeat = 1;
        Database::setValue("MusicMode", "repeat", 1);
    }
    else if (repeat == 1)
    {
        repeat = 2;
        Database::setValue("MusicMode", "repeat", 2);
    }
    else if (repeat == 2)
    {
        repeat = 0;
        Database::setValue("MusicMode", "repeat", 0);
    }
    return repeat;
}

bool MusicStream::changeRandom()
{
    if (random)
    {
        random = false;
        Database::setValue("MusicMode", "random", false);
    }
    else
    {
        random = true;
        Database::setValue("MusicMode", "random", true);
    }
    return random;
}

void MusicStream::playNewMusic(QVariant row)
{
    emit updateValue(CurrentSound, row);

    if (isRunning())
    {
        newSound = true;
        mplay = false;
    }
    else
    {
        play();
    }
}

//================================================================================================================
// private slots
//================================================================================================================

void MusicStream::updateTag()
{
    int len = tagList.length();

    if (len > 0)
    {
        if (tagListCount >= len)
            tagListCount = 0;

        emit updateValue(CurrentTag, tagList[tagListCount]);
        tagListCount++;
    }
}

void MusicStream::updateSoundPosition()
{
    if (Database::value("MusicConfig", "continuePlaying").toBool() && isRunning())
        Database::setValue("MusicMode", "soundPosition", static_cast<int>(getPosition()));
}

void MusicStream::updateCDMode()
{
#ifndef Q_OS_ANDROID
    if (playlist->isEmpty())
    {
        if (BASS_CD_IsReady(currentDrive))
        {
            updateTrackList();
            emit updatePlaylistStyle();
        }
    }
    else if (!BASS_CD_IsReady(currentDrive))
    {
        stop();
        BASS_StreamFree(stream);
        stream = 0;
        playlist->clear();
        if (timerTag->isActive())
            emit stopTagTimer();
        emit updatePlaylistStyle();
        emit updateValue(PlaylistLength);
        emit updateValue(CurrentTag, "Sem CD de Áudio");
    }

    if (BASS_ChannelIsActive(stream) == 0)
        BASS_CD_Release(currentDrive);

    if (BASS_CD_DoorIsOpen(currentDrive))
    {
        if (timerTag->isActive())
            emit stopTagTimer();
        emit updateValue(CurrentTag, "Sem CD de Áudio");
    }
#endif
}

void MusicStream::createFade()
{
    fade = new Fade("MusicConfig");
}

//================================================================================================================
// private
//================================================================================================================

void MusicStream::createEvents()
{
    connect(timerTag, SIGNAL(timeout()), this, SLOT(updateTag()));
    connect(this, SIGNAL(startTagTimer(int)), timerTag, SLOT(start(int)));
    connect(this, SIGNAL(stopTagTimer()), timerTag, SLOT(stop()));

    connect(soundPositionTimer, SIGNAL(timeout()), this, SLOT(updateSoundPosition()));
    connect(this, SIGNAL(startSoundPositionTimer(int)), soundPositionTimer, SLOT(start(int)));
    connect(this, SIGNAL(stopSoundPositionTimer()), soundPositionTimer, SLOT(stop()));

#ifndef Q_OS_ANDROID
    connect(cdTimer, SIGNAL(timeout()), this, SLOT(updateCDMode()));
    connect(this, SIGNAL(startCDTimer(int)), cdTimer, SLOT(start(int)));
    connect(this, SIGNAL(stopCDTimer()), cdTimer, SLOT(stop()));
#endif

    connect(playlist, SIGNAL(playNewMusic(QVariant)), this, SLOT(playNewMusic(QVariant)));
    connect(this, SIGNAL(newFade()), this, SLOT(createFade()), Qt::QueuedConnection);
}

void MusicStream::updateTrackList()
{
#ifndef Q_OS_ANDROID
    int tc = BASS_CD_GetTracks(currentDrive);
    playlist->clear();

    if (timerTag->isActive())
        emit stopTagTimer();

    if (tc == -1)
    {
        emit updateValue(CurrentTag, "Sem CD de Áudio");
        return;
    }

    for (int i = 0; i < tc; i++)
    {
        char text[100];
        int len = BASS_CD_GetTrackLength(currentDrive, i);
        sprintf(text, "Track %02d", i+1);

        if (len == -1)
        {
            strcat(text, " (data)");
        }
        else
        {
            len/=176400;
            sprintf(text+strlen(text), " (%d:%02d)", len/60, len%60);
        }

        playlist->addRow(text);
    }

    emit updateValue(PlaylistLength);
    emit updateValue(CurrentTag, "Modo CD de Áudio");
#endif
}

void MusicStream::run()
{
    DWORD act;
    mpause = false;
    mstop = false;

    do
    {
        emit updateValue(FileTypeLabel, "Carregando...");

        QString currentFile = playlist->getCurrentFile();
        if (currentFile.isEmpty())
            break;

        mplay = false;
        isMusic = false;

        if (isCDMode)
        {
#ifndef Q_OS_ANDROID
            if ((stream = BASS_CD_StreamCreate(currentDrive, playlist->getCurrentIndex(), BASS_STREAM_AUTOFREE)))
                mplay = true;
#endif
        }
        else if ((stream = BASS_StreamCreateFile(0, currentFile.toLocal8Bit().constData(), 0, 0, BASS_STREAM_AUTOFREE)))
        {
            mplay = true;
        }
        else if ((stream = BASS_MusicLoad(FALSE, currentFile.toLocal8Bit().constData(), 0, 0, BASS_STREAM_AUTOFREE, 0)))
        {
            isMusic = true;
            mplay = true;
        }

        if (mplay)
        {
            double timeLength;
            QString fileType;
            BASS_CHANNELINFO info;

            fade = nullptr;
            tagListCount = 0;

            if (isMusic)
                timeLength = BASS_ChannelGetLength(stream, BASS_POS_MUSIC_ORDER) - 1;
            else
                timeLength = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetLength(stream, BASS_POS_BYTE));

            BASS_ChannelGetInfo(stream, &info);
            fileType = getFileType(info.ctype);

            if (!isMusic)
                fileType += QString(fileType.isEmpty() ? "%1 kbps" : " | %1 kbps").arg(static_cast<int>(
                                                      (BASS_StreamGetFilePosition(stream, BASS_FILEPOS_END)
                                                       /(125*timeLength)+0.5)));

            fileType += QString(" | ") + (info.chans == 1 ? "Mono"
                                                          : (info.chans == 2 ? "Stereo"
                                                                             : (info.chans > 2 ? "Surround"
                                                                                               : "")));

            tagList.clear();

            if (isCDMode)
            {
                tagList << "Reproduzindo um CD de Áudio";
            }
            else
            {
                tagList << Global::cStrToQString(TAGS_Read(stream, "%IFV2(%TITL,%ICAP(%TITL),no title)"));
                tagList << Global::cStrToQString(TAGS_Read(stream, "%IFV2(%ARTI,%ICAP(%ARTI),no artist)"));
                tagList << Global::cStrToQString(TAGS_Read(stream, "%IFV1(%ALBM,%IUPC(%ALBM))"));
                tagList << Global::cStrToQString(TAGS_Read(stream, "%GNRE"));
                tagList << Global::cStrToQString(TAGS_Read(stream, "%YEAR"));
                tagList << Global::cStrToQString(TAGS_Read(stream, "%CMNT")).replace(QRegExp("\n|\r\n"), " ").trimmed();
            }

            tagList.removeAll("");
            tagList.removeDuplicates();

            if (tagList.isEmpty())
                tagList << AppName;

            updateTag();
            setupDSP_EQ();

            if (timerTag->isActive())
                emit stopTagTimer();

            emit startTagTimer(5000);
            emit setTotals(static_cast<QWORD>(timeLength));
            emit updateValue(FileTypeLabel, fileType);
            emit newFade();

            while (!fade)
                msleep(1);

            int fadeOut = Database::value("MusicConfig", "fadeOut", 0).toInt();

            if (Database::value("MusicConfig", "continuePlaying").toBool()
                              && Database::value("MusicMode", "soundPosition").toInt() > 0)
            {
                setPosition(Database::value("MusicMode", "soundPosition").toInt());
                Database::setValue("MusicMode", "soundPosition", 0);
            }

            if (mpause)
            {
                emit updateInfo(0, 0);
                while (mpause && mplay && mstop == false)
                    msleep(50);
            }
            else
            {
                emit playButtonEnabled(false);
                emit pauseButtonEnabled(true);
                emit stopButtonEnabled(true);

                fade->in(stream, getVolume());
            }

            if (mplay && !mstop && Database::value("MusicConfig", "notifiSysTray").toBool())
                emit showNotification((isCDMode ? "CD: " : "") + playlist->getCurrentTitle());

            emit startSoundPositionTimer(3000);

            while ((act = BASS_ChannelIsActive(stream)) && mplay && mstop == false)
            {
                QWORD position = 0;

                if (isMusic)
                {
                    QWORD pos = BASS_ChannelGetPosition(stream, BASS_POS_MUSIC_ORDER);
                    if (pos != (QWORD)-1)
                        position = pos;
                }
                else
                {
                    double pos = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
                    position = static_cast<QWORD>(pos);

                    if (static_cast<int>(timeLength - pos) < fadeOut)
                        mplay = false;
                }

                emit updateInfo(position, (act == 1 ? BASS_ChannelGetLevel(stream) : 0));
                msleep(20);
            }

            emit stopSoundPositionTimer();
            fade->out(stream);
            fade = nullptr;
            stream = 0;
            emit stopTagTimer();
        }
        else if (!mstop)
        {
            QString time = "[" + QTime::currentTime().toString("HH:mm") + "] ";

            emit playButtonEnabled(false);
            emit pauseButtonEnabled(true);
            emit stopButtonEnabled(true);
            emit updateValue(CurrentTag, "Erro na reprodução!");
            emit updateValue(FileTypeLabel, "Erro!");

            if (Database::value("Config", "errorNotification").toString() == "dialog")
                emit showErrorDlg(Global::getErrorHtml(time + "Erro na reprodução!"));
            else if (Database::value("Config", "errorNotification").toString() == "systray")
                emit showError(Global::getErrorText(time + "Erro na reprodução!"));

            stdCout(Global::getErrorText(time + "Playback error."));
            msleep(1000);

            if (playlist->length() < 2)
                break;

            switch (BASS_ErrorGetCode()) {
              case 8:
              case 12:
                mstop = true;
                break;
            }
        }

        if (!mstop)
        {
            if (mprev)
            {
                playlist->prev();
                mprev = false;
            }
            else if (newSound)
            {
                newSound = false;
            }
            else
            {
                playlist->next();
            }
        }
    } while (!mstop);

    mpause = false;
    mstop = false;
    emit threadFinished();
    emit updateValue(FileTypeLabel, "---");
}

//================================================================================================================
// class MusicPlaylist
//================================================================================================================

MusicPlaylist::MusicPlaylist(QWidget *parent, const int &_playlistMode, const int &repeat, const bool &random, bool &stop)
                           : QTreeView(parent), playlistMode(_playlistMode), mrepeat(repeat), mrandom(random), mstop(stop)
{
    this->parent = parent;
    filteredText = false;
    currentIndex[0] = Database::value("MusicMode", "indexPlaylist").toInt();
    currentIndex[1] = Database::value("MusicMode", "indexMusics").toInt();
    currentIndex[2] = Database::value("MusicMode", "indexFavorites").toInt();
    currentIndex[3] = 0;

    model = new QStandardItemModel(0, 2);
    model->setHeaderData(0, Qt::Horizontal, "Músicas");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setFixedHeight(165);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setModel(proxyModel);
    setColumnHidden(1,true);

    connect(this, SIGNAL(selectRowSignal(int)), this, SLOT(selectRow(int)));
    connect(this, &QTreeView::doubleClicked, [=]() {
        getCurrentIndex() = selectionModel()->selectedRows()[0].row();
        emit playNewMusic(getCurrentIndex());
    });
}

MusicPlaylist::~MusicPlaylist()
{
    Database::setValue("MusicMode", "indexPlaylist", currentIndex[0]);
    Database::setValue("MusicMode", "indexMusics", currentIndex[1]);
    Database::setValue("MusicMode", "indexFavorites", currentIndex[2]);
    delete model;
    delete proxyModel;
}

void MusicPlaylist::play()
{
    if (getCurrentIndex() >= proxyModel->rowCount())
        getCurrentIndex() = 0;

    emit selectRowSignal(getCurrentIndex());
}

void MusicPlaylist::prev()
{
    if (mrepeat != 1 && random())
    {
        getCurrentIndex()--;

        if (getCurrentIndex() < 0 || getCurrentIndex() >= proxyModel->rowCount())
            getCurrentIndex() = proxyModel->rowCount() - 1;
    }

    emit selectRowSignal(getCurrentIndex());
}

void MusicPlaylist::next()
{
    if (mrepeat != 1 && random())
    {
        getCurrentIndex()++;

        if (getCurrentIndex() >= proxyModel->rowCount())
        {
            getCurrentIndex() = 0;

            if (mrepeat == 0)
                mstop = true;
        }
    }

    emit selectRowSignal(getCurrentIndex());
}

void MusicPlaylist::addRow(QString arg)
{
    QList<QStandardItem *> item;

    item << new QStandardItem(QFileInfo(arg).completeBaseName().replace(QRegExp("_+| +"), " ")
                              .replace(QRegExp(" +"), " ").trimmed());
    item << new QStandardItem(arg);

    model->appendRow(item);
}

void MusicPlaylist::clear()
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());
}

void MusicPlaylist::removeRow(const int &row)
{
    if (proxyModel->hasChildren() && row < proxyModel->rowCount())
        proxyModel->removeRow(row);
}

int MusicPlaylist::length()
{
    return proxyModel->rowCount();
}

bool MusicPlaylist::isEmpty()
{
    return !model->hasChildren();
}

void MusicPlaylist::setCurrentIndex(int idx)
{
    getCurrentIndex() = idx;
}

int &MusicPlaylist::getCurrentIndex()
{
    return currentIndex[filteredText ? 3 : playlistMode];
}

QString MusicPlaylist::getCurrentTitle()
{
    return proxyModel->index(getCurrentIndex(), 0).data().toString();
}

QString MusicPlaylist::getCurrentFile()
{
    if (mstop)
    {
        mstop = false;
        return QString();
    }

    return proxyModel->index(getCurrentIndex(),1).data().toString();
}

QString MusicPlaylist::getRow(const int &arg)
{
    if (proxyModel->hasChildren())
        return proxyModel->data(proxyModel->index(arg, 1)).toString();

    return QString();
}

//================================================================================================================
// public slots
//================================================================================================================

void MusicPlaylist::selectRow(int arg)
{
    emit updateValue(MusicStream::CurrentSound, getCurrentIndex());
    selectionModel()->setCurrentIndex(proxyModel->index(arg,0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void MusicPlaylist::textFilterChanged(QString arg)
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
    emit updateValue(MusicStream::CurrentSound, (length() == 0 ? -1 : getCurrentIndex()));
    emit updateValue(MusicStream::PlaylistLength, 0);
}

//================================================================================================================
// private
//================================================================================================================

bool MusicPlaylist::random()
{
    if (mrandom)
    {
        int playlistSize = proxyModel->rowCount();
        if (playlistSize > 2)
        {
            int currentIndexRand, _srand = 1;
            while (_srand < 11)
            {
                qsrand(QTime::currentTime().msec() + _srand);
                currentIndexRand = qrand() % playlistSize;

                if (getCurrentIndex() != currentIndexRand)
                    break;

                _srand++;
            }
            getCurrentIndex() = currentIndexRand;
            return false;
        }
    }

    return true;
}

void MusicPlaylist::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return)
    {
        getCurrentIndex() = selectionModel()->selectedRows()[0].row();
        emit playNewMusic(getCurrentIndex());
        event->accept();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

