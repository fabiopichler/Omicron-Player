/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Global.h"
#include "Widgets.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QObject>

class Titlebar : public Widget
{
    Q_OBJECT

public:
    Titlebar(QWidget *, const int &flag = -1);
    ~Titlebar();

private:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private slots:
    void quitApp();

public:
    QLabel *label;

private:
    QWidget *parent;
    QLabel *titleIcon;
    QPushButton *buttonMinimize, *buttonMinimizeTray, *buttonClose;
    QPoint cursor;
};
