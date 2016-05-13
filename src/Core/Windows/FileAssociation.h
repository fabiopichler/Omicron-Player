/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include <QApplication>

#ifdef Q_OS_WIN

class FileAssociation
{
public:
    FileAssociation(const QString &);
    QStringList registerList(const QList<QStringList> &, const bool &forStandard = false);
    bool isDefault(const QString &);
    void remove(const QList<QStringList> &extList);

private:
    QString execPath;
    QString progId;
};

#endif // Q_OS_WIN
