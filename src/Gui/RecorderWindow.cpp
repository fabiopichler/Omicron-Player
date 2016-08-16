/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RecorderWindow.h"
#include <QDesktopServices>

RecorderWindow::RecorderWindow(QObject *parentMain, QWidget *parent) : Widget(parent)
{
    this->parentMain = parentMain;
    mode = "Recorder";
    errorWindow = new ErrorWindow(this);
    recorderStream = new RecorderStream(this);
    recordList = recorderStream->recordList;

    createMenuBar();
    createWidgets();
    createLabels();
    createButtons();
    createLayouts();
    createEvents();
}

RecorderWindow::~RecorderWindow()
{
    delete recorderStream;
    delete errorWindow;
}

//================================================================================================================
// private
//================================================================================================================

void RecorderWindow::createMenuBar()
{
    QMenu *fileMenu = new QMenu("Arquivo");
    displayRecordingsAction = fileMenu->addAction("Exibir pasta com Gravações");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("Sair");
    exitAction->setShortcut(QString("Ctrl+Q"));

    QMenu *modeMenu = new QMenu("Modo");
    musicModeAction = modeMenu->addAction("Modo Músicas");
    radioModeAction = modeMenu->addAction("Modo Web Rádio");
    QAction *currentAction = modeMenu->addAction("Modo Gravador");
    modeMenu->addSeparator();
    serverModeAction = modeMenu->addAction("Modo Servidor");

    currentAction->setCheckable(true);
    currentAction->setChecked(true);
    currentAction->setDisabled(true);

    QMenu *toolsMenu = new QMenu("Ferramentas");
    equalizerAction = toolsMenu->addAction("Equalizador");
    toolsMenu->addSeparator();
    configAction = toolsMenu->addAction("Configurações");

    QMenu *aboutMenu = new QMenu("Ajuda");
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

void RecorderWindow::createWidgets()
{
    int current = EncoderList::current;

    // --- deviceComboBox ---
    deviceComboBox = new ComboBox;
    deviceComboBox->setFixedWidth(180);

    QStringList list = recorderStream->getDeviceInfo();
    int device = Database::value("RecorderMode", "device").toInt();

    for (int i = 0; i < list.length(); i++)
    {
        deviceComboBox->addItem(list[i], i - 1);

        if ((i - 1) == device)
            deviceComboBox->setCurrentIndex(i);
    }

    // --- encoderComboBox ---
    encoderComboBox = new ComboBox;
    encoderComboBox->setStyleSheet("min-width: 1px;");

    for (int i = 0; i < 3; i++)
        encoderComboBox->addItem(recorderStream->encoderList[i].name);

    encoderComboBox->setCurrentIndex(current);

    // --- bitrateComboBox ---
    bitrateComboBox = new ComboBox;
    bitrateComboBox->setStyleSheet("min-width: 1px;");

    for (int i = 0; i < recorderStream->encoderList[current].bitrate.length(); i++)
        bitrateComboBox->addItem(QString("%1 kbps").arg(recorderStream->encoderList[current].bitrate[i]));

    bitrateComboBox->setCurrentIndex(recorderStream->encoderList[current].index);

    // --- leftChProgressBar ---
    leftChProgressBar = new QProgressBar;
    leftChProgressBar->setOrientation(Qt::Horizontal);
    leftChProgressBar->setInvertedAppearance(true);
    leftChProgressBar->setFixedHeight(20);
    leftChProgressBar->setTextVisible(false);
    leftChProgressBar->setRange(0,32768);

    // --- rightChProgressBar ---
    rightChProgressBar = new QProgressBar;
    rightChProgressBar->setOrientation(Qt::Horizontal);
    rightChProgressBar->setFixedHeight(20);
    rightChProgressBar->setTextVisible(false);
    rightChProgressBar->setRange(0,32768);

    // --- timeSlider ---
    timeSlider = new Slider(Qt::Horizontal);
    timeSlider->setEnabled(false);
    timeSlider->setMaximum(0);
    timeSlider->setObjectName("timeSlider");
    timeSlider->setStyleSheet("margin: 0 20");

    // --- statusWidget ---
    statusWidget = new Widget;
    statusWidget->setMinimumWidth(10);
    statusWidget->setFixedHeight(22);
    statusWidget->setContentsMargins(10, 0, 10, 0);
    statusWidget->setObjectName("statusWidget");
}

void RecorderWindow::createLabels()
{
    timeLabel = new QLabel("--:--:--");
    timeLabel->setMinimumWidth(80);
    timeLabel->setAlignment(Qt::AlignCenter);

    statusLabel = new QLabel("Parado");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("background: transparent");

    nameLabel = new QLabel();
    currentTitleLabel = new QLabel("---");
}

void RecorderWindow::createButtons()
{
    recordButton = new QPushButton;
    stopButton = new QPushButton;
    playButton = new QPushButton;

    recordButton->setToolTip("Gravar");
    stopButton->setToolTip("Parar");
    playButton->setToolTip("Reproduzir");

    recordButton->setObjectName("record");
    stopButton->setObjectName("stop");
    playButton->setObjectName("play");

    stopButton->setEnabled(false);
}

void RecorderWindow::createLayouts()
{
    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(statusLabel, 1);
    statusLayout->setSpacing(0);
    statusLayout->setMargin(0);

    statusWidget->setLayout(statusLayout);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(deviceComboBox);
    topLayout->addWidget(encoderComboBox);
    topLayout->addWidget(bitrateComboBox);
    topLayout->addWidget(statusWidget);
    topLayout->setContentsMargins(8,0,8,0);

    QVBoxLayout *recordListLayout = new QVBoxLayout;
    recordListLayout->addWidget(recordList);
    recordListLayout->addWidget(timeSlider);
    recordListLayout->setContentsMargins(8,0,8,0);

    QHBoxLayout *channelLayout = new QHBoxLayout;
    channelLayout->addWidget(leftChProgressBar);
    channelLayout->addWidget(rightChProgressBar);
    channelLayout->setSpacing(1);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(recordButton);
    bottomLayout->addWidget(stopButton);
    bottomLayout->addWidget(playButton);
    bottomLayout->addLayout(channelLayout);
    bottomLayout->addWidget(timeLabel);
    bottomLayout->setAlignment(Qt::AlignBottom);
    bottomLayout->setSpacing(5);
    bottomLayout->setContentsMargins(8,0,8,0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(recordListLayout);
    mainLayout->addLayout(bottomLayout);

    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(0,8,0,8);

    setLayout(mainLayout);
}

void RecorderWindow::createEvents()
{
    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(recorderStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(recorderStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(recorderStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(errorWindow, SIGNAL(stopStream()), recorderStream, SLOT(stop()));
    connect(timeSlider, SIGNAL(sliderMoved(int)), recorderStream, SLOT(setPosition(int)));

    connect(displayRecordingsAction, SIGNAL(triggered()), this, SLOT(displayRecordings()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(musicModeAction, SIGNAL(triggered()), parentMain, SLOT(startMusicMode()));
    connect(radioModeAction, SIGNAL(triggered()), parentMain, SLOT(startRadioMode()));
    connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
    connect(equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(configAction, SIGNAL(triggered()), parentMain, SLOT(initConfigDialog()));
    connect(checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(websiteAction, SIGNAL(triggered()), parentMain, SLOT(openSite()));
    connect(aboutAction, SIGNAL(triggered()), parentMain, SLOT(about()));

    connect(deviceComboBox, SIGNAL(activated(int)), this, SLOT(deviceChanged(int)));
    connect(encoderComboBox, SIGNAL(activated(int)), this, SLOT(encoderChanged(int)));
    connect(bitrateComboBox, SIGNAL(activated(int)), this, SLOT(bitrateChanged(int)));

    connect(parentMain, SIGNAL(playStream()), recorderStream, SLOT(record()));
    connect(parentMain, SIGNAL(stopStream()), recorderStream, SLOT(stop()));

    connect(recordButton, SIGNAL(clicked()), recorderStream, SLOT(record()));
    connect(stopButton, SIGNAL(clicked()), recorderStream, SLOT(stop()));
    connect(playButton, SIGNAL(clicked()), recorderStream, SLOT(play()));

    connect(recorderStream, SIGNAL(updateValue(RecorderStream::Event, QVariant)), this, SLOT(update(RecorderStream::Event, QVariant)));
    connect(recorderStream, SIGNAL(updateInfo(QWORD, DWORD)), this, SLOT(update(QWORD, DWORD)));
    connect(recorderStream, SIGNAL(startedStream()), this, SLOT(startedStream()));
    connect(recorderStream, SIGNAL(endOfStream()), this, SLOT(endOfStream()));
}

//================================================================================================================
// private slots
//================================================================================================================

void RecorderWindow::deviceChanged(int)
{
    int device = deviceComboBox->currentData().toInt();
    recorderStream->initDevice(device);
    Database::setValue("RecorderMode", "device", device);
}

void RecorderWindow::encoderChanged(int current)
{
    EncoderList::current = current;
    bitrateComboBox->clear();

    for (int i = 0; i < recorderStream->encoderList[current].bitrate.length(); i++)
        bitrateComboBox->addItem(QString("%1 kbps").arg(recorderStream->encoderList[current].bitrate[i]));

    bitrateComboBox->setCurrentIndex(recorderStream->encoderList[current].index);
    Database::setValue("RecorderMode", "encoder", current);
}

void RecorderWindow::bitrateChanged(int index)
{
    recorderStream->encoderList[EncoderList::current].index = index;

    switch (EncoderList::current)
    {
    case 0:
        Database::setValue("RecorderMode", "aac", index);
        break;

    case 1:
        Database::setValue("RecorderMode", "ogg", index);
        break;

    case 2:
        Database::setValue("RecorderMode", "mp3", index);
        break;
    }
}

void RecorderWindow::initEqualizer()
{
    Equalizer eq(this);
    connect(&eq, SIGNAL(updateEqualizer(int,int)), recorderStream, SLOT(updateFX(int,int)));
    eq.exec();
    disconnect(&eq, SIGNAL(updateEqualizer(int,int)), recorderStream, SLOT(updateFX(int,int)));
}

void RecorderWindow::update(RecorderStream::Event index, QVariant value)
{
    switch (index)
    {
    case RecorderStream::Status:
        statusLabel->setText(value.toString());
        break;

    case RecorderStream::TimeSlider:
        timeSlider->setEnabled(true);
        timeSlider->setMaximum(value.toInt());
        break;

    case RecorderStream::Recording:
        statusLabel->setProperty("recording", value.toBool());
        statusLabel->style()->unpolish(statusLabel);
        statusLabel->style()->polish(statusLabel);
        statusLabel->update();
        timeLabel->setProperty("recording", value.toBool());
        timeLabel->style()->unpolish(timeLabel);
        timeLabel->style()->polish(timeLabel);
        timeLabel->update();
        break;
    }
}

void RecorderWindow::update(QWORD time, DWORD level)
{
    timeLabel->setText(QString::asprintf("%02i:%02i:%02i",
                                         static_cast<unsigned int>(time/3600),
                                         static_cast<unsigned int>((time/60)%60),
                                         static_cast<unsigned int>(time%60)));

    timeSlider->setValue(time);
    leftChProgressBar->setValue(LOWORD(level));
    rightChProgressBar->setValue(HIWORD(level));
}

void RecorderWindow::startedStream()
{
    recordButton->setEnabled(false);
    stopButton->setEnabled(true);
    playButton->setEnabled(false);
    deviceComboBox->setEnabled(false);
    encoderComboBox->setEnabled(false);
    bitrateComboBox->setEnabled(false);
}

void RecorderWindow::endOfStream()
{
    timeLabel->setText("--:--:--");
    statusLabel->setText("Parado");
    leftChProgressBar->setValue(0);
    rightChProgressBar->setValue(0);
    timeSlider->setMaximum(0);
    timeSlider->setEnabled(false);
    recordButton->setEnabled(true);
    stopButton->setEnabled(false);
    playButton->setEnabled(true);
    deviceComboBox->setEnabled(true);
    encoderComboBox->setEnabled(true);
    bitrateComboBox->setEnabled(true);
}

void RecorderWindow::displayRecordings()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(Database::value("RadioConfig", "recordPath").toString() + "/Recorder")))
        Warning(this, "Não foi possível localizar a pasta com as gravações.");
}

void RecorderWindow::displayError(QString arg)
{
    errorWindow->addError(arg);
    errorWindow->show();
}
