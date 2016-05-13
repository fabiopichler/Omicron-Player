/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Gui/TitleBar.h"
#include "../Core/Database.h"

#include <QDialog>

class DialogBase : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBase(QWidget *);
    ~DialogBase();
    void setWindowTitle(const QString &);
    void setLayout(QLayout *);

public slots:
    virtual void close();

private:
    void paintEvent(QPaintEvent *);

private:
    Titlebar *titleBar;
};
