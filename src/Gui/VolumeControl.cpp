/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "VolumeControl.h"
#include <QtWidgets>

VolumeControl::VolumeControl(QWidget *parent) : QPushButton(parent)
{
    mmute = false;
    setObjectName("volumeBt");
    setToolTip("Ajustar o Volume");

    QWidget *popup = new QWidget(this);
    popup->setMinimumWidth(240);

    button = new QPushButton(popup);
    button->setObjectName("volumeBt");
    button->setToolTip("Desativar som");

    slider = new Slider(Qt::Horizontal, popup);
    slider->setRange(0, 100);

    label = new QLabel(popup);
    label->setAlignment(Qt::AlignCenter);
    label->setText("0%");
    label->setMinimumWidth(45);

    connect(button, &QPushButton::clicked, this, &VolumeControl::changeMute);
    connect(slider, &QAbstractSlider::valueChanged, this, &VolumeControl::volumeChanged);
    connect(slider, &QAbstractSlider::valueChanged, this, &VolumeControl::updateVolume);

    QBoxLayout *popupLayout = new QHBoxLayout(popup);
    popupLayout->setMargin(2);
    popupLayout->addWidget(label);
    popupLayout->addWidget(slider);
    popupLayout->addSpacing(4);
    popupLayout->addWidget(button);

    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(popup);

    QMenu *menu = new QMenu(this);
    menu->addAction(action);
    menu->installEventFilter(this);

    setMenu(menu);
}

int VolumeControl::volume() const
{
    return slider->value();
}

//================================================================================================================
// public slots
//================================================================================================================

void VolumeControl::setVolume(int volume)
{
    slider->setValue(volume);
}

//================================================================================================================
// private slots
//================================================================================================================

void VolumeControl::updateVolume(int volume)
{
    label->setText(QString::number(volume) + "%");
    mute(false, false);
}

void VolumeControl::changeMute()
{
    if (mmute)
        mute(false);
    else
        mute(true);
}

//================================================================================================================
// private
//================================================================================================================

void VolumeControl::mute(bool act, bool event)
{
    if (act == mmute)
        return;

    if (act)
    {
        mmute = true;
        setProperty("mute", true);
        button->setProperty("mute", true);
        button->setToolTip("Ativar som");
        if (event)
            emit volumeChanged(0);
    }
    else
    {
        mmute = false;
        setProperty("mute", false);
        button->setProperty("mute", false);
        button->setToolTip("Desativar som");
        if (event)
            emit volumeChanged(slider->value());
    }

    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();

    style()->unpolish(this);
    style()->polish(this);
    update();
}

bool VolumeControl::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Show && obj == this->menu())
    {
        menu()->move(mapToGlobal(QPoint(width() - menu()->width(), (height() - menu()->height()) / 2)));
        return true;
    }
    return false;
}
