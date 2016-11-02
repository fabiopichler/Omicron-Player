/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "MusicWindow.h"
#include "RadioWindow.h"
#include "RecorderWindow.h"
#include "TitleBar.h"

class MainWindow : public MyWidget
{
    Q_OBJECT
    Q_PROPERTY(int marginLayout READ marginLayout WRITE setMarginLayout)
    Q_PROPERTY(int spacingLayout READ spacingLayout WRITE setSpacingLayout)

public:
    MainWindow(QObject *, QSettings *);
    ~MainWindow();
    void initWindow(MyWidget *);
    QString &currentMode();

    int marginLayout() const;
    int spacingLayout() const;
    void setMarginLayout(int);
    void setSpacingLayout(int);

public slots:
    void setWindowTitle(const QString &);

private:
    void changeWindow();

private:
    QObject *parentMain;
    MyWidget *centralWidget;
    Titlebar *titleBar;
    QSettings *iniSettings;
    QVBoxLayout *mainLayout;
};
