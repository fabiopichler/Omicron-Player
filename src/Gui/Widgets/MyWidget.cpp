/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MyWidget.h"
#include <QStyleOption>
#include <QPainter>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent)
{
}

void MyWidget::updateStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void MyWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
