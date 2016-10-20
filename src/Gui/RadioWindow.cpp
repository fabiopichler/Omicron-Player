/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RadioWindow.h"
#include "../Core/Theme.h"
#include <QDesktopServices>

RadioWindow::RadioWindow(QObject *parentMain, QWidget *parent) : MyWidget(parent)
{
    this->parentMain = parentMain;
    this->parent = parent;
    showLoader = false;
    mode = "Radio";
    errorWindow = new ErrorWindow(this);
    radioStream = new RadioStream(this);
    playlist = radioStream->playlist;

    if (!(uiWidget = Theme::loadUi("RadioWindow.xml", this)))
    {
        throw "Ops! Algo deu errado...\nHouve um erro ao carregar o arquivo \"RadioWindow.xml\" do tema atual.";
        return;
    }

    createMenuBar();
    createWidgets();
    createLabels();
    createButtons();
    createLayouts();
    createEvents();
    changePlaylist(-1);

    if (!playlist->isEmpty() && Database::value("Config", "autoPlay").toBool())
        radioStream->play();
}

RadioWindow::~RadioWindow()
{
    Database::setValue("Current", "volume", volumeControl->volume());

    delete radioStream;
    delete errorWindow;
}

//================================================================================================================
// private
//================================================================================================================

void RadioWindow::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
    openLinkAction = fileMenu->addAction("Abrir Link Rapidamente");
    displayRecordingsAction = fileMenu->addAction("Exibir pasta com Gravações");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("Sair");
    exitAction->setShortcut(QString("Ctrl+Q"));

    QMenu *modeMenu = new QMenu("Modo");
    musicModeAction = modeMenu->addAction("Modo Músicas");
    QAction *currentAction = modeMenu->addAction("Modo Web Rádio");
    recorderModeAction = modeMenu->addAction("Modo Gravador");
    modeMenu->addSeparator();
    serverModeAction = modeMenu->addAction("Modo Servidor");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
    playlistManagerAction = toolsMenu->addAction("Gerenciador de Rádios");
    equalizerAction = toolsMenu->addAction("Equalizador");
    toolsMenu->addSeparator();
    configAction = toolsMenu->addAction("Configurações");

    QMenu *aboutMenu = new QMenu("Ajuda");
    checkUpdateAction = aboutMenu->addAction("Verificar por Atualizações");
    websiteAction = aboutMenu->addAction("Visitar o Website Oficial");
    aboutMenu->addSeparator();
    aboutAction = aboutMenu->addAction("Sobre");

    QMenuBar *menuBar = uiWidget->findChild<QMenuBar *>("menuBar");
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(modeMenu);
    menuBar->addMenu(toolsMenu);
    menuBar->addMenu(aboutMenu);
}

void RadioWindow::createWidgets()
{
    int volume = Database::value("Current", "volume", 100).toInt();

    volumeControl = new VolumeControl(this);
    volumeControl->setVolume(volume);
    radioStream->setVolume(volume);

    loaderMovie = new QMovie;

    searchLineEdit = uiWidget->findChild<QLineEdit *>("searchLineEdit");
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setPlaceholderText("Pesquisa rápida");

    leftChProgressBar = uiWidget->findChild<QProgressBar *>("leftChProgressBar");
    leftChProgressBar->setObjectName("channelsProgressBar");
    leftChProgressBar->setRange(0,32768);
    leftChProgressBar->setTextVisible(false);

    rightChProgressBar = uiWidget->findChild<QProgressBar *>("rightChProgressBar");
    rightChProgressBar->setObjectName("channelsProgressBar");
    rightChProgressBar->setRange(0,32768);
    rightChProgressBar->setTextVisible(false);

    bufferProgressBar = uiWidget->findChild<QProgressBar *>("bufferProgressBar");
    bufferProgressBar->setRange(0,100);
    bufferProgressBar->setToolTip("Buffer");
}

