/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RadioWindow.h"
#include <QDesktopServices>

#include <OmicronTK11/Qt/ErrorWindow.hpp>
#include <OmicronTK11/Qt/Label.hpp>

using namespace OmicronTK11;

RadioWindow::RadioWindow(QObject *parentMain)
    : WindowBase(parentMain),
      m_ui(this)
{
    this->parentMain = parentMain;
    showLoader = false;
    mode = "Radio";
    radioStream = new RadioStream(this);
    playlist = radioStream->playlist;

    if (!m_ui.init(radioStream))
    {
        throw "31: if (!m_ui.init(radioStream))";
    }

    createWidgets();
    createLabels();
    createButtons();
    createEvents();
    changePlaylist(-1);

    if (!playlist->isEmpty() && Database::value("Config", "autoPlay").toBool())
        radioStream->play();
}

RadioWindow::~RadioWindow()
{
    Database::setValue("Current", "volume", m_ui.m_volumeControl->volume());

    delete radioStream;
    delete m_errorWindow;
}

bool RadioWindow::init()
{
    return true;
}

//================================================================================================================
// private
//================================================================================================================

void RadioWindow::createWidgets()
{
    int volume = Database::value("Current", "volume", 100).toInt();

    m_ui.m_volumeControl->setVolume(volume);
    radioStream->setVolume(volume);

    m_ui.m_searchLineEdit->setClearButtonEnabled(true);
    m_ui.m_searchLineEdit->setPlaceholderText("Pesquisa rápida");

    m_ui.m_leftChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_leftChProgressBar->setRange(0,32768);
    m_ui.m_leftChProgressBar->setTextVisible(false);

    m_ui.m_rightChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_rightChProgressBar->setRange(0,32768);
    m_ui.m_rightChProgressBar->setTextVisible(false);

    m_ui.m_bufferProgressBar->setRange(0,100);
    m_ui.m_bufferProgressBar->setToolTip("Buffer");
}

void RadioWindow::createLabels()
{
    m_ui.m_timeLabel->setText("--:--:--");
    m_ui.m_statusLabel->setText("---");

    m_ui.m_timeLabel->setTextFormat(Qt::PlainText);
    m_ui.m_statusLabel->setTextFormat(Qt::PlainText);
    m_ui.m_nameLabel->setTextFormat(Qt::RichText);
    m_ui.m_streamTitleLabel->setTextFormat(Qt::PlainText);

    m_ui.m_nameLabel->setObjectName("radioTitleLabel");
    m_ui.m_nameLabel->setOpenExternalLinks(true);

    m_ui.m_streamTitleLabel->setObjectName("radioTitleLabel");
    m_ui.m_streamTitleLabel->setFixedHeight(21);

    int hour = QTime::currentTime().hour();

    if (hour > 5 && hour < 12)
        m_ui.m_streamTitleLabel->setText("Bom Dia");
    else if (hour > 11 && hour < 18)
        m_ui.m_streamTitleLabel->setText("Boa Tarde");
    else
        m_ui.m_streamTitleLabel->setText("Boa Noite");
}

void RadioWindow::createButtons()
{
    m_ui.m_allPlaylistsModeButton->setText("Todos");
    m_ui.m_customPlaylistModeButton->setText("Personalizados");
    m_ui.m_favoriteModeButton->setText("Favoritos");

    m_ui.m_playButton->setToolTip("Reproduzir");
    m_ui.m_stopButton->setToolTip("Parar");
    m_ui.m_prevButton->setToolTip("Rádio anterior");
    m_ui.m_nextButton->setToolTip("Próxima rádio");
    m_ui.m_recordButton->setToolTip("Gravar");
    m_ui.m_openLinkButton->setToolTip("Abrir Link Rapidamente");
    m_ui.m_radioPlaylistButton->setToolTip("Editar Lista de Rádios");

    m_ui.m_allPlaylistsModeButton->setObjectName("tabStyle");
    m_ui.m_customPlaylistModeButton->setObjectName("tabStyle");
    m_ui.m_favoriteModeButton->setObjectName("tabStyleLast");

    m_ui.m_stopButton->setEnabled(false);
}

