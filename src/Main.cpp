/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

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

#include <OmicronTK11/Qt/AppInfo.hpp>
#include <OmicronTK11/Qt/Theme.hpp>

using namespace OmicronTK11;

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

// Função main() (meio óbvio, né?)
int main(int argc, char **argv)
{
    int code = EXIT_SUCCESS;
    SingleApplication *app = nullptr;
    Main *appMain = nullptr;
    QStringList libraryPaths;

    // Se estiver em desenvolvimento, acrescentar a pasta de plugins do sistema.
    // Nota: Definir Global::inDevelopment como false, antes de compilar para o lançamento.
    if (Global::inDevelopment)
    {
        QStringList list = QApplication::libraryPaths();
        if (!list.isEmpty())
            libraryPaths << list[0]; // Para o desenvolvimento
    }
    libraryPaths << QFileInfo(Global::cStrToQString(argv[0])).absolutePath() + "/plugins";
    QApplication::setLibraryPaths(libraryPaths);

#ifdef Q_OS_WIN // Windows
    // Se desinstalar o programa (no Windows), iniciá-lo e passar o argumento --uninstall-app para
    // remover as informações do regitro do Windows.
    if (argc == 2 && strcmp(argv[1], "--uninstall-app") == 0)
    {
        Global::setupSupportedFiles();
        FileAssociation(Global::cStrToQString(argv[0])).remove(Global::supportedFiles);

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
        // Deletar os objetos das classes SingleApplication e Main, caso reinicie o programa.
        delete appMain;
        delete app;

#ifdef QT_NO_DEBUG // RELEASE
        stdCout("\n"+AppName+" "+CurrentVersion+" (Ver. " FILE_VERSION ")\n");

        app = new SingleApplication(argc, argv, AppNameId+"-d2sj406hav164hdbc8s9264g");
#else // DEBUG
        app = new SingleApplication(argc, argv, AppNameId+"-debug-676fgm5m5l5a4x31bc");
#endif // QT_NO_DEBUG

        // Se executar o programa, e ele já estiver executando (outro processo),
        // verificar se existem os argumentos --open-file ou --add-file, caso existam,
        // enviar uma mensagem para o processo principal, informando se deve abrir
        // um novo arquivo ou apenas adicionar o arquivo no playlist.
        if (app->isRunning() && argc > 2 && (strcmp(argv[1], "--open-file") == 0 || strcmp(argv[1], "--add-file") == 0))
        {
            int code;
            char *msg = (char *) malloc(strlen(argv[1]) + strlen(argv[2]) + 10);

            sprintf(msg, "%s:%s", argv[1], argv[2]);

            // Envia a mensagem pro processo principal, contendo as informações necessárias.
            if (app->sendMessage(msg))
                code = EXIT_SUCCESS;
            else
                code = EXIT_FAILURE;

            delete app;
            delete msg;
            return code;
        }
        // Verificar se o programa já está rodando (processo principal), caso esteja, enviar mensagens contendo
        // os parâmetros recebidos pelo processo atual, para o processo principal.
        else if (app->isRunning())
        {
            if (argc < 2)
            {
                // Caso não tenha passado argumentos extras pro programa, será enviada
                // uma mensagem  pro processo principal, contendo a string --empty.
                app->sendMessage("--empty");
            }
            else
            {
                // Caso o programa tenha recebido argumentos, enviar cada um deles separadamente ao processo principal.
                for (int i = 1; i < argc; i++)
                    app->sendMessage(argv[i]);
            }

            // A mensagem contendo "--end-arguments", é necessária pro processo principal "saber" que
            // todas as mensagens foram enviadas.
            if (app->sendMessage("--end-arguments"))
            {
                delete app;
                return EXIT_SUCCESS;
            }

            // Caso o processo principal tenha quebrado (por algum motivo qualquer), a conexão com
            // o servidor local continuará existindo e app->isRunning() retornará true.
            // Nesse caso, por não existir um processo principal (apenas a conexão dele),
            // será criado um novo servidor local e o processo atual se tornará o principal,
            // permitindo que outros processos enviem mensagens para o processo atual.
            if (!app->createServer())
            {
                delete app;
                return EXIT_FAILURE;
            }
        }

#ifndef QT_NO_TRANSLATION
        QString translatorFileName = QLatin1String("qt_");
        translatorFileName += QLocale::system().name();
        QTranslator *translator = new QTranslator(app);

        if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            app->installTranslator(translator);
#endif

        app->setStyle(new ProxyStyle);
        app->setQuitOnLastWindowClosed(false);
        appMain = new Main;
        QObject::connect(app, SIGNAL(messageAvailable(QStringList)), appMain, SLOT(receiveMessage(QStringList)));

        // Verifica se o programa foi iniciado corretamente.
        if (appMain->init(argc))
        {
#ifdef QT_NO_DEBUG // RELEASE
            stdCout("The " + AppName + " was initialized correctly.");
#endif // QT_NO_DEBUG
        }
        else
        {
            // Caso o programa não tenha sido iniciado corretamente, o loop será quebrado e o processo encerrado.

            stdCout("Error! Could not initialize.");
            code = EXIT_FAILURE;
            break;
        }

        code = app->exec();

    } while (appMain->continueRunning);

    delete appMain;
    delete app;

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
    QFontDatabase::removeAllApplicationFonts();
}

