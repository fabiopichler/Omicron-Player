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
#include "ConfigPages.h"

#include <QListWidgetItem>
#include <QStackedWidget>

class ConfigDialog : public DialogBase
{
    Q_OBJECT

public:
    ConfigDialog(QObject *, QWidget *);

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void createWidgets();
    void createLayouts();
    void createEvents();
    void createIcons();

private:
    QObject *parentMain;
    QWidget *parent;
    MyWidget *contentsWidget;
    QListWidget *contentsListWidget;
    QStackedWidget *pagesWidget;
    QLabel *contentsTitle;
    QStringList titles;
    QPushButton *defaultsButton, *closeButton;
    static int lastPage;
};
