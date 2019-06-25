/*******************************************************************************
  Omicron Media Player
  Copyright (c) 2016, Fábio Pichler
  All rights reserved.
  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net
*******************************************************************************/

#include "WindowBase.h"
#include "../Core/Database.h"
#include "../Tools/ConfigDialog.h"
#include "AboutDialog.hpp"

#include <OmicronTK11/Qt/AppInfo.hpp>
#include <OmicronTK11/Qt/ErrorWindow.hpp>

#include <QBoxLayout>
#include <QIcon>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QStyle>
#include <QDesktopWidget>

#include <sstream>
#include <iostream>
#include <vector>

using namespace OmicronTK11;

WindowBase::WindowBase(QObject *appMain)
    : appMain(appMain)
{
    customPalette = qApp->palette();

    m_errorWindow = new OTKQT::ErrorWindow(this);

    mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(QString(OTKQT::AppInfo::themePath()).append("/images/titlebar/icon2.png")));
    setFixedWidth(500);
}

WindowBase::~WindowBase()
{
    QPoint position(pos());
    Database::setValue("Current", "windowPosition", QString("%1,%2").arg(position.x()).arg(position.y()));
    Database::setValue("Current", "mode", mode);
}

void WindowBase::show()
{
    std::istringstream f(Database::value("Current", "windowPosition").toString().toStdString());
    std::vector<int> position;
    std::string s;

    while (std::getline(f, s, ','))
        position.push_back(std::stoi(s));

    if (position.size() == 2)
    {
        move(position[0], position[1]);
    }
    else
    {
        adjustSize();
        move(QApplication::desktop()->screen()->rect().center() - rect().center());
    }

    QWidget::show();
}

void WindowBase::setWidget(QWidget *widget)
{
    mainLayout->addWidget(widget, 1);
    setLayout(mainLayout);

}

QString &WindowBase::currentMode()
{
    return mode;
}

QColor WindowBase::linkColor() const
{
    return customPalette.color(QPalette::Link);
}

int WindowBase::marginLayout() const
{
    return mainLayout->margin();
}

int WindowBase::spacingLayout() const
{
    return mainLayout->spacing();
}

void WindowBase::setLinkColor(QColor color)
{
    customPalette.setColor(QPalette::Link, color);
    customPalette.setColor(QPalette::LinkVisited, color);
    qApp->setPalette(customPalette);
}

void WindowBase::setMarginLayout(int margin)
{
    mainLayout->setMargin(margin);
}

void WindowBase::setSpacingLayout(int spacing)
{
    mainLayout->setSpacing(spacing);
}

//! Abre o site oficial no navegador padrão do usuário.
void WindowBase::openSite()
{
    QDesktopServices::openUrl(QUrl(OfficialSite));
}

//! Abre a página oficial do Facebook no navegador padrão do usuário.
void WindowBase::openFacebook()
{
    QDesktopServices::openUrl(QUrl(PageOnFacebook));
}

//! Exibe um diálogo com o "sobre o programa".
void WindowBase::about()
{
    AboutDialog(this).exec();
}

//! Inicializa um diálogo contendo as configurações do programa.
void WindowBase::initConfigDialog()
{
    ConfigDialog(appMain, this).exec();
}

