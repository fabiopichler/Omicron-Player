/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Theme.h"
#include "Database.h"

QSettings *Theme::settings = nullptr;

void Theme::init()
{
    settings = nullptr;
}

void Theme::free()
{
    delete settings;
    settings = nullptr;
}

bool Theme::load()
{
#ifdef Q_OS_ANDROID // Não carregar o tema personalizado no android. (apenas para testes)
    return true;
#endif

    QString css,
            style,
            themePath = Database::value("Config", "theme").toString();

    if (themePath.contains(QRegExp("^app:")))
    {
        themePath = Global::getAppPath() + (Global::inDevelopment ? "../../project/" : "")
                                         + "themes/" + themePath.remove(QRegExp("^app:")) + "/";
    }
    else if (themePath.contains(QRegExp("^config:")))
    {
        themePath = Global::getConfigPath() + "themes/" + themePath.remove(QRegExp("^config:")) + "/";
    }

    if (!QFile::exists(themePath + "theme.ini"))
    {
        if (Database::value("Config", "theme").toString() == "app:tche-media-theme")
        {
            QMessageBox::critical(nullptr, "Erro", "Não foi possível carregar o Tema Padrão. Reinstale o programa para "
                                                   "resolver este problema.");
            return false;
        }

        Database::setValue("Config", "theme", "app:tche-media-theme");
        Database::setValue("Config", "style", "default");
        return load();
    }

    Global::setThemePath(themePath);

    delete settings;
    settings = new QSettings(themePath + "theme.ini", QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    style = Database::value("Config", "style").toString();

    if (!QFile::exists(themePath + style + ".css"))
    {
        Database::setValue("Config", "style", "default");
        style = "default";
    }

    css = loadCss("images.css");
    css += loadCss("theme.css");

    if (style != "default")
        css += loadCss(style + ".css");

#ifdef Q_OS_WIN
    css += loadCss("windows.css");
#elif defined(Q_OS_LINUX)
    css += loadCss("linux.css");
#endif // Q_OS_WIN

    qApp->setStyleSheet(css);

    return true;
}

QVariant Theme::get(const QString &key, const QVariant &value)
{
    return settings->value("Theme/" + key, value);
}

QString Theme::getString(const QString &key, const QVariant &value)
{
    return settings->value("Theme/" + key, value).toString().replace("{theme_path}", Global::getThemePath());
}

QList<QStringList> Theme::themes()
{
    QList<QStringList> list;

    for (int i = 0; i < 2; i++)
    {
        QString path;

        if (i == 0)
            path = Global::getAppPath(Global::inDevelopment ? "../../project/themes" : "themes");
        else
            path = Global::getConfigPath("themes");

        QDirIterator dir(path, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        while (dir.hasNext())
        {
            dir.next();
            QString iniPath = dir.filePath() + "/theme.ini";

            if (QFile::exists(iniPath))
            {
                QStringList values;
                QSettings ini(iniPath, QSettings::IniFormat);
                ini.setIniCodec("UTF-8");

                values << ini.value("Info/Name", "Unnamed").toString();
                values << (i == 0 ? "app:" : "config:") + dir.fileName();

                list << values;
            }
        }
    }

    return list;
}

QList<QStringList> Theme::styles()
{
    QList<QStringList> list;
    QStringList styles = settings->value("Styles/List").toStringList();
    list += {"Padrão", "default"};

    for (int i = 0; i < styles.length(); i++)
    {
        QStringList s = styles[i].split("=");
        if (QFile::exists(Global::getThemePath(s[0] + ".css")))
        {
            s.move(0, 1);
            list << s;
        }
    }

    return list;
}

//================================================================================================================
// private
//================================================================================================================

QString Theme::loadCss(const QString &name)
{
    QString str;
    QFile file(Global::getThemePath(name));

    if (file.exists())
    {
        if (file.open(QFile::ReadOnly))
        {
            str = file.readAll();
            file.close();
        }
        else
        {
            QMessageBox::warning(nullptr,"Erro!","Erro ao carregar o arquivo de estilo: "+name);
        }
    }

    return str.replace("{theme_path}", Global::getThemePath()) + "\n\n";
}

