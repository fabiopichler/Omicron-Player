/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Widgets.h"
#include <QStyleOption>
#include <QPainter>
#include <QListView>
#include <QMouseEvent>

//================================================================================================================
// class Widget
//================================================================================================================

Widget::Widget(QWidget *parent) : QWidget(parent)
{
}

void Widget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//================================================================================================================
// class ComboBox
//================================================================================================================

ComboBox::ComboBox()
{
    QListView *listView = new QListView(this);
    listView->setObjectName("ComboBox");
    setView(listView);
}

//================================================================================================================
// class Slider
//================================================================================================================

Slider::Slider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{
    sendEvent = false;
    setPageStep(0);
}

//---------------------------------------------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------------------------------------------

void Slider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        sendEvent = true;

    QSlider::mousePressEvent(event);
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    sendEvent = false;
    QSlider::mouseMoveEvent(event);
}

void Slider::mouseReleaseEvent(QMouseEvent *event)
{
    if (sendEvent)
    {
        sendEvent = false;

        double halfHandleWidth = (0.5 * 9) + 0.5;
        int adaptedPosX = event->x();

        if (adaptedPosX < halfHandleWidth)
            adaptedPosX = halfHandleWidth;
        if (adaptedPosX > width() - halfHandleWidth)
            adaptedPosX = width() - halfHandleWidth;

        double newWidth = (width() - halfHandleWidth) - halfHandleWidth;
        double normalizedPosition = (adaptedPosX - halfHandleWidth) / newWidth;
        int pos = minimum() + ((maximum()-minimum()) * normalizedPosition);

        setValue(pos);
        emit sliderMoved(pos);
        event->accept();
    }

    QSlider::mouseReleaseEvent(event);
}