void RadioWindow::createLabels()
{
    timeLabel = uiWidget->findChild<QLabel *>("timeLabel");
    statusLabel = uiWidget->findChild<QLabel *>("statusLabel");
    nameLabel = uiWidget->findChild<QLabel *>("radioTitleLabel");
    streamTitleLabel = uiWidget->findChild<QLabel *>("musicTitleLabel");

    timeLabel->setText("--:--:--");
    statusLabel->setText("---");

    timeLabel->setTextFormat(Qt::PlainText);
    statusLabel->setTextFormat(Qt::PlainText);
    nameLabel->setTextFormat(Qt::RichText);
    streamTitleLabel->setTextFormat(Qt::PlainText);

    nameLabel->setObjectName("radioTitleLabel");
    nameLabel->setOpenExternalLinks(true);

    streamTitleLabel->setObjectName("radioTitleLabel");
    streamTitleLabel->setFixedHeight(21);

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        streamTitleLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        streamTitleLabel->setText("Boa Tarde");
    else
        streamTitleLabel->setText("Boa Noite");
}

void RadioWindow::createButtons()
{
    playButton = uiWidget->findChild<QPushButton *>("play");
    stopButton = uiWidget->findChild<QPushButton *>("stop");
    prevButton = uiWidget->findChild<QPushButton *>("prev");
    nextButton = uiWidget->findChild<QPushButton *>("next");
    recordButton = uiWidget->findChild<QPushButton *>("record");
    changeFavoriteButton = uiWidget->findChild<QPushButton *>("changeFavorite");
    openLinkButton = uiWidget->findChild<QPushButton *>("openLink");
    playlistButton = uiWidget->findChild<QPushButton *>("radioPlaylist");
    allPlaylistsButton = uiWidget->findChild<QPushButton *>("allPlaylists");
    customPlaylistButton = uiWidget->findChild<QPushButton *>("customPlaylist");
    favoriteButton = uiWidget->findChild<QPushButton *>("favorite");

    allPlaylistsButton->setText("Todos");
    customPlaylistButton->setText("Personalizados");
    favoriteButton->setText("Favoritos");

    playButton->setToolTip("Reproduzir");
    stopButton->setToolTip("Parar");
    prevButton->setToolTip("Rádio anterior");
    nextButton->setToolTip("Próxima rádio");
    recordButton->setToolTip("Gravar");
    openLinkButton->setToolTip("Abrir Link Rapidamente");
    playlistButton->setToolTip("Editar Lista de Rádios");

    allPlaylistsButton->setObjectName("tabStyle");
    customPlaylistButton->setObjectName("tabStyle");
    favoriteButton->setObjectName("tabStyleLast");

    stopButton->setEnabled(false);
}

