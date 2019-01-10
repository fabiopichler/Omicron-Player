/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MyLabel(QWidget * = nullptr);
    ~MyLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
};
