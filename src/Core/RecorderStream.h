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

#pragma once

#include "Global.h"
#include "Stream.h"

#include <bassenc.h>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>

class RecordList;

class RecorderStream : public Stream
{
    Q_OBJECT

public:

    enum Event
    {
        Status,
        TimeSlider,
        Recording
    };

    RecorderStream(QWidget *);
    ~RecorderStream();

    void initDevice(int = -2);
    QStringList getDeviceInfo();

public slots:
    void record();
    void play();
    void stop();
    void loadList();
    void deleteFile();

private:
    void createEvents();
    void error(const char *);
    static BOOL CALLBACK recordingCallback(HRECORD, const void *, DWORD, void *);
    static void CALLBACK endSync(HSYNC, DWORD, DWORD, void *);

private slots:
    void update();
    void checkDir();

signals:
    void updateInfo(QWORD, DWORD);
    void updateValue(RecorderStream::Event, QVariant);
    void startedStream();
    void endOfStream();
    void startUpdadeTimer(int);
    void stopUpdateTimer();
    void showNotification(QString);
    void showError(QString);
    void showErrorDlg(QString);

public:
    RecordList *recordList;
    EncoderList encoderList[3];

private:
    QTimer *updateTimer, *checkDirTimer;
    QDateTime pathLastModified;

    int input;
    QString recordPath;

    HRECORD rchan;
    HENCODE hencode;
};

class RecordList : public QTreeView
{
    Q_OBJECT

public:
    RecordList(QWidget *);
    ~RecordList();
    void addRow(QString);
    void clear();
    void removeRow(const int &);
    int length();
    bool isEmpty();
    int getSelectedRows();
    QString getCurrentTitle();
    QString getCurrentFile();
    QString getRow(const int &);

public slots:
    void selectRow(int);

private:
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

signals:
    void play();

private:
    QStandardItemModel* model;
    QSortFilterProxyModel *proxyModel;
    QWidget *parent;
};
