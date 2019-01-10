/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Global.h"

#include <bass.h>
#include <QThread>

#ifndef Q_OS_ANDROID
    #include <basscd.h>
#endif

struct EncoderList
{
    QString name;
    QList<int> bitrate;
    int index;
    static int current;
};

class Stream : public QThread
{
    Q_OBJECT

public:
    Stream();

    static bool init();
    static void free();

    const int &getVolume() const;
    double getPosition() const;
    static QString getFileType(const DWORD &);

public slots:
    void updateFX(int, int);
    void setVolume(int);
    void setPosition(int);

protected:
    void setupDSP_EQ();

    HSTREAM stream;
    DWORD fxEQ, volfx;

    bool mplay, mpause, mprev;
    bool mstop;

private:
    int volume;
};
