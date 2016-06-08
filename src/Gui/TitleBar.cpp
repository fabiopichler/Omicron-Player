/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "TitleBar.h"

Titlebar::Titlebar(QWidget *parent, const int &flag) : Widget(parent)
{
    this->parent = parent;
    setObjectName("titleBar");

    titleIcon = new QLabel;
    titleIcon->setObjectName("windowIcon");
    //titleIcon->setPixmap(QPixmap::fromImage(QImage(GlobalData::getQrcPath("icon.png"))
    //                                        .scaled(24,24,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    titleIcon->setPixmap(QPixmap(Global::getQrcPath("icon.png")));
    titleIcon->setStyleSheet("border: none; background: transparent;");
    titleIcon->setContentsMargins(4,0,6,0);

    label = new QLabel(parent->windowTitle());
    label->setObjectName("windowTitle");

    buttonMinimize = new QPushButton;
    buttonMinimize->setObjectName("minimize");
    buttonMinimize->setToolTip("Minimizar");

    buttonMinimizeTray = new QPushButton;
    buttonMinimizeTray->setObjectName("minimizeTray");
    buttonMinimizeTray->setToolTip("Minimizar para a Bandeja");

    buttonClose = new QPushButton;
    buttonClose->setObjectName("close");
    buttonClose->setToolTip("Fechar");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(titleIcon);
    layout->addWidget(label, 1);

    if (flag != DIALOG)
    {
        layout->addWidget(buttonMinimize);
        layout->addWidget(buttonMinimizeTray);
    }

    layout->addWidget(buttonClose);
    layout->setContentsMargins(3,2,4,2);
    layout->setSpacing(1);
    setLayout(layout);

    if (flag == DIALOG)
    {
        connect(buttonClose, SIGNAL(clicked()), parent, SLOT(close()));
    }
    else
    {
        connect(buttonMinimize, SIGNAL(clicked()), parent, SLOT(showMinimized()));
        connect(buttonMinimizeTray, SIGNAL(clicked()), parent, SLOT(hide()));
        connect(buttonClose, SIGNAL(clicked()), this, SLOT(quitApp()));
    }
}

Titlebar::~Titlebar()
{
}

//================================================================================================================
// private
//================================================================================================================

void Titlebar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        cursor = mapToParent(event->pos());
}

void Titlebar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        parent->move(event->globalPos() - cursor);
}

//================================================================================================================
// private slots
//================================================================================================================

void Titlebar::quitApp()
{
    parent->hide();
    qApp->quit();
}
