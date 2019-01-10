/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "../../src/Gui/Widgets/MyComboBox.h"
#include "MyComboBoxPlugin.h"

#include <QtPlugin>

MyComboBoxPlugin::MyComboBoxPlugin(QObject *parent) : QObject(parent)
{
    m_initialized = false;
}

void MyComboBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool MyComboBoxPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MyComboBoxPlugin::createWidget(QWidget *parent)
{
    return new MyComboBox(parent);
}

QString MyComboBoxPlugin::name() const
{
    return QLatin1String("MyComboBox");
}

QString MyComboBoxPlugin::group() const
{
    return QLatin1String("");
}

QIcon MyComboBoxPlugin::icon() const
{
    return QIcon();
}

QString MyComboBoxPlugin::toolTip() const
{
    return QLatin1String("");
}

QString MyComboBoxPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool MyComboBoxPlugin::isContainer() const
{
    return false;
}

QString MyComboBoxPlugin::domXml() const
{
    return QLatin1String("<widget class=\"MyComboBox\" name=\"myComboBox\">\n</widget>\n");
}

QString MyComboBoxPlugin::includeFile() const
{
    return QLatin1String("mycombobox.h");
}

