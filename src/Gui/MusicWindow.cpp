/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MusicWindow.h"

MusicWindow::MusicWindow(QObject *parentMain, QWidget *parent) : DropArea(parent)
{
    this->parentMain = parentMain;
    notDisableButtons = false;
    updateListStyle = true;
    mode = "Music";
    errorWindow = new ErrorWindow(this);
    musicStream = new MusicStream(this);
    playlist = musicStream->playlist;
    static bool initialized = true;

    createMenuBar();
    createWidgets();
    createLabels();
    createButtons();
    createLayouts();
    createEvents();
    loadPlaylist((initialized && Global::argc > 1) ? 1 : -1);
    updatePlaylistStyle();

    if (playlist->isEmpty() && Database::value("MusicMode", "soundPosition").toInt() > 0)
        Database::setValue("MusicMode", "soundPosition", 0);

    if (!playlist->isEmpty())
        playlist->selectRow(playlist->getCurrentIndex());

    if (!playlist->isEmpty()
            && ((initialized && musicStream->playlistMode == 1)
                || Database::value("Config", "autoPlay").toBool()
                    || (Database::value("Config", "continuePlaying").toBool()
                        && Database::value("MusicMode", "soundPosition").toInt() > 0)))
    {
        musicStream->play();
    }

    initialized = false;
}

MusicWindow::~MusicWindow()
{
    delete musicStream;
    delete errorWindow;
}

//================================================================================================================
// private
//================================================================================================================

void MusicWindow::createMenuBar()
{
    fileMenu = new QMenu("Arquivo");
    openCDAction = fileMenu->addAction("Abrir CD de Áudio");
    fileMenu->addSeparator();
    openMusicAction = fileMenu->addAction("Abrir Músicas");
    addMusicAction = fileMenu->addAction("Adicionar Músicas");
    fileMenu->addSeparator();
    openDirAction = fileMenu->addAction("Abrir Pasta com Músicas");
    addDirAction = fileMenu->addAction("Adicionar Pasta com Músicas");
    fileMenu->addSeparator();
    openPlaylistAction = fileMenu->addAction("Abrir Playlist");
    clearPlaylistAction = fileMenu->addAction("Limpar a Lista de Reprodução");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("Sair");
    exitAction->setShortcut(QString("Ctrl+Q"));

    modeMenu = new QMenu("Modo");
    radioModeAction = modeMenu->addAction("Modo Web Rádio");

    toolsMenu = new QMenu("Ferramentas");
    playlistAction = toolsMenu->addAction("Gerenciador de Playlist");
    equalizerAction = toolsMenu->addAction("Equalizador");
    toolsMenu->addSeparator();
    configAction = toolsMenu->addAction("Configurações");

    aboutMenu = new QMenu("Ajuda");
    checkUpdateAction = aboutMenu->addAction("Verificar por Atualizações");
    websiteAction = aboutMenu->addAction("Visitar o Website Oficial");
    aboutMenu->addSeparator();
    aboutAction = aboutMenu->addAction("Sobre");

    menuBar = new QMenuBar();
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(modeMenu);
    menuBar->addMenu(toolsMenu);
    menuBar->addMenu(aboutMenu);
}

void MusicWindow::createWidgets()
{
    timeSlider = new Slider(Qt::Horizontal);
    timeSlider->setEnabled(false);
    timeSlider->setMaximum(0);
    timeSlider->setObjectName("timeSlider");

    /*volumeSlider = new Slider(Qt::Horizontal);
    volumeSlider->setMaximum(100);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setStyleSheet("background: none");
    volumeSlider->setObjectName("volumeSlider");*/

    searchLineEdit = new QLineEdit;
    searchLineEdit->setObjectName("searchLineEdit");
    searchLineEdit->setClearButtonEnabled(true);

    leftChProgressBar = new QProgressBar;
    leftChProgressBar->setOrientation(Qt::Horizontal);
    leftChProgressBar->setInvertedAppearance(true);
    leftChProgressBar->setFixedHeight(20);
    leftChProgressBar->setTextVisible(false);
    leftChProgressBar->setRange(0,32768);

    rightChProgressBar = new QProgressBar;
    rightChProgressBar->setOrientation(Qt::Horizontal);
    rightChProgressBar->setFixedHeight(20);
    rightChProgressBar->setTextVisible(false);
    rightChProgressBar->setRange(0,32768);

    infoWidget = new Widget;
    infoWidget->setObjectName("radioInfoWidget");
}

