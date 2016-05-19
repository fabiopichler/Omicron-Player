/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Widget.h"

#include <QPushButton>
#include <QMenu>
#include <QLabel>

class VolumeControl : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    VolumeControl(QWidget *parent = 0);
    int volume() const;

public slots:
    void setVolume(int volume);

private slots:
    void updateVolume(int);
    void changeMute();

private:
    void mute(bool, bool event = true);
    bool eventFilter(QObject * obj, QEvent *event);

signals:
    void volumeChanged(int volume);

private:
    QPushButton *button;
    QLabel *label;
    Slider *slider;
    bool mmute;
};

