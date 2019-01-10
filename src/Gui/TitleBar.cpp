/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "TitleBar.h"
#include "../Core/Theme.h"

Titlebar::Titlebar(QWidget *parent, const int &flag) : MyWidget(parent)
{
    this->parent = parent;

    QWidget *uiWidget;

    if (!(uiWidget = Theme::loadUi("TitleBar.xml", this)))
    {
        throw "Ops! Algo deu errado...\nHouve um erro ao carregar o arquivo \"TitleBar.xml\" do tema atual.";
        return;
    }

    titleIcon = uiWidget->findChild<QLabel *>("windowIcon");
    titleIcon->setPixmap(QPixmap(Global::getQrcPath("icon.png")));

    label = uiWidget->findChild<QLabel *>("windowTitle");
    label->setText(parent->windowTitle());

    buttonMinimize = uiWidget->findChild<QPushButton *>("minimize");
    buttonMinimize->setToolTip("Minimizar");

    buttonMinimizeTray = uiWidget->findChild<QPushButton *>("minimizeTray");
    buttonMinimizeTray->setToolTip("Minimizar para a Bandeja");

    buttonClose = uiWidget->findChild<QPushButton *>("close");
    buttonClose->setToolTip("Fechar");

    if (flag == DIALOG)
    {
        buttonMinimize->hide();
        buttonMinimizeTray->hide();
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(uiWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
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
