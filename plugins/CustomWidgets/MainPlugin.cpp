/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MyComboBoxPlugin.h"
#include "MySliderPlugin.h"
#include "MyLabelPlugin.h"
#include "MainPlugin.h"

Widgets::Widgets(QObject *parent) : QObject(parent)
{
    m_widgets.append(new MyComboBoxPlugin(this));
    m_widgets.append(new MySliderPlugin(this));
    m_widgets.append(new MyLabelPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> Widgets::customWidgets() const
{
    return m_widgets;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(widgetsplugin, Widgets)
#endif // QT_VERSION < 0x050000
