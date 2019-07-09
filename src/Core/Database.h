/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#pragma once

#include "Global.h"

#include <QtSql>

#include <vector>

class Database : public QObject
{
    Q_OBJECT

public:
    static bool init(const QString &);
    static void free();

    // Values
    static bool add(const QString &, const QString &, const QVariant &);
    static bool setValue(const QString &, const QString &, const QVariant &);
    static bool setValues(const QString &, const QVariantList &, const QVariantList &);
    static QVariant value(const QString &, const QString &, const QVariant &defValue = QVariant());
    static bool exist(const QString &, const QString &);

    // Remove
    static bool clear(const QString &, const QString &);
    static bool remove(const QString &, const QString &, const QString &);
    static bool removeRows(const QString &);
    static bool removeTable(const QString &);
    static bool removeDatabase();

    // Favorites
    static bool addMusicFavorite(QString);
    static QStringList getMusicFavorites();
    static bool addRadioFavorite(QStringList);
    static QList<QStringList> getRadioFavorites();

    // Equalizer
    static bool addEqualizerPreset(const QString &, const std::vector<int> &);
    static bool setEqualizerValues(const QString &);
    static bool setEqualizerPreset(const int &, const QString &, const std::vector<int> &);
    static std::vector<int> getEqualizerPreset(const int &);
    static int getEqualizerPresets(std::vector<int> &, std::vector<std::string> &);

    // Others
    static void defaultConfig();
    static void upgrade();

private:
    static QSqlDatabase *db;
};