//! Método para inicializar o programa.
/*!
  \param argc Quantidade de argumentos recebidos ao inicializar o programa.
  \return Retorna true se o programa foi inicializado corretamente, false se algo deu errado.
*/
bool Main::init(const int &argc)
{
    // Inicializar a classe Global.
    if (!Global::init(argc))
        return false;

    // Criar o diretório pras configurações, caso não exista.
    if (!QDir().exists(Global::getConfigPath()))
        QDir().mkpath(Global::getConfigPath());

    // Criar o diretório pros temas instalados pelo usuário, caso não exista.
    if (!QDir().exists(Global::getConfigPath("themes")))
        QDir().mkpath(Global::getConfigPath("themes"));

    OTKQT::AppInfo::setSharePath("/mnt/projects/Desktop/C++/omicron-player-classic/project");
    OTKQT::AppInfo::setConfigPath(Global::getConfigPath().toStdString().c_str());

    // Carregar as fontes pro programa.
    QFontDatabase::addApplicationFont(Global::getQrcPath("fonts/verdana.ttf"));
    QFontDatabase::addApplicationFont(Global::getQrcPath("fonts/verdanab.ttf"));

    // Inicializar a conexão com o banco de dados.
    if (Database::init(Global::getConfigPath("Config.db")))
    {
        // Verificar se a versão do programa continua a mesma.
        // Caso tenha mudado, dar um upgrade nas configurações (tabelas do banco de dados,
        // registro do Windows e etc.).
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
        // Mostrar uma mensagem e encerrar o programa,
        // caso não seja possível abrir uma conexão com o banco de dados.
        QMessageBox::critical(nullptr, "Erro", "Ops! Algo deu errado...\n"
                              "Não foi possível criar ou configurar o Banco de Dados.");
        return false;
    }

    // Criar um arquivo extra de configurações.
    iniSettings = new QSettings(Global::getConfigPath("Settings.ini"), QSettings::IniFormat);
    iniSettings->setIniCodec("UTF-8");

    // Criar o ícone na bandeja do sistema.
    trayIcon = new QSystemTrayIcon(QIcon(Global::getQrcPath("tray-icons/icon-1.png")), 0);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                               this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    // Configurar a lista de rádios.
    setupRadiolist();

    // Inicializar a classe responsável pelo tema visual do programa.
    OTKQT::Theme::s_setThemeValue = [](const QString &name) { return Database::setValue("Config", "theme", name); };
    OTKQT::Theme::s_setStyleValue = [](const QString &name) { return Database::setValue("Config", "style", name); };
    OTKQT::Theme::s_themeValue = [] { return Database::value("Config", "theme").toString(); };
    OTKQT::Theme::s_styleValue = [] { return Database::value("Config", "style").toString(); };

    if (!OTKQT::Theme::init("app:omicron-media-player"))
        return false;

    // Carregar o tema visual do programa. Caso houver algum erro no processo, retornará false.
    if (!OTKQT::Theme::load())
        return false;

    // Instanciar a classe para conectar à internet e verificar se existem novas versões do programa.
    updateApp = new UpdateApp(this, iniSettings);

    // Carrega o equalizador do banco de dados para Global::equalizerValues.
    Equalizer::loadValues(Global::equalizerValues);

    // Configura o diretório de gravações no banco de dados, caso não esteja configurado.
    if (Database::value("RadioConfig", "recordPath").toString().isEmpty())
        Database::setValue("RadioConfig", "recordPath",
                           QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/"+AppName+" Recordings/");

    // Inicializa a biblioteca BASS e seus plugins. Caso haja algum erro, retorna false.
    if (!Stream::init())
        return false;

    QString mode = Database::value("Current", "mode").toString();

    try
    {
        // Verifica qual o modo que deve ser inicializado (Modo Radio, Modo Recorder, Modo Server ou Modo Music).
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
    if (iniSettings->value("Radiolist/FileName").toString().isEmpty())
        iniSettings->setValue("Radiolist/FileName", FileRadiolist);

    if (iniSettings->value("Radiolist/Date").toString().isEmpty())
        iniSettings->setValue("Radiolist/Date", FileRadiolistDate);

    if (!iniSettings->value("Radiolist/NewFileName").toString().isEmpty()
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

    if (iniSettings->value("Radiolist/FileName").toString() == FileRadiolist
                                 && !QFile::exists(Global::getConfigPath(FileRadiolist)))
    {
        QFile::copy(Global::getQrcPath("RadioList."+RadioPlaylistExt), Global::getConfigPath(FileRadiolist));
        QFile::setPermissions(Global::getConfigPath(FileRadiolist),
                              QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|QFile::ReadGroup
                              |QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);
    }
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

    // Oculta ou exibe a interface gráfica.
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
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_start.png")),titles[0]),
            SIGNAL(triggered()), this, SIGNAL(playStream()));

    if (window->currentMode() == "Music")
    {
        // Pause
        connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_pause.png")),titles[1]),
                SIGNAL(triggered()), this, SIGNAL(pauseStream()));
    }

    // Stop
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_stop.png")),titles[2]),
            SIGNAL(triggered()), this, SIGNAL(stopStream()));

    // Faixa anterior
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_skip_backward.png")),titles[3]),
            SIGNAL(triggered()), this, SIGNAL(prevStream()));

    // Próxima faixa
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_skip_forward.png")),titles[4]),
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
    // Se o parâmentro --empty foi passado, apenas ativar a janela do programa.
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
