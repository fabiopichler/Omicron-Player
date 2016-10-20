/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MyLabel.h"

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{
}

MyLabel::~MyLabel()
{
}

void MyLabel::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}
