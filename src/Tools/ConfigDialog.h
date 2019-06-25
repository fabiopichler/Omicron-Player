/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "ConfigPages.h"

#include <OmicronTK11/Qt/Dialog.hpp>

#include <QListWidgetItem>
#include <QStackedWidget>

class ConfigDialog : public OmicronTK11::OTKQT::Dialog
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

signals:
    void restartApp();

private:
    QObject *parentMain;
    QWidget *parent;
    OmicronTK11::OTKQT::Widget *contentsWidget;
    QListWidget *contentsListWidget;
    QStackedWidget *pagesWidget;
    QLabel *contentsTitle;
    QStringList titles;
    QPushButton *defaultsButton, *closeButton;
    static int lastPage;
};
