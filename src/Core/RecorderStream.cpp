/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "RecorderStream.h"
#include "Database.h"
#include <QKeyEvent>
#include <iostream>

RecorderStream::RecorderStream(QWidget *parent)
{
    rchan = 0;
    stream = 0;

    recordList = new RecordList(parent);
    updateTimer = new QTimer(this);
    checkDirTimer = new QTimer(this);

    EncoderList::current = Database::value("RecorderMode", "encoder").toInt();

    encoderList[0].name = "AAC";
    encoderList[0].index = Database::value("RecorderMode", "aac").toInt();
    encoderList[0].bitrate = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256};

    encoderList[1].name = "OGG";
    encoderList[1].index = Database::value("RecorderMode", "ogg").toInt();
    encoderList[1].bitrate = {        48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

    encoderList[2].name = "MP3";
    encoderList[2].index = Database::value("RecorderMode", "mp3").toInt();
    encoderList[2].bitrate = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

    createEvents();
    initDevice();
    loadList();

    checkDirTimer->start(2000);
}

RecorderStream::~RecorderStream()
{
    stop();
    checkDirTimer->stop();

    delete updateTimer;
    delete checkDirTimer;

    BASS_RecordFree();
}

void RecorderStream::initDevice(int device)
{
    if (device == -2)
        device =  Database::value("RecorderMode", "device").toInt();

    BASS_RecordFree();

    if (!BASS_RecordInit(device))
    {
        QMessageBox::warning(nullptr,"Aviso",Global::getErrorHtml("Não foi possível iniciar o dispositivo selecionado!<br>"
                             "Verifique se está funcionando corretamente e tente novamente."));
    }
}

QStringList RecorderStream::getDeviceInfo()
{
    QStringList list;
    BASS_DEVICEINFO deviceInfo;

    list << "Dispositivo Padrão";

    for (int i = 0; BASS_RecordGetDeviceInfo(i, &deviceInfo); i++)
    {
        if (deviceInfo.flags & BASS_DEVICE_ENABLED)
            list << deviceInfo.name;
    }

    return list;
}

//================================================================================================================
// public slots
//================================================================================================================

void RecorderStream::record()
{
    stop();

    QString cmd, encPath = "\"" + qApp->applicationDirPath() + "/plugins/encoders/";
    int bitrate = encoderList[encoderList[0].current].bitrate[encoderList[encoderList[0].current].index];
    QString recordFileName = recordPath+"/Record "+QDateTime::currentDateTime().toString("yyyy-MM-dd HH.mm.ss");

    switch (EncoderList::current)
    {
    case 0:
        {
            QString profile;

            if (bitrate < 128)
                profile = "29";
            else
                profile = "5";

            cmd = QString(encPath+"fdkaac\" -p "+profile+" -f 2 -b %1 -I -S -R - -o \""+recordFileName+".aac\"").arg(bitrate);
        }
        break;

    case 1:
        cmd = QString(encPath+"oggenc\" -b %1 - -o \""+recordFileName+".ogg\"").arg(bitrate);
        break;

    case 2:
        cmd = QString(encPath+"lame\" -s 44100 -b %1 - \""+recordFileName+".mp3\"").arg(bitrate);
        break;
    }

    if (!(rchan = BASS_RecordStart(44100, 2, BASS_RECORD_PAUSE, &recordingCallback, 0)))
    {
        error("Não foi possível iniciar a gravação");
        return;
    }

    if (!(hencode = BASS_Encode_Start(rchan, cmd.toLocal8Bit().constData(), BASS_ENCODE_AUTOFREE|BASS_ENCODE_FP_16BIT, nullptr, 0)))
    {
        error("Não foi possível iniciar a gravação");
        BASS_ChannelStop(rchan);
        rchan = 0;
        return;
    }

    BASS_ChannelSetSync(rchan, BASS_SYNC_END, 0, &endSync, this);
    BASS_ChannelPlay(rchan, FALSE);

    emit startUpdadeTimer(20);
    emit updateValue(Recording, true);
    emit updateValue(Status, "Gravando");
    emit startedStream();
}

