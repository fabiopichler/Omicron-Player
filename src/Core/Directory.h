/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Gui/DialogBase.h"

#include <QThread>
#include <QFileInfo>
#include <functional>
#include <QLabel>

class Directory : public QThread
{
    Q_OBJECT

public:
    explicit Directory(QWidget *);
    ~Directory();

    void getAllFiles(QString &, std::function<void(QFileInfo &)>);
    void executeLater(std::function<void(void)>);

public slots:
    void update(QFileInfo);

private:
    void run();

signals:
    void updateFiles(QFileInfo);

private:
    QWidget *parent;
    DialogBase *dialog;
    QLabel *label;

    QString dirName;
    int countFiles;
    bool stop;

    std::function<void(QFileInfo &)> func;
    std::function<void(void)> execLater;
};
