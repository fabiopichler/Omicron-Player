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

#include "RadioUi.hpp"

#include "../Core/Database.h"
#include "../Tools/Equalizer.h"
#include "../Core/MusicStream.h"

#include "RadioWindow.h"

#include <OmicronTK/Qt/AppInfo.hpp>
#include <OmicronTK/Qt/ErrorWindow.hpp>
#include <OmicronTK/Qt/Theme.hpp>
#include <OmicronTK/Qt/DirectoryDialog.hpp>
#include <OmicronTK/Qt/VolumeControl.hpp>
#include <OmicronTK/Qt/Slider.hpp>
#include <OmicronTK/Qt/ComboBox.hpp>
#include <OmicronTK/Qt/TitleBarWidget.hpp>
#include <OmicronTK/Qt/Label.hpp>

#include <OmicronTK/Qt/Lua/LuaObject.hpp>
#include <OmicronTK/Qt/Lua/LuaWidget.hpp>
#include <OmicronTK/Qt/Lua/LuaLabel.hpp>
#include <OmicronTK/Qt/Lua/LuaPushButton.hpp>
#include <OmicronTK/Qt/Lua/LuaProgressBar.hpp>
#include <OmicronTK/Qt/Lua/LuaLineEdit.hpp>
#include <OmicronTK/Qt/Lua/LuaLayout.hpp>

#include <QMenu>
#include <QMenuBar>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QDebug>
#include <QMovie>
#include <QTime>

#include <QBoxLayout>

using namespace OmicronTK;

RadioUi::RadioUi(RadioWindow *musicWindow)
    : m_radioWindow(musicWindow)
{
    m_volumeControl = nullptr;

    m_luaState.requiref<OTKQT::LuaObject>();
    m_luaState.requiref<OTKQT::LuaWidget>();
    m_luaState.requiref<OTKQT::LuaLabel>();
    m_luaState.requiref<OTKQT::LuaPushButton>();
    m_luaState.requiref<OTKQT::LuaProgressBar>();
    m_luaState.requiref<OTKQT::LuaLineEdit>();
    m_luaState.requiref<OTKQT::LuaLayout>();
}

bool RadioUi::init(RadioStream *musicStream)
{
    m_centralWidget = new QWidget(m_radioWindow);
    m_menuBar = new QMenuBar(m_centralWidget);
    m_playlistView = musicStream->playlist;
    m_playButton = new QPushButton;
    m_stopButton = new QPushButton;
    m_prevButton = new QPushButton;
    m_nextButton = new QPushButton;
    m_recordButton = new QPushButton;
    m_openLinkButton = new QPushButton;
    m_radioPlaylistButton = new QPushButton;
    m_changeFavoriteButton = new QPushButton;
    m_allPlaylistsModeButton = new QPushButton;
    m_customPlaylistModeButton = new QPushButton;
    m_favoriteModeButton = new QPushButton;
    m_timeLabel = new QLabel;
    m_statusLabel = new QLabel;
    m_nameLabel = new QLabel;
    m_streamTitleLabel = new OTKQT::Label;
    m_loaderMovie = new QMovie;
    m_leftChProgressBar = new QProgressBar;
    m_rightChProgressBar = new QProgressBar;
    m_bufferProgressBar = new QProgressBar;
    m_searchLineEdit = new QLineEdit;
    m_volumeControl = new OTKQT::VolumeControl(m_radioWindow);

    m_luaState.push("m_mainWindow", m_radioWindow);
    m_luaState.push("m_titleBar", new OTKQT::TitleBarWidget(m_radioWindow));
    m_luaState.push("m_centralWidget", m_centralWidget);
    m_luaState.push("m_menuBar", m_menuBar);
    m_luaState.push("m_playlistView", m_playlistView);
    m_luaState.push("m_playButton", m_playButton);
    m_luaState.push("m_stopButton", m_stopButton);
    m_luaState.push("m_prevButton", m_prevButton);
    m_luaState.push("m_nextButton", m_nextButton);
    m_luaState.push("m_recordButton", m_recordButton);
    m_luaState.push("m_openLinkButton", m_openLinkButton);
    m_luaState.push("m_radioPlaylistButton", m_radioPlaylistButton);
    m_luaState.push("m_changeFavoriteButton", m_changeFavoriteButton);
    m_luaState.push("m_allPlaylistsModeButton", m_allPlaylistsModeButton);
    m_luaState.push("m_customPlaylistModeButton", m_customPlaylistModeButton);
    m_luaState.push("m_favoriteModeButton", m_favoriteModeButton);
    m_luaState.push("m_timeLabel", m_timeLabel);
    m_luaState.push("m_statusLabel", m_statusLabel);
    m_luaState.push("m_nameLabel", m_nameLabel);
    m_luaState.push("m_streamTitleLabel", m_streamTitleLabel);
    m_luaState.push("m_loaderMovie", m_loaderMovie);
    m_luaState.push("m_leftChProgressBar", m_leftChProgressBar);
    m_luaState.push("m_rightChProgressBar", m_rightChProgressBar);
    m_luaState.push("m_bufferProgressBar", m_bufferProgressBar);
    m_luaState.push("m_searchLineEdit", m_searchLineEdit);
    m_luaState.push("m_volumeControl", m_volumeControl);

    m_luaState.push("themePath", OTKQT::AppInfo::themePath().toStdString());

    m_luaState.addDirPath(OTKQT::AppInfo::themePath().toStdString());

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("/WindowBase.lua").toStdString()))
        return false;

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("RadioWindow.lua").toStdString()))
        return false;

    createMenuBar();

    m_nameLabel->setOpenExternalLinks(true);

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        m_streamTitleLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        m_streamTitleLabel->setText("Boa Tarde");
    else
        m_streamTitleLabel->setText("Boa Noite");

    m_radioWindow->setWidget(m_centralWidget);

    return true;
}

//================================================================================================================
// private
//================================================================================================================

void RadioUi::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
    m_openLinkAction = fileMenu->addAction("Abrir Link Rapidamente");
    m_displayRecordingsAction = fileMenu->addAction("Exibir pasta com Gravações");
    fileMenu->addSeparator();
    m_exitAction = fileMenu->addAction("Sair");
    m_exitAction->setShortcut(QString("Ctrl+Q"));

    QMenu *modeMenu = new QMenu("Modo");
    m_musicModeAction = modeMenu->addAction("Modo Músicas");
    QAction *currentAction = modeMenu->addAction("Modo Web Rádio");
    m_recorderModeAction = modeMenu->addAction("Modo Gravador");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
    m_playlistManagerAction = toolsMenu->addAction("Gerenciador de Rádios");
    m_equalizerAction = toolsMenu->addAction("Equalizador");
    toolsMenu->addSeparator();
    m_configAction = toolsMenu->addAction("Configurações");

    QMenu *aboutMenu = new QMenu("Ajuda");
    m_checkUpdateAction = aboutMenu->addAction("Verificar por Atualizações");
    aboutMenu->addSeparator();
    m_websiteAction = aboutMenu->addAction("Visitar o Website Oficial");
    m_facebookAction = aboutMenu->addAction("Curtir a Página no Facebook");
    aboutMenu->addSeparator();
    m_aboutAction = aboutMenu->addAction("Sobre");

    m_menuBar->addMenu(fileMenu);
    m_menuBar->addMenu(modeMenu);
    m_menuBar->addMenu(toolsMenu);
    m_menuBar->addMenu(aboutMenu);
}