void MusicWindow::createLabels()
{
    timeLabel = new QLabel("--:--");
    totalTimeLabel = new QLabel("--:--");
    currentSoundLabel = new QLabel(QString::asprintf("%02i",
                                  (playlist->isEmpty() ? 0 : Database::value("MusicMode", "index", 0).toInt() + 1)));
    totalSoundLabel = new QLabel(QString::asprintf("%03i", playlist->length()));
    currentTagLabel = new QLabel;
    separatorLabel = new QLabel("/");

    timeLabel->setTextFormat(Qt::PlainText);
    totalTimeLabel->setTextFormat(Qt::PlainText);
    currentSoundLabel->setTextFormat(Qt::PlainText);
    totalSoundLabel->setTextFormat(Qt::PlainText);
    currentTagLabel->setTextFormat(Qt::PlainText);
    separatorLabel->setTextFormat(Qt::PlainText);

    currentSoundLabel->setToolTip("Faixa atual");
    totalSoundLabel->setToolTip("Total de faixas");

    currentTagLabel->setFixedHeight(20);
    currentTagLabel->setObjectName("radioTitleLabel");
    timeLabel->setObjectName("radioTitleLabel");
    totalTimeLabel->setObjectName("radioTitleLabel");

    currentSoundLabel->setStyleSheet("font-weight: bold");
    separatorLabel->setStyleSheet("font-weight: bold");
    totalSoundLabel->setStyleSheet("font-weight: bold");

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        currentTagLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        currentTagLabel->setText("Boa Tarde");
    else
        currentTagLabel->setText("Boa Noite");
}

void MusicWindow::createButtons()
{
    playButton = new QPushButton;
    pauseButton = new QPushButton;
    stopButton = new QPushButton;
    prevButton = new QPushButton;
    nextButton = new QPushButton;
    changeFavoriteButton = new QPushButton;
    repeatButton = new QPushButton;
    randomButton = new QPushButton;
    playlistButton = new QPushButton;
    playlistModeButton = new QPushButton("Playlist");
    musicModeButton = new QPushButton("Músicas/CD");
    favoriteButton = new QPushButton("Favoritos");
    //muteButton = new QPushButton;

    playButton->setToolTip("Reproduzir");
    pauseButton->setToolTip("Pausar");
    stopButton->setToolTip("Parar");
    prevButton->setToolTip("Faixa anterior");
    nextButton->setToolTip("Próxima faixa");
    playlistButton->setToolTip("Editar Playlist");

    playButton->setObjectName("play");
    pauseButton->setObjectName("pause");
    stopButton->setObjectName("stop");
    prevButton->setObjectName("prev");
    nextButton->setObjectName("next");
    changeFavoriteButton->setObjectName("changeFavorite");
    repeatButton->setObjectName("repeat");
    randomButton->setObjectName("random");
    playlistButton->setObjectName("musicPlaylist");
    playlistModeButton->setObjectName("tabStyle");
    musicModeButton->setObjectName("tabStyle");
    favoriteButton->setObjectName("tabStyleLast");
    //muteButton->setObjectName("volumeButton");

    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);

    changeRepeatStyle(Database::value("MusicMode", "repeat", 0).toInt());
    changeRandomStyle(Database::value("MusicMode", "random", false).toBool());
}