void RadioWindow::createLayouts()
{
    uiWidget->findChild<QVBoxLayout *>("playlistLayout")->addWidget(playlist);
    uiWidget->findChild<QHBoxLayout *>("bottomLayout")->addWidget(volumeControl);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(uiWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

void RadioWindow::createEvents()
{
    connect(errorWindow, SIGNAL(stopStream()), radioStream, SLOT(stop()));
    connect(volumeControl, &VolumeControl::volumeChanged, radioStream, &RadioStream::setVolume);

    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(radioStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(radioStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(radioStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(openLinkAction, SIGNAL(triggered()), this, SLOT(openLink()));
    connect(displayRecordingsAction, SIGNAL(triggered()), this, SLOT(displayRecordings()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(musicModeAction, SIGNAL(triggered()), parentMain, SLOT(startMusicMode()));
    connect(recorderModeAction, SIGNAL(triggered()), parentMain, SLOT(startRecorderMode()));
    connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
    connect(playlistManagerAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(configAction, SIGNAL(triggered()), parentMain, SLOT(initConfigDialog()));
    connect(checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(websiteAction, SIGNAL(triggered()), parentMain, SLOT(openSite()));
    connect(aboutAction, SIGNAL(triggered()), parentMain, SLOT(about()));

    //connect(streamTitleLabel, SIGNAL(clicked()), radioStream, SLOT());
    connect(searchLineEdit, SIGNAL(textChanged(QString)), playlist, SLOT(textFilterChanged(QString)));

    connect(parentMain, SIGNAL(playStream()), radioStream, SLOT(play()));
    connect(parentMain, SIGNAL(stopStream()), radioStream, SLOT(stop()));
    connect(parentMain, SIGNAL(prevStream()), radioStream, SLOT(prev()));
    connect(parentMain, SIGNAL(nextStream()), radioStream, SLOT(next()));

    connect(playButton, SIGNAL(clicked()), radioStream, SLOT(play()));
    connect(stopButton, SIGNAL(clicked()), radioStream, SLOT(stop()));
    connect(prevButton, SIGNAL(clicked()), radioStream, SLOT(prev()));
    connect(nextButton, SIGNAL(clicked()), radioStream, SLOT(next()));
    connect(changeFavoriteButton, SIGNAL(clicked()), this, SLOT(changeFavorite()));
    connect(openLinkButton, SIGNAL(clicked()), this, SLOT(openLink()));
    connect(playlistButton, SIGNAL(clicked()), this, SLOT(initPlaylist()));
    connect(recordButton, SIGNAL(clicked()), radioStream, SLOT(record()));

    connect(allPlaylistsButton, &QPushButton::clicked, [=]() { changePlaylist(0); });
    connect(customPlaylistButton, &QPushButton::clicked, [=]() { changePlaylist(1); });
    connect(favoriteButton, &QPushButton::clicked, [=]() { changePlaylist(2); });

    connect(radioStream, SIGNAL(playButtonEnabled(bool)), playButton, SLOT(setEnabled(bool)));
    connect(radioStream, SIGNAL(recordButtonEnabled(bool)), recordButton, SLOT(setEnabled(bool)));
    connect(radioStream, SIGNAL(stopButtonEnabled(bool)), stopButton, SLOT(setEnabled(bool)));
    connect(radioStream, SIGNAL(updateInfo(QWORD, DWORD, DWORD, DWORD)),
            this, SLOT(update(QWORD, DWORD, DWORD, DWORD)));
    connect(radioStream, SIGNAL(updateValue(RadioStream::Event, QVariant)),
            this, SLOT(update(RadioStream::Event, QVariant)));
    connect(radioStream, SIGNAL(threadFinished(bool,bool)), this, SLOT(threadFinished(bool,bool)));

    connect(playlist, SIGNAL(updateValue(RadioStream::Event, QVariant)),
            this, SLOT(update(RadioStream::Event, QVariant)));
    connect(playlist, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
}

void RadioWindow::changePlaylist(int plMode)
{
    int playlistMode;

    if (plMode == radioStream->playlistMode)
        return;

    if (plMode > -1)
        playlistMode = plMode;
    else
        playlistMode = Database::value("RadioMode", "playlistMode").toInt();

    if (playlistMode == 2)
    {
        changeFavoriteButton->setProperty("remove", true);
        changeFavoriteButton->setToolTip("Remover dos Favoritos");
    }
    else
    {
        changeFavoriteButton->setProperty("remove", false);
        changeFavoriteButton->setToolTip("Adicionar aos Favoritos");
    }

    if (playlistMode == 0)
    {
        allPlaylistsButton->setProperty("tabStyle", true);
        customPlaylistButton->setProperty("tabStyle", false);
        favoriteButton->setProperty("tabStyle", false);
        radioStream->loadPlaylist(2);
    }
    else if (playlistMode == 1)
    {
        allPlaylistsButton->setProperty("tabStyle", false);
        customPlaylistButton->setProperty("tabStyle", true);
        favoriteButton->setProperty("tabStyle", false);
        radioStream->loadPlaylist(1);
    }
    else if (playlistMode == 2)
    {
        allPlaylistsButton->setProperty("tabStyle", false);
        customPlaylistButton->setProperty("tabStyle", false);
        favoriteButton->setProperty("tabStyle", true);
        radioStream->loadPlaylist(0);
    }

    changeFavoriteButton->style()->unpolish(changeFavoriteButton);
    changeFavoriteButton->style()->polish(changeFavoriteButton);
    changeFavoriteButton->update();

    allPlaylistsButton->style()->unpolish(allPlaylistsButton);
    allPlaylistsButton->style()->polish(allPlaylistsButton);
    allPlaylistsButton->update();

    customPlaylistButton->style()->unpolish(customPlaylistButton);
    customPlaylistButton->style()->polish(customPlaylistButton);
    customPlaylistButton->update();

    favoriteButton->style()->unpolish(favoriteButton);
    favoriteButton->style()->polish(favoriteButton);
    favoriteButton->update();

    radioStream->playlistMode = playlistMode;

    if (plMode > -1)
        Database::setValue("RadioMode", "playlistMode", playlistMode);

    if (!radioStream->isRunning())
        update(RadioStream::RadioName, playlist->getCurrentIndex());

    updatePlaylistStyle();

    if (!playlist->isEmpty())
        playlist->selectRow(playlist->getCurrentIndex());
}

void RadioWindow::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::StyleChange)
    {
        if (loaderMovie->state() == QMovie::Running)
        {
            loaderMovie->stop();
            loaderMovie->setFileName(Global::getThemePath("images/ajax-loader.gif"));
            loaderMovie->start();
        }
    }

    QWidget::changeEvent(e);
}

//================================================================================================================
// private slots
//================================================================================================================

void RadioWindow::initEqualizer()
{
    Equalizer *eq = new Equalizer(this);
    connect(eq, SIGNAL(updateEqualizer(int,int)), radioStream, SLOT(updateFX(int,int)));
    eq->exec();
    delete eq;
}

void RadioWindow::initPlaylist()
{
    RadioPlaylistManager(this).exec();
    changePlaylist(-1);
}

void RadioWindow::openLink()
{
    DialogBase *ol = new DialogBase(this);
    QLabel *lb = new QLabel("Insira um link para uma Web Rádio");
    QLineEdit *lineEdit = new QLineEdit;
    QPushButton *btOk = new QPushButton("Reproduzir");
    QPushButton *btCancel = new QPushButton("Cancelar");
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    connect(btOk, &QPushButton::clicked, [=]()
    {
        QString link = QUrl(lineEdit->text()).toString();

        if (!link.isEmpty())
        {
            nameLabel->setText("Link rápido");
            radioStream->openLink(link);
        }

        Database::setValue("RadioMode", "quick-link", link);
        ol->close();
    });

    connect(btCancel, SIGNAL(clicked()), ol, SLOT(close()));

    lineEdit->setClearButtonEnabled(true);
    lineEdit->setText(QUrl(Database::value("RadioMode", "quick-link").toUrl()).toString());

    hLayout->addWidget(btOk);
    hLayout->addWidget(btCancel);
    hLayout->setAlignment(Qt::AlignRight);
    hLayout->setSpacing(8);

    layout->addWidget(lb);
    layout->addWidget(lineEdit);
    layout->addLayout(hLayout);
    layout->setMargin(8);
    layout->setSpacing(8);

    ol->setWindowTitle("Link Rápido » "+AppName);
    ol->setMinimumSize(350,90);
    ol->setLayout(layout);
    ol->exec();

    delete ol;
}

void RadioWindow::changeFavorite()
{
    QModelIndexList selectedRows = playlist->selectionModel()->selectedRows();

    if (selectedRows.length() == 1)
    {
        if (Database::value("RadioMode", "playlistMode").toInt() == 2)
        {
            if (Database::remove("RadioFavorites", "title", playlist->getRadioName(selectedRows[0].row()).replace("'","&#39;")))
            {
                emit showNotification("Favorito Removido");
                playlist->removeRow(selectedRows[0].row());
                updatePlaylistStyle();
            }
            else
            {
                emit showError("Erro ao remover o Favorito");
            }
        }
        else
        {
            if (Database::addRadioFavorite(playlist->getRow(selectedRows[0].row())))
                emit showNotification("Adicionado aos Favoritos");
            else
                emit showNotification("Não foi adicionado aos Favoritos");
        }
    }
}

void RadioWindow::showContextMenu(const QPoint &point)
{
    QModelIndex index = playlist->indexAt(point);

    if (!index.isValid())
        return;

    QMenu menu;
    QAction *selectedItem;
    QStringList urls = playlist->getAllUrls(index.row());
    QMenu *urlMenu = menu.addMenu("URLs (reproduzir)");

    for (int i = 0; i < urls.length() && i < 5; i++)
        urlMenu->addAction(urls[i])->setData(i);

    menu.addSeparator();
    menu.addAction("Reproduzir")->setData(5);

    QAction *activateAction = menu.addAction("Ativar");
    activateAction->setData(6);

    menu.addAction("Gravar")->setData(7);
    menu.addSeparator();

    if (Database::value("RadioMode", "playlistMode").toInt() == 2)
    {
        menu.addAction("Remover dos favoritos")->setData(8);
    }
    else
    {
        menu.addAction("Adicionar aos favoritos")->setData(8);
        menu.addSeparator();
        menu.addAction("Informações da rádio")->setData(9);
    }

    if (radioStream->isRunning())
        activateAction->setDisabled(true);

    if ((selectedItem = menu.exec(playlist->mapToGlobal(point))))
    {
        int row = index.row();
        int selected = selectedItem->data().toInt();

        switch (selected)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            playlist->setCurrentIndex(row);
            radioStream->playNewRadio(row, selected);
            break;

        case 5:
            playlist->setCurrentIndex(row);
            radioStream->playNewRadio(row);
            break;

        case 6:
            playlist->setCurrentIndex(row);
            update(RadioStream::RadioName, row);
            break;

        case 7:
            if (radioStream->isRunning())
                radioStream->stop(true);

            playlist->setCurrentIndex(row);
            update(RadioStream::RadioName, row);
            radioStream->record();
            break;

        case 8:
            changeFavorite();
            break;

        case 9:
            ShowRadioInfo(this, playlist->getRow(row)).exec();
            break;
        }
    }
}

void RadioWindow::updatePlaylistStyle()
{
    if (playlist->isEmpty())
    {
        if (!radioStream->isRunning())
            nameLabel->setText(AppName + " (<a href=\"" + OfficialSite + "\">" + OfficialSite + "</a>)");
        playlist->hideColumn(0);
        playlist->hideColumn(1);
        playlist->hideColumn(2);
        playlist->setProperty("playlistStyle", "radio");
    }
    else
    {
        playlist->showColumn(2);
        playlist->setColumnWidth(2,110);
        playlist->showColumn(1);
        playlist->setColumnWidth(1,120);
        playlist->showColumn(0);
        playlist->setColumnWidth(0,210);
        playlist->setProperty("playlistStyle", false);
    }

    playlist->style()->unpolish(playlist);
    playlist->style()->polish(playlist);
    playlist->update();
}

void RadioWindow::update(RadioStream::Event type, QVariant value)
{
    switch (type)
    {
    case RadioStream::StatusLabel:
        statusLabel->setText(value.toString());
        break;

    case RadioStream::NameLabel:
        nameLabel->setText(value.toString());
        break;

    case RadioStream::StreamTitleLabel:
        if (!showLoader)
            streamTitleLabel->setText(value.toString());
        break;

    case RadioStream::BufferProgressBar:
        bufferProgressBar->setValue(value.toInt());
        break;

    case RadioStream::ShowLoaderMovie:
        if (value.toBool())
        {
            loaderMovie->stop();
            loaderMovie->setFileName(Global::getThemePath("images/ajax-loader.gif"));
            streamTitleLabel->clear();
            streamTitleLabel->setMovie(loaderMovie);
            loaderMovie->start();
        }
        else
        {
            streamTitleLabel->clear();
            loaderMovie->stop();
        }
        break;

    case RadioStream::RadioName:
        if (!radioStream->isRunning())
            nameLabel->setText(playlist->getRadioName(value.toInt()));
        break;

    case RadioStream::Recording:
        if (value.toBool())
        {
            timeLabel->setProperty("recording", true);
            timeLabel->setText("Aguarde...");
        }
        else
        {
            timeLabel->setProperty("recording", false);
        }
        timeLabel->style()->unpolish(timeLabel);
        timeLabel->style()->polish(timeLabel);
        timeLabel->update();
        break;
    }
}

void RadioWindow::update(QWORD time, DWORD level, DWORD progress, DWORD act)
{
    timeLabel->setText(QString::asprintf((radioStream->mrecord ? "REC (%02i:%02i:%02i)" : "%02i:%02i:%02i"),
                                         static_cast<unsigned int>(time/3600),
                                         static_cast<unsigned int>((time/60)%60),
                                         static_cast<unsigned int>(time%60)));
    leftChProgressBar->setValue(LOWORD(level));
    rightChProgressBar->setValue(HIWORD(level));
    bufferProgressBar->setValue(progress);
    bufferProgressBar->setToolTip(QString("Buffer: %1%").arg(progress));

    if (act == 2)
    {
        if (!showLoader)
        {
            showLoader = true;
            update(RadioStream::ShowLoaderMovie, true);
        }
    }
    else if (showLoader)
    {
        showLoader = false;
        update(RadioStream::ShowLoaderMovie, false);
        radioStream->doMeta();
    }
}

void RadioWindow::threadFinished(bool stop, bool isQuickLink)
{
    leftChProgressBar->setValue(0);
    rightChProgressBar->setValue(0);
    bufferProgressBar->setValue(0);
    bufferProgressBar->setToolTip("Buffer");
    timeLabel->setText("--:--:--");
    statusLabel->setText("---");
    streamTitleLabel->setText("Parado");
    loaderMovie->stop();
    recordButton->setEnabled(true);

    if (isQuickLink)
        nameLabel->setText("Link Rápido");
    else if (stop && playlist->isEmpty())
        nameLabel->setText(AppName + " (<a href=\"" + OfficialSite + "\">" + OfficialSite + "</a>)");
    else
        nameLabel->setText(playlist->getCurrentTitle());

    if (stop)
    {
        playButton->setEnabled(true);
        stopButton->setEnabled(false);
    }
}

void RadioWindow::displayRecordings()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(Database::value("RadioConfig", "recordPath").toString())))
        Warning(this, "Não foi possível localizar a pasta com as gravações.");
}

void RadioWindow::displayError(QString arg)
{
    errorWindow->addError(arg);
    errorWindow->show();
}

//================================================================================================================
// class ShowRadioInfo
//================================================================================================================

ShowRadioInfo::ShowRadioInfo(QWidget *parent, QStringList radio) : DialogBase(parent)
{
    QStringList labelQStringList;
    labelQStringList << "Título" << "Descrição" << "Gênero" << "País" << "Link 1" << "Link 2" << "Link 3" << "Link 4" << "Link 5";
    int i;

    radio.move(8,1);

    QPushButton *okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->setContentsMargins(0,4,0,0);
    buttonLayout->setSpacing(5);
    buttonLayout->setAlignment(Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    for (i = 0; i < 9; i++)
    {
        QLabel *label = new QLabel(labelQStringList[i]);
        QLineEdit *lineEdit = new QLineEdit(radio[i]);

        label->setObjectName("radioInfoLabel");
        lineEdit->setObjectName("radioInfoEdit");
        lineEdit->setReadOnly(true);

        layout->addWidget(label);
        layout->addWidget(lineEdit);
        layout->addSpacing(10);
    }

    layout->addLayout(buttonLayout);
    layout->setContentsMargins(10,5,10,10);
    layout->setSpacing(0);

    setLayout(layout);
    setWindowTitle("Informações da Rádio » "+AppName);
    setFixedWidth(460);
}

ShowRadioInfo::~ShowRadioInfo()
{
}
