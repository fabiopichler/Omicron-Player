/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Main.h"
#include "Tools/ConfigDialog.h"
#include "Core/Windows/FileAssociation.h"
#include <iostream>
#include <QProxyStyle>

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
    SingleApplication *app = nullptr;
    Main *appMain = nullptr;
    QStringList libraryPaths;

    if (Global::inDevelopment)
        libraryPaths << QApplication::libraryPaths()[0]; // Para o desenvolvimento
    libraryPaths << QFileInfo(Global::cStrToQString(argv[0])).absolutePath() + "/plugins";
    QApplication::setLibraryPaths(libraryPaths);

#ifdef Q_OS_WIN
    if (argc == 2 && strcmp(argv[1], "--uninstall-app") == 0)
    {
        Global::setupSupportedFiles();
        FileAssociation(Global::cStrToQString(argv[0])).remove(Global::supportedFiles);

        QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

        if (Database::init(configPath + "/" + AppConfigDir + "/Config.db"))
            Database::setValue("Version", "current", "uninstalled");

        Database::free();
        return EXIT_SUCCESS;
    }
#endif

    qRegisterMetaType<DWORD>("DWORD");
    qRegisterMetaType<QWORD>("QWORD");
    qRegisterMetaType<MusicStream::Event>("MusicStream::Event");
    qRegisterMetaType<RadioStream::Event>("RadioStream::Event");

    do
    {
        delete appMain;
        delete app;

#ifdef QT_NO_DEBUG // RELEASE
        stdCout("\n"+AppName+" "+CurrentVersion+" (Ver. " FILE_VERSION ")\n");

        app = new SingleApplication(argc, argv, AppNameId+"-d2sj406hav164hdbc8s9264g");
#else // DEBUG
        app = new SingleApplication(argc, argv, AppNameId+"-debug-676fgm5m5l5a4x31bc");
#endif // QT_NO_DEBUG

        if (app->isRunning() && argc > 2 && (strcmp(argv[1], "--open-file") == 0 || strcmp(argv[1], "--add-file") == 0))
        {
            int code;
            char *msg = (char *) malloc(strlen(argv[1]) + strlen(argv[2]) + 2);

            sprintf(msg, "%s:%s", argv[1], argv[2]);

            if (app->sendMessage(msg))
                code = EXIT_SUCCESS;
            else
                code = EXIT_FAILURE;

            delete app;
            delete msg;
            return code;
        }
        else if (app->isRunning())
        {
            if (argc < 2)
            {
                app->sendMessage("--empty");
            }
            else
            {
                for (int i = 1; i < argc; i++)
                    app->sendMessage(argv[i]);
            }

            if (app->sendMessage("--end-arguments"))
            {
                delete app;
                return EXIT_SUCCESS;
            }

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

        if (appMain->init(argc))
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

        code = app->exec();

    } while (appMain->continueRunning);

    delete appMain;
    delete app;

    return code;
}

//================================================================================================================
// class Main
//================================================================================================================
Main::Main()
{
    continueRunning = false;
    trayIcon = nullptr;
    updateApp = nullptr;
    window = nullptr;
}

Main::~Main()
{
    delete trayIcon;
    delete updateApp;
    delete window;
    delete iniSettings;

    Theme::free();
    Database::free();
    QFontDatabase::removeAllApplicationFonts();
    BASS_Stop();
    BASS_PluginFree(0);
    BASS_Free();
}