void RadioWindow::createEvents()
{
    connect(m_errorWindow, SIGNAL(stopStream()), radioStream, SLOT(stop()));
    connect(m_ui.m_volumeControl, &OTKQT::VolumeControl::volumeChanged, radioStream, &RadioStream::setVolume);

    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(radioStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(radioStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(radioStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(m_ui.m_openLinkAction, SIGNAL(triggered()), this, SLOT(openLink()));
    connect(m_ui.m_displayRecordingsAction, SIGNAL(triggered()), this, SLOT(displayRecordings()));
    connect(m_ui.m_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_ui.m_musicModeAction, SIGNAL(triggered()), parentMain, SLOT(startMusicMode()));
    connect(m_ui.m_recorderModeAction, SIGNAL(triggered()), parentMain, SLOT(startRecorderMode()));
    //connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
    connect(m_ui.m_playlistManagerAction, SIGNAL(triggered()), this, SLOT(initPlaylist()));
    connect(m_ui.m_equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(m_ui.m_configAction, SIGNAL(triggered()), this, SLOT(initConfigDialog()));
    connect(m_ui.m_checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(m_ui.m_websiteAction, SIGNAL(triggered()), this, SLOT(openSite()));
    connect(m_ui.m_facebookAction, SIGNAL(triggered()), this, SLOT(openFacebook()));
    connect(m_ui.m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    connect(m_ui.m_streamTitleLabel, SIGNAL(clicked()), radioStream, SLOT(webSearch()));
    connect(m_ui.m_searchLineEdit, SIGNAL(textChanged(QString)), playlist, SLOT(textFilterChanged(QString)));

    connect(parentMain, SIGNAL(playStream()), radioStream, SLOT(play()));
    connect(parentMain, SIGNAL(stopStream()), radioStream, SLOT(stop()));
    connect(parentMain, SIGNAL(prevStream()), radioStream, SLOT(prev()));
    connect(parentMain, SIGNAL(nextStream()), radioStream, SLOT(next()));

    connect(m_ui.m_playButton, SIGNAL(clicked()), radioStream, SLOT(play()));
    connect(m_ui.m_stopButton, SIGNAL(clicked()), radioStream, SLOT(stop()));
    connect(m_ui.m_prevButton, SIGNAL(clicked()), radioStream, SLOT(prev()));
    connect(m_ui.m_nextButton, SIGNAL(clicked()), radioStream, SLOT(next()));
    connect(m_ui.m_changeFavoriteButton, SIGNAL(clicked()), this, SLOT(changeFavorite()));
    connect(m_ui.m_openLinkButton, SIGNAL(clicked()), this, SLOT(openLink()));
    connect(m_ui.m_radioPlaylistButton, SIGNAL(clicked()), this, SLOT(initPlaylist()));
    connect(m_ui.m_recordButton, SIGNAL(clicked()), radioStream, SLOT(record()));

    connect(m_ui.m_allPlaylistsModeButton, &QPushButton::clicked, [=]() { changePlaylist(0); });
    connect(m_ui.m_customPlaylistModeButton, &QPushButton::clicked, [=]() { changePlaylist(1); });
    connect(m_ui.m_favoriteModeButton, &QPushButton::clicked, [=]() { changePlaylist(2); });

    connect(radioStream, SIGNAL(playButtonEnabled(bool)), m_ui.m_playButton, SLOT(setEnabled(bool)));
    connect(radioStream, SIGNAL(recordButtonEnabled(bool)), m_ui.m_recordButton, SLOT(setEnabled(bool)));
    connect(radioStream, SIGNAL(stopButtonEnabled(bool)), m_ui.m_stopButton, SLOT(setEnabled(bool)));
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
        m_ui.m_allPlaylistsModeButton->setProperty("tabStyle", true);
        m_ui.m_customPlaylistModeButton->setProperty("tabStyle", false);
        m_ui.m_favoriteModeButton->setProperty("tabStyle", false);
        radioStream->loadPlaylist(2);
    }
    else if (playlistMode == 1)
    {
        m_ui.m_allPlaylistsModeButton->setProperty("tabStyle", false);
        m_ui.m_customPlaylistModeButton->setProperty("tabStyle", true);
        m_ui.m_favoriteModeButton->setProperty("tabStyle", false);
        radioStream->loadPlaylist(1);
    }
    else if (playlistMode == 2)
    {
        m_ui.m_allPlaylistsModeButton->setProperty("tabStyle", false);
        m_ui.m_customPlaylistModeButton->setProperty("tabStyle", false);
        m_ui.m_favoriteModeButton->setProperty("tabStyle", true);
        radioStream->loadPlaylist(0);
    }

    OTKQT::Widget::updateStyle(m_ui.m_changeFavoriteButton);
    OTKQT::Widget::updateStyle(m_ui.m_allPlaylistsModeButton);
    OTKQT::Widget::updateStyle(m_ui.m_customPlaylistModeButton);
    OTKQT::Widget::updateStyle(m_ui.m_favoriteModeButton);

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
        if (m_ui.m_loaderMovie->state() == QMovie::Running)
        {
            m_ui.m_loaderMovie->stop();
            m_ui.m_loaderMovie->setFileName(Global::getThemePath("images/ajax-loader.gif"));
            m_ui.m_loaderMovie->start();
        }
    }

    QWidget::changeEvent(e);
}

//================================================================================================================
// private slots
//================================================================================================================

void RadioWindow::initEqualizer()
{
    Equalizer *eq = new Equalizer(this, Global::equalizerValues);
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
    OTKQT::Dialog *ol = new OTKQT::Dialog(this);
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
            m_ui.m_nameLabel->setText("Link rápido");
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
            m_ui.m_nameLabel->setText(AppName + " (<a href=\"" + OfficialSite + "\">" + OfficialSite + "</a>)");
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

    OTKQT::Widget::updateStyle(playlist);
}

void RadioWindow::update(RadioStream::Event type, QVariant value)
{
    switch (type)
    {
    case RadioStream::StatusLabel:
        m_ui.m_statusLabel->setText(value.toString());
        break;

    case RadioStream::NameLabel:
        m_ui.m_nameLabel->setText(value.toString());
        break;

    case RadioStream::StreamTitleLabel:
        if (!showLoader)
            m_ui.m_streamTitleLabel->setText(value.toString());
        break;

    case RadioStream::BufferProgressBar:
        m_ui.m_bufferProgressBar->setValue(value.toInt());
        break;

    case RadioStream::ShowLoaderMovie:
        if (value.toBool())
        {
            m_ui.m_loaderMovie->stop();
            m_ui.m_loaderMovie->setFileName(Global::getThemePath("images/ajax-loader.gif"));
            m_ui.m_streamTitleLabel->clear();
            m_ui.m_streamTitleLabel->setMovie(m_ui.m_loaderMovie);
            m_ui.m_loaderMovie->start();
        }
        else
        {
            m_ui.m_streamTitleLabel->clear();
            m_ui.m_loaderMovie->stop();
        }
        break;

    case RadioStream::RadioName:
        if (!radioStream->isRunning())
            m_ui.m_nameLabel->setText(playlist->getRadioName(value.toInt()));
        break;

    case RadioStream::Recording:
        if (value.toBool())
        {
            m_ui.m_timeLabel->setProperty("recording", true);
            m_ui.m_timeLabel->setText("Aguarde...");
        }
        else
        {
            m_ui.m_timeLabel->setProperty("recording", false);
        }
        OTKQT::Widget::updateStyle(m_ui.m_timeLabel);
        break;

    case RadioStream::WebSearch:
        if (value.toBool())
        {
            m_ui.m_streamTitleLabel->setToolTip("Clique para pesquisar no Google");
            m_ui.m_streamTitleLabel->setProperty("searchHover", true);
            m_ui.m_streamTitleLabel->setCursor(Qt::PointingHandCursor);
        }
        else
        {
            m_ui.m_streamTitleLabel->setToolTip("");
            m_ui.m_streamTitleLabel->setProperty("searchHover", false);
            m_ui.m_streamTitleLabel->setCursor(Qt::ArrowCursor);
        }
        OTKQT::Widget::updateStyle(m_ui.m_streamTitleLabel);
        break;
    }
}

void RadioWindow::update(QWORD time, DWORD level, DWORD progress, DWORD act)
{
    m_ui.m_timeLabel->setText(QString().sprintf((radioStream->mrecord ? "REC (%02i:%02i:%02i)" : "%02i:%02i:%02i"),
                                         static_cast<unsigned int>(time/3600),
                                         static_cast<unsigned int>((time/60)%60),
                                         static_cast<unsigned int>(time%60)));
    m_ui.m_leftChProgressBar->setValue(LOWORD(level));
    m_ui.m_rightChProgressBar->setValue(HIWORD(level));
    m_ui.m_bufferProgressBar->setValue(progress);
    m_ui.m_bufferProgressBar->setToolTip(QString("Buffer: %1%").arg(progress));

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
    m_ui.m_leftChProgressBar->setValue(0);
    m_ui.m_rightChProgressBar->setValue(0);
    m_ui.m_bufferProgressBar->setValue(0);
    m_ui.m_bufferProgressBar->setToolTip("Buffer");
    m_ui.m_timeLabel->setText("--:--:--");
    m_ui.m_statusLabel->setText("---");
    m_ui.m_streamTitleLabel->setText("Parado");
    m_ui.m_loaderMovie->stop();
    m_ui.m_recordButton->setEnabled(true);
    update(RadioStream::WebSearch, false);

    if (isQuickLink)
        m_ui.m_nameLabel->setText("Link Rápido");
    else if (stop && playlist->isEmpty())
        m_ui.m_nameLabel->setText(AppName + " (<a href=\"" + OfficialSite + "\">" + OfficialSite + "</a>)");
    else
        m_ui.m_nameLabel->setText(playlist->getCurrentTitle());

    if (stop)
    {
        m_ui.m_playButton->setEnabled(true);
        m_ui.m_stopButton->setEnabled(false);
    }
}

void RadioWindow::displayRecordings()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(Database::value("RadioConfig", "recordPath").toString())))
        Warning(this, "Não foi possível localizar a pasta com as gravações.");
}

void RadioWindow::displayError(QString arg)
{
    m_errorWindow->addError(arg);
    m_errorWindow->show();
}

//================================================================================================================
// class ShowRadioInfo
//================================================================================================================

ShowRadioInfo::ShowRadioInfo(QWidget *parent, QStringList radio) : OTKQT::Dialog(parent)
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
