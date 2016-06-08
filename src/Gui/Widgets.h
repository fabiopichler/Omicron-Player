/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QWidget>
#include <QComboBox>
#include <QSlider>

//================================================================================================================
// class Widget
//================================================================================================================

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QString mode;

private:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
};

//================================================================================================================
// class ComboBox
//================================================================================================================

class ComboBox : public QComboBox
{
public:
    ComboBox();
};

//================================================================================================================
// class Slider
//================================================================================================================

class Slider : public QSlider
{
public:
    Slider(Qt::Orientation, QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

    bool sendEvent;
};

