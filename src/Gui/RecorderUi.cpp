#include "RecorderUi.hpp"

#include "../Core/Database.h"
#include "../Tools/Equalizer.h"
#include "../Core/MusicStream.h"

#include "RecorderWindow.h"

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

#include <OmicronTK/Qt/Lua/LuaSlider.hpp>
#include <OmicronTK/Qt/Lua/LuaComboBox.hpp>

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

RecorderUi::RecorderUi(RecorderWindow *musicWindow)
    : m_recorderWindow(musicWindow)
{
    m_luaState.requiref<OTKQT::LuaObject>();
    m_luaState.requiref<OTKQT::LuaWidget>();
    m_luaState.requiref<OTKQT::LuaLabel>();
    m_luaState.requiref<OTKQT::LuaPushButton>();
    m_luaState.requiref<OTKQT::LuaProgressBar>();
    m_luaState.requiref<OTKQT::LuaLineEdit>();
    m_luaState.requiref<OTKQT::LuaLayout>();
    m_luaState.requiref<OTKQT::LuaSlider>();
    m_luaState.requiref<OTKQT::LuaComboBox>();
}

bool RecorderUi::init(RecorderStream *musicStream)
{
    m_centralWidget = new QWidget(m_recorderWindow);
    m_menuBar = new QMenuBar(m_centralWidget);
    m_playlistView = musicStream->recordList;
    m_playButton = new QPushButton;
    m_stopButton = new QPushButton;
    m_recordButton = new QPushButton;
    m_timeLabel = new OTKQT::Label;
    m_statusLabel = new OTKQT::Label;
    m_leftChProgressBar = new QProgressBar;
    m_rightChProgressBar = new QProgressBar;
    m_timeSlider = new OTKQT::Slider;
    m_deviceComboBox = new OTKQT::ComboBox;
    m_encoderComboBox = new OTKQT::ComboBox;
    m_bitrateComboBox = new OTKQT::ComboBox;
    m_deleteButton = new QPushButton;

    m_luaState.push("m_mainWindow", m_recorderWindow);
    m_luaState.push("m_titleBar", new OTKQT::TitleBarWidget(m_recorderWindow));
    m_luaState.push("m_centralWidget", m_centralWidget);
    m_luaState.push("m_menuBar", m_menuBar);
    m_luaState.push("m_playlistView", m_playlistView);
    m_luaState.push("m_playButton", m_playButton);
    m_luaState.push("m_stopButton", m_stopButton);
    m_luaState.push("m_recordButton", m_recordButton);
    m_luaState.push("m_timeLabel", m_timeLabel);
    m_luaState.push("m_statusLabel", m_statusLabel);
    m_luaState.push("m_leftChProgressBar", m_leftChProgressBar);
    m_luaState.push("m_rightChProgressBar", m_rightChProgressBar);
    m_luaState.push("m_timeSlider", m_timeSlider);
    m_luaState.push("m_deviceComboBox", m_deviceComboBox);
    m_luaState.push("m_encoderComboBox", m_encoderComboBox);
    m_luaState.push("m_bitrateComboBox", m_bitrateComboBox);
    m_luaState.push("m_deleteButton", m_deleteButton);

    m_luaState.push("themePath", OTKQT::AppInfo::themePath().toStdString());

    m_luaState.addDirPath(OTKQT::AppInfo::themePath().toStdString());

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("/WindowBase.lua").toStdString()))
        return false;

    if (!m_luaState.loadFile(QString(OTKQT::AppInfo::themePath()).append("RecorderWindow.lua").toStdString()))
        return false;

    createMenuBar();

    m_recorderWindow->setWidget(m_centralWidget);

    return true;
}

//================================================================================================================
// private
//================================================================================================================

void RecorderUi::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
    m_displayRecordingsAction = fileMenu->addAction("Exibir pasta com Gravações");
    fileMenu->addSeparator();
    m_exitAction = fileMenu->addAction("Sair");
    m_exitAction->setShortcut(QString("Ctrl+Q"));

    QMenu *modeMenu = new QMenu("Modo");
    m_musicModeAction = modeMenu->addAction("Modo Músicas");
    m_radioModeAction = modeMenu->addAction("Modo Web Rádio");
    QAction *currentAction = modeMenu->addAction("Modo Gravador");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
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
