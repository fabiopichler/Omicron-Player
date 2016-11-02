/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

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
