/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "FileAssociation.h"
#include "../Global.h"

#ifdef Q_OS_WIN

//#define _WIN32_WINNT 0x0600

#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <Shlobj.h>
#include <Winreg.h>
#include "RegDelnodeRecurse.cpp"

FileAssociation::FileAssociation(const QString &path) : execPath(path)
{
    progId = AppNameId + ".";

    execPath.replace("/", "\\");
    execPath.push_front("\"");
    execPath.push_back("\"");
}

QStringList FileAssociation::registerList(const QList<QStringList> &extList, const bool &forStandard)
{
    QStringList errors;
    QSettings *progIdQS = nullptr,
              *classes = nullptr,
              *explorer = nullptr;

    for (int i = 0; i < extList.length(); i++)
    {
        QString ext = extList[i][0];
        QString desc = extList[i][1];
        QString _progId = progId + ext;

        if (ext.isEmpty() || desc.isEmpty() || _progId.isEmpty())
            continue;

        progIdQS = new QSettings("HKEY_CURRENT_USER\\Software\\Classes\\" + _progId, QSettings::NativeFormat);
        classes = new QSettings("HKEY_CURRENT_USER\\Software\\Classes\\." + ext, QSettings::NativeFormat);
        explorer = new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."
                          + ext, QSettings::NativeFormat);

        if ((!progIdQS || !explorer) || (progIdQS && !progIdQS->isWritable()) || (explorer && !explorer->isWritable()))
        {
            delete progIdQS;
            delete classes;
            delete explorer;
            continue;
        }

        progIdQS->setValue("Default", desc);

        if (ext == PlaylistExt)
        {
            progIdQS->setValue("DefaultIcon/Default", execPath + ",2");
            progIdQS->setValue("shell/open/Default", "Abrir playlist com o " + AppName);
        }
        else
        {
            progIdQS->setValue("DefaultIcon/Default", execPath + ",1");
            progIdQS->setValue("shell/enqueue/Default", "Adicionar à lista do " + AppName);
            progIdQS->setValue("shell/enqueue/MultiSelectModel", "Player");
            progIdQS->setValue("shell/enqueue/command/Default", execPath + " --add-file \"%1\"");
            progIdQS->setValue("shell/open/Default", "Reproduzir com o " + AppName);
        }

        progIdQS->setValue("shell/open/MultiSelectModel", "Single");
        progIdQS->setValue("shell/open/command/Default", execPath + " --open-file \"%1\"");
        classes->setValue("OpenWithProgIds/" + _progId, "");
        explorer->setValue("OpenWithProgIds/" + _progId, "");

        if (forStandard)
        {
            classes->setValue("Default", _progId);

            /*HKEY hKey;
            if (RegOpenKeyExW(HKEY_CURRENT_USER,
                              QString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."+ext)
                              .toStdWString().data(), 0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
            {
                RegDeleteTreeW(hKey, L"UserChoice");
                RegCloseKey(hKey);
            }*/

            RegDelnode(HKEY_CURRENT_USER,
                       (wchar_t*) QString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."
                                          +ext+"\\UserChoice").toStdWString().data());

            explorer->setValue("UserChoice/Progid", _progId);

            if (explorer->status() != QSettings::NoError || progIdQS->status() != QSettings::NoError)
                errors << ext + " - " + desc;
        }

        delete progIdQS;
        delete explorer;
        delete classes;
    }

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, NULL, NULL);

    return errors;
}

bool FileAssociation::isDefault(const QString &ext)
{
    QString value;

    value = QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."
                      + ext, QSettings::NativeFormat).value("UserChoice/Progid").toString();

    return (value == progId + ext);
}

void FileAssociation::remove(const QList<QStringList> &extList)
{
    for (int i = 0; i < extList.length(); i++)
    {
        QString ext = extList[i][0];
        QString _progId = progId + ext;
        HKEY hKey;

        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          QString("Software\\Classes\\."+ext+"\\OpenWithProgIds").toStdWString().data(),
                          0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
        {
            RegDeleteValueW(hKey, _progId.toStdWString().data());
            RegCloseKey(hKey);
        }

        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          QString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."
                                  +ext+"\\OpenWithProgIds")
                          .toStdWString().data(), 0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
        {
            RegDeleteValueW(hKey, _progId.toStdWString().data());
            RegCloseKey(hKey);
        }

        /*if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          QString("Software\\Classes").toStdWString().data(), 0L, KEY_ALL_ACCESS,
                          &hKey) == ERROR_SUCCESS)
        {
            RegDeleteTreeW(hKey, _progId.toStdWString().data());
            RegCloseKey(hKey);
        }*/

        RegDelnode(HKEY_CURRENT_USER, (wchar_t*) QString("Software\\Classes\\" + _progId).toStdWString().data());

        if (QSettings("HKEY_CURRENT_USER\\Software\\Classes\\."+ext, QSettings::NativeFormat)
                            .value("Default").toString() == _progId)
        {
            if (RegOpenKeyExW(HKEY_CURRENT_USER,
                              QString("Software\\Classes\\."+ext).toStdWString().data(), 0L, KEY_ALL_ACCESS,
                              &hKey) == ERROR_SUCCESS)
            {
                RegDeleteValueW(hKey, NULL);
                RegCloseKey(hKey);
            }
        }

        if (isDefault(ext))
        {
            /*if (RegOpenKeyExW(HKEY_CURRENT_USER,
                              QString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."+ext)
                              .toStdWString().data(), 0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
            {
                RegDeleteTreeW(hKey, L"UserChoice");
                RegCloseKey(hKey);
            }*/

            RegDelnode(HKEY_CURRENT_USER,
                       (wchar_t*) QString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\."
                                          +ext+"\\UserChoice").toStdWString().data());
        }
    }

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSHNOWAIT, NULL, NULL);
}

#endif // Q_OS_WIN
