
#pragma once

#include "Global.h"

#include <QSettings>

class Theme
{
public:
    static void init();
    static void free();
    static bool load();
    static QVariant get(const QString &key, const QVariant &value = QVariant());
    static QString getString(const QString &key, const QVariant &value = QVariant());
    static QList<QStringList> themes();
    static QList<QStringList> styles();

private:
    static QString loadCss(const QString &);

private:
    static QSettings *settings;
};
