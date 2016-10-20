/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "../../src/Gui/Widgets/MySlider.h"
#include "MySliderPlugin.h"

#include <QtPlugin>

MySliderPlugin::MySliderPlugin(QObject *parent) : QObject(parent)
{
    m_initialized = false;
}

void MySliderPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool MySliderPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MySliderPlugin::createWidget(QWidget *parent)
{
    return new MySlider(parent);
}

QString MySliderPlugin::name() const
{
    return QLatin1String("MySlider");
}

QString MySliderPlugin::group() const
{
    return QLatin1String("");
}

QIcon MySliderPlugin::icon() const
{
    return QIcon();
}

QString MySliderPlugin::toolTip() const
{
    return QLatin1String("");
}

QString MySliderPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool MySliderPlugin::isContainer() const
{
    return false;
}

QString MySliderPlugin::domXml() const
{
    return QLatin1String("<widget class=\"MySlider\" name=\"mySlider\">\n</widget>\n");
}

QString MySliderPlugin::includeFile() const
{
    return QLatin1String("myslider.h");
}

