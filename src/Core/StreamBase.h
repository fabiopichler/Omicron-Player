/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Global.h"
#include "Database.h"

#include <bass.h>

class StreamBase : public QThread
{
    Q_OBJECT

public:
    StreamBase();
    const int &getVolume() const;

public slots:
    void updateFX(int, int);
    void setVolume(int);

protected:
    void setupDSP_EQ();

    HSTREAM stream;
    DWORD fxEQ, volfx;

    bool mplay, mpause, mprev;
    bool mstop;

private:
    int volume;
};
