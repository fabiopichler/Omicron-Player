/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QThread>
#include <QMutex>
#include <bass.h>

class Fade : public QThread
{
    Q_OBJECT

public:
    Fade(const QString &, const bool &);
    ~Fade();

    void in(const HSTREAM &, const int &);
    void out(const HSTREAM &);

private:
    void streamFree(const HSTREAM &);
    void run() Q_DECL_OVERRIDE;

    const QString configName;
    QMutex mutex;
    HSTREAM oldStream;
    long waitTimeout;
    const bool &mstop;
};
