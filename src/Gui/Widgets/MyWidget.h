/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QWidget>

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget * = 0);
    static void updateStyle(QWidget *);

    QString mode;

private:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
};
