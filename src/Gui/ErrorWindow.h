/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"
#include "DialogBase.h"
#include "../Core/Global.h"

#include <QTextEdit>

class ErrorWindow : public DialogBase
{
    Q_OBJECT

public:
    ErrorWindow(QWidget *);
    ~ErrorWindow();
    void addError(QString);

private:
    void createWidgets();
    void createLayouts();
    void createEvents();

signals:
    void stopStream();

private:
    QLabel *infoLabel;
    QPushButton *stopButton, *clearButton, *closeButton;
    QTextEdit *contentTextEdit;
};