void MusicWindow::createLayouts()
{
    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(currentSoundLabel);
    labelLayout->addWidget(separatorLabel);
    labelLayout->addWidget(totalSoundLabel);
    labelLayout->setAlignment(Qt::AlignRight);
    labelLayout->setSpacing(0);

    QHBoxLayout *topLayout = new QHBoxLayout;
    //topLayout->addWidget(volumeSlider);
    //topLayout->addWidget(muteButton);
    topLayout->addWidget(changeFavoriteButton);
    topLayout->addStretch(1);
    topLayout->addLayout(labelLayout);
    topLayout->addWidget(repeatButton);
    topLayout->addWidget(randomButton);
    topLayout->addWidget(playlistButton);
    topLayout->setSpacing(10);
    topLayout->setContentsMargins(8,0,8,0);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(playlistModeButton);
    searchLayout->addWidget(musicModeButton);
    searchLayout->addWidget(favoriteButton);
    searchLayout->addSpacing(8);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->setSpacing(0);
    searchLayout->setContentsMargins(8,0,8,0);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeSlider);
    timeLayout->addWidget(totalTimeLabel);
    timeLayout->setAlignment(Qt::AlignCenter);
    timeLayout->setSpacing(8);
    timeLayout->setContentsMargins(0,0,0,0);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(currentTagLabel, 0, Qt::AlignHCenter);
    infoLayout->addLayout(timeLayout);
    infoLayout->setSpacing(0);
    infoLayout->setContentsMargins(8,6,8,9);
    infoWidget->setLayout(infoLayout);

    QHBoxLayout *channelLayout = new QHBoxLayout;
    channelLayout->addWidget(leftChProgressBar);
    channelLayout->addWidget(rightChProgressBar);
    channelLayout->setSpacing(1);

    QVBoxLayout *playlistLayout = new QVBoxLayout;
    playlistLayout->addWidget(playlist);
    playlistLayout->setContentsMargins(8,0,8,0);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(playButton);
    bottomLayout->addWidget(pauseButton);
    bottomLayout->addWidget(stopButton);
    bottomLayout->addWidget(prevButton);
    bottomLayout->addWidget(nextButton);
    bottomLayout->addLayout(channelLayout);
    bottomLayout->setAlignment(Qt::AlignBottom);
    bottomLayout->setSpacing(5);
    bottomLayout->setContentsMargins(8,0,8,0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(playlistLayout);
    mainLayout->addWidget(infoWidget);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(0,8,0,8);

    setLayout(mainLayout);
}

