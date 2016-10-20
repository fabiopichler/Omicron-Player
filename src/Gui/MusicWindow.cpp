/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MusicWindow.h"
#include "../Core/Theme.h"
#include "../Core/Directory.h"

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

    if (!(uiWidget = Theme::loadUi("MusicWindow.xml", this)))
    {
        throw "Ops! Algo deu errado...\nHouve um erro ao carregar o arquivo \"MusicWindow.xml\" do tema atual.";
        return;
    }

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
                    || (Database::value("MusicConfig", "continuePlaying").toBool()
                        && Database::value("MusicMode", "soundPosition").toInt() > 0)))
    {
        musicStream->play();
    }

    initialized = false;
}

MusicWindow::~MusicWindow()
{
    Database::setValue("Current", "volume", volumeControl->volume());

    delete musicStream;
    delete errorWindow;
}

//================================================================================================================
// private
//================================================================================================================

void MusicWindow::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
#ifndef Q_OS_ANDROID
    openCDAction = fileMenu->addAction("Abrir CD de Áudio");
    fileMenu->addSeparator();
#endif
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

    QMenu *modeMenu = new QMenu("Modo");
    QAction *currentAction = modeMenu->addAction("Modo Músicas");
    radioModeAction = modeMenu->addAction("Modo Web Rádio");
    recorderModeAction = modeMenu->addAction("Modo Gravador");
    modeMenu->addSeparator();
    serverModeAction = modeMenu->addAction("Modo Servidor");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
    playlistAction = toolsMenu->addAction("Gerenciador de Playlist");
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

void MusicWindow::createWidgets()
{
    int volume = Database::value("Current", "volume", 100).toInt();

    volumeControl = new VolumeControl(this);
    volumeControl->setVolume(volume);
    musicStream->setVolume(volume);

    timeSlider = uiWidget->findChild<QSlider *>("timeSlider");
    timeSlider->setEnabled(false);
    timeSlider->setMaximum(0);

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
}

void MusicWindow::createLabels()
{
    currentTrackLabel = uiWidget->findChild<QLabel *>("currentTrackLabel");
    totalTracksLabel = uiWidget->findChild<QLabel *>("totalTracksLabel");
    currentTagLabel = uiWidget->findChild<QLabel *>("currentTagLabel");
    totalTimeLabel = uiWidget->findChild<QLabel *>("totalTimeLabel");
    fileTypeLabel = uiWidget->findChild<QLabel *>("fileTypeLabel");
    timeLabel = uiWidget->findChild<QLabel *>("timeLabel");

    currentTrackLabel->setTextFormat(Qt::PlainText);
    totalTracksLabel->setTextFormat(Qt::PlainText);
    currentTagLabel->setTextFormat(Qt::PlainText);
    totalTimeLabel->setTextFormat(Qt::PlainText);
    fileTypeLabel->setTextFormat(Qt::PlainText);
    timeLabel->setTextFormat(Qt::PlainText);

    currentTrackLabel->setToolTip("Faixa atual");
    totalTracksLabel->setToolTip("Total de faixas");

    currentTagLabel->setFixedHeight(20);

    currentTagLabel->setObjectName("radioTitleLabel");
    totalTimeLabel->setObjectName("radioTitleLabel");
    currentTrackLabel->setObjectName("trackLabel");
    totalTracksLabel->setObjectName("trackLabel");
    timeLabel->setObjectName("radioTitleLabel");

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        currentTagLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        currentTagLabel->setText("Boa Tarde");
    else
        currentTagLabel->setText("Boa Noite");

    timeLabel->setText("--:--");
    totalTimeLabel->setText("--:--");
    currentTrackLabel->setText(QString::asprintf("%03i",
                                      (playlist->isEmpty() ? 0 : Database::value("MusicMode", "index", 0).toInt() + 1)));
    totalTracksLabel->setText(QString::asprintf("%03i", playlist->length()));
    fileTypeLabel->setText("---");
}

