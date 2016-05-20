/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "DialogBase.h"
#include "../Core/Theme.h"
#include <QStyleOption>
#include <QPainter>

DialogBase::DialogBase(QWidget *parent) : QDialog(parent)
{
    titleBar = new Titlebar(this, DIALOG);

    setObjectName("window");
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(AppName);
}

DialogBase::~DialogBase()
{
}

void DialogBase::setWindowTitle(const QString &title)
{
    titleBar->label->setText(title);
    QDialog::setWindowTitle(title);
}

void DialogBase::setLayout(QLayout *layout)
{
    Widget *mainWidget = new Widget(this);
    mainWidget->setLayout(layout);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(mainWidget);

    QDialog::setLayout(mainLayout);
}

int DialogBase::marginLayout() const
{
    return mainLayout->margin();
}

int DialogBase::spacingLayout() const
{
    return mainLayout->spacing();
}

void DialogBase::setMarginLayout(int margin)
{
    mainLayout->setMargin(margin);
}

void DialogBase::setSpacingLayout(int spacing)
{
    mainLayout->setSpacing(spacing);
}

//================================================================================================================
// public slots
//================================================================================================================

void DialogBase::close()
{
    QDialog::close();
}

//================================================================================================================
// private
//================================================================================================================

void DialogBase::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
