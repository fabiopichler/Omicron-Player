/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Global.h"

#include <QSettings>

const QString defaultTheme = "app:fpm-player-dark";

class Theme
{
public:
    static void init();
    static void free();
    static bool load();
    static QWidget *loadUi(QString, QWidget * = nullptr);
    static bool setDefault(const QString &);
    static QVariant get(const QString &key, const QVariant &value = QVariant());
    static QString getString(const QString &key, const QVariant &value = QVariant());
    static QList<QStringList> themes();
    static QList<QStringList> styles();

private:
    static QString loadCss(const QString &);

private:
    static QSettings *settings;
};
