/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MusicWindow.h"

#include <OmicronTK11/Qt/DirectoryDialog.hpp>
#include <OmicronTK11/Qt/ComboBox.hpp>
#include <OmicronTK11/Qt/ErrorWindow.hpp>
#include <OmicronTK11/Qt/Slider.hpp>

using namespace OmicronTK11;

MusicWindow::MusicWindow(QObject *parentMain)
    : WindowBase(parentMain),
      m_ui(this)
{
    this->parentMain = parentMain;
    notDisableButtons = false;
    updateListStyle = true;
    mode = "Music";
    musicStream = new MusicStream(this);
    playlist = musicStream->playlist;
    static bool initialized = true;

    if (!m_ui.init(musicStream))
    {
        throw "36: if (!m_ui.init(musicStream))";
        return;
    }

    createWidgets();
    createLabels();
    createButtons();
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
    Database::setValue("Current", "volume", m_ui.m_volumeControl->volume());

    delete musicStream;
    delete m_errorWindow;
}

bool MusicWindow::init()
{
    return true;
}

//================================================================================================================
// private
//================================================================================================================

void MusicWindow::createWidgets()
{
    int volume = Database::value("Current", "volume", 100).toInt();

    m_ui.m_volumeControl->setVolume(volume);
    musicStream->setVolume(volume);

    m_ui.m_timeSlider->setEnabled(false);
    m_ui.m_timeSlider->setMaximum(0);

    m_ui.m_searchLineEdit->setClearButtonEnabled(true);
    m_ui.m_searchLineEdit->setPlaceholderText("Pesquisa rápida");

    m_ui.m_leftChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_leftChProgressBar->setRange(0,32768);
    m_ui.m_leftChProgressBar->setTextVisible(false);

    m_ui.m_rightChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_rightChProgressBar->setRange(0,32768);
    m_ui.m_rightChProgressBar->setTextVisible(false);
}

void MusicWindow::createLabels()
{
    m_ui.m_currentTrackLabel->setTextFormat(Qt::PlainText);
    m_ui.m_totalTracksLabel->setTextFormat(Qt::PlainText);
    m_ui.m_currentTagLabel->setTextFormat(Qt::PlainText);
    m_ui.m_totalTimeLabel->setTextFormat(Qt::PlainText);
    m_ui.m_fileTypeLabel->setTextFormat(Qt::PlainText);
    m_ui.m_timeLabel->setTextFormat(Qt::PlainText);

    m_ui.m_currentTrackLabel->setToolTip("Faixa atual");
    m_ui.m_totalTracksLabel->setToolTip("Total de faixas");

    m_ui.m_currentTagLabel->setFixedHeight(20);

    m_ui.m_currentTagLabel->setObjectName("radioTitleLabel");
    m_ui.m_totalTimeLabel->setObjectName("radioTitleLabel");
    m_ui.m_currentTrackLabel->setObjectName("trackLabel");
    m_ui.m_totalTracksLabel->setObjectName("trackLabel");
    m_ui.m_timeLabel->setObjectName("radioTitleLabel");

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        m_ui.m_currentTagLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        m_ui.m_currentTagLabel->setText("Boa Tarde");
    else
        m_ui.m_currentTagLabel->setText("Boa Noite");

    m_ui.m_timeLabel->setText("--:--");
    m_ui.m_totalTimeLabel->setText("--:--");
    m_ui.m_currentTrackLabel->setText(QString().sprintf("%03i",
                                      (playlist->isEmpty() ? 0 : Database::value("MusicMode", "index", 0).toInt() + 1)));
    m_ui.m_totalTracksLabel->setText(QString().sprintf("%03i", playlist->length()));
    m_ui.m_fileTypeLabel->setText("---");
}

void MusicWindow::createButtons()
{
    m_ui.m_playlistTabButton->setText("Playlist");
    m_ui.m_musicTabButton->setText("Músicas/CD");
    m_ui.m_favoritesTabButton->setText("Favoritos");

    m_ui.m_playButton->setToolTip("Reproduzir");
    m_ui.m_pauseButton->setToolTip("Pausar");
    m_ui.m_stopButton->setToolTip("Parar");
    m_ui.m_prevButton->setToolTip("Faixa anterior");
    m_ui.m_nextButton->setToolTip("Próxima faixa");
    m_ui.m_playlistManagerButton->setToolTip("Editar Playlist");

    m_ui.m_playlistTabButton->setObjectName("tabStyle");
    m_ui.m_musicTabButton->setObjectName("tabStyle");
    m_ui.m_favoritesTabButton->setObjectName("tabStyleLast");

    m_ui.m_pauseButton->setEnabled(false);
    m_ui.m_stopButton->setEnabled(false);

    changeRepeatStyle(Database::value("MusicMode", "repeat", 0).toInt());
    changeRandomStyle(Database::value("MusicMode", "random", false).toBool());
}