void RecorderStream::play()
{
    if (recordList->isEmpty())
        return;

    stop();

    if (!(stream = BASS_StreamCreateFile(FALSE, recordList->getCurrentFile().toLocal8Bit().constData(), 0, 0, 0)))
    {
        error("Erro ao reproduzir a gravação");
        stream = 0;
        return;
    }

    emit updateValue(TimeSlider, BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetLength(stream, BASS_POS_BYTE)));

    setupDSP_EQ();
    BASS_ChannelSetSync(stream, BASS_SYNC_END, 0, &endSync, this);
    BASS_ChannelPlay(stream, FALSE);

    emit startUpdadeTimer(20);
    emit updateValue(Status, "Reproduzindo");
    emit startedStream();
}

void RecorderStream::stop()
{
    emit stopUpdateTimer();
    emit updateValue(Recording, false);
    emit endOfStream();

    BASS_ChannelStop(rchan);
    BASS_Encode_Stop(hencode);
    BASS_StreamFree(rchan);
    BASS_StreamFree(stream);

    if (rchan != 0)
        loadList();

    rchan = 0;
    stream = 0;
}

void RecorderStream::loadList()
{
    recordList->clear();
    recordPath = Database::value("RadioConfig", "recordPath").toString() + "/Recorder";

    if (!QDir().exists(recordPath))
        QDir().mkpath(recordPath);

    QDirIterator dirIt(recordPath, QDirIterator::Subdirectories);
    QRegExp filter(QString(FileNameFilter).replace(" ", "|").remove("*."), Qt::CaseInsensitive);

    while (dirIt.hasNext())
    {
        QFileInfo fileInfo(dirIt.next());

        if (fileInfo.isFile() && fileInfo.suffix().contains(filter))
            recordList->addRow(fileInfo.absoluteFilePath());
    }

    recordList->selectRow(0);
    pathLastModified = QFileInfo(recordPath).lastModified();
}

