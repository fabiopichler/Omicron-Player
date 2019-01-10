/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "../../src/Gui/Widgets/MyLabel.h"
#include "MyLabelPlugin.h"

#include <QtPlugin>

MyLabelPlugin::MyLabelPlugin(QObject *parent) : QObject(parent)
{
    m_initialized = false;
}

void MyLabelPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool MyLabelPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MyLabelPlugin::createWidget(QWidget *parent)
{
    return new MyLabel(parent);
}

QString MyLabelPlugin::name() const
{
    return QLatin1String("MyLabel");
}

QString MyLabelPlugin::group() const
{
    return QLatin1String("");
}

QIcon MyLabelPlugin::icon() const
{
    return QIcon();
}

QString MyLabelPlugin::toolTip() const
{
    return QLatin1String("");
}

QString MyLabelPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool MyLabelPlugin::isContainer() const
{
    return false;
}

QString MyLabelPlugin::domXml() const
{
    return QLatin1String("<widget class=\"MyLabel\" name=\"myLabel\">\n</widget>\n");
}

QString MyLabelPlugin::includeFile() const
{
    return QLatin1String("mylabel.h");
}

