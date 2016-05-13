/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "Global.h"

#include <QtSql>

class Database : public QObject
{
    Q_OBJECT

public:
    static bool init(const QString &);
    static void free();

    // Values
    static bool addValue(const QString &, const QString &, const QVariant &);
    static bool setValue(const QString &, const QString &, const QVariant &);
    static bool setValues(const QString &, const QVariantList &, const QVariantList &);
    static QVariant value(const QString &, const QString &, const QVariant &defValue = QVariant());

    // Remove
    static bool clear(const QString &, const QString &);
    static bool remove(const QString &, const QString &id = QString(), const QString &col = QString());
    static bool removeTable(const QString &);
    static bool removeDatabase();

    // Favorites
    static bool addMusicFavorite(QString);
    static QStringList getMusicFavorites();
    static bool addRadioFavorite(QStringList);
    static QList<QStringList> getRadioFavorites();

    // Equalizer
    static bool addEqualizerPreset(const QString &, const QList<int> &);
    static bool setEqualizerValues(const QString &);
    static bool setEqualizerPreset(const int &, const QString &, const QList<int> &);
    static QList<int> getEqualizerPreset(const int &);
    static int getEqualizerPresets(QList<int> &, QStringList &);

    // Others
    static void defaultConfig();

private:
    static QSqlDatabase *db;
};