void MusicWindow::createEvents()
{
    connect(playlist, SIGNAL(updateValue(MusicStream::Event, QVariant)),
            this, SLOT(update(MusicStream::Event, QVariant)));
    connect(playlist, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

    connect(parentMain, SIGNAL(openMusic(QStringList)), this, SLOT(openMusic(QStringList)));
    connect(parentMain, SIGNAL(addMusic(QStringList)), this, SLOT(addMusic(QStringList)));
    connect(this, SIGNAL(filesFromDropArea(QVector<QString>)), parentMain, SLOT(receiveMessage(QVector<QString>)));

    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(musicStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(musicStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(musicStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(musicStream, SIGNAL(updatePlaylistStyle()), this, SLOT(updatePlaylistStyle()));
    connect(this, SIGNAL(dragAndDrop(bool)), this, SLOT(updatePlaylistStyle(bool)));
    connect(m_errorWindow, SIGNAL(stopStream()), musicStream, SLOT(stop()));

    connect(m_ui.m_volumeControl, &OTKQT::VolumeControl::volumeChanged, musicStream, &MusicStream::setVolume);

    connect(m_ui.m_openMusicAction, SIGNAL(triggered()), this, SLOT(openMusic()));
    connect(m_ui.m_addMusicAction, SIGNAL(triggered()), this, SLOT(addMusic()));
    connect(m_ui.m_openDirAction, SIGNAL(triggered()), this, SLOT(openDirectory()));
    connect(m_ui.m_addDirAction, SIGNAL(triggered()), this, SLOT(addDirectory()));
#ifndef Q_OS_ANDROID
    connect(m_ui.m_openCDAction, SIGNAL(triggered()), this, SLOT(openCD()));
#endif
    connect(m_ui.m_openPlaylistAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(m_ui.m_clearPlaylistAction, SIGNAL(triggered()), this, SLOT(clearPlaylist()));
    connect(m_ui.m_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_ui.m_radioModeAction, SIGNAL(triggered()), parentMain, SLOT(startRadioMode()));
    connect(m_ui.m_recorderModeAction, SIGNAL(triggered()), parentMain, SLOT(startRecorderMode()));
    //connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
    connect(m_ui.m_playlistAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(m_ui.m_equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(m_ui.m_configAction, SIGNAL(triggered()), this, SLOT(initConfigDialog()));
    connect(m_ui.m_checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(m_ui.m_websiteAction, SIGNAL(triggered()), this, SLOT(openSite()));
    connect(m_ui.m_facebookAction, SIGNAL(triggered()), this, SLOT(openFacebook()));
    connect(m_ui.m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    connect(m_ui.m_searchLineEdit, SIGNAL(textChanged(QString)), playlist, SLOT(textFilterChanged(QString)));

    connect(parentMain, SIGNAL(playStream()), musicStream, SLOT(play()));
    connect(parentMain, SIGNAL(pauseStream()), musicStream, SLOT(pause()));
    connect(parentMain, SIGNAL(stopStream()), musicStream, SLOT(stop()));
    connect(parentMain, SIGNAL(prevStream()), musicStream, SLOT(prev()));
    connect(parentMain, SIGNAL(nextStream()), musicStream, SLOT(next()));

    connect(m_ui.m_playButton, SIGNAL(clicked()), musicStream, SLOT(play()));
    connect(m_ui.m_pauseButton, SIGNAL(clicked()), musicStream, SLOT(pause()));
    connect(m_ui.m_stopButton, SIGNAL(clicked()), musicStream, SLOT(stop()));
    connect(m_ui.m_prevButton, SIGNAL(clicked()), musicStream, SLOT(prev()));
    connect(m_ui.m_nextButton, SIGNAL(clicked()), musicStream, SLOT(next()));
    connect(m_ui.m_changeFavoriteButton, SIGNAL(clicked()), this, SLOT(changeFavorite()));
    connect(m_ui.m_playlistManagerButton, SIGNAL(clicked()), this, SLOT(initPlaylist()));

    connect(m_ui.m_repeatButton, &QPushButton::clicked, [=]() { changeRepeatStyle(musicStream->changeRepeat()); });
    connect(m_ui.m_randomButton, &QPushButton::clicked, [=]() { changeRandomStyle(musicStream->changeRandom()); });
    connect(m_ui.m_playlistTabButton, &QPushButton::clicked, [=]() { loadPlaylist(0); updatePlaylistStyle(); });
    connect(m_ui.m_musicTabButton, &QPushButton::clicked, [=]() { loadPlaylist(1); updatePlaylistStyle(); });
    connect(m_ui.m_favoritesTabButton, &QPushButton::clicked, [=]() { loadPlaylist(2); updatePlaylistStyle(); });

    connect(musicStream, SIGNAL(playButtonEnabled(bool)), m_ui.m_playButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(pauseButtonEnabled(bool)), m_ui.m_pauseButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(stopButtonEnabled(bool)), m_ui.m_stopButton, SLOT(setEnabled(bool)));
    connect(musicStream, SIGNAL(initPlaylist(bool)), this, SLOT(initPlaylist(bool)));
    connect(musicStream, SIGNAL(playNewMusic(QStringList)), this, SLOT(playNewMusic(QStringList)));
    connect(musicStream, SIGNAL(setTotals(QWORD)), this, SLOT(totals(QWORD)));
    connect(musicStream, SIGNAL(updateValue(MusicStream::Event, QVariant)), this, SLOT(update(MusicStream::Event, QVariant)));
    connect(musicStream, SIGNAL(updateInfo(QWORD, DWORD)), this, SLOT(update(QWORD, DWORD)));
    connect(musicStream, SIGNAL(threadFinished()), this, SLOT(threadFinished()));

    connect(m_ui.m_timeSlider, SIGNAL(sliderMoved(int)), musicStream, SLOT(setPosition(int)));
}

void MusicWindow::changeRepeatStyle(const short &arg)
{
    switch (arg)
    {
    case 0:
        m_ui.m_repeatButton->setProperty("style", false);
        m_ui.m_repeatButton->setToolTip("Repetir: Desativado");
        break;
    case 1:
        m_ui.m_repeatButton->setProperty("style", "one");
        m_ui.m_repeatButton->setToolTip("Repetir uma faixa");
        break;
    case 2:
        m_ui.m_repeatButton->setProperty("style", "all");
        m_ui.m_repeatButton->setToolTip("Repetir todas as faixas");
        break;
    }

    OTKQT::Widget::updateStyle(m_ui.m_repeatButton);
}

void MusicWindow::changeRandomStyle(const bool &arg)
{
    if (arg)
    {
        m_ui.m_randomButton->setProperty("active", true);
        m_ui.m_randomButton->setToolTip("Modo Aleatório Ativado");
    }
    else
    {
        m_ui.m_randomButton->setProperty("active", false);
        m_ui.m_randomButton->setToolTip("Modo Aleatório Desativado");
    }

    OTKQT::Widget::updateStyle(m_ui.m_randomButton);
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
        m_ui.m_changeFavoriteButton->setProperty("remove", true);
        m_ui.m_changeFavoriteButton->setToolTip("Remover dos Favoritos");
    }
    else
    {
        m_ui.m_changeFavoriteButton->setProperty("remove", false);
        m_ui.m_changeFavoriteButton->setToolTip("Adicionar aos Favoritos");
    }

    if (playlistMode == 0)
    {
        m_ui.m_playlistTabButton->setProperty("tabStyle", true);
        m_ui.m_musicTabButton->setProperty("tabStyle", false);
        m_ui.m_favoritesTabButton->setProperty("tabStyle", false);
    }
    else if (playlistMode == 1)
    {
        m_ui.m_playlistTabButton->setProperty("tabStyle", false);
        m_ui.m_musicTabButton->setProperty("tabStyle", true);
        m_ui.m_favoritesTabButton->setProperty("tabStyle", false);
    }
    else if (playlistMode == 2)
    {
        m_ui.m_playlistTabButton->setProperty("tabStyle", false);
        m_ui.m_musicTabButton->setProperty("tabStyle", false);
        m_ui.m_favoritesTabButton->setProperty("tabStyle", true);
    }

    OTKQT::Widget::updateStyle(m_ui.m_changeFavoriteButton);
    OTKQT::Widget::updateStyle(m_ui.m_playlistTabButton);
    OTKQT::Widget::updateStyle(m_ui.m_musicTabButton);
    OTKQT::Widget::updateStyle(m_ui.m_favoritesTabButton);

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
    Equalizer eq(this, Global::equalizerValues);
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
            m_ui.m_playButton->setEnabled(true);
            m_ui.m_pauseButton->setEnabled(false);
            m_ui.m_stopButton->setEnabled(false);
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
        OTKQT::DirectoryDialog *dir = new OTKQT::DirectoryDialog(this);

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
    OTKQT::ComboBox *actionCombo = new OTKQT::ComboBox;
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

    OTKQT::Dialog *displayMsg = new OTKQT::Dialog(this);
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
    m_ui.m_currentTagLabel->setText("Parado");
    updatePlaylistStyle();
}

void MusicWindow::totals(QWORD time)
{
    if (musicStream->isMusic)
    {
        m_ui.m_totalTimeLabel->setText(QString().sprintf("%03d.%03d",LOWORD(time),HIWORD(time)));
        time = LOWORD(time);
    }
    else if (time < 3600)
    {
        m_ui.m_totalTimeLabel->setText(QString().sprintf("%02i:%02i", static_cast<unsigned int>(time/60),
                                                  static_cast<unsigned int>(time%60)));
    }
    else
    {
        m_ui.m_totalTimeLabel->setText(QString().sprintf("%02i:%02i:%02i",
                                             static_cast<unsigned int>(time/3600),
                                             static_cast<unsigned int>((time/60)%60),
                                             static_cast<unsigned int>(time%60)));
    }
    m_ui.m_timeSlider->setEnabled(true);
    m_ui.m_timeSlider->setMaximum(time);
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

    OTKQT::Widget::updateStyle(playlist);

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
            m_ui.m_currentTagLabel->setText(value.toString());
            break;
        case MusicStream::CurrentSound:
            m_ui.m_currentTrackLabel->setText(QString().sprintf("%03i", value.toInt()+1));
            break;
        case MusicStream::PlaylistLength:
            m_ui.m_totalTracksLabel->setText(QString().sprintf("%03i", playlist->length()));
            break;
        case MusicStream::FileTypeLabel:
            m_ui.m_fileTypeLabel->setText(value.toString());
            break;
    }
}

void MusicWindow::update(QWORD time, DWORD level)
{
    if (musicStream->isMusic)
    {
        m_ui.m_timeLabel->setText(QString().sprintf("%03d.%03d",LOWORD(time),HIWORD(time)));
        time = LOWORD(time);
    }
    else if (time < 3600)
    {
        m_ui.m_timeLabel->setText(QString().sprintf("%02i:%02i",
                                             static_cast<unsigned int>(time/60),
                                             static_cast<unsigned int>(time%60)));
    }
    else
    {
        m_ui.m_timeLabel->setText(QString().sprintf("%02i:%02i:%02i",
                                             static_cast<unsigned int>(time/3600),
                                             static_cast<unsigned int>((time/60)%60),
                                             static_cast<unsigned int>(time%60)));
    }

    m_ui.m_timeSlider->setValue(time);
    m_ui.m_leftChProgressBar->setValue(LOWORD(level));
    m_ui.m_rightChProgressBar->setValue(HIWORD(level));
}

void MusicWindow::threadFinished()
{
    m_ui.m_timeLabel->setText("--:--");
    m_ui.m_totalTimeLabel->setText("--:--");
    m_ui.m_currentTagLabel->setText(musicStream->isCDMode ? "Modo CD de Áudio" : "Parado");
    m_ui.m_leftChProgressBar->setValue(0);
    m_ui.m_rightChProgressBar->setValue(0);
    m_ui.m_timeSlider->setMaximum(1);
    m_ui.m_timeSlider->setValue(0);
    m_ui.m_timeSlider->setEnabled(false);

    if (!notDisableButtons)
    {
        m_ui.m_playButton->setEnabled(true);
        m_ui.m_pauseButton->setEnabled(false);
        m_ui.m_stopButton->setEnabled(false);
    }
}

void MusicWindow::displayError(QString arg)
{
    m_errorWindow->addError(arg);
    m_errorWindow->show();
}