void RecorderStream::deleteFile()
{
    if (recordList->isEmpty())
        return;

    if (QMessageBox::question(nullptr, "Apagar gravação?",
                              "Apagar \"" + recordList->getCurrentTitle() + "\"?",
                              QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        if (QFile(recordList->getCurrentFile()).remove())
            loadList();
        else
            QMessageBox::warning(nullptr, "Erro ao remover", "Não foi possível remover a gravação!");
    }
}

//================================================================================================================
// private
//================================================================================================================

void RecorderStream::createEvents()
{
    connect(this, SIGNAL(startUpdadeTimer(int)), updateTimer, SLOT(start(int)));
    connect(this, SIGNAL(stopUpdateTimer()), updateTimer, SLOT(stop()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(checkDirTimer, SIGNAL(timeout()), this, SLOT(checkDir()));

    connect(recordList, SIGNAL(play()), this, SLOT(play()));
}

void RecorderStream::error(const char *message)
{
    int errorCode = BASS_ErrorGetCode();

    QString time = "[" + QTime::currentTime().toString("HH:mm") + "] ";

    if (Database::value("Config", "errorNotification").toString() == "dialog")
        emit showErrorDlg(Global::getErrorHtml(time + message, "", errorCode));

    else if (Database::value("Config", "errorNotification").toString() == "systray")
        emit showError(Global::getErrorText(time + message, "", errorCode));

    stdCout(Global::getErrorText(time + "Error when playing back the file.", "", errorCode));
}

BOOL CALLBACK RecorderStream::recordingCallback(HRECORD handle, const void *, DWORD, void *)
{
    return BASS_Encode_IsActive(handle);
}

void CALLBACK RecorderStream::endSync(HSYNC, DWORD, DWORD, void *user)
{
    static_cast<RecorderStream *>(user)->stop();
}

//================================================================================================================
// private slots
//================================================================================================================

void RecorderStream::update()
{
    DWORD s;

    if (stream == 0)
        s = rchan;
    else
        s = stream;

    emit updateInfo(static_cast<QWORD>(BASS_ChannelBytes2Seconds(s, BASS_ChannelGetPosition(s, BASS_POS_BYTE))),
                      BASS_ChannelGetLevel(s));
}

void RecorderStream::checkDir()
{
    if (!BASS_ChannelIsActive(stream) && !BASS_ChannelIsActive(rchan)
                          && QFileInfo(recordPath).lastModified() != pathLastModified)
        loadList();
}

//================================================================================================================
// class RecordList
//================================================================================================================

RecordList::RecordList(QWidget *parent) : QTreeView(parent)
{
    this->parent = parent;

    model = new QStandardItemModel(0, 4);
    model->setHeaderData(0, Qt::Horizontal, "Gravações");
    model->setHeaderData(1, Qt::Horizontal, "Tipo");
    model->setHeaderData(2, Qt::Horizontal, "Duração");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setFixedHeight(180);
    setSortingEnabled(true);
    sortByColumn(0, Qt::DescendingOrder);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setModel(proxyModel);

    setColumnWidth(0, 280);
    setColumnWidth(1, 70);
    setColumnHidden(3,true);

    connect(this, &RecordList::doubleClicked, this, &RecordList::play);
}

RecordList::~RecordList()
{
    delete model;
    delete proxyModel;
}

void RecordList::addRow(QString arg)
{
    QList<QStandardItem *> item;
    HSTREAM file;
    BASS_CHANNELINFO info;

    if (!(file = BASS_StreamCreateFile(FALSE, arg.toLocal8Bit().constData(), 0, 0, 0)))
        return;

    DWORD time = static_cast<DWORD>(BASS_ChannelBytes2Seconds(file, BASS_ChannelGetLength(file, BASS_POS_BYTE)));

    BASS_ChannelGetInfo(file, &info);
    BASS_StreamFree(file);

    item << new QStandardItem(QFileInfo(arg).completeBaseName().replace("-", "/")
                                                               .replace(".", ":")
                                                               .replace(" ", " - ")
                                                               .trimmed());
    item << new QStandardItem(Stream::getFileType(info.ctype));

    item << new QStandardItem(QString().sprintf("%02i:%02i:%02i",
                                                static_cast<unsigned int>(time/3600),
                                                static_cast<unsigned int>((time/60)%60),
                                                static_cast<unsigned int>(time%60)));

    item << new QStandardItem(arg);

    model->appendRow(item);
}

void RecordList::clear()
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());
}

void RecordList::removeRow(const int &row)
{
    if (proxyModel->hasChildren() && row < proxyModel->rowCount())
        proxyModel->removeRow(row);
}

int RecordList::length()
{
    return proxyModel->rowCount();
}

bool RecordList::isEmpty()
{
    return !model->hasChildren();
}

int RecordList::getSelectedRows()
{
    QModelIndexList selectedRows = selectionModel()->selectedRows();

    if (selectedRows.length() == 1)
        return selectedRows[0].row();

    return -1;
}

QString RecordList::getCurrentTitle()
{
    return proxyModel->index(getSelectedRows(), 0).data().toString();
}

QString RecordList::getCurrentFile()
{
    return proxyModel->index(getSelectedRows(), 3).data().toString();
}

QString RecordList::getRow(const int &arg)
{
    if (proxyModel->hasChildren())
        return proxyModel->data(proxyModel->index(arg, 3)).toString();

    return QString();
}

//================================================================================================================
// public slots
//================================================================================================================

void RecordList::selectRow(int arg)
{
    selectionModel()->setCurrentIndex(proxyModel->index(arg,0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

//================================================================================================================
// private
//================================================================================================================

void RecordList::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return)
    {
        emit play();
        event->accept();
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}