void MusicWindow::createEvents()
{
    connect(playlist, SIGNAL(updateValues(MusicStream::Event, QVariant)),
            this, SLOT(update(MusicStream::Event, QVariant)));
    connect(playlist, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

    connect(parentMain, SIGNAL(openMusic(QStringList)), this, SLOT(openMusic(QStringList)));
    connect(parentMain, SIGNAL(addMusic(QStringList)), this, SLOT(addMusic(QStringList)));
    connect(this, SIGNAL(filesFromDropArea(QStringList)), parentMain, SLOT(receiveMessage(QStringList)));

    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(musicStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(musicStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(musicStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(musicStream, SIGNAL(updatePlaylistStyle()), this, SLOT(updatePlaylistStyle()));
    connect(this, SIGNAL(dragAndDrop(bool)), this, SLOT(updatePlaylistStyle(bool)));
    connect(errorWindow, SIGNAL(stopStream()), musicStream, SLOT(stop()));

    connect(openMusicAction, SIGNAL(triggered()), this, SLOT(openMusic()));
    connect(addMusicAction, SIGNAL(triggered()), this, SLOT(addMusic()));
    connect(openDirAction, SIGNAL(triggered()), this, SLOT(openDirectory()));
    connect(addDirAction, SIGNAL(triggered()), this, SLOT(addDirectory()));
    connect(openCDAction, SIGNAL(triggered()), this, SLOT(openCD()));
    connect(openPlaylistAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(clearPlaylistAction, SIGNAL(triggered()), this, SLOT(clearPlaylist()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(radioModeAction, SIGNAL(triggered()), parentMain, SLOT(startRadioMode()));
    connect(playlistAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(configAction, SIGNAL(triggered()), parentMain, SLOT(initConfigDialog()));
    connect(checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(websiteAction, SIGNAL(triggered()), parentMain, SLOT(openSite()));
    connect(aboutAction, SIGNAL(triggered()), parentMain, SLOT(about()));

    connect(searchLineEdit, SIGNAL(textChanged(QString)), playlist, SLOT(textFilterChanged(QString)));

    connect(parentMain, SIGNAL(playStream()), musicStream, SLOT(play()));
    connect(parentMain, SIGNAL(pauseStream()), musicStream, SLOT(pause()));
    connect(parentMain, SIGNAL(stopStream()), musicStream, SLOT(stop()));
    connect(parentMain, SIGNAL(prevStream()), musicStream, SLOT(prev()));
    connect(parentMain, SIGNAL(nextStream()), musicStream, SLOT(next()));

    connect(playButton, SIGNAL(clicked()), musicStream, SLOT(play()));
    connect(pauseButton, SIGNAL(clicked()), musicStream, SLOT(pause()));
    connect(stopButton, SIGNAL(clicked()), musicStream, SLOT(stop()));
    connect(prevButton, SIGNAL(clicked()), musicStream, SLOT(prev()));
    connect(nextButton, SIGNAL(clicked()), musicStream, SLOT(next()));
    connect(changeFavoriteButton, SIGNAL(clicked()), this, SLOT(changeFavorite()));
    connect(playlistButton, SIGNAL(clicked()), this, SLOT(initPlaylist()));

    connect(repeatButton, &QPushButton::clicked, [=]() { changeRepeatStyle(musicStream->changeRepeat()); });
    connect(randomButton, &QPushButton::clicked, [=]() { changeRandomStyle(musicStream->changeRandom()); });
    connect(playlistModeButton, &QPushButton::clicked, [=]() { loadPlaylist(0); updatePlaylistStyle(); });
    connect(musicModeButton, &QPushButton::clicked, [=]() { loadPlaylist(1); updatePlaylistStyle(); });
    connect(favoriteButton, &QPushButton::clicked, [=]() { loadPlaylist(2); updatePlaylistStyle(); });

    connect(musicStream, SIGNAL(playButtonEnabled(bool)), playButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(pauseButtonEnabled(bool)), pauseButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(stopButtonEnabled(bool)), stopButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(initPlaylist(bool)), this, SLOT(initPlaylist(bool)));
    connect(musicStream, SIGNAL(playNewMusic(QStringList)), this, SLOT(playNewMusic(QStringList)));
    connect(musicStream, SIGNAL(setTotals(QWORD)), this, SLOT(totals(QWORD)));
    connect(musicStream, SIGNAL(updateValues(MusicStream::Event, QVariant)), this, SLOT(update(MusicStream::Event, QVariant)));
    connect(musicStream, SIGNAL(updateValues(QWORD, DWORD)), this, SLOT(update(QWORD, DWORD)));
    connect(musicStream, SIGNAL(threadFinished()), this, SLOT(threadFinished()));

    connect(timeSlider, SIGNAL(sliderMoved(int)), musicStream, SLOT(setPosition(int)));
}

void MusicWindow::changeRepeatStyle(const short &arg)
{
    switch (arg)
    {
    case 0:
        repeatButton->setProperty("style", false);
        repeatButton->setToolTip("Repetir: Desativado");
        break;
    case 1:
        repeatButton->setProperty("style", "one");
        repeatButton->setToolTip("Repetir uma faixa");
        break;
    case 2:
        repeatButton->setProperty("style", "all");
        repeatButton->setToolTip("Repetir todas as faixas");
        break;
    }

    repeatButton->style()->unpolish(repeatButton);
    repeatButton->style()->polish(repeatButton);
    repeatButton->update();
}

void MusicWindow::changeRandomStyle(const bool &arg)
{
    if (arg)
    {
        randomButton->setProperty("active", true);
        randomButton->setToolTip("Modo Aleatório Ativado");
    }
    else
    {
        randomButton->setProperty("active", false);
        randomButton->setToolTip("Modo Aleatório Desativado");
    }

    randomButton->style()->unpolish(randomButton);
    randomButton->style()->polish(randomButton);
    randomButton->update();
}

void MusicWindow::loadPlaylist(int plMode, const bool &disableCdMode, const bool &load,
                                 const bool &stop, const QString &playlistName)
{
    int playlistMode;

    if (plMode > -1)
        playlistMode = plMode;
    else
        playlistMode = Database::value("MusicMode", "playlistMode").toInt();

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
        playlistModeButton->setProperty("tabStyle", true);
        musicModeButton->setProperty("tabStyle", false);
        favoriteButton->setProperty("tabStyle", false);
    }
    else if (playlistMode == 1)
    {
        playlistModeButton->setProperty("tabStyle", false);
        musicModeButton->setProperty("tabStyle", true);
        favoriteButton->setProperty("tabStyle", false);
    }
    else if (playlistMode == 2)
    {
        playlistModeButton->setProperty("tabStyle", false);
        musicModeButton->setProperty("tabStyle", false);
        favoriteButton->setProperty("tabStyle", true);
    }

    changeFavoriteButton->style()->unpolish(changeFavoriteButton);
    changeFavoriteButton->style()->polish(changeFavoriteButton);
    changeFavoriteButton->update();

    playlistModeButton->style()->unpolish(playlistModeButton);
    playlistModeButton->style()->polish(playlistModeButton);
    playlistModeButton->update();

    musicModeButton->style()->unpolish(musicModeButton);
    musicModeButton->style()->polish(musicModeButton);
    musicModeButton->update();

    favoriteButton->style()->unpolish(favoriteButton);
    favoriteButton->style()->polish(favoriteButton);
    favoriteButton->update();

    musicStream->loadPlaylist(playlistMode, disableCdMode, load, stop, playlistName);

    if (plMode > -1)
        Database::setValue("MusicMode", "playlistMode", playlistMode);
}

QStringList MusicWindow::fileDialog()
{
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setDirectory(Database::value("Current", "fileDialog").toString());
    dialog.setNameFilter("Audio ("+FileNameFilter+");;Todos Arquivos (*.*)");

    if (dialog.exec())
    {
        Database::setValue("Current", "fileDialog", dialog.directory().absolutePath());
        return dialog.selectedFiles();
    }

    return QStringList();
}

//================================================================================================================
// private slots
//================================================================================================================

void MusicWindow::initEqualizer()
{
    Equalizer eq(this);
    connect(&eq, SIGNAL(updateEqualizer(int,int)), musicStream, SLOT(updateFX(int,int)));
    eq.exec();
    disconnect(&eq, SIGNAL(updateEqualizer(int,int)), musicStream, SLOT(updateFX(int,int)));
}

void MusicWindow::initPlaylist(bool play)
{
    MusicPlaylistManager pl(this);

    if (pl.exec())
    {
        if (musicStream->isRunning())
            play = true;

        if (pl.playlistChanged && musicStream->isRunning())
        {
            play = true;
            notDisableButtons = true;
            musicStream->stop();
            notDisableButtons = false;
        }

        loadPlaylist(0, false, true, musicStream->playlistMode == 0 ? false : true);
        updatePlaylistStyle();

        if (pl.playlistChanged)
            playlist->setCurrentIndex(0);

        if (!playlist->isEmpty() && play)
        {
            musicStream->play();
        }
        else if (play)
        {
            playButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
        }
    }
}

void MusicWindow::playNewMusic(QStringList arg)
{
    musicStream->setupCDMode(false);
    musicStream->stop();

    if (arg.length() == 1 && arg[0].contains(QRegExp("."+PlaylistExt+"$", Qt::CaseInsensitive)))
    {
        musicStream->playlistMode = -2;
        Database::setValue("MusicMode", "playlist", arg[0]);
        loadPlaylist(0, false, true, true, arg[0]);
    }
    else
    {
        loadPlaylist(1, true, false);
        playlist->clear();

        for (int i = 0; i < arg.length(); i++)
            if (!arg[i].contains(QRegExp("."+PlaylistExt+"$", Qt::CaseInsensitive)))
                playlist->addRow(arg[i]);
    }

    playlist->setCurrentIndex(0);
    updatePlaylistStyle();
    musicStream->play();
}

void MusicWindow::openMusic(QStringList list)
{
    if (list.isEmpty())
    {
        list = fileDialog();
    }

    if (!list.isEmpty())
        playNewMusic(list);
}

void MusicWindow::addMusic(QStringList list)
{
    if (list.isEmpty())
        list = fileDialog();

    if (!list.isEmpty())
    {
        if (musicStream->isCDMode)
            musicStream->setupCDMode(false);

        if (musicStream->playlistMode != 1)
            loadPlaylist(1, true, true);

        for (int i = 0; i < list.length(); i++)
            playlist->addRow(list.at(i));

        updatePlaylistStyle();
    }
}

void MusicWindow::openDirectory()
{
    addDirectory(true);
}

void MusicWindow::addDirectory(bool isOpenMode)
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Adicionar diretório",
                                                        Database::value("Current", "fileDialogDir", QDir::homePath()).toString());

    if (!dirName.isEmpty())
    {
        bool firstRun = true;
        Database::setValue("Current", "fileDialogDir", dirName);
        QDirIterator dirIt(dirName, QDirIterator::Subdirectories);
        QRegExp filter(QString(FileNameFilter).replace(" ", "|").remove("*."), Qt::CaseInsensitive);

        if (musicStream->isCDMode)
            musicStream->setupCDMode(false);

        if (musicStream->playlistMode != 1)
            loadPlaylist(1, true, true, false);

        while (dirIt.hasNext())
        {
            QFileInfo fileInfo(dirIt.next());

            if (fileInfo.isFile())
            {
                if (fileInfo.suffix().contains(filter))
                {
                    if (firstRun && isOpenMode)
                    {
                        musicStream->stop();
                        playlist->clear();
                        playlist->setCurrentIndex(0);
                        firstRun = false;
                    }

                    playlist->addRow(fileInfo.filePath());
                }
            }
        }

        updatePlaylistStyle();

        if (isOpenMode)
            musicStream->play();
    }
}

void MusicWindow::openCD()
{
    ComboBox *actionCombo = new ComboBox;
    int a = 0;
    static int curdrive = 0;
    bool playCD = false;
    BASS_CD_INFO cdi;

    while (a < 10 && BASS_CD_GetInfo(a, &cdi))
    {
        char text[100];
        sprintf(text,"Unidade %c: %s %s v%s",'A'+cdi.letter,cdi.vendor,cdi.product,cdi.rev);
        actionCombo->addItem(text);
        a++;
    }

    if (a == 0)
    {
        delete actionCombo;
        QMessageBox::information(this, "Unidade de CD não encontrada", "Nenhuma unidade de CD/DVD foi encontrada.");
        return;
    }

    if (curdrive < a)
        actionCombo->setCurrentIndex(curdrive);

    DialogBase *displayMsg = new DialogBase(this);
    displayMsg->setMinimumSize(400, 80);

    QLabel *label = new QLabel("Abrir um CD de Áudio.\n\nEscolha a unidade de DVD/CD-ROM:");
    QPushButton *play = new QPushButton("Reproduzir");
    QPushButton *open = new QPushButton("Abrir");
    QPushButton *cancel = new QPushButton("Cancelar");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(play);
    buttonLayout->addWidget(open);
    buttonLayout->addWidget(cancel);
    buttonLayout->setAlignment(Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(actionCombo);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(8, 6, 8, 8);
    mainLayout->setSpacing(8);

    displayMsg->setLayout(mainLayout);

    connect(play, &QPushButton::clicked, [=, &playCD]() {
        playCD = true;
        displayMsg->accept();
    });
    connect(open, SIGNAL(clicked()), displayMsg, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), displayMsg, SLOT(close()));

    if (displayMsg->exec())
    {
        curdrive = actionCombo->currentIndex();

        if (curdrive >= a)
            curdrive = 0;

        loadPlaylist(1);
        musicStream->stop();
        musicStream->setupCDMode(false);
        musicStream->setupCDMode(true, curdrive);
        updatePlaylistStyle();

        if (playCD)
            musicStream->play();
    }

    delete displayMsg;
}

void MusicWindow::changeFavorite()
{
    if (musicStream->isCDMode)
    {
        emit showNotification("A faixa do CD, não foi adicionada aos Favoritos");
        return;
    }

    QModelIndexList selectedRows = playlist->selectionModel()->selectedRows();

    if (selectedRows.length() == 1)
    {
        if (Database::value("MusicMode", "playlistMode").toInt() == 2)
        {
            if (Database::remove("MusicFavorites", playlist->getRow(selectedRows[0].row()).replace("'","&#39;"), "path"))
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
            if (Database::addMusicFavorite(playlist->getRow(selectedRows[0].row())))
                emit showNotification("Adicionado aos Favoritos");
            else
                emit showNotification("Não foi adicionado aos Favoritos");
        }
    }
}

void MusicWindow::showContextMenu(const QPoint &point)
{
    QModelIndex index = playlist->indexAt(point);

    if (!index.isValid())
        return;

    QMenu menu;
    QAction *selectedItem;

    menu.addAction("Reproduzir")->setData(0);

    if (Database::value("MusicMode", "playlistMode").toInt() == 2)
        menu.addAction("Remover dos favoritos")->setData(1);
    else
        menu.addAction("Adicionar aos favoritos")->setData(1);

    if ((selectedItem = menu.exec(playlist->mapToGlobal(point))))
    {
        int row = index.row();
        int selected = selectedItem->data().toInt();

        switch (selected)
        {
        case 0:
            playlist->setCurrentIndex(row);
            musicStream->playNewMusic(row);
            break;

        case 1:
            changeFavorite();
            break;
        }
    }
}

void MusicWindow::clearPlaylist()
{
    musicStream->stop();

    if (musicStream->playlistMode == 0)
        Database::clear("MusicMode", "playlist");
    else if (musicStream->playlistMode == 1)
        musicStream->setupCDMode(false);

    playlist->clear();
    currentTagLabel->setText("Parado");
    updatePlaylistStyle();
}

void MusicWindow::totals(QWORD time)
{
    if (musicStream->isMusic)
    {
        totalTimeLabel->setText(QString::asprintf("%03d.%03d",LOWORD(time),HIWORD(time)));
        time = LOWORD(time);
    }
    else
    {
        totalTimeLabel->setText(QString::asprintf("%02i:%02i", static_cast<unsigned int>(time/60),
                                                  static_cast<unsigned int>(time%60)));
    }
    timeSlider->setEnabled(true);
    timeSlider->setMaximum(time);
}

void MusicWindow::updatePlaylistStyle(bool arg)
{
    if (arg)
    {
        updateListStyle = true;
        playlist->hideColumn(0);
        playlist->setProperty("playlistStyle", "drag");
    }
    else if (playlist->isEmpty())
    {
        updateListStyle = true;
        playlist->hideColumn(0);
        playlist->setToolTip("Arraste arquivos para Reproduzir");
        playlist->setProperty("playlistStyle", "drop");
    }
    else if (updateListStyle)
    {
        updateListStyle = false;
        playlist->showColumn(0);
        playlist->setColumnWidth(0,440);
        playlist->setToolTip("");
        playlist->setProperty("playlistStyle", false);
    }

    playlist->style()->unpolish(playlist);
    playlist->style()->polish(playlist);
    playlist->update();

    if (playlist->isEmpty())
    {
        update(MusicStream::CurrentSound, QVariant(-1));
    }
    else
    {
        if (playlist->getCurrentIndex() >= playlist->length())
        {
            playlist->setCurrentIndex(0);
            update(MusicStream::CurrentSound, QVariant(0));
        }
        else
        {
            update(MusicStream::CurrentSound, QVariant(playlist->getCurrentIndex()));
        }
        playlist->selectRow(playlist->getCurrentIndex());
    }

    update(MusicStream::PlaylistLength);
}

void MusicWindow::update(MusicStream::Event index, QVariant value)
{
    switch (index)
    {
        case MusicStream::CurrentTag:
            currentTagLabel->setText(value.toString());
            break;
        case MusicStream::CurrentSound:
            currentSoundLabel->setText(QString::asprintf("%02i", value.toInt()+1));
            break;
        case MusicStream::PlaylistLength:
            totalSoundLabel->setText(QString::asprintf("%03i", playlist->length()));
            break;
    }
}

void MusicWindow::update(QWORD time, DWORD level)
{
    if (musicStream->isMusic)
    {
        timeLabel->setText(QString::asprintf("%03d.%03d",LOWORD(time),HIWORD(time)));
        time = LOWORD(time);
    }
    else
    {
        timeLabel->setText(QString::asprintf("%02i:%02i",
                                             static_cast<unsigned int>(time/60),
                                             static_cast<unsigned int>(time%60)));
    }
    timeSlider->setValue(time);
    leftChProgressBar->setValue(LOWORD(level));
    rightChProgressBar->setValue(HIWORD(level));
}

void MusicWindow::threadFinished()
{
    timeLabel->setText("--:--");
    totalTimeLabel->setText("--:--");
    currentTagLabel->setText(musicStream->isCDMode ? "Modo CD de Áudio" : "Parado");
    leftChProgressBar->setValue(0);
    rightChProgressBar->setValue(0);
    timeSlider->setMaximum(1);
    timeSlider->setValue(0);
    timeSlider->setEnabled(false);

    if (!notDisableButtons)
    {
        playButton->setEnabled(true);
        pauseButton->setEnabled(false);
        stopButton->setEnabled(false);
    }
}

void MusicWindow::displayError(QString arg)
{
    errorWindow->addError(arg);
    errorWindow->show();
}
