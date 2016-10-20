/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MySlider.h"
#include <QMouseEvent>

MySlider::MySlider(QWidget *parent) : QSlider(parent)
{
    sendEvent = false;
    setPageStep(0);
}

//---------------------------------------------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------------------------------------------

void MySlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        sendEvent = true;

    QSlider::mousePressEvent(event);
}

void MySlider::mouseMoveEvent(QMouseEvent *event)
{
    sendEvent = false;
    QSlider::mouseMoveEvent(event);
}

void MySlider::mouseReleaseEvent(QMouseEvent *event)
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
