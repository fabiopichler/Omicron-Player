/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "../Core/Global.h"
#include "DropArea.h"
#include "../Core/Database.h"

#include <QDragEnterEvent>
#include <QMimeData>

DropArea::DropArea(QWidget *parent) : Widget(parent)
{
    setAcceptDrops(true);
}

//================================================================================================================
// protected
//================================================================================================================

void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
    emit dragAndDrop(true);
    event->acceptProposedAction();
}

void DropArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void DropArea::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QString filter = QString(FileNameFilter).replace(" ", "|").remove("*");
        QStringList list;

        QList<QUrl> urls = mimeData->urls();
        for (int i = 0; i < urls.length(); i++)
            list << urls[i].toString();

#ifdef Q_OS_WIN
        QString prefix = "file:///";
#elif defined(Q_OS_LINUX)
        QString prefix = "file://";
#endif

        if (list.length() == 1 && list[0].contains(QRegExp("."+PlaylistExt+"$", Qt::CaseInsensitive)))
        {
            list[0].remove(prefix);
        }
        else if (Database::value("MusicConfig", "allowAnyFile", false).toBool())
        {
            for (int i = list.length()-1; i > -1; i--)
                list[i].remove(prefix);
        }
        else
        {
            for (int i = list.length()-1; i > -1; i--)
            {
                if (list[i].contains(QRegExp("^"+prefix+"(.*)("+filter+")$",Qt::CaseInsensitive)))
                    list[i].remove(prefix);
                else
                    list.removeAt(i);
            }
        }

        if (!list.isEmpty())
        {
            list.push_front("--nohide");
            emit filesFromDropArea(list);
        }
    }

    emit dragAndDrop(false);
    event->acceptProposedAction();
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit dragAndDrop(false);
    event->accept();
}

