/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Global.h"

#include <OmicronTK11/Qt/DropArea.hpp>

class QVBoxLayout;

namespace OmicronTK11 {
namespace OTKQT {
    class ErrorWindow;
}
}

class WindowBase : public OmicronTK11::OTKQT::DropArea
{
    Q_OBJECT
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor)
    Q_PROPERTY(int marginLayout READ marginLayout WRITE setMarginLayout)
    Q_PROPERTY(int spacingLayout READ spacingLayout WRITE setSpacingLayout)

public:
    WindowBase(QObject *appMain);
    ~WindowBase();

    virtual bool init() = 0;

    void show();
    void setWidget(QWidget *);
    QString &currentMode();

    QColor linkColor() const;
    int marginLayout() const;
    int spacingLayout() const;
    void setLinkColor(QColor);
    void setMarginLayout(int);
    void setSpacingLayout(int);

public slots:
    void setWidgetObj(QObject *w) { setWidget(static_cast<QWidget *>(w)); }

    void initConfigDialog();
    void openSite();
    void openFacebook();
    void about();

signals:
    void showError(QString);
    void showNotification(QString);

private:
    void changeWindow();

protected:
    QObject *appMain;
    OmicronTK11::OTKQT::ErrorWindow *m_errorWindow;
    QString mode;

private:
    QVBoxLayout *mainLayout;
    QPalette customPalette;
};
