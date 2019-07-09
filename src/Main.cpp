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

#include "Main.h"
#include "Tools/ConfigDialog.h"
#include "Core/Windows/FileAssociation.h"
#include "Gui/WindowBase.h"
#include "Gui/MusicWindow.h"
#include "Gui/RadioWindow.h"
#include "Gui/RecorderWindow.h"

#include <iostream>
#include <QProxyStyle>
#include <QMenu>

#include "Gui/AboutDialog.hpp"

#include <OmicronTK/Qt/AppInfo.hpp>
#include <OmicronTK/Qt/Theme.hpp>
#include <OmicronTK/Qt/Network/LocalServer.hpp>
#include <OmicronTK/Qt/Network/LocalSocket.hpp>

using namespace OmicronTK;

// Classe responsável por retirar o efeito de "foco" do QTreeView.
class ProxyStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
                                                         QPainter * painter, const QWidget * widget = 0) const
    {
        if (element == PE_FrameFocusRect)
            return;
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

int main(int argc, char **argv)
{
    int code = EXIT_SUCCESS;
    QApplication *qApplication = nullptr;
    OTKQT::LocalServer *localServer = nullptr;
    Main *app = nullptr;

    Global::initAppPath();

    QString execPath = Global::getAppPath();
    QString pluginPath = execPath + "/../";

#ifdef QT_NO_DEBUG // RELEASE
    #ifdef Q_OS_UNIX
        OTKQT::AppInfo::setSharePath(execPath + "/../share/" TARGET);

        if (QDir().exists(execPath + "/../" ARCHITECTURE_DEB "/" TARGET))
            pluginPath += ARCHITECTURE_DEB "/" TARGET "/plugins";
        else
            pluginPath += ARCHITECTURE "/" TARGET "/plugins";
    #else
        OTKQT::AppInfo::setSharePath(execPath);
        pluginPath = execPath + "/plugins";
    #endif

#else // DEBUG
    pluginPath = execPath + "/plugins";

    OTKQT::AppInfo::setSharePath(execPath + "/../../project");
    OTKQT::AppInfo::setPluginsPath(pluginPath);

#endif // QT_NO_DEBUG

    QStringList libraryPaths = QApplication::libraryPaths();

    libraryPaths <<  pluginPath;

    QApplication::setLibraryPaths(libraryPaths);
    OTKQT::AppInfo::setPluginsPath(pluginPath);

#ifdef Q_OS_WIN // Windows
    // Se desinstalar o programa (no Windows), iniciá-lo e passar o argumento --uninstall-app para
    // remover as informações do regitro do Windows.
    if (argc == 2 && strcmp(argv[1], "--uninstall-app") == 0)
    {
        Global::setupSupportedFiles();
        FileAssociation(ToQString(argv[0])).remove(Global::supportedFiles);

        QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

        // Informar "uninstalled" como valor da versão na base de dados.
        if (Database::init(configPath + "/" + AppConfigDir + "/Config.db"))
            Database::setValue("Version", "current", "uninstalled");

        Database::free();
        return EXIT_SUCCESS;
    }
#endif

    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<DWORD>("DWORD");
    qRegisterMetaType<QWORD>("QWORD");
    qRegisterMetaType<MusicStream::Event>("MusicStream::Event");
    qRegisterMetaType<RadioStream::Event>("RadioStream::Event");
    qRegisterMetaType<RecorderStream::Event>("RecorderStream::Event");

    // Loop que mantém o programa aberto, caso reinicie-o.
    do
    {
        delete localServer;
        delete app;
        delete qApplication;

        qApplication = new QApplication(argc, argv);

#ifdef QT_NO_DEBUG // RELEASE
        stdCout("\n"+AppName+" "+CurrentVersion+" (Ver. " FILE_VERSION ")\n");

        localServer = new OTKQT::LocalServer(1000, AppNameId + "-f52ha94hd601nsdg628r16");
#else // DEBUG
        localServer = new OTKQT::LocalServer(1000, AppNameId + "-debug-an50xg24ng64lnv7");
#endif // QT_NO_DEBUG

        if (localServer->isRunning())
        {
            OTKQT::LocalSocket localSocket(*localServer);
            QVector<QString> arguments;

            if (argc > 2 && (strcmp(argv[1], "--open-file") == 0 || strcmp(argv[1], "--add-file") == 0))
            {
                arguments.push_back(ToQString(argv[1]));
                arguments.push_back(ToQString(argv[2]));
            }
            else if (argc < 2)
            {
                arguments.push_back("--empty");
            }
            else
            {
                for (int i = 1; i < argc; i++)
                    arguments.push_back(ToQString(argv[i]));
            }

            if (localSocket.sendMessage(arguments))
                code = EXIT_SUCCESS;
            else
                code = EXIT_FAILURE;

            delete localServer;
            delete qApplication;

            return code;
        }

#ifndef QT_NO_TRANSLATION
        QString translatorFileName = QLatin1String("qt_");
        translatorFileName += QLocale::system().name();
        QTranslator *translator = new QTranslator(qApplication);

        if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            qApplication->installTranslator(translator);
#endif

        qApplication->setStyle(new ProxyStyle);
        qApplication->setQuitOnLastWindowClosed(false);
        app = new Main;
        QObject::connect(localServer, SIGNAL(messageAvailable(QVector<QString>)), app, SLOT(receiveMessage(QVector<QString>)));

        if (app->init(argc))
        {
#ifdef QT_NO_DEBUG // RELEASE
            stdCout("The " + AppName + " was initialized correctly.");
#endif // QT_NO_DEBUG
        }
        else
        {
            stdCout("Error! Could not initialize.");
            code = EXIT_FAILURE;
            break;
        }

        code = qApplication->exec();

    } while (app->continueRunning);

    delete localServer;
    delete app;
    delete qApplication;

    return code;
}

//================================================================================================================
// class Main
//================================================================================================================
//! Construtor da classe.
Main::Main()
{
    continueRunning = false;
    trayIcon = nullptr;
    updateApp = nullptr;
    window = nullptr;
    iniSettings = nullptr;
}

//! Destrutor da Classe.
Main::~Main()
{
    delete trayIcon;
    delete updateApp;
    delete window;
    delete iniSettings;

    OTKQT::Theme::free();
    Database::free();
    Stream::free();
}

//! Método para inicializar o programa.
/*!
  \param argc Quantidade de argumentos recebidos ao inicializar o programa.
  \return Retorna true se o programa foi inicializado corretamente, false se algo deu errado.
*/
bool Main::init(const int &argc)
{
    if (!Global::init(argc))
        return false;

    if (!QDir().exists(Global::getConfigPath()))
        QDir().mkpath(Global::getConfigPath());

    if (!QDir().exists(Global::getConfigPath("themes")))
        QDir().mkpath(Global::getConfigPath("themes"));

    OTKQT::AppInfo::setConfigPath(Global::getConfigPath());
    OTKQT::AppInfo::setAppName(AppName);

    if (Database::init(Global::getConfigPath("Config.db")))
    {
        if (Database::value("Version", "current").toString() != CurrentVersion)
        {
            Database::upgrade();
            Database::setValue("Version", "current", CurrentVersion);
            Database::setValue("Version", "updates_lastCheck", "0000-00-00");
#ifdef Q_OS_WIN
            // Adicionar (ou atualizar) as informações de tipo de arquivos no registro do Windows.
            // Caso as informações já existam, elas serão apenas atualizadas pela nova versão do programa.
            FileAssociation(QApplication::applicationFilePath()).registerList(Global::supportedFiles, false);
#endif
        }
    }
    else
    {
        QMessageBox::critical(nullptr, "Erro", "Ops! Algo deu errado...\n"
                              "Não foi possível criar ou configurar o Banco de Dados.");
        return false;
    }

    iniSettings = new QSettings(Global::getConfigPath("Settings.ini"), QSettings::IniFormat);
    iniSettings->setIniCodec("UTF-8");

    trayIcon = new QSystemTrayIcon(QIcon(Global::getQrcPath("tray-icons/icon-1.png")), 0);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                               this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    setupRadiolist();

    OTKQT::Theme::s_setThemeValue = [](const QString &name) { return Database::setValue("Config", "theme", name); };
    OTKQT::Theme::s_setStyleValue = [](const QString &name) { return Database::setValue("Config", "style", name); };
    OTKQT::Theme::s_themeValue = [] { return Database::value("Config", "theme").toString(); };
    OTKQT::Theme::s_styleValue = [] { return Database::value("Config", "style").toString(); };

    if (!OTKQT::Theme::init("app:omicron-media-player"))
        return false;

    if (!OTKQT::Theme::load())
        return false;

    updateApp = new UpdateApp(this, iniSettings);

    Equalizer::loadValues(Global::equalizerValues);

    if (Database::value("RadioConfig", "recordPath").toString().isEmpty())
        Database::setValue("RadioConfig", "recordPath",
                           QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/"+AppName+" Recordings/");

    if (!Stream::init())
        return false;

    QString mode = Database::value("Current", "mode").toString();

    try
    {
        if (mode == "Radio" && argc < 2)
            startRadioMode();

        else if (mode == "Recorder" && argc < 2)
            startRecorderMode();

        else
            startMusicMode();
    }
    catch (const char *e)
    {
        QMessageBox::critical(nullptr, "Erro", e);
        return false;
    }

    return true;
}

//================================================================================================================
// public slots
//================================================================================================================

//! Inicia o Modo Música.
void Main::startMusicMode()
{
    delete window;
    window = new MusicWindow(this);
    window->setWindowTitle("Músicas » "+AppName);
    window->show();
    trayIcon->setToolTip(AppName+" » Modo Músicas");
    updateTrayIconMenu();
}

//! Inicia o Modo Rádio.
void Main::startRadioMode()
{
    delete window;
    window = new RadioWindow(this);
    window->setWindowTitle("Rádio » "+AppName);
    window->show();
    trayIcon->setToolTip(AppName+" » Modo Web Rádio");
    updateTrayIconMenu();
}

//! Inicia o Modo Gravador.
void Main::startRecorderMode()
{
    delete window;
    window = new RecorderWindow(this);
    window->setWindowTitle("Gravador » "+AppName);
    window->show();
    trayIcon->setToolTip(AppName+" » Modo Gravador");
    updateTrayIconMenu();
}

//! Define o título da janela do programa.
/*!
  \param title String a ser passa para o novo título.
*/
void Main::setWindowTitle(QString title)
{
    window->setWindowTitle(title);
}

//! Exibe uma notificação de erro do programa.
/*!
  \param msg String contendo uma mensagem de erro.
*/
void Main::showError(QString msg)
{
    trayIcon->showMessage(AppName, msg, QSystemTrayIcon::Warning);
}

//! Exibe uma notificação do programa.
/*!
  \param msg String contendo uma mensagem de notificação.
*/
void Main::showNotification(QString msg)
{
    trayIcon->showMessage(AppName, msg);
}

//! Reinicia o programa.
void Main::restart()
{
    continueRunning = true;
    qApp->quit();
}

//================================================================================================================
// private
//================================================================================================================

//! Configura a lista de rádios.
void Main::setupRadiolist()
{
    if (iniSettings->value("Radiolist/FileName").toString().isEmpty()
            || iniSettings->value("Radiolist/Date").toString().isEmpty())
    {
        QString args("\"");
        args.append(OTKQT::AppInfo::pluginsPath())
                .append("/7-Zip/7zr\" x -y \"")
                .append(OTKQT::AppInfo::sharePath())
                .append("/RadioList.7z")
                .append("\" -o\"")
                .append(Global::getConfigPath())
                .append("\"");

        if (QProcess::execute(args) == 0)
        {
            iniSettings->setValue("Radiolist/FileName", FileRadiolist);
            iniSettings->setValue("Radiolist/Date", FileRadiolistDate);
        }
        else
        {
            qDebug() << "Erro ao Extrair o RadioList padrão";
        }
    }
    else if (!iniSettings->value("Radiolist/NewFileName").toString().isEmpty()
                       && QFile::exists(Global::getConfigPath(iniSettings->value("Radiolist/NewFileName").toString())))
    {
        if (QFile::remove(Global::getConfigPath(iniSettings->value("Radiolist/FileName").toString())))
        {
            iniSettings->setValue("Radiolist/FileName", iniSettings->value("Radiolist/NewFileName"));
            iniSettings->setValue("Radiolist/Date", iniSettings->value("Radiolist/NewDate"));
            iniSettings->remove("Radiolist/NewFileName");
            iniSettings->remove("Radiolist/NewDate");
        }
    }

    Global::setRadiolistName(iniSettings->value("Radiolist/FileName").toString());
}

//! Atualiza o menu (e seus ícones) da bandeja do sistema.
void Main::updateTrayIconMenu()
{
    QStringList titles;
    QMenu *trayIconMenu = new QMenu;

    if (window->currentMode() == "Music")
        titles << "Reproduzir Faixa" << "Pausar faixa atual" << "Parar faixa atual" << "Faixa anterior" << "Próxima faixa";
    else// if (window->currentMode() == "Radio")
        titles << "Reproduzir Rádio" << "" << "Parar Rádio atual" << "Rádio anterior" << "Próxima rádio";

    connect(trayIconMenu->addAction("Ocultar/Exibir Interface"), &QAction::triggered, [=]() {
        if (window->isHidden())
        {
            window->showNormal();
            window->activateWindow();
        }
        else
        {
            window->hide();
        }
    });

    trayIconMenu->addSeparator();

    // Play
    connect(trayIconMenu->addAction(QIcon(OTKQT::AppInfo::themePath()+"images/media_playback_start.png"),titles[0]),
            SIGNAL(triggered()), this, SIGNAL(playStream()));

    if (window->currentMode() == "Music")
    {
        // Pause
        connect(trayIconMenu->addAction(QIcon(OTKQT::AppInfo::themePath()+"images/media_playback_pause.png"),titles[1]),
                SIGNAL(triggered()), this, SIGNAL(pauseStream()));
    }

    // Stop
    connect(trayIconMenu->addAction(QIcon(OTKQT::AppInfo::themePath()+"images/media_playback_stop.png"),titles[2]),
            SIGNAL(triggered()), this, SIGNAL(stopStream()));

    // Faixa anterior
    connect(trayIconMenu->addAction(QIcon(OTKQT::AppInfo::themePath()+"images/media_skip_backward.png"),titles[3]),
            SIGNAL(triggered()), this, SIGNAL(prevStream()));

    // Próxima faixa
    connect(trayIconMenu->addAction(QIcon(OTKQT::AppInfo::themePath()+"images/media_skip_forward.png"),titles[4]),
            SIGNAL(triggered()), this, SIGNAL(nextStream()));

    trayIconMenu->addSeparator();

    // Fechar o programa
    connect(trayIconMenu->addAction("Fechar"), SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIcon->setContextMenu(trayIconMenu);
}

//================================================================================================================
// private slots
//================================================================================================================

void Main::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger: // Um clique (botão esquerdo)
        if (!window->isHidden() && !window->isMinimized())
        {
            window->hide();
        }
        else
        {
            window->showNormal();
            window->activateWindow();
        }
        break;
    case QSystemTrayIcon::DoubleClick: // Dois cliques (botão esquerdo)
        break;
    case QSystemTrayIcon::MiddleClick: // Botão do meio
        window->activateWindow();
        break;
    case QSystemTrayIcon::Context: // Botão Direito
        break;
    case QSystemTrayIcon::Unknown:
        break;
    }
}

//! Verificar por atualizações.
void Main::checkUpdate()
{
    updateApp->startChecking(true);
}

//! Recebe mensagens de outras instâncias.
/*!
  \param arg Lista de argumentos.
*/
void Main::receiveMessage(QVector<QString> arg)
{
    if (!arg.isEmpty() && arg[0] == "--empty")
    {
        window->hide();
        window->showNormal();
        window->activateWindow();
        return;
    }

    if (!arg.isEmpty() && window->currentMode() == "Music")
    {
        if (arg[0] == "--nohide")
        {
            arg.removeAt(0);
            window->activateWindow();
        }
        else if (arg[0] == "--add-file")
        {
            arg.removeAt(0);
            emit addMusic(arg);
            window->activateWindow();
            return;
        }
        else if (!window->isHidden())
        {
#ifdef Q_OS_LINUX
            window->hide();
            window->showNormal();
#endif //Q_OS_LINUX
            window->activateWindow();
        }

        if (arg[0] == "--open-file")
            arg.removeAt(0);

        emit openMusic(arg);
    }
    else if (window->currentMode() != "Music")
    {
        window->activateWindow();
        QMessageBox::warning(window, "Aviso", "O "+AppName+" está aberto no <strong>Modo Web Rádio</strong>."
                             "<br><br>Primeiro, feche o programa ou altere para o <strong>\"Modo Músicas\"</strong>.");
    }
}

//! Define as configurações padrão do programa, após isso, reinicie-o.
void Main::defaultConfig()
{
    if (QMessageBox::warning(nullptr, "Definir a Configuração Padrão?", "Você está prestes a redefinir todas as "
                             "configurações do programa.\nTodas as configurações serão perdidas, "
                             "incluindo o equalizador e favoritos.\n\nDeseja continuar?",
                             QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        Database::removeDatabase();
        restart();
    }
}