bool Main::init(const int &argc)
{
    if (!Global::init(argc))
        return false;

    if (!QDir().exists(Global::getConfigPath()))
        QDir().mkdir(Global::getConfigPath());

    if (!QDir().exists(Global::getConfigPath("themes")))
        QDir().mkdir(Global::getConfigPath("themes"));

    QFontDatabase::addApplicationFont(Global::getQrcPath("fonts/verdana.ttf"));
    QFontDatabase::addApplicationFont(Global::getQrcPath("fonts/verdanab.ttf"));

    if (Database::init(Global::getConfigPath("Config.db")))
    {
        if (Database::value("Version", "current").toString() != CurrentVersion)
        {
            Database::upgrade();
            Database::setValue("Version", "current", CurrentVersion);
            Database::setValue("Version", "updates_lastCheck", "0000-00-00");
#ifdef Q_OS_WIN
            FileAssociation(QApplication::applicationFilePath()).registerList(Global::supportedFiles, false);
#endif
        }
    }
    else
    {
        return false;
    }

    iniSettings = new QSettings(Global::getConfigPath("Settings.ini"), QSettings::IniFormat);
    iniSettings->setIniCodec("UTF-8");

    trayIcon = new QSystemTrayIcon(QIcon(Global::getQrcPath("tray-icons/icon-1.png")), 0);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                               this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    setupRadiolist();

    Theme::init();
    if (!Theme::load())
        return false;

    updateApp = new UpdateApp(this, iniSettings);

    for (int i = 0; i < 16; i++)
        Global::equalizerValues << Database::value("CurrentEqualizer", QString::number(i)).toInt();

    if (Database::value("Config", "recordPath").toString().isEmpty())
        Database::setValue("Config", "recordPath",
                           QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/"+AppName+" Recordings/");

    if (!setupBass())
        return false;

    if (Database::value("Current", "mode").toString() != "Radio" || argc > 1)
        startMusicMode();
    else
        startRadioMode();

    return true;
}

//================================================================================================================
// public slots
//================================================================================================================

void Main::startMusicMode()
{
    delete window;
    window = new MainWindow(this, iniSettings);
    window->initWindow(new MusicWindow(this, window));
    window->setWindowTitle("Músicas » "+AppName);
    trayIcon->setToolTip(AppName+" » Modo Músicas");
    updateTrayIconMenu();
}

void Main::startRadioMode()
{
    delete window;
    window = new MainWindow(this, iniSettings);
    window->initWindow(new RadioWindow(this, window));
    window->setWindowTitle("Rádio » "+AppName);
    trayIcon->setToolTip(AppName+" » Modo Web Rádio");
    updateTrayIconMenu();
}

void Main::setWindowTitle(QString arg)
{
    window->setWindowTitle(arg);
}

void Main::showError(QString msg)
{
    trayIcon->showMessage(AppName, msg, QSystemTrayIcon::Warning);
}

void Main::showNotification(QString msg)
{
    trayIcon->showMessage(AppName, msg);
}

void Main::openSite()
{
    QDesktopServices::openUrl(QUrl(OfficialSite));
}

void Main::about()
{
    DialogBase ab(window);
    QPushButton btOk("Ok");
    QVBoxLayout layout;

    QLabel lb("<h1 align=\"center\">"+AppName+"</h1>"
              "<h3 align=\"center\">Versão: "+CurrentVersion+" ("+CurrentReleaseDate+")</h3>"
              "<div align=\"center\"><strong>Copyright © 2015-2016, Fábio Pichler, Todos os direitos reservados.</strong><br><br>"
              "Visite: <a href=\"http://fabiopichler.net\">http://fabiopichler.net</a><br></div>"
              "<div>Este é um programa livre, gratuito e de código aberto,<br>"
              "está licenciado sobre a <a href=\"http://fabiopichler.net/bsd-3-license/\">BSD 3-Clause License</a><br><br>"
              "O "+AppName+" é um software para reprodução de arquivos de<br>"
              "áudio (MP3, WMA, WAV, OGG, etc.), Web Rádios e CD de música.</div>"
              "<h3>Bibliotecas/Recursos usados</h3>"
              "<div><strong>Qt:</strong> <a href=\"http://www.qt.io\">http://www.qt.io</a><br>"
              "<strong>BASS:</strong> <a href=\"http://www.un4seen.com\">http://www.un4seen.com</a><br>"
              "<strong>Ícones:</strong> <a href=\"http://flaticons.net\">http://flaticons.net</a> | "
              "<a href=\"http://findicons.com/search/media\">http://findicons.com/search/media</a></div>");

    connect(&btOk, SIGNAL(clicked()), &ab, SLOT(close()));

    lb.setOpenExternalLinks(true);

    layout.addWidget(&lb);
    layout.addWidget(&btOk, 0, Qt::AlignCenter);
    layout.setContentsMargins(40,10,40,20);
    layout.setSpacing(20);

    ab.setWindowTitle("Sobre o "+AppName);
    ab.setLayout(&layout);
    ab.exec();
}

void Main::initConfigDialog()
{
    ConfigDialog *configDialog = new ConfigDialog(this, window);
    configDialog->exec();
    delete configDialog;
}

//================================================================================================================
// private
//================================================================================================================

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

bool Main::setupBass()
{
    if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
        QMessageBox::warning(nullptr,"Incorrect BASS.DLL",
                             "An incorrect version of BASS.DLL was loaded (2.4 is required)");
        return false;
    }

    if (HIWORD(BASS_FX_GetVersion())!=BASSVERSION) {
        QMessageBox::warning(nullptr,"Incorrect BASS_FX.DLL",
                             "An incorrect version of BASS_FX.DLL was loaded (2.4 is required)");
        return false;
    }

    int device = Database::value("Config", "device").toInt();
    BASS_DEVICEINFO deviceInfo;
    bool deviceOk = false;

    for (int i = 1; BASS_GetDeviceInfo(i,&deviceInfo); i++)
    {
        if (deviceInfo.flags&BASS_DEVICE_ENABLED)
        {
            if (device == i)
            {
                deviceOk = true;
                break;
            }
        }
    }

    if (!BASS_Init((deviceOk ? device : -1), 44100, 0, 0, nullptr))
    {
        Database::setValue("Config", "device", -1);
        if (!BASS_Init(-1, 44100, 0, 0, nullptr))
        {
            QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Não foi possível iniciar o programa!<br>"
                                             "Verifique se seu dispositivo de áudio está funcionando corretamente"
                                                                        " e tente novamente."));
            return false;
        }
    }

