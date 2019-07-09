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

#include "MusicUi.h"
#include "../Core/Database.h"
#include "../Tools/Equalizer.h"
#include "../Core/MusicStream.h"

#include "MusicWindow.h"

#include <OmicronTK/Qt/AppInfo.hpp>
#include <OmicronTK/Qt/ErrorWindow.hpp>
#include <OmicronTK/Qt/Theme.hpp>
#include <OmicronTK/Qt/DirectoryDialog.hpp>
#include <OmicronTK/Qt/VolumeControl.hpp>
#include <OmicronTK/Qt/Slider.hpp>
#include <OmicronTK/Qt/ComboBox.hpp>
#include <OmicronTK/Qt/TitleBarWidget.hpp>

#include <OmicronTK/Qt/Lua/LuaObject.hpp>
#include <OmicronTK/Qt/Lua/LuaWidget.hpp>
#include <OmicronTK/Qt/Lua/LuaLabel.hpp>
#include <OmicronTK/Qt/Lua/LuaPushButton.hpp>
#include <OmicronTK/Qt/Lua/LuaSlider.hpp>
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

#include <QBoxLayout>

using namespace OmicronTK;

MusicUi::MusicUi(MusicWindow *musicWindow)
    : m_musicWindow(musicWindow)
{
    m_volumeControl = nullptr;

    m_luaState.requiref<OTKQT::LuaObject>();
    m_luaState.requiref<OTKQT::LuaWidget>();
    m_luaState.requiref<OTKQT::LuaLabel>();
    m_luaState.requiref<OTKQT::LuaPushButton>();
    m_luaState.requiref<OTKQT::LuaSlider>();
    m_luaState.requiref<OTKQT::LuaProgressBar>();
    m_luaState.requiref<OTKQT::LuaLineEdit>();
    m_luaState.requiref<OTKQT::LuaLayout>();
}

bool MusicUi::init(MusicStream *musicStream)
{
    m_centralWidget = new QWidget(m_musicWindow);
    m_menuBar = new QMenuBar(m_centralWidget);
    m_playlistView = musicStream->playlist;
    m_volumeControl = new OTKQT::VolumeControl(m_musicWindow);
    m_timeSlider = new OTKQT::Slider;
    m_searchLineEdit = new QLineEdit;
    m_leftChProgressBar = new QProgressBar;
    m_rightChProgressBar = new QProgressBar;
    m_currentTrackLabel = new QLabel;
    m_totalTracksLabel = new QLabel;
    m_currentTagLabel = new QLabel;
    m_totalTimeLabel = new QLabel;
    m_fileTypeLabel = new QLabel;
    m_timeLabel = new QLabel;
    m_playButton = new QPushButton;
    m_pauseButton = new QPushButton;
    m_stopButton = new QPushButton;
    m_prevButton = new QPushButton;
    m_nextButton = new QPushButton;
    m_changeFavoriteButton = new QPushButton;
    m_repeatButton = new QPushButton;
    m_randomButton = new QPushButton;
    m_playlistManagerButton = new QPushButton;
    m_musicTabButton = new QPushButton;
    m_playlistTabButton = new QPushButton;
    m_favoritesTabButton = new QPushButton("Favoritos");

    m_luaState.push("m_mainWindow", m_musicWindow);
    m_luaState.push("m_titleBar", new OTKQT::TitleBarWidget(m_musicWindow));
    m_luaState.push("m_centralWidget", m_centralWidget);
    m_luaState.push("m_menuBar", m_menuBar);
    m_luaState.push("m_playlistView", m_playlistView);
    m_luaState.push("m_volumeControl", m_volumeControl);
    m_luaState.push("m_timeSlider", m_timeSlider);
    m_luaState.push("m_searchLineEdit", m_searchLineEdit);
    m_luaState.push("m_leftChProgressBar", m_leftChProgressBar);
    m_luaState.push("m_rightChProgressBar", m_rightChProgressBar);
    m_luaState.push("m_currentTrackLabel", m_currentTrackLabel);
    m_luaState.push("m_totalTracksLabel", m_totalTracksLabel);
    m_luaState.push("m_currentTagLabel", m_currentTagLabel);
    m_luaState.push("m_totalTimeLabel", m_totalTimeLabel);
    m_luaState.push("m_fileTypeLabel", m_fileTypeLabel);
    m_luaState.push("m_timeLabel", m_timeLabel);
    m_luaState.push("m_playButton", m_playButton);
    m_luaState.push("m_pauseButton", m_pauseButton);
    m_luaState.push("m_stopButton", m_stopButton);
    m_luaState.push("m_prevButton", m_prevButton);
    m_luaState.push("m_nextButton", m_nextButton);
    m_luaState.push("m_changeFavoriteButton", m_changeFavoriteButton);
    m_luaState.push("m_repeatButton", m_repeatButton);
    m_luaState.push("m_randomButton", m_randomButton);
    m_luaState.push("m_playlistManagerButton", m_playlistManagerButton);
    m_luaState.push("m_musicTabButton", m_musicTabButton);
    m_luaState.push("m_playlistTabButton", m_playlistTabButton);
    m_luaState.push("m_favoritesTabButton", m_favoritesTabButton);

    m_luaState.push("themePath", OTKQT::AppInfo::themePath().toStdString());

    m_luaState.addDirPath(OTKQT::AppInfo::themePath().toStdString());

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("/WindowBase.lua").toStdString()))
        return false;

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("MusicWindow.lua").toStdString()))
        return false;

    createMenuBar();

    m_currentTrackLabel->setTextFormat(Qt::PlainText);
    m_totalTracksLabel->setTextFormat(Qt::PlainText);
    m_currentTagLabel->setTextFormat(Qt::PlainText);
    m_totalTimeLabel->setTextFormat(Qt::PlainText);
    m_fileTypeLabel->setTextFormat(Qt::PlainText);
    m_timeLabel->setTextFormat(Qt::PlainText);

    m_pauseButton->setShortcut(Qt::Key_MediaPause);
    m_stopButton->setShortcut(Qt::Key_MediaStop);
    m_prevButton->setShortcut(Qt::Key_MediaPrevious);
    m_nextButton->setShortcut(Qt::Key_MediaNext);

    m_musicWindow->setWidget(m_centralWidget);

    return true;
}

//================================================================================================================
// private
//================================================================================================================

void MusicUi::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
#ifndef Q_OS_ANDROID
    m_openCDAction = fileMenu->addAction("Abrir CD de Áudio");
    fileMenu->addSeparator();
#endif
    m_openMusicAction = fileMenu->addAction("Abrir Músicas");
    m_addMusicAction = fileMenu->addAction("Adicionar Músicas");
    fileMenu->addSeparator();
    m_openDirAction = fileMenu->addAction("Abrir Pasta com Músicas");
    m_addDirAction = fileMenu->addAction("Adicionar Pasta com Músicas");
    fileMenu->addSeparator();
    m_openPlaylistAction = fileMenu->addAction("Abrir Playlist");
    m_clearPlaylistAction = fileMenu->addAction("Limpar a Lista de Reprodução");
    fileMenu->addSeparator();
    m_exitAction = fileMenu->addAction("Sair");
    m_exitAction->setShortcut(QString("Ctrl+Q"));

    QMenu *modeMenu = new QMenu("Modo");
    QAction *currentAction = modeMenu->addAction("Modo Músicas");
    m_radioModeAction = modeMenu->addAction("Modo Web Rádio");
    m_recorderModeAction = modeMenu->addAction("Modo Gravador");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
    m_playlistAction = toolsMenu->addAction("Gerenciador de Playlist");
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