void MusicWindow::createButtons()
{
    playButton = uiWidget->findChild<QPushButton *>("play");
    pauseButton = uiWidget->findChild<QPushButton *>("pause");
    stopButton = uiWidget->findChild<QPushButton *>("stop");
    prevButton = uiWidget->findChild<QPushButton *>("prev");
    nextButton = uiWidget->findChild<QPushButton *>("next");
    changeFavoriteButton = uiWidget->findChild<QPushButton *>("changeFavorite");
    repeatButton = uiWidget->findChild<QPushButton *>("repeat");
    randomButton = uiWidget->findChild<QPushButton *>("random");
    playlistButton = uiWidget->findChild<QPushButton *>("musicPlaylist");
    playlistModeButton = uiWidget->findChild<QPushButton *>("playlistMode");
    musicModeButton = uiWidget->findChild<QPushButton *>("musicMode");
    favoriteButton = uiWidget->findChild<QPushButton *>("favorite");

    playlistModeButton->setText("Playlist");
    musicModeButton->setText("Músicas/CD");
    favoriteButton->setText("Favoritos");

    playButton->setToolTip("Reproduzir");
    pauseButton->setToolTip("Pausar");
    stopButton->setToolTip("Parar");
    prevButton->setToolTip("Faixa anterior");
    nextButton->setToolTip("Próxima faixa");
    playlistButton->setToolTip("Editar Playlist");

    playlistModeButton->setObjectName("tabStyle");
    musicModeButton->setObjectName("tabStyle");
    favoriteButton->setObjectName("tabStyleLast");

    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);

    changeRepeatStyle(Database::value("MusicMode", "repeat", 0).toInt());
    changeRandomStyle(Database::value("MusicMode", "random", false).toBool());
}

void MusicWindow::createLayouts()
{
    uiWidget->findChild<QVBoxLayout *>("playlistLayout")->addWidget(playlist);
    uiWidget->findChild<QHBoxLayout *>("bottomLayout")->addWidget(volumeControl);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(uiWidget);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

void MusicWindow::createEvents()
{
    connect(playlist, SIGNAL(updateValue(MusicStream::Event, QVariant)),
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

    connect(volumeControl, &VolumeControl::volumeChanged, musicStream, &MusicStream::setVolume);

    connect(openMusicAction, SIGNAL(triggered()), this, SLOT(openMusic()));
    connect(addMusicAction, SIGNAL(triggered()), this, SLOT(addMusic()));
    connect(openDirAction, SIGNAL(triggered()), this, SLOT(openDirectory()));
    connect(addDirAction, SIGNAL(triggered()), this, SLOT(addDirectory()));
#ifndef Q_OS_ANDROID
    connect(openCDAction, SIGNAL(triggered()), this, SLOT(openCD()));
#endif
    connect(openPlaylistAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(clearPlaylistAction, SIGNAL(triggered()), this, SLOT(clearPlaylist()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(radioModeAction, SIGNAL(triggered()), parentMain, SLOT(startRadioMode()));
    connect(recorderModeAction, SIGNAL(triggered()), parentMain, SLOT(startRecorderMode()));
    connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
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
    connect(musicStream, SIGNAL(updateValue(MusicStream::Event, QVariant)), this, SLOT(update(MusicStream::Event, QVariant)));
    connect(musicStream, SIGNAL(updateInfo(QWORD, DWORD)), this, SLOT(update(QWORD, DWORD)));
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
    static bool firstRun;
    QString dirName = QFileDialog::getExistingDirectory(this, "Adicionar diretório",
                                                        Database::value("Current", "fileDialogDir", QDir::homePath()).toString());

    if (!dirName.isEmpty())
    {
        Database::setValue("Current", "fileDialogDir", dirName);

        if (musicStream->isCDMode)
            musicStream->setupCDMode(false);

        if (musicStream->playlistMode != 1)
            loadPlaylist(1, true, true, false);

        firstRun = true;
        Directory *dir = new Directory(this);

        dir->getAllFiles(dirName, [=](QFileInfo &fileInfo) {
            if (firstRun && isOpenMode)
            {
                musicStream->stop();
                playlist->clear();
                playlist->setCurrentIndex(0);
                firstRun = false;
            }

            playlist->addRow(fileInfo.filePath());
        });

        dir->executeLater([=]() {
            updatePlaylistStyle();

            if (isOpenMode)
                musicStream->play();
        });

        dir->start();
    }
}

void MusicWindow::openCD()
{
#ifndef Q_OS_ANDROID
    MyComboBox *actionCombo = new MyComboBox;
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
#endif
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
            if (Database::remove("MusicFavorites", "path", playlist->getRow(selectedRows[0].row()).replace("'","&#39;")))
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
    if (musicStream->playlistMode == 2)
    {
        if (QMessageBox::question(this, "Favoritos", "Apagar todos os favoritos?",
                                  QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
            return;

        Database::removeRows("MusicFavorites");
    }

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
            currentTrackLabel->setText(QString::asprintf("%03i", value.toInt()+1));
            break;
        case MusicStream::PlaylistLength:
            totalTracksLabel->setText(QString::asprintf("%03i", playlist->length()));
            break;
        case MusicStream::FileTypeLabel:
            fileTypeLabel->setText(value.toString());
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