#ifndef Q_OS_ANDROID // Desativar os plugins no Android, apenas para os testes (ainda não funciona nele).
    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+bass_aac).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_aac));

    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+bass_ac3).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_ac3));

    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+bass_ape).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_ape));

    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+bassflac).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bassflac));

    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+basswv).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+basswv));

    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+bassopus).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bassopus));
#endif
#ifdef Q_OS_WIN
    if (!BASS_PluginLoad(Global::getAppPath(PathAudioPlugins+basswma).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+basswma));
#endif // Q_OS_WIN

    QString proxy = Database::value("Config", "net_proxy", "0").toString();

    BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
    BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
    BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, Database::value("Config", "net_readtimeout", 20000).toInt());
    BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT, Database::value("Config", "net_timeout", 20000).toInt());
    BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);
    BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (proxy == "0" ? NULL : proxy.toLocal8Bit().data()));
    BASS_SetConfig(BASS_CONFIG_CD_AUTOSPEED, TRUE);

    return true;
}

void Main::updateTrayIconMenu()
{
    QStringList titles;
    if (window->currentMode() == "Music")
        titles << "Reproduzir Faixa" << "Pausar faixa atual" << "Parar faixa atual" << "Faixa anterior" << "Próxima faixa";
    else
        titles << "Reproduzir Rádio" << "" << "Parar Rádio atual" << "Rádio anterior" << "Próxima rádio";

    QMenu *trayIconMenu = new QMenu;

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
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_start.png")),titles[0]),
            SIGNAL(triggered()), this, SIGNAL(playStream()));

    if (window->currentMode() == "Music")
    {
        connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_pause.png")),titles[1]),
                SIGNAL(triggered()), this, SIGNAL(pauseStream()));
    }

    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_playback_stop.png")),titles[2]),
            SIGNAL(triggered()), this, SIGNAL(stopStream()));
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_skip_backward.png")),titles[3]),
            SIGNAL(triggered()), this, SIGNAL(prevStream()));
    connect(trayIconMenu->addAction(QIcon(Global::getThemePath("images/media_skip_forward.png")),titles[4]),
            SIGNAL(triggered()), this, SIGNAL(nextStream()));

    trayIconMenu->addSeparator();
    connect(trayIconMenu->addAction("Sair"), SIGNAL(triggered()), qApp, SLOT(quit()));

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

void Main::checkUpdate()
{
    updateApp->startChecking(true);
}

void Main::receiveMessage(QStringList arg)
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

void Main::defaultConfig()
{
    if (QMessageBox::warning(nullptr, "Definir a Configuração Padrão?", "Você está prestes a redefinir todas as "
                             "configurações do programa.\nTodas as configurações serão perdidas, "
                             "incluindo o equalizador e favoritos.\n\nDeseja continuar?",
                             QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        continueRunning = true;
        Database::removeDatabase();
        qApp->quit();
    }
}
